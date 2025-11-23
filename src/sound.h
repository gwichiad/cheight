#ifndef SOUND_H
#define SOUND_H

#include <SDL3/SDL.h>

struct AudioToneGenerator {
        SDL_AudioStream *stream;
        int current_sine_sample;
        int frequency;
        int sample_rate;
        bool is_playing;
};

void audio_tone_init(struct AudioToneGenerator *gen, 
                SDL_AudioStream *stream, int frequency, int sample_rate);
void audio_tone_start(struct AudioToneGenerator *gen);
void audio_tone_stop(struct AudioToneGenerator *gen);
void audio_tone_update(struct AudioToneGenerator *gen);

#endif // !SOUND_H
