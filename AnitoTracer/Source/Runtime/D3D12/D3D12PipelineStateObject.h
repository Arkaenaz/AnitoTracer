#pragma once

#include "D3D12Device.h"

namespace Anito {
	class D3D12PipelineStateObject
	{
	public:
		D3D12PipelineStateObject(const D3D12Device& device);
		~D3D12PipelineStateObject();
		D3D12PipelineStateObject(const D3D12PipelineStateObject&) = delete;
		D3D12PipelineStateObject& operator=(const D3D12PipelineStateObject&) = delete;

		ID3D12PipelineState* getDXState();

	protected:
		const D3D12Device& device;
		ID3D12PipelineState* pipelineState;
	};
}

