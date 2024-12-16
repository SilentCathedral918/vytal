#pragma once

#include "vytal/defines/core/filesystem.h"
#include "vytal/defines/core/mem.h"
#include "vytal/defines/shared.h"

VT_API Bool platform_fs_open_file(FileHandle *handle, ConstStr filepath, const FileIOMode io_mode, const FileMode file_mode);
VT_API Bool platform_fs_close_file(FileHandle *handle);
VT_API Bool platform_fs_file_exists(ConstStr filepath);
VT_API ByteSize platform_fs_file_size(FileHandle *handle);
VT_API Bool     platform_fs_file_read_line(FileHandle *handle, Str *line_buffer, ByteSize *line_buffer_length);
VT_API Bool     platform_fs_file_write_line(FileHandle *handle, ConstStr text);
VT_API Bool     platform_fs_file_read_all(FileHandle *handle, ByteSize *read_size, Str *read_out);
VT_API Bool platform_fs_file_read_data(FileHandle *handle, const ByteSize data_size, ByteSize *read_size, VoidPtr *read_out);
VT_API Bool platform_fs_file_write_data(FileHandle *handle, const VoidPtr data, const ByteSize data_size);
VT_API ConstStr platform_fs_get_filename_from_path(ConstStr filepath);
VT_API Bool     platform_fs_create_directory(ConstStr directory_name);
