RGBLIGHT_ENABLE = no
OLED_ENABLE = yes
# VIA_ENABLE = yes
UNICODE_ENABLE = yes

# Add naginata support
SRC += twpair_on_jis.c
SRC += naginata_v15.c

LTO_ENABLE = yes
CONSOLE_ENABLE = no
COMMAND_ENABLE = no
MOUSEKEY_ENABLE = no
EXTRAKEY_ENABLE = no
GRAVE_ESC_ENABLE = no