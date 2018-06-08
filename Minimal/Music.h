#pragma once

#ifndef MUSIC_H
#define MUSIC_H

#include "al.h"
#include "alc.h"
#include <glm/glm.hpp>
#include <iostream>
//#include <al.h>
//#include <alc.h>
//#include "Gist.h"
#define NUM_BUFFERS 2
//#define NUM_SOURCES 2
#define NUM_SOURCES 1
#define NUM_ENVIRONMENTS 2

ALfloat listenerPos[3] = { 0.0,0.0,4.0 };
ALfloat listenerVel[3] = { 0.0,0.0,0.0 };
ALfloat listenerOri[6] = { 0.0,0.0,1.0, 0.0,1.0,0.0 };

ALfloat source0Pos[] = { -2.0, 0.0, 0.0 };
ALfloat source0Vel[] = { 0.0, 0.0, 0.0 };

ALuint  buffer[NUM_BUFFERS];
ALuint  source[NUM_SOURCES];
ALuint  environment[NUM_ENVIRONMENTS];

ALsizei file_size, freq;
ALenum  format;
ALvoid *data;

ALCdevice* dev;
ALCcontext* ctx;
const int SRATE = 44100;
const int SSIZE = 1024;

ALint sample;
using namespace std;
using namespace glm;

/*
 * Struct that holds the RIFF data of the Wave file.
 * The RIFF data is the meta data information that holds,
 * the ID, size and format of the wave file
 */
struct RIFF_Header {
	char chunkID[4];
	int chunkSize; // size does not include chunkSize or chunkID
	char format[4];
};

/*
 * Struct to hold fmt subchunk data for WAVE files.
 */
struct WAVE_Format {
	char subChunkID[4];
	int subChunkSize;
	short audioFormat;
	short numChannels;
	int sampleRate;
	int byteRate;
	short blockAlign;
	short bitsPerSample;
};

/*
 * Struct to hold the data of the wave file
 */
struct WAVE_Data {
	char subChunkID[4]; // should contain the word data
	int subChunk2Size; // stores the size of the data block
};

class Music {
private:
	const char* file_path;
	
public:
	int i;
	//get the playing state
	ALint playState;
	ALCsizei val = 0;
	ALbyte databuffer[22050];
	ALCdevice *device;
	Music(const char* path) {
		i = 9;
		file_path = path;
	}

	~Music() {

	}

	void CheckError(int op = -1, int _err = 0) {
		int err;
		if (op == -1)
			err = alGetError(); // clear any error messages
		else
			err = _err;
		if (err != AL_NO_ERROR) {
			if (err == AL_INVALID_NAME)
				fprintf(stderr, "Error : Invalid Name\n");
			else if (err == AL_INVALID_ENUM)
				fprintf(stderr, "Error : Invalid Enum\n");
			else if (err == AL_INVALID_VALUE)
				fprintf(stderr, "Error : Invalid Value\n");
			else if (err == AL_INVALID_OPERATION)
				fprintf(stderr, "Error : Invalid Operation\n");
			else if (err == AL_OUT_OF_MEMORY)
				fprintf(stderr, "Error : Out of Memory\n");
			exit(1);
		}
		return;
	}

	bool loadWavFile(const char* filename,
		ALuint* buffer, ALsizei* size, ALsizei* frequency, ALenum* format) {
		fprintf(stderr, "[1] filename = %s\n", filename);
		FILE* soundFile = NULL;
		WAVE_Format wave_format;
		RIFF_Header riff_header;
		WAVE_Data wave_data;
		unsigned char* data = 0;

		soundFile = fopen(filename, "rb");
		// Read in the first chunk into the struct
		fread(&riff_header, sizeof(RIFF_Header), 1, soundFile);

		// Read in the 2nd chunk for the wave info
		fread(&wave_format, sizeof(WAVE_Format), 1, soundFile);

		// check for extra parameters;
		if (wave_format.subChunkSize > 16)
			fseek(soundFile, sizeof(short), SEEK_CUR);

		// Read in the the last byte of data before the sound file
		fread(&wave_data, sizeof(WAVE_Data), 1, soundFile);

		// Allocate memory for data
		data = new unsigned char[wave_data.subChunk2Size];
		fread(data, wave_data.subChunk2Size, 1, soundFile);

		// Now we set the variables that we passed in with the
		// data from the structs
		*size = wave_data.subChunk2Size;
		*frequency = wave_format.sampleRate;

		// The format is worked out by looking at the number of
		// channels and the bits per sample.
		if (wave_format.numChannels == 1) {
			if (wave_format.bitsPerSample == 8)
				*format = AL_FORMAT_MONO8;
			else if (wave_format.bitsPerSample == 16)
				*format = AL_FORMAT_MONO16;
		}
		else if (wave_format.numChannels == 2) {
			if (wave_format.bitsPerSample == 8)
				*format = AL_FORMAT_STEREO8;
			else if (wave_format.bitsPerSample == 16)
				*format = AL_FORMAT_STEREO16;
		}

		// create our openAL buffer and check for success
		alGenBuffers(1, buffer);
		alBufferData(*buffer, *format, (void*)data, *size, *frequency);
		
		// clean up and return true if successful
		fclose(soundFile);
		fprintf(stderr, "load ok\n");
		delete[] data;
		return true;
	}

	void init() {
		alListenerfv(AL_POSITION, listenerPos);
		alListenerfv(AL_VELOCITY, listenerVel);
		alListenerfv(AL_ORIENTATION, listenerOri); 

		// Generate buffers, or else no sound will happen!
		alGenSources(NUM_SOURCES, source);
		CheckError();

		// BGM test
		loadWavFile(file_path, buffer, &file_size, &freq, &format);
		CheckError();

		alSourcef(source[0], AL_PITCH, 1.0f);
		alSourcef(source[0], AL_GAIN, 1.0f);
		alSourcefv(source[0], AL_POSITION, source0Pos);
		alSourcefv(source[0], AL_VELOCITY, source0Vel);
		alSourcei(source[0], AL_BUFFER, buffer[0]);
		//alSourcei(source[0], AL_LOOPING, AL_TRUE); //
		alSourcePlay(source[0]);
		alGetError();
		device = alcCaptureOpenDevice(NULL, SRATE, AL_FORMAT_STEREO16, SSIZE);
		if (alGetError() != AL_NO_ERROR) {
			return;
		}
		alcCaptureStart(device);

		return;
	}

	void play() {
		dev = alcOpenDevice(NULL);
		ctx = alcCreateContext(dev, NULL);
		alcMakeContextCurrent(ctx);
		init();
	}

	void close() {
		alcMakeContextCurrent(NULL);
		alcDestroyContext(ctx);
		alcCloseDevice(dev);
	}

	void parse() {
		alcGetIntegerv(device, ALC_CAPTURE_SAMPLES, (ALCsizei)sizeof(ALint), &sample);
		alcCaptureSamples(device, (ALCvoid *)databuffer, sample);
		cout << databuffer[i]<<i << endl;
		i++;
		/*alcGetIntegerv(dev, ALC_CAPTURE_SAMPLES, (ALCsizei)sizeof(ALint), &val);

		alcCaptureSamples(dev, (ALCvoid *)buffer, val);
		if (val != 0)
		{
			cout << i++ <<val<< endl;
		}*/
			/*alGetSourcei(source[0], AL_SOURCE_STATE, &playState);
			alcCaptureSamples(dev, databuffer, val);

			cout << databuffer[i] << endl;
			i++;
			cout << "val is "<<val << endl;;*/
		
		
	}
};


#endif