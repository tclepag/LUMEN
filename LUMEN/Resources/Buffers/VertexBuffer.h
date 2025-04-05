#pragma once

#include <d3d11.h>
#include <dxgi.h>
#include <wrl/client.h>
#include <stdexcept>
#include "Utils/GeometryTypes.h"

using namespace Microsoft::WRL;


namespace Resources::Buffers {
	class VertexBuffer {
	public:
		VertexBuffer(ID3D11Device* device, const Vertex* vertices, size_t vertexCount);
		~VertexBuffer();
		void Bind(ID3D11DeviceContext* context);
		void Unbind(ID3D11DeviceContext* context);
		void Update(ID3D11DeviceContext* context, const Vertex* vertices, size_t vertexCount);
	private:
		ComPtr<ID3D11Buffer> buffer;
		size_t size;
	};
}