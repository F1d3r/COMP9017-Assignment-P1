#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include <math.h>


typedef struct sound_seg {
    struct sound_seg* parent;
    struct sound_seg** children;    // Children is a pointer to an set of pointers.
    uint16_t num_children;
    uint16_t trackLen;
    int16_t* data;
    struct sound_seg* next;
} Track;


typedef struct {
    uint32_t chunkID;
    uint32_t chunkSize;
    uint32_t format;
    uint32_t subChunk1ID;
    uint32_t subChunk1Size;
    uint16_t audioFormat;
    uint16_t numChannels;
    uint32_t sampleRate;
    uint32_t byteRate;
    uint16_t blockAlign;
    uint16_t bitsPerSample;
    uint32_t subChunk2ID;
    uint32_t subChunk2Size;
} WAVHeader;

void print_header(WAVHeader header);


// Given an unsigned integer with big endian, convert it to little endian form.
uint16_t convert_to_little_uint16(uint16_t big);


int16_t convert_to_little_int16(int16_t big);


uint32_t convert_to_little_uint32(uint32_t big);


void handle_endian(WAVHeader *header);


// Load a WAV file into buffer
int wav_load(const char* filename, int16_t* dest);


// Create/write a WAV file from buffer
void wav_save(const char* fname, int16_t* src, size_t len);


Track* tr_init();

void tr_read(Track* track, int16_t* buff, size_t pos, size_t len);


void tr_destroy(Track* track);


// Parent and child shares the data, the data lenght.
void add_child(Track* parent, Track* child);


void remove_child(Track* parent, Track* child);


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


// Return the lenght start from track on the list.
size_t tr_length(Track* track);


// Given two root tracks share same children, merge them and all children.
void merge_track(Track* prev, Track* next);


bool check_neighbor(Track* prev, Track* next);


// Check if a track list can merge some tracks. Used to save memory.
void check_merge(Track* track);


void tr_insert(Track* src_track, Track* dest_track, size_t destpos, size_t srcpos, size_t len);


Track* get_delete_start(Track* track, size_t pos, size_t len);


Track* get_delete_end(Track* track, size_t pos, size_t len);


// Handle tracks link.
bool tr_delete_range(Track* track, size_t pos, size_t len);


// Given two integers, return a string composed of them: "<start>,<end>".
char* get_string(int start, int end);


// Given a target track and a advertisement sample, calculate the cross
// correlation. Compare to the autocorrelation of the ad. Find out if
// the portion of target is the ad.
// Returns a string containing <start>,<end> ad pairs in target
char* tr_identify(Track* target, Track* ad);