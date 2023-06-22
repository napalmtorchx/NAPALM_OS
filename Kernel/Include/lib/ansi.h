#pragma once
#include <lib/types.h>

#define ANSI_RESET             "\x1b[0m"

#define ANSI_FG_DARKRED        "\x1b[31m"
#define ANSI_FG_DARKGREEN      "\x1b[32m"
#define ANSI_FG_DARKYELLOW     "\x1b[33m"
#define ANSI_FG_DARKBLUE       "\x1b[34m"
#define ANSI_FG_DARKMAGENTA    "\x1b[35m"
#define ANSI_FG_DARKCYAN       "\x1b[36m"
#define ANSI_FG_GRAY           "\x1b[37m"
#define ANSI_FG_DARKGRAY       "\x1b[90m"
#define ANSI_FG_RED            "\x1b[91m"
#define ANSI_FG_GREEN          "\x1b[92m"
#define ANSI_FG_YELLOW         "\x1b[93m"
#define ANSI_FG_BLUE           "\x1b[94m"
#define ANSI_FG_MAGENTA        "\x1b[95m"
#define ANSI_FG_CYAN           "\x1b[96m"
#define ANSI_FG_WHITE          "\x1b[97m"

#define ANSI_BG_DARKRED        "\x1b[41m"
#define ANSI_BG_DARKGREEN      "\x1b[42m"
#define ANSI_BG_DARKYELLOW     "\x1b[43m"
#define ANSI_BG_DARKBLUE       "\x1b[44m"
#define ANSI_BG_DARKMAGENTA    "\x1b[45m"
#define ANSI_BG_DARKCYAN       "\x1b[46m"
#define ANSI_BG_GRAY           "\x1b[47m"
#define ANSI_BG_DARKGRAY       "\x1b[100m"
#define ANSI_BG_RED            "\x1b[101m"
#define ANSI_BG_GREEN          "\x1b[102m"
#define ANSI_BG_YELLOW         "\x1b[103m"
#define ANSI_BG_BLUE           "\x1b[104m"
#define ANSI_BG_MAGENTA        "\x1b[105m"
#define ANSI_BG_CYAN           "\x1b[106m"
#define ANSI_BG_WHITE          "\x1b[107m"

uint8_t ansi_fg_to_vga(uint8_t ansi_code, uint8_t def_result);
uint8_t ansi_bg_to_vga(uint8_t ansi_code, uint8_t def_result);