#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>

/*
 * Data structures for the game engine
 */

typedef struct {
    void* data;
    size_t size;
} MemoryBuffer;

typedef struct {
    int width;        // Screen width (640 / 0x280)
    int height;       // Screen height (480 / 0x1E0)
    int color_depth;  // Color depth (8 or 16 bits)
    void* buffer;     // Rendering buffer
    int buffer_size;  // Size of rendering buffer
    void* color_tables; // At offset 0x2C in original struct
} GraphicsContext;

typedef struct {
    char name[32];           // Language identifier (e.g., "FR")
    char files_path[128];    // Path to language-specific files
} LanguageEntry;

/*
 * Global variables
 */
extern GraphicsContext* graphics_context;       // graphics_context
extern char game_path[256];                     // game_path
extern char game_temp_path[256];                // game_temp_path
extern int colour_mode_0_8bit_1_16bit;          // colour_mode_0_8bit_1_16bit
extern bool debug_enabled;                      // debug_enabled 
extern bool debug_view_active;                  // debug_view_active
extern bool is_demo;                            // is_demo
extern bool is_using_hdd;                       // is_using_hdd
extern bool audio_enabled;                      // audio_enabled
extern bool samples_enabled;                    // samples_enabled
extern bool music_enabled;                      // music_enabled
extern bool input_processing_enabled;           // input_processing_enabled
extern int game_exit_code;                      // game_exit_code
extern int language_id;                         // language_id
extern int current_level_id;                    // current_level_id
extern int need_change_level_id;                // need_change_level_id_4B7278
extern uint32_t current_game_time;              // current_game_time
extern uint32_t timing_offset;                  // timing_offset
extern uint32_t timing_multiplier;              // timing_multiplier
extern uint32_t time_backup;                    // time_backup
extern void* _ls_mem_buffer;                    // _ls_mem_buffer

// Memory buffers
extern MemoryBuffer ui_health_buffer;           // ui_health_buffer
extern MemoryBuffer ui_inventory_slot_buffer;   // ui_inventory_slot_buffer
extern MemoryBuffer ui_weapon_slot_buffer;      // ui_weapon_slot_buffer
extern MemoryBuffer ui_special_meter_buffer;    // ui_special_meter_buffer
extern MemoryBuffer ui_status_effects_buffer;   // ui_status_effects_buffer
extern MemoryBuffer environment_overlay_buffer; // environment_overlay_buffer
extern MemoryBuffer particle_effect_buffer;     // particle_effect_buffer
extern MemoryBuffer weather_effect_buffer;      // weather_effect_buffer
extern MemoryBuffer lighting_effect_buffer;     // lighting_effect_buffer
extern MemoryBuffer hit_effect_buffer;          // hit_effect_buffer
extern MemoryBuffer magic_effect_buffer;        // magic_effect_buffer
extern MemoryBuffer projectile_effect_buffer;   // projectile_effect_buffer
extern MemoryBuffer shield_effect_buffer;       // shield_effect_buffer
extern MemoryBuffer special_attack_buffer;      // special_attack_buffer
extern MemoryBuffer g_creature_data;            // g_creature_data

// Additional buffers that don't have descriptive names
extern MemoryBuffer resource_buffer_1;          // dword_5491A0
extern MemoryBuffer resource_buffer_2;          // dword_5491B0
extern MemoryBuffer resource_buffer_3;          // dword_5491B4
extern MemoryBuffer resource_buffer_4;          // dword_5491E4
extern MemoryBuffer max_effect_buffer_size;     // dword_4B8444

// Global data for various game systems
extern LanguageEntry language_list[];           // language_list
extern float x_position;                        // x_position

/*
 * Function declarations
 */
