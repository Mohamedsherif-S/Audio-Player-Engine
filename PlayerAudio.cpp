#include "PlayerAudio.h"


PlayerAudio::~PlayerAudio()
{
    transportSource.setSource(nullptr);
}

bool PlayerAudio::loadFile(const juce::File& file)
{
    auto* reader = formatManager.createReaderFor(file);
    if (reader != nullptr)
    {
        std::unique_ptr<juce::AudioFormatReaderSource> newSource(new juce::AudioFormatReaderSource(reader, true));
        transportSource.setSource(newSource.get(), 0, nullptr, reader->sampleRate);
        readerSource.reset(newSource.release());

        currentFile = file;

        
        duration = reader->lengthInSamples / reader->sampleRate;

       
        fileTitle = file.getFileNameWithoutExtension();

        int mins = (int)(duration / 60);
        int secs = (int)std::fmod(duration, 60);

        fileDuration = juce::String::formatted("%02d:%02d", mins, secs);

        return true;
    }
    return false;
}


void PlayerAudio::play()
{
    if (!transportSource.isPlaying())
        transportSource.start();
}

void PlayerAudio::pause()
{
    if (transportSource.isPlaying())
        transportSource.stop();
}

void PlayerAudio::stop()
{
    transportSource.stop();
    transportSource.setPosition(0.0);
}

void PlayerAudio::restart()
{
    transportSource.setPosition(0.0);
    transportSource.start();
}

void PlayerAudio::goToStart()
{
    transportSource.setPosition(0.0);
}

void PlayerAudio::jumpRelative(double seconds)
{
    double newPos = transportSource.getCurrentPosition() + seconds;
    if (newPos < 0.0) newPos = 0.0;
    if (newPos > duration) newPos = duration;
    transportSource.setPosition(newPos);
}

void PlayerAudio::setGain(float newGain)
{
    transportSource.setGain(newGain);
}

void PlayerAudio::setPosition(double newPosition)
{
    double absPos = newPosition * duration;
    transportSource.setPosition(absPos);
}

double PlayerAudio::getCurrentPosition() const
{
    return transportSource.getCurrentPosition();
}

double PlayerAudio::getDuration() const
{
    return duration;
}

void PlayerAudio::setLooping(bool shouldLoop)
{
    looping = shouldLoop;
    transportSource.setLooping(shouldLoop);
}


void PlayerAudio::setLoopStart(double pos)
{
    pointA = pos;
}

void PlayerAudio::setLoopEnd(double pos)
{
    pointB = pos;
}

void PlayerAudio::setABLoopEnabled(bool enabled)
{
    abLoopEnabled = enabled;
}

void PlayerAudio::checkAndLoopAB()
{
    if (abLoopEnabled && pointB > pointA && transportSource.isPlaying())
    {
        double pos = transportSource.getCurrentPosition();
        if (pos >= pointB)
            transportSource.setPosition(pointA);
    }
}

void PlayerAudio::update()
{
    if (looping &&
        !transportSource.isPlaying() &&
        transportSource.getCurrentPosition() >= transportSource.getLengthInSeconds())
    {
        transportSource.setPosition(0.0);
        transportSource.start();
    }
}

void PlayerAudio::addToPlaylist(const juce::File& file)
{
    playlistFiles.add(file);
}

void PlayerAudio::previousTrack()
{
    if (playlistFiles.isEmpty()) return;

    int index = playlistFiles.indexOf(currentFile);
    if (index > 0)
    {
        juce::File prevFile = playlistFiles[index - 1];
        loadFile(prevFile);
        play();
    }
}

void PlayerAudio::nextTrack()
{
    if (playlistFiles.isEmpty()) return;

    int index = playlistFiles.indexOf(currentFile);
    if (index < playlistFiles.size() - 1)
    {
        juce::File nextFile = playlistFiles[index + 1];
        loadFile(nextFile);
        play();
    }
}

void PlayerAudio::addMarker(const juce::String& name, double position)
{
    markers.push_back({ name, position });
}

void PlayerAudio::jumpToMarker(int index)
{
    if (index >= 0 && index < markers.size())
        transportSource.setPosition(markers[index].second);
}

std::vector<std::pair<juce::String, double>>& PlayerAudio::getMarkers()
{
    return markers;
}

juce::String PlayerAudio::getFileTitle() const
{
    return currentFile.existsAsFile() ? currentFile.getFileNameWithoutExtension() : "Unknown";
}

juce::String PlayerAudio::getFileDuration() const
{
    int mins = (int)(duration / 60);
    int secs = (int)std::fmod(duration, 60);
    return juce::String::formatted("%02d:%02d", mins, secs);
}

void PlayerAudio::changeListenerCallback(juce::ChangeBroadcaster* source)
{
    if (source == &transportSource)
    {
        if (!transportSource.isPlaying())
        {
            if (looping)
            {
                transportSource.setPosition(0.0);
                transportSource.start();
                return;
            }

            nextTrack();
        }
    }
}

void PlayerAudio::getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill)
{
    transportSource.getNextAudioBlock(bufferToFill);

    for (int ch = 0; ch < bufferToFill.buffer->getNumChannels(); ++ch)
        bufferToFill.buffer->applyGain(ch,
            bufferToFill.startSample,
            bufferToFill.numSamples,
            deckGain);
}

void PlayerAudio::prepareToPlay(int samplesPerBlockExpected, double sampleRate)
{
    transportSource.prepareToPlay(samplesPerBlockExpected, sampleRate);
}

void PlayerAudio::releaseResources()
{
    transportSource.releaseResources();
}
void PlayerAudio::saveLastSession()
{
    juce::PropertiesFile::Options options;
    options.applicationName = "MyDJPlayer";
    options.filenameSuffix = "settings";
    options.osxLibrarySubFolder = "Application Support";
    options.folderName = "MyDJPlayer";
    options.filenameSuffix = sessionKey_;



    juce::PropertiesFile props(options);

    if (currentFile.existsAsFile())
    {
        props.setValue("lastFilePath", currentFile.getFullPathName());
        props.setValue("lastPosition", transportSource.getCurrentPosition());
        props.saveIfNeeded();
    }
}
void PlayerAudio::loadLastSession()
{
    juce::PropertiesFile::Options options;
    options.applicationName = "MyDJPlayer";
    options.filenameSuffix = "settings";
    options.filenameSuffix = sessionKey_;

    options.osxLibrarySubFolder = "Application Support";
    options.folderName = "MyDJPlayer";

    juce::PropertiesFile props(options);

    juce::String path = props.getValue("lastFilePath", "");
    double position = props.getDoubleValue("lastPosition", 0.0);

    DBG("Loaded Session Key: " << sessionKey_);
    DBG("Loaded Last File Path: " << path);


    if (path.isNotEmpty())
    {
        juce::File lastFile(path);
        if (lastFile.existsAsFile())
        {
            if (loadFile(lastFile))
            {
                transportSource.setPosition(position);
            }
        }
    }
}
PlayerAudio::PlayerAudio(const juce::String& sessionKey)
    : sessionKey_(sessionKey)
{
    formatManager.registerBasicFormats();
    transportSource.addChangeListener(this);
}

