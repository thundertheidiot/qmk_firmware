
/* Copyright 2021 Dane Evans
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <stdint.h>
#include <stdio.h>
#include "print.h"
#include "tapdance.h"

#include QMK_KEYBOARD_H

enum sofle_layers { _DEFAULTS = 0, _QWERTY = 0, _NORMAL, _GAME, _NUMPAD, _LOWER, _RAISE, _ADJUST};

enum custom_keycodes {
    KC_D_MUTE = SAFE_RANGE,
    KC_MODESC,
    INTERNAL_LGUI_GESC,
    KC_ENCODER_RIGHT,
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

tap_dance_action_t tap_dance_actions[] = {
  [TD_NORMAL] = ACTION_TAP_DANCE_FN_ADVANCED(NULL, td_normal_finished, td_normal_reset)
};
void keyboard_post_init_user(void) {
  set_tri_layer_layers(_LOWER, _RAISE, _ADJUST);
#ifdef CONSOLE_ENABLE
  debug_enable = true;
  debug_matrix = true;
#endif
}

layer_state_t layer_state_set_user(layer_state_t state) {
  return update_tri_layer_state(state, _LOWER, _RAISE, _ADJUST);
}

// KEYMAPS

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    [_QWERTY] = LAYOUT(
  KC_LCTL,  KC_1,   KC_2,    KC_3,    KC_4,    KC_5,                     KC_6,    KC_7,    KC_8,    KC_9,    KC_0,    KC_BSPC,
  CU_TAB,   KC_Q,   KC_W,    KC_E,    KC_R,    KC_T,                     KC_Y,    KC_U,    KC_I,    KC_O,    KC_P,    KC_BSLS,
  LGUI_GESC,CU_A,   CU_S,    CU_D,    CU_F,    KC_G,                     KC_H,    CU_J,    CU_K,    CU_L,    CU_SCLN, KC_QUOT,
  SC_LSPO,  KC_Z,   KC_X,    KC_C,    KC_V,    KC_B, TD_NORMAL,  KC_MPLY,  KC_N,    KC_M,    KC_COMM, KC_DOT,  KC_SLSH, SC_RSPC,
                       KC_BSPC, KC_LALT, MO(_NUMPAD), TL_LOWR, CU_SPC, LCTL_T(KC_ENT), TL_UPPR, TT(_NORMAL), KC_RCTL, TD_NORMAL
        ),

    [_NORMAL] = LAYOUT(
  KC_LCTL,  KC_1,   KC_2,    KC_3,    KC_4,    KC_5,                     KC_6,    KC_7,    KC_8,    KC_9,    KC_0,    KC_BSPC,
  KC_TAB,   KC_Q,   KC_W,    KC_E,    KC_R,    KC_T,                     KC_Y,    KC_U,    KC_I,    KC_O,    KC_P,    KC_BSLS,
  LGUI_GESC,KC_A,   KC_S,    KC_D,    KC_F,    KC_G,                     KC_H,    KC_J,    KC_K,    KC_L,    KC_SCLN, KC_QUOT,
  KC_LSFT,  KC_Z,   KC_X,    KC_C,    KC_V,    KC_B, _______,  KC_MPLY,  KC_N,    KC_M,    KC_COMM, KC_DOT,  KC_SLSH, SC_SENT,
                       KC_BSPC, KC_LALT, KC_LGUI, TT(_LOWER), KC_SPC, LCTL_T(KC_ENT), TT(_RAISE), TO(_QWERTY), KC_RCTL, KC_DEL
        ),

    [_GAME] = LAYOUT(
  KC_ESC,   KC_1,   KC_2,    KC_3,    KC_4,    KC_5,                     KC_6,    KC_7,    KC_8,    KC_9,    KC_0,    _______,
  KC_3,   KC_TAB,   KC_Q,    KC_W,    KC_E,    KC_R,                     _______, _______, KC_UP,   _______, _______, _______,
  KC_LSFT,  KC_2,   KC_A,    KC_S,    KC_D,    KC_F,                     _______, KC_LEFT, KC_DOWN, KC_RIGHT,_______, _______,
  KC_LCTL,  KC_1,   KC_Z,    KC_X,    KC_C,    KC_V, KC_G,  _______,  _______,    _______, _______, _______, _______, _______,
                     KC_BSPC, KC_LALT, KC_LGUI, KC_B, KC_SPC, KC_ENT, MO(_RAISE), TO(_QWERTY), KC_RCTL, KC_DEL
        ),

    [_NUMPAD] = LAYOUT(
  _______, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, _______, _______, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,
  XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, KC_CIRC, KC_7, KC_8, KC_9, KC_ASTR, XXXXXXX,
  XXXXXXX, KC_LGUI, KC_LALT, KC_LSFT, KC_LCTL, XXXXXXX, KC_MINS, KC_4, KC_5, KC_6, KC_EQL, KC_PIPE,
  XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, _______, _______, KC_PLUS, KC_1, KC_2, KC_3, KC_SLSH, _______,
                       _______, OSM(MOD_MEH), _______, _______, _______, _______, _______, KC_0, KC_PDOT, _______
        ),

    [_LOWER] = LAYOUT(
  _______, _______, _______, _______, _______, _______,                   _______, _______, _______,_______, KC_MINS, KC_EQL,
  KC_F1,   KC_F2,   KC_F3,   KC_F4,   KC_F5,   KC_F6,                     KC_F7,   KC_F8,   KC_F9,   KC_F10,  KC_F11,  KC_F12,
  QK_BOOT, _______, _______, _______, _______, KC_LBRC,                   KC_RBRC, _______, _______, _______, _______, _______,
  _______, _______, _______, _______, KC_MODESC, _______, _______, _______, _______, _______, _______, _______, _______, _______,
                    _______, _______, _______, _______, _______, _______, _______, TO(_QWERTY), _______, _______
        ),

    [_RAISE] = LAYOUT(
  KC_HOME, KC_F13, KC_F14, KC_F15, KC_F16, KC_F17,                     KC_F18, KC_F19, KC_F20, KC_F21, KC_VOLD, KC_VOLU,
  _______, _______, _______, _______, _______, _______,                     KC_PGUP, KC_PGDN, _______, _______, KC_MPLY, _______,
  _______, _______, KC_PSCR, KC_DEL,  _______, TO(_GAME),                     KC_LEFT, KC_DOWN, KC_UP,   KC_RIGHT,_______, XXXXXXX,
  _______, _______, _______, _______, _______, _______,  KC_MPRV, KC_MNXT,  KC_PLUS, KC_MINS, KC_END,  KC_LBRC, KC_RBRC, KC_DEL,
                    _______, _______, _______, _______,  _______, _______,  _______, TO(_QWERTY), _______, _______
),

    /* [_LOWER] = LAYOUT( */
    /*     //,------------------------------------------------.                    ,---------------------------------------------------. */
    /*     _______, KC_F1, KC_F2, KC_F3, KC_F4, KC_F5, KC_F6, KC_F7, KC_F8, KC_F9, KC_F10, KC_F11, */
    /*     //|------+-------+--------+--------+--------+------|                   |--------+-------+--------+--------+--------+---------| */
    /*     KC_GRV, KC_1, KC_2, KC_3, KC_4, KC_5, KC_6, KC_7, KC_8, KC_9, KC_0, KC_BSPC, */
    /*     //|------+-------+--------+--------+--------+------|                   |--------+-------+--------+--------+--------+---------| */
    /*     QK_BOOT, KC_NO, KC_NO, KC_NO, KC_WH_U, KC_PGUP, KC_LEFT, KC_DOWN, KC_UP, KC_RGHT, KC_NO, KC_DEL, */
    /*     //|------+-------+--------+--------+--------+------|  ===  |   |  ===  |--------+-------+--------+--------+--------+---------| */
    /*     _______, KC_NO, KC_NO, KC_NO, KC_WH_D, KC_PGDN, _______, _______, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, _______, */
    /*     //|------+-------+--------+--------+--------+------|  ===  |   |  ===  |--------+-------+--------+--------+--------+---------| */
    /*     _______, _______, _______, _______, _______, _______, _______, _______, _______, _______ */
    /*     //            \--------+--------+--------+---------+-------|   |--------+---------+--------+---------+-------/ */
    /*     ), */

    /* [_RAISE] = LAYOUT( */
    /*     //,------------------------------------------------.                    ,---------------------------------------------------. */
    /*     _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, */
    /*     //|------+-------+--------+--------+--------+------|                   |--------+-------+--------+--------+--------+---------| */
    /*     _______, KC_INS, KC_PSCR, KC_APP, XXXXXXX, XXXXXXX, KC_CIRC, KC_AMPR, KC_ASTR, KC_LPRN, KC_RPRN, KC_BSPC, */
    /*     //|------+-------+--------+--------+--------+------|                   |--------+-------+--------+--------+--------+---------| */
    /*     _______, KC_LALT, KC_LCTL, KC_LSFT, XXXXXXX, KC_CAPS, KC_MINS, KC_EQL, KC_LCBR, KC_RCBR, KC_PIPE, KC_GRV, */
    /*     //|------+-------+--------+--------+--------+------|  ===  |   |  ===  |--------+-------+--------+--------+--------+---------| */
    /*     _______, KC_UNDO, KC_CUT, KC_COPY, KC_PASTE, XXXXXXX, _______, _______, KC_UNDS, KC_PLUS, KC_LBRC, KC_RBRC, KC_BSLS, KC_TILD, */
    /*     //|------+-------+--------+--------+--------+------|  ===  |   |  ===  |--------+-------+--------+--------+--------+---------| */
    /*     _______, _______, _______, _______, _______, _______, _______, _______, _______, _______ */
    /*     //            \--------+--------+--------+---------+-------|   |--------+---------+--------+---------+-------/ */
    /*     ), */
    /* ADJUST
     * ,-----------------------------------------.                    ,-----------------------------------------.
     * |      |      |      |      |      |      |                    |      |      |      |      |      |      |
     * |------+------+------+------+------+------|                    |------+------+------+------+------+------|
     * | QK_BOOT|      |      |      |      |      |                    |      |      |      |      |      |      |
     * |------+------+------+------+------+------|                    |------+------+------+------+------+------|
     * |RGB_TOG|hue^ |sat ^ | bri ^|      |      |-------.   ,-------|desk <|      |      |desk >|      |      |
     * |------+------+------+------+------+------|  MUTE |    |       |------+------+------+------+------+------|
     * | mode | hue dn|sat d|bri dn|      |QWERTY|-------|    |-------|      | PREV | PLAY | NEXT |      |      |
     * `-----------------------------------------/       /     \      \-----------------------------------------'
     *            | LGUI | LAlt | LCTR |LOWER | /Enter  /       \Space \  |RAISE | RCTR | RAlt | RGUI |
     *            |      |      |      |      |/       /         \      \ |      |      |      |      |
     *            `----------------------------------'           '------''---------------------------'
     */
    [_ADJUST] = LAYOUT(
        //,------------------------------------------------.                    ,---------------------------------------------------.
        EE_CLR, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,
        //|------+-------+--------+--------+--------+------|                   |--------+-------+--------+--------+--------+---------|
        QK_BOOT, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,
        //|------+-------+--------+--------+--------+------|                   |--------+-------+--------+--------+--------+---------|
        RGB_TOG, RGB_HUI, RGB_SAI, RGB_VAI, XXXXXXX, XXXXXXX, C(G(KC_LEFT)), KC_NO, KC_NO, C(G(KC_RGHT)), XXXXXXX, XXXXXXX,
        //|------+-------+--------+--------+--------+------|  ===  |   |  ===  |--------+-------+--------+--------+--------+---------|
        RGB_MOD, RGB_HUD, RGB_SAD, RGB_VAD, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, KC_MPRV, KC_MPLY, KC_MNXT, XXXXXXX, XXXXXXX,
        //|------+-------+--------+--------+--------+------|  ===  |   |  ===  |--------+-------+--------+--------+--------+---------|
        _______, _______, _______, _______, _______, _______, _______, _______, _______, _______
        //            \--------+--------+--------+---------+-------|   |--------+---------+--------+---------+-------/
        ),
    /* NUMPAD
     * ,-----------------------------------------.                    ,-----------------------------------------.
     * | trans|      |      |      |      |      |                    |      |NumLck|      |      |      |      |
     * |------+------+------+------+------+------|                    |------+------+------+------+------+------|
     * |  `   |      |      |      |      |      |                    |   ^  |   7  |   8  |   9  |   *  |      |
     * |------+------+------+------+------+------|                    |------+------+------+------+------+------|
     * | trans|      |      |      |      |      |-------.    ,-------|   -  |   4  |   5  |   6  |      |   |  |
     * |------+------+------+------+------+------|  MUTE |    |       |------+------+------+------+------+------|
     * | trans|      |      |      |      |      |-------|    |-------|   +  |   1  |   2  |   3  |   \  | Shift|
     * `-----------------------------------------/       /     \      \-----------------------------------------'
     *            | Bspc | WIN  |LOWER | Enter| /Space  /       \Enter \  |SPACE | 0    |  .   | RAlt |
     *            |      |      |      |      |/       /         \      \ |      |      |      |      |
     *            `----------------------------------'           '------''---------------------------'
     */
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

  oled_write_ln_P(PSTR("Sofle\n"), false);

  oled_write_P(PSTR("C:"), false);
  if (is_caps_word_on()) {
    oled_write_P(PSTR("on \n"), false);
  } else {
    oled_write_P(PSTR("off\n"), false);
  }

  // Print current layer
  oled_write_ln_P(PSTR("LAYER"), false);
  switch (get_highest_layer(layer_state)) {
  case _QWERTY:
    oled_write_P(PSTR("Base\n\n"), false);
    break;
  case _NORMAL:
    oled_write_P(PSTR("Skill\nIssue"), false);
    break;
  case _RAISE:
    oled_write_P(PSTR("Raise\n\n"), false);
    break;
  case _LOWER:
    oled_write_P(PSTR("Lower\n\n"), false);
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
  oled_write_P(PSTR("BOOT"), false);
  oled_render_dirty(true);
}

