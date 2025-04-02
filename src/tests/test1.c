#include <stdio.h>

#include "../../include/child_parent.h"
#include "../../include/helper.h"
#include "../../include/list_op.h"
#include "../../include/struct.h"
#include "../../include/track.h"
#include "../../include/wav.h"

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
    myHeader.chunk_ID = 0x52494646;
    myHeader.chunk_size = 36 + len;
    myHeader.format = 0x57415645;
    myHeader.sub_chunk1_ID = 0x666d7420;
    myHeader.sub_chunk1_size = 16;
    myHeader.audio_format = 1;
    myHeader.num_channels = 1;
    myHeader.sample_rate = 8000;
    myHeader.byte_rate = 8000 * 1 * 16 / 8;
    myHeader.block_align = 1 * 16 / 8;
    myHeader.bits_per_sample = 16;
    myHeader.sub_chunk2_ID = 0x64617461;
    myHeader.sub_chunk2_size = len * 1 * 16 / 8;
    print_header(myHeader);

    
    // Test handle_endian.
    handle_endian(&myHeader);
    print_header(myHeader);


    // Test tr_load and tr_save.
    int16_t buff1[10] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
    int16_t buff2[10] = {};

    // Note: remember to change the file path if called individually in different current path.
    wav_save("../wav/test.wav", buff1, 10);
    int success = wav_load("../wav/test.wav", buff2);

    if(success != 0){
        // Print the buff2 to see if all content are saved and load.
        for(int i = 0; i < 10; i++){
            printf("%d\n", buff2[i]);
        }
    }

    return;
}