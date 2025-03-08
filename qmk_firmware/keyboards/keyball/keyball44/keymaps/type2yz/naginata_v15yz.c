/* Copyright eswai <@eswai> / Satoru NAKAYA <@tor-nky>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

/*

TODO

*/

#include QMK_KEYBOARD_H
#include "naginata.h"

#include <string.h>

#define MX 5
#define MY 5

static bool is_naginata = false; // 薙刀式がオンかオフか
static uint8_t naginata_layer = 0; // NG_*を配置しているレイヤー番号
static uint16_t ngon_keys[2]; // 薙刀式をオンにするキー(通常HJ)
static uint16_t ngoff_keys[2]; // 薙刀式をオフにするキー(通常FG)
static uint32_t pressed_keys; // 押しているキーのビットをたてる
static int8_t n_pressed_keys; // 押しているキーの数
static uint16_t nginput[MX][MY];
static int8_t ix_nginput = -1; // 最後の入力位置
static int8_t iy_nginput[MX]; // 最後の入力位置

// 31キーを32bitの各ビットに割り当てる
#define B_Q    (1UL<<0)
#define B_W    (1UL<<1)
#define B_E    (1UL<<2)
#define B_R    (1UL<<3)
#define B_T    (1UL<<4)

#define B_Y    (1UL<<5)
#define B_U    (1UL<<6)
#define B_I    (1UL<<7)
#define B_O    (1UL<<8)
#define B_P    (1UL<<9)

#define B_A    (1UL<<10)
#define B_S    (1UL<<11)
#define B_D    (1UL<<12)
#define B_F    (1UL<<13)
#define B_G    (1UL<<14)

#define B_H    (1UL<<15)
#define B_J    (1UL<<16)
#define B_K    (1UL<<17)
#define B_L    (1UL<<18)
#define B_SCLN (1UL<<19)

#define B_Z    (1UL<<20)
#define B_X    (1UL<<21)
#define B_C    (1UL<<22)
#define B_V    (1UL<<23)
#define B_B    (1UL<<24)

#define B_N    (1UL<<25)
#define B_M    (1UL<<26)
#define B_COMM (1UL<<27)
#define B_DOT  (1UL<<28)
#define B_SLSH (1UL<<29)

#define B_SHFT (1UL<<30)

// キーコードとキービットの対応
// メモリ削減のため配列はNG_Qを0にしている
const uint32_t ng_key[] = {
  [NG_Q    - NG_Q]  = B_Q,
  [NG_W    - NG_Q]  = B_W,
  [NG_E    - NG_Q]  = B_E,
  [NG_R    - NG_Q]  = B_R,
  [NG_T    - NG_Q]  = B_T,

  [NG_Y    - NG_Q]  = B_Y,
  [NG_U    - NG_Q]  = B_U,
  [NG_I    - NG_Q]  = B_I,
  [NG_O    - NG_Q]  = B_O,
  [NG_P    - NG_Q]  = B_P,

  [NG_A    - NG_Q]  = B_A,
  [NG_S    - NG_Q]  = B_S,
  [NG_D    - NG_Q]  = B_D,
  [NG_F    - NG_Q]  = B_F,
  [NG_G    - NG_Q]  = B_G,

  [NG_H    - NG_Q]  = B_H,
  [NG_J    - NG_Q]  = B_J,
  [NG_K    - NG_Q]  = B_K,
  [NG_L    - NG_Q]  = B_L,
  [NG_SCLN - NG_Q]  = B_SCLN,

  [NG_Z    - NG_Q]  = B_Z,
  [NG_X    - NG_Q]  = B_X,
  [NG_C    - NG_Q]  = B_C,
  [NG_V    - NG_Q]  = B_V,
  [NG_B    - NG_Q]  = B_B,

  [NG_N    - NG_Q]  = B_N,
  [NG_M    - NG_Q]  = B_M,
  [NG_COMM - NG_Q]  = B_COMM,
  [NG_DOT  - NG_Q]  = B_DOT,
  [NG_SLSH - NG_Q]  = B_SLSH,

  [NG_SHFT - NG_Q]  = B_SHFT,
  [NG_SHFT2 - NG_Q] = B_SHFT,
};

// カナ変換テーブル
typedef struct {
  uint32_t shift;
  uint32_t douji;
  char kana[6];
  void (*func)(void);
} naginata_kanamap;

