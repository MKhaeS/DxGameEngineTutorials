#pragma once

#include "../../DxShell/DxShell.h"
#include "../Geometry/Vector.h"
#include "GuiText.h"
#include "../TextureManager.h"


constexpr auto NUM_PANELS          = 10;
constexpr auto NUM_LABELS          = 100;


class Gui {
public:
    class Panel {
    public:
        void SetPosition ( Geometry::Point2i position );
        void SetSize ( Geometry::Size2i size );
        void SetBgColor ( DirectX::XMFLOAT4 color );
    private:
        friend class Gui;
        Panel ( )                         = default;
        ~Panel ( )                        = default;
        Panel ( const Panel& )            = delete;
        Panel ( Panel&& )                 = delete;
        Panel& operator= ( const Panel& ) = delete;
        Panel& operator= ( Panel&& )      = delete;

        Geometry::Point2i position = { 0, 0 };
        Geometry::Size2i  size     = { 100, 100 };
        DirectX::XMFLOAT4 bgColor  = { 1.0f, 1.0f, 1.0f, 1.0f };
    };

    static void       Initialize   ( HWND window_handle );
    static void       DeInitialize ( );

    static GuiFont*   CreateFontFromFile ( std::string font_filename,
                                            DxTexture* font_texture );

    static Panel*     CreatePanel ( );
    static void       PanelSetPosition ( Panel* panel, Geometry::Point2i position );
    static void       PanelSetSize ( Panel* panel, Geometry::Size2i size );
    static void       PanelSetBgColor ( Panel* panel, DirectX::XMFLOAT4 color );

    static GuiString* CreateLabel ( GuiFont* font = &fonts[0] );
    static void       SetLabelString ( GuiString* label, std::string string );
    static void       SetLabelColor ( GuiString* label, DirectX::XMFLOAT4 color );
    static void       SetLabelPosition ( GuiString* label, DirectX::XMINT2 position );
    static void       LabelAddChar ( GuiString* label, char ch );
    static void       LabelDeleteLastChar ( GuiString* label );



    static void       Render ( );


    

    
private:
    struct UPlaneAttributes {
        Geometry::Point2i position;
        Geometry::Size2i  size;
    };

    Gui ( )                       = delete;
    ~Gui ( )                      = delete;
    Gui ( const Gui& )            = delete;
    Gui ( Gui&& )                 = delete;
    Gui& operator= ( const Gui& ) = delete;
    Gui& operator= ( Gui&& )      = delete;

    static void InitScreen ( HWND window_handle );
    static void InitPanelsAttributesBuffer ( );
    static void InitUnitPlaneGeometry ( );
    static void InitTexturedPlaneShader ( );
    static void InitPanelShader ( );
    static void InitFontShader ( );
    static void InitBasicFonts ( );
    

    static inline HWND             windowHandle = 0;
    static inline Geometry::Size2i screenSize   = { 0, 0 };

    static inline DxBuffer* uPlaneVertexBuffer     = nullptr;
    static inline DxBuffer* uPlaneIndexBuffer      = nullptr;
    static inline DxBuffer* panelsBuffer = nullptr;
    static inline DxBuffer* labelsInstanceBuffer   = nullptr;
    static inline DxBuffer* guiCharDescBuffer      = nullptr;

    
    static inline GuiCharBufDesc*    guiCharDesc;

    static inline DxShader* texturedPlaneShader  = nullptr;
    static inline DxShader* panelShader   = nullptr;
    static inline DxShader* fontShader           = nullptr;

    static inline DxTexture* fontTexture = nullptr;

    static inline GuiFont    fonts[10];
    static inline int        numFonts = 0;

    static inline Panel*           panels       = nullptr;
    static inline int              numPanels = 0;

    static inline GuiString  labels[10];
    static inline int        numLabels = 0;





};