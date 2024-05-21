#include <dlfcn.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <unistd.h>

__int64_t (*create_move_original)(void *, float, void *) = NULL;
void **client_mode_vtable = NULL;
FILE *logfile = NULL;

void vlog(FILE *log_file, const char *format, ...)
{
    va_list args;
    va_start(args, format);

    vfprintf(log_file, format, args);
    fflush(log_file);

    va_end(args);
}

void write_to_table(void **vtable, int index, void *func)
{
    const long page_size = sysconf(_SC_PAGESIZE);
    // Sets last three digits to zero
    void *table_page = (void *)((__uint64_t)vtable & ~(page_size - 1));
    vlog(logfile, "vfunc table page found at %p\n", table_page);

    if (mprotect(table_page, page_size, PROT_READ | PROT_WRITE) != 0)
    {
        vlog(logfile, "mprotect failed to change page protection\n");

        fclose(logfile);
        return;
    }

    vtable[22] = func;

    if (mprotect(table_page, page_size, PROT_READ) != 0)
    {
        vlog(logfile, "mprotect failed to reset page protection\n");

        fclose(logfile);
        return;
    }
}

__int64_t create_move_hook(void *this, float sample_time, void *user_cmd)
{
    FILE *log_file = fopen("/home/pat/Documents/tf_c/tf_c.log", "a");
    static short hooked = 0;

    if (hooked == 0)
    {
        fprintf(log_file, "Hi from hook function! time: %f ptr: %p og: %p\n", sample_time, user_cmd, create_move_original);
        hooked = 1;
    }

    fclose(log_file);

    return create_move_original(this, sample_time, user_cmd);
}

__attribute__((constructor)) void init()
{
    logfile = fopen("/home/pat/Documents/tf_c/tf_c.log", "w");

    vlog(logfile, "tf_c loaded\n");

    void *client_lib = dlopen("/home/pat/.steam/debian-installation/steamapps/common/Team Fortress 2/tf/bin/linux64/client.so", RTLD_NOLOAD | RTLD_NOW);
    if (!client_lib)
    {
        vlog(logfile, "Failed to load client.so\n");

        fclose(logfile);
        return;
    }

    vlog(logfile, "client.so loaded at %p\n", client_lib);

    __int64_t (*create_interface)(char *, __int32_t *) = dlsym(client_lib, "CreateInterface");
    if (!create_interface)
    {
        vlog(logfile, "Failed to get CreateInterface\n");

        fclose(logfile);
        return;
    }

    vlog(logfile, "CreateInterface found at %p\n", create_interface);

    void *client_interface = (void *)create_interface("VClient017", NULL);
    if (!client_interface)
    {
        vlog(logfile, "Failed to get VClient017 interface\n");

        fclose(logfile);
        return;
    }

    vlog(logfile, "VClient017 interface found at %p\n", client_interface);

    /* The first 8 bytes of class that implements an interface (pure virtual class)
     * is a pointer to an array of function pointers resolved at runtime (virtual function table) */
    void **client_vtable = *(void ***)client_interface;
    vlog(logfile, "Client vfunc table found at %p\n", client_vtable);

    void *hud_process_input_addr = client_vtable[10];
    vlog(logfile, "hud_process_input found at %p\n", hud_process_input_addr);

    __uint32_t *client_mode_eaddr = (__uint32_t *)((__uint64_t)(hud_process_input_addr) + 3);
    vlog(logfile, "Ptr to ClientMode effective address (offset): %p\n", client_mode_eaddr);
    vlog(logfile, "ClientMode effective address (offset): %p\n", *client_mode_eaddr);

    void *client_mode_next_instruction = (void *)((__uint64_t)(hud_process_input_addr) + 7);
    vlog(logfile, "Next instruction after hud_process_input: %p\n", client_mode_next_instruction);

    void **client_mode_ptr = (void **)((__uint64_t)(client_mode_next_instruction) + *client_mode_eaddr);
    vlog(logfile, "Ptr to ClientMode: %p\n", client_mode_ptr);

    void *client_mode_interface = *client_mode_ptr;
    vlog(logfile, "ClientMode: %p\n", client_mode_interface);

    client_mode_vtable = *(void ***)client_mode_interface;
    vlog(logfile, "ClientMode vfunc table found at %p\n", client_mode_vtable);

    create_move_original = client_mode_vtable[22];
    vlog(logfile, "CreateMove found at %p\n", create_move_original);

    write_to_table(client_mode_vtable, 22, create_move_hook);

    vlog(logfile, "We should be hooked...\n");
    vlog(logfile, "Original: %p, Hook: %p, [22]: %p\n", create_move_original, create_move_hook, client_mode_vtable[22]);

    fclose(logfile);
}

__attribute__((destructor)) void unload()
{
    FILE *log_file = fopen("/home/pat/Documents/tf_c/tf_c.log", "a");

    write_to_table(client_mode_vtable, 22, create_move_original);

    fprintf(log_file, "tf_c unloaded\n");

    fclose(log_file);
}