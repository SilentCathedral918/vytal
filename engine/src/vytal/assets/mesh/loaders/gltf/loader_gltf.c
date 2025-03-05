#include "loader_gltf.h"

#define CGLTF_IMPLEMENTATION
#include <cgltf.h>

// for pre-fetching
#include <xmmintrin.h>

#include "vytal/core/memory/zone/memory_zone.h"

MeshResult _mesh_loader_load_from_data(cgltf_data *data, Mesh *out_mesh) {
    if (cgltf_validate(data) != cgltf_result_success)
        return MESH_ERROR_FILE_PARSE_FAILED;

    if (data->meshes_count == 0 || data->meshes[0].primitives_count == 0)
        return MESH_ERROR_FILE_PARSE_FAILED;

    cgltf_mesh      *mesh_      = &data->meshes[0];
    cgltf_primitive *primitive_ = &mesh_->primitives[0];

    if (!primitive_->indices || primitive_->attributes_count == 0)
        return MESH_ERROR_FILE_PARSE_FAILED;

    // vertex buffer data
    cgltf_buffer_view *vertex_view_  = primitive_->attributes[0].data->buffer_view;
    VoidPtr            vertex_data_  = (BytePtr)vertex_view_->buffer->data + vertex_view_->offset;
    ByteSize           vertex_size_  = vertex_view_->size;
    UInt32             vertex_count_ = primitive_->attributes[0].data->count;

    // index buffer data
    cgltf_buffer_view *index_view_  = primitive_->indices->buffer_view;
    VoidPtr            index_data_  = (BytePtr)index_view_->buffer->data + index_view_->offset;
    ByteSize           index_size_  = index_view_->size;
    UInt32             index_count_ = primitive_->indices->count;

    // texture buffer data
    ByteSize total_image_size_ = 0;
    for (ByteSize i = 0; i < data->images_count; ++i) {
        cgltf_image *image_ = &data->images[i];

        if (image_->buffer_view)
            total_image_size_ += image_->buffer_view->size;
    }

    ByteSize total_size_ = sizeof(struct Mesh_Handle) + ((sizeof(ByteSize) * data->images_count) * 2) + (vertex_size_ + index_size_ + total_image_size_);
    ByteSize alloc_size_ = 0;
    if (memory_zone_allocate("assets", total_size_, (VoidPtr *)out_mesh, &alloc_size_) != MEMORY_ZONE_SUCCESS)
        return MESH_ERROR_ALLOCATION_FAILED;

    // configure members
    {
        (*out_mesh)->_texture_offsets = (ByteSize *)((BytePtr)(*out_mesh) + sizeof(struct Mesh_Handle));
        (*out_mesh)->_texture_sizes   = (ByteSize *)((BytePtr)(*out_mesh)->_texture_offsets + (sizeof(ByteSize) * data->images_count));
        (*out_mesh)->_vertex_offset   = sizeof(struct Mesh_Handle) + ((sizeof(ByteSize) * data->images_count) * 2);
        (*out_mesh)->_index_offset    = (*out_mesh)->_vertex_offset + vertex_size_;

        ByteSize running_offset_ = (*out_mesh)->_index_offset + index_size_;
        for (ByteSize i = 0; i < data->images_count; ++i) {
            cgltf_image *image_ = &data->images[i];

            if (image_->buffer_view) {
                (*out_mesh)->_texture_offsets[i] = running_offset_;
                (*out_mesh)->_texture_sizes[i]   = image_->buffer_view->size;
                running_offset_ += VYTAL_APPLY_ALIGNMENT(image_->buffer_view->size, MEMORY_ALIGNMENT_SIZE);
            }
        }

        (*out_mesh)->_vertex_count = vertex_count_;
        (*out_mesh)->_vertex_size  = vertex_size_;
        (*out_mesh)->_index_count  = index_count_;
        (*out_mesh)->_index_size   = index_size_;

        switch (data->file_type) {
            case cgltf_file_type_gltf:
                (*out_mesh)->_format = MESH_FORMAT_GLTF;
                break;

            case cgltf_file_type_glb:
                (*out_mesh)->_format = MESH_FORMAT_GLB;
                break;

            default:
                return MESH_ERROR_FILE_PARSE_FAILED;
        }

        (*out_mesh)->_memory_size = alloc_size_;

        // vertex buffer data
        {
#if defined(__GNUC__) || defined(__clang__)
            __builtin_prefetch(vertex_data_, 0, 3);
#else
            _mm_prefetch((BytePtr)vertex_data_, _MM_HINT_T0);
#endif
            memcpy((BytePtr)(*out_mesh) + (*out_mesh)->_vertex_offset, vertex_data_, vertex_size_);
        }

        // index buffer data
        {
#if defined(__GNUC__) || defined(__clang__)
            __builtin_prefetch(index_data_, 0, 3);
#else
            _mm_prefetch((BytePtr)index_data_, _MM_HINT_T0);
#endif
            memcpy((BytePtr)(*out_mesh) + (*out_mesh)->_index_offset, index_data_, index_size_);
        }

        // texture buffer data
        for (ByteSize i = 0, j = 0; i < data->images_count; ++i) {
            cgltf_image *image_ = &data->images[i];
            if (!image_->buffer_view) continue;

            UInt8 *image_data_ = (UInt8 *)data->buffers[0].data + image_->buffer_view->offset;
            memcpy((BytePtr)(*out_mesh) + (*out_mesh)->_texture_offsets[j], image_data_, (*out_mesh)->_texture_sizes[j]);
            ++j;
        }
    }

    return MESH_SUCCESS;
}

MeshResult mesh_loader_gltf_load_from_file(ConstStr filepath, Mesh *out_mesh) {
    if (!filepath || !out_mesh) return MESH_ERROR_INVALID_PARAM;

    cgltf_options options_ = {0};
    cgltf_data   *data_    = NULL;

    if (cgltf_parse_file(&options_, filepath, &data_) != cgltf_result_success)
        return MESH_ERROR_FILE_PARSE_FAILED;

    if (cgltf_load_buffers(&options_, data_, filepath) != cgltf_result_success)
        return MESH_ERROR_FILE_PARSE_FAILED;

    MeshResult load_from_data_ = _mesh_loader_load_from_data(data_, out_mesh);
    if (load_from_data_ != MESH_SUCCESS) {
        cgltf_free(data_);
        return load_from_data_;
    }

    cgltf_free(data_);
    return MESH_SUCCESS;
}

MeshResult mesh_loader_gltf_load_from_memory(const VoidPtr buffer, const ByteSize buffer_size, Mesh *out_mesh) {
    if (!buffer || !buffer_size || !out_mesh) return MESH_ERROR_INVALID_PARAM;

    cgltf_options options_ = {0};
    cgltf_data   *data_    = NULL;

    if (cgltf_parse(&options_, buffer, (cgltf_size)buffer_size, &data_) != cgltf_result_success)
        return MESH_ERROR_FILE_PARSE_FAILED;

    if (cgltf_load_buffers(&options_, data_, NULL) != cgltf_result_success)
        return MESH_ERROR_FILE_PARSE_FAILED;

    MeshResult load_from_data_ = _mesh_loader_load_from_data(data_, out_mesh);
    if (load_from_data_ != MESH_SUCCESS) {
        cgltf_free(data_);
        return load_from_data_;
    }

    cgltf_free(data_);
    return MESH_SUCCESS;
}

MeshResult mesh_loader_gltf_unload(Mesh mesh) {
    if (!mesh) return MESH_ERROR_INVALID_PARAM;

    if (memory_zone_deallocate("assets", mesh, mesh->_memory_size) != MEMORY_ZONE_SUCCESS)
        return MESH_ERROR_DEALLOCATION_FAILED;

    mesh = NULL;
    return MESH_SUCCESS;
}