const PROGMEM naginata_kanamap ngdickana[] = {
  // 清音
  {.shift = 0UL        , .douji = B_J             , .kana = "a"      , .func = nofunc }, // a
  {.shift = 0UL        , .douji = B_I             , .kana = "i"      , .func = nofunc }, // i
  {.shift = 0UL        , .douji = B_K             , .kana = "u"      , .func = nofunc }, // u
  {.shift = 0UL        , .douji = B_O             , .kana = "e"      , .func = nofunc }, // e
  {.shift = 0UL        , .douji = B_M             , .kana = "o"      , .func = nofunc }, // o
  {.shift = 0UL        , .douji = B_F             , .kana = "k"      , .func = nofunc }, // k
  {.shift = 0UL        , .douji = B_R             , .kana = "s"      , .func = nofunc }, // s
  {.shift = 0UL        , .douji = B_E             , .kana = "t"      , .func = nofunc }, // t
  {.shift = 0UL        , .douji = B_V             , .kana = "n"      , .func = nofunc }, // n
  {.shift = 0UL        , .douji = B_C             , .kana = "h"      , .func = nofunc }, // h
  {.shift = B_SHFT     , .douji = B_W             , .kana = "f"      , .func = nofunc }, // f
  {.shift = 0UL        , .douji = B_D|B_F         , .kana = "m"      , .func = nofunc }, // m
  {.shift = 0UL        , .douji = B_L             , .kana = "y"      , .func = nofunc }, // y
  {.shift = 0UL        , .douji = B_D             , .kana = "r"      , .func = nofunc }, // r
  {.shift = 0UL        , .douji = B_C|B_V         , .kana = "w"      , .func = nofunc }, // w
  {.shift = 0UL        , .douji = B_COMM          , .kana = "nn"     , .func = nofunc }, // ん
  // 記号
  {.shift = 0UL        , .douji = B_DOT           , .kana = "-"      , .func = nofunc }, // ー
  {.shift = 0UL        , .douji = B_S             , .kana = "kakko"  , .func = nofunc }, // かっこ
  {.shift = 0UL        , .douji = B_Z             , .kana = "ya"     , .func = nofunc }, // や 単語登録して
  {.shift = 0UL        , .douji = B_X             , .kana = "?"      , .func = nofunc }, // ？
  {.shift = B_SHFT     , .douji = B_X             , .kana = "!"      , .func = nofunc }, // ！
  {.shift = 0UL        , .douji = B_SCLN          , .kana = "/"      , .func = nofunc }, // /
  {.shift = 0UL        , .douji = B_SLSH          , .kana = ":"      , .func = nofunc }, // :
  {.shift = B_SHFT     , .douji = B_SLSH          , .kana = ";"      , .func = nofunc }, // ;
  // 濁音 
  {.shift = B_SHFT     , .douji = B_F             , .kana = "g"      , .func = nofunc }, // g
  {.shift = B_SHFT     , .douji = B_R             , .kana = "z"      , .func = nofunc }, // z
  {.shift = B_SHFT     , .douji = B_E             , .kana = "d"      , .func = nofunc }, // d
  {.shift = B_SHFT     , .douji = B_C             , .kana = "b"      , .func = nofunc }, // b
  {.shift = B_SHFT     , .douji = B_C|B_V         , .kana = "v"      , .func = nofunc }, // v
  // 半濁音
  {.shift = 0UL        , .douji = B_W             , .kana = "p"      , .func = nofunc }, // p
  // 小書き
  {.shift = B_SHFT     , .douji = B_U             , .kana = "x"      , .func = nofunc }, // x
  {.shift = 0UL        , .douji = B_Q             , .kana = "xtu"    , .func = nofunc }, // っ
 
  // 清音外来音 濁音外来音
  {.shift = 0UL        , .douji = B_E|B_L|B_I     , .kana = "thi"    , .func = nofunc }, // てぃ tyi
  {.shift = 0UL        , .douji = B_E|B_U|B_I     , .kana = "thi"    , .func = nofunc }, // てぃ txi
  {.shift = B_SHFT     , .douji = B_E|B_L|B_I     , .kana = "dhi"    , .func = nofunc }, // でぃ dyi
  {.shift = B_SHFT     , .douji = B_E|B_U|B_I     , .kana = "dhi"    , .func = nofunc }, // でぃ dxi
  {.shift = 0UL        , .douji = B_E|B_U|B_K     , .kana = "twu"    , .func = nofunc }, // とぅ txu
  {.shift = B_SHFT     , .douji = B_E|B_U|B_K     , .kana = "dwu"    , .func = nofunc }, // どぅ dxu
  {.shift = B_SHFT     , .douji = B_E|B_L|B_K     , .kana = "dhu"    , .func = nofunc }, // でゅ dyu
  {.shift = B_C|B_V    , .douji = B_L|B_M         , .kana = "who"    , .func = nofunc }, // うぉ wyo

  // 追加
  {.shift = 0UL        , .douji = B_SHFT          , .kana = " "      , .func = nofunc },
  {.shift = 0UL        , .douji = B_G             , .kana = ","SS_TAP(X_ENTER)  , .func = nofunc },
  {.shift = 0UL        , .douji = B_H             , .kana = "."SS_TAP(X_ENTER)  , .func = nofunc },
  {.shift = 0UL        , .douji = B_U             , .kana = SS_TAP(X_BACKSPACE) , .func = nofunc },

  // enter
  {.shift = 0UL        , .douji = B_V|B_M         , .kana = SS_TAP(X_ENTER)  , .func = nofunc }, // enter
  // enter+シフト(連続シフト)
  {.shift = B_SHFT     , .douji = B_V|B_M         , .kana = SS_TAP(X_ENTER)  , .func = nofunc }, // enter+シフト(連続シフト)

  {.shift = 0UL        , .douji = B_H|B_J         , .kana = ""       , .func = naginata_on }, //　かなオン
  {.shift = 0UL        , .douji = B_F|B_G         , .kana = ""       , .func = naginata_off }, //　かなオフ

  // 編集モード
  /*
  {.shift = B_D|B_F    , .douji = B_Y             , .kana = ""       , .func = ngh_DFY }, // {Home}
  {.shift = B_D|B_F    , .douji = B_U             , .kana = ""       , .func = ngh_DFU }, // +{End}{BS}
  {.shift = B_D|B_F    , .douji = B_I             , .kana = ""       , .func = ngh_DFI }, // {vk1Csc079}
  {.shift = B_D|B_F    , .douji = B_O             , .kana = ""       , .func = ngh_DFO }, // {Del}
  {.shift = B_D|B_F    , .douji = B_P             , .kana = ""       , .func = ngh_DFP }, // {Esc 3}
  {.shift = B_D|B_F    , .douji = B_H             , .kana = ""       , .func = ngh_DFH }, // {Enter}{End}
  {.shift = B_D|B_F    , .douji = B_J             , .kana = ""       , .func = ngh_DFJ }, // {↑}
  {.shift = B_D|B_F    , .douji = B_K             , .kana = ""       , .func = ngh_DFK }, // +{↑}
  {.shift = B_D|B_F    , .douji = B_L             , .kana = ""       , .func = ngh_DFL }, // +{↑ 7}
  {.shift = B_D|B_F    , .douji = B_SCLN          , .kana = ""       , .func = ngh_DFSCLN }, // ^i
  {.shift = B_D|B_F    , .douji = B_N             , .kana = ""       , .func = ngh_DFN }, // {End}
  {.shift = B_D|B_F    , .douji = B_M             , .kana = ""       , .func = ngh_DFM }, // {↓}
  {.shift = B_D|B_F    , .douji = B_COMM          , .kana = ""       , .func = ngh_DFCOMM }, // +{↓}
  {.shift = B_D|B_F    , .douji = B_DOT           , .kana = ""       , .func = ngh_DFDOT }, // +{↓ 7}
  {.shift = B_D|B_F    , .douji = B_SLSH          , .kana = ""       , .func = ngh_DFSLSH }, // ^u

  {.shift = B_J|B_K    , .douji = B_Q             , .kana = ""       , .func = ngh_JKQ }, // ^{End}
  {.shift = B_J|B_K    , .douji = B_W             , .kana = ""       , .func = ngh_JKW }, // 『』{改行}{↑}
  {.shift = B_J|B_K    , .douji = B_E             , .kana = "dhi"    , .func = nofunc  }, // ディ
  {.shift = B_J|B_K    , .douji = B_R             , .kana = ""       , .func = ngh_JKR }, // ^s
  {.shift = B_J|B_K    , .douji = B_T             , .kana = ""       , .func = ngh_JKT }, // ・
  {.shift = B_J|B_K    , .douji = B_A             , .kana = ""       , .func = ngh_JKA }, // ……{改行}
  {.shift = B_J|B_K    , .douji = B_S             , .kana = ""       , .func = ngh_JKS }, // (){改行}{↑}
  {.shift = B_J|B_K    , .douji = B_D             , .kana = ""       , .func = ngh_JKD }, // ？{改行}
  {.shift = B_J|B_K    , .douji = B_F             , .kana = ""       , .func = ngh_JKF }, // 「」{改行}{↑}
  {.shift = B_J|B_K    , .douji = B_G             , .kana = ""       , .func = ngh_JKG }, // 《》{改行}{↑}
  {.shift = B_J|B_K    , .douji = B_Z             , .kana = ""       , .func = ngh_JKZ }, // ――{改行}
  {.shift = B_J|B_K    , .douji = B_X             , .kana = ""       , .func = ngh_JKX }, // 【】{改行}{↑}
  {.shift = B_J|B_K    , .douji = B_C             , .kana = ""       , .func = ngh_JKC }, // ！{改行}
  {.shift = B_J|B_K    , .douji = B_V             , .kana = ""       , .func = ngh_JKV }, // {改行}{↓}
  {.shift = B_J|B_K    , .douji = B_B             , .kana = ""       , .func = ngh_JKB }, // {改行}{←}
*/

 /*
  {.shift = B_C|B_V    , .douji = B_Y             , .kana = ""       , .func = ngh_CVY }, // +{Home}
  {.shift = B_C|B_V    , .douji = B_U             , .kana = ""       , .func = ngh_CVU }, // ^x
  {.shift = B_C|B_V    , .douji = B_I             , .kana = ""       , .func = ngh_CVI }, // ^v
  {.shift = B_C|B_V    , .douji = B_O             , .kana = ""       , .func = ngh_CVO }, // ^y
  {.shift = B_C|B_V    , .douji = B_P             , .kana = ""       , .func = ngh_CVP }, // ^z
  {.shift = B_C|B_V    , .douji = B_H             , .kana = ""       , .func = ngh_CVH }, // ^c
  {.shift = B_C|B_V    , .douji = B_J             , .kana = ""       , .func = ngh_CVJ }, // {→}
  {.shift = B_C|B_V    , .douji = B_K             , .kana = ""       , .func = ngh_CVK }, // +{→}
  {.shift = B_C|B_V    , .douji = B_L             , .kana = ""       , .func = ngh_CVL }, // +{→ 5}
  {.shift = B_C|B_V    , .douji = B_SCLN          , .kana = ""       , .func = ngh_CVSCLN }, // +{→ 20}
  {.shift = B_C|B_V    , .douji = B_N             , .kana = ""       , .func = ngh_CVN }, // +{End}
  {.shift = B_C|B_V    , .douji = B_M             , .kana = ""       , .func = ngh_CVM }, // {←}
  {.shift = B_C|B_V    , .douji = B_COMM          , .kana = ""       , .func = ngh_CVCOMM }, // +{←}
  {.shift = B_C|B_V    , .douji = B_DOT           , .kana = ""       , .func = ngh_CVDOT }, // +{← 5}
  {.shift = B_C|B_V    , .douji = B_SLSH          , .kana = ""       , .func = ngh_CVSLSH }, // +{← 20}

  {.shift = B_M|B_COMM , .douji = B_Q             , .kana = ""       , .func = ngh_MCQ }, // {Home}{→}{End}{Del 4}{←}
  {.shift = B_M|B_COMM , .douji = B_W             , .kana = ""       , .func = ngh_MCW }, // ^x『^v』{改行}{Space}+{↑}^x
  {.shift = B_M|B_COMM , .douji = B_E             , .kana = ""       , .func = ngh_MCE }, // {Home}{改行}{Space 3}{←}
  {.shift = B_M|B_COMM , .douji = B_R             , .kana = ""       , .func = ngh_MCR }, // {Space 3}
  {.shift = B_M|B_COMM , .douji = B_T             , .kana = ""       , .func = ngh_MCT }, // 〇{改行}
  {.shift = B_M|B_COMM , .douji = B_A             , .kana = ""       , .func = ngh_MCA }, // {Home}{→}{End}{Del 2}{←}
  {.shift = B_M|B_COMM , .douji = B_S             , .kana = ""       , .func = ngh_MCS }, // ^x(^v){改行}{Space}+{↑}^x
  {.shift = B_M|B_COMM , .douji = B_D             , .kana = ""       , .func = ngh_MCD }, // {Home}{改行}{Space 1}{←}
  {.shift = B_M|B_COMM , .douji = B_F             , .kana = ""       , .func = ngh_MCF }, // ^x「^v」{改行}{Space}+{↑}^x
  {.shift = B_M|B_COMM , .douji = B_G             , .kana = ""       , .func = ngh_MCG }, // ^x｜{改行}^v《》{改行}{↑}{Space}+{↑}^x
  {.shift = B_M|B_COMM , .douji = B_Z             , .kana = ""       , .func = ngh_MCZ }, // 　　　×　　　×　　　×{改行 2}
  {.shift = B_M|B_COMM , .douji = B_X             , .kana = ""       , .func = ngh_MCX }, // ^x【^v】{改行}{Space}+{↑}^x
  {.shift = B_M|B_COMM , .douji = B_C             , .kana = ""       , .func = ngh_MCC }, // ／{改行}
  {.shift = B_M|B_COMM , .douji = B_V             , .kana = ""       , .func = ngh_MCV }, // {改行}{End}{改行}「」{改行}{↑}
  {.shift = B_M|B_COMM , .douji = B_B             , .kana = ""       , .func = ngh_MCB }, // {改行}{End}{改行}{Space}
*/

};

