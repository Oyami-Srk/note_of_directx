#pragma once

#define DX4S_MAIN(__ARG_NAME_OF_CLASS__,__ARG_WINDOW_TITLE__,__ARG_WINDOW_WIDTH__,__ARG_WINDOW_HEIGHT__)\
	int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR pScmdline, int iCmdshow) {\
		__ARG_NAME_OF_CLASS__ DX4S; \
		return DX4S(hInstance, __ARG_WINDOW_TITLE__, __ARG_WINDOW_WIDTH__, __ARG_WINDOW_HEIGHT__); \
}

#include "../dx4s.h"

namespace dx4s {
	class base {
	public:
		enum ValueFlag {
			ScreenSizeX, ScreenSizeY, PosX, PosY,	//float
			IsQuit, IsFullScr, FpsDisp, FpsLimit,	//bool
			Position, ScreenSize,					//Double Value:float
			AppName,								//LPCSTR*
			FpsLimitCont							//int
		};
	public:		//Main Function
		int operator()(HINSTANCE hInstance, const char *Title, int Width, int Height);
		~base();
		LRESULT CALLBACK MessageHandler(HWND, UINT, WPARAM, LPARAM);
		static LRESULT CALLBACK WndProc(HWND _hwnd, UINT _msg, WPARAM _wparam, LPARAM _lparam);
		void Get(ValueFlag Flag, ...);
		void Set(ValueFlag Flag, ...);
		int Run(void);


	private:	//DirectX Function
		int Dx_Initialize(void);
		int Dx_Release(void);

	public:		//User Defined Function
		virtual int Init() { return 0; }
		virtual int Load() { return 0; }
		virtual int Release() { return 0; }
		virtual void Update(float dt) {}
		virtual void Render(float dt) {}

	private:	//Value
		bool m_fulScr = false;//Not Full Scrren
		bool m_isExit = false;//Not Exiting
		bool isDispFps = true;
		bool isLimitFps = true;
		int Fps_Limit_Count = 60;
		struct {
			int Width, Height;
			int PosX = -1, PosY = -1;
		} m_dispInfo;

		LPCSTR m_ApplicationName;
		HINSTANCE m_hInstance;
		HWND m_hWnd;

	protected:	//DirectX
		ID3D11Device *d3dDevice;
		ID3D11DeviceContext *d3dContext;
		IDXGISwapChain *swapChain;
		D3D_FEATURE_LEVEL featureLevel;
		D3D_DRIVER_TYPE driverType;
		ID3D11RenderTargetView *Target;
	};
	static base *ApplicationHandle = nullptr;//Ó¦ÓÃ³ÌÐò¾ä±ú
}