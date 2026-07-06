#include <stdlib.h>
#include <string.h>

#include "common.h"
#include "memory.h"

/*
 * Memory manager port — Dark Earth LIBSYS
 *
 * Original allocator: custom doubly-linked free-list heap backed by
 * GlobalAlloc/GlobalLock (Win32). The heap metadata uses 0x20-byte slot
 * descriptors (MAX_HEAP_SLOTS = 0xC8 = 200) and 0x14-byte block headers
 * within each GlobalAlloc'd segment.
 *
 * Port decision:
 *   We set global_state_flags |= MEM_FLAG_HEAP (bit 3) during
 *   ls_init_memory_manager so that allocate_memory/free_memory always take
 *   the malloc/free branch. This matches the asm's own fallback path
 *   (present since the debug build already routed through malloc) and avoids
 *   porting the full Win32 HANDLE-based heap — which would be dead code on
 *   macOS/Linux.
 *
 *   The only semantic difference: the original heap tracked blocks in a
 *   GlobalAlloc'd segment; we track total_memory_used via a simple counter
 *   instead. Block sizes are not stored on free (matching malloc semantics).
 *
 * memory_alloc/memory_calloc/memory_free (common.c) are NOT changed and do
 * NOT delegate here — they remain independent OS-backed helpers used only by
 * the C re-implementation skeleton, not by ported game code.
 */

/* Global state flags — controls alloc path in allocate_memory/free_memory.
 * Bit 3 (0x08): use malloc/free path (we set this on init).
 * Bit 4 (0x10): zero-fill on alloc (calloc semantics).
 * Bit 0 (0x01): memory logging / gruyere check in register_image_buffer.
 * Bit 7 (0x80): used elsewhere (rendering flags etc.) — not touched here. */
u32 global_state_flags = 0;

/* Named image-buffer registry — slot 0..14 (MEM_SLOT_*).
 * Populated by register_game_image_buffers from the game's buffer globals.
 * Mirrors the _use_malloc heap-slot table that register_image_buffer walks. */
void *g_image_buffer_registry[MEM_SLOT_COUNT] = {0};

/* total_memory_used is declared extern in common.h; define it here because
 * memory.c owns the allocator that tracks it. common.c left its definition
 * commented out. */
i32 total_memory_used = 0;

/* ---------------------------------------------------------------------------
 * ls_init_memory_manager — 47E8E3
 * Called from ls_startup.
 *
 * Original:
 *   1. Calls lsys_init_memory_manager — zeroes the 0xC8-slot heap table.
 *   2. Calls ls_get_heap — GlobalAlloc's a memory segment, builds a
 *      free-list inside it, stores the descriptor in _ls_mem_buffer.
 *   3. Sets word_55C772 = 0 (alloc counter).
 *   Returns 1 on success, 0 if GlobalAlloc / heap init failed.
 *
 * Port:
 *   We skip the custom heap and instead set MEM_FLAG_HEAP so all subsequent
 *   allocate_memory / free_memory calls use malloc / free.
 *   g_image_buffer_registry is zeroed (matches lsys_init_memory_manager
 *   zeroing the slot table).
 *   total_memory_used is reset to 0 (matches word_55C772 = 0).
 * ------------------------------------------------------------------------- */
// 47E8E3
i32 ls_init_memory_manager(void) {
    /* Zero slot registry — matches lsys_init_memory_manager rep stosd on 0xC8 entries */
    memset(g_image_buffer_registry, 0, sizeof(g_image_buffer_registry));

    /* Select malloc/free path — avoids porting Win32 GlobalAlloc heap on non-Windows */
    global_state_flags |= MEM_FLAG_HEAP;

    /* Reset allocation counter — matches word_55C772 = 0 */
    total_memory_used = 0;

    return 1; /* success — matches original returning 1 when ls_get_heap succeeds */
}

/* ---------------------------------------------------------------------------
 * allocate_memory — 4032DC
 * Original calling convention: __thiscall-style with ECX=resource_type, EDX=size.
 * Ported as explicit parameters.
 *
 * Original logic:
 *   if !(global_state_flags & 0x8):   // custom heap path
 *       register_game_image_buffers()
 *       ls_get_memory_heap(size, resource_type)  // free-list alloc
 *   elif (global_state_flags & 0x10): // malloc + zero path
 *       malloc(size); memset(ptr, 0, size)
 *   else:                             // malloc only
 *       malloc(size)
 *
 * Port:
 *   We always set MEM_FLAG_HEAP, so the elif/else branches are always taken.
 *   MEM_FLAG_ZERO controls whether to zero-fill (matching the 0x10 bit).
 *   total_memory_used is updated on success.
 * ------------------------------------------------------------------------- */
// 4032DC
void *allocate_memory(i32 resource_type, i32 size) {
    void *ptr = NULL;

    if (!(global_state_flags & MEM_FLAG_HEAP)) {
        /* Original custom heap path — not reachable after ls_init_memory_manager,
         * but kept to preserve the branch structure faithfully. */
        register_game_image_buffers();
        /* ls_get_memory_heap not ported — fall through to NULL */
        return NULL;
    }

    if (global_state_flags & MEM_FLAG_ZERO) {
        /* calloc semantics (malloc + memset 0) */
        ptr = malloc((size_t)size);
        if (ptr) {
            memset(ptr, 0, (size_t)size);
        }
    } else {
        ptr = malloc((size_t)size);
    }

    if (ptr) {
        total_memory_used += size;
    }

    (void)resource_type; /* resource_type preserved for asm parity; unused on malloc path */
    return ptr;
}

