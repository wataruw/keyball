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

#ifdef CONSOLE_ENABLE
#   include "print.h"
#endif

// Basic definitions
#ifndef TAPPING_TERM
#   define TAPPING_TERM 200
#endif

#ifndef ONESHOT_TIMEOUT
#   define ONESHOT_TIMEOUT 5000
#endif

#define TAPPING_TERM_PER_KEY
#define LANGUAGE_RETURN JP

// レイヤー構造
#define BASE 0
#define KSFT 1   // KATANA Shift
#define HEN1 2   // 編集モード1
#define HEN2 3   // 編集モード2
//促音やmwに使うレイヤー
#define after_R 4 // Rを押したときのレイヤー
#define after_K 5 // Kを押したときのレイヤー
#define after_T 6 // Tを押したときのレイヤー
#define after_S 7 // Sを押したときのレイヤー
#define after_P 8 // Pを押したときのレイヤー
#define after_H 9 // Hを押したときのレイヤー
#define after_N 10// Nを押したときのレイヤー
#define after_G 11// Gを押したときのレイヤー
#define after_D 12// Dを押したときのレイヤー
#define after_Z 13// Zを押したときのレイヤー
#define after_B 14// Bを押したときのレイヤー
#define after_F 15// Fを押したときのレイヤー
#define HEN3 16   // 編集モード2のシフト
//Lower(SYMB),Raise(META)
#define SYMB 29
#define META 30

#define _______ KC_TRNS
#define XXXXXXX KC_NO

static bool winmac = true; // win or mac
static bool eisu = false;  // eisu on off
static bool kana = false;  // kana on off

// Fix for SEND_STRING macro formatting
#define MACRO_SEND_STR(data) SEND_STRING(data)
#define MACRO_TAP_STR(key) SEND_STRING(SS_TAP(key))

// macro name
enum custom_keycodes {
// んと（。）確定
  NX = SAFE_RANGE,
  DTET, 
// 英数かな、WinMac切り替え
  MC_EISU,
  MC_KANA,
  SW_2MAC,
  SW_2WIN,
//それぞれの子音を打った時の挙動
  MC_K,
  MC_R,
  MC_S,
  MC_T,
  MC_P,
  MC_H,
  MC_N,
  MC_G,
  MC_Z,
  MC_D,
  MC_F,
  MC_B,
//促音
  MC_XTUK,
  MC_XTUG,
  MC_XTUS,
  MC_XTUZ,
  MC_XTUT,
  MC_XTUD,
  MC_XTUH,
  MC_XTUB,
  MC_XTUP,
  MC_XTUF,
//ふたつの子音で決まる子音
  MC_M,
  MC_W,
  MC_V,
// 編集モードなどのマクロ
  MC_QUES,
  MC_EXRM,
  MC_NKGR,
  MC_MARU,
  MC_SNTN,
  MC_NGDS,
  MC_SLSH,
  MC_COLN,
  MC_KGHR,
  MC_KGTJ,
  MC_KKHR,
  MC_KKTJ,
  MC_NJHR,
  MC_NJTJ,
  MC_FRST,
  MC_FRHR,
  MC_FRTJ,
  MC_SMHR,
  MC_SMTJ,
  MC_REDO,
  MC_SAVE,
  MC_COPY,
  MC_PSTE,
  MC_UNDO,
  MC_CUT,
  MC_SHEN,
  MC_HIRA,
  MC_KATA,
  MC_EDBS,
  MC_ETED,
  MC_UP5,
  MC_LT5,
  MC_RT5,
  MC_DN5,
  MC_KG1,
  MC_KG2,
  MC_KG3,
  MC_SP1,
  MC_SP2,
  MC_SP3
};

// 日本語キーボード用のキーコード定義
#define JP_ZKHK KC_GRV  // Zenkaku/Hankaku
#define JP_MHEN KC_INT5 // Muhenkan
#define JP_HENK KC_INT4 // Henkan
#define JP_KANA KC_INT2 // Katakana/Hiragana

