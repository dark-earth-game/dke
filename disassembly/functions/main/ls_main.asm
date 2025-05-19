ls_main         proc near               ; CODE XREF: WinMain(x,x,x,x)+F0Vp

command_line_args= dword ptr -34h
var_30          = word ptr -30h
var_2C          = dword ptr -2Ch
var_28          = dword ptr -28h
var_24          = dword ptr -24h
var_20          = byte ptr -20h
var_1C          = byte ptr -1Ch
var_18          = byte ptr -18h
available_memory= dword ptr -10h
audio_sample_rate= word ptr -8
audio_channels  = word ptr -6
audio_bit_depth = dword ptr -4

                push    ebp
                mov     ebp, esp
                sub     esp, 34h
                push    ebx
                push    esi
                push    edi
                mov     [ebp+command_line_args], edx
                mov     [ebp+var_30], cx
                call    load_configuration
                push    offset aHdiskEarth ; "HDISK:>EARTH>"
                push    offset game_path ; char *
                call    _strcpy
                add     esp, 8
                movsx   eax, [ebp+var_30]
                cmp     eax, 2
                jnz     loc_460BF6
                mov     colour_mode_0_8bit_1_16bit, 0
                push    offset off_4B7378 ; char *
                mov     eax, [ebp+command_line_args]
                mov     eax, [eax+4]
                push    eax             ; char *
                call    _strcmp
                add     esp, 8
                test    eax, eax
                jnz     loc_460BD0
                mov     colour_mode_0_8bit_1_16bit, 1

loc_460BD0:                             ; CODE XREF: ls_main+54^j
                push    offset a256     ; "256"
                mov     eax, [ebp+command_line_args]
                mov     eax, [eax+4]
                push    eax             ; char *
                call    _strcmp
                add     esp, 8
                test    eax, eax
                jnz     loc_460BF6
                mov     colour_mode_0_8bit_1_16bit, 0

loc_460BF6:                             ; CODE XREF: ls_main+2E^j ls_main+7A^j
                lea     ecx, [ebp+var_18]
                call    system_check_memory_487410
                cmp     [ebp+available_memory], 1F00000h
                jnb     loc_460C15
                mov     colour_mode_0_8bit_1_16bit, 0

loc_460C15:                             ; CODE XREF: ls_main+99^j
                lea     edx, [ebp+var_1C]
                lea     ecx, [ebp+var_20]
                call    sub_4843F0
                call    sub_4953B0
                mov     ecx, offset aDarkearth ; "DarkEarth"
                call    lsi_set_window_title
                call    sub_491450
                call    sub_495390
                cmp     audio_enabled, 0
                jz      loc_460C63
                mov     [ebp+audio_sample_rate], 5622h
                mov     [ebp+audio_channels], 2
                mov     [ebp+audio_bit_depth], 10h
                lea     edx, [ebp+audio_sample_rate]
                xor     ecx, ecx
                call    lsys_set_sound_format

loc_460C63:                             ; CODE XREF: ls_main+D4^j
                cmp     colour_mode_0_8bit_1_16bit, 0
                jz      loc_460CB7
                push    1               ; int
                push    8               ; int
                push    0A7D8C0h        ; int
                push    10h             ; int
                mov     edx, 1E0h
                mov     ecx, 280h       ; char
                call    graphics_init
                mov     eax, graphics_context
                mov     ecx, [eax+2Ch]
                call    sub_418679
                mov     eax, graphics_context
                mov     eax, [eax+2Ch]
                mov     colour_mode_0_8bit_1_16bit, eax
                call    initialize_color_mapping_tables
                xor     edx, edx
                xor     ecx, ecx
                call    ls_init_movie
                jmp     loc_460CDF
; ---------------------------------------------------------------------------

loc_460CB7:                             ; CODE XREF: ls_main+FE^j
                push    1               ; int
                push    8               ; int
                push    0A7D8C0h        ; int
                push    8               ; int
                mov     edx, 1E0h
                mov     ecx, 280h       ; char
                call    graphics_init
                call    __cfltcvt_init
                xor     edx, edx
                xor     ecx, ecx
                call    ls_init_movie

