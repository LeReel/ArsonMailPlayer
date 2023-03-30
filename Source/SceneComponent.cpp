#include "SceneComponent.h"
#include "MailAPIsManager.h"

const juce::String MAILBOX_URL =
    "https://graph.microsoft.com/v1.0";
juce::String ACCESS_TOKEN =
    "EwCAA8l6BAAUAOyDv0l6PcCVu89kmzvqZmkWABkAAZQz12auR1lRG22AHkTAzLBkUMZz1k+VehO7pBcMQBYc9zSr+LefiQqAOEilnnyJH3cOawYnC/yS4nBinq0/8bQ++VBD9NSglv82Fi9F+2qpmgkWcGiLap4susgiTwwXIZw5R5Q74EIFaPHpvubvJw7+MW949Kv9AUpvJY3d4iCqma4l8WFTXqganjXFfeX77IRQYzAfo+oiIcTpOC/jdnuMtmUAcACTV45vZzbBbLieOA3hX2kX98X8F09jV+YMT8ZSnUUhJPeel6F5g3SexQWZYFHZWionz5XP7fuAPgm0rb1fPm63M6tf/qmmxJlovYe5WUdOiRBIpSInVRql6zQDZgAACAVTQN0HFclYUALY2A3gfZRfTwMuMy8tueaEDV19RZBeiRp+mm9Eui5GTGHz0zJ+hH5Yz6KPmZefSWwQw0EsoIl4w2er8/LHBXcPtti9IAyA6egISoSVsKLQB1GjPIalNBKMkwmBnok0WyfH9ehuiGnIsPj6cIaPjxpBSZFijKaONCLL31ZtyAwgfASBz/xpFNzv+aQe+7DtX5r4dqatheg5FFSXBfP71ce4aVs/b1GGi66uKhJ4mUBLvT0sNhqsuCn/Jj4geQBYgb+/cVJ2cNYEJuRTFwb9FN4CXAdTuDr/sgLkJu1RFFNrXtM/CvWbFMmX9jck5Ukd9twZW7q3Q4eiuTO9wtcA1n2iY64sxZXYdtvQUvQlOpJIz4QSWOJS7u5IeKWyk1cEijBbhGhgARuqYy4LbkybP66GzV3hMocJMIHHlQG40rv27RjxPRwLd90lz8wAnLlB3i5zv50h6xciXJTHfAd9iCGDJ7OKGqqzt+FOy/r6VGlpJ+YZqxFOf1pybdsB/WM4h26ylOGmt/v55XjTmrLJ9S67T8VKOos3RyQ8CWmVAZv40k6XM/WnNDfBDD8yjyLtr7ica4Wg9369nhVPcZIOgUJD30hM1I/hCUW1hjMH5FL6kXI7/cRV8MMbIZUuqRwovIy6VtXEs3G9PxYdlUoPwlytF5n73z85D8MbiULicUi91TbzUvPjaRxzZEILVWX/8u0dDa70JNshUp4nvaqeJEZZ8+NfZXG/9ViYEDtzG7mTGuvGwvQH7+Ar6CgNRzxESAKGgkO4jPL1wjb0vZJ4Z9QnhwI=";

SceneComponent::SceneComponent()// : Thread("ArsonPlayer")
{
    setSize(getWidth(), getHeight());

    //TODO: launch quickstart.py to log into mailbox that holds .mp3 attachments
    //TODO: launch attachments retrieving to populate songList

    //songsList.onChange = [this] { onSongChoosed(); };
    songsList.SetSceneOwner(this);
    addAndMakeVisible(&songsList);
    addAndMakeVisible(&currentPlaying);
    currentPlaying.SetSceneComponent(this);

    addAndMakeVisible(&openButton);
    openButton.setButtonText("Open");
    openButton.onClick = [this] { openButtonClicked(); };
    openButton.setColour(juce::TextButton::buttonColourId, juce::Colours::darkslategrey);
    openButton.setEnabled(true);

    //addAndMakeVisible(urlBox);
    //urlBox.setText("Search...");
    //urlBox.onReturnKey = [this] { fetchButton.triggerClick(); };

    addAndMakeVisible(fetchButton);
    //fetchButton.onClick = [this] { startThread(); };
    fetchButton.onClick = [this] { Fetch(); };

    addAndMakeVisible(resultsBox);
}

SceneComponent::~SceneComponent()
{
    // Uncomment if inheriting Listener
    // songsList.removeListener(this);
}

void SceneComponent::paint(juce::Graphics& g)
{
    g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId)); // clear the background

    g.setColour(juce::Colours::grey);
    g.drawRect(getLocalBounds(), 1); // draw an outline around the component
}

void SceneComponent::resized()
{
    int _width = getWidth(), _height = getHeight();

    songsList.setBounds(0, 0, _width, _height / 3);
    resultsBox.setBounds(0, _height / 3, _width, _height / 3);

    currentPlaying.setBounds(0, _height - _height / 4, _width, _height / 4);

    openButton.setBounds(0, _height - _height / 4, _width / 8, _height / 8);
    fetchButton.setBounds(0, _height - _height / 8, _width / 8, _height / 8);

    auto area = getLocalBounds();

    {
        //auto topArea = area.removeFromTop(40);
        //fetchButton.setBounds(topArea.removeFromRight(180).reduced(8));
        //urlBox.setBounds(topArea.reduced(8));
    }

    //resultsBox.setBounds(area.reduced(8));
}