// 薙刀式のレイヤー、オンオフするキー
void set_naginata(uint8_t layer, uint16_t *onk, uint16_t *offk) {
  naginata_layer = layer;
  ngon_keys[0] = *onk;
  ngon_keys[1] = *(onk+1);
  ngoff_keys[0] = *offk;
  ngoff_keys[1] = *(offk+1);

  clear_nginput();

  naginata_config.raw = eeconfig_read_user();
  if (naginata_config.os != NG_WIN && naginata_config.os != NG_MAC && naginata_config.os != NG_LINUX) {
    naginata_config.os = NG_WIN;
    // naginata_config.live_conv = 1;
    naginata_config.tategaki = 1;
    // naginata_config.kouchi_shift = 0;
    eeconfig_update_user(naginata_config.raw);
  }
  ng_set_unicode_mode(naginata_config.os);
}

// 薙刀式をオン
void naginata_on(void) {
  is_naginata = true;
  naginata_clear();
  layer_on(naginata_layer);

  tap_code(KC_LANGUAGE_1); // Mac
  tap_code(KC_INTERNATIONAL_4); // Win
}

// 薙刀式をオフ
void naginata_off(void) {
  is_naginata = false;
  naginata_clear();
  layer_off(naginata_layer);

  tap_code(KC_LANGUAGE_2); // Mac
  tap_code(KC_INTERNATIONAL_5); // Win
}

// 薙刀式のon/off状態を返す
bool naginata_state(void) {
  return is_naginata;
}

void switchOS(uint8_t os) {
  naginata_config.os = os;
  eeconfig_update_user(naginata_config.raw);
  ng_set_unicode_mode(naginata_config.os);
}

void ng_set_unicode_mode(uint8_t os) {
  switch (os) {
    case NG_WIN:
      set_unicode_input_mode(UNICODE_MODE_WINCOMPOSE);
      break;
    case NG_MAC:
      set_unicode_input_mode(UNICODE_MODE_MACOS);
      break;
    case NG_LINUX:
      set_unicode_input_mode(UNICODE_MODE_LINUX);
      break;
  }
}

