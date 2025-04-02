#include <stdio.h>

#include "../include/child_parent.h"
#include "../include/helper.h"
#include "../include/list_op.h"
#include "../include/struct.h"
#include "../include/track.h"
#include "../include/wav.h"

void main(){ 
    // Test helper functions.
    // Test endian convertion.
    uint16_t input1 = 0x1234;
    printf("%04x\n\n", convert_to_little_uint16(input1));
    input1 = 0xaabb;
    printf("%04x\n\n", convert_to_little_uint16(input1));

    int16_t input2 = 0x1234;
    printf("%04x\n\n", (uint16_t)convert_to_little_int16(input2));
    input2 = 0xaabb;
    printf("%04x\n\n", (uint16_t)convert_to_little_int16(input2));

    uint32_t input3 = 0x12345678;
    printf("%08x\n\n", convert_to_little_uint32(input3));
    input3 = 0xaabbccdd;
    printf("%08x\n\n", convert_to_little_uint32(input3));


    // Test print header.
    int len = 10;
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
    print_header(myHeader);

    
    // Test handle_endian.
    handle_endian(&myHeader);
    print_header(myHeader);


    // Test tr_load and tr_save.
    int16_t buff1[10] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
    int16_t buff2[10];
    wav_save("../wav/test.wav", buff1, 10);
    wav_load("../wav/test.wav", buff2);

    // Print the buff2 to see if all content are saved and load.
    for(int i = 0; i < 10; i++){
        printf("%d\n", buff2[i]);
    }

    return;
}