//
//  sound_watch.c
//  WatchQuake2 Watch App
//
//  Created by ByteOverlord on 7.12.2022.
//

#include "client.h"
#include "snd_loc.h"

#include "WQ2Common.h"

#include <pthread/pthread.h>
pthread_mutex_t snd_lock;

int sndBufferSize = 0;
int sndDesiredSamples = 0;

int g_WQAudio_freq = 0;//48000;
int g_WQAudio_bits = 0;//32;
int g_WQAudio_channels = 0;//1;
int g_WQAudio_interleaved = 0;
int g_WQAudio_type = 0;

extern volatile dma_t* dma;

qboolean SNDDMA_Init(void)
{
    pthread_mutex_init(&snd_lock, NULL);
    pthread_mutex_lock(&snd_lock);

    // currently only handling mono sound
    uint samples = 0;
    uint freq = g_WQAudio_freq;
    uint channels = 1;//g_WQAudio_channels;
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

    uint tmp = samples * channels * 10;
    if (tmp & (tmp - 1))
    {
        // make it a power of two
        uint val = 1;
        while (val < tmp)
        {
            val <<= 1;
        }
        tmp = val;
    }

    dma = malloc(sizeof(dma_t));
    dma->samplebits = g_WQAudio_bits;
    dma->speed = freq;
    dma->channels = channels;
    dma->samples = tmp;// mono samples in buffer
    dma->samplepos = 0;
    dma->flags = 0;
    if (g_WQAudio_interleaved)
    {
        dma->flags |= DMA_FLAG_IS_INTERLEAVED;
    }
    if (g_WQAudio_type)
    {
        dma->flags |= DMA_FLAG_IS_FLOAT;
    }
    dma->submission_chunk = 1;
    sndDesiredSamples = samples;
    while ((sndDesiredSamples % 16) != 0)
    {
        sndDesiredSamples++;
    }
    sndBufferSize = dma->samples * (dma->samplebits / 8);
    dma->buffer = AlignedMalloc(sndBufferSize,16);
    memset(dma->buffer,0,sndBufferSize);

    pthread_mutex_unlock(&snd_lock);

    return qTrue;
}

int SNDDMA_GetDMAPos(void)
{
    return dma->samplepos;
}

void SNDDMA_Shutdown(void)
{
    AlignedFree(dma->buffer);
}

void SNDDMA_BeginPainting (void)
{
}

void SNDDMA_Submit(void)
{
}
