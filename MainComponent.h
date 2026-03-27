#pragma once

#include <JuceHeader.h>
#include "PlayerAudio.h"
#include "PlayerGUI.h"

class MainComponent : public juce::AudioAppComponent
{
public:
    MainComponent();
    ~MainComponent() override;

    void prepareToPlay(int samplesPerBlockExpected, double sampleRate) override;
    void getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill) override;
    void releaseResources() override;

    void resized() override;

private:
   
    PlayerAudio playerAudio1;
    PlayerAudio playerAudio2;

  
    std::unique_ptr<PlayerGUI> playerGUI1;
    std::unique_ptr<PlayerGUI> playerGUI2;

    juce::MixerAudioSource mixsound;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainComponent)
};
