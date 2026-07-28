/*
 * material.c — MAT material library loader (sub_45018B)
 *
 * Loads .MAT files exported by 3DITOR containing texture references
 * and material definitions for 3D models. Each .MAT bundles:
 *   - 32-byte header ("3DITOR v1.X MATR...")
 *   - 352-byte MaterialLib descriptor
 *   - N bitmap entries (276 bytes each) with texture filenames
 *   - M object entries (72 bytes each) mapping surfaces to textures
 *
 * Textures are .BMP in 3DITOR; renamed to .GIF/.CIF at load time.
 */

#include "material.h"
#include "libsys/file_system.h"
#include "libsys/graphics/cif.h"
#include "libsys/graphics/graphic.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

static void str_lower(c8 *s) {
    for (; *s; s++) *s = (c8)tolower((u8)*s);
}

static void *load_texture(const c8 *vpath, const c8 *game_path) {
    c8 resolved[512];
    if (!ls_resolve_path(game_path, vpath, resolved, (i32)sizeof(resolved)))
        return NULL;

    cif_image_t img;
    memset(&img, 0, sizeof(img));
    b8 ok = cif_load(&img, resolved);

    if (!ok) {
        i32 len = (i32)strlen(resolved);
        if (len >= 4) {
            c8 *ext = resolved + len - 3;
            if (toupper((u8)ext[0]) == 'G' &&
                toupper((u8)ext[1]) == 'I' &&
                toupper((u8)ext[2]) == 'F') {
                ext[0] = 'C'; ext[1] = 'I'; ext[2] = 'F';
                ok = cif_load(&img, resolved);
            }
        }
    }

    if (!ok) return NULL;

    bmap_t *bmap = (bmap_t *)malloc(sizeof(bmap_t));
    if (!bmap) { cif_free(&img); return NULL; }
    memset(bmap, 0, sizeof(bmap_t));

    bmap->width        = (i16)img.width;
    bmap->height       = img.height;
    bmap->bpp_flags    = 8;
    bmap->bit_depth    = 8;
    bmap->stride       = img.width;
    bmap->pixels       = img.pixels;
    bmap->first_pixel  = img.pixels;
    bmap->owns_buffer  = 1;
    bmap->pixel_format = 2;
    bmap->palette      = img.palette;
    bmap->owns_palette = (img.palette != NULL) ? 1 : 0;
    img.palette        = NULL;

    return bmap;
}

static void free_texture(void *tex) {
    if (!tex) return;
    bmap_t *bmap = (bmap_t *)tex;
    if (bmap->owns_buffer && bmap->pixels)  free(bmap->pixels);
    if (bmap->owns_palette && bmap->palette) free(bmap->palette);
    free(bmap);
}

/* Find last '>' separator in a path and return pointer after it */
static c8 *path_last_sep(c8 *path) {
    c8 *last = NULL;
    for (c8 *p = path; *p; p++)
        if (*p == '>') last = p;
    return last ? last + 1 : path;
}

