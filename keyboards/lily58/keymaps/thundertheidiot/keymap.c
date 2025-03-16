#include "tapdance.h"
#include QMK_KEYBOARD_H

enum layer_number { _QWERTY = 0, _NORMAL, _GAME, _NUMPAD, _NAV, _LOWER, _RAISE, _ADJUST, _EMPTY };

enum custom_keycodes {
    KC_MODESC,
    INTERNAL_LGUI_GESC,
};

// wrap this to get record->tap.count
#define LGUI_GESC LGUI_T(INTERNAL_LGUI_GESC)

#define CU_TAB LT(_NUMPAD, KC_TAB)
#define CU_A LGUI_T(KC_A)
#define CU_S LALT_T(KC_S)
#define CU_D LSFT_T(KC_D)
#define CU_F LCTL_T(KC_F)

#define CU_J LCTL_T(KC_J)
#define CU_K RSFT_T(KC_K)
#define CU_L LALT_T(KC_L)
#define CU_SCLN LGUI_T(KC_SCLN)

#define CU_SPC LT(_NUMPAD, KC_SPC)
/* #define CU_ENT LT(_NAV, KC_ENT) */
#define CU_ENT LCTL_T(KC_ENT)

#ifdef TAPPING_TERM_PER_KEY
uint16_t get_tapping_term(uint16_t keycode, keyrecord_t *record) {
    switch (keycode) {
        case CU_A:
        case CU_S:
        case CU_D:
        case CU_F:
        case CU_J:
        case CU_K:
        case CU_L:
        case CU_SCLN:
            return 175;
        case CU_SPC:
            return 150;
        default:
            return TAPPING_TERM;
    }
}
#endif

// CAPS WORD

bool caps_word_press_user(uint16_t keycode) {
    switch (keycode) {
        case KC_A ... KC_Z:
        case KC_MINS:
        case KC_SCLN:
        case KC_QUOT:
            add_weak_mods(MOD_BIT(KC_LSFT));
            return true;

        case KC_1 ... KC_0:
        case KC_BSPC:
        case KC_DEL:
        case KC_UNDS:
            return true;

        default:
            return false;
    }
}

// TAPDANCE ENUM

enum {
    TD_NORMAL,
};

void td_normal_finished(tap_dance_state_t *state, void *user_data);
void td_normal_reset(tap_dance_state_t *state, void *user_data);

tap_dance_action_t tap_dance_actions[] = {[TD_NORMAL] = ACTION_TAP_DANCE_FN_ADVANCED(NULL, td_normal_finished, td_normal_reset)};
void               keyboard_post_init_user(void) {
    set_tri_layer_layers(_LOWER, _RAISE, _EMPTY);
#ifdef CONSOLE_ENABLE
    debug_enable = true;
    debug_matrix = true;
#endif
}

layer_state_t layer_state_set_user(layer_state_t state) {
    return update_tri_layer_state(state, _LOWER, _RAISE, _EMPTY);
}

