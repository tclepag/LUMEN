#pragma once

#include <d3d11.h>
#include <dxgi.h>
#include <wrl/client.h>
#include <stdexcept>

using namespace Microsoft::WRL;

namespace Resources::Buffers {
	class ConstantBuffer {
	public:
		ConstantBuffer(ID3D11Device* device, const void* data, size_t size);
		~ConstantBuffer();
		void Bind(ID3D11DeviceContext* context);
		void Unbind(ID3D11DeviceContext* context);
		void Update(ID3D11DeviceContext* context, const void* data, size_t size);
	private:
		ComPtr<ID3D11Buffer> buffer;
		size_t size;
	};
}