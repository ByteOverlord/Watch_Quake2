//
//  g_tbl_movs.h
//  WatchQuake2 Watch App
//
//  Created by ByteOverlord on 31.8.2023.
//

#ifndef g_tbl_movs_h
#define g_tbl_movs_h

#include "q_shared.h"

void InitMoveAddressTable(void);
byte* GetMoveAddress(uint32_t hash);
uint32_t GetMoveHash(byte* addr);

#endif /* g_tbl_movs_h */
