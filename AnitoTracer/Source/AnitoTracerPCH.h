#pragma once

#define NOMINMAX

#include <Windows.h>

#include <d3d12.h>
#include <dxgi1_6.h>
#include <d3dcompiler.h>

#ifdef _DEBUG
#include <d3d12sdklayers.h>
#include <dxgidebug.h>
#endif

#include "Libraries/D3DX12/d3dx12.h"

#include "Common/Logger.h"
#include "Common/Time.h"
