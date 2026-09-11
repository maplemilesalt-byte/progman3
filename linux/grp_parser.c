#include "grp_parser.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static uint16_t rd16(const unsigned char *p) {
    return (uint16_t)p[0] | ((uint16_t)p[1] << 8);
}

static uint32_t rd32(const unsigned char *p) {
    return (uint32_t)p[0] | ((uint32_t)p[1] << 8) |
           ((uint32_t)p[2] << 16) | ((uint32_t)p[3] << 24);
}

static int get_string(const unsigned char *data, size_t size, uint32_t off,
                      char *out, size_t out_size) {
    size_t i = 0;

    if (!out || out_size == 0 || off >= size)
        return -1;

    while ((size_t)off + i < size && i + 1 < out_size) {
        unsigned char c = data[off + i];
        if (c == 0) {
            out[i] = '\0';
            return 0;
        }
        out[i++] = (char)c;
    }

    out[i] = '\0';
    return ((size_t)off + i < size && data[off + i] == 0) ? 0 : -1;
}

void pm3_free_group(PM3Group *group) {
    if (!group)
        return;
    free(group->items);
    group->items = NULL;
    group->item_count = 0;
    group->name[0] = '\0';
}

int pm3_load_group(const char *path, PM3Group *group) {
    FILE *fp = NULL;
    unsigned char *data = NULL;
    long file_size;
    size_t size;
    uint32_t group_size;
    uint16_t count;
    size_t item_table;

    if (!path || !group)
        return -1;

    memset(group, 0, sizeof(*group));

    fp = fopen(path, "rb");
    if (!fp)
        return -2;

    if (fseek(fp, 0, SEEK_END) != 0) goto fail;
    file_size = ftell(fp);
    if (file_size < 0) goto fail;
    if (fseek(fp, 0, SEEK_SET) != 0) goto fail;

    size = (size_t)file_size;
    if (size < 44) goto fail;

    data = malloc(size);
    if (!data) goto fail;
    if (fread(data, 1, size, fp) != size) goto fail;
    fclose(fp);
    fp = NULL;

    /* GROUPDEF layout from the NT Program Manager .GRP format. */
    if (rd32(data + 0) != 0x504D4343u && rd32(data + 0) != 0x504D4344u)
        goto fail;

    group_size = rd32(data + 4);
    if (group_size > size || group_size < 44)
        goto fail;

    {
        uint32_t name_off = rd32(data + 20);
        if (get_string(data, group_size, name_off,
                       group->name, sizeof(group->name)) != 0)
            goto fail;
    }

    count = rd16(data + 36);
    item_table = 44;
    if (item_table + (size_t)count * 4 > group_size)
        goto fail;

    group->items = calloc(count ? count : 1, sizeof(*group->items));
    if (!group->items && count)
        goto fail;

    for (size_t i = 0; i < count; ++i) {
        uint32_t item_off = rd32(data + item_table + i * 4);
        PM3Item *item = &group->items[group->item_count];

        if (item_off == 0)
            continue;
        if ((size_t)item_off + 28 > group_size)
            goto fail;

        item->x = (int16_t)rd16(data + item_off + 0);
        item->y = (int16_t)rd16(data + item_off + 2);

        if (get_string(data, group_size, rd32(data + item_off + 20),
                       item->name, sizeof(item->name)) != 0)
            goto fail;
        if (get_string(data, group_size, rd32(data + item_off + 24),
                       item->command, sizeof(item->command)) != 0)
            goto fail;
        if (get_string(data, group_size, rd32(data + item_off + 28),
                       item->icon_path, sizeof(item->icon_path)) != 0)
            goto fail;

        group->item_count++;
    }

    free(data);
    return 0;

fail:
    if (fp) fclose(fp);
    free(data);
    pm3_free_group(group);
    return -3;
}
