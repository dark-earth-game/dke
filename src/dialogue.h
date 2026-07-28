#ifndef DKE_DIALOGUE_H
#define DKE_DIALOGUE_H

/*
 * dialogue.h — minimal LIEU dialogue: subtitle text (NWDIAL%02d.TXT) +
 * spoken WAV (dialog/L%02d/<code>.WAV), driven by LIEU L-code line ids.
 *
 * The LIEU script registers dialogue via named_object_int(name=L-code).
 * dlg_say queues the line; dlg_tick plays them one at a time (busy timer,
 * mirroring the engine's single-slot sub_46BC74); dlg_active_text is drawn
 * as a subtitle by the scene.
 */
#include "libsys/common.h"

/* Queue a dialogue line by its L-code (e.g. "L01D02"). */
void dlg_say(const c8 *code);

/* Per-frame: advance the queue; when idle, pop next line, load its text,
 * play its WAV, arm the display timer. root = game_path, lang e.g. "FR". */
void dlg_tick(i32 level, const c8 *root, const c8 *lang, u32 now_ms);

/* Current subtitle text, or NULL if none showing. */
const c8 *dlg_active_text(void);

/* Language int (DKE.CFG Language) -> 2-letter folder code, e.g. 2 -> "US". */
const c8 *dlg_lang_code(i32 id);

/* Reset queue + active line (on level enter/exit). */
void dlg_reset(void);

#endif /* DKE_DIALOGUE_H */
