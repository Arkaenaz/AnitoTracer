#include "AnitoTracerPCH.h"
#include "D3D12PipelineState.h"

#include <d3dcompiler.h>
#include <filesystem>

#include "D3D12RenderSystem.h"

namespace Anito
{
	D3D12PipelineState::D3D12PipelineState(const D3D12Device& device, const D3D12GraphicsPipeline& pipeline)
	{
        D3D12_GRAPHICS_PIPELINE_STATE_DESC stateDesc = pipeline.getDesc();
        HRESULT hr = device.get()->CreateGraphicsPipelineState(&stateDesc, IID_PPV_ARGS(&this->pipelineState));
        if (SUCCEEDED(hr))
        {
            Logger::debug(this, "Pipeline State created successfully");
        }
        else
        {
            Logger::error(this, "Pipeline State not created successfully");
        }
	}

	D3D12PipelineState::~D3D12PipelineState()
	{
        this->pipelineState->Release();
	}

	ID3D12PipelineState* D3D12PipelineState::getDXState()
	{
        return this->pipelineState;
	}
}