void load_configuration(void);
void lsi_set_window_title(const char* title);
void lsys_set_sound_format(int* audio_config, int flags);
GraphicsContext* graphics_init(int width, int height, int color_depth, int buffer_size, int param1, int param2);
void initialize_color_mapping_tables(void);
void ls_init_movie(int param1, int param2);
void set_position_coordinates(int x, int y);
void draw_text(const char* text);
void* ls_get_heap(int size);
void ls_release_heap(void* buffer);
void ls_max_memory_heap(void* buffer);
void system_check_memory(uint8_t* system_info, int* available_memory);
bool ls_is_path_exist(const char* path);
void game_set_game_path(const char* path);
bool select_language_version(const char* lang_check_file);
void game_play_intro(void);
void* image_load_compressed(const char* path, void* buffer);
void* load_image_color_data(const char* path, void* buffer);
void ls_clear_bmap(GraphicsContext* context);
void update_palette_with_default_colors(void* palette);
void draw_image(void* image, GraphicsContext* context, int x1, int y1, int width, int height, int x2, int y2);
void ls_delay(int milliseconds);
void ls_free_palette(void* palette);
void ls_free_bmap(void* bitmap);
bool ls_wait_key(void);
bool update_status_display(int param, float* position);
void config_load_dke_cfg(void);
void config_load_gamedke_cfg(void);
void game_init(void);
void scene_load_level(int level_id);
void ls_set_restore_callback(GraphicsContext* context, void* callback);
uint32_t system_get_gametime(void);
void set_display_mode(int mode);
bool process_mainmenu(void);
void update_game_timing_state(int param1, int param2);
void restore_display_mode(int param1, int param2);
void ls_update_event(void);
void process_game_frame(void);
void scene_release_level_resources(void);
void flush_display_buffer(void);
void game_release_resources(void);
void audio_release(void);
void system_release_all_opened_files(void);
void ls_shut_movie(void);
void finalize_graphics(void);
void ls_get_module_name(char* buffer);
uint32_t ls_read_timer(void);
void audio_create_buffer(void);

/**
 * Main game function that handles initialization, game loop, and cleanup for Dark Earth
 * Original name: ls_main
 *
 * @param cmd_args Command line arguments pointer
 * @param flag Command line flag value
 * @return int Result code (0 for success)
 */
