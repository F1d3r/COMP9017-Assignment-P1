#include <stdio.h>

#include "../../include/child_parent.h"
#include "../../include/helper.h"
#include "../../include/list_op.h"
#include "../../include/struct.h"
#include "../../include/track.h"
#include "../../include/wav.h"

void main(){ 
    // Test the tr_identify and tr_resolve
    Track* myTrack1 = tr_init();
    int16_t buff1[10] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
    tr_write(myTrack1, buff1, 0, 10);

    Track* myTrack2 = tr_init();
    int16_t buff2[10] = {11, 22, 33};
    tr_write(myTrack2, buff2, 0, 3);

    tr_insert(myTrack2, myTrack1, 2, 0, 3);
    tr_insert(myTrack2, myTrack1, 5+3, 0, 3);
    tr_insert(myTrack2, myTrack1, 6+6, 0, 3);

    char* result = tr_identify(myTrack1, myTrack2);
    printf("%s\n\n", result);
    free(result);

    Track** lists = malloc(sizeof(Track*)*2);
    lists[0] = myTrack1;
    lists[1] = myTrack2;
    tr_resolve(lists, 2);
    print_data(myTrack1, true);
    print_data(myTrack2, true);
    free(lists);

    
    tr_destroy(myTrack1);
    tr_destroy(myTrack2);

    return;
}