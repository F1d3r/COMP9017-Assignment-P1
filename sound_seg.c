#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>

#define BUFFER_LEN 128
#define WAV_HEADER_OFFSET 44


// This represents the track.
struct sound_seg {
    //TODO
    size_t trackLen;
    int16_t *data;
};

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


void print_header(WAVHeader header){
    printf("Chunk ID: ");
    printf("%c", (char)((header.chunkID & 0xFF000000)>>24));
    printf("%c", (char)((header.chunkID & 0x00FF0000)>>16));
    printf("%c", (char)((header.chunkID & 0x0000FF00)>>8));
    printf("%c\n", (char)((header.chunkID & 0x000000FF)));

    printf("Chunk Size: %d.\n", 36+header.subChunk2Size);

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
        printf("PCM.\n");
    }else{
        printf("Other.\n");
    }

    printf("Number of channels: ");
    if(header.numChannels == 1){
        printf("Mono.\n");
    }else if(header.numChannels == 2){
        printf("Stereo.\n");
    }else{
        printf("Other.\n");
    }

    printf("Sample rate: %d.\n", header.sampleRate);

    printf("Byte rate: %d.\n", header.byteRate);

    printf("Block align: %d.\n", header.blockAlign);

    printf("Bits per sample: %d.\n", header.bitsPerSample);

    printf("Subchunk 2 ID: ");
    printf("%c", (char)((header.subChunk2ID & 0xFF000000)>>24));
    printf("%c", (char)((header.subChunk2ID & 0x00FF0000)>>16));
    printf("%c", (char)((header.subChunk2ID & 0x0000FF00)>>8));
    printf("%c\n", (char)((header.subChunk2ID & 0x000000FF)));

    printf("Subchunk 2 Size: %d.\n\n", header.subChunk2Size);
}


// Given an unsigned integer with big endian, convert it to little endian form.
uint16_t convert_to_little_uint16(uint16_t big){
    // For 0xABCD, OR(0xCD00, 0x00AB)
    return ((big >> 8) | (big << 8));
}


int16_t convert_to_little_int16(int16_t big){
    // For 0xABCD, OR(0xCD00, 0x00AB)
    return ((big >> 8) | (big << 8));
}


uint32_t convert_to_little_uint32(uint32_t big){
    // For 0xABCDEFGH, OR(0xGH000000, 0x00EF0000, 0x0000CD00, 0x000000AB)
    return ((big & (0x000000FF)) << 24 | (big & (0x0000FF00)) << 8 | (big & (0x00FF0000)) >> 8 | (big & (0xFF000000)) >> 24);
}


