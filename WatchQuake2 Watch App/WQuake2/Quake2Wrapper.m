//
//  Quake2Wrapper.m
//  WatchQuake2 Watch App
//
//  Created by ByteOverlord on 26.11.2022.
//

#include "Quake2Wrapper.h"

#import <Foundation/Foundation.h>

#import "WQ2SoundCallback.h"

#include "Threading.h"
#include "Threading.hpp"
#include "sys_watch.h"
#include "cd_watch.h"
#include "client.h"

#include <pthread/pthread.h>
pthread_mutex_t input_lock;
#define INPUT_INIT pthread_mutex_init(&input_lock,NULL);
#define INPUT_LOCK pthread_mutex_lock(&input_lock);
#define INPUT_UNLOCK pthread_mutex_unlock(&input_lock);

// Quake keys
#define    KEY_TAB           9
#define    KEY_ENTER         13
#define    KEY_ESCAPE        27
#define    KEY_SPACE         32
#define    KEY_BACKSPACE     127
#define    KEY_UPARROW       128
#define    KEY_DOWNARROW     129
#define    KEY_LEFTARROW     130
#define    KEY_RIGHTARROW    131
#define    KEY_PGDN          149
//

typedef struct
{
    i16 vel;
    u16 keys;
} WQInput_t;

void WQBlitFrameBuffer(unsigned char* dstRGBA, unsigned char* srcPaletteIndexed, unsigned int* palette);

void WQInitGameScreen(void);

void WQJumpCommand(int pressed);
void WQSwitchWeaponCommand(int next);
void WQShootCommand(int pressed);
void WQArrowCommand(int up);
void WQMenuToggle(void);
void WQConsoleToggle(void);
void WQRestart(void);
void WQChangeLevel(int e, int m, bool keepStats);
void WQKeyEvent(int key, int pressed);

void WQTick(void);
bool WQNeedsUpdate(void);
void WQUpdate(void);
void WQEndFrame(void);

extern unsigned char* g_DataImages[3];// palette indexed
extern u32* g_8to32tables[3];
unsigned char* g_DataRGBA = NULL;// converted final image
CGColorSpaceRef g_ColorSpaceRef;
CGContextRef g_Context;

//int g_frameBufferWriteIndex = 0;
int g_frameBufferReadIndex = 0;

TimeStepAccumulator_t logicAcc;
u64 prevTime = 0;

int g_WQFrameCounter;
u64 g_WQFrametimeAggregate;
u64 g_WQLastMeasurementTimeStamp;

float g_WQForwardSpeed = 0;
float g_WQStrafeSpeed = 0;
float g_WQTurnX = 0;
float g_WQTurnY = 0;
int g_WQAnyKey = 0;

CGPoint g_WQPrevPanPoint = {0,0};
CGPoint g_WQCurPanPoint = {0,0};
int g_WQPanState = 0;
int g_WQPrevPanState = 0;

#define WQ_PAN_MODE_NONE 0
#define WQ_PAN_MODE_AIM 1
#define WQ_PAN_MODE_WEAPONSELECT 2
#define WQ_PAN_MODE_STRAFE 3
#define WQ_PAN_MODE_ITEMSELECT 4

CGPoint g_WQSlideStart = {0,0};
CGPoint g_WQSlidePos = {0,0};

CGPoint g_WQPanEventPositions[8];
u8 g_WQPanEventStates;
int g_WQNumPanEvents;

i16 g_WQSlideEventSpeeds[8];
int g_WQNumSlideEvents;

int g_WQPanMode;

float g_WQSwimTimer = 0;
float g_WQCrouchTimer = 0;
float g_WQJumpTimer = 0;
float g_WQShootTimer = 0;
bool g_WQFullAuto = false;
float g_WQArrowTimer = 0;
int g_WQTicksSinceTap = 0;
uint g_WQFlags = 0;
u32 g_WQCrouchState = 0;

WQGameStats_t g_GameStats;

u32 g_TImerFPS = 60;

keydest_t prevDest = key_menu;

uint g_WQVidScreenWidth;
uint g_WQVidScreenHeight;

WQInput_t g_WQPlayerInput;

#define WQ_BENCHMARK_STATE_NULL 0
#define WQ_BENCHMARK_STATE_WAIT 1
#define WQ_BENCHMARK_STATE_RUNNING 2
#define WQ_BENCHMARK_STATE_ENDED 3

typedef struct
{
    u64 timeStart;
    u64 timeEnd;
    u64 acc;
    u64 lowest;
    u64 highest;
    u32 frames;
    u32 state;
} WQBenchmarkStats_t;

WQBenchmarkStats_t g_BenchmarkStats;

char benchmarkBuffer[256];

void WQBenchmarkStats_Init(WQBenchmarkStats_t* b)
{
    b->timeStart = 0;
    b->timeEnd = 0;
    b->acc = 0;
    b->lowest = ~((u64)0);
    b->highest = 0;
    b->frames = 0;
    b->state = WQ_BENCHMARK_STATE_NULL;
    memset(benchmarkBuffer,0,256);
}

void WQBenchmarkStats_Begin(WQBenchmarkStats_t* b, double delay, double time)
{
    double secondsToNanoSeconds = 1000000000.0;
    b->timeStart = GetTimeNanoSeconds() + delay * secondsToNanoSeconds;
    b->timeEnd = time * secondsToNanoSeconds;
    b->acc = 0;
    b->lowest = ~((u64)0);
    b->highest = 0;
    b->frames = 0;
    b->state = WQ_BENCHMARK_STATE_WAIT;
    snprintf(benchmarkBuffer,256,"Benchmark waiting --");
}

