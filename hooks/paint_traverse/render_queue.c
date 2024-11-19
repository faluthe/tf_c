#include "../../source_sdk/surface/surface.h"
#include "../../source_sdk/math/vec3.h"
#include "paint_traverse.h"

#include <pthread.h>
#include <wchar.h>

struct render_data_t
{
    const wchar_t *text;
    int x;
    int y;
    struct vec3_t color;
    float data;
};

struct bbox_decorator_t
{
    const wchar_t *text;
    struct vec3_t color;
    void *entity;
};

struct render_data_t render_queue[RENDER_QUEUE_SIZE];
struct bbox_decorator_t bbox_queue[RENDER_QUEUE_SIZE];
pthread_mutex_t render_queue_mutex = {0};

void init_render_queue()
{
    pthread_mutex_init(&render_queue_mutex, NULL);
}

void add_to_render_queue(const wchar_t *text, int x, int y, struct vec3_t color, float data)
{
    pthread_mutex_lock(&render_queue_mutex);

    for (int i = 0; i < RENDER_QUEUE_SIZE; i++)
    {
        if (render_queue[i].text == NULL)
        {
            render_queue[i].text = text;
            render_queue[i].x = x;
            render_queue[i].y = y;
            render_queue[i].color = color;
            render_queue[i].data = data;
            break;
        }
    }

    pthread_mutex_unlock(&render_queue_mutex);
}

void add_bbox_decorator(const wchar_t *text, struct vec3_t color, void *entity)
{
    pthread_mutex_lock(&render_queue_mutex);

    for (int i = 0; i < RENDER_QUEUE_SIZE; i++)
    {
        if (bbox_queue[i].text == NULL)
        {
            bbox_queue[i].text = text;
            bbox_queue[i].color = color;
            bbox_queue[i].entity = entity;
            break;
        }
    }

    pthread_mutex_unlock(&render_queue_mutex);
}

void draw_render_queue()
{
    pthread_mutex_lock(&render_queue_mutex);

    for (int i = 0; i < RENDER_QUEUE_SIZE; i++)
    {
        if (render_queue[i].text != NULL)
        {
            draw_set_text_pos(render_queue[i].x, render_queue[i].y);
            draw_set_text_color(render_queue[i].color.x, render_queue[i].color.y, render_queue[i].color.z, 255);
            if (render_queue[i].data > 0.0f)
            {
                wchar_t data_str[64];
                swprintf(data_str, 64, L"%ls: %.2f", render_queue[i].text, render_queue[i].data);
                draw_print_text(data_str, wcslen(data_str));
            } else
            {
                draw_print_text(render_queue[i].text, wcslen(render_queue[i].text));
            }
        }
    }

    pthread_mutex_unlock(&render_queue_mutex);
}

void draw_bbox_decorators(int start_x, int start_y, void *entity)
{
    pthread_mutex_lock(&render_queue_mutex);

    int k = 0;
    for (int i = 0; i < RENDER_QUEUE_SIZE; i++)
    {
        if (bbox_queue[i].text != NULL && entity == bbox_queue[i].entity)
        {
            draw_set_text_color(bbox_queue[i].color.x, bbox_queue[i].color.y, bbox_queue[i].color.z, 255);
            draw_set_text_pos(start_x, start_y + (k++ * 20));
            draw_print_text(bbox_queue[i].text, wcslen(bbox_queue[i].text));
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
        bbox_queue[i].text = NULL;
    }

    pthread_mutex_unlock(&render_queue_mutex);
}

void destroy_render_queue()
{
    pthread_mutex_destroy(&render_queue_mutex);
}