loc_460CDF:                             ; CODE XREF: ls_main+146^j
                cmp     debug_enabled, 0
                jz      loc_460D02
                mov     edx, 20h ; ' '
                xor     ecx, ecx
                call    set_position_coordinates ; Sets the current and target screen or cursor positions for interpolation or transition effects
                mov     ecx, offset aInitLibsysFini ; "Init Libsys fini ..."
                call    draw_text

loc_460D02:                             ; CODE XREF: ls_main+17A^j
                mov     ecx, offset sub_461AF6
                call    sub_480AF0
                cmp     audio_enabled, 0
                jnz     loc_460D2D
                mov     samples_enabled, 0
                mov     music_enabled, 0

loc_460D2D:                             ; CODE XREF: ls_main+1A7^j
                cmp     debug_enabled, 0
                jz      loc_460D50
                mov     edx, 20h ; ' '
                xor     ecx, ecx
                call    set_position_coordinates ; Sets the current and target screen or cursor positions for interpolation or transition effects
                mov     ecx, offset aInitMemory ; "Init memory ..."
                call    draw_text

loc_460D50:                             ; CODE XREF: ls_main+1C8^j
                mov     ecx, offset byte_548C00
                call    ls_get_module_name
                call    ls_read_timer
                imul    eax, timing_multiplier
                mov     timing_offset, eax
                push    offset byte_548C00 ; char *
                call    __strlwr
                add     esp, 4
                mov     ecx, 1B7740h
                call    ls_get_heap
                mov     ui_health_buffer, eax
                mov     ecx, 0F4240h
                call    ls_get_heap
                mov     dword_5491B4, eax
                mov     ecx, 4DDD0h
                call    ls_get_heap
                mov     ui_inventory_slot_buffer, eax
                mov     ecx, 26EE8h
                call    ls_get_heap
                mov     ui_weapon_slot_buffer, eax
                mov     ecx, 67070h
                call    ls_get_heap
                mov     environment_overlay_buffer, eax
                mov     ecx, dword_4B8444
                call    ls_get_heap
                mov     particle_effect_buffer, eax
                mov     ecx, dword_4B8444
                call    ls_get_heap
                mov     weather_effect_buffer, eax
                mov     ecx, dword_4B8444
                call    ls_get_heap
                mov     lighting_effect_buffer, eax
                mov     ecx, dword_4B8444
                call    ls_get_heap
                mov     hit_effect_buffer, eax
                mov     ecx, dword_4B8444
                call    ls_get_heap
                mov     magic_effect_buffer, eax
                mov     ecx, dword_4B8444
                call    ls_get_heap
                mov     projectile_effect_buffer, eax
                mov     ecx, dword_4B8444
                call    ls_get_heap
                mov     shield_effect_buffer, eax
                mov     ecx, dword_4B8444
                call    ls_get_heap
                mov     special_attack_buffer, eax
                mov     ecx, particle_effect_buffer
                call    ls_max_memory_heap
                mov     dword_4B8444, eax
                mov     ecx, 4D3E0h
                call    ls_get_heap
                mov     dword_5491A0, eax
                mov     ecx, 3D0900h
                call    ls_get_heap
                mov     dword_5491B0, eax
                mov     ecx, 1CCA0h
                call    ls_get_heap
                mov     g_creature_data, eax
                mov     ecx, 65000h
                call    ls_get_heap
                mov     dword_5491E4, eax
                cmp     colour_mode_0_8bit_1_16bit, 0
                jz      loc_460EB0
                mov     ecx, 0F4240h
                call    ls_get_heap
                mov     ui_special_meter_buffer, eax
                jmp     loc_460EBF
; ---------------------------------------------------------------------------

loc_460EB0:                             ; CODE XREF: ls_main+32A^j
                mov     ecx, 0F4240h
                call    ls_get_heap
                mov     ui_special_meter_buffer, eax