const char* WQGetBenchmarkString(void)
{
    return benchmarkBuffer;
}

void WQBenchmarkStats_Update(WQBenchmarkStats_t* b, u64 currentTime, u64 dt)
{
    if (b->state == WQ_BENCHMARK_STATE_NULL)
    {
        return;
    }
    else if (b->state == WQ_BENCHMARK_STATE_WAIT)
    {
        if (currentTime >= b->timeStart)
        {
            b->timeStart = currentTime;
            b->timeEnd += currentTime;
            snprintf(benchmarkBuffer,256,"Timer: %i\nAvg: --.--ms\nLow: --.--ms\nHigh: --.--ms",(int)GetDeltaTime(b->timeEnd - currentTime));
            b->state = WQ_BENCHMARK_STATE_RUNNING;
        }
        else
        {
            if ((b->frames % 60) == 0)
            {
                snprintf(benchmarkBuffer,256,"Benchmark starts in %i",(int)GetDeltaTime(b->timeStart - currentTime));
            }
        }
    }
    else if (b->state == WQ_BENCHMARK_STATE_RUNNING)
    {
        b->acc += dt;
        b->lowest = dt < b->lowest ? dt : b->lowest;
        b->highest = dt > b->highest ? dt : b->highest;
        b->frames++;
        if (currentTime >= b->timeEnd)
        {
            double avg = (b->acc / b->frames) / 1000000.0;
            double lo = b->lowest / 1000000.0;
            double hi = b->highest / 1000000.0;
            snprintf(benchmarkBuffer,256,"Result\nAvg: %.2fms\nLow: %.2fms\nHigh: %.2fms",avg,lo,hi);
            b->state = WQ_BENCHMARK_STATE_ENDED;
        }
        else
        {
            if ((b->frames % 60) == 0)
            {
                double avg = (b->acc / b->frames) / 1000000.0;
                double lo = b->lowest / 1000000.0;
                double hi = b->highest / 1000000.0;
                snprintf(benchmarkBuffer,256,"Timer: %i\nAvg: %.2fms\nLow: %.2fms\nHigh: %.2fms",(int)GetDeltaTime(b->timeEnd - currentTime),avg,lo,hi);
            }
            //snprintf(benchmarkBuffer,256,"Benchmark running: %.2f",GetDeltaTime(b->timeEnd - currentTime));
        }
    }
    else if (b->state == WQ_BENCHMARK_STATE_ENDED)
    {
        
    }
}

// todo
//
// replace volatile ints with _Atomic ints?
//
//#include <stdatomic.h>
//_Atomic int g_WQState = 1;

int g_WQResTimer = 0;

#define WQ_RESOLUTION_UPDATE_DELAY 4
#define WQ_RESOLUTION_RESIZE_TICK 2

volatile int g_WQState = WQ_STATE_PLAY;

extern void M_Menu_SaveSettings(void);

int WQRequestState(int state)
{
INPUT_LOCK

    if (g_WQState != state)
    {
        if (state == WQ_STATE_PAUSE)
        {
            printf("Game Paused\n");
            g_WQState = WQ_STATE_PAUSE;
            M_Menu_SaveSettings();
            SetGameLoopState(0);
        }
        else if (state == WQ_STATE_PLAY)
        {
            printf("Game Resumed\n");
            g_WQState = WQ_STATE_PLAY;
            SetGameLoopState(1);
        }
    }

INPUT_UNLOCK

    return 0;
}

void WQClerAllInputState(void)
{
    g_WQForwardSpeed = 0;
    g_WQStrafeSpeed = 0;
    g_WQTurnX = 0;
    g_WQTurnY = 0;
    g_WQPlayerInput.keys = 0;
    g_WQPlayerInput.vel = 0;
    g_WQPanState = 0;
    g_WQPrevPanState = g_WQPanState;
    g_WQCurPanPoint.x = 0;
    g_WQCurPanPoint.y = 0;
    g_WQPrevPanPoint = g_WQCurPanPoint;
    g_WQAnyKey = 0;
    
    memset(g_WQPanEventPositions,0,sizeof(g_WQPanEventPositions));
    g_WQPanEventStates = 0;
    g_WQNumPanEvents = 0;

    memset(g_WQSlideEventSpeeds,0,sizeof(g_WQSlideEventSpeeds));
    g_WQNumSlideEvents = 0;

    g_WQPanMode = 0;
}

void WQNotifyActive(int isActive)
{
INPUT_LOCK

   if (isActive)
   {
       //printf("Notify Active\n");

       // clear player input
       WQClerAllInputState();
   }
   else
   {
       //printf("Notify Inactive\n");
   }

INPUT_UNLOCK
}

static int g_headphonesActive = 0;

void WQNotifyHeadphoneState(int isActive, int Hz, uint bits, int channels, int interleaved, int type)
{
INPUT_LOCK
    g_headphonesActive = isActive;
INPUT_UNLOCK
}

void WQInputInit(WQInput_t* input)
{
    input->vel = 0;
    input->keys = 0;
}

void WQInputClear(WQInput_t* input)
{
    input->vel = 0;
    input->keys = 0;
}

