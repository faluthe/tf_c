#include "surface.h"

#include "../../utils/utils.h"

#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <wchar.h>

static void *interface = NULL;

void set_surface_interface(void *surface_interface)
{
    interface = surface_interface;
}

void draw_set_color(int r, int g, int b, int a)
{
    void **vtable = *(void ***)interface;

    void (*func)(void *, int, int, int, int) = vtable[10];

    func(interface, r, g, b, a);
}

void draw_filled_rect(int x0, int y0, int x1, int y1)
{
    void **vtable = *(void ***)interface;

    void (*func)(void *, int, int, int, int) = vtable[14];

    func(interface, x0, y0, x1, y1);
}

unsigned long text_create_font()
{
    void **vtable = *(void ***)interface;

    unsigned long (*func)(void *) = vtable[66];

    return func(interface);
}

__int8_t text_set_font_glyph_set(unsigned long font, const char *name, int tall, int weight, int blur, int scanlines, int flags)
{
    void **vtable = *(void ***)interface;

    __int8_t (*func)(void *, unsigned long, const char *, int, int, int, int, int, int, int) = vtable[67];

    return func(interface, font, name, tall, weight, blur, scanlines, flags, 0, 0);
}

void draw_set_text_font(unsigned long font)
{
    void **vtable = *(void ***)interface;

    void (*func)(void *, unsigned long) = vtable[17];

    func(interface, font);
}

void draw_set_text_color(int r, int g, int b, int a)
{
    void **vtable = *(void ***)interface;

    void (*func)(void *, int, int, int, int) = vtable[18];

    func(interface, r, g, b, a);
}

void draw_set_text_pos(unsigned int x, unsigned int y)
{
    void **vtable = *(void ***)interface;

    void (*func)(void *, unsigned int, unsigned int) = vtable[20];

    func(interface, x, y);
}

void draw_print_text(const wchar_t *text, int text_len)
{
    void **vtable = *(void ***)interface;

    void (*func)(void *, const wchar_t *, int, int) = vtable[22];

    func(interface, text, text_len, 0);
}

void set_cursor_visible(bool visible)
{
    void **vtable = *(void ***)interface;

    void (*func)(void *, bool) = vtable[52];

    func(interface, visible);
}