// void mac_live_conversion_toggle() {
//   naginata_config.live_conv ^= 1;
//   eeconfig_update_user(naginata_config.raw);
// }

void tategaki_toggle() {
  naginata_config.tategaki ^= 1;
  eeconfig_update_user(naginata_config.raw);
}

// void kouchi_shift_toggle() {
//   naginata_config.kouchi_shift ^= 1;
//   eeconfig_update_user(naginata_config.raw);
// }

void ng_show_os(void) {
  switch (naginata_config.os) {
    case NG_WIN:
      send_string("win");
      break;
    case NG_MAC:
      send_string("mac");
      // if (naginata_config.live_conv) {
      //   send_string("/:lc");
      // } else {
      //   send_string("/-lc");
      // }
      break;
    case NG_LINUX:
      send_string("linux");
      break;
  }
  if (naginata_config.tategaki) {
    send_string("/tate");
  } else {
    send_string("/yoko");
  }
  // if (naginata_config.kouchi_shift) {
  //   send_string("/:kouchi");
  // } else {
  //   send_string("/-kouchi");
  // }
}

#define MAX_STRLEN 40
void ng_send_unicode_string_P(const char *pstr) {
  if (strlen_P(pstr) > MAX_STRLEN) return;
  char str[MAX_STRLEN];
  strcpy_P(str, pstr);

  switch (naginata_config.os) {
    case NG_LINUX:
      tap_code(KC_INTERNATIONAL_5);
      send_unicode_string(str);
      tap_code(KC_INTERNATIONAL_4);
      break;
    case NG_WIN:
      send_unicode_string(str);
      tap_code(KC_ENT);
      break;
    case NG_MAC: // Karabiner-Elementsが必要
      tap_code(KC_LANGUAGE_2); // 未確定文字を確定する
      wait_ms(50);
      register_code(KC_LCTL); // Unicode HEX Inputへ切り替え
      wait_ms(50);
      tap_code(KC_F20);
      wait_ms(50);
      unregister_code(KC_LCTL);
      wait_ms(50);
      send_unicode_string(str);
      wait_ms(50);
      register_code(KC_LSFT); // 日本語入力へ切り替え。再変換にならないように「shift+かな」「かな」の2打にする。
      wait_ms(50);
      tap_code(KC_LANGUAGE_1);
      wait_ms(50);
      unregister_code(KC_LSFT);
      wait_ms(50);
      tap_code(KC_LANGUAGE_1);
      break;
  }
}

// modifierが押されたら薙刀式レイヤーをオフしてベースレイヤーに戻す
// get_mods()がうまく動かない
static int n_modifier = 0;

bool process_modifier(uint16_t keycode, keyrecord_t *record) {
  if (IS_MODIFIER_KEYCODE(keycode) || IS_QK_MOD_TAP(keycode)) {
    if (record->event.pressed) {
      n_modifier++;
      layer_off(naginata_layer);
    } else {
      n_modifier--;
      if (n_modifier <= 0) {
        n_modifier = 0;
        layer_on(naginata_layer);
      }
    }
    return true;
  }
  return false;
}

static uint16_t fghj_buf = 0; // 押しているJかKのキーコード
static uint8_t nkeypress = 0; // 同時にキーを押している数

// 薙刀式の起動処理(容量が大きいCOMBOを使わない)
bool enable_naginata(uint16_t keycode, keyrecord_t *record) {
  // キープレス
  if (record->event.pressed) {
    nkeypress++;
    // 1キー目、JKの前に他のキーを押していないこと
    if (fghj_buf == 0 && nkeypress == 1) {
      // かなオンキーの場合
      if (keycode == ngon_keys[0] || keycode == ngon_keys[1] || keycode == ngoff_keys[0] || keycode == ngoff_keys[1]) {
        fghj_buf = keycode;
        return false;
      }
    // ２キー目
    } else {
      // ２キー目、１キー目、両方ともかなオンキー
      if ((keycode == ngon_keys[0] && fghj_buf == ngon_keys[1]) ||
          (keycode == ngon_keys[1] && fghj_buf == ngon_keys[0])) {
        naginata_on();
        fghj_buf = 0;
        nkeypress = 0;
        return false;
      } else if ((keycode == ngoff_keys[0] && fghj_buf == ngoff_keys[1]) ||
          (keycode == ngoff_keys[1] && fghj_buf == ngoff_keys[0])) {
        naginata_off();
        fghj_buf = 0;
        nkeypress = 0;
        return false;
      // ２キー目はかなオンキーではない
      } else {
        tap_code(fghj_buf); // 1キー目を出力
        fghj_buf = 0;
        nkeypress = 0;
        return true; // 2キー目はQMKにまかせる
      }
    }
  } else {
    nkeypress = 0;
    // J/K単押しだった
    if (fghj_buf > 0) {
      tap_code(fghj_buf);
      fghj_buf = 0;

      // Shift + Jで、先にShiftを外した場合にShiftがリリースされない不具合対策
      if (IS_MODIFIER_KEYCODE(keycode)) {
        unregister_code(keycode);
      } else if (IS_QK_MOD_TAP(keycode)) {
        if (keycode & (MOD_LCTL << 8))
          unregister_code(KC_LEFT_CTRL);
        if (keycode & (MOD_LSFT << 8))
          unregister_code(KC_LEFT_SHIFT);
        if (keycode & (MOD_LALT << 8))
          unregister_code(KC_LEFT_ALT);
        if (keycode & (MOD_LGUI << 8))
          unregister_code(KC_LEFT_GUI);
        if (keycode & (MOD_RCTL << 8))
          unregister_code(KC_RIGHT_CTRL);
        if (keycode & (MOD_RSFT << 8))
          unregister_code(KC_RIGHT_SHIFT);
        if (keycode & (MOD_RALT << 8))
          unregister_code(KC_RIGHT_ALT);
        if (keycode & (MOD_RGUI << 8))
          unregister_code(KC_RIGHT_GUI);
      }
      return false;
    }
  }

  fghj_buf = 0;
  return true;
}

// バッファをクリアする
void naginata_clear(void) {
  n_modifier = 0;
  nkeypress = 0;
  fghj_buf = 0;
  pressed_keys = 0;
  n_pressed_keys = 0;
  
  clear_nginput();
}

void clear_nginput() {
  for (int i = 0; i < MX; i++) {
    for (int j = 0; j < MY; j++) {
      nginput[i][j] = 0;
    }
    iy_nginput[i] = -1;
  }
  ix_nginput = -1;
}

