#include "base.h"
#include <cstdarg>
#include <cstdio>
#include <ctime>
#include <thread>
#include <mutex>
#include <mmsystem.h>
#pragma comment( lib,"winmm.lib" )

int dx4s::base::operator()(HINSTANCE hInstance, const char * Title, int Width, int Height) {
	if (this->m_isExit && this->m_fulScr) {
		OutputDebugString("Error: Symbols Value Wrong!\n");
		return 1;
	}
	if (this->Init() != 0) {
		OutputDebugString("Wrong In DirectX Func: Init.\n");
		return 3;
	}

	WNDCLASSEX wce;
	DEVMODE devScrSetting = { 0 };
	ApplicationHandle = this;//Maybe Delete;
	this->m_ApplicationName = Title;//Application Name;
	this->m_hInstance = hInstance;
	wce.style = CS_HREDRAW | CS_VREDRAW;
	wce.lpfnWndProc = WndProc;
	wce.cbClsExtra = 0;
	wce.cbWndExtra = 0;
	wce.hInstance = this->m_hInstance;
	wce.hIcon = LoadIcon(NULL, IDI_WINLOGO);
	wce.hIconSm = wce.hIcon;
	wce.hCursor = LoadCursor(NULL, IDC_ARROW);
	wce.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);//Default Black Backgroung;
	wce.lpszMenuName = NULL;
	wce.lpszClassName = this->m_ApplicationName;
	wce.cbSize = sizeof(WNDCLASSEX);

	//注册窗口类
	if (!RegisterClassEx(&wce)) {
		OutputDebugString("Cannot Register Class!");
		return 2;
	}

	if (this->m_fulScr) {
		devScrSetting.dmSize = sizeof(devScrSetting);
		devScrSetting.dmPelsWidth = (unsigned long)GetSystemMetrics(SM_CXSCREEN);
		devScrSetting.dmPelsHeight = (unsigned long)GetSystemMetrics(SM_CYSCREEN);
		devScrSetting.dmBitsPerPel = 32;
		devScrSetting.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;
		ChangeDisplaySettings(&devScrSetting, CDS_FULLSCREEN);
		this->Set(Position, 0, 0);
		this->Set(ScreenSize, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN));
	} else {
		int tmpPosX = 0, tmpPosY = 0;
		this->Get(Position, &tmpPosX, &tmpPosY);
		if (tmpPosX == tmpPosY && tmpPosX == -1) {
			this->Set(
				Position,
				(GetSystemMetrics(SM_CXSCREEN) - Width) / 2,
				(GetSystemMetrics(SM_CYSCREEN) - Height) / 2
			);
		}
		this->Set(ScreenSize, Width, Height);
	}
	this->Get(ScreenSize, &Width, &Height);
	bool isFulScr = false;
	this->Get(IsFullScr, &isFulScr);
	int posX = 0, posY = 0;
	this->Get(Position, &posX, &posY);

	if (isFulScr) {
		this->m_hWnd = CreateWindowEx(WS_EX_APPWINDOW, this->m_ApplicationName, this->m_ApplicationName,
									  WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_POPUP,
									  posX, posY, 
									  GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN), 
									  NULL, NULL, this->m_hInstance, NULL);
	} else {
		RECT rect = { 0, 0, Width, Height };
		AdjustWindowRect(&rect, WS_OVERLAPPEDWINDOW, false);
		this->m_hWnd = CreateWindowEx(WS_EX_APPWINDOW, this->m_ApplicationName, this->m_ApplicationName,
									  WS_OVERLAPPEDWINDOW,
									  posX, posY,
									  rect.right - rect.left, rect.bottom - rect.top,
									  NULL, NULL, this->m_hInstance, NULL);
	}

	char DebugOutBuffer[256] = { 0 };

	sprintf(DebugOutBuffer, "Width=%d;Height=%d\n", Width, Height);
	OutputDebugString(DebugOutBuffer);
	memset(DebugOutBuffer, 0, 256);

	sprintf(DebugOutBuffer, "Pos_X=%d;Pos_Y=%d\n", posX, posY);
	OutputDebugString(DebugOutBuffer);
	memset(DebugOutBuffer, 0, 256);

	sprintf(DebugOutBuffer, "Class Name=%s\n", this->m_ApplicationName);
	OutputDebugString(DebugOutBuffer);
	memset(DebugOutBuffer, 0, 256);

	ShowWindow(this->m_hWnd, SW_SHOW);
	SetForegroundWindow(this->m_hWnd);
	SetFocus(this->m_hWnd);
	if (this->Dx_Initialize() != 0) {
		OutputDebugString("Wrong In DirectX Func: Dx_Initialize.\n");
		return 5;
	}
	if (this->Load() != 0) {
		OutputDebugString("Wrong In DirectX Func: Load\n");
		return 4;
	}
	return this->Run();
}

