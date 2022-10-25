#include <iostream>
#include <portaudio.h>
#include <math.h>
#include <chrono>
#include <vector>
#include "Note.hpp"
#define SAMPLE_RATE 44100
#define PLAY_DURATION 20
#define TABLE_SIZE 100
#define NUMBER_OF_CHANNELS 2
// FREQ = SAMPLE_RATE/TABLE_SIZE

// callback, will repeatedly be called thourought the program to compute the data
int MusicPlayerCallback(const void* input,
                    void* output,
                    unsigned long
                    frameCount,
                    const PaStreamCallbackTimeInfo* timeInfo,
                    PaStreamCallbackFlags StatusFlags,
                    void* userData);

// computes how much time passed since the beginning
double timePassed();

// stream pointer
PaStream* stream;

// data structure that will allow the storage of the sound data
struct MP_StreamData
{
    float dataL;
    float dataR;
    double tempoL;
    double tempoR;
    std::vector<Note> notesToPlay;
};

MP_StreamData data;

double base_frequency = 110.0; // La 2
double amplitudeL = 0.3;
double amplitudeR = 0.3;

double semitonesL = 24;
double semitonesR = 24;

double _12thRootOf2  = pow(2.0, 1.0/12.0);
double frequencyL;
double frequencyR;

int musicLength;


std::chrono::time_point<std::chrono::high_resolution_clock> start;
std::chrono::time_point<std::chrono::high_resolution_clock> now;


// MAIN ====================================================
int main(int, char**)
{
    data.tempoL = 60;
    data.tempoR = 60;

    Note notesToPlay[6] = {
        Note(1, 22),
        Note(3, 31),
        Note(0.25, 26),
        Note(0.25, 27),
        Note(0.5, 28),
        Note(1.5, 22)
    };

    for (size_t i = 0; i < (sizeof(notesToPlay) / sizeof(notesToPlay[0])); i++)
    {
        data.notesToPlay.push_back(notesToPlay[i]);
    }
    musicLength = data.notesToPlay.size();

    /*int notesToPlay[18] = {22, 31, 26, 27, 28, 22,
                        34, 31, 32,
                        34, 31, 32,
                        34, 28, 29,
                        32, 26, 27};
    */
    
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
    static double beatL = data->tempoL/60.0;
    static double beatR = data->tempoL/60.0;

    static int pointInMusic;
    static double o_maxTimeOfNote;
    static double maxTimeOfNote;

    if (maxTimeOfNote == 0)
    {
        maxTimeOfNote += data->notesToPlay[0].getDuration();
    }

    if (timePassed() > maxTimeOfNote)
    {
        pointInMusic++;
        maxTimeOfNote += data->notesToPlay[pointInMusic].getDuration();
    }

    if (timePassed() > o_maxTimeOfNote && timePassed() <= maxTimeOfNote && pointInMusic < musicLength)
    {
        semitonesL = data->notesToPlay[pointInMusic].getSemitones();
        semitonesR = data->notesToPlay[pointInMusic].getSemitones();
    }
    else if (pointInMusic >= musicLength)
    {
        amplitudeL = 0;
        amplitudeR = 0;
    }
    
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

        if (NUMBER_OF_CHANNELS > 1)
        {
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

    }

    o_maxTimeOfNote = maxTimeOfNote;
    
    return 0;
}



// OTHER FUNCTIONS =======================================================

double timePassed ()
{
    now = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = now - start;
    return elapsed.count();
}
