#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>

#define BUFFER_LEN 524288
#define INT_BUFFER_LEN 11
#define WAV_HEADER_OFFSET 44


typedef struct sound_seg {
    struct sound_seg* parent;
    struct sound_seg** children;    // Children is a pointer to an set of pointers.
    size_t num_children;
    struct sound_seg* next;
    size_t trackLen;
    int16_t* data;
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


void print_header(WAVHeader header){
    printf("Chunk ID: ");
    printf("%c", (char)((header.chunkID & 0xFF000000)>>24));
    printf("%c", (char)((header.chunkID & 0x00FF0000)>>16));
    printf("%c", (char)((header.chunkID & 0x0000FF00)>>8));
    printf("%c\n", (char)((header.chunkID & 0x000000FF)));

    printf("Chunk Size: %d.\n", 36+header.subChunk2Size);

    printf("Format: ");
    printf("%c", (char)((header.format & 0xFF000000)>>24));
    printf("%c", (char)((header.format & 0x00FF0000)>>16));
    printf("%c", (char)((header.format & 0x0000FF00)>>8));
    printf("%c\n", (char)((header.format & 0x000000FF)));

    printf("Subchunk 1 ID: ");
    printf("%c", (char)((header.subChunk1ID & 0xFF000000)>>24));
    printf("%c", (char)((header.subChunk1ID & 0x00FF0000)>>16));
    printf("%c", (char)((header.subChunk1ID & 0x0000FF00)>>8));
    printf("%c\n", (char)((header.subChunk1ID & 0x000000FF)));

    printf("Audio format: ");
    if(header.audioFormat == 1){
        printf("PCM.\n");
    }else{
        printf("Other.\n");
    }

    printf("Number of channels: ");
    if(header.numChannels == 1){
        printf("Mono.\n");
    }else if(header.numChannels == 2){
        printf("Stereo.\n");
    }else{
        printf("Other.\n");
    }

    printf("Sample rate: %d.\n", header.sampleRate);

    printf("Byte rate: %d.\n", header.byteRate);

    printf("Block align: %d.\n", header.blockAlign);

    printf("Bits per sample: %d.\n", header.bitsPerSample);

    printf("Subchunk 2 ID: ");
    printf("%c", (char)((header.subChunk2ID & 0xFF000000)>>24));
    printf("%c", (char)((header.subChunk2ID & 0x00FF0000)>>16));
    printf("%c", (char)((header.subChunk2ID & 0x0000FF00)>>8));
    printf("%c\n", (char)((header.subChunk2ID & 0x000000FF)));

    printf("Subchunk 2 Size: %d.\n\n", header.subChunk2Size);
}


// Given an unsigned integer with big endian, convert it to little endian form.
uint16_t convert_to_little_uint16(uint16_t big){
    // For 0xABCD, OR(0xCD00, 0x00AB)
    return ((big >> 8) | (big << 8));
}


int16_t convert_to_little_int16(int16_t big){
    // For 0xABCD, OR(0xCD00, 0x00AB)
    return ((big >> 8) | (big << 8));
}


uint32_t convert_to_little_uint32(uint32_t big){
    // For 0xABCDEFGH, OR(0xGH000000, 0x00EF0000, 0x0000CD00, 0x000000AB)
    return ((big & (0x000000FF)) << 24 | (big & (0x0000FF00)) << 8 | (big & (0x00FF0000)) >> 8 | (big & (0xFF000000)) >> 24);
}


void handle_endian(WAVHeader *header){
    header->chunkID = convert_to_little_uint32(header->chunkID);
    header->format = convert_to_little_uint32(header->format);
    header->subChunk1ID = convert_to_little_uint32(header->subChunk1ID);
    header->subChunk2ID = convert_to_little_uint32(header->subChunk2ID);
}


// Load a WAV file into buffer
int wav_load(const char* filename, int16_t* dest){

    // Read the binary information from the file.
    FILE *myWAV = fopen(filename, "rb");
    // Process the exception
    if(!myWAV){
        perror("Open file failed.\n");
        return 0;
    }

    // Read the header.
    WAVHeader header;
    fread(&header, sizeof(WAVHeader), 1, myWAV);
    handle_endian(&header);
    print_header(header);

    // Fixed size buffer version.
    fread(dest, sizeof(int16_t), header.subChunk2Size/2, myWAV);

    // // Dynamically allocate memory for the buffer.
    // // Allocate space for the dest.
    // *dest = malloc(header.subChunk2Size);

    // for(int i = 0; i < header.subChunk2Size/2; i++){
    //     fread((*dest)+i, sizeof(int16_t), 1, myWAV);
    // }


    fclose(myWAV);

    return 1;
}


// Create/write a WAV file from buffer
void wav_save(const char* fname, int16_t* src, size_t len){
    // Define a WAV header.
    // WAVHeader myHeader;
    // myHeader.chunkID = 0x52494646;
    // myHeader.chunkSize = convert_to_little_uint32(36 + len);
    // myHeader.format = 0x57415645;
    // myHeader.subChunk1ID = 0x666d7420;
    // myHeader.subChunk1Size = convert_to_little_uint32(16);
    // myHeader.audioFormat = convert_to_little_uint16(1);
    // myHeader.numChannels = convert_to_little_uint16(1);
    // myHeader.sampleRate = convert_to_little_uint32(8000);
    // myHeader.byteRate = convert_to_little_uint32(8000 * 1 * 16 / 8);
    // myHeader.blockAlign = convert_to_little_uint16(1 * 16 / 8);
    // myHeader.bitsPerSample = convert_to_little_uint16(16);
    // myHeader.subChunk2ID = 0x64617461;
    // myHeader.subChunk2Size = convert_to_little_uint32(len);
    
    WAVHeader myHeader;
    myHeader.chunkID = 0x52494646;
    myHeader.chunkSize = 36 + len;
    myHeader.format = 0x57415645;
    myHeader.subChunk1ID = 0x666d7420;
    myHeader.subChunk1Size = 16;
    myHeader.audioFormat = 1;
    myHeader.numChannels = 1;
    myHeader.sampleRate = 8000;
    myHeader.byteRate = 8000 * 1 * 16 / 8;
    myHeader.blockAlign = 1 * 16 / 8;
    myHeader.bitsPerSample = 16;
    myHeader.subChunk2ID = 0x64617461;
    myHeader.subChunk2Size = len * 1 * 16 / 8;

    handle_endian(&myHeader);
    print_header(myHeader);

    // Define the file.
    FILE *myWAV = fopen(fname, "wb");
    // Write the header into the file, with the size of the header, once, by bit.
    fwrite(&myHeader, sizeof(WAVHeader), 1, myWAV);

    // Then, write the data into the file.
    // There are |len| uint16_t to write.
    for(int i = 0; i < len; i++){
        fwrite((src++), sizeof(int16_t), 1, myWAV);
    }


    fclose(myWAV);

    return;
}


Track* tr_init(){
    Track* myTrack = malloc(sizeof(Track));
    myTrack->parent = NULL;
    myTrack->num_children = 0;
    myTrack->children = NULL;
    myTrack->next = NULL;
    myTrack->trackLen = 0;
    myTrack->data = NULL;
}


void tr_destroy(Track* track){
    if(track->next != NULL){
        tr_destroy(track->next);
    }
    if(track->data != NULL){
        if(track->parent == NULL){
            printf("Freeing %ld samples.\n", track->trackLen);
            free(track->data);
        }
    }
    printf("Freeing track %p.\n", track);
    free(track->children);
    free(track);

    return;
}


// Return the lenght start from track on the list.
size_t tr_length(Track* track){
    size_t len = 0;
    while(track != NULL){
        len += track->trackLen;
        track = track->next;
    }

    return len;
}


void tr_read(Track* track, int16_t* buff, size_t pos, size_t len){
    if(len == 0){
        return;
    }

    // Go to the correct track to read.
    while(pos >= track->trackLen){
        pos -= track->trackLen;
        track = track->next;
    }
    size_t buffPos = 0;
    while(track->trackLen - pos < len){
        memcpy(buff + buffPos, track->data + pos, (track->trackLen - pos)*2);
        buffPos += (track->trackLen - pos);
        len -= (track->trackLen - pos);
        pos -= pos;
        track = track->next;
    }
    // Read the last track.
    memcpy(buff+buffPos, track->data, len*2);

    return;
}


void tr_write(Track* track, int16_t* buff, size_t pos, size_t len){
    if(len == 0){
        return;
    }

    Track* temp = track;
    // Go to the correct track to write.
    while(pos > temp->trackLen){
        pos -= temp->trackLen;
        temp = temp->next;
    }
    size_t buffPos = 0;
    bool not_enough_space = false;

    // If the track is empty.
    if(temp->trackLen == 0){
        // Create a new data memory to store all data.
        temp->data = malloc(sizeof(int16_t)*len);
        temp->trackLen = len;
        memcpy(temp->data + pos, buff + buffPos, (temp->trackLen - pos)*2);
        len -= len;
        return;
    }
    
    // Keep writing the data into tracks.
    while(len >= temp->trackLen - pos){
        // Otherwise, overwrite the track without extend it.
        // Overwrite the all space.
        memcpy(temp->data + pos, buff + buffPos, (temp->trackLen - pos)*2);
        buffPos += (temp->trackLen - pos);
        len -= (temp->trackLen - pos);
        pos -= pos;
        // If the track is not the last on the list.
        temp = temp->next;
        if(temp == NULL){
            break;
        }
    }
    // If the data has been all writen to the list, the write is complete.
    if(len == 0){
        return;
    }else{  
        // If the last track is already been filled.
        if(temp == NULL){
            // Go to the last track.
            temp = track;
            while(temp->next != NULL){
                temp = temp->next;
            }
            // Create a new track, and write the content in the new track.
            Track* new_track = tr_init();
            tr_write(new_track, buff+buffPos, 0, len);
            // Add the new track at the end of the list.
            temp->next = new_track;
        }else{  // The last track is now being write.
            memcpy(temp->data+pos, buff+buffPos, sizeof(int16_t)*len);
            len -= len;
        }

    }


    // // Go to the correct track to write.
    // while(pos > track->trackLen){
    //     track = track->next;
    //     pos -= track->trackLen;
    // }
    // size_t buffPos = 0;
    // bool not_enough_space = false;
    // // When the current track has not enough space to write.
    // while(len >= track->trackLen - pos){
    //     // If this is the last track.
    //     if(track->next == NULL){
    //         // The track list has not enough space to fit the data.
    //         not_enough_space = true;
    //         break;
    //     }
    //     // Otherwise, overwrite the track without extend it.
    //     // Overwrite the all space.
    //     memcpy(track->data + pos, buff + buffPos, (track->trackLen - pos)*2);
    //     buffPos += (track->trackLen - pos);
    //     len -= (track->trackLen - pos);
    //     pos -= pos;
    //     // If the track is not the last on the list.
    //     track = track->next;
    // }
    // // If the while is break, overwrite the last track and extend it.
    // if(not_enough_space){
    //     // If the track is refering other, do not extend it, create new track instead.
    //     if(track->parent != NULL){
    //         memcpy(track->data + pos, buff + buffPos, (track->trackLen - pos)*2);
    //         buffPos += (track->trackLen - pos);
    //         len -= (track->trackLen - pos);
    //         Track* new_track = tr_init();
    //         track->next = new_track;
    //         tr_write(new_track, buff+buffPos, 0, len);
            
    //     }else{  // If this is the root track, extend it.
    //         // Update the new track lenght.
    //         track->trackLen = (pos+len)>track->trackLen ? (pos+len):track->trackLen;
    //         // Reallocate memory for the data.
    //         int16_t *temp = realloc(track->data, track->trackLen*2);
    //         // Check if the reallocation success.
    //         if(temp == NULL){
    //             printf("The reallocation failed.\n");
    //             return;
    //         }else{
    //             // Update the data pointer if success.
    //             printf("The reallocation success.\n");
    //             track->data = temp;
    //         }
    //         // Copy the content to the position
    //         memcpy(track->data + pos, buff+buffPos, len*2);
    //     }
    // }else{  // The last track can fit the content.
    //     // Copy the content to the position
    //     memcpy(track->data + pos, buff+buffPos, len*2);
    //     int16_t* temp = realloc(track->data, (pos+len)*2);
    //     if(temp == NULL){
    //         printf("The reallocation failed.\n");
    //         return;
    //     }else{
    //         // Update the data pointer if success.
    //         printf("The reallocation success.\n");
    //         // Apply to children.
    //         if(track->num_children != 0){
    //             for(int i = 0; i < track->num_children; i++){
    //                 track->children[i]->data = temp;
    //                 track->children[i]->trackLen = pos+len;
    //             }
    //         }
    //         track->trackLen = pos+len;
    //         track->data = temp;
    //     }
    // }

    return;
}


// Split the track into two, at the position split_pos.
// The element at split_pos belongs to the right track.
void tr_split(Track* track_to_split, size_t split_pos){
    // Create a new track.
    Track* new_track = tr_init();
    // Update the length.
    new_track->trackLen = track_to_split->trackLen - split_pos;
    track_to_split->trackLen = track_to_split->trackLen - new_track->trackLen;
    // Set the data pointer.
    new_track->data = malloc(sizeof(int16_t) * new_track->trackLen);
    
    memcpy(new_track->data, track_to_split->data + split_pos, sizeof(int16_t)*new_track->trackLen);
    
    
    track_to_split->data = realloc(track_to_split->data, sizeof(int16_t)*track_to_split->trackLen);

    // Update the link.
    new_track->next = track_to_split->next;
    track_to_split->next = new_track;
    printf("Two new tracks: %p, %p.\n", track_to_split, new_track);
    printf("Lenght of two new tracks: %ld, %ld.\n", track_to_split->trackLen, new_track->trackLen);
    printf("Starting from %d, %d.\n", track_to_split->data[0], new_track->data[0]);

    // Split the children as well.
    if(track_to_split->num_children != 0){
        for(int i = 0; i < track_to_split->num_children; i++){
            printf("Spliting the child %p as well.\n", track_to_split->children[i]);
            tr_split(track_to_split->children[i], split_pos);
        }
    }

    printf("\n");
    return;
}


// Make a copy of the track list, from start, to end track.
Track* copy_track_list(Track* src_track_start, Track* src_track_end){
    printf("Copying tracks from %p to %p.\n", src_track_start, src_track_end);
    Track* copy_track = tr_init();
    memcpy(copy_track, src_track_start, sizeof(Track));
    copy_track->next = NULL;
    // Update parent-child relationship
    copy_track->parent = src_track_start;
    src_track_start->num_children += 1;
    src_track_start->children = realloc(src_track_start->children, sizeof(Track*) * src_track_start->num_children);
    src_track_start->children[src_track_start->num_children-1] = copy_track;
    printf("A new track created: %p\n", copy_track);
    printf("Its parent: %p\n", copy_track->parent);

    if(src_track_end == src_track_end){
        printf("\n");
        return copy_track;
    }
    Track* copy_temp = copy_track;
    Track* src_temp = src_track_start->next;
    while(src_temp != src_track_end->next){
        copy_temp->next = tr_init();
        memcpy(copy_temp->next, src_temp, sizeof(Track));
        // Update parent-child relationship
        copy_temp->parent = src_temp;
        src_temp->num_children += 1;
        src_temp->children = realloc(src_temp->children, sizeof(Track*) * src_temp->num_children);
        src_temp->children[src_temp->num_children-1] = copy_temp;
        printf("A new track created: %p\n", copy_temp);
        printf("Its parent: %p\n", copy_temp->parent);
        // Update the link.
        copy_temp = copy_temp->next;
        src_temp = src_temp->next;
    }

    printf("\n");
    return copy_track;
}


// Insert a portion of src_track into dest_track at position destpos
// void tr_insert(Track* src_track, Track* dest_track, 
//     size_t destpos, size_t srcpos, size_t len) {
    
//     Track* src_start_track = src_track;
//     // Go to the correct track from.
//     while(srcpos >= src_start_track->trackLen){
//         srcpos -= src_start_track->trackLen;
//         src_start_track = src_start_track->next;
//     }
//     // If the starting point is at the middle, split the track.
//     if(srcpos != 0){
//         printf("Spliting %p to two tracks at position %ld.\n", src_start_track, srcpos);
//         tr_split(src_start_track, srcpos);
//         src_start_track = src_start_track->next;
//     }

//     // Go to the last track of src to insert.
//     Track* src_end_track = src_start_track;
//     while(len >= src_end_track->trackLen){
//         len -= src_end_track->trackLen;
//         src_end_track = src_end_track->next;
//     }
//     // If the starting point is at the middle, split the track.
//     if(len != src_end_track->trackLen){
//         printf("Spliting %p to two tracks at position %ld.\n", src_end_track, len);
//         tr_split(src_end_track, len);
//     }

//     // Now, the track from src_start_track, to src_end_track is the sublist to insert.
//     // Copy the sublist.
//     Track* copy_track = copy_track_list(src_start_track, src_end_track);
    
    
//     // Go to the position to insert at the dest.
//     Track* dest_track_to_insert = dest_track;
//     while(destpos > dest_track_to_insert->trackLen){
//         destpos -= dest_track_to_insert->trackLen;
//         dest_track_to_insert = dest_track_to_insert->next;
//     }
//     // If the position to insert is 0
//     if(destpos == 0){
//         Track* new_track = tr_init();
//         memcpy(new_track, dest_track_to_insert, sizeof(Track));
//         memcpy(dest_track_to_insert, src_start_track, sizeof(Track));
//         Track* last_on_copy_list = copy_track;
//         while(last_on_copy_list->next != NULL){
//             last_on_copy_list = last_on_copy_list->next;
//         }
//         if(last_on_copy_list != copy_track){
//             last_on_copy_list->next = new_track;
//         }else{
//             dest_track_to_insert->next = new_track;
//         }
//         free(copy_track);
//     }   // If the insertion position is the last of the track.
//     else if(destpos == dest_track_to_insert->trackLen){
//         Track* temp = dest_track_to_insert->next;
//         dest_track_to_insert->next = copy_track;

//         Track* last_on_copy_list = copy_track;
//         while(last_on_copy_list->next != NULL){
//             last_on_copy_list = last_on_copy_list->next;
//         }
//         last_on_copy_list->next = temp;
//     }else{
//         printf("Spliting %p to two tracks at position %ld.\n", dest_track_to_insert, destpos);
//         tr_split(dest_track_to_insert, destpos);
//         Track* temp = dest_track_to_insert->next;
//         dest_track_to_insert->next = copy_track;
//         Track* last_on_copy_list = copy_track;
//         while(last_on_copy_list->next != NULL){
//             last_on_copy_list = last_on_copy_list->next;
//         }
//         last_on_copy_list->next = temp;
//     }
    

//     printf("\n");
//     return;
// }


// Parent and child shares the data, the data lenght.
void add_child(Track* parent, Track* child){
    // If the child has already been child of the parent. Do not add again.
    bool already_child = false;
    for(int i = 0; i < parent->num_children; i++){
        if(child == parent->children[i]){
            already_child = true;
            break;;
        }
    }
    if(!already_child){
        // Add child as parent's children.
        parent->num_children += 1;
        parent->children = realloc(parent->children, sizeof(Track*)*parent->num_children);
        parent->children[parent->num_children-1] = child;
    }
    
    // Add parent as child's parent.
    child->parent = parent;
    // Share data.
    child->data = parent->data;
    child->trackLen = parent->trackLen;
}


void remove_child(Track* parent, Track* child){
    for(int i = 0; i < parent->num_children; i++){
        if(child == parent->children[i]){
            Track** temp = parent->children;
            parent->num_children -= 1;
            parent->children = malloc(parent->num_children);
            // Transfer the children data.
            memcpy(parent->children, temp, sizeof(Track*)*(i));
            memcpy(parent->children, temp+i+1, sizeof(Track*)*(parent->num_children-i));
            // Free the old one.
            free(temp);
            return;
        }
    }

}


void split_children(Track* parent){
    for(int i = 0; i < parent->num_children; i++){
        Track* child = parent->children[i];
        Track* new_child = tr_init();
        // Update link
        new_child->next = child->next;
        child->next = new_child;
        // Update child-parent relationship.
        add_child(parent->next, new_child);
        add_child(parent, child);
        
        split_children(child);
    }
}


// The valid pos range is [0, track.trackLen]
// The element at pos will belongs to the right track afte split.
void split_track(Track* track, size_t pos){
    // First go find the track to be split.
    while(pos > track->trackLen){
        pos -= track->trackLen;
        track = track->next;
    }
    // Now the pos is at range [0, track.trackLen]
    // If the track to split is not a parent track, split its parent.
    if(track->parent != NULL){
        split_track(track->parent, pos);
        return;
    }
    // Then the root parent is to be splited.
    // If the pos is the first or the one after the last.
    if(pos == 0 || pos == track->trackLen){
        // Do not need to split.
        return;
    }
    // Now the pos is at range [1, track.trackLen-1]
    Track* new_track = tr_init();
    // Update data length.
    new_track->trackLen = track->trackLen - pos;
    track->trackLen = pos;
    // Update link
    new_track->next = track->next;
    track->next = new_track;
    // Copy data for the new track.
    new_track->data = malloc(sizeof(int16_t) * new_track->trackLen);
    memcpy(new_track->data, track->data+pos, sizeof(int16_t)*new_track->trackLen);
    // Reallocate the data for old track.
    track->data = realloc(track->data, sizeof(int16_t)*track->trackLen);

    // Now the root tracks are done. Then propagate to all its children.
    split_children(track);

    return;
}


Track* copy_track(Track* src_start, Track* src_end){
    Track* temp = src_start;
    int num_tracks = 0;
    while(temp != src_end->next){
        num_tracks += 1;
        temp = temp->next;
    }

    Track* copy = tr_init();
    add_child(src_start, copy);
    temp = copy;
    for(int i = 1; i < num_tracks; i++){
        Track* current = src_start;
        for(int j = 0; j < i; j++){
            current = current->next;
        }
        Track* new = tr_init();
        add_child(current, new);
        temp->next = new;
        temp = new;
    }

    return copy;
}


void tr_insert(Track* src_track, Track* dest_track, size_t destpos, size_t srcpos, size_t len){
    // Split tracks according to given positions.
    split_track(dest_track, destpos);
    split_track(src_track, srcpos);
    split_track(src_track, srcpos+len);

    // Get the track from src, to start, to end, and from dest, before the insertion.
    Track* src_start = src_track;
    size_t temp = 0;
    while(temp != srcpos){
        temp += src_start->trackLen;
        src_start = src_start->next;
    }
    Track* src_end = src_start;
    temp = src_end->trackLen;
    while(temp != len){
        src_end = src_end->next;
        temp += src_end->trackLen;
    }
    Track* dest_before = dest_track;
    temp = dest_before->trackLen;
    while(temp != destpos){
        dest_before = dest_before->next;
        temp += dest_before->trackLen;
    }

    Track* copy = copy_track(src_start, src_end);
    Track* last = copy;
    while(last->next != NULL){
        last = last->next;
    }
    last->next = dest_before->next;
    dest_before->next = copy;

    

}


// Handle data, and parent/child relationship.
void delete_data(Track* track, size_t pos, size_t len){
    // If the track has parent.
    if(track->parent != NULL){
        if(pos != 0){
            // Split its parent at pos, and the track itself.
            split_track(track->parent, pos);
        }
        if(len != track->trackLen - pos){
            // Split its parent at pos + len.
            split_track(track->parent, pos+len);
        }

        // If the split is not performed. This track should be the one to be removed.
        if(pos == 0){
            // Unbond the next track and its parent.
            remove_child(track->parent, track);
            // free(track);
        }else{  // The next is the one to be removed.
            Track* next = track->next;
            // Unbond the next track and its parent.
            remove_child(next->parent, next);
            track->next = next->next;
            free(next);
        }
        
        

    }else{  // If the track has no parent. It is a root track.
        // Split the track at pos and len
        split_track(track, pos+len);
        split_track(track, pos);
        // If pos is 0, the track is to be deleted.
        Track* next = track->next;
        if(pos != 0){
            track->next = next->next;
            free(next->data);
            free(next);
        }else{
            // If the whole track is removed.
            if(len != track->trackLen){
                memcpy(track, next, sizeof(Track));
                free(next);
                free(track->data);
            }else{
                free(track->data);
            }
            
        }
    

    }


    return;
}


// Handle tracks link.
bool tr_delete_range(Track* track, size_t pos, size_t len){

    // First find the start and end track to delete.
    Track* delete_start = track;
    while(pos >= delete_start->trackLen){
        pos -= delete_start->trackLen;
        delete_start = delete_start->next;
    }
    size_t temp_len = len;
    Track* delete_end = delete_start;
    while(temp_len > delete_end->trackLen - pos){
        temp_len -= delete_end->trackLen;
        delete_end = delete_end->next;
    }

    // Go through the delete list, find if any track is a parent.
    bool has_parent = false;
    Track* temp_track = delete_start;
    while(temp_track != delete_end->next){
        if(temp_track->num_children != 0){
            has_parent = true;
            break;
        }
        temp_track = temp_track->next;
    }
    // If there is a parent to be deleted, abort the delete and return false.
    if(has_parent){
        return false;
    }

    // Now, all the tracks are deletable.
    Track* current = delete_start;
    Track* end = delete_end->next;
    while(current != end){
        size_t delete_len = len >= current->trackLen-pos ? current->trackLen-pos:len;
        delete_data(current, pos, delete_len);
        len -= current->trackLen - pos;
        pos -= pos;
        current->trackLen -= delete_len;

        // If the whole track is deleted. Update the link.
        if(current->trackLen == 0){
            // Remove the track on the list.
            // If the current track is the first in the list.
            if(current == track){
                // If the head track has other track after it.
                if(current->next != NULL){
                    // Create a new track.
                    Track* new_track = tr_init();
                    // Copy the next track.
                    memcpy(new_track, current->next, sizeof(Track));
                    // Tell any parent of next.
                    if(current->next->parent != NULL){
                        // Transfer the children relationship.
                        add_child(current->next->parent, current);
                        remove_child(current->next->parent, current->next);
                    }
                    // Remove next track.
                    free(current->next);
                }
                
            }else{  // If the track to be remove has a track before it.
                // Find the track before.
                Track* before = track;
                while(before->next != current){
                    before = before->next;
                }
                // Update links.
                before->next = current->next;
                // Free the track
                Track* temp = current;
                current = current->next;
                free(temp);
                continue;
            }
        }

        current = current->next;
    }
    printf("Test");


    return true;
}


void print_data(Track* myTrack){
    int len = 0;
    // Count the number of datas.
    Track* temp = myTrack;
    while(temp != NULL){
        len += temp->trackLen;
        temp = temp->next;
    }
    printf("%d data: ", len);
    temp = myTrack;

    // Print the data in every track.
    int track_num = 0;
    while(myTrack != NULL){
        printf("track %d (%p): ", track_num, myTrack);
        for (int i = 0; i < myTrack->trackLen; i++)
        {
            printf("%d ", myTrack->data[i]);
        }
        printf("\n");
        myTrack = myTrack->next;
        track_num ++;
    }

    while(temp != NULL){
        if(temp->parent == NULL){
            printf("Root track: %p, data:",temp);
            for(int i = 0; i < temp->trackLen; i++){
                printf(" %d", temp->data[i]);
            }
            printf(". Children:");
            for(int i = 0; i < temp->num_children; i++){
                printf(" %p", temp->children[i]);
            }
            printf("\n");
        }
        temp = temp->next;
    }


    
    printf("\n");
}


void main(){

    // Initialization.
    Track* myTrackA = tr_init();
    Track* myTrackB = tr_init();

    int16_t buff1[10] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
    int16_t buff2[9] = {11, 22, 33, 44, 55, 66, 77, 88, 99};

    tr_write(myTrackA, buff1, 0, 10);
    tr_write(myTrackB, buff2, 0, 9);
    tr_insert(myTrackA, myTrackA, 2, 3, 4);
    print_data(myTrackA);
    bool test = tr_delete_range(myTrackA, 2, 6);
    printf("Deleted: %d\n", test);
    print_data(myTrackA);
    test = tr_delete_range(myTrackA, 2, 3);
    printf("Deleted: %d\n", test);
    print_data(myTrackA);

    // insert_track(myTrackA, myTrackA, 2, 3, 4);
    // print_data(myTrackA);
    // insert_track(myTrackA, myTrackA, 2, 3, 4);
    // print_data(myTrackA);

    // insert_track(myTrackA, myTrackB, 2, 3, 4);
    // print_data(myTrackA);
    // print_data(myTrackB);
    // insert_track(myTrackB, myTrackA, 4, 1, 3);
    // print_data(myTrackA);
    // print_data(myTrackB);

    // tr_write(myTrackA, buff1, 2, 5);
    // print_data(myTrackA);
    // print_data(myTrackB);

    tr_destroy(myTrackA);
    tr_destroy(myTrackB);



    return;
}