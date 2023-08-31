#include <SDL2/SDL.h>
#include <SDL2/SDL_audio.h>

#include <emscripten.h>

#include <emscripten/em_macros.h>
#include <stdio.h>
#include "z_libpd.h"

// ==============================================
// ============== UTILS =========================
// ==============================================
EMSCRIPTEN_KEEPALIVE
int pd_sendFloat(char *receiver, float f) {
    int result = libpd_float(receiver, f);
    return result;
}

// ==============================================
EMSCRIPTEN_KEEPALIVE
int pd_sendSymbol(char *receiver, char *thing) {
    int result = libpd_symbol(receiver, thing);
    return result;
}

// ===============================
// ========= MEMORY ==============
// ===============================
EMSCRIPTEN_KEEPALIVE // expose strlen to javascript
int webpd_strlen(char *s) {
    return strlen(s);
}


// expose malloc to javascript
EMSCRIPTEN_KEEPALIVE
void webpd_free(void *ptr) {
    free(ptr);
}

// = expose malloc to javascri
EMSCRIPTEN_KEEPALIVE
void *webpd_malloc(int length) {
    return malloc(length + 1);
}

// set string to the pointer address
EMSCRIPTEN_KEEPALIVE 
void webpd_setString(char *ptr, char *s) {
    strcpy(ptr, s);
}


// ==============================================
void audio(void *userdata, Uint8 *stream, int len){
    float inbuf[64], outbuf[64][2];
    float *b = (float *) stream;
    int m = len / sizeof(float) / 2;
    int k = 0;
    
    printf("Length is %d\n", len);

    while (m > 0) {
        for (int i = 0; i < 64; ++i)
            inbuf[i] = 0;
        libpd_process_float(1, &inbuf[0], &outbuf[0][0]);
        for (int i = 0; i < 64; ++i)
            for (int j = 0; j < 2; ++j)
                b[k++] = outbuf[i][j];
        m -= 64;
    }
    if (m < 0)
        fprintf(stderr, "buffer overflow, m went negative: %d\n", m);
}

// ==============================================
void pdprint(const char *s) {
    printf("%s", s);
}

// ==============================================
int main(int argc, char **argv){
    SDL_Init(SDL_INIT_AUDIO);
    SDL_AudioSpec want, have;
    want.freq = 48000;
    want.format = AUDIO_F32;
    want.channels = 2;
    want.samples = 64;
    want.callback = audio;
    SDL_AudioDeviceID dev = SDL_OpenAudioDevice(NULL, 0, &want, &have, SDL_AUDIO_ALLOW_ANY_CHANGE);

    libpd_set_printhook(pdprint);

    libpd_init();
    libpd_init_audio(1, 2, have.freq);

    libpd_start_message(1); // liga o dsp
    libpd_add_float(1.0f);
    libpd_finish_message("pd", "dsp");

    libpd_openfile("main.pd", "."); // PATCH
    // -----------

    // start audio processing
    SDL_PauseAudioDevice(dev, 0);

    
    return 0;
}
