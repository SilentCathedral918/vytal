#include "parse.h"

#include <ctype.h>
#include <stdio.h>
#include <string.h>

ParseResult parse_trim_whitespace(Str *str) {
    if (!str || !(*str)) return PARSE_ERROR_INVALID_PARAM;
    if (!*(*str)) return PARSE_ERROR_EMPTY_STRING;

    // trim leading space
    {
        while (isspace((Int8)(*(*str)))) ++(*str);
        if (**str == '\0') return PARSE_SUCCESS;
    }

    // trim trailing space
    {
        Str end_ = (*str) + strlen(*str) - 1;
        while (end_ > (*str) && isspace((Int8)(*end_))) --end_;
        *(end_ + 1) = '\0';
    }

    return PARSE_SUCCESS;
}

ParseResult parse_clean_filepath(Str *filepath) {
    if (!filepath || !(*filepath)) return PARSE_ERROR_INVALID_PARAM;
    if (!*(*filepath)) return PARSE_ERROR_EMPTY_STRING;

    ParseResult trim_ = parse_trim_whitespace(filepath);
    if (trim_ != PARSE_SUCCESS)
        return trim_;

    // remove surrounding quotes
    {
        if (*(*filepath) == '\"') ++(*filepath);
        ByteSize path_length_ = strlen(*filepath);
        if (path_length_ > 0 && (*filepath)[path_length_ - 1] == '\"')
            (*filepath)[path_length_ - 1] = '\0';
    }

    // convert backslashes to forward-slashes
    // path\\to\\file -> path/to/file
    {
        for (ByteSize i = 0; (*filepath)[i]; ++i)
            if ((*filepath)[i] == '\\') (*filepath)[i] = '/';
    }

    return PARSE_SUCCESS;
}

Bool parse_key_value(ConstStr line, Str key, Str value) {
    if (!line || !key || !value) return false;

    Str equal_ = strchr(line, '=');
    if (!equal_) return false;
    *equal_ = '\0';

    if (parse_trim_whitespace((Str *)&line) != PARSE_SUCCESS) return false;

    Str value_ = equal_ + 1;
    {
        // handle non-filepath value
        if (!strchr(value_, '\\')) {
            if (parse_trim_whitespace((Str *)&value_) != PARSE_SUCCESS)
                return false;

            // remove surrounding quotes
            {
                if (*value_ == '\"') ++value_;
                ByteSize value_length_ = strlen(value_);
                if (value_length_ > 0 && value_[value_length_ - 1] == '\"')
                    value_[value_length_ - 1] = '\0';
            }
        }

        // otherwise, handle filepath value
        else {
            if (parse_clean_filepath((Str *)&value_) != PARSE_SUCCESS)
                return false;
        }
    }

    strcpy(key, line);
    strcpy(value, value_);

    return true;
}

ByteSize parse_memory_size(Str value) {
    ByteSize value_   = 0;
    Char     unit_[3] = {0};

    // remove surrounding quotes
    {
        if (*value == '\"') ++value;
        ByteSize path_length_ = strlen(value);
        if (path_length_ > 0 && value[path_length_ - 1] == '\"')
            value[path_length_ - 1] = '\0';
    }

    if (sscanf(value, "%zu%2s", &value_, unit_) != 2) return 0;

    if (!strcmp(unit_, "KB")) return KB_IN_BYTES(value_);
    if (!strcmp(unit_, "MB")) return MB_IN_BYTES(value_);
    if (!strcmp(unit_, "GB")) return GB_IN_BYTES(value_);

    return value_;
}
