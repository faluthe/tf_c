#include <stdint.h>
#include <stdbool.h>
#include <wchar.h>

void set_surface_interface(void *surface_interface);
void draw_set_color(int r, int g, int b, int a);
void draw_filled_rect(int x0, int y0, int x1, int y1);
void draw_outlined_rect(int x0, int y0, int x1, int y1);
unsigned long text_create_font();
__int8_t text_set_font_glyph_set(unsigned long font, const char *name, int tall, int weight, int blur, int scanlines, int flags);
void draw_set_text_font(unsigned long font);
void draw_set_text_color(int r, int g, int b, int a);
void draw_set_text_pos(unsigned int x, unsigned int y);
void draw_print_text(const wchar_t *text, int text_len);
void set_cursor_visible(bool visible);
void draw_circle(int x, int y, int radius, int segments);