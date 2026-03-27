#pragma once
#include <JuceHeader.h>
#include "PlayerAudio.h"

class PlayerGUI : public juce::Component,
    public juce::Button::Listener,
    public juce::Slider::Listener,
    public juce::ListBoxModel,
    public juce::Timer
{
public:
    PlayerGUI(PlayerAudio& audio);
    ~PlayerGUI() override;

    void paint(juce::Graphics& g) override;
    void resized() override;
    void buttonClicked(juce::Button* button) override;
    void sliderValueChanged(juce::Slider* slider) override;

    // ListBox
    int getNumRows() override;
    void paintListBoxItem(int row, juce::Graphics& g, int width, int height, bool selected) override;
    void listBoxItemClicked(int row, const juce::MouseEvent&) override;

    void timerCallback() override;
    std::vector<juce::TextButton*> getButtons();

private:
    PlayerAudio& playerAudio;

    // Buttons
    juce::TextButton startButton{ "Start" }, stopButton{ "Stop" }, muteButton{ "Mute" },
        loopButton{ "Loop" }, darkModeButton{ "Dark" }, playButton{ "Play" },
        pauseButton{ "Pause" }, backButton{ "<<" }, forwardButton{ ">>" },
        prevTrackButton{ "|<" }, nextTrackButton{ ">|" }, setAButton{ "Set A" },
        setBButton{ "Set B" }, abLoopButton{ "A-B Loop" },
        loadButton{ "Load" }, restartButton{ "Restart" },
        addMarkerButton{ "Add Marker" }, addToPlaylistButton{ "Add To Playlist" };

    // Sliders
    juce::Slider volumeSlider, positionSlider;

    // Labels
    juce::Label titleLabel, artistLabel, durationLabel;

    // Lists
    juce::ComboBox playlistBox;
    juce::ListBox markerList;
   
    std::unique_ptr<juce::FileChooser> fileChooser;
    juce::Label metadataLabel;
    

    juce::Slider deckVolumeSlider;


    // State
    bool abLoopEnabled = false;
    bool loopEnabled = false;
    bool darkModeEnabled = false;
    bool isMuted = false;
    float volumeBeforeMute = 0.5f;

    void setupButton(juce::TextButton& button);
};
