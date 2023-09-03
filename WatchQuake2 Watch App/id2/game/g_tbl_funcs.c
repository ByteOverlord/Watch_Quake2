//
//  g_tbl_funcs.c
//  WatchQuake2 Watch App
//
//  Created by ByteOverlord on 31.8.2023.
//

#include "g_tbl_funcs.h"

#include "g_local.h"

#include "g_save_common.h"

/*
edict_t
void        (*prethink) (edict_t *ent);
void        (*think)(edict_t *self);
void        (*blocked)(edict_t *self, edict_t *other);
void        (*touch)(edict_t *self, edict_t *other, cplane_t *plane, csurface_t *surf);
void        (*use)(edict_t *self, edict_t *other, edict_t *activator);
void        (*pain)(edict_t *self, edict_t *other, float kick, int damage);
void        (*die)(edict_t *self, edict_t *inflictor, edict_t *attacker, int damage, vec3_t point);
*/

/*
mframe_t
void    (*aifunc)(edict_t *self, float dist);
void    (*thinkfunc)(edict_t *self);
*/

/*
moveinfo_t
void        (*endfunc)(edict_t *);
*/

/*
monsterinfo_t
void        (*stand)(edict_t *self);
void        (*idle)(edict_t *self);
void        (*search)(edict_t *self);
void        (*walk)(edict_t *self);
void        (*run)(edict_t *self);
void        (*dodge)(edict_t *self, edict_t *other, float eta);
void        (*attack)(edict_t *self);
void        (*melee)(edict_t *self);
void        (*sight)(edict_t *self, edict_t *other);
qboolean    (*checkattack)(edict_t *self);
*/

#define SAV_FUNC(func) extern void func (edict_t *self);
#define SAV_FUNC_SIGHT(func) extern void func (edict_t *self);
#define SAV_FUNC_PAIN(func) extern void func (edict_t *self, edict_t *other, float kick, int damage);
#define SAV_FUNC_DIE(func) extern void func (edict_t *self, edict_t *inflictor, edict_t *attacker, int damage, vec3_t point);
#define SAV_FUNC_CHECK(func) extern qboolean func (edict_t *self);
#define SAV_FUNC_DODGE(func) extern void func (edict_t *self, edict_t *attacker, float eta);

#define SAV_FUNC_BLOCKED(func) extern void func (edict_t *self, edict_t *other);
#define SAV_FUNC_TOUCH(func) extern void func (edict_t *self, edict_t *other, cplane_t *plane, csurface_t *surf);
#define SAV_FUNC_USE(func) extern void func (edict_t *self, edict_t *other, edict_t *activator);

#include "g_func_list.h"

#undef SAV_FUNC
#undef SAV_FUNC_SIGHT
#undef SAV_FUNC_PAIN
#undef SAV_FUNC_DIE
#undef SAV_FUNC_CHECK
#undef SAV_FUNC_DODGE
#undef SAV_FUNC_BLOCKED
#undef SAV_FUNC_TOUCH
#undef SAV_FUNC_USE

#define xstr(a) str(a)
#define str(a) #a

#define SAV_FUNC(func) {(byte*)func,xstr(func),0},
#define SAV_FUNC_SIGHT(func) {(byte*)func,xstr(func),0},
#define SAV_FUNC_PAIN(func) {(byte*)func,xstr(func),0},
#define SAV_FUNC_DIE(func) {(byte*)func,xstr(func),0},
#define SAV_FUNC_CHECK(func) {(byte*)func,xstr(func),0},
#define SAV_FUNC_DODGE(func) {(byte*)func,xstr(func),0},
#define SAV_FUNC_BLOCKED(func) {(byte*)func,xstr(func),0},
#define SAV_FUNC_TOUCH(func) {(byte*)func,xstr(func),0},
#define SAV_FUNC_USE(func) {(byte*)func,xstr(func),0},

saveaddr_t tbl_funcs[] = {
#include "g_func_list.h"
    {NULL,NULL,0}
};

void InitFunctionAddressTable(void)
{
    printf("Checking Function Pointers\n");
    InitAddressTable(tbl_funcs);
    printf("Done\n");
}

byte* GetFunctionAddress(uint32_t hash)
{
    return GetAddressByHash(tbl_funcs,hash);
}

uint32_t GetFunctionHash(byte* addr)
{
    return GetHashByAddress(tbl_funcs,addr);
}