void WQBlitFrameBuffer(unsigned char* dstRGBA, unsigned char* srcPaletteIndexed, unsigned int* palette)
{
    u32 width = g_WQVidScreenWidth;
    u32 height = g_WQVidScreenHeight;
    u32 pixelCount = width * height;
#ifdef WQ_BLITTER_BLOCK64
    u32* fb = __builtin_assume_aligned((u32*)dstRGBA,8);
    u32 blocks8 = pixelCount / 8;
    u32 leftOver = pixelCount - (blocks8 * 8);
    u64* vPtr = __builtin_assume_aligned((u64*)srcPaletteIndexed,8);
    u32 p = blocks8;
    while (p--)
    {
        u64 block = *vPtr++;
        fb[0] = palette[(block >> 0) & 0xFF];
        fb[1] = palette[(block >> 8) & 0xFF];
        fb[2] = palette[(block >> 16) & 0xFF];
        fb[3] = palette[(block >> 24) & 0xFF];
        fb[4] = palette[(block >> 32) & 0xFF];
        fb[5] = palette[(block >> 40) & 0xFF];
        fb[6] = palette[(block >> 48) & 0xFF];
        fb[7] = palette[(block >> 56) & 0xFF];
        fb += 8;
    }
#else
    u32* fb = __builtin_assume_aligned((u32*)dstRGBA,4);
    u32 blocks4 = width / 4;
    u32 leftOver = pixelCount - (blocks4 * 4);
    u32* vPtr = __builtin_assume_aligned((u32*)srcPaletteIndexed,4);
    u32 p = blocks4;
    while (p--)
    {
        u32 block = *vPtr++;
        fb[0] = palette[(block >> 0) & 0xFF];
        fb[1] = palette[(block >> 8) & 0xFF];
        fb[2] = palette[(block >> 16) & 0xFF];
        fb[3] = palette[(block >> 24) & 0xFF];
        fb += 4;
    }
#endif
    u8* vbPtr = (u8*)vPtr;
    p = leftOver;
    while (p--)
    {
        *fb++ = palette[*vbPtr++];
    }
}

void WQSetScreenSize(int width, int height, float pixelsPerDot)
{
    g_GameStats.devWidth = width;
    g_GameStats.devHeight = height;
    g_GameStats.devPixelsPerDot = pixelsPerDot;
    g_WQVidScreenWidth = width * pixelsPerDot;
    g_WQVidScreenHeight = height * pixelsPerDot;
    while ((g_WQVidScreenWidth % 8) != 0)
    {
        g_WQVidScreenWidth++;
    }
}

void WQInitGameScreen(void)
{
    // allocate enough space for largest supported resolution
    uint maxImgSize = 1600*1200;

    uint width = g_WQVidScreenWidth;//vid_width.value;
    uint height = g_WQVidScreenHeight;//vid_height.value;

    g_DataRGBA = AlignedMalloc(maxImgSize*4,16);
    memset(g_DataRGBA,0,maxImgSize*4);

    g_ColorSpaceRef = CGColorSpaceCreateDeviceRGB();
    g_Context = CGBitmapContextCreate(g_DataRGBA,width,height,8,width*4,g_ColorSpaceRef,kCGImageAlphaPremultipliedLast);
}

void WQResizeGameScreen(void)
{
    CGContextRelease(g_Context);
    uint width = g_WQVidScreenWidth;
    uint height = g_WQVidScreenHeight;
    g_Context = CGBitmapContextCreate(g_DataRGBA,width,height,8,width*4,g_ColorSpaceRef,kCGImageAlphaPremultipliedLast);
}

void WQRemoveGameScreen(void)
{
    CGContextRelease(g_Context);
    AlignedFree(g_DataRGBA);
    g_DataRGBA = NULL;
}

CGImageRef WQCreateGameImage(void)
{
    // triple buffering
    int idx = g_frameBufferReadIndex;
    WQBlitFrameBuffer(g_DataRGBA,g_DataImages[idx],g_8to32tables[idx]);
    ++g_frameBufferReadIndex;
    g_frameBufferReadIndex %= 3;
    return CGBitmapContextCreateImage(g_Context);
}

volatile int frames = 0;

void refresh_screen(void*);

char statsBuffer[256];

int statsHidden = 0;

void WQUpdateStats(void)
{
    double meanframeRate = 60;
    u64 now = GetTimeNanoSeconds();
    if (g_WQLastMeasurementTimeStamp != 0)
    {
        meanframeRate = GetDeltaTime(now - g_WQLastMeasurementTimeStamp) * g_WQFrameCounter;
    }
    double meanFrametime = (g_WQFrametimeAggregate / g_WQFrameCounter) / 1000000.0;
    g_GameStats.frameCounter = meanframeRate;
    g_GameStats.meanFrameTime = meanFrametime;
    g_WQFrameCounter = 0;
    g_WQFrametimeAggregate = 0;
    g_WQLastMeasurementTimeStamp = now;
    //if (scr_showfps.value != 0)
    {
        snprintf(statsBuffer,256,"FPS: %d, CPU: %.2fms, %dx%d", g_GameStats.frameCounter, g_GameStats.meanFrameTime, g_WQVidScreenWidth, g_WQVidScreenHeight);
    }
}

WQGameStats_t WQGetStats(void)
{
    return g_GameStats;
}

#include <string.h>

const char* WQGetStatsString(void)
{
    return statsBuffer;
}

void* GameLoop(void* p)
{

INPUT_LOCK
    if (g_WQState == WQ_STATE_PAUSE)
    {
        INPUT_UNLOCK
        return NULL;
    }
INPUT_UNLOCK

    ++frames;
    if (statsHidden == 0 && frames == 60)
    {
        // hack!
        // fixes stuttering audio when ImageOverlay.Text displays first time
        //scr_showfps.value = 0;
        statsHidden = 1;
    }
    WQTick();
    bool updated = false;
    while (WQNeedsUpdate())
    {
        WQUpdate();
        //WQEndFrame();
        g_WQFrameCounter++;
        if (g_TImerFPS)
        {
            --g_TImerFPS;
        }
        updated = true;
    }
    if (updated)
    {
        if (!g_TImerFPS)
        {
            WQUpdateStats();
            g_TImerFPS = 60;
        }
    }
    return NULL;
}

