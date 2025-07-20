#include "tapdance.h"
#include "print.h"
#include QMK_KEYBOARD_H

enum layer_number { _QWERTY = 0, _COLEMAKDH, _USFI, _SYMBOL, _GAME, _NUMPAD, _LOWER, _RAISE, _ADJUST, _EMPTY };

enum custom_keycodes {
    KC_MODESC = SAFE_RANGE,
    INTERNAL_LGUI_GESC,

    // Fake US Layout
    FI_2,
    FI_4,
    FI_6,
    FI_7,
    FI_8,
    FI_9,
    FI_0,
    FI_COMM,
    FI_DOT,
    FI_SLSH,
    FI_SCLN,
    FI_QUOT,

    FI_LBRC,
    FI_RBRC,
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

#define CU_Z LT(_SYMBOL, KC_Z)
#define CU_SLSH LT(_SYMBOL, KC_SLSH)

#define CU_LOWR LT(_LOWER, KC_ESC)
#define CU_RAISE LT(_RAISE, KC_BSPC)

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

tap_dance_action_t tap_dance_actions[] = {};

void keyboard_post_init_user(void) {
    set_tri_layer_layers(_LOWER, _RAISE, _ADJUST);
#ifdef CONSOLE_ENABLE
    debug_enable = true;
    debug_matrix = false;
#endif
}

layer_state_t layer_state_set_user(layer_state_t state) {
    return update_tri_layer_state(state, _LOWER, _RAISE, _ADJUST);
}

// KEYMAPS

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {

    // clang-format off
    [_QWERTY] = LAYOUT(
		       KC_LCTL, KC_1,    KC_2,    KC_3,    KC_4,    KC_5,                      KC_6,    KC_7,    KC_8,    KC_9,    KC_0,    KC_BSPC,
		       CU_TAB,  KC_Q,    KC_W,    KC_E,    KC_R,    KC_T,                      KC_Y,    KC_U,    KC_I,    KC_O,    KC_P,    KC_BSLS,
		       LGUI_GESC,CU_A,   CU_S,    CU_D,    CU_F,    KC_G,                      KC_H,    CU_J,    CU_K,    CU_L,    CU_SCLN, KC_QUOT,
		       KC_LSFT, CU_Z,    KC_X,    KC_C,    KC_V,    KC_B,    KC_LBRC, KC_RBRC, KC_N,    KC_M,    KC_COMM, KC_DOT,  CU_SLSH, KC_RSFT,
		                                  KC_LALT, KC_LGUI, CU_LOWR, CU_SPC,  CU_ENT,  CU_RAISE,KC_NO,   KC_RCTL
		     ),

    [_USFI] = LAYOUT(
		       _______, _______, FI_2,    _______, FI_4,    _______,                   FI_6,    FI_7,    FI_8,    FI_9,    FI_0,    _______,
		       _______, _______, _______, _______, _______, _______,                   _______, _______, _______, _______, _______, _______,
		       _______, _______, _______, _______, _______, _______,                   _______, _______, _______, _______, _______, _______,
		       _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,
		                                  _______, _______, _______, _______, _______, _______, TO(_QWERTY), _______
		     ),

    [_SYMBOL] = LAYOUT(
		       _______, _______, _______, _______, _______, _______,                   _______, _______, _______, _______, _______, _______,
		       _______, KC_GRAVE,KC_LT,   KC_GT,   KC_MINS, KC_PIPE,                   KC_CIRC, KC_LCBR, KC_RCBR, KC_DLR,  KC_UNDS, _______,
		       _______, KC_EXLM, KC_ASTR, KC_SLSH, KC_EQL,  KC_AMPR,                   KC_HASH, KC_LPRN, KC_RPRN, KC_SCLN, KC_DQUO, _______,
		       _______, KC_TILD, KC_PLUS, KC_LBRC, KC_RBRC, KC_PERC, _______, _______, KC_AT,   KC_COLN, KC_COMM, KC_DOT,  KC_QUOT, _______,
		                                  _______, _______, _______, _______, _______, _______, _______, _______
		     ),

    [_GAME] = LAYOUT(
		       KC_ESC, _______,  _______, _______, _______, _______,                   _______, _______, _______, _______, _______, _______,
		       KC_3,    KC_TAB,  KC_Q,    KC_W,    KC_E,    KC_R,                      _______, _______, KC_UP,   _______, _______, _______,
		       KC_LSFT, KC_2,    KC_A,    KC_S,    KC_D,    KC_F,                      _______, KC_LEFT, KC_DOWN, KC_RIGHT,_______, _______,
		       KC_LCTL, KC_1,    KC_Z,    KC_X,    KC_C,    KC_V,    KC_G,    _______, _______, _______, _______, _______, _______, _______,
		                                  KC_LALT, KC_LGUI, KC_B,    KC_SPC,  KC_ENT,  TL_UPPR, TO(_QWERTY), _______
		     ),

    [_NUMPAD] = LAYOUT(
		       _______, _______, _______, _______, _______, _______,                   _______, _______, _______, _______, KC_MINS, KC_EQL,
		       _______, _______, _______, _______, _______, _______,                   _______, KC_7,    KC_8,    KC_9,    _______, _______,
		       _______, KC_LGUI, KC_LALT, KC_LSFT, KC_LCTL, _______,                   _______, KC_4,    KC_5,    KC_6,    _______, _______,
		       _______, _______, _______, _______, _______, _______, _______, _______, _______, KC_1,    KC_2,    KC_3,    _______, _______,
		                                  _______, _______, _______, _______, KC_ENT,  KC_0,    _______, _______
		     ),

    [_LOWER] = LAYOUT(
		       _______, _______, _______, _______, _______, _______,                   _______, _______, _______, _______, KC_MINS, KC_EQL,
		       KC_F1,   KC_F2,   KC_F3,   KC_F4,   KC_F5,   KC_F6,                     KC_F7,   KC_F8,   KC_F9,   KC_F10,  KC_F11,  KC_F12,
		       QK_BOOT, _______, _______, _______, _______, _______,                   KC_LEFT, KC_DOWN, KC_UP,  KC_RIGHT, _______, _______,
		       _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,
		                                  _______, _______, _______, _______, _______, _______, TO(_QWERTY), _______
		     ),

    [_RAISE] = LAYOUT(
		      KC_HOME,  KC_F13,  KC_F14,  KC_F15,  KC_F16,  KC_F17,                    KC_F18,  KC_F19,  KC_F20,  KC_F21,  KC_VOLD, KC_VOLU,
		      _______, KC_TAB,  _______, _______, _______, _______,                    KC_PGUP, KC_PGDN, _______, _______, KC_MPLY, _______,
		      _______, _______, KC_PSCR, KC_DEL, TG(_USFI), TO(_GAME),                 KC_LEFT, KC_DOWN, KC_UP,   KC_RIGHT,_______, _______,
		      _______, _______, _______, _______, _______, _______, KC_MPRV, KC_MNXT,  KC_PLUS, KC_MINS, KC_END, KC_LBRC, KC_RBRC,  KC_DEL,
		                                 _______, _______, _______, _______, _______, _______, TO(_QWERTY), _______
		      ),

    [_EMPTY] = LAYOUT(
		       _______, _______, _______, _______, _______, _______,                   _______, _______, _______, _______, _______, _______,
		       _______, _______, _______, _______, _______, _______,                   _______, _______, _______, _______, _______, _______,
		       _______, _______, _______, _______, _______, _______,                   _______, _______, _______, _______, _______, _______,
		       _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,
		                                  _______, _______, _______, _______, _______, _______, _______, _______
		     ),

    // clang-format on
};

// OLED
#ifdef OLED_ENABLE

static void render_logo(void) {
    static const char PROGMEM qmk_logo[] = {0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0x89, 0x8A, 0x8B, 0x8C, 0x8D, 0x8E, 0x8F, 0x90, 0x91, 0x92, 0x93, 0x94, 0xA0, 0xA1, 0xA2, 0xA3, 0xA4, 0xA5, 0xA6, 0xA7, 0xA8, 0xA9, 0xAA, 0xAB, 0xAC, 0xAD, 0xAE, 0xAF, 0xB0, 0xB1, 0xB2, 0xB3, 0xB4, 0xC0, 0xC1, 0xC2, 0xC3, 0xC4, 0xC5, 0xC6, 0xC7, 0xC8, 0xC9, 0xCA, 0xCB, 0xCC, 0xCD, 0xCE, 0xCF, 0xD0, 0xD1, 0xD2, 0xD3, 0xD4, 0x00};

    oled_write_P(qmk_logo, false);
}

static bool oled_needs_redraw = false;

static void print_status_narrow(void) {
    if (oled_needs_redraw) {
        oled_needs_redraw = false;
        oled_clear();
    }

    if (is_caps_word_on()) {
        oled_write_ln_P(PSTR("MEOW"), false);
    } else {
        oled_write_ln_P(PSTR("meow"), false);
    }

    oled_write_ln_P(PSTR(":3\n"), false);

    // Print current layer
    oled_write_ln_P(PSTR("\n"), false);
    switch (get_highest_layer(layer_state)) {
        case _QWERTY:
            oled_write_P(PSTR("Base\n\n"), false);
            break;
        case _RAISE:
            oled_write_P(PSTR("Raise\n\n"), false);
            break;
        case _LOWER:
            oled_write_P(PSTR("Lower\n\n"), false);
            break;
        case _SYMBOL:
            oled_write_P(PSTR("Smbl\n\n"), false);
            break;
        case _ADJUST:
            oled_write_P(PSTR("Adj\n\n"), false);
            break;
        case _NUMPAD:
            oled_write_P(PSTR("Nump\n\n"), false);
            break;
        case _GAME:
            oled_write_P(PSTR("Game\n\n"), false);
            break;
        case _USFI:
            oled_write_P(PSTR("UsFi\n\n"), false);
            break;
        default:
            oled_write_ln_P(PSTR("Undef"), false);
    }
}

oled_rotation_t oled_init_user(oled_rotation_t rotation) {
    if (is_keyboard_master()) {
        return OLED_ROTATION_270;
    }
    return rotation;
}

bool oled_task_user(void) {
    if (is_keyboard_master()) {
        print_status_narrow();
    } else {
        render_logo();
    }
    return false;
}

void oled_render_boot(bool bootloader) {
    oled_clear();
    oled_write_P(PSTR("Boot-\nloader"), false);
    oled_render_dirty(true);
}

bool shutdown_user(bool jump_to_bootloader) {
    oled_render_boot(jump_to_bootloader);
    return true;
}

#endif

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

