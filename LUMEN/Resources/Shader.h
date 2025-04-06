#pragma once

#include <d3d11.h>
#include <dxgi.h>
#include <d3dcompiler.h>
#include <wrl/client.h>
#include <stdexcept>
#include <string>

using namespace Microsoft::WRL;

namespace Resources::Shaders {
	template <typename T>
	class Shader {
	public:
		Shader(ID3D11Device* device, const std::wstring& filePath, const std::string& entryPoint, const std::string& target);
		~Shader();
		void Bind(ID3D11DeviceContext* context);
		void Unbind(ID3D11DeviceContext* context);
		void SetMatrix(ID3D11DeviceContext* context, const std::string& name, const float* matrix);
		void SetFloat(ID3D11DeviceContext* context, const std::string& name, float value);
		void SetVector(ID3D11DeviceContext* context, const std::string& name, const float* vector);

		void SetLayout(ID3D11DeviceContext* context, const D3D11_INPUT_ELEMENT_DESC* layout, UINT numElements) {
			context->IASetInputLayout(layout);
		}

		void Load();

		void SetFilePath(const std::wstring& filePath);
		bool Loaded() const { return isLoaded; }
	private:
		ID3D11Device* device = nullptr;
		ComPtr<ID3DBlob> shaderBlob;
		ComPtr<T> shader;
		bool isLoaded = false;
		std::wstring filePath;
		std::string entryPoint;
		std::string target;

		void CheckCompileErrors(ID3DBlob* shaderBlob, HRESULT hr);
	};
}