// KEYMAPS

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {

    /* QWERTY
     * ,-----------------------------------------.                    ,-----------------------------------------.
     * | ESC  |   1  |   2  |   3  |   4  |   5  |                    |   6  |   7  |   8  |   9  |   0  |  `   |
     * |------+------+------+------+------+------|                    |------+------+------+------+------+------|
     * | Tab  |   Q  |   W  |   E  |   R  |   T  |                    |   Y  |   U  |   I  |   O  |   P  |  -   |
     * |------+------+------+------+------+------|                    |------+------+------+------+------+------|
     * |LCTRL |   A  |   S  |   D  |   F  |   G  |-------.    ,-------|   H  |   J  |   K  |   L  |   ;  |  '   |
     * |------+------+------+------+------+------|   [   |    |    ]  |------+------+------+------+------+------|
     * |LShift|   Z  |   X  |   C  |   V  |   B  |-------|    |-------|   N  |   M  |   ,  |   .  |   /  |RShift|
     * `-----------------------------------------/       /     \      \-----------------------------------------'
     *                   | LAlt | LGUI |LOWER | /Space  /       \Enter \  |RAISE |BackSP| RGUI |
     *                   |      |      |      |/       /         \      \ |      |      |      |
     *                   `----------------------------'           '------''--------------------'
     */

    [_QWERTY] = LAYOUT(KC_LCTL, KC_1, KC_2, KC_3, KC_4, KC_5, KC_6, KC_7, KC_8, KC_9, KC_0, KC_BSPC, CU_TAB, KC_Q, KC_W, KC_E, KC_R, KC_T, KC_Y, KC_U, KC_I, KC_O, KC_P, KC_BSLS, LGUI_GESC, CU_A, CU_S, CU_D, CU_F, KC_G, KC_H, CU_J, CU_K, CU_L, CU_SCLN, KC_QUOT, SC_LSPO, KC_Z, KC_X, KC_C, KC_V, KC_B, KC_LBRC, KC_RBRC, KC_N, KC_M, KC_COMM, KC_DOT, KC_SLSH, SC_RSPC, KC_LALT, KC_LGUI, TL_LOWR, CU_SPC, CU_ENT, TL_UPPR, TT(_NORMAL), KC_RCTL),
    [_NORMAL] = LAYOUT(KC_LCTL, KC_1, KC_2, KC_3, KC_4, KC_5, KC_6, KC_7, KC_8, KC_9, KC_0, KC_BSPC, KC_TAB, KC_Q, KC_W, KC_E, KC_R, KC_T, KC_Y, KC_U, KC_I, KC_O, KC_P, KC_BSLS, LGUI_GESC, KC_A, KC_S, KC_D, KC_F, KC_G, KC_H, KC_J, KC_K, KC_L, KC_SCLN, KC_QUOT, KC_LSFT, KC_Z, KC_X, KC_C, KC_V, KC_B, KC_LBRC, KC_RBRC, KC_N, KC_M, KC_COMM, KC_DOT, KC_SLSH, SC_SENT, KC_LALT, KC_LGUI, TT(_LOWER), KC_SPC, LCTL_T(KC_ENT), TT(_RAISE), TO(_QWERTY), KC_RCTL),

    [_GAME] = LAYOUT(KC_ESC, KC_1, KC_2, KC_3, KC_4, KC_5, KC_6, KC_7, KC_8, KC_9, KC_0, _______, KC_3, KC_TAB, KC_Q, KC_W, KC_E, KC_R, _______, _______, KC_UP, _______, _______, _______, KC_LSFT, KC_2, KC_A, KC_S, KC_D, KC_F, _______, KC_LEFT, KC_DOWN, KC_RIGHT, _______, _______, KC_LCTL, KC_1, KC_Z, KC_X, KC_C, KC_V, KC_G, _______, _______, _______, _______, _______, _______, _______, KC_LALT, KC_LGUI, KC_B, KC_SPC, KC_ENT, MO(_RAISE), TO(_QWERTY), KC_RCTL),

    [_NUMPAD] = LAYOUT(_______, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, _______, _______, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, KC_CIRC, KC_7, KC_8, KC_9, KC_ASTR, XXXXXXX, XXXXXXX, KC_LGUI, KC_LALT, KC_LSFT, KC_LCTL, XXXXXXX, KC_MINS, KC_4, KC_5, KC_6, KC_EQL, KC_PIPE, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, _______, _______, KC_PLUS, KC_1, KC_2, KC_3, KC_SLSH, _______, OSM(MOD_MEH), _______, _______, _______, _______, _______, KC_0, KC_PDOT),

    [_NAV] = LAYOUT(_______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, KC_WH_U, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, KC_WH_L, KC_WH_D, KC_WH_R, _______, KC_LEFT, KC_DOWN, KC_UP, KC_RIGHT, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______),

    [_LOWER] = LAYOUT(_______, _______, _______, _______, _______, _______, _______, _______, _______, _______, KC_MINS, KC_EQL, KC_F1, KC_F2, KC_F3, KC_F4, KC_F5, KC_F6, KC_F7, KC_F8, KC_F9, KC_F10, KC_F11, KC_F12, QK_BOOT, _______, _______, _______, _______, KC_LBRC, KC_RBRC, _______, _______, _______, _______, _______, _______, _______, _______, _______, KC_MODESC, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, TO(_QWERTY), _______),

    [_RAISE] = LAYOUT(KC_HOME, KC_F13, KC_F14, KC_F15, KC_F16, KC_F17, KC_F18, KC_F19, KC_F20, KC_F21, KC_VOLD, KC_VOLU, _______, _______, _______, _______, _______, _______, KC_PGUP, KC_PGDN, _______, _______, KC_MPLY, _______, _______, _______, KC_PSCR, KC_DEL, _______, TO(_GAME), KC_LEFT, KC_DOWN, KC_UP, KC_RIGHT, _______, XXXXXXX, _______, _______, _______, _______, _______, _______, KC_MPRV, KC_MNXT, KC_PLUS, KC_MINS, KC_END, KC_LBRC, KC_RBRC, KC_DEL, _______, _______, _______, _______, _______, _______, TO(_QWERTY), _______),

    [_EMPTY] = LAYOUT(_______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______),
};

// TAPDANCE
static td_tap_t td_normal_state = {.is_press_action = true, .state = TD_NONE};

void td_normal_finished(tap_dance_state_t *state, void *user_data) {
    td_normal_state.state = cur_dance(state);
    switch (td_normal_state.state) {
        case TD_SINGLE_TAP:
            tap_code(KC_A);
            break;
        case TD_SINGLE_HOLD:
            register_code16(KC_B);
            break;
        case TD_DOUBLE_TAP:
            tap_code16(KC_C);
            break;
        case TD_DOUBLE_HOLD:
            tap_code16(KC_D);
            break;
        default:
            break;
    }
}

void td_normal_reset(tap_dance_state_t *state, void *user_data) {
    switch (td_normal_state.state) {
        case TD_SINGLE_HOLD:
            unregister_code16(MO(_NORMAL));
            break;
        default:
            break;
    }
}

// KEYCODES

/* static uint32_t key_timer; */

static uint8_t custom_key_data = 0;
#define MODESC (1 << 0)

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    int8_t tap   = record->tap.count;
    int8_t press = record->event.pressed;

    switch (keycode) {
        case LGUI_GESC:
            caps_word_off();

            int8_t shifted = get_mods() & MOD_MASK_SG;

            if (!shifted && tap && press) {
                tap_code(KC_ESC);
            } else if ((shifted && press) || (shifted && tap && press)) {
                register_code(KC_GRAVE);
            } else if (!shifted && press) {
                register_mods(MOD_MASK_GUI);
                custom_key_data |= MODESC;
            } else {
                if (custom_key_data & MODESC) {
                    unregister_mods(MOD_MASK_GUI);
                    custom_key_data &= ~MODESC;
                } else if (shifted) {
                    unregister_code(KC_GRAVE);
                }
            }
            return false;
            break;
    }
    return true;
}