void handle_endian(WAVHeader *header){
    header->chunkID = convert_to_little_uint32(header->chunkID);
    header->format = convert_to_little_uint32(header->format);
    header->subChunk1ID = convert_to_little_uint32(header->subChunk1ID);
    header->subChunk2ID = convert_to_little_uint32(header->subChunk2ID);
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
    print_header(header);

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
    // Define a WAV header.
    // WAVHeader myHeader;
    // myHeader.chunkID = 0x52494646;
    // myHeader.chunkSize = convert_to_little_uint32(36 + len);
    // myHeader.format = 0x57415645;
    // myHeader.subChunk1ID = 0x666d7420;
    // myHeader.subChunk1Size = convert_to_little_uint32(16);
    // myHeader.audioFormat = convert_to_little_uint16(1);
    // myHeader.numChannels = convert_to_little_uint16(1);
    // myHeader.sampleRate = convert_to_little_uint32(8000);
    // myHeader.byteRate = convert_to_little_uint32(8000 * 1 * 16 / 8);
    // myHeader.blockAlign = convert_to_little_uint16(1 * 16 / 8);
    // myHeader.bitsPerSample = convert_to_little_uint16(16);
    // myHeader.subChunk2ID = 0x64617461;
    // myHeader.subChunk2Size = convert_to_little_uint32(len);
    
    WAVHeader myHeader;
    myHeader.chunkID = 0x52494646;
    myHeader.chunkSize = 36 + len;
    myHeader.format = 0x57415645;
    myHeader.subChunk1ID = 0x666d7420;
    myHeader.subChunk1Size = 16;
    myHeader.audioFormat = 1;
    myHeader.numChannels = 1;
    myHeader.sampleRate = 8000;
    myHeader.byteRate = 8000 * 1 * 16 / 8;
    myHeader.blockAlign = 1 * 16 / 8;
    myHeader.bitsPerSample = 16;
    myHeader.subChunk2ID = 0x64617461;
    myHeader.subChunk2Size = len * 1 * 16 / 8;

    handle_endian(&myHeader);
    print_header(myHeader);

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


// Initialize a new sound_seg object
struct sound_seg* tr_init() {
    struct sound_seg *myTrack;
    myTrack->trackLen = 0;
    myTrack->data = NULL;

    return myTrack;
}


// Destroy a sound_seg object and free all allocated memory
void tr_destroy(struct sound_seg* obj) {

    if(obj->data != NULL){
        free(obj->data);
    }

    return;
}


// Return the length of the segment
size_t tr_length(struct sound_seg* seg) {
    return seg->trackLen;
}


// Read len elements from position pos into dest
void tr_read(struct sound_seg* track, int16_t* dest, size_t pos, size_t len) {
    int16_t *dataPos = track->data;
    for(int i = 0; i < pos; i++){
        dataPos ++;
    }
    memcpy(dest, dataPos, len);

    return;
}


// Write len elements from src into position pos
void tr_write(struct sound_seg* track, int16_t* src, size_t pos, size_t len) {
    printf("Old data position: %p\n", track->data);
    // Reallocate memory for the data.
    int16_t *temp = realloc(track->data, (pos+len)*2);
    // Check if the reallocation success.
    if(temp == NULL){
        printf("The reallocation failed.\n");
        return;
    }else{
        // Update the data pointer if success.
        printf("The reallocation success.\n");
        track->data = temp;
    }
    // Update the new track lenght.
    track->trackLen = pos+len;

    printf("Data location: %p\n", track->data);

    int16_t *posToWrite = track->data;
    for(int i = 0; i < pos; i++){
        posToWrite++;
    }
    
    printf("Position to write: %p\n", posToWrite);
    memcpy(posToWrite, src, len*2);

    return;
}


// Delete a range of elements from the track
bool tr_delete_range(struct sound_seg* track, size_t pos, size_t len) {
    int16_t *newData = realloc(track->data, track->trackLen-len);
    if(newData == NULL){
        printf("Reallocation failed.\n");
        return false;
    }
    // Update the new length.
    track->trackLen -= len;
    // Copy the old value
    memcpy(newData, track->data, pos);
    memcpy(newData+pos, newData+pos+len, track->trackLen-pos);

    // Free old data.
    free(track->data);
    // Update data pointer.
    track->data = newData;

    return true;
}


// Returns a string containing <start>,<end> ad pairs in target
char* tr_identify(struct sound_seg* target, struct sound_seg* ad){
    return NULL;
}


// Insert a portion of src_track into dest_track at position destpos
void tr_insert(struct sound_seg* src_track,
            struct sound_seg* dest_track,
            size_t destpos, size_t srcpos, size_t len) {
    return;
}



// void main(){

//     int16_t buff[BUFFER_LEN];

//     wav_load("./test.wav", buff);

//     for(int i = 0; i < sizeof(buff)/sizeof(int16_t); i++){
//         if(buff[i] != 0){
//             printf("%d ", buff[i]);
//         }
//     }
//     printf("\n");

//     struct sound_seg *myTrack = tr_init();
//     printf("Position of my track: %p\n", myTrack);

//     tr_write(myTrack, buff, 0, 10);
//     for(int i = 0; i < myTrack->trackLen; i++){
//         printf("Data %d: %d\n", i, myTrack->data[i]);
//     }

//     tr_destroy(myTrack);




//     // Dynamical version.
//     // if(wav_load("./input.wav", dest) == 1){
//     //     printf("Complete load.\n");
//     // }else{
//     //     printf("Failed.\n");
//     //     if(dest != NULL){
//     //         free(dest);
//     //     }
//     //     return;
//     // }

//     // wav_save("./output.wav", dest, 47616/2);

//     // if(dest != NULL){
//     //     free(dest);
//     // }
    


// }