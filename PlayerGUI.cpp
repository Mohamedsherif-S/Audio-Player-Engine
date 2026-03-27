#include "PlayerGUI.h"

PlayerGUI::PlayerGUI(PlayerAudio& audio)
    : playerAudio(audio)
{
    setSize(1300, 700);

    for (auto* btn : getButtons())
        setupButton(*btn);



    // Deck Volume Slider
    deckVolumeSlider.setRange(0.0, 1.0, 0.01);
    deckVolumeSlider.setValue(1.0);
    deckVolumeSlider.addListener(this);
    addAndMakeVisible(deckVolumeSlider);



    // Volume slider
    volumeSlider.setRange(0.0, 1.0, 0.01);
    volumeSlider.setValue(0.5);
    volumeSlider.addListener(this);
    addAndMakeVisible(volumeSlider);

    // Position slider
    positionSlider.setSliderStyle(juce::Slider::LinearHorizontal);
    positionSlider.setRange(0.0, 1.0, 0.001);
    positionSlider.addListener(this);
    addAndMakeVisible(positionSlider);


    metadataLabel.setJustificationType(juce::Justification::centred);
    metadataLabel.setText("No file loaded", juce::dontSendNotification);
    addAndMakeVisible(metadataLabel);

    deckVolumeSlider.setSliderStyle(juce::Slider::LinearVertical);




    // Labels

    titleLabel.setText("Title: ", juce::dontSendNotification);
    artistLabel.setText("Artist: ", juce::dontSendNotification);
    durationLabel.setText("Duration: ", juce::dontSendNotification);

    for (auto* label : { &titleLabel, &artistLabel, &durationLabel })
    {
        label->setColour(juce::Label::textColourId, juce::Colours::white);
        label->setJustificationType(juce::Justification::centredLeft);
        addAndMakeVisible(label);
    }   

    addAndMakeVisible(playlistBox);
    addAndMakeVisible(markerList);
    markerList.setModel(this);

    playlistBox.onChange = [this]()
        {
            int index = playlistBox.getSelectedItemIndex();

            if (index >= 0 && index < playerAudio.getPlaylist().size())
            {
                juce::File file = playerAudio.getPlaylist()[index];

                if (playerAudio.loadFile(file))
                {
                    titleLabel.setText("Title: " + playerAudio.getFileTitle(), juce::dontSendNotification);
                    artistLabel.setText("Artist: Unknown", juce::dontSendNotification);
                    durationLabel.setText("Duration: " + playerAudio.getFileDuration(), juce::dontSendNotification);
                }
            }
        };



    startTimerHz(10);
    titleLabel.setText("Title: " + playerAudio.getFileTitle(), juce::dontSendNotification);
    durationLabel.setText("Duration: " + playerAudio.getFileDuration(), juce::dontSendNotification);
    artistLabel.setText("Artist: Unknown", juce::dontSendNotification);
}

PlayerGUI::~PlayerGUI()
{
    for (auto* btn : getButtons())
        btn->removeListener(this);
}

void PlayerGUI::paint(juce::Graphics& g)
{
    if (darkModeEnabled)
        g.fillAll(juce::Colours::black);
    else
    {
        juce::ColourGradient gradient(
            juce::Colours::midnightblue, 0, 0,
            juce::Colours::darkslateblue, getWidth(), getHeight(), false);
        g.setGradientFill(gradient);
        g.fillAll();
    }

    g.setColour(juce::Colours::white);
    g.drawRoundedRectangle(getLocalBounds().toFloat().reduced(5), 10.0f, 1.0f);
}

void PlayerGUI::setupButton(juce::TextButton& button)
{
    button.addListener(this);
    addAndMakeVisible(button);
    button.setColour(juce::TextButton::buttonColourId, juce::Colours::darkslateblue.withBrightness(0.4f));
    button.setColour(juce::TextButton::textColourOffId, juce::Colours::white);
    button.setColour(juce::TextButton::buttonOnColourId, juce::Colours::deepskyblue);
}

