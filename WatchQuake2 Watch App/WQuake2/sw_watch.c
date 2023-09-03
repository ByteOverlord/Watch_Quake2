//
//  vid_watch.c
//  WatchQuake2 Watch App
//
//  Created by ByteOverlord on 26.11.2022.
//

#include "sw_watch.h"

#include "r_local.h"

#include "WQ2Common.h"

extern viddef_t viddef;
extern viddef_t vid;

unsigned char* g_DataImages[3];
u32* g_8to32tables[3];

u32 framebufferWriteIndex = 0;
qboolean paletteSet = qFalse;

extern uint g_WQVidScreenWidth;
extern uint g_WQVidScreenHeight;

#include "Quake2Wrapper.h"

void SWimp_BeginFrame( float camera_separation )
{
}

void SWimp_EndFrame (void)
{
    u32 prevIndex = framebufferWriteIndex;
    framebufferWriteIndex++;
    framebufferWriteIndex %= 3;

    vid.buffer = g_DataImages[framebufferWriteIndex];
    memcpy(&viddef,&vid,sizeof(viddef));

    if (!paletteSet)
    {
        int prevPrevIndex = prevIndex - 1;
        if (prevPrevIndex < 0)
        {
            prevPrevIndex = 2;
        }
        memcpy(g_8to32tables[prevIndex],g_8to32tables[prevPrevIndex],256 * 4);
    }
    paletteSet = qFalse;
    WQEndFrame();
}

int SWimp_Init( void *hInstance, void *wndProc )
{
    int maxSize = 1600 * 1200;
    int paletteSize = 256;

    g_DataImages[0] = AlignedMalloc(maxSize * 3,16);
    g_DataImages[1] = g_DataImages[0] + maxSize;
    g_DataImages[2] = g_DataImages[1] + maxSize;

    memset(g_DataImages[0],0,maxSize * 3);

    g_8to32tables[0] = AlignedMalloc(paletteSize * 4 * 3,16);
    g_8to32tables[1] = g_8to32tables[0] + paletteSize;
    g_8to32tables[2] = g_8to32tables[1] + paletteSize;

    memset(g_8to32tables[0],0,paletteSize * 4 * 3);

    vid.buffer = g_DataImages[0];
    vid.rowbytes = g_WQVidScreenWidth;
    vid.width = g_WQVidScreenWidth;
    vid.height = g_WQVidScreenHeight;
    //vid.colormap
    //vic.alphamap
    memcpy(&viddef,&vid,sizeof(viddef));
    
    return 0;
}

void SWimp_SetPalette( const unsigned char *palette)
{
    if (palette)
    {
        memcpy(g_8to32tables[framebufferWriteIndex],palette,256 * 4);
        paletteSet = qTrue;
    }
}

void SWimp_Shutdown( void )
{
    AlignedFree(vid.buffer);
}

rserr_t SWimp_SetMode( int *pwidth, int *pheight, int mode, qboolean fullscreen )
{
    *pwidth = g_WQVidScreenWidth;
    *pheight = g_WQVidScreenHeight;
    return rserr_ok;
}

void SWimp_AppActivate( qboolean active )
{
}
