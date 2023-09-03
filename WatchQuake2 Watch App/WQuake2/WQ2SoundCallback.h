//
//  WQ2SoundCallback.h
//  WatchQuake2 Watch App
//
//  Created by ByteOverlord on 7.12.2022.
//

#ifndef WQ2SoundCallback_h
#define WQ2SoundCallback_h

#include "WQ2Common.h"

void* WQAudioMixerLoop(void* p);
void WQAudioCallback(void *const *channelData, uint frameCount, uint channelCount, uint bits);

#endif /* WQ2SoundCallback_h */
