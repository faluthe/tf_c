#include "weapon_entity.h"

__int32_t get_weapon_id(void *weapon_entity)
{
    void **vtable = *(void ***)weapon_entity;

    __int32_t (*func)(void *) = vtable[449];
    
    return func(weapon_entity);
}

float get_next_attack(void *weapon_entity)
{
    return *(float *)((uintptr_t)weapon_entity + 0xe94);
}