// void SceneComponent::run()
// {
//     //TODO: Input as an influence
//     //const juce::String result = GetResultText(urlBox.getText());
//     const juce::String result = GetResultText(MAILBOX_URL + "/me/messages");
//
//     RetrieveAttachments(result);
//
//     juce::MessageManagerLock mml(this);
//
//     if (mml.lockWasGained())
//         resultsBox.loadContent(result);
// }
//
// juce::String SceneComponent::GetResultText(const juce::URL& _url)
// {
//     juce::StringPairArray responseHeaders;
//     juce::String _contentTypeHeaders = "Authorization: Bearer " + ACCESS_TOKEN + ", Content-Type: application/json";
//
//     int statusCode = 0;
//
//     //	// Set the authorization header
//     //	struct curl_slist* headers = NULL;
//     //	headers = curl_slist_append(headers, "Authorization: Bearer [ACCESS_TOKEN]");
//
//     if (auto stream = _url.createInputStream(juce::URL::InputStreamOptions(juce::URL::ParameterHandling::inAddress)
//                                              .withConnectionTimeoutMs(10000)
//                                              .withResponseHeaders(&responseHeaders)
//                                              .withStatusCode(&statusCode)
//                                              .withExtraHeaders(_contentTypeHeaders)))
//     {
//         if (statusCode == 401) // Failed to authenticate
//         {
//             if (RefreshToken(statusCode))
//             {
//                 if (auto _retryStream = _url.createInputStream(
//                     juce::URL::InputStreamOptions(juce::URL::ParameterHandling::inAddress)
//                     .withConnectionTimeoutMs(10000)
//                     .withResponseHeaders(&responseHeaders)
//                     .withStatusCode(&statusCode)
//                     .withExtraHeaders(_contentTypeHeaders)))
//                 {
//                     return _retryStream->readEntireStreamAsString();
//                 }
//                 else
//                 {
//                     return "Failed to refresh token, status code = " + juce::String(statusCode);
//                 }
//             }
//         }
//
//         return (statusCode != 0 ? "Status code: " + juce::String(statusCode) + juce::newLine : juce::String())
//             + "Response headers: " + juce::newLine
//             + responseHeaders.getDescription() + juce::newLine
//             + "----------------------------------------------------" + juce::newLine
//             + stream->readEntireStreamAsString();
//     }
//     else
//     {
//         if (statusCode != 0)
//             return "Failed to connect, status code = " + juce::String(statusCode);
//
//         return "Failed to connect!";
//     }
// }

// bool SceneComponent::RefreshToken(int& statusCode)
// {
//     juce::StringPairArray responseHeaders;
//
//     juce::URL _refreshURL = MAILBOX_URL; // + "/login";
//
//     juce::DynamicObject* _obj = new juce::DynamicObject();
//     _obj->setProperty("user", "robinguy@live.fr");
//     _obj->setProperty("password", "4qse3m6b4Qse3m6b");
//     juce::var _json(_obj);
//     juce::String _jsonString = juce::JSON::toString(_json);
//
//     if (!_jsonString.isEmpty())
//     {
//         _refreshURL = _refreshURL.withPOSTData(_jsonString);
//     }
//
//     const juce::String _contentTypeHeaders =
//         "Content-Type: application/json";
//     const juce::String _idHeaders =
//         "username: robinguy@live.fr";
//     const juce::String _pwdHeaders =
//         "password: 4qse3m6b4Qse3m6b";
//
//     if (auto _refreshTokenStream = _refreshURL
//                                    //.withPOSTData("/login HTTP/1.1")
//                                    //.withParameter("user", "robinguy@live.fr")
//                                    //.withParameter("password", "4qse3m6b4Qse3m6b")
//                                    .createInputStream(
//                                        juce::URL::InputStreamOptions(juce::URL::ParameterHandling::inAddress)
//                                        //.withHttpRequestCmd("POST")
//                                        .withConnectionTimeoutMs(10000)
//                                        .withResponseHeaders(&responseHeaders)
//                                        .withStatusCode(&statusCode)
//                                        //.withExtraHeaders("/login")
//                                        .withExtraHeaders(_contentTypeHeaders)
//                                        //.withExtraHeaders(_idHeaders)
//                                        //.withExtraHeaders(_pwdHeaders)
//                                    ))
//     {
//         //TODO: Set new token value
//         return true;
//     }
//     return false;
// }
//
// void SceneComponent::RetrieveAttachments(const juce::String& _jsonString)
// {
//     // Parse the JSON response
//     juce::var _responseJson = juce::JSON::parse(_jsonString);
//
//     // Get the list of emails
//     if (juce::Array<juce::var>* _emails = _responseJson.getProperty("stringValue", juce::var()).getArray())
//     {
//         // Iterate through the emails
//         for (auto& _email : *_emails)
//         {
//             // Get the ID of the email
//             juce::String _id = _email.getProperty("id", juce::var()).toString();
//
//             // Get the list of attachments for the email
//             juce::String attachmentsUrl = MAILBOX_URL + _id + "/attachments";
//         }
//     }
// }

void SceneComponent::Fetch()
{
    MailAPIsManager* _mailAPI = new MailAPIsManager();
    _mailAPI->OpenGmail("test", "test");
}

void SceneComponent::onSongChoosed(SongTableElement& _song)
{
    currentPlaying.OnSongChoose(_song);
}

void SceneComponent::openButtonClicked()
{
    juce::Array<juce::String> _s = {"*.wav ", " *.mp3"};
    chooser = std::make_unique<juce::FileChooser>("Select a Wave file to play...",
                                                  juce::File{},
                                                  "*.mp3");

    constexpr auto chooserFlags = juce::FileBrowserComponent::openMode |
        juce::FileBrowserComponent::canSelectMultipleItems;

    chooser->launchAsync(chooserFlags, [this](const juce::FileChooser& _chooser)
    {
        const juce::Array<juce::File> _files = _chooser.getResults();

        // Adds selected files to songsList
        songsList.InitTableList(_files);
    });
}
