#include "TableSongListComponent.h"
#include "SceneComponent.h"

#pragma region TableSongListComponent
TableSongListComponent::TableSongListComponent()
{
    columnsList = ATTRIBUTES_LIST;
}

TableSongListComponent::~TableSongListComponent()
{
}

void TableSongListComponent::cellDoubleClicked(int rowNumber, int columnId, const juce::MouseEvent& mouse_event)
{
    SetCurrentSelected(rowNumber);

    sceneOwner->onSongChoosed(GetCurrentSelected());
}

void TableSongListComponent::paintRowBackground(juce::Graphics& g, int rowNumber, int width, int height,
                                                bool rowIsSelected)
{
    auto alternateColour = getLookAndFeel().findColour(juce::ListBox::backgroundColourId)
                                           .interpolatedWith(getLookAndFeel().findColour(juce::ListBox::textColourId),
                                                             0.03f);
    if (rowIsSelected)
        g.fillAll(juce::Colours::rebeccapurple);
    else if (rowNumber % 2)
        g.fillAll(alternateColour);
}

void TableSongListComponent::paintCell(juce::Graphics& g, int rowNumber, int columnId, int width, int height,
                                       bool rowIsSelected)
{
    g.setColour(rowIsSelected || rowNumber == currentSelectionRow
                    ? juce::Colours::darkorange
                    : getLookAndFeel().findColour(juce::ListBox::textColourId));
    g.setFont(font);

    SongTableElement& _element = *datasList[rowNumber];

    g.drawText(_element.GetStringAttribute(columnsList[columnId - 1]), 2, 0, width - 4, height,
               juce::Justification::centredLeft, true);
}

void TableSongListComponent::sortOrderChanged(int newSortColumnId, bool isForwards)
{
    if (newSortColumnId != 0)
    {
        // DataSorter sorter(GetColumnNameAttribute(newSortColumnId), isForwards);
        // datasList.sort(sorter);
        // 
        table.updateContent();
    }
}

juce::Component* TableSongListComponent::refreshComponentForCell(int rowNumber, int columnId, bool cond,
                                                                 Component* existingComponentToUpdate)
{
    jassert(existingComponentToUpdate == nullptr);
    return nullptr;
}

int TableSongListComponent::getColumnAutoSizeWidth(int columnId)
{
    int widest = 32;

    for (auto i = getNumRows(); --i >= 0;)
    {
        SongTableElement& _element = *datasList[i];

        juce::String _text = _element.GetStringAttribute(GetColumnNameAttribute(columnId - 1));

        widest = juce::jmax(widest, font.getStringWidth(_text));
    }

    return widest + 8;
}

SongTableElement& TableSongListComponent::GetCurrentSelected() const
{
    return *datasList[currentSelectionRow];
}

void TableSongListComponent::InitTableList(juce::Array<juce::File> _files)
{
    const bool _isFirstInit = table.getHeader().getNumColumns(false) == 0;

    LoadDatas(_files);

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
                                        juce::TableHeaderComponent::defaultFlags);
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
    const int _childrenCount = datasList.size();
    for (int i = 0; i < _childrenCount; i ++)
    {
        const bool _isSelected = rowNumber == -1 ? false : i == rowNumber;
        if (_isSelected) currentSelectionRow = i;
    }
}

void TableSongListComponent::SetSceneOwner(SceneComponent* _owner)
{
    sceneOwner = _owner;
}

SceneComponent* TableSongListComponent::GetSceneOwner()
{
    return sceneOwner;
}

void TableSongListComponent::LoadDatas(juce::Array<juce::File> _files)
{
    for (juce::File _file : _files)
    {
        if (_file == juce::File() || ! _file.exists())
            return;

        auto* _element = new SongTableElement(_file);
        datasList.add(_element);
    }

    datasAmount = datasList.size();
}

void TableSongListComponent::ChangeCell(int _move)
{
    if (currentSelectionRow + _move < 0 || currentSelectionRow + _move >= datasAmount)
    {
        return;
    }
    currentSelectionRow += _move;
    SetCurrentSelected(currentSelectionRow);

    sceneOwner->onSongChoosed(GetCurrentSelected());
}

juce::String TableSongListComponent::GetColumnNameAttribute(int _columnId) const
{
    return columnsList[_columnId];
}
#pragma endregion TableSongListComponent

#pragma region DataSorter
int DataSorter::compareElements(const juce::XmlElement* first, const juce::XmlElement* second) const
{
    return 0;

    // int result = first->getStringAttribute(attributeToSort)
    //                   .compareNatural(second->getStringAttribute(attributeToSort));
    //
    // if (result == 0)
    //     result = first->getStringAttribute("ID")
    //                   .compareNatural(second->getStringAttribute("ID"));
    //
    // return direction * result;
}
#pragma endregion DataSorter
