#include "utils.h"

#include <stdarg.h>
#include <stdio.h>

FILE *log_file = NULL;

void log_msg(const char *format, ...)
{
    if (log_file == NULL)
    {
        log_file = fopen("/tmp/tf_c.log", "w");
    }

    va_list args;
    va_start(args, format);
    
    vfprintf(log_file, format, args);
    fflush(log_file);

    va_end(args);
}

void close_log()
{
    if (log_file != NULL)
    {
        fclose(log_file);
    }
}
