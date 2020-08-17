#include <cstdio>
#include <cstddef>
#include <unistd.h>
#include "audio.h"
#include "Sound.h"

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