//
//  g_save_common.c
//  WatchQuake2 Watch App
//
//  Created by ByteOverlord on 2.9.2023.
//

#include "g_save_common.h"

uint32_t StringHash(const char* str)
{
    uint32_t h = 5381;
    const char* s = str;
    while(*s)
    {
        h += *s;
        h = (h << 5) + h;
        s++;
    }
    return h;
}

void InitAddressTable(saveaddr_t* tbl_sav)
{
    for (int i=0; tbl_sav[i].ptr != NULL; i++)
    {
        tbl_sav[i].hash = StringHash(tbl_sav[i].name);
    }
    for (int i=0; tbl_sav[i].ptr != NULL; i++)
    {
        uint32_t hash0 = tbl_sav[i].hash;
        if (hash0 == 0)
        {
            printf("null hash = %s\n",tbl_sav[i].name);
        }
        for (int j=i+1; tbl_sav[j].ptr != NULL; j++)
        {
            uint32_t hash1 = tbl_sav[j].hash;
            if (tbl_sav[i].ptr == tbl_sav[j].ptr)
            {
                printf("dublicate reference, %i %i -> %s %s\n",i,j,tbl_sav[i].name,tbl_sav[j].name);
            }
            else if (hash0 == hash1)
            {
                printf("hash collision, %s == %s\n",tbl_sav[i].name,tbl_sav[j].name);
            }
        }
    }
}

byte* GetAddressByHash(saveaddr_t* tbl_sav, uint32_t hash)
{
    for (int i=0; tbl_sav[i].ptr != NULL; i++)
    {
        saveaddr_t* savfunc = &tbl_sav[i];
        if (savfunc->hash == hash)
        {
            return savfunc->ptr;
        }
    }
    return NULL;
}

uint32_t GetHashByAddress(saveaddr_t* tbl_sav, byte* addr)
{
    for (int i=0; tbl_sav[i].ptr != NULL; i++)
    {
        saveaddr_t* savfunc = &tbl_sav[i];
        if (savfunc->ptr == addr)
        {
            return savfunc->hash;
        }
    }
    return 0;
}
