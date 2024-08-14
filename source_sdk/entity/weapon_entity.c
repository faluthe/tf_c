#include "weapon_entity.h"

__int32_t get_weapon_id(void *weapon_entity)
{
    void **vtable = *(void ***)weapon_entity;

    __int32_t (*func)(void *) = vtable[449];
    
    return func(weapon_entity);
}