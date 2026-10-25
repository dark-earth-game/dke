/*
 * levels.c — level number -> native LIEU script entry.
 */

#include "lieu_level.h"
#include "levels_table.h"

lieu_state_t *g_lvl_S;

lieu_entry_t lieu_level_entry(i32 level) {
    if (level < 0 || level >= (i32)(sizeof(k_level_entry) / sizeof(k_level_entry[0])))
        return NULL;
    return k_level_entry[level];
}
