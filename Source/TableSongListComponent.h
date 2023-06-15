#pragma once

#include "Utils.h"

const juce::Array<juce::String> ATTRIBUTES_LIST{"Title", "Artist", "Album", "Favorite"};

class SongTableElement
{

public:
    // Stores associatedFile as member, reads its metadata and puts them in attribute Array
    SongTableElement(const juce::File& _associatedFile);

    juce::File& GetAssociatedFile() { return associatedFile; }

    void SetStringAttribute(const juce::String& _attribute, const juce::String& _value)
    {
        attributes[_attribute] = _value;
    }
    void SetIsFavorite(const bool _isFavorite)
    {
        isFavorite = _isFavorite;
        SetStringAttribute("Favorite", isFavorite ? "X" : "");
    }
    void SwitchIsFavorite()
    {
        SetIsFavorite(!isFavorite);
    }
    
    juce::String GetStringAttribute(const juce::String& _attribute)
    {
        return attributes[_attribute];
    }
    bool GetIsFavorite() const
    {
        return isFavorite;
    }

private:
    juce::File associatedFile;
    std::map<juce::String, juce::String> attributes;
    bool isFavorite = false;
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
        return dataAmount;
    }

    void paintRowBackground(juce::Graphics& g,
                            int rowNumber,
                            int /*width*/,
                            int /*height*/,
                            bool rowIsSelected) override;
    void paintCell(juce::Graphics& g,
                   int rowNumber,
                   int columnId,
                   int width,
                   int height,
                   bool) override;
    /**
     * \brief Used to create or update a custom component to go in a cell
     * \return If existingComponentToUpdate is null, then must create a new component suitable for the cell, and return it.
     */
    Component* refreshComponentForCell(int /*rowNumber*/,
                                       int /*columnId*/,
                                       bool /*isRowSelected*/,
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
    void SetCurrentSelected(const SongTableElement& _songElement);

    SongTableElement& GetCurrentSelected() const;

    // Fills dataList with given files
    // Makes the table visible if first init
    void InitTableList(const juce::Array<juce::File>& _files);
    //Create SongTableElement from each file if not already in dataList
    void LoadData(const juce::Array<juce::File>& _files);
    // Updates currentPlayingRow and calls (SceneComponent)owner.onSongChose
    void ChangeCell(const int _move, const bool _isLoopAll = false, const bool _isRandom = false);
    // Returns columnList[_columnId]
    juce::String GetColumnNameAttribute(int _columnId) const;

    void AddSongToList(SongTableElement* _toAdd)
    {
        dataList.add(_toAdd);
        dataAmount++;

        table.updateContent();
        repaint();
        resized();
    }
    void RemoveSongFromList(const SongTableElement* _toRemove)
    {
        const int _dataSize = dataList.size();
        for (int i = 0; i < _dataSize; ++i)
        {
            if (dataList[i] == _toRemove)
            {
                dataList.remove(i);
                break;
            }
        }
        dataAmount--;

        table.deselectAllRows();

        table.updateContent();
        repaint();
        resized();
    }

    juce::Array<SongTableElement*>& GetDataList() { return dataList; }
#pragma endregion Customs
#pragma endregion Methods

#pragma region Fields

private:
    // A table of cells
    juce::TableListBox table{{}, this};
    // Contains table's columns' attributes
    juce::Array<juce::String> columnsList;
    // Array of songs as SongTableElements
    juce::Array<SongTableElement*> dataList;

    juce::Font font{14.0f};
    juce::Colour rowColour;
    juce::Colour rowColour_interpolated;
    juce::Array<int> alreadyPlayedRandom;

    int dataAmount = 0, currentPlayingRow = -1, currentSelectionColumn = 0;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TableSongListComponent)
#pragma endregion Fields
};
