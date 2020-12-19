#include "Gui.h"

#include <fstream>

#include "../Geometry/Vertex.h"



void Gui::Initialize ( HWND window_handle ) {    
    
    InitScreen ( window_handle );
    InitUnitPlaneGeometry ( );
    InitPanelsAttributesBuffer ( );
    //InitTexturedPlaneShader ( );
    InitPanelShader ( );
    InitFontShader ( );
    InitBasicFonts ( );

}

void Gui::DeInitialize ( ) { }

GuiFont * Gui::CreateFontFromFile ( std::string font_filename,
                                     DxTexture* font_texture ) {
    auto new_font = &fonts[numFonts];
        
    if ( !font_texture ) {
        return nullptr;
    }

    new_font->texture = font_texture ;

    std::ifstream font_file ( font_filename );
    if ( !font_file )
        return nullptr;
    font_file.seekg ( 0, std::ios::end );  
    int length = (int)font_file.tellg ( );
    font_file.seekg ( 0, std::ios::beg );      
     
    
    char* buffer = new char[100]; 

    font_file.getline ( buffer, 100 );
    while ( !font_file.eof ( ) ) {
        int id = atoi ( &buffer[8] );
        int x  = atoi ( &buffer[18] );
        int y  = atoi ( &buffer[25] );
        int w  = atoi ( &buffer[36] );
        int h  = atoi ( &buffer[48] );
        int xo = atoi ( &buffer[61] );
        int yo = atoi ( &buffer[74] );

        new_font->charSet[id].x = x;
        new_font->charSet[id].y = y;
        new_font->charSet[id].width = w;
        new_font->charSet[id].height = h;
        new_font->charSet[id].xoffset = xo;
        new_font->charSet[id].yoffset = yo;
        font_file.getline ( buffer, 100 );
    }

    font_file.close ( );
    delete[ ] buffer;

    numFonts++;

    return new_font;
}

GuiString * Gui::CreateLabel ( GuiFont * font ) {
    auto new_label = &labels[numLabels];
    auto instance_offset = GuiString::MAX_STRING_SIZE * numLabels;
    new_label->cbuf = guiCharDesc + instance_offset;
    new_label->instanceOffset = instance_offset;

    new_label->font = font;
    new_label->expression = new char[GuiString::MAX_STRING_SIZE];

    numLabels++;

    return new_label;
}

void Gui::SetLabelString ( GuiString * label, std::string string ) {
    label->exprRndrSize  = min ( GuiString::MAX_STRING_SIZE, (int)string.size ( ) );
    memcpy ( (void*)label->expression, (void*)string.data ( ), label->exprRndrSize );
    auto wnd_w   = (float)screenSize.x;
    auto wnd_h   = (float)screenSize.y;
    label->offsetX = 0.0f;
    for ( int i = 0; i < label->exprRndrSize; ++i ) {
        char c = label->expression[i];
        GuiChar gc = label->font->charSet[c];
        label->cbuf[i].offset    = { label->offsetX, -(float)gc.yoffset / wnd_h };
        label->cbuf[i].size      = { (float)gc.width / wnd_w, (float)gc.height / wnd_h };
        // -- !!! Texture size ---
        label->cbuf[i].uvOffset  = { (float)gc.x / 512.0f, (float)gc.y / 512.0f };
        label->cbuf[i].uvScale   = { (float)gc.width / 512.0f,(float)gc.height / 512.0f };
        // -- !!! Color ---
        label->cbuf[i].color     = label->color;
        label->cbuf[i].position  = { 2 * (label->xPos - wnd_w / 2) / wnd_w,
                              2 * (wnd_h / 2 - label->yPos) / wnd_h };
        label->cbuf[i].fontScale = label->scale;
        //cbuf[i].instanceOffset = instanceOffset;
        label->offsetX += (float)gc.width / wnd_w;
    }
    label->lastCharWidth = label->font->charSet[string[label->exprRndrSize]].width / wnd_w;
}

