#pragma once
#include "../utils.h"

using namespace DirectX;

namespace dx4s {
	namespace utils {
		class Sprite2d {
		public:
			Sprite2d(ID3D11Device *dxdev, ID3D11DeviceContext *dxcon, const char *SrcFile);
			~Sprite2d();
			bool Render();
			void SetPosition(float x, float y);
			void SetRotation(float rot);
			void SetScale(float x, float y);

		private:

			XMMATRIX getWorldMatrix(void);

			ID3D11Device *d3dDevice;
			ID3D11DeviceContext *d3dContext;
			ID3D11ShaderResourceView *colorMap;
			ID3D11SamplerState *colorMap_Sampler;
			ID3D11BlendState *blendState;
			ID3D11Buffer *vertexBuffer = nullptr;
			ID3D11VertexShader *VS = nullptr;
			ID3D11PixelShader *PS = nullptr;
			ID3D11InputLayout *inputLayout = nullptr;
			XMFLOAT2 Position;
			XMFLOAT2 Scale;
			float Rotation;
			XMMATRIX vpMatrix;
			ID3D11Buffer *mvp;

			char *shaderCode = "\
				cbuffer cbChangesPerFrame : register(b0){\
					matrix mvp;\
				}\
				Texture2D colorMap :register(t0);\
				SamplerState colorMap_Sampler : register(s0);\
				struct Vertex {\
					float4 pos : POSITION;\
					float2 tex : TEXTURE;\
				};\
				struct Input {\
					float4 pos : SV_POSITION;\
					float2 tex : TEXTURE;\
				};\
				Input VS_Main(Vertex vertex) {\
					Input Out = (Input)0;\
					Out.pos = mul(vertex.pos, mvp);\
					Out.tex = vertex.tex;\
					return Out;\n\
				}\
				float4 PS_Main(Input input) : SV_TARGET{\
					return colorMap.Sample(colorMap_Sampler, input.tex);\
				}\
			";
		};
	}
}