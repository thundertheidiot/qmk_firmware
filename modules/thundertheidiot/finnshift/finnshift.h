// Copyright 2026 thundertheidiot
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include <stdbool.h>

// finnshift -- translate a US ANSI keymap into Finnish (Nordic ISO) scancodes.
//
// For hosts whose OS keyboard layout is set to Finnish: when the mode is
// enabled, punctuation is translated so the Finnish layout produces the same
// glyphs a US layout would (e.g. shift+, yields '<' by sending the '<>'| key),
// and Ctrl+Alt (either side) + w/a/o/e types å/ä/ö/€. Unrelated modifiers are
// kept, so shortcuts like super+; still work against the translated key.
// AltGr-based glyphs use the Right Alt modifier, which is the level3/AltGr key
// on Windows, macOS and most Linux setups.
//
// Toggle the mode with the FI_SHIFT_TOGGLE keycode (alias "FI_TGGL"). The mode
// starts disabled on every boot. Translated keys are registered on press and
// unregistered on release, so the host's auto-repeat keeps working while held.

bool finnshift_enabled(void);

// Types the US symbol `us_keycode` (e.g. KC_TILD) as a tap: translated through
// the Finnish layout when the mode is on, raw otherwise. For macros that emit
// symbols directly instead of going through the keymap.
void finnshift_send_symbol(uint16_t us_keycode);