void Gui::SetLabelColor ( GuiString * label, DirectX::XMFLOAT4 color ) {
    label->color = color;    
}

void Gui::SetLabelPosition ( GuiString * label, DirectX::XMINT2 position ) {
    label->xPos = position.x;
    label->yPos = position.y;
}

void Gui::LabelAddChar ( GuiString * label, char ch ) {
    auto expr_rndr_size = label->exprRndrSize;

    if ( expr_rndr_size >= GuiString::MAX_STRING_SIZE ) {
        return;
    }
    label->expression[expr_rndr_size] = ch;
    auto wnd_w   = (float)screenSize.x;
    auto wnd_h   = (float)screenSize.y;
    GuiChar gc = label->font->charSet[ch];
    label->cbuf[expr_rndr_size].offset    = { label->offsetX, -(float)gc.yoffset / wnd_h };
    label->cbuf[expr_rndr_size].size      = { (float)gc.width, (float)gc.height };
    // -- !!! Texture size ---
    label->cbuf[expr_rndr_size].uvOffset  = { (float)gc.x / 512.0f, (float)gc.y / 512.0f };
    label->cbuf[expr_rndr_size].uvScale   = { (float)gc.width / 512.0f,(float)gc.height / 512.0f };
    // -- !!! Color ---
    label->cbuf[expr_rndr_size].color     = label->color;
    label->cbuf[expr_rndr_size].position  = { 2 * (label->xPos - wnd_w / 2) / wnd_w,
    2 * (wnd_h / 2 - label->yPos) / wnd_h };
    label->cbuf[expr_rndr_size].fontScale = label->scale;
    //cbuf[i].instanceOffset = instanceOffset;
    label->offsetX += (float)gc.width / wnd_w;
    label->lastCharWidth = gc.width / wnd_w;
    label->exprRndrSize++;
}

void Gui::LabelDeleteLastChar ( GuiString * label ) {
    auto expr_rndr_size = label->exprRndrSize;
    if ( expr_rndr_size > 0 ) {
        label->exprRndrSize--;
        label->expression[expr_rndr_size] = 0;
        auto wnd_w   = (float)screenSize.x;
        label->cbuf[expr_rndr_size].size      = { 0.0f, 0.0f };
        label->offsetX -= label->lastCharWidth;
        if ( label->exprRndrSize > 0 ) {
            label->lastCharWidth = label->font->charSet[label->expression[expr_rndr_size - 1]].width / wnd_w;
        }
    }
}

void Gui::Render ( ) {   
    /*DxShell::BindShader ( texturedPlaneShader );
    DxShell::BindConstants ( 0, 2, &screenSize, 0 );
    DxShell::BindCBV ( 1, uPlaneAttributesBuffer );
    DxShell::BindTextureArray ( 2, texture );

    DxShell::BindVertexBuffer ( uPlaneVertexBuffer );
    DxShell::BindIndexBuffer ( uPlaneIndexBuffer );

    DxShell::DrawIndexed ( 6, numTexturedPlaneInstances, 0, 0 );*/


    DxShell::BindShader ( panelShader );
    DxShell::BindConstants ( 0, 2, &screenSize, 0 );
    DxShell::BindCBV ( 1, panelsBuffer );
    DxShell::BindVertexBuffer ( uPlaneVertexBuffer );
    DxShell::BindIndexBuffer ( uPlaneIndexBuffer );
    DxShell::DrawIndexed ( 6, numPanels, 0, 0 );

    DxShell::BindShader ( fontShader );
    //DxShell::BindConstants ( 0, 2, &screenSize, 0 );
    DxShell::BindCBV ( 0, guiCharDescBuffer );
    DxShell::BindTextureArray ( 1, fontTexture );
    DxShell::BindVertexBuffers ( uPlaneVertexBuffer, labelsInstanceBuffer );
    DxShell::BindIndexBuffer ( uPlaneIndexBuffer );
    for ( int i =0; i < numLabels; i++ ) {
        DxShell::DrawIndexed ( 6, labels[i].exprRndrSize, 0, labels[i].instanceOffset );
    }
}

