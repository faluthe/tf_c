#include <stdbool.h>
#include <X11/Xlib.h>
#include <X11/keysym.h>

// Conveniently, X11's keysym matches the ASCII standard, so we can pass in human readable char constants. E. g. is_key_down('c') for the c key
// But, for keyboard keys that are not human readable (ALT, CTRL, ESC, etc.) we can't do the exact same.
// https://www.cl.cam.ac.uk/~mgk25/ucs/keysymdef.h is a list of all the key macros. E. g. is_key_down(XK_Alt_L) for the left ALT key.

bool init_x11();
bool is_key_down(int key);
