#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>

#include "palette.h"
#include "../common.h"
#include "../memory.h"

/* ---------------------------------------------------------------------------
 * Palette globals
 *
 * Both buffers are 34 dwords = 136 bytes, matching the rep movsd ecx=0x22
 * observed in sub_491450 and sub_4953B0.
 * --------------------------------------------------------------------------*/

/* Source buffer — original asm: "alternative_palette_data" */
u8 alternative_palette_data[136] = {0};

/* Destination buffer — original asm: "palette_target_buffer" */
u8 palette_target_buffer[136] = {0};

/* ---------------------------------------------------------------------------
 * Global palette / colour-table buffers referenced by rendering paths.
 *
 * color_table — single-byte palette entry table used by rendering and raster
 *   code (configure_game_palette, apply_palette_transformation, etc.).
 *   Original asm: "color_table db ?" at data segment ~0x00401561.
 *   Size: 0x300 bytes (RGB triplets for 256 palette entries — standard VGA
 *   palette), inferred from the memcpy 0x300 in game_init phase 7 that copies
 *   MENUGEN.GIF palette data here.
 *
 * word_509B40 — inventory/persona slot table filled by palette_postprocess_configure
 *   (sub_425D50).  98 entries × 8 i16 values = 0x310 bytes (0x620 / 2).
 *   Original asm: "word_509B40 dw ?" at data segment 0x00509B40.
 * --------------------------------------------------------------------------*/

/* 0x00401561 — RGB palette table used by rendering (configure_game_palette etc.) */
u8 color_table[0x300] = {0};

/* 0x00509B40 — persona/inventory slot table: 98 entries × 8 i16 slots = 0x310 words */
i16 word_509B40[0x310] = {0};

/* ---------------------------------------------------------------------------
 * palette_postprocess_configure  (sub_425D50 @ 0x00425D50)
 *
 * Loads persona/inventory slot data into word_509B40 (0x620 bytes / 0x310 i16s).
 *
 * Data source priority:
 *   1. Cached binary: "PERSOS>INVENT.CTX" (or "$game_path/PERSOS>INVENT.CTX"
 *      on HDD). Contains a 4-byte date stamp + 0x620 raw bytes.
 *      Accepted if CTX exists AND either is_using_hdd is true OR the CTX date
 *      matches the source INVENT.TXT modification time.
 *   2. Parsed text: "PERSOS>INVENT.TXT" — 98 entries of the form "P%02d :"
 *      followed by up to 8 space-separated decimal values per entry. After
 *      parsing, the result is written back as a new CTX cache file.
 *
 * word_509B40 layout: 98 persona entries (outer loop 0..97), each with 8 i16
 * inventory-slot values (inner loop). Total = 0x310 words = 0x620 bytes.
 *
 * Local helpers:
 *   select_language_version — tests whether a language-specific file variant
 *     exists; here used to test CTX existence.  Returns non-NULL if present.
 *   ls_switch_memory        — switches active memory context (no-op in port).
 *   ls_load_file_at         — loads an entire file into a caller-supplied buffer.
 *   ls_get_filedate         — returns a 4-byte opaque file modification date.
 * --------------------------------------------------------------------------*/

/*
 * select_language_version_palette — local stub: tests whether the given path
 * exists (language-specific file variant check).  Returns 1 if present, 0 if
 * not.  Full implementation lives in game_init.c as a static; this stub
 * mirrors the original asm call at sub_425D50+80.
 *
 * TODO: unify with select_language_version in game_init.c once that function
 * is promoted to a shared header.
 */
static i32 select_language_version_palette(const c8 *path) {
    /* TODO: sub_XXXXXX — full language-variant file existence check */
    return ls_exists(path) ? 1 : 0;
}

/*
 * ls_get_filedate — returns an opaque 4-byte file modification stamp.
 * Original asm: "ls_get_filedate" via ECX=path.
 * Returns file mtime as u32 timestamp.  Original packs DOS date/time;
 * we return Unix epoch (good enough for comparison — the only caller
 * checks newer-than).
 */
static u32 ls_get_filedate(const c8 *path) {
    if (!path) return 0;
    struct stat st;
    if (stat(path, &st) != 0) return 0;
    return (u32)st.st_mtime;
}

