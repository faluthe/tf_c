#include "../../source_sdk/math/vec3.h"

#include <math.h>

struct vec3_t get_distance(struct vec3_t pos1, struct vec3_t pos2)
{
    struct vec3_t distance;
    distance.x = pos1.x - pos2.x;
    distance.y = pos1.y - pos2.y;
    distance.z = pos1.z - pos2.z;
    return distance;
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