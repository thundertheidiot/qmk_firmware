// Copyright 2026 thundertheidiot
// SPDX-License-Identifier: GPL-2.0-or-later

#include QMK_KEYBOARD_H
#include "finnshift.h"

ASSERT_COMMUNITY_MODULES_MIN_API_VERSION(1, 0, 0);

// What a translated key emits: `code` is registered while the physical key is
// held, with `mods` applied. Only shift (MOD_LSFT) and AltGr (MOD_RALT) are
// ever needed to produce a given glyph on the Finnish layout. Dead keys
// (`^ ~) can't be held meaningfully; they are emitted as an immediate double
// tap, which produces the literal glyph.
typedef struct {
    uint8_t  mods;
    uint16_t code;
    bool     dead;
} emission_t;

#define EMIT(m, c)      ((emission_t){(m), (c), false})
#define EMIT_DEAD(m, c) ((emission_t){(m), (c), true})

typedef enum {
    TR_NONE, // key untouched by finnshift
    TR_PASS, // glyph identical on both layouts; let QMK process the key
    TR_EMIT, // needs translation; `out` holds the emission
} tr_result_t;

static bool finnshift_on = false;

typedef struct {
    bool     active;
    uint16_t trigger;     // physical keycode whose release ends the hold
    uint16_t code;        // emitted keycode (0 for dead-key swallows)
    uint8_t  added_mods;  // mods we registered for the emission
    uint8_t  saved_mods;  // real mods suppressed for the duration
    uint8_t  saved_weak;  // weak mods (caps word, mod-tap taps) suppressed
} pending_t;

static pending_t pending;

// Mods captured while a translated key is being emitted.
typedef struct {
    uint8_t cur;
    uint8_t cur_weak;
    uint8_t drop;
} mod_guard_t;

bool finnshift_enabled(void) {
    return finnshift_on;
}

static void finish_pending(void) {
    if (!pending.active) {
        return;
    }
    if (pending.code) {
        unregister_code(pending.code);
    }
    if (pending.added_mods) {
        unregister_mods(pending.added_mods);
    }
    register_mods(pending.saved_mods);
    add_weak_mods(pending.saved_weak);
    pending = (pending_t){0};
}

// A plain shift key was released while a translation is held; don't resurrect
// it on release, or it would come back as a ghost modifier.
static void forget_shift(void) {
    pending.saved_mods &= ~(MOD_BIT(KC_LSFT) | MOD_BIT(KC_RSFT));
    pending.saved_weak &= ~(MOD_BIT(KC_LSFT) | MOD_BIT(KC_RSFT));
}

// Suppress only the mods that conflict with the emission: shift when its state
// doesn't match, ctrl+alt when AltGr semantics are in play, and oneshot mods
// (they apply once, which is this key). GUI and other mods are kept.
static void suppress_conflicts(emission_t em, bool altgr, mod_guard_t *g) {
    g->cur      = get_mods();
    g->cur_weak = get_weak_mods();
    uint8_t oneshot    = get_oneshot_mods();
    uint8_t union_mods = g->cur | g->cur_weak | oneshot;

    g->drop = oneshot;
    if (!(em.mods & MOD_LSFT)) {
        g->drop |= union_mods & MOD_MASK_SHIFT;
    }
    if (altgr || (em.mods & MOD_RALT)) {
        g->drop |= union_mods & (MOD_MASK_CTRL | MOD_MASK_ALT);
    }

    unregister_mods(g->cur & g->drop);
    del_weak_mods(g->cur_weak & g->drop);
    set_oneshot_mods(0);
}

static void restore_mods(mod_guard_t g) {
    register_mods(g.cur & g.drop);
    add_weak_mods(g.cur_weak & g.drop);
}

static uint8_t emission_added(emission_t em, mod_guard_t g) {
    // Shift the user already holds doesn't need to be (re-)added.
    return em.mods & ~((g.cur | g.cur_weak) & MOD_MASK_SHIFT);
}

