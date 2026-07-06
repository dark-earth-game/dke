#pragma once

#include "common.h"
#include "../state.h"

/*
 * LIBSYS startup helpers — ported from sub_487D40 and sub_487E30.
 *
 * Globals mirror original addresses:
 *   dword_55C768  → g_ls_master
 *   dword_55C75C  → g_ls_debug
 *   dword_55C760  → g_ls_info
 *   dword_55C764  → g_ls_lang_id
 *   word_4C0B68   → g_ls_pause_state
 *
 * Virtual-root globals (sub_485840):
 *   unk_55C79A    → g_virtual_root[64]
 *   byte_55C89C   → g_ls_workspace_a[0xAA0*4]
 *   unk_55C638    → g_ls_workspace_b[0x44*4]
 *   dword_55C738  → g_ls_drive_type
 *   dword_55C73C  → g_ls_drive_num
 */

/* 0x4C0B68 – pause/flush state flag, reset to 0 at startup */
extern u16 g_ls_pause_state;

/* 0x55C768 – master logging/debug mode */
extern i32 g_ls_master;
/* 0x55C75C – debug logging enabled */
extern i32 g_ls_debug;
/* 0x55C760 – info logging enabled */
extern i32 g_ls_info;
/* 0x55C764 – system language ID (GetSystemDefaultLangID & 0x3FF) */
extern i32 g_ls_lang_id;

/* 0x55C79A – virtual root path string, e.g. "HDISK0:>" */
extern c8  g_virtual_root[64];

/* 0x55C738 – drive type (1=FLOPPY, 10=HDISK, 20=HDISK, 30=REMOTE_SYS, …) */
extern i32 g_ls_drive_type;
/* 0x55C73C – drive number */
extern i32 g_ls_drive_num;

/* 0x55C89C – workspace buffer A (0xAA0 dwords = 10880 bytes) */
extern u8  g_ls_workspace_a[0xAA0 * 4];
/* 0x55C638 – workspace buffer B (0x44 dwords = 272 bytes) */
extern u8  g_ls_workspace_b[0x44  * 4];

/* 00487D40 – reset LIBSYS pause-state flag */
void ls_libsys_flag_reset(void);

/* 00487E30 – apply LIBSYS flags from parsed config, detect host language */
void ls_libsys_init_flags(state_t *state);

/* 00485840 – build virtual root path into g_virtual_root */
void ls_build_virtual_root(void);
