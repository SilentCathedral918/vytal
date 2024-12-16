#pragma once

#include "types.h"

#include <stdio.h>

// -------------------------------- modes -------------------------------- //

typedef enum FileSystem_File_IO_Mode {
    FILE_IO_MODE_READ,
    FILE_IO_MODE_WRITE,
    FILE_IO_MODE_READ_WRITE,
    FILE_IO_MODE_APPEND
} FileIOMode;

typedef enum FileSystem_File_Mode { FILE_MODE_TEXT, FILE_MODE_BINARY } FileMode;

// -------------------------------- types -------------------------------- //

typedef FILE *FileStream;

typedef struct FileSystem_File_Handle {
    FileStream _stream;
    Bool       _active;
} FileHandle;