/*
 * ls_switch_memory — switches the active memory pool for allocations.
 * Original: ECX = memory-slot id.  Intentionally a no-op — Dark Earth's
 * per-pool memory management is not needed in the port.
 */
static void ls_switch_memory(i32 slot) {
    (void)slot;
}

/*
 * ls_load_file_at — loads the file at 'path' into 'buf' (size bytes).
 * Original: ECX=path, EDX=buf, call ls_load_file_at.
 * Returns number of bytes loaded, 0 on failure.
 */
static i32 ls_load_file_at(const c8 *path, void *buf, i32 size) {
    FILE *f = fopen(path, "rb");
    if (!f) return 0;
    i32 n = (i32)fread(buf, 1, (size_t)size, f);
    fclose(f);
    return n;
}

/* 00425D50 */
void palette_postprocess_configure(const c8 *game_path_arg, i32 is_using_hdd) {
    c8 ctx_path[0x31C]; /* var_31C */
    c8 txt_path[0x108]; /* var_108 */

    /* -- Build file paths ------------------------------------------------- */
    if (is_using_hdd) {
        /* HDD: prepend game_path to the virtual path */
        snprintf(ctx_path, sizeof(ctx_path),
                 "%sPERSOS>INVENT.CTX", game_path_arg ? game_path_arg : "");
    } else {
        snprintf(ctx_path, sizeof(ctx_path), "PERSOS>INVENT.CTX");
    }
    snprintf(txt_path, sizeof(txt_path), "PERSOS>INVENT.TXT");

    /* -- Zero the inventory slot table ------------------------------------ */
    memset(word_509B40, 0, sizeof(word_509B40)); /* 0x620 bytes @ 425D9B */

    /* -- Try to load cached binary (CTX) ---------------------------------- */
    if (select_language_version_palette(ctx_path)) {
        FILE *f = fopen(ctx_path, "rb");
        if (f) {
            u32 ctx_date = 0;
            /* Read 4-byte date stamp */
            fread(&ctx_date, 4, 1, f); /* ls_read(ecx=handle, edx=&var_4, 4) */

            if (is_using_hdd) {
                /* HDD path: always accept cached CTX */
                fread(word_509B40, 0x620, 1, f); /* ls_read 0x620 bytes */
                fclose(f);
                print("palette_postprocess_configure: loaded CTX cache (HDD)\n");
                return;
            } else {
                /* Non-HDD: accept CTX only if date matches TXT mtime */
                u32 txt_date = ls_get_filedate(txt_path);
                if (txt_date == ctx_date) {
                    fread(word_509B40, 0x620, 1, f);
                    fclose(f);
                    print("palette_postprocess_configure: loaded CTX cache (date match)\n");
                    return;
                }
                fclose(f);
                /* Fall through to re-parse TXT */
            }
        }
    }

    /* -- Parse PERSOS>INVENT.TXT into word_509B40 ------------------------- */
    /*
     * Original asm flow:
     *   ls_switch_memory(ui_special_meter_buffer)
     *   filesize = ls_get_filesize(txt_path)
     *   buf = allocate_memory(ui_special_meter_buffer, filesize)
     *   ls_switch_memory(ui_special_meter_buffer)
     *   txtlen = ls_get_filesize(txt_path)          ; re-read after switch
     *   ls_load_file_at(txt_path, buf)
     *   var_21C = 0                                  ; persona index 0..97
     *   loop outer (persona_idx < 98):
     *     var_10C = buf                              ; cursor into file data
     *     sprintf(key, "P%02d :", persona_idx)      ; e.g. "P00 :"
     *     inner search loop: scan byte-by-byte for key (strncmp 5 chars)
     *       if match or past end → break
     *     if past end → skip this persona
     *     advance cursor past 6-byte key ("P%02d :")
     *     var_8 = 0                                  ; slot index 0..7
     *     slot loop:
     *       if *cursor == 'x' → stop (end-of-entry marker)
     *       val = atoi(cursor)
     *       word_509B40[persona_idx * 8 + slot] = (i16)val
     *       advance cursor: val>100 → +4, val>10 → +3, else → +2
     *       slot++
     *     persona_idx++
     *   free_memory(buf)
     *   write CTX cache: open ctx_path, write date+word_509B40
     *
     * Helpers: ls_switch_memory, ls_load_file_at, ls_get_filedate.
     */

    i32 txt_size = ls_get_filesize(txt_path);
    if (txt_size <= 0) {
        print("palette_postprocess_configure: INVENT.TXT not found or empty\n");
        return;
    }

    /* ls_switch_memory(ui_special_meter_buffer) @ 425E73 */
    ls_switch_memory(MEM_SLOT_SPECIAL);

    /* Allocate a buffer for the TXT content */
    c8 *buf = (c8 *)allocate_memory(MEM_SLOT_SPECIAL, txt_size + 1);
    if (!buf) {
        print("palette_postprocess_configure: allocate_memory failed\n");
        return;
    }

    /* ls_switch_memory(ui_special_meter_buffer) @ 425E88 */
    ls_switch_memory(MEM_SLOT_SPECIAL);

    /* ls_load_file_at(txt_path, buf) @ 425E9B */
    i32 loaded = ls_load_file_at(txt_path, buf, txt_size);
    buf[loaded] = '\0'; /* NUL-terminate for safety */

    i32 txt_len = txt_size; /* var_110 = filesize (re-read in orig) */

    /* -- Outer persona loop (98 personas, indices 0..97) ------------------ */
    for (i32 persona_idx = 0; persona_idx < 0x62; persona_idx++) {
        /* var_10C = buf (cursor reset to start of file for each persona) */
        c8 *cursor = buf;

        /* Build search key "P%02d :" (5 chars compared by strncmp) */
        c8 key[8];
        snprintf(key, sizeof(key), "P%02d :", persona_idx);

        /* Scan for key in file data (byte-by-byte, stop near end) */
        while (1) {
            i32 offset = (i32)(cursor - buf);
            if (offset >= txt_len - 5) {
                /* Past (filesize - 6): persona not found → skip */
                goto next_persona;
            }
            if (strncmp(cursor, key, 5) == 0) break;
            cursor++;
        }

        /* Found: advance past 6-byte key "P%02d :" */
        cursor += 6;

        /* -- Inner slot loop (up to 8 slots per persona) ------------------ */
        for (i32 slot = 0; slot < 8; slot++) {
            i32 offset = (i32)(cursor - buf);
            /* Bounds check: stop if within 6 bytes of end */
            if (offset >= txt_len - 5) break;

            /* 'x' terminates the entry (end-of-slot marker) */
            if ((u8)*cursor == 'x') break;

            i32 val = atoi(cursor);
            word_509B40[persona_idx * 8 + slot] = (i16)val;

            /* Advance cursor based on decimal digit count */
            if (val > 100) cursor += 4;
            else if (val > 10) cursor += 3;
            else cursor += 2;
        }

    next_persona:
        ; /* GCC requires a statement after a label */
    }

    free_memory(buf);

    /* -- Write CTX cache -------------------------------------------------- */
    {
        u32 txt_date = ls_get_filedate(txt_path);
        FILE *cf = fopen(ctx_path, "wb");
        if (cf) {
            fwrite(&txt_date, 4, 1, cf);
            fwrite(word_509B40, 0x620, 1, cf);
            fclose(cf);
            print("palette_postprocess_configure: wrote CTX cache\n");
        }
    }

    print("palette_postprocess_configure: parsed INVENT.TXT (%d personas)\n", 0x62);
}

/* ---------------------------------------------------------------------------
 * sub_491450  (0x00491450)
 *
 *   push  esi
 *   mov   ecx, 22h          ; 34 dwords = 136 bytes
 *   push  edi
 *   mov   esi, offset alternative_palette_data
 *   mov   edi, offset palette_target_buffer
 *   rep   movsd
 *   pop   edi
 *   pop   esi
 *   retn
 *
 * Copies the alternative palette data into the active palette target buffer.
 * --------------------------------------------------------------------------*/
// 00491450
void palette_copy_alternative(void) {
    memcpy(palette_target_buffer, alternative_palette_data, sizeof(palette_target_buffer));
    print("palette_copy_alternative: copied %u bytes to palette_target_buffer\n",
          (u32)sizeof(palette_target_buffer));
}
