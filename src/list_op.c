#include <stdio.h>
#include <string.h>

#include "../include/list_op.h"
#include "../include/track.h"
#include "../include/child_parent.h"


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
    new_track->track_len = track_to_split->track_len - split_pos;
    track_to_split->track_len = track_to_split->track_len - new_track->track_len;
    // Set the data pointer.
    new_track->data = malloc(sizeof(int16_t) * new_track->track_len);
    
    memcpy(new_track->data, track_to_split->data + split_pos, sizeof(int16_t)*new_track->track_len);
    
    
    track_to_split->data = realloc(track_to_split->data, sizeof(int16_t)*track_to_split->track_len);

    // Update the link.
    new_track->next = track_to_split->next;
    track_to_split->next = new_track;
    printf("Two new tracks: %p, %p.\n", track_to_split, new_track);
    printf("Lenght of two new tracks: %d, %d.\n", track_to_split->track_len, new_track->track_len);
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


// The valid pos range is [0, track.track_len]
// The element at pos will belongs to the right track afte split.
void split_track(Track* track, size_t pos){
    // First go find the track to be split.
    while(pos > track->track_len){
        pos -= track->track_len;
        track = track->next;
    }
    // Now the pos is at range [0, track.track_len]
    // If the track to split is not a parent track, split its parent.
    if(track->parent != NULL){
        split_track(track->parent, pos);
        return;
    }
    // Then the root parent is to be splited.
    // If the pos is the first or the one after the last.
    if(pos == 0 || pos == track->track_len){
        // Do not need to split.
        return;
    }
    // Now the pos is at range [1, track.track_len-1]
    Track* new_track = tr_init();
    // Update data length.
    new_track->track_len = track->track_len - pos;
    track->track_len = pos;
    // Update link
    new_track->next = track->next;
    track->next = new_track;
    // Copy data for the new track.
    new_track->data = malloc(sizeof(int16_t) * new_track->track_len);
    memcpy(new_track->data, track->data+pos, sizeof(int16_t)*new_track->track_len);
    // Reallocate the data for old track.
    track->data = realloc(track->data, sizeof(int16_t)*track->track_len);

    // Now the root tracks are done. Then propagate to all its children.
    split_children(track);

    return;
}



// Given two root tracks share same children, merge them and all children.
void merge_track(Track* prev, Track* next){
    // printf("Track %p and %p merged.\n", prev, next);
    // Copy content of next.
    if(prev->parent == NULL){   // If it is root, operate the data.
        prev->data = realloc(prev->data, sizeof(int16_t)*(prev->track_len+next->track_len));
        memcpy(prev->data+prev->track_len, next->data, sizeof(int16_t)*next->track_len);
    }else{
        // Else, just refer to the parent.
        prev->data = prev->parent->data;
    }
    prev->track_len += next->track_len;
    // Update link.
    prev->next = next->next;
    // Free next track.
    if(next->parent == NULL){ 
        // Handle the recursion. Free data only if the next is root.
        free(next->data);
        next->track_len = 0;
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
    return same_children;
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


Track* get_delete_start(Track* track, size_t pos, size_t len){
    // First find the start and end track to delete.
    Track* delete_start = track;
    
    while(pos >= delete_start->track_len){
        pos -= delete_start->track_len;
        delete_start = delete_start->next;
    }

    return delete_start;
}


Track* get_delete_end(Track* track, size_t pos, size_t len){
    Track* delete_end = track;
    
    while(pos >= delete_end->track_len){
        pos -= delete_end->track_len;
        delete_end = delete_end->next;
    }
    while(len > delete_end->track_len - pos){
        len -= delete_end->track_len - pos;
        delete_end = delete_end->next;
        pos -= pos;
    }

    return delete_end;
}
