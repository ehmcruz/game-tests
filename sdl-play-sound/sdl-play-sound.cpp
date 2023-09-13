#ifdef __MINGW32__
	#define SDL_MAIN_HANDLED
#endif

//#include <SDL.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>

#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>

using namespace std;

volatile int alive = 1;

SDL_Window *screen;
SDL_Renderer *renderer;

static int audio_open = 0;
static Mix_Music *music = NULL;
static Mix_Chunk *sound_explosion = NULL;

static void CleanUp(int exitcode)
{
	if(Mix_PlayingMusic()) {
		Mix_FadeOutMusic(1500);
		SDL_Delay(1500);
	}
	if (sound_explosion) {
		Mix_FreeChunk(sound_explosion);
		sound_explosion = nullptr;
	}
	if (music) {
		Mix_FreeMusic(music);
		music = NULL;
	}
	if (audio_open) {
		Mix_CloseAudio();
		audio_open = 0;
	}
	SDL_Quit();
	exit(exitcode);
}

int main (int argc, char **argv)
{
	SDL_Event event;
	const Uint8 *keyboard_state_array;
	Uint16 audio_format;
	int audio_channels;
	int audio_buffers;
	int audio_volume = MIX_MAX_VOLUME;
	int audio_rate;
	int looping = 0;
	//double loop_start, loop_end, loop_length, current_position;
	double current_position;

	if (argc != 3) {
		cout << argv[0] << " [mp3 file] [wav file]" << endl;
		cout << "mp3 file for background music" << endl;
		cout << "wav file for sound effects" << endl;
		exit(1);
	}

	audio_rate = 44100; //MIX_DEFAULT_FREQUENCY;
	audio_format = MIX_DEFAULT_FORMAT;
	audio_channels = 2; //MIX_DEFAULT_CHANNELS;
	audio_buffers = 4096;

	SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);

	screen = SDL_CreateWindow("My Game Window",
		SDL_WINDOWPOS_UNDEFINED,
		SDL_WINDOWPOS_UNDEFINED,
		800, 500,
		SDL_WINDOW_OPENGL);

	renderer = SDL_CreateRenderer(screen, -1, 0);

	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
	SDL_RenderClear(renderer);
	SDL_RenderPresent(renderer);

	keyboard_state_array = SDL_GetKeyboardState(NULL);

	if (Mix_OpenAudio(audio_rate, audio_format, audio_channels, audio_buffers) < 0) {
		SDL_Log("Couldn't open audio: %s\n", SDL_GetError());
		return(2);
	}
	else {
		Mix_QuerySpec(&audio_rate, &audio_format, &audio_channels);
		SDL_Log("Opened audio at %d Hz %d bit%s %s %d bytes audio buffer\n", audio_rate,
			(audio_format&0xFF),
			(SDL_AUDIO_ISFLOAT(audio_format) ? " (float)" : ""),
			(audio_channels > 2) ? "surround" : (audio_channels > 1) ? "stereo" : "mono",
			audio_buffers);
	}

	audio_open = 1;

	/* Set the music volume */
	Mix_VolumeMusic(audio_volume);

	const bool rwops = false;
	const char *fname = argv[1];
	const char *typ;

	const char *fname_explosion = (argc == 3) ? argv[2] : nullptr;

	if (rwops) {
		music = Mix_LoadMUS_RW(SDL_RWFromFile(fname, "rb"), SDL_TRUE);
	} else {
		music = Mix_LoadMUS(fname);
	}

	if (music == NULL) {
		SDL_Log("Couldn't load %s: %s\n",
			fname, SDL_GetError());
		CleanUp(2);
	}

	if (fname_explosion) {
		sound_explosion = Mix_LoadWAV(fname_explosion);

		if (sound_explosion == NULL) {
			SDL_Log("Couldn't load %s: %s\n",
				fname_explosion, SDL_GetError());
			CleanUp(2);
		}
	}

	switch (Mix_GetMusicType(music)) {
		case MUS_CMD:
			typ = "CMD";
			break;
		case MUS_WAV:
			typ = "WAV";
			break;
		case MUS_MOD:
		case MUS_MODPLUG_UNUSED:
			typ = "MOD";
			break;
		case MUS_FLAC:
			typ = "FLAC";
			break;
		case MUS_MID:
			typ = "MIDI";
			break;
		case MUS_OGG:
			typ = "OGG Vorbis";
			break;
		case MUS_MP3:
		case MUS_MP3_MAD_UNUSED:
			typ = "MP3";
			break;
		case MUS_OPUS:
			typ = "OPUS";
			break;
//		case MUS_WAVPACK:
//			typ = "WavPack";
//			break;
		case MUS_NONE:
		default:
			typ = "NONE";
			break;
	}
	SDL_Log("Detected music type: %s", typ);

/*	loop_start = Mix_GetMusicLoopStartTime(music);
	loop_end = Mix_GetMusicLoopEndTime(music);
	loop_length = Mix_GetMusicLoopLengthTime(music);

	SDL_Log("Playing %s, duration %f\n", argv[i], Mix_MusicDuration(music));
	if (loop_start > 0.0 && loop_end > 0.0 && loop_length > 0.0) {
		SDL_Log("Loop points: start %g s, end %g s, length %g s\n", loop_start, loop_end, loop_length);
	}*/

	//Mix_FadeInMusic(music, looping, 2000);
	Mix_PlayMusic(music, -1);

	while (alive) {
		while (SDL_PollEvent(&event)) {
			switch (event.type) {
				case SDL_QUIT:
					alive = 0;
					break;

				case SDL_KEYDOWN:
					switch (event.key.keysym.sym) {
						case SDLK_SPACE:
						if (sound_explosion)
							Mix_PlayChannel(-1, sound_explosion, 0);
						break;
					}
			}
		}
	}

	Mix_FreeMusic(music);
	music = NULL;

	CleanUp(0);
	
	return 0;
}