#include <sys/stat.h>

int do_mkdir(const char* path, mode_t mode)
{
    struct stat st;
    int status = 0;

    if (stat(path,&st) != 0)
    {
        if (mkdir(path,mode) != 0 && errno != EEXIST)
        {
            return -1;
        }
    }
    else if (!S_ISDIR(st.st_mode))
    {
        errno = ENOTDIR;
        status = -1;
    }
    return status;
}

int CreateFolder(const char* path)
{
    mode_t mode = S_IRWXU;
    char* copypath = strdup(path);
    char* pp = copypath;
    char* sp;
    int status = 0;

    while (status == 0 && (sp = strchr(pp,'/')) != 0)
    {
        if (sp != pp)
        {
            *sp = '\0';
            status = do_mkdir(copypath,mode);
            *sp = '/';
        }
        pp = sp + 1;
    }
    if (status == 0)
    {
        status = do_mkdir(path,mode);
    }
    free(copypath);
    return status;
}

void WQOnBenchmarkBegin(int delay, int time)
{
    printf("benchmark begin\n");
    WQBenchmarkStats_Begin(&g_BenchmarkStats,delay,time);
}

void WQOnBenchmarkClear(void)
{
    printf("benchmark clear\n");
    WQBenchmarkStats_Init(&g_BenchmarkStats);
}

#include "sys_watch.h"

#include "qcommon.h"

void WQOnHostSpawn(void)
{
    INPUT_LOCK
    WQClerAllInputState();
    INPUT_UNLOCK
}

cvar_t *showfps;
cvar_t *swapJumpAndCrouch;

void WQInit(void)
{
    INPUT_INIT
    INPUT_LOCK

    memset(benchmarkBuffer,0,256);
    memset(statsBuffer,0,256);

    WQBenchmarkStats_Init(&g_BenchmarkStats);
    
    g_WQFrameCounter = 0;
    g_WQFrametimeAggregate = 0;
    g_WQLastMeasurementTimeStamp = 0;

    NSBundle* bundle = [NSBundle mainBundle];
    NSString* resourceDir = bundle.resourcePath;
    NSString* commandLine = [[NSUserDefaults standardUserDefaults] stringForKey:@"sys_commandline0"];
    
    if (commandLine == nil)
    {
        commandLine = @"";
    }
    //resourceDir = [resourceDir stringByAppendingString:@"/baseq2"];
    NSString* musicDir = [resourceDir stringByAppendingString:@"/baseq2/music/"];
    NSString* saveDir = [NSSearchPathForDirectoriesInDomains(NSApplicationSupportDirectory,NSUserDomainMask,YES) objectAtIndex:0];
    //NSString* saveDir = [NSSearchPathForDirectoriesInDomains(NSDocumentDirectory,NSUserDomainMask,YES) objectAtIndex:0];
    saveDir = [saveDir stringByAppendingString:@"/watchquake2"];
    CreateFolder([saveDir UTF8String]);
    
    CDAudio_SetPath([musicDir UTF8String]);
    Sys_SetPaths([resourceDir UTF8String] , [resourceDir UTF8String], [saveDir UTF8String], [commandLine UTF8String]);
    Sys_Init();

    WQInitGameScreen();

    prevTime = GetTimeNanoSeconds();
    TimeStepAccumulator_Set(&logicAcc,60,2);

    g_GameStats.frameCounter = 0;
    g_GameStats.meanFrameTime = 0.0f;
    g_GameStats.width = g_WQVidScreenWidth;
    g_GameStats.height = g_WQVidScreenHeight;

    WQInputInit(&g_WQPlayerInput);

    memset(g_WQPanEventPositions,0,sizeof(g_WQPanEventPositions));
    g_WQPanEventStates = 0;
    g_WQNumPanEvents = 0;

    memset(g_WQSlideEventSpeeds,0,sizeof(g_WQSlideEventSpeeds));
    g_WQNumSlideEvents = 0;

    g_WQPanMode = 0;
    g_WQCrouchState = 0;

    INPUT_UNLOCK
}

int wqLoopSet = 0;

void WQSetLoop(void)
{
    INPUT_LOCK
    if (!wqLoopSet)
    {
        char* args[] = {"","","",""};
        Qcommon_Init(0,args);
        
        Cmd_ExecuteString ("unbindall");
        
        showfps = Cvar_Get ("cl_showfps", "0", 0);
        swapJumpAndCrouch = Cvar_Get("cl_jumpandcrouch","0",CVAR_ARCHIVE);
        
        int threads = GetHardwareConcurrency();
        printf("hardware concurrency %i\n",threads);
        SetGameUpdateTasks(&GameLoop,&WQAudioMixerLoop,"ge.WatchQuake2.queue");
        SetGameLoopState(1);
        wqLoopSet = 1;
    }
    INPUT_UNLOCK
}

u64 frametimeStart;
u64 q2DeltaTime_ms;
float q2DeltaTimeF;

int WQShowFPS(void)
{
    if (showfps)
    {
        return showfps->value > 0.5f ? 1 : 0;
    }
    return 0;
}

extern unsigned sys_msg_time;

