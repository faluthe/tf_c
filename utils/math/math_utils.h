/* Solve Polynomials of up to the fourth degree. (over real numbers)
 * Algorithms by Ferrari, Tartaglia, Cardano, et al. (16th century Italy)
 *
 * Implementation by Yair Chuchem, 2011
 * https://github.com/yairchu/quartic
 */

int solve_real_poly(int degree, const double* poly, double* results);
float positive_quadratic_root(float a, float b, float c);
struct vec3_t get_difference(struct vec3_t pos1, struct vec3_t pos2);
float get_distance(struct vec3_t pos1, struct vec3_t pos2);
float angle_between_vectors(struct vec3_t vec1, struct vec3_t vec2);
