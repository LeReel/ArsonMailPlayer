/*
  ==============================================================================

	MailAPIsManager.cpp
	Created: 9 Feb 2023 9:11:12pm
	Author:  Robin

  ==============================================================================
*/

#include "MailAPIsManager.h"

const juce::String GMAIL_URL =
    //"https://www.googleapis.com/gmail/v1/users/me";//"{$userId}/messages/{$id}?format=raw";
    //"imap.gmail.com:993";
    "https://www.googleapis.com/gmail/v1/users/me/messages";
//"https://mail.google.com/mail/u/0/#inbox";
// "https://mail.google.com";
//"https://www.googleapis.com/auth/gmail.readonly";

struct MemoryStruct
{
    char* memory;
    size_t size;
};

MailAPIsManager::MailAPIsManager() : Thread("ArsonPlayer")
{
    addAndMakeVisible(&userBox);
    userBox.setText("ID");
    addAndMakeVisible(&passwordBox);
    passwordBox.setText("Password");

    addAndMakeVisible(&confirmButton);
    confirmButton.setButtonText("Confirm");
    confirmButton.onClick = [this] { confirmButtonClicked(); };
    confirmButton.setColour(juce::TextButton::buttonColourId, juce::Colours::rebeccapurple);
    confirmButton.setEnabled(true);

    addAndMakeVisible(&urlBox);
    urlBox.setText("URL");
    addAndMakeVisible(&resultsBox);
}

MailAPIsManager::~MailAPIsManager()
{
    stopThread(0);
}

void MailAPIsManager::run()
{
    const juce::String result = Fetch_Gmail();

    juce::MessageManagerLock mml(this);

    if (mml.lockWasGained())
        resultsBox.loadContent(result);
}

// The base URL for the Gmail API.
const char* const kBaseUrl = "https://www.googleapis.com/gmail/v1/users/";

// The scopes required by the Gmail API.
const char* const kGmailScopes = "https://www.googleapis.com/auth/gmail.readonly";

// The buffer size for reading response data.
const int kBufferSize = 1024;

// The callback function for writing response data to a string.
static size_t WriteStringCallback(void* contents, size_t size, size_t nmemb, void* userp)
{
    ((std::string*)userp)->append((char*)contents, size * nmemb);
    return size * nmemb;
}

juce::String MailAPIsManager::Fetch_Gmail()
{
    // The user's email ID and password.
    const juce::String _email_id = userBox.getText();
    const juce::String _email_password = passwordBox.getText();
    
    // // Set up an SSL context for secure connections.
    // juce::SSLContext ssl_context;
    // ssl_context.setSystemRootCertificates();
    //
    // // Set up an HTTP client to make requests to the Gmail API.
    // juce::URL gmail_api_url(kBaseUrl + juce::String(kEmailId) + "/messages");
    // juce::URL::OpenStreamProgressTask task(gmail_api_url, nullptr, &ssl_context);
    //
    // task.addHeader("Authorization", "Bearer " + juce::String(MakeGoogleCredential(kGmailScopes)->token()));
    // task.addHeader("Content-Type", "application/json");
    // task.addHeader("Accept", "application/json");
    // juce::String response_data = task.run();
    //
    // // Parse the JSON response data.
    // juce::var json_data = juce::JSON::parse(response_data);
    // if (json_data.isVoid())
    // {
    //     return "Failed to parse JSON data";
    // }
    //
    // return juce::JSON::toString(json_data, true);

    juce::StringPairArray responseHeaders;
    juce::String _contentTypeHeaders = "Content-Type: application/json";

    int statusCode = 0;

    const juce::URL _url(GMAIL_URL);

    if (auto stream = _url.createInputStream(juce::URL::InputStreamOptions(juce::URL::ParameterHandling::inAddress)
                                             .withConnectionTimeoutMs(10000)
                                             .withResponseHeaders(&responseHeaders)
                                             .withStatusCode(&statusCode)
                                             .withExtraHeaders(_contentTypeHeaders)))
    {
        return (statusCode != 0 ? "Status code: " + juce::String(statusCode) + juce::newLine : juce::String())
            + "Response headers: " + juce::newLine
            + responseHeaders.getDescription() + juce::newLine
            + "----------------------------------------------------" + juce::newLine
            + stream->readEntireStreamAsString();
    }
    else
    {
        if (statusCode != 0)
            return "Arson Failed to connect, status code = " + juce::String(statusCode);

        return "Arson Failed to connect!";
    }
}

void MailAPIsManager::confirmButtonClicked()
{
    startThread();
}

void MailAPIsManager::paint(juce::Graphics& graphics)
{
}

void MailAPIsManager::resized()
{
    int _width = getWidth(), _height = getHeight();

    userBox.setBounds(0, 0, _width / 3, _height / 3);
    passwordBox.setBounds(_width / 3, 0, _width / 3, _height / 3);
    confirmButton.setBounds(_width - _width / 3, 0, _width / 3, _height / 3);

    urlBox.setBounds(0, _height / 3, _width, _height / 3);

    resultsBox.setBounds(0, _height - _height / 3, _width, _height / 3);
}
