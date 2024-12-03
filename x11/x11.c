#include "../utils/utils.h"
#include "x11.h"

#include <stdbool.h>
#include <X11/Xlib.h>

static Display* display;

bool is_key_down(int key)
{
    char keys[32];
    XQueryKeymap(display, keys);

    KeyCode key_code = XKeysymToKeycode(display, key);
    return (keys[key_code / 8] & (1 << (key_code % 8))) != 0;
}

bool init_x11()
{
    // We pass in NULL for the display name since
    // the name of the display is irrelevant
    display = XOpenDisplay(NULL);
    if (display == NULL) return false;

    return true;
}
