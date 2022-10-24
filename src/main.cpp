#include <iostream>
#include <portaudio.h>
#include <math.h>
#define SAMPLE_RATE 44100
#define PLAY_DURATION 2
#define TABLE_SIZE 100
// FREQ = SAMPLE_RATE/TABLE_SIZE

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
struct MP_StreamData { float data; };
MP_StreamData data;

double base_frequency = 110.0; // La 2
double amplitude = 0.3;

double semitones = 24;
double _12thRootOf2  = pow(2.0, 1.0/12.0);
double frequency = base_frequency * pow(_12thRootOf2, semitones);

int main(int, char**) {
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

    (void) input;
    (void) timeInfo;
    (void) StatusFlags;

    float outValue = 0;

    static unsigned long n;

    for (unsigned long i = 0; i < frameCount; i++, n++)
    {

        data->data = (float) (amplitude * sin(n * frequency * 2 * 3.141592653589 / (double) SAMPLE_RATE));

        if (data->data > 0)
        {
            outValue = 0.2;
        }
        else
        {
            outValue = -0.2;
        }

        *out++ = outValue;
    }
    
    return 0;
}