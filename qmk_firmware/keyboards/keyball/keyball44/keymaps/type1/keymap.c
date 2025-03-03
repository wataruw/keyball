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
#include "twpair_on_jis.h"
#include "naginata.h"

// 薙刀式のオン/オフに使うキーの定義
static uint16_t ng_on_keys[] = {KC_H, KC_J};  // HJで薙刀式オン
static uint16_t ng_off_keys[] = {KC_F, KC_G}; // FGで薙刀式オフ

// clang-format off
const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
  // keymap for default
  [0] = LAYOUT_universal(
    KC_TAB   , KC_Q     , KC_W     , KC_E     , KC_R     , KC_T     ,                                        KC_Y     , KC_U     , KC_I     , KC_O     , KC_P     , KC_BSPC  ,
    KC_LCTL  , KC_A     , KC_S     , KC_D     , KC_F     , KC_G     ,                                        KC_H     , KC_J     , KC_K     , KC_L     , KC_SCLN  , KC_ENT   ,
    KC_LSFT  , KC_Z     , KC_X     , KC_C     , KC_V     , KC_B     ,                                        KC_N     , KC_M     , KC_COMM  , KC_DOT   , KC_SLSH  , KC_ESC   ,
              KC_LNG2, KC_LNG1, KC_LGUI                  , KC_SPC , MO(2),                       LT(1,KC_BTN1), KC_SPC, _______,     _______  , LT(3,KC_BTN2)
  ),

  [1] = LAYOUT_universal(
    _______  ,  KC_PMNS , KC_SCLN  , KC_7    , KC_8     , KC_9    ,                                         KC_HOME  , KC_UP    , KC_PGUP  , _______  , AML_TO   , _______  ,
    _______  ,  KC_PPLS , KC_QUOT  , KC_4    , KC_5     , KC_6    ,                                         KC_LEFT  , KC_BTN1  , KC_RGHT  , KC_BTN2  , AML_I50  , KC_DEL   ,
    _______  ,  KC_PDOT , KC_0     , KC_1    , KC_2     , KC_3     ,                                        KC_END   , KC_DOWN  , KC_PGDN  , _______  , KBC_SAVE , _______  ,
             RCTL_T(KC_C) , RCTL_T(KC_V) , _______       , _______ , KC_BTN1  ,                      _______  , _______  , _______       , _______  , _______
  ),

  [2] = LAYOUT_universal(
    _______  ,S(KC_1)   , S(KC_2)  , S(KC_3) , S(KC_4)  , S(KC_5)  ,                                         S(KC_6)  , S(KC_7)  , S(KC_8)  , S(KC_9)  , S(KC_SLSH), _______ ,
    _______  ,_______   , _______  , _______ , _______  , _______  ,                                         S(KC_EQL), KC_MINS  , KC_RBRC  , KC_BSLS  , KC_INT1  , KC_DEL   ,
    _______  ,_______   , _______  , _______ , _______  , _______  ,                                         KC_LBRC  ,S(KC_MINS),S(KC_RBRC),S(KC_BSLS), KC_INT3  , KC_EQL   ,
                  _______  , _______  , _______  ,         _______  , _______  ,                     _______  , _______  , _______       , _______  , _______
  ),

  // 薙刀式レイヤー
  [3] = LAYOUT_universal(
    _______  , NG_Q     , NG_W     , NG_E     , NG_R     , NG_T     ,                                        NG_Y     , NG_U     , NG_I     , NG_O     , NG_P     , KC_BSPC  ,
    _______  , NG_A     , NG_S     , NG_D     , NG_F     , NG_G     ,                                        NG_H     , NG_J     , NG_K     , NG_L     , NG_SCLN  , KC_ENT   ,
    _______  , NG_Z     , NG_X     , NG_C     , NG_V     , NG_B     ,                                        NG_N     , NG_M     , NG_COMM  , NG_DOT   , NG_SLSH  , KC_ESC   ,
              KC_LNG1, KC_LNG2, KC_LGUI                  , NG_SHFT  , _______  ,                   _______  , NG_SHFT2   , _______        , _______  , _______
  ),
};
// clang-format on

void keyboard_post_init_user(void) {
    // 薙刀式の初期化
    set_naginata(3, ng_on_keys, ng_off_keys); // レイヤー3を薙刀式レイヤーとして使用
}

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    // 薙刀式の処理
    if (!process_naginata(keycode, record)) {
        return false;
    }
    
    return twpair_on_jis(keycode, record);
}

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
