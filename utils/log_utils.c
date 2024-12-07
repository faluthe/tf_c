#include "../config/config.h"
#include "utils.h"

#include <stdarg.h>
#include <stdio.h>

void log_msg(const char *format, ...)
{
    va_list args;
    va_start(args, format);
    
    vfprintf(stderr, format, args);
    fflush(stderr);

    va_end(args);
}