#pragma once

#include "vytal/defines/core/types.h"

typedef enum Asset_Image_Format {
    ASSET_IMAGE_TYPE_JPEG,
    ASSET_IMAGE_TYPE_PNG,
} ImageFormat;

typedef struct Asset_Image {
    UInt64      _id;
    ConstStr    _filepath;
    VoidPtr     _size;
    ByteSize    _data_size;
    ImageFormat _format;
} Image;
