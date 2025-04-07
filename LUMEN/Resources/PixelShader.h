#pragma once
#include <d3d11.h>
#include <wrl/client.h>
#include <string>

using Microsoft::WRL::ComPtr;

namespace Resources::Shaders {
    class PixelShader {
    public:
        PixelShader(ID3D11Device* device, const std::wstring& filePath, const std::string& entryPoint);
        ~PixelShader();
        void Load();
        void Bind(ID3D11DeviceContext* context);
        void Unbind(ID3D11DeviceContext* context);
    private:
        ID3D11Device* device;
        std::wstring filePath;
        std::string entryPoint;
        ComPtr<ID3DBlob> shaderBlob;
        ComPtr<ID3D11PixelShader> shader;
        bool isLoaded = false;
    };
}
