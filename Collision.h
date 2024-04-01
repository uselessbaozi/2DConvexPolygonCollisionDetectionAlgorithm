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

	bool RoughCollision(const BasicGraphics::PointF& p1, const BasicGraphics::PointF& p2, const int MaxBlock);
	void CollisionResponse(BasicGraphics::Graphics& g1, BasicGraphics::Graphics& g2, const BasicGraphics::VectorF depth);
}