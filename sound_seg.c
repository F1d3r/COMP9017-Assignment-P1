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

// Load a WAV file into buffer
int wav_load(const char* filename, int16_t** dest){

    // Read the binary information from the file.
    FILE *myWAV = fopen(filename, "rb");
    // Process the exception
    if(!myWAV){
        perror("Open file failed.\n");
        return 0;
    }

    // Pass the header.
    fseek(myWAV, WAV_HEADER_OFFSET-4, SEEK_SET);
    // 4 Bytes unsined integer indicates the size of data.
    uint32_t dataSize = 0;
    fread(&dataSize, sizeof(uint32_t), 1, myWAV);
    printf("The size of the data: %d\n", dataSize);
    *dest = malloc(dataSize);

    for(int i = dataSize*2-1; i >= 0; i--){
        fread((*dest)+i, sizeof(int16_t), 1, myWAV);
    }
    // fread(*dest, sizeof(int16_t), dataSize*2, myWAV);


    fclose(myWAV);

    return 1;
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

// Create/write a WAV file from buffer
void wav_save(const char* fname, int16_t* src, size_t len){
    // Define a WAV header.
    WAVHeader myHeader;
    myHeader.chunkID = 0x52494646;
    myHeader.chunkSize = convert_to_little_uint32(36 + len);
    myHeader.format = 0x57415645;
    myHeader.subChunk1ID = 0x666d7420;
    myHeader.subChunk1Size = convert_to_little_uint32(16);
    myHeader.audioFormat = convert_to_little_uint16(1);
    myHeader.numChannels = convert_to_little_uint16(1);
    myHeader.sampleRate = convert_to_little_uint32(8000);
    myHeader.byteRate = convert_to_little_uint32(8000 * 1 * 16 / 8);
    myHeader.blockAlign = convert_to_little_uint16(1 * 16 / 8);
    myHeader.bitsPerSample = convert_to_little_uint16(16);
    myHeader.subChunk2ID = 0x64617461;
    myHeader.subChunk2Size = convert_to_little_uint32(len);

    // Define the file.
    FILE *myWAV = fopen(fname, "wb");
    // Write the header into the file, with the size of the header, once, by bit.
    fwrite(&myHeader, sizeof(WAVHeader), 1, myWAV);

    // Then, write the data into the file.
    // There are |len| uint16_t to write.
    for(int i = 0; i < len; i++){
        
    }


    fclose(myWAV);



    return;
}

// Initialize a new sound_seg object
struct sound_seg* tr_init() {
    return NULL;
}

// Destroy a sound_seg object and free all allocated memory
void tr_destroy(struct sound_seg* obj) {
    return;
}

// Return the length of the segment
size_t tr_length(struct sound_seg* seg) {
    return (size_t)-1;
}

// Read len elements from position pos into dest
void tr_read(struct sound_seg* track, int16_t* dest, size_t pos, size_t len) {
    return;
}

// Write len elements from src into position pos
void tr_write(struct sound_seg* track, int16_t* src, size_t pos, size_t len) {
    return;
}

// Delete a range of elements from the track
bool tr_delete_range(struct sound_seg* track, size_t pos, size_t len) {
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

void main(){

    int16_t *dest;

    if(wav_load("./Good Morning.wav", &dest) == 1){
        printf("Complete load.\n");
    }


    if(dest != NULL){
        free(dest);
    }
    


}


// void main(){
    
//     // char* fname = "test.wav";
//     // uint16_t = 

//     // wav_save(const char* fname, int16_t* src, size_t len)

// }
