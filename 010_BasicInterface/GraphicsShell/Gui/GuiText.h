#pragma once

#include <string>
#include <DirectXMath.h>

#include "../../DxShell/DxShell.h"


class GuiChar {
private:
    friend class Gui;

    int x;
    int y;
    int width;
    int height;
    int xoffset;
    int yoffset;
};

struct GuiCharBufDesc {
    DirectX::XMFLOAT2 offset;
    DirectX::XMFLOAT2 size;
    DirectX::XMFLOAT2 uvOffset;
    DirectX::XMFLOAT2 uvScale;
    DirectX::XMFLOAT4 color;
    DirectX::XMFLOAT2 position;
    float             fontScale;
    int               instanceOffset;
};


class GuiFont {
public:
private:
    friend class Gui;

    GuiFont ( )                           = default;
    ~GuiFont ( )                          = default;
    GuiFont ( const GuiFont& )            = delete;
    GuiFont ( GuiFont&& )                 = delete;
    GuiFont& operator= ( const GuiFont& ) = delete;
    GuiFont& operator= ( GuiFont&& )      = delete;

    GuiChar    charSet[200];
    DxTexture* texture = nullptr;
};


class GuiString {
public:   
    static const auto MAX_STRING_SIZE = 100;

    void SetString ( const std::string& string );
    void AddChar ( char ch );
    void DeleteLastChar ( );
    void SetPosition ( DirectX::XMINT2 position );
    void SetColor ( const DirectX::XMFLOAT4& color );
private:
    friend class  Gui;

    

    GuiString () = default;
    ~GuiString ( );


    GuiFont*           font;
    char*              expression;
    int                exprRndrSize = 0;
    GuiCharBufDesc*    cbuf;
    int                instanceOffset;
    int                xPos = 0;
    int                yPos = 0;
    float              scale = 0.5f;
    DirectX::XMFLOAT4  color;
    float              offsetX = 0.0f;
    float              lastCharWidth = 0.0f;
};

