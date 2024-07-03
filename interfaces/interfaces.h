#include <stdbool.h>

extern void **client_vtable;
extern void **client_mode_vtable;
extern void **vgui_panel_vtable;

bool init_interfaces();