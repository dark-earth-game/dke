#include "libsys/file_system.h"
#include "game_detection.h"

game_type_e detect_game() {
    return DKE_UK;
}

const c8* get_game_title(game_type_e game) {
    switch (game) {
        case DKE_FR: return "DKE: Dark Earth (French)";
        case DKE_UK: return "DKE: Dark Earth (English)";
        case DKE_US: return "DKE: Dark Earth (English US)";
        case DKE_DE: return "DKE: Dark Earth (German)";
        case DKE_IT: return "DKE: Dark Earth (Italian)";
        case DKE_SP: return "DKE: Dark Earth (Spanish)";
        case DKE_JP: return "DKE: Dark Earth (Japanese)";
        case DKE_PL: return "DKE: Dark Earth (Polish)";
        case DKE_RU: return "DKE: Dark Earth (Russian)";
        case DKE_CZ: return "DKE: Dark Earth (Czech)";
        case DKE_PT: return "DKE: Dark Earth (Portuguese)";
        case DKE_DEMO: return "DKE: Dark Earth Demo";
        default: return "DKE: Dark Earth";
    }
}
