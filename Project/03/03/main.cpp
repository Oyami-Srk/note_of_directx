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
		return 0;
	}

	typedef struct Vertex {
		XMFLOAT3 Position;
		//XMFLOAT2 Texture;
	} Vertex;

	int Load() {
		D3D11_INPUT_ELEMENT_DESC inputLayoutDesc[] = {
			{
				"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0
			},
		};
		HRESULT res;
		ID3DBlob *vsBuffer = nullptr;
		if (!dx4s::utils::ShaderCompile("shader.fx", "VS_Main", "vs_4_0", &vsBuffer))
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
		if (!dx4s::utils::ShaderCompile("shader.fx", "PS_Main", "ps_4_0", &psBuffer))
			return -1;
		res = d3dDevice->CreatePixelShader(psBuffer->GetBufferPointer(), psBuffer->GetBufferSize(), 0, &PS);
		if (FAILED(res))
			return -1; 
		Vertex vertexPos[] = {
			XMFLOAT3(0.0f, 0.5f, 0.1f),
			XMFLOAT3(0.5f, -0.5f, 0.1f),
			XMFLOAT3(-0.5f, -0.5f, 0.1f),
		};
		D3D11_BUFFER_DESC BufferDesc = { 0 };
		BufferDesc.ByteWidth = sizeof(Vertex) * 3;
		BufferDesc.Usage = D3D11_USAGE_DEFAULT;
		BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		D3D11_SUBRESOURCE_DATA subData = { 0 };
		subData.pSysMem = vertexPos;
		res = d3dDevice->CreateBuffer(&BufferDesc, &subData, &vertexBuffer);
		if (FAILED(res))
			return -1;
		return 0;
	}

	void Render(float dt) {
		float colors[] = { 0.0f,0.0f,0.25f,1.0f };
		d3dContext->ClearRenderTargetView(Target, colors);

		d3dContext->VSSetShader(VS, 0, 0);
		d3dContext->PSSetShader(PS, 0, 0);
		d3dContext->IASetInputLayout(inputLayout);
		unsigned int stride = sizeof(XMFLOAT3);
		unsigned int offset = 0;
		d3dContext->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
		d3dContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		d3dContext->Draw(3, 0);
		swapChain->Present(0, 0);
	}
};

DX4S_MAIN(MainClass, "DirectX For Study", 800, 600)