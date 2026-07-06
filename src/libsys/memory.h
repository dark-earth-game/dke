#pragma once

#include "common.h"

/*
 * LIBSYS memory manager
 *
 * Original: custom doubly-linked free-list heap backed by GlobalAlloc/GlobalLock (Win32).
 * Port: the Win32 OS allocator calls are replaced with malloc/free.
 *   - global_state_flags bit 3 (0x8) = use malloc path (set by ls_init_memory_manager).
 *   - global_state_flags bit 4 (0x10) = zero-fill on alloc (calloc semantics).
 * allocate_memory/free_memory are the engine-facing alloc/free used by game code.
 * memory_alloc/memory_calloc/memory_free in common.c are separate helpers for C-side
 * engine utilities; they stay OS-backed and do NOT delegate here — the two allocators
 * are independent (matching the original binary where only game code went through
 * allocate_memory, not the re-implementation skeleton code).
 *
 * Image buffer slots (MEM_LIBSYS, MEM_DKE, etc.) are tracked in a 200-entry table
 * (MAX_HEAP_SLOTS) matching the 0xC8 slot count from lsys_init_memory_manager.
 */

#define MAX_HEAP_SLOTS  200     /* 0xC8 — from lsys_init_memory_manager */
#define MEM_FLAG_HEAP   0x08    /* bit 3: use malloc path */
#define MEM_FLAG_ZERO   0x10    /* bit 4: zero-fill on alloc */

/* Named image buffer slot IDs used by register_game_image_buffers */
#define MEM_SLOT_LIBSYS     0
#define MEM_SLOT_HEALTH     1
#define MEM_SLOT_SPECIAL    2
#define MEM_SLOT_STATUS     3
#define MEM_SLOT_INVENTORY  4
#define MEM_SLOT_WEAPON     5
#define MEM_SLOT_ENVIRON    6
#define MEM_SLOT_PARTICLE   7
#define MEM_SLOT_WEATHER    8
#define MEM_SLOT_LIGHTING   9
#define MEM_SLOT_HIT        10
#define MEM_SLOT_MAGIC      11
#define MEM_SLOT_PROJECTILE 12
#define MEM_SLOT_SHIELD     13
#define MEM_SLOT_SPECIAL_ATK 14
#define MEM_SLOT_COUNT      15

/* Global state flags (matches global_state_flags in dkev.asm) */
extern u32 global_state_flags;

/*
 * Named buffer registry — parallel to the _use_malloc heap-slot table.
 * Each slot stores a pointer to the current buffer for that ID.
 * register_game_image_buffers populates this from state->game.*_buffer fields.
 */
extern void *g_image_buffer_registry[MEM_SLOT_COUNT];

/* allocation counter — also exported as total_memory_used from common.h */
extern i32 total_memory_used;

/* 47E8E3 */
i32 ls_init_memory_manager(void);

/* 4032DC — ECX=resource_type, EDX=size in original __thiscall convention;
 * ported as (resource_type, size) params */
void *allocate_memory(i32 resource_type, i32 size);

/* 40333A — ECX=ptr in original; ported as (ptr) */
void free_memory(void *ptr);

/* called from allocate_memory and free_memory when global_state_flags bit 3 is clear
 * (non-malloc heap path) — registers named image-buffer slots into g_image_buffer_registry */
/* ~4031AD */
void register_game_image_buffers(void);