void WQTick(void)
{
    frametimeStart = GetTimeNanoSeconds();
    u64 dt = frametimeStart - prevTime;
    q2DeltaTime_ms = dt / 1000000;
    if (q2DeltaTime_ms > 16)
    {
        q2DeltaTime_ms = 16;
    }
    q2DeltaTimeF = q2DeltaTime_ms / 1000.0;
    prevTime = frametimeStart;
    TimeStepAccumulator_Update(&logicAcc,dt);
}

bool WQNeedsUpdate(void)
{
    return TimeStepAccumulator_Tick(&logicAcc);
}

#include "qcommon.h"

void WQKeyEvent(int key, int pressed)
{
    Key_Event(key,pressed,sys_msg_time);
}

void WQGiveAll(void)
{
    Cmd_ExecuteString("give all");
}

void WQShowHelp(void)
{
    Cmd_ExecuteString("cmd help");
}

void WQJumpCommand(int pressed)
{
    if (pressed)
    {
        Cmd_ExecuteString("+moveup");
        g_WQJumpTimer = 0.5f;
    }
    else
    {
        Cmd_ExecuteString("-moveup");
    }
}
void WQCrouchCommand(void)
{
    if (!g_WQCrouchState)
    {
        Cmd_ExecuteString("+movedown");
    }
    else
    {
        Cmd_ExecuteString("-movedown");
    }
    g_WQCrouchState = !g_WQCrouchState;
}

void WQArrowLeft(int pressed)
{
    if (pressed)
    {
        Cmd_ExecuteString("+left");
    }
    else
    {
        Cmd_ExecuteString("-left");
    }
}

void WQArrowRight(int pressed)
{
    if (pressed)
    {
        Cmd_ExecuteString("+right");
    }
    else
    {
        Cmd_ExecuteString("-right");
    }
}

void WQArrowUp(int pressed)
{
    if (pressed)
    {
        Cmd_ExecuteString("+forward");
    }
    else
    {
        Cmd_ExecuteString("-forward");
    }
}

void WQArrowDown(int pressed)
{
    if (pressed)
    {
        Cmd_ExecuteString("+back");
    }
    else
    {
        Cmd_ExecuteString("-back");
    }
}

void WQSwitchWeaponCommand(int next)
{
    if (next)
    {
        Cmd_ExecuteString("weapnext");
    }
    else
    {
        Cmd_ExecuteString("weapprev");
    }
}

void WQSwitchItemCommand(int next)
{
    if (next)
    {
        Cmd_ExecuteString("invnext");
    }
    else
    {
        Cmd_ExecuteString("invprev");
    }
}

void WQUseItemCommand(int cmd)
{
    if (cmd)
    {
        Cmd_ExecuteString("invuse");
    }
    else
    {
        Cmd_ExecuteString("invdrop");
    }
}

void WQShootCommand(int pressed)
{
    if (pressed)
    {
        if (g_WQShootTimer <= 0.0f)
        {
            Cmd_ExecuteString ("+attack 133");
            g_WQAnyKey = 1;
            g_WQShootTimer = 0.5f;
        }
    }
    else
    {
        Cmd_ExecuteString ("-attack 133");
    }
}

void WQArrowCommand(int up)
{
    if (up)
    {
        if (g_WQArrowTimer <= 0.0)
        {
            WQKeyEvent(KEY_UPARROW,true);
            g_WQArrowTimer = 0.2;
        }
        WQKeyEvent(KEY_DOWNARROW,false);
    }
    else
    {
        if (g_WQArrowTimer <= 0.0)
        {
            WQKeyEvent(KEY_DOWNARROW,true);
            g_WQArrowTimer = 0.2;
        }
        WQKeyEvent(KEY_UPARROW,false);
    }
}

void WQUpdateSpeed(float* f, float d)
{
    if (*f > 0.0)
    {
        *f = *f - d;
        if (*f < 0.0)
        {
            *f = 0.0;
        }
    }
    else if (*f < 0.0)
    {
        *f = *f + d;
        if (*f > 0.0)
        {
            *f = 0.0;
        }
    }
    *f = *f > 2.0 ? 2.0 : *f;
    *f = *f < -2.0 ? 2.0 : *f;
}

