#pragma once

void naginata_type(void);
void naginata_clear(void);
void compress_buffer(int nt);
bool naginata_lookup(int nt, bool shifted);
int number_of_candidates(void);
int count_bit(uint32_t);
bool process_modifier(uint16_t, keyrecord_t *);
void naginata_on(void);
void naginata_off(void);
bool naginata_state(void);
void switchOS(uint8_t);
void ng_set_unicode_mode(uint8_t);
void ng_show_os(void);
void mac_live_conversion_toggle(void);
void tategaki_toggle(void);
void kouchi_shift_toggle(void);
bool process_naginata(uint16_t, keyrecord_t *);
void set_naginata(uint8_t, uint16_t *, uint16_t *);
void ng_cut(void);
void ng_copy(void);
void ng_paste(void);
void ng_up(uint8_t);
void ng_down(uint8_t);
void ng_left(uint8_t);
void ng_right(uint8_t);
void ng_home(void);
void ng_end(void);
void ng_katakana(void);
void ng_save(void);
void ng_hiragana(void);
void ng_redo(void);
void ng_undo(void);
void ng_saihenkan(void);
void ng_eof(void);

typedef enum naginata_keycodes {
  NG_Q = SAFE_RANGE,
  NG_W,
  NG_E,
  NG_R,
  NG_T,
  NG_Y,
  NG_U,
  NG_I,
  NG_O,
  NG_P,
  NG_A,
  NG_S,
  NG_D,
  NG_F,
  NG_G,
  NG_H,
  NG_J,
  NG_K,
  NG_L,
  NG_SCLN,
  NG_Z,
  NG_X,
  NG_C,
  NG_V,
  NG_B,
  NG_N,
  NG_M,
  NG_COMM,
  NG_DOT,
  NG_SLSH,
  NG_SHFT,
  NG_SHFT2,
  NG_ON,
  NG_OFF,
  NG_CLR,
  NGSW_WIN,
  NGSW_MAC,
  NGSW_LNX,
  NG_MLV,
  NG_SHOS,
  NG_TAYO,
  NG_KOTI,
} NGKEYS;

typedef union {
  uint32_t raw;
  struct {
    uint8_t os;
    bool live_conv :1;
    bool tategaki :1;
    bool kouchi_shift :1;
  };
} user_config_t;

user_config_t naginata_config;

#define NG_SAFE_RANGE SAFE_RANGE + 42
#define NG_WIN 1
#define NG_MAC 2
#define NG_LINUX 3