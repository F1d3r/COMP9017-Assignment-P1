#ifndef SOUND_SEG_H
#define SOUND_SEG_H

#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include <math.h>


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

void print_header(WAVHeader header);


// Given an unsigned integer with big endian, convert it to little endian form.
uint16_t convert_to_little_uint16(uint16_t big);


int16_t convert_to_little_int16(int16_t big);


uint32_t convert_to_little_uint32(uint32_t big);


void handle_endian(WAVHeader *header);


// Load a WAV file into buffer
int wav_load(const char* filename, int16_t* dest);


// Create/write a WAV file from buffer
void wav_save(const char* fname, int16_t* src, size_t len);


#endif