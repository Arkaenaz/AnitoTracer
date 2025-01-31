#pragma once

#include <vector>

#include "D3D12GraphicsContext.h"

namespace DirectXUtil
{
	class GraphicsPipeline
	{
	public:
		static const WCHAR* kVertexPixelShader;

		static D3D12_ROOT_SIGNATURE_DESC createGraphicsRootSigDesc();
		static D3D12_RASTERIZER_DESC createDefaultRasterizerDesc();
		static D3D12_BLEND_DESC createDefaultBlendDesc();
		static D3D12_SHADER_BYTECODE compileVertexShader(const wchar_t* fileName, const char* entryPointName, ID3DBlob* shaderBlob);
		static D3D12_SHADER_BYTECODE compilePixelShader(const wchar_t* fileName, const char* entryPointName, ID3DBlob* shaderBlob);
	};
}
