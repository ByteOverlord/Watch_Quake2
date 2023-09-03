//
//  g_save_common.h
//  WatchQuake2 Watch App
//
//  Created by ByteOverlord on 2.9.2023.
//

#ifndef g_save_common_h
#define g_save_common_h

#include "q_shared.h"

uint32_t StringHash(const char* str);

typedef struct
{
    byte* ptr;
    const char* name;
    uint32_t hash;
} saveaddr_t;

void InitAddressTable(saveaddr_t* tbl_sav);
byte* GetAddressByHash(saveaddr_t* tbl_sav, uint32_t hash);
uint32_t GetHashByAddress(saveaddr_t* tbl_sav, byte* addr);

#endif /* g_save_common_h */