void PlayerGUI::resized()
{
    using namespace juce;
    auto bounds = getLocalBounds().reduced(10);
    auto topArea = bounds.removeFromTop(60);
    auto bottomArea = bounds.removeFromBottom(70);
    auto centerArea = bounds;

    // Top buttons
    FlexBox topRow;
    topRow.flexDirection = FlexBox::Direction::row;
    topRow.justifyContent = FlexBox::JustifyContent::flexStart;
    topRow.alignItems = FlexBox::AlignItems::center;
    Array<TextButton*> topBtns = { &loadButton ,&startButton, &stopButton, &muteButton, &loopButton, &restartButton, &darkModeButton };
    for (auto* b : topBtns)
        topRow.items.add(FlexItem(*b).withMinWidth(70).withMinHeight(30).withMargin(4));
    topRow.performLayout(topArea.reduced(4));

    // Labels
    int labelH = 26;
    auto infoRect = centerArea.removeFromTop(3 * labelH + 15);
    titleLabel.setBounds(infoRect.removeFromTop(labelH).reduced(8, 2));
    artistLabel.setBounds(infoRect.removeFromTop(labelH).reduced(8, 2));
    durationLabel.setBounds(infoRect.removeFromTop(labelH).reduced(8, 2));


    deckVolumeSlider.setBounds(getWidth() - 50, 10, 30, 120);


    // Position slider
    auto sliderRect = centerArea.removeFromTop(36);
    positionSlider.setBounds(sliderRect.reduced(14, 6));

    // Playlist box
    auto playlistArea = centerArea.removeFromTop(40);
    playlistBox.setBounds(playlistArea.reduced(8, 4));

    // Add buttons (Add to playlist + Add marker)
    auto addBtnsArea = centerArea.removeFromTop(40);
    int btnW = 130, btnH = 30;
    addMarkerButton.setBounds(addBtnsArea.removeFromLeft(btnW).reduced(4));

    // Marker list
    auto markerArea = centerArea.removeFromTop(120);
    markerList.setBounds(markerArea.reduced(8, 4));

    // Volume slider
    auto volRect = centerArea.removeFromTop(40).reduced(12, 6);
    volumeSlider.setBounds(volRect);

    // Bottom buttons
    auto bottomButtonsArea = bounds.removeFromBottom(60);
    bottomButtonsArea.translate(0, -20);

    FlexBox bottomRow;
    bottomRow.flexDirection = FlexBox::Direction::row;
    bottomRow.justifyContent = FlexBox::JustifyContent::spaceAround;
    bottomRow.alignItems = FlexBox::AlignItems::center;

    Array<TextButton*> bottomBtns = {
        &backButton, &prevTrackButton, &playButton, &pauseButton,
        &nextTrackButton, &forwardButton, &setAButton, &setBButton,
        &abLoopButton
    };

    for (auto* b : bottomBtns)
        bottomRow.items.add(FlexItem(*b).withMinWidth(70).withMinHeight(36).withMargin(3));

    bottomRow.performLayout(bottomButtonsArea.reduced(6));

}