void Gui::InitScreen ( HWND window_handle ) {
    windowHandle = window_handle;
    RECT wnd_rect;
    if ( !GetClientRect ( window_handle, &wnd_rect ) ) 
    { return; }

    screenSize.x = wnd_rect.right  - wnd_rect.left;
    screenSize.y = wnd_rect.bottom - wnd_rect.top;
}



void Gui::InitUnitPlaneGeometry ( ) {
    // !! RELEASE TEMP BUFFER??
    auto vertex_buffer_size   = 4 * sizeof ( Geometry::Vertex2d );
    auto index_buffer_size    = 6 * sizeof ( int );
    auto instance_buffer_size = NUM_LABELS * sizeof ( int );
    auto temp_buffer_size     =  vertex_buffer_size + index_buffer_size + instance_buffer_size;

    auto temp_buffer = DxShell::CreateBuffer ( temp_buffer_size, DxBuffer::Type::CPU_BUFFER );
    auto temp_buffer_pointer =  reinterpret_cast<BYTE*>(temp_buffer->GetDataPointer ( ));

    auto vertices = reinterpret_cast<Geometry::Vertex2d*>(temp_buffer_pointer);
    vertices[0] = { { 0.0f,  0.0f }, {0.0f, 0.0f} };
    vertices[1] = { { 0.0f, -1.0f }, {0.0f, 1.0f} };
    vertices[2] = { { 1.0f, -1.0f }, {1.0f, 1.0f} };
    vertices[3] = { { 1.0f,  0.0f }, {1.0f, 0.0f} };

    auto index_buffer_offset = vertex_buffer_size;
    auto indices = reinterpret_cast<int*>(temp_buffer_pointer + index_buffer_offset);
    indices[0] = 0;
    indices[1] = 3;
    indices[2] = 1;
    indices[3] = 3;
    indices[4] = 2;
    indices[5] = 1;

    auto instance_buffer_offset = index_buffer_offset + index_buffer_size;
    auto instances = reinterpret_cast<int*>(temp_buffer_pointer + instance_buffer_offset);
    for ( int i = 0; i < NUM_LABELS; ++i ) {
        instances[i] = GuiString::MAX_STRING_SIZE * i;
    }

    uPlaneVertexBuffer   = DxShell::CreateBuffer ( vertex_buffer_size, DxBuffer::Type::GPU_BUFFER );
    uPlaneIndexBuffer    = DxShell::CreateBuffer ( index_buffer_size, DxBuffer::Type::GPU_BUFFER );
    labelsInstanceBuffer = DxShell::CreateBuffer ( instance_buffer_size, DxBuffer::Type::GPU_BUFFER );
    DxShell::CopyBuffer ( uPlaneVertexBuffer, temp_buffer, vertex_buffer_size, 0, 0 );
    DxShell::CopyBuffer ( uPlaneIndexBuffer, temp_buffer, index_buffer_size, 0, index_buffer_offset );
    DxShell::CopyBuffer ( labelsInstanceBuffer, temp_buffer, instance_buffer_size, 0, instance_buffer_offset );


    uPlaneVertexBuffer->SetAsVertexBuffer ( sizeof ( Geometry::Vertex2d ) );
    labelsInstanceBuffer->SetAsVertexBuffer ( sizeof ( int ) );
    uPlaneIndexBuffer->SetAsIndexBuffer ( );
}