loc_460EBF:                             ; CODE XREF: ls_main+33F^j
                mov     ecx, 32h ; '2'
                call    ls_get_heap
                mov     ui_status_effects_buffer, eax
                call    sub_442D45
                call    audio_create_buffer_48DA70
                push    1
                call    sub_48E280
                add     esp, 4
                mov     byte_4EDB00, 0
                cmp     is_using_hdd, 0
                jnz     loc_460F05
                mov     ecx, offset aRemoteDev1Dke ; "REMOTE_DEV1:>dke"
                call    game_set_game_path_485CC0
                jmp     loc_460F65
; ---------------------------------------------------------------------------

loc_460F05:                             ; CODE XREF: ls_main+384^j
                mov     eax, offset word_55C770
                add     eax, 2Ah ; '*'
                push    eax
                push    offset aS_5     ; "%s>"
                push    offset game_path ; char *
                call    _sprintf
                add     esp, 0Ch
                mov     ecx, offset unk_4B73CC
                call    game_set_game_path_485CC0
                push    offset game_path
                push    offset aSdatas  ; "%sDATAS"
                push    offset game_temp_path ; char *
                call    _sprintf
                add     esp, 0Ch
                mov     ecx, offset game_temp_path ; char *
                call    ls_is_path_exist
                test    eax, eax
                jz      loc_460F65
                push    offset game_path ; char *
                push    offset byte_4EDB00 ; char *
                call    _strcpy
                add     esp, 8

loc_460F65:                             ; CODE XREF: ls_main+394^j ls_main+3E1^j
                call    game_play_intro_469F32
                cmp     is_demo, 0
                jz      loc_460F8E
                cmp     is_using_hdd, 0
                jz      loc_460F8E
                mov     ecx, offset aHdiskDkedemo ; "HDISK:>DKEDEMO>"
                call    game_set_game_path_485CC0

loc_460F8E:                             ; CODE XREF: ls_main+405^j ls_main+412^j
                cmp     is_using_hdd, 0
                jz      loc_461000
                mov     language_id, 2
                mov     [ebp+var_24], 0
                jmp     loc_460FB4
; ---------------------------------------------------------------------------

loc_460FB1:                             ; CODE XREF: ls_main:loc_460FFBVj
                inc     [ebp+var_24]

loc_460FB4:                             ; CODE XREF: ls_main+440^j
                cmp     [ebp+var_24], 7
                jge     loc_461000
                mov     eax, [ebp+var_24]
                lea     eax, [eax+eax*2]
                add     eax, offset language_list ; "FR"
                push    eax
                push    offset aDialogSChk ; "DIALOG>%s.CHK"
                push    offset game_temp_path ; char *
                call    _sprintf
                add     esp, 0Ch
                mov     ecx, offset game_temp_path
                call    select_language_version
                test    eax, eax
                jz      loc_460FFB
                mov     eax, [ebp+var_24]
                mov     language_id, eax
                jmp     loc_461000
; ---------------------------------------------------------------------------

loc_460FFB:                             ; CODE XREF: ls_main+47C^j
                jmp     loc_460FB1
; ---------------------------------------------------------------------------

loc_461000:                             ; CODE XREF: ls_main+429^j ls_main+44C^j ...
                cmp     is_demo, 0
                jz      loc_46111C
                mov     ecx, offset aDatasMenusDemo ; "DATAS>MENUS>DEMO.GIF"
                mov     edx, _ls_mem_buffer
                call    image_load_compressed
                mov     [ebp+var_28], eax
                mov     ecx, offset aDatasMenusDemo_0 ; "DATAS>MENUS>DEMO.GIF"
                mov     edx, _ls_mem_buffer
                call    load_image_color_data
                mov     [ebp+var_2C], eax
                mov     ecx, graphics_context
                call    ls_clear_bmap
                mov     ecx, [ebp+var_2C]
                call    update_palette_with_default_colors
                push    0
                push    0
                push    1E0h
                push    280h
                push    0
                push    0
                mov     edx, graphics_context
                mov     ecx, [ebp+var_28]
                call    draw_image
                mov     ecx, 0BB8h
                call    ls_delay
                mov     ecx, [ebp+var_2C]
                call    ls_free_palette
                mov     ecx, [ebp+var_28]
                call    ls_free_bmap
                mov     ecx, graphics_context
                call    ls_clear_bmap
                mov     eax, language_id
                push    eax
                push    offset aDatasMenusHelp_0 ; "DATAS>MENUS>HELP_%01d.GIF"
                push    offset game_temp_path ; char *
                call    _sprintf
                add     esp, 0Ch
                mov     ecx, offset game_temp_path
                mov     edx, _ls_mem_buffer
                call    image_load_compressed
                mov     [ebp+var_28], eax
                mov     ecx, offset game_temp_path
                mov     edx, _ls_mem_buffer
                call    load_image_color_data
                mov     [ebp+var_2C], eax
                mov     ecx, graphics_context
                call    ls_clear_bmap
                mov     ecx, [ebp+var_2C]
                call    update_palette_with_default_colors
                push    0
                push    0
                push    1E0h
                push    280h
                push    0
                push    0
                mov     edx, graphics_context
                mov     ecx, [ebp+var_28]
                call    draw_image
                call    ls_wait_key
                mov     ecx, [ebp+var_2C]
                call    ls_free_palette
                mov     ecx, [ebp+var_28]
                call    ls_free_bmap
                mov     ecx, graphics_context
                call    ls_clear_bmap

