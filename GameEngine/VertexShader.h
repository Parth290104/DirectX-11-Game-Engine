#pragma once
#include "Bindable.h"

class VertexShader : public Bindable
{
public:
	VertexShader(Graphics& graphicsObject, const std::wstring& path);
	void Bind(Graphics& graphicsObject) noexcept override;
	ID3DBlob* GetByteCode() const noexcept;

private:
	Microsoft::WRL::ComPtr<ID3DBlob> pID3DBlob_ByteCodeBlob;
	Microsoft::WRL::ComPtr<ID3D11VertexShader> pID3D11VertexShader;
};