dx4s::base::~base() {
	try {
		if (this->Release() != 0)
			OutputDebugString("Wrong In User Defined Func : Release.\n");
		if (this->Dx_Release() != 0)
			OutputDebugString("Wrong In DirectX Func: Dx_Release.\n");
	} catch (...) {
		OutputDebugString("High Level Error(s) in Destoty Function!\n");
	}
}

LRESULT dx4s::base::MessageHandler(HWND _hwnd, UINT _msg, WPARAM _wparam, LPARAM _lparam) {
	switch (_msg) {
	default:
		return DefWindowProc(_hwnd, _msg, _wparam, _lparam);
		break;
	case WM_KEYDOWN:
		if (_wparam == VK_ESCAPE) {
			this->Set(IsQuit, true);
			return 0;
		}
		//send to other func...
		break;
	case WM_KEYUP:
		//send to other func...
		break;
	//case WM_MOUSE...;
	}
}

LRESULT dx4s::base::WndProc(HWND _hwnd, UINT _msg, WPARAM _wparam, LPARAM _lparam) {
	switch (_msg) {
	default:
		return ApplicationHandle->MessageHandler(_hwnd, _msg, _wparam, _lparam);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
		break;
	case WM_CLOSE:
		PostQuitMessage(0);
		return 0;
		break;
	}
}

void dx4s::base::Get(ValueFlag Flag, ...) {
	va_list args;
	va_start(args, Flag);
	switch (Flag) {
	case IsQuit:
		*va_arg(args, bool*) = this->m_isExit;
		break;
	case IsFullScr:
		*va_arg(args, bool*) = this->m_fulScr;
		break;
	case ScreenSize:
		*va_arg(args, int*) = this->m_dispInfo.Width;
		*va_arg(args, int*) = this->m_dispInfo.Height;
		break;
	case ScreenSizeX:
		*va_arg(args, int*) = this->m_dispInfo.Width;
		break;
	case ScreenSizeY:
		*va_arg(args, int*) = this->m_dispInfo.Height;
		break;
	case Position:
		*va_arg(args, int*) = this->m_dispInfo.PosX;
		*va_arg(args, int*) = this->m_dispInfo.PosY;
		break;
	case PosX:
		*va_arg(args, int*) = this->m_dispInfo.PosX;
		break;
	case PosY:
		*va_arg(args, int*) = this->m_dispInfo.PosY;
		break;
		break;
	case AppName:
		*va_arg(args, LPCSTR*) = this->m_ApplicationName;
		break;
	default:
		*va_arg(args, int*) = 0;
	}
	va_end(args);
}

