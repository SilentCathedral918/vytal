#include "logger.h"

#include <ctype.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "vytal/core/containers/map/map.h"
#include "vytal/core/containers/string/string.h"
#include "vytal/core/hal/clock/wall/wall.h"
#include "vytal/core/helpers/parse/parse.h"
#include "vytal/core/misc/console/console.h"

#define EXTRACT_FILENAME(filepath) ({                                  \
    ConstStr _backslash = strrchr((filepath), '\\');                   \
    ConstStr _slash     = strrchr((filepath), '/');                    \
    ConstStr _filename  = (_backslash > _slash) ? _backslash : _slash; \
    _filename ? _filename + 1 : (filepath);                           \
})

typedef struct Logger_State {
    Map  _logger_map;
    Bool _initialized;
} Logger_State;

struct Logger_Handle {
    String     _name;
    LoggerFlag _flags;
    File       _file;
};

static Logger_State *state = NULL;

LoggerResult _logger_write_to_file(File *file, ConstStr message) {
    if (file->_stream && file->_active) {
        ByteSize msg_length_ = strlen(message);

        if (platform_filesystem_write_data(file, (const VoidPtr)message, msg_length_) != FILE_SUCCESS)
            return LOGGER_ERROR_FILE_WRITE_FAILED;
    }

    return LOGGER_SUCCESS;
}

static void _logger_set_log_level_color(LoggerVerbosity verbosity) {
    switch (verbosity) {
        case LOG_VERBOSITY_FATAL:
            console_set_foreground_rgb(255, 85, 85);
            console_set_background_rgb(51, 0, 0);
            break;
        case LOG_VERBOSITY_ERROR:
            console_set_foreground_rgb(255, 110, 110);
            console_set_background_rgb(68, 0, 0);
            break;
        case LOG_VERBOSITY_WARNING:
            console_set_foreground_rgb(241, 250, 140);
            console_set_background_rgb(51, 51, 0);
            break;
        case LOG_VERBOSITY_INFO:
            console_set_foreground_rgb(0, 255, 255);
            console_set_background_rgb(11, 123, 176);
            break;
        case LOG_VERBOSITY_VERBOSE:
            console_set_foreground_rgb(98, 114, 164);
            console_set_background_rgb(34, 34, 68);
            break;
    }
}

LoggerResult logger_startup(File *file) {
    if (state) return LOGGER_ERROR_STATE_ALREADY_INITIALIZED;

    // allocate and configure the state
    state = calloc(1, sizeof(Logger_State));

    // construct the logger map
    if (container_map_construct(sizeof(struct Logger_Handle), &state->_logger_map) != CONTAINER_SUCCESS) {
        free(state);
        return LOGGER_ERROR_MAP_CONSTRUCTION_FAILED;
    }

    Str    line_           = calloc(1, LINE_BUFFER_MAX_SIZE);
    Logger staging_logger_ = calloc(1, sizeof(struct Logger_Handle));

    ByteSize seek_length_ = 0;
    while (platform_filesystem_read_line(file, &seek_length_, &line_) == FILE_SUCCESS) {
        memset(staging_logger_, 0, sizeof(struct Logger_Handle));

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

        Str psep_output_ = strstr(value_, "->");
        Str filepath_    = NULL;

        // extract output log filepath
        {
            if (psep_output_) {
                *psep_output_ = '\0';
                filepath_     = psep_output_ + 2;

                if (parse_clean_filepath(&filepath_) != PARSE_SUCCESS)
                    continue;
            }
        }

        // extract log flags
        Str   end_;
        Int64 logger_flags_ = strtol(value_, &end_, 2);

        // prepare logger data
        {
            if (container_string_construct(key_, &staging_logger_->_name) != CONTAINER_SUCCESS)
                return LOGGER_ERROR_MAP_ITEM_ALLOCATION_FAILED;

            staging_logger_->_flags = logger_flags_;
            staging_logger_->_file  = (File){0};

            if (filepath_) {
                if (platform_filesystem_open_file(&staging_logger_->_file, filepath_, FILE_IO_MODE_WRITE, FILE_MODE_TEXT) != FILE_SUCCESS)
                    return LOGGER_ERROR_FILE_OPEN_FAILED;
            }
        }

        // insert to map
        if (container_map_insert(&state->_logger_map, key_, staging_logger_) != CONTAINER_SUCCESS)
            return LOGGER_ERROR_MAP_ITEM_ALLOCATION_FAILED;
    }

    free(staging_logger_);
    free(line_);

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
                return LOGGER_ERROR_FILE_CLOSE_FAILED;

        memset(logger_, 0, sizeof(struct Logger_Handle));
    }

    // destruct the logger map
    if (container_map_destruct(state->_logger_map) != CONTAINER_SUCCESS)
        return LOGGER_ERROR_MAP_DESTRUCTION_FAILED;

    free(state);

    state = NULL;
    return LOGGER_SUCCESS;
}