// 薙刀式の入力処理
bool process_naginata(uint16_t keycode, keyrecord_t *record) {

  if (record->event.pressed) {
    n_pressed_keys++;
  } else {
    if (n_pressed_keys > 0) {
      n_pressed_keys--;
    }
  }
  if (n_pressed_keys == 0)
    pressed_keys = 0;

  #if defined(CONSOLE_ENABLE)
      uprintf(">process_naginata pressed_keys=%lu, %d\n", pressed_keys, n_pressed_keys);
  #endif

  // まれに薙刀モードオンのまま、レイヤーがオフになることがあるので、対策
  if (n_modifier == 0 && is_naginata && !layer_state_is(naginata_layer))
    layer_on(naginata_layer);
  if (n_modifier == 0 && !is_naginata && layer_state_is(naginata_layer))
    layer_off(naginata_layer);
  if (n_modifier > 0 && layer_state_is(naginata_layer))
    layer_off(naginata_layer);

  // OS切り替え(UNICODE出力)
  if (record->event.pressed) {
    switch (keycode) {
      // case NG_ON:
      //   naginata_on();
      //   return false;
      //   break;
      // case NG_OFF:
      //   naginata_off();
      //   return false;
      //   break;
      // case NG_CLR:
      //   naginata_clear();
      //   return false;
      //   break;
      case NGSW_WIN:
        switchOS(NG_WIN);
        return false;
        break;
      case NGSW_MAC:
        switchOS(NG_MAC);
        return false;
        break;
      case NGSW_LNX:
        switchOS(NG_LINUX);
        return false;
        break;
      // case NG_MLV:
      //   mac_live_conversion_toggle();
      //   return false;
      //   break;
      case NG_SHOS:
        ng_show_os();
        return false;
        break;
      case NG_TAYO:
        tategaki_toggle();
        return false;
        break;
      // case NG_KOTI:
      //   kouchi_shift_toggle();
      //   return false;
      //   break;
    }
  }

  if (!is_naginata)
    // return true;
    return enable_naginata(keycode, record);

  if (process_modifier(keycode, record))
    return true;

  if (keycode >= NG_Q && keycode <= NG_SHFT2) {
    if (record->event.pressed) {
      #if defined(CONSOLE_ENABLE)
          uprintf(">process_naginata pressed=%u ix_nginput=%u\n", keycode, ix_nginput);
      #endif

      pressed_keys |= ng_key[keycode - NG_Q]; // キーの重ね合わせ

      if (ix_nginput == -1 || keycode == NG_SHFT || keycode == NG_SHFT2) {
        ix_nginput++;
        iy_nginput[ix_nginput]++;
        nginput[ix_nginput][iy_nginput[ix_nginput]] = keycode;

      } else {
        uint16_t tmp[MY];
        for (int i = 0; i <= iy_nginput[ix_nginput]; i++) {
          tmp[i] = nginput[ix_nginput][i];
        }
        tmp[iy_nginput[ix_nginput] + 1] = keycode;

        // 前のキーとの同時押しの可能性があるなら前に足す
        // 同じキー連打を除外
        if (nginput[ix_nginput][iy_nginput[ix_nginput]] != keycode && number_of_candidates(tmp, iy_nginput[ix_nginput] + 2, false) > 0) {
          iy_nginput[ix_nginput]++;
          nginput[ix_nginput][iy_nginput[ix_nginput]] = keycode;
          // ix_nginput++;
        // 前のキーと同時押しはない
        } else {
          // 連続シフトではない
          ix_nginput++;
          iy_nginput[ix_nginput]++;
          nginput[ix_nginput][iy_nginput[ix_nginput]] = keycode;
        }
      }
      
      // 連続シフト
      static const uint16_t rs[10][2] = {{NG_D, NG_F}, {NG_C, NG_V}, {NG_J, NG_K}, {NG_M, NG_COMM}, {NG_SHFT, 0}, {NG_SHFT2, 0}, {NG_F, 0}, {NG_V, 0}, {NG_J, 0}, {NG_M, 0}};

      for (int i = 0; i < 10; i++) {
        int c = -1;
        if (keycode == rs[i][0] || keycode == rs[i][1]) c = 1;

        uint32_t brs = 0UL;
        for (int j = 0; j < 2; j++) {
          if (rs[i][j] > 0)
            brs |=  ng_key[rs[i][j] - NG_Q];
        }

        uint16_t rskc[] = {0, 0, 0, 0};
        int p = 0;
        for (; p < 2; p++) {
          if (rs[i][p] > 0) {
            rskc[p] = rs[i][p];            
          } else {
            break;
          }
        }
        for (int j = 0; j < MY; j++) {
          if (nginput[ix_nginput][j] > 0) {
            rskc[p++] = nginput[ix_nginput][j];
          } else {
            break;
          }
        }

        if (c <  0 && ((brs & pressed_keys) == brs) && number_of_candidates(rskc, p, true) >  0) {
          for (int i = 0; i < p; i++) {
            nginput[ix_nginput - 1][i] = rskc[i];
          }
          iy_nginput[ix_nginput] = p;
          for (int j = 0; j < MY; j++) {
            if (rskc[j] > 0) {
              nginput[ix_nginput][j] = rskc[j];
            } else {
              iy_nginput[ix_nginput] = j - 1;
              break;
            }
          }
          break;
        }
      }

      if (ix_nginput > 1 || number_of_candidates(nginput[0], iy_nginput[0] + 1, false) == 1) {
        ng_type(nginput[0], iy_nginput[ix_nginput] + 1);
        for (int i = 0; i <= ix_nginput; i++) {
          for (int j = 0; j < MY; j++) {
            nginput[i][j] = nginput[i + 1][j];
          }
          iy_nginput[i] = iy_nginput[i + 1];
        }
        ix_nginput--;
      }

      #if defined(CONSOLE_ENABLE)
        uprintf("<process_naginata pressed=%u ix_nginput=%u\n", keycode, ix_nginput);
      #endif

    } else { // key release
      #if defined(CONSOLE_ENABLE)
          uprintf(">process_naginata released=%u ix_nginput=%u\n", keycode, ix_nginput);
      #endif
      pressed_keys &= ~ng_key[keycode - NG_Q]; // キーの重ね合わせ

      if (pressed_keys == 0UL) {
        for (int i = 0; i <= ix_nginput; i++) {
          ng_type(nginput[i], iy_nginput[i] + 1);
        }
        clear_nginput();
      }

      #if defined(CONSOLE_ENABLE)
          uprintf("<process_naginata released=%u ix_nginput=%u\n", keycode, ix_nginput);
      #endif
    }
    return false;
  }

  return true;
}

// キー入力を文字に変換して出力する
void ng_type(uint16_t *keys, int size) {
  #if defined(CONSOLE_ENABLE)
    uprintf(">ng_type size=%u\n", size);
  #endif
  if (size < 1) return;

  naginata_kanamap bngdickana;

  if (size == 1 && *keys == NG_SHFT2) {
    tap_code16(KC_ENT);
    return;
  }

  // bool ftype = false;
  uint32_t keyset = 0UL;
  for (int i = 0; i < size; i++) {
    keyset |= ng_key[*(keys + i) - NG_Q];
  }
  for (int i = 0; i < sizeof ngdickana / sizeof bngdickana; i++) {
    memcpy_P(&bngdickana, &ngdickana[i], sizeof(bngdickana));
    if ((bngdickana.shift | bngdickana.douji) == keyset) {
      if (bngdickana.kana[0] > 0) {
        send_string(bngdickana.kana);
      } else {
        bngdickana.func();
      }
      // ftype = true;
      break;
    }
  }
  // JIみたいにJIを含む同時押しはたくさんあるが、JIのみの同時押しがないとき
  // 最後の１キーを別に分けて変換する
  // if (!ftype) {
  //   uint16_t a[MY], b[MY];
  //   for (int i = 0; i < size - 1; i++) {
  //     a[i] = *(keys + i);
  //   }
  //   b[0] = *(keys + size - 1);
  //   ng_type(a, size - 1);
  //   ng_type(b, 1);
  // }

  #if defined(CONSOLE_ENABLE)
    uprintf("<ng_type\n");
  #endif
}

