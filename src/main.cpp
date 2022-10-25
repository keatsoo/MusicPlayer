#include <iostream>
#include <portaudio.h>
#include <math.h>
#include <chrono>
#define SAMPLE_RATE 44100
#define PLAY_DURATION 1
#define TABLE_SIZE 100
#define NUMBER_OF_CHANNELS 2
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
struct MP_StreamData { float dataL; float dataR; double tempoL; double tempoR; };
MP_StreamData data;

double base_frequency = 110.0; // La 2
double amplitudeL = 0.3;
double amplitudeR = 0.3;

double semitonesL = 24;
double semitonesR = 24;

double _12thRootOf2  = pow(2.0, 1.0/12.0);
double frequencyL;
double frequencyR;


std::chrono::time_point<std::chrono::high_resolution_clock> start;
std::chrono::time_point<std::chrono::high_resolution_clock> now;


// MAIN ====================================================
int main(int, char**) {
    data.tempoL = 60;
    data.tempoR = 60;
    
    // init the library and check for errors
    PaError err = Pa_Initialize();
    if (err != paNoError) std::cout << "PortAudio error : " << Pa_GetErrorText(err);

    start = std::chrono::high_resolution_clock::now();

    // open a stream
    err = Pa_OpenDefaultStream(&stream, 0, NUMBER_OF_CHANNELS, paFloat32, SAMPLE_RATE, 64, MusicPlayerCallback, &data);

    err = Pa_StartStream(stream);
    Pa_Sleep(PLAY_DURATION * 1000);
    err = Pa_CloseStream(stream);

    // uninit the library
    err = Pa_Terminate();
    if (err != paNoError) std::cout << "PortAudio error : " << Pa_GetErrorText(err);

    return 0;
}



// CALLBACK =====================================================
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

    static unsigned long nL;
    static unsigned long nR;
    static const double beatL = data->tempoL/60.0;

    // hardcoded melody goes here 

    frequencyL = base_frequency * pow(_12thRootOf2, semitonesL);
    double freqL = frequencyL;

    frequencyR = base_frequency * pow(_12thRootOf2, semitonesR);
    double freqR = frequencyR;

    for (unsigned long i = 0; i < frameCount; i++, nL++, nR++)
    {

        data->dataL = (float) (amplitudeL * sin(nL * freqL * 2 * 3.141592653589 / (double) SAMPLE_RATE));
        if (data->dataL > 0)
        {
            *out++ = 0.2;
        }
        else if (data->dataL < 0)
        {
            *out++ = -0.2;
        }
        else
        {
            *out++ = 0;
        }

        if (NUMBER_OF_CHANNELS > 1) {
            data->dataR = (float) (amplitudeR * sin(nR * freqR * 2 * 3.141592653589 / (double) SAMPLE_RATE));
            if (data->dataR > 0)
            {
                *out++ = 0.2;
            }
            else if (data->dataR < 0)
            {
                *out++ = -0.2;
            }
            else
            {
                *out++ = 0;
            }
        }



        std:: cout << timePassed() << "\n";

    }
    
    return 0;
}



// OTHER FUNCTIONS =======================================================
double timePassed () {
    now = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = now - start;
    return elapsed.count();
}
