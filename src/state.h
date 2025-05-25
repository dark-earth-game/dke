#pragma once

#include "libsys/common.h"
#include "libsys/system.h"
#include "libsys/graphics/graphic.h"
#include "libsys/audio/audio.h"

#include "game_detection.h"

typedef struct {
    u8 *ls_mem_buffer;
    u8 *ui_health_buffer;
    u8 *ui_inventory_slot_buffer;
    u8 *ui_weapon_slot_buffer;
    u8 *ui_special_meter_buffer;
    u8 *ui_status_effects_buffer;
    u8 *resource_buffer_1;
    u8 *resource_buffer_2;
    u8 *resource_buffer_3;
    u8 *resource_buffer_4;
    u8 *environment_overlay_buffer;
    u8 *particle_effect_buffer;
    u8 *weather_effect_buffer;
    u8 *lighting_effect_buffer;
    u8 *hit_effect_buffer;
    u8 *magic_effect_buffer;
    u8 *projectile_effect_buffer;
    u8 *shield_effect_buffer;
    u8 *special_attack_buffer;
    u8 *g_creature_data;
    u8 *max_effect_buffer_size;
    u8 *x_position;
    u8 *y_position;
    u8 *z_position;
    u8 *rotation;
    u8 *scale;
} game_t;

typedef struct {
    game_type_e game_type;
    b8 ls_debug_enabled;
    b8 debug_enabled;
    b8 is_demo;
    b8 use_key;
    b8 audio_enabled;
    b8 samples_enabled;
    b8 music_enabled;
    b8 input_processing_enabled;
    b8 debug_view_active;
    b8 lock_fps;
    c8 title[64];
    i32 language_id;
    // u8 language_list[11][3] = {
    //     "FR", "GB", "US", "DE", "IT", "ES", "JP", "PL", "RU", "CZ", "PT"
    // };
    i32 current_level_id;
    i32 need_change_level_id;
    u32 current_game_time;
    u32 timing_offset;
    u32 timing_multiplier;
    u32 time_backup;
    c8 _module_name[64];
    const c8 *game_path;
    c8 game_temp_path[256];
    u8 colour_mode; // 0 for 8-bit, 1 for 16-bit

    // system structures
    system_t system;
    graphic_t graphic;
    audio_t audio;
    game_t game;
} state_t;

extern state_t *state;

void state_init(state_t *state);
void state_release(state_t *state);
