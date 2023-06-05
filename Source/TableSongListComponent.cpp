#include "TableSongListComponent.h"
#include "SceneComponent.h"

#pragma region TableSongListComponent
TableSongListComponent::TableSongListComponent()
{
    columnsList = ATTRIBUTES_LIST;

    rowColour = getLookAndFeel().findColour(juce::ListBox::backgroundColourId);
    rowColour_interpolated = rowColour.interpolatedWith(getLookAndFeel().findColour(juce::ListBox::textColourId),
                                                        0.03f);
}

TableSongListComponent::~TableSongListComponent()
{
}

void TableSongListComponent::cellClicked(int rowNumber, int columnId, const juce::MouseEvent& mouse_event)
{
    //If favorite
    if (columnId == 4)
    {
        datasList[rowNumber]->SwitchIsFavorite();
        if (SceneComponent* _sC = dynamic_cast<SceneComponent*>(componentOwner))
        {
            _sC->onFavoriteClicked(*datasList[rowNumber]);
        }
    }
}

void TableSongListComponent::cellDoubleClicked(int rowNumber, int columnId, const juce::MouseEvent& mouse_event)
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
}

void TableSongListComponent::paintRowBackground(juce::Graphics& g, int rowNumber, int width, int height,
                                                bool rowIsSelected)
{
    if (rowIsSelected)
        g.fillAll(juce::Colours::rebeccapurple);
    else if (rowNumber % 2)
        g.fillAll(rowColour_interpolated);
}

void TableSongListComponent::paintCell(juce::Graphics& g, int rowNumber, int columnId, int width, int height,
                                       bool rowIsSelected)
{
    g.setColour(/*rowIsSelected ||*/ rowNumber == currentSelectionRow
                                         ? juce::Colours::darkorange
                                         : juce::Colours::whitesmoke);
    g.setFont(font);

    SongTableElement& _element = *datasList[rowNumber];

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
    const int _numRows = getNumRows();

    for (int i = _numRows; --i >= 0;)
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
    for (int i = 0; i < _childrenCount; i++)
    {
        const bool _isSelected = rowNumber == -1 ? false : i == rowNumber;
        if (_isSelected) currentSelectionRow = i;
    }
}

void TableSongListComponent::LoadDatas(juce::Array<juce::File> _files)
{
    for (juce::File _file : _files)
    {
        if (_file == juce::File() || !_file.exists())
            return;

        auto* _element = new SongTableElement(_file);
        const juce::String _title = _element->GetStringAttribute("Title");

        bool _alreadyInList = false;

        for (SongTableElement* _data : datasList)
        {
            if (_data->GetStringAttribute("Title") == _title)
            {
                _alreadyInList = true;
                break;
            }
        }

        if (!_alreadyInList) datasList.add(_element);
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
