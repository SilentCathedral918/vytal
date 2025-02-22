#include "logger.h"

#include <ctype.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "vytal/core/configuration/cvar/cvar.h"
#include "vytal/core/containers/map/map.h"
#include "vytal/core/containers/string/string.h"
#include "vytal/core/hal/clock/wall/wall.h"
#include "vytal/core/misc/console/console.h"

#define LOG_MESSAGE_WIDTH 70  // max width for messages
#define FILE_NAME_MAX 16      // max width for filename
#define FUNC_NAME_MAX 16      // max width for function name

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

static Logger_State *state                 = NULL;
static ConstStr      loggers_cvar_filepath = "loggers.cfg";

LoggerResult _logger_write_to_file(File *file, ConstStr message) {
    if (file->_stream && file->_active) {
        ByteSize msg_length_ = strlen(message);

        if (platform_filesystem_write_data(file, (const VoidPtr)message, msg_length_) != FILE_SUCCESS)
            return LOGGER_ERROR_FILE_WRITE_FAILED;
    }

    return LOGGER_SUCCESS;
}

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

static void _logger_set_log_level_color(LoggerVerbosity verbosity) {
    switch (verbosity) {
        case LOG_VERBOSITY_FATAL:
            console_set_foreground_rgb(255, 255, 255);  // white text
            console_set_background_rgb(255, 0, 0);      // red background
            break;

        case LOG_VERBOSITY_ERROR:
            console_set_foreground_rgb(255, 255, 255);
            console_set_background_rgb(255, 100, 0);  // orange background
            break;

        case LOG_VERBOSITY_WARNING:
            console_set_foreground_rgb(0, 0, 0);
            console_set_background_rgb(255, 255, 0);  // yellow background
            break;

        case LOG_VERBOSITY_INFO:
            console_set_foreground_rgb(255, 255, 255);
            console_set_background_rgb(0, 0, 255);  // blue background
            break;

        case LOG_VERBOSITY_VERBOSE:
            console_set_foreground_rgb(255, 255, 255);
            console_set_background_rgb(128, 0, 128);  // purple background
            break;
    }
}

LoggerResult logger_startup(void) {
    if (state) return LOGGER_ERROR_STATE_ALREADY_INITIALIZED;

    // allocate and configure the state
    state = malloc(sizeof(Logger_State));
    memset(state, 0, sizeof(Logger_State));

    // construct the logger map
    if (container_map_construct(sizeof(struct Logger_Handle), &state->_logger_map) != CONTAINER_SUCCESS) {
        free(state);
        return LOGGER_ERROR_MAP_CONSTRUCTION_FAILED;
    }

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
        if (!pequal_) continue;

        *pequal_ = '\0';

        Str key_   = trimmed_;
        Str psep_  = strchr(key_, '.');
        Str name_  = _logger_trim_whitespace(psep_ + 1);
        Str value_ = _logger_trim_whitespace(pequal_ + 1);
        Str end_;
        Str psep_output_ = strstr(value_, "->");
        Str filepath_    = NULL;
        if (psep_output_) {
            *psep_output_ = '\0';
            filepath_     = _logger_trim_whitespace(psep_output_ + 2);
        }
        Int64 logger_flags_ = strtol(value_, &end_, 2);

        // prepare logger data
        {
            if (container_string_construct(name_, &staging_logger_->_name) != CONTAINER_SUCCESS)
                return LOGGER_ERROR_MAP_ITEM_ALLOCATION_FAILED;

            staging_logger_->_flags = logger_flags_;
            staging_logger_->_file  = (File){0};

            if (filepath_) {
                if (platform_filesystem_open_file(&staging_logger_->_file, filepath_, FILE_IO_MODE_WRITE, FILE_MODE_TEXT) != FILE_SUCCESS)
                    return LOGGER_ERROR_FILE_OPEN_FAILED;
            }
        }

        // insert to map
        if (container_map_insert(&state->_logger_map, name_, staging_logger_) != CONTAINER_SUCCESS)
            return LOGGER_ERROR_MAP_ITEM_ALLOCATION_FAILED;
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

    Int32 total_width       = 120;
    Int32 logger_name_width = (Int32)(total_width * 0.20);
    Int32 timestamp_width   = (Int32)(total_width * 0.20);
    Int32 verbosity_width   = (Int32)(total_width * 0.15);
    Int32 filename_width    = (Int32)(total_width * 0.20);
    Int32 funcname_width    = (Int32)(total_width * 0.25);

    Char file_display_[FILENAME_BUFFER_MAX_SIZE];
    Char func_display_[FILENAME_BUFFER_MAX_SIZE];

    // format file and func display
    // add "..." if truncated (some filenames or function names may be too long)
    {
        strncpy(file_display_, filename_, filename_width);
        strncpy(func_display_, at_function, funcname_width);

        if (strlen(filename_) > filename_width)
            strcpy(file_display_ + filename_width - 3, "...");

        if (strlen(at_function) > funcname_width)
            strcpy(func_display_ + funcname_width - 3, "...");
    }

    // print log metadata
    {
        console_reset();
        console_write("\n");

        // Logger name
        console_set_foreground_rgb(102, 255, 51);  // green
        console_set_background_rgb(0, 0, 153);     // dark blue
        console_write(" %*s ", logger_name_width, container_string_get(logger_->_name));
        console_reset();

        // timestamp
        console_set_foreground_rgb(255, 255, 255);  // white
        console_set_background_rgb(0, 0, 0);        // black
        console_write(" %-*s ", timestamp_width, log_time_);
        console_reset();

        // verbosity
        _logger_set_log_level_color(verbosity);  // set background for verbosity
        console_write(" %*s ", verbosity_width, verbosity_);
        console_reset();

        // filename & line
        console_set_foreground_rgb(255, 255, 255);  // white
        console_write(" %*s:%d ", filename_width, file_display_, at_line);
        console_reset();

        // function name
        console_set_foreground_rgb(0, 255, 0);  // function name (green)
        console_write(" %-*s ", funcname_width, func_display_);
        console_reset();
        console_write("\n");
    }

    // dynamically wrap message
    {
        Int32 msg_length_ = strlen(log_content_);
        Int32 line_start_ = 0;

        while (line_start_ < msg_length_) {
            // determine end index (wrap at word boundaries)
            Int32 line_end_ = line_start_ + total_width;
            if (line_end_ > msg_length_) line_end_ = msg_length_;

            if ((line_end_ < msg_length_) && (log_content_[line_end_] != ' '))
                while ((line_end_ > line_start_) && (log_content_[line_end_]) != ' ') --line_end_;

            if (line_end_ == line_start_) line_end_ = line_start_ + total_width;  // force wrap around if there is no space

            // print message block
            {
                console_set_foreground_rgb(255, 255, 255);  // white text
                console_write(" %-*s", total_width, &log_content_[line_start_]);
                console_reset();
                console_write("\n");
            }

            // move to next line
            line_start_ = line_end_;
            while (log_content_[line_start_] == ' ') ++line_start_;  // skip extra spaces
        }
    }

    // write to file (if exists, otherwise do nothing)
    Char log_file_entry_[LINE_BUFFER_MAX_SIZE * 2];
    snprintf(log_file_entry_, sizeof(log_file_entry_), "[%s] (%s:%d) <%s> %s: %s\n",
             log_time_, filename_, at_line, at_function, verbosity_, log_content_);

    // verbose messages tend not to be written to output file
    return (verbosity == LOG_VERBOSITY_VERBOSE) ? CONTAINER_SUCCESS : _logger_write_to_file(&(logger_->_file), log_file_entry_);
}
