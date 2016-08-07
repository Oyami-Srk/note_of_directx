#include "dx4s/dx4s.h"

using namespace DirectX;

class MainClass :public dx4s::base {
private:
	ID3D11Buffer *vertexBuffer = nullptr;
	ID3D11VertexShader *VS = nullptr;
	ID3D11PixelShader *PS = nullptr;
	ID3D11InputLayout *inputLayout = nullptr;
public:
	int Release() {
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
		return 0;
	}

	typedef struct Vertex {
		XMFLOAT3 Position;
		XMFLOAT2 Texture;
	} Vertex;

	ID3D11ShaderResourceView *colorMap;
	ID3D11SamplerState *colorMap_Sampler;

	int Load() {
		/*D3D11_INPUT_ELEMENT_DESC inputLayoutDesc[] = {
			{
				"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0
			},
			{
				"TEXTURE", 0, DXGI_FORMAT_R32G32_FLOAT, 0, sizeof(XMFLOAT3), D3D11_INPUT_PER_VERTEX_DATA, 0
			}
		};
		HRESULT res;
		ID3DBlob *vsBuffer = nullptr;
		if (!dx4s::tools::ShaderCompile("shader.fx", "VS_Main", "vs_4_0", &vsBuffer))
			return -1;

		res = d3dDevice->CreateVertexShader(vsBuffer->GetBufferPointer(), vsBuffer->GetBufferSize(), 0, &VS);
		if (FAILED(res)) {
			if (vsBuffer)
				vsBuffer->Release();
			return -1;
		}
		
		res = d3dDevice->CreateInputLayout(inputLayoutDesc, ARRAYSIZE(inputLayoutDesc), vsBuffer->GetBufferPointer(), vsBuffer->GetBufferSize(), &inputLayout);
		if (FAILED(res))
			return -1;
		ID3DBlob *psBuffer = nullptr;
		if (!dx4s::tools::ShaderCompile("shader.fx", "PS_Main", "ps_4_0", &psBuffer))
			return -1;
		res = d3dDevice->CreatePixelShader(psBuffer->GetBufferPointer(), psBuffer->GetBufferSize(), 0, &PS);
		if (FAILED(res))
			return -1; 
		Vertex vertexPos[] = {
			{ XMFLOAT3(0.5f, 0.5f, 0.5f), XMFLOAT2(1.0f, 0.0f) },
			{ XMFLOAT3(0.5f, -0.5f, 0.5f), XMFLOAT2(1.0f, 1.0f) },
			{ XMFLOAT3(-0.5f, -0.5f, 0.5f), XMFLOAT2(0.0f, 1.0f) },
			{ XMFLOAT3(-0.5f, -0.5f, 0.5f), XMFLOAT2(0.0f, 1.0f) },
			{ XMFLOAT3(-0.5f, 0.5f, 0.5f), XMFLOAT2(0.0f, 0.0f) },
			{ XMFLOAT3(0.5f, 0.5f, 0.5f), XMFLOAT2(1.0f, 0.0f) },
		};

		D3D11_BUFFER_DESC BufferDesc = { 0 };
		BufferDesc.ByteWidth = sizeof(Vertex) * 6;
		BufferDesc.Usage = D3D11_USAGE_DEFAULT;
		BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		D3D11_SUBRESOURCE_DATA subData = { 0 };
		subData.pSysMem = vertexPos;
		res = d3dDevice->CreateBuffer(&BufferDesc, &subData, &vertexBuffer);
		if (FAILED(res))
			return -1;

		res = D3DX11CreateShaderResourceViewFromFile(
			this->d3dDevice,
			"Tex.dds",
			NULL,
			NULL,
			&colorMap,
			NULL
		);

		if (FAILED(res))
			return -1;
		D3D11_SAMPLER_DESC colorMapDesc;
		ZeroMemory(&colorMapDesc, sizeof(D3D11_SAMPLER_DESC));
		colorMapDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
		colorMapDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
		colorMapDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
		colorMapDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
		colorMapDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
		colorMapDesc.MaxLOD = D3D11_FLOAT32_MAX;

		res = d3dDevice->CreateSamplerState(&colorMapDesc, &colorMap_Sampler);
		if (FAILED(res))
			return -1;
*/
		sprite[1] = new dx4s::utils::Sprite2d(this->d3dDevice, this->d3dContext, "Tex.dds");
		sprite[0] = new dx4s::utils::Sprite2d(this->d3dDevice, this->d3dContext, "AlphaTex.png");

		sprite[0]->SetPosition(100.0f, 200.0f);
		sprite[1]->SetPosition(300.0f, 400.0f);
		return 0;
	}

	dx4s::utils::Sprite2d *sprite[2];
	float rot = 0;

	void Render(float dt) {
		float colors[] = { 0.0f,0.0f,0.00f,1.0f };
		d3dContext->ClearRenderTargetView(Target, colors);
		sprite[0]->Render();
		sprite[1]->SetRotation(rot);
		rot += 0.1;
		sprite[1]->Render();
		swapChain->Present(0, 0);
	}
};

DX4S_MAIN(MainClass, "DirectX For Study", 800, 600)