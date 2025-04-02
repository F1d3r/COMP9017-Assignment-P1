#include <stdio.h>

#include "../include/child_parent.h"
#include "../include/helper.h"
#include "../include/list_op.h"
#include "../include/struct.h"
#include "../include/track.h"
#include "../include/wav.h"

void main(){ 
    // Test tr_read.
    Track* myTrack = tr_init();

    // Test tr_write
    int16_t buff1[10] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
    tr_write(myTrack, buff1, 0, 10);

    int16_t buff2[15];
    tr_read(myTrack, buff2, 0, 10);
    for(int i = 0; i < 10; i++){
        printf("%d\n", buff2[i]);
        buff1[i] = (i+1) * 11;
    }
    printf("\n");
    
    tr_write(myTrack, buff1, 5, 10);
    tr_read(myTrack, buff2, 0, 15);
    for(int i = 0; i < 15; i++){
        printf("%d\n", buff2[i]);
    }

    tr_destroy(myTrack);

    return;
}