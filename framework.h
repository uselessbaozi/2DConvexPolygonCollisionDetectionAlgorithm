#pragma once

#pragma comment (lib, "d2d1.lib")
#pragma comment (lib, "DWrite.lib")
#include <d2d1.h>
#include <d2d1helper.h>
#include <dwrite.h>
#include <wincodec.h>

#include <Windows.h>
#include <windowsx.h>
#include <wrl/client.h>
#define CComPtr Microsoft::WRL::ComPtr

#include <string>
#include <cctype>
#include <vector>
#include <list>
#include <cmath>
#include <algorithm>
#include <memory>
#include <cstdlib>
#include <wchar.h>
#include <locale>
#include <codecvt>
#include <random>

#include <ThreadPool.h>

namespace BasicGraphics
{
	struct ColorF
	{
		float r;
		float g;
		float b;
		float a;

		operator D2D1::ColorF()
		{
			return D2D1::ColorF(r, g, b, a);
		}
	};
	struct PointF
	{
		float x;
		float y;

		operator D2D_POINT_2F()
		{
			return D2D_POINT_2F{ x,y };
		}
		void operator+= (const float& delta)
		{
			this->x += delta;
			this->y += delta;
		}
		PointF operator+ (const float& delta)const
		{
			return PointF{ this->x + delta,this->y + delta };
		}
		PointF operator- (const float& delta)const
		{
			return PointF{ this->x - delta,this->y - delta };
		}
		PointF operator+ (const PointF& delta)const
		{
			return PointF{ this->x + delta.x,this->y + delta.y };
		}
		PointF operator- (const PointF& delta)const
		{
			return PointF{ this->x - delta.x,this->y - delta.y };
		}
		PointF operator*(const float& delta)const
		{
			return PointF{ this->x * delta,this->y * delta };
		}
		double operator*(const PointF& delta)const
		{
			return this->x * delta.x + this->y * delta.y;
		}
		PointF operator/(const float& delta)const
		{
			return PointF{ this->x / delta,this->y / delta };
		}
	};
	typedef PointF VectorF;
	inline double GetLength(VectorF v)
	{
		return sqrt(v.x * v.x + v.y * v.y);
	}
	struct VectorCompare
	{
		bool operator() (const BasicGraphics::VectorF& a, const BasicGraphics::VectorF& b) const
		{
			if (a.x < a.y)
				return true;
			else
				return false;
		}
	};
	struct Physics
	{
		Physics(bool stationary, float m = 1.0f) :stationary(stationary), m(m), v{ 0,0 }, a{ 0,0 }{}

		float m;
		VectorF v;
		VectorF a;
		bool stationary;
	};
	struct MutexWithMove
	{
		MutexWithMove() = default;
		MutexWithMove(MutexWithMove&&) :mtx() {}

		std::mutex mtx;
	};
	struct Graphics
	{
		Graphics(PointF address, bool stationary, float m = 1.0f) 
			:address(address), color(ColorF{ 0.5,0.5,0.5,1.0 }), phy(stationary, m){}

		MutexWithMove mtx;
		PointF address;
		ColorF color;
		Physics phy;
	};
	struct Rectangle :public Graphics
	{
		Rectangle(float left, float top, float right, float bottom, PointF address, bool stationary, float m = 1.0f)
			:left(left), top(top), right(right), bottom(bottom), Graphics(address, stationary, m) {}

		float left;
		float top;
		float right;
		float bottom;

		operator D2D_RECT_F()
		{
			return D2D_RECT_F{ address.x + left,address.y + top,address.x + right,address.y + bottom };
		}
	};
	struct ConvexPolygon :public Graphics
	{
		ConvexPolygon(std::vector<VectorF> vertex, PointF address, bool stationary, float m = 1.0f)
			:vertex(vertex), Graphics(address, stationary, m) {}

		std::vector<VectorF> vertex;

		CComPtr<ID2D1PathGeometry> pGeometry = nullptr;
	};
	struct Circle :public Graphics
	{
		Circle(float radius, PointF address, bool stationary, float m = 1.0f)
			:radius(radius), Graphics(address, stationary, m) {}

		float radius;

		operator D2D1_ELLIPSE()
		{
			return D2D1_ELLIPSE{ address + radius,radius,radius };
		}
	};
}

inline bool iscontrol(wchar_t c)
{
	UINT32 i(c);
	if (c < 0x20 || (c < 0x80 && c>0x70))
		return true;
	else
		return false;
}