#include "../../source_sdk/surface/surface.h"
#include "../../source_sdk/math/vec3.h"
#include "../../utils/utils.h"
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

struct timer_t
{
    int x;
    int y;
    struct vec3_t color;
    float curtime;
    float starttime;
    float endtime;
};

struct render_data_t render_queue[RENDER_QUEUE_SIZE];
struct bbox_decorator_t bbox_queue[RENDER_QUEUE_SIZE];
struct timer_t timer_queue[RENDER_QUEUE_SIZE];
pthread_mutex_t render_queue_mutex = { 0 };

void init_render_queue()
{
    pthread_mutex_init(&render_queue_mutex, NULL);

    for (int i = 0; i < RENDER_QUEUE_SIZE; i++)
    {
        timer_queue[i].curtime = -1.0f;
    }
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

void add_timer(int x, int y, struct vec3_t color, float curtime, float starttime, float endtime)
{
    pthread_mutex_lock(&render_queue_mutex);

    for (int i = 0; i < RENDER_QUEUE_SIZE; i++)
    {
        if (timer_queue[i].curtime < 0.0f)
        {
            timer_queue[i].x = x;
            timer_queue[i].y = y;
            timer_queue[i].color = color;
            timer_queue[i].curtime = curtime;
            timer_queue[i].starttime = starttime;
            timer_queue[i].endtime = endtime;
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

void draw_timer_queue()
{
    pthread_mutex_lock(&render_queue_mutex);

    for (int i = 0; i < RENDER_QUEUE_SIZE; i++)
    {
        if (timer_queue[i].curtime >= 0.0f)
        {
            draw_set_color(0, 0, 0, 255 / 2);
            draw_filled_rect(timer_queue[i].x - 50, timer_queue[i].y, timer_queue[i].x + 50, timer_queue[i].y + 3);

            draw_set_color(timer_queue[i].color.x, timer_queue[i].color.y, timer_queue[i].color.z, 255);

            // Calculate the reverse progress (remaining time fraction)
            float progress = (timer_queue[i].endtime - timer_queue[i].curtime) / (timer_queue[i].endtime - timer_queue[i].starttime);

            // Ensure progress is clamped between 0 and 1
            if (progress < 0.0f) 
            {
                progress = 0.0f;
            } 
            else if (progress > 1.0f) 
            {
                progress = 1.0f;
            }

            // Calculate the length of the progress bar
            float x_len = 100 * progress;

            // Draw the rectangle, starting at the right and shrinking toward the left
            draw_filled_rect(timer_queue[i].x - 50 + 1, timer_queue[i].y + 1, timer_queue[i].x - 50 + x_len, timer_queue[i].y + 2);
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
        timer_queue[i].curtime = -1.0f;
    }

    pthread_mutex_unlock(&render_queue_mutex);
}

void destroy_render_queue()
{
    pthread_mutex_destroy(&render_queue_mutex);
}