static void start_pending(uint16_t trigger, emission_t em, bool altgr) {
    finish_pending();

    mod_guard_t g;
    suppress_conflicts(em, altgr, &g);

    pending.active     = true;
    pending.trigger    = trigger;
    pending.code       = em.code;
    pending.added_mods = emission_added(em, g);
    pending.saved_mods = g.cur & g.drop;
    pending.saved_weak = g.cur_weak & g.drop;

    register_mods(pending.added_mods);
    register_code(em.code);
}

// Emit a key as an instantaneous tap (used for dead keys and for tapped
// mod-taps/layer-taps, whose physical key is already up again).
static void emit_tap(emission_t em, bool altgr) {
    mod_guard_t g;
    suppress_conflicts(em, altgr, &g);
    uint8_t added = emission_added(em, g);
    uint8_t taps  = em.dead ? 2 : 1;
    for (uint8_t i = 0; i < taps; i++) {
        register_mods(added);
        register_code(em.code);
        unregister_code(em.code);
        unregister_mods(added);
    }
    restore_mods(g);
}

// Basic punctuation keys, keyed by US position. `sh` is the shift state at
// press time. Returns TR_PASS for glyphs where US and Finnish agree untouched.
static tr_result_t translate_punct(uint16_t keycode, uint8_t mods, emission_t *out) {
    bool sh = mods & MOD_MASK_SHIFT;
    switch (keycode) {
        case KC_GRV:
            *out = sh ? EMIT_DEAD(MOD_RALT, KC_RBRC) : EMIT_DEAD(MOD_LSFT, KC_EQL);
            return TR_EMIT; // ` ~ (dead keys)
        case KC_2:
            if (!sh) return TR_PASS;
            *out = EMIT(MOD_RALT, KC_2);
            return TR_EMIT; // @
        case KC_4:
            if (!sh) return TR_PASS;
            *out = EMIT(MOD_RALT, KC_4);
            return TR_EMIT; // $
        case KC_6:
            if (!sh) return TR_PASS;
            *out = EMIT_DEAD(MOD_LSFT, KC_RBRC);
            return TR_EMIT; // ^ (dead key)
        case KC_7:
            if (!sh) return TR_PASS;
            *out = EMIT(MOD_LSFT, KC_6);
            return TR_EMIT; // &
        case KC_8:
            if (!sh) return TR_PASS;
            *out = EMIT(MOD_LSFT, KC_NUHS);
            return TR_EMIT; // *
        case KC_9:
            if (!sh) return TR_PASS;
            *out = EMIT(MOD_LSFT, KC_8);
            return TR_EMIT; // (
        case KC_0:
            if (!sh) return TR_PASS;
            *out = EMIT(MOD_LSFT, KC_9);
            return TR_EMIT; // )
        case KC_MINS:
            *out = sh ? EMIT(MOD_LSFT, KC_SLSH) : EMIT(0, KC_SLSH);
            return TR_EMIT; // - _
        case KC_EQL:
            *out = sh ? EMIT(0, KC_MINS) : EMIT(MOD_LSFT, KC_0);
            return TR_EMIT; // = +
        case KC_LBRC:
            *out = sh ? EMIT(MOD_RALT, KC_7) : EMIT(MOD_RALT, KC_8);
            return TR_EMIT; // [ {
        case KC_RBRC:
            *out = sh ? EMIT(MOD_RALT, KC_0) : EMIT(MOD_RALT, KC_9);
            return TR_EMIT; // ] }
        case KC_BSLS:
            *out = sh ? EMIT(MOD_RALT, KC_NUBS) : EMIT(MOD_RALT, KC_MINS);
            return TR_EMIT; // \ |
        case KC_SCLN:
            *out = sh ? EMIT(MOD_LSFT, KC_DOT) : EMIT(MOD_LSFT, KC_COMM);
            return TR_EMIT; // ; :
        case KC_QUOT:
            *out = sh ? EMIT(MOD_LSFT, KC_2) : EMIT(0, KC_NUHS);
            return TR_EMIT; // ' "
        case KC_COMM:
            if (!sh) return TR_PASS;
            *out = EMIT(0, KC_NUBS);
            return TR_EMIT; // <
        case KC_DOT:
            if (!sh) return TR_PASS;
            *out = EMIT(MOD_LSFT, KC_NUBS);
            return TR_EMIT; // >
        case KC_SLSH:
            *out = sh ? EMIT(MOD_LSFT, KC_MINS) : EMIT(MOD_LSFT, KC_7);
            return TR_EMIT; // / ?
    }
    return TR_NONE;
}

