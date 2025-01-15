#pragma once

#include "D3D12Device.h"

namespace Anito {
	class D3D12CommandQueue
	{
	public:
		D3D12CommandQueue(const D3D12Device& device);
		~D3D12CommandQueue();
		D3D12CommandQueue(const D3D12CommandQueue&) = delete;
		D3D12CommandQueue& operator=(const D3D12CommandQueue&) = delete;

		void execute(const std::vector<ID3D12CommandList*>& commandLists) const;

		ID3D12CommandQueue* get() const;

	private:
		ID3D12CommandQueue* commandQueue;
	};
}