void WQUpdate(void)
{
    float logicDT = q2DeltaTime_ms / 1000.0f;
    // Input handling
    // lock needed, swiftui should be running in another thread
    INPUT_LOCK

    if (cls.key_dest != prevDest)// switched input destination (game, menu, etc.)
    {
        // cancel shoot command
        g_WQFullAuto = qFalse;
        g_WQShootTimer = 1.0f;
        WQShootCommand(qFalse);
        //
        //WQClerAllInputState();
    }
    else if (cls.key_dest == key_game)
    {
        if (g_WQPlayerInput.keys & WQ_INPUT_FIRE)
        {
            if (g_WQTicksSinceTap < 20)// double tap
            {
                if (g_WQFullAuto)
                {
                    g_WQFullAuto = qFalse;
                    WQShootCommand(qFalse);
                    g_WQShootTimer = 0.0f;
                }
                else
                {
                    g_WQFullAuto = qTrue;
                    WQShootCommand(qTrue);
                }
                // don't reset g_QWTicksSinceTap here, otherwise triple tap counts as double tap twise in row!
            }
            else // single tap
            {
                if (g_WQFullAuto)
                {
                    g_WQFullAuto = qFalse;
                    WQShootCommand(qFalse);
                    g_WQShootTimer = 0.0;
                }
                else
                {
                    WQShootCommand(qTrue);
                }
                g_WQTicksSinceTap = 0;
            }
        }
        if (g_WQPlayerInput.keys & (WQ_INPUT_NEXTWEAPON | WQ_INPUT_PREVWEAPON))
        {
            WQSwitchWeaponCommand(g_WQPlayerInput.keys & WQ_INPUT_NEXTWEAPON);
        }
        if (g_WQPlayerInput.keys & WQ_INPUT_JUMP)
        {
            WQJumpCommand(qTrue);
        }
        if (g_WQPlayerInput.keys & WQ_INPUT_HELP)
        {
            WQShowHelp();
        }
        if (g_WQPlayerInput.keys & WQ_INPUT_CROUCH)
        {
            WQCrouchCommand();
        }
        if (g_WQPlayerInput.keys & (WQ_INPUT_INV_NEXT | WQ_INPUT_INV_PREV))
        {
            WQSwitchItemCommand(g_WQPlayerInput.keys & WQ_INPUT_INV_NEXT);
        }
        if (g_WQPlayerInput.keys & WQ_INPUT_INV_USE)
        {
            WQUseItemCommand(1);
        }
        if (g_WQPlayerInput.keys & WQ_INPUT_CHEAT)
        {
            WQGiveAll();
        }
        g_WQForwardSpeed += g_WQPlayerInput.vel * (4.0f / 32767.0f);
        g_WQForwardSpeed = g_WQForwardSpeed > 1.0f ? 1.0f : g_WQForwardSpeed;
        g_WQForwardSpeed = g_WQForwardSpeed < -1.0f ? -1.0f : g_WQForwardSpeed;
        if (g_WQJumpTimer > 0.0f)
        {
            g_WQJumpTimer -= logicDT;
            if (g_WQJumpTimer <= 0.0f)
            {
                WQJumpCommand(qFalse);
            }
        }
        if (!g_WQFullAuto && g_WQShootTimer > 0.0f)
        {
            g_WQShootTimer -= logicDT;
            if (g_WQShootTimer <= 0.0f)
            {
                WQShootCommand(qFalse);
            }
        }
        g_WQTurnX = 0.0f;
        g_WQTurnY = 0.0f;
        for (int i=0; i<g_WQNumPanEvents; i++)
        {
            g_WQPanState = g_WQPanEventStates & 0x01;
            g_WQCurPanPoint = g_WQPanEventPositions[i];
            if (g_WQPanState)
            {
                if (!g_WQPrevPanState)// touch started
                {
                    g_WQPrevPanPoint = g_WQCurPanPoint;
                }
            }
            else // touch ended
            {
                g_WQPrevPanPoint = g_WQCurPanPoint;
            }
            g_WQPrevPanState = g_WQPanState;
            
            if (g_WQPanState)
            {
                CGPoint vel;
                vel.x =  g_WQCurPanPoint.x - g_WQPrevPanPoint.x;
                vel.y =  g_WQCurPanPoint.y - g_WQPrevPanPoint.y;
                g_WQPrevPanPoint = g_WQCurPanPoint;
                if ((vel.x * vel.x + vel.y * vel.y) > 0.1)// stop drifting
                {
                    g_WQTurnX += vel.x;
                    g_WQTurnY += vel.y;
                }
            }
            else
            {
                g_WQTurnX = 0.0f;
                g_WQTurnY = 0.0f;
            }
            g_WQPanEventStates >>= 1;
        }
        for (int i=0; i<g_WQNumSlideEvents; i++)
        {
            g_WQStrafeSpeed += g_WQSlideEventSpeeds[i] * (4.0f / 32767.0f);
            g_WQStrafeSpeed = g_WQStrafeSpeed > 1.0f ? 1.0f : g_WQStrafeSpeed;
            g_WQStrafeSpeed = g_WQStrafeSpeed < -1.0f ? -1.0f : g_WQStrafeSpeed;
        }
    }

    if (g_WQPlayerInput.keys & WQ_INPUT_MENU_ENTER)
    {
        WQKeyEvent(KEY_ENTER,qTrue);
        WQKeyEvent(KEY_ENTER,qFalse);
    }
    if (g_WQPlayerInput.keys & WQ_INPUT_MENU_ESCAPE)
    {
        WQKeyEvent(KEY_ESCAPE,qTrue);
        WQKeyEvent(KEY_ESCAPE,qFalse);
    }
    if (g_WQPlayerInput.keys & WQ_INPUT_MENU_LEFT)
    {
        WQKeyEvent(KEY_LEFTARROW,qTrue);
        WQKeyEvent(KEY_LEFTARROW,qFalse);
    }
    if (g_WQPlayerInput.keys & WQ_INPUT_MENU_RIGHT)
    {
        WQKeyEvent(KEY_RIGHTARROW,qTrue);
        WQKeyEvent(KEY_RIGHTARROW,qFalse);
    }
    if (g_WQPlayerInput.keys & (WQ_INPUT_MENU_UP | WQ_INPUT_MENU_DOWN))
    {
        WQArrowCommand(g_WQPlayerInput.keys & WQ_INPUT_MENU_UP);
    }
    if (g_WQArrowTimer > 0.0f)
    {
        g_WQArrowTimer -= logicDT;
        if (g_WQArrowTimer <= 0.0f)
        {
            WQKeyEvent(KEY_UPARROW,qFalse);
            WQKeyEvent(KEY_DOWNARROW,qFalse);
        }
    }

    g_WQTicksSinceTap++;

    prevDest = cls.key_dest;// remember where input was directed last frame

    WQInputClear(&g_WQPlayerInput);

    memset(g_WQPanEventPositions,0,sizeof(g_WQPanEventPositions));
    g_WQPanEventStates = 0;
    g_WQNumPanEvents = 0;

    memset(g_WQSlideEventSpeeds,0,sizeof(g_WQSlideEventSpeeds));
    g_WQNumSlideEvents = 0;
    
    //g_WQMaxInputsPerFrame = g_WQInputsPerFrame > g_WQMaxInputsPerFrame ? g_WQInputsPerFrame : g_WQMaxInputsPerFrame;
    //g_WQInputsPerFrame = 0;

    float friction = Cvar_VariableValue("movement_friction");
    WQUpdateSpeed(&g_WQForwardSpeed, friction * (1.0 / 60.0f));
    WQUpdateSpeed(&g_WQStrafeSpeed, friction * (1.0 / 60.0f));
    
    INPUT_UNLOCK
    Qcommon_Frame((int)q2DeltaTime_ms);
}

