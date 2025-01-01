#include "../../source_sdk/math/vec3.h"

#include <math.h>

struct vec3_t get_difference(struct vec3_t pos1, struct vec3_t pos2)
{
    struct vec3_t difference;
    difference.x = pos1.x - pos2.x;
    difference.y = pos1.y - pos2.y;
    difference.z = pos1.z - pos2.z;
    return difference;
}

float get_distance(struct vec3_t pos1, struct vec3_t pos2)
{
    return sqrt(((pos1.x - pos2.x) * (pos1.x - pos2.x)) + ((pos1.y - pos2.y) * (pos1.y - pos2.y)) + ((pos1.z - pos2.z) * (pos1.z - pos2.z)));
}

float positive_quadratic_root(float a, float b, float c)
{
    float discriminant = (b * b) - (4 * a * c);
    if (discriminant < 0)
    {
        return -1;
    }

    float root1 = (-b + sqrt(discriminant)) / (2 * a);
    float root2 = (-b - sqrt(discriminant)) / (2 * a);

    if (root2 > 0 && root1 > 0)
    {
        if (root1 < root2)
        {
            return root1;
        }
        else
        {
            return root2;
        }
    }

    if (root1 > 0)
    {
        return root1;
    }

    if (root2 > 0)
    {
        return root2;
    }

    return -1;
}

float angle_between_vectors(struct vec3_t vec1, struct vec3_t vec2)
{
    float dot_product = vec1.x * vec2.x + vec1.y * vec2.y + vec1.z * vec2.z;
    float magnitude1 = sqrt(vec1.x * vec1.x + vec1.y * vec1.y + vec1.z * vec1.z);
    float magnitude2 = sqrt(vec2.x * vec2.x + vec2.y * vec2.y + vec2.z * vec2.z);
    return acos(dot_product / (magnitude1 * magnitude2)) * 180 / M_PI;
}

void angle_vectors(struct vec3_t angles, struct vec3_t *forward, struct vec3_t *right, struct vec3_t *up)
{
    // Sin/cosine for rotation, pitch, yaw
    float sp = sin(angles.x * (M_PI / 180));
    float cp = cos(angles.x * (M_PI / 180));
    float sy = sin(angles.y * (M_PI / 180));
    float cy = cos(angles.y * (M_PI / 180));
    float sr = sin(angles.z * (M_PI / 180));
    float cr = cos(angles.z * (M_PI / 180));

    if (forward)
    {
        forward->x = cp * cy;
        forward->y = cp * sy;
        forward->z = -sp;
    }

    if (right)
    {
        right->x = (-1 * sr * sp * cy + -1 * cr * -sy);
        right->y = (-1 * sr * sp * sy + -1 * cr * cy);
        right->z = -1 * sr * cp;
    }

    if (up)
    {
        up->x = (cr * sp * cy + -sr * -sy);
        up->y = (cr * sp * sy + -sr * cy);
        up->z = cr * cp;
    }
}

struct vec3_t get_view_angle(struct vec3_t diff)
{
    // Common side between two right triangles
    float c = sqrt((diff.x * diff.x) + (diff.y * diff.y));

    float pitch_angle = atan2(diff.z, c) * 180 / M_PI;
    float yaw_angle = atan2(diff.y, diff.x) * 180 / M_PI;

    struct vec3_t view_angle = {.x = -pitch_angle, .y = yaw_angle, .z = 0};

    return view_angle;
}

float vec_lenght2d(struct vec3_t vec)
{
    return sqrtf(vec.x * vec.x + vec.y * vec.y);
}

float deg_2_rad(float n)
{
    return n * M_PI / 180.0f;
}

float delta_rad_angle2f(float a, float b)
{

    float delta = isfinite(a - b) ? remainder(a - b, 360) : 0;

    if (a > b && delta >= M_PI)
    {
        delta -= M_PI * 2;
    }
    else if (delta <= -M_PI)
    {
        delta += M_PI * 2;
    }

    return delta;
}
