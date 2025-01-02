#include "logger.h"

#include "vytal/core/containers/map/map.h"
#include "vytal/core/hal/clock/wallclock.h"
#include "vytal/core/hal/memory/vtmem.h"
#include "vytal/core/misc/console/console.h"
#include "vytal/core/misc/string/vtstr.h"

#define OUTPUT_BUFFER_MAX_SIZE VT_SIZE_KB_MULT(32) // 32 KB
#define ENGINE_LOGGER_ID "Engine"
#define ENGINE_LOG_FILEPATH "log/engine.txt"

typedef struct Logger_State {
    Map     _log_map;
    LogFlag _log_flags;
    Bool    _initialized;
} Logger_State;

static Logger_State *state = NULL;

Bool _logger_append_to_file(FileHandle *handle, ConstStr message) {
    if (!handle->_active || !handle->_stream)
        return false;

    ByteSize message_length_ = misc_str_strlen(message);
    return platform_fs_file_write_data(handle, VT_CAST(const VoidPtr, message), message_length_);
}

Bool _logger_add_default(void) {
    Logger new_logger_;
    hal_mem_memzero(VT_CAST(const VoidPtr, new_logger_._id), sizeof(new_logger_._id));
    misc_str_strncpy(new_logger_._id, ENGINE_LOGGER_ID, sizeof(new_logger_._id));

    if (!platform_fs_open_file(&(new_logger_._file_handle), ENGINE_LOG_FILEPATH, FILE_IO_MODE_WRITE, FILE_MODE_TEXT)) {
        // in case the directory doesn't exist
        {
            // create one...

            ConstStr dir_sep_ = platform_fs_get_filename_from_path(ENGINE_LOG_FILEPATH);
            if (!dir_sep_)
                return false;

            ByteSize idx_                              = VT_CAST(PtrDiff, dir_sep_ - ENGINE_LOG_FILEPATH);
            Char     dir_name_[OUTPUT_BUFFER_MAX_SIZE] = {'\0'};
            misc_str_strncpy(dir_name_, ENGINE_LOG_FILEPATH, idx_);

            if (!platform_fs_create_directory(dir_name_))
                return false;
        }

        // create new file in the directory
        if (!platform_fs_open_file(&(new_logger_._file_handle), ENGINE_LOG_FILEPATH, FILE_IO_MODE_WRITE, FILE_MODE_TEXT))
            return false;
    }

    // insert new logger to map
    return container_map_insert(state->_log_map, ENGINE_LOGGER_ID, &new_logger_);
}

Bool logger_startup(void) {
    if (state)
        return false;

    // allocate the state
    state = hal_mem_malloc(sizeof(Logger_State));

    // configure the state members
    {
        state->_log_map = container_map_construct(sizeof(Logger), NULL);

        state->_log_flags   = LOG_FLAG_NONE;
        state->_initialized = true;
    }

    // add engine logger
    return _logger_add_default();
}

Bool logger_shutdown(void) {
    if (!state || !state->_initialized)
        return false;

    // free and set members to zero
    {
        container_map_destruct(state->_log_map);
        hal_mem_memzero(state, sizeof(Logger_State));
    }

    // free self
    {
        hal_mem_free(state);
        state = NULL;
    }

    return true;
}

Bool logger_add(ConstStr logger_id, ConstStr filepath) {
    if (!state || !state->_initialized || !state->_log_map || !logger_id ||
        (misc_str_strcmp(logger_id, ENGINE_LOGGER_ID, false) == 0))
        return false;

    Logger new_logger_;
    hal_mem_memzero(VT_CAST(const VoidPtr, new_logger_._id), sizeof(new_logger_._id));
    misc_str_strncpy(new_logger_._id, logger_id, sizeof(new_logger_._id));

    if (!platform_fs_open_file(&(new_logger_._file_handle), filepath, FILE_IO_MODE_WRITE, FILE_MODE_TEXT)) {
        // in case the directory doesn't exist
        {
            // create one...

            ConstStr dir_sep_ = platform_fs_get_filename_from_path(filepath);
            if (!dir_sep_)
                return false;

            ByteSize idx_                              = VT_CAST(PtrDiff, dir_sep_ - filepath);
            Char     dir_name_[OUTPUT_BUFFER_MAX_SIZE] = {'\0'};
            misc_str_strncpy(dir_name_, filepath, idx_);

            if (!platform_fs_create_directory(dir_name_))
                return false;
        }

        // create new file in the directory
        if (!platform_fs_open_file(&(new_logger_._file_handle), filepath, FILE_IO_MODE_WRITE, FILE_MODE_TEXT))
            return false;
    }

    return container_map_insert(state->_log_map, logger_id, &new_logger_);
}

