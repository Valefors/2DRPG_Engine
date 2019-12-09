#define DR_MP3_IMPLEMENTATION
#define DR_WAV_IMPLEMENTATION
#define MINIAUDIO_IMPLEMENTATION

#include <iostream>
#include <wchar.h>
#include <vector>

#include "dr_mp3.h"
#include "dr_wav.h"
#include "miniaudio.h"
#include "maths.h"
#include "globals.h"
#include "audio.h"

typedef struct InternalAudioSource
{
	ma_decoder *decoder;
	ma_device *device;
} InternalAudioSource;

std::vector<InternalAudioSource *> internalAudioSources;

static void free_internal_audio_source(InternalAudioSource *internalAudioSource)
{
	ma_device_uninit(internalAudioSource->device);
	ma_decoder_uninit(internalAudioSource->decoder);

	delete internalAudioSource->decoder;
	delete internalAudioSource->device;
	delete internalAudioSource;
}

static void audio_source_play_callback(ma_device *device, void *output, const void *input, ma_uint32 frameCount)
{
	AudioSource *audioSource = (AudioSource *)device->pUserData;
	InternalAudioSource *internalAudioSource = internalAudioSources[audioSource->id];

	if (!audioSource->playing)
	{
		return;
	}

	ma_uint32 nbFramesRead = ma_decoder_read_pcm_frames(internalAudioSource->decoder, output, frameCount);

	if (nbFramesRead == 0)
	{
		audioSource->playing = false;
		ma_decoder_seek_to_pcm_frame(internalAudioSource->decoder, 0);
	}

	for (unsigned int frame = 0; frame < nbFramesRead; frame++)
	{
		for (unsigned int channel = 0; channel < internalAudioSource->decoder->outputChannels; channel++)
		{
			((float *)output)[internalAudioSource->decoder->outputChannels * frame + channel] *= audioSource->volume;
		}
	}
}

AudioSource *create_audio_source(const char *fileName)
{
	AudioSource *audioSource = new AudioSource;
	InternalAudioSource *internalAudioSource = new InternalAudioSource;
	audioSource->id = -1;
	for (int i = 0; i < internalAudioSources.size(); i++)
	{
		if (!internalAudioSources[i])
		{
			audioSource->id = i;
			internalAudioSources[i] = internalAudioSource;
			break;
		}
	}
	if (audioSource->id == -1)
	{
		audioSource->id = internalAudioSources.size();
		internalAudioSources.push_back(internalAudioSource);
	}
	audioSource->volume = 1.0f;
	audioSource->playing = false;

	internalAudioSource->decoder = new ma_decoder;
	if (ma_decoder_init_file(fileName, NULL, internalAudioSource->decoder) != MA_SUCCESS)
	{
		std::cout << "could not decode " << fileName << ".\n";
		exit(EXIT_FAILURE);
	}

	ma_device_config config = ma_device_config_init(ma_device_type_playback);
	config.playback.format = internalAudioSource->decoder->outputFormat;
	config.playback.channels = internalAudioSource->decoder->outputChannels;
	config.sampleRate = internalAudioSource->decoder->outputSampleRate;
	config.dataCallback = audio_source_play_callback;
	config.pUserData = audioSource;

	internalAudioSource->device = new ma_device;
	if (ma_device_init(NULL, &config, internalAudioSource->device) != MA_SUCCESS)
	{
		ma_decoder_uninit(internalAudioSource->decoder);
		std::cout << "Failed to open playback device.\n";
		exit(EXIT_FAILURE);
	}

	if (ma_device_start(internalAudioSource->device) != MA_SUCCESS)
	{
		ma_device_uninit(internalAudioSource->device);
		ma_decoder_uninit(internalAudioSource->decoder);
		std::cout << "Failed to start playback device.\n";
		exit(EXIT_FAILURE);
	}

	return audioSource;
}

void reset_audio_source(AudioSource *audioSource)
{
	ma_decoder_seek_to_pcm_frame(internalAudioSources[audioSource->id]->decoder, 0);
}

void stop_audio_source(AudioSource *audioSource)
{
	ma_device_stop(internalAudioSources[audioSource->id]->device);
	free_internal_audio_source(internalAudioSources[audioSource->id]);
	internalAudioSources[audioSource->id] = NULL;
}

void free_audio()
{
	for (unsigned int i = 0; i < internalAudioSources.size(); i++)
	{
		if (internalAudioSources[i])
		{
			free_internal_audio_source(internalAudioSources[i]);
		}
	}
	internalAudioSources.clear();
}