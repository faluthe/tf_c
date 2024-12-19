#include "../../config/config.h"
#include "../../source_sdk/debug_overlay/debug_overlay.h"
#include "../../source_sdk/global_vars/global_vars.h"
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
    struct vec3_t world_pos;
    struct vec3_t color;
    float starttime;
    float endtime;
};

struct render_data_t render_queue[RENDER_QUEUE_SIZE];
struct bbox_decorator_t bbox_queue[RENDER_QUEUE_SIZE];
struct timer_t timer_queue[RENDER_QUEUE_SIZE];
struct vec3_t projectile_target_line[1024];
pthread_mutex_t render_queue_mutex = { 0 };

void init_render_queue()
{
    pthread_mutex_init(&render_queue_mutex, NULL);

    for (int i = 0; i < RENDER_QUEUE_SIZE; i++)
    {
        timer_queue[i].starttime = -1.0f;
    }

    for (int i = 0; i < 1024; i++)
    {
        projectile_target_line[i] = (struct vec3_t){ 0.0f, 0.0f, 0.0f };
    }
}

// Called per frame
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

// Called per frame
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

// Called once, self clears
void add_timer(struct vec3_t world_pos, struct vec3_t color, float starttime, float endtime)
{
    pthread_mutex_lock(&render_queue_mutex);

    for (int i = 0; i < RENDER_QUEUE_SIZE; i++)
    {
        if (timer_queue[i].starttime < 0.0f)
        {
            timer_queue[i].world_pos = world_pos;
            timer_queue[i].color = color;
            timer_queue[i].starttime = starttime;
            timer_queue[i].endtime = endtime;
            break;
        }
    }

    pthread_mutex_unlock(&render_queue_mutex);
}

// Called per frame
void add_projectile_target_line_point(struct vec3_t point)
{
    pthread_mutex_lock(&render_queue_mutex);

    for (int i = 0; i < 1024; i++)
    {
        if (projectile_target_line[i].x == 0.0f && projectile_target_line[i].y == 0.0f && projectile_target_line[i].z == 0.0f)
        {
            projectile_target_line[i] = point;
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
    static const float linger_time = 0.5f;
    pthread_mutex_lock(&render_queue_mutex);

    for (int i = 0; i < RENDER_QUEUE_SIZE; i++)
    {
        if (timer_queue[i].starttime >= 0.0f)
        {
            struct vec3_t screen_pos;
            if (screen_position(&timer_queue[i].world_pos, &screen_pos) != 0)
            {
                continue;
            }

            draw_set_color(0, 0, 0, 255 / 2);
            draw_filled_rect(screen_pos.x - 50, screen_pos.y, screen_pos.x + 50, screen_pos.y + 3);

            draw_set_color(timer_queue[i].color.x, timer_queue[i].color.y, timer_queue[i].color.z, 255);

            // Calculate the reverse progress (remaining time fraction)
            float progress = (timer_queue[i].endtime - get_global_vars_curtime()) / (timer_queue[i].endtime - timer_queue[i].starttime);

            if (progress <= 0.0f - linger_time)
            {
                timer_queue[i].starttime = -1.0f;
                continue;
            }

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
            draw_filled_rect(screen_pos.x - 50 + 1, screen_pos.y + 1, screen_pos.x - 50 + x_len, screen_pos.y + 2);
        }
    }

    pthread_mutex_unlock(&render_queue_mutex);
}

void draw_projectile_target_line()
{
    pthread_mutex_lock(&render_queue_mutex);

    draw_set_color(
        config.aimbot.projectile_preview.entity_prediction_color.r * 255.0f,
        config.aimbot.projectile_preview.entity_prediction_color.g * 255.0f,
        config.aimbot.projectile_preview.entity_prediction_color.b * 255.0f,
        config.aimbot.projectile_preview.entity_prediction_color.a * 255
    );
    struct vec3_t last_pos = { 0.0f, 0.0f, 0.0f };
    for (int i = 0; i < 1024; i++)
    {
        if (projectile_target_line[i].x == 0.0f && projectile_target_line[i].y == 0.0f && projectile_target_line[i].z == 0.0f)
        {
            break;
        }

        struct vec3_t screen_pos;
        if (screen_position(&projectile_target_line[i], &screen_pos) != 0)
        {
            continue;
        }

        if (last_pos.x != 0.0f && last_pos.y != 0.0f)
        {
            draw_line(last_pos.x, last_pos.y, screen_pos.x, screen_pos.y);
        }

        last_pos = screen_pos;
    }

    pthread_mutex_unlock(&render_queue_mutex);
}

// Clears queues for next frame
void clear_render_queue()
{
    pthread_mutex_lock(&render_queue_mutex);

    for (int i = 0; i < RENDER_QUEUE_SIZE; i++)
    {
        render_queue[i].text = NULL;
        bbox_queue[i].text = NULL;
    }

    for (int i = 0; i < 1024; i++)
    {
        projectile_target_line[i] = (struct vec3_t){ 0.0f, 0.0f, 0.0f };
    }

    pthread_mutex_unlock(&render_queue_mutex);
}

void destroy_render_queue()
{
    pthread_mutex_destroy(&render_queue_mutex);
}