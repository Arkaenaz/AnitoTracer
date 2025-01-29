#include "AnitoTracerPCH.h"
#include "D3D12PipelineStateObject.h"

#include <d3dcompiler.h>
#include <filesystem>

#include "D3D12RenderSystem.h"

namespace Anito
{
	D3D12PipelineStateObject::D3D12PipelineStateObject(const D3D12Device& device)
		: device(device) {}

	D3D12PipelineStateObject::~D3D12PipelineStateObject()
	{
        this->pipelineState->Release();
	}

	ID3D12PipelineState* D3D12PipelineStateObject::getDXState()
	{
        return this->pipelineState;
	}
}