Bool logger_remove(ConstStr logger_id) {
    if (!state || !state->_initialized || !logger_id || (misc_str_strcmp(logger_id, ENGINE_LOGGER_ID, false) == 0))
        return false;

    if (!state->_log_map || container_map_isempty(state->_log_map))
        return false;

    return container_map_remove(state->_log_map, logger_id);
}

Bool logger_toggle_flag(const LogFlag flag) {
    if (!state || !state->_initialized)
        return false;

    VT_BITFLAG_TOGGLE(state->_log_flags, flag);
    return true;
}

Bool logger_toggle_timestamp(void) { return logger_toggle_flag(LOG_FLAG_TIMESTAMP); }

Bool logger_toggle_file_line(void) { return logger_toggle_flag(LOG_FLAG_FILE_LINE); }

Bool logger_print(ConstStr logger_id, LogVerbosity verbosity, ConstStr at_file, Int32 at_line, ConstStr message, ...) {
    if (!state || !logger_id)
        return false;

    Logger   logger_ = container_map_get_value(state->_log_map, Logger, logger_id);
    ConstStr verbosity_;

    switch (verbosity) {
    case LOG_VERBOSITY_FATAL:
        verbosity_ = "FATAL";
        break;

    case LOG_VERBOSITY_ERROR:
        verbosity_ = "ERROR";
        break;

    case LOG_VERBOSITY_WARNING:
        verbosity_ = "WARNING";
        break;

    case LOG_VERBOSITY_INFO:
        verbosity_ = "INFO";
        break;

    case LOG_VERBOSITY_DEBUG:
        verbosity_ = "DEBUG";
        break;

    case LOG_VERBOSITY_TRACE:
        verbosity_ = "TRACE";
        break;

    default:
        break;
    }

    Char log_content_[OUTPUT_BUFFER_MAX_SIZE] = {'\0'};
    Char log_time_[64]                        = {'\0'};
    Char log_file_[OUTPUT_BUFFER_MAX_SIZE]    = {'\0'};

    // format log content
    {
        VaList va_list_;
        va_start(va_list_, message);
        vsnprintf(log_content_, OUTPUT_BUFFER_MAX_SIZE, message, va_list_);
        va_end(va_list_);
    }

    // format log time
    {
        WallClock clock_ = hal_wallclock_now();
        strftime(log_time_, 64, "%F %T", &(clock_._time_info));
    }

    // print logger id
    {
        misc_console_setforeground_rgb(102, 255, 51);
        misc_console_setbackground_rgb(0, 0, 153);
        misc_console_write(" %s ", logger_id);
        misc_console_reset();
    }

    // print verbosity
    switch (verbosity) {
    case LOG_VERBOSITY_FATAL: {
        misc_console_setforeground_rgb(255, 153, 153);
        misc_console_setbackground_rgb(255, 0, 0);
        misc_console_write(" %s ", verbosity_);
        misc_console_reset();
    } break;

    case LOG_VERBOSITY_ERROR: {
        misc_console_setforeground_rgb(0, 255, 255);
        misc_console_setbackground_rgb(255, 0, 0);
        misc_console_write(" %s ", verbosity_);
        misc_console_reset();
    } break;

    case LOG_VERBOSITY_WARNING: {
        misc_console_setforeground_rgb(0, 64, 255);
        misc_console_setbackground_rgb(229, 229, 16);
        misc_console_write(" %s ", verbosity_);
        misc_console_reset();
    } break;

    case LOG_VERBOSITY_INFO: {
        misc_console_setforeground_rgb(101, 191, 104);
        misc_console_setbackground_rgb(0, 153, 255);
        misc_console_write(" %s ", verbosity_);
        misc_console_reset();
    } break;

    case LOG_VERBOSITY_DEBUG: {
        misc_console_setforeground_rgb(0, 255, 255);
        misc_console_setbackground_rgb(0, 0, 204);
        misc_console_write(" %s ", verbosity_);
        misc_console_reset();
    } break;

    case LOG_VERBOSITY_TRACE: {
        misc_console_setforeground_rgb(0, 255, 255);
        misc_console_setbackground_rgb(51, 102, 153);
        misc_console_write(" %s ", verbosity_);
        misc_console_reset();
    } break;

    default:
        break;
    }

    // print toggled log flags
    {
        if (VT_BITFLAG_IF_SET(state->_log_flags, LOG_FLAG_TIMESTAMP))
            misc_console_write(" [%s]", log_time_);

        if (VT_BITFLAG_IF_SET(state->_log_flags, LOG_FLAG_FILE_LINE))
            misc_console_write("(%s:%d)", at_file, at_line);
    }

    // print out the log content
    misc_console_writeln(" %s", log_content_);

    // write to log file under associated logger ID
    misc_str_fmt(log_file_, OUTPUT_BUFFER_MAX_SIZE, "[%s] (%s, %d) %s: %s \n", log_time_, at_file, at_line, verbosity_,
                 log_content_);
    return _logger_append_to_file(&(logger_._file_handle), log_file_);
}
