#pragma once

#include "vytal/core/platform/filesystem/filesystem.h"
#include "vytal/defines/core/logger.h"
#include "vytal/defines/shared.h"

VYTAL_API LoggerResult logger_startup(void);
VYTAL_API LoggerResult logger_shutdown(void);

VYTAL_API LoggerResult logger_print(
    ConstStr        logger_id,
    LoggerVerbosity verbosity,
    ConstStr        at_file,
    Int32           at_line,
    ConstStr        at_function,
    ConstStr        message,
    ...);

#define VYTAL_LOG_FATAL(message, ...) \
    logger_print("VYTAL", LOG_VERBOSITY_FATAL, __FILE__, __LINE__, __func__, message, ##__VA_ARGS__)

#define VYTAL_LOG_ERROR(message, ...) \
    logger_print("VYTAL", LOG_VERBOSITY_ERROR, __FILE__, __LINE__, __func__, message, ##__VA_ARGS__)

#define VYTAL_LOG_WARNING(message, ...) \
    logger_print("VYTAL", LOG_VERBOSITY_WARNING, __FILE__, __LINE__, __func__, message, ##__VA_ARGS__)

#define VYTAL_LOG_INFO(message, ...) \
    logger_print("VYTAL", LOG_VERBOSITY_INFO, __FILE__, __LINE__, __func__, message, ##__VA_ARGS__)

#define VYTAL_LOG_VERBOSE(message, ...) \
    logger_print("VYTAL", LOG_VERBOSITY_VERBOSE, __FILE__, __LINE__, __func__, message, ##__VA_ARGS__)

#define LOGGER_LOG_FATAL(logger_id, message, ...) \
    logger_print(logger_id, LOG_VERBOSITY_FATAL, __FILE__, __LINE__, __func__, message, ##__VA_ARGS__)

#define LOGGER_LOG_ERROR(logger_id, message, ...) \
    logger_print(logger_id, LOG_VERBOSITY_ERROR, __FILE__, __LINE__, __func__, message, ##__VA_ARGS__)

#define LOGGER_LOG_WARNING(logger_id, message, ...) \
    logger_print(logger_id, LOG_VERBOSITY_WARNING, __FILE__, __LINE__, __func__, message, ##__VA_ARGS__)

#define LOGGER_LOG_INFO(logger_id, message, ...) \
    logger_print(logger_id, LOG_VERBOSITY_INFO, __FILE__, __LINE__, __func__, message, ##__VA_ARGS__)

#define LOGGER_LOG_VERBOSE(logger_id, message, ...) \
    logger_print(logger_id, LOG_VERBOSITY_VERBOSE, __FILE__, __LINE__, __func__, message, ##__VA_ARGS__)