// Tap Dance definitions
enum {
    TD_ESC_CAPS,
};

tap_dance_action_t tap_dance_actions[] = {
    [TD_ESC_CAPS] = ACTION_TAP_DANCE_DOUBLE(KC_ESC, KC_CAPS),
};

// oneshot層の定義
#define ONESHOT_START 1
#define ONESHOT_PRESSED 2

// Key overrides
const key_override_t delete_key_override = ko_make_basic(MOD_MASK_SHIFT, KC_BSPACE, KC_DELETE);
const key_override_t **key_overrides = (const key_override_t *[]){
    &delete_key_override,
    NULL
};

// macro実装部
bool process_record_user(uint16_t keycode, keyrecord_t *record) {
  switch (keycode) {
    case SW_2MAC:
      if (record->event.pressed) {
        winmac = false; // Mac
        SEND_STRING("Mac");   
      }
      break;
    case SW_2WIN:
      if (record->event.pressed) {
        winmac = true; // Win
        SEND_STRING("Win");   
      }
      break;
    case MC_EISU: // 英数
      if (record->event.pressed) {
        eisu = true;
        layer_on(HEN1);
      } else {
        layer_off(HEN1);
        if (eisu) {
          if (winmac) {
            tap_code(JP_MHEN);  // Win
          } else {
            tap_code(KC_LANG2); // Mac
          }
        }
        eisu = false;        
      }
      return false;
      break;
    case MC_KANA: // かな
      if (record->event.pressed) {
        kana = true;
        layer_on(HEN2);
      } else {
        layer_off(HEN2);
        if (kana) {
          if (winmac) {
            tap_code(JP_HENK);  // Win
          } else {
            tap_code(KC_LANG1); // Mac
          }
        }
        kana = false;        
      }
      return false;
      break;
    case NX:
      if (record->event.pressed) {
        MACRO_SEND_STR("nn");   // ローマ字のん単打
      }
      break;
    case DTET:
      if (record->event.pressed) {
        MACRO_SEND_STR("." SS_TAP(X_ENTER));  // 。確定
      }
      break;
    case MC_R:
      if (record->event.pressed) {
        MACRO_SEND_STR("r");
        set_oneshot_layer(after_R, ONESHOT_START);
      } else {
        clear_oneshot_layer_state(ONESHOT_PRESSED);
      }
      break;
    case MC_K:
      if (record->event.pressed) {
        MACRO_SEND_STR("k");
        set_oneshot_layer(after_K, ONESHOT_START);
      } else {
        clear_oneshot_layer_state(ONESHOT_PRESSED);
      }
      break;
    case MC_T:
      if (record->event.pressed) {
        MACRO_SEND_STR("t");
        set_oneshot_layer(after_T, ONESHOT_START);
      } else {
        clear_oneshot_layer_state(ONESHOT_PRESSED);
      }
      break;
    case MC_S:
      if (record->event.pressed) {
        MACRO_SEND_STR("s");
        set_oneshot_layer(after_S, ONESHOT_START);
      } else {
        clear_oneshot_layer_state(ONESHOT_PRESSED);
      }
      break;
    case MC_P:
      if (record->event.pressed) {
        MACRO_SEND_STR("p");
        set_oneshot_layer(after_P, ONESHOT_START);
      } else {
        clear_oneshot_layer_state(ONESHOT_PRESSED);
      }
      break;
    case MC_H:
      if (record->event.pressed) {
        MACRO_SEND_STR("h");
        set_oneshot_layer(after_H, ONESHOT_START);
      } else {
        clear_oneshot_layer_state(ONESHOT_PRESSED);
      }
      break;
    case MC_N:
      if (record->event.pressed) {
        MACRO_SEND_STR("n");
        set_oneshot_layer(after_N, ONESHOT_START);
      } else {
        clear_oneshot_layer_state(ONESHOT_PRESSED);
      }
      break;
    case MC_G:
      if (record->event.pressed) {
        MACRO_SEND_STR("g");
        set_oneshot_layer(after_G, ONESHOT_START);
      } else {
        clear_oneshot_layer_state(ONESHOT_PRESSED);
      }
      break;
    case MC_D:
      if (record->event.pressed) {
        MACRO_SEND_STR("d");
        set_oneshot_layer(after_D, ONESHOT_START);
      } else {
        clear_oneshot_layer_state(ONESHOT_PRESSED);
      }
      break;
    case MC_Z:
      if (record->event.pressed) {
        MACRO_SEND_STR("z");
        set_oneshot_layer(after_Z, ONESHOT_START);
      } else {
        clear_oneshot_layer_state(ONESHOT_PRESSED);
      }
      break;
    case MC_F:
      if (record->event.pressed) {
        MACRO_SEND_STR("f");
        set_oneshot_layer(after_F, ONESHOT_START);
      } else {
        clear_oneshot_layer_state(ONESHOT_PRESSED);
      }
      break;
    case MC_B:
      if (record->event.pressed) {
        MACRO_SEND_STR("b");
        set_oneshot_layer(after_B, ONESHOT_START);
      } else {
        clear_oneshot_layer_state(ONESHOT_PRESSED);
      }
      break;
    case MC_M:
      if (record->event.pressed) {
        MACRO_SEND_STR(SS_TAP(X_BSPACE) "m");
      }
      break;
    case MC_W:
      if (record->event.pressed) {
        MACRO_SEND_STR(SS_TAP(X_BSPACE) "w");
      }
      break;
    case MC_V:
      if (record->event.pressed) {
        MACRO_SEND_STR(SS_TAP(X_BSPACE) "v");
      }
      break;
    case MC_XTUK:
      if (record->event.pressed) {
        MACRO_SEND_STR(SS_TAP(X_BSPACE) "xtuk");
      }
      break;
    case MC_XTUG:
      if (record->event.pressed) {
        MACRO_SEND_STR(SS_TAP(X_BSPACE) "xtug");
      }
      break;
    case MC_XTUS:
      if (record->event.pressed) {
        MACRO_SEND_STR(SS_TAP(X_BSPACE) "xtus");
      }
      break;
    case MC_XTUZ:
      if (record->event.pressed) {
        MACRO_SEND_STR(SS_TAP(X_BSPACE) "xtuz");
      }
      break;
    case MC_XTUT:
      if (record->event.pressed) {
        MACRO_SEND_STR(SS_TAP(X_BSPACE) "xtut");
      }
      break;
    case MC_XTUD:
      if (record->event.pressed) {
        MACRO_SEND_STR(SS_TAP(X_BSPACE) "xtud");
      }
      break;
    case MC_XTUH:
      if (record->event.pressed) {
        MACRO_SEND_STR(SS_TAP(X_BSPACE) "xtuh");
      }
      break;
    case MC_XTUB:
      if (record->event.pressed) {
        MACRO_SEND_STR(SS_TAP(X_BSPACE) "xtub");
      }
      break;
    case MC_XTUP:
      if (record->event.pressed) {
        MACRO_SEND_STR(SS_TAP(X_BSPACE) "xtup");
      }
      break;
    case MC_XTUF:
      if (record->event.pressed) {
        MACRO_SEND_STR(SS_TAP(X_BSPACE) "xtuf");
      }
      break;
    case MC_REDO:
      if (record->event.pressed) {
        if (winmac) {
          MACRO_SEND_STR(SS_LCTRL("y")); //win
        } else {
          MACRO_SEND_STR(SS_LGUI(SS_LSFT("z"))); //mac
        }
      }
      eisu = false;
      kana = false;
      break;
    case MC_SAVE:
      if (record->event.pressed) {
        if (winmac) {
          MACRO_SEND_STR(SS_LCTRL("s")); //win
        } else {
          MACRO_SEND_STR(SS_LGUI("s")); //mac
        }
      }
      eisu = false;
      kana = false;
      break;
    case MC_COPY:
      if (record->event.pressed) {
        if (winmac) {
          MACRO_SEND_STR(SS_LCTRL("c")); //win
        } else {
          MACRO_SEND_STR(SS_LGUI("c")); //mac
        }
      }
      eisu = false;
      kana = false;
      break;
    case MC_PSTE:
      if (record->event.pressed) {
        if (winmac) {
          MACRO_SEND_STR(SS_LCTRL("v")); //win
        } else {
          MACRO_SEND_STR(SS_LGUI("v")); //mac
        }
      }
      eisu = false;
      kana = false;
      break;
    case MC_UNDO:
      if (record->event.pressed) {
        if (winmac) {
          MACRO_SEND_STR(SS_LCTRL("z")); //win
        } else {
          MACRO_SEND_STR(SS_LGUI("z")); //mac
        }
      }
      eisu = false;
      kana = false;
      break;
    case MC_CUT:
      if (record->event.pressed) {
        if (winmac) {
          MACRO_SEND_STR(SS_LCTRL("x")); //win
        } else {
          MACRO_SEND_STR(SS_LGUI("x")); //mac
        }
      }
      eisu = false;
      kana = false;
      break;
    case MC_SHEN:
      if (record->event.pressed) {
        if (winmac) {
          MACRO_SEND_STR(SS_TAP(X_INT4)); //win
        } else {
          tap_code(KC_LANG1);
          tap_code(KC_LANG1); //macかなキーダブルクリック
        }
      }
      eisu = false;
      kana = false;
      break;
    case MC_HIRA:
      if (record->event.pressed) {
        if (winmac) {
          MACRO_SEND_STR(SS_LCTRL("u")); //win
        } else {
          MACRO_SEND_STR(SS_LCTRL("j")); //mac
        }
      }
      eisu = false;
      kana = false;
      break;
    case MC_KATA:
      if (record->event.pressed) {
        if (winmac) {
          MACRO_SEND_STR(SS_LCTRL("i")); //win
        } else {
          MACRO_SEND_STR(SS_LCTRL("k")); //mac
        }
      }
      eisu = false;
      kana = false;
      break;
    case MC_QUES:
      if (record->event.pressed) {
        MACRO_SEND_STR(SS_LSFT(SS_TAP(X_SLASH)) SS_TAP(X_ENTER));
      }
      break;
    case MC_EXRM:
      if (record->event.pressed) {
        MACRO_SEND_STR(SS_LSFT(SS_TAP(X_1)) SS_TAP(X_ENTER));
      }
      break;
    case MC_NKGR:
      if (record->event.pressed) {
        MACRO_SEND_STR("nakaguro" SS_TAP(X_SPACE) SS_TAP(X_ENTER));
      }
      break;
    case MC_MARU:
      if (record->event.pressed) {
        MACRO_SEND_STR("siromaru" SS_TAP(X_SPACE) SS_TAP(X_ENTER));
      }
      break;
    case MC_SNTN:
      if (record->event.pressed) {
        MACRO_SEND_STR("santen" SS_TAP(X_SPACE) SS_TAP(X_ENTER));
        MACRO_SEND_STR("santen" SS_TAP(X_SPACE) SS_TAP(X_ENTER));
      }
      break;
    case MC_NGDS:
      if (record->event.pressed) {
        MACRO_SEND_STR("nagadasshu" SS_TAP(X_SPACE) SS_TAP(X_ENTER));
        MACRO_SEND_STR("nagadasshu" SS_TAP(X_SPACE) SS_TAP(X_ENTER));
      }
      break;
    case MC_SLSH:
      if (record->event.pressed) {
        MACRO_SEND_STR(SS_TAP(X_SLASH) SS_TAP(X_ENTER));
      }
      break;
    case MC_COLN:
      if (record->event.pressed) {
        MACRO_SEND_STR(SS_TAP(X_QUOTE) SS_TAP(X_ENTER));
      }
      break;
    case MC_KGHR:
      if (record->event.pressed) {
        MACRO_SEND_STR(SS_TAP(X_RBRACKET) SS_TAP(X_ENTER));
      }
      break;
    case MC_KGTJ:
      if (record->event.pressed) {
        MACRO_SEND_STR(SS_TAP(X_BSLASH) SS_TAP(X_ENTER));
      }
      break;
    case MC_KKHR:
      if (record->event.pressed) {
        MACRO_SEND_STR(SS_LSFT(SS_TAP(X_8)) SS_TAP(X_ENTER));
      }
      break;
    case MC_KKTJ:
      if (record->event.pressed) {
        MACRO_SEND_STR(SS_LSFT(SS_TAP(X_9)) SS_TAP(X_ENTER));
      }
      break;
    case MC_NJHR:
      if (record->event.pressed) {
        MACRO_SEND_STR("nijuhira" SS_TAP(X_SPACE) SS_TAP(X_ENTER));
      }
      break;
    case MC_NJTJ:
      if (record->event.pressed) {
        MACRO_SEND_STR("nijutoji" SS_TAP(X_SPACE) SS_TAP(X_ENTER));
      }
      break;
    case MC_FRST:
      if (record->event.pressed) {
        MACRO_SEND_STR(SS_LSFT(SS_TAP(X_INT3)) SS_TAP(X_ENTER));
      }
      break;
    case MC_FRHR:
      if (record->event.pressed) {
        MACRO_SEND_STR("furihira" SS_TAP(X_SPACE) SS_TAP(X_ENTER));
      }
      break;
    case MC_FRTJ:
      if (record->event.pressed) {
        MACRO_SEND_STR("furitoji" SS_TAP(X_SPACE) SS_TAP(X_ENTER));
      }
      break;
    case MC_SMHR:
      if (record->event.pressed) {
        MACRO_SEND_STR("sumihira" SS_TAP(X_SPACE) SS_TAP(X_ENTER));
      }
      break;
    case MC_SMTJ:
      if (record->event.pressed) {
        MACRO_SEND_STR("sumitoji" SS_TAP(X_SPACE) SS_TAP(X_ENTER));
      }
      break;
    case MC_EDBS:
      if (record->event.pressed) {
        MACRO_SEND_STR(SS_LSFT(SS_TAP(X_END)));
        MACRO_SEND_STR(SS_TAP(X_BSPACE));
      }
      break;
    case MC_ETED:
      if (record->event.pressed) {
        MACRO_SEND_STR(SS_TAP(X_ENTER) SS_TAP(X_END));
      }
      break;
    case MC_UP5:
      if (record->event.pressed) {
        MACRO_SEND_STR(SS_TAP(X_UP) SS_TAP(X_UP) SS_TAP(X_UP) SS_TAP(X_UP) SS_TAP(X_UP));
      }
      break;
    case MC_LT5:
      if (record->event.pressed) {
        MACRO_SEND_STR(SS_TAP(X_LEFT) SS_TAP(X_LEFT) SS_TAP(X_LEFT) SS_TAP(X_LEFT) SS_TAP(X_LEFT));
      }
      break;
    case MC_RT5:
      if (record->event.pressed) {
        MACRO_SEND_STR(SS_TAP(X_RIGHT) SS_TAP(X_RIGHT) SS_TAP(X_RIGHT) SS_TAP(X_RIGHT) SS_TAP(X_RIGHT));
      }
      break;
    case MC_DN5:
      if (record->event.pressed) {
        MACRO_SEND_STR(SS_TAP(X_DOWN) SS_TAP(X_DOWN) SS_TAP(X_DOWN) SS_TAP(X_DOWN) SS_TAP(X_DOWN));
      }
      break;
    case MC_KG1:
      if (record->event.pressed) {
        MACRO_SEND_STR(SS_TAP(X_BSLASH) SS_TAP(X_ENTER) SS_TAP(X_ENTER) " ");
      }
      break;
    case MC_KG2:
      if (record->event.pressed) {
        MACRO_SEND_STR(SS_TAP(X_BSLASH) SS_TAP(X_ENTER) SS_TAP(X_ENTER) SS_TAP(X_RBRACKET) SS_TAP(X_ENTER));
      }
      break;
    case MC_KG3:
      if (record->event.pressed) {
        MACRO_SEND_STR(SS_TAP(X_BSLASH) SS_TAP(X_ENTER) SS_TAP(X_ENTER));
      }
      break;
    case MC_SP1:
      if (record->event.pressed) {
        MACRO_SEND_STR(SS_TAP(X_HOME) SS_TAP(X_ENTER) " " SS_TAP(X_END));
      }
      break;
    case MC_SP2:
      if (record->event.pressed) {
        MACRO_SEND_STR(SS_TAP(X_HOME) SS_TAP(X_ENTER) "   " SS_TAP(X_END));
      }
      break;
    case MC_SP3:
      if (record->event.pressed) {
        MACRO_SEND_STR("   ");
      }
      break;
    default:
      if (record->event.pressed) {
        eisu = false;
        kana = false;
      }
      break;
  }
  return true;
}

