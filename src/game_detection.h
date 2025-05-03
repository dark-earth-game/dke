#pragma once

#include "core/common.h"

#define FPS_LIMIT 60

#define DEFAULT_WIDTH 1280
#define DEFAULT_HEIGHT 800


typedef enum {
    // official releases
    DKE_FR = 0x00,
    DKE_UK = 0x01,
    DKE_US = 0x02,
    DKE_DE = 0x03,
    DKE_IT = 0x04,
    DKE_SP = 0x05,
    DKE_JP = 0x06,
    DKE_PL = 0x07,
    // community translations
    DKE_RU = 0x10,
    DKE_CZ = 0x11,
    DKE_PT = 0x12,
    // others
    DKE_DEMO = 0x100,
    DKE_UNKNOWN = 0x200,
} game_type_e;

game_type_e detect_game();

const c8* get_game_title(game_type_e game);
