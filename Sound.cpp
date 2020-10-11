#include <cstdio>
#include <cstddef>
#include <unistd.h>

#include <cstdlib>
#include <cstring>

#include "audio.h"
#include "Sound.h"


// Taken from libgo2/src/audio.c and modified
/*
libgo2 - Support library for the ODROID-GO Advance
Copyright (C) 2020 OtherCrashOverride
This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 2.1 of the License, or (at your option) any later version.
This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Lesser General Public License for more details.
You should have received a copy of the GNU Lesser General Public
License along with this library; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*/
void go2_audio_submit_fix(go2_audio_t* audio, const short* data, int frames)
{
    if (!audio || !audio->isAudioInitialized) return;


    if (!alcMakeContextCurrent(audio->context))
    {
        printf("alcMakeContextCurrent failed.\n");
        return;
    }

    ALint processed = 0;
    while(!processed)
    {
        alGetSourceiv(audio->source, AL_BUFFERS_PROCESSED, &processed);

        if (!processed)
        {
            sleep(0);
            //printf("Audio overflow.\n");
            //return;
        }
    }

    // Iterate over the finished buffers and clear out completed sounds
    // Leave one for our current sound.
    ALuint buff[processed];
    alSourceUnqueueBuffers(audio->source, processed, (ALuint *)&buff);
    for (int i = 0; i < processed - 1; ++i) 
    {
    	alBufferData(buff[i], AL_FORMAT_STEREO16, NULL, 0, audio->frequency);
    	alSourceQueueBuffers(audio->source, 1, (ALuint *)&(buff[i]));
    }

    // Get an available buffer
    ALuint openALBufferID = buff[processed-1];
    //alSourceUnqueueBuffers(audio->source, 1, &openALBufferID);

    ALuint format = AL_FORMAT_STEREO16;

    int dataByteLength = frames * sizeof(short) * SOUND_CHANNEL_COUNT;
    alBufferData(openALBufferID, format, data, dataByteLength, audio->frequency);

    alSourceQueueBuffers(audio->source, 1, &openALBufferID);

    ALint result;
    alGetSourcei(audio->source, AL_SOURCE_STATE, &result);

    if (result != AL_PLAYING)
    {
        alSourcePlay(audio->source);
    }
}

go2_audio_t* go2_audio_create(int frequency)
{
    const int BUFFER_COUNT = 4;
    go2_audio_t* result = (go2_audio_t *)malloc(sizeof(*result));
    if (!result)
    {
        printf("malloc failed.\n");
        goto out;
    }

    memset(result, 0, sizeof(*result));


    result->frequency = frequency;

    result->device = alcOpenDevice(NULL);
    if (!result->device)
    {
        printf("alcOpenDevice failed.\n");
        goto err_00;
    }

    result->context = alcCreateContext(result->device, NULL);
    if (!alcMakeContextCurrent(result->context))
    {
        printf("alcMakeContextCurrent failed.\n");
        goto err_01;
    }

    alGenSources((ALuint)1, &result->source);

    alSourcef(result->source, AL_PITCH, 1);
    alSourcef(result->source, AL_GAIN, 1);
    alSource3f(result->source, AL_POSITION, 0, 0, 0);
    alSource3f(result->source, AL_VELOCITY, 0, 0, 0);
    alSourcei(result->source, AL_LOOPING, AL_FALSE);

    //memset(audioBuffer, 0, AUDIOBUFFER_LENGTH * sizeof(short));

    for (int i = 0; i < BUFFER_COUNT; ++i)
    {
        ALuint buffer;
        alGenBuffers((ALuint)1, &buffer);
        alBufferData(buffer, AL_FORMAT_STEREO16, NULL, 0, frequency);
        alSourceQueueBuffers(result->source, 1, &buffer);
    }

    alSourcePlay(result->source);

    result->isAudioInitialized = true;

    // testing
    //uint32_t vol = go2_audio_volume_get(result);
    //printf("audio: vol=%d\n", vol);
    //go2_audio_path_get(result);


    return result;


err_01:
    alcCloseDevice(result->device);

err_00:
    free(result);

out:
    return NULL;
};

void go2_audio_destroy(go2_audio_t* audio)
{
    alDeleteSources(1, &audio->source);
    alcDestroyContext(audio->context);
    alcCloseDevice(audio->device);

    free(audio);
};