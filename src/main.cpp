#include <iostream>
#include <portaudio.h>
#include <math.h>
#define SAMPLE_RATE 44100
#define PLAY_DURATION 5
#define TABLE_SIZE 32


// callback, will repeatedly be called a thourought the program to compute the data
int MusicPlayerCallback(const void* input,
                    void* output,
                    unsigned long
                    frameCount,
                    const PaStreamCallbackTimeInfo* timeInfo,
                    PaStreamCallbackFlags StatusFlags,
                    void* userData);

// stream pointer
PaStream* stream;

// data structure that will allow the storage of the sound data
struct MP_StreamData { float data[TABLE_SIZE]; };
MP_StreamData data;

double frequency = 440.0;
double amplitude = 1.0;

int main(int, char**) {
    // initialize the sine values
    for (size_t i = 0; i < TABLE_SIZE; i++)
    {
        data.data[i] = (float) (amplitude * sin( frequency * 2.0 * 3.141592653589 * ((double) i) ));
        std::cout << data.data[i] << "\n";
    }
    

    // init the library and check for errors
    PaError err = Pa_Initialize();
    if (err != paNoError) std::cout << "PortAudio error : " << Pa_GetErrorText(err);

    // open a stream
    err = Pa_OpenDefaultStream(&stream, 0, 1, paFloat32, SAMPLE_RATE, 64, MusicPlayerCallback, &data);

    err = Pa_StartStream(stream);
    Pa_Sleep(PLAY_DURATION * 1000);
    err = Pa_CloseStream(stream);

    // uninit the library
    err = Pa_Terminate();
    if (err != paNoError) std::cout << "PortAudio error : " << Pa_GetErrorText(err);

    return 0;
}


int MusicPlayerCallback(const void* input,
                    void* output,
                    unsigned long
                    frameCount,
                    const PaStreamCallbackTimeInfo* timeInfo,
                    PaStreamCallbackFlags StatusFlags,
                    void* userData)
{
    MP_StreamData* data = (MP_StreamData*) userData;
    float* out = (float*) output;

    unsigned int i;
    (void) input;

    for (i = 0; i < frameCount; i++)
    {
        *out++ = data->data[(int)i];
    }
    
    return 0;
}