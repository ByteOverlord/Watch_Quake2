//
//  g_tbl_movs.c
//  WatchQuake2 Watch App
//
//  Created by ByteOverlord on 31.8.2023.
//

#include "g_tbl_movs.h"

#include "g_local.h"

#include "g_save_common.h"

#define SAV_MOV(mov) extern mmove_t mov;
#include "g_move_list.h"
#undef SAV_MOV

#define xstr(a) str(a)
#define str(a) #a

#define SAV_MOV(mov) {(byte*)&mov,xstr(mov),0},

saveaddr_t tbl_movs[] = {
#include "g_move_list.h"
    {NULL,NULL,0}
};

void InitMoveAddressTable(void)
{
    printf("Checking Move Pointers\n");
    InitAddressTable(tbl_movs);
    printf("Done\n");
}

byte* GetMoveAddress(uint32_t hash)
{
    return GetAddressByHash(tbl_movs,hash);
}

uint32_t GetMoveHash(byte* addr)
{
    return GetHashByAddress(tbl_movs,addr);
}
