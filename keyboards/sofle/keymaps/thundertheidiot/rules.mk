MOUSEKEY_ENABLE = yes
EXTRAKEY_ENABLE = yes
# CONSOLE_ENABLE = yes # debug
COMMAND_ENABLE = no
RGBLIGHT_ENABLE = no 
ENCODER_ENABLE = yes
LTO_ENABLE = yes
OLED_ENABLE = yes
NKRO_ENABLE = yes
TRI_LAYER_ENABLE = yes
TAP_DANCE_ENABLE = yes
CAPS_WORD_ENABLE = yes
# OLED_DRIVER = SSD1306

SRC += 	./led.c \
	./tapdance.c

CONVERT_TO=promicro_rp2040
