#include "App.h"

LRESULT __stdcall MainWndMsgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	return App::GetApp()->AppMsgProc(hwnd, msg, wParam, lParam);
}

App* App::thisApp = nullptr;

App::App(HINSTANCE hInstance) :hAppInst(hInstance), hAppMainWnd(nullptr)
{
	thisApp = this;
}

App::~App()
{
}

App* App::GetApp()
{
	return thisApp;
}

void App::Initialize()
{
	InitMainWnd();
	InitD2D();
	InitChildClass();
}

void App::InitMainWnd()
{
	WNDCLASS wc;
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = MainWndMsgProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = hAppInst;
	wc.hIcon = LoadIcon(0, IDI_APPLICATION);
	wc.hCursor = LoadCursor(0, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(NULL_BRUSH);
	wc.lpszMenuName = 0;
	wc.lpszClassName = L"DrawFillTriangle";

	if (!RegisterClass(&wc))
		throw - 1;

	RECT R = { 0, 0, 800, 600 };
	AdjustWindowRect(&R, WS_OVERLAPPEDWINDOW, false);
	int width = R.right - R.left;
	int height = R.bottom - R.top;

	hAppMainWnd = CreateWindow(L"DrawFillTriangle", L"DrawFillTriangle",
		WS_OVERLAPPEDWINDOW ^ WS_THICKFRAME, CW_USEDEFAULT, CW_USEDEFAULT, width, height, 0, 0, hAppInst, 0);
	if (!hAppMainWnd)
		throw - 1;

	ShowWindow(hAppMainWnd, SW_SHOW);
	UpdateWindow(hAppMainWnd);

}

void App::InitD2D()
{
	HRESULT hr(S_OK);

	hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, pD2DFactroy.GetAddressOf());
	if (FAILED(hr))
		throw hr;

	D2D_SIZE_U size(D2D1::SizeU(800, 600));
	hr = pD2DFactroy->CreateHwndRenderTarget(
		D2D1::RenderTargetProperties(),
		D2D1::HwndRenderTargetProperties(
			hAppMainWnd,
			size
		),
		pRenderTarget.GetAddressOf()
	);
	if (FAILED(hr))
		throw hr;

	hr = pRenderTarget->CreateSolidColorBrush(
		D2D1::ColorF(D2D1::ColorF::Black),
		pSolidColorBrush.GetAddressOf()
	);
	if (FAILED(hr))
		throw hr;

	hr = CoCreateInstance(
		CLSID_WICImagingFactory,
		NULL,
		CLSCTX_INPROC_SERVER,
		IID_PPV_ARGS(pWICFactory.GetAddressOf())
	);
	if (FAILED(hr))
		throw hr;

	hr = DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(pWriteFactory.Get()), reinterpret_cast<IUnknown**>(pWriteFactory.GetAddressOf()));
	if (FAILED(hr))
		throw hr;

	hr = pWriteFactory->CreateTextFormat(
		L"Arial",
		NULL,
		DWRITE_FONT_WEIGHT_NORMAL,
		DWRITE_FONT_STYLE_NORMAL,
		DWRITE_FONT_STRETCH_NORMAL,
		10.0f,
		L"zh-cn",
		pStyle.GetAddressOf()
	);
	if (FAILED(hr))
		throw hr;

	hr = pStyle->SetWordWrapping(DWRITE_WORD_WRAPPING_NO_WRAP);
	if (FAILED(hr))
		throw hr;
}

void App::CreateBitmapW(const wchar_t* addr, ID2D1Bitmap** bitmap)
{
	HRESULT hr(S_OK);
	CComPtr<IWICBitmapDecoder> pDecoder;
	CComPtr<IWICBitmapFrameDecode> pSource;
	CComPtr<IWICFormatConverter> pConverter;

	hr = pWICFactory->CreateDecoderFromFilename(
		addr,
		NULL,
		GENERIC_READ,
		WICDecodeMetadataCacheOnLoad,
		pDecoder.GetAddressOf()
	);
	if (FAILED(hr))
		throw hr;

	hr = pDecoder->GetFrame(0, pSource.GetAddressOf());
	if (FAILED(hr))
		throw hr;

	hr = pWICFactory->CreateFormatConverter(pConverter.GetAddressOf());
	if (FAILED(hr))
		throw hr;

	hr = pConverter->Initialize(
		pSource.Get(),
		GUID_WICPixelFormat32bppPBGRA,
		WICBitmapDitherTypeNone,
		NULL,
		0.f,
		WICBitmapPaletteTypeCustom
	);
	if (FAILED(hr))
		throw hr;

	hr = pRenderTarget->CreateBitmapFromWicBitmap(
		pConverter.Get(),
		NULL,
		bitmap
	);
	if (FAILED(hr))
		throw hr;
}

void App::CreateTextLayoutW(std::wstring s, float maxHeight, float maxWidth, IDWriteTextLayout** textLayout)
{
	HRESULT hr;
	pStyle->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
	pStyle->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);
	hr = pWriteFactory->CreateTextLayout(
		s.c_str(), s.length(), pStyle.Get(), maxWidth, maxHeight, textLayout
	);
	if (FAILED(hr))
		throw hr;

	DWRITE_TEXT_RANGE temp;
	temp.startPosition = 0;
	temp.length = s.length();
	hr = (*textLayout)->SetFontSize(maxHeight, temp);
	if (FAILED(hr))
		throw hr;
}

inline D2D1_RECT_F RectToD2D(const RECT& r)
{
	return D2D1_RECT_F{
			(float)r.left,
			(float)r.top,
			(float)r.right,
			(float)r.bottom
	};
}
void App::DrawRect(const RECT& r)
{
	pSolidColorBrush->SetColor(D2D_COLOR_F{ 0.0,0.0,0.0,1.0 });
	pRenderTarget->DrawRectangle(RectToD2D(r), pSolidColorBrush.Get());
}

void App::DrawFillRect(const RECT& r)
{
	pSolidColorBrush->SetColor(D2D_COLOR_F{ 0.0,0.0,0.0,1.0 });
	pRenderTarget->FillRectangle(RectToD2D(r), pSolidColorBrush.Get());
}

void App::DrawFillRect(const D2D_RECT_F& r)
{
	pSolidColorBrush->SetColor(D2D_COLOR_F{ 0.0,0.0,0.0,1.0 });
	pRenderTarget->FillRectangle(r, pSolidColorBrush.Get());
}

void App::DrawBitmap(ID2D1Bitmap* b, const RECT& r)
{
	pRenderTarget->DrawBitmap(b, RectToD2D(r));
}

void App::DrawTextW(const std::wstring& t, const RECT& r, const DWRITE_PARAGRAPH_ALIGNMENT paragraphAlignment, DWRITE_TEXT_ALIGNMENT textAlignment)
{
	pSolidColorBrush->SetColor(D2D_COLOR_F{ 0.0,0.0,0.0,1.0 });
	pStyle->SetParagraphAlignment(paragraphAlignment);
	pStyle->SetTextAlignment(textAlignment);
	pRenderTarget->DrawTextW(t.c_str(), t.length(), pStyle.Get(), RectToD2D(r), pSolidColorBrush.Get());
}

void App::DrawTextLayout(IDWriteTextLayout* l, const POINTF& p)
{
	pSolidColorBrush->SetColor(D2D_COLOR_F{ 0.0,0.0,0.0,1.0 });
	pRenderTarget->DrawTextLayout(D2D1_POINT_2F{ p.x,p.y }, l, pSolidColorBrush.Get());
}

LRESULT __stdcall App::AppMsgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
//#ifdef _DEBUG
//	wchar_t c[20], endc[3](L"\n\0");
//	_ultow_s(msg, c, 16);
//	wcscat_s(c, endc);
//	OutputDebugString(c);
//#endif // _DEBUG

	switch (msg)
	{
	case WM_SIZE:
	{
		UINT width(LOWORD(lParam));
		UINT height(HIWORD(lParam));
		OnResize(width, height);
		break;
	}
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return ChildMsgProc(hwnd, msg, wParam, lParam);
	}
	return 0;
}

int App::Run()
{
	MSG msg{ 0 };
	while (msg.message != WM_QUIT)
	{
		if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			Update();
			Draw();
		}
	}

	return (int)msg.wParam;
}

void App::OnResize(UINT height, UINT width)
{
	if (!pRenderTarget)
		return;
	pRenderTarget->Resize(D2D1::SizeU(width, height));
}