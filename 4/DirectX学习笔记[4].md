# DirectX*11*学习笔记[4]
Author: [*OyaKti(SRK)*](http://blog.qvq.moe/ "OyaKti的弱鸡心得")
Date: 2016/7/29-31
## Direct2D进阶 贴图与精灵
------
<!--more-->
<span id="home"></span>
[1.纹理映射简介](#1)
[2.纹理映射操作](#2)
[3.封装精灵类](#3)
    [3-1.精灵类结构](#3-1)
    [3-2.精灵类实现](#3-2)




* ## <span id="1">纹理映射简介</span>

    纹理通常是映射在图形物体表面的颜色值数据, 也可以是其他数据, 例如法线, 透明度等.
    不像opengl这种纯图形API只能用其他库来处理贴图(例如libpng, 我被这东西坑过), DirectX内置了一些贴图处理的方法, 我们可以使用D3DX11CreateTextureFromFile来从文件中加载贴图. 该函数原型如下.
    ```
    HRESULT D3DX11CreateTextureFromFile(
        ID3D11Device* pDevice,
        LPCTSTR pSrcFile,
        D3DX11_IMAGE_LOAD_INFO* pLoadInfo,
        ID3DX11ThreadPump* pPump,
        ID3D11Resource** ppTexture,
        HRESULT* pHResult
    );
    ```
    第一个参数是D3D设备驱动. 第二个则是文件路径. 第三个是图像的结构. 第四个是多线程pump, 异步加载用. 第五个是纹理对象地址. 第六个则是多线程返回值.
    Direct3D中主要有三种纹理类型:ID3D11Texture1D, ID3D11Texture2D, ID3D11Texture3D. 其中最常用的是2D贴图. 1D贴图和3D贴图为高级的特性, 不对其进行介绍.
    纹理贴图中的每个像素被称为纹理元素(Texel), 一个纹理元素在颜色映射中为一个颜色值, 一个颜色值通常是32位的RGBA格式. 若一张纹理贴图过大, 它消耗的显存和GPU是庞大的, Mip Maps就解决了这个问题. Mip分级机制使贴图分为不同大小, 决定使用贴图某种大小的因素则是视口和物体的距离. 当距离过远时, 则使用较低分辨率的贴图以减少消耗, 距离近则采用高分辨率的贴图来提升质量.
    下图显示了Mip不同压缩等级的预览:
    插图4-1

    [返回目录](#home)

* ## <span id="2">纹理映射操作</span>

    通常2D贴图使用UV坐标表示贴图坐标, 而贴图坐标通常是和顶点位置坐标绑定在一起的, 这样子顶点的定义就如下了.
    ```
    typedef struct Vertex {
		XMFLOAT3 Position;
		XMFLOAT2 Texture;
	} Vertex;
    ```
    很显然, 顶点的数据结构发生了改变, 所以我们要重新定义顶点结构. 新的顶点结构如下.
    ```
    D3D11_INPUT_ELEMENT_DESC inputLayoutDesc[] = {
		{
			"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0
		},
		{
			"TEXTURE", 0, DXGI_FORMAT_R32G32_FLOAT, 0,sizeof(XMFLOAT3), D3D11_INPUT_PER_VERTEX_DATA, 0
		}
	};
    ```
    当然, 我们也要重新定义顶点数据, 顶点的位置坐标和UV坐标的关系如下:
    插图4-2.
    但是实际上, DX的UV坐标中的的V坐标是倒置的, 所以我们需要用1.0f去减去正常的V坐标, 才能得到正确的UV坐标, opengl就不这么蛋疼. 所以正确的顶点定义应该如下.
    ```
    Vertex vertexPos[] = {
		{ XMFLOAT3(0.5f, 0.5f, 0.5f), XMFLOAT2(1.0f, 0.0f) },
		{ XMFLOAT3(0.5f, -0.5f, 0.5f), XMFLOAT2(1.0f, 1.0f) },
		{ XMFLOAT3(-0.5f, -0.5f, 0.5f), XMFLOAT2(0.0f, 1.0f) },
		{ XMFLOAT3(-0.5f, -0.5f, 0.5f), XMFLOAT2(0.0f, 1.0f) },
		{ XMFLOAT3(-0.5f, 0.5f, 0.5f), XMFLOAT2(0.0f, 0.0f) },
		{ XMFLOAT3(0.5f, 0.5f, 0.5f), XMFLOAT2(1.0f, 0.0f) },
	};
    ```
    照样加载顶点缓存, 不过缓存的大小可要记得修改为`sizeof(Vertex) * 6`.
    然后我们就要加载贴图了.
    我们需要一个着色器资源视图(ID3D11ShaderResourceView)和采样状态(ID3D11SamplerState)来加载贴图, 在全局中声明这两个变量.
    ```
    ID3D11ShaderResourceView *colorMap;
	ID3D11SamplerState *colorMap_Sampler;
    ```
    着色器资源视图用于访问资源, 就好比指针一样. 然后我们通过D3DX11CreateShaderResourceViewFromFile这个超长的函数来加载着色器资源视图.
    ```
    HRESULT res = D3DX11CreateShaderResourceViewFromFile(
		this->d3dDevice,
		"Tex.dds",
		NULL,
		NULL,
		&colorMap,
		NULL
	);
	if (FAILED(res))
		return -1;
    ```
    D3DX11CreateShaderResourceViewFromFile的定义于D3DX11CreateTextureFromFile, 就不过多介绍了.
    加载贴图的最后一步就是创建采样状态, 其代码如下.
    ```
    D3D11_SAMPLER_DESC colorMapDesc;
	ZeroMemory(&colorMapDesc, sizeof(D3D11_SAMPLER_DESC));
	colorMapDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	colorMapDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	colorMapDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	colorMapDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	colorMapDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	colorMapDesc.MaxLOD = D3D11_FLOAT32_MAX;
	HRESULT res = d3dDevice->CreateSamplerState(&colorMapDesc, &colorMap_Sampler);
    if (FAILED(res))
		return -1;
    ```
    采样状态用于访问一个纹理的采样状态, 而采样状态决定了纹理的绘制方式.
    采样状态描述符(D3D11_SAMPLER_DESC)的定义如下.
    ```
    typedef struct D3D11_SAMPLER_DESC {
        D3D11_FILTER Filter;
        D3D11_TEXTURE_ADDRESS_MODE AddressU;    //纹理地址U的地址模式
        D3D11_TEXTURE_ADDRESS_MODE AddressV;    //纹理地址V的地址模式
        D3D11_TEXTURE_ADDRESS_MODE AddressW;    //纹理地址W的地址模式
        FLOAT MipLODBias;
        UINT MaxAnisotropy;
        D3D11_COMPARISON_FUNC ComparisonFunc;
        FLOAT BorderColor[4];
        FLOAT MinLOD;   //最小Mip级别
        FLOAT MaxLOD;   //最大Mip级别
        } D3D11_SAMPLER_DESC;
    ```
    地址模式用于处理当所给UV坐标超出默认坐标的情况, WRAP为平铺, 一般我们就用这个.
    我们仅需添加两行代码和更改一行代码就能完成渲染,
    ```
    d3dContext->PSSetShaderResources(0, 1, &colorMap);
	d3dContext->PSSetSamplers(0, 1, &colorMap_Sampler);
	d3dContext->Draw(6, 0);
    ```
    PSSetShaderResources和PSSetSamplers给像素着色器加载了资源和采样状态, 这两个函数的参数类似, 第一个参数为开始槽, 第二个则是数量, 第三个则是要加载的资源/采样状态的地址.
    然后就剩下了着色器.
    我们修改了代码中的顶点定义, 着色器也要做出同样的修改.
    全部着色器代码如下.
    ```
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
    ```
    register用于绑定对象, t0表示第一个贴图, s0表示第一个取样状态, 就是这样! 贴图t0通过PSSetShaderResources传递, 而取样状态s0通过PSSetSamplers传递.

    [返回目录](#home)

* ## <span id="3-1">精灵类结构</span>

    精灵类作为dx4s的工具类被开发, 其设计如下图所示.
    插图4-3

    [返回目录](#home)

* ## <span id="3-2">精灵类实现</span>

    详见dx4s\utils\sprite2d.
    插图4-4

    [返回目录](#home)
