#include "memory_manager.h"

#include <ctype.h>
#include <stdlib.h>
#include <string.h>

#include "vytal/core/helpers/parse/parse.h"
#include "vytal/core/memory/zone/memory_zone.h"
#include "vytal/core/platform/filesystem/filesystem.h"

static MemoryManager *manager = NULL;

MemoryManagerResult memory_manager_startup(File *file) {
    if (manager) return MEMORY_MANAGER_ERROR_ALREADY_INITIALIZED;
    if (!file) return MEMORY_MANAGER_ERROR_INVALID_PARAM;
    if (!file->_active || !file->_stream) return MEMORY_MANAGER_ERROR_FILE_INACTIVE_OR_INVALID_STREAM;

    UInt32   num_zones_      = 0;
    ByteSize total_capacity_ = 0;
    Str      line_           = calloc(1, LINE_BUFFER_MAX_SIZE);

    // save the section start position for second pass
    Int64 section_start_ = platform_filesystem_get_seek_position(file);

    // first pass: gather infos and allocate
    {
        while (platform_filesystem_read_line(file, NULL, &line_) == FILE_SUCCESS) {
            Str trimmed_ = line_;

            if (parse_trim_whitespace(&trimmed_) != PARSE_SUCCESS)
                return MEMORY_MANAGER_ERROR_PARSE_FAILED;

            if (*trimmed_ == '#' || *trimmed_ == '\0') continue;
            if (*trimmed_ == '[') break;

            Char key_[LINE_BUFFER_MAX_SIZE]   = {0};
            Char value_[LINE_BUFFER_MAX_SIZE] = {0};
            if (!parse_key_value(trimmed_, key_, value_)) continue;

            ByteSize num_sizeclasses_ = 0;
            ByteSize capacity_        = parse_memory_size(value_);
            memory_zone_compute_size_classes(&num_sizeclasses_, NULL, capacity_);
            ByteSize sizeclasses_size_ = sizeof(MemoryZoneSizeClass) * num_sizeclasses_;

            total_capacity_ += ((ByteSize)capacity_ + sizeclasses_size_);
            ++num_zones_;
        }

        // allocate a large chunk to cover entire memory manager
        {
            ByteSize manager_size_ = sizeof(MemoryManager);
            ByteSize zones_size_   = sizeof(MemoryZone) * num_zones_;

            UIntPtr block_ = (UIntPtr)calloc(1, manager_size_ + zones_size_ + total_capacity_);
            if (!block_) return MEMORY_MANAGER_ERROR_ALLOCATION_FAILED;

            manager            = (MemoryManager *)block_;
            manager->_zones    = (MemoryZone *)(block_ + manager_size_);
            manager->_pool     = (VoidPtr)(block_ + manager_size_ + zones_size_);
            manager->_capacity = total_capacity_;
        }
    }

    // second pass: parsing zones
    {
        // return to section start
        platform_filesystem_seek_to_position(file, section_start_);

        ByteSize seek_length_ = 0;
        UIntPtr  start_addr_  = (UIntPtr)manager->_pool;
        while (platform_filesystem_read_line(file, &seek_length_, &line_) == FILE_SUCCESS) {
            Str trimmed_ = line_;

            if (parse_trim_whitespace(&trimmed_) != PARSE_SUCCESS)
                return MEMORY_MANAGER_ERROR_PARSE_FAILED;

            if (*trimmed_ == '#' || *trimmed_ == '\0') continue;
            if (*trimmed_ == '[') {
                platform_filesystem_seek_from_current(file, -seek_length_);
                break;
            }

            Char key_[LINE_BUFFER_MAX_SIZE]   = {0};
            Char value_[LINE_BUFFER_MAX_SIZE] = {0};
            if (!parse_key_value(trimmed_, key_, value_)) continue;

            MemoryZone *zone_  = &manager->_zones[manager->_zone_count++];
            zone_->_name       = strdup(key_);
            zone_->_start_addr = (VoidPtr)start_addr_;

            ByteSize capacity_   = parse_memory_size(value_);
            zone_->_size_classes = (MemoryZoneSizeClass *)(start_addr_ + capacity_);
            memory_zone_compute_size_classes(&zone_->_num_classes, &zone_->_size_classes, capacity_);
            zone_->_capacity = capacity_;

            start_addr_ += (capacity_ + (sizeof(MemoryZoneSizeClass) * zone_->_num_classes));
        }
    }

    free(line_);
    return MEMORY_MANAGER_SUCCESS;
}

MemoryManagerResult memory_manager_shutdown(void) {
    if (!manager) return MEMORY_MANAGER_ERROR_NOT_INITIALIZED;

    for (size_t i = 0; i < manager->_zone_count; ++i) {
        MemoryZone *zone_ = &manager->_zones[i];

        for (size_t j = 0; j < zone_->_num_classes; ++j) {
            MemoryZoneSizeClass *class_ = &zone_->_size_classes[j];

            if ((class_->_blocks) && (class_->_capacity > 0)) {
                free(class_->_blocks);
                class_->_blocks = NULL;
            }
        }

        free(zone_->_name);
        zone_->_name = NULL;
    }

    free(manager);
    manager = NULL;

    return MEMORY_MANAGER_SUCCESS;
}

MemoryManager *memory_manager_get(void) {
    return manager;
}

ByteSize memory_manager_used_memory(void) {
    return manager->_used_memory;
}

ByteSize memory_manager_capacity(void) {
    return manager->_capacity;
}

MemoryZone *memory_manager_zones(void) {
    return manager->_zones;
}

ByteSize memory_manager_zone_count(void) {
    return manager->_zone_count;
}
