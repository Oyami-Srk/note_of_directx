# DirectX*11*学习笔记[3]
Author: [*OyaKti(SRK)*](http://blog.qvq.moe/ "OyaKti的弱鸡心得")
Date: 2016/7/25-27
## Direct2D初步 三角形的绘制
------
<!--more-->
<span id="home"></span>
[1.导出项目模板以便操作](#1)
[2.计算机几何简介](#2)
[3.定义三角形](#3)
[4.渲染三角形](#4)
    [4-1.初始化渲染环境](#4.1)
    [4-2.执行渲染](#4.2)
[x.附录](#x)


* ## <span id="1">导出项目模板以便操作</span>

    Visual Studio有一种很方便的功能,能将现有的项目导出至模板从而不用每一次都从头创建新的工程.我们现在将上次笔记所创建的项目导出至模板.
    首先,点击文件->导出模板, 这里我们要创建一个项目模板. 所以点击项目模板然后选中我们的项点击下一步.
    按照图中填入库并点击确定,然后你就可以通过新建项目来直接建立Dx4s的项目啦!
    目前我已将dx4s上传至github, 不方便下载工程的可以去github克隆. [点我进入](https://github.com/OyamiKoroto/dx4s "gayhub).

* ## <span id="2">计算机几何简介</span>

    正如我们现实中的几何学, 计算机渲染中所有的几何体都是由点线面组成的. 而归根结底都是由点组成的, 点与点之间的连线构成了几何体的边, 边构成了面, 最后面构成了体.
    同样在计算机几何里, 顶点就是构成物体的基础. 顶点定义了空间里的一个点, 通过这个点和其的各种属性, 以构成不同的几何体来提供给渲染器渲染. 顶点的属性十分多, 但是位置属性是其最基础的属性. 通常我们用三个浮点型来分别表述xyz属性.
    尽管DirectX是3D渲染的API, 但是我们依旧可以利用它来高效的渲染2D图形.
    我们主要讨论2D图形的渲染, 所以更多有关于计算机几何的知识我没有在此进行说明, 我会在以后的笔记里进行更详细的叙述. 目前, 我们仅需知道顶点的作用和定义就可以了.

* ## <span id="3">定义三角形</span>

    三角形是计算机图形学里的常见图形, 定义并渲染一个三角形通常是很多图形引擎教程的第一课, 所以, 我们也要定义并渲染一个三角形.
    一个三角形显而易见由三个顶点构成, 每一个顶点都包含三个浮点数. 我们使用XNA的数学库来定义一个顶点的结构体.
    ```
    typedef struct Vertex{
        XMFLOAT3 Position;
    } Vertex;
    ```
    可能有人会觉得奇怪, 这里只有一个成员, 但是为什么要单独定义一个结构体而不直接使用数组呢. 原因很简单, 前面已经说了, 顶点的属性不单单包含位置关系而且还包含其他的一些属性, 我们定义一个结构体就是为了能让其他的属性一并包含在里面, 这里我们只定义了最基础的位置属性, 以后我们还会定义其他的属性. 通常情况下大多数程序猿会用Vector(向量)来代替Vertex(顶点)的位置属性, 这是合情合理的, 不单单是因为这俩长得像, 而且从数学上来讲, 也是正确的. 向量通常表示空间内的某一个点到原点的方向, 而3D空间的原点的xyz分量通常总为0, 所以向量的方向刚好和这个点相对于原点的相对位置相同. 而相对于原点的相对位置也就是绝对位置, 所以Vector结构通常也用来定义Vertex的位置.
    知道如何定义顶点了,我们接下来就定义三个顶点来构造出一个三角形.
    构造三角形之前, 我们需要定义坐标系. 虽然DirectX没有强制的坐标系规定, 但是因为DirectX默认使用的是左手坐标系, 所以我们将不做多余的工作, 也使用左手坐标系.
    坐标系的介绍不过多阐述, Z轴正方向向里, X轴正方向向屏幕右侧, Y轴则是上方.
    我们现在定义了坐标系, 就要定义顶点了. 顶点的定义如下.
    ```
    Vertex vertexs[] = {
		XMFLOAT3(0.0f, 0.0f, 0.1f),
		XMFLOAT3(0.5f, -0.5f, 0.1f),
		XMFLOAT3(-0.5f, -0.5f, 0.1f),
	};
    ```
    通常情况下, 为了节约内存开销, 定义顶点应遵循只定义唯一顶点的原则.
    我们现在就有了顶点的定义数据, 接下来, 我们就要将数据传递给显卡, 并经过着色器进行渲染.

* ## <span id="4.1">渲染三角形: 初始化渲染环境</span>

    我们已经定义了构成一个三角形所需的三个顶点, 现在要将这些顶点放置在缓存里, 这就叫做顶点缓存. 通常缓存的位置是在最有利于和图形硬件交换数据的地方, 也就是显存. 以下代码显示了如何创建一个顶点缓存.
    ```
    ID3D11Buffer *vertexBuffer = nullptr;   //目标缓存, 定义为全局变量
    D3D11_BUFFER_DESC BufferDesc = { 0 };   //缓存描述符
	BufferDesc.ByteWidth = sizeof(Vertex) * 3;     //设置缓存大小
	BufferDesc.Usage = D3D11_USAGE_DEFAULT;    //设置缓存方法
	BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;   //设置缓存绑定方式
	D3D11_SUBRESOURCE_DATA subData = { 0 };    //子资源
	subData.pSysMem = vertexs;   //用顶点数据填充子资源
	res = d3dDevice->CreateBuffer(&BufferDesc, &subData, &vertexBuffer);   //创建缓存
	if (FAILED(res))
		return -1;
    ```
    首先我们需要对缓存描述符进行初始化, 缓存大小即是我们定义的顶点数据的大小.
    在我们已经确定好缓存的数据的情况下, 就要将其填充至缓存, 这种时候我们就需要使用子资源来填充缓存. D3D11_SUBRESOURCE_DATA有三个成员, pSysMem为一个内存指针, 指向我们需要填充到缓存的数据. SysMemPitch和SysMemSlicePitch则用于处理贴图.
    CreateBuffer函数用于创建缓存, 参数分别为缓存描述符地址, 子资源地址和缓存目标地址. 通过这个函数, 我们就可以创建一个由描述符描述, 子数据填充的顶点缓存了.
    缓存通常是储存在显存内, 用于提供给图形设备渲染的数据. 但是缓存说白了就是内存里的一大坨数据, 发送给图形设备, 它并不能知道这坨数据的顺序, 大小, 属性等.所以我们要通知图形设备各种数据的各种属性, D3D提供了一个描述符, 叫做输入布局描述符. 该描述符组成的数组描述了一个顶点的结构布局. 该描述符的定义如下:
    ```
    typedef struct {
        LPCSTR SemanticName;
        UINT SemanticIndex;
        DXGI_FORMAT Format;
        UINT InputSlot;
        UINT AlignedByteOffset;
        D3D11_INPUT_CLASSIFICATION InputSlotClass;
        UINT InstanceDataStepRate;
    } 	D3D11_INPUT_ELEMENT_DESC;
    ```
    第一个元素是语义名称, 让元素绑定一个HLSL着色器(shader)作为输入输出对象. 第二个是语义索引, 不同元素若想绑定相同的语义时, 利用语义索引来进行处理. 第三个是元素的格式, 指明了数据的格式. 第四个是输入槽, 当绑定和传递了多个顶点缓存, 输入槽就是顶点缓存的索引. 第五个是字节对齐偏移量, 字面意义. 第六个是输入槽类型, 指明了元素的输入类型(逐顶点或逐对象). 第七个是实例的数量.
    创建一个输入布局需要使用CreateInputLayout函数, 该函数原型如下:
    ```
    HRESULT CreateInputLayout(
        const D3D11_INPUT_ELEMENT_DESC *pInputElementDescs,
        UINT NumElements,
        const void *pShaderBytecodeWithInputSignature,
        SIZE_T BytecodeLength,
        ID3D11InputLayout **ppInputLayout
    );
    ```
    第一个参数是输入布局描述符数组的地址, 第二个则是数组元素的数量, 第三个是一个着色器已编译代码的地址, 第四个是着色器代码的大小. 第五个则是输入布局的对象指针.
    创建一个输入布局, 所需要的参数中有着色器. 那么什么是着色器呢? 着色器是在GPU上执行的一种程序, 用来渲染. 以下代码显示了如何编译一个顶点着色器代码.
    ```
    DWORD Flag = D3DCOMPILE_ENABLE_STRICTNESS;
    #if defined(_DEBUG) || defined(DEBUG)
	   Flag |= D3DCOMPILE_DEBUG;
    #endif
	ID3DBlob *errBuffer = nullptr;
    ID3DBlob *vsBuffer = nullptr;
	HRESULT res = D3DX11CompileFromFile("shader.fx", NULL, NULL, "VS_Main", "vs_4_0", Flag, NULL, NULL, &vsBuffer, &errBuffer, NULL);
		if (FAILED(res)) {
			if (errBuffer) {
				OutputDebugString((LPCSTR)errBuffer->GetBufferPointer());
				errBuffer->Release();
			}
		}
	if (errBuffer)
		errBuffer->Release();
    ```
    我们使用D3DX11CompileFromFile来编译着色器代码, 该函数原型如下:
    ```
    HRESULT D3DX11CompileFromFile(
        LPCTSTR pSrcFile,
        const D3D10_SHADER_MACRO* pDefines,
        LPD3D10INCLUDE pInclude,
        LPCSTR pFunctionName,
        LPCSTR pProfile,
        UINT Flags1,
        UINT Flags2,
        ID3DX11ThreadPump* pPump,
        ID3D10Blob** ppShader,
        ID3D10Blob** ppErrorMsgs,
        HRESULT* pHResult
    );  
    ```
    第一个参数为着色器源码的地址, 第二个则是着色器的全局宏, 类似于C语言中的宏, 若要定义一个宏, 语法如下:
    ```
    const D3D10_SHADER_MACRO Macros[] = { "HELLO", "hello", };
    ```
    第三各参数处理着色器的include语句, 第四个参数是着色器入口点, 第五个参数是着色器类型, 第六个参数是[编译标识](#x), 第七个参数是[特效标识](#x), 第八个是用于异步编译着色器的线程pump指针, 第九个是编译完成后字节码的存放地址, 第十个是错误和警告信息的内存地址. 第十一个则是线程pump的返回值.
    上面的代码可以写作函数来方便调用:
    ```
    //utils.h
    namespace dx4s {
        class utils {
            public:
            static bool ShaderCompile(const char *Filename,
                const char *entryPoint,
                const char *proFile,
                ID3DBlob **buffer,
                UINT Flag = D3DCOMPILE_ENABLE_STRICTNESS
            );
        };
    }

    //utils.cpp
    bool dx4s::utils::ShaderCompile(const char *Filename, const char *entryPoint, const char *proFile, ID3DBlob **buffer, UINT Flag /* = D3DCOMPILE_ENABLE_STRICTNESS */) {
        #if defined(_DEBUG) || defined(DEBUG)
        Flag |= D3DCOMPILE_DEBUG;
        #endif
        ID3DBlob *errBuffer = nullptr;
        HRESULT res = D3DX11CompileFromFile(Filename, NULL, NULL, entryPoint, proFile, Flag, NULL, NULL, buffer, &errBuffer, NULL);
        if (FAILED(res)) {
            if (errBuffer) {
                OutputDebugString((LPCSTR)errBuffer->GetBufferPointer());
                errBuffer->Release();
            }
            return false;
        }
        if (errBuffer)
        errBuffer->Release();
        return true;
    }
    ```
    利用该函数编译一个顶点着色器的代码如下:
    ```
    ID3DBlob *vsBuffer = nullptr;
	if (!dx4s::utils::ShaderCompile("shader.fx", "VS_Main", "vs_4_0", &vsBuffer))
			return -1;
    ```
    编译完了一个顶点着色器, 我们就获得了这个着色器的字节码, 但是若想将这些字节码运行在GPU上, 我们就要加载我们已经编译好的字节码.
    以下代码创建了一个顶点着色器:
    ```
    ID3D11VertexShader *VS = nullptr;   //该变量应为全局变量
    HRESULT res = d3dDevice->CreateVertexShader(vsBuffer->GetBufferPointer(), vsBuffer->GetBufferSize(), 0, &VS);
	if (FAILED(res)) {
		if (vsBuffer)
			vsBuffer->Release();
		return -1;
	}
    ```
    然后就是指定顶点元素的布局, 我们已经介绍了输入布局描述符(D3D11_INPUT_ELEMENT_DESC)的定义, 我们现在就要初始化一个语义为POSITION的, 格式为DXGI_FORMAT_R32G32B32_FLOAT, 输入类型为逐顶点的布局数组:
    ```
    D3D11_INPUT_ELEMENT_DESC inputLayoutDesc[] = {
		{
			"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0
		},
	};
    HRESULT res = d3dDevice->CreateInputLayout(inputLayoutDesc, ARRAYSIZE(inputLayoutDesc), vsBuffer->GetBufferPointer(), vsBuffer->GetBufferSize(), &inputLayout);
	if (FAILED(res))
		return -1;
    ```
    然后我们通常要载入像素着色器, 代码和载入顶点着色器类似.
    ```
    ID3DBlob *psBuffer = nullptr;
	if (!dx4s::utils::ShaderCompile("shader.fx", "PS_Main", "ps_4_0", &psBuffer))
		return -1;
	HRESULT res = d3dDevice->CreatePixelShader(psBuffer->GetBufferPointer(), psBuffer->GetBufferSize(), 0, &PS);
	if (FAILED(res))
		return -1;
    ```
    至此, 我们就完成了所有初始化的工作. 这些工作应该仅执行一次, 所以我们就在Load()函数内执行这些工作. 为了渲染, 我们需要将以下变量定义为全局变量.
    ```
    ID3D11Buffer *vertexBuffer = nullptr;
	ID3D11VertexShader *VS = nullptr;
	ID3D11PixelShader *PS = nullptr;
	ID3D11InputLayout *inputLayout = nullptr;
    ```
    至此, 所有渲染环境的工作都进行完毕. 可以进行渲染了.

* ## <span id="4.2">渲染三角形: 执行渲染</span>

    我们已经创建了顶点着色器, 像素着色器, 输入布局, 顶点缓存在初始化代码中. 若要进行渲染, 我们首先要绑定这些.
    用以下两条代码绑定顶点着色器和像素着色器.
    ```
    d3dContext->VSSetShader(VS, 0, 0);
	d3dContext->PSSetShader(PS, 0, 0);
    ```
    这两条代码的参数相似, 分别为着色器, 实例数组, 实例数量.
    然后用`d3dContext->IASetInputLayout(inputLayout);`绑定输入布局.
    最后我们要载入顶点缓存, 代码如下:
    ```
    unsigned int stride = sizeof(XMFLOAT3);
	unsigned int offset = 0;
	d3dContext->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
    ```
    IASetVertexBuffers用于载入一个或多个顶点缓存, 原型如下:
    ```
    void IASetVertexBuffers(
        UINT StartSlot,
        UINT NumBuffers,
        ID3D11Buffer* const* ppVertexBuffers,
        const UINT* pStrides,
        const UINT* pOffsets
    );
    ```
    第一个参数是绑定缓存的开始槽, 也就是缓存数组中缓存的索引编号. 第二个是设置缓存数量, 第三个是缓存数组, 第四个是定点步长, 是每个顶点的字节数. 最后一个参数是字节偏移量.第三第四个参数若有多个缓存则为数组.
    然后我们就要设置几何类型, 使用如下代码设置一个三角形列表(若仅有三个顶点, 则为三角形)的几何类型.
    ```
    d3dContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    ```
    最后, 我们用如下代码将三角形绘制出来:
    ```
    d3dContext->Draw(3, 0);
	swapChain->Present(0, 0);
    ```
    Draw函数的第一个参数代表了顶点数量, 第二个则是开始顶点的位置, 也就是开始顶点距离顶点缓存开头的偏移量.
    至此, 我们就完成了绘制一个三角形的所有工作.
    等等, 你说shader.fx? 对了, 我们还需要写着色器代码.
    shader.fx全文如下:
    ```
    float4 VS_Main(float4 pos : POSITION) : SV_POSITION {
    	return pos;
    }
    float4 PS_Main(float4 pos : SV_POSITION) : SV_TARGET {
    	return float4(0.5f,0.0f,0.0f,1.0f);
    }
    ```
    VS_Main是顶点着色器, 接收我们的POSITION语义绑定的输入布局, 返回值则是SV_POSITION, 顶点着色器对传入的顶点位置进行处理后输出, 但是我们这里不用做任何处理, 直接输出就可以了. 像素着色器始终返回一个固定的颜色值, 并写入缓存, 通过调用交换链的Present来显示到屏幕上.
    现在, 一个红色的三角形就绘制完成了!

* ## <span id="x">附录</span>
    编译标识:
    ```
    D3D10_SHADER_AVOID_FLOW_CONTROL                 //任何时候都禁止流控制  
    D3D10_SHADER_DEBUG                              //编译着色器时插入调试信息  
    D3D10_SHADER_ENABLE_STRICTNESS                  //禁止过时的语法  
    D3D10_SHADER_ENABLE_BACKWARDS_COMPATIBILITY     //在着色器 4.0 中允许过时的语法  
    D3D10_SHADER_FORCE_VS_SOFTWARE_NO_OPT           //强制顶点着色器在下一个最高的支持版本上编译  
    D3D10_SHADER_FORCE_PS_SOFTWARE_NO_OPT           //强制像素着色器在下一个最高的支持版本上编译  
    D3D10_SHADER_IEEE_STRICTNESS                    //在编译器中启用严格 IEEE 标准  
    D3D10_SHADER_NO_PRESHADER                       //禁止编译器从静态表达式中退出  
    D3D10_SHADER_OPTIMIZATION_LEVEL0 (0-3)          //设置优化级别，第 0 级不优化，运行最慢，第 3 级完全优化
    D3D10_SHADER_PACK_MATRIX_ROW_MAJOR              //使用行主序布局的矩阵声明  
    D3D10_SHADER_PACK_MATRIX_COLUMN_MAJOR           //使用列主序布局的矩阵声明  
    D3D10_SHADER_PARTIAL_PRECISION                  //强制使用局部精度计算，在某些硬件上会提升性能  
    D3D10_SHADER_PREFER_FLOW_CONTROL                //告诉编译器在任何可能的时候尽量使用流控制
    D3D10_SHADER_SKIP_OPTIMIZATION                  //编译着色器代码时，完全跳过优化阶段  
    D3D10_SHADER_WARNINGS_ARE_ERRORS                //编译时将警告视为错误  
    ```
    特效标识:
    ```
    D3D10_EFFECT_COMPILE_CHILD_EFFECT       //允许编译子特效  
    D3D10_EFFECT_COMPILE_ALLOW_SLOW_OPS     //禁止最佳性能模式  
    D3D10_EFFECT_SINGLE_THREADED            //加载特效池(pool)时禁止与其它线程同步
    ```