// Direct symbol keycodes (e.g. from symbol layers): always translated, shift
// comes with the emission.
static tr_result_t translate_direct(uint16_t keycode, emission_t *out) {
    switch (keycode) {
        case KC_EXLM: *out = EMIT(MOD_LSFT, KC_1); return TR_EMIT;       // !
        case KC_AT:   *out = EMIT(MOD_RALT, KC_2); return TR_EMIT;       // @
        case KC_HASH: *out = EMIT(MOD_LSFT, KC_3); return TR_EMIT;       // #
        case KC_DLR:  *out = EMIT(MOD_RALT, KC_4); return TR_EMIT;       // $
        case KC_PERC: *out = EMIT(MOD_LSFT, KC_5); return TR_EMIT;       // %
        case KC_CIRC: *out = EMIT_DEAD(MOD_LSFT, KC_RBRC); return TR_EMIT; // ^ (dead)
        case KC_AMPR: *out = EMIT(MOD_LSFT, KC_6); return TR_EMIT;       // &
        case KC_ASTR: *out = EMIT(MOD_LSFT, KC_NUHS); return TR_EMIT;    // *
        case KC_LPRN: *out = EMIT(MOD_LSFT, KC_8); return TR_EMIT;       // (
        case KC_RPRN: *out = EMIT(MOD_LSFT, KC_9); return TR_EMIT;       // )
        case KC_UNDS: *out = EMIT(MOD_LSFT, KC_SLSH); return TR_EMIT;    // _
        case KC_PLUS: *out = EMIT(0, KC_MINS); return TR_EMIT;           // +
        case KC_LCBR: *out = EMIT(MOD_RALT, KC_7); return TR_EMIT;       // {
        case KC_RCBR: *out = EMIT(MOD_RALT, KC_0); return TR_EMIT;       // }
        case KC_PIPE: *out = EMIT(MOD_RALT, KC_NUBS); return TR_EMIT;    // |
        case KC_TILD: *out = EMIT_DEAD(MOD_RALT, KC_RBRC); return TR_EMIT; // ~ (dead)
        case KC_COLN: *out = EMIT(MOD_LSFT, KC_DOT); return TR_EMIT;     // :
        case KC_DQUO: *out = EMIT(MOD_LSFT, KC_2); return TR_EMIT;       // "
        case KC_QUES: *out = EMIT(MOD_LSFT, KC_MINS); return TR_EMIT;    // ?
        case KC_LABK: *out = EMIT(0, KC_NUBS); return TR_EMIT;           // <
        case KC_RABK: *out = EMIT(MOD_LSFT, KC_NUBS); return TR_EMIT;    // >
    }
    return TR_NONE;
}

