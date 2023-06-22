#pragma once
#include <lib/types.h>
#include <hal/device.h>
#include <hal/ints/idt.h>

typedef enum
{
    KEY_INVALID,
    KEY_ESCAPE,
    KEY_D1,
    KEY_D2,
    KEY_D3,
    KEY_D4,
    KEY_D5,
    KEY_D6,
    KEY_D7,
    KEY_D8,
    KEY_D9,
    KEY_D0,
    KEY_MINUS,
    KEY_EQUAL,
    KEY_BACKSPACE,
    KEY_TAB,
    KEY_Q,
    KEY_W,
    KEY_E,
    KEY_R,
    KEY_T,
    KEY_Y,
    KEY_U,
    KEY_I,
    KEY_O,
    KEY_P,
    KEY_LBRACKET,
    KEY_RBRACKET,
    KEY_ENTER,
    KEY_LCTRL,
    KEY_A,
    KEY_S,
    KEY_D,
    KEY_F,
    KEY_G,
    KEY_H,
    KEY_J,
    KEY_K,
    KEY_L,
    KEY_COLON,
    KEY_QUOTE,
    KEY_BACKTICK,
    KEY_LSHIFT,
    KEY_BACKSLASH,
    KEY_Z,
    KEY_X,
    KEY_C,
    KEY_V,
    KEY_B,
    KEY_N,
    KEY_M,
    KEY_COMMA,
    KEY_PERIOD,
    KEY_SLASH,
    KEY_RSHIFT,
    KEY_KP_MULTIPLY,
    KEY_LALT,
    KEY_SPACE,
    KEY_CAPS_LOCK,
    KEY_F1,
    KEY_F2,
    KEY_F3,
    KEY_F4,
    KEY_F5,
    KEY_F6,
    KEY_F7,
    KEY_F8,
    KEY_F9,
    KEY_F10,
    KEY_NUM_LOCK,
    KEY_SCROLL_LOCK,
    KEY_KP_HOME,
    KEY_KP_UP,
    KEY_KP_PGUP,
    KEY_KP_MINUS,
    KEY_KP_LEFT,
    KEY_KP_D5,
    KEY_KP_RIGHT,
    KEY_KP_PLUS,
    KEY_KP_END,
    KEY_KP_DOWN,
    KEY_KP_PGDN,
    KEY_KP_PERIOD,
    KEY_F11 = 0X57,
    KEY_F12,
} KEYCODE;

typedef void (*kbd_event_t)(void* sender, void* arg);

typedef struct
{
    char upper[60];
    char lower[60];
} kbd_layout_t;

typedef struct
{
    kbd_event_t fn_char;
    kbd_event_t fn_enter;
    kbd_event_t fn_del;
    void*       handle;
    char*       data;
    size_t      sz;
    int         pos;

    bool l_shift   : 1;
    bool r_shift   : 1;
    bool ctrl      : 1;
    bool alt       : 1;
    bool caps      : 1;
    bool esc       : 1;
    bool enable_nl : 1;
} attr_pack kbd_state_t;

static const kbd_layout_t KB_LAYOUT_US = (kbd_layout_t)
{
    .upper = "??!@#$%^&*()_+??QWERTYUIOP{}??ASDFGHJKL:\"~?|ZXCVBNM<>???? \0",
    .lower = "??1234567890-=??qwertyuiop[]??asdfghjkl;'`?\\zxcvbnm,./??? \0",
};

typedef struct
{
    device_t      base;
    kbd_state_t*  state;
    kbd_layout_t* layout;
    uint8_t       code;
    bool          keymap[256];
} kbd_device_t;

kbd_state_t kbd_state_create(void* data, size_t sz, void* handle, bool enable_nl, kbd_event_t fn_ch, kbd_event_t fn_del, kbd_event_t fn_ret);

void kbd_init(void);
bool kbd_start(kbd_device_t* dev, void* unused);
int  kbd_stop(kbd_device_t* dev);
void kbd_handle(uint8_t code);
void kbd_update_keymap(void);
void kbd_update_state(void);

void kbd_setstate(kbd_state_t* state);
kbd_state_t* kbd_state(void);
bool kbd_keydown(KEYCODE code);
bool kbd_keyup(KEYCODE code);