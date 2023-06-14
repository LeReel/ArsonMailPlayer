#include "TableSongListComponent.h"
#include "SceneComponent.h"

#include <fstream>

#pragma region SongTableElement
SongTableElement::SongTableElement(const juce::File& _associatedFile)
{
    associatedFile = _associatedFile;

    ReadMetadata(_associatedFile);

    attributes["Favorite"] = "";
}

void SongTableElement::ReadMetadata(const juce::File& _file)
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
        for (int i = 0; i < 3; i++)
        {
            tag[i] = _inputFile.get();
        }
        tag[3] = '\0';

        // "TAG" is missing
        if (std::strcmp(tag, "TAG") != 0)
        {
            attributes["Title"] = _file.getFileName().removeCharacters(".mp3");
            attributes["Artist"] = attributes["Album"] = "Unknown";
            return;
        }

        SetMetadataAttribute(_inputFile, attributes, "Title");
        SetMetadataAttribute(_inputFile, attributes, "Artist");
        SetMetadataAttribute(_inputFile, attributes, "Album");

        _inputFile.close();
    }
}

void SongTableElement::SetMetadataAttribute(std::ifstream& _file,
                                            std::map<juce::String, juce::String>& _attributesMap,
                                            const juce::String& _attributeKey)
{
    // Metadata max size is 30 (+ 1 char for '\0')
    char* _attribute = new char[31];

    for (int i = 0; i < 30; i++)
    {
        const char _char = _file.get();
        _attribute[i] = _char;
    }
    _attribute[30] = '\0';

    _attributesMap[_attributeKey] = _attribute;

    // Avoids leak memory caused by read-only called to fill attribute variable
    delete[] _attribute;
}
#pragma endregion SongTableElement

#pragma region TableSongListComponent
TableSongListComponent::TableSongListComponent()
{
    columnsList = ATTRIBUTES_LIST;

    rowColour = getLookAndFeel().findColour(juce::ListBox::backgroundColourId);
    rowColour_interpolated = rowColour.interpolatedWith(getLookAndFeel().findColour(juce::ListBox::textColourId),
                                                        0.03f);
}

TableSongListComponent::~TableSongListComponent() = default;

void TableSongListComponent::cellClicked(int rowNumber, int columnId, const juce::MouseEvent&)
{
    //If not Favorite cell
    if (columnId != 4)
    {
        return;
    }

    SongTableElement* _songElement = dataList[rowNumber];
    if (!_songElement)
    {
        return;
    }
    _songElement->SwitchIsFavorite();

    if (SceneComponent* _sC = dynamic_cast<SceneComponent*>(componentOwner))
    {
        _sC->onFavoriteClicked(*_songElement);
    }

    repaint();
    resized();
    table.updateContent();
}

void TableSongListComponent::cellDoubleClicked(int rowNumber, int columnId, const juce::MouseEvent&)
{
    //If favorite
    if (columnId == 4)
    {
        return;
    }

    SetCurrentSelected(rowNumber);

    if (SceneComponent* _sC = dynamic_cast<SceneComponent*>(componentOwner))
    {
        _sC->onSongChose(GetCurrentSelected());
    }
    table.updateContent();
}

void TableSongListComponent::paintRowBackground(juce::Graphics& g,
                                                int rowNumber,
                                                int,
                                                int,
                                                bool rowIsSelected)
{
    if (rowIsSelected)
        g.fillAll(juce::Colours::rebeccapurple);
    else if (rowNumber % 2)
        g.fillAll(rowColour_interpolated);
}

void TableSongListComponent::paintCell(juce::Graphics& g,
                                       int rowNumber,
                                       int columnId,
                                       int width,
                                       int height,
                                       bool)
{
    g.setColour(rowNumber == currentPlayingRow
                    ? juce::Colours::darkorange
                    : juce::Colours::whitesmoke);
    g.setFont(font);

    SongTableElement& _element = *dataList[rowNumber];

    const juce::String _attribute = _element.GetStringAttribute(columnsList[columnId - 1]);

    g.drawText(_attribute,
               2,
               0,
               width - 4,
               height,
               columnId == 4 //Favorite column
                   ? juce::Justification::centred
                   : juce::Justification::centredLeft,
               true);
}

juce::Component* TableSongListComponent::refreshComponentForCell(int,
                                                                 int,
                                                                 bool,
                                                                 Component* existingComponentToUpdate)
{
    jassert(existingComponentToUpdate == nullptr);
    return nullptr;
}