loc_46111C:                             ; CODE XREF: ls_main+49B^j
                mov     edx, offset x_position
                mov     ecx, 1
                call    update_status_display
                test    eax, eax
                jz      loc_461142
                mov     input_processing_enabled, 1
                jmp     loc_46114C
; ---------------------------------------------------------------------------

loc_461142:                             ; CODE XREF: ls_main+5C1^j
                mov     input_processing_enabled, 0

loc_46114C:                             ; CODE XREF: ls_main+5D1^j
                push    offset game_path
                push    offset aSanmAnmBat ; "%sANM>ANM.BAT"
                push    offset game_temp_path ; char *
                call    _sprintf
                add     esp, 0Ch
                cmp     is_using_hdd, 0
                jnz     loc_461182
                mov     ecx, offset game_temp_path
                call    select_language_version
                test    eax, eax
                jz      loc_46118C

loc_461182:                             ; CODE XREF: ls_main+5FE^j
                mov     dword_4B7294, 1

loc_46118C:                             ; CODE XREF: ls_main+610^j
                cmp     debug_enabled, 0
                jz      loc_4611AF
                mov     edx, 20h ; ' '
                xor     ecx, ecx
                call    set_position_coordinates ; Sets the current and target screen or cursor positions for interpolation or transition effects
                mov     ecx, offset aInitConfig ; "Init config ..."
                call    draw_text

loc_4611AF:                             ; CODE XREF: ls_main+627^j
                cmp     is_using_hdd, 0
                jnz     loc_4611C1
                call    config_load_dke_cfg_45B361

loc_4611C1:                             ; CODE XREF: ls_main+64A^j
                call    config_load_gamedke_cfg_45AE70
                xor     eax, eax
                mov     al, byte_507350
                or      al, 2
                mov     byte_507350, al
                call    sub_44CF90
                call    game_init_46BEEB
                mov     eax, level_id_4B70E4
                mov     current_level_id, eax
                mov     ecx, current_level_id
                call    scene_load_level_46DFEC
                mov     edx, offset sub_4608C2
                mov     ecx, graphics_context
                call    ls_set_restore_callback
                call    system_get_gametime ; System Get Game Time
                mov     dword_4ED9D4, eax
                mov     dword_4B7244, 0
                mov     dword_4B7248, 0
                call    system_get_gametime ; System Get Game Time
                mov     current_game_time, eax
                mov     eax, current_game_time
                mov     time_backup, eax
                mov     eax, current_game_time
                mov     dword_4EDFAC, eax
                mov     dword_505C78, 0
                mov     ecx, 15h
                call    set_display_mode
                call    system_get_gametime ; System Get Game Time
                mov     menu_gametime_5072C8, eax
                call    process_mainmenu_437EB7
                test    eax, eax
                jz      loc_46127E
                mov     edx, 15h
                mov     ecx, 1
                call    update_game_timing_state
                jmp     loc_46128D
; ---------------------------------------------------------------------------

loc_46127E:                             ; CODE XREF: ls_main+6F8^j
                mov     edx, 15h
                mov     ecx, 1
                call    restore_display_mode

