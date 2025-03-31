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

volatile int num_tracks = 0;
volatile int num_samples = 0;


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

    // Go to the correct track to write.
    Track* temp = track;
    while(pos > temp->trackLen){
        pos -= temp->trackLen;
        temp = temp->next;
    }
    size_t buffPos = 0;

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

    return;
}


void tr_destroy(Track* track){
    if(track->next != NULL){
        tr_destroy(track->next);
    }
    if(track->data != NULL){
        if(track->parent == NULL){
            printf("Freeing %d samples.\n", track->trackLen);
            num_samples += track->trackLen;
            printf("Total: %d\n", num_samples);
            free(track->data);
            track->data = NULL;
            track->trackLen = 0;
        }
    }
    printf("Freeing track %p.\n", track);
    num_tracks += 1;
    printf("Total: %d\n", num_tracks);
    free(track->children);
    free(track);

    return;
}


// Parent and child shares the data, the data lenght.
void add_child(Track* parent, Track* child){
    // If the child has already been child of the parent. Do not add again.
    for(int i = 0; i < parent->num_children; i++){
        if(child == parent->children[i]){
            // Unify the data and data lenght of the child to parent.
            child->parent = parent;
            child->data = parent->data;
            child->trackLen = parent->trackLen;
            return;
        }
    }

    // Add child as parent's children.
    parent->num_children += 1;
    // Reallocate memory for the children.
    parent->children = realloc(parent->children, sizeof(Track*)*parent->num_children);
    parent->children[parent->num_children-1] = child;
    
    // Add parent as child's parent.
    child->parent = parent;
    // Share data.
    child->data = parent->data;
    child->trackLen = parent->trackLen;
}


void remove_child(Track* parent, Track* child){
    for(int i = 0; i < parent->num_children; i++){
        if(child == parent->children[i]){
            parent->num_children -= 1;
            // If this is the last child of the parent.
            if(parent->num_children == 0){
                // Free the children space.
                free(parent->children);
                parent->children = NULL;
            }else{
                // Record the old children.
                Track** temp = malloc(sizeof(Track*)*(parent->num_children+1));
                memcpy(temp, parent->children, sizeof(Track*)*(parent->num_children+1));
                // Reallocate space for children.
                parent->children = realloc(parent->children, sizeof(Track*)*parent->num_children);
                // Transfer the children data.
                memcpy(parent->children, temp, sizeof(Track*)*(i));
                memcpy(parent->children+i, temp+i+1, sizeof(Track*)*(parent->num_children-i));
                //?// memcpy(parent->children, temp[i+1], sizeof(Track*)*(parent->num_children-i));
                // Remove the old children.
                free(temp);
            }
            // Set the parent of child to NULL
            child->parent = NULL;
            return;
        }
    }

}


// Copy the src_track, to get a new track.
Track* tr_copy(Track* src_track){
    Track* new_track = tr_init();
    memcpy(new_track, src_track, sizeof(Track));
    // Generate new children.
    if(new_track->num_children != 0){
        new_track->children = malloc(sizeof(Track*)*new_track->num_children);
        memcpy(new_track->children, src_track->children, sizeof(Track*)*new_track->num_children);
    }
    
    return new_track;
}


