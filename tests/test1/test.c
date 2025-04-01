#include "src/sound_seg.h"

void main(){ 

    struct sound_seg* s0 = tr_init();
    tr_write(s0, ((int16_t[]){0, 1, 2, 3, 4, 5, 6}), 0, 7);
    
    tr_insert(s0, s0, 2, 0, 3);

    print_data(s0);

    Track* lists[1] = {s0};
    tr_resolve(lists, 1);
    print_data(s0);


    tr_destroy(s0);

    return;
}