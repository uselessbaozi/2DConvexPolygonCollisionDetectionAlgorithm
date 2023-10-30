#pragma once
#include "framework.h"

namespace Collision
{
	bool CircleCircle(const BasicGraphics::Circle& c1, const BasicGraphics::Circle& c2, BasicGraphics::VectorF& depth);
	bool CirclePolygon(const BasicGraphics::Circle& c1, const BasicGraphics::ConvexPolygon& p2, BasicGraphics::VectorF& depth);
	bool CircleRect(const BasicGraphics::Circle& c1, const BasicGraphics::Rectangle& r2, BasicGraphics::VectorF& depth);
	bool PolygonPolygon(const BasicGraphics::ConvexPolygon& p1, const BasicGraphics::ConvexPolygon& p2, BasicGraphics::VectorF& depth);
	bool PolygonRect(const BasicGraphics::ConvexPolygon& p1, const BasicGraphics::Rectangle& r2, BasicGraphics::VectorF& depth);
	bool RectRect(const BasicGraphics::Rectangle& r1, const BasicGraphics::Rectangle& r2, BasicGraphics::VectorF& depth);

	void CollisionResponse(BasicGraphics::PointF& g1Addr, BasicGraphics::Physics& g1Phy, BasicGraphics::PointF& g2Addr, BasicGraphics::Physics& g2Phy, const BasicGraphics::VectorF depth);
}