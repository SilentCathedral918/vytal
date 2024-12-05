#include "vtmem.h"

void hal_mem_memswap(VoidPtr left, VoidPtr right, ByteSize size) {
    const ByteSize threshold = 65536; // 65536 bytes

    if (size <= threshold) {
        SizePtr pleft_    = left;
        SizePtr pright_   = right;
        SizePtr pleftend_ = VT_CAST(SizePtr, left) + (size / sizeof(ByteSize));

        // buffer chunks of ByteSize bytes
        while (pleft_ < pleftend_) {
            ByteSize temp_ = *pleft_;
            *pleft_        = *pright_;
            *pright_       = temp_;
            ++pleft_;
            ++pright_;
        }

        // remaining bytes
        UBytePtr pleftrem_  = VT_CAST(UBytePtr, pleft_);
        UBytePtr prightrem_ = VT_CAST(UBytePtr, pright_);
        for (ByteSize i = 0; i < size % sizeof(ByteSize); ++i) {
            UInt8 temp_ = *pleftrem_;
            *pleftrem_  = *prightrem_;
            *prightrem_ = temp_;

            ++pleftrem_;
            ++prightrem_;
        }
    } else {
        VoidPtr temp_ = hal_mem_malloc(size);

        hal_mem_memcpy(temp_, left, size);
        hal_mem_memcpy(left, right, size);
        hal_mem_memcpy(right, temp_, size);

        hal_mem_free(temp_);
    }
}
