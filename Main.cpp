#include "Main.h"

class ConvexPolygonCollision :public App
{
public:
	ConvexPolygonCollision(HINSTANCE hInstance);
	~ConvexPolygonCollision();

public:
	void InitChildClass()override;
	void Draw()override;
	void Update()override;
	LRESULT _stdcall ChildMsgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)override;
private:
	Ground* thisGround;
	std::list<std::unique_ptr<AppControl::UnknownControl>> control;

public:
	void CreateGeometry(BasicGraphics::ConvexPolygon& p);
};

std::mt19937 gen(1);
std::uniform_int_distribution<> shortshortDist(3, 7);
std::uniform_int_distribution<> shortDist(20, 50);
std::uniform_int_distribution<> intDist(50, 550);
std::uniform_int_distribution<> longDist(-100, 100);
std::uniform_int_distribution<> boolDist(0, 1);

int	WINAPI wWinMain(
	_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR lpCmdLine,
	_In_ int nShowCmd)
{
	ThreadPool::Pool pool;
	Ground ground;
	ConvexPolygonCollision demo(hInstance);
	demo.Initialize();
	return demo.Run();
}

void AC()
{
	Ground::GetGround()->AddCircle(
		BasicGraphics::PointF{
			1.0f * intDist(gen),
			1.0f * intDist(gen)
		},
		false,
		shortDist(gen)/2
	);
}
void AR()
{
	Ground::GetGround()->AddRectangle(
		BasicGraphics::PointF{
			1.0f * intDist(gen),
			1.0f * intDist(gen)
		},
		false,
		longDist(gen) * 0.01f,
		longDist(gen) * 0.01f,
		longDist(gen) * 0.01f + shortDist(gen),
		longDist(gen) * 0.01f + shortDist(gen)
		);
}
void AP()
{
	std::vector<BasicGraphics::VectorF> temp;
	int a(shortDist(gen) / 2), b(shortDist(gen) / 2) ,
		a2(a * a), b2(b * b),
		size(shortshortDist(gen));
	float x(0.0), y(0.0);
	for (int i = 0; i < size; i++)
	{
		x = longDist(gen) * 0.01 * a;
		y = sqrt((1.0f - x * x / a2) * b2 + b) * (boolDist(gen) ? 1.0 : -1.0);
		temp.push_back(
			BasicGraphics::VectorF{
				x,
				y
			}
		);
	}
	Ground::GetGround()->AddConvexPolygon(
		BasicGraphics::PointF{
			1.0f * intDist(gen),
			1.0f * intDist(gen)
		},
		false,
		temp
	);
}

ConvexPolygonCollision::ConvexPolygonCollision(HINSTANCE hInstance) :App(hInstance)
{
	thisGround = Ground::GetGround();
	
	/*thisGround->AddCircle(BasicGraphics::PointF{ 100.0,100.0 }, true, 10.0);
	std::vector<BasicGraphics::VectorF> temp;
	temp.push_back(BasicGraphics::VectorF{ 0.0,0.0 });
	temp.push_back(BasicGraphics::VectorF{ 10.0,0.0 });
	temp.push_back(BasicGraphics::VectorF{ 0.0,10.0 });
	thisGround->AddConvexPolygon(BasicGraphics::PointF{ 200.0,200.0 }, true, temp);*/
}

ConvexPolygonCollision::~ConvexPolygonCollision()
{
}

void ConvexPolygonCollision::InitChildClass()
{
	std::unique_ptr<AppControl::UnknownControl> temp0(new AppControl::BottomW(RECT{ 10,10,40,40 }, L"circle", L"",AC));
	std::unique_ptr<AppControl::UnknownControl> temp1(new AppControl::BottomW(RECT{ 10,40,40,70 }, L"rect", L"",AR));
	std::unique_ptr<AppControl::UnknownControl> temp2(new AppControl::BottomW(RECT{ 10,70,40,100 }, L"polygon", L"",AP));
	control.push_back(std::move(temp0));
	control.push_back(std::move(temp1));
	control.push_back(std::move(temp2));

}

