#include "state.h"
#include "libsys/common.h"

inline void state_init(state_t *state) {
    state->game_type    = DKE_UNKNOWN;
    state->audio_enabled = true;
    state->music_enabled = true;      /* win.ini [DKE] MOD=TRUE */
}

inline void state_release(state_t *state) {
    memory_free(state);
}