void dx4s::base::Set(ValueFlag Flag, ...) {
	va_list args;
	va_start(args, Flag);
	switch (Flag) {
	case IsQuit:
		this->m_isExit = va_arg(args, bool);
		break;
	case IsFullScr:
		this->m_fulScr = va_arg(args, bool);
		break;
	case ScreenSize:
		this->m_dispInfo.Width = va_arg(args, int);
		this->m_dispInfo.Height = va_arg(args, int);
		break;
	case ScreenSizeX:
		this->m_dispInfo.Width = va_arg(args, int);
		break;
	case ScreenSizeY:
		this->m_dispInfo.Height = va_arg(args, int);
		break;
	case Position:
		this->m_dispInfo.PosX = va_arg(args, int);
		this->m_dispInfo.PosY = va_arg(args, int);
		break;
	case PosX:
		this->m_dispInfo.PosX = va_arg(args, int);
		break;
	case PosY:
		this->m_dispInfo.PosY = va_arg(args, int);
		break;
	case FpsDisp:
		this->isDispFps = va_arg(args, bool);
		break;
	case FpsLimit:
		this->isLimitFps = va_arg(args, bool);
		break;
	case FpsLimitCont:
		this->Fps_Limit_Count = va_arg(args, int);
		break;
	}
	va_end(args);
}

int dx4s::base::Dx_Initialize(void) {
	RECT dimensions;
	GetClientRect(this->m_hWnd, &dimensions);

	unsigned int width = dimensions.right - dimensions.left;
	unsigned int height = dimensions.bottom - dimensions.top;
	D3D_DRIVER_TYPE driverTypes[] = {
		D3D_DRIVER_TYPE_HARDWARE, //硬件设备,直接渲染
		D3D_DRIVER_TYPE_WARP,	//WARP设备,在CPU上完全模拟D3D
		D3D_DRIVER_TYPE_REFERENCE, //引用设备,在CPU上模拟硬件设备不支持的D3D
		D3D_DRIVER_TYPE_SOFTWARE	//软件设备,开发者自己的渲染驱动
	};
	unsigned int totalDriverTypes = ARRAYSIZE(driverTypes);
	D3D_FEATURE_LEVEL featureLevels[] = {
		D3D_FEATURE_LEVEL_11_0,
		D3D_FEATURE_LEVEL_10_1,
		D3D_FEATURE_LEVEL_10_0
	};
	unsigned int totalFeatureLevels = ARRAYSIZE(featureLevels);

	DXGI_SWAP_CHAIN_DESC swapChainDesc;
	ZeroMemory(&swapChainDesc, sizeof(swapChainDesc));
	swapChainDesc.BufferCount = 1;
	swapChainDesc.BufferDesc.Width = width;
	swapChainDesc.BufferDesc.Height = height;
	swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapChainDesc.BufferDesc.RefreshRate.Numerator = 60;
	swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;//交换链用于输出,可以被渲染
	swapChainDesc.OutputWindow = this->m_hWnd;
	swapChainDesc.Windowed = true;
	swapChainDesc.SampleDesc.Count = 1;
	swapChainDesc.SampleDesc.Quality = 0;

	unsigned int creationFlags = 0;
#ifdef _DEBUG
	creationFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	HRESULT result;
	unsigned int driver = 0;
	for (; driver < totalDriverTypes; ++driver) {
		result = D3D11CreateDeviceAndSwapChain(0,
											   driverTypes[driver],
											   0,
											   creationFlags,
											   featureLevels,
											   totalFeatureLevels,
											   D3D11_SDK_VERSION,
											   &swapChainDesc,
											   &this->swapChain,
											   &this->d3dDevice,
											   &this->featureLevel,
											   &this->d3dContext);
		if (SUCCEEDED(result)) {
			this->driverType = driverTypes[driver];
			char OutBuf[256];
			char *TypeStr = new char[16];
			switch (driverTypes[driver]) {
			case D3D_DRIVER_TYPE_HARDWARE:
				TypeStr = "Hardware";
				break;
			case D3D_DRIVER_TYPE_WARP:
				TypeStr = "Warp";
				break;
			case D3D_DRIVER_TYPE_REFERENCE:
				TypeStr = "Reference";
				break;
			case D3D_DRIVER_TYPE_SOFTWARE:
				TypeStr = "Software";
				break;
			default:
				TypeStr = "Unknow";
			}
			wsprintf(OutBuf, "Debug: DriverType is %s\n", TypeStr);
			OutputDebugString(OutBuf);
			break;
		}
	}

	if (FAILED(result)) {
		OutputDebugString("Error: Failed to create the D3D Device!\n");
		return 1;
	}
	ID3D11Texture2D *backBufferTex;
	result = this->swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&backBufferTex);
	if (FAILED(result)) {
		OutputDebugString("Error: Failed to get the swap chain back buffer!\n");
		return 2;
	}

	result = this->d3dDevice->CreateRenderTargetView(backBufferTex, NULL, &this->Target);
	if (backBufferTex)
		backBufferTex->Release();
	if (FAILED(result)) {
		OutputDebugString("Error: Failed to create the render target view!");
		return 3;
	}
	this->d3dContext->OMSetRenderTargets(1, &this->Target, NULL);
	D3D11_VIEWPORT viewPort;
	viewPort.Width = static_cast<float>(width);
	viewPort.Height = static_cast<float>(height);
	viewPort.MinDepth = 0.0f;
	viewPort.MaxDepth = 1.0f;
	viewPort.TopLeftX = 0.0f;
	viewPort.TopLeftY = 0.0f;
	this->d3dContext->RSSetViewports(1, &viewPort);
	return 0;
}

