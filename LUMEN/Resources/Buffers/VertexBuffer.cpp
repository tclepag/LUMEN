#include "VertexBuffer.h"

namespace Resources::Buffers {
    VertexBuffer::VertexBuffer(ID3D11Device* device, const Vertex* vertices, size_t vertexCount) : size(vertexCount) {
        D3D11_BUFFER_DESC bufferDesc = {};
        bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
        bufferDesc.ByteWidth = static_cast<UINT>(sizeof(Vertex) * vertexCount);
        bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
        bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
        bufferDesc.MiscFlags = 0;
        bufferDesc.StructureByteStride = 0;

        D3D11_SUBRESOURCE_DATA initData = {};
        initData.pSysMem = vertices;

        HRESULT hr = device->CreateBuffer(&bufferDesc, &initData, &buffer);
        if (FAILED(hr)) {
            throw std::runtime_error("Failed to create vertex buffer");
        }
    }

    VertexBuffer::~VertexBuffer() {
        // ComPtr will automatically release the buffer
    }

    void VertexBuffer::Bind(ID3D11DeviceContext* context) {
        UINT stride = sizeof(Vertex);
        UINT offset = 0;
        context->IASetVertexBuffers(0, 1, buffer.GetAddressOf(), &stride, &offset);
    }

    void VertexBuffer::Unbind(ID3D11DeviceContext* context) {
        ID3D11Buffer* nullBuffer = nullptr;
        UINT stride = 0;
        UINT offset = 0;
        context->IASetVertexBuffers(0, 1, &nullBuffer, &stride, &offset);
    }

    void VertexBuffer::Update(ID3D11DeviceContext* context, const Vertex* vertices, size_t vertexCount) {
        if (vertexCount != size) {
            throw std::runtime_error("Vertex count mismatch");
        }

        D3D11_MAPPED_SUBRESOURCE mappedResource;
        HRESULT hr = context->Map(buffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
        if (FAILED(hr)) {
            throw std::runtime_error("Failed to map vertex buffer");
        }

        memcpy(mappedResource.pData, vertices, sizeof(Vertex) * vertexCount);
        context->Unmap(buffer.Get(), 0);
    }
}
