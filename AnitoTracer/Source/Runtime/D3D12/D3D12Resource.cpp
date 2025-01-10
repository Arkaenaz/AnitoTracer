#include "AnitoTracerPCH.h"
#include "D3D12Resource.h"

namespace Anito
{
	D3D12Resource::D3D12Resource(ID3D12Resource2* resource) : resource(resource)
	{
		
	}
	D3D12Resource::~D3D12Resource()
	{
		this->resource->Release();
	}

	ID3D12Resource2* D3D12Resource::get() const
	{
		return this->resource;
	}
}
