/*
 * levels.c — level number -> native LIEU script entry.
 */

#include "levels.h"

lieu_entry_t lieu_level_entry(i32 level) {
    switch (level) {
    case 1:  return lieu01_fonction_editor;
    default: return NULL;
    }
}
