#pragma once

#include "AccelerationStructureBuffers.h"
#include "../D3D12VertexBuffer.h"

namespace Anito {
    class AccelerationStructures
    {

    public:
        inline static D3D12_HEAP_PROPERTIES kUploadHeapProps = D3D12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD, D3D12_CPU_PAGE_PROPERTY_UNKNOWN, D3D12_MEMORY_POOL_UNKNOWN, 0, 0);
        inline static D3D12_HEAP_PROPERTIES kDefaultHeapProps = D3D12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT, D3D12_CPU_PAGE_PROPERTY_UNKNOWN, D3D12_MEMORY_POOL_UNKNOWN, 0, 0);

        D3D12Buffer* CreateBuffer(ID3D12Device9& pDevice, UINT size, D3D12_RESOURCE_FLAGS flags, D3D12_RESOURCE_STATES initState, D3D12_HEAP_PROPERTIES heapProps)
        {
            D3D12_RESOURCE_DESC bufDesc = {};
            bufDesc.Alignment = 0;
            bufDesc.DepthOrArraySize = 1;
            bufDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
            bufDesc.Flags = flags;
            bufDesc.Format = DXGI_FORMAT_UNKNOWN;
            bufDesc.Height = 1;
            bufDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
            bufDesc.MipLevels = 1;
            bufDesc.SampleDesc = { 1, 0 };
            bufDesc.Width = size;

            ID3D12Resource2* pBuffer;
        	pDevice.CreateCommittedResource(&heapProps, D3D12_HEAP_FLAG_NONE, &bufDesc, initState, nullptr, IID_PPV_ARGS(&pBuffer));
            
            return new D3D12Buffer(pBuffer);
        }

        AccelerationStructureBuffers CreateBottomLevelAS(ID3D12Device9& device, ID3D12GraphicsCommandList4* pCmdList, D3D12VertexBuffer* vertexBuffer)
        {
            D3D12_RAYTRACING_GEOMETRY_DESC geomDesc = {};
            geomDesc.Type = D3D12_RAYTRACING_GEOMETRY_TYPE_TRIANGLES;
            geomDesc.Triangles.VertexBuffer.StartAddress = vertexBuffer->get()->getGPUVirtualAddress();
            geomDesc.Triangles.VertexBuffer.StrideInBytes = vertexBuffer->getSizeVertex();
            geomDesc.Triangles.VertexFormat = DXGI_FORMAT_R32G32B32_FLOAT;
            geomDesc.Triangles.VertexCount = vertexBuffer->get()->get()->GetDesc().Width / vertexBuffer->getSizeVertex();
            geomDesc.Flags = D3D12_RAYTRACING_GEOMETRY_FLAG_OPAQUE;

            // Get the size requirements for the scratch and AS buffers
            D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_INPUTS inputs = {};
            inputs.DescsLayout = D3D12_ELEMENTS_LAYOUT_ARRAY;
            inputs.Flags = D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAG_NONE;
            inputs.NumDescs = 1;
            inputs.pGeometryDescs = &geomDesc;
            inputs.Type = D3D12_RAYTRACING_ACCELERATION_STRUCTURE_TYPE_BOTTOM_LEVEL;

            D3D12_RAYTRACING_ACCELERATION_STRUCTURE_PREBUILD_INFO info = {};
            device.GetRaytracingAccelerationStructurePrebuildInfo(&inputs, &info);

            // Create the buffers. They need to support UAV, and since we are going to immediately use them, we create them with an unordered-access state
            AccelerationStructureBuffers buffers = {};
            buffers.scratch = CreateBuffer(device, (UINT)info.ScratchDataSizeInBytes, D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS, D3D12_RESOURCE_STATE_COMMON, kDefaultHeapProps);
            buffers.accelerationStructure = CreateBuffer(device, (UINT)info.ResultDataMaxSizeInBytes, D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS, D3D12_RESOURCE_STATE_RAYTRACING_ACCELERATION_STRUCTURE, kDefaultHeapProps);

            // Create the bottom-level AS
            D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_DESC asDesc = {};
            asDesc.Inputs = inputs;
            asDesc.DestAccelerationStructureData = buffers.accelerationStructure->getGPUVirtualAddress();
            asDesc.ScratchAccelerationStructureData = buffers.scratch->getGPUVirtualAddress();

            pCmdList->BuildRaytracingAccelerationStructure(&asDesc, 0, nullptr);

            // We need to insert a UAV barrier before using the acceleration structures in a raytracing operation

            D3D12_RESOURCE_BARRIER uavBarrier = CD3DX12_RESOURCE_BARRIER::UAV(buffers.accelerationStructure->get());
            pCmdList->ResourceBarrier(1, &uavBarrier);

            return buffers;
        }

        AccelerationStructureBuffers CreateTopLevelAS(ID3D12Device9& device, ID3D12GraphicsCommandList4* pCmdList, ID3D12Resource* pBottomLevelAS, long tlasSize)
        {
            // First, get the size of the TLAS buffers and create them
            D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_INPUTS inputs = {};
            inputs.DescsLayout = D3D12_ELEMENTS_LAYOUT_ARRAY;
            inputs.Flags = D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAG_NONE;
            inputs.NumDescs = 1;
            inputs.Type = D3D12_RAYTRACING_ACCELERATION_STRUCTURE_TYPE_TOP_LEVEL;

            D3D12_RAYTRACING_ACCELERATION_STRUCTURE_PREBUILD_INFO info = {};
            device.GetRaytracingAccelerationStructurePrebuildInfo(&inputs, &info);

            // Create the buffers
            AccelerationStructureBuffers buffers = {};
            buffers.scratch = CreateBuffer(device, (UINT)info.ScratchDataSizeInBytes, D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS, D3D12_RESOURCE_STATE_COMMON, kDefaultHeapProps);
            buffers.accelerationStructure = CreateBuffer(device, (UINT)info.ResultDataMaxSizeInBytes, D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS, D3D12_RESOURCE_STATE_RAYTRACING_ACCELERATION_STRUCTURE, kDefaultHeapProps);
            tlasSize = info.ResultDataMaxSizeInBytes;

            // The instance desc should be inside a buffer, create and map the buffer
            buffers.instanceDesc = CreateBuffer(device, sizeof(D3D12_RAYTRACING_INSTANCE_DESC), D3D12_RESOURCE_FLAG_NONE, D3D12_RESOURCE_STATE_GENERIC_READ, kUploadHeapProps);
            D3D12_RAYTRACING_INSTANCE_DESC pInstanceDesc = {};

            // Initialize the instance desc. We only have a single instance
            pInstanceDesc.InstanceID = 0;                          // This value will be exposed to the shader via InstanceID()
            pInstanceDesc.InstanceContributionToHitGroupIndex = 0; // This is the offset inside the shader-table. We only have a single geometry, so the offset 0
            pInstanceDesc.Flags = D3D12_RAYTRACING_INSTANCE_FLAG_NONE;
            pInstanceDesc.Transform[0][0] = pInstanceDesc.Transform[1][1] = pInstanceDesc.Transform[2][2] = 1;
            pInstanceDesc.AccelerationStructure = pBottomLevelAS->GetGPUVirtualAddress();
            pInstanceDesc.InstanceMask = 1;

            void* data;
            buffers.instanceDesc->copyFromCPU(&pInstanceDesc, sizeof(pInstanceDesc));

            // Create the TLAS
            D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_DESC asDesc = {};
            asDesc.Inputs = inputs;
            asDesc.Inputs.InstanceDescs = buffers.instanceDesc->getGPUVirtualAddress();
            asDesc.DestAccelerationStructureData = buffers.accelerationStructure->getGPUVirtualAddress();
            asDesc.ScratchAccelerationStructureData = buffers.scratch->getGPUVirtualAddress();

            pCmdList->BuildRaytracingAccelerationStructure(&asDesc, 0, nullptr);

            // We need to insert a UAV barrier before using the acceleration structures in a raytracing operation
            D3D12_RESOURCE_BARRIER uavBarrier = CD3DX12_RESOURCE_BARRIER::UAV(buffers.accelerationStructure->get());
            pCmdList->ResourceBarrier(1, &uavBarrier);

            return buffers;
        }
    };
}
