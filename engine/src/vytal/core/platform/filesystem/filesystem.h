#pragma once

#include "vytal/defines/core/filesystem.h"
#include "vytal/defines/shared.h"

VYTAL_API FileResult platform_filesystem_open_file(
    File            *file,
    ConstStr         filepath,
    const FileIOMode io_mode,
    const FileMode   file_mode);

VYTAL_API FileResult platform_filesystem_close_file(File *file);