mat_lib_t *mat_load(const c8 *path, const c8 *game_path, i32 is_hdd,
                    void *default_tex) {
    if (!path || !game_path) return NULL;

    const c8 *clean = path;
    if (clean[0] == '#') clean++;

    c8 resolved[512];
    if (!ls_resolve_path(game_path, clean, resolved, (i32)sizeof(resolved))) {
        print("mat_load: path overflow '%s'\n", clean);
        return NULL;
    }

    FILE *f = ls_open(resolved, "rb");
    if (!f) {
        print("mat_load: cannot open '%s'\n", resolved);
        return NULL;
    }

    /* Phase 1: Read & validate 32-byte header */
    c8 hdr[MAT_HEADER_SIZE];
    ls_read_buffer(f, hdr, MAT_HEADER_SIZE);

    if (strncmp(hdr + 7, "v1.3", 4) != 0 &&
        strncmp(hdr + 12, "MATR", 4) != 0 &&
        strncmp(hdr + 7, "v1.1", 4) != 0) {
        print("mat_load: bad header in '%s'\n", clean);
        ls_close(f);
        return NULL;
    }

    /* Phase 2: Read 352-byte MaterialLib */
    mat_lib_t *lib = (mat_lib_t *)calloc(1, sizeof(mat_lib_t));
    if (!lib) { ls_close(f); return NULL; }

    u8 raw[MAT_LIB_SIZE];
    ls_read_buffer(f, raw, MAT_LIB_SIZE);

    memcpy(lib->header, raw, 16);
    memcpy(lib->misc, raw + 0x100, 0x34);
    memcpy(&lib->nmaterials, raw + 0x134, 4);
    memcpy(&lib->nobjects,   raw + 0x138, 4);
    memcpy(&lib->nbitmaps,   raw + 0x13C, 4);

    strncpy(lib->name, clean, MAT_NAME_LEN - 1);
    lib->name[MAT_NAME_LEN - 1] = '\0';

    print("mat_load: '%s' — %u bitmaps, %u objects, %u materials\n",
          clean, lib->nbitmaps, lib->nobjects, lib->nmaterials);

    if (lib->nbitmaps == 0) {
        ls_close(f);
        return lib;
    }

    /* Phase 3: Read bitmap entries */
    u32 nb = lib->nbitmaps;
    mat_bitmap_t *bmps = (mat_bitmap_t *)calloc(nb, sizeof(mat_bitmap_t));
    if (!bmps) { free(lib); ls_close(f); return NULL; }
    lib->bitmaps = bmps;

    for (u32 i = 0; i < nb; i++) {
        u8 braw[MAT_BITMAP_SIZE];
        ls_read_buffer(f, braw, MAT_BITMAP_SIZE);
        memcpy(bmps[i].filename, braw, MAT_NAME_LEN);
        memcpy(&bmps[i].uv_field, braw + 0x100, 4);
        bmps[i].texture = NULL;
        memcpy(bmps[i].extra, braw + 0x108, 12);
    }

    /* Phase 4: Rename extensions .BMP -> .GIF and build full paths */
    c8 path_cache[MAT_MAX_TEXTURES][256];
    u32 cache_count = (nb < MAT_MAX_TEXTURES) ? nb : MAT_MAX_TEXTURES;

    c8 mat_dir[512];
    strncpy(mat_dir, clean, sizeof(mat_dir) - 1);
    mat_dir[sizeof(mat_dir) - 1] = '\0';

    for (u32 i = 0; i < nb; i++) {
        i32 len = (i32)strlen(bmps[i].filename);
        if (len >= 3) {
            bmps[i].filename[len - 3] = 'G';
            bmps[i].filename[len - 2] = 'I';
            bmps[i].filename[len - 1] = 'F';
        }

        c8 fullpath[512];
        if (len > 1 && bmps[i].filename[1] == ':') {
            snprintf(fullpath, sizeof(fullpath), "%s",
                     &bmps[i].filename[11]);
        } else {
            strncpy(fullpath, mat_dir, sizeof(fullpath) - 1);
            fullpath[sizeof(fullpath) - 1] = '\0';
            c8 *sep = path_last_sep(fullpath);
            strncpy(sep, bmps[i].filename,
                    sizeof(fullpath) - (u32)(sep - fullpath) - 1);
        }
        str_lower(fullpath);
        if (i < cache_count)
            strncpy(path_cache[i], fullpath, 255);
    }

    /* Phase 5: Read object entries (temporary) */
    u32 nobj = lib->nobjects;
    u32 nmat = lib->nmaterials;
    mat_object_t *objs = NULL;

    if (nobj > 0) {
        objs = (mat_object_t *)calloc(nobj, sizeof(mat_object_t));
        if (objs) {
            for (u32 i = 0; i < nobj; i++)
                ls_read_buffer(f, &objs[i], MAT_OBJECT_SIZE);
        }
    }

    if (nmat > 0) {
        lib->materials = calloc(nmat, MAT_MATERIAL_SIZE);
    }

    /* Phase 6: Load textures */
    for (u32 i = 0; i < nb && i < cache_count; i++) {
        c8 *fp = path_cache[i];
        i32 fplen = (i32)strlen(fp);

        if (fplen >= 10 && strcmp(fp + fplen - 10, "objets.gif") == 0) {
            bmps[i].texture = default_tex;
            continue;
        }

        if (is_hdd && fplen > 7) {
            c8 *after_sep = path_last_sep(fp);
            if (strncasecmp(after_sep, "flam", 4) == 0)
                strncpy(fp, "datas>objets>flam.gif", 255);
        }

        bmps[i].texture = load_texture(fp, game_path);
        if (!bmps[i].texture)
            print("mat_load: texture '%s' not found\n", fp);

        if (bmps[i].texture) {
            bmap_t *bm = (bmap_t *)bmps[i].texture;
            if (bm->width > 256) {
                print("mat_load: texture '%s' width %d > 256, clamped\n",
                      fp, bm->width);
            }
        }
    }

    /* Phase 7: Map objects -> materials */
    if (objs && lib->materials && nmat > 0) {
        u8 *mats = (u8 *)lib->materials;
        u32 out_idx = 0;

        for (u32 i = 0; i < nobj && out_idx < nmat; i++) {
            objs[i].flags &= ~1u;

            if (objs[i].visible_flag == 0)
                objs[i].flags |= 0x100;

            if (((objs[i].flags >> 8) & 1) && ((objs[i].flags >> 0) & 2))
                continue;

            u32 tex_idx = objs[i].texture_index;
            if (tex_idx >= nb)
                tex_idx = nb - 1;

            if (tex_idx < nb && bmps[tex_idx].texture) {
                bmap_t *img = (bmap_t *)bmps[tex_idx].texture;
                memcpy(objs[i].data, &bmps[tex_idx].texture, sizeof(void *));
                i32 data_off = img->stride * img->height;
                u32 uv_sum = objs[i].v_offset + objs[i].u_offset;
                if (uv_sum > 256)
                    data_off += (i32)(uv_sum - 256) << 8;
                memcpy(objs[i].data + 8, &data_off, 4);
                memcpy(objs[i].data + 4, &bmps[tex_idx].uv_field, 4);
            }

            memcpy(mats + out_idx * MAT_MATERIAL_SIZE, &objs[i],
                   MAT_MATERIAL_SIZE);

            if (i < MAT_NAME_LEN)
                lib->name[MAT_NAME_LEN - 1 - i] = (c8)out_idx;

            out_idx++;
        }

        if (out_idx != nmat)
            print("mat_load: warning — nmaterials mismatch (%u vs %u) in '%s'\n",
                  out_idx, nmat, clean);
    }

    free(objs);
    ls_close(f);
    return lib;
}

void mat_free(mat_lib_t *lib) {
    if (!lib) return;

    if (lib->bitmaps) {
        mat_bitmap_t *bmps = (mat_bitmap_t *)lib->bitmaps;
        for (u32 i = 0; i < lib->nbitmaps; i++)
            free_texture(bmps[i].texture);
        free(bmps);
    }

    if (lib->materials) free(lib->materials);
    free(lib);
}
