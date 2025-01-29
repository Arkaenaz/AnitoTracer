#pragma once

#include "Runtime/D3D12/D3D12Buffer.h"

using namespace Anito;

struct AccelerationStructureBuffers
{
    D3D12Buffer* scratch;
    D3D12Buffer* accelerationStructure;
    D3D12Buffer* instanceDesc;
    UINT64                 ResultDataMaxSizeInBytes;

    void Release()
	{
        delete scratch;
        delete accelerationStructure;
        delete instanceDesc;
    }
};