#include "GuiText.h"
#include "Gui.h"

void GuiString::SetString ( const std::string & string ) {
    Gui::SetLabelString ( this, string );
}

void GuiString::AddChar ( char ch ) {
    Gui::LabelAddChar ( this, ch );
}

void GuiString::DeleteLastChar ( ) {
    Gui::LabelDeleteLastChar ( this );
}

void GuiString::SetPosition ( DirectX::XMINT2 position ) {
    Gui::SetLabelPosition ( this, position );
}

void GuiString::SetColor ( const DirectX::XMFLOAT4 & color ) {
    Gui::SetLabelColor ( this, color );
}

GuiString::~GuiString ( ) {
    if ( expression ) {
        delete[ ] expression;
        expression = nullptr;
    }
}
