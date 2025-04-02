#include <string.h>
#include <stdlib.h>

#include "../include/helper.h"

// Given two integers, return a string composed of them: "<start>,<end>".
char* get_string(int start, int end){
    char buf1[11];
    char buf2[11];
    // sprintf(buf1, "%d", start);
    // sprintf(buf2, "%d", end);
    char* str = malloc(sizeof(char) * (strlen(buf1) + strlen(buf2) + 2));
    strcpy(str, buf1);
    strcpy(str+strlen(buf1), ",");
    strcpy(str+strlen(buf1) + 1, buf2);

    return str;
}
