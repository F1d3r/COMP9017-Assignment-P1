#ifndef STRUCT_H
#define STRUCT_H

#include <stdint.h>
#include <stdlib.h>

typedef struct sound_seg {
    struct sound_seg* parent;
    struct sound_seg** children;    // Children is a pointer to an set of pointers.
    uint16_t num_children;
    uint16_t track_len;
    int16_t* data;
    struct sound_seg* next;
} Track;


typedef struct {
    uint32_t chunk_ID;
    uint32_t chunk_size;
    uint32_t format;
    uint32_t sub_chunk1_ID;
    uint32_t sub_chunk1_size;
    uint16_t audio_format;
    uint16_t num_channels;
    uint32_t sample_rate;
    uint32_t byte_rate;
    uint16_t block_align;
    uint16_t bits_per_sample;
    uint32_t sub_chunk2_ID;
    uint32_t sub_chunk2_size;
} WAVHeader;

#endif