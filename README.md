# MusicPlayer

Plays music out of a "music-assembly" file.

__TO DO :__
- [x] Incorporate a frequency system
- [x] Incorporate a tempo system (normally it works????)
- [x] Stereo sound
- [x] Able to play a hardcoded melody
- [x] Able to read a song out of an array
- [x] Create a syntax
- [x] Create a reader for the syntax
- [x] Able to read music off a "music-assembly" file
- [ ] General improvements and bug-fixing

To build :
```
g++ -o MusicPlayer src/main.cpp -lrt -lasound -ljack -lpthread -lportaudio
```

__Syntax :__ (don't include the brackets)
- `T <bpm>` sets the tempo to `<bpm>`
- `N <note> <duration>` adds a note `<note>` of duration `<duration>` (note : only use # and never bémol, as it won't work)
- `Z` ends the music

OMORI Title Screen (hardcoded melody), enter it in the callback function (you need to adapt it because of the stereo function) :
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

Array with the same song :
```cpp
    Note notesToPlay[18] =
    {
        Note(1, 22),
        Note(3, 31),
        Note(0.25, 26),
        Note(0.25, 27),
        Note(0.5, 28),
        Note(1.5, 22),

        Note(1.25, 34),
        Note(0.25, 31),
        Note(2, 32),

        Note(1.25, 34),
        Note(0.25, 31),
        Note(2, 32),

        Note(1.25, 34),
        Note(0.25, 28),
        Note(2.5, 29),

        Note(1.5, 32),
        Note(1.5, 26),
        Note(3, 27)
    };

    for (size_t i = 0; i < (sizeof(notesToPlay) / sizeof(notesToPlay[0])); i++)
    {
        data.notesToPlay.push_back(notesToPlay[i]);
    }
    musicLength = data.notesToPlay.size();
```