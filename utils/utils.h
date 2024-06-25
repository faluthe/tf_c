#include <stdbool.h>

void log_msg(const char *format, ...);
void close_log();
bool write_to_table(void **vtable, int index, void *func);
void *get_localplayer();