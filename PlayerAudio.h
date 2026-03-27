#pragma once
#include <JuceHeader.h>
#include <vector>

class PlayerAudio : public juce::AudioSource, public juce::ChangeListener
{
public:
    PlayerAudio(const juce::String& sessionKey);    
    ~PlayerAudio() override;


    void prepareToPlay(int samplesPerBlockExpected, double sampleRate) override;
    void getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill) override;
    void releaseResources() override;


    bool loadFile(const juce::File& file);
    void play();
    void pause();
    void stop();
    void restart();
    void goToStart();
    void jumpRelative(double seconds);
    void setGain(float newGain);
    void setPosition(double newPosition);

    // Loop
    void setLooping(bool shouldLoop);
    void setABLoopEnabled(bool enabled);
    void setLoopStart(double pos);
    void setLoopEnd(double pos);
    void checkAndLoopAB();
    void update();


    // Playlist 
    void addToPlaylist(const juce::File& file);
    void previousTrack();
    void nextTrack();
    const juce::Array<juce::File>& getPlaylist() const { return playlistFiles; }


    // Markers
    void addMarker(const juce::String& name, double position);
    void jumpToMarker(int index);
    std::vector<std::pair<juce::String, double>>& getMarkers();

    
    double getCurrentPosition() const;
    double getDuration() const;
    juce::String getFileTitle() const;
    juce::String getFileDuration() const;



    void setDeckGain(float newGain)
    {
        deckGain = juce::jlimit(0.0f, 1.0f, newGain);
    }

    float getDeckGain() const
    {
        return deckGain;
    }


    void changeListenerCallback(juce::ChangeBroadcaster* source) override;


    juce::AudioTransportSource& getTransportSource() { return transportSource; }
    const juce::AudioTransportSource& getTransportSource() const { return transportSource; }


    bool isPlaying() const { return transportSource.isPlaying(); }
    double getTotalLength() const { return duration; }
    juce::AudioFormatReader* getReader() const { return readerSource ? readerSource->getAudioFormatReader() : nullptr; }
    void saveLastSession();
    void loadLastSession();

private:

    juce::AudioFormatManager formatManager;
    std::unique_ptr<juce::AudioFormatReaderSource> readerSource;
    juce::AudioTransportSource transportSource;
    juce::String fileTitle;
    juce::String fileDuration;
    juce::String sessionKey_;
   



    
    juce::File currentFile;
    double duration = 0.0;
    bool looping = false;

    // AB Loop
    bool abLoopEnabled = false;
    double pointA = 0.0;
    double pointB = 0.0;

    // Playlist
    juce::Array<juce::File> playlistFiles;

    // Markers
    std::vector<std::pair<juce::String, double>> markers;

    float deckGain = 1.0f;

 
};