// Ctrl+Alt acts as AltGr: å ä ö € with (optionally shifted) w a o e.
static tr_result_t translate_altgr_letter(uint16_t keycode, uint8_t mods, emission_t *out) {
    bool sh = mods & MOD_MASK_SHIFT;
    switch (keycode) {
        case KC_W: *out = EMIT(sh ? MOD_LSFT : 0, KC_LBRC); return TR_EMIT; // å Å
        case KC_A: *out = EMIT(sh ? MOD_LSFT : 0, KC_QUOT); return TR_EMIT; // ä Ä
        case KC_O: *out = EMIT(sh ? MOD_LSFT : 0, KC_SCLN); return TR_EMIT; // ö Ö
        case KC_E: *out = EMIT(MOD_RALT, KC_5); return TR_EMIT;             // €
    }
    return TR_NONE;
}

static tr_result_t translate(uint16_t keycode, uint8_t mods, bool altgr, emission_t *out) {
    tr_result_t r = altgr ? translate_altgr_letter(keycode, mods, out) : TR_NONE;
    if (r == TR_NONE) {
        r = translate_punct(keycode, mods, out);
    }
    if (r == TR_NONE) {
        r = translate_direct(keycode, out);
    }
    return r;
}

bool process_record_finnshift(uint16_t keycode, keyrecord_t *record) {
    switch (keycode) {
        case FI_SHIFT_TOGGLE:
            if (record->event.pressed) {
                finish_pending();
                finnshift_on = !finnshift_on;
            }
            return false;
    }

    if (!finnshift_on) {
        return true;
    }

    if (pending.active && !record->event.pressed && (keycode == KC_LSFT || keycode == KC_RSFT)) {
        forget_shift();
    }

    uint8_t mods  = get_mods() | get_weak_mods() | get_oneshot_mods();
    bool    altgr = (mods & MOD_MASK_CTRL) && (mods & MOD_MASK_ALT);

    // Tapped mod-taps/layer-taps never reach the hooks with their inner
    // keycode: action.c registers it directly after the hooks. Intercept the
    // synthetic release and translate the inner key ourselves, as a tap.
    if (!record->event.pressed && record->tap.count > 0) {
        uint16_t inner = KC_NO;
        if (IS_QK_MOD_TAP(keycode)) {
            inner = QK_MOD_TAP_GET_TAP_KEYCODE(keycode);
        } else if (IS_QK_LAYER_TAP(keycode)) {
            inner = QK_LAYER_TAP_GET_TAP_KEYCODE(keycode);
        }
        if (inner != KC_NO && !altgr && (mods & (MOD_MASK_CTRL | MOD_MASK_ALT | MOD_MASK_GUI))) {
            return true; // shortcut: leave the tap untranslated
        }
        if (inner != KC_NO) {
            emission_t em;
            if (translate(inner, mods, altgr, &em) == TR_EMIT) {
                emit_tap(em, altgr);
                return false; // keep QMK from also sending the raw inner key
            }
        }
        return true;
    }

    if (!record->event.pressed) {
        if (pending.active && keycode == pending.trigger) {
            finish_pending();
            return false;
        }
        return true;
    }

    // Leave shortcuts untranslated: with ctrl/alt/gui held, the OS-side
    // binding is what matters, and it matches the untranslated key exactly
    // like finnshift-off. Typing (no mods or shift only) is translated.
    if (!altgr && (mods & (MOD_MASK_CTRL | MOD_MASK_ALT | MOD_MASK_GUI))) {
        return true;
    }

    emission_t em;
    if (translate(keycode, mods, altgr, &em) != TR_EMIT) {
        return true;
    }

    if (em.dead) {
        emit_tap(em, altgr);
        pending.active  = true; // nothing held; just swallow the release
        pending.trigger = keycode;
    } else {
        start_pending(keycode, em, altgr);
    }
    return false;
}

void finnshift_send_symbol(uint16_t us_keycode) {
    emission_t em;
    uint8_t    mods = get_mods() | get_weak_mods() | get_oneshot_mods();
    if (finnshift_on && translate(us_keycode, mods, false, &em) == TR_EMIT) {
        emit_tap(em, false);
    } else {
        tap_code16(us_keycode);
    }
}

void suspend_power_down_finnshift(void) {
    finish_pending();
}