bool compareList0(uint16_t *keys, uint16_t a) {
    if (*keys == a)
        return true;
    else
        return false;
}

bool compareList01(uint16_t *keys, uint16_t a, uint16_t b) {
    if ((*keys == a && *(keys+1) == b) || (*keys == b && *(keys+1) == a))
        return true;
    else
        return false;
}

int number_of_candidates(uint16_t *keys, int size, bool strict) {
  #if defined(CONSOLE_ENABLE)
    uprintf(">number_of_candidates\n");
  #endif
  if (size == 0) return 0;

  naginata_kanamap bngdickana; // PROGMEM buffer
  int noc = 0;
  static const uint16_t hrs[4][2] = {{NG_D, NG_F}, {NG_C, NG_V}, {NG_J, NG_K}, {NG_M, NG_COMM}};

  if (strict) { // 完全一致
    // シフトの単打
    if (size == 1 && (compareList0(keys, NG_SHFT) || compareList0(keys, NG_SHFT2))) {
      #if defined(CONSOLE_ENABLE)
      uprintf("<number_of_candidates noc=%u\n", 1);
      #endif
      return 1;
    }
    // シフト
    if (size > 1 && (compareList0(keys, NG_SHFT) || compareList0(keys, NG_SHFT2))) {
      uint32_t keyset = 0UL;
      for (int i = 1; i < size; i++) {
        keyset |= ng_key[*(keys + i) - NG_Q];
      }
      for (int i = 0; i < sizeof ngdickana / sizeof bngdickana; i++) {
        memcpy_P(&bngdickana, &ngdickana[i], sizeof(bngdickana));
        if (bngdickana.shift == B_SHFT && bngdickana.douji == keyset) {
          noc++;
          if (noc > 1) break;
        }
      }
      #if defined(CONSOLE_ENABLE)
      uprintf("<number_of_candidates noc=%u\n", noc);
      #endif
      return noc;
    }
    // 編集モード
    for (int j = 0; j < 4; j++) {
      if (size == 3 && compareList01(keys, hrs[j][0], hrs[j][1])) {
        for (int i = 0; i < sizeof ngdickana / sizeof bngdickana; i++) {
          memcpy_P(&bngdickana, &ngdickana[i], sizeof(bngdickana));
          if (bngdickana.shift == (ng_key[hrs[j][0] - NG_Q] | ng_key[hrs[j][1] - NG_Q]) && bngdickana.douji == ng_key[*(keys + 2) - NG_Q]) {
            #if defined(CONSOLE_ENABLE)
            uprintf("<number_of_candidates noc=%u\n", 1);
            #endif
            return 1;
          }
        }
      }
    }
    // 同時押し、単打
    uint32_t keyset = 0UL;
    for (int i = 0; i < size; i++) {
      keyset |= ng_key[*(keys + i) - NG_Q];
    }
    for (int i = 0; i < sizeof ngdickana / sizeof bngdickana; i++) {
      memcpy_P(&bngdickana, &ngdickana[i], sizeof(bngdickana));
      if (bngdickana.shift == 0UL && bngdickana.douji == keyset) {
        noc++;
        if (noc > 1) break;
      }
    }
    #if defined(CONSOLE_ENABLE)
    uprintf("<number_of_candidates noc=%u\n", noc);
    #endif
    return noc;

  } else { // 部分一致
    // シフトの単打
    if (size == 1 && (compareList0(keys, NG_SHFT) || compareList0(keys, NG_SHFT2))) {
      #if defined(CONSOLE_ENABLE)
      uprintf("<number_of_candidates noc=%u\n", 2);
      #endif
      return 2;
    }
    // 編集モードの途中
    if (size == 2 && (compareList01(keys, NG_D, NG_F) || compareList01(keys, NG_C, NG_V) || compareList01(keys, NG_J, NG_K) || compareList01(keys, NG_M, NG_COMM))) {
      #if defined(CONSOLE_ENABLE)
      uprintf("<number_of_candidates noc=%u\n", 2);
      #endif
      return 2;
    }
    // シフトの途中
    if (size > 1 && (compareList0(keys, NG_SHFT) || compareList0(keys, NG_SHFT2))) {
      uint32_t keyset = 0UL;
      for (int i = 1; i < size; i++) {
        keyset |= ng_key[*(keys + i) - NG_Q];
      }
      for (int i = 0; i < sizeof ngdickana / sizeof bngdickana; i++) {
        memcpy_P(&bngdickana, &ngdickana[i], sizeof(bngdickana));
        if (bngdickana.shift == B_SHFT && ((bngdickana.douji & keyset) == keyset)) {
          noc++;
          if (noc > 1) break;
        }
      }
      #if defined(CONSOLE_ENABLE)
      uprintf("<number_of_candidates noc=%u\n", noc);
      #endif
      return noc;
    }
    // 編集モード
    for (int j = 0; j < 4; j++) {
      if (size == 3 && compareList01(keys, hrs[j][0], hrs[j][1])) {
        for (int i = 0; i < sizeof ngdickana / sizeof bngdickana; i++) {
          memcpy_P(&bngdickana, &ngdickana[i], sizeof(bngdickana));
          if (bngdickana.shift == (ng_key[hrs[j][0] - NG_Q] | ng_key[hrs[j][1] - NG_Q]) && bngdickana.douji == ng_key[*(keys + 2) - NG_Q]) {
            #if defined(CONSOLE_ENABLE)
            uprintf("<number_of_candidates noc=%u\n", 1);
            #endif
            return 1;
          }
        }
      }
    }
    // 同時押し、単打
    uint32_t keyset = 0UL;
    for (int i = 0; i < size; i++) {
      keyset |= ng_key[*(keys + i) - NG_Q];
    }
    for (int i = 0; i < sizeof ngdickana / sizeof bngdickana; i++) {
      memcpy_P(&bngdickana, &ngdickana[i], sizeof(bngdickana));
      if (bngdickana.shift == 0UL && ((bngdickana.douji & keyset) == keyset)) {
        noc++;
        if (noc > 1) break;
      }
    }
    #if defined(CONSOLE_ENABLE)
      uprintf("<number_of_candidates noc=%u\n", noc);
    #endif
    return noc;
  }

  #if defined(CONSOLE_ENABLE)
    uprintf("<number_of_candidates noc=%u\n", noc);
  #endif

  return noc;
}

