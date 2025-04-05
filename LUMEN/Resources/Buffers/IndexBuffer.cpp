#include "IndexBuffer.h"

namespace Resources::Buffers {
	IndexBuffer::IndexBuffer(ID3D11Device* device, const Index* indices, size_t indexCount) : size(indexCount) {
		D3D11_BUFFER_DESC bufferDesc = {};
		bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
		bufferDesc.ByteWidth = static_cast<UINT>(sizeof(Index) * indexCount);
		bufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
		bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		bufferDesc.MiscFlags = 0;
		bufferDesc.StructureByteStride = 0;
		D3D11_SUBRESOURCE_DATA initData = {};
		initData.pSysMem = indices;
		HRESULT hr = device->CreateBuffer(&bufferDesc, &initData, &buffer);
		if (FAILED(hr)) {
			throw std::runtime_error("Failed to create index buffer");
		}
	}
	IndexBuffer::~IndexBuffer() {}
	void IndexBuffer::Bind(ID3D11DeviceContext* context) {
		context->IASetIndexBuffer(buffer.Get(), DXGI_FORMAT_R32_UINT, 0);
	}
	void IndexBuffer::Unbind(ID3D11DeviceContext* context) {
		ID3D11Buffer* nullBuffer = nullptr;
		context->IASetIndexBuffer(nullBuffer, DXGI_FORMAT_UNKNOWN, 0);
	}
	void IndexBuffer::Update(ID3D11DeviceContext* context, const Index* indices, size_t indexCount) {
		if (indexCount != size) {
			throw std::runtime_error("Index count mismatch");
		}
		D3D11_MAPPED_SUBRESOURCE mappedResource;
		HRESULT hr = context->Map(buffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
		if (FAILED(hr)) {
			throw std::runtime_error("Failed to map index buffer");
		}
		memcpy(mappedResource.pData, indices, sizeof(Index) * indexCount);
		context->Unmap(buffer.Get(), 0);
	}
}