#include <dlfcn.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

void vlog(FILE *log_file, const char *format, ...)
{
    va_list args;
    va_start(args, format);

    vfprintf(log_file, format, args);
    fflush(log_file);

    va_end(args);
}

__attribute__((constructor)) void init()
{
    FILE *log_file = fopen("/home/pat/Documents/tf_c/tf_c.log", "w");

    vlog(log_file, "tf_c loaded\n");

    void *client_lib = dlopen("/home/pat/.steam/debian-installation/steamapps/common/Team Fortress 2/tf/bin/linux64/client.so", RTLD_NOLOAD | RTLD_NOW);
    if (!client_lib)
    {
        vlog(log_file, "Failed to load client.so\n");

        fclose(log_file);
        return;
    }

    vlog(log_file, "client.so loaded at %p\n", client_lib);

    __int64_t (*create_interface)(char *, __int32_t *) = dlsym(client_lib, "CreateInterface");
    if (!create_interface)
    {
        vlog(log_file, "Failed to get CreateInterface\n");

        fclose(log_file);
        return;
    }

    vlog(log_file, "CreateInterface found at %p\n", create_interface);

    void *client_interface = (void *)create_interface("VClient017", NULL);
    if (!client_interface)
    {
        vlog(log_file, "Failed to get VClient017 interface\n");

        fclose(log_file);
        return;
    }

    vlog(log_file, "VClient017 interface found at %p\n", client_interface);

    /* The first 8 bytes of class that implements an interface (pure virtual class)
     * is a pointer to an array of function pointers resolved at runtime (virtual function table) */
    void **client_vtable = *(void ***)client_interface;
    vlog(log_file, "Client vfunc table found at %p\n", client_vtable);

    void *hud_process_input_addr = client_vtable[10];
    vlog(log_file, "hud_process_input found at %p\n", hud_process_input_addr);

    __uint32_t *client_mode_eaddr = (__uint32_t *)((__uint64_t)(hud_process_input_addr) + 3);
    vlog(log_file, "Ptr to ClientMode effective address (offset): %p\n", client_mode_eaddr);
    vlog(log_file, "ClientMode effective address (offset): %p\n", *client_mode_eaddr);

    void *client_mode_next_instruction = (void *)((__uint64_t)(hud_process_input_addr) + 7);
    vlog(log_file, "Next instruction after hud_process_input: %p\n", client_mode_next_instruction);

    void **client_mode_ptr = (void **)((__uint64_t)(client_mode_next_instruction) + *client_mode_eaddr);
    vlog(log_file, "Ptr to ClientMode: %p\n", client_mode_ptr);

    void *client_mode_interface = *client_mode_ptr;
    vlog(log_file, "ClientMode: %p\n", client_mode_interface);

    void **client_mode_vtable = *(void ***)client_mode_interface;
    vlog(log_file, "ClientMode vfunc table found at %p\n", client_mode_vtable);

    __int64_t (*create_move_original)(void *, float, void *) = client_mode_vtable[23];
    vlog(log_file, "CreateMove found at %p\n", create_move_original);

    fclose(log_file);
}

__attribute__((destructor)) void unload()
{
    FILE *log_file = fopen("/home/pat/Documents/tf_c/tf_c.log", "a");

    fprintf(log_file, "tf_c unloaded\n");

    fclose(log_file);
}