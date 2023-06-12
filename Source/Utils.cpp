#include "Utils.h"

void Utils::InitButton(juce::Component* _parent,
                       juce::Button& _button,
                       const juce::String& _text,
                       const std::function<void()>& _callback,
                       juce::Colour _colour,
                       bool _isEnabled)
{
    std::vector<juce::Button*> _dumpVec;
    InitButton(_parent,
               _dumpVec,
               _button,
               _text,
               _callback,
               _colour,
               _isEnabled);
}

void Utils::InitButton(juce::Component* _parent,
                       std::vector<juce::Button*>& _buttonsArray,
                       juce::Button& _button,
                       const juce::String& _text,
                       const std::function<void()>& _callback,
                       juce::Colour _colour,
                       bool _isEnabled)
{
    _button.setButtonText(_text);
    _button.onClick = _callback;
    _button.setColour(juce::TextButton::buttonColourId, _colour);
    _button.setEnabled(_isEnabled);

    _buttonsArray.push_back(&_button);

    _parent->addAndMakeVisible(&_button);
}

void Utils::SetComponentOwner(IMyComponent* _owned, IMyComponent* _owner)
{
    _owned->SetOwner(_owner);
}

juce::Array<juce::File> Utils::LoadSongListFromJson()
{
    const juce::File _jsonFile(GetJsonFilePath());
    const juce::String _jsonString = _jsonFile.loadFileAsString();

    juce::Array<juce::File> _files;
    
    juce::var _parsedJson;
    if (juce::JSON::parse(_jsonString, _parsedJson).wasOk())
    {
        const juce::Array<juce::var>* _pathsArray = _parsedJson.getProperty("paths", 0).getArray();

        for (int i = 0; i < _pathsArray->size(); ++i)
        {
            juce::String _folderPath = _parsedJson["paths"][i].toString();
            juce::File _folder(_folderPath);
            juce::Array<juce::File> _childFiles = _folder.findChildFiles(2, true, "*.mp3");

            for(const auto& _file : _childFiles)
            {
                _files.add(_file);
            }
        }
    }

    return _files;
}
