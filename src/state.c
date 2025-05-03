#include "state.h"
#include "core/common.h"

inline void state_init(state_t *state) {
    state->system.quit = false;
    state->system.title = "DKE: Dark Earth";
    state->game_type = DKE_UNKNOWN;
}

inline void state_release(state_t *state) {
    memory_free(state);
}
