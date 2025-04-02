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
    header->chunk_ID = convert_to_little_uint32(header->chunk_ID);
    header->format = convert_to_little_uint32(header->format);
    header->sub_chunk1_ID = convert_to_little_uint32(header->sub_chunk1_ID);
    header->sub_chunk2_ID = convert_to_little_uint32(header->sub_chunk2_ID);
}


void print_header(WAVHeader header){
    printf("Chunk ID: ");
    printf("%c", (char)((header.chunk_ID & 0xFF000000)>>24));
    printf("%c", (char)((header.chunk_ID & 0x00FF0000)>>16));
    printf("%c", (char)((header.chunk_ID & 0x0000FF00)>>8));
    printf("%c\n", (char)((header.chunk_ID & 0x000000FF)));

    printf("Chunk Size: %d\n", header.sub_chunk2_size+CHUNK_SIZE1_OFFSET);

    printf("Format: ");
    printf("%c", (char)((header.format & 0xFF000000)>>24));
    printf("%c", (char)((header.format & 0x00FF0000)>>16));
    printf("%c", (char)((header.format & 0x0000FF00)>>8));
    printf("%c\n", (char)((header.format & 0x000000FF)));

    printf("Subchunk 1 ID: ");
    printf("%c", (char)((header.sub_chunk1_ID & 0xFF000000)>>24));
    printf("%c", (char)((header.sub_chunk1_ID & 0x00FF0000)>>16));
    printf("%c", (char)((header.sub_chunk1_ID & 0x0000FF00)>>8));
    printf("%c\n", (char)((header.sub_chunk1_ID & 0x000000FF)));

    printf("Audio format: ");
    if(header.audio_format == 1){
        printf("PCM\n");
    }else{
        printf("Other\n");
    }

    printf("Number of channels: ");
    if(header.num_channels == 1){
        printf("Mono\n");
    }else if(header.num_channels == 2){
        printf("Stereo\n");
    }else{
        printf("Other\n");
    }

    printf("Sample rate: %d\n", header.sample_rate);

    printf("Byte rate: %d\n", header.byte_rate);

    printf("Block align: %d\n", header.block_align);

    printf("Bits per sample: %d\n", header.bits_per_sample);

    printf("Subchunk 2 ID: ");
    printf("%c", (char)((header.sub_chunk2_ID & 0xFF000000)>>BITS_PER_BYTE*3));
    printf("%c", (char)((header.sub_chunk2_ID & 0x00FF0000)>>BITS_PER_BYTE*2));
    printf("%c", (char)((header.sub_chunk2_ID & 0x0000FF00)>>BITS_PER_BYTE));
    printf("%c\n", (char)((header.sub_chunk2_ID & 0x000000FF)));

    printf("Subchunk 2 Size: %d\n\n", header.sub_chunk2_size);
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
    fread(dest, sizeof(int16_t), header.sub_chunk2_size/2, myWAV);

    // // Dynamically allocate memory for the buffer.
    // // Allocate space for the dest.
    // *dest = malloc(header.sub_chunk2_size);

    // for(int i = 0; i < header.sub_chunk2_size/2; i++){
    //     fread((*dest)+i, sizeof(int16_t), 1, myWAV);
    // }

    fclose(myWAV);

    return 1;
}


// Create/write a WAV file from buffer
void wav_save(const char* fname, int16_t* src, size_t len){
    WAVHeader myHeader;
    myHeader.chunk_ID = CHUNK_ID;
    myHeader.chunk_size = CHUNK_SIZE1_OFFSET + len;
    myHeader.format = FORMAT;
    myHeader.sub_chunk1_ID = SUB_CHUNK1_ID;
    myHeader.sub_chunk1_size = SUB_CHUNK1_SIZE;
    myHeader.audio_format = AUDIO_FORMAT;
    myHeader.num_channels = NUM_CHANNEL;
    myHeader.sample_rate = SAMPLE_RATE;
    myHeader.block_align = NUM_CHANNEL * BITS_PER_SAMPLE / BITS_PER_BYTE;
    myHeader.bits_per_sample = BITS_PER_SAMPLE;
    myHeader.byte_rate = SAMPLE_RATE * NUM_CHANNEL * BITS_PER_SAMPLE / BITS_PER_BYTE;
    myHeader.sub_chunk2_ID = SUB_CHUNK2_ID;
    myHeader.sub_chunk2_size = len * NUM_CHANNEL * SAMPLE_RATE / BITS_PER_BYTE;

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