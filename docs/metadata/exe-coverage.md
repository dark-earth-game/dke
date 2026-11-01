# dkev.exe coverage

Generated from the IDA 7.0 dump (see docs/ENGINE_GAPS.md). One row per function in the exe.

Status: **cited** = address or name appears in src/; PORTED / PARTIAL / MISSING / SKIP = classified by reading the pseudocode.

| addr | name | C lines | status | port | what / impact |
|---|---|---|---|---|---|
| 00401000 | calculate_entity_damage | 32 | cited |  |  |
| 00401129 | check_entity_special_abilities | 10 | cited |  |  |
| 00401194 | is_creature_in_special_level_range | 10 | cited |  |  |
| 004011FB | is_creature_special_level_type | 10 | cited |  |  |
| 0040126C | is_master_eligible_creature | 10 | cited |  |  |
| 004012D6 | consume_action_resource | 25 | cited |  |  |
| 00401401 | handle_entity_state_transition | 44 | cited |  |  |
| 0040152B | process_entity_damage | 161 | cited |  |  |
| 0040199A | process_combat_block | 50 | cited |  |  |
| 00401B8D | end_entity_invulnerability | 53 | cited |  |  |
| 00401DAD | process_entity_damage_attempt | 45 | cited |  |  |
| 00401F75 | is_point_in_view_cone | 25 | cited |  |  |
| 004020ED | calculate_attack_trajectory | 86 | cited |  |  |
| 00402414 | check_entity_revival | 12 | cited |  |  |
| 004024AF | render_entity_with_ui | 130 | cited |  |  |
| 00402877 | create_model_render_data | 213 | cited |  |  |
| 00402EFC | process_parry_effects | 64 | cited |  |  |
| 00403057 | render_parry_effects | 39 | cited |  |  |
| 004031B0 | register_game_image_buffers | 34 | cited |  |  |
| 004032A8 | allocate_memory | 31 | cited |  |  |
| 0040332F | free_memory | 17 | cited |  |  |
| 0040337A | create_image_buffer | 15 | cited |  |  |
| 004033DF | destroy_bitmap | 5 | SKIP |  | Thin wrapper over ls_free_bmap/ls_free_palette; original memory/bitmap manager replaced. |
| 004033F8 | initialize_resource | 11 | cited |  |  |
| 00403424 | release_resource_safe | 9 | SKIP |  | Thin wrapper over ls_free_bmap/ls_free_palette; original memory/bitmap manager replaced. |
| 00403450 | configure_game_palette | 34 | cited |  |  |
| 0040352F | apply_palette_transformation | 35 | cited |  |  |
| 004035D2 | graphics_triangle_rasteriser | 127 | SKIP |  | Legacy flat/textured triangle rasteriser, only reached from uncalled 3DF converter; port has own rasteriser in model.c. |
| 00403930 | graphics_trinagle_rasteriser_drawlines | 13 | SKIP |  | Debug rectangle outline helper; no callers. |
| 004039C3 | rectangle_editor | 97 | SKIP |  | In-game rectangle editor dev tool. |
| 00403E3B | capture_screen_region | 5 | SKIP |  | Empty stub. |
| 00403F50 | calculate_scale_factor | 18 | SKIP |  | Control-point scale pass whose result is discarded; no side effects. |
| 00403FD8 | process_node_hierarchy | 30 | SKIP |  | Control-point scale pass whose result is discarded; no side effects. |
| 004040B8 | process_scene_hierarchy_control_points | 237 | cited |  |  |
| 00404B03 | clean_node_hierarchy_control_points | 33 | SKIP | obc.c:obc free (clefs) | Frees per-node control-point arrays on entity destroy; port frees clefs with the node. |
| 00404BD4 | process_scene_hierarchy | 14 | SKIP |  | Control-point scale pass whose result is discarded; no side effects. |
| 00404C26 | process_scene_hierarchy_with_memory | 14 | cited |  |  |
| 00404C7E | cleanup_scene_hierarchy | 24 | SKIP | obc.c:obc free (clefs) | Frees per-node control-point arrays on entity destroy; port frees clefs with the node. |
| 00404CF4 | process_scene_root_node | 13 | cited |  |  |
| 00404D3D | clean_root_scene_hierarchy | 19 | SKIP | obc.c:obc free (clefs) | Frees per-node control-point arrays on entity destroy; port frees clefs with the node. |
| 00404D92 | create_single_control_point | 42 | cited |  |  |
| 00404F10 | cleanup_root_node_control_points | 24 | SKIP | obc.c:obc free (clefs) | Frees per-node control-point arrays on entity destroy; port frees clefs with the node. |
| 00404F80 | render_sprite_time_based_effect | 10 | cited |  |  |
| 00404FC9 | cleanup_renderer_instance | 14 | SKIP |  | Stops and frees a sound renderer instance; platform audio replaces it. |
| 00405003 | update_game_progression | 143 | cited |  |  |
| 00405641 | system_get_gametime | 8 | cited |  |  |
| 00405663 | graphics_display_error_message_2 | 49 | SKIP |  | MessageBox error dialog; no callers. |
| 004057E3 | update_status_display_with_throttling | 16 | SKIP |  | Throttled debug status display. |
| 00405828 | process_input_command | 45 | cited |  |  |
| 004059D0 | ensure_object_initialization | 10 | SKIP |  | Lock/unlock/init wrappers for bitmaps (ls_lock_bmap); not needed. |
| 004059F8 | ensure_object_deinitialization | 10 | SKIP |  | Lock/unlock/init wrappers for bitmaps (ls_lock_bmap); not needed. |
| 00405A20 | render_graphics_element_if_visible | 10 | SKIP |  | Lock/unlock/init wrappers for bitmaps (ls_lock_bmap); not needed. |
| 00405A46 | cleanup_graphics_element_if_hidden | 10 | SKIP |  | Lock/unlock/init wrappers for bitmaps (ls_lock_bmap); not needed. |
| 00405A6C | update_entity_animation_timings | 52 | cited |  |  |
| 00405C25 | log_message | 28 | cited |  |  |
| 00405D37 | read_file_data | 15 | SKIP |  | ls_open+read+close wrapper. |
| 00405D96 | init_font_system | 13 | cited |  |  |
| 00405DD2 | set_position_coordinates | 16 | cited |  |  |
| 00405E0A | display_text_message | 47 | SKIP |  | Debug text prompt waiting for ESC/Enter, used by dev tools. |
| 00405EDA | draw_text | 30 | cited |  |  |
| 00405FD1 | process_input_event | 69 | cited |  |  |
| 004062A9 | sub_4062A9 | 5 | SKIP |  | Thunks to ls_get/set_mouse; platform layer handles mouse. |
| 004062CE | sub_4062CE | 5 | SKIP |  | Thunks to ls_get/set_mouse; platform layer handles mouse. |
| 004062ED | sub_4062ED | 5 | SKIP |  | Thunks to ls_get/set_mouse; platform layer handles mouse. |
| 00406316 | sub_406316 | 16 | cited |  |  |
| 0040636D | sub_40636D | 29 | cited |  |  |
| 004063FE | set_display_mode | 38 | cited |  |  |
| 004064C3 | restore_display_mode | 114 | cited |  |  |
| 004067EA | update_game_timing_state | 108 | MISSING |  | Timer-stack pop after pause/menu: shifts entity, effect, object, world and level timers by the paused duration. — After pausing or closing a menu, timers jump by the elapsed real time; cooldowns, level timer and animations skip ahead. |
| 00406AC3 | set_entities_combat_mode | 31 | cited |  |  |
| 00406B51 | synchronize_entity_combat_modes | 44 | MISSING |  | Adaptive mesh LOD: lowers or raises entity+0x94 detail level (1..3) from measured frame rate. — No frame-rate-driven LOD change; entity LOD stays at 2. Minor visual only. |
| 00406C30 | synchronize_entity_combat_modes_up | 43 | MISSING |  | Adaptive mesh LOD: lowers or raises entity+0x94 detail level (1..3) from measured frame rate. — No frame-rate-driven LOD change; entity LOD stays at 2. Minor visual only. |
| 00406D09 | decode_string | 16 | SKIP |  | XOR string decode for the on-screen watermark. |
| 00406D74 | sub_406D74 | 125 | SKIP |  | Per-frame debug overlay, FPS counter, memory stats and watermark text. |
| 00407159 | process_input_command | 1222 | cited |  |  |
| 00409F2A | update_game_ui | 51 | SKIP |  | Rebuilds 16-bit UI background and HUD after a menu; port redraws every frame. |
| 0040A0C0 | create_palette_entry | 37 | SKIP |  | 8-bit palette closest-colour search for palette-index mode; port renders XRGB. |
| 0040A140 | find_closest_palette_colors | 62 | SKIP |  | 8-bit palette closest-colour search for palette-index mode; port renders XRGB. |
| 0040A280 | check_keyboard_letter | 13 | MISSING |  | P-key pause screen: random DATAS>GLOBAL image, key-sequence cheat codes (FORTYTWO easy mode, BIGHEAD/BIGFOOT/DWARF model overrides), PrintScreen PCX save. — No pause screen; P does nothing. Cheat/easy mode and model overrides unavailable. |
| 0040A2B0 | display_pause_screen | 289 | MISSING |  | P-key pause screen: random DATAS>GLOBAL image, key-sequence cheat codes (FORTYTWO easy mode, BIGHEAD/BIGFOOT/DWARF model overrides), PrintScreen PCX save. — No pause screen; P does nothing. Cheat/easy mode and model overrides unavailable. |
| 0040AA10 | system_get_gametime | 105 | cited |  |  |
| 0040ACC0 | render_game_background | 36 | MISSING |  | Gamma picker: brightness, contrast and saturation sliders with mouse, writes DKEGAME.CFG values. — First-run gamma picker absent; brightness/contrast/saturation are stored but never editable or applied. |
| 0040ADE0 | handle_screen_gamma_picker | 196 | MISSING |  | Gamma picker: brightness, contrast and saturation sliders with mouse, writes DKEGAME.CFG values. — First-run gamma picker absent; brightness/contrast/saturation are stored but never editable or applied. |
| 0040B3F0 | lookup_color_mapping | 8 | cited |  |  |
| 0040B440 | transform_color_6bit | 7 | cited |  |  |
| 0040B490 | initialize_color_mapping_tables | 60 | cited |  |  |
| 0040B550 | apply_ui_theme | 282 | PARTIAL | libsys/graphics/colour_tables.c | Builds 16-bit colour LUTs from palette with envred/envgreen/envblue theme tint; port uses direct XRGB so tint is not applied. — Per-level environment colour tint not applied; values small, subtle colour difference. |
| 0040BB00 | render_loading_transition | 41 | cited |  |  |
| 0040BCA0 | blit_texture_with_colormap | 126 | SKIP |  | Circle-plot blit for 8-bit iris transition (render_loading_transition); port uses lieu_cb.c op 10 dissolve instead. |
| 0040C010 | apply_palette_transformation_2 | 69 | PARTIAL |  | 8-bit palette remap with theme tint offsets; tint not applied in port. — Same env tint gap; subtle. |
| 0040C160 | generate_color_lookup_table | 72 | SKIP |  | 8-bit lookup/shade table builder with no callers; dead. |
| 0040C2A0 | process_color_palette | 57 | cited |  |  |
| 0040C350 | process_entity_animation_states | 51 | SKIP |  | Debug-only node-flag setter (dword_4B70D8 overlay). |
| 0040C3C0 | initialize_health_hud | 31 | cited |  |  |
| 0040C430 | free_health_hud | 6 | SKIP |  | Frees HUD/health buffer; memory manager replaced. |
| 0040C440 | process_health_hud | 155 | SKIP |  | 8-bit lookup/shade table builder with no callers; dead. |
| 0040C840 | update_palette_with_default_colors | 59 | cited |  |  |
| 0040C920 | update_palette | 49 | MISSING |  | Apply brightness, contrast and saturation to palette entries via RGB<->HSI-like conversion. — Gamma/colour settings from DKEGAME.CFG have no effect on rendering. |
| 0040CA80 | convert_rgb_to_color_space | 44 | MISSING |  | Apply brightness, contrast and saturation to palette entries via RGB<->HSI-like conversion. — Gamma/colour settings from DKEGAME.CFG have no effect on rendering. |
| 0040CBF0 | process_color_values | 72 | MISSING |  | Apply brightness, contrast and saturation to palette entries via RGB<->HSI-like conversion. — Gamma/colour settings from DKEGAME.CFG have no effect on rendering. |
| 0040CE30 | display_weapons_inventory | 8 | cited |  |  |
| 0040CE5A | clear_weapons_inventory | 9 | SKIP |  | Release/clear inventory lists on level or game release; port manages inventory in inventory.c. |
| 0040CE85 | create_shell_casing_effect | 91 | cited |  |  |
| 0040D265 | register_player_creature | 83 | cited |  |  |
| 0040D440 | reset_player_inventory | 24 | SKIP |  | Release/clear inventory lists on level or game release; port manages inventory in inventory.c. |
| 0040D4B9 | register_player_creature | 103 | cited |  |  |
| 0040D979 | process_entity_interaction | 43 | cited |  |  |
| 0040DAF8 | calculate_entity_position | 18 | cited |  |  |
| 0040DBD6 | process_entity_direction | 575 | cited |  |  |
| 0040F54A | process_entity_transformations | 165 | cited |  |  |
| 0040FD49 | handle_data_l44_faux | 36 | cited |  |  |
| 0040FE80 | process_vertex_data | 103 | PORTED | effect.c:effect_load_faux | FAUX.INF path parser for level 44 blades. |
| 0041033B | calculate_interpolated_values | 19 | cited |  |  |
| 00410490 | get_distance_between_points | 123 | cited |  |  |
| 004109EA | get_distance_between_entities | 72 | cited |  |  |
| 00410D3F | is_entity_close_proximity | 25 | SKIP |  | No callers; dead helper. |
| 00410E1C | set_vector_angle | 11 | SKIP |  | No callers; dead helper. |
| 00410E8A | run_entity_callback | 16 | MISSING |  | Bone-chain leaf world position for flame projectile launch (process_counter_attack). — Part of unported enemy flame projectile. |
| 00410EEC | allocate_and_initialize_health_buffer | 10 | cited |  |  |
| 00410F49 | free_health_buffer | 9 | SKIP |  | Frees HUD/health buffer; memory manager replaced. |
| 00410F76 | process_counter_attack | 100 | cited |  |  |
| 00411333 | process_entity_direction_and_state | 169 | cited |  |  |
| 004119B7 | handle_input_and_execute_action | 13 | MISSING |  | Per-tick wrapper calling process_entity_direction_and_state (flame projectile flight) plus debug J-key counter-attack. — Enemy flame projectile never flies or hits (see combat_damage.c TODO). |
| 00411A00 | execute_counter_attack | 46 | cited |  |  |
| 00411B8E | update_weapon_state | 16 | cited |  |  |
| 00411C06 | process_entity_action | 5 | cited |  |  |
| 00411C27 | initialize_entity_positions | 28 | MISSING |  | Flame impact: 12 spark entities, 154-particle burst and its per-frame render (update_game_hud effect type 10). — Flame weapon impact has no sparks or particle burst; no damage to player (-100 HP). |
| 00411D09 | process_entity_random_transformations | 86 | MISSING |  | Flame impact: 12 spark entities, 154-particle burst and its per-frame render (update_game_hud effect type 10). — Flame weapon impact has no sparks or particle burst; no damage to player (-100 HP). |
| 00411FE8 | sub_411FE8 | 248 | MISSING |  | Flame impact: 12 spark entities, 154-particle burst and its per-frame render (update_game_hud effect type 10). — Flame weapon impact has no sparks or particle burst; no damage to player (-100 HP). |
| 00412979 | sprintf | 23 | cited |  |  |
| 00412A2A | sub_412979 | 24 | cited |  |  |
| 00412ACF | sub_412ACF | 24 | cited |  |  |
| 00412B73 | sub_412B73 | 9 | PORTED | object.c:place | Places object tree at script position (sub_475F7A + sub_412ACF). |
| 00412B94 | sub_412B94 | 117 | cited |  |  |
| 00412FB0 | sub_412FB0 | 21 | SKIP |  | 8-bit half-size capture preview copy and its free. |
| 004130AA | sub_4130AA | 12 | cited |  |  |
| 004130DB | sub_4130DB | 10 | SKIP |  | 8-bit half-size capture preview copy and its free. |
| 004130FB | sub_4130FB | 51 | cited |  |  |
| 004132F3 | sub_4132F3 | 17 | PORTED | anim2d.c:anim2d_release | Clears 2D backdrop animation state. |
| 0041333A | sub_41333A | 144 | cited |  |  |
| 00413857 | sub_413857 | 101 | cited |  |  |
| 00414170 | sub_414170 | 23 | cited |  |  |
| 00414211 | sub_414211 | 109 | SKIP |  | Dev tool that walks J:\DKE\ANM folders to convert animations. |
| 00414777 | sub_414777 | 69 | SKIP |  | Dev tool that walks J:\DKE\ANM folders to convert animations. |
| 00414A0A | sub_414A0A | 28 | cited |  |  |
| 00414AC2 | sub_414AC2 | 31 | cited |  |  |
| 00414B8C | sub_414B8C | 24 | cited |  |  |
| 00414C8A | sub_414C8A | 23 | cited |  |  |
| 00414DDE | sub_414DDE | 21 | cited |  |  |
| 00414E66 | sub_414E66 | 20 | cited |  |  |
| 00414F72 | sub_414F72 | 12 | cited |  |  |
| 00415021 | sub_415021 | 110 | cited |  |  |
| 004154A0 | sub_4154A0 | 12 | cited |  |  |
| 004154E1 | sub_4154E1 | 12 | cited |  |  |
| 0041553F | sub_41553F | 46 | PARTIAL | actor.c:actor_blend_pose | Matrix-based pose blend (flag 0x800) between clips; port uses per-angle lerp only. — Some clip transitions blend with slightly wrong in-between poses; cosmetic. |
| 0041560C | sub_41560C | 78 | PARTIAL | actor.c:actor_blend_pose | Matrix-based pose blend (flag 0x800) between clips; port uses per-angle lerp only. — Some clip transitions blend with slightly wrong in-between poses; cosmetic. |
| 0041599C | sub_41599C | 75 | PARTIAL | actor.c:actor_blend_pose | Matrix-based pose blend (flag 0x800) between clips; port uses per-angle lerp only. — Some clip transitions blend with slightly wrong in-between poses; cosmetic. |
| 00415C99 | sub_415C99 | 5 | PARTIAL | actor.c:actor_blend_pose | Matrix-based pose blend (flag 0x800) between clips; port uses per-angle lerp only. — Some clip transitions blend with slightly wrong in-between poses; cosmetic. |
| 00415CC0 | sub_415CC0 | 37 | cited |  |  |
| 00415D80 | sub_415D80 | 27 | PORTED | scene.c:scene_render (subtitle) | Restores screen strips around the subtitle box; port redraws the full frame. |
| 00415E40 | sub_415E40 | 47 | PORTED | scene.c:scene_render (subtitle) | Restores screen strips around the subtitle box; port redraws the full frame. |
| 00415FB0 | sub_415FB0 | 68 | cited |  |  |
| 004161B0 | sub_4161B0 | 5 | cited |  |  |
| 004161C0 | sub_4161C0 | 9 | SKIP |  | Clears or frees subtitle buffers. |
| 004161D0 | sub_4161D0 | 48 | cited |  |  |
| 004162F0 | sub_4162F0 | 11 | SKIP |  | Clears or frees subtitle buffers. |
| 00416320 | sub_416320 | 139 | PARTIAL | scene.c:scene_render (subtitle block) | Bitmap-glyph subtitle drawing in box 72..635 x 460; port word-wraps with engine font in its own bar. — Subtitle look and position differ from original; text still shown. |
| 004166A0 | sub_4166A0 | 109 | cited |  |  |
| 004168F0 | sub_4168F0 | 28 | PARTIAL | scene.c:scene_render (subtitle block) | Bitmap-glyph subtitle drawing in box 72..635 x 460; port word-wraps with engine font in its own bar. — Subtitle look and position differ from original; text still shown. |
| 00416950 | sub_416950 | 94 | PARTIAL | scene.c:scene_render (subtitle block) | Bitmap-glyph subtitle drawing in box 72..635 x 460; port word-wraps with engine font in its own bar. — Subtitle look and position differ from original; text still shown. |
| 00416AC0 | update_display_state | 49 | cited |  |  |
| 00416C10 | sub_416C10 | 79 | cited |  |  |
| 00416F70 | render_entity_bounds | 540 | cited |  |  |
| 00417E40 | sub_417E40 | 23 | PARTIAL | menu/persos.c:prs_fill_tri | Polygon clip and flat fill for entity shadows; ported only for the PERSOS screen. — In-game actor shadows are not drawn. |
| 00417E80 | sub_417E80 | 295 | cited |  |  |
| 00418300 | sub_418300 | 44 | PARTIAL | menu/persos.c:prs_fill_tri | Polygon clip and flat fill for entity shadows; ported only for the PERSOS screen. — In-game actor shadows are not drawn. |
| 00418470 | sub_418470 | 27 | PARTIAL | menu/persos.c:prs_fill_tri | Polygon clip and flat fill for entity shadows; ported only for the PERSOS screen. — In-game actor shadows are not drawn. |
| 004184C0 | sub_4184C0 | 140 | cited |  |  |
| 00418810 | sub_418810 | 19 | cited |  |  |
| 004188A3 | sub_4188A3 | 79 | cited |  |  |
| 00418BA3 | sub_418BA3 | 32 | SKIP |  | Debug pool-usage counters overlay. |
| 00418CC2 | sub_418CC2 | 31 | cited |  |  |
| 00418D7F | is_entity_in_animation | 20 | cited |  |  |
| 00418EE5 | sub_418EE5 | 62 | cited |  |  |
| 00419117 | sub_419117 | 5 | cited |  |  |
| 0041918C | sub_41918C | 44 | cited |  |  |
| 00419307 | prepare_entity_animation | 18 | cited |  |  |
| 004193CC | sub_4193CC | 201 | cited |  |  |
| 00419B2C | sub_419B2C | 129 | cited |  |  |
| 00419F86 | sub_419F86 | 64 | cited |  |  |
| 0041A194 | sub_41A194 | 235 | cited |  |  |
| 0041A8B2 | sub_41A8B2 | 37 | cited |  |  |
| 0041A9ED | process_entity_state_change | 33 | cited |  |  |
| 0041AD23 | sub_41AD23 | 48 | cited |  |  |
| 0041AED1 | sub_414F72 | 664 | cited |  |  |
| 0041CCB7 | sub_44411D | 339 | cited |  |  |
| 0041DE03 | get_node_type | 19 | cited |  |  |
| 0041DE86 | sub_41DE86 | 104 | cited |  |  |
| 0041E26B | get_animation_id | 383 | cited |  |  |
| 0041F52F | sub_41F52F | 23 | SKIP |  | Free-slot search and free of CAN position/message pools; port uses malloc. |
| 0041F605 | sub_41F605 | 22 | SKIP |  | Free-slot search and free of CAN position/message pools; port uses malloc. |
| 0041F6BA | sub_41F6BA | 39 | cited |  |  |
| 0041F7C8 | sub_41F7C8 | 50 | cited |  |  |
| 0041FA4A | sub_41FA4A | 14 | SKIP |  | Free-slot search and free of CAN position/message pools; port uses malloc. |
| 0041FAA2 | sub_41FAA2 | 13 | MISSING |  | Entity animation clock resync (set_display_mode / pause resume): shifts entity timers by elapsed time. — Entity clocks not shifted after pause or display-mode change. |
| 0041FAD1 | update_entity_animation | 16 | MISSING |  | Entity animation clock resync (set_display_mode / pause resume): shifts entity timers by elapsed time. — Entity clocks not shifted after pause or display-mode change. |
| 0041FB48 | sub_41FB48 | 50 | MISSING |  | Walks bone parent chain for world position of the two foot bones; used by unported sub_41AD23. — Walk/run restart foot ordering missing; minor animation foot-sync. |
| 0041FC77 | sub_41FC77 | 17 | cited |  |  |
| 0041FDC2 | sub_41FDC2 | 46 | cited |  |  |
| 0041FF00 | sub_41FF00 | 110 | PARTIAL | scene.c:scene_load_inf_pitch | Builds look-at rotation matrices for camera setup (sub_4204BF); port reads Q14 matrices from INF. — Only affects levels whose camera is built from the camera object; likely none. |
| 0042035A | sub_42035A | 17 | SKIP |  | Camera matrix/FOV build from position+direction; original caches result in INF, port reads cached params |
| 00420418 | sub_420418 | 19 | SKIP |  | Camera matrix/FOV build from position+direction; original caches result in INF, port reads cached params |
| 004204BF | sub_4204BF | 81 | SKIP |  | Camera matrix/FOV build from position+direction; original caches result in INF, port reads cached params |
| 004207FA | calculate_model_scale | 23 | cited |  |  |
| 004208DD | sub_4208DD | 16 | PORTED | scene.c:scene_actor_zval | calculate_model_scale: depth to 41-step Z table value |
| 00420990 | sub_420D80 | 167 | PARTIAL | model.c:model_render_world / scene.c | World model render pipeline pieces (project, clip, raster setup); ported with approximated shading — Shading/lighting approximated (clamped key-light band), not engine-exact |
| 00420CE0 | sub_420CE0 | 19 | PORTED | obc.c:fmat_rotate_translation | Rotate matrix translation by its rotation part |
| 00420D80 | sub_420D80 | 474 | cited |  |  |
| 00421960 | sub_421960 | 10 | cited |  |  |
| 004219A0 | sub_4219A0 | 6 | SKIP |  | Original memory/file/sample-cache management, debug logging or buffer release; replaced by platform/malloc/sfx layer |
| 004219B0 | sub_4219B0 | 44 | cited |  |  |
| 00421AA0 | sub_421AA0 | 245 | PARTIAL | model.c:model_render_world / scene.c | World model render pipeline pieces (project, clip, raster setup); ported with approximated shading — Shading/lighting approximated (clamped key-light band), not engine-exact |
| 00421EC0 | sub_421EC0 | 70 | PARTIAL | model.c:model_render_world / scene.c | World model render pipeline pieces (project, clip, raster setup); ported with approximated shading — Shading/lighting approximated (clamped key-light band), not engine-exact |
| 004220A0 | sub_4220A0 | 43 | PARTIAL | model.c:model_render_world / scene.c | World model render pipeline pieces (project, clip, raster setup); ported with approximated shading — Shading/lighting approximated (clamped key-light band), not engine-exact |
| 00422130 | sub_422130 | 27 | PARTIAL | model.c:model_render_world / scene.c | World model render pipeline pieces (project, clip, raster setup); ported with approximated shading — Shading/lighting approximated (clamped key-light band), not engine-exact |
| 00422180 | sub_422180 | 147 | PARTIAL | model.c:model_render_world / scene.c | World model render pipeline pieces (project, clip, raster setup); ported with approximated shading — Shading/lighting approximated (clamped key-light band), not engine-exact |
| 004223D0 | sub_4223D0 | 14 | SKIP |  | Dead code (no callers) or tiny memory-manager/tree-teardown helper replaced by free() |
| 00422420 | sub_422420 | 17 | SKIP |  | Dead code (no callers) or tiny memory-manager/tree-teardown helper replaced by free() |
| 00422550 | sub_422550 | 5 | SKIP |  | Dead code (no callers) or tiny memory-manager/tree-teardown helper replaced by free() |
| 0042257B | sub_42257B | 58 | PORTED | inventory.c:inv_text_rec (text loader) | Parse @N-indexed text file into string table |
| 0042278C | sub_42278C | 30 | SKIP |  | Dead code (no callers) or tiny memory-manager/tree-teardown helper replaced by free() |
| 0042282E | sub_42282E | 25 | PORTED | inventory.c:inv_text_rec (text loader) | Parse @N-indexed text file into string table |
| 004228E0 | sub_4228E0 | 113 | cited |  |  |
| 00422CA2 | sub_422CA2 | 76 | PORTED | model_ome.c / obc.c | OME/OBC struct record unpacker (switch by type) |
| 00422E31 | sub_422E31 | 123 | cited |  |  |
| 004232C3 | sub_4232C3 | 25 | cited |  |  |
| 004233B1 | sub_4233B1 | 15 | SKIP |  | Dead code (no callers) or tiny memory-manager/tree-teardown helper replaced by free() |
| 00423413 | sub_423413 | 6 | SKIP |  | Original memory/file/sample-cache management, debug logging or buffer release; replaced by platform/malloc/sfx layer |
| 00423429 | sub_423429 | 21 | SKIP |  | Dead code (no callers) or tiny memory-manager/tree-teardown helper replaced by free() |
| 0042349F | sub_42349F | 29 | SKIP |  | Dead code (no callers) or tiny memory-manager/tree-teardown helper replaced by free() |
| 0042358B | sub_42358B | 11 | SKIP |  | Dead code (no callers) or tiny memory-manager/tree-teardown helper replaced by free() |
| 004235B4 | sub_4235B4 | 14 | SKIP |  | Dead code (no callers) or tiny memory-manager/tree-teardown helper replaced by free() |
| 0042360D | sub_42360D | 21 | SKIP |  | Dead code (no callers) or tiny memory-manager/tree-teardown helper replaced by free() |
| 004236E0 | sub_4236E0 | 24 | PORTED | libsys/movie_decoder.c:crh_next_frame_ex | Movie video codec (bit reader, block decode); replaced by own CRH decoder |
| 00423730 | sub_423730 | 872 | PORTED | libsys/movie_decoder.c:crh_next_frame_ex | Movie video codec (bit reader, block decode); replaced by own CRH decoder |
| 00425D50 | sub_425D50 | 83 | cited |  |  |
| 00426066 | sub_426066 | 13 | cited |  |  |
| 004260CB | sub_4260CB | 16 | cited |  |  |
| 0042614C | sub_42614C | 73 | cited |  |  |
| 00426335 | sub_426335 | 11 | cited |  |  |
| 0042639A | sub_42639A | 92 | cited |  |  |
| 004268C9 | sub_4268C9 | 28 | cited |  |  |
| 00426A24 | sub_426A24 | 31 | cited |  |  |
| 00426B24 | sub_426B24 | 14 | cited |  |  |
| 00426BB5 | sub_426BB5 | 45 | cited |  |  |
| 00426D2C | sub_475BB6 | 36 | cited |  |  |
| 00426E41 | sub_426E41 | 48 | cited |  |  |
| 0042701B | sub_42701B | 73 | cited |  |  |
| 004272B0 | sub_4272B0 | 22 | cited |  |  |
| 0042735E | sub_42735E | 44 | PORTED | inventory.c:inventory_screen_open | Init 100 OBnn inventory slots before opening inventory |
| 004274C2 | sub_4274C2 | 32 | cited |  |  |
| 00427593 | sub_427593 | 40 | cited |  |  |
| 004276B6 | sub_4276B6 | 46 | cited |  |  |
| 00427816 | sub_427816 | 8 | cited |  |  |
| 0042784E | sub_42784E | 94 | cited |  |  |
| 00427B10 | sub_427B10 | 35 | SKIP |  | LoadLibrary/GetProcAddress wrapper of LIEU DLLs; replaced by statically linked level ports |
| 00427BE5 | sub_427BE5 | 16 | SKIP |  | LoadLibrary/GetProcAddress wrapper of LIEU DLLs; replaced by statically linked level ports |
| 00427C2B | sub_427C2B | 223 | PORTED | lieu_cb.c:lieu callback table | Fill host-callback table handed to level DLL |
| 00428517 | sub_428517 | 30 | SKIP |  | LoadLibrary/GetProcAddress wrapper of LIEU DLLs; replaced by statically linked level ports |
| 00428714 | sub_428714 | 209 | cited |  |  |
| 00428E60 | decompress_lzw_428E60 | 115 | cited |  |  |
| 00429199 | decompress_lzw_rect_429199 | 163 | PORTED | libsys/graphics/cif.c:cif_decompress_lzw | LZW rect decompressor; port decodes whole CIF image |
| 004295E0 | sub_4295E0 | 13 | cited |  |  |
| 00429695 | sub_429695 | 146 | cited |  |  |
| 00429D4E | check_entities_collision | 103 | cited |  |  |
| 0042A360 | draw_particle | 126 | PORTED | combat_damage.c (parry sparks) | draw_particle: shaded line into bitmap, parry spark rays |
| 0042A5E3 | sub_42A5E3 | 276 | MISSING |  | Low-detail ('multiclip' game_state_flags) actor path: screen bbox plus scaled 128x128 fade sprite for far actors — Only when level multiclip config nonzero; port always renders full 3D actor |
| 0042B0A6 | sub_42B0A6 | 173 | MISSING |  | Low-detail ('multiclip' game_state_flags) actor path: screen bbox plus scaled 128x128 fade sprite for far actors — Only when level multiclip config nonzero; port always renders full 3D actor |
| 0042B63D | sub_42B63D | 142 | MISSING |  | Low-detail ('multiclip' game_state_flags) actor path: screen bbox plus scaled 128x128 fade sprite for far actors — Only when level multiclip config nonzero; port always renders full 3D actor |
| 0042BC7C | sub_42BC7C | 34 | MISSING |  | Frame fade/crossfade between two buffers (64-step blend, palette-index darken) for world-tick transitions and dirty-rect blends — Script op 42 world step is TODO in lieu_cb; scripted fade-ins/crossfades absent |
| 0042BD44 | sub_42BD44 | 35 | MISSING |  | Frame fade/crossfade between two buffers (64-step blend, palette-index darken) for world-tick transitions and dirty-rect blends — Script op 42 world step is TODO in lieu_cb; scripted fade-ins/crossfades absent |
| 0042BE0E | sub_42BE0E | 412 | cited |  |  |
| 0042CDA6 | play_sound_at_position | 82 | cited |  |  |
| 0042D03F | sub_42D03F | 189 | MISSING |  | Per-frame 32-particle blood spray with gravity, z-tested vs depth buffer — No blood particle spray on hits (TODO in combat_damage.c) |
| 0042D81F | sub_42D81F | 134 | cited |  |  |
| 0042DDED | sub_42DDED | 68 | cited |  |  |
| 0042DF9E | sub_42DF9E | 12 | cited |  |  |
| 0042E013 | sub_42E013 | 11 | cited |  |  |
| 0042E060 | sub_42E060 | 24 | cited |  |  |
| 0042E0E0 | sub_42E0E0 | 231 | cited |  |  |
| 0042EA19 | sub_42EA19 | 132 | MISSING |  | Frame fade/crossfade between two buffers (64-step blend, palette-index darken) for world-tick transitions and dirty-rect blends — Script op 42 world step is TODO in lieu_cb; scripted fade-ins/crossfades absent |
| 0042EE82 | sub_42EE82 | 122 | MISSING |  | Frame fade/crossfade between two buffers (64-step blend, palette-index darken) for world-tick transitions and dirty-rect blends — Script op 42 world step is TODO in lieu_cb; scripted fade-ins/crossfades absent |
| 0042F31B | sub_42F31B | 41 | MISSING |  | Underwater/flash render modes: rising bubbles, wavy full-screen distortion, palette flicker — Levels with underwater config lack bubbles and wave distortion; render_mode_flag never set |
| 0042F476 | sub_42F476 | 26 | cited |  |  |
| 0042F565 | sub_42F565 | 94 | MISSING |  | Underwater/flash render modes: rising bubbles, wavy full-screen distortion, palette flicker — Levels with underwater config lack bubbles and wave distortion; render_mode_flag never set |
| 0042F88E | allocate_memory | 56 | cited |  |  |
| 0042FA6D | sub_42FA6D | 18 | SKIP |  | Original memory/file/sample-cache management, debug logging or buffer release; replaced by platform/malloc/sfx layer |
| 0042FADF | render_fullscreen_effect | 131 | MISSING |  | Underwater/flash render modes: rising bubbles, wavy full-screen distortion, palette flicker — Levels with underwater config lack bubbles and wave distortion; render_mode_flag never set |
| 00430081 | sub_430081 | 96 | MISSING |  | Underwater/flash render modes: rising bubbles, wavy full-screen distortion, palette flicker — Levels with underwater config lack bubbles and wave distortion; render_mode_flag never set |
| 0043040A | create_image_buffer | 44 | cited |  |  |
| 0043057C | sub_43057C | 14 | SKIP |  | Original memory/file/sample-cache management, debug logging or buffer release; replaced by platform/malloc/sfx layer |
| 004305EF | sub_4305EF | 154 | MISSING |  | Scaled transparent sprite blit for screen-space effect entity types 6-8 — Billboard/sprite effect entities (types 6-8) not drawn |
| 00430A30 | seb_get_memory | 29 | cited |  |  |
| 00430AD6 | init_my_memory | 8 | cited |  |  |
| 00430AF5 | seb_release_all | 16 | cited |  |  |
| 00430BCB | init_font | 56 | cited |  |  |
| 00430D06 | close_font | 10 | MISSING |  | Cinematic subtitle (SBT) font load, render and cleanup — Movie subtitles not shown |
| 00430D34 | read_line_file | 21 | cited |  |  |
| 00430E15 | read_and_init_sbt | 80 | cited |  |  |
| 004311BE | close_sbt | 21 | MISSING |  | Cinematic subtitle (SBT) font load, render and cleanup — Movie subtitles not shown |
| 00431216 | draw_sbt_32k | 23 | MISSING |  | Cinematic subtitle (SBT) font load, render and cleanup — Movie subtitles not shown |
| 00431298 | ls_extern_sbt | 101 | MISSING |  | Cinematic subtitle (SBT) font load, render and cleanup — Movie subtitles not shown |
| 004314A2 | clear_sbt_32k | 5 | MISSING |  | Cinematic subtitle (SBT) font load, render and cleanup — Movie subtitles not shown |
| 004314CB | my_read_timer | 33 | SKIP |  | Movie thread/timer/semaphore, video-mode switch and getter/setter glue replaced by platform layer |
| 00431595 | close_decompress | 13 | SKIP |  | Movie thread/timer/semaphore, video-mode switch and getter/setter glue replaced by platform layer |
| 004315C7 | read_entete | 54 | cited |  |  |
| 00431675 | init_decompress | 40 | cited |  |  |
| 0043176F | WaitForSingleObject | 99 | SKIP |  | Movie thread/timer/semaphore, video-mode switch and getter/setter glue replaced by platform layer |
| 00431A04 | StartAddress | 92 | SKIP |  | Movie thread/timer/semaphore, video-mode switch and getter/setter glue replaced by platform layer |
| 00431D22 | cine_clear_movie_background | 5 | cited |  |  |
| 00431D38 | cine_set_movie_correction_32k | 55 | MISSING |  | Movie colour correction (brightness/contrast/gamma) lookup tables — Cinematic colour correction not applied; minor visual difference |
| 00431E65 | ls_init_movie | 68 | cited |  |  |
| 0043211E | ls_shut_movie | 8 | cited |  |  |
| 0043213E | unused_sub_43213E | 38 | SKIP |  | Dead code (no callers) or tiny memory-manager/tree-teardown helper replaced by free() |
| 00432241 | ls_set_movie_on_2 | 23 | SKIP |  | Movie thread/timer/semaphore, video-mode switch and getter/setter glue replaced by platform layer |
| 004322C8 | ls_set_movie_on_3 | 19 | SKIP |  | Movie thread/timer/semaphore, video-mode switch and getter/setter glue replaced by platform layer |
| 0043231D | ls_set_movie_on | 41 | cited |  |  |
| 00432478 | ls_set_movie_off | 16 | SKIP |  | Movie thread/timer/semaphore, video-mode switch and getter/setter glue replaced by platform layer |
| 004324D0 | ls_movie_callback | 14 | SKIP |  | Movie thread/timer/semaphore, video-mode switch and getter/setter glue replaced by platform layer |
| 0043251E | cp_restore_screen | 5 | cited |  |  |
| 0043253A | ls_set_restore_callback | 77 | PARTIAL | libsys/movie.c:ls_play_movie | Real ls_play_movie (mislabelled): own CRH decoder; no fade, subtitles, correction, priority threads — Missing end fade, subtitles, colour correction |
| 00432742 | ls_is_movie_256_colours | 6 | SKIP |  | Movie thread/timer/semaphore, video-mode switch and getter/setter glue replaced by platform layer |
| 00432757 | ls_get_movie_size | 6 | SKIP |  | Movie thread/timer/semaphore, video-mode switch and getter/setter glue replaced by platform layer |
| 0043276C | ls_set_movie_size | 14 | SKIP |  | Movie thread/timer/semaphore, video-mode switch and getter/setter glue replaced by platform layer |
| 00432799 | ls_set_movie_subtitles | 10 | MISSING |  | Cinematic subtitle (SBT) font load, render and cleanup — Movie subtitles not shown |
| 004327B2 | ls_set_movie_correction | 30 | MISSING |  | Movie colour correction (brightness/contrast/gamma) lookup tables — Cinematic colour correction not applied; minor visual difference |
| 00432846 | unused_sub_432846 | 17 | SKIP |  | Dead code (no callers) or tiny memory-manager/tree-teardown helper replaced by free() |
| 004328E7 | cp_init | 17 | cited |  |  |
| 004329EE | cp_uninit | 10 | cited |  |  |
| 00432A3B | cine_modify_rgb | 55 | MISSING |  | Movie colour correction (brightness/contrast/gamma) lookup tables — Cinematic colour correction not applied; minor visual difference |
| 00432BE1 | cine_get_hsi_value | 40 | MISSING |  | Movie colour correction (brightness/contrast/gamma) lookup tables — Cinematic colour correction not applied; minor visual difference |
| 00432D7D | cine_get_rgb_value | 72 | MISSING |  | Movie colour correction (brightness/contrast/gamma) lookup tables — Cinematic colour correction not applied; minor visual difference |
| 004330DD | cp_play_fade | 115 | MISSING |  | cp_play_fade: fade out last movie frame after playback — Movies end without fade-out |
| 00433490 | sub_433490 | 76 | cited |  |  |
| 00433610 | sub_433610 | 21 | cited |  |  |
| 00433670 | sub_433670 | 250 | cited |  |  |
| 00434042 | sub_434042 | 24 | PORTED | menu/persos.c (cheap shadow) | Copy bone vertices for shadow triangle, flag clipped |
| 00434160 | sub_434160 | 37 | cited |  |  |
| 004342C7 | sub_4342C7 | 382 | cited |  |  |
| 0043503E | sub_43503E | 36 | cited |  |  |
| 00435560 | sub_435560 | 5 | SKIP |  | LoadLibrary/GetProcAddress wrapper of LIEU DLLs; replaced by statically linked level ports |
| 00435586 | sub_435586 | 9 | SKIP |  | LoadLibrary/GetProcAddress wrapper of LIEU DLLs; replaced by statically linked level ports |
| 004355AB | sub_4355AB | 5 | SKIP |  | LoadLibrary/GetProcAddress wrapper of LIEU DLLs; replaced by statically linked level ports |
| 004355E0 | sub_4355E0 | 50 | SKIP |  | Ambient colour-light UI theme tint, only in 16-bit colour mode; port is 8-bit |
| 004357CF | initialize_effect_position | 39 | cited |  |  |
| 004358D3 | sub_4358D3 | 26 | cited |  |  |
| 004359FF | apply_model_transform | 13 | cited |  |  |
| 00435A3C | set_entity_texture | 5 | PORTED | scene.c:scene_collect_tree | Thunks to sub_4358D3 / initialize_effect_position lighting |
| 00435A65 | sub_435A65 | 42 | cited |  |  |
| 00435B01 | sub_435B01 | 6 | PORTED | scene.c:scene_collect_tree | Thunks to sub_4358D3 / initialize_effect_position lighting |
| 00435B34 | sub_435B34 | 5 | PORTED | scene.c:scene_collect_tree | Thunks to sub_4358D3 / initialize_effect_position lighting |
| 00435B5D | sub_435B5D | 5 | PORTED | scene.c:scene_collect_tree | Thunks to sub_4358D3 / initialize_effect_position lighting |
| 004362E0 | sub_4362E0 | 105 | SKIP |  | Debug/editor entity-pose or free-camera text overlay |
| 00436651 | get_node_bounds | 127 | cited |  |  |
| 00436AB0 | sub_436AB0 | 42 | cited |  |  |
| 00436C56 | sub_436C56 | 152 | cited |  |  |
| 004372AF | sub_4372AF | 51 | SKIP |  | Original memory/file/sample-cache management, debug logging or buffer release; replaced by platform/malloc/sfx layer |
| 004373EF | sub_4373EF | 21 | cited |  |  |
| 004374B0 | sub_4374B0 | 36 | cited |  |  |
| 004375DF | sub_4375DF | 97 | cited |  |  |
| 004378EA | sub_4378EA | 27 | cited |  |  |
| 004379C4 | image_load_compressed | 126 | cited |  |  |
| 00437DDD | sprintf | 33 | cited |  |  |
| 00437EB7 | ls_pause_sound | 972 | cited |  |  |
| 0043A63D | sub_48DCA0 | 148 | cited |  |  |
| 0043AA96 | sub_43AA96 | 31 | PORTED | menu/menu.c redraw | Menu screen bitmap refresh blit |
| 0043AB20 | sub_43AB20 | 308 | cited |  |  |
| 0043B9ED | ls_clear_bmap | 695 | cited |  |  |
| 0043D5D8 | sub_43D5D8 | 22 | cited |  |  |
| 0043D690 | sub_43D690 | 66 | cited |  |  |
| 0043D8B3 | sub_43D8B3 | 75 | cited |  |  |
| 0043DB85 | sub_43DB85 | 30 | cited |  |  |
| 0043DCC1 | sub_43DCC1 | 51 | cited |  |  |
| 0043DF23 | sub_43DF23 | 19 | cited |  |  |
| 0043DF94 | sub_43DF94 | 27 | cited |  |  |
| 0043E038 | ls_read_timer | 560 | cited |  |  |
| 0043F57C | sub_43F57C | 78 | cited |  |  |
| 0043F7BD | sub_43F7BD | 37 | cited |  |  |
| 0043F8C7 | sub_43F8C7 | 501 | cited |  |  |
| 00440B92 | draw_image | 471 | cited |  |  |
| 00441D2F | sub_441D2F | 14 | cited |  |  |
| 00441DB9 | sub_441DB9 | 14 | cited |  |  |
| 00441E43 | sub_441E43 | 354 | cited |  |  |
| 00442B29 | image_load_compressed | 25 | cited |  |  |
| 00442C10 | register_image_buffer | 32 | cited |  |  |
| 00442D45 | sub_442D45 | 61 | SKIP |  | Original memory/file/sample-cache management, debug logging or buffer release; replaced by platform/malloc/sfx layer |
| 00442E59 | log_allocation | 63 | SKIP |  | Original memory/file/sample-cache management, debug logging or buffer release; replaced by platform/malloc/sfx layer |
| 00443037 | system_release_all_opened_filse_443037 | 21 | SKIP |  | Original memory/file/sample-cache management, debug logging or buffer release; replaced by platform/malloc/sfx layer |
| 004430B0 | sub_4430B0 | 24 | cited |  |  |
| 0044313F | sub_44313F | 31 | cited |  |  |
| 00443273 | sub_443273 | 20 | cited |  |  |
| 004432D7 | sub_4432D7 | 66 | PORTED | lieu_cb.c:cb_free_anim | FreeAnim script action (case 0x27) |
| 0044356F | sub_44356F | 44 | cited |  |  |
| 00443643 | sub_443643 | 45 | cited |  |  |
| 00443720 | sub_443720 | 207 | cited |  |  |
| 004440A6 | sub_4440A6 | 5 | cited |  |  |
| 0044411D | sub_44411D | 11 | cited |  |  |
| 00444170 | sub_444170 | 13 | SKIP |  | Debug/editor entity-pose or free-camera text overlay |
| 004441A5 | sub_4441A5 | 52 | cited |  |  |
| 004442EE | sub_4442EE | 88 | cited |  |  |
| 0044453E | sub_44453E | 139 | cited |  |  |
| 004449A5 | sub_4449A5 | 27 | cited |  |  |
| 00444A3E | sub_444A3E | 26 | cited |  |  |
| 00444AC0 | sub_444AC0 | 15 | cited |  |  |
| 00444B10 | sub_444B10 | 35 | cited |  |  |
| 00444BF7 | strcpy | 107 | cited |  |  |
| 00444FF5 | sub_444FF5 | 7 | SKIP |  | Debug/editor entity-pose or free-camera text overlay |
| 0044502D | sub_44502D | 50 | cited |  |  |
| 00445189 | sub_445189 | 46 | cited |  |  |
| 004452CD | sub_4452CD | 36 | cited |  |  |
| 004453E1 | sub_4453E1 | 41 | cited |  |  |
| 004454F2 | sub_4454F2 | 56 | cited |  |  |
| 004455EE | sub_4455EE | 39 | cited |  |  |
| 0044569A | sub_44569A | 35 | cited |  |  |
| 00445765 | sub_445765 | 42 | cited |  |  |
| 004458AD | sub_4458AD | 18 | cited |  |  |
| 00445919 | sub_445919 | 26 | cited |  |  |
| 0044599B | sub_44599B | 72 | cited |  |  |
| 00445C82 | sub_445C82 | 53 | cited |  |  |
| 00445E99 | sub_445E99 | 36 | cited |  |  |
| 00445FD5 | sub_445FD5 | 49 | cited |  |  |
| 00446159 | sub_446159 | 36 | cited |  |  |
| 00446227 | sub_446227 | 55 | cited |  |  |
| 0044637B | sub_44637B | 42 | cited |  |  |
| 0044648B | sub_44648B | 64 | cited |  |  |
| 0044660D | sub_44660D | 25 | cited |  |  |
| 00446685 | sub_446685 | 20 | cited |  |  |
| 004466E9 | sub_4466E9 | 25 | cited |  |  |
| 00446763 | sub_446763 | 27 | cited |  |  |
| 00446808 | sub_446808 | 34 | cited |  |  |
| 004468E1 | sub_4468E1 | 39 | cited |  |  |
| 004469BA | sub_4469BA | 14 | cited |  |  |
| 00446A05 | sub_446A05 | 64 | cited |  |  |
| 00446B54 | sub_446B54 | 26 | cited |  |  |
| 00446BE9 | sub_446BE9 | 33 | cited |  |  |
| 00446CBC | sub_446CBC | 38 | cited |  |  |
| 00446D84 | sub_446D84 | 29 | cited |  |  |
| 00446E19 | memset | 928 | cited |  |  |
| 00448F4D | sub_448F4D | 233 | cited |  |  |
| 00449589 | sub_449589 | 39 | cited |  |  |
| 0044968B | sub_44968B | 172 | cited |  |  |
| 00449BAD | sub_449BAD | 81 | cited |  |  |
| 00449E03 | sub_449E03 | 53 | cited |  |  |
| 00449F9F | sub_449F9F | 88 | cited |  |  |
| 0044A263 | sub_44A263 | 11 | cited |  |  |
| 0044A28C | sub_44A28C | 10 | cited |  |  |
| 0044A2B5 | sub_44A2B5 | 78 | cited |  |  |
| 0044A58B | sub_44A58B | 1054 | cited |  |  |
| 0044CDB8 | sub_44CDB8 | 64 | cited |  |  |
| 0044CF90 | sub_44CF90 | 11 | cited |  |  |
| 0044CFD3 | sub_44CFD3 | 11 | SKIP |  | Original memory/file/sample-cache management, debug logging or buffer release; replaced by platform/malloc/sfx layer |
| 0044CFF6 | sub_44CFF6 | 19 | cited |  |  |
| 0044D06F | sub_44D06F | 12 | cited |  |  |
| 0044D0B1 | sub_44D0B1 | 31 | SKIP |  | Original memory/file/sample-cache management, debug logging or buffer release; replaced by platform/malloc/sfx layer |
| 0044D1C5 | sub_44D1C5 | 21 | cited |  |  |
| 0044D23D | sub_44D23D | 5 | cited |  |  |
| 0044D488 | sub_44D488 | 47 | cited |  |  |
| 0044D5BA | sub_44D5BA | 29 | cited |  |  |
| 0044D68C | sub_44D68C | 11 | MISSING |  | Pause and resume all 50 sound channels around the pause screen (P key) — No pause screen or pause key in port |
| 0044D6C9 | sub_44D6C9 | 11 | MISSING |  | Pause and resume all 50 sound channels around the pause screen (P key) — No pause screen or pause key in port |
| 0044D706 | sub_44D706 | 14 | cited |  |  |
| 0044D74B | sub_44D74B | 23 | SKIP |  | Original memory/file/sample-cache management, debug logging or buffer release; replaced by platform/malloc/sfx layer |
| 0044D7C8 | sub_44D7C8 | 36 | cited |  |  |
| 0044D8EE | get_sound_id | 50 | cited |  |  |
| 0044DAE3 | sub_44DAE3 | 23 | PORTED | sfx.c (k_names / voice recycle) | Sound name to id lookup / free finished voice slots |
| 0044DC61 | play_entity_sound | 83 | cited |  |  |
| 0044DEC9 | sub_44DEC9 | 26 | cited |  |  |
| 0044E0F9 | sub_44E0F9 | 16 | SKIP |  | Original memory/file/sample-cache management, debug logging or buffer release; replaced by platform/malloc/sfx layer |
| 0044E13E | sub_44E13E | 54 | SKIP |  | Original memory/file/sample-cache management, debug logging or buffer release; replaced by platform/malloc/sfx layer |
| 0044E37A | sub_44E37A | 20 | PORTED | sfx.c (k_names / voice recycle) | Sound name to id lookup / free finished voice slots |
| 0044E46A | sub_44E46A | 18 | cited |  |  |
| 0044E4BB | sub_44E4BB | 20 | SKIP |  | Original memory/file/sample-cache management, debug logging or buffer release; replaced by platform/malloc/sfx layer |
| 0044E57B | sub_44E57B | 168 | cited |  |  |
| 0044EC90 | sub_44EC90 | 106 | PORTED | libsys/movie_decoder.c:crh_next_frame_ex | Movie video codec (bit reader, block decode); replaced by own CRH decoder |
| 0044EFE0 | sub_44EFE0 | 46 | PORTED | libsys/movie_decoder.c:crh_next_frame_ex | Movie video codec (bit reader, block decode); replaced by own CRH decoder |
| 0044F0A0 | sub_44F0A0 | 28 | PORTED | libsys/movie_decoder.c:crh_next_frame_ex | Movie video codec (bit reader, block decode); replaced by own CRH decoder |
| 0044F0F0 | cp_uncompress_image | 19 | PORTED | libsys/movie_decoder.c:crh_next_frame_ex | Movie video codec (bit reader, block decode); replaced by own CRH decoder |
| 0044F160 | cp_uncompress_composante | 95 | PORTED | libsys/movie_decoder.c:crh_next_frame_ex | Movie video codec (bit reader, block decode); replaced by own CRH decoder |
| 0044F2A0 | setup_creature | 35 | cited |  |  |
| 0044F454 | sub_44F454 | 39 | cited |  |  |
| 0044F58E | sub_44F58E | 13 | cited |  |  |
| 0044F605 | sub_44F605 | 25 | SKIP |  | Original memory/file/sample-cache management, debug logging or buffer release; replaced by platform/malloc/sfx layer |
| 0044F6E8 | sub_44F6E8 | 56 | cited |  |  |
| 0044F92D | sub_44F92D | 150 | cited |  |  |
| 0045018B | strcpy | 156 | cited |  |  |
| 00450AF9 | clear_primary_inventory | 27 | SKIP |  | Frees entity/inventory object trees and bitmaps via the original allocator; port needs no equivalent. |
| 00450BEB | sub_450BEB | 9 | cited |  |  |
| 00450C70 | sub_450C70 | 20 | cited |  |  |
| 00450CDD | sub_450CDD | 20 | cited |  |  |
| 00450D4A | get_entity_distance | 22 | cited |  |  |
| 00450DE8 | sub_450DE8 | 48 | cited |  |  |
| 00450FD3 | sub_450FD3 | 37 | cited |  |  |
| 0045113A | sub_45113A | 17 | cited |  |  |
| 004511D5 | sub_4511D5 | 22 | cited |  |  |
| 004512E4 | sub_4512E4 | 77 | cited |  |  |
| 004515FD | sub_4515FD | 242 | cited |  |  |
| 00451BF9 | sub_451BF9 | 374 | cited |  |  |
| 00452A60 | sub_452A60 | 26 | SKIP |  | MMX 16bpp triangle test path gated by dword_4B70B0, which is never set (dead code). |
| 00452B20 | sub_452B20 | 107 | cited |  |  |
| 00452EA6 | sub_452EA6 | 95 | cited |  |  |
| 004531D6 | sub_4531D6 | 31 | cited |  |  |
| 004532C7 | sub_4532C7 | 16 | cited |  |  |
| 00453346 | sub_453346 | 152 | cited |  |  |
| 004538FB | sub_4538FB | 101 | cited |  |  |
| 00453C99 | clear_secondary_inventory | 23 | SKIP |  | Frees entity/inventory object trees and bitmaps via the original allocator; port needs no equivalent. |
| 00453CED | sub_453CED | 15 | SKIP |  | Frees entity/inventory object trees and bitmaps via the original allocator; port needs no equivalent. |
| 00453D4F | sub_453D4F | 6 | SKIP |  | Frees entity/inventory object trees and bitmaps via the original allocator; port needs no equivalent. |
| 00453D65 | sub_453D65 | 21 | SKIP |  | Frees entity/inventory object trees and bitmaps via the original allocator; port needs no equivalent. |
| 00453DDB | sub_453DDB | 29 | SKIP |  | Frees entity/inventory object trees and bitmaps via the original allocator; port needs no equivalent. |
| 00453EC7 | sub_453EC7 | 11 | SKIP |  | Frees entity/inventory object trees and bitmaps via the original allocator; port needs no equivalent. |
| 00453EF0 | sub_453EF0 | 14 | SKIP |  | Frees entity/inventory object trees and bitmaps via the original allocator; port needs no equivalent. |
| 00453F49 | sub_453F49 | 21 | SKIP |  | Frees entity/inventory object trees and bitmaps via the original allocator; port needs no equivalent. |
| 00453FB0 | sub_453FB0 | 47 | SKIP |  | Swaps pixels of two same-size bitmaps; helper for the in-game menu screen composition. |
| 004540AD | sub_4540AD | 64 | MISSING |  | 3x3 edge smoothing blend of 16bpp bitmaps (entity edge anti-alias); only reached via game-context vtable slot 288. — Entity silhouettes lack the original edge smoothing; cosmetic. |
| 004542FD | sub_4542FD | 53 | MISSING |  | 3x3 edge smoothing blend of 16bpp bitmaps (entity edge anti-alias); only reached via game-context vtable slot 288. — Entity silhouettes lack the original edge smoothing; cosmetic. |
| 004544F0 | draw_image | 23 | cited |  |  |
| 0045459A | overlay_cursor_image | 23 | cited |  |  |
| 00454644 | sub_454644 | 5 | SKIP |  | Thunks to ls_fill_rect / ls_clear_bmap for the game-context vtable. |
| 00454675 | sub_454675 | 5 | SKIP |  | Thunks to ls_fill_rect / ls_clear_bmap for the game-context vtable. |
| 00454690 | sub_454690 | 63 | MISSING |  | CJR big-animation player: open/close .CJR+.OFF, per-frame chunk decoders, bounds, bonus-mode player drop motion. — CJR big animations never play; LIEU op 17 is a TODO; effect type 12 draws nothing. |
| 00454810 | sub_454810 | 146 | MISSING |  | CJR big-animation player: open/close .CJR+.OFF, per-frame chunk decoders, bounds, bonus-mode player drop motion. — CJR big animations never play; LIEU op 17 is a TODO; effect type 12 draws nothing. |
| 00454C30 | sub_454C30 | 27 | MISSING |  | CJR big-animation player: open/close .CJR+.OFF, per-frame chunk decoders, bounds, bonus-mode player drop motion. — CJR big animations never play; LIEU op 17 is a TODO; effect type 12 draws nothing. |
| 00454CB0 | sub_454CB0 | 77 | MISSING |  | CJR big-animation player: open/close .CJR+.OFF, per-frame chunk decoders, bounds, bonus-mode player drop motion. — CJR big animations never play; LIEU op 17 is a TODO; effect type 12 draws nothing. |
| 00454E00 | sub_454E00 | 55 | MISSING |  | CJR big-animation player: open/close .CJR+.OFF, per-frame chunk decoders, bounds, bonus-mode player drop motion. — CJR big animations never play; LIEU op 17 is a TODO; effect type 12 draws nothing. |
| 004552A0 | sub_4552A0 | 77 | MISSING |  | CJR big-animation player: open/close .CJR+.OFF, per-frame chunk decoders, bounds, bonus-mode player drop motion. — CJR big animations never play; LIEU op 17 is a TODO; effect type 12 draws nothing. |
| 004553B0 | sub_4553B0 | 177 | MISSING |  | CJR big-animation player: open/close .CJR+.OFF, per-frame chunk decoders, bounds, bonus-mode player drop motion. — CJR big animations never play; LIEU op 17 is a TODO; effect type 12 draws nothing. |
| 004555A0 | sub_4555A0 | 73 | MISSING |  | CJR big-animation player: open/close .CJR+.OFF, per-frame chunk decoders, bounds, bonus-mode player drop motion. — CJR big animations never play; LIEU op 17 is a TODO; effect type 12 draws nothing. |
| 004556A0 | sub_4556A0 | 94 | MISSING |  | CJR big-animation player: open/close .CJR+.OFF, per-frame chunk decoders, bounds, bonus-mode player drop motion. — CJR big animations never play; LIEU op 17 is a TODO; effect type 12 draws nothing. |
| 004557A0 | cine_clear_movie_background__ | 5 | MISSING |  | CJR big-animation player: open/close .CJR+.OFF, per-frame chunk decoders, bounds, bonus-mode player drop motion. — CJR big animations never play; LIEU op 17 is a TODO; effect type 12 draws nothing. |
| 004557B0 | sub_4557B0 | 73 | MISSING |  | CJR big-animation player: open/close .CJR+.OFF, per-frame chunk decoders, bounds, bonus-mode player drop motion. — CJR big animations never play; LIEU op 17 is a TODO; effect type 12 draws nothing. |
| 00455840 | sub_455840 | 23 | MISSING |  | CJR big-animation player: open/close .CJR+.OFF, per-frame chunk decoders, bounds, bonus-mode player drop motion. — CJR big animations never play; LIEU op 17 is a TODO; effect type 12 draws nothing. |
| 00455890 | sub_455890 | 9 | MISSING |  | CJR big-animation player: open/close .CJR+.OFF, per-frame chunk decoders, bounds, bonus-mode player drop motion. — CJR big animations never play; LIEU op 17 is a TODO; effect type 12 draws nothing. |
| 004558A0 | sub_4558A0 | 81 | cited |  |  |
| 00455B65 | sub_455B65 | 54 | cited |  |  |
| 00455E53 | sub_455E53 | 88 | PORTED | scene.c:scene_draw_zone_overlay | Debug overlay drawing SOL zone outlines with clipped projected lines. |
| 0045619D | sub_45619D | 306 | cited |  |  |
| 00456E6A | sub_456E6A | 61 | SKIP |  | Debug draw of the 64x64 nav grid, invoked from process_menu debug key. |
| 00456FB7 | sub_456FB7 | 66 | cited |  |  |
| 0045720D | sub_45720D | 48 | cited |  |  |
| 00457345 | generate_effect | 55 | cited |  |  |
| 00457565 | sub_457565 | 15 | SKIP | world.c:world_load_level_data | 3DS ASCII (.ASC) camera/zone parser building SOL limits; fallback only when cached .INF is absent. Port reads .INF. |
| 004575D3 | sub_4575D3 | 23 | SKIP | world.c:world_load_level_data | 3DS ASCII (.ASC) camera/zone parser building SOL limits; fallback only when cached .INF is absent. Port reads .INF. |
| 0045766F | sub_45766F | 40 | SKIP | world.c:world_load_level_data | 3DS ASCII (.ASC) camera/zone parser building SOL limits; fallback only when cached .INF is absent. Port reads .INF. |
| 0045775C | sub_45775C | 15 | SKIP | world.c:world_load_level_data | 3DS ASCII (.ASC) camera/zone parser building SOL limits; fallback only when cached .INF is absent. Port reads .INF. |
| 004577AB | sub_4577AB | 5 | SKIP | world.c:world_load_level_data | 3DS ASCII (.ASC) camera/zone parser building SOL limits; fallback only when cached .INF is absent. Port reads .INF. |
| 004577C9 | sub_4577C9 | 13 | SKIP | world.c:world_load_level_data | 3DS ASCII (.ASC) camera/zone parser building SOL limits; fallback only when cached .INF is absent. Port reads .INF. |
| 0045780C | sub_45780C | 13 | SKIP | world.c:world_load_level_data | 3DS ASCII (.ASC) camera/zone parser building SOL limits; fallback only when cached .INF is absent. Port reads .INF. |
| 0045783C | sub_45783C | 147 | SKIP | world.c:world_load_level_data | 3DS ASCII (.ASC) camera/zone parser building SOL limits; fallback only when cached .INF is absent. Port reads .INF. |
| 00457CB4 | sub_457CB4 | 53 | cited |  |  |
| 00457DD0 | sub_457DD0 | 214 | PARTIAL | scene.c:scene_load_dat_config | Parses level .DAT: cameras, light, clipz, colorlight, flash, env, multiclip; port reads only ncamera/ambient/zbuffer/light. — Clip-Z person/wave, coloured lights, flashes and env fades absent. |
| 00458A57 | finalize_render_data | 42 | SKIP | world.c:world_load_level_data | 3DS ASCII (.ASC) camera/zone parser building SOL limits; fallback only when cached .INF is absent. Port reads .INF. |
| 00458C52 | sub_458C52 | 61 | SKIP | world.c:world_load_level_data | 3DS ASCII (.ASC) camera/zone parser building SOL limits; fallback only when cached .INF is absent. Port reads .INF. |
| 00458F3D | sub_458F3D | 6 | SKIP | world.c:world_load_level_data | 3DS ASCII (.ASC) camera/zone parser building SOL limits; fallback only when cached .INF is absent. Port reads .INF. |
| 00458F60 | sub_458F60 | 43 | cited |  |  |
| 004590C7 | sub_4590C7 | 83 | cited |  |  |
| 004593F7 | sub_4593F7 | 115 | cited |  |  |
| 004599A2 | sub_4599A2 | 88 | cited |  |  |
| 00459CD2 | sub_459CD2 | 48 | PORTED | actor_move.c:sol_inside + nav_grid.c:classify_point | Point-in-collision-zone test over SOL zone list. |
| 00459E80 | sub_459E80 | 216 | cited |  |  |
| 0045A67C | sub_45A67C | 38 | cited |  |  |
| 0045A7EF | sub_45A7EF | 42 | PORTED | scene.c:scene_draw_zone_overlay | Debug overlay drawing SOL zone outlines with clipped projected lines. |
| 0045A976 | sub_45A976 | 15 | PORTED | scene.c:scene_draw_zone_overlay | Debug overlay drawing SOL zone outlines with clipped projected lines. |
| 0045AA40 | sub_45AA40 | 104 | PORTED | scene.c:scene_draw_zone_overlay | Debug overlay drawing SOL zone outlines with clipped projected lines. |
| 0045AC76 | sub_45AC76 | 8 | PORTED | scene.c:scene_draw_zone_overlay | Debug overlay drawing SOL zone outlines with clipped projected lines. |
| 0045ACC5 | sub_45ACC5 | 46 | cited |  |  |
| 0045AE70 | config_load_gamedke_cfg_45AE70 | 58 | cited |  |  |
| 0045B089 | flush_display_buffer | 45 | cited |  |  |
| 0045B207 | sub_45B207 | 38 | cited |  |  |
| 0045B361 | config_load_dke_cfg_45B361 | 139 | PARTIAL | main.c:load_dke_config | Parses DKE.CFG; port lacks Brightness/Contrast/Saturation/Language/Detail keys and script var-name table. — Detail/render_mode and display tuning from DKE.CFG ignored. |
| 0045B98E | sub_45B98E | 18 | SKIP |  | Developer script-variable viewer / debug image drawer. |
| 0045BA2C | sub_45BA2C | 22 | SKIP |  | Developer script-variable viewer / debug image drawer. |
| 0045BB9C | sub_45BB9C | 154 | SKIP |  | Developer script-variable viewer / debug image drawer. |
| 0045C152 | sub_45C152 | 18 | SKIP |  | Developer script-variable viewer / debug image drawer. |
| 0045C230 | sub_45C230 | 109 | SKIP |  | Developer script-variable viewer / debug image drawer. |
| 0045C700 | sub_45C700 | 65 | cited |  |  |
| 0045C9C5 | update_companion_status | 37 | cited |  |  |
| 0045CAE8 | sub_45CAE8 | 97 | cited |  |  |
| 0045CD9C | sub_45CD9C | 62 | cited |  |  |
| 0045D0BA | sub_45D0BA | 28 | cited |  |  |
| 0045D1F6 | update_player_interface | 17 | cited |  |  |
| 0045D27A | sub_45D27A | 37 | cited |  |  |
| 0045D395 | ls_set_mouse | 27 | cited |  |  |
| 0045D427 | sub_45D427 | 8 | SKIP |  | Developer script-variable viewer / debug image drawer. |
| 0045D4D9 | sub_45D4D9 | 26 | SKIP |  | Developer script-variable viewer / debug image drawer. |
| 0045D560 | sub_45D560 | 5 | cited |  |  |
| 0045E070 | sub_45E070 | 29 | cited |  |  |
| 0045E157 | draw_selection_rectangle | 53 | PORTED | model.c:model_render_world | Entity model rasteriser with dirty-cell grid, face sort and compositing; port redraws whole frame with painter sort. |
| 0045E2CB | sub_45E2CB | 16 | PORTED | model.c:model_render_world | Entity model rasteriser with dirty-cell grid, face sort and compositing; port redraws whole frame with painter sort. |
| 0045E383 | sub_45E383 | 16 | PORTED | model.c:model_render_world | Entity model rasteriser with dirty-cell grid, face sort and compositing; port redraws whole frame with painter sort. |
| 0045E43A | render_selection_indicator | 47 | PORTED | model.c:model_render_world | Entity model rasteriser with dirty-cell grid, face sort and compositing; port redraws whole frame with painter sort. |
| 0045E56D | check_entity_selection | 374 | PORTED | model.c:model_render_world | Entity model rasteriser with dirty-cell grid, face sort and compositing; port redraws whole frame with painter sort. |
| 0045F3D0 | sub_45F3D0 | 14 | SKIP |  | Named-file cache (100 entries) in original memory manager; port loads files directly. |
| 0045F457 | sub_45F457 | 82 | SKIP |  | Named-file cache (100 entries) in original memory manager; port loads files directly. |
| 0045F724 | sub_45F724 | 24 | SKIP |  | Named-file cache (100 entries) in original memory manager; port loads files directly. |
| 0045F7E6 | sub_45F7E6 | 24 | SKIP |  | Named-file cache (100 entries) in original memory manager; port loads files directly. |
| 0045F881 | sub_45F881 | 47 | SKIP |  | Named-file cache (100 entries) in original memory manager; port loads files directly. |
| 0045F9D6 | sub_45F9D6 | 83 | SKIP |  | Named-file cache (100 entries) in original memory manager; port loads files directly. |
| 0045FD40 | sub_45FD40 | 139 | cited |  |  |
| 0046034D | sub_46034D | 18 | MISSING |  | P92 flying imp AI helpers (hit test on player, apply offset and animation); called from sub_45FD40. — Flying imp enemy (type 0x5C) has no AI. |
| 004603CB | sub_4603CB | 17 | MISSING |  | P92 flying imp AI helpers (hit test on player, apply offset and animation); called from sub_45FD40. — Flying imp enemy (type 0x5C) has no AI. |
| 00460444 | sub_460444 | 109 | cited |  |  |
| 004608A0 | sub_4608A0 | 7 | PORTED | anim2d.c:anim2d_load_camera | Frees and reloads the camera 2D animation set (sub_4132F3 + sub_4130FB). |
| 004608C2 | sub_4608C2 | 12 | SKIP |  | Shows a still image full-screen in display mode 20; only referenced from game-context vtable. |
| 0046091B | load_configuration | 34 | cited |  |  |
| 00460B6C | load_configuration | 307 | cited |  |  |
| 00461468 | sub_461468 | 207 | cited |  |  |
| 00461AF6 | sub_461AF6 | 35 | MISSING |  | Clears per-entity +0xD88 flag for player and companions, or sets viewport state 2. — Minor; stale flag state across scene resets. |
| 00461BA0 | sub_461BA0 | 27 | cited |  |  |
| 00461CA7 | sub_461CA7 | 61 | cited |  |  |
| 00461DC4 | sub_461DC4 | 304 | cited |  |  |
| 00462BCC | sub_462BCC | 45 | cited |  |  |
| 00462C9E | sub_462C9E | 124 | cited |  |  |
| 0046319A | sub_46319A | 177 | cited |  |  |
| 0046386D | sub_46386D | 14 | cited |  |  |
| 00463906 | sub_463906 | 14 | cited |  |  |
| 0046399F | sub_46399F | 16 | cited |  |  |
| 00463A24 | sub_463A24 | 60 | cited |  |  |
| 00463C2E | sub_463C2E | 101 | cited |  |  |
| 00463FB5 | sub_463FB5 | 31 | cited |  |  |
| 004640BC | sub_4640BC | 217 | cited |  |  |
| 004648DC | sub_4648DC | 17 | cited |  |  |
| 0046493B | system_get_gametime | 153 | cited |  |  |
| 00464F6D | log_message | 86 | cited |  |  |
| 00465278 | sub_465278 | 70 | cited |  |  |
| 004654CE | sub_4654CE | 111 | cited |  |  |
| 00465830 | sub_465830 | 32 | PORTED | obc.c (obc_node_t prev links) | Object tree utility: rebuild sibling back-links / flatten pending-child links. |
| 004658D4 | sub_4658D4 | 58 | cited |  |  |
| 00465B47 | sub_465B47 | 17 | cited |  |  |
| 00465BDF | play_entity_animation | 177 | cited |  |  |
| 004663E3 | sub_4663E3 | 5 | cited |  |  |
| 004663F4 | sub_4663F4 | 16 | PORTED | actor.c (E32(e,0xC04) = -1) | Resets entity +0xC04 to -1 for type 1 entities. |
| 0046642E | sub_46642E | 213 | cited |  |  |
| 00466C25 | sub_466C25 | 23 | SKIP |  | Dirty-rectangle bookkeeping for entity redraw; port redraws every frame. |
| 00466D0F | sub_466D0F | 14 | SKIP |  | Dirty-rectangle bookkeeping for entity redraw; port redraws every frame. |
| 00466D4F | update_game_hud | 965 | cited |  |  |
| 00468D56 | sub_468D56 | 107 | cited |  |  |
| 00468FFE | sub_468FFE | 44 | cited |  |  |
| 00469222 | sub_469222 | 136 | cited |  |  |
| 0046977B | sub_46977B | 62 | cited |  |  |
| 004699D9 | sub_4699D9 | 36 | cited |  |  |
| 00469AAC | sub_469AAC | 22 | PORTED | obc.c (obc_node_t prev links) | Object tree utility: rebuild sibling back-links / flatten pending-child links. |
| 00469B4F | sub_469B4F | 61 | cited |  |  |
| 00469D30 | sub_469D30 | 5 | SKIP |  | CD / HDD path probing and insert-CD prompt (original media handling). |
| 00469D4E | sub_469D4E | 24 | SKIP |  | CD / HDD path probing and insert-CD prompt (original media handling). |
| 00469DCA | sub_469DCA | 15 | SKIP |  | CD / HDD path probing and insert-CD prompt (original media handling). |
| 00469E34 | sub_469E34 | 24 | cited |  |  |
| 00469EE8 | sub_469EE8 | 9 | cited |  |  |
| 00469F32 | game_play_intro_469F32 | 35 | cited |  |  |
| 00469FFF | sub_469FFF | 98 | SKIP |  | CD / HDD path probing and insert-CD prompt (original media handling). |
| 0046A358 | sub_46A358 | 38 | cited |  |  |
| 0046A4D5 | draw_image | 487 | cited |  |  |
| 0046B883 | sub_46B883 | 17 | cited |  |  |
| 0046B909 | sub_46B909 | 16 | cited |  |  |
| 0046B984 | sprintf | 33 | cited |  |  |
| 0046BA63 | sub_46BA63 | 15 | cited |  |  |
| 0046BAEB | sub_46BAEB | 45 | cited |  |  |
| 0046BC74 | sub_46BC74 | 69 | cited |  |  |
| 0046BE40 | can_do_32k | 27 | cited |  |  |
| 0046BEEB | set_position_coordinates | 189 | cited |  |  |
| 0046C599 | game_release_resources_46C599 | 152 | SKIP |  | Shutdown cleanup of buffers, bitmaps and object lists; OS frees at exit. |
| 0046C930 | sub_46C930 | 96 | cited |  |  |
| 0046CE97 | sub_46CE97 | 30 | PORTED | model.c (GRD ramp use) | Loads .GRD shading ramp into palette/index buffers (display_pause_screen caller). |
| 0046CF6D | sub_46C930 | 40 | PORTED | scene.c:scene_load_camera_backdrop | Loads camera F backdrop (CIF) and palette. |
| 0046D0AF | sub_46C930 | 10 | PARTIAL | scene.c:scene_load_camera_backdrop | Loads camera Z-depth image into render_buffer2; port loads Z but leaves it unused. — No depth occlusion of actors behind scenery. |
| 0046D0EA | system_get_gametime | 64 | cited |  |  |
| 0046D305 | sub_46D305 | 266 | cited |  |  |
| 0046DFEC | set_display_mode | 158 | cited |  |  |
| 0046E4A8 | scene_release_level_resources_46E4A8 | 128 | cited |  |  |
| 0046E883 | sub_46E883 | 112 | cited |  |  |
| 0046EC83 | update_overlay_effects | 325 | cited |  |  |
| 0046F6E5 | update_visual_effects | 23 | cited |  |  |
| 0046F7B5 | sub_46F7B5 | 28 | cited |  |  |
| 0046F89E | sub_46F89E | 76 | SKIP |  | Finds darkest/brightest palette entries for 8bpp mode; port is 16/32-bit. |
| 0046FB40 | sub_46FB40 | 70 | cited |  |  |
| 0046FDC9 | sub_46FDC9 | 60 | cited |  |  |
| 0046FF9A | sub_46FF9A | 87 | cited |  |  |
| 004701E0 | sub_4701E0 | 60 | cited |  |  |
| 00470507 | sub_470507 | 176 | cited |  |  |
| 00470A8F | sub_470A8F | 33 | cited |  |  |
| 00470BB6 | sub_470BB6 | 63 | cited |  |  |
| 00470E54 | sub_470E54 | 84 | cited |  |  |
| 004711F9 | sub_4711F9 | 92 | cited |  |  |
| 004715D3 | sub_4715D3 | 149 | cited |  |  |
| 00471B80 | sub_471B80 | 20 | cited |  |  |
| 00471BDF | sub_471BDF | 10 | SKIP |  | Tiny helper: creature class letter to index. |
| 00471C18 | sub_471C18 | 106 | cited |  |  |
| 004723C0 | sub_4723C0 | 45 | cited |  |  |
| 0047257D | sub_47257D | 34 | cited |  |  |
| 0047269E | is_character_type_a | 5 | cited |  |  |
| 004726C8 | is_character_type_b | 5 | cited |  |  |
| 004726F2 | is_character_type_c | 5 | cited |  |  |
| 0047271C | sub_47271C | 5 | cited |  |  |
| 00472746 | sub_472746 | 5 | cited |  |  |
| 00472770 | sub_472770 | 5 | cited |  |  |
| 0047279D | sub_47279D | 5 | cited |  |  |
| 004727C7 | sub_4727C7 | 12 | cited |  |  |
| 0047286D | sub_47286D | 12 | cited |  |  |
| 004728DF | sub_4728DF | 23 | cited |  |  |
| 004729B0 | get_character_action_index | 20 | cited |  |  |
| 00472A48 | sub_472A48 | 17 | cited |  |  |
| 00472AF1 | sub_472AF1 | 14 | cited |  |  |
| 00472B47 | sub_472B47 | 28 | cited |  |  |
| 00472C5C | sub_472C5C | 71 | cited |  |  |
| 00472FB5 | sub_472FB5 | 23 | cited |  |  |
| 0047306E | sub_47306E | 34 | MISSING |  | Saves per-level object position and rotation into table 4DCDA0 (34 levels x 6 variants). — Dropped object positions not remembered across level changes. |
| 004731C6 | sub_4731C6 | 12 | cited |  |  |
| 004731F6 | sub_4731F6 | 58 | cited |  |  |
| 0047347E | sub_47347E | 29 | MISSING |  | Saves per-level object position and rotation into table 4DCDA0 (34 levels x 6 variants). — Dropped object positions not remembered across level changes. |
| 004735A2 | sub_4735A2 | 65 | cited |  |  |
| 00473756 | sub_473756 | 71 | cited |  |  |
| 0047398A | sub_426BB5 | 115 | cited |  |  |
| 00473DBC | sub_473DBC | 64 | cited |  |  |
| 0047404E | sub_472B47 | 99 | PARTIAL | scene.c:scene_weapon_entity_attach | Attaches object to entity (detaches previous, picks armeb chain, zeroes offsets); port covers sub_473756 form only. — Item equip path may miss detach of previous item. |
| 00474462 | sscanf | 75 | cited |  |  |
| 00474731 | sub_474731 | 77 | cited |  |  |
| 00474900 | sub_474900 | 20 | SKIP |  | CIF/GIF encoder (LZW) for saving images; dev/screenshot only. |
| 00474969 | sub_474969 | 14 | SKIP |  | CIF/GIF encoder (LZW) for saving images; dev/screenshot only. |
| 00474997 | sub_474997 | 87 | SKIP |  | CIF/GIF encoder (LZW) for saving images; dev/screenshot only. |
| 00474CF5 | log_message | 108 | cited |  |  |
| 004750CF | initialize_bitmap_buffer | 20 | SKIP | libsys/graphics/image.c | Image decode/palette load wrappers and region background reload; port draws whole backdrop each frame. |
| 0047512E | strcpy | 73 | cited |  |  |
| 0047545A | sub_47545A | 46 | cited |  |  |
| 004755B5 | sub_4755B5 | 55 | SKIP | libsys/graphics/image.c | Image decode/palette load wrappers and region background reload; port draws whole backdrop each frame. |
| 004757D8 | load_image_color_data | 46 | cited |  |  |
| 00475990 | sub_475990 | 55 | cited |  |  |
| 00475B15 | get_entity_character | 20 | cited |  |  |
| 00475BB6 | sub_475BB6 | 66 | cited |  |  |
| 00475E47 | sub_475E47 | 20 | SKIP |  | Frees object slots of other levels when heap is low (memory pressure). |
| 00475F7A | sub_475F7A | 75 | cited |  |  |
| 004762BF | sub_4762BF | 117 | cited |  |  |
| 004765DB | sub_4765DB | 39 | SKIP | libsys/graphics/image.c | Image decode/palette load wrappers and region background reload; port draws whole backdrop each frame. |
| 0047674F | sub_47674F | 104 | cited |  |  |
| 00476BEE | sub_476BEE | 68 | cited |  |  |
| 00476E86 | sub_476E86 | 33 | MISSING |  | On level load marks object slots from other levels for removal and clears carried-over flags. — Object slots may persist wrongly between levels. |
| 0047701F | sub_47701F | 50 | cited |  |  |
| 004771D1 | sub_4771D1 | 71 | cited |  |  |
| 004774BC | sub_4774BC | 220 | PARTIAL | libsys/graphics/graphic.c:ls_bit_move | Pointer-table sprite blit kernels (8bpp and 16bpp, plain/shaded/alpha); port has copy and colour-key only. — Alpha and shaded 2D blits not implemented. |
| 0047776C | sub_47776C | 387 | PARTIAL | libsys/graphics/graphic.c:ls_bit_move | Pointer-table sprite blit kernels (8bpp and 16bpp, plain/shaded/alpha); port has copy and colour-key only. — Alpha and shaded 2D blits not implemented. |
| 00477BB0 | sub_477BB0 | 272 | PARTIAL | libsys/graphics/graphic.c:ls_bit_move | Pointer-table sprite blit kernels (8bpp and 16bpp, plain/shaded/alpha); port has copy and colour-key only. — Alpha and shaded 2D blits not implemented. |
| 00477E28 | sub_477E28 | 20 | SKIP |  | Bounds-checking stub of a blit kernel wrapper; no callers. |
| 00477EAC | f | 2 | cited |  |  |
| 00477FD4 | sub_477FD4 | 30 | cited |  |  |
| 00478014 | sub_478014 | 15 | PORTED | model.c:model_render_world | Entity model rasteriser with dirty-cell grid, face sort and compositing; port redraws whole frame with painter sort. |
| 00478030 | sub_478030 | 48 | PORTED | model.c:model_render_world | Entity model rasteriser with dirty-cell grid, face sort and compositing; port redraws whole frame with painter sort. |
| 004780B4 | memset | 102 | cited |  |  |
| 004781E4 | sub_4781E4 | 55 | PARTIAL | libsys/graphics/graphic.c:ls_bit_move | Pointer-table sprite blit kernels (8bpp and 16bpp, plain/shaded/alpha); port has copy and colour-key only. — Alpha and shaded 2D blits not implemented. |
| 0047826B | sub_47826B | 45 | PARTIAL | libsys/graphics/graphic.c:ls_bit_move | Pointer-table sprite blit kernels (8bpp and 16bpp, plain/shaded/alpha); port has copy and colour-key only. — Alpha and shaded 2D blits not implemented. |
| 004782DC | sub_4782DC | 220 | PARTIAL | libsys/graphics/graphic.c:ls_bit_move | Pointer-table sprite blit kernels (8bpp and 16bpp, plain/shaded/alpha); port has copy and colour-key only. — Alpha and shaded 2D blits not implemented. |
| 004785BC | BYTE1 | 358 | SKIP |  | CIF/GIF encoder (LZW) for saving images; dev/screenshot only. |
| 00478A20 | sub_478A20 | 25 | SKIP |  | CIF/GIF encoder (LZW) for saving images; dev/screenshot only. |
| 00478A79 | f | 2 | cited |  |  |
| 00478CCC | sub_478CCC | 64 | SKIP |  | CIF/GIF encoder (LZW) for saving images; dev/screenshot only. |
| 00478DC0 | unused_sub_478DC0 | 2672 | SKIP |  | Unused MMX routine. |
| 0047AAE0 | sub_47AAE0 | 5 | SKIP |  | MMX 16bpp triangle test path gated by dword_4B70B0, which is never set (dead code). |
| 0047AB00 | unused_sub_47AB00 | 6 | SKIP |  | Unused thunk. |
| 0047AB30 | sub_47AB30 | 1855 | SKIP |  | MMX 16bpp triangle test path gated by dword_4B70B0, which is never set (dead code). |
| 0047CBA0 | ls_startup | 21 | cited |  |  |
| 0047CBF0 | ls_shut_down | 37 | SKIP |  | libsys memory manager / shutdown / string glue replaced by platform layer and malloc. |
| 0047CCD0 | ls_exit | 9 | SKIP |  | libsys memory manager / shutdown / string glue replaced by platform layer and malloc. |
| 0047CCE0 | f | 2 | cited |  |  |
| 0047CCF0 | ls_random | 16 | cited |  |  |
| 0047CD00 | unused_ls_ex_to_long | 39 | SKIP |  | libsys memory manager / shutdown / string glue replaced by platform layer and malloc. |
| 0047CD90 | ls_mem_copy | 5 | SKIP |  | libsys memory manager / shutdown / string glue replaced by platform layer and malloc. |
| 0047CDA0 | ls_str_upper | 0 | SKIP |  | libsys memory manager / shutdown / string glue replaced by platform layer and malloc. |
| 0047CDB0 | ls_give_extension | 57 | SKIP |  | Extension helper for file names; port resolves paths in file_system.c. |
| 0047CE80 | ls_set_rectangle | 12 | SKIP | libsys/graphics/graphic.c:ls_bit_move | Rect/format/restore-callback helpers of DirectDraw bit move; replaced by port clipping. |
| 0047CEA0 | unused_ls_set_bmap_format | 7 | SKIP | libsys/graphics/graphic.c:ls_bit_move | Rect/format/restore-callback helpers of DirectDraw bit move; replaced by port clipping. |
| 0047CEB0 | ls_init_bmap_ex | 76 | cited |  |  |
| 0047D090 | ls_free_bmap | 26 | cited |  |  |
| 0047D0E0 | ls_set_restore_callback | 6 | SKIP | libsys/graphics/graphic.c:ls_bit_move | Rect/format/restore-callback helpers of DirectDraw bit move; replaced by port clipping. |
| 0047D0F0 | ls_set_rectangle_2 | 79 | cited |  |  |
| 0047D1F0 | ls_set_rectangle_3 | 83 | SKIP | libsys/graphics/graphic.c:ls_bit_move | Rect/format/restore-callback helpers of DirectDraw bit move; replaced by port clipping. |
| 0047D310 | sub_47D310 | 6 | cited |  |  |
| 0047D320 | sub_47D320 | 6 | PORTED | libsys/graphics/font.c | Set text colour / style globals for draw_text. |
| 0047D330 | sub_47D330 | 10 | cited |  |  |
| 0047D340 | sub_47D340 | 10 | PORTED | libsys/graphics/font.c | Set text colour / style globals for draw_text. |
| 0047D350 | ls_duplicate_bmap | 11 | SKIP |  | ls_duplicate_bmap; no callers. |
| 0047D390 | ls_clear_bmap | 5 | cited |  |  |
| 0047D3B0 | ls_box | 86 | cited |  |  |
| 0047D7E0 | ls_draw_line | 199 | SKIP |  | Bresenham line/plot for wireframe debug rasteriser; port has scene_draw_line for overlays. |
| 0047DA90 | ls_bit_move_256 | 90 | SKIP |  | DirectDraw 256-colour bit-move, lock and flip paths replaced by software surfaces. |
| 0047DC10 | ls_fast_bit_move | 20 | SKIP |  | DirectDraw 256-colour bit-move, lock and flip paths replaced by software surfaces. |
| 0047DCB0 | ls_bit_move | 122 | cited |  |  |
| 0047DFC0 | ls_fill_rect | 204 | cited |  |  |
| 0047E2A0 | ls_invert_rect | 94 | cited |  |  |
| 0047E3C0 | ls_point | 34 | SKIP |  | ls_point pixel read used by a rarely used effect; port reads buffers directly. |
| 0047E4B0 | ls_plot | 25 | SKIP |  | Bresenham line/plot for wireframe debug rasteriser; port has scene_draw_line for overlays. |
| 0047E500 | ls_lock_rect | 72 | SKIP |  | DirectDraw 256-colour bit-move, lock and flip paths replaced by software surfaces. |
| 0047E600 | ls_lock_bmap | 33 | cited |  |  |
| 0047E680 | ls_unlock_bmap | 25 | cited |  |  |
| 0047E6E0 | ls_flip_bmap | 57 | SKIP |  | DirectDraw 256-colour bit-move, lock and flip paths replaced by software surfaces. |
| 0047E720 | ls_get_video_mode_caps | 38 | cited |  |  |
| 0047E7A0 | ls_get_bit_move_status | 44 | SKIP |  | DirectDraw 256-colour bit-move, lock and flip paths replaced by software surfaces. |
| 0047E7C0 | ls_get_heap | 60 | cited |  |  |
| 0047E8D0 | ls_init_memory_manager | 17 | cited |  |  |
| 0047E920 | ls_shut_memory_manager | 10 | SKIP |  | libsys memory manager / shutdown / string glue replaced by platform layer and malloc. |
| 0047E950 | ls_release_heap | 39 | cited |  |  |
| 0047E9D0 | ls_find_heap | 21 | SKIP |  | libsys memory manager / shutdown / string glue replaced by platform layer and malloc. |
| 0047EA10 | ls_max_block | 6 | SKIP |  | libsys memory manager / shutdown / string glue replaced by platform layer and malloc. |
| 0047EA20 | ls_max_block_heap | 44 | SKIP |  | libsys memory manager / shutdown / string glue replaced by platform layer and malloc. |
| 0047EAC0 | ls_max_memory | 6 | SKIP |  | libsys memory manager / shutdown / string glue replaced by platform layer and malloc. |
| 0047EAD0 | ls_max_memory_heap | 23 | SKIP |  | libsys memory manager / shutdown / string glue replaced by platform layer and malloc. |
| 0047EB30 | ls_get_memory | 6 | cited |  |  |
| 0047EB40 | ls_set_memory_name | 49 | SKIP |  | libsys memory manager / shutdown / string glue replaced by platform layer and malloc. |
| 0047EC00 | ls_get_memory_name | 38 | SKIP |  | libsys memory manager / shutdown / string glue replaced by platform layer and malloc. |
| 0047EC60 | ls_enumerate_memory | 30 | SKIP |  | libsys memory manager / shutdown / string glue replaced by platform layer and malloc. |
| 0047ECA0 | ls_get_memory_heap | 116 | cited |  |  |
| 0047EE30 | ls_release | 16 | cited |  |  |
| 0047EE60 | ls_release_heap_memory | 55 | SKIP |  | libsys memory manager / shutdown / string glue replaced by platform layer and malloc. |
| 0047EF50 | ls_select_memory_up_heap | 6 | SKIP |  | libsys memory manager / shutdown / string glue replaced by platform layer and malloc. |
| 0047EF70 | ls_select_memory_down_heap | 6 | SKIP |  | libsys memory manager / shutdown / string glue replaced by platform layer and malloc. |
| 0047EF90 | ls_switch_memory | 9 | cited |  |  |
| 0047EFC0 | unused_sub_47EFC0 | 14 | SKIP |  | Unused or dead libsys helper (palette/IDA macro stubs). |
| 0047F050 | WORD2 | 9 | SKIP |  | Unused or dead libsys helper (palette/IDA macro stubs). |
| 0047F060 | unused_sub_47F060 | 109 | SKIP |  | Unused or dead libsys helper (palette/IDA macro stubs). |
| 0047F280 | unused_ls_load_palette | 17 | SKIP |  | Unused or dead libsys helper (palette/IDA macro stubs). |
| 0047F2E0 | unused_sub_47F2E0 | 13 | SKIP |  | Unused or dead libsys helper (palette/IDA macro stubs). |
| 0047F310 | unused_sub_47F310 | 15 | SKIP |  | Unused or dead libsys helper (palette/IDA macro stubs). |
| 0047F340 | unused_sub_47F340 | 49 | SKIP |  | Unused or dead libsys helper (palette/IDA macro stubs). |
| 0047F470 | unused_sub_47F470 | 22 | SKIP |  | Unused or dead libsys helper (palette/IDA macro stubs). |
| 0047F4E0 | unused_sub_47F4E0 | 13 | SKIP |  | Unused or dead libsys helper (palette/IDA macro stubs). |
| 0047F510 | ls_init_palette | 10 | cited |  |  |
| 0047F530 | ls_free_palette | 13 | cited |  |  |
| 0047F550 | ls_set_palette | 16 | SKIP |  | Sets palette entry through DirectDraw; port converts palette itself. |
| 0047F5A0 | ls_convert_colour | 49 | cited |  |  |
| 0047F620 | ls_read_key | 6 | SKIP |  | Keyboard polling via Win32; replaced by platform events. |
| 0047F630 | ls_wait_key | 10 | cited |  |  |
| 0047F640 | ls_flush_keyboard | 0 | cited |  |  |
| 0047F650 | unused_ls_wait_no_key | 10 | SKIP |  | Keyboard polling via Win32; replaced by platform events. |
| 0047F670 | ls_key_map | 11 | SKIP |  | Keyboard polling via Win32; replaced by platform events. |
| 0047F690 | ls_is_key_pressed | 16 | SKIP |  | Keyboard polling via Win32; replaced by platform events. |
| 0047F6C0 | ls_load_pcx_ex | 83 | SKIP |  | PCX loader for dev tools. |
| 0047F880 | ls_read_pcx_line | 39 | SKIP |  | PCX loader for dev tools. |
| 0047F8E0 | ls_load_pcx_pal | 44 | SKIP |  | PCX loader for dev tools. |
| 0047F990 | ls_save_pcx | 97 | SKIP |  | PCX screenshot saver (debug key). |
| 0047FB70 | ls_write_pcx_line | 62 | SKIP |  | PCX screenshot saver (debug key). |
| 0047FC80 | ls_delay | 15 | cited |  |  |
| 0047FCA0 | unused_ls_set_regulation | 13 | SKIP |  | Frame regulation and timer glue; replaced by ls_tick/ls_delay. |
| 0047FCC0 | unused_ls_regulate | 18 | SKIP |  | Frame regulation and timer glue; replaced by ls_tick/ls_delay. |
| 0047FCF0 | ls_read_timer | 0 | SKIP |  | Frame regulation and timer glue; replaced by ls_tick/ls_delay. |
| 0047FD20 | unused_ls_wait_user_event | 28 | SKIP |  | Frame regulation and timer glue; replaced by ls_tick/ls_delay. |
| 0047FD90 | ls_reset_last_event_timer | 11 | SKIP |  | Frame regulation and timer glue; replaced by ls_tick/ls_delay. |
| 0047FDA0 | unused_ls_get_last_event_timer | 7 | SKIP |  | Frame regulation and timer glue; replaced by ls_tick/ls_delay. |
| 0047FDB0 | ls_init_sound_system | 12 | cited |  |  |
| 0047FDD0 | ls_shut_sound_system | 10 | cited |  |  |
| 0047FDE0 | ls_beep | 0 | SKIP |  | ls_beep; no-op. |
| 0047FDF0 | ls_load_sound | 40 | cited |  |  |
| 0047FEC0 | ls_create_sound | 36 | SKIP | sfx.c | Creates DirectSound buffer for a sample; sfx.c plays WAVs through host audio. |
| 0047FF70 | ls_free_sound | 30 | cited |  |  |
| 0047FFE0 | unused_ls_wait_no_sound | 12 | SKIP | sfx.c | Creates DirectSound buffer for a sample; sfx.c plays WAVs through host audio. |
| 00480000 | ls_play_sound | 33 | cited |  |  |
| 00480080 | ls_stop_sound | 27 | cited |  |  |
| 004800F0 | ls_get_sound_status | 21 | cited |  |  |
| 00480140 | unused_ls_get_sound_position | 20 | SKIP |  | Dead code, unused_ prefix, no callers |
| 004801A0 | unused_ls_get_sound_frequency | 33 | SKIP |  | Dead code, unused_ prefix, no callers |
| 00480200 | ls_set_sound_volume | 33 | PORTED | src/sfx.c:voice_volume | Set sound-buffer volume; port mixer voice volume with master_vol |
| 00480260 | ls_resume_sound | 23 | MISSING |  | Pause/resume the sound buffers when the P key pause screen opens/closes — In-game pause (P) does not silence sound or show the pause screen |
| 004802B0 | ls_pause_sound | 24 | MISSING |  | Pause/resume the sound buffers when the P key pause screen opens/closes — In-game pause (P) does not silence sound or show the pause screen |
| 00480300 | sub_480300 | 25 | SKIP |  | Win32 window/paint/ini glue replaced by platform layer |
| 00480370 | sub_480370 | 57 | SKIP |  | Win32 window/paint/ini glue replaced by platform layer |
| 004804D0 | sub_4804D0 | 112 | cited |  |  |
| 004806F0 | lsi_get_profile_string | 42 | SKIP |  | Win32 window/paint/ini glue replaced by platform layer |
| 004807B0 | WinMain | 63 | cited |  |  |
| 004808F0 | win_init_instance | 48 | SKIP |  | Win32 window/paint/ini glue replaced by platform layer |
| 00480A40 | lsys_update_event | 33 | PORTED | src/libsys/system.h:ls_update_events | Pump window/input events |
| 00480AF0 | sub_480AF0 | 6 | SKIP |  | Win32 window/paint/ini glue replaced by platform layer |
| 00480B20 | sub_480B20 | 45 | SKIP |  | Win32 window/paint/ini glue replaced by platform layer |
| 00480C30 | lsi_set_window_title | 6 | cited |  |  |
| 00480C50 | sub_480C50 | 5 | SKIP |  | Win32 window/paint/ini glue replaced by platform layer |
| 00480C60 | sub_480C60 | 13 | SKIP |  | Win32 window/paint/ini glue replaced by platform layer |
| 00480C90 | sub_480C90 | 24 | SKIP |  | Win32 window/paint/ini glue replaced by platform layer |
| 00480CF0 | sub_480CF0 | 28 | cited |  |  |
| 00480DB0 | ls_shut_palette_system | 20 | cited |  |  |
| 00480DF0 | sub_480DF0 | 38 | SKIP |  | DirectDraw palette upload; port palette.c/platform framebuffer |
| 00480E80 | sub_480E80 | 79 | SKIP |  | DirectDraw palette upload; port palette.c/platform framebuffer |
| 00480FC0 | unused_sub_480FC0 | 65 | SKIP |  | Dead code, unused_ prefix, no callers |
| 00481140 | unused_sub_481140 | 40 | SKIP |  | Dead code, unused_ prefix, no callers |
| 004811D0 | lsys_init_joy_system | 28 | cited |  |  |
| 00481250 | sub_481250 | 11 | SKIP |  | Joystick (joyGetPosEx) read; joystick replaced by platform layer |
| 00481290 | update_status_display | 55 | SKIP |  | Joystick (joyGetPosEx) read; joystick replaced by platform layer |
| 00481390 | ls_update_event | 12 | PORTED | src/libsys/system.h:ls_update_events | Pump window/input events |
| 004813C0 | sub_4813C0 | 9 | SKIP |  | Error/log message boxes, DX version, ls1.cfg log: diagnostics |
| 004813E0 | ls_error | 12 | SKIP |  | Error/log message boxes, DX version, ls1.cfg log: diagnostics |
| 00481430 | sub_481430 | 38 | SKIP |  | Error/log message boxes, DX version, ls1.cfg log: diagnostics |
| 004814D0 | ls_message | 17 | SKIP |  | Error/log message boxes, DX version, ls1.cfg log: diagnostics |
| 00481550 | ls_copy_file | 24 | SKIP |  | Original file-manager copy/write/close helpers, replaced by stdio |
| 004815C0 | select_language_version | 20 | cited |  |  |
| 00481640 | ls_open | 26 | cited |  |  |
| 00481740 | ls_close | 10 | cited |  |  |
| 00481760 | ls_read | 5 | cited |  |  |
| 00481770 | ls_write | 5 | SKIP |  | Original file-manager copy/write/close helpers, replaced by stdio |
| 00481780 | ls_seek | 5 | cited |  |  |
| 00481790 | ls_load_file_at | 20 | cited |  |  |
| 004817E0 | ls_load_file | 24 | PORTED | src/libsys/file_system.c | File close/read/write/seek/delete/rename/exists/load wrappers |
| 00481840 | ls_get_filesize | 13 | cited |  |  |
| 00481880 | ls_get_filedate | 13 | cited |  |  |
| 004819B0 | ls_delete_file | 15 | SKIP |  | Original file-manager copy/write/close helpers, replaced by stdio |
| 004819E0 | ls_rename_file | 24 | SKIP |  | Dev text-entry prompt (debug level/perso cheats); not shipped gameplay |
| 00481A60 | sub_481A60 | 47 | SKIP |  | Dev text-entry prompt (debug level/perso cheats); not shipped gameplay |
| 00481BE0 | render_text_with_style | 132 | SKIP |  | Dev text-entry prompt (debug level/perso cheats); not shipped gameplay |
| 00481F70 | sub_481F70 | 51 | SKIP |  | Dev text-entry prompt (debug level/perso cheats); not shipped gameplay |
| 004820B0 | sub_4820B0 | 26 | cited |  |  |
| 00482120 | sub_482120 | 52 | SKIP |  | Display-depth bitmap remap and image-dispatch; port uses 32-bit XRGB pipeline |
| 00482200 | sub_482200 | 31 | cited |  |  |
| 004822E0 | sub_4822E0 | 31 | cited |  |  |
| 00482370 | sub_482370 | 121 | SKIP |  | Display-depth bitmap remap and image-dispatch; port uses 32-bit XRGB pipeline |
| 00482470 | sub_482470 | 45 | SKIP |  | Display-depth bitmap remap and image-dispatch; port uses 32-bit XRGB pipeline |
| 00482560 | sub_482560 | 23 | cited |  |  |
| 004825B0 | sub_4825B0 | 24 | SKIP |  | Display-depth bitmap remap and image-dispatch; port uses 32-bit XRGB pipeline |
| 00482630 | ls_get_mouse | 45 | PORTED | src/libsys/system.h motion_x/mouse_left | Mouse position/buttons/delta; platform layer fills system_t |
| 004826C0 | ls_set_mouse | 10 | cited |  |  |
| 004826D0 | ls_set_mouse_range | 20 | cited |  |  |
| 00482710 | ls_get_mouse_delta | 36 | PORTED | src/libsys/system.h motion_x/mouse_left | Mouse position/buttons/delta; platform layer fills system_t |
| 00482860 | sub_482860 | 62 | SKIP |  | Display-depth bitmap remap and image-dispatch; port uses 32-bit XRGB pipeline |
| 00482950 | lsi_remap_bmap_depth | 147 | SKIP |  | Display-depth bitmap remap and image-dispatch; port uses 32-bit XRGB pipeline |
| 00482B90 | ls_fast_get_nearest_colour | 204 | SKIP |  | Display-depth bitmap remap and image-dispatch; port uses 32-bit XRGB pipeline |
| 00482F20 | ls_init_fast_nearest_colour | 0 | SKIP |  | Display-depth bitmap remap and image-dispatch; port uses 32-bit XRGB pipeline |
| 00482F40 | ls_build_palette_colour | 109 | cited |  |  |
| 00483130 | ls_palette_sort | 7 | cited |  |  |
| 00483170 | lsi_remap_bmap | 400 | cited |  |  |
| 004839B0 | lsi_remap_bmap_888x555 | 131 | SKIP |  | Display-depth bitmap remap and image-dispatch; port uses 32-bit XRGB pipeline |
| 00483CF0 | lsi_remap_bmap_888x565 | 130 | SKIP |  | Display-depth bitmap remap and image-dispatch; port uses 32-bit XRGB pipeline |
| 00484050 | ls_change_remap_colour | 12 | SKIP |  | Display-depth bitmap remap and image-dispatch; port uses 32-bit XRGB pipeline |
| 00484060 | ls_get_colour | 35 | SKIP |  | Display-depth bitmap remap and image-dispatch; port uses 32-bit XRGB pipeline |
| 00484110 | ls_init_graphic_system | 17 | cited |  |  |
| 00484140 | ls_shut_graphic_system | 19 | SKIP |  | Display-depth bitmap remap and image-dispatch; port uses 32-bit XRGB pipeline |
| 00484180 | ls_set_video_mode | 75 | cited |  |  |
| 00484300 | update_display_buffer | 0 | SKIP |  | Empty thunk (display refresh no-op in pause screen path) |
| 00484310 | sub_484310 | 14 | SKIP |  | CPU feature detection and display-mode enumeration at startup |
| 00484320 | ls_get_video_mode_caps | 15 | cited |  |  |
| 00484350 | ls_read_mac_word | 5 | PORTED | src/libsys/file_system.c:ls_read_* | Endian-aware word/long readers; port reads little-endian directly |
| 00484370 | ls_read_mac_long | 5 | PORTED | src/libsys/file_system.c:ls_read_* | Endian-aware word/long readers; port reads little-endian directly |
| 004843A0 | ls_read_pc_word | 5 | PORTED | src/libsys/file_system.c:ls_read_* | Endian-aware word/long readers; port reads little-endian directly |
| 004843C0 | ls_read_pc_long | 5 | PORTED | src/libsys/file_system.c:ls_read_* | Endian-aware word/long readers; port reads little-endian directly |
| 004843F0 | sub_4843F0 | 88 | cited |  |  |
| 004845D0 | sub_4845D0 | 111 | SKIP |  | CPU feature detection and display-mode enumeration at startup |
| 00484770 | sub_484770 | 17 | SKIP |  | CPU feature detection and display-mode enumeration at startup |
| 004847A0 | sub_4847A0 | 29 | SKIP |  | CPU feature detection and display-mode enumeration at startup |
| 004847F0 | sub_4847F0 | 52 | SKIP |  | CPU feature detection and display-mode enumeration at startup |
| 00484890 | sub_484890 | 11 | SKIP |  | CPU feature detection and display-mode enumeration at startup |
| 004848D0 | sub_4848D0 | 5 | SKIP |  | CPU feature detection and display-mode enumeration at startup |
| 00484920 | sub_484920 | 16 | SKIP |  | CPU feature detection and display-mode enumeration at startup |
| 00484970 | sub_484970 | 18 | SKIP |  | CPU feature detection and display-mode enumeration at startup |
| 004849C0 | sub_4849C0 | 54 | SKIP |  | CPU feature detection and display-mode enumeration at startup |
| 00484A70 | sub_484A70 | 14 | SKIP |  | CPU feature detection and display-mode enumeration at startup |
| 00484AC0 | ls_gif_decompress_ex | 111 | SKIP |  | GIF decoder; shipped data uses pre-converted CIF files |
| 00484D00 | ls_gif_load_pal | 46 | SKIP |  | GIF decoder; shipped data uses pre-converted CIF files |
| 00484E10 | ls_load_gif_ex | 22 | cited |  |  |
| 00484E80 | ls_decompress_gif_ex | 11 | SKIP |  | GIF decoder; shipped data uses pre-converted CIF files |
| 00484EB0 | ls_decompress_gif | 39 | SKIP |  | GIF decoder; shipped data uses pre-converted CIF files |
| 00484F20 | ls_load_gif_pal | 30 | SKIP |  | GIF decoder; shipped data uses pre-converted CIF files |
| 00484F80 | ls_decompress_gif_pal | 7 | SKIP |  | GIF decoder; shipped data uses pre-converted CIF files |
| 00484FA0 | ls_direct_sound_error | 11 | SKIP |  | DirectSound/stream buffer glue replaced by platform audio |
| 00484FC0 | lsi_init_cooporative_sound | 22 | SKIP |  | DirectSound/stream buffer glue replaced by platform audio |
| 00485020 | lsys_init_sound_system | 100 | cited |  |  |
| 004851E0 | lsys_shut_sound_system | 21 | cited |  |  |
| 00485220 | lsys_load_sound | 105 | PORTED | src/libsys/audio/wav.c | WAV file/memory RIFF parsing and sound load |
| 004853E0 | lsys_set_sound_volume | 17 | PORTED | src/sfx.c:voice_volume | Set sound-buffer volume; port mixer voice volume with master_vol |
| 00485420 | lsys_set_sound_frequency | 15 | PORTED | src/sfx.c:voice_apply | Per-buffer pitch/frequency; port sets mixer voice rate |
| 00485460 | lsys_set_sound_pan | 37 | PORTED | src/sfx.c:voice_apply (sub_44D06F pan) | Clamp and apply DirectSound pan; port mixer pan |
| 004854E0 | lsys_play_sound | 22 | PORTED | src/sfx.c:mixer voices | DirectSound buffer play/stop/status/position; software mixer voices replace it |
| 00485520 | lsys_stop_sound | 18 | PORTED | src/sfx.c:mixer voices | DirectSound buffer play/stop/status/position; software mixer voices replace it |
| 00485550 | lsys_free_sound | 15 | cited |  |  |
| 00485580 | lsys_get_sound_status | 15 | PORTED | src/sfx.c:mixer voices | DirectSound buffer play/stop/status/position; software mixer voices replace it |
| 004855D0 | lsys_get_sound_position | 15 | PORTED | src/sfx.c:mixer voices | DirectSound buffer play/stop/status/position; software mixer voices replace it |
| 00485600 | lsys_set_sound_position | 22 | PORTED | src/sfx.c:mixer voices | DirectSound buffer play/stop/status/position; software mixer voices replace it |
| 00485660 | lsys_beep | 5 | SKIP |  | DirectSound/stream buffer glue replaced by platform audio |
| 00485670 | lsys_set_sound_format | 48 | SKIP |  | DirectSound/stream buffer glue replaced by platform audio |
| 00485740 | lsys_lock_sound | 37 | SKIP |  | DirectSound/stream buffer glue replaced by platform audio |
| 004857C0 | lsys_unlock_sound | 31 | SKIP |  | DirectSound/stream buffer glue replaced by platform audio |
| 00485840 | sub_485840 | 50 | cited |  |  |
| 00485A10 | ls_extract_volume_and_directory | 266 | SKIP |  | Path/CD-volume resolution; replaced by ls_resolve_path |
| 00485C20 | sub_485C20 | 21 | SKIP |  | Path/CD-volume resolution; replaced by ls_resolve_path |
| 00485C90 | sub_485C90 | 11 | SKIP |  | Path/CD-volume resolution; replaced by ls_resolve_path |
| 00485CC0 | game_set_game_path_485CC0 | 34 | PORTED | src/libsys/file_system.c:ls_get_base_path | Derive game root path from module path |
| 00485DC0 | sub_485DC0 | 139 | SKIP |  | Path/CD-volume resolution; replaced by ls_resolve_path |
| 00486190 | ls_make_path | 83 | cited |  |  |
| 004863B0 | lsi_get_clean_path | 45 | SKIP |  | Path/CD-volume resolution; replaced by ls_resolve_path |
| 00486460 | ls_is_path_exist | 0 | cited |  |  |
| 00486470 | sub_486470 | 31 | PORTED | src/libsys/movie.c | Scaled blit of movie frame to screen (nearest-neighbour) |
| 00486580 | sub_486580 | 96 | PORTED | src/libsys/movie.c | Scaled blit of movie frame to screen (nearest-neighbour) |
| 004866F0 | sub_4866F0 | 64 | PORTED | src/libsys/graphics/cif.c:cif_load | Load CIF/RLE image into a bitmap |
| 004867B0 | sub_4867B0 | 167 | PORTED | src/libsys/graphics/cif.c:cif_load | Load CIF/RLE image into a bitmap |
| 00486B60 | sub_486B60 | 141 | SKIP |  | TGA screenshot save bound to a debug key in cinematic player |
| 00486F80 | sub_486F80 | 47 | PORTED | src/libsys/audio/wav.c | WAV file/memory RIFF parsing and sound load |
| 00487050 | sub_487050 | 57 | PORTED | src/libsys/audio/wav.c | WAV file/memory RIFF parsing and sound load |
| 00487200 | sub_487200 | 36 | PORTED | src/libsys/audio/wav.c | WAV file/memory RIFF parsing and sound load |
| 004872C0 | lsys_init_memory_manager | 9 | cited |  |  |
| 004872F0 | lsys_get_memory | 23 | SKIP |  | Original memory manager / string / module helpers |
| 00487350 | sub_487350 | 13 | SKIP |  | Original memory manager / string / module helpers |
| 00487410 | system_check_memory_487410 | 20 | SKIP |  | Original memory manager / string / module helpers |
| 004874C0 | lsys_random | 5 | cited |  |  |
| 004874E0 | sub_4874E0 | 9 | SKIP |  | Original memory manager / string / module helpers |
| 00487500 | lsys_str_upper | 15 | SKIP |  | Original memory manager / string / module helpers |
| 00487520 | sub_487520 | 32 | SKIP |  | Original memory manager / string / module helpers |
| 004875F0 | ls_get_module_name | 27 | SKIP |  | Original memory manager / string / module helpers |
| 00487680 | sub_487680 | 10 | SKIP |  | Original memory manager / string / module helpers |
| 00487760 | lsys_init_cd_system | 19 | cited |  |  |
| 004878F0 | lsys_stop_cd | 12 | SKIP |  | CD audio control; stub in cd.c |
| 004879D0 | lsys_shut_cd_system | 10 | SKIP |  | CD audio control; stub in cd.c |
| 00487A00 | sub_487A00 | 32 | SKIP |  | Keyboard/mouse/exit Win32 glue replaced by platform layer |
| 00487B20 | lsys_init_mouse_system | 6 | cited |  |  |
| 00487B30 | lsys_shut_mouse_system | 10 | cited |  |  |
| 00487B50 | lsys_get_click_mouse | 6 | SKIP |  | Keyboard/mouse/exit Win32 glue replaced by platform layer |
| 00487B60 | lsys_set_click_mouse | 6 | SKIP |  | Keyboard/mouse/exit Win32 glue replaced by platform layer |
| 00487B70 | lsys_get_mouse | 24 | PORTED | src/libsys/system.h motion_x/mouse_left | Mouse position/buttons/delta; platform layer fills system_t |
| 00487BE0 | sub_487BE0 | 10 | SKIP |  | Keyboard/mouse/exit Win32 glue replaced by platform layer |
| 00487C20 | lsys_set_mouse_range | 11 | SKIP |  | Keyboard/mouse/exit Win32 glue replaced by platform layer |
| 00487C50 | lsys_get_mouse_delta | 32 | PORTED | src/libsys/system.h motion_x/mouse_left | Mouse position/buttons/delta; platform layer fills system_t |
| 00487D20 | sub_487D20 | 5 | SKIP |  | Keyboard/mouse/exit Win32 glue replaced by platform layer |
| 00487D30 | sub_487D30 | 10 | SKIP |  | Keyboard/mouse/exit Win32 glue replaced by platform layer |
| 00487D40 | sub_487D40 | 6 | cited |  |  |
| 00487D60 | lsys_key_map | 5 | SKIP |  | Keyboard/mouse/exit Win32 glue replaced by platform layer |
| 00487D80 | lsys_read_key | 16 | SKIP |  | Keyboard/mouse/exit Win32 glue replaced by platform layer |
| 00487DF0 | lsys_key_pressed | 5 | SKIP |  | Keyboard/mouse/exit Win32 glue replaced by platform layer |
| 00487E00 | sub_487E00 | 16 | SKIP |  | Keyboard/mouse/exit Win32 glue replaced by platform layer |
| 00487E30 | sub_487E30 | 78 | cited |  |  |
| 00487F30 | ls_shut_system | 11 | SKIP |  | Keyboard/mouse/exit Win32 glue replaced by platform layer |
| 00487F40 | lsys_exit | 5 | SKIP |  | Keyboard/mouse/exit Win32 glue replaced by platform layer |
| 00487F50 | lsi_open_stat_app | 39 | cited |  |  |
| 004880C0 | sub_4880C0 | 38 | SKIP |  | Error/log message boxes, DX version, ls1.cfg log: diagnostics |
| 00488220 | sub_488220 | 43 | PORTED | src/libsys/file_system.c:ls_read_* | Endian-aware word/long readers; port reads little-endian directly |
| 00488290 | sub_488290 | 45 | SKIP |  | Buffer/file writer helpers for saver tools |
| 00488390 | sub_488390 | 27 | PORTED | src/libsys/file_system.c:ls_read_* | Endian-aware word/long readers; port reads little-endian directly |
| 004883F0 | sub_4883F0 | 34 | PORTED | src/libsys/file_system.c:ls_read_* | Endian-aware word/long readers; port reads little-endian directly |
| 00488460 | sub_488460 | 33 | PORTED | src/libsys/file_system.c:ls_read_* | Endian-aware word/long readers; port reads little-endian directly |
| 004884D0 | sub_4884D0 | 27 | SKIP |  | Buffer/file writer helpers for saver tools |
| 00488530 | sub_488530 | 36 | SKIP |  | Buffer/file writer helpers for saver tools |
| 004885C0 | sub_4885C0 | 42 | SKIP |  | Buffer/file writer helpers for saver tools |
| 00488A30 | sub_488A30 | 28 | SKIP |  | DirectDraw surface create/lock/blit glue |
| 00488AA0 | sub_488AA0 | 15 | SKIP |  | Error/log message boxes, DX version, ls1.cfg log: diagnostics |
| 00488AD0 | graphics_fill_image_488AD0 | 39 | SKIP |  | DirectDraw surface create/lock/blit glue |
| 00488B60 | sub_488B60 | 12 | SKIP |  | DirectDraw surface create/lock/blit glue |
| 00488B90 | graphics_draw_image_events_488B90 | 31 | SKIP |  | DirectDraw surface create/lock/blit glue |
| 00488C60 | graphics_create_surface_488C60 | 89 | SKIP |  | DirectDraw surface create/lock/blit glue |
| 00488DC0 | sub_488DC0 | 76 | SKIP |  | DirectDraw surface create/lock/blit glue |
| 00488EF0 | sub_488EF0 | 90 | cited |  |  |
| 00489110 | sub_489110 | 11 | SKIP |  | DirectDraw surface create/lock/blit glue |
| 00489130 | lsys_bit_move_256 | 61 | cited |  |  |
| 00489340 | sub_489340 | 52 | cited |  |  |
| 00489440 | graphics_draw_image_489440 | 45 | SKIP |  | DirectDraw surface create/lock/blit glue |
| 004894E0 | graphics_draw_image_fullscreen_4894E0 | 5 | SKIP |  | DirectDraw surface create/lock/blit glue |
| 00489500 | sub_489500 | 10 | SKIP |  | DirectDraw surface create/lock/blit glue |
| 00489590 | sub_489590 | 47 | SKIP |  | DirectDraw surface create/lock/blit glue |
| 00489620 | sub_489620 | 17 | SKIP |  | DirectDraw surface create/lock/blit glue |
| 00489650 | lsys_get_format_info | 154 | cited |  |  |
| 004898F0 | lsi_bit_move_alpha | 284 | cited |  |  |
| 0048A1B0 | lsys_set_timer | 5 | SKIP |  | Win32 timer glue; ls_tick in platform layer |
| 0048A1C0 | lsys_free_timer | 5 | SKIP |  | Win32 timer glue; ls_tick in platform layer |
| 0048A210 | lsys_read_timer | 6 | SKIP |  | Win32 timer glue; ls_tick in platform layer |
| 0048A220 | lsys_pause_timer | 12 | SKIP |  | Win32 timer glue; ls_tick in platform layer |
| 0048A240 | lsys_resume_timer | 19 | SKIP |  | Win32 timer glue; ls_tick in platform layer |
| 0048A270 | ls_wave_error | 5 | SKIP |  | DirectSound/stream buffer glue replaced by platform audio |
| 0048A280 | sub_48A280 | 44 | PORTED | src/libsys/audio/wav.c | WAV file/memory RIFF parsing and sound load |
| 0048A330 | sub_48A330 | 54 | PORTED | src/libsys/audio/wav.c | WAV file/memory RIFF parsing and sound load |
| 0048A490 | sub_48A490 | 49 | PORTED | src/libsys/audio/wav.c | WAV file/memory RIFF parsing and sound load |
| 0048A560 | get_dx_version | 194 | SKIP |  | Error/log message boxes, DX version, ls1.cfg log: diagnostics |
| 0048A910 | lsys_message | 50 | SKIP |  | Error/log message boxes, DX version, ls1.cfg log: diagnostics |
| 0048AAF0 | sys_get_error_code_48AAF0 | 17 | SKIP |  | Error/log message boxes, DX version, ls1.cfg log: diagnostics |
| 0048ABB0 | ls_init_graphic_system_ex | 6 | SKIP |  | DirectDraw video-mode setup/pixel-format glue |
| 0048ABC0 | ls_shut_graphic_system_ex | 13 | SKIP |  | DirectDraw video-mode setup/pixel-format glue |
| 0048AC20 | lsys_init_graphic_system | 35 | SKIP |  | DirectDraw video-mode setup/pixel-format glue |
| 0048ACA0 | sub_48ACA0 | 13 | SKIP |  | DirectDraw video-mode setup/pixel-format glue |
| 0048ACE0 | sub_48ACE0 | 14 | SKIP |  | DirectDraw video-mode setup/pixel-format glue |
| 0048AD40 | sub_48AD40 | 17 | SKIP |  | DirectDraw video-mode setup/pixel-format glue |
| 0048AD90 | sub_48AD90 | 38 | SKIP |  | DirectDraw video-mode setup/pixel-format glue |
| 0048AE50 | lsi_is_displaymode_palettized | 21 | SKIP |  | DirectDraw video-mode setup/pixel-format glue |
| 0048AED0 | sub_48AED0 | 33 | cited |  |  |
| 0048AFB0 | update_display_buffer_0 | 5 | SKIP |  | DirectDraw video-mode setup/pixel-format glue |
| 0048AFD0 | sub_48AFD0 | 34 | SKIP |  | DirectDraw video-mode setup/pixel-format glue |
| 0048B090 | sub_48B090 | 78 | SKIP |  | DirectDraw video-mode setup/pixel-format glue |
| 0048B1D0 | lsi_extract_field | 15 | SKIP |  | DirectDraw video-mode setup/pixel-format glue |
| 0048B200 | sub_48B200 | 85 | SKIP |  | DirectDraw video-mode setup/pixel-format glue |
| 0048B3F0 | lsys_get_filedate | 12 | PORTED | src/libsys/file_system.c | File close/read/write/seek/delete/rename/exists/load wrappers |
| 0048B420 | lsys_open | 5 | cited |  |  |
| 0048B440 | lsys_close | 5 | PORTED | src/libsys/file_system.c | File close/read/write/seek/delete/rename/exists/load wrappers |
| 0048B450 | lsys_read | 29 | PORTED | src/libsys/file_system.c | File close/read/write/seek/delete/rename/exists/load wrappers |
| 0048B4E0 | lsys_write | 31 | PORTED | src/libsys/file_system.c | File close/read/write/seek/delete/rename/exists/load wrappers |
| 0048B580 | lsys_seek | 5 | PORTED | src/libsys/file_system.c | File close/read/write/seek/delete/rename/exists/load wrappers |
| 0048B5A0 | lsys_delete | 5 | PORTED | src/libsys/file_system.c | File close/read/write/seek/delete/rename/exists/load wrappers |
| 0048B5C0 | lsys_rename | 5 | PORTED | src/libsys/file_system.c | File close/read/write/seek/delete/rename/exists/load wrappers |
| 0048B5E0 | lsys_init_mouse_system_ | 6 | SKIP |  | DirectDraw video-mode setup/pixel-format glue |
| 0048B660 | sub_48B660 | 31 | cited |  |  |
| 0048B6D0 | start_pass_upsample | 11 | SKIP |  | PCX/LBM/JPEG loader or saver for dev tools; not used by CIF data path |
| 0048B6F0 | sub_48B6F0 | 23 | SKIP |  | PCX/LBM/JPEG loader or saver for dev tools; not used by CIF data path |
| 0048B750 | sub_48B750 | 136 | SKIP |  | PCX/LBM/JPEG loader or saver for dev tools; not used by CIF data path |
| 0048BC40 | LOBYTE | 77 | SKIP |  | PCX/LBM/JPEG loader or saver for dev tools; not used by CIF data path |
| 0048BE30 | ls_load_pcx_ex | 29 | SKIP |  | PCX/LBM/JPEG loader or saver for dev tools; not used by CIF data path |
| 0048BEB0 | sub_48BEB0 | 5 | SKIP |  | Display-depth bitmap remap and image-dispatch; port uses 32-bit XRGB pipeline |
| 0048BEC0 | ls_gif_decoder | 272 | SKIP |  | GIF decoder; shipped data uses pre-converted CIF files |
| 0048C570 | ls_gif_clip_outline | 23 | SKIP |  | GIF decoder; shipped data uses pre-converted CIF files |
| 0048C5D0 | sub_48C5D0 | 236 | cited |  |  |
| 0048C920 | lsys_is_path_exist | 18 | PORTED | src/libsys/file_system.c | File close/read/write/seek/delete/rename/exists/load wrappers |
| 0048C9C0 | f | 37 | cited |  |  |
| 0048C9F0 | ls_load_lbm_pal | 42 | SKIP |  | PCX/LBM/JPEG loader or saver for dev tools; not used by CIF data path |
| 0048CA80 | ls_load_params | 89 | SKIP |  | PCX/LBM/JPEG loader or saver for dev tools; not used by CIF data path |
| 0048CB80 | ls_iff_set_word | 17 | SKIP |  | PCX/LBM/JPEG loader or saver for dev tools; not used by CIF data path |
| 0048CBD0 | ls_load_params | 198 | SKIP |  | PCX/LBM/JPEG loader or saver for dev tools; not used by CIF data path |
| 0048CEA0 | ls_save_lbm_pal | 96 | SKIP |  | PCX/LBM/JPEG loader or saver for dev tools; not used by CIF data path |
| 0048D1E0 | ls_save_lbm | 127 | SKIP |  | PCX/LBM/JPEG loader or saver for dev tools; not used by CIF data path |
| 0048D4B0 | build_plane_line | 35 | SKIP |  | PCX/LBM/JPEG loader or saver for dev tools; not used by CIF data path |
| 0048D510 | lock_stream_sound_2 | 21 | SKIP |  | DirectSound/stream buffer glue replaced by platform audio |
| 0048D550 | unlock_stream_sound_2 | 18 | SKIP |  | DirectSound/stream buffer glue replaced by platform audio |
| 0048D590 | ls_init_sound_stream | 59 | SKIP |  | DirectSound/stream buffer glue replaced by platform audio |
| 0048D690 | sub_48D690 | 6 | SKIP |  | DirectSound/stream buffer glue replaced by platform audio |
| 0048D6B0 | sub_48D6B0 | 17 | SKIP |  | DirectSound/stream buffer glue replaced by platform audio |
| 0048D6E0 | sub_48D6E0 | 11 | SKIP |  | DirectSound/stream buffer glue replaced by platform audio |
| 0048D890 | seb_traite_zik | 44 | cited |  |  |
| 0048D8E7 | seb_traite_zik_flip | 44 | cited |  |  |
| 0048D940 | sub_48D940 | 30 | cited |  |  |
| 0048DA10 | sub_48DA10 | 17 | cited |  |  |
| 0048DA70 | audio_create_buffer_48DA70 | 76 | SKIP |  | DirectSound/stream buffer glue replaced by platform audio |
| 0048DC00 | audio_releasse_48DC00 | 25 | SKIP |  | DirectSound/stream buffer glue replaced by platform audio |
| 0048DC60 | sub_48DC60 | 15 | cited |  |  |
| 0048DCA0 | sub_48DCA0 | 11 | cited |  |  |
| 0048DCF0 | lsys_play_cd | 82 | cited |  |  |
| 0048DE30 | sub_48DE30 | 42 | cited |  |  |
| 0048DED0 | sub_48DED0 | 31 | cited |  |  |
| 0048DF40 | sub_48DF40 | 29 | cited |  |  |
| 0048DFB0 | sub_48DFB0 | 17 | cited |  |  |
| 0048E030 | sub_48E030 | 14 | cited |  |  |
| 0048E070 | sub_48E070 | 6 | cited |  |  |
| 0048E080 | sub_48E080 | 99 | cited |  |  |
| 0048E280 | sub_48E280 | 10 | cited |  |  |
| 0048E2A0 | sub_48E2A0 | 13 | cited |  |  |
| 0048E2D0 | sub_48E2D0 | 9 | SKIP |  | DirectSound/stream buffer glue replaced by platform audio |
| 0048E2F8 | LOBYTE | 48 | cited |  |  |
| 0048E3A8 | HIWORD | 89 | cited |  |  |
| 0048E491 | sub_48E8AE | 10 | PORTED | src/music.c | MOD tracker internals (effects, state reset, format probe) |
| 0048E4A9 | sub_48E4A9 | 36 | cited |  |  |
| 0048E516 | sub_48E516 | 55 | cited |  |  |
| 0048E5B7 | sub_48E5B7 | 22 | cited |  |  |
| 0048E5E2 | sub_48E5E2 | 95 | cited |  |  |
| 0048E6EB | __ROL2__ | 118 | cited |  |  |
| 0048E879 | sub_48E879 | 24 | cited |  |  |
| 0048E8A2 | sub_48E8A2 | 9 | PORTED | src/music.c | MOD tracker internals (effects, state reset, format probe) |
| 0048E8AE | sub_48E8AE | 16 | cited |  |  |
| 0048E8C8 | sub_48E8C8 | 32 | cited |  |  |
| 0048E922 | f | 6 | cited |  |  |
| 0048E927 | f | 7 | cited |  |  |
| 0048E930 | LOBYTE | 20 | cited |  |  |
| 0048E953 | JUMPOUT | 6 | PORTED | src/music.c | MOD tracker internals (effects, state reset, format probe) |
| 0048E957 | LOBYTE | 12 | cited |  |  |
| 0048E966 | LOBYTE | 20 | cited |  |  |
| 0048E989 | f | 14 | cited |  |  |
| 0048E9AD | f | 14 | cited |  |  |
| 0048E9C3 | f | 8 | cited |  |  |
| 0048E9CD | f | 12 | cited |  |  |
| 0048E9EA | f | 18 | cited |  |  |
| 0048EA02 | f | 14 | cited |  |  |
| 0048EA1E | f | 14 | cited |  |  |
| 0048EA3B | f | 9 | cited |  |  |
| 0048EA48 | f | 27 | cited |  |  |
| 0048EA6B | f | 11 | cited |  |  |
| 0048EA76 | f | 11 | cited |  |  |
| 0048EA83 | __OFSUB__ | 13 | PORTED | src/music.c | MOD tracker internals (effects, state reset, format probe) |
| 0048EA8D | f | 9 | cited |  |  |
| 0048EA93 | f | 10 | cited |  |  |
| 0048EA9B | f | 14 | cited |  |  |
| 0048EABD | f | 14 | cited |  |  |
| 0048EADD | f | 14 | cited |  |  |
| 0048EAFE | sub_48EAFE | 33 | cited |  |  |
| 0048EB37 | sub_48EB37 | 25 | cited |  |  |
| 0048EB8D | sub_48EB8D | 6 | cited |  |  |
| 0048EB97 | sub_48EB97 | 6 | cited |  |  |
| 0048EB9E | __OFADD__ | 26 | cited |  |  |
| 0048EBE0 | __OFSUB__ | 21 | cited |  |  |
| 0048EBF9 | f | 20 | cited |  |  |
| 0048EC1A | sub_48EC1A | 13 | cited |  |  |
| 0048EC2B | f | 15 | cited |  |  |
| 0048EC3B | sub_48EC3B | 45 | cited |  |  |
| 0048ECBB | sub_48ECBB | 28 | cited |  |  |
| 0048ECF4 | f | 12 | cited |  |  |
| 0048ED08 | LOBYTE | 20 | cited |  |  |
| 0048ED3B | memset | 61 | cited |  |  |
| 0048EDBC | LOWORD | 21 | cited |  |  |
| 0048EE01 | LOWORD | 250 | cited |  |  |
| 0048F03C | sub_48F03C | 19 | PORTED | src/music.c | MOD tracker internals (effects, state reset, format probe) |
| 0048F06F | sub_48F06F | 31 | PORTED | src/music.c | MOD tracker internals (effects, state reset, format probe) |
| 0048F0B6 | sub_48F0B6 | 14 | cited |  |  |
| 0048F0F5 | sub_48F0F5 | 9 | cited |  |  |
| 0048F11B | sub_48F11B | 9 | PORTED | src/music.c | MOD tracker internals (effects, state reset, format probe) |
| 0048F138 | sub_48F138 | 22 | PORTED | src/music.c | MOD tracker internals (effects, state reset, format probe) |
| 0048F190 | preload_shading_table | 17 | cited |  |  |
| 0048F1D0 | sub_48F1D0 | 39 | cited |  |  |
| 0048F230 | sub_48F230 | 35 | cited |  |  |
| 0048F2B0 | set_render_mode | 5 | SKIP |  | Tiny setter/release thunk on render context |
| 0048F2C0 | sub_48F2C0 | 55 | cited |  |  |
| 0048F390 | sub_48F390 | 11 | SKIP |  | Tiny setter/release thunk on render context |
| 0048F3B4 | sub_48F3B4 | 18 | PORTED | src/obc.c:fmat_* + src/model.c | Fixed-point Q14 matrix/vector transform helpers inlined in port |
| 0048F424 | transform_vector | 15 | cited |  |  |
| 0048F4B4 | sub_48F4B4 | 12 | PORTED | src/obc.c:fmat_* + src/model.c | Fixed-point Q14 matrix/vector transform helpers inlined in port |
| 0048F53C | sub_48F53C | 16 | PORTED | src/obc.c:fmat_* + src/model.c | Fixed-point Q14 matrix/vector transform helpers inlined in port |
| 0048F5B0 | sub_48F5B0 | 21 | PORTED | src/obc.c:fmat_* + src/model.c | Fixed-point Q14 matrix/vector transform helpers inlined in port |
| 0048FA20 | create_rotation_matrix | 44 | cited |  |  |
| 0048FCD0 | sub_48FCD0 | 28 | PARTIAL | src/model.c | Per-axis 8.8 scale of model matrix (default 256) not applied in port renderer — Scaled models would render at unit size; most use 256 |
| 0048FD60 | create_position_matrix | 21 | cited |  |  |
| 0048FDA0 | sub_48FDA0 | 26 | PORTED | src/scene.c / src/model.c | High-precision vector rotate by camera matrix, inlined |
| 0048FEB0 | sub_48FEB0 | 70 | PORTED | src/obc.c:fmat_* + src/model.c | Fixed-point Q14 matrix/vector transform helpers inlined in port |
| 0048FFB0 | sub_48FFB0 | 9 | PORTED | src/scene.c / src/model.c | High-precision vector rotate by camera matrix, inlined |
| 004901E0 | transform_vertex | 40 | cited |  |  |
| 004902C0 | sub_4902C0 | 52 | SKIP |  | Matrix/rotation helper in the INVENT.DLL host-API table; inventory.c does its own object rotation — low |
| 004903C0 | multiply_matrices | 23 | cited |  |  |
| 00490520 | sub_490520 | 61 | PORTED | src/model.c:model_render_world | Per-face light intensity from normal dot light direction; port computes shade 0..63 per vertex |
| 00490620 | sub_490620 | 25 | cited |  |  |
| 00490690 | sub_490690 | 25 | PORTED | src/model.c:model_from_atoms | Quad as two triangles dispatch wrappers; port splits quads to triangles at model build |
| 00490700 | sub_490700 | 25 | PORTED | src/model.c:model_from_atoms | Quad as two triangles dispatch wrappers; port splits quads to triangles at model build |
| 00490770 | sub_490770 | 13 | PORTED | src/model.c:model_from_atoms | Quad as two triangles dispatch wrappers; port splits quads to triangles at model build |
| 004907B0 | sub_4907B0 | 17 | PORTED | src/model.c:model_from_atoms | Quad as two triangles dispatch wrappers; port splits quads to triangles at model build |
| 00490820 | sub_490820 | 27 | PORTED | src/model.c:model_from_atoms | Quad as two triangles dispatch wrappers; port splits quads to triangles at model build |
| 00490898 | sub_490898 | 5 | SKIP |  | Pixel/line/wireframe outline primitives (debug render modes); game faces are filled |
| 004908B7 | sub_4908B7 | 400 | SKIP |  | Pixel/line/wireframe outline primitives (debug render modes); game faces are filled |
| 00490B05 | sub_490B05 | 25 | SKIP |  | Pixel/line/wireframe outline primitives (debug render modes); game faces are filled |
| 00490B86 | sub_490B86 | 32 | PORTED | src/model.c:model_from_atoms | Quad as two triangles dispatch wrappers; port splits quads to triangles at model build |
| 00490C30 | sub_490C30 | 30 | PARTIAL | src/model.c:fill_tri_shaded | Flat-colour/untextured-flat triangle fill kernel; port draws every face through the shaded gouraud kernel — Flat-shaded faces get gouraud ramp instead; small visual differences |
| 00490C5E | sub_490C5E | 802 | PARTIAL | src/model.c:fill_tri_shaded | Flat-colour/untextured-flat triangle fill kernel; port draws every face through the shaded gouraud kernel — Flat-shaded faces get gouraud ramp instead; small visual differences |
| 00491420 | sub_491420 | 6 | SKIP |  | Fatal "libsys3d null pointer" error stub for unused primitive-table slots |
| 00491450 | sub_491450 | 7 | cited |  |  |
| 00491470 | sub_491470 | 75 | cited |  |  |
| 004915C0 | sub_4915C0 | 79 | cited |  |  |
| 004917B0 | validate_scale_parameters | 54 | PORTED | src/scene.c:scene_project_world | Projects camera-space point to screen with on-screen test; port projection does same maths |
| 00491DD0 | initialize_object | 12 | SKIP |  | Lock/unlock bmap and compute pixel pointer; platform framebuffer replaces bmap locking |
| 00491E00 | deinitialize_object | 12 | SKIP |  | Lock/unlock bmap and compute pixel pointer; platform framebuffer replaces bmap locking |
| 00491E20 | init_all_systems | 19 | cited |  |  |
| 00491E90 | sub_491E90 | 9 | SKIP |  | Exit cleanup: free bmap and ls_shut_down; platform layer handles shutdown |
| 00491ED0 | sub_491ED0 | 18 | cited |  |  |
| 00491F00 | sub_491F00 | 13 | SKIP |  | Releases 16-bit gouraud LUT at exit; process teardown frees it |
| 00491FD0 | sub_491FD0 | 127 | cited |  |  |
| 00492310 | sub_492310 | 21 | cited |  |  |
| 00492AC0 | sub_492AC0 | 96 | PARTIAL | src/actor.c:actor_blend_pose | Double-precision 3x3 matrix inverse/multiply/Euler helpers for slerp pose blend; port lacks that blend branch — Clips with CAN flag 0x800 get plain angle lerp; wrong rotation paths or pops |
| 00492BF0 | sub_492BF0 | 42 | PARTIAL | src/actor.c:actor_blend_pose | Double-precision 3x3 matrix inverse/multiply/Euler helpers for slerp pose blend; port lacks that blend branch — Clips with CAN flag 0x800 get plain angle lerp; wrong rotation paths or pops |
| 00492C60 | sub_492C60 | 9 | cited |  |  |
| 00492CF0 | sub_492CF0 | 28 | PARTIAL | src/actor.c:actor_blend_pose | Double-precision 3x3 matrix inverse/multiply/Euler helpers for slerp pose blend; port lacks that blend branch — Clips with CAN flag 0x800 get plain angle lerp; wrong rotation paths or pops |
| 00492DA0 | sub_492DA0 | 36 | PARTIAL | src/actor.c:actor_blend_pose | Double-precision 3x3 matrix inverse/multiply/Euler helpers for slerp pose blend; port lacks that blend branch — Clips with CAN flag 0x800 get plain angle lerp; wrong rotation paths or pops |
| 00492EB0 | sub_492EB0 | 23 | PARTIAL | src/actor.c:actor_blend_pose | Double-precision 3x3 matrix inverse/multiply/Euler helpers for slerp pose blend; port lacks that blend branch — Clips with CAN flag 0x800 get plain angle lerp; wrong rotation paths or pops |
| 00492F30 | sub_492F30 | 61 | PARTIAL | src/actor.c:actor_blend_pose | Double-precision 3x3 matrix inverse/multiply/Euler helpers for slerp pose blend; port lacks that blend branch — Clips with CAN flag 0x800 get plain angle lerp; wrong rotation paths or pops |
| 004930F0 | sub_4930F0 | 74 | PORTED | src/model.c:fill_tri_shaded | Triangle/quad submit packing vertex attributes into rasterizer table; port calls fill kernel directly |
| 004931E0 | sub_4931E0 | 89 | PORTED | src/model.c:fill_tri_shaded | Triangle/quad submit packing vertex attributes into rasterizer table; port calls fill kernel directly |
| 00493300 | sub_493300 | 22 | SKIP |  | Matrix/rotation helper in the INVENT.DLL host-API table; inventory.c does its own object rotation — low |
| 00493340 | sub_493340 | 24 | SKIP |  | Matrix/rotation helper in the INVENT.DLL host-API table; inventory.c does its own object rotation — low |
| 004933D0 | sub_4933D0 | 24 | SKIP |  | Matrix/rotation helper in the INVENT.DLL host-API table; inventory.c does its own object rotation — low |
| 00493460 | sub_493460 | 24 | SKIP |  | Matrix/rotation helper in the INVENT.DLL host-API table; inventory.c does its own object rotation — low |
| 004934F0 | sub_4934F0 | 23 | SKIP |  | Matrix/rotation helper in the INVENT.DLL host-API table; inventory.c does its own object rotation — low |
| 00493540 | sub_493540 | 23 | SKIP |  | Matrix/rotation helper in the INVENT.DLL host-API table; inventory.c does its own object rotation — low |
| 00493590 | sub_493590 | 23 | SKIP |  | Matrix/rotation helper in the INVENT.DLL host-API table; inventory.c does its own object rotation — low |
| 004935E0 | sub_4935E0 | 23 | SKIP |  | Matrix/rotation helper in the INVENT.DLL host-API table; inventory.c does its own object rotation — low |
| 00493630 | sub_493630 | 50 | SKIP |  | Matrix/rotation helper in the INVENT.DLL host-API table; inventory.c does its own object rotation — low |
| 00493740 | sub_493740 | 50 | SKIP |  | Matrix/rotation helper in the INVENT.DLL host-API table; inventory.c does its own object rotation — low |
| 00493850 | sub_493850 | 26 | cited |  |  |
| 004938E0 | LODWORD | 162 | PORTED | src/model.c:model_render_batch / src/inventory.c | Host-API draw-object-with-bounds/light into bmap; port renders item models directly |
| 00493C10 | sub_493C10 | 165 | PORTED | src/model.c:model_render_batch / src/inventory.c | Host-API draw-object-with-bounds/light into bmap; port renders item models directly |
| 00493FD0 | sub_493FD0 | 11 | SKIP |  | Host-API clear-rect (ls_fill_rect) for INVENT.DLL; port clears via platform blit |
| 00494510 | sub_494510 | 22 | SKIP |  | Aligns 16KB display/palette shade buffers in static memory; port holds GRD in plain arrays |
| 00494560 | sub_494560 | 50 | cited |  |  |
| 004945F0 | sub_4945F0 | 541 | cited |  |  |
| 00495390 | sub_495390 | 8 | cited |  |  |
| 004953B0 | sub_4953B0 | 7 | cited |  |  |
| 004953D0 | sub_4953D0 | 68 | PARTIAL | src/model.c:fill_tri_shaded | Flat-colour/untextured-flat triangle fill kernel; port draws every face through the shaded gouraud kernel — Flat-shaded faces get gouraud ramp instead; small visual differences |
| 004954B0 | sub_4954B0 | 253 | PARTIAL | src/model.c:fill_tri_shaded | Flat-colour/untextured-flat triangle fill kernel; port draws every face through the shaded gouraud kernel — Flat-shaded faces get gouraud ramp instead; small visual differences |
| 00495AC8 | HIWORD | 54 | PORTED | src/model.c:fill_tri_shaded | Opaque inner pixel loop of textured/ramp gouraud kernel; ported as affine per-pixel loop |
| 00495B56 | JUMPOUT | 60 | PARTIAL | src/model.c:fill_tri_shaded | Colour-key transparent (flag 0x200) pixel-loop variant of textured kernels; port has no index-0 skip — Transparent texel faces (grates, leaves, hair cards) draw opaque black/solid |
| 00495BFA | sub_495BFA | 1129 | cited |  |  |
| 00496738 | BYTE1 | 50 | PARTIAL | src/model.c:fill_tri_shaded | Colour-key transparent (flag 0x200) pixel-loop variant of textured kernels; port has no index-0 skip — Transparent texel faces (grates, leaves, hair cards) draw opaque black/solid |
| 004967B1 | JUMPOUT | 49 | PARTIAL | src/model.c:fill_tri_shaded | Colour-key transparent (flag 0x200) pixel-loop variant of textured kernels; port has no index-0 skip — Transparent texel faces (grates, leaves, hair cards) draw opaque black/solid |
| 00496833 | sub_496833 | 941 | PARTIAL | src/model.c:fill_tri_shaded | Textured kernel variants (palette-offset lit / unlit texel) ; port has one lit affine textured kernel — Unlit or palette-tinted texture modes render with standard GRD shading |
| 00497188 | BYTE1 | 97 | PARTIAL | src/model.c:fill_tri_shaded | Colour-key transparent (flag 0x200) pixel-loop variant of textured kernels; port has no index-0 skip — Transparent texel faces (grates, leaves, hair cards) draw opaque black/solid |
| 00497266 | JUMPOUT | 48 | PARTIAL | src/model.c:fill_tri_shaded | Colour-key transparent (flag 0x200) pixel-loop variant of textured kernels; port has no index-0 skip — Transparent texel faces (grates, leaves, hair cards) draw opaque black/solid |
| 004972E0 | sub_4972E0 | 936 | PARTIAL | src/model.c:fill_tri_shaded | Textured kernel variants (palette-offset lit / unlit texel) ; port has one lit affine textured kernel — Unlit or palette-tinted texture modes render with standard GRD shading |
| 00497C20 | LOBYTE | 87 | PORTED | src/model.c:fill_tri_shaded | Opaque inner pixel loop of textured/ramp gouraud kernel; ported as affine per-pixel loop |
| 00497CD9 | __OFSUB__ | 749 | cited |  |  |
| 004982F0 | sub_4982F0 | 12 | PARTIAL | src/model.c:fill_tri_shaded | Flat-colour/untextured-flat triangle fill kernel; port draws every face through the shaded gouraud kernel — Flat-shaded faces get gouraud ramp instead; small visual differences |
| 00498330 | sub_498330 | 409 | PARTIAL | src/model.c:model_render_world | Polygon clip against view frustum/near plane producing new vertices; port only culls and clamps to bbox — Triangles crossing near plane are dropped instead of clipped; edge popping |
| 00498C68 | sub_498C68 | 15 | PORTED | src/model.c:model_render_world | Quicksort of polygons by depth (painter order); port sorts faces by summed camera depth |
| 00498C84 | sub_498C84 | 48 | PORTED | src/model.c:model_render_world | Quicksort of polygons by depth (painter order); port sorts faces by summed camera depth |
| 00498F30 | sub_498F30 | 36 | SKIP |  | Pixel/line/wireframe outline primitives (debug render modes); game faces are filled |
| 00498FD0 | sub_498FD0 | 117 | SKIP |  | Pixel/line/wireframe outline primitives (debug render modes); game faces are filled |
| 00499170 | sub_499170 | 117 | SKIP |  | Pixel/line/wireframe outline primitives (debug render modes); game faces are filled |
| 00499310 | sub_499310 | 12 | SKIP |  | Pixel/line/wireframe outline primitives (debug render modes); game faces are filled |
| 00499360 | sub_499360 | 13 | SKIP |  | Pixel/line/wireframe outline primitives (debug render modes); game faces are filled |
| 004993C0 | sub_4993C0 | 12 | SKIP |  | Pixel/line/wireframe outline primitives (debug render modes); game faces are filled |
| 00499410 | sub_499410 | 13 | SKIP |  | Pixel/line/wireframe outline primitives (debug render modes); game faces are filled |
| 00499470 | sub_499470 | 120 | SKIP |  | Pixel/line/wireframe outline primitives (debug render modes); game faces are filled |
| 00499630 | sub_499630 | 215 | PORTED | src/model.c:model_render_world | Vertex transform/project/light stage and projection setup; port does rotate, perspective divide, per-vertex shade |
| 00499990 | sub_499990 | 73 | PORTED | src/model.c:model_render_world | Vertex transform/project/light stage and projection setup; port does rotate, perspective divide, per-vertex shade |
| 00499AD0 | sub_499AD0 | 43 | PORTED | src/model.c:model_render_world | Vertex transform/project/light stage and projection setup; port does rotate, perspective divide, per-vertex shade |
| 00499C20 | sub_499C20 | 44 | PORTED | src/model.c:model_render_world | Vertex transform/project/light stage and projection setup; port does rotate, perspective divide, per-vertex shade |
| 00499CE0 | sub_499CE0 | 462 | PORTED | src/model.c:model_render_world | Vertex transform/project/light stage and projection setup; port does rotate, perspective divide, per-vertex shade |
| 0049A790 | sub_49A790 | 37 | PARTIAL | src/model.c:model_render_world | Edge-plane intersection vertex interpolation used by clipper; not ported — Same as clipper: no clipped geometry |
| 0049A950 | sub_49A950 | 52 | PORTED | src/model.c:model_render_world | Vertex transform/project/light stage and projection setup; port does rotate, perspective divide, per-vertex shade |
| 0049AA40 | sub_49AA40 | 25 | SKIP |  | 16-bit twins of pixel/line/flat/quad wrappers in primitive table; port renders RGBA32 directly |
| 0049AAB0 | sub_49AAB0 | 25 | SKIP |  | 16-bit twins of pixel/line/flat/quad wrappers in primitive table; port renders RGBA32 directly |
| 0049AB20 | sub_49AB20 | 25 | SKIP |  | 16-bit twins of pixel/line/flat/quad wrappers in primitive table; port renders RGBA32 directly |
| 0049AB90 | sub_49AB90 | 13 | SKIP |  | 16-bit twins of pixel/line/flat/quad wrappers in primitive table; port renders RGBA32 directly |
| 0049ABD0 | sub_49ABD0 | 20 | SKIP |  | 16-bit twins of pixel/line/flat/quad wrappers in primitive table; port renders RGBA32 directly |
| 0049AC50 | sub_49AC50 | 13 | SKIP |  | 16-bit twins of pixel/line/flat/quad wrappers in primitive table; port renders RGBA32 directly |
| 0049AC90 | sub_49AC90 | 9 | SKIP |  | 16-bit twins of pixel/line/flat/quad wrappers in primitive table; port renders RGBA32 directly |
| 0049ACC0 | sub_49ACC0 | 107 | SKIP |  | 16-bit twins of pixel/line/flat/quad wrappers in primitive table; port renders RGBA32 directly |
| 0049AE30 | sub_49AE30 | 28 | SKIP |  | 16-bit twins of pixel/line/flat/quad wrappers in primitive table; port renders RGBA32 directly |
| 0049AEB0 | sub_49AEB0 | 35 | SKIP |  | 16-bit twins of pixel/line/flat/quad wrappers in primitive table; port renders RGBA32 directly |
| 0049AF50 | HIWORD | 55 | PARTIAL | src/model.c:fill_tri_shaded | Inner pixel-loop pieces of 16-bit kernels (opaque / transparent); port uses RGBA32 path — Transparency variant missing as for 8-bit |
| 0049AFE0 | JUMPOUT | 61 | PARTIAL | src/model.c:fill_tri_shaded | Inner pixel-loop pieces of 16-bit kernels (opaque / transparent); port uses RGBA32 path — Transparency variant missing as for 8-bit |
| 0049B086 | sub_49B086 | 1126 | PARTIAL | src/model.c:fill_tri_shaded | 16-bit-colour twin of 8-bit textured/ramp kernel; port shades palette+GRD into RGBA32 — Colour-depth path absent; visually equal to 8-bit path |
| 0049BBB8 | BYTE1 | 45 | PARTIAL | src/model.c:fill_tri_shaded | Inner pixel-loop pieces of 16-bit kernels (opaque / transparent); port uses RGBA32 path — Transparency variant missing as for 8-bit |
| 0049BC39 | JUMPOUT | 53 | PARTIAL | src/model.c:fill_tri_shaded | Inner pixel-loop pieces of 16-bit kernels (opaque / transparent); port uses RGBA32 path — Transparency variant missing as for 8-bit |
| 0049BCC8 | sub_49BCC8 | 941 | PARTIAL | src/model.c:fill_tri_shaded | 16-bit-colour twin of 8-bit textured/ramp kernel; port shades palette+GRD into RGBA32 — Colour-depth path absent; visually equal to 8-bit path |
| 0049C620 | BYTE1 | 50 | PARTIAL | src/model.c:fill_tri_shaded | Inner pixel-loop pieces of 16-bit kernels (opaque / transparent); port uses RGBA32 path — Transparency variant missing as for 8-bit |
| 0049C6A0 | JUMPOUT | 47 | PARTIAL | src/model.c:fill_tri_shaded | Inner pixel-loop pieces of 16-bit kernels (opaque / transparent); port uses RGBA32 path — Transparency variant missing as for 8-bit |
| 0049C727 | sub_49C727 | 936 | PARTIAL | src/model.c:fill_tri_shaded | 16-bit-colour twin of 8-bit textured/ramp kernel; port shades palette+GRD into RGBA32 — Colour-depth path absent; visually equal to 8-bit path |
| 0049D064 | BYTE1 | 67 | PARTIAL | src/model.c:fill_tri_shaded | Inner pixel-loop pieces of 16-bit kernels (opaque / transparent); port uses RGBA32 path — Transparency variant missing as for 8-bit |
| 0049D110 | __OFSUB__ | 748 | PARTIAL | src/model.c:fill_tri_shaded | 16-bit-colour twin of 8-bit textured/ramp kernel; port shades palette+GRD into RGBA32 — Colour-depth path absent; visually equal to 8-bit path |
| 0049D720 | sub_49D720 | 15 | SKIP |  | 16-bit twins of pixel/line/flat/quad wrappers in primitive table; port renders RGBA32 directly |
| 0049D768 | sub_49D768 | 30 | SKIP |  | 16-bit twins of pixel/line/flat/quad wrappers in primitive table; port renders RGBA32 directly |
| 0049D796 | sub_49D796 | 729 | PARTIAL | src/model.c:fill_tri_shaded | 16-bit-colour twin of 8-bit textured/ramp kernel; port shades palette+GRD into RGBA32 — Colour-depth path absent; visually equal to 8-bit path |
| 0049DFCC | sub_49DFCC | 6 | SKIP |  | Tiny CRT init thunk |
| 0049DFEB | sub_49DFEB | 5 | SKIP |  | Tiny CRT init thunk |
| 0049DFF3 | sub_49DFF3 | 71 | SKIP |  | 3DF (3dfx Glide) texture file loader/converter; hardware path unused by software port |
| 0049E190 | f | 10 | cited |  |  |
| 0049E1C0 | convert_3dfinfo_to_texinfo | 24 | SKIP |  | 3DF (3dfx Glide) texture file loader/converter; hardware path unused by software port |
| 0049E200 | sub_49E200 | 33 | SKIP |  | 3DF (3dfx Glide) texture file loader/converter; hardware path unused by software port |
| 0049E2A4 | sub_49E2A4 | 798 | SKIP |  | 3DF (3dfx Glide) texture file loader/converter; hardware path unused by software port |
| 0049E9B0 | sub_49E9B0 | 15 | SKIP |  | MSVC C runtime float/time/math-error helper |
| 0049EB20 | sqrt_function | 5 | SKIP |  | sqrt / fp math thunk; libm replaces it |
| 0049FAA0 | sub_49FAA0 | 5 | SKIP |  | sqrt / fp math thunk; libm replaces it |
| 0049FAAA | sub_49FAAA | 5 | SKIP |  | sqrt / fp math thunk; libm replaces it |
| 0049FAB1 | sub_49FAB1 | 5 | cited |  |  |
| 004A2040 | sub_4A2040 | 16 | SKIP |  | lsys_delete/lsys_rename file wrappers; platform file_system replaces them |
| 004A2080 | sub_4A2080 | 16 | SKIP |  | lsys_delete/lsys_rename file wrappers; platform file_system replaces them |
| 004A4FE0 | sub_4A4FE0 | 137 | SKIP |  | MSVC C runtime float/time/math-error helper |
| 004A9B30 | sub_4A9B30 | 6 | SKIP |  | MSVC C runtime float/time/math-error helper |
| 004A9B50 | sub_4A9B50 | 6 | SKIP |  | MSVC C runtime float/time/math-error helper |
| 004A9C00 | sub_4A9C00 | 10 | SKIP |  | MSVC C runtime float/time/math-error helper |
| 004A9C80 | sub_4A9C80 | 10 | SKIP |  | MSVC C runtime float/time/math-error helper |
| 004A9E80 | sub_4A9E80 | 6 | SKIP |  | MSVC C runtime float/time/math-error helper |
