#include "VertexShader.h"
#include <d3dcompiler.h>
#include <stdexcept>

namespace Resources::Shaders {
    VertexShader::VertexShader(ID3D11Device* device, const std::wstring& filePath, const std::string& entryPoint)
        : device(device), filePath(filePath), entryPoint(entryPoint) {
    }

    VertexShader::~VertexShader() {
        if (shaderBlob) {
            shaderBlob->Release();
        }
        if (shader) {
            shader->Release();
        }
    }

    void VertexShader::Load() {
        if (isLoaded) {
            return;
        }
        HRESULT hr = D3DCompileFromFile(filePath.c_str(), nullptr, nullptr, entryPoint.c_str(), "vs_5_0",
            0, 0, &shaderBlob, nullptr);
        if (FAILED(hr)) {
            throw std::runtime_error("Failed to compile vertex shader: " + std::to_string(hr));
        }
        hr = device->CreateVertexShader(shaderBlob->GetBufferPointer(), shaderBlob->GetBufferSize(), nullptr, shader.GetAddressOf());
        if (FAILED(hr)) {
            throw std::runtime_error("Failed to create vertex shader: " + std::to_string(hr));
        }
        isLoaded = true;
    }

    void VertexShader::Bind(ID3D11DeviceContext* context) {
        context->VSSetShader(shader.Get(), nullptr, 0);
    }

    void VertexShader::Unbind(ID3D11DeviceContext* context) {
        context->VSSetShader(nullptr, nullptr, 0);
    }
}