int TableSongListComponent::getColumnAutoSizeWidth(int columnId)
{
    int widest = 32;
    const int _numRows = getNumRows();

    for (int i = _numRows; --i >= 0;)
    {
        SongTableElement& _element = *dataList[i];

        juce::String _text = _element.GetStringAttribute(GetColumnNameAttribute(columnId - 1));

        widest = juce::jmax(widest, font.getStringWidth(_text));
    }

    return widest + 8;
}

SongTableElement& TableSongListComponent::GetCurrentSelected() const
{
    return *dataList[currentPlayingRow];
}

void TableSongListComponent::InitTableList(const juce::Array<juce::File>& _files)
{
    const bool _isFirstInit = table.getHeader().getNumColumns(false) == 0;

    LoadData(_files);

    if (_isFirstInit)
    {
        addAndMakeVisible(table);

        for (int i = 0, _columnId = 1; i < columnsList.size(); i++, _columnId++)
        {
            const juce::String _columnString = columnsList[i];
            table.getHeader().addColumn(_columnString,
                                        _columnId,
                                        getColumnAutoSizeWidth(_columnId),
                                        50,
                                        400,
                                        juce::TableHeaderComponent::ColumnPropertyFlags::notSortable);
        }

        table.getHeader().setColour(juce::TableHeaderComponent::backgroundColourId, juce::Colours::rebeccapurple);
        table.setColour(juce::ListBox::outlineColourId, juce::Colours::darkorange);
    }
    table.setOutlineThickness(1);

    table.getHeader().setSortColumnId(1, true);
    table.setMultipleSelectionEnabled(false);

    resized();
}

void TableSongListComponent::SetCurrentSelected(const int rowNumber)
{
    currentPlayingRow = rowNumber;
}

void TableSongListComponent::SetCurrentSelected(const SongTableElement& _songElement)
{
    for (int i = 0; i < dataAmount; ++i)
    {
        if (dataList[i] == &_songElement)
        {
            currentPlayingRow = i;
            break;
        }
    }
}

void TableSongListComponent::LoadData(const juce::Array<juce::File>& _files)
{
    for (const auto& _file : _files)
    {
        if (_file == juce::File() || !_file.exists())
            return;

        auto* _element = new SongTableElement(_file);
        const juce::String _title = _element->GetStringAttribute("Title");

        bool _alreadyInList = false;

        for (SongTableElement* _data : dataList)
        {
            if (_data->GetStringAttribute("Title") == _title)
            {
                _alreadyInList = true;
                break;
            }
        }

        if (!_alreadyInList) dataList.add(_element);
    }

    dataAmount = dataList.size();
}

void TableSongListComponent::ChangeCell(const int _move, const bool _isLoopAll, const bool _isRandom)
{
    if (dataAmount == 0 || currentPlayingRow == -1) //No LoadedDatas or no CurrentPlaying yet selected
    {
        return;
    }

    if (_isRandom)
    {
        const int _maxRands = dataAmount, _alreadyPlayedSize = alreadyPlayedRandom.size();
        if (_alreadyPlayedSize == _maxRands)
        {
            alreadyPlayedRandom.clear();
        }

        int _rand = -1;
        bool _canExit = false;
        do
        {
            _rand = rand() % dataAmount;
            const bool _isPlaying = _rand == currentPlayingRow, _contains = alreadyPlayedRandom.contains(_rand);
            _canExit = !_isPlaying && !_contains;
        }
        while (!_canExit);

        alreadyPlayedRandom.add(_rand);

        currentPlayingRow = _rand;
    }
    else if (currentPlayingRow + _move < 0 || currentPlayingRow + _move >= dataAmount)
    {
        if (!_isLoopAll)
        {
            return;
        }
        currentPlayingRow = currentPlayingRow + _move < 0
                                ? dataAmount - 1
                                : 0;
    }
    else
    {
        currentPlayingRow += _move;
    }

    if (SceneComponent* _sC = dynamic_cast<SceneComponent*>(componentOwner))
    {
        _sC->onSongChose(GetCurrentSelected());
    }
}

juce::String TableSongListComponent::GetColumnNameAttribute(int _columnId) const
{
    return columnsList[_columnId];
}
#pragma endregion TableSongListComponent
