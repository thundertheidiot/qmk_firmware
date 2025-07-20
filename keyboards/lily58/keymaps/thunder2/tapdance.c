#include QMK_KEYBOARD_H
#include "tapdance.h"

td_state_t cur_dance(tap_dance_state_t *state) {
  if (state->count == 1) {
    if (state->interrupted || !state->pressed) {
      return TD_SINGLE_TAP;
    } else {
      return TD_SINGLE_HOLD;
    }
  } else if (state->count == 2) {
    if (state->interrupted) {
      return TD_DOUBLE_SINGLE_TAP;
    } else if (state->pressed) {
      return TD_DOUBLE_HOLD;
    } else {
      return TD_DOUBLE_TAP;
    }
  }

  return TD_UNKNOWN;
}