// キーマップ定義
const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
  [BASE] = LAYOUT_universal(
    MC_T,    MC_S,    MC_P,    KC_UP,   KC_BSPC, KC_I,                                      KC_E,    XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,
    MC_R,    MC_K,    KC_LEFT, KC_RGHT, KC_A,    KC_U,                                      KC_Y,    XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,
    MC_H,    MC_N,    KC_DOWN, KC_ENT,  KC_O,    NX,                                        KC_MINS, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,
             MC_EISU, LT(KSFT, KC_SPC), MO(SYMB), MO(META), LT(KSFT, KC_SPC),     MC_KANA, KC_LSFT, XXXXXXX, XXXXXXX, XXXXXXX
  ),

  [KSFT] = LAYOUT_universal(
    MC_D,    MC_Z,    MC_F,       S(KC_UP),   KC_X,    KC_I,                                        KC_E,    XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,
    MC_R,    MC_G,    S(KC_LEFT), S(KC_RGHT), KC_A,    KC_U,                                        KC_Y,    XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,
    MC_B,    KC_COMM, S(KC_DOWN), DTET,       KC_O,    NX,                                         KC_MINS, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,
             _______, _______, MO(SYMB), MO(META), _______, _______, KC_LSFT,                      XXXXXXX, XXXXXXX, XXXXXXX
  ),

  [HEN1] = LAYOUT_universal(
    MC_REDO, MC_SAVE, KC_ESC,  KC_HOME, MC_EDBS, MC_SHEN,                                          KC_DEL,  XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,
    MC_COPY, MC_PSTE, MC_KATA, MC_ETED, KC_UP,   S(KC_UP),                                        MC_UP5,  XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,
    MC_UNDO, MC_CUT,  MC_HIRA, KC_END,  KC_DOWN, S(KC_DOWN),                                      MC_DN5,  XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,
             _______, KC_LSFT, _______, _______, KC_LSFT, _______, KC_LSFT,                       XXXXXXX, XXXXXXX, XXXXXXX
  ),

  [HEN2] = LAYOUT_universal(
    MC_NKGR, MC_MARU, MC_SP1,  MC_KG1,  KC_PGDN, KC_PGUP,                                         MC_FRST, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,
    MC_EXRM, MC_QUES, MC_SP2,  MC_KG2,  MC_KGHR, MC_KKHR,                                        MC_FRHR, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,
    MC_SNTN, MC_NGDS, MC_SP3,  MC_KG3,  MC_KGTJ, MC_KKTJ,                                        MC_FRTJ, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,
             _______, MO(HEN3), _______, _______, MO(HEN3), _______, KC_LSFT,                    XXXXXXX, XXXXXXX, XXXXXXX
  ),

  [HEN3] = LAYOUT_universal(
    MC_SLSH, MC_COLN, _______, _______, _______, _______,                                          _______, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,
    _______, _______, _______, _______, MC_NJHR, MC_SMHR,                                         _______, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,
    _______, _______, _______, _______, MC_NJTJ, MC_SMTJ,                                         _______, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,
             _______, _______, _______, _______, _______, _______, _______,                       XXXXXXX, XXXXXXX, XXXXXXX
  ),

  [after_R] = LAYOUT_universal(
    MC_XTUT, MC_XTUS, MC_XTUP, _______, _______, _______,                                         _______, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,
    _______, MC_M,    _______, _______, _______, _______,                                         _______, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,
    MC_XTUH, _______, _______, _______, _______, _______,                                         _______, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,
             _______, _______, _______, _______, _______, _______, _______,                      XXXXXXX, XXXXXXX, XXXXXXX
  ),

  [after_K] = LAYOUT_universal(
    MC_XTUT, MC_XTUS, MC_XTUP, _______, _______, _______,                                         _______, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,
    MC_M,    _______, _______, _______, _______, _______,                                         _______, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,
    MC_XTUH, _______, _______, _______, _______, _______,                                         _______, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,
             _______, _______, _______, _______, _______, _______, _______,                      XXXXXXX, XXXXXXX, XXXXXXX
  ),

  [after_T] = LAYOUT_universal(
    _______, MC_XTUS, MC_XTUP, _______, _______, _______,                                         _______, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,
    _______, MC_XTUK, _______, _______, _______, _______,                                         _______, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,
    MC_XTUH, _______, _______, _______, _______, _______,                                         _______, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,
             _______, _______, _______, _______, _______, _______, _______,                      XXXXXXX, XXXXXXX, XXXXXXX
  ),

  [after_S] = LAYOUT_universal(
    MC_XTUT, _______, MC_XTUP, _______, _______, _______,                                         _______, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,
    _______, MC_XTUK, _______, _______, _______, _______,                                         _______, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,
    MC_XTUH, _______, _______, _______, _______, _______,                                         _______, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,
             _______, _______, _______, _______, _______, _______, _______,                      XXXXXXX, XXXXXXX, XXXXXXX
  ),

  [after_P] = LAYOUT_universal(
    MC_XTUT, MC_XTUS, _______, _______, _______, _______,                                         _______, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,
    _______, MC_XTUK, _______, _______, _______, _______,                                         _______, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,
    MC_XTUH, _______, _______, _______, _______, _______,                                         _______, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,
             _______, _______, _______, _______, _______, _______, _______,                      XXXXXXX, XXXXXXX, XXXXXXX
  ),

  [after_H] = LAYOUT_universal(
    MC_XTUT, MC_XTUS, MC_XTUP, _______, _______, _______,                                         _______, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,
    _______, MC_XTUK, _______, _______, _______, _______,                                         _______, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,
    _______, MC_W,    _______, _______, _______, _______,                                         _______, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,
             _______, _______, _______, _______, _______, _______, _______,                      XXXXXXX, XXXXXXX, XXXXXXX
  ),

  [after_N] = LAYOUT_universal(
    MC_XTUT, MC_XTUS, MC_XTUP, _______, _______, _______,                                         _______, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,
    _______, MC_XTUK, _______, _______, _______, _______,                                         _______, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,
    MC_W,    _______, _______, _______, _______, _______,                                         _______, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,
             _______, _______, _______, _______, _______, _______, _______,                      XXXXXXX, XXXXXXX, XXXXXXX
  ),

  [after_G] = LAYOUT_universal(
    MC_XTUD, MC_XTUZ, MC_XTUF, _______, _______, _______,                                         _______, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,
    MC_M,    _______, _______, _______, _______, _______,                                         _______, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,
    MC_XTUB, _______, _______, _______, _______, _______,                                         _______, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,
             _______, _______, _______, _______, _______, _______, _______,                      XXXXXXX, XXXXXXX, XXXXXXX
  ),

  [after_D] = LAYOUT_universal(
    _______, MC_XTUZ, MC_XTUF, _______, _______, _______,                                         _______, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,
    _______, MC_XTUG, _______, _______, _______, _______,                                         _______, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,
    MC_XTUB, _______, _______, _______, _______, _______,                                         _______, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,
             _______, _______, _______, _______, _______, _______, _______,                      XXXXXXX, XXXXXXX, XXXXXXX
  ),

  [after_Z] = LAYOUT_universal(
    MC_XTUD, _______, MC_XTUF, _______, _______, _______,                                         _______, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,
    _______, MC_XTUG, _______, _______, _______, _______,                                         _______, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,
    MC_XTUB, _______, _______, _______, _______, _______,                                         _______, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,
             _______, _______, _______, _______, _______, _______, _______,                      XXXXXXX, XXXXXXX, XXXXXXX
  ),

  [after_B] = LAYOUT_universal(
    MC_XTUD, MC_XTUZ, MC_XTUB, _______, _______, _______,                                         _______, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,
    _______, MC_XTUG, _______, _______, _______, _______,                                         _______, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,
    _______, MC_V,    _______, _______, _______, _______,                                         _______, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,
             _______, _______, _______, _______, _______, _______, _______,                      XXXXXXX, XXXXXXX, XXXXXXX
  ),

  [after_F] = LAYOUT_universal(
    MC_XTUD, MC_XTUZ, _______, _______, _______, _______,                                         _______, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,
    _______, MC_XTUG, _______, _______, _______, _______,                                         _______, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,
    MC_XTUB, _______, _______, _______, _______, _______,                                         _______, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,
             _______, _______, _______, _______, _______, _______, _______,                      XXXXXXX, XXXXXXX, XXXXXXX
  ),

  [SYMB] = LAYOUT_universal(
    KC_C,    KC_J,    _______, S(KC_MINS), KC_7,    KC_8,                                         KC_9,    XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,
    KC_L,    KC_J,    KC_SLSH, KC_MINS,    KC_4,    KC_5,                                         KC_6,    XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,
    _______, _______, S(KC_QUOT), S(KC_SCLN), KC_1,  KC_2,                                         KC_3,    XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,
             _______, KC_LSFT, _______, SW_2MAC,    KC_0,    _______, KC_LSFT,                   XXXXXXX, XXXXXXX, XXXXXXX
  ),

  [META] = LAYOUT_universal(
    KC_MINS, KC_EQL,  KC_JYEN, _______, S(KC_7), S(KC_8),                                         S(KC_9), XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,
    KC_LBRC, KC_RBRC, KC_BSLS, _______, S(KC_4), S(KC_5),                                         S(KC_6), XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,
    KC_SCLN, KC_QUOT, S(KC_RO),_______, S(KC_1), S(KC_2),                                         S(KC_3), XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,
             _______, KC_LSFT, SW_2WIN, _______, KC_LSFT, _______, KC_LSFT,                     XXXXXXX, XXXXXXX, XXXXXXX
  ),

  [3] = LAYOUT_universal(
    RGB_TOG, _______, _______, _______, _______, _______,                                         _______, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,
    _______, _______, _______, _______, _______, _______,                                         _______, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,
    _______, _______, _______, _______, _______, _______,                                         _______, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,
             _______, _______, _______, _______, _______, _______, _______,                      XXXXXXX, XXXXXXX, XXXXXXX
  )
};

// トラックボールのスクロールモード設定を保持
layer_state_t layer_state_set_user(layer_state_t state) {
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
