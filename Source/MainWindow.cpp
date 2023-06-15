#include "MainWindow.h"

MainWindow::MainWindow(const juce::String& _name) : DocumentWindow(
    _name,
    juce::Colours::coral,
    allButtons)
{
    setUsingNativeTitleBar(false); // Can't be transparent with native

    // "Owned" means that the window is now responsible for the MainComponent's lifetime
    // and will automatically destroy it as its destructor is called
    setContentOwned(new MainComponent(), true);

    setResizable(true, false);
    
    // The MainComponent object's size gets set in its own constructor
    // before the MainWindow object is positioned and sized.
    // This is the reason why the calls to functions Component::getWidth() and Component::getHeight()
    // in the MainWindow class can figure out the size of the window
    // so that the main component is displayed with the correct size
    centreWithSize(getWidth(),getHeight());

    Component::setVisible(true);
}

void MainWindow::closeButtonPressed()
{
    juce::JUCEApplication::getInstance()->systemRequestedQuit();
}
