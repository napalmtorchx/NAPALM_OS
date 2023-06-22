#include <hal/devices/kbd.h>
#include <core/kernel.h>

#define KBD_CHAR_MAX 58

static kbd_device_t _kbd;

kbd_state_t kbd_state_create(void* data, size_t sz, void* handle, bool enable_nl, kbd_event_t fn_ch, kbd_event_t fn_del, kbd_event_t fn_ret)
{
    kbd_state_t state = (kbd_state_t)
    {
        .fn_char    = fn_ch,
        .fn_enter   = fn_ret,
        .fn_del     = fn_del,
        .handle     = handle,
        .data       = data,
        .sz         = sz,
        .pos        = 0,
        .l_shift    = false,
        .r_shift    = false,
        .ctrl       = false,
        .alt        = false,
        .caps       = false,
        .esc        = false,
        .enable_nl  = enable_nl,
    };
    return state;
}

void kbd_init(void)
{
    _kbd = (kbd_device_t)
    {
        .base   = (device_t){ "ps2_keyboard", NULL_DUID, kbd_start, kbd_stop, false },
        .state  = NULL,
        .layout = &KB_LAYOUT_US,
        .code   = 0,
        .keymap = { 0 },
    };
    devmgr_register(&_kbd);
    devmgr_start(_kbd.base.uid, NULL);
}

bool kbd_start(kbd_device_t* dev, void* unused)
{
    _kbd.code   = 0;
    _kbd.state  = NULL;
    _kbd.layout = &KB_LAYOUT_US;
    memset(_kbd.keymap, 0, sizeof(_kbd.keymap));
    return true;
}

int kbd_stop(kbd_device_t* dev)
{
    return true;
}

void kbd_handle(uint8_t code)
{
    if (!_kbd.base.running) { return; }

    _kbd.code = code;
    kbd_update_keymap();
    kbd_update_state();
}

void kbd_update_keymap(void)
{
    if (_kbd.code <= 0x58) { _kbd.keymap[_kbd.code] = true; }
    else if (_kbd.code >= 0x81 && _kbd.code < 0xD8) { _kbd.keymap[_kbd.code - 128] = false; }
}

void kbd_update_state(void)
{
    if (_kbd.state == NULL) { return; }

    if (_kbd.code == KEY_BACKSPACE)
    {
        if (_kbd.state->fn_del != NULL) { _kbd.state->fn_del(_kbd.state->handle, NULL); }
        if (strlen(_kbd.state->data) > 0)
        {
            strback(_kbd.state->data);
        }
    }
    else if (_kbd.code == KEY_LSHIFT)       { _kbd.state->l_shift = true; }
    else if (_kbd.code == KEY_RSHIFT)       { _kbd.state->r_shift = true; }
    else if (_kbd.code == 0xAA)             { _kbd.state->l_shift = false; }
    else if (_kbd.code == 0xB6)             { _kbd.state->r_shift = false; }
    else if (_kbd.code == KEY_CAPS_LOCK)    { _kbd.state->caps = !_kbd.state->caps; }
    else if (_kbd.code == KEY_LCTRL)        { _kbd.state->ctrl = true; }
    else if (_kbd.code == KEY_LALT)         { _kbd.state->alt = true; }
    else if (_kbd.code == KEY_ESCAPE)       { _kbd.state->esc = true; }
    else if (_kbd.code == KEY_ENTER)
    {
        if (_kbd.state->enable_nl && _kbd.state->data != NULL) { stradd(_kbd.state->data, '\n'); }
        if (_kbd.state->fn_enter != NULL) { _kbd.state->fn_enter(_kbd.state->handle, NULL); }
    }
    else if (_kbd.code < KBD_CHAR_MAX)
    {
        char ascii = 0;
        bool caps = (((_kbd.state->l_shift || _kbd.state->r_shift) && !_kbd.state->caps) || ((!_kbd.state->l_shift && !_kbd.state->r_shift) && _kbd.state->caps));

        if (caps) { ascii = _kbd.layout->upper[_kbd.code]; }
        else      { ascii = _kbd.layout->lower[_kbd.code]; }

        if (ascii >= 32 && ascii <= 126 && _kbd.state->data != NULL) { stradd(_kbd.state->data, ascii); }
        if (_kbd.state->fn_char != NULL) { _kbd.state->fn_char(_kbd.state->handle, (void*)ascii); }
    }
}

void kbd_setstate(kbd_state_t* state) { _kbd.state = state; }

kbd_state_t* kbd_state(void) { return _kbd.state; }

bool kbd_keydown(KEYCODE code) { return _kbd.keymap[code]; }

bool kbd_keyup(KEYCODE code) { return !_kbd.keymap[code]; }
