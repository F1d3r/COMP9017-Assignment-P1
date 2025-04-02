#include <stdio.h>

#include "../include/child_parent.h"
#include "../include/helper.h"
#include "../include/list_op.h"
#include "../include/struct.h"
#include "../include/track.h"
#include "../include/wav.h"

void main(){ 
    // Test the tr_init() and tr_destroy()
    Track* myTrack1 = tr_init();
    printf("%p\n", myTrack1->next);
    printf("%d\n", myTrack1->trackLen);
    printf("%p\n", myTrack1->parent);
    printf("%d\n", myTrack1->num_children);
    printf("%p\n", myTrack1->children);
    printf("%p\n\n", myTrack1->data);

    Track* myTrack2 = tr_init();
    printf("%p\n", myTrack2->next);
    printf("%d\n", myTrack2->trackLen);
    printf("%p\n", myTrack2->parent);
    printf("%d\n", myTrack2->num_children);
    printf("%p\n", myTrack2->children);
    printf("%p\n\n", myTrack2->data);

    tr_destroy(myTrack1);
    tr_destroy(myTrack2);

    return;
}