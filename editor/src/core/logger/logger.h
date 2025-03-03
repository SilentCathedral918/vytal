#pragma once

// for logger
#include <vytal.h>

#define EDITOR_LOG_FATAL(message, ...) \
    logger_print("VYTAL_EDITOR", LOG_VERBOSITY_FATAL, __FILE__, __LINE__, __func__, message, ##__VA_ARGS__)

#define EDITOR_LOG_ERROR(message, ...) \
    logger_print("VYTAL_EDITOR", LOG_VERBOSITY_ERROR, __FILE__, __LINE__, __func__, message, ##__VA_ARGS__)

#define EDITOR_LOG_WARNING(message, ...) \
    logger_print("VYTAL_EDITOR", LOG_VERBOSITY_WARNING, __FILE__, __LINE__, __func__, message, ##__VA_ARGS__)

#define EDITOR_LOG_INFO(message, ...) \
    logger_print("VYTAL_EDITOR", LOG_VERBOSITY_INFO, __FILE__, __LINE__, __func__, message, ##__VA_ARGS__)

#define EDITOR_LOG_VERBOSE(message, ...) \
    logger_print("VYTAL_EDITOR", LOG_VERBOSITY_VERBOSE, __FILE__, __LINE__, __func__, message, ##__VA_ARGS__)