void refresh_screen(void*);

void WQEndFrame(void)
{
    dispatch_async_f(dispatch_get_main_queue(),NULL,refresh_screen);
    u64 frametimeEnd = GetTimeNanoSeconds();
    u64 frametimeU64 = frametimeEnd - frametimeStart;
    g_WQFrametimeAggregate += frametimeU64;
    WQBenchmarkStats_Update(&g_BenchmarkStats,frametimeEnd,frametimeU64);
}

void WQInputTapAndPan(CGPoint location, int type)
{
    INPUT_LOCK
    if (type == -1)// cancel
    {
        // clear buffered pan events
        memset(g_WQPanEventPositions,0,sizeof(g_WQPanEventPositions));
        g_WQPanEventStates = 0;
        g_WQNumPanEvents = 1;

        memset(g_WQSlideEventSpeeds,0,sizeof(g_WQSlideEventSpeeds));
        g_WQNumSlideEvents = 0;

        g_WQPanMode = WQ_PAN_MODE_NONE;
    }
    else
    {
        bool addPan = g_WQPanMode <= 1;
        CGFloat x = g_GameStats.devWidth / 2.0f;
        CGFloat x2 = x / 2.0f;
        x += x2;
        CGFloat y = g_GameStats.devHeight - g_GameStats.devHeight / 10.0;// * 0.18f;// / 10.0;
        if (type == 2)// tapping
        {
            if (location.y < g_GameStats.devHeight / 10.0)// upper part of the screen
            {
                g_WQPlayerInput.keys |= WQ_INPUT_HELP;
                //g_WQPlayerInput.keys |= location.x > x2 ? WQ_INPUT_SWIM_UP : WQ_INPUT_SWIM_DOWN;
            }
            else if (location.x > g_GameStats.devWidth / 2.0 && location.y > g_GameStats.devHeight * 0.15 && location.y < g_GameStats.devHeight * 0.25)// item select
            {
                g_WQPlayerInput.keys |= WQ_INPUT_INV_USE;
            }
            else if (location.y > y)// lower part of the screen, inventory
            {
                int swapInput = 0;
                if (swapJumpAndCrouch)
                {
                    swapInput = swapJumpAndCrouch->value > 0.5f ? 1 : 0;
                }
                if (location.x < g_GameStats.devWidth / 2)
                {
                    g_WQPlayerInput.keys |= swapInput ? WQ_INPUT_JUMP : WQ_INPUT_CROUCH;
                }
                else
                {
                    g_WQPlayerInput.keys |= swapInput ? WQ_INPUT_CROUCH : WQ_INPUT_JUMP;
                }
            }
            else
            {
                g_WQPlayerInput.keys |= WQ_INPUT_MENU_ENTER;
                g_WQPlayerInput.keys |= WQ_INPUT_FIRE;
            }
            g_WQPanMode = 0;
        }
        else if (type == 1)// pan update
        {
            if (g_WQPanMode == WQ_PAN_MODE_NONE)
            {
                if (location.y < g_GameStats.devHeight / 10.0)// upper part of the screen
                {
                    addPan = false;
                    g_WQSlideStart.x = g_WQSlidePos.x = location.x;
                    g_WQSlideStart.y = g_WQSlidePos.y = location.y;
                    g_WQPanMode = WQ_PAN_MODE_WEAPONSELECT;
                }
                else if (location.x > g_GameStats.devWidth / 2.0 && location.y > g_GameStats.devHeight * 0.15 && location.y < g_GameStats.devHeight * 0.25)// item select
                {
                    addPan = false;
                    g_WQSlideStart.x = g_WQSlidePos.x = location.x;
                    g_WQSlideStart.y = g_WQSlidePos.y = location.y;
                    g_WQPanMode = WQ_PAN_MODE_ITEMSELECT;
                }
                else if (location.y > y)// lower part of the screen, inventory
                {
                    addPan = false;
                    g_WQSlideStart.x = g_WQSlidePos.x = location.x;
                    g_WQSlideStart.y = g_WQSlidePos.y = location.y;
                    g_WQPanMode = WQ_PAN_MODE_STRAFE;
                }
                else
                {
                    if (g_WQPanMode != WQ_PAN_MODE_WEAPONSELECT)
                    {
                        g_WQPanMode = WQ_PAN_MODE_AIM;
                    }
                }
            }
            else
            {
                if (g_WQPanMode == WQ_PAN_MODE_AIM)
                {
                    
                }
                else if (g_WQPanMode == WQ_PAN_MODE_WEAPONSELECT)
                {
                    addPan = false;
                    g_WQSlidePos.x = location.x;
                    g_WQSlidePos.y = location.y;
                }
                else if (g_WQPanMode == WQ_PAN_MODE_ITEMSELECT)
                {
                    addPan = false;
                    g_WQSlidePos.x = location.x;
                    g_WQSlidePos.y = location.y;
                }
                else if (g_WQPanMode == WQ_PAN_MODE_STRAFE)
                {
                    addPan = false;
                    if (g_WQNumSlideEvents < 8)
                    {
                        float delta = location.x - g_WQSlidePos.x;
                        delta /= g_GameStats.devWidth;
                        int vel = (delta * 1.0f) * 32767;
                        vel = vel < -32767 ? -32767 : vel;
                        vel = vel > 32767 ? 32767 : vel;
                        g_WQSlideEventSpeeds[g_WQNumSlideEvents] = vel;
                        ++g_WQNumSlideEvents;
                    }
                    g_WQSlidePos.x = location.x;
                    g_WQSlidePos.y = location.y;
                }
            }
        }
        else if (type == 0)// pan end
        {
            if (g_WQPanMode == WQ_PAN_MODE_WEAPONSELECT)// upper part of the screen
            {
                addPan = false;
                float diff = location.x - g_WQSlideStart.x;
                if (diff > 4.0)
                {
                    g_WQPlayerInput.keys |= WQ_INPUT_NEXTWEAPON;
                }
                else if (diff < -4.0)
                {
                    g_WQPlayerInput.keys |= WQ_INPUT_PREVWEAPON;
                }
            }
            else if (g_WQPanMode == WQ_PAN_MODE_ITEMSELECT)
            {
                addPan = false;
                float diff = location.x - g_WQSlideStart.x;
                if (diff > 4.0)
                {
                    g_WQPlayerInput.keys |= WQ_INPUT_INV_NEXT;
                }
                else if (diff < -4.0)
                {
                    g_WQPlayerInput.keys |= WQ_INPUT_INV_PREV;
                }
            }
            else if (g_WQPanMode == WQ_PAN_MODE_STRAFE)// lower part of the screen, inventory
            {
                addPan = false;
                float diff = location.x - g_WQSlideStart.x;
                if (diff > 4.0)
                {
                    g_WQPlayerInput.keys |= WQ_INPUT_MENU_RIGHT;
                }
                else if (diff < -4.0)
                {
                    g_WQPlayerInput.keys |= WQ_INPUT_MENU_LEFT;
                }
            }
            g_WQPanMode = WQ_PAN_MODE_NONE;
        }
        
        // if tapping, type == 0
        // else type 0-1
        if (addPan && g_WQNumPanEvents < 8)
        {
            g_WQPanEventPositions[g_WQNumPanEvents] = location;
            if ((type & 0x01))
            {
                g_WQPanEventStates |= 0x01 << g_WQNumPanEvents;
            }
            ++g_WQNumPanEvents;
        }
        
        //++g_WQInputsPerFrame;
    }
    INPUT_UNLOCK
}

