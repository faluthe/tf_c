#include "utils.h"

#include <dlfcn.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
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

    vtable[index] = func;

    if (mprotect(table_page, page_size, PROT_READ) != 0)
    {
        log_msg("mprotect failed to reset page protection\n");
        return false;
    }

    return true;
}

void *pattern_scan(const char *lib_name, __uint8_t *pattern, const char *mask)
{
    size_t pattern_length = strlen(mask);
    FILE *maps = fopen("/proc/self/maps", "r");
    if (!maps)
    {
        log_msg("Failed to open /proc/self/maps\n");
        return NULL;
    }

    // Find library in maps that fully matches lib_name
    char corrected_lib_name[256];
    corrected_lib_name[0] = '/';
    corrected_lib_name[1] = '\0';
    strcpy(corrected_lib_name, lib_name);

    char line[512];
    while (fgets(line, sizeof(line), maps))
    {
        if (strstr(line, corrected_lib_name))
        {
            __uint64_t start, end;
            char perms[5];
            if (sscanf(line, "%lx-%lx %4s", &start, &end, perms) != 3)
            {
                log_msg("Failed to parse /proc/self/maps\n");
                fclose(maps);
                return NULL;
            }

            if (strchr(perms, 'r') == NULL)
            {
                continue;
            }

            __uint8_t *scan_start = (__uint8_t *)start;
            __uint8_t *scan_end = (__uint8_t *)(end - pattern_length);
            
            for (; scan_start < scan_end; scan_start++)
            {
                size_t i;
                for (i = 0; i < pattern_length; i++)
                {
                    if (mask[i] != '?' && pattern[i] != scan_start[i])
                    {
                        break;
                    }
                }

                if (i == pattern_length)
                {
                    fclose(maps);
                    return (void *)scan_start;
                }
            }
        }
    }

    fclose(maps);
    return NULL;
}

bool init_sdl_hook(void *lib_handle, const char *func_name, void *hook, void **original)
{
    void *func = dlsym(lib_handle, func_name);

    if (!func)
    {
        log_msg("Failed to get %s\n", func_name);
        return false;
    }

    log_msg("%s wrapper found at %p\n", func_name, func);

    /* The symbols resolve to a wrapper function. They are a just a 2 byte
     * `jmp` and then a SIGNED (?) 4 byte offset relative to the instruction
     * pointer. Adding `ip + the offset` is a pointer to the address of the 
     * function that is wrapped. We save the orignal and call it in the hook. */
    int32_t offset = *(int32_t *)((uint64_t)func + 2);
    void **ptr_to_func = (void *)((uint64_t)func + 6 + offset);
    *original = *ptr_to_func;

    log_msg("Original %s at %p\n", func_name, *original);

    // The page should have rw perms already! No mprotect needed
    *ptr_to_func = hook;

    return true;
}

bool restore_sdl_hook(void *lib_handle, const char *func_name, void *original)
{
    void *func = dlsym(lib_handle, func_name);

    if (!func)
    {
        log_msg("Failed to get %s\n", func_name);
        return false;
    }

    int32_t offset = *(int32_t *)((uint64_t)func + 2);
    *(void **)((uint64_t)func + 6 + offset) = original;

    return true;
}