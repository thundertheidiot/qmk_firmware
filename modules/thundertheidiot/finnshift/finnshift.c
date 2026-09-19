// Copyright 2026 thundertheidiot
// SPDX-License-Identifier: GPL-2.0-or-later

#include QMK_KEYBOARD_H
#include <stdint.h>
#include "finnshift.h"

ASSERT_COMMUNITY_MODULES_MIN_API_VERSION(1, 0, 0);

// What a translated key emits: `code` is registered while the physical key is
// held, with `mods` applied. Only shift (MOD_BIT_LSHIFT) and AltGr
// (MOD_BIT_RALT) are ever needed to produce a given glyph on the Finnish
// layout -- these are the 8-bit HID values, NOT the 5-bit MOD_* tap codes.
// Dead keys (` ^ ~) can't be held meaningfully; they are emitted as an
// immediate double tap, which produces the literal glyph.
typedef struct {
    uint8_t  mods;
    uint16_t code;
    bool     dead;
} emission_t;

#define EMIT(m, c) ((emission_t){(m), (c), false})
#define EMIT_DEAD(m, c) ((emission_t){(m), (c), true})

typedef enum {
    TR_NONE, // key untouched by finnshift
    TR_PASS, // glyph identical on both layouts; let QMK process the key
    TR_EMIT, // needs translation; `out` holds the emission
} tr_result_t;

static bool finnshift_on = false;

// Modifier context of a translation: what we registered (added_mods), what we
// suppressed (saved_*), and what the mod situation was at emission time.
typedef struct {
    uint8_t added_mods;
    uint8_t saved_mods;
    uint8_t saved_weak;
    uint8_t em_mods;    // emission mod requirements
    uint8_t union_mods; // full mod union at emission time
} ctx_t;

// The currently held translated key.
static struct {
    bool     active;
    uint16_t trigger; // physical keycode whose release ends the hold
    uint16_t code;    // emitted keycode (0 = nothing held, swallow only)
    ctx_t    ctx;
} held;

// After a translated key finishes, its modifier context stays latched (AltGr
// held, conflicting mods suppressed) so consecutive translations like arr[]
// don't bounce AltGr up and down. It is torn down as soon as anything that
// isn't another matching translation happens.
static struct {
    bool  active;
    ctx_t ctx;
} settled;

bool finnshift_enabled(void) {
    return finnshift_on;
}

static void held_release(void) {
    if (!held.active) {
        return;
    }
    if (held.code) {
        unregister_code(held.code);
    }
    held.active = false;
}

// Finish the held key but keep its modifier context latched.
static void held_settle(void) {
    if (!held.active) {
        return;
    }
    if (held.code) {
        unregister_code(held.code);
    }
    settled.active = true;
    settled.ctx    = held.ctx;
    held.active    = false;
}

static void settled_teardown(void) {
    if (!settled.active) {
        return;
    }
    settled.active = false;
    if (settled.ctx.added_mods) {
        unregister_mods(settled.ctx.added_mods);
    }
    register_mods(settled.ctx.saved_mods);
    add_weak_mods(settled.ctx.saved_weak);
}

static void finish_all(void) {
    held_release();
    settled_teardown();
}

// A plain shift key was released while a translation is held; don't resurrect
// it on release, or it would come back as a ghost modifier.
static void forget_shift(void) {
    held.ctx.saved_mods &= ~(MOD_BIT_LSHIFT | MOD_BIT_RSHIFT);
    held.ctx.saved_weak &= ~(MOD_BIT_LSHIFT | MOD_BIT_RSHIFT);
}

// Suppress only the mods that conflict with the emission: shift when its state
// doesn't match, ctrl+alt when AltGr semantics are in play, and oneshot mods
// (they apply once, which is this key). GUI and other mods are kept.
static void ctx_start(ctx_t *c, emission_t em, bool altgr) {
    c->union_mods = get_mods() | get_weak_mods() | get_oneshot_mods();
    uint8_t cur   = get_mods();
    uint8_t weak  = get_weak_mods();
    uint8_t drop  = get_oneshot_mods();

    if (!(em.mods & MOD_BIT_LSHIFT)) {
        drop |= c->union_mods & MOD_MASK_SHIFT;
    }
    if (altgr || (em.mods & MOD_BIT_RALT)) {
        drop |= c->union_mods & (MOD_MASK_CTRL | MOD_MASK_ALT);
    }

    c->em_mods    = em.mods;
    c->added_mods = em.mods & ~((cur | weak) & MOD_MASK_SHIFT);
    c->saved_mods = cur & drop;
    c->saved_weak = weak & drop;

    unregister_mods(cur & drop);
    del_weak_mods(weak & drop);
    set_oneshot_mods(0);
}

