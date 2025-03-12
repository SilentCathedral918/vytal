#pragma once

#include "vytal/defines/core/filesystem.h"
#include "vytal/defines/shared.h"

VYTAL_API FileResult platform_filesystem_open_file(
    File            *file,
    ConstStr         filepath,
    const FileIOMode io_mode,
    const FileMode   file_mode);
VYTAL_API FileResult platform_filesystem_close_file(File *file);

VYTAL_API Bool     platform_filesystem_file_exists(ConstStr filepath);
VYTAL_API ByteSize platform_filesystem_file_size(File *file);

VYTAL_API FileResult platform_filesystem_read_line(
    File     *file,
    ByteSize *out_read_size,
    Str      *out_read_data);
VYTAL_API FileResult platform_filesystem_read_data(
    File          *file,
    const ByteSize data_size,
    VoidPtr        out_read_data);
VYTAL_API FileResult platform_filesystem_read_all(
    File     *file,
    ByteSize *out_read_size,
    Str       out_read_data);
VYTAL_API FileResult platform_filesystem_read_binary_uint32(
    File     *file,
    ByteSize *out_word_count,
    UInt32  **out_read_data);

VYTAL_API FileResult platform_filesystem_write_line(File *file, ConstStr content);
VYTAL_API FileResult platform_filesystem_write_data(File *file, VoidPtr data, const ByteSize data_size);

VYTAL_API FileResult platform_filesystem_extract_filename_from_filepath(ConstStr filepath, Str *out_filename);

VYTAL_API FileResult platform_filesystem_seek_to_position(File *file, const Int64 target);
VYTAL_API FileResult platform_filesystem_seek_from_current(File *file, const Int64 distance);
VYTAL_API Int64      platform_filesystem_get_seek_position(File *file);
