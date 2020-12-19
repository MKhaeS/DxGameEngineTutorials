#include "Gui.h"

void Gui::InitPanelsAttributesBuffer ( ) {
    panelsBuffer = DxShell::CreateBuffer ( NUM_PANELS * sizeof ( Panel ),
                                           DxBuffer::Type::CPU_BUFFER );
    panels = reinterpret_cast<Panel*>(panelsBuffer->GetDataPointer ( ));
}

Gui::Panel * Gui::CreatePanel ( ) {
    if ( panels == nullptr ) {
        return nullptr;
    }

    auto new_panel = &panels[numPanels];
    
    numPanels++;

    return new_panel;
}

void Gui::Panel::SetPosition ( Geometry::Point2i position ) {
    this->position = position;
}

void Gui::PanelSetPosition ( Panel* panel, Geometry::Point2i position ) {
    if ( panel == nullptr ) {
        return;
    }
    panel->position = position;    
}

void Gui::Panel::SetSize ( Geometry::Size2i size ) {
    this->size = size;
}

void Gui::PanelSetSize ( Panel * panel, Geometry::Size2i size ) {
    if ( panel == nullptr ) {
        return;
    }
    panel->size = size;
}

void Gui::Panel::SetBgColor ( DirectX::XMFLOAT4 color ) {
    this->bgColor = color;
}

void Gui::PanelSetBgColor ( Panel * panel, DirectX::XMFLOAT4 color ) {
    if ( panel == nullptr ) {
        return;
    }
    panel->bgColor = color;
}
