#pragma once

#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")

#include <dxgi1_6.h>
#include <d3d12.h>
#include <d3dcompiler.h>
#include <dxgidebug.h>

#include <DirectXMath.h>

#define DX_DEBUG
#define MAX_TEXTURES 1000;
#define RETURN_IF_FAILED(function) {HRESULT res = function; if(FAILED(res)) return res;}

#include <wrl.h>

using namespace Microsoft::WRL;