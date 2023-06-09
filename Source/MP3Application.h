#pragma once

#include "MainWindow.h"

class MP3Application : public juce::JUCEApplication
{
    std::unique_ptr<MainWindow> mainWindow;

public:
    MP3Application() = default;

    const juce::String getApplicationName() override { return ProjectInfo::projectName; }
    const juce::String getApplicationVersion() override { return ProjectInfo::versionString; }
    bool moreThanOneInstanceAllowed() override { return true; }

    /// <summary>
    /// Any code in that function will be executed as soon as app starts
    /// </summary>
    void initialise(const juce::String& /*commandLineParams*/) override;
    void shutdown() override;
};

//==============================================================================
// This macro generates the main() routine that launches the app.
START_JUCE_APPLICATION (MP3Application)