#pragma once

#include <d3d11.h>
#include <dxgi.h>
#include <wrl/client.h>
#include <stdexcept>
#include "Utils/GeometryTypes.h"

using namespace Microsoft::WRL;

namespace Resources::Buffers {
	class IndexBuffer {
	public:
		IndexBuffer(ID3D11Device* device, const Index* indices, size_t indexCount);
		~IndexBuffer();
		void Bind(ID3D11DeviceContext* context);
		void Unbind(ID3D11DeviceContext* context);
		void Update(ID3D11DeviceContext* context, const Index* indices, size_t indexCount);
	private:
		ComPtr<ID3D11Buffer> buffer;
		size_t size;
	};
}