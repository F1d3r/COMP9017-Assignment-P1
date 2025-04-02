#ifndef CHILD_PARENT_H
#define CHILD_PARENT_H

#include "struct.h"

// Parent and child shares the data, the data lenght.
void add_child(Track* parent, Track* child);


void remove_child(Track* parent, Track* child);

#endif