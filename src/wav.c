#include <stdio.h>

#include "../include/wav.h"

#define BITS_PER_BYTE 8
// From spec of WAV file.
#define CHUNK_SIZE1_OFFSET 36
#define CHUNK_ID 0x52494646
#define FORMAT 0x57415645
#define SUB_CHUNK1_ID 0x666d7420
#define SUB_CHUNK1_SIZE 16
#define AUDIO_FORMAT 1
#define NUM_CHANNEL 1
#define SAMPLE_RATE 8000
#define BITS_PER_SAMPLE 16
#define SUB_CHUNK2_ID 0x64617461

// Given an unsigned integer with big endian, convert it to little endian form.
uint16_t convert_to_little_uint16(uint16_t big){
    // For 0xABCD, OR(0xCD00, 0x00AB)
    return ((big >> BITS_PER_BYTE) | (big << BITS_PER_BYTE));
}


int16_t convert_to_little_int16(int16_t big){
    // For 0xABCD, OR(0xCD00, 0x00AB)
    return (((uint16_t)big >> BITS_PER_BYTE) | ((uint16_t)big << BITS_PER_BYTE));
}


uint32_t convert_to_little_uint32(uint32_t big){
    // For 0xABCDEFGH, OR(0xGH000000, 0x00EF0000, 0x0000CD00, 0x000000AB)
    // FFs are masks.
    return ((big & (0x000000FF)) << BITS_PER_BYTE*3 | (big & (0x0000FF00)) << BITS_PER_BYTE 
    | (big & (0x00FF0000)) >> BITS_PER_BYTE | (big & (0xFF000000)) >> BITS_PER_BYTE*3);
}


void handle_endian(WAVHeader *header){
    header->chunkID = convert_to_little_uint32(header->chunkID);
    header->format = convert_to_little_uint32(header->format);
    header->subChunk1ID = convert_to_little_uint32(header->subChunk1ID);
    header->subChunk2ID = convert_to_little_uint32(header->subChunk2ID);
}


void print_header(WAVHeader header){
    printf("Chunk ID: ");
    printf("%c", (char)((header.chunkID & 0xFF000000)>>24));
    printf("%c", (char)((header.chunkID & 0x00FF0000)>>16));
    printf("%c", (char)((header.chunkID & 0x0000FF00)>>8));
    printf("%c\n", (char)((header.chunkID & 0x000000FF)));

    printf("Chunk Size: %d\n", header.subChunk2Size+CHUNK_SIZE1_OFFSET);

    printf("Format: ");
    printf("%c", (char)((header.format & 0xFF000000)>>24));
    printf("%c", (char)((header.format & 0x00FF0000)>>16));
    printf("%c", (char)((header.format & 0x0000FF00)>>8));
    printf("%c\n", (char)((header.format & 0x000000FF)));

    printf("Subchunk 1 ID: ");
    printf("%c", (char)((header.subChunk1ID & 0xFF000000)>>24));
    printf("%c", (char)((header.subChunk1ID & 0x00FF0000)>>16));
    printf("%c", (char)((header.subChunk1ID & 0x0000FF00)>>8));
    printf("%c\n", (char)((header.subChunk1ID & 0x000000FF)));

    printf("Audio format: ");
    if(header.audioFormat == 1){
        printf("PCM\n");
    }else{
        printf("Other\n");
    }

    printf("Number of channels: ");
    if(header.numChannels == 1){
        printf("Mono\n");
    }else if(header.numChannels == 2){
        printf("Stereo\n");
    }else{
        printf("Other\n");
    }

    printf("Sample rate: %d\n", header.sampleRate);

    printf("Byte rate: %d\n", header.byteRate);

    printf("Block align: %d\n", header.blockAlign);

    printf("Bits per sample: %d\n", header.bitsPerSample);

    printf("Subchunk 2 ID: ");
    printf("%c", (char)((header.subChunk2ID & 0xFF000000)>>BITS_PER_BYTE*3));
    printf("%c", (char)((header.subChunk2ID & 0x00FF0000)>>BITS_PER_BYTE*2));
    printf("%c", (char)((header.subChunk2ID & 0x0000FF00)>>BITS_PER_BYTE));
    printf("%c\n", (char)((header.subChunk2ID & 0x000000FF)));

    printf("Subchunk 2 Size: %d\n\n", header.subChunk2Size);
}


// Load a WAV file into buffer
int wav_load(const char* filename, int16_t* dest){

    // Read the binary information from the file.
    FILE *myWAV = fopen(filename, "rb");
    // Process the exception
    if(!myWAV){
        perror("Open file failed.\n");
        return 0;
    }

    // Read the header.
    WAVHeader header;
    fread(&header, sizeof(WAVHeader), 1, myWAV);
    handle_endian(&header);
    // print_header(header);

    // Fixed size buffer version.
    fread(dest, sizeof(int16_t), header.subChunk2Size/2, myWAV);

    // // Dynamically allocate memory for the buffer.
    // // Allocate space for the dest.
    // *dest = malloc(header.subChunk2Size);

    // for(int i = 0; i < header.subChunk2Size/2; i++){
    //     fread((*dest)+i, sizeof(int16_t), 1, myWAV);
    // }

    fclose(myWAV);

    return 1;
}


// Create/write a WAV file from buffer
void wav_save(const char* fname, int16_t* src, size_t len){
    WAVHeader myHeader;
    myHeader.chunkID = CHUNK_ID;
    myHeader.chunkSize = CHUNK_SIZE1_OFFSET + len;
    myHeader.format = FORMAT;
    myHeader.subChunk1ID = SUB_CHUNK1_ID;
    myHeader.subChunk1Size = SUB_CHUNK1_SIZE;
    myHeader.audioFormat = AUDIO_FORMAT;
    myHeader.numChannels = NUM_CHANNEL;
    myHeader.sampleRate = SAMPLE_RATE;
    myHeader.blockAlign = NUM_CHANNEL * BITS_PER_SAMPLE / BITS_PER_BYTE;
    myHeader.bitsPerSample = BITS_PER_SAMPLE;
    myHeader.byteRate = SAMPLE_RATE * NUM_CHANNEL * BITS_PER_SAMPLE / BITS_PER_BYTE;
    myHeader.subChunk2ID = SUB_CHUNK2_ID;
    myHeader.subChunk2Size = len * NUM_CHANNEL * SAMPLE_RATE / BITS_PER_BYTE;

    handle_endian(&myHeader);
    // print_header(myHeader);

    // Define the file.
    FILE *myWAV = fopen(fname, "wb");
    // Write the header into the file, with the size of the header, once, by bit.
    fwrite(&myHeader, sizeof(WAVHeader), 1, myWAV);

    // Then, write the data into the file.
    // There are |len| uint16_t to write.
    for(int i = 0; i < len; i++){
        fwrite((src++), sizeof(int16_t), 1, myWAV);
    }


    fclose(myWAV);

    return;
}