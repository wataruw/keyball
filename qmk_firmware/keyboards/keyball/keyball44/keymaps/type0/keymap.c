/*
Copyright 2022 @Yowkees
Copyright 2022 MURAOKA Taro (aka KoRoN, @kaoriya)

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include QMK_KEYBOARD_H
#include "keymap_japanese.h"
#include "quantum.h"

// clang-format off
const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
  // keymap for default (VIA)
  [0] = LAYOUT_universal(
    KC_TAB   , KC_Q     , KC_W     , KC_E     , KC_R     , KC_T     ,                                        KC_Y     , KC_U     , KC_I     , KC_O     , KC_P     , KC_BSPC  ,
    KC_LCTL  , KC_A     , KC_S     , KC_D     , KC_F     , KC_G     ,                                        KC_H     , KC_J     , KC_K     , KC_L     , KC_SCLN  , KC_ENT   ,
    KC_LSFT  , KC_Z     , KC_X     , KC_C     , KC_V     , KC_B     ,                                        KC_N     , KC_M     , KC_COMM  , KC_DOT   , KC_SLSH  , KC_ESC   ,
              KC_LNG2, KC_LNG1, KC_LGUI                  , KC_SPC , MO(2),                       LT(1,KC_BTN1), KC_SPC, _______          , _______  , LT(3,KC_BTN2)
  ),

  [1] = LAYOUT_universal(
    _______  ,  KC_PMNS , JP_SCLN  , JP_7    , JP_8     , JP_9    ,                                         KC_HOME  , KC_UP    , KC_PGUP  , _______  , AML_TO   , _______  ,
    _______  ,  KC_PPLS , JP_COLN  , JP_4    , JP_5     , JP_6    ,                                         KC_LEFT  , KC_BTN1  , KC_RGHT  , KC_BTN2  , AML_I50  , KC_DEL   ,
    _______  ,  KC_PDOT , JP_0     , JP_1    , JP_2     , JP_3     ,                                        KC_END   , KC_DOWN  , KC_PGDN  , _______  , KBC_SAVE , _______  ,
             LCTL(KC_C) , LCTL(KC_V)   , _______       , _______ , KC_BTN1  ,                      _______  , _______  , _______       , _______  , _______
  ),

  [2] = LAYOUT_universal(
    _______  ,S(JP_1)   , S(JP_2)  , S(JP_3) , S(JP_4)  , S(JP_5)  ,                                        S(JP_6)   , S(JP_7)  , S(JP_8)  , S(JP_9)  , S(JP_SLSH), _______ ,
    _______  ,_______   , _______  , _______ , _______  , _______  ,                                        S(JP_CIRC), JP_MINS  , JP_LBRC  , JP_RBRC  , JP_BSLS  , KC_DEL   ,
    _______  ,_______   , _______  , _______ , _______  , _______  ,                                          JP_AT   ,S(JP_MINS),S(JP_LBRC),S(JP_RBRC), JP_YEN   , JP_CIRC  ,
                  _______  , _______  , _______  ,         _______  , _______  ,                     _______  , _______  , _______       , _______  , _______
  ),

  [3] = LAYOUT_universal(
    _______  , AML_TO   , AML_I50  , AML_D50  , _______  , _______  ,                                        _______  , _______  , _______  , _______  , _______  , _______  ,
    _______  , _______  , _______  , _______  , _______  , SCRL_DVI ,                                        _______  , _______  , _______  , _______  , _______  , _______  ,
    _______  , _______  , _______  , _______  , _______  , SCRL_DVD ,                                        CPI_D1K  , CPI_D100 , CPI_I100 , CPI_I1K  , _______  , KBC_SAVE ,
                  QK_BOOT  , KBC_RST  , _______  ,        _______  , _______  ,                   _______  , _______  , _______       , KBC_RST  , QK_BOOT
  ),
};
// clang-format on

layer_state_t layer_state_set_user(layer_state_t state) {
    // Auto enable scroll mode when the highest layer is 3
    keyball_set_scroll_mode(get_highest_layer(state) == 3);
    return state;
}

#ifdef OLED_ENABLE

#    include "lib/oledkit/oledkit.h"

void oledkit_render_info_user(void) {
    keyball_oled_render_keyinfo();
    keyball_oled_render_ballinfo();
    keyball_oled_render_layerinfo();
}
#endif
