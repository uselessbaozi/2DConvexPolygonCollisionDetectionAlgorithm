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
		float m;
		VectorF v;
		VectorF a;
		bool stationary;
	};
	struct Rectangle
	{
		PointF address;
		float left;
		float top;
		float right;
		float bottom;
		ColorF color;
		Physics phy;

		operator D2D_RECT_F()
		{
			return D2D_RECT_F{ address.x + left,address.y + top,address.x + right,address.y + bottom };
		}
	};
	struct ConvexPolygon
	{
		PointF address;
		std::vector<VectorF> vertex;
		ColorF color;
		Physics phy;

		CComPtr<ID2D1PathGeometry> pGeometry = nullptr;
	};
	struct Circle
	{
		PointF address;
		float radius;
		ColorF color;
		Physics phy;

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