        // Fake US Layout
        case FI_2:
            if (record->event.pressed) {
                if (get_mods() & MOD_MASK_SHIFT) {
                    unregister_mods(MOD_MASK_SHIFT);

                    register_code(KC_RALT);
                    register_code(KC_2);

                    unregister_code(KC_2);
                    unregister_code(KC_RALT);

                    register_mods(get_mods() | MOD_MASK_SHIFT);
                } else {
                    tap_code(KC_2);
                }
            }
            return false;
            break;
        case FI_4:
            if (record->event.pressed) {
                if (get_mods() & MOD_MASK_SHIFT) {
                    unregister_mods(MOD_MASK_SHIFT);

                    register_code(KC_RALT);
                    register_code(KC_4);

                    unregister_code(KC_4);
                    unregister_code(KC_RALT);

                    register_mods(get_mods() | MOD_MASK_SHIFT);
                } else {
                    tap_code(KC_4);
                }
            }
            return false;
            break;
        case FI_6:
            if (record->event.pressed) {
                if (get_mods() & MOD_MASK_SHIFT) {
                    tap_code(KC_RBRC);
                } else {
                    tap_code(KC_6);
                }
            }
            return false;
            break;
        case FI_7:
            if (record->event.pressed) {
                if (get_mods() & MOD_MASK_SHIFT) {
                    tap_code(KC_6);
                } else {
                    tap_code(KC_7);
                }
            }
            return false;
            break;
        case FI_8:
            if (record->event.pressed) {
                if (get_mods() & MOD_MASK_SHIFT) {
                    tap_code(KC_BSLS);
                } else {
                    tap_code(KC_8);
                }
            }
            return false;
            break;
        case FI_9:
            if (record->event.pressed) {
                if (get_mods() & MOD_MASK_SHIFT) {
                    tap_code(KC_8);
                } else {
                    tap_code(KC_9);
                }
            }
            return false;
            break;
        case FI_0:
            if (record->event.pressed) {
                if (get_mods() & MOD_MASK_SHIFT) {
                    tap_code(KC_9);
                } else {
                    tap_code(KC_0);
                }
            }
            return false;
            break;
    }
    return true;
}
