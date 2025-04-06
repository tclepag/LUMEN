#include "Shader.h"

namespace Resources::Shaders {
	template <typename T>
	Shader<T>::Shader(ID3D11Device* device, const std::wstring& filePath, const std::string& entryPoint, const std::string& target)
		: device(device), filePath(filePath), entryPoint(entryPoint), target(target) {
	}
	template <typename T>
	Shader<T>::~Shader() {
		if (shaderBlob) {
			shaderBlob->Release();
		}
		if (shader) {
			shader->Release();
		}
	}
	template <typename T>
	void Shader<T>::Load() {
		if (isLoaded) {
			return;
		}
		HRESULT hr = D3DCompileFromFile(filePath.c_str(), nullptr, nullptr, entryPoint.c_str(), target.c_str(),
			0, 0, &shaderBlob, nullptr);
		if (FAILED(hr)) {
			throw std::runtime_error("Failed to compile shader: " + std::to_string(hr));
		}
		if (target == "vs_5_0") {
			hr = device->CreateVertexShader(shaderBlob->GetBufferPointer(), shaderBlob->GetBufferSize(), nullptr, &shader);
		}
		else if (target == "ps_5_0") {
			hr = device->CreatePixelShader(shaderBlob->GetBufferPointer(), shaderBlob->GetBufferSize(), nullptr, &shader);
		}
		else {
			throw std::runtime_error("Unsupported shader target: " + target);
		}
		if (FAILED(hr)) {
			throw std::runtime_error("Failed to create shader: " + std::to_string(hr));
		}
		isLoaded = true;
	}
	template <typename T>
	void Shader<T>::Bind(ID3D11DeviceContext* context) {
		if (target == "vs_5_0") {
			context->VSSetShader(static_cast<ID3D11VertexShader*>(shader.Get()), nullptr, 0);
		}
		else if (target == "ps_5_0") {
			context->PSSetShader(static_cast<ID3D11PixelShader*>(shader.Get()), nullptr, 0);
		}
	}
	template <typename T>
	void Shader<T>::Unbind(ID3D11DeviceContext* context) {
		if (target == "vs_5_0") {
			context->VSSetShader(nullptr, nullptr, 0);
		}
		else if (target == "ps_5_0") {
			context->PSSetShader(nullptr, nullptr, 0);
		}
	}
	template <typename T>
	void Shader<T>::SetMatrix(ID3D11DeviceContext* context, const std::string& name, const float* matrix) {
		ID3D11Buffer* buffer = nullptr;
		D3D11_BUFFER_DESC bufferDesc = {};
		bufferDesc.ByteWidth = sizeof(float) * 16;
		bufferDesc.Usage = D3D11_USAGE_DEFAULT;
		bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		bufferDesc.CPUAccessFlags = 0;
		D3D11_SUBRESOURCE_DATA initData = {};
		initData.pSysMem = matrix;
		HRESULT hr = device->CreateBuffer(&bufferDesc, &initData, &buffer);
		if (FAILED(hr)) {
			throw std::runtime_error("Failed to create constant buffer: " + std::to_string(hr));
		}
		context->VSSetConstantBuffers(0, 1, &buffer);
		buffer->Release();
	}
	template <typename T>
	void Shader<T>::SetFloat(ID3D11DeviceContext* context, const std::string& name, float value) {
		ID3D11Buffer* buffer = nullptr;
		D3D11_BUFFER_DESC bufferDesc = {};
		bufferDesc.ByteWidth = sizeof(float);
		bufferDesc.Usage = D3D11_USAGE_DEFAULT;
		bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		bufferDesc.CPUAccessFlags = 0;
		D3D11_SUBRESOURCE_DATA initData = {};
		initData.pSysMem = &value;
		HRESULT hr = device->CreateBuffer(&bufferDesc, &initData, &buffer);
		if (FAILED(hr)) {
			throw std::runtime_error("Failed to create constant buffer: " + std::to_string(hr));
		}
		context->PSSetConstantBuffers(0, 1, &buffer);
		buffer->Release();
	}
	template <typename T>
	void Shader<T>::SetVector(ID3D11DeviceContext* context, const std::string& name, const float* vector) {
		ID3D11Buffer* buffer = nullptr;
		D3D11_BUFFER_DESC bufferDesc = {};
		bufferDesc.ByteWidth = sizeof(float) * 4;
		bufferDesc.Usage = D3D11_USAGE_DEFAULT;
		bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		bufferDesc.CPUAccessFlags = 0;
		D3D11_SUBRESOURCE_DATA initData = {};
		initData.pSysMem = vector;
		HRESULT hr = device->CreateBuffer(&bufferDesc, &initData, &buffer);
		if (FAILED(hr)) {
			throw std::runtime_error("Failed to create constant buffer: " + std::to_string(hr));
		}
		context->PSSetConstantBuffers(0, 1, &buffer);
		buffer->Release();
	}
	template <typename T>
	void Shader<T>::SetFilePath(const std::wstring& filePath) {
		this->filePath = filePath;
	}
	template <typename T>
	void Shader<T>::CheckCompileErrors(ID3DBlob* shaderBlob, HRESULT hr) {
		if (shaderBlob) {
			std::string errorMsg(static_cast<char*>(shaderBlob->GetBufferPointer()), shaderBlob->GetBufferSize());
			shaderBlob->Release();
			throw std::runtime_error("Shader compilation error: " + errorMsg);
		}
		else {
			throw std::runtime_error("Shader compilation failed with HRESULT: " + std::to_string(hr));
		}
	}
	template class Shader<ID3D11VertexShader>;
	template class Shader<ID3D11PixelShader>;
	}