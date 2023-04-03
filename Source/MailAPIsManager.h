/*
  ==============================================================================

    MailAPIsManager.h
    Created: 9 Feb 2023 9:11:12pm
    Author:  Robin

  ==============================================================================
*/

#pragma once

#include <iostream>
#include <vector>

#include "CurrentPlayingComponent.h"

class MailAPIsManager : public juce::Component, public juce::Thread
{
public:
    MailAPIsManager();
    ~MailAPIsManager();

    void run() override;
    
    void OpenOutlook(char* _username, char* _password);
    void OpenGmail(char* _username, char* _password);
    
    void RetrieveAttachments(const juce::String& _jsonString);
    
    bool RefreshOutlookToken(int& statusCode);
    
    juce::String GetResultText_Gmail(const juce::URL& _url);
    juce::String GetResultText_Outlook(const juce::URL& _url);

    void Fetch();
    
    static size_t WriteMemoryCallback(char *contents, size_t size, size_t nmemb, void *userp);

    void confirmButtonClicked();

    void paint(juce::Graphics&) override;
    void resized() override;

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MailAPIsManager)

    juce::TextEditor userBox;
    juce::TextEditor passwordBox;
    
    juce::TextEditor urlBox;

    juce::TextButton confirmButton;
    
    juce::CodeDocument resultsDocument;
    juce::CodeEditorComponent resultsBox{resultsDocument, nullptr};
};