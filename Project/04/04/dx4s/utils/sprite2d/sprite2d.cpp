#include "sprite2d.h"

using namespace dx4s::utils;
using namespace DirectX;

typedef struct Vertex {
	XMFLOAT3 Position;
	XMFLOAT2 Texture;
} Vertex;

XMMATRIX Sprite2d::getWorldMatrix() {
	XMMATRIX translation = XMMatrixTranslation(Position.x, Position.y, 0.0f);
	XMMATRIX rotation = XMMatrixRotationZ(Rotation);
	XMMATRIX scale = XMMatrixScaling(Scale.x, Scale.y, 1.0f);
	//return translation* rotation * scale;
	return scale * rotation * translation;
}

Sprite2d::Sprite2d(ID3D11Device *dxdev, ID3D11DeviceContext *dxcon, const char *SrcFile) {
	this->d3dContext = dxcon;
	this->d3dDevice = dxdev;
	D3D11_VIEWPORT viewPort;
	UINT viewPortNum = 1;
	d3dContext->RSGetViewports(&viewPortNum, &viewPort);
	float vWidth = viewPort.Width;
	float vHeight = viewPort.Height;
	this->Position = XMFLOAT2(1.0f, 1.0f);
	this->Scale = XMFLOAT2(1.0f, 1.0f);
	this->Rotation = 0.0f;

	D3D11_INPUT_ELEMENT_DESC inputLayoutDesc[] = {
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"TEXTURE", 0, DXGI_FORMAT_R32G32_FLOAT, 0, sizeof(XMFLOAT3), D3D11_INPUT_PER_VERTEX_DATA, 0}
	};
	ID3DBlob *vsBuffer = nullptr;
	if (!dx4s::tools::ShaderCompileFromString(shaderCode, "VS_Main", "vs_4_0", &vsBuffer)) {
		OutputDebugString("Cannot Load Vertex Shader!\n");
		return;
	}

	if (FAILED(d3dDevice->CreateVertexShader(vsBuffer->GetBufferPointer(), vsBuffer->GetBufferSize(), 0, &VS))) {
		if (vsBuffer)
			vsBuffer->Release();
		OutputDebugString("Cannot Create Vertex Shader!\n");
		return;
	}

	if (FAILED(d3dDevice->CreateInputLayout(inputLayoutDesc, ARRAYSIZE(inputLayoutDesc), vsBuffer->GetBufferPointer(), vsBuffer->GetBufferSize(), &inputLayout))) {
		OutputDebugString("Cannot Create Input Layout!\n");
		return;
	}
	ID3DBlob *psBuffer = nullptr;
	if (!dx4s::tools::ShaderCompileFromString(shaderCode, "PS_Main", "ps_4_0", &psBuffer)) {
		OutputDebugString("Cannot Load Pixiel Shader!\n");
		return;
	}
	if (FAILED(d3dDevice->CreatePixelShader(psBuffer->GetBufferPointer(), psBuffer->GetBufferSize(), 0, &PS))) {
		OutputDebugString("Cannot Create Pixiel Shader!\n");
		return;
	}

	if (FAILED(D3DX11CreateShaderResourceViewFromFile(this->d3dDevice, SrcFile, NULL, NULL, &colorMap, NULL))) {
		OutputDebugString("Cannot Create Resource View!\n");
		return;
	}
	D3D11_SAMPLER_DESC colorMapDesc;
	ZeroMemory(&colorMapDesc, sizeof(D3D11_SAMPLER_DESC));
	colorMapDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	colorMapDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	colorMapDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	colorMapDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	colorMapDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	colorMapDesc.MaxLOD = D3D11_FLOAT32_MAX;

	if (FAILED(d3dDevice->CreateSamplerState(&colorMapDesc, &colorMap_Sampler))) {
		OutputDebugString("Cannot Create Sampler State!\n");
		return;
	}

	ID3D11Resource *Tex;
	colorMap->GetResource(&Tex);
	D3D11_TEXTURE2D_DESC TexDesc;
	((ID3D11Texture2D*)Tex)->GetDesc(&TexDesc);
	Tex->Release();
	float width = TexDesc.Width;
	float height = TexDesc.Height;

	Vertex vertexPos[] = {
		{ XMFLOAT3(width / 2.0f, height / 2.0f, 1.0f), XMFLOAT2(1.0f, 0.0f) },
		{ XMFLOAT3(width / 2.0f, -height / 2.0f, 1.0f), XMFLOAT2(1.0f, 1.0f) },
		{ XMFLOAT3(-width / 2.0f, -height / 2.0f, 1.0f), XMFLOAT2(0.0f, 1.0f) },
		{ XMFLOAT3(-width / 2.0f, -height / 2.0f, 1.0f), XMFLOAT2(0.0f, 1.0f) },
		{ XMFLOAT3(-width / 2.0f, height / 2.0f, 1.0f), XMFLOAT2(0.0f, 0.0f) },
		{ XMFLOAT3(width / 2.0f, height / 2.0f, 1.0f), XMFLOAT2(1.0f, 0.0f) },
	};

	D3D11_BUFFER_DESC BufferDesc = { 0 };
	BufferDesc.ByteWidth = sizeof(Vertex) * 6;
	BufferDesc.Usage = D3D11_USAGE_DEFAULT;
	BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	D3D11_SUBRESOURCE_DATA subData = { 0 };
	subData.pSysMem = vertexPos;
	if (FAILED(d3dDevice->CreateBuffer(&BufferDesc, &subData, &vertexBuffer))) {
		OutputDebugString("Cannot Create Vertex Buffer!\n");
		return;
	}

	D3D11_BUFFER_DESC consDesc = { 0 };
	consDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	consDesc.ByteWidth = sizeof(XMMATRIX);
	consDesc.Usage = D3D11_USAGE_DEFAULT;
	if (FAILED(d3dDevice->CreateBuffer(&consDesc, 0, &mvp))) {
		OutputDebugString("Cannot Create MVP Buffer!\n");
		return;
	}

	XMMATRIX view = XMMatrixIdentity();
	XMMATRIX proj = XMMatrixOrthographicOffCenterLH(0.0f, vWidth, 0.0f, vHeight, 0.1f, 100.0f);
	vpMatrix = XMMatrixMultiply(view, proj);

	D3D11_BLEND_DESC blendDesc;
	ZeroMemory(&blendDesc, sizeof(blendDesc));
	blendDesc.RenderTarget[0].BlendEnable = TRUE;
	blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;
	blendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ZERO;
	blendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	blendDesc.RenderTarget[0].RenderTargetWriteMask = 0x0F;
	float blendFactor[4] = { 0.0f };
	d3dDevice->CreateBlendState(&blendDesc, &blendState);
	d3dContext->OMSetBlendState(blendState, blendFactor, 0xFFFFFFFF);
}

