//
//  sys_watch.c
//  WatchQuake2 Watch App
//
//  Created by ByteOverlord on 26.11.2022.
//

#include "sys_watch.h"

#include "qcommon.h"
//#include "winquake.h"
//#include "resource.h"
#include <stdio.h>

#include <errno.h>
#include <float.h>
#include <fcntl.h>
#include <stdio.h>
//#include <direct.h>
//#include <io.h>
//#include <conio.h>
//#include "../win32/conproc.h"

//#define MINIMUM_WIN_MEMORY    0x0a00000
//#define MAXIMUM_WIN_MEMORY    0x1000000
int starttime;
int curtime;
//extern int curtime;

unsigned    sys_msg_time;
unsigned    sys_frame_time;

#define    MAX_NUM_ARGVS    128
int            argc;
char        *argv[MAX_NUM_ARGVS];

extern char    fs_bundledir[MAX_OSPATH];
extern char    fs_savedir[MAX_OSPATH];

void Sys_Error (char *error, ...)
{
    va_list        argptr;

    printf ("Sys_Error: ");
    va_start (argptr,error);
    vprintf (error,argptr);
    va_end (argptr);
    printf ("\n");
    exit(1);
}

void Sys_Quit (void)
{
/*    CL_Shutdown();
    Qcommon_Shutdown ();
    if (dedicated && dedicated->value)
        FreeConsole ();
    DeinitConProc ();*/
    exit(0);
}

char *Sys_ScanForCD (void)
{
    return NULL;
}

void Sys_CopyProtect (void)
{
    
}

void Sys_SetPaths(const char* resourcesDir, const char* documentsDir, const char* saveDir, const char* commandLine)
{
    size_t gamedirlength = strlen(resourcesDir);
    memset(fs_bundledir,0,MAX_OSPATH);
    memcpy(fs_bundledir,resourcesDir,gamedirlength);

    size_t savedirlength = strlen(saveDir);
    memset(fs_savedir,0,MAX_OSPATH);
    memcpy(fs_savedir,saveDir,savedirlength);
}

void Sys_Init(void)
{
    /*if (dedicated->value)
    {
        if (!AllocConsole ())
            Sys_Error ("Couldn't create dedicated server console");
        hinput = GetStdHandle (STD_INPUT_HANDLE);
        houtput = GetStdHandle (STD_OUTPUT_HANDLE);
    
        // let QHOST hook in
        InitConProc (argc, argv);
    }*/
}

static char    console_text[256];
static int    console_textlen;

char *Sys_ConsoleInput (void)
{
/*    if (!dedicated || !dedicated->value)
        return NULL;*/
    return NULL;
}

void Sys_ConsoleOutput (char *string)
{
/*    int        dummy;
    char    text[256];

    if (!dedicated || !dedicated->value)
        return;*/
}

void Sys_SendKeyEvents (void)
{
    // grab frame time
    //sys_frame_time = timeGetTime();    // FIXME: should this be at start?
    sys_frame_time = Sys_Milliseconds();
}

char *Sys_GetClipboardData( void )
{
    return NULL;
}

void Sys_AppActivate (void)
{
    
}

void Sys_UnloadGame (void)
{
    
}

int hunkcount = 0;

byte *membase = NULL;
int hunkmaxsize = 0;
int cursize = 0;

#include "WQ2Common.h"

void *Hunk_Begin (int maxsize)
{
    cursize = 0;
    hunkmaxsize = maxsize;
    membase = malloc(maxsize);
    memset (membase, 0, maxsize);
    if (!membase)
        Sys_Error ("Hunk_Begin reserve failed");
    //printf("Hunk_Begin[%i] %i\n",hunkcount,maxsize);
    return membase;
}

void *Hunk_Alloc (int size)
{
    // round to cacheline
    size = (size+31)&~31;
    cursize += size;
    if (cursize > hunkmaxsize)
    {
        Sys_Error ("Hunk_Alloc overflow");
    }
    //printf("Hunk_Alloc %i\n",size);
    return (void *)(membase+cursize-size);
}

void Hunk_Free (void *base)
{
    if (hunkcount == 0)
    {
        printf("Hunk_Free tried to free when hunkcount == 0\n");
    }
    if (base)
    {
        free(base);
        --hunkcount;
        //printf("Hunk_Free hunkcount %i\n",hunkcount);
    }
    else
    {
        printf("Hunk_Free tried to free NULL pointer\n");
    }
}