// Make a copy of the list from start to end.
Track* copy_list(Track* src_start, Track* src_end){
    // Calculate the number of tracks to create.
    Track* temp = src_start;
    int num_tracks = 0;
    while(temp != src_end->next){
        num_tracks += 1;
        temp = temp->next;
    }

    // Create first copy track.
    Track* copy = tr_init();
    // Add the track as a child of source track.
    add_child(src_start, copy);
    temp = copy;
    for(int i = 1; i < num_tracks; i++){
        // Find the corresponding source track.
        Track* current = src_start;
        for(int j = 0; j < i; j++){
            current = current->next;
        }
        // Add a new track, as the child of source track.
        Track* new = tr_init();
        add_child(current, new);
        temp->next = new;
        temp = new;
    }

    return copy;
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
    printf("Lenght of two new tracks: %d, %d.\n", track_to_split->trackLen, new_track->trackLen);
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


// Return the lenght start from track on the list.
size_t tr_length(Track* track){
    size_t len = 0;
    while(track != NULL){
        len += track->trackLen;
        track = track->next;
    }

    return len;
}


// Given two root tracks share same children, merge them and all children.
void merge_track(Track* prev, Track* next){
    printf("Track %p and %p merged.\n", prev, next);
    // Copy content of next.
    if(prev->parent == NULL){   // If it is root, operate the data.
        prev->data = realloc(prev->data, sizeof(int16_t)*(prev->trackLen+next->trackLen));
        memcpy(prev->data+prev->trackLen, next->data, sizeof(int16_t)*next->trackLen);
    }else{
        // Else, just refer to the parent.
        prev->data = prev->parent->data;
    }
    prev->trackLen += next->trackLen;
    // Update link.
    prev->next = next->next;
    // Free next track.
    if(next->parent == NULL){
        free(next->data);
        next->trackLen = 0;
    }
    if(next->num_children != 0){
        free(next->children);
        next->children = NULL;
        next->num_children = 0;
    }
    free(next);

    // Propagate to children.
    for(int i = 0; i < prev->num_children; i++){
        merge_track(prev->children[i], prev->children[i]->next);
    }
}


bool check_neighbor(Track* prev, Track* next){
    // Check if they share exactly the same children.
    if(prev->num_children != next->num_children){
        // If the number of children is not the same, merge fails.
        return false;
    }

    // Check if thest two has exactly same neighbored children.
    bool same_children = true;
    for(int i = 0; i < prev->num_children; i++){
        bool find_match = false;
        for(int j = 0; j < next->num_children; j++){
            // If the j-th child of next root is the next of i-th child of current root.
            if(prev->children[i]->next == next->children[j]){
                // We find the match.
                // Check if the match has same property.
                if(!check_neighbor(prev->children[i], next->children[j])){
                    return false;
                }
                // If these match neighbor has same property.
                find_match = true;
            }
        }
        // If the i-th children of current is not found in next root.
        if(!find_match){
            // These two roots do not share same children.
            same_children = false;
        }
    }
    // If these two tracks do not share same children, merge fails.
    if(!same_children){
        return false;
    }

    return true;
}


// Check if a track list can merge some tracks. Used to save memory.
void check_merge(Track* track){
    // Go through the track list. Find the merge-able tracks.
    Track* current = track;
    while(current != NULL){
        // Get the next track of current.
        Track* next = current->next;
        // If there is no next track.
        if(next == NULL){
            // The merge is completed. Return.
            return;
        }

        // Find the root parent of the two tracks.
        Track* current_root_parent = current;
        while(current_root_parent->parent != NULL){
            current_root_parent = current_root_parent->parent;
        }
        Track* next_root_parent = next;
        while(next_root_parent->parent != NULL){
            next_root_parent = next_root_parent->parent;
        }

        // Check if they are neighbor
        if(current_root_parent->next != next_root_parent){
            // If not, these two track can not be merged.
            current = current->next;
            continue;
        }  

        // If the root is neighbor.

        // Check if all children of the two roots are neighbor.
        if(check_neighbor(current_root_parent, next_root_parent)){
            // Now, these two roots are neighbor and share same children.
            // And all their children are neighbors.
            // And all their neibor children has the same property.
            // We can merge them!
            merge_track(current_root_parent, next_root_parent);
            continue;
        }

        // If not, the merge fails.
        current = current->next;
        continue;
    }

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

    Track* copy = copy_list(src_start, src_end);
    
    // If the despos is 0, insert before the first track.
    if(destpos == 0){
        // Create a new track. Copy the first track.
        Track* new_track = tr_copy(dest_track);
        // Also transfer the relationship to new track.
        if(dest_track->parent != NULL){
            add_child(dest_track->parent, new_track);
            remove_child(dest_track->parent, dest_track);
        }
        // Change children's parent.
        for(int i = 0; i < new_track->num_children; i++){
            new_track->children[i]->parent = new_track;
        }
        // Remove old children.
        if(dest_track->num_children != 0){
            free(dest_track->children);
            dest_track->children = NULL;
            dest_track->num_children = 0;
        }
        // Copy the copy track to the head track.
        memcpy(dest_track, copy, sizeof(Track));
        // Change the children of parent of copy to the head track.
        if(copy->parent != NULL){
            add_child(copy->parent, dest_track);
            remove_child(copy->parent, copy);
        }
        // Also transfer the children of the copy to the head track.
        if(dest_track->num_children != 0){
            dest_track->children = malloc(sizeof(Track*)*dest_track->num_children);
            memcpy(dest_track->children, copy->children, sizeof(Track*)*dest_track->num_children);
            // Propagate to all children.
            for(int i = 0; i < copy->num_children; i ++){
                copy->children[i]->parent = dest_track;
            }
            // Remove the children of copy.
            free(copy->children);
            copy->num_children = 0;
            copy->children = NULL;
        }
        free(copy);
        // Find the last track of the copy list.
        Track* last = dest_track;
        while(last->next != NULL){
            last = last->next;
        }
        // Update the link.
        last->next = new_track;
    }else{  // Else, insert after the the dest_before.
        Track* dest_before = dest_track;
        temp = dest_before->trackLen;
        while(temp < destpos){
            dest_before = dest_before->next;
            temp += dest_before->trackLen;
        }
        
        Track* last = copy;
        while(last->next != NULL){
            last = last->next;
        }
        last->next = dest_before->next;
        dest_before->next = copy;
    }

    check_merge(dest_track);
    check_merge(src_track);

}


void print_data(Track* myTrack){
    int len = 0;
    // Count the number of datas.
    Track* temp = myTrack;
    while(temp != NULL){
        len += temp->trackLen;
        temp = temp->next;
    }
    printf("%d data:\n", len);
    temp = myTrack;

    // Print the data in every track.
    int track_num = 0;
    int sample_num = 0;
    while(myTrack != NULL){
        printf("track %d(%p), %d data: ", track_num, myTrack, myTrack->trackLen);
        printf("Parent: %p, Children:", myTrack->parent == NULL?NULL:myTrack->parent);
        for(int i = 0; i < myTrack->num_children; i++){
            printf(" %p", myTrack->children[i]);
        }
        printf("\n");
        for (int i = 0; i < myTrack->trackLen; i++)
        {
            printf("[%d][%d]%d ", i, sample_num, myTrack->data[i]);
            sample_num ++;
        }
        printf("\n");
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
    printf("\n");
}


Track* get_delete_start(Track* track, size_t pos, size_t len){
    // First find the start and end track to delete.
    Track* delete_start = track;
    
    while(pos >= delete_start->trackLen){
        pos -= delete_start->trackLen;
        delete_start = delete_start->next;
    }

    return delete_start;
}


Track* get_delete_end(Track* track, size_t pos, size_t len){
    Track* delete_end = track;
    
    while(pos >= delete_end->trackLen){
        pos -= delete_end->trackLen;
        delete_end = delete_end->next;
    }
    while(len > delete_end->trackLen - pos){
        len -= delete_end->trackLen - pos;
        delete_end = delete_end->next;
        pos -= pos;
    }

    return delete_end;
}


// Handle tracks link.
bool tr_delete_range(Track* track, size_t pos, size_t len){

    // First find the start and end track to delete.
    Track* delete_start = get_delete_start(track, pos, len);
    Track* delete_end = get_delete_end(track, pos, len);
    // Go through the delete list, find if any track is a parent.
    bool has_parent = false;
    Track* current = delete_start;
    while(current != delete_end->next){
        if(current->num_children != 0){
            has_parent = true;
            break;
        }
        current = current->next;
    }
    // If there is a parent to be deleted, abort the delete and return false.
    if(has_parent){
        printf("The list has a parent track, delete aborted.\n");
        return false;
    }

    // Now, the list is deletable.
    // Split the track.
    split_track(track, len+pos);
    split_track(track, pos);
    // Calculate the delete to start and end again.
    delete_start = get_delete_start(track, pos, len);
    delete_end = get_delete_end(track, pos, len);
    // Record the track before delete.
    Track* track_before_delete = delete_start == track?NULL:track;
    if(track_before_delete != NULL){
        while(track_before_delete->next != delete_start){
            track_before_delete = track_before_delete->next;
        }
    }
    // Record the track after the deletion.
    Track* track_after_delete = delete_end->next;
    // Now, the list from delete_start to delete_end is to be removed.
    // If the first track is to be removed.
    if(delete_start == track){
        // If there are remaining tracks after delete.
        if(track_after_delete != NULL){
            // Remove the tracks on the list except the first track.
            delete_start = delete_start->next;
            Track* current = delete_start;
            while(current != track_after_delete){
                if(current->parent != NULL){
                    remove_child(current->parent, current);
                }else{  // It has no parent, no child. Free the data.
                    free(current->data);
                }
                Track* temp = current;
                current = current->next;
                if(temp->num_children != 0){
                    free(temp->children);
                }
                free(temp);
            }
            // If the first track has no parent. It is a root track.
            if(track->parent == NULL){
                // Free the data in first track.
                free(track->data);
            }else{  // If the track has parent.
                // Remove the relationship.
                remove_child(track->parent, track);
            }
            // Copy the track after.
            memcpy(track, track_after_delete, sizeof(Track));
            // Also transfer the children of next track.
            if(track->num_children != 0){
                track->children = malloc(sizeof(Track*)*track->num_children);
                memcpy(track->children, track_after_delete->children, sizeof(Track*)*track->num_children);
            }
            // Remove self as child, in case there is a self insertion deleted.
            remove_child(track, track);
            remove_child(track_after_delete, track);
            // Update parent relationship.
            // If it has a parent, remove it from the parent's children list.
            // Add first track to the parent's children list.
            if(track_after_delete->parent != NULL){
                add_child(track_after_delete->parent, track);
                remove_child(track_after_delete->parent, track_after_delete);
            }
            // If it has any child.
            if(track_after_delete->num_children != 0){
                for(int i = 0; i < track_after_delete->num_children; i++){
                    // Set the first track as their parent.
                    if(track_after_delete->children[i] != track){
                        track_after_delete->children[i]->parent = track;
                    }
                    
                }
            }
            // Remove the track.
            if(track_after_delete->num_children != 0){
                free(track_after_delete->children);
            }
            free(track_after_delete);
        }else{  // If all tracks are to be deleted.
            // Remove all other tracks except the first track.
            delete_start = delete_start->next;
            Track* current = delete_start;
            while(current != NULL){
                if(current->parent != NULL){
                    remove_child(current->parent, current);
                }else{
                    free(current->data);
                }
                Track* temp = current;
                current = current->next;
                if(temp->num_children != 0){
                    free(temp->children);
                }
                free(temp);
            }
            // If the first track has a parent.
            if(track->parent != NULL){
                remove_child(track->parent, track);
            }else{
                free(track->data);
            }
            // Set the length of track to 0.
            track->trackLen = 0;
        }
    }else{  // The first track to delete is not the first track.
        // Remove all the tracks from start to end.
        current = delete_start;
        while(current != track_after_delete){
            if(current->parent != NULL){
                remove_child(current->parent, current);
            }else{
                free(current->data);
                current->data = NULL;
                current->trackLen = 0;
            }
            Track* temp = current;
            current = current->next;
            if(temp->num_children != 0){
                free(temp->children);
            }
            free(temp);
        }
        // Update the link.
        track_before_delete->next = track_after_delete;
    }


    // check_merge(track);

    return true;
}


// Given two integers, return a string composed of them: "<start>,<end>".
char* get_string(int start, int end){
    char buf1[INT_BUFFER_LEN];
    char buf2[INT_BUFFER_LEN];
    sprintf(buf1, "%d", start);
    sprintf(buf2, "%d", end);
    char* str = malloc(sizeof(char) * (strlen(buf1) + strlen(buf2) + 2));
    strcpy(str, buf1);
    strcpy(str+strlen(buf1), ",");
    strcpy(str+strlen(buf1) + 1, buf2);

    return str;
}


// Returns a string containing <start>,<end> ad pairs in target
char* tr_identify(struct sound_seg* target, struct sound_seg* ad){
    int start = 0;
    int end = 0;
    bool match = false;
    char* result;

    for(int i = 0; i < target->trackLen; i++){
        start = i;
        for(int j = 0; j < ad->trackLen; j++){
            end = i+j;
            printf("start: %d, end: %d.\n", start, end);
            // If one sample is not match, break.
            if(target->data[i+j] != ad->data[j]){
                printf("Not match this iteration.\n");
                end = 0;
                break;
            }
        }
        // If the match size end-start = ad->trackLen, indicating the whole match.
        if(end == start+ad->trackLen-1){
            printf("Find a match.\n");
            // printf("start: %d, end: %d.\n", start, end);
            match = true;
            break;
        }
    }

    if(match){
        result = get_string(start, end);
    }else{
        result = malloc(sizeof(char));
        strcpy(result, "\0");
    }

    return result;
}


void main(){ 


    return;
}