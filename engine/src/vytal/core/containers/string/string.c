#include "string.h"

#include "vytal/core/hal/memory/vtmem.h"
#include "vytal/core/misc/console/console.h"
#include "vytal/core/misc/string/vtstr.h"
#include "vytal/managers/memory/memmgr.h"

#include <ctype.h>
#include <stdarg.h>

#define CHAR_ARRAY_MAX_LENGTH 16384 // 16KB

typedef struct Container_String_Data {
    ByteSize _length;
    ByteSize _capacity;
    Str      _str;
} StringData;

VT_INLINE StringData *_container_string_get_internal_data(String str) {
    return (!str || !(str->_internal_data)) ? NULL : (str->_internal_data);
}

String container_string_construct(ConstStr input) {
    ByteSize input_length_ = misc_str_strlen(input);

    // size set to 0 since 'containers' use pool allocator, where size is already determined
    VoidPtr chunk_ = memory_manager_allocate(0, MEMORY_TAG_CONTAINERS);
    if (!chunk_)
        return NULL;

    // there would be three elements...

    // element 1: the string container self
    String string_ = VT_CAST(String, chunk_);

    // element 2: the string internal data struct
    StringData *data_  = VT_CAST(StringData *, string_ + 1);
    data_->_length     = input_length_;
    ByteSize capacity_ = 1;
    for (; capacity_ < input_length_; capacity_ *= 2) {
    }
    data_->_capacity = capacity_;

    // element 3: the raw string data
    data_->_str = VT_CAST(Str, data_ + 1);
    strcpy(data_->_str, input);

    // assign internal data to string ownership
    string_->_internal_data = data_;

    return string_;
}

Bool container_string_destruct(String str) {
    if (!str || !(str->_internal_data))
        return false;

    // free the entire string chunk
    {
        memory_manager_deallocate(str, MEMORY_TAG_CONTAINERS);
        str = NULL;
    }

    return true;
}

Str      container_string_get(String str) { return (_container_string_get_internal_data(str)->_str); }
ByteSize container_string_length(String str) { return (_container_string_get_internal_data(str)->_length); }
ByteSize container_string_capacity(String str) { return (_container_string_get_internal_data(str)->_capacity); }
