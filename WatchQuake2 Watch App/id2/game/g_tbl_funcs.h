//
//  g_tbl_funcs.h
//  WatchQuake2 Watch App
//
//  Created by ByteOverlord on 31.8.2023.
//

#ifndef g_tbl_funcs_h
#define g_tbl_funcs_h

#include "q_shared.h"

void InitFunctionAddressTable(void);
byte* GetFunctionAddress(uint32_t hash);
uint32_t GetFunctionHash(byte* addr);

#endif /* g_tbl_funcs_h */