static void ctx_restore(const ctx_t *c) {
    register_mods(c->saved_mods);
    add_weak_mods(c->saved_weak);
}

static bool settled_matches(emission_t em, uint8_t union_mods) {
    if (!settled.active || settled.ctx.em_mods != em.mods) {
        return false;
    }
    // The context suppresses some of the user's physically-held mods (they
    // disappear from get_mods()), so the reported union legitimately differs
    // from the one recorded at emission time. Instead of comparing them, only
    // require that no UNACCOUNTED-FOR modifier appeared: everything in the
    // current union must be explained by the emission's own mods, the mods
    // recorded at emission time, or the mods the context itself latched.
    uint8_t known = settled.ctx.union_mods | settled.ctx.added_mods;
    return (union_mods & ~known) == 0;
}

static void start_pending(uint16_t trigger, emission_t em, bool altgr, uint8_t union_mods) {
    held_settle();

    if (settled_matches(em, union_mods)) {
        // Modifier context is already in place: just press the key. This keeps
        // AltGr down between consecutive translations (no up/down bouncing).
        held.active    = true;
        held.trigger   = trigger;
        held.code      = em.code;
        held.ctx       = settled.ctx;
        settled.active = false;
        register_code(em.code);
        return;
    }

    settled_teardown();

    ctx_t c;
    ctx_start(&c, em, altgr);
    register_mods(c.added_mods);
    register_code(em.code);

    held.active  = true;
    held.trigger = trigger;
    held.code    = em.code;
    held.ctx     = c;
}

// Emit a key as an instantaneous tap (used for dead keys and for tapped
// mod-taps/layer-taps, whose physical key is already up again). If the settled
// context already provides the right mods, they are left untouched.
static void emit_tap(emission_t em, bool altgr, uint8_t union_mods) {
    uint8_t taps = em.dead ? 2 : 1;

    if (settled_matches(em, union_mods)) {
        while (taps--) {
            register_code(em.code);
            unregister_code(em.code);
        }
        return;
    }

    settled_teardown();

    ctx_t c;
    ctx_start(&c, em, altgr);
    register_mods(c.added_mods);
    while (taps--) {
        register_code(em.code);
        unregister_code(em.code);
    }
    unregister_mods(c.added_mods);
    ctx_restore(&c);
}

// Basic punctuation keys, keyed by US position. `sh` is the shift state at
// press time. Returns TR_PASS for glyphs where US and Finnish agree untouched.
static tr_result_t translate_punct(uint16_t keycode, uint8_t mods, emission_t *out) {
    bool sh = mods & MOD_MASK_SHIFT;
    switch (keycode) {
        case KC_GRV:
            *out = sh ? EMIT_DEAD(MOD_BIT_RALT, KC_RBRC) : EMIT_DEAD(MOD_BIT_LSHIFT, KC_EQL);
            return TR_EMIT; // ` ~ (dead keys)
        case KC_2:
            if (!sh) return TR_PASS;
            *out = EMIT(MOD_BIT_RALT, KC_2);
            return TR_EMIT; // @
        case KC_4:
            if (!sh) return TR_PASS;
            *out = EMIT(MOD_BIT_RALT, KC_4);
            return TR_EMIT; // $
        case KC_6:
            if (!sh) return TR_PASS;
            *out = EMIT_DEAD(MOD_BIT_LSHIFT, KC_RBRC);
            return TR_EMIT; // ^ (dead key)
        case KC_7:
            if (!sh) return TR_PASS;
            *out = EMIT(MOD_BIT_LSHIFT, KC_6);
            return TR_EMIT; // &
        case KC_8:
            if (!sh) return TR_PASS;
            *out = EMIT(MOD_BIT_LSHIFT, KC_NUHS);
            return TR_EMIT; // *
        case KC_9:
            if (!sh) return TR_PASS;
            *out = EMIT(MOD_BIT_LSHIFT, KC_8);
            return TR_EMIT; // (
        case KC_0:
            if (!sh) return TR_PASS;
            *out = EMIT(MOD_BIT_LSHIFT, KC_9);
            return TR_EMIT; // )
        case KC_MINS:
            *out = sh ? EMIT(MOD_BIT_LSHIFT, KC_SLSH) : EMIT(0, KC_SLSH);
            return TR_EMIT; // - _
        case KC_EQL:
            *out = sh ? EMIT(0, KC_MINS) : EMIT(MOD_BIT_LSHIFT, KC_0);
            return TR_EMIT; // = +
        case KC_LBRC:
            *out = sh ? EMIT(MOD_BIT_RALT, KC_7) : EMIT(MOD_BIT_RALT, KC_8);
            return TR_EMIT; // [ {
        case KC_RBRC:
            *out = sh ? EMIT(MOD_BIT_RALT, KC_0) : EMIT(MOD_BIT_RALT, KC_9);
            return TR_EMIT; // ] }
        case KC_BSLS:
            *out = sh ? EMIT(MOD_BIT_RALT, KC_NUBS) : EMIT(MOD_BIT_RALT, KC_MINS);
            return TR_EMIT; // \ |
        case KC_SCLN:
            *out = sh ? EMIT(MOD_BIT_LSHIFT, KC_DOT) : EMIT(MOD_BIT_LSHIFT, KC_COMM);
            return TR_EMIT; // ; :
        case KC_QUOT:
            *out = sh ? EMIT(MOD_BIT_LSHIFT, KC_2) : EMIT(0, KC_NUHS);
            return TR_EMIT; // ' "
        case KC_COMM:
            if (!sh) return TR_PASS;
            *out = EMIT(0, KC_NUBS);
            return TR_EMIT; // <
        case KC_DOT:
            if (!sh) return TR_PASS;
            *out = EMIT(MOD_BIT_LSHIFT, KC_NUBS);
            return TR_EMIT; // >
        case KC_SLSH:
            *out = sh ? EMIT(MOD_BIT_LSHIFT, KC_MINS) : EMIT(MOD_BIT_LSHIFT, KC_7);
            return TR_EMIT; // / ?
    }
    return TR_NONE;
}

