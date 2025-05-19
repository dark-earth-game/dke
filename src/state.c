#include "state.h"
#include "libsys/common.h"

inline void state_init(state_t *state) {
    state->game_type = DKE_UNKNOWN;
}

inline void state_release(state_t *state) {
    memory_free(state);
}
