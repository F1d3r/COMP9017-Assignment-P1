#ifndef WAV_H
#define WAV_H

#include "struct.h"

// Given an unsigned integer with big endian, convert it to little endian form.
uint16_t convert_to_little_uint16(uint16_t big);


int16_t convert_to_little_int16(int16_t big);


uint32_t convert_to_little_uint32(uint32_t big);


void handle_endian(WAVHeader *header);


void print_header(WAVHeader header);


// Load a WAV file into buffer
int wav_load(const char* filename, int16_t* dest);


// Create/write a WAV file from buffer
void wav_save(const char* fname, int16_t* src, size_t len);

#endif