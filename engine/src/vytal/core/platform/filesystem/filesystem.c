#include "filesystem.h"

#include <string.h>

VYTAL_INLINE ConstStr _platform_filesystem_lookup_file_mode(const FileIOMode io_mode,
                                                            const FileMode   file_mode) {
    switch (io_mode) {
        case FILE_IO_MODE_READ_WRITE:
            return (file_mode == FILE_MODE_BINARY) ? "r+b" : "r+";

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
    if (!file || !filepath)
        return FILE_ERROR_NULL_PTR;

    if (file->_active)
        return FILE_ERROR_ALREADY_OPEN;

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
    if (!file)
        return FILE_ERROR_NULL_PTR;

    if (!file->_active || !file->_stream)
        return FILE_ERROR_NOT_OPEN;

    if (fclose(file->_stream) != 0)
        return FILE_ERROR_CLOSE_FAILED;

    memset(file, 0, sizeof(File));
    return FILE_SUCCESS;
}
