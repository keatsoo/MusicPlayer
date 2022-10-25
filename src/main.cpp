#include <iostream>
#include <portaudio.h>
#include <math.h>
#include <chrono>
#define SAMPLE_RATE 44100
#define PLAY_DURATION 1
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

double timePassed();
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

double tempo = 60;

std::chrono::time_point<std::chrono::high_resolution_clock> start;
std::chrono::time_point<std::chrono::high_resolution_clock> now;


int main(int, char**) {
    // init the library and check for errors
    PaError err = Pa_Initialize();
    if (err != paNoError) std::cout << "PortAudio error : " << Pa_GetErrorText(err);

    start = std::chrono::high_resolution_clock::now();

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

    static unsigned long n;
    static const double beat = tempo/60.0;

    // hardcoded melody goes here 

    frequency = base_frequency * pow(_12thRootOf2, semitones);
    double freq = frequency;

    for (unsigned long i = 0; i < frameCount; i++, n++)
    {

        data->data = (float) (amplitude * sin(n * freq * 2 * 3.141592653589 / (double) SAMPLE_RATE));

        if (data->data > 0)
        {
            *out++ = 0.2;
        }
        else if (data->data < 0)
        {
            *out++ = -0.2;
        }
        else
        {
            *out++ = 0;
        }

        std:: cout << timePassed() << " for beat = " << beat << " and tempo = " << tempo << " bpm.\n";

    }
    
    return 0;
}

double timePassed () {
    now = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = now - start;
    return elapsed.count();
}