int game_main(void* cmd_args, int flag)
{
    // Local variables for system checks and configuration
    char _module_name[64];        // byte_548C00
    uint8_t _system_info[8];      // var_20, var_1C, var_18
    int _available_memory;        // available_memory
    int _audio_config[3];         // audio_sample_rate, audio_channels, audio_bit_depth
    int _lang_index = 0;          // var_24
    void* _image_buffer = NULL;   // var_28
    void* _palette_buffer = NULL; // var_2C
    
    // Initialize the game
    load_configuration();
    
    // Set default game path
    strcpy(game_path, "HDISK:>EARTH>");
    
    // Check color mode from command line
    if (flag == 2) {
        colour_mode_0_8bit_1_16bit = 0;
        
        // Check for high color mode command line option
        if (strcmp(((char**)cmd_args)[1], "HIGH") == 0) {
            colour_mode_0_8bit_1_16bit = 1;
        }
        else if (strcmp(((char**)cmd_args)[1], "256") == 0) {
            colour_mode_0_8bit_1_16bit = 0;
        }
    }
    
    // Check available system memory
    system_check_memory(_system_info, &_available_memory);
    if (_available_memory < 0x1F00000) {
        colour_mode_0_8bit_1_16bit = 0; // Force 8-bit mode if low on memory
    }
    
    // Initialize window and system
    lsi_set_window_title("DarkEarth");
    
    // Set up audio if enabled
    if (audio_enabled) {
        _audio_config[0] = 0x5622; // Sample rate (22050)
        _audio_config[1] = 2;      // Channels (stereo)
        _audio_config[2] = 0x10;   // Bit depth (16-bit)
        lsys_set_sound_format(_audio_config, 0);
    }
    
    // Initialize graphics based on color mode
    if (colour_mode_0_8bit_1_16bit != 0) {
        // Initialize 16-bit graphics
        graphics_context = graphics_init(640, 480, 16, 0xA7D8C0, 8, 1);
        
        // Update color mode based on actual graphics context
        colour_mode_0_8bit_1_16bit = *(int*)((char*)graphics_context + 0x2C);
        
        initialize_color_mapping_tables();
        ls_init_movie(0, 0);
    } else {
        // Initialize 8-bit graphics
        graphics_context = graphics_init(640, 480, 8, 0xA7D8C0, 8, 1);
        ls_init_movie(0, 0);
    }
    
    // Show debug message if enabled
    if (debug_enabled) {
        set_position_coordinates(0, 0x20);
        draw_text("Init Libsys fini ...");
    }
    
    // Disable audio components if audio is disabled
    if (!audio_enabled) {
        samples_enabled = false;
        music_enabled = false;
    }
    
    // Debug memory initialization message
    if (debug_enabled) {
        set_position_coordinates(0, 0x20);
        draw_text("Init memory ...");
    }
    
    // Get module name and initialize timing
    ls_get_module_name(_module_name);
    timing_offset = ls_read_timer() * timing_multiplier;
    
    // Convert module name to lowercase
    for (int i = 0; _module_name[i]; i++) {
        if (_module_name[i] >= 'A' && _module_name[i] <= 'Z') {
            _module_name[i] += 32;
        }
    }
    
    // Allocate memory for game resources and buffers
    ui_health_buffer.data = ls_get_heap(0x1B7740);
    resource_buffer_3.data = ls_get_heap(0xF4240);
    ui_inventory_slot_buffer.data = ls_get_heap(0x4DDD0);
    ui_weapon_slot_buffer.data = ls_get_heap(0x26EE8);
    environment_overlay_buffer.data = ls_get_heap(0x67070);
    
    // Allocate effect buffers (all the same size)
    particle_effect_buffer.data = ls_get_heap(max_effect_buffer_size.size);
    weather_effect_buffer.data = ls_get_heap(max_effect_buffer_size.size);
    lighting_effect_buffer.data = ls_get_heap(max_effect_buffer_size.size);
    hit_effect_buffer.data = ls_get_heap(max_effect_buffer_size.size);
    magic_effect_buffer.data = ls_get_heap(max_effect_buffer_size.size);
    projectile_effect_buffer.data = ls_get_heap(max_effect_buffer_size.size);
    shield_effect_buffer.data = ls_get_heap(max_effect_buffer_size.size);
    special_attack_buffer.data = ls_get_heap(max_effect_buffer_size.size);
    
    // Update max effect buffer size based on particle buffer
    max_effect_buffer_size.size = ls_max_memory_heap(particle_effect_buffer.data);
    
    // Allocate remaining game resource buffers
    resource_buffer_1.data = ls_get_heap(0x4D3E0);
    resource_buffer_2.data = ls_get_heap(0x3D0900);
    g_creature_data.data = ls_get_heap(0x1CCA0);
    resource_buffer_4.data = ls_get_heap(0x65000);
    
    // Allocate UI special meter buffer (same for both color modes)
    ui_special_meter_buffer.data = ls_get_heap(0xF4240);
    
    // Allocate status effects buffer
    ui_status_effects_buffer.data = ls_get_heap(0x32);
    
    // Initialize audio systems
    audio_create_buffer();
    
    // Set game paths based on storage type
    if (!is_using_hdd) {
        game_set_game_path("REMOTE_DEV1:>dke");
    } else {
        char _drive_name[32];
        sprintf(game_path, "%s>", _drive_name);
        game_set_game_path("unk_4B73CC");
        
        // Check for data directory
        sprintf(game_temp_path, "%sDATAS", game_path);
        if (ls_is_path_exist(game_temp_path)) {
            strcpy(_module_name, game_path);
        }
    }
    
    // Play game intro sequence
    game_play_intro();
    
    // Set demo path if needed
    if (is_demo && is_using_hdd) {
        game_set_game_path("HDISK:>DKEDEMO>");
    }
    
    // Load language files for HDD mode
    if (is_using_hdd) {
        language_id = 2; // Default to language ID 2 (likely French)
        
        // Try to find valid language files
        for (_lang_index = 0; _lang_index < 7; _lang_index++) {
            sprintf(game_temp_path, "DIALOG>%s.CHK", language_list[_lang_index].name);
            
            if (select_language_version(game_temp_path)) {
                language_id = _lang_index;
                break;
            }
        }
    }
    
    // Demo-specific screens
    if (is_demo) {
        // Load and display demo splash screen
        _image_buffer = image_load_compressed("DATAS>MENUS>DEMO.GIF", _ls_mem_buffer);
        _palette_buffer = load_image_color_data("DATAS>MENUS>DEMO.GIF", _ls_mem_buffer);
        
        ls_clear_bmap(graphics_context);
        update_palette_with_default_colors(_palette_buffer);
        draw_image(_image_buffer, graphics_context, 0, 0, 640, 480, 0, 0);
        
        ls_delay(3000); // Wait 3 seconds
        
        ls_free_palette(_palette_buffer);
        ls_free_bmap(_image_buffer);
        ls_clear_bmap(graphics_context);
        
        // Load and display help screen for current language
        sprintf(game_temp_path, "DATAS>MENUS>HELP_%01d.GIF", language_id);
        _image_buffer = image_load_compressed(game_temp_path, _ls_mem_buffer);
        _palette_buffer = load_image_color_data(game_temp_path, _ls_mem_buffer);
        
        ls_clear_bmap(graphics_context);
        update_palette_with_default_colors(_palette_buffer);
        draw_image(_image_buffer, graphics_context, 0, 0, 640, 480, 0, 0);
        
        ls_wait_key(); // Wait for key press
        
        ls_free_palette(_palette_buffer);
        ls_free_bmap(_image_buffer);
        ls_clear_bmap(graphics_context);
    }
    
    // Update display status and input handling
    input_processing_enabled = update_status_display(1, &x_position);
    
    // Load animation file
    sprintf(game_temp_path, "%sANM>ANM.BAT", game_path);
    if (is_using_hdd || select_language_version(game_temp_path)) {
        // Set animation enabled flag
    }
    
    // Display configuration debug message
    if (debug_enabled) {
        set_position_coordinates(0, 0x20);
        draw_text("Init config ...");
    }
    
    // Load configuration files
    if (!is_using_hdd) {
        config_load_dke_cfg();
    }
    config_load_gamedke_cfg();
    
    // Initialize game systems
    game_init();
    
    // Load initial level
    current_level_id = /* level_id_4B70E4 */; // Initial level ID
    scene_load_level(current_level_id);
    
    // Set up graphics callback
    ls_set_restore_callback(graphics_context, /* callback_function */);
    
    // Initialize timing
    uint32_t _game_time = system_get_gametime();
    current_game_time = system_get_gametime();
    time_backup = current_game_time;
    
    // Set display mode
    set_display_mode(0x15);
    
    // Process main menu
    if (process_mainmenu()) {
        update_game_timing_state(1, 0x15);
    } else {
        restore_display_mode(1, 0x15);
    }
    
    // Main game loop
    while (game_exit_code != 1 && /* dword_4B7298 */ == 0) {
        // Process game frame
        ls_update_event();
        process_game_frame();
        
        // Check for level change
        if (need_change_level_id != -1) {
            scene_release_level_resources();
            
            // Debug message for level change
            if (debug_enabled) {
                sprintf(game_temp_path, "Nouvelle piece : %ld ", need_change_level_id);
                set_position_coordinates(0, 0);
                draw_text(game_temp_path);
            }
            
            // Load new level
            scene_load_level(need_change_level_id);
            need_change_level_id = -1;
        }
    }
    
    // Handle any pending audio (dword_4B729C)
    if (/* dword_4B729C */ != 0) {
        // Call audio handler function
        /* dword_4B729C = 0; */
    }
    
    // Cleanup and shutdown sequence
    flush_display_buffer();
    scene_release_level_resources();
    game_release_resources();
    audio_release();
    system_release_all_opened_files();
    ls_shut_movie();
    
    // Release all allocated memory buffers
    ls_release_heap(ui_status_effects_buffer.data);
    ls_release_heap(ui_special_meter_buffer.data);
    ls_release_heap(resource_buffer_4.data);
    ls_release_heap(g_creature_data.data);
    ls_release_heap(resource_buffer_2.data);
    ls_release_heap(resource_buffer_1.data);
    ls_release_heap(special_attack_buffer.data);
    ls_release_heap(shield_effect_buffer.data);
    ls_release_heap(projectile_effect_buffer.data);
    ls_release_heap(magic_effect_buffer.data);
    ls_release_heap(hit_effect_buffer.data);
    ls_release_heap(lighting_effect_buffer.data);
    ls_release_heap(weather_effect_buffer.data);
    ls_release_heap(particle_effect_buffer.data);
    ls_release_heap(environment_overlay_buffer.data);
    ls_release_heap(ui_weapon_slot_buffer.data);
    ls_release_heap(resource_buffer_3.data);
    ls_release_heap(ui_health_buffer.data);
    
    // Final debug message
    if (debug_view_active) {
        set_position_coordinates(0, 0x88);
        draw_text("................");
    }
    
    // Finalize graphics and exit
    finalize_graphics();
    
    return 0;
}

/**
 * Process a single frame of game logic
 * Original name: sub_461468
 */
void process_game_frame(void)
{
    // Game frame logic would be implemented here
}

/**
 * Finalize graphics system cleanup
 * Original name: sub_491E90
 */
void finalize_graphics(void)
{
    // Graphics cleanup implementation
}