// Direct symbol keycodes (e.g. from symbol layers): always translated, shift
// comes with the emission.
static tr_result_t translate_direct(uint16_t keycode, emission_t *out) {
    switch (keycode) {
        case KC_EXLM:
            *out = EMIT(MOD_BIT_LSHIFT, KC_1);
            return TR_EMIT; // !
        case KC_AT:
            *out = EMIT(MOD_BIT_RALT, KC_2);
            return TR_EMIT; // @
        case KC_HASH:
            *out = EMIT(MOD_BIT_LSHIFT, KC_3);
            return TR_EMIT; // #
        case KC_DLR:
            *out = EMIT(MOD_BIT_RALT, KC_4);
            return TR_EMIT; // $
        case KC_PERC:
            *out = EMIT(MOD_BIT_LSHIFT, KC_5);
            return TR_EMIT; // %
        case KC_CIRC:
            *out = EMIT_DEAD(MOD_BIT_LSHIFT, KC_RBRC);
            return TR_EMIT; // ^ (dead)
        case KC_AMPR:
            *out = EMIT(MOD_BIT_LSHIFT, KC_6);
            return TR_EMIT; // &
        case KC_ASTR:
            *out = EMIT(MOD_BIT_LSHIFT, KC_NUHS);
            return TR_EMIT; // *
        case KC_LPRN:
            *out = EMIT(MOD_BIT_LSHIFT, KC_8);
            return TR_EMIT; // (
        case KC_RPRN:
            *out = EMIT(MOD_BIT_LSHIFT, KC_9);
            return TR_EMIT; // )
        case KC_UNDS:
            *out = EMIT(MOD_BIT_LSHIFT, KC_SLSH);
            return TR_EMIT; // _
        case KC_PLUS:
            *out = EMIT(0, KC_MINS);
            return TR_EMIT; // +
        case KC_LCBR:
            *out = EMIT(MOD_BIT_RALT, KC_7);
            return TR_EMIT; // {
        case KC_RCBR:
            *out = EMIT(MOD_BIT_RALT, KC_0);
            return TR_EMIT; // }
        case KC_PIPE:
            *out = EMIT(MOD_BIT_RALT, KC_NUBS);
            return TR_EMIT; // |
        case KC_TILD:
            *out = EMIT_DEAD(MOD_BIT_RALT, KC_RBRC);
            return TR_EMIT; // ~ (dead)
        case KC_COLN:
            *out = EMIT(MOD_BIT_LSHIFT, KC_DOT);
            return TR_EMIT; // :
        case KC_DQUO:
            *out = EMIT(MOD_BIT_LSHIFT, KC_2);
            return TR_EMIT; // "
        case KC_QUES:
            *out = EMIT(MOD_BIT_LSHIFT, KC_MINS);
            return TR_EMIT; // ?
        case KC_LABK:
            *out = EMIT(0, KC_NUBS);
            return TR_EMIT; // <
        case KC_RABK:
            *out = EMIT(MOD_BIT_LSHIFT, KC_NUBS);
            return TR_EMIT; // >
    }
    return TR_NONE;
}

