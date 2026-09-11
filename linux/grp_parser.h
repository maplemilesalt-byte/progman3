#ifndef PROGMAN3_GRP_PARSER_H
#define PROGMAN3_GRP_PARSER_H

#include <stddef.h>

#define PM3_MAX_STRING 1024

typedef struct {
    char name[PM3_MAX_STRING];
    char command[PM3_MAX_STRING];
    char icon_path[PM3_MAX_STRING];
    int x;
    int y;
} PM3Item;

typedef struct {
    char name[PM3_MAX_STRING];
    PM3Item *items;
    size_t item_count;
} PM3Group;

int pm3_load_group(const char *path, PM3Group *group);
void pm3_free_group(PM3Group *group);

#endif