void WQInputLongPress(void)
{
    INPUT_LOCK

    g_WQPlayerInput.keys |= WQ_INPUT_MENU_ESCAPE;
    //++g_WQInputsPerFrame;
    INPUT_UNLOCK
}

void WQInputCrownRotate(float f, float delta)
{
    INPUT_LOCK

    int vel = (delta * 0.025f) * 32767;
    vel = vel < -32767 ? -32767 : vel;
    vel = vel > 32767 ? 32767 : vel;
    g_WQPlayerInput.vel = vel;// 16bits should be enough for everyone...
    
    if (delta < 0)
    {
        g_WQPlayerInput.keys |= WQ_INPUT_MENU_DOWN;
    }
    else if (delta > 0)
    {
        g_WQPlayerInput.keys |= WQ_INPUT_MENU_UP;
    }
    //++g_WQInputsPerFrame;
    INPUT_UNLOCK
}

extern int g_WQAudio_freq;
extern int g_WQAudio_bits;
extern int g_WQAudio_channels;
extern int g_WQAudio_interleaved;
extern int g_WQAudio_type;

extern int sndDesiredSamples;

int WQGetFrameBufferLength(void)
{
    // currently only handling mono sound
    uint samples = 0;
    uint freq = g_WQAudio_freq;
    // 11025 -> 256  samples
    // 22050 -> 512  samples
    // 44100 -> 1024 samples
    // 48000 -> 2048 samples
    // 96000 -> 4096 samples
    if (freq <= 11025)
    {
        samples = 256;
    }
    else if (freq <= 22050)
    {
        samples = 512;
    }
    else if (freq <= 44100)
    {
        samples = 1024;
    }
    else if (freq <= 56000)
    {
        samples = 2048;// 48 kHz
    }
    else
    {
        samples = 4096;// 96 kHz
    }
    sndDesiredSamples = samples;
    while ((sndDesiredSamples % 16) != 0)
    {
        sndDesiredSamples++;
    }
    return sndDesiredSamples;
}

void WQSetAudioFormat(int Hz, uint bits, int channels, int interleaved, int type)
{
    g_WQAudio_freq = Hz;
    g_WQAudio_bits = bits;
    g_WQAudio_channels = channels;
    g_WQAudio_interleaved = interleaved;
    g_WQAudio_type = type;

    CDAudio_SetMixerSamplerate(Hz);
}

void refresh_mapselect(void*);

void WQOnMapsNavigate(void)
{
    dispatch_async_f(dispatch_get_main_queue(),NULL,refresh_mapselect);
}

int M_IsInMapSelect(void);
const char* M_GetSelectedMapName(void);

const char* WQGetSelectedMapName(void)
{
    if (M_IsInMapSelect())
    {
        return M_GetSelectedMapName();
    }
    return 0;
}