void nofunc() {}

void ng_T() {
  ng_left(1);
}

void ng_Y() {
  ng_right(1);
}

void ng_ST() {
  register_code(KC_LSFT);
  ng_left(1);
  unregister_code(KC_LSFT);
}

void ng_SY() {
  register_code(KC_LSFT);
  ng_right(1);
  unregister_code(KC_LSFT);
}

/*
void ngh_JKQ() { // ^{End}
  ng_eof();
}

void ngh_JKW() { // 『』{改行}{↑}
  ng_send_unicode_string_P(PSTR("『』"));
  ng_up(1);
}

// void ngh_JKE() { // ディ
//   send_string("dhi");
// }

void ngh_JKR() { // ^s
  ng_save();
}

void ngh_JKT() { // ・
  ng_send_unicode_string_P(PSTR("・"));
}

void ngh_JKA() { // ……{改行}
  ng_send_unicode_string_P(PSTR("……"));
}

void ngh_JKS() { // (){改行}{↑}
  ng_send_unicode_string_P(PSTR("()"));
  ng_up(1);
}

void ngh_JKD() { // ？{改行}
  tap_code16(LSFT(KC_SLSH));
  tap_code16(KC_ENT);
}

void ngh_JKF() { // 「」{改行}{↑}
  ng_send_unicode_string_P(PSTR("「」"));
  ng_up(1);
}

void ngh_JKG() { // 《》{改行}{↑}
  ng_send_unicode_string_P(PSTR("《》"));
  ng_up(1);
}

void ngh_JKZ() { // ――{改行}
  ng_send_unicode_string_P(PSTR("――"));
}

void ngh_JKX() { // 【】{改行}{↑}
  ng_send_unicode_string_P(PSTR("【】"));
  ng_up(1);
}

void ngh_JKC() { // ！{改行}
  tap_code16(LSFT(KC_1));
  tap_code16(KC_ENT);
}

void ngh_JKV() { // {改行}{↓}
  tap_code(KC_ENT);
  ng_down(1);
}

void ngh_JKB() { // {改行}{←}
  tap_code(KC_ENT);
  ng_left(1);
}
*/

/*
void ngh_DFY() { // {Home}
  ng_home();
}

void ngh_DFU() { // +{End}{BS}
  register_code(KC_LSFT);
  ng_end();
  unregister_code(KC_LSFT);
  tap_code(KC_BSPC);
}

void ngh_DFI() { // {vk1Csc079}
  ng_saihenkan();
}

void ngh_DFO() { // {Del}
  tap_code(KC_DEL);
}

void ngh_DFP() { // {Esc 3}
  tap_code(KC_ESC);
  tap_code(KC_ESC);
  tap_code(KC_ESC);
}

void ngh_DFH() { // {Enter}{End}
  tap_code(KC_ENT);
  ng_end();
}

void ngh_DFJ() { // {↑}
  ng_up(1);
}

void ngh_DFK() { // +{↑}
  register_code(KC_LSFT);
  ng_up(1);
  unregister_code(KC_LSFT);
}

void ngh_DFL() { // +{↑ 7}
  register_code(KC_LSFT);
  ng_up(7);
  unregister_code(KC_LSFT);
}

void ngh_DFSCLN() { // ^i
  ng_katakana();
}

void ngh_DFN() { // {End}
  ng_end();
}

void ngh_DFM() { // {↓}
  ng_down(1);
}

void ngh_DFCOMM() { // +{↓}
  register_code(KC_LSFT);
  ng_down(1);
  unregister_code(KC_LSFT);
}

void ngh_DFDOT() { // +{↓ 7}
  register_code(KC_LSFT);
  ng_down(7);
  unregister_code(KC_LSFT);
}

void ngh_DFSLSH() { // ^u
  ng_hiragana();
}
*/

/*
void ngh_MCQ() { // {Home}{→}{End}{Del 4}{←}
  ng_home();
  ng_right(1);
  ng_end();
  tap_code(KC_DEL);
  tap_code(KC_DEL);
  tap_code(KC_DEL);
  tap_code(KC_DEL);
  ng_left(1);
}

void ngh_MCW() { // ^x『^v』{改行}{Space}+{↑}^x
  ng_cut();
  ng_send_unicode_string_P(PSTR("『"));
  ng_paste();
  ng_send_unicode_string_P(PSTR("』"));
  tap_code(KC_SPC);
  register_code(KC_LSFT);
  ng_up(1);
  unregister_code(KC_LSFT);
  ng_cut();
}

void ngh_MCE() { // {Home}{改行}{Space 3}{←}
  ng_home();
  tap_code(KC_ENT);
  tap_code(KC_SPC);
  tap_code(KC_SPC);
  tap_code(KC_SPC);
  ng_left(1);
}

void ngh_MCR() { // {Space 3}
  tap_code(KC_SPC);
  tap_code(KC_SPC);
  tap_code(KC_SPC);
}

void ngh_MCT() { // 〇{改行}
  ng_send_unicode_string_P(PSTR("〇"));
}

void ngh_MCA() { // {Home}{→}{End}{Del 2}{←}
  ng_home();
  ng_right(1);
  ng_end();
  tap_code(KC_DEL);
  tap_code(KC_DEL);
  ng_left(1);
}

void ngh_MCS() { // ^x(^v){改行}{Space}+{↑}^x
  ng_cut();
  ng_send_unicode_string_P(PSTR("("));
  ng_paste();
  ng_send_unicode_string_P(PSTR(")"));
  tap_code(KC_SPC);
  register_code(KC_LSFT);
  ng_up(1);
  unregister_code(KC_LSFT);
  ng_cut();
}

void ngh_MCD() { // {Home}{改行}{Space 1}{←}
  ng_home();
  tap_code(KC_ENT);
  tap_code(KC_SPC);
  ng_left(1);
}

void ngh_MCF() { // ^x「^v」{改行}{Space}+{↑}^x
  ng_cut();
  ng_send_unicode_string_P(PSTR("「"));
  ng_paste();
  ng_send_unicode_string_P(PSTR("」"));
  tap_code(KC_SPC);
  register_code(KC_LSFT);
  ng_up(1);
  unregister_code(KC_LSFT);
  ng_cut();
}

void ngh_MCG() { // ^x｜{改行}^v《》{改行}{↑}{Space}+{↑}^x
  ng_cut();
  ng_send_unicode_string_P(PSTR("｜"));
  ng_paste();
  tap_code(KC_SPC);
  register_code(KC_LSFT);
  ng_up(1);
  unregister_code(KC_LSFT);
  ng_cut();
  ng_send_unicode_string_P(PSTR("《》"));
  ng_up(1);
}

void ngh_MCZ() { // 　　　×　　　×　　　×{改行 2}
  ng_send_unicode_string_P(PSTR("　　　×　　　×　　　×"));
  tap_code(KC_ENT);
  tap_code(KC_ENT);
}

void ngh_MCX() { // ^x【^v】{改行}{Space}+{↑}^x
  ng_cut();
  ng_send_unicode_string_P(PSTR("【"));
  ng_paste();
  ng_send_unicode_string_P(PSTR("】"));
  tap_code(KC_SPC);
  register_code(KC_LSFT);
  ng_up(1);
  unregister_code(KC_LSFT);
  ng_cut();
}

void ngh_MCC() { // ／{改行}
  ng_send_unicode_string_P(PSTR("／"));
}

void ngh_MCV() { // {改行}{End}{改行}「」{改行}{↑}
  tap_code(KC_ENT);
  ng_end();
  tap_code(KC_ENT);
  ng_send_unicode_string_P(PSTR("「」"));
  ng_up(1);
}

void ngh_MCB() { // {改行}{End}{改行}{Space}
  tap_code(KC_ENT);
  ng_end();
  tap_code(KC_ENT);
  tap_code(KC_SPC);
}

void ngh_CVY() { // +{Home}
  register_code(KC_LSFT);
  ng_home();
  unregister_code(KC_LSFT);
}

void ngh_CVU() { // ^x
  ng_cut();
}

void ngh_CVI() { // ^v
  ng_paste();
}

void ngh_CVO() { // ^y
  ng_redo();
}

void ngh_CVP() { // ^z
  ng_undo();
}

void ngh_CVH() { // ^c
  ng_copy();
}

void ngh_CVJ() { // {→}
  ng_right(1);
}

void ngh_CVK() { // +{→}
  register_code(KC_LSFT);
  ng_right(1);
  unregister_code(KC_LSFT);
}

void ngh_CVL() { // +{→ 5}
  register_code(KC_LSFT);
  ng_right(5);
  unregister_code(KC_LSFT);
}

void ngh_CVSCLN() { // +{→ 20}
  register_code(KC_LSFT);
  ng_right(20);
  unregister_code(KC_LSFT);
}

void ngh_CVN() { // +{End}
  register_code(KC_LSFT);
  ng_end();
  unregister_code(KC_LSFT);
}

void ngh_CVM() { // {←}
  ng_left(1);
}

void ngh_CVCOMM() { // +{←}
  register_code(KC_LSFT);
  ng_left(1);
  unregister_code(KC_LSFT);
}

void ngh_CVDOT() { // +{← 5}
  register_code(KC_LSFT);
  ng_left(5);
  unregister_code(KC_LSFT);
}

void ngh_CVSLSH() { // +{← 20}
  register_code(KC_LSFT);
  ng_left(20);
  unregister_code(KC_LSFT);
}
*/

