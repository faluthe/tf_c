#include "utils.h"

#include <dlfcn.h>
#include <stdbool.h>
#include <stdint.h>
#include <sys/mman.h>
#include <unistd.h>

bool write_to_table(void **vtable, int index, void *func)
{
    const long page_size = sysconf(_SC_PAGESIZE);
    // Sets last three digits to zero
    void *table_page = (void *)((__uint64_t)vtable & ~(page_size - 1));
    log_msg("vfunc table page found at %p\n", table_page);

    if (mprotect(table_page, page_size, PROT_READ | PROT_WRITE) != 0)
    {
        log_msg("mprotect failed to change page protection\n");
        return false;
    }

    vtable[22] = func;

    if (mprotect(table_page, page_size, PROT_READ) != 0)
    {
        log_msg("mprotect failed to reset page protection\n");
        return false;
    }

    return true;
}