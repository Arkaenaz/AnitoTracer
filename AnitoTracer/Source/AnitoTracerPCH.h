#pragma once

#include <winsdkver.h>
#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0A00
#endif
#include <sdkddkver.h>

#define NOMINMAX

#define NODRAWTEXT
#define NOGDI
#define NOBITMAP

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include <filesystem>

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
