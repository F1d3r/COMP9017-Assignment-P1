#include "struct.h"
#include <stdbool.h>

// Copy the src_track, to get a new track.
Track* tr_copy(Track* src_track);


// Make a copy of the list from start to end.
Track* copy_list(Track* src_start, Track* src_end);


// Split the track into two, at the position split_pos.
// The element at split_pos belongs to the right track.
void tr_split(Track* track_to_split, size_t split_pos);


void split_children(Track* parent);


// The valid pos range is [0, track.trackLen]
// The element at pos will belongs to the right track afte split.
void split_track(Track* track, size_t pos);


// Given two root tracks share same children, merge them and all children.
void merge_track(Track* prev, Track* next);


bool check_neighbor(Track* prev, Track* next);


// Check if a track list can merge some tracks. Used to save memory.
void check_merge(Track* track);


Track* get_delete_start(Track* track, size_t pos, size_t len);


Track* get_delete_end(Track* track, size_t pos, size_t len);
