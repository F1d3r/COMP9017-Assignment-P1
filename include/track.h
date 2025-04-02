#ifndef TRACK_H
#define TRACK_H

#include <stdbool.h>
#include "struct.h"

Track* tr_init();


void tr_read(Track* track, int16_t* buff, size_t pos, size_t len);


void tr_write(Track* track, int16_t* buff, size_t pos, size_t len);


void tr_destroy(Track* track);


// Return the lenght start from track on the list.
size_t tr_length(Track* track);



void tr_insert(Track* src_track, Track* dest_track, size_t destpos, size_t srcpos, size_t len);


void print_data(Track* myTrack, bool easy);


// Handle tracks link.
bool tr_delete_range(Track* track, size_t pos, size_t len);


// Given a target track and a advertisement sample, calculate the cross
// correlation. Compare to the autocorrelation of the ad. Find out if
// the portion of target is the ad.
// Returns a string containing <start>,<end> ad pairs in target
char* tr_identify(Track* target, Track* ad);


void tr_resolve(Track** list, size_t list_len);

#endif