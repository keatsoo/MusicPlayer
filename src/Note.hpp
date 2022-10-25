class Note
{
    private:
        double m_duration;
        double m_semitones;
    public:
        Note(double duration, double semitones);
        double getDuration();
        double getSemitones();
};

Note::Note(double duration, double semitones)
{
    m_duration = duration;
    m_semitones = semitones;
}

double Note::getDuration()
{
    return m_duration;
}

double Note::getSemitones()
{
    return m_semitones;
}