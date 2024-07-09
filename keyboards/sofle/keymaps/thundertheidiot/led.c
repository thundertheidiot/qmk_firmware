#define INDICATOR_BRIGHTNESS 30

#define HSV_OVERRIDE_HELP(h, s, v, Override) h, s, Override
#define HSV_OVERRIDE(hsv, Override) HSV_OVERRIDE_HELP(hsv, Override)

// Light combinations
#define SET_INDICATORS(hsv)                                     \
    {0, 1, HSV_OVERRIDE_HELP(hsv, INDICATOR_BRIGHTNESS)}, {     \
        36 + 0, 1, HSV_OVERRIDE_HELP(hsv, INDICATOR_BRIGHTNESS) \
    }
#define SET_UNDERGLOW(hsv) \
    {1, 6, hsv}, {         \
        36 + 1, 6, hsv     \
    }
#define SET_NUMPAD(hsv)                     \
    {36 + 15, 5, hsv}, {36 + 22, 3, hsv}, { \
        36 + 27, 3, hsv                     \
    }
#define SET_NUMROW(hsv)                                                               \
    {11, 2, hsv}, {21, 2, hsv}, {31, 2, hsv}, {36 + 11, 2, hsv}, {36 + 21, 2, hsv}, { \
        36 + 31, 2, hsv                                                               \
    }
#define SET_INNER_COL(hsv) \
    {32, 4, hsv}, {        \
        36 + 32, 4, hsv    \
    }

#define SET_OUTER_COL(hsv) \
    {8, 4, hsv}, {         \
        36 + 8, 4, hsv     \
    }
#define SET_THUMB_CLUSTER(hsv) \
    {26, 2, hsv}, {            \
        36 + 26, 2, hsv        \
    }
#define SET_LAYER_ID(hsv)                                                                                                                                                                          \
    {0, 1, HSV_OVERRIDE_HELP(hsv, INDICATOR_BRIGHTNESS)}, {36 + 0, 1, HSV_OVERRIDE_HELP(hsv, INDICATOR_BRIGHTNESS)}, {1, 6, hsv}, {36 + 1, 6, hsv}, {8, 4, hsv}, {36 + 8, 4, hsv}, {26, 2, hsv}, { \
        36 + 26, 2, hsv                                                                                                                                                                            \
    }

#ifdef RGBLIGHT_ENABLE
char layer_state_str[70];
// Now define the array of layers. Later layers take precedence

// QWERTY,
// Light on inner column and underglow
const rgblight_segment_t PROGMEM layer_qwerty_lights[]    = RGBLIGHT_LAYER_SEGMENTS(SET_LAYER_ID(HSV_RED)

   );
const rgblight_segment_t PROGMEM layer_colemakdh_lights[] = RGBLIGHT_LAYER_SEGMENTS(SET_LAYER_ID(HSV_PINK));

// _NUM,
// Light on outer column and underglow
const rgblight_segment_t PROGMEM layer_num_lights[] = RGBLIGHT_LAYER_SEGMENTS(SET_LAYER_ID(HSV_TEAL)

);
// _SYMBOL,
// Light on inner column and underglow
const rgblight_segment_t PROGMEM layer_symbol_lights[] = RGBLIGHT_LAYER_SEGMENTS(SET_LAYER_ID(HSV_BLUE)

);
// _COMMAND,
// Light on inner column and underglow
const rgblight_segment_t PROGMEM layer_command_lights[] = RGBLIGHT_LAYER_SEGMENTS(SET_LAYER_ID(HSV_PURPLE));

//_NUMPAD
const rgblight_segment_t PROGMEM layer_numpad_lights[] = RGBLIGHT_LAYER_SEGMENTS(SET_INDICATORS(HSV_ORANGE), SET_UNDERGLOW(HSV_ORANGE), SET_NUMPAD(HSV_BLUE), {7, 4, HSV_ORANGE}, {25, 2, HSV_ORANGE}, {36 + 6, 4, HSV_ORANGE}, {36 + 25, 2, HSV_ORANGE});

const rgblight_segment_t *const PROGMEM my_rgb_layers[] = RGBLIGHT_LAYERS_LIST(

    layer_qwerty_lights,
    layer_num_lights, // overrides layer 1
    layer_symbol_lights, layer_command_lights, layer_numpad_lights,
    layer_switcher_lights, // Overrides other layers
    layer_colemakdh_lights);

layer_state_t layer_state_set_user(layer_state_t state) {
    rgblight_set_layer_state(0, layer_state_cmp(state, _DEFAULTS) && layer_state_cmp(default_layer_state, _QWERTY));

    rgblight_set_layer_state(1, layer_state_cmp(state, _LOWER));
    rgblight_set_layer_state(2, layer_state_cmp(state, _RAISE));
    rgblight_set_layer_state(3, layer_state_cmp(state, _ADJUST));
    rgblight_set_layer_state(4, layer_state_cmp(state, _NUMPAD));
    return state;
}
void keyboard_post_init_user(void) {
    // Enable the LED layers
    rgblight_layers = my_rgb_layers;

    rgblight_mode(10); // haven't found a way to set this in a more useful way
}
#endif