void PlayerGUI::buttonClicked(juce::Button* button)
{
    if (button == &startButton) playerAudio.goToStart();
    else if (button == &stopButton) playerAudio.stop();
    else if (button == &playButton) playerAudio.play();
    else if (button == &pauseButton) playerAudio.pause();
    else if (button == &restartButton) playerAudio.restart();
    else if (button == &backButton) playerAudio.jumpRelative(-10.0);
    else if (button == &forwardButton) playerAudio.jumpRelative(10.0);
    else if (button == &prevTrackButton) 
    {
        playerAudio.previousTrack();
        titleLabel.setText("Title: " + playerAudio.getFileTitle(), juce::dontSendNotification);
        durationLabel.setText("Duration: " + playerAudio.getFileDuration(), juce::dontSendNotification);
        artistLabel.setText("Artist: Unknown", juce::dontSendNotification);
    }




    else if (button == &nextTrackButton)

    {
        playerAudio.nextTrack();
        titleLabel.setText("Title: " + playerAudio.getFileTitle(), juce::dontSendNotification);
        durationLabel.setText("Duration: " + playerAudio.getFileDuration(), juce::dontSendNotification);
        artistLabel.setText("Artist: Unknown", juce::dontSendNotification);
    }






    else if (button == &setAButton)
    {
        double pos = playerAudio.getCurrentPosition();
        playerAudio.setLoopStart(pos);
        setAButton.setButtonText("A: " + juce::String(pos, 2));
    }
    else if (button == &setBButton)
    {
        double pos = playerAudio.getCurrentPosition();
        playerAudio.setLoopEnd(pos);
        setBButton.setButtonText("B: " + juce::String(pos, 2));
    }
    else if (button == &abLoopButton)
    {
        abLoopEnabled = !abLoopEnabled;
        abLoopButton.setButtonText(abLoopEnabled ? "AB Loop On" : "AB Loop Off");
        playerAudio.setABLoopEnabled(abLoopEnabled);
    }
    else if (button == &loopButton)
    {
        loopEnabled = !loopEnabled;
        loopButton.setButtonText(loopEnabled ? "Unloop" : "Loop");
        playerAudio.setLooping(loopEnabled);
    }
    else if (button == &muteButton)
    {
        isMuted = !isMuted;
        if (isMuted)
        {
            volumeBeforeMute = (float)volumeSlider.getValue();
            volumeSlider.setValue(0.0f, juce::dontSendNotification);
            muteButton.setButtonText("Unmute");
            playerAudio.setGain(0.0f);
        }
        else
        {
            volumeSlider.setValue(volumeBeforeMute, juce::dontSendNotification);
            muteButton.setButtonText("Mute");
            playerAudio.setGain(volumeBeforeMute);
        }
    }
    else if (button == &darkModeButton)
    {
        darkModeEnabled = !darkModeEnabled;
        repaint();
    }
    else if (button == &loadButton)
    {
        fileChooser = std::make_unique<juce::FileChooser>(
            "Load Audio File", juce::File{}, "*.wav;*.mp3;*.aiff"
        );

        fileChooser->launchAsync(
            juce::FileBrowserComponent::openMode | juce::FileBrowserComponent::canSelectFiles,
            [this](const juce::FileChooser& fc)
            {
                if (fc.getResults().size() > 0)
                {
                    juce::File file = fc.getResult();

                    playerAudio.addToPlaylist(file);

                    playlistBox.addItem(file.getFileName(), playlistBox.getNumItems() + 1);

                    if (playerAudio.loadFile(file))
                    {
                        titleLabel.setText("Title: " + playerAudio.getFileTitle(), juce::dontSendNotification);
                        artistLabel.setText("Artist: Unknown", juce::dontSendNotification);
                        durationLabel.setText("Duration: " + playerAudio.getFileDuration(), juce::dontSendNotification);
                    }
                }
            }
        );
    }

    else if (button == &addMarkerButton)
    {
        double pos = playerAudio.getCurrentPosition();
        juce::String name = "Marker " + juce::String(playerAudio.getMarkers().size() + 1);
        playerAudio.addMarker(name, pos);
        markerList.updateContent();
        markerList.repaint();
    }
}

void PlayerGUI::sliderValueChanged(juce::Slider* slider)
{
    if (slider == &deckVolumeSlider)
        playerAudio.setDeckGain((float)slider->getValue());


    if (slider == &volumeSlider)
        playerAudio.setGain((float)slider->getValue());
    else if (slider == &positionSlider)
        playerAudio.setPosition((double)slider->getValue());
}

int PlayerGUI::getNumRows()
{
    return (int)playerAudio.getMarkers().size();

}

void PlayerGUI::paintListBoxItem(int row, juce::Graphics& g, int width, int height, bool selected)
{
    if (row < 0 || row >= getNumRows()) return;
    auto& marker = playerAudio.getMarkers()[row];
    if (selected) g.fillAll(juce::Colours::darkblue);
    g.setColour(juce::Colours::white);
    g.drawText(marker.first, 5, 0, width - 10, height, juce::Justification::centredLeft);
}

void PlayerGUI::listBoxItemClicked(int row, const juce::MouseEvent&)
{
    playerAudio.jumpToMarker(row);
}

void PlayerGUI::timerCallback()
{
    playerAudio.update();
    double dur = playerAudio.getDuration();
    if (dur > 0.0)
        positionSlider.setValue(playerAudio.getCurrentPosition() / dur, juce::dontSendNotification);

    if (abLoopEnabled)
        playerAudio.checkAndLoopAB();
}

std::vector<juce::TextButton*> PlayerGUI::getButtons()
{
    return { &startButton, &stopButton, &muteButton, &loopButton, &darkModeButton,
             &playButton, &pauseButton, &backButton, &forwardButton,
             &prevTrackButton, &nextTrackButton,
             &setAButton, &setBButton, &abLoopButton,
             &loadButton, &restartButton, &addMarkerButton, };
}
