#include "../color/color.h"
#include "convar/convar.h"

void set_cvar_interface(void *cvar_interface);
convar find_var(const char *var_name);
void con_color_printf(struct Color *clr, const char *fmt, ...);
void con_printf(const char *fmt, ...);