void ConvexPolygonCollision::Draw()
{
	pRenderTarget->BeginDraw();
	pRenderTarget->Clear(D2D1::ColorF(D2D1::ColorF::White));

	for (auto& iter : thisGround->GroundCircle)
	{
		pSolidColorBrush->SetColor(iter.color);
		pRenderTarget->FillEllipse(iter, pSolidColorBrush.Get());
	}
	for (auto& iter : thisGround->GroundRectangle)
	{
		pSolidColorBrush->SetColor(iter.color);
		pRenderTarget->FillRectangle(iter, pSolidColorBrush.Get());
	}
	for (auto& iter : thisGround->GroundConvexPolygon)
	{
		CreateGeometry(iter);
		pSolidColorBrush->SetColor(iter.color);
		pRenderTarget->FillGeometry(iter.pGeometry.Get(), pSolidColorBrush.Get());
	}
	for (auto& iter : control)
	{
		if (iter->GetVisible())
			iter->Draw();
	}

	HRESULT hr(S_OK);
	hr = pRenderTarget->EndDraw();
	if (FAILED(hr))
		throw - 1;
}

void ConvexPolygonCollision::Update()
{
	thisGround->CollisionStart();
}

LRESULT _stdcall ConvexPolygonCollision::ChildMsgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	bool useMsg(false);
	switch (msg)
	{
	case WM_LBUTTONDOWN:
		for (auto controlIter = control.cbegin(); controlIter != control.cend(); controlIter++)
		{
			if (!controlIter->get()->GetVisible())
				continue;
			if (controlIter->get()->Click(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)))
				break;
		}
		break;
	case WM_LBUTTONUP:
		for (auto controlIter = control.cbegin(); controlIter != control.cend(); controlIter++)
		{
			if (!controlIter->get()->GetVisible())
				continue;
			if (controlIter->get()->UnClick())
				break;
		}
		break;
	case WM_MOUSEMOVE:
		for (auto controlIter = control.cbegin(); controlIter != control.cend(); controlIter++)
		{
			if (!controlIter->get()->GetVisible())
				continue;
			if (controlIter->get()->Move(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)))
				break;
		}
		break;
	case WM_KEYDOWN:
		for (auto controlIter = control.cbegin(); controlIter != control.cend(); controlIter++)
		{
			if (!controlIter->get()->GetVisible())
				continue;
			if (controlIter->get()->KeyDown(wParam))
			{
				useMsg = true;
				break;
			}
		}
		break;
	case WM_CHAR:
		for (auto controlIter = control.cbegin(); controlIter != control.cend(); controlIter++)
		{
			if (!controlIter->get()->GetVisible())
				continue;
			if (controlIter->get()->InputChar(wParam))
				break;
		}
		break;
	default:
		return DefWindowProc(hwnd, msg, wParam, lParam);
	}
	return 0;
}

void ConvexPolygonCollision::CreateGeometry(BasicGraphics::ConvexPolygon& p)
{
	HRESULT hr(S_OK);
	hr = pD2DFactroy->CreatePathGeometry(p.pGeometry.ReleaseAndGetAddressOf());
	if (FAILED(hr))
		throw - 1;

	CComPtr<ID2D1GeometrySink> pSink;
	hr = p.pGeometry->Open(pSink.GetAddressOf());
	if (FAILED(hr))
		throw - 1;

	pSink->SetFillMode(D2D1_FILL_MODE_WINDING);

	size_t pointNum(p.vertex.size());
	if (pointNum == 0)
		throw - 1;
	pSink->BeginFigure(p.vertex[pointNum - 1] + p.address, D2D1_FIGURE_BEGIN_FILLED);
	auto points(std::make_shared<D2D_POINT_2F[]>(pointNum));
	for (int i = 0; i < pointNum; i++)
	{
		points[i] = p.vertex[i] + p.address;
	}
	pSink->AddLines(points.get(), pointNum);
	pSink->EndFigure(D2D1_FIGURE_END_CLOSED);

	hr = pSink->Close();
	if (FAILED(hr))
		throw - 1;
}