loc_46128D:                             ; CODE XREF: ls_main+70D^j
                mov     ecx, 1
                call    sub_4813C0

loc_461297:                             ; CODE XREF: ls_main:loc_461320Vj
                cmp     game_exit_code, 1
                jz      loc_461325
                cmp     dword_4B7298, 0
                jnz     loc_461325
                inc     dword_505C78
                call    ls_update_event
                call    sub_461468
                cmp     need_change_level_id_4B7278, 0FFFFFFFFh
                jz      loc_461320
                call    scene_release_level_resources_46E4A8
                cmp     debug_enabled, 0
                jz      loc_46130B
                mov     eax, need_change_level_id_4B7278
                push    eax
                push    offset aNouvellePieceL ; "Nouvelle piece : %ld "
                push    offset game_temp_path ; char *
                call    _sprintf
                add     esp, 0Ch
                xor     edx, edx
                xor     ecx, ecx
                call    set_position_coordinates ; Sets the current and target screen or cursor positions for interpolation or transition effects
                mov     ecx, offset game_temp_path
                call    draw_text

loc_46130B:                             ; CODE XREF: ls_main+76E^j
                mov     ecx, need_change_level_id_4B7278
                call    scene_load_level_46DFEC
                mov     need_change_level_id_4B7278, 0FFFFFFFFh

loc_461320:                             ; CODE XREF: ls_main+75C^j
                jmp     loc_461297
; ---------------------------------------------------------------------------

loc_461325:                             ; CODE XREF: ls_main+732^j ls_main+73F^j
                cmp     dword_4B729C, 0
                jz      loc_46134A
                mov     eax, dword_4B729C
                push    eax
                call    sub_48DCA0
                add     esp, 4
                mov     dword_4B729C, 0

loc_46134A:                             ; CODE XREF: ls_main+7C0^j
                call    flush_display_buffer
                call    scene_release_level_resources_46E4A8
                call    game_release_resources_46C599
                call    sub_44CFD3
                call    audio_releasse_48DC00
                call    system_release_all_opened_filse_443037
                call    ls_shut_movie
                mov     ecx, ui_status_effects_buffer
                call    ls_release_heap
                mov     ecx, ui_special_meter_buffer
                call    ls_release_heap
                mov     ecx, dword_5491E4
                call    ls_release_heap
                mov     ecx, g_creature_data
                call    ls_release_heap
                mov     ecx, dword_5491B0
                call    ls_release_heap
                mov     ecx, dword_5491A0
                call    ls_release_heap
                mov     ecx, special_attack_buffer
                call    ls_release_heap
                mov     ecx, shield_effect_buffer
                call    ls_release_heap
                mov     ecx, projectile_effect_buffer
                call    ls_release_heap
                mov     ecx, magic_effect_buffer
                call    ls_release_heap
                mov     ecx, hit_effect_buffer
                call    ls_release_heap
                mov     ecx, lighting_effect_buffer
                call    ls_release_heap
                mov     ecx, weather_effect_buffer
                call    ls_release_heap
                mov     ecx, particle_effect_buffer
                call    ls_release_heap
                mov     ecx, environment_overlay_buffer
                call    ls_release_heap
                mov     ecx, ui_weapon_slot_buffer
                call    ls_release_heap
                mov     ecx, dword_5491B4
                call    ls_release_heap
                mov     ecx, ui_health_buffer
                call    ls_release_heap
                cmp     debug_view_active, 0
                jz      loc_461456
                mov     edx, 88h ; '�'
                xor     ecx, ecx
                call    set_position_coordinates ; Sets the current and target screen or cursor positions for interpolation or transition effects
                mov     ecx, offset asc_4B7480 ; "................"
                call    draw_text

loc_461456:                             ; CODE XREF: ls_main+8CE^j
                call    sub_491E90
                xor     ax, ax
                jmp     $+5
; ---------------------------------------------------------------------------

loc_461463:                             ; CODE XREF: ls_main+8F2^j
                pop     edi
                pop     esi
                pop     ebx
                leave
                retn
ls_main         endp