int dx4s::base::Dx_Release(void) {
	if (this->Target)
		this->Target->Release();
	if (this->swapChain)
		this->swapChain->Release();
	if (this->d3dContext)
		this->d3dContext->Release();
	 if (this->d3dDevice)
	 	this->d3dDevice->Release();
	return 0;
}

int dx4s::base::Run() {
	MSG msg;//消息结构
	bool done, result = true;
	ZeroMemory(&msg, sizeof(MSG));
	done = false;
	double m_LastTime = time(NULL);
	double m_DelayTime = 0;

	LARGE_INTEGER frequency, minDeltaCount, count, oldCount;
	float fFrequency;
	QueryPerformanceFrequency(&frequency);
	fFrequency = (float)frequency.QuadPart;
	minDeltaCount.QuadPart = frequency.QuadPart / Fps_Limit_Count;
	QueryPerformanceCounter(&oldCount); // 获取微秒级时间

	char FPSbuf[64];

	while (!done) {
		//处理Windows消息
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		//接收到WM_QUIT,退出
		if (this->m_isExit || WM_QUIT == msg.message)
			done = true;
		//执行渲染
		this->Update(m_DelayTime);
		this->Render(m_DelayTime);
		m_DelayTime = time(NULL) - m_LastTime;
		if (this->isDispFps) {
			static int frameCount = 0;
			static LARGE_INTEGER lastTime, curTime;

			frameCount++;
			QueryPerformanceCounter(&curTime);
			float deltaTime = (curTime.QuadPart - lastTime.QuadPart) / fFrequency; // 单位s
			if (deltaTime >= 1.0f) {
				float fps = frameCount / deltaTime;
				frameCount = 0;
				sprintf(FPSbuf, "%s |FPS Counter:%3.1f", this->m_ApplicationName, fps);
				SetWindowTextA(this->m_hWnd, FPSbuf);
				lastTime = curTime;
			}
		}

		if (this->isLimitFps) {
			QueryPerformanceCounter(&count);
			LARGE_INTEGER deltaCount;
			while ((deltaCount.QuadPart = count.QuadPart - oldCount.QuadPart) < minDeltaCount.QuadPart) // 经过1/60s之前循环等待
			{
				if (deltaCount.QuadPart * 1000 / frequency.QuadPart < 15) // 经过了15ms以内，数值越大占CPU越少，但太大了会不精确
					Sleep(1); // 把CPU交给其他程序
				QueryPerformanceCounter(&count);
			}
			oldCount = count;
		}
		m_LastTime = time(NULL);
	}
	return 0;
}