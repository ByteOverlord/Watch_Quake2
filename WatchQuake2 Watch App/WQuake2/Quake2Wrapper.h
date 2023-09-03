//
//  Quake2Wrapper.h
//  WatchQuake2
//
//  Created by ByteOverlord on 26.11.2022.
//

#ifndef Quake2Wrapper_h
#define Quake2Wrapper_h

#import <CoreGraphics/CoreGraphics.h>

#define WQ_STATE_EXIT -1
#define WQ_STATE_PAUSE 0
#define WQ_STATE_PLAY  1

#ifdef __cplusplus
extern "C" {
#endif

#include "WQ2Common.h"

extern float g_WQForwardSpeed;
extern float g_WQTurnX;
extern float g_WQTurnY;
extern u32 g_WQCrouchState;

#define WQ_INPUT_FIRE        (u16)(0x0001)
#define WQ_INPUT_PREVWEAPON  (u16)(0x0002)
#define WQ_INPUT_NEXTWEAPON  (u16)(0x0004)
#define WQ_INPUT_JUMP        (u16)(0x0008)

#define WQ_INPUT_MENU_ESCAPE (u16)(0x0010)
#define WQ_INPUT_MENU_ENTER  (u16)(0x0020)
#define WQ_INPUT_MENU_LEFT   (u16)(0x0040)
#define WQ_INPUT_MENU_RIGHT  (u16)(0x0080)

#define WQ_INPUT_MENU_UP     (u16)(0x0100)
#define WQ_INPUT_MENU_DOWN   (u16)(0x0200)
#define WQ_INPUT_CHEAT       (u16)(0x0400)
#define WQ_INPUT_INV_PREV    (u16)(0x0800)

#define WQ_INPUT_CROUCH      (u16)(0x1000)
#define WQ_INPUT_INV_NEXT    (u16)(0x2000)
#define WQ_INPUT_INV_USE     (u16)(0x4000)
#define WQ_INPUT_HELP        (u16)(0x8000)

typedef struct
{
    i32 frameCounter;
    float meanFrameTime;
    i32 width;
    i32 height;
    i32 devWidth;
    i32 devHeight;
    float devPixelsPerDot;
} WQGameStats_t;

int WQRequestState(int);
void WQNotifyActive(int isActive);
void WQNotifyHeadphoneState(int isActive, int Hz, uint bits, int channels, int interleaved, int type);
void WQSetScreenSize(int width, int height, float pixelsPerDot);
CGImageRef WQCreateGameImage(void);
void WQInit(void);
void WQSetLoop(void);
void WQFree(void);
int WQGetFrame(void);
WQGameStats_t WQGetStats(void);
const char* WQGetStatsString(void);
const char* WQGetBenchmarkString(void);
int WQShowFPS(void);
void WQEndFrame(void);

void WQInputTapAndPan(CGPoint point, int type);
void WQInputLongPress(void);
void WQInputCrownRotate(float f, float delta);

int WQGetFrameBufferLength(void);
void WQSetAudioFormat(int Hz, uint bits, int channels, int interleaved, int type);

#endif /* Quake2Wrapper_h */
