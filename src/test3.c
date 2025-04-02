#include <stdio.h>

#include "../include/child_parent.h"
#include "../include/helper.h"
#include "../include/list_op.h"
#include "../include/struct.h"
#include "../include/track.h"
#include "../include/wav.h"

void main(){ 
    // Test print_data
    Track* myTrack = tr_init();
    // Since the print_dat function prints the memory address 
    // of track, parent and children, it is hard to write
    // output expectation file to compare.
    // The test here is done by reading the output at the terminal.
    print_data(myTrack, true);


    // Test tr_write
    int16_t buff1[10] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
    tr_write(myTrack, buff1, 0, 10);
    print_data(myTrack, true);

    int16_t buff2[10];
    tr_read(myTrack, buff2, 0, 10);
    for(int i = 0; i < 10; i++){
        printf("%d\n", buff2[i]);
    }

    tr_destroy(myTrack);


    return;
}