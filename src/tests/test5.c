#include <stdio.h>

#include "../../include/child_parent.h"
#include "../../include/helper.h"
#include "../../include/list_op.h"
#include "../../include/struct.h"
#include "../../include/track.h"
#include "../../include/wav.h"

void main(){ 
    // Test the tr_insert and tr_delete
    Track* myTrack1 = tr_init();
    int16_t buff1[10] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
    tr_write(myTrack1, buff1, 0, 10);

    Track* myTrack2 = tr_init();
    int16_t buff2[10] = {11, 22, 33, 44, 55, 66};
    tr_write(myTrack2, buff2, 0, 6);

    tr_insert(myTrack2, myTrack1, 2, 2, 3);
    print_data(myTrack1, true);

    tr_insert(myTrack2, myTrack1, 3, 0, 2);
    print_data(myTrack1, true);

    // Test write after insert, reflect to parent/child.
    tr_write(myTrack1, buff1, 0, 4);
    print_data(myTrack1, true);
    print_data(myTrack2, true);

    tr_destroy(myTrack1);
    tr_destroy(myTrack2);


    Track* myTrack3 = tr_init();
    int16_t buff3[10] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
    tr_write(myTrack3, buff3, 0, 10);

    Track* myTrack4 = tr_init();
    int16_t buff4[10] = {11, 22, 33, 44, 55, 66};
    tr_write(myTrack4, buff4, 0, 6);

    tr_insert(myTrack4, myTrack3, 2, 2, 3);
    tr_delete_range(myTrack3, 2, 3);

    print_data(myTrack3, true);

    tr_destroy(myTrack3);
    tr_destroy(myTrack4);


    return;
}