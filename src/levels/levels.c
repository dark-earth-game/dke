/*
 * levels.c — level number -> native LIEU script entry.
 */

#include "lieu_level.h"

lieu_state_t *g_lvl_S;

lieu_entry_t lieu_level_entry(i32 level) {
    switch (level) {
    case 1:  return lieu01_fonction_editor;
    case 2:  return lieu02_fonction_editor;
    default: return NULL;
    }
}
