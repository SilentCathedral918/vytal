#include "logger.h"

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "vytal/core/configuration/cvar/cvar.h"
#include "vytal/core/containers/map/map.h"
#include "vytal/core/containers/string/string.h"
#include "vytal/core/platform/filesystem/filesystem.h"

typedef struct Logger_State {
    Map  _logger_map;
    Bool _initialized;
} Logger_State;

struct Logger_Handle {
    String     _name;
    LoggerFlag _flags;
    File       _file;
};

static Logger_State *state                 = NULL;
static ConstStr      loggers_cvar_filepath = "loggers.cfg";

VYTAL_INLINE Str _logger_trim_whitespace(Str str) {
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

LoggerResult logger_startup(void) {
    if (state) return LOGGER_ERROR_STATE_ALREADY_INITIALIZED;

    // allocate and configure the state
    state = malloc(sizeof(Logger_State));
    memset(state, 0, sizeof(Logger_State));

    // construct the logger map
    if (container_map_construct(sizeof(struct Logger_Handle), &state->_logger_map) != CONTAINER_SUCCESS)
        return LOGGER_ERROR_MAP_CONSTRUCTION_FAILED;

    File   cvar_file_      = {0};
    Str    cvar_line_      = calloc(1, LINE_BUFFER_MAX_SIZE);
    Logger staging_logger_ = calloc(1, sizeof(struct Logger_Handle));

    FileResult open_file_ = platform_filesystem_open_file(&cvar_file_, loggers_cvar_filepath, FILE_IO_MODE_READ, FILE_MODE_TEXT);
    if (open_file_ != FILE_SUCCESS) return MEMORY_MANAGER_ERROR_FILE_OPEN_FAILED;

    // first scan for loggers required
    while (platform_filesystem_read_line(&cvar_file_, NULL, &cvar_line_) == FILE_SUCCESS) {
        memset(staging_logger_, 0, sizeof(struct Logger_Handle));

        Str trimmed_ = _logger_trim_whitespace(cvar_line_);
        if (*trimmed_ == '#' || *trimmed_ == '\0') continue;

        Str pequal_ = strchr(trimmed_, '=');
        Str value_  = _logger_trim_whitespace(pequal_ + 1);
        Str end_;

        if (pequal_) {
            *pequal_ = '\0';

            Str key_  = trimmed_;
            Str psep_ = strchr(key_, '.');
            Str name_ = _logger_trim_whitespace(psep_ + 1);

            Int64 logger_flags_ = strtol(value_, &end_, 2);
            if (end_ == value_ || *end_ != '\0') return LOGGER_ERROR_INVALID_FORMAT;

            // prepare logger data
            {
                if (container_string_construct(name_, &staging_logger_->_name) != CONTAINER_SUCCESS)
                    return LOGGER_ERROR_MAP_ITEM_ALLOCATION_FAILED;

                staging_logger_->_flags = logger_flags_;
                staging_logger_->_file  = (File){0};
            }

            // insert to map
            if (container_map_insert(&state->_logger_map, name_, staging_logger_) != CONTAINER_SUCCESS)
                return LOGGER_ERROR_MAP_ITEM_ALLOCATION_FAILED;
        }
    }

    free(staging_logger_);
    free(cvar_line_);

    FileResult close_file_ = platform_filesystem_close_file(&cvar_file_);
    if (close_file_ != FILE_SUCCESS) return MEMORY_MANAGER_ERROR_FILE_CLOSE_FAILED;

    state->_initialized = true;
    return LOGGER_SUCCESS;
}

LoggerResult logger_shutdown(void) {
    if (!state) return LOGGER_ERROR_STATE_NOT_INITIALIZED;
    if (!state->_initialized) return LOGGER_ERROR_STATE_NOT_INITIALIZED;

    // go through very logger in the map and deallocate its members
    ByteSize capacity_ = container_map_capacity(state->_logger_map);
    for (ByteSize i = 0; i < capacity_; ++i) {
        Logger logger_ = container_map_at_index(state->_logger_map, i);

        if (!logger_->_name) continue;

        // destruct the name
        if (container_string_destruct(logger_->_name) != CONTAINER_SUCCESS)
            return LOGGER_ERROR_MAP_ITEM_DEALLOCATION_FAILED;

        // close the file (if there is one)
        if (logger_->_file._stream && logger_->_file._active)
            if (platform_filesystem_close_file(&logger_->_file) != FILE_SUCCESS)
                return LOGGER_ERROR_MAP_ITEM_DEALLOCATION_FAILED;

        memset(logger_, 0, sizeof(struct Logger_Handle));
    }

    // destruct the logger map
    if (container_map_destruct(state->_logger_map) != CONTAINER_SUCCESS)
        return LOGGER_ERROR_MAP_DESTRUCTION_FAILED;

    free(state);

    state = NULL;
    return LOGGER_SUCCESS;
}
