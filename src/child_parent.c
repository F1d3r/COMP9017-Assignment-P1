#include <string.h>
#include <stdlib.h>

#include "../include/child_parent.h"

// Parent and child shares the data, the data lenght.
void add_child(Track* parent, Track* child){
    // If the child has already been child of the parent. Do not add again.
    for(int i = 0; i < parent->num_children; i++){
        if(child == parent->children[i]){
            // Unify the data and data lenght of the child to parent.
            child->parent = parent;
            child->data = parent->data;
            child->track_len = parent->track_len;
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
    child->track_len = parent->track_len;
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