Sprite2d::~Sprite2d() {
	if (VS)
		VS->Release();
	if (PS)
		PS->Release();
	if (inputLayout)
		inputLayout->Release();
	if (vertexBuffer)
		vertexBuffer->Release();
	if (colorMap)
		colorMap->Release();
	if (colorMap_Sampler)
		colorMap_Sampler->Release();
	if (mvp)
		mvp->Release();
	if (blendState)
		blendState->Release();
}

bool Sprite2d::Render() {
	d3dContext->VSSetShader(VS, 0, 0);
	d3dContext->PSSetShader(PS, 0, 0);
	d3dContext->IASetInputLayout(inputLayout);
	unsigned int stride = sizeof(Vertex);
	unsigned int offset = 0;
	d3dContext->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
	d3dContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	d3dContext->PSSetShaderResources(0, 1, &colorMap);
	d3dContext->PSSetSamplers(0, 1, &colorMap_Sampler);
	//====
	//d3dContext->Draw(6, 0);
	//XMMATRIX MVP = XMMatrixTranspose(XMMatrixMultiply(this->getWorldMatrix(), vpMatrix));
	XMMATRIX MVP = XMMatrixTranspose(this->getWorldMatrix() * vpMatrix);
	d3dContext->UpdateSubresource(mvp, 0, 0, &MVP, 0, 0);
	d3dContext->VSSetConstantBuffers(0, 1, &mvp);
	d3dContext->Draw(6, 0);
	//====
	d3dContext->PSSetSamplers(0, 0, NULL);
	d3dContext->PSSetShaderResources(0, 0, NULL);
	d3dContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_UNDEFINED);
	d3dContext->IASetVertexBuffers(0, 0, NULL, NULL, NULL);
	d3dContext->IASetInputLayout(NULL);
	d3dContext->PSSetShader(NULL, 0, 0);
	d3dContext->VSSetShader(NULL, 0, 0);
	return true;
}

void Sprite2d::SetPosition(float x, float y) {
	this->Position = XMFLOAT2(x, y);
}

void Sprite2d::SetRotation(float rot) {
	this->Rotation = rot;
}

void Sprite2d::SetScale(float x, float y) {
	this->Scale = XMFLOAT2(x, y);
}
