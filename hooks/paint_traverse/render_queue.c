#include "../../source_sdk/surface/surface.h"
#include "paint_traverse.h"

#include <pthread.h>
#include <wchar.h>

struct render_data_t
{
    const wchar_t *text;
    int x;
    int y;
};

struct render_data_t render_queue[RENDER_QUEUE_SIZE];
pthread_mutex_t render_queue_mutex = {0};

void init_render_queue()
{
    pthread_mutex_init(&render_queue_mutex, NULL);
}

void add_to_render_queue(const wchar_t *text, int x, int y)
{
    pthread_mutex_lock(&render_queue_mutex);

    for (int i = 0; i < RENDER_QUEUE_SIZE; i++)
    {
        if (render_queue[i].text == NULL)
        {
            render_queue[i].text = text;
            render_queue[i].x = x;
            render_queue[i].y = y;
            break;
        }
    }

    pthread_mutex_unlock(&render_queue_mutex);
}

void draw_render_queue()
{
    pthread_mutex_lock(&render_queue_mutex);

    draw_set_text_color(255, 0, 0, 255);
    for (int i = 0; i < RENDER_QUEUE_SIZE; i++)
    {
        if (render_queue[i].text != NULL)
        {
            draw_set_text_pos(render_queue[i].x, render_queue[i].y);
            draw_print_text(render_queue[i].text, wcslen(render_queue[i].text));
        }
    }

    pthread_mutex_unlock(&render_queue_mutex);
}

void clear_render_queue()
{
    pthread_mutex_lock(&render_queue_mutex);

    for (int i = 0; i < RENDER_QUEUE_SIZE; i++)
    {
        render_queue[i].text = NULL;
    }

    pthread_mutex_unlock(&render_queue_mutex);
}

void destroy_render_queue()
{
    pthread_mutex_destroy(&render_queue_mutex);
}