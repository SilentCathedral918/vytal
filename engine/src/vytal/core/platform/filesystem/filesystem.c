#include "filesystem.h"

#include <string.h>

VYTAL_INLINE ConstStr _platform_filesystem_lookup_file_mode(const FileIOMode io_mode,
                                                            const FileMode   file_mode) {
    switch (io_mode) {
        case FILE_IO_MODE_READ_WRITE:
            return (file_mode == FILE_MODE_BINARY) ? "rb+" : "r+";

        case FILE_IO_MODE_READ:
            return (file_mode == FILE_MODE_BINARY) ? "rb" : "r";

        case FILE_IO_MODE_WRITE:
            return (file_mode == FILE_MODE_BINARY) ? "wb" : "w";

        case FILE_IO_MODE_APPEND:
            return (file_mode == FILE_MODE_BINARY) ? "a+b" : "a";

        default:
            return NULL;
    }
}

FileResult platform_filesystem_open_file(
    File            *file,
    ConstStr         filepath,
    const FileIOMode io_mode,
    const FileMode   file_mode) {
    if (!file || !filepath) return FILE_ERROR_INVALID_PARAM;
    if (file->_active) return FILE_ERROR_ALREADY_OPEN;

    // configure stream file mode
    ConstStr file_mode_ = _platform_filesystem_lookup_file_mode(io_mode, file_mode);

    // open the file
    file->_stream = fopen(filepath, file_mode_);
    if (!file->_stream)
        return FILE_ERROR_OPEN_FAILED;

    file->_active = true;
    return FILE_SUCCESS;
}

FileResult platform_filesystem_close_file(File *file) {
    if (!file) return FILE_ERROR_INVALID_PARAM;
    if (!file->_active || !file->_stream) return FILE_ERROR_NOT_OPEN;

    if (fclose(file->_stream) != 0)
        return FILE_ERROR_CLOSE_FAILED;

    memset(file, 0, sizeof(File));
    return FILE_SUCCESS;
}

Bool platform_filesystem_file_exists(const char *filepath) {
    if (!filepath) return false;

    Bool  file_exists_ = false;
    FILE *stream_;

    if ((stream_ = fopen(filepath, "r")) != NULL) {
        file_exists_ = true;
        fclose(stream_);
    }

    return file_exists_;
}

ByteSize platform_filesystem_file_size(File *file) {
    if (!file) return 0;
    if (!file->_active || !file->_stream) return 0;

    ByteSize file_size_ = 0;
    FILE    *stream_    = file->_stream;

    // measure the size of the file with cursor positions
    {
        fseek(stream_, 0, SEEK_END);
        file_size_ = ftell(stream_);
        fseek(stream_, 0, SEEK_SET);
    }

    return file_size_;
}

FileResult platform_filesystem_read_line(
    File     *file,
    ByteSize *out_read_size,
    Str      *out_read_data) {
    if (!file) return FILE_ERROR_INVALID_PARAM;
    if (!file->_active || !file->_stream) return FILE_ERROR_NOT_OPEN;

    if (out_read_data) {
        if (fgets(*out_read_data, LINE_BUFFER_MAX_SIZE, file->_stream) != NULL) {
            if (out_read_size)
                *out_read_size = strlen(*out_read_data);

            return FILE_SUCCESS;
        }
    }

    return FILE_ERROR_READ_FAILED;
}

FileResult platform_filesystem_read_data(
    File          *file,
    const ByteSize data_size,
    VoidPtr        out_read_data) {
    if (!file) return FILE_ERROR_INVALID_PARAM;
    if (!file->_active || !file->_stream) return FILE_ERROR_NOT_OPEN;
    if (!data_size || !out_read_data) return FILE_ERROR_INVALID_PARAM;

    ByteSize file_size_ = platform_filesystem_file_size(file);
    if (!file_size_) return FILE_ERROR_READ_FAILED;

    ByteSize read_size_ = fread(out_read_data, 1, data_size, file->_stream);
    return (read_size_ == data_size) ? FILE_SUCCESS : FILE_ERROR_READ_FAILED;

    return FILE_ERROR_READ_FAILED;
}

