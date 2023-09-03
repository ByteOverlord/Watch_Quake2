//
//  WQ2SoundCallback.c
//  WatchQuake2 Watch App
//
//  Created by ByteOverlord on 7.12.2022.
//

#include "WQ2SoundCallback.h"

#include "qcommon.h"
#include "sound.h"
#include "snd_loc.h"

#include <pthread/pthread.h>
extern pthread_mutex_t snd_lock;

void* WQAudioMixerLoop(void* p)
{
    SND_LOCK
    S_MixerUpdate();
unlock:
    SND_UNLOCK
    return NULL;
}

extern int sndBufferSize;
extern int sndDesiredSamples;

#include <assert.h>
#include <math.h>

void CheckAudioValue(float f)
{
    if (isnan(f) || isinf(f))
    {
        assert(0);
    }
    if (f < -1.1f)
    {
        assert(0);
    }
    if (f > 1.1f)
    {
        assert(0);
    }
}

#include <string.h>

void WQAudioCallback(void *const *channelData, uint frameCount, uint channelCount, uint bits)
{
    unsigned char* ch0 = (unsigned char*)channelData[0];
    unsigned char* ch1 = (unsigned char*)channelData[1];

    int cached_samplepos = dma->samplepos;
    int cached_samplebits = dma->samplebits;
    int cached_bufferSize = sndBufferSize;
    unsigned char* srcBuffer = dma->buffer;

    int framebufferLength = frameCount;// sndDesiredSamples;

    int pos = cached_samplepos * (cached_samplebits / 8);
    if (pos >= cached_bufferSize)
    {
        dma->samplepos = cached_samplepos = pos = 0;
    }
    int toBuffend = cached_bufferSize - pos;
    int len = framebufferLength * (cached_samplebits / 8);
    int len1 = len;
    int len2 = 0;
    if (len1 > toBuffend)
    {
        len1 = toBuffend;
        len2 = len - len1;
    }
    float* src = __builtin_assume_aligned((float*)(srcBuffer + pos),16);
#ifdef DEBUG
    assert(AlignedCheck(src,16));
    if (dma->flags & DMA_FLAG_IS_FLOAT)
    {
        int len1Samples = len1 / (bits / 8);
        for (int i=0; i<len1Samples; i++)
        {
            CheckAudioValue(src[i]);
        }
    }
#else
#endif
    if (ch0)
    {
        memcpy(ch0,src,len1);
    }
    if (ch1)
    {
        memcpy(ch1,src,len1);
    }
    ch0 += len1;
    ch1 += len1;
    if (len2 <= 0)
    {
        dma->samplepos += len1 / (dma->samplebits / 8);
    }
    else
    {
        src = __builtin_assume_aligned((float*)(srcBuffer),16);
#ifdef DEBUG
        assert(AlignedCheck(src,16));
        if (dma->flags & DMA_FLAG_IS_FLOAT)
        {
            int len2Samples = len2 / (bits / 8);
            for (int i=0; i<len2Samples; i++)
            {
                CheckAudioValue(src[i]);
            }
        }
#else
#endif
        if (ch0)
        {
            memcpy(ch0,src,len2);
        }
        if (ch1)
        {
            memcpy(ch1,src,len2);
        }
        dma->samplepos = len2 / (dma->samplebits / 8);
    }
    if (dma->samplepos * (dma->samplebits / 8) >= cached_bufferSize)
    {
        dma->samplepos = 0;
    }
}
