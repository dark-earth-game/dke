#ifndef DKE_DIALOGUE_H
#define DKE_DIALOGUE_H

/*
 * dialogue.h — minimal LIEU dialogue: subtitle text (NWDIAL%02d.TXT) +
 * spoken WAV (dialog/L%02d/<code>.WAV), driven by LIEU L-code line ids.
 *
 * The LIEU script queues a DIALOGUE action per line (sub_4449A5); when it
 * reaches the head of its pile, dlg_start shows it (sub_46BC74) and the
 * single slot stays busy for the voice length. dlg_active_text is drawn as
 * a subtitle by the scene.
 */
#include "libsys/common.h"

/* sub_46BC74: show + voice line `code` (e.g. "L01D02") now; returns the
 * display time in ms. Callers check dlg_is_active() first. */
u32 dlg_start(const c8 *code, u32 now_ms);

/* Per-frame: record the level/root/lang context and close the line when its
 * timer runs out (advance = 1 cuts it short). root = game_path, lang "US". */
void dlg_tick(i32 level, const c8 *root, const c8 *lang, u32 now_ms, i32 advance);

/* True while a line holds the dialogue slot (dword_4FF098 != 0). */
i32 dlg_is_active(void);

/* Current subtitle text, or NULL if none showing. */
const c8 *dlg_active_text(void);

/* Language int (DKE.CFG Language) -> 2-letter folder code, e.g. 2 -> "US". */
const c8 *dlg_lang_code(i32 id);

/* Reset queue + active line (on level enter/exit). */
void dlg_reset(void);

#endif /* DKE_DIALOGUE_H */
