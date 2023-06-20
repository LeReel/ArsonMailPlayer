#include "Utils.h"

#include <fstream>

// Overload of InitButton() for calls that don't pass an array
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

juce::Array<juce::var>* Utils::GetJsonPropertyArray(juce::var& _propertyVar,
                                                    const juce::String& _property,
                                                    juce::var& _parsedJson)
{
    // Reads JSON and convert it to String
    const juce::String _jsonString = GetJSONFile().loadFileAsString();
    // _parsedJson will store JSON structure
    // Parse the JSON string
    juce::JSON::parse(_jsonString, _parsedJson);
    _propertyVar = _parsedJson.getProperty(_property, 0);
    
    return _propertyVar.getArray();
}

juce::Array<juce::File> Utils::LoadSongListFromJson()
{
    // Retrieves JSON file
    const juce::File _jsonFile = GetJSONFile();
    // Loads it as a String
    const juce::String _jsonString = _jsonFile.loadFileAsString();

    // Array that will contains files to return
    juce::Array<juce::File> _files;
    // Var that will holds JSON content
    juce::var _parsedJson;

    // Parses jsonString and stores the result in parsedJson
    if (juce::JSON::parse(_jsonString, _parsedJson).wasOk())
    {
        // Retrieves folders' paths as an Array 
        const juce::Array<juce::var>* _pathsArray = _parsedJson.getProperty("paths", 0).getArray();
        
        for (int i = 0; i < _pathsArray->size(); ++i)
        {
            // Gets the path
            juce::String _folderPath = _parsedJson["paths"][i].toString();
            // Creates a File object (which in fact is a folder) from retrieved path
            juce::File _folder(_folderPath);
            
            // Retrieves folder's children and stores them in an Array
            juce::Array<juce::File> _children = _folder.findChildFiles(2, true, "*.mp3");
            // Stores each file in returned Array
            for (const auto& _file : _children)
            {
                _files.add(_file);
            }
        }
    }

    return _files;
}

void Utils::ReadMetadata(const juce::File& _file,
                         std::map<juce::String,
                                  juce::String>& _attributesMap)
{
    // Reads .mp3 files metadata
    if (_file.getFileExtension() == ".mp3")
    {
        std::ifstream _inputFile;
        _inputFile.open(_file.getFullPathName().toStdString(), std::ios::in);
        if (!_inputFile.good())
        {
            return;
        }
        // Sets an input position indicator at the end of file to read its metadata
        _inputFile.seekg(0, std::ios::end);
        // Stores input position indicator
        const int _end = _inputFile.tellg();
        // Goes to the "TAG" tag (file's end - 128 char (4(attributes) * 30(char max for an attribute)))
        _inputFile.seekg(_end - 128);

        // Skips "TAG" tag
        char tag[4];
        for (int i = 0; i < 3; ++i)
        {
            tag[i] = _inputFile.get();
        }
        tag[3] = '\0';

        // "TAG" is missing
        if (std::strcmp(tag, "TAG") != 0)
        {
            _attributesMap["Title"] = _file.getFileName().removeCharacters(".mp3");
            _attributesMap["Artist"] = _attributesMap["Album"] = "Unknown";
            return;
        }

        SetMetadataAttribute(_inputFile, _attributesMap, "Title");
        SetMetadataAttribute(_inputFile, _attributesMap, "Artist");
        SetMetadataAttribute(_inputFile, _attributesMap, "Album");

        _inputFile.close();
    }
}

void Utils::SetMetadataAttribute(std::ifstream& _file,
                                 std::map<juce::String, juce::String>& _attributesMap,
                                 const juce::String& _attributeKey)
{
    // Metadata max size is 30 (+ 1 char for '\0')
    char* _attribute = new char[31];
    // Reads each character
    for (int i = 0; i < 30; ++i)
    {
        // Updates cursor position in file
        const int _char = _file.get();
        _attribute[i] = _char;
    }
    _attribute[30] = '\0';
    _attributesMap[_attributeKey] = _attribute;
}
