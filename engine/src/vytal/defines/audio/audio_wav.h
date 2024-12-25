#pragma once

#include "defines/core/types.h"

// ---------------------------- chunks ---------------------------- //

#pragma pack(push, 1) // prevent compiler inserting paddings
typedef struct Audio_Wav_Riff_Chunk {
    Char   _riff[4];
    UInt32 _file_size;
    Char   _wave[4];
} AudioWavRiffChunk;

typedef struct Audio_Wav_Fmt_Chunk {
    Char   _id[4];
    UInt32 _size;
    UInt16 _format;
    UInt16 _channels;
    UInt32 _sample_rate;
    UInt32 _byte_rate;
    UInt16 _block_align;
    UInt16 _bits_per_sample;
} AudioWavFmtChunk;

typedef struct Audio_Wav_Data_Chunk {
    Char   _id[4];
    UInt32 _size;
    UInt8 *_pcm_data;
} AudioWavDataChunk;
#pragma pack(pop)

// ---------------------------- file ---------------------------- //

typedef struct Audio_Wav_File {
    AudioWavRiffChunk _riff;
    AudioWavFmtChunk  _fmt;
    AudioWavDataChunk _data;
} AudioWavFile;
