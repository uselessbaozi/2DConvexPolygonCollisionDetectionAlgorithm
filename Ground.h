#pragma once
#include "framework.h"
#include "Collision.h"
constexpr int MAXBLOCK(52);
class Ground
{
public:
	Ground();
	Ground(const Ground& rhs) = delete;
	Ground& operator=(const Ground& rhs) = delete;
	~Ground();

public:
	static Ground* GetGround();
private:
	static Ground* thisGround;

public:
	void AddConvexPolygon(const BasicGraphics::PointF address, const bool stationary, const std::vector<BasicGraphics::VectorF> vertex);
	void AddRectangle(const BasicGraphics::PointF address, const bool stationary, const float left, const float top, const float right, const float bottom);
	void AddCircle(const BasicGraphics::PointF address, const bool stationary, const float radius);
	void CollisionStart();

public:
	std::vector<BasicGraphics::Circle> GroundCircle;
	std::vector<BasicGraphics::Rectangle> GroundRectangle;
	std::vector<BasicGraphics::ConvexPolygon> GroundConvexPolygon;

	ThreadPool::Pool* pool;
};