/* ---------------------------------------------------------------------------
 * free_memory — 40333A
 * Original calling convention: ECX=ptr.
 *
 * Original logic:
 *   if ptr == NULL: return
 *   if (global_state_flags & 0x8): free(ptr)
 *   else: ls_release(ptr); register_game_image_buffers()
 *
 * Port:
 *   The custom heap release path (ls_release) is not reachable after
 *   ls_init_memory_manager sets MEM_FLAG_HEAP. total_memory_used is not
 *   decremented (matching malloc semantics — block size not tracked on free).
 * ------------------------------------------------------------------------- */
// 40333A
void free_memory(void *ptr) {
    if (!ptr) {
        return;
    }

    if (global_state_flags & MEM_FLAG_HEAP) {
        free(ptr);
    } else {
        /* Custom heap path — ls_release not ported; register_game_image_buffers called */
        register_game_image_buffers();
    }
}

/* ---------------------------------------------------------------------------
 * register_game_image_buffers — ~4031AD
 * Called from allocate_memory (loc_403315) and free_memory+0x41 on the
 * custom-heap path. Also called directly after heap operations to refresh
 * the named-buffer slot registry.
 *
 * Original:
 *   Calls register_image_buffer(buffer_ptr, slot_id) for each of 15 named
 *   image buffers (MEM_LIBSYS=0 through special_attack=14).
 *   register_image_buffer itself checks global_state_flags bit 0 to decide
 *   whether to log memory fragmentation stats.
 *
 * Port:
 *   We populate g_image_buffer_registry from the same extern buffer globals
 *   that the original code used. The fragmentation logging from
 *   register_image_buffer is omitted (no heap to measure fragmentation on).
 *   Slot IDs match MEM_SLOT_* constants in memory.h.
 *
 * NOTE: the buffer globals below are declared extern here; their actual
 * storage lives in state->game.* fields populated during game init.
 * When those fields are not yet initialized, the pointers are NULL — safe
 * because register_game_image_buffers is only meaningful after game_init.
 * ------------------------------------------------------------------------- */

/* Named image-buffer globals — mirrors the original IDA globals that held
 * pointers to Win32 DIBSECTION / scratch buffers.  Populated by game init code
 * (game_init_46BEEB etc.) via state->game.*_buffer when that code is ported.
 * Defined here as module-level globals (NULL until game init runs) so that
 * register_game_image_buffers compiles cleanly with no unresolved externs.
 * They match the names used in dkev.asm verbatim. */
u8 *_ls_mem_buffer_g           = NULL; /* _ls_mem_buffer (slot 0 / MEM_LIBSYS) */
u8 *ui_health_buffer           = NULL; /* slot 1 */
u8 *ui_special_meter_buffer    = NULL; /* slot 2 */
u8 *ui_status_effects_buffer   = NULL; /* slot 3 */
u8 *ui_inventory_slot_buffer   = NULL; /* slot 4 */
u8 *ui_weapon_slot_buffer      = NULL; /* slot 5 */
u8 *environment_overlay_buffer = NULL; /* slot 6 */
u8 *particle_effect_buffer     = NULL; /* slot 7 */
u8 *weather_effect_buffer      = NULL; /* slot 8 */
u8 *lighting_effect_buffer     = NULL; /* slot 9 */
u8 *hit_effect_buffer          = NULL; /* slot 10 */
u8 *magic_effect_buffer        = NULL; /* slot 11 */
u8 *projectile_effect_buffer   = NULL; /* slot 12 */
u8 *shield_effect_buffer       = NULL; /* slot 13 */
u8 *special_attack_buffer      = NULL; /* slot 14 */

// ~4031AD
void register_game_image_buffers(void) {
    g_image_buffer_registry[MEM_SLOT_LIBSYS]      = _ls_mem_buffer_g;
    g_image_buffer_registry[MEM_SLOT_HEALTH]       = ui_health_buffer;
    g_image_buffer_registry[MEM_SLOT_SPECIAL]      = ui_special_meter_buffer;
    g_image_buffer_registry[MEM_SLOT_STATUS]       = ui_status_effects_buffer;
    g_image_buffer_registry[MEM_SLOT_INVENTORY]    = ui_inventory_slot_buffer;
    g_image_buffer_registry[MEM_SLOT_WEAPON]       = ui_weapon_slot_buffer;
    g_image_buffer_registry[MEM_SLOT_ENVIRON]      = environment_overlay_buffer;
    g_image_buffer_registry[MEM_SLOT_PARTICLE]     = particle_effect_buffer;
    g_image_buffer_registry[MEM_SLOT_WEATHER]      = weather_effect_buffer;
    g_image_buffer_registry[MEM_SLOT_LIGHTING]     = lighting_effect_buffer;
    g_image_buffer_registry[MEM_SLOT_HIT]          = hit_effect_buffer;
    g_image_buffer_registry[MEM_SLOT_MAGIC]        = magic_effect_buffer;
    g_image_buffer_registry[MEM_SLOT_PROJECTILE]   = projectile_effect_buffer;
    g_image_buffer_registry[MEM_SLOT_SHIELD]       = shield_effect_buffer;
    g_image_buffer_registry[MEM_SLOT_SPECIAL_ATK]  = special_attack_buffer;
}
