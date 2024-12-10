#include "../../source_sdk/math/vec3.h"

float positive_quadratic_root(float a, float b, float c);
struct vec3_t get_difference(struct vec3_t pos1, struct vec3_t pos2);
float get_distance(struct vec3_t pos1, struct vec3_t pos2);
float angle_between_vectors(struct vec3_t vec1, struct vec3_t vec2);
void angle_vectors(struct vec3_t angles, struct vec3_t *forward, struct vec3_t *right, struct vec3_t *up);