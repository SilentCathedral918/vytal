#include "memory_manager.h"

#include <ctype.h>
#include <stdlib.h>
#include <string.h>

#include "vytal/core/memory/zone/memory_zone.h"
#include "vytal/core/platform/filesystem/filesystem.h"

static MemoryManager *manager                    = NULL;
static ConstStr       memory_zones_cvar_filepath = "memory_zones.cfg";

VYTAL_INLINE Str _memory_manager_trim_whitespace(Str str) {
    // trim leading space
    {
        while (isspace((Int8)*str)) ++str;
        if (*str == '\0') return str;
    }

    // trim trailing space
    {
        Str end_ = str + strlen(str) - 1;
        while (end_ > str && isspace((unsigned char)*end_)) --end_;
        *(end_ + 1) = '\0';
    }

    return str;
}

MemoryManagerResult memory_manager_startup(void) {
    if (manager) return MEMORY_MANAGER_ERROR_ALREADY_INITIALIZED;

    UInt32   num_zones_      = 0;
    ByteSize total_capacity_ = 0;
    File     cvar_file_      = {0};
    Str      cvar_line_      = calloc(1, LINE_BUFFER_MAX_SIZE);

    FileResult open_file_ = platform_filesystem_open_file(&cvar_file_, memory_zones_cvar_filepath, FILE_IO_MODE_READ, FILE_MODE_TEXT);
    if (open_file_ != FILE_SUCCESS) return MEMORY_MANAGER_ERROR_FILE_OPEN_FAILED;

    // first scan for total pool capacity and number of zones required
    while (platform_filesystem_read_line(&cvar_file_, NULL, &cvar_line_) == FILE_SUCCESS) {
        Str trimmed_ = _memory_manager_trim_whitespace(cvar_line_);
        if (*trimmed_ == '#' || *trimmed_ == '\0') continue;

        Str value_ = _memory_manager_trim_whitespace(strchr(trimmed_, '=') + 1);
        Str end_;

        Int64 capacity_ = strtol(value_, &end_, 0);
        if (end_ == value_ || *end_ != '\0') return MEMORY_MANAGER_ERROR_INVALID_FORMAT;

        ByteSize num_sizeclasses_ = 0;
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
        if (!block_) return MEMORY_MANAGER_ERROR_OUT_OF_MEMORY;

        manager            = (MemoryManager *)block_;
        manager->_zones    = (MemoryZone *)(block_ + manager_size_);
        manager->_pool     = (VoidPtr)(block_ + manager_size_ + zones_size_);
        manager->_capacity = total_capacity_;
    }

    // reset to the begin of the file
    platform_filesystem_seek_to_position(&cvar_file_, 0);

    // second scan for parsing zones
    UIntPtr start_addr_ = (UIntPtr)manager->_pool;
    while (platform_filesystem_read_line(&cvar_file_, NULL, &cvar_line_) == FILE_SUCCESS) {
        Str trimmed_ = _memory_manager_trim_whitespace(cvar_line_);

        // ignore comments and empty lines
        if (*trimmed_ == '#' || *trimmed_ == '\0') continue;

        Str pequal_ = strchr(trimmed_, '=');
        Str value_  = _memory_manager_trim_whitespace(pequal_ + 1);
        Str end_;

        if (pequal_) {
            *pequal_ = '\0';

            Str key_  = trimmed_;
            Str psep_ = strchr(key_, '.');
            Str name_ = _memory_manager_trim_whitespace(psep_ + 1);

            MemoryZone *zone_  = &manager->_zones[manager->_zone_count++];
            zone_->_name       = strdup(name_);
            zone_->_start_addr = (VoidPtr)start_addr_;

            Int64 capacity_ = strtol(value_, &end_, 0);
            if (end_ == value_ || *end_ != '\0') return MEMORY_MANAGER_ERROR_INVALID_FORMAT;

            zone_->_size_classes = (MemoryZoneSizeClass *)(start_addr_ + capacity_);
            memory_zone_compute_size_classes(&zone_->_num_classes, zone_->_size_classes, capacity_);
            zone_->_capacity = (ByteSize)capacity_;
            start_addr_ += (capacity_ + sizeof(MemoryZoneSizeClass) * zone_->_num_classes);
        }
    }

    free(cvar_line_);

    FileResult close_file_ = platform_filesystem_close_file(&cvar_file_);
    if (close_file_ != FILE_SUCCESS) return MEMORY_MANAGER_ERROR_FILE_CLOSE_FAILED;

    return MEMORY_MANAGER_SUCCESS;
}

MemoryManagerResult memory_manager_shutdown(void) {
    if (!manager) return MEMORY_MANAGER_ERROR_NOT_INITIALIZED;

    for (size_t i = 0; i < manager->_zone_count; ++i) {
        MemoryZone *zone_ = &manager->_zones[i];

        for (size_t j = 0; j < zone_->_num_classes; ++j) {
            MemoryZoneSizeClass *class_ = &zone_->_size_classes[i];

            if ((class_->_blocks) && (class_->_capacity > 0))
                free(class_->_blocks);
        }

        free(zone_->_name);
    }

    free(manager);
    manager = NULL;

    return MEMORY_MANAGER_SUCCESS;
}

MemoryManager *memory_manager_get(void) {
    return manager;
}

ConstStr memory_manager_get_zones_cvar_filepath(void) {
    return memory_zones_cvar_filepath;
}
