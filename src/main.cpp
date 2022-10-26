#include <iostream>
#include <portaudio.h>
#include <math.h>
#include <chrono>
#include <vector>
#include "Note.hpp"
#include <fstream>
#include <string>
#include <algorithm>
#include <cmath>

#define SAMPLE_RATE 44100
#define TABLE_SIZE 100
#define NUMBER_OF_CHANNELS 2

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
int playDuration;

double noteToSemitone (std::string note);

std::chrono::time_point<std::chrono::high_resolution_clock> start;
std::chrono::time_point<std::chrono::high_resolution_clock> now;


// MAIN ====================================================
int main(int, char**)
{

    std::ifstream file;
    std::cout << "Enter your file's directory : ";
    std::string fileDir;
    std::cin >> fileDir;
    file.open (fileDir);

    if (file.is_open())
    {
        char myChar;
        bool readingTempo = false;
        std::string tempoString;

        bool readingNote = false;
        int passedSpaces = 0;

        double myDuration;
        double myNote;

        std::string noteString;
        std::string durationString;

        while (file)
        {
            myChar = file.get();
            std::cout << myChar;

            if (myChar == 'T' || myChar == 't')
            {
                readingTempo = true;
            }

            if (readingTempo)
            {
                if (myChar != ' ' && myChar != '\n' && myChar != 'T' && myChar != 't')
                {
                    tempoString += myChar;
                }
                else if (myChar == '\n')
                {
                    data.tempoL = std::stod(tempoString);
                    data.tempoR = std::stod(tempoString);
                    readingTempo = false;
                    std::cout << "tempo is of : " << data.tempoL << "\n";
                }
            }


            if (myChar == 'N' || myChar == 'n')
            {
                readingNote = true;
            }

            if (readingNote)
            {
                if (myChar == ' ')
                {
                    passedSpaces++;
                }

                if (myChar != ' ' && myChar != '\n' && myChar != 'N' && myChar != 'n' && passedSpaces == 1) {
                    noteString += myChar;
                }
                
                if (myChar != ' ' && myChar != '\n' && myChar != 'N' && myChar != 'n' && passedSpaces == 2)
                {
                    durationString += myChar;
                }
                
                if (myChar == '\n') {
                    readingNote = false;

                    data.notesToPlay.push_back(Note(std::stod(durationString), noteToSemitone(noteString)));
                    playDuration += int (std::ceil(std::stod(durationString)) * data.tempoL / 60.0);
                    std::cout << "pushed back with duration = " << std::stod(durationString) << " and semitones = " << noteToSemitone(noteString) << "\n";

                    durationString = "";
                    noteString = "";
                    passedSpaces = 0;
                }
                
            }

            if (myChar == 'Z' || myChar == 'z')
            {
                data.notesToPlay.push_back(Note(1, -255));
                musicLength = data.notesToPlay.size();
                std::cout << "Duration : " << playDuration << "\n";
                break;
            }
            
            
            
        }
        
        
        // init the library and check for errors
        PaError err = Pa_Initialize();
        if (err != paNoError) std::cout << "PortAudio error : " << Pa_GetErrorText(err);

        start = std::chrono::high_resolution_clock::now();

        // open a stream
        err = Pa_OpenDefaultStream(&stream, 0, NUMBER_OF_CHANNELS, paFloat32, SAMPLE_RATE, 64, MusicPlayerCallback, &data);

        err = Pa_StartStream(stream);
        Pa_Sleep(playDuration * 1000);
        err = Pa_CloseStream(stream);

        // uninit the library
        err = Pa_Terminate();
        if (err != paNoError) std::cout << "PortAudio error : " << Pa_GetErrorText(err); 
    }
    else
    {
        std::cout << "Uh oh! Unable to open file! Maybe have you entered the wrong directory? \n";
    }

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

    static bool musicFinished;

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
    else if (pointInMusic >= musicLength || data->notesToPlay[pointInMusic].getSemitones() == -255)
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

double noteToSemitone (std::string note) {
    std::vector<std::string> notes;
    std::vector<double> semitones;
    int index;
    
    for (double i = -33; i < 107; i++)
    {
        semitones.push_back(i);
    }

    std::vector<std::string> noteLetters = {"C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B"};
    
    for (size_t d = 0; d <= 8; d++)
    {
        for (size_t i = 0; i < noteLetters.size(); i++)
        {
            notes.push_back(noteLetters[i] + std::to_string(d)); 
        }
    }
    
    std::vector<std::string>::iterator it = std::find(notes.begin(), notes.end(), note);

    if (it != notes.end())
    {
        // std::cout << "note found!!\n";
        index = std::distance(notes.begin(), it);
    }

    return semitones[index];
    
}
