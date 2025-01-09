#pragma once

#include "vytal/core/platform/filesystem/filesystem.h"
#include "vytal/defines/core/logger.h"
#include "vytal/defines/shared.h"

Bool logger_startup(void);
Bool logger_shutdown(void);

VT_API Bool logger_add(ConstStr logger_id, ConstStr filepath);
VT_API Bool logger_remove(ConstStr logger_id);
VT_API Bool logger_toggle_flag(const LogFlag flag);
VT_API Bool logger_toggle_timestamp(void);
VT_API Bool logger_toggle_file_line(void);
VT_API Bool logger_print(ConstStr logger_id, LogVerbosity verbosity, ConstStr at_file, Int32 at_line, ConstStr message, ...);

#define VT_LOG_FATAL(logger_id, message, ...) logger_print(logger_id, LOG_VERBOSITY_FATAL, platform_fs_get_filename_from_path(__FILE__), __LINE__, message, ##__VA_ARGS__)
#define VT_LOG_ERROR(logger_id, message, ...) logger_print(logger_id, LOG_VERBOSITY_ERROR, platform_fs_get_filename_from_path(__FILE__), __LINE__, message, ##__VA_ARGS__)
#define VT_LOG_WARNING(logger_id, message, ...) logger_print(logger_id, LOG_VERBOSITY_WARNING, platform_fs_get_filename_from_path(__FILE__), __LINE__, message, ##__VA_ARGS__)
#define VT_LOG_INFO(logger_id, message, ...) logger_print(logger_id, LOG_VERBOSITY_INFO, platform_fs_get_filename_from_path(__FILE__), __LINE__, message, ##__VA_ARGS__)

#if defined(VT_DEBUG)
#    define VT_LOG_DEBUG(logger_id, message, ...) logger_print(logger_id, LOG_VERBOSITY_DEBUG, platform_fs_get_filename_from_path(__FILE__), __LINE__, message, ##__VA_ARGS__)
#    define VT_LOG_TRACE(logger_id, message, ...) logger_print(logger_id, LOG_VERBOSITY_TRACE, platform_fs_get_filename_from_path(__FILE__), __LINE__, message, ##__VA_ARGS__)

#else
#    define VT_LOG_DEBUG(logger_id, message, ...)
#    define VT_LOG_TRACE(logger_id, message, ...)

#endif
