#pragma once

#include "D3D12Device.h"

namespace Anito {
	class D3D12GraphicsPipeline;
	class D3D12RenderSystem;
	class D3D12PipelineState
	{
	public:
		D3D12PipelineState(const D3D12Device& device, const D3D12GraphicsPipeline& pipeline);
		~D3D12PipelineState();
		D3D12PipelineState(const D3D12PipelineState&) = delete;
		D3D12PipelineState& operator=(const D3D12PipelineState&) = delete;

		ID3D12PipelineState* getDXState();
	private:
		ID3D12PipelineState* pipelineState;
	};
}