bool shutdown_user(bool jump_to_bootloader) {
  oled_render_boot(jump_to_bootloader);
  return true;
}

#endif

// TAPDANCE
td_state_t cur_dance(tap_dance_state_t *state);

static td_tap_t td_normal_state = {
    .is_press_action = true,
    .state           = TD_NONE
};

void td_normal_finished(tap_dance_state_t *state, void *user_data) {
  td_normal_state.state = cur_dance(state);
  switch (td_normal_state.state) {
  case TD_SINGLE_TAP:
    tap_code16(OSL(_NORMAL));
    break;
  case TD_SINGLE_HOLD:
    register_code16(MO(_NORMAL));
    break;
  case TD_DOUBLE_TAP:
    tap_code16(TG(_NORMAL));
    break;
  default: break;
  }
}

void td_normal_reset(tap_dance_state_t *state, void *user_data) {
  switch (td_normal_state.state) {
  case TD_SINGLE_HOLD:
    unregister_code16(MO(_NORMAL));
    break;
  default: break;
  }
}

// KEYCODES

static uint8_t custom_key_data = 0;
#define MODESC (1 << 0)

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
  int8_t tap     = record->tap.count;
  int8_t press   = record->event.pressed;

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

  case TO(_QWERTY):
  case TD_NORMAL:
  case TT(_NORMAL):
    oled_needs_redraw = true;
    break;

  case KC_D_MUTE:
    if (record->event.pressed) {
      register_mods(mod_config(MOD_MEH));
      register_code(KC_UP);
    } else {
      unregister_mods(mod_config(MOD_MEH));
      unregister_code(KC_UP);
    }
  }
  return true;
}

#ifdef ENCODER_ENABLE

bool encoder_update_user(uint8_t index, bool clockwise) {
    if (index == 0) {
        if (clockwise) {
            tap_code(KC_VOLU);
        } else {
            tap_code(KC_VOLD);
        }
    } else if (index == 1) {
        switch (get_highest_layer(layer_state)) {
            case _QWERTY:
                if (clockwise) {
                    tap_code(KC_PGDN);
                } else {
                    tap_code(KC_PGUP);
                }
                break;
            case _RAISE:
            case _LOWER:
                if (clockwise) {
                    tap_code(KC_MPRV);
                } else {
                    tap_code(KC_MNXT);
                }
                break;
            default:
                if (clockwise) {
                    tap_code(KC_WH_D);
                } else {
                    tap_code(KC_WH_U);
                }
                break;
        }
    }
    return true;
}

#endif