int Hunk_End (void)
{
    hunkcount++;
    //printf("Hunk_End %i\n",cursize);
    return cursize;
}

#include "WQ2Common.h"

int Sys_Milliseconds (void)
{
    static u64 base;
    static qboolean init = qFalse;
    if (!init)
    {
        base = GetTimeNanoSeconds();
        init = qTrue;
    }
    curtime = (int)((GetTimeNanoSeconds() - base) / 1000000);
    return curtime;
}

#include <sys/stat.h>

void Sys_Mkdir (char *path)
{
    mode_t mode = S_IRWXU;

    struct stat st;
    int status = 0;

    if (stat(path,&st) != 0)
    {
        if (mkdir(path,mode) != 0 && errno != EEXIST)
        {
            return ;
        }
    }
    else if (!S_ISDIR(st.st_mode))
    {
        errno = ENOTDIR;
        status = -1;
    }
}

#include <dirent.h>

static char    findbase[MAX_OSPATH];
static char    findpath[MAX_OSPATH];
static char    findpattern[MAX_OSPATH];
static DIR* fdir = NULL;

qboolean CompareAttributes(char *path, char *name, unsigned musthave, unsigned canthave)
{
    struct stat st;
    char fn[MAX_OSPATH];

    // . and .. never match
    if (strcmp(name, ".") == 0 || strcmp(name, "..") == 0)
        return qFalse;

    sprintf(fn, "%s/%s", path, name);
    if (stat(fn, &st) == -1)
        return qFalse; // shouldn't happen

    if ( ( st.st_mode & S_IFDIR ) && ( canthave & SFF_SUBDIR ) )
        return qFalse;

    if ( ( musthave & SFF_SUBDIR ) && !( st.st_mode & S_IFDIR ) )
        return qFalse;

    return qTrue;
}

//#include <glob.h>
#include <fnmatch.h>

char *Sys_FindFirst (char *path, unsigned musthave, unsigned canhave)
{
    struct dirent *d;
    char *p;

    if (fdir)
        Sys_Error ("Sys_BeginFind without close");

    /*size_t len = strlen(path);
    memcpy(findpath,path,len);
    findpath[len] = 0;
    return findpath;*/

    strcpy(findbase, path);

    if ((p = strrchr(findbase, '/')) != NULL) {
        *p = 0;
        strcpy(findpattern, p + 1);
    } else
        strcpy(findpattern, "*");

    if (strcmp(findpattern, "*.*") == 0)
        strcpy(findpattern, "*");
        
    if ((fdir = opendir(findbase)) == NULL)
        return NULL;
    while ((d = readdir(fdir)) != NULL) {
        if (!*findpattern || fnmatch(findpattern, d->d_name,0) != FNM_NOMATCH) {
    //            if (*findpattern)
    //                printf("%s matched %s\n", findpattern, d->d_name);
            if (CompareAttributes(findbase, d->d_name, musthave, canhave)) {
                sprintf (findpath, "%s/%s", findbase, d->d_name);
                return findpath;
            }
        }
    }
    return NULL;
}

char *Sys_FindNext (unsigned musthave, unsigned canhave)
{
    struct dirent *d;

    if (fdir == NULL)
        return NULL;
    while ((d = readdir(fdir)) != NULL) {
        if (!*findpattern || fnmatch(findpattern, d->d_name,0) != FNM_NOMATCH) {
//            if (*findpattern)
//                printf("%s matched %s\n", findpattern, d->d_name);
            if (CompareAttributes(findbase, d->d_name, musthave, canhave)) {
                sprintf (findpath, "%s/%s", findbase, d->d_name);
                return findpath;
            }
        }
    }
    return NULL;
}

void Sys_FindClose (void)
{
    if (fdir != NULL)
        closedir(fdir);
    fdir = NULL;
}

#include "g_main.h"

typedef void *(*GetGameAPI_Func) (void *);

void *Sys_GetGameAPI (void *parms)
{
    GetGameAPI_Func getGameApi = (GetGameAPI_Func)&GetGameAPI;
    //getGameApi = (void *)GetProcAddress (game_library, "GetGameAPI");
    if (!getGameApi)
    {
        Sys_UnloadGame();
        return NULL;
    }
    return getGameApi(parms);
}