void ng_cut() {
  switch (naginata_config.os) {
    case NG_WIN:
    case NG_LINUX:
      tap_code16(LCTL(KC_X));
      break;
    case NG_MAC:
      tap_code16(LCMD(KC_X));
      break;
  }
}

void ng_copy() {
  switch (naginata_config.os) {
    case NG_WIN:
    case NG_LINUX:
      tap_code16(LCTL(KC_C));
      break;
    case NG_MAC:
      tap_code16(LCMD(KC_C));
      break;
  }
}

void ng_paste() {
  switch (naginata_config.os) {
    case NG_WIN:
    case NG_LINUX:
      tap_code16(LCTL(KC_V));
      break;
    case NG_MAC:
      register_code(KC_LCMD);
      wait_ms(100);
      tap_code(KC_V);
      wait_ms(100);
      unregister_code(KC_LCMD);
      wait_ms(100);
      break;
  }
}

void ng_up(uint8_t c) {
  for (uint8_t i = 0; i < c; i++) { // サイズ削減
    if (naginata_config.tategaki) {
      tap_code(KC_UP);
    } else {
      tap_code(KC_LEFT);
    }
  }
}

void ng_down(uint8_t c) {
  for (uint8_t i = 0; i < c; i++) {
    if (naginata_config.tategaki) {
      tap_code(KC_DOWN);
    } else {
      tap_code(KC_RIGHT);
    }
  }
}

void ng_left(uint8_t c) {
  for (uint8_t i = 0; i < c; i++) {
    if (naginata_config.tategaki) {
      tap_code(KC_LEFT);
    } else {
      tap_code(KC_DOWN);
    }
  }
}

void ng_right(uint8_t c) {
  for (uint8_t i = 0; i < c; i++) {
    if (naginata_config.tategaki) {
      tap_code(KC_RIGHT);
    } else {
      tap_code(KC_UP);
    }
  }
}

void ng_home() {
  switch (naginata_config.os) {
    case NG_WIN:
    case NG_LINUX:
      tap_code(KC_HOME);
      break;
    case NG_MAC:
      tap_code16(LCTL(KC_A));
      break;
  }
}

void ng_end() {
  switch (naginata_config.os) {
    case NG_WIN:
    case NG_LINUX:
      tap_code(KC_END);
      break;
    case NG_MAC:
      tap_code16(LCTL(KC_E));
      break;
  }
}

void ng_katakana() {
  switch (naginata_config.os) {
    case NG_WIN:
    case NG_LINUX:
      tap_code16(LCTL(KC_I));
      break;
    case NG_MAC:
      tap_code16(LCTL(KC_K));
      break;
  }
}

void ng_save() {
  switch (naginata_config.os) {
    case NG_WIN:
    case NG_LINUX:
      tap_code16(LCTL(KC_S));
      break;
    case NG_MAC:
      tap_code16(LCMD(KC_S));
      break;
  }
}

void ng_hiragana() {
  switch (naginata_config.os) {
    case NG_WIN:
    case NG_LINUX:
      tap_code16(LCTL(KC_U));
      break;
    case NG_MAC:
      tap_code16(LCTL(KC_J));
      break;
  }
}

void ng_redo() {
  switch (naginata_config.os) {
    case NG_WIN:
    case NG_LINUX:
      tap_code16(LCTL(KC_Y));
      break;
    case NG_MAC:
      tap_code16(LSFT(LCMD(KC_Z)));
      break;
  }
}

void ng_undo() {
  switch (naginata_config.os) {
    case NG_WIN:
    case NG_LINUX:
      tap_code16(LCTL(KC_Z));
      break;
    case NG_MAC:
      tap_code16(LCMD(KC_Z));
      break;
  }
}

void ng_saihenkan() {
  switch (naginata_config.os) {
    case NG_WIN:
    case NG_LINUX:
      tap_code(KC_INT4);
      break;
    case NG_MAC:
      tap_code(KC_LANGUAGE_1);
      tap_code(KC_LANGUAGE_1);
      break;
  }
}

void ng_eof() {
  switch (naginata_config.os) {
    case NG_WIN:
    case NG_LINUX:
      tap_code16(LCTL(KC_END));
      break;
    case NG_MAC:
      tap_code16(LCMD(KC_DOWN));
      break;
  }
}