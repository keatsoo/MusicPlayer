# MusicPlayer

Plays music out of a "music-assembly" file.

__TO DO :__
- [x] Incorporate a frequency system
- [x] Incorporate a tempo system (normally it works????)
- [ ] Stereo sound
- [x] Able to play a hardcoded melody
- [ ] Create a syntax
- [ ] Create a reader for the syntax
- [ ] Able to read music off a "music-assembly" file
- [ ] General improvements and bug-fixing

To build :
```
g++ -o MusicPlayer src/main.cpp -lrt -lasound -ljack -lpthread -lportaudio
```

OMORI Title Screen (hardcoded melody), enter it in the callback function :

```cpp
if (timePassed() <= beat) semitones = 22;
if (timePassed() <= 4*beat && timePassed() > beat) semitones = 31;
if (timePassed() <= 4.25*beat && timePassed() > 4*beat) semitones = 26;
if (timePassed() <= 4.50*beat && timePassed() > 4.25*beat) semitones = 27;
if (timePassed() <= 5*beat && timePassed() > 4.50*beat) semitones = 28;
if (timePassed() <= 6.5*beat && timePassed() > 5*beat) semitones = 22;

if (timePassed() <= 7.5*beat && timePassed() > 6.5*beat) semitones = 34;
if (timePassed() <= 8.5*beat && timePassed() > 7.5*beat) semitones = 31;
if (timePassed() <= 10.75*beat && timePassed() > 8.5*beat) semitones = 32;

if (timePassed() <= 11.75*beat && timePassed() > 10.75*beat) semitones = 34;
if (timePassed() <= 12*beat && timePassed() > 11.75*beat) semitones = 31;
if (timePassed() <= 14.5*beat && timePassed() > 12*beat) semitones = 32;

if (timePassed() <= 15.5*beat && timePassed() > 14.5*beat) semitones = 34;
if (timePassed() <= 16.5*beat && timePassed() > 15.5*beat) semitones = 28;
if (timePassed() <= 19*beat && timePassed() > 16.5*beat) semitones = 29;

if (timePassed() <= 20*beat && timePassed() > 19*beat) semitones = 32;
if (timePassed() <= 21*beat && timePassed() > 20*beat) semitones = 26;
if (timePassed() <= 22*beat && timePassed() > 21*beat) semitones = 27;
```
