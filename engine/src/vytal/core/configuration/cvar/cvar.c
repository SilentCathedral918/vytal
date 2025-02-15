#include "cvar.h"

#include <ctype.h>
#include <stdlib.h>
#include <string.h>

#include "vytal/core/hash/hash.h"
#include "vytal/core/platform/filesystem/filesystem.h"

static CVar *cvar_table = NULL;

VYTAL_INLINE UInt32 _cvar_hash(ConstStr name) {
    return (UInt32)hash_str(name, HASH_MODE_XX64) % CVAR_HASHMAP_SIZE;
}

VYTAL_INLINE Str _cvar_trim_whitespace(Str str) {
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

VYTAL_INLINE CVarType _cvar_detect_type(Str value) {
    if (!value) return CVAR_TYPE_STRING;
    if (*value == '\0') return CVAR_TYPE_STRING;

    Str end_;

    // boolean
    if (!strcasecmp(value, "true") || !strcasecmp(value, "false"))
        return CVAR_TYPE_BOOL;

    // integer
    strtol(value, &end_, 10);
    if (*end_ == '\0') return CVAR_TYPE_INT;

    // float
    strtof(value, &end_);
    if (*end_ == '\0') return CVAR_TYPE_FLOAT;

    return CVAR_TYPE_STRING;
}

CVarResult cvar_startup(void) {
    cvar_table = malloc(sizeof(CVar) * CVAR_HASHMAP_SIZE);
    memset(cvar_table, 0, sizeof(CVar) * CVAR_HASHMAP_SIZE);

    return CVAR_SUCCESS;
}

CVarResult cvar_shutdown(void) {
    if (!cvar_table) return CVAR_ERROR_NOT_INITIALIZED;

    for (size_t i = 0; i < CVAR_HASHMAP_SIZE; ++i) {
        CVar *cvar_ = &cvar_table[i];
        if (!cvar_->_name) continue;

        free(cvar_->_name);
        free(cvar_->_description);

        if (cvar_->_type == CVAR_TYPE_STRING) {
            free(cvar_->_value._str);
            free(cvar_->_default_value._str);
        }
    }

    free(cvar_table);
    cvar_table = NULL;

    return CVAR_SUCCESS;
}

CVarResult cvar_register(ConstStr name, CVarType type, CVarData default_value, Bool persistent, ConstStr description) {
    if (!name) return CVAR_ERROR_INVALID_PARAM;

    UInt32 index_ = _cvar_hash(name);

    // check if there is already a cvar with matching name
    {
        CVar *cvar_ = &cvar_table[index_];
        if (cvar_->_name)
            return cvar_set(name, default_value);
    }

    CVar *new_cvar_ = &cvar_table[index_];

    new_cvar_->_name = strdup(name);
    if (!new_cvar_->_name) return CVAR_ERROR_MEMORY_ALLOCATION;

    new_cvar_->_type       = type;
    new_cvar_->_persistent = persistent;

    if (description) {
        new_cvar_->_description = strdup(description);
        if (!new_cvar_->_description) {
            free(new_cvar_->_name);
            return CVAR_ERROR_MEMORY_ALLOCATION;
        }
    }

    switch (type) {
        case CVAR_TYPE_INT:
            new_cvar_->_value._int         = default_value._int;
            new_cvar_->_default_value._int = default_value._int;
            break;

        case CVAR_TYPE_FLOAT:
            new_cvar_->_value._flt         = default_value._flt;
            new_cvar_->_default_value._flt = default_value._flt;
            break;

        case CVAR_TYPE_BOOL:
            new_cvar_->_value._bool         = default_value._bool;
            new_cvar_->_default_value._bool = default_value._bool;
            break;

        case CVAR_TYPE_STRING:
            new_cvar_->_value._str = strdup(default_value._str ? default_value._str : "");
            if (!new_cvar_->_value._str) {
                free(new_cvar_->_name);
                free(new_cvar_->_description);
                return CVAR_ERROR_MEMORY_ALLOCATION;
            }

            new_cvar_->_default_value._str = strdup(default_value._str ? default_value._str : "");
            if (!new_cvar_->_default_value._str) {
                free(new_cvar_->_name);
                free(new_cvar_->_description);
                free(new_cvar_->_value._str);
                return CVAR_ERROR_MEMORY_ALLOCATION;
            }

            break;

        default:
            free(new_cvar_->_name);
            free(new_cvar_->_description);

            return CVAR_ERROR_INVALID_DATA_TYPE;
    }

    return CVAR_SUCCESS;
}

CVarResult cvar_get(ConstStr name, CVarData *out_value) {
    if (!name || !out_value) return CVAR_ERROR_INVALID_PARAM;

    UInt32 index_ = _cvar_hash(name);
    CVar  *cvar_  = &cvar_table[index_];

    if (!cvar_->_name) return CVAR_ERROR_NOT_REGISTERED;

    *out_value = cvar_->_value;
    return CVAR_SUCCESS;
}

CVarResult cvar_set(ConstStr name, CVarData new_value) {
    if (!name) return CVAR_ERROR_INVALID_PARAM;

    UInt32 index_ = _cvar_hash(name);
    CVar  *cvar_  = &cvar_table[index_];
    if (!cvar_->_name) return CVAR_ERROR_NOT_REGISTERED;

    if (cvar_->_type == CVAR_TYPE_STRING) {
        free(cvar_->_value._str);
        cvar_->_value._str = strdup(new_value._str ? new_value._str : "");

        if (!cvar_->_value._str) return CVAR_ERROR_MEMORY_ALLOCATION;
    } else {
        cvar_->_value = new_value;
    }

    return CVAR_SUCCESS;
}

CVarResult cvar_load_from_file(ConstStr filepath, ConstStr description) {
    if (!filepath) return CVAR_ERROR_INVALID_PARAM;
    if (*filepath == '\0') return CVAR_ERROR_INVALID_PARAM;

    File       cvar_file_ = {0};
    FileResult open_file_ = platform_filesystem_open_file(&cvar_file_, filepath, FILE_IO_MODE_READ, FILE_MODE_TEXT);
    if (open_file_ != FILE_SUCCESS) return CVAR_ERROR_FILE_OPEN_FAILED;

    Str cvar_line_ = malloc(LINE_BUFFER_MAX_SIZE);
    while (platform_filesystem_read_line(&cvar_file_, NULL, &cvar_line_) == FILE_SUCCESS) {
        Str trimmed_ = _cvar_trim_whitespace(cvar_line_);

        // ignore comments and empty lines
        if (*trimmed_ == '#' || *trimmed_ == '\0') continue;

        Str pequal_ = strchr(trimmed_, '=');
        if (pequal_) {
            *pequal_ = '\0';

            Str key_   = trimmed_;
            Str value_ = _cvar_trim_whitespace(pequal_ + 1);
            Str end_;

            UInt32 index_ = _cvar_hash(trimmed_);
            CVar  *cvar_  = &cvar_table[index_];

            // already exists, simply update the value
            if (cvar_->_name) {
                switch (cvar_->_type) {
                    case CVAR_TYPE_INT: {
                        Int32 val_ = strtol(value_, &end_, 10);
                        cvar_set(trimmed_, (CVarData){._int = val_});
                        break;
                    }

                    case CVAR_TYPE_FLOAT: {
                        Flt32 val_ = strtof(value_, &end_);
                        cvar_set(trimmed_, (CVarData){._flt = val_});
                        break;
                    }

                    case CVAR_TYPE_BOOL: {
                        Bool val_ = !strcasecmp(value_, "true");
                        cvar_set(trimmed_, (CVarData){._bool = val_});
                        break;
                    }

                    case CVAR_TYPE_STRING: {
                        cvar_set(trimmed_, (CVarData){._str = value_});
                        break;
                    }

                    default:
                        return CVAR_ERROR_INVALID_DATA_TYPE;
                }

                continue;
            }

            // otherwise
            else {
                CVarType value_type_ = _cvar_detect_type(value_);
                switch (value_type_) {
                    case CVAR_TYPE_INT: {
                        Int32 val_ = strtol(value_, &end_, 10);
                        cvar_register(key_, value_type_, (CVarData){._int = val_}, true, description);
                        break;
                    }

                    case CVAR_TYPE_FLOAT: {
                        Flt32 val_ = strtof(value_, &end_);
                        cvar_register(key_, value_type_, (CVarData){._flt = val_}, true, description);
                        break;
                    }

                    case CVAR_TYPE_BOOL: {
                        Bool val_ = !strcasecmp(value_, "true");
                        cvar_register(key_, value_type_, (CVarData){._bool = val_}, true, description);
                        break;
                    }

                    case CVAR_TYPE_STRING: {
                        cvar_register(key_, value_type_, (CVarData){._str = value_}, true, description);
                        break;
                    }

                    default:
                        return CVAR_ERROR_INVALID_DATA_TYPE;
                }
            }
        }
    }
    free(cvar_line_);

    FileResult close_file_ = platform_filesystem_close_file(&cvar_file_);
    if (close_file_ != FILE_SUCCESS) return CVAR_ERROR_FILE_CLOSE_FAILED;
    return CVAR_SUCCESS;
}

CVarResult cvar_save_to_file(ConstStr filepath) {
    if (!filepath || *filepath == '\0') return CVAR_ERROR_INVALID_PARAM;

    ConstStr temp_filename_ = "temp_cvar_file.cfg";

    // open original file for reading
    File       cvar_file_ = {0};
    FileResult open_file_ = platform_filesystem_open_file(&cvar_file_, filepath, FILE_IO_MODE_READ, FILE_MODE_TEXT);
    if (open_file_ != FILE_SUCCESS) return CVAR_ERROR_FILE_OPEN_FAILED;

    // open a temp file for writing
    File       temp_file_      = {0};
    FileResult open_temp_file_ = platform_filesystem_open_file(&temp_file_, temp_filename_, FILE_IO_MODE_WRITE, FILE_MODE_TEXT);
    if (open_temp_file_ != FILE_SUCCESS) {
        platform_filesystem_close_file(&cvar_file_);
        return CVAR_ERROR_FILE_OPEN_FAILED;
    }

    Str cvar_line_ = malloc(LINE_BUFFER_MAX_SIZE);
    if (!cvar_line_) {
        if (platform_filesystem_close_file(&temp_file_) != FILE_SUCCESS) return CVAR_ERROR_FILE_CLOSE_FAILED;
        if (platform_filesystem_close_file(&cvar_file_) != FILE_SUCCESS) return CVAR_ERROR_FILE_CLOSE_FAILED;
        return CVAR_ERROR_MEMORY_ALLOCATION;
    }

    while (platform_filesystem_read_line(&cvar_file_, NULL, &cvar_line_) == FILE_SUCCESS) {
        Str trimmed_ = _cvar_trim_whitespace(cvar_line_);

        // handle comments and empty lines
        if (*trimmed_ == '#' || *trimmed_ == '\0') {
            fputs(cvar_line_, temp_file_._stream);
            if (*trimmed_ == '#') fputc('\n', temp_file_._stream);
            continue;
        }

        Str pequal_ = strchr(trimmed_, '=');
        if (pequal_) {
            *pequal_ = '\0';
            Str key_ = trimmed_;

            UInt32 index_ = _cvar_hash(key_);
            CVar  *cvar_  = &cvar_table[index_];

            if (cvar_->_name && cvar_->_persistent) {
                fprintf(temp_file_._stream, "%s=", cvar_->_name);

                switch (cvar_->_type) {
                    case CVAR_TYPE_INT:
                        fprintf(temp_file_._stream, "%d\n", cvar_->_value._int);
                        break;

                    case CVAR_TYPE_FLOAT:
                        fprintf(temp_file_._stream, "%f\n", cvar_->_value._flt);
                        break;

                    case CVAR_TYPE_BOOL:
                        fprintf(temp_file_._stream, "%s\n", cvar_->_value._bool ? "true" : "false");
                        break;

                    case CVAR_TYPE_STRING:
                        fprintf(temp_file_._stream, "%s\n", cvar_->_value._str);
                        break;

                    default:
                        free(cvar_line_);
                        if (platform_filesystem_close_file(&temp_file_) != FILE_SUCCESS) return CVAR_ERROR_FILE_CLOSE_FAILED;
                        if (platform_filesystem_close_file(&cvar_file_) != FILE_SUCCESS) return CVAR_ERROR_FILE_CLOSE_FAILED;
                        return CVAR_ERROR_INVALID_DATA_TYPE;
                }

                // skip writing old cvar value
                continue;
            }
        }

        // write the unchanged cvar line
        fputs(cvar_line_, temp_file_._stream);
    }

    free(cvar_line_);

    if (platform_filesystem_close_file(&temp_file_) != FILE_SUCCESS) return CVAR_ERROR_FILE_CLOSE_FAILED;
    if (platform_filesystem_close_file(&cvar_file_) != FILE_SUCCESS) return CVAR_ERROR_FILE_CLOSE_FAILED;

    // Replace the old file with the modified one
    if ((remove(filepath) != 0) || (rename(temp_filename_, filepath) != 0))
        return CVAR_ERROR_FILE_WRITE_FAILED;

    return CVAR_SUCCESS;
}
