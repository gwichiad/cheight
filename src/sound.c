#include "sound.h"

void audio_tone_init(struct AudioToneGenerator *gen,
                SDL_AudioStream *stream, int frequency, int sample_rate) {
        gen->stream = stream;
        gen->current_sine_sample = 0;
        gen->frequency = frequency;
        gen->sample_rate = sample_rate;
        gen->is_playing = false;
}
void audio_tone_start(struct AudioToneGenerator *gen) {
        gen->is_playing = true;
}
void audio_tone_stop(struct AudioToneGenerator *gen) {
        gen->is_playing = false;
        SDL_ClearAudioStream(gen->stream);
}
void audio_tone_update(struct AudioToneGenerator *gen) {
        if (!gen->is_playing) {
                return;
        }
        int minimum_audio = (gen->sample_rate * sizeof(float)) / 2;
        if (SDL_GetAudioStreamQueued(gen->stream) < minimum_audio) {
                static float samples[512];
                int i;
                for (i = 0; i < SDL_arraysize(samples); i++) {
                        float phase = gen->current_sine_sample * gen->frequency 
                                / (float)gen->sample_rate;
                        samples[i] = SDL_sinf(phase * 2 * SDL_PI_F);
                        gen->current_sine_sample++;
                }
                gen->current_sine_sample %= gen->sample_rate;
                SDL_PutAudioStreamData(gen->stream, samples, sizeof(samples));
        }
}