LoggerResult logger_print(
    ConstStr        logger_id,
    LoggerVerbosity verbosity,
    ConstStr        at_file,
    Int32           at_line,
    ConstStr        at_function,
    ConstStr        message,
    ...) {
    if (!state) return LOGGER_ERROR_STATE_NOT_INITIALIZED;
    if (!logger_id || !message) return LOGGER_ERROR_INVALID_PARAM;

    // extract filename from filepath
    ConstStr filename_ = EXTRACT_FILENAME(at_file);

    Logger logger_;
    if (container_map_search(state->_logger_map, logger_id, (VoidPtr *)&logger_) != CONTAINER_SUCCESS) {
        free(state);
        return LOGGER_ERROR_INVALID_LOGGER_NAME;
    }

    ConstStr verbosity_values_[] = {"FATAL", "ERROR", "WARNING", "INFO", "VERBOSE"};
    if ((verbosity < LOG_VERBOSITY_FATAL) || (verbosity > LOG_VERBOSITY_VERBOSE))
        return LOGGER_ERROR_INVALID_PARAM;
    ConstStr verbosity_ = verbosity_values_[verbosity];

    _logger_set_log_level_color(verbosity);

    // get timestamp
    Char      log_time_[64] = {'\0'};
    WallClock clock_        = clock_wall_now();
    strftime(log_time_, sizeof(log_time_), "%F %T", &(clock_._time_info));

    // format log message
    VaList va_list_;
    Char   log_content_[LINE_BUFFER_MAX_SIZE] = {'\0'};
    va_start(va_list_, message);
    vsnprintf(log_content_, sizeof(log_content_), message, va_list_);
    va_end(va_list_);

    // ideal total width for most modern terminals = 120

    Int32 padding_           = 4;
    Int32 logger_name_width_ = (container_string_size(logger_->_name) * 2) + padding_;
    Int32 timestamp_width_   = strlen(log_time_) + padding_;
    Int32 verbosity_width_   = (strlen(verbosity_) * 2) + padding_;
    Int32 file_line_width_   = 20 + padding_;
    Int32 func_name_width_   = 25 + padding_;
    Int32 total_width_       = logger_name_width_ + timestamp_width_ + verbosity_width_ + file_line_width_ + func_name_width_;

    Char file_display_[FILENAME_BUFFER_MAX_SIZE];
    Char func_display_[FILENAME_BUFFER_MAX_SIZE];

    // format file and func display
    // add "..." if truncated (some filenames or function names may be too long)
    {
        if (VYTAL_BITFLAG_IF_SET(logger_->_flags, LOG_FLAG_FILE_LINE)) {
            if (strlen(filename_) > file_line_width_ - padding_)
                strcpy(file_display_ + (file_line_width_ - padding_) - 3, "...");

            sprintf(file_display_, "%s (%d)", filename_, at_line);
        }

        if (VYTAL_BITFLAG_IF_SET(logger_->_flags, LOG_FLAG_FUNC_NAME)) {
            strncpy(func_display_, at_function, func_name_width_ - padding_);

            if (strlen(at_function) > func_name_width_ - padding_)
                strcpy(func_display_ + (func_name_width_ - padding_) - 3, "...");
        }
    }

    // print log metadata
    {
        console_reset();
        console_write("\n");

        // Logger name
        console_set_foreground_rgb(234, 202, 45);  // gold
        console_set_background_rgb(220, 20, 60);   // crimson red
        console_write("  %*s  ", logger_name_width_ - padding_, container_string_get(logger_->_name));
        console_reset();

        // timestamp
        if (VYTAL_BITFLAG_IF_SET(logger_->_flags, LOG_FLAG_TIMESTAMP)) {
            console_set_foreground_rgb(139, 233, 253);
            console_set_background_rgb(30, 30, 63);
            console_write("  %*s  ", timestamp_width_ - padding_, log_time_);
            console_reset();
        }

        // filename & line
        if (VYTAL_BITFLAG_IF_SET(logger_->_flags, LOG_FLAG_FILE_LINE)) {
            console_set_foreground_rgb(45, 255, 246);
            console_set_background_rgb(42, 30, 64);
            console_write("  %*s  ", file_line_width_ - padding_, file_display_);
            console_reset();
        }

        // function name
        if (VYTAL_BITFLAG_IF_SET(logger_->_flags, LOG_FLAG_FUNC_NAME)) {
            console_set_foreground_rgb(61, 255, 43);
            console_set_background_rgb(28, 15, 44);
            console_write("  %*s  ", func_name_width_ - padding_, func_display_);
            console_reset();
        }

        // verbosity
        _logger_set_log_level_color(verbosity);  // set background for verbosity
        console_write("  %*s  ", verbosity_width_ - padding_, verbosity_);
        console_reset();

        console_write("\n");
    }

    // dynamically wrap message
    {
        Int32 msg_length_ = strlen(log_content_);
        Int32 line_start_ = 0;

        while (line_start_ < msg_length_) {
            // determine end index (wrap at word boundaries)
            Int32 line_end_ = line_start_ + (total_width_ - padding_);

            // make sure we do not exceed message length
            if (line_end_ > msg_length_) line_end_ = msg_length_;

            if ((line_end_ < msg_length_) && (log_content_[line_end_] != ' ')) {
                Int32 search_pos_ = line_end_;
                while ((search_pos_ > line_start_) && (log_content_[search_pos_] != ' ')) --search_pos_;

                if (search_pos_ > line_start_) line_end_ = search_pos_;
            }

            // force wrap around if there is no space
            if (line_end_ == line_start_) line_end_ = line_start_ + (total_width_ - padding_);

            // print message block
            {
                _logger_set_log_level_color(verbosity);
                console_write("  %-*.*s  ", total_width_ - padding_, line_end_ - line_start_, &log_content_[line_start_]);
                console_reset();
                console_write("\n");
            }

            // move to next line
            line_start_ = line_end_;
            while ((line_start_ < msg_length_) && (log_content_[line_start_] == ' ')) ++line_start_;  // skip extra spaces
        }
    }

    // write to file (if exists, otherwise do nothing)
    Char log_file_entry_[LINE_BUFFER_MAX_SIZE * 2];
    snprintf(log_file_entry_, sizeof(log_file_entry_), "[%s] (%s:%d) <%s> %s: %s\n",
             log_time_, filename_, at_line, at_function, verbosity_, log_content_);

    // verbose messages tend not to be written to output file
    return (verbosity == LOG_VERBOSITY_VERBOSE) ? CONTAINER_SUCCESS : _logger_write_to_file(&(logger_->_file), log_file_entry_);
}
