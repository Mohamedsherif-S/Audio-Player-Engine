#include "MainComponent.h"

MainComponent::MainComponent()
    : playerAudio1("deckA"),
    playerAudio2("deckB")
{

    playerAudio1.loadLastSession();
    playerAudio2.loadLastSession();

   
    playerGUI1 = std::make_unique<PlayerGUI>(playerAudio1);
    playerGUI2 = std::make_unique<PlayerGUI>(playerAudio2);

    addAndMakeVisible(*playerGUI1);
    addAndMakeVisible(*playerGUI2);

    setSize(1500, 700);

  
    mixsound.addInputSource(&playerAudio1, false);
    mixsound.addInputSource(&playerAudio2, false);


   
    setAudioChannels(0, 2);
}

MainComponent::~MainComponent()
{
   
    playerAudio1.saveLastSession();
    playerAudio2.saveLastSession();

    shutdownAudio();
}

void MainComponent::prepareToPlay(int samplesPerBlockExpected, double sampleRate)
{
    playerAudio1.prepareToPlay(samplesPerBlockExpected, sampleRate);
    playerAudio2.prepareToPlay(samplesPerBlockExpected, sampleRate);
    mixsound.prepareToPlay(samplesPerBlockExpected, sampleRate);
}

void MainComponent::getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill)
{
    mixsound.getNextAudioBlock(bufferToFill);
}

void MainComponent::releaseResources()
{
    mixsound.releaseResources();
    playerAudio1.releaseResources();
    playerAudio2.releaseResources();
}

void MainComponent::resized()
{
    auto area = getLocalBounds().reduced(15);
    int spacing = 15;
    int playerWidth = (area.getWidth() - spacing) / 2;

    auto leftArea = area.removeFromLeft(playerWidth);
    area.removeFromLeft(spacing);
    auto rightArea = area;

    playerGUI1->setBounds(leftArea.reduced(5));
    playerGUI2->setBounds(rightArea.reduced(5));
}
