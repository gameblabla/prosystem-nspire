// Copyright 2005 Greg Stanton
// 
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

#include "ProSystem.h"
#include "Sound.h"
#define SOUND_SOURCE "Sound.cpp"

#ifdef SOUND_ENABLED

SDL_AudioSpec spec;
SDL_AudioSpec obtained;
	
uint sound_lenght;
#define FEQSND 44100
#define MAX_SAMPLE_SIZE 524
//(FEQSND/60)
unsigned char a320_sound_emu[MAX_SAMPLE_SIZE];  // Sound buffer
unsigned char rgBuffer[16*MAX_SAMPLE_SIZE];
unsigned short rgproduced, rgsize, rgconsumed;

#endif

// ----------------------------------------------------------------------------
// Ring buffer
// ----------------------------------------------------------------------------
void ringBuffer_Produce(byte * data, size_t dataSize)
{
#ifdef SOUND_ENABLED
	byte *ptrSnd = (byte *) data;
  
    while (dataSize) {
      rgBuffer[rgproduced] = *(ptrSnd++);
      rgproduced=(rgproduced+1>=rgsize ? 0 : rgproduced+1);
      dataSize--;
    }
#endif
}

void ringBuffer_consume(byte * data, size_t dataSize)
{
#ifdef SOUND_ENABLED
	byte *ptrSnd = (byte *) data;
  
    while (dataSize) {
      *(ptrSnd++) = rgBuffer[rgconsumed];
	  rgBuffer[rgconsumed] = 0;
      rgconsumed=(rgconsumed+1>=rgsize ? 0 : rgconsumed+1);
      dataSize--;
    }
#endif
}

// ----------------------------------------------------------------------------
// Resample
// ----------------------------------------------------------------------------
static uint sound_GetSampleLength(uint length, uint unit, uint unitMax) {
#ifdef SOUND_ENABLED
  uint sampleLength = length / unitMax;
  uint sampleRemain = length % unitMax;
  if(sampleRemain != 0 && sampleRemain >= unit) {
    sampleLength++;
  }
  return sampleLength;
#else
	return 0;
#endif
}

static void sound_Resample(const byte* source, byte* target, int length) {
#ifdef SOUND_ENABLED
  int measurement = obtained.freq;
  int sourceIndex = 0;
  int targetIndex = 0;
  
  while(targetIndex < length) {
    if(measurement >= 31440) {
      target[targetIndex++] = source[sourceIndex];
      measurement -= 31440;
    }
    else {
      sourceIndex++;
      measurement += obtained.freq;
    }
  }
#endif
}

void sound_Store() 
{ 
#ifdef SOUND_ENABLED
	byte sample[1920];
	unsigned int i;
	
	unsigned int length = sound_GetSampleLength(obtained.freq, FPS, prosystem_frequency);; //obtained.freq/prosystem_frequency;
	sound_Resample(tia_buffer, sample, length);

  	if(cartridge_pokey) {
  		byte pokeySample[1920];
  		sound_Resample(pokey_buffer, pokeySample, length);
  		for(i = 0; i < length; i++) {
  			sample[i] += pokeySample[i];
  			sample[i] = sample[i] / 2;
  		}
  	}
  	
	ringBuffer_Produce(sample, length);
#endif
}

void audio_callback(void *userdata, Uint8 *stream, int len) {
#ifdef SOUND_ENABLED
	ringBuffer_consume(stream, len);
#endif
}

void sound_InitSDL(void) {
#ifdef SOUND_ENABLED
	SDL_InitSubSystem(SDL_INIT_AUDIO);	 

	spec.freq = FEQSND;
	spec.format = AUDIO_U8;
	spec.channels = 1;
	spec.samples = MAX_SAMPLE_SIZE;
	spec.callback = audio_callback;
	spec.userdata = NULL;
	
	memset(a320_sound_emu,0x00,sizeof(a320_sound_emu));
	
	if (SDL_OpenAudio(&spec, &obtained) < 0) {
		fprintf(stderr, "Couldn't start audio: %s\n", SDL_GetError());
		exit(1);
	}

	rgproduced = rgconsumed = 0;
	rgsize = 16*MAX_SAMPLE_SIZE;
	memset(rgBuffer,0x00,sizeof(rgBuffer));
#endif
}
