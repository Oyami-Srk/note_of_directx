Texture2D colorMap :register(t0);
SamplerState colorMap_Sampler :register(s0);

struct Vertex {
	float4 pos : POSITION;
	float2 tex : TEXTURE;
};

struct Input {
	float4 pos : SV_POSITION;
	float2 tex : TEXTURE;
};

Input VS_Main(Vertex vertex) {
	Input Out = (Input)0;
	Out.pos = vertex.pos;
	Out.tex = vertex.tex;
	return Out;
}

float4 PS_Main(Input input) : SV_TARGET {
	return colorMap.Sample(colorMap_Sampler, input.tex);
}
