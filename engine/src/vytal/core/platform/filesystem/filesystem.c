#include "filesystem.h"

#include "vytal/core/hal/memory/vtmem.h"
#include "vytal/core/misc/string/vtstr.h"

#include <stdio.h>

#if defined(VT_PLATFORM_WINDOWS)
#    include <Windows.h>

#elif defined(VT_PLATFORM_LINUX)
#    include <sys/stat.h>

#endif

#define LINE_BUFFER_MAX_SIZE VT_SIZE_KB_MULT(32) // 32 KB

Bool platform_fs_open_file(FileHandle *handle, ConstStr filepath, const FileIOMode io_mode, const FileMode file_mode) {
    ConstStr file_mode_;

    // configure stream file mode
    switch (io_mode) {
    case FILE_IO_MODE_READ_WRITE:
        file_mode_ = (file_mode == FILE_MODE_BINARY) ? "w+b" : "w+";
        break;

    case FILE_IO_MODE_READ:
        file_mode_ = (file_mode == FILE_MODE_BINARY) ? "rb" : "r";
        break;

    case FILE_IO_MODE_WRITE:
        file_mode_ = (file_mode == FILE_MODE_BINARY) ? "wb" : "w";
        break;

    case FILE_IO_MODE_APPEND:
        file_mode_ = (file_mode == FILE_MODE_BINARY) ? "a+b" : "a";
        break;

    default:
        return false;
    }

    // init the file handle
    {
        FileStream in_file_ = fopen(filepath, file_mode_);
        if (!in_file_)
            return false;

        handle->_stream = in_file_;
        handle->_active = true;
    }

    return true;
}

Bool platform_fs_close_file(FileHandle *handle) {
    if (!handle->_active || !handle->_stream)
        return false;

    // close the file stream and de-activate the handle
    {
        fclose(handle->_stream);
        handle->_active = false;
    }

    return true;
}

Bool platform_fs_file_exists(ConstStr filepath) {
    Bool       file_exists_ = false;
    FileStream stream_;

    // if stream is succesfully opened then that indicates that the file exists
    if ((stream_ = fopen(filepath, "r")) != NULL) {
        file_exists_ = true;
        fclose(stream_);
    }

    return file_exists_;
}

ByteSize platform_fs_file_size(FileHandle *handle) {
    if (!handle->_active || !handle->_stream)
        return 0;

    ByteSize   file_size_ = 0;
    FileStream stream_    = handle->_stream;

    // measure the size of the file with cursor positions
    {
        fseek(stream_, 0, SEEK_END);
        file_size_ = ftell(stream_);
        fseek(stream_, 0, SEEK_SET);
    }

    return file_size_;
}

Bool platform_fs_file_read_line(FileHandle *handle, Str *line_buffer, ByteSize *line_buffer_length) {
    if (!handle->_active || !handle->_stream)
        return false;

    if (line_buffer && line_buffer_length) {
        // if line is not empty and read-able
        if (fgets(*line_buffer, LINE_BUFFER_MAX_SIZE, handle->_stream) != NULL) {
            *line_buffer_length = misc_str_strlen(*line_buffer);
            return true;
        }
    }

    return false;
}

Bool platform_fs_file_write_line(FileHandle *handle, ConstStr text) {
    if (!handle->_active || !handle->_stream)
        return false;

    FileStream stream_ = handle->_stream;
    Int32      res_    = fprintf(stream_, "%s", text);
    if (res_ != EOF)
        res_ = fputc('\n', stream_);

    fflush(stream_);
    return (res_ != EOF);
}

Bool platform_fs_file_read_all(FileHandle *handle, ByteSize *read_size, Str read_out) {
    if (!handle->_active || !handle->_stream || !read_out)
        return false;

    if (!read_size)
        fread(read_out, sizeof(Char), platform_fs_file_size(handle), handle->_stream);
    else
        *read_size = fread(read_out, sizeof(Char), platform_fs_file_size(handle), handle->_stream);

    return true;
}

Bool platform_fs_file_read_data(FileHandle *handle, const ByteSize data_size, ByteSize *read_size, VoidPtr read_out) {
    if (!handle->_active || !handle->_stream || (data_size == 0) || !read_out)
        return false;

    if (!read_size)
        fread(read_out, data_size, 1, handle->_stream);
    else
        *read_size = fread(read_out, data_size, 1, handle->_stream);

    return true;
}

Bool platform_fs_file_write_data(FileHandle *handle, const VoidPtr data, const ByteSize data_size) {
    if (!handle->_active || !handle->_stream || !data || (data_size == 0))
        return false;

    FileStream stream_ = handle->_stream;
    fwrite(data, data_size, 1, handle->_stream);

    fflush(stream_);
    return true;
}

ConstStr platform_fs_get_filename_from_path(ConstStr filepath) {
    return VT_CAST(ConstStr, misc_str_strrchr(VT_CAST(Str, filepath), '\\') ? misc_str_strrchr(VT_CAST(Str, filepath), '\\') + 1
                             : misc_str_strrchr(VT_CAST(Str, filepath), '/') ? misc_str_strrchr(VT_CAST(Str, filepath), '/') + 1
                                                                             : filepath);
}

Bool platform_fs_create_directory(ConstStr directory_name) {
#if defined(VT_PLATFORM_WINDOWS)
    return CreateDirectoryA(directory_name, NULL);

#elif defined(VT_PLATFORM_LINUX)
    return (mkdir(directory_name, 0777) == 0);

#endif
}

Bool platform_fs_seek_to_position(FileHandle *handle, const UIntPtr target) {
    if (!handle || target == EOF)
        return false;

    fseek(handle->_stream, target, SEEK_SET);
    return true;
}

Bool platform_fs_seek_from_current(FileHandle *handle, const ByteSize distance) {
    if (!handle || distance == 0)
        return false;

    fseek(handle->_stream, distance, SEEK_CUR);
    return true;
}

Int64 platform_fs_get_seek_position(FileHandle *handle) {
    if (!handle)
        return -1;

    return ftell(handle->_stream);
}
