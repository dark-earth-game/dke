__int16 __usercall ls_main@<ax>(
        int arg_index@<edx>,
        __int16 a2@<cx>,
        int a3@<ebx>,
        int *command_line_params@<edi>,
        int a5@<esi>)
{
  int image_color_data; // [esp+14h] [ebp-2Ch]
  int v9; // [esp+14h] [ebp-2Ch]
  int compressed; // [esp+18h] [ebp-28h]
  int v11; // [esp+18h] [ebp-28h]
  int i; // [esp+1Ch] [ebp-24h]
  char v13[4]; // [esp+20h] [ebp-20h] BYREF
  char v14[4]; // [esp+24h] [ebp-1Ch] BYREF
  char v15[8]; // [esp+28h] [ebp-18h] BYREF
  unsigned int available_memory; // [esp+30h] [ebp-10h]
  __int16 audio_sample_rate[2]; // [esp+38h] [ebp-8h] BYREF
  int audio_bit_depth; // [esp+3Ch] [ebp-4h]

  load_configuration();
  strcpy(game_path, aHdiskEarth);
  if ( a2 == 2 )
  {
    colour_mode_0_8bit_1_16bit = strcmp(*(const char **)(arg_index + 4), off_4B7378) == 0;
    if ( !strcmp(*(const char **)(arg_index + 4), a256) )
      colour_mode_0_8bit_1_16bit = 0;
  }
  system_check_memory_487410(v15);
  if ( available_memory < 0x1F00000 )
    colour_mode_0_8bit_1_16bit = 0;
  sub_4843F0(v13, v14);
  sub_4953B0();
  lsi_set_window_title(aDarkearth);
  sub_491450();
  sub_495390();
  if ( audio_enabled )
  {
    audio_sample_rate[0] = 22050;
    audio_sample_rate[1] = 2;
    audio_bit_depth = 16;
    lsys_set_sound_format(0, audio_sample_rate);
  }
  if ( colour_mode_0_8bit_1_16bit )
  {
    graphics_init(128, 16, 11000000, 8, 1);
    sub_418679(*(_DWORD *)(graphics_context + 44));
    colour_mode_0_8bit_1_16bit = *(_DWORD *)(graphics_context + 44);
    initialize_color_mapping_tables();
  }
  else
  {
    graphics_init(128, 8, 11000000, 8, 1);
    _cfltcvt_init();
  }
  ls_init_movie(0, 0);
  if ( debug_enabled )
  {
    set_position_coordinates(0, 32);
    draw_text(aInitLibsysFini);
  }
  sub_480AF0(sub_461AF6);
  if ( !audio_enabled )
  {
    samples_enabled = 0;
    music_enabled = 0;
  }
  if ( debug_enabled )
  {
    set_position_coordinates(0, 32);
    draw_text(aInitMemory);
  }
  ls_get_module_name(byte_548C00);
  timing_offset = timing_multiplier * ls_read_timer();
  _strlwr(byte_548C00);
  ui_health_buffer = ls_get_heap(1800000);
  dword_5491B4 = ls_get_heap(1000000);
  ui_inventory_slot_buffer = ls_get_heap(318928);
  ui_weapon_slot_buffer = ls_get_heap(159464);
  environment_overlay_buffer = ls_get_heap(422000);
  particle_effect_buffer = ls_get_heap(dword_4B8444);
  weather_effect_buffer = ls_get_heap(dword_4B8444);
  lighting_effect_buffer = ls_get_heap(dword_4B8444);
  hit_effect_buffer = ls_get_heap(dword_4B8444);
  magic_effect_buffer = ls_get_heap(dword_4B8444);
  projectile_effect_buffer = ls_get_heap(dword_4B8444);
  shield_effect_buffer = ls_get_heap(dword_4B8444);
  special_attack_buffer = ls_get_heap(dword_4B8444);
  dword_4B8444 = ls_max_memory_heap(particle_effect_buffer);
  dword_5491A0 = ls_get_heap(316384);
  dword_5491B0 = ls_get_heap(4000000);
  g_creature_data = ls_get_heap(117920);
  dword_5491E4 = ls_get_heap(413696);
  ui_special_meter_buffer = ls_get_heap(1000000);
  ui_status_effects_buffer = ls_get_heap(50);
  sub_442D45();
  audio_create_buffer_48DA70();
  sub_48E280(1);
  byte_4EDB00 = 0;
  if ( is_using_hdd )
  {
    sprintf(game_path, "%s>", (const char *)&word_55C770 + 42);
    game_set_game_path_485CC0(&unk_4B73CC);
    sprintf(&game_temp_path, "%sDATAS", game_path);
    if ( ls_is_path_exist(&game_temp_path) )
      strcpy(&byte_4EDB00, game_path);
  }
  else
  {
    game_set_game_path_485CC0(aRemoteDev1Dke);
  }
  game_play_intro_469F32();
  if ( is_demo && is_using_hdd )
    game_set_game_path_485CC0(aHdiskDkedemo);
  if ( is_using_hdd )
  {
    language_id = 2;
    for ( i = 0; i < 7; ++i )
    {
      sprintf(&game_temp_path, "DIALOG>%s.CHK", &language_list[3 * i]);
      if ( select_language_version(&game_temp_path) )
      {
        language_id = i;
        break;
      }
    }
  }
  if ( is_demo )
  {
    compressed = image_load_compressed(aDatasMenusDemo, ls_mem_buffer);
    image_color_data = load_image_color_data(aDatasMenusDemo_0, ls_mem_buffer);
    ls_clear_bmap(graphics_context);
    update_palette_with_default_colors(image_color_data);
    draw_image(0, 0, 640, 480, 0, 0);
    ls_delay(3000);
    ls_free_palette(image_color_data);
    ls_free_bmap(compressed);
    ls_clear_bmap(graphics_context);
    sprintf(&game_temp_path, "DATAS>MENUS>HELP_%01d.GIF", language_id);
    v11 = image_load_compressed(&game_temp_path, ls_mem_buffer);
    v9 = load_image_color_data(&game_temp_path, ls_mem_buffer);
    ls_clear_bmap(graphics_context);
    update_palette_with_default_colors(v9);
    draw_image(0, 0, 640, 480, 0, 0);
    ls_wait_key();
    ls_free_palette(v9);
    ls_free_bmap(v11);
    ls_clear_bmap(graphics_context);
  }
  input_processing_enabled = update_status_display(1, &x_position) != 0;
  sprintf(&game_temp_path, "%sANM>ANM.BAT", game_path);
  if ( is_using_hdd || select_language_version(&game_temp_path) )
    dword_4B7294 = 1;
  if ( debug_enabled )
  {
    set_position_coordinates(0, 32);
    draw_text(aInitConfig);
  }
  if ( !is_using_hdd )
    config_load_dke_cfg_45B361();
  config_load_gamedke_cfg_45AE70();
  byte_507350 |= 2u;
  sub_44CF90();
  game_init_46BEEB();
  current_level_id = level_id_4B70E4;
  scene_load_level_46DFEC(level_id_4B70E4);
  ls_set_restore_callback(graphics_context, sub_4608C2);
  dword_4ED9D4 = system_get_gametime();
  dword_4B7244 = 0;
  dword_4B7248 = 0;
  current_game_time = system_get_gametime();
  time_backup = current_game_time;
  dword_4EDFAC = current_game_time;
  dword_505C78 = 0;
  set_display_mode(21);
  menu_gametime_5072C8 = system_get_gametime();
  if ( process_mainmenu_437EB7() )
    update_game_timing_state(1, 21);
  else
    restore_display_mode(1, 21);
  sub_4813C0(1);
  while ( game_exit_code != 1 && !dword_4B7298 )
  {
    ++dword_505C78;
    ls_update_event();
    sub_461468();
    if ( need_change_level_id_4B7278 != -1 )
    {
      scene_release_level_resources_46E4A8();
      if ( debug_enabled )
      {
        sprintf(&game_temp_path, "Nouvelle piece : %ld ", need_change_level_id_4B7278);
        set_position_coordinates(0, 0);
        draw_text(&game_temp_path);
      }
      scene_load_level_46DFEC(need_change_level_id_4B7278);
      need_change_level_id_4B7278 = -1;
    }
  }
  if ( dword_4B729C )
  {
    sub_48DCA0(dword_4B729C);
    dword_4B729C = 0;
  }
  flush_display_buffer();
  scene_release_level_resources_46E4A8();
  game_release_resources_46C599();
  sub_44CFD3();
  audio_releasse_48DC00();
  system_release_all_opened_filse_443037();
  ls_shut_movie();
  ls_release_heap(ui_status_effects_buffer);
  ls_release_heap(ui_special_meter_buffer);
  ls_release_heap(dword_5491E4);
  ls_release_heap(g_creature_data);
  ls_release_heap(dword_5491B0);
  ls_release_heap(dword_5491A0);
  ls_release_heap(special_attack_buffer);
  ls_release_heap(shield_effect_buffer);
  ls_release_heap(projectile_effect_buffer);
  ls_release_heap(magic_effect_buffer);
  ls_release_heap(hit_effect_buffer);
  ls_release_heap(lighting_effect_buffer);
  ls_release_heap(weather_effect_buffer);
  ls_release_heap(particle_effect_buffer);
  ls_release_heap(environment_overlay_buffer);
  ls_release_heap(ui_weapon_slot_buffer);
  ls_release_heap(dword_5491B4);
  ls_release_heap(ui_health_buffer);
  if ( debug_view_active )
  {
    set_position_coordinates(0, 136);
    draw_text(asc_4B7480);
  }
  sub_491E90();
  return 0;
}