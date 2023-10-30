#pragma once
#include "framework.h"

LRESULT __stdcall MainWndMsgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

class App
{
public:
	App(HINSTANCE hInstance);
	App(const App& rhs) = delete;
	App& operator=(const App& rhs) = delete;
	~App();
protected:
	HINSTANCE hAppInst;

public:
	static App* GetApp();
protected:
	static App* thisApp;

public:
	void Initialize();
	void InitMainWnd();
	void InitD2D();
	virtual void InitChildClass() = 0;
protected:
	HWND hAppMainWnd;
	CComPtr<ID2D1Factory> pD2DFactroy;
	CComPtr<ID2D1HwndRenderTarget> pRenderTarget;
	CComPtr<ID2D1SolidColorBrush> pSolidColorBrush;
	CComPtr<IWICImagingFactory> pWICFactory;
	CComPtr<IDWriteFactory> pWriteFactory;
	CComPtr<IDWriteTextFormat> pStyle;

public:
	void CreateBitmapW(const wchar_t* addr, ID2D1Bitmap** bitmap);
	void CreateTextLayoutW(std::wstring s, float maxHeight, float maxWidth, IDWriteTextLayout** textLayout);
	void DrawRect(const RECT& r);
	void DrawFillRect(const RECT& r);
	void DrawFillRect(const D2D_RECT_F& r);
	void DrawBitmap(ID2D1Bitmap* b, const RECT& r);
	void DrawTextW(const std::wstring& t, const RECT& r, const DWRITE_PARAGRAPH_ALIGNMENT paragraphAlignment, DWRITE_TEXT_ALIGNMENT textAlignment);
	void DrawTextLayout(IDWriteTextLayout* l, const POINTF& p);

public:
	LRESULT __stdcall AppMsgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
	virtual LRESULT __stdcall ChildMsgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) = 0;
	int Run();
	void OnResize(UINT height, UINT width);
	virtual void Update() = 0;
	virtual void Draw() = 0;
};