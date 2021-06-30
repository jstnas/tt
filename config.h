#ifndef CONFIG_H
#define CONFIG_H

#define TKEY_UP 107
#define TKEY_DOWN 106
#define TKEY_SUBMIT 10
#define TKEY_MENU 27
#define TKEY_BACK 263
#define TKEY_SPACE 32
#define TKEY_RESIZE 410

static char* escape_delay = "0";

static short color_background = -1;
static short color_foreground = 8;
static short color_correct = 7;
static short color_incorrect = 1;
static short color_accent = 3;

static unsigned target_width = 80;
static unsigned target_height = 4;

static unsigned menu_padding_x = 1;
static unsigned menu_padding_y = 1;

#endif
