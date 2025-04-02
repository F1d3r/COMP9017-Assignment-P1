#ifndef STRUCT_H
#define STRUCT_H

#include <stdint.h>
#include <stdlib.h>

typedef struct sound_seg {
    struct sound_seg* parent;
    struct sound_seg** children;    // Children is a pointer to an set of pointers.
    uint16_t num_children;
    uint16_t trackLen;
    int16_t* data;
    struct sound_seg* next;
} Track;


typedef struct {
    uint32_t chunkID;
    uint32_t chunkSize;
    uint32_t format;
    uint32_t subChunk1ID;
    uint32_t subChunk1Size;
    uint16_t audioFormat;
    uint16_t numChannels;
    uint32_t sampleRate;
    uint32_t byteRate;
    uint16_t blockAlign;
    uint16_t bitsPerSample;
    uint32_t subChunk2ID;
    uint32_t subChunk2Size;
} WAVHeader;

#endif