void Gui::InitTexturedPlaneShader ( ) {
    texturedPlaneShader = DxShell::CreateShader ( );
    texturedPlaneShader->AssignInputLayot ( Geometry::Vertex2d::INPUT_LAYOUT );
    texturedPlaneShader->AssignVertexHlsl ( L"GraphicsShell/Shaders/Vertex/TexturedPlane.hlsl" );
    texturedPlaneShader->AssignPixelHlsl ( L"GraphicsShell/Shaders/Pixel/TexturedPlane.hlsl" );
    texturedPlaneShader->AddConstantInput ( 2, 0, D3D12_SHADER_VISIBILITY_VERTEX );
    texturedPlaneShader->AddConstantBufferInput ( 1, D3D12_SHADER_VISIBILITY_VERTEX);
    texturedPlaneShader->AddTextureArrayInput ( 2, 0, D3D12_SHADER_VISIBILITY_PIXEL );

    DxShell::CommitShader ( texturedPlaneShader );
}

void Gui::InitPanelShader ( ) {
    panelShader = DxShell::CreateShader ( );
    panelShader->AssignInputLayot ( Geometry::Vertex2d::INPUT_LAYOUT );
    panelShader->AssignVertexHlsl ( L"GraphicsShell/Shaders/Vertex/Panel.hlsl" );
    panelShader->AssignPixelHlsl ( L"GraphicsShell/Shaders/Pixel/TexturedPlane.hlsl" );
    panelShader->AddConstantInput ( 2, 0, D3D12_SHADER_VISIBILITY_VERTEX );
    panelShader->AddConstantBufferInput ( 1, D3D12_SHADER_VISIBILITY_VERTEX );

    DxShell::CommitShader ( panelShader );
}

void Gui::InitFontShader ( ) {
    D3D12_INPUT_ELEMENT_DESC instanceElementDesc =
    { "INSTANCEPOS", 0, DXGI_FORMAT_R32_UINT, 1, 0,
       D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, GuiString::MAX_STRING_SIZE };

    const auto vertex2d_layout_num_elements = Geometry::Vertex2d::NUM_ELEMENTS;
    D3D12_INPUT_ELEMENT_DESC vert_element_desc[vertex2d_layout_num_elements + 1];
    for ( int i = 0; i < vertex2d_layout_num_elements; i++ ) {
        vert_element_desc[i] = Geometry::Vertex2d::INPUT_ELEMENTS[i];
    }
    vert_element_desc[vertex2d_layout_num_elements] = instanceElementDesc;

    D3D12_INPUT_LAYOUT_DESC layout_desc;
    layout_desc.pInputElementDescs = vert_element_desc;
    layout_desc.NumElements        = vertex2d_layout_num_elements + 1;

    fontShader = DxShell::CreateShader ( );
    fontShader->AssignInputLayot ( layout_desc );
    fontShader->AssignVertexHlsl ( L"GraphicsShell/Shaders/Vertex/Font.hlsl" );
    fontShader->AssignPixelHlsl ( L"GraphicsShell/Shaders/Pixel/Font.hlsl" );
    fontShader->AddConstantBufferInput ( 0, D3D12_SHADER_VISIBILITY_ALL);
    fontShader->AddTextureArrayInput ( 1, 0, D3D12_SHADER_VISIBILITY_PIXEL );

    DxShell::CommitShader ( fontShader );
}

void Gui::InitBasicFonts ( ) {
    // !!! Make as GPU_BUFFER
    auto char_buffer_size = sizeof ( GuiCharBufDesc ) * NUM_LABELS * GuiString::MAX_STRING_SIZE;
    guiCharDescBuffer = DxShell::CreateBuffer ( char_buffer_size, DxBuffer::Type::CPU_BUFFER );
    guiCharDesc = reinterpret_cast<GuiCharBufDesc*>(guiCharDescBuffer->GetDataPointer ( ));
    fontTexture = TextureManager::CreateTextureFromDds ( "Fonts/Textures/SourcePro.dds" );
    CreateFontFromFile ( "Fonts/SourcePro.fnt", fontTexture );
}


