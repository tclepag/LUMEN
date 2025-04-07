#include "PixelShader.h"
#include <d3dcompiler.h>
#include <stdexcept>

namespace Resources::Shaders {
    PixelShader::PixelShader(ID3D11Device* device, const std::wstring& filePath, const std::string& entryPoint)
        : device(device), filePath(filePath), entryPoint(entryPoint) {
    }

    PixelShader::~PixelShader() {
        if (shaderBlob) {
            shaderBlob->Release();
        }
        if (shader) {
            shader->Release();
        }
    }

    void PixelShader::Load() {
        if (isLoaded) {
            return;
        }
        HRESULT hr = D3DCompileFromFile(filePath.c_str(), nullptr, nullptr, entryPoint.c_str(), "ps_5_0",
            0, 0, &shaderBlob, nullptr);
        if (FAILED(hr)) {
            throw std::runtime_error("Failed to compile pixel shader: " + std::to_string(hr));
        }
        hr = device->CreatePixelShader(shaderBlob->GetBufferPointer(), shaderBlob->GetBufferSize(), nullptr, shader.GetAddressOf());
        if (FAILED(hr)) {
            throw std::runtime_error("Failed to create pixel shader: " + std::to_string(hr));
        }
        isLoaded = true;
    }

    void PixelShader::Bind(ID3D11DeviceContext* context) {
        context->PSSetShader(shader.Get(), nullptr, 0);
    }

    void PixelShader::Unbind(ID3D11DeviceContext* context) {
        context->PSSetShader(nullptr, nullptr, 0);
    }
}
