//
//  net_watch.c
//  WatchQuake2 Watch App
//
//  Created by ByteOverlord on 26.11.2022.
//

/*
Copyright (C) 1997-2001 Id Software, Inc.

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

*/
// net_wins.c

#include "qcommon.h"

#define MAX_LOOPBACK 4

typedef struct
{
    byte    data[MAX_MSGLEN];
    int        datalen;
} loopmsg_t;

typedef struct
{
    loopmsg_t    msgs[MAX_LOOPBACK];
    int            get, send;
} loopback_t;

cvar_t        *net_shownet;
static cvar_t    *noudp;
static cvar_t    *noipx;

loopback_t    loopbacks[2];
int            ip_sockets[2];
int            ipx_sockets[2];

qboolean NET_CompareAdr (netadr_t a, netadr_t b)
{
    if (a.type != b.type)
        return qFalse;

    if (a.type == NA_LOOPBACK)
        return qTrue;

    if (a.type == NA_IP)
    {
        if (a.ip[0] == b.ip[0] && a.ip[1] == b.ip[1] && a.ip[2] == b.ip[2] && a.ip[3] == b.ip[3] && a.port == b.port)
            return qTrue;
        return qFalse;
    }

    if (a.type == NA_IPX)
    {
        if ((memcmp(a.ipx, b.ipx, 10) == 0) && a.port == b.port)
            return qTrue;
        return qFalse;
    }
    return qFalse;
}

/*
===================
NET_CompareBaseAdr

Compares without the port
===================
*/
qboolean NET_CompareBaseAdr (netadr_t a, netadr_t b)
{
    if (a.type != b.type)
        return qFalse;

    if (a.type == NA_LOOPBACK)
        return qTrue;

    if (a.type == NA_IP)
    {
        if (a.ip[0] == b.ip[0] && a.ip[1] == b.ip[1] && a.ip[2] == b.ip[2] && a.ip[3] == b.ip[3])
            return qTrue;
        return qFalse;
    }

    if (a.type == NA_IPX)
    {
        if ((memcmp(a.ipx, b.ipx, 10) == 0))
            return qTrue;
        return qFalse;
    }
    return qFalse;
}

char* NET_AdrToString (netadr_t a)
{
    static    char    s[64];

    if (a.type == NA_LOOPBACK)
        Com_sprintf (s, sizeof(s), "loopback");
    else if (a.type == NA_IP)
        Com_sprintf (s, sizeof(s), "%i.%i.%i.%i:%i", a.ip[0], a.ip[1], a.ip[2], a.ip[3], ntohs(a.port));
    else
        Com_sprintf (s, sizeof(s), "%02x%02x%02x%02x:%02x%02x%02x%02x%02x%02x:%i", a.ipx[0], a.ipx[1], a.ipx[2], a.ipx[3], a.ipx[4], a.ipx[5], a.ipx[6], a.ipx[7], a.ipx[8], a.ipx[9], ntohs(a.port));

    return s;
}

qboolean NET_StringToSockaddr (char *s, struct sockaddr *sadr)
{
    assert(0);
    return qFalse;
}

/*
=============
NET_StringToAdr

localhost
idnewt
idnewt:28000
192.246.40.70
192.246.40.70:28000
=============
*/
qboolean NET_StringToAdr (char *s, netadr_t *a)
{
    //struct sockaddr sadr;
    
    if (!strcmp (s, "localhost"))
    {
        memset (a, 0, sizeof(*a));
        a->type = NA_LOOPBACK;
        return qTrue;
    }

    /*if (!NET_StringToSockaddr (s, &sadr))
        return false;
    
    SockadrToNetadr (&sadr, a);

    return true;*/
    return qFalse;
}

qboolean    NET_IsLocalAddress (netadr_t adr)
{
    return adr.type == NA_LOOPBACK;
}

/*
=============================================================================

LOOPBACK BUFFERS FOR LOCAL PLAYER

=============================================================================
*/

qboolean NET_GetLoopPacket (netsrc_t sock, netadr_t *net_from, sizebuf_t *net_message)
{
    int        i;
    loopback_t    *loop;

    loop = &loopbacks[sock];

    if (loop->send - loop->get > MAX_LOOPBACK)
        loop->get = loop->send - MAX_LOOPBACK;

    if (loop->get >= loop->send)
        return qFalse;

    i = loop->get & (MAX_LOOPBACK-1);
    loop->get++;

    memcpy (net_message->data, loop->msgs[i].data, loop->msgs[i].datalen);
    net_message->cursize = loop->msgs[i].datalen;
    memset (net_from, 0, sizeof(*net_from));
    net_from->type = NA_LOOPBACK;
    return qTrue;

}

void NET_SendLoopPacket (netsrc_t sock, int length, void *data, netadr_t to)
{
    int        i;
    loopback_t    *loop;

    loop = &loopbacks[sock^1];

    i = loop->send & (MAX_LOOPBACK-1);
    loop->send++;

    memcpy (loop->msgs[i].data, data, length);
    loop->msgs[i].datalen = length;
}

qboolean NET_GetPacket (netsrc_t sock, netadr_t *net_from, sizebuf_t *net_message)
{
    /*int     ret;
    struct sockaddr from;
    int        fromlen;
    int        net_socket;
    int        protocol;
    int        err;*/

    if (NET_GetLoopPacket (sock, net_from, net_message))
        return qTrue;

    /*for (protocol = 0 ; protocol < 2 ; protocol++)
    {
        if (protocol == 0)
            net_socket = ip_sockets[sock];
        else
            net_socket = ipx_sockets[sock];

        if (!net_socket)
            continue;

        fromlen = sizeof(from);
        ret = recvfrom (net_socket, net_message->data, net_message->maxsize
            , 0, (struct sockaddr *)&from, &fromlen);
        if (ret == -1)
        {
            err = WSAGetLastError();

            if (err == WSAEWOULDBLOCK)
                continue;
            if (dedicated->value)    // let dedicated servers continue after errors
                Com_Printf ("NET_GetPacket: %s", NET_ErrorString());
            else
                Com_Error (ERR_DROP, "NET_GetPacket: %s", NET_ErrorString());
            continue;
        }

        SockadrToNetadr (&from, net_from);

        if (ret == net_message->maxsize)
        {
            Com_Printf ("Oversize packet from %s\n", NET_AdrToString (*net_from));
            continue;
        }

        net_message->cursize = ret;
        return qTrue;
    }*/

    return qFalse;
}

void NET_SendPacket (netsrc_t sock, int length, void *data, netadr_t to)
{
    /*int        ret;
    struct sockaddr    addr;
    int        net_socket;*/

    if ( to.type == NA_LOOPBACK )
    {
        NET_SendLoopPacket (sock, length, data, to);
        return;
    }
}

/*
====================
NET_Config

A single player game will only use the loopback code
====================
*/

void NET_Config (qboolean multiplayer)
{
    
}

// sleeps msec or until net socket is ready
void NET_Sleep(int msec)
{
}

/*
====================
NET_Init
====================
*/
void NET_Init (void)
{
    /*WORD    wVersionRequested;
    int        r;

    wVersionRequested = MAKEWORD(1, 1);

    r = WSAStartup (MAKEWORD(1, 1), &winsockdata);

    if (r)
        Com_Error (ERR_FATAL,"Winsock initialization failed.");

    Com_Printf("Winsock Initialized\n");*/

    noudp = Cvar_Get ("noudp", "0", CVAR_NOSET);
    noipx = Cvar_Get ("noipx", "0", CVAR_NOSET);

    net_shownet = Cvar_Get ("net_shownet", "0", 0);
}