// Ctrl+Alt acts as AltGr: å ä ö € with (optionally shifted) w a o e.
static tr_result_t translate_altgr_letter(uint16_t keycode, uint8_t mods, emission_t *out) {
    bool sh = mods & MOD_MASK_SHIFT;
    switch (keycode) {
        case KC_W:
            *out = EMIT(sh ? MOD_BIT_LSHIFT : 0, KC_LBRC);
            return TR_EMIT; // å Å
        case KC_A:
            *out = EMIT(sh ? MOD_BIT_LSHIFT : 0, KC_QUOT);
            return TR_EMIT; // ä Ä
        case KC_O:
            *out = EMIT(sh ? MOD_BIT_LSHIFT : 0, KC_SCLN);
            return TR_EMIT; // ö Ö
        case KC_E:
            *out = EMIT(MOD_BIT_RALT, KC_5);
            return TR_EMIT; // €
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
                finish_all();
                finnshift_on = !finnshift_on;
            }
            return false;
    }

    if (!finnshift_on) {
        return true;
    }

    if (held.active && !record->event.pressed && (keycode == KC_LSFT || keycode == KC_RSFT)) {
        forget_shift();
    }

    uint8_t union_mods = get_mods() | get_weak_mods() | get_oneshot_mods();
    // The latched/held translation context holds AltGr by itself; that is not
    // the user's input, so exclude it when deciding whether they are holding
    // shortcut modifiers.
    uint8_t user_mods = union_mods;
    if (held.active) {
        user_mods &= ~held.ctx.added_mods;
    }
    if (settled.active) {
        user_mods &= ~settled.ctx.added_mods;
    }
    // Conversely, mods the context suppresses (e.g. shift while an AltGr
    // context is latched) are still physically held; the user's typing intent
    // includes them. Our own latched additions (the LSHIFT added for '=', the
    // RALT added for '@', ...) do NOT count as user intent.
    uint8_t intent_mods = user_mods;
    if (held.active) {
        intent_mods |= held.ctx.saved_mods;
    }
    if (settled.active) {
        intent_mods |= settled.ctx.saved_mods;
    }
    bool altgr = (intent_mods & MOD_MASK_CTRL) && (intent_mods & MOD_MASK_ALT);

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
        if (inner != KC_NO && !altgr && (user_mods & (MOD_MASK_CTRL | MOD_MASK_ALT | MOD_MASK_GUI))) {
            settled_teardown();
            return true; // shortcut: leave the tap untranslated
        }
        if (inner != KC_NO) {
            emission_t em;
            if (translate(inner, intent_mods, altgr, &em) == TR_EMIT) {
                emit_tap(em, altgr, union_mods);
                return false; // keep QMK from also sending the raw inner key
            }
        }
        settled_teardown();
        return true;
    }

    if (!record->event.pressed) {
        if (held.active && keycode == held.trigger) {
            // Finish the hold, but keep the modifier context latched so the
            // next translation doesn't have to rebuild it.
            if (held.code) {
                unregister_code(held.code);
            }
            settled.active = true;
            settled.ctx    = held.ctx;
            held.active    = false;
            return false;
        }
        settled_teardown();
        return true;
    }

    // Leave shortcuts untranslated: with ctrl/alt/gui held, the OS-side
    // binding is what matters, and it matches the untranslated key exactly
    // like finnshift-off. Typing (no mods or shift only) is translated.
    if (!altgr && (user_mods & (MOD_MASK_CTRL | MOD_MASK_ALT | MOD_MASK_GUI))) {
        settled_teardown();
        return true;
    }

    emission_t em;
    if (translate(keycode, intent_mods, altgr, &em) != TR_EMIT) {
        settled_teardown();
        return true;
    }

    if (em.dead) {
        bool reuse = settled_matches(em, union_mods);
        emit_tap(em, altgr, union_mods);
        held.active  = true; // nothing held; swallow the release
        held.trigger = keycode;
        held.code    = 0;
        held.ctx     = reuse ? settled.ctx : (ctx_t){0};
    } else {
        start_pending(keycode, em, altgr, union_mods);
    }
    return false;
}

void finnshift_send_symbol(uint16_t us_keycode) {
    emission_t em;
    uint8_t    union_mods = get_mods() | get_weak_mods() | get_oneshot_mods();
    if (finnshift_on && translate(us_keycode, union_mods, false, &em) == TR_EMIT) {
        emit_tap(em, false, union_mods);
    } else {
        tap_code16(us_keycode);
    }
}

void suspend_power_down_finnshift(void) {
    finish_all();
}
