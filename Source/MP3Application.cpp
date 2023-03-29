#include "MP3Application.h"

void MP3Application::initialise(const juce::String& commandLineParameters)
{
    mainWindow.reset(new MainWindow(getApplicationName()));
}

void MP3Application::shutdown()
{
    mainWindow = nullptr;
}
