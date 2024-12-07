#include <stdbool.h>
#include <stdint.h>

void log_msg(const char *format, ...);
void close_log();
bool write_to_table(void **vtable, int index, void *func);
void *get_localplayer();
void *pattern_scan(const char *lib_name, __uint8_t *pattern, const char *mask);
bool init_sdl_hook(void *lib_handle, const char *func_name, void *hook, void **original);
bool restore_sdl_hook(void *lib_handle, const char *func_name, void *original);