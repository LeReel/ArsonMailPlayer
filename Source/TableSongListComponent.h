#pragma once

#include "Utils.h"

class SceneComponent;

const juce::Array<juce::String> ATTRIBUTES_LIST{"Title", "Artist", "Album", "Favorite"};

class SongTableElement
{
    juce::File associatedFile;

    bool isFavorite = false;

    std::map<juce::String, juce::String> attributes;

public:
    SongTableElement(juce::File _associatedFile)
    {
        associatedFile = _associatedFile;

        for (juce::String _attribute : ATTRIBUTES_LIST)
        {
            if (_attribute == "Favorite")
            {
                attributes.insert(std::pair<juce::String, juce::String>(_attribute, " "));
                continue;
            }

            attributes.insert(std::pair<juce::String, juce::String>(_attribute, "Unknown"));
        }
        attributes["Title"] = associatedFile.getFileName();
    }

    juce::File& GetAssociatedFile() { return associatedFile; }

    bool GetIsFavorite()
    {
        return isFavorite;
    }

    juce::String GetStringAttribute(juce::String _attribute)
    {
        return attributes[_attribute];
    }

    void SwitchIsFavorite()
    {
        SetIsFavorite(!isFavorite);
    }

    void SetIsFavorite(const bool _isFavorite)
    {
        isFavorite = _isFavorite;
        SetStringAttibute("Favorite", isFavorite ? "X" : "");
    }

    void SetStringAttibute(juce::String _attribute, juce::String _value)
    {
        attributes[_attribute] = _value;
    }
};

class TableSongListComponent : public juce::Component,
                               public IMyComponent,
                               public juce::TableListBoxModel
{
public:
    TableSongListComponent();
    ~TableSongListComponent() override;

#pragma region Methods
#pragma region Overrides
    void cellClicked(int rowNumber, int columnId, const juce::MouseEvent&) override;
    void cellDoubleClicked(int rowNumber, int columnId, const juce::MouseEvent&) override;

    int getNumRows() override
    {
        return datasAmount;
    }

    void paintRowBackground(juce::Graphics& g,
                            int rowNumber,
                            int width,
                            int height,
                            bool rowIsSelected) override;
    void paintCell(juce::Graphics& g,
                   int rowNumber,
                   int columnId,
                   int width,
                   int height,
                   bool rowIsSelected) override;
    void sortOrderChanged(int newSortColumnId, bool isForwards) override;
    /**
     * \brief Used to create or update a custom component to go in a cell
     * \return If existingComponentToUpdate is null, then must create a new component suitable for the cell, and return it.
     */
    Component* refreshComponentForCell(int rowNumber,
                                       int columnId,
                                       bool isRowSelected,
                                       Component* existingComponentToUpdate) override;
    int getColumnAutoSizeWidth(int columnId) override;

    void resized() override
    {
        table.setBoundsInset(juce::BorderSize<int>(8));
        table.autoSizeAllColumns();
    }
#pragma endregion Overrides
#pragma region Customs
    void SetCurrentSelected(const int rowNumber);

    SongTableElement& GetCurrentSelected() const;

    void InitTableList(juce::Array<juce::File> _files);
    void LoadDatas(juce::Array<juce::File> _files);
    void ChangeCell(const int _move, const bool _isLoopAll = false, const bool _isRandom = false);
    /**
     * \brief Get 'Name' attribute from columnList[columnID]
     * \return Element.Name as String
     */
    juce::String GetColumnNameAttribute(int _columnId) const;

    void AddSongToList(SongTableElement* _toAdd)
    {
        datasList.add(_toAdd);
        datasAmount++;
    }

    void RemoveSongFromList(const SongTableElement* _toRemove)
    {
        const int _dataSize = datasList.size();
        for (int i = 0; i < _dataSize; ++i)
        {
            if (datasList[i] == _toRemove)
            {
                datasList.remove(i);
                break;
            }
        }
        datasAmount--;
    }
#pragma endregion Customs
#pragma endregion Methods

#pragma region Fields

private:
    juce::TableListBox table{{}, this};
    juce::Font font{14.0f};

    juce::Colour rowColour;
    juce::Colour rowColour_interpolated;

    juce::Array<juce::String> columnsList;
    juce::Array<SongTableElement*> datasList;
    int datasAmount = 0, currentPlayingRow = -1, currentSelectionColumn = 0;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TableSongListComponent)
#pragma endregion Fields
};

class DataSorter
{
public:
    DataSorter(const juce::String& attributeToSortBy, bool forwards)
        : attributeToSort(attributeToSortBy),
          direction(forwards ? 1 : -1)
    {
    }

    int compareElements(const juce::XmlElement* first, const juce::XmlElement* second) const;

private:
    juce::String attributeToSort;
    int direction = 0;
};