FileResult platform_filesystem_read_all(File *file, ByteSize *out_read_size, Str out_read_data) {
    if (!file) return FILE_ERROR_INVALID_PARAM;
    if (!file->_active || !file->_stream) return FILE_ERROR_NOT_OPEN;
    if (!out_read_data) return FILE_ERROR_INVALID_PARAM;

    ByteSize file_size_ = platform_filesystem_file_size(file);
    if (!file_size_) return FILE_ERROR_READ_FAILED;

    ByteSize read_size_ = fread(out_read_data, 1, file_size_, file->_stream);

    if (out_read_size)
        *out_read_size = read_size_;

    if (read_size_ < file_size_)
        out_read_data[read_size_] = '\0';

    return (read_size_ > 0) ? FILE_SUCCESS : FILE_ERROR_READ_FAILED;
}

FileResult platform_filesystem_write_line(File *file, ConstStr content) {
    if (!file) return FILE_ERROR_INVALID_PARAM;
    if (!file->_active || !file->_stream) return FILE_ERROR_NOT_OPEN;
    if (!content) return FILE_ERROR_INVALID_PARAM;

    FILE *stream_ = file->_stream;

    Int32 write_result_ = fprintf(stream_, "%s", content);
    if (write_result_ != EOF)
        write_result_ = fputc('\n', stream_);

    fflush(stream_);
    return FILE_SUCCESS;
}

FileResult platform_filesystem_write_data(File *file, VoidPtr data, const ByteSize data_size) {
    if (!file) return FILE_ERROR_INVALID_PARAM;
    if (!file->_active || !file->_stream) return FILE_ERROR_NOT_OPEN;
    if (!data || !data_size) return FILE_ERROR_INVALID_PARAM;

    FILE *stream_ = file->_stream;
    fwrite(data, data_size, 1, stream_);

    fflush(stream_);
    return FILE_SUCCESS;
}

FileResult platform_filesystem_extract_filename_from_filepath(ConstStr filepath, Str *out_filename) {
    if (!filepath || !out_filename) return FILE_ERROR_INVALID_PARAM;
    if (!*filepath || !*out_filename) return FILE_ERROR_INVALID_PARAM;

    // remove directory separators

    ConstStr backslash_ = strrchr(filepath, '\\');
    ConstStr slash_     = strrchr(filepath, '/');

    // get pointer to last separator found
    ConstStr filename_ = (backslash_ > slash_) ? backslash_ : slash_;

    // if found one, move past separator
    // if not, use full filepath
    filename_ = filename_ ? filename_ + 1 : filepath;

    // remove extension
    {
        ConstStr ext_         = strrchr(filename_, '.');
        ByteSize length_      = ext_ ? (ByteSize)(ext_ - filename_) : strlen(filename_);
        ByteSize buffer_size_ = strlen(*out_filename);

        if (length_ >= buffer_size_) return FILE_ERROR_INSUFFICIENT_BUFFER;
        snprintf(*out_filename, buffer_size_, "%.*s", (Int32)length_, filename_);
    }

    return FILE_SUCCESS;
}

FileResult platform_filesystem_seek_to_position(File *file, const Int64 target) {
    if (!file) return FILE_ERROR_INVALID_PARAM;
    if (!file->_active || !file->_stream) return FILE_ERROR_NOT_OPEN;
    if (target < 0) return FILE_ERROR_INVALID_PARAM;

    if (fseek(file->_stream, target, SEEK_SET) != 0) return FILE_ERROR_IO;
    return FILE_SUCCESS;
}

FileResult platform_filesystem_seek_from_current(File *file, const Int64 distance) {
    if (!file) return FILE_ERROR_INVALID_PARAM;
    if (!file->_active || !file->_stream) return FILE_ERROR_NOT_OPEN;
    if (distance == 0) return FILE_ERROR_INVALID_PARAM;

    Int64 position_ = ftell(file->_stream);

    // prevent seeking beyond file boundaries
    {
        if (position_ == -1) return FILE_ERROR_IO;

        ByteSize file_size_ = platform_filesystem_file_size(file);
        if (position_ + distance > file_size_) return FILE_ERROR_EOF;
    }

    if (fseek(file->_stream, position_ + distance, SEEK_SET) != 0)
        return FILE_ERROR_IO;

    return FILE_SUCCESS;
}

Int64 platform_filesystem_get_seek_position(File *file) {
    if (!file) return FILE_ERROR_INVALID_PARAM;
    if (!file->_active || !file->_stream) return FILE_ERROR_NOT_OPEN;

    return ftell(file->_stream);
}
