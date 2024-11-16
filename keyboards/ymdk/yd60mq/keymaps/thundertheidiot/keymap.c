#include QMK_KEYBOARD_H
/* #include <rgblight.h> */

enum layers { _DEFAULT = 0, _NORMAL, _GAME, _NUMPAD, _FN };

enum custom_keycodes {
    INTERNAL_LGUI_GESC = SAFE_RANGE,
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

/* void keyboard_post_init_user(void) { */
/*   rgblight_setrgb(127, 0, 127); */
/* } */

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

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {

    [_DEFAULT] = LAYOUT_all(KC_ESC, KC_1, KC_2, KC_3, KC_4, KC_5, KC_6, KC_7, KC_8, KC_9, KC_0, KC_MINS, KC_EQL, KC_BSLS, KC_BSPC, KC_TAB, KC_Q, KC_W, KC_E, KC_R, KC_T, KC_Y, KC_U, KC_I, KC_O, KC_P, KC_LBRC, KC_RBRC, KC_BSLS, LGUI_GESC, CU_A, CU_S, CU_D, CU_F, KC_G, KC_H, CU_J, CU_K, CU_L, CU_SCLN, KC_QUOT, KC_NUHS, KC_ENT, KC_LSFT, KC_NUBS, KC_Z, KC_X, KC_C, KC_V, KC_B, KC_N, KC_M, KC_COMM, KC_DOT, KC_SLSH, KC_VOLD, KC_UP, KC_VOLU, KC_LCTL, KC_LGUI, KC_LALT, MO(1), CU_SPC, MO(1), TO(_DEFAULT), MO(_FN), KC_LEFT, KC_DOWN, KC_RGHT),

    [_NORMAL] = LAYOUT_all(KC_ESC, KC_1, KC_2, KC_3, KC_4, KC_5, KC_6, KC_7, KC_8, KC_9, KC_0, KC_MINS, KC_EQL, KC_BSLS, KC_BSPC, KC_TAB, KC_Q, KC_W, KC_E, KC_R, KC_T, KC_Y, KC_U, KC_I, KC_O, KC_P, KC_LBRC, KC_RBRC, KC_BSLS, LGUI_GESC, KC_A, KC_S, KC_D, KC_F, KC_G, KC_H, KC_J, KC_K, KC_L, KC_SCLN, KC_QUOT, KC_NUHS, KC_ENT, KC_LSFT, KC_NUBS, KC_Z, KC_X, KC_C, KC_V, KC_B, KC_N, KC_M, KC_COMM, KC_DOT, KC_SLSH, KC_VOLD, KC_UP, KC_VOLU, KC_LCTL, KC_LGUI, KC_LALT, MO(1), KC_SPC, MO(1), _______, _______, KC_LEFT, KC_DOWN, KC_RGHT),

    [_NUMPAD] = LAYOUT_all(_______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, KC_7, KC_8, KC_9, _______, _______, _______, _______, _______, KC_LGUI, KC_LALT, KC_LSFT, KC_LCTL, _______, _______, KC_4, KC_5, KC_6, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, KC_1, KC_2, KC_3, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______),

    [_FN] = LAYOUT_all(QK_BOOT, KC_F1, KC_F2, KC_F3, KC_F4, KC_F5, KC_F6, KC_F7, KC_F8, KC_F9, KC_F10, KC_F11, KC_F12, _______, KC_DEL, _______, _______, _______, _______, UG_TOGG, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, KC_PSCR, _______, _______, TG(_NORMAL), _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, KC_END, _______, _______, _______, _______, KC_DEL, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______),

    /* 	[_NUMPAD] = LAYOUT_all( */
    /*     _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, */
    /*     _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, */
    /*     _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, */
    /*     _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, */
    /*     _______, _______, _______,                   _______, _______, _______,                   _______, _______, _______, _______, _______ */
    /* ), */
};

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
