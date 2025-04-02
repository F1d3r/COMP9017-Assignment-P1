#include <string.h>
#include <math.h>
#include <stdio.h>

#include "../include/track.h"
#include "../include/list_op.h"
#include "../include/child_parent.h"

#define INT_BUFFER_LEN 20


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
            // printf("Freeing %d samples.\n", track->trackLen);
            // num_samples += track->trackLen;
            // printf("Total: %d\n", num_samples);
            free(track->data);
            track->data = NULL;
            track->trackLen = 0;
        }
    }
    // printf("Freeing track %p.\n", track);
    // num_tracks += 1;
    // printf("Total: %d\n", num_tracks);
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


void print_data(Track* myTrack, bool easy){
    int len = 0;
    // Count the number of datas.
    Track* temp = myTrack;
    while(temp != NULL){
        len += temp->trackLen;
        temp = temp->next;
    }
    if(!easy){
        printf("%d data:\n", len);
    }
    temp = myTrack;

    // Print the data in every track.
    int track_num = 0;
    int sample_num = 0;
    while(myTrack != NULL){
        if(!easy){
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
            myTrack = myTrack->next;
            track_num ++;
        }else{
            printf("track[%d], %d data:\n", track_num, myTrack->trackLen);
            for (int i = 0; i < myTrack->trackLen; i++)
            {
                printf("%d\n", myTrack->data[i]);
                sample_num ++;
            }
            myTrack = myTrack->next;
            track_num ++;
        }
    }

    // Print the root.
    if(!easy){
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

    }
    
    printf("\n");
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


    check_merge(track);

    return true;
}


// Given a target track and a advertisement sample, calculate the cross
// correlation. Compare to the autocorrelation of the ad. Find out if
// the portion of target is the ad.
// Returns a string containing <start>,<end> ad pairs in target
char* tr_identify(Track* target, Track* ad){
    // Initialize result.
    char* result = malloc(1);
    *result = '\0';

    char buff1[INT_BUFFER_LEN];
    char buff2[INT_BUFFER_LEN];

    // Convert target and ad to array.
    int16_t* target_data = NULL;
    int16_t* ad_data = NULL;
    // Calculate the number of samples in target and ad.
    int num_target_sample = 0;
    int num_ad_sample = 0;
    while(target != NULL){
        num_target_sample += target->trackLen;
        target_data = realloc(target_data, sizeof(int16_t)*num_target_sample);
        memcpy(target_data+(num_target_sample-target->trackLen), target->data, sizeof(int16_t)*target->trackLen);
        target = target->next;
    }
    while(ad != NULL){
        num_ad_sample += ad->trackLen;
        ad_data = realloc(ad_data, sizeof(int16_t)*num_ad_sample);
        memcpy(ad_data+(num_ad_sample-ad->trackLen), ad->data, sizeof(int16_t)*ad->trackLen);
        ad = ad->next;
    }
    if(num_target_sample < num_ad_sample){
        return result;
    }

    // Calculate autocorrelation.
    long auto_correlation = 0;
    for(int i = 0; i < num_ad_sample; i++){
        auto_correlation += pow(ad_data[i], 2);
    }

    bool match = false;
    // Now, from the start, calculate the cross correlation.
    for(int i = 0; i <= num_target_sample-num_ad_sample; i++){
        long cross_correlation = 0;
        // Calculate cross correlation for current position.
        for(int j = 0; j < num_ad_sample; j++){
            cross_correlation += target_data[i+j]*ad_data[j];
        }
        float ratio = (float)cross_correlation/(float)auto_correlation;
        // If the cross_correlation is 95%-105% to the autocorrelation.
        // printf("%d:\n", i);
        // printf("Ratio: %f\n", ratio);
        // printf("Auto: %ld\n", auto_correlation);
        // printf("Cross: %ld\n", cross_correlation);
        // printf("Ratio: %f\n", ratio);
        if(0.95f <= ratio && ratio <= 1.05f){
            // printf("Find a match.\n");
            // Convert the index to string.
            sprintf(buff1, "%d", i);
            sprintf(buff2, "%d", i+num_ad_sample-1);
            result = realloc(result, strlen(result)+strlen(buff1)+strlen(buff2)+3);
            strcpy(result+strlen(result), buff1);
            strcpy(result+strlen(result), ",");
            strcpy(result+strlen(result), buff2);
            strcpy(result+strlen(result), "\n");

            // Jump to next possible possition. Given the ad is not overlapping.
            i += num_ad_sample-1;
        }
    }
    // Free target and ad data.
    free(target_data);
    free(ad_data);

    // If the result is empty, return.
    if(strlen(result) == 0){
        return result;
    }

    // Remove the '\n' at the end if exists.
    if(result[strlen(result)-1] == '\n'){
        result[strlen(result)-1] = '\0';
    }

    return result;
}


void tr_resolve(Track** list, size_t list_len){

    // Go through all lists,
    // go through all tracks in list.
    // Find its parent.
    for(int i = 0; i < list_len; i++){
        Track* current = list[i];
        while(current != NULL){
            // If the track has no parent, continue to next.
            if(current->parent == NULL){
                current = current->next;
                continue;
            }
            // Else find its parent.
            for(int j = 0; j < list_len; j++){
                Track* temp = list[j];
                bool find_parent_in_current_list = false;
                while(temp != NULL){
                    // If find the parent of current track in the given array of lists.
                    if(current->parent == temp){
                        remove_child(temp, current);
                        // Duplicate the data.
                        current->data = malloc(sizeof(int16_t)*current->trackLen);
                        memcpy(current->data, temp->data, sizeof(int16_t)*current->trackLen);
                        find_parent_in_current_list = true;
                        break;
                    }
                    temp = temp->next;
                }
                if(find_parent_in_current_list){
                    break;
                }
            }

            current = current->next;
        }
    }


    // Then, merge tracks to save memory.
    for(int i = 0; i < list_len; i++){
        check_merge(list[i]);
    }

}