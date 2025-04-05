#include "ConstantBuffer.h"

namespace Resources::Buffers {
	ConstantBuffer::ConstantBuffer(ID3D11Device* device, const void* data, size_t size) : size(size) {
		D3D11_BUFFER_DESC bufferDesc = {};
		bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
		bufferDesc.ByteWidth = static_cast<UINT>(size);
		bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		bufferDesc.MiscFlags = 0;
		bufferDesc.StructureByteStride = 0;
		D3D11_SUBRESOURCE_DATA initData = {};
		initData.pSysMem = data;
		HRESULT hr = device->CreateBuffer(&bufferDesc, &initData, &buffer);
		if (FAILED(hr)) {
			throw std::runtime_error("Failed to create constant buffer");
		}
	}
	ConstantBuffer::~ConstantBuffer() {}
	void ConstantBuffer::Bind(ID3D11DeviceContext* context) {
		context->VSSetConstantBuffers(0, 1, buffer.GetAddressOf());
		context->PSSetConstantBuffers(0, 1, buffer.GetAddressOf());
	}
	void ConstantBuffer::Unbind(ID3D11DeviceContext* context) {
		ID3D11Buffer* nullBuffer = nullptr;
		context->VSSetConstantBuffers(0, 1, &nullBuffer);
		context->PSSetConstantBuffers(0, 1, &nullBuffer);
	}
	void ConstantBuffer::Update(ID3D11DeviceContext* context, const void* data, size_t size) {
		if (size != this->size) {
			throw std::runtime_error("Constant buffer size mismatch");
		}
		D3D11_MAPPED_SUBRESOURCE mappedResource;
		HRESULT hr = context->Map(buffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
		if (FAILED(hr)) {
			throw std::runtime_error("Failed to map constant buffer");
		}
		memcpy(mappedResource.pData, data, size);
		context->Unmap(buffer.Get(), 0);
	}
}