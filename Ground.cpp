#include "Ground.h"

Ground::Ground() :pool(ThreadPool::Pool::GetPool())
{
	thisGround = this;
}

Ground::~Ground()
{
}

Ground* Ground::GetGround()
{
	return thisGround;
}

Ground* Ground::thisGround = nullptr;

void Ground::AddConvexPolygon(const BasicGraphics::PointF address, const bool stationary, const std::vector<BasicGraphics::VectorF> vertex)
{
	int pointNum(vertex.size());
	if (pointNum < 3)
		throw - 1;

	float x(0.0), y(0.0);
	for (auto& iter : vertex)
	{
		x += iter.x;
		y += iter.y;
	}
	BasicGraphics::PointF centerPoint{ x / pointNum,y / pointNum };

	std::vector<std::pair<BasicGraphics::VectorF, double>> tempVertexMap;
	double vertexAngle(0.0);
	for (auto& iter : vertex)
	{
		vertexAngle = atan2(iter.x - centerPoint.x, iter.y - centerPoint.y);
		if (vertexAngle < 0.0)
			vertexAngle += 180.0;
		if (!std::any_of(tempVertexMap.begin(), tempVertexMap.end(), [vertexAngle](const std::pair<BasicGraphics::VectorF, double>& o) {
			return o.second == vertexAngle;
			}))
			tempVertexMap.push_back(std::pair<BasicGraphics::VectorF, double>(iter, vertexAngle));
	}

	std::sort(tempVertexMap.begin(), tempVertexMap.end(), [](const auto& a, const auto& b)->bool
		{
			return a.second > b.second;
		});

	std::vector<BasicGraphics::PointF> tempVertex;
	for (auto& iter : tempVertexMap)
	{
		tempVertex.push_back(iter.first);
	}

	/*BasicGraphics::ConvexPolygon temp;
	temp.address = address;
	temp.vertex = tempVertex;
	temp.phy.stationary = stationary;
	temp.color = BasicGraphics::ColorF{ 0.5,0.5,0.5,1.0 };
	temp.phy.m = 1.0;
	temp.phy.a = { 0.0,0.0 };
	temp.phy.v = { 0.0,0.0 };*/
	GroundConvexPolygon.push_back(std::move(BasicGraphics::ConvexPolygon(
		std::move(tempVertex), address, stationary
	)));
}

void Ground::AddRectangle(const BasicGraphics::PointF address, const bool stationary, const float left, const float top, const float right, const float bottom)
{
	/*BasicGraphics::Rectangle temp;
	temp.address = address;
	temp.phy.stationary = stationary;
	temp.left = left;
	temp.top = top;
	temp.right = right;
	temp.bottom = bottom;
	temp.color = BasicGraphics::ColorF{ 0.5,0.5,0.5,1.0 };
	temp.phy.m = 1.0;
	temp.phy.a = { 0.0,0.0 };
	temp.phy.v = { 0.0,0.0 };*/
	GroundRectangle.push_back(std::move(BasicGraphics::Rectangle(
		left, top, right, bottom, address, stationary
	)));
}

void Ground::AddCircle(const BasicGraphics::PointF address, const bool stationary, const float radius)
{
	/*BasicGraphics::Circle temp;
	temp.address = address;
	temp.phy.stationary = stationary;
	temp.radius = radius;
	temp.color = BasicGraphics::ColorF{ 0.5,0.5,0.5,1.0 };
	temp.phy.m = 1.0;
	temp.phy.a = { 0.0,0.0 };
	temp.phy.v = { 0.0,0.0 };*/
	GroundCircle.push_back(BasicGraphics::Circle(
		radius, address, stationary
	));
}

void Ground::CollisionStart()
{
	//BasicGraphics::VectorF depth;
	int circleSize(GroundCircle.size()),
		polygonSize(GroundConvexPolygon.size()),
		rectSize(GroundRectangle.size());
	for (int i = 0; i < circleSize - 1; i++)
	{
		for (int j = i + 1; j < circleSize; j++)
		{
			pool->AddTask([i, j, this]() {
				BasicGraphics::VectorF depth;
				std::scoped_lock lock(GroundCircle[i].mtx.mtx, GroundCircle[j].mtx.mtx);
				if (!Collision::RoughCollision(GroundCircle[i].address, GroundCircle[j].address, MAXBLOCK))
					return;
				if (!Collision::CircleCircle(GroundCircle[i], GroundCircle[j], depth))
					return;
				Collision::CollisionResponse(
					GroundCircle[i], GroundCircle[j],
					depth);
				});
		}
	}
	for (int i = 0; i < rectSize - 1; i++)
	{
		for (int j = i + 1; j < rectSize; j++)
		{
			pool->AddTask([i, j, this]() {
				BasicGraphics::VectorF depth;
				std::scoped_lock lock(GroundRectangle[i].mtx.mtx, GroundRectangle[j].mtx.mtx);
				if (!Collision::RoughCollision(GroundRectangle[i].address, GroundRectangle[j].address, MAXBLOCK))
					return;
				if (!Collision::RectRect(GroundRectangle[i], GroundRectangle[j], depth))
					return;
				Collision::CollisionResponse(
					GroundRectangle[i], GroundRectangle[j],
					depth);
				});
		}
	}
	for (int i = 0; i < polygonSize - 1; i++)
	{
		for (int j = i + 1; j < polygonSize; j++)
		{
			pool->AddTask([i, j, this]() {
				BasicGraphics::VectorF depth;
				std::scoped_lock lock(GroundConvexPolygon[i].mtx.mtx, GroundConvexPolygon[j].mtx.mtx);
				if (!Collision::RoughCollision(GroundConvexPolygon[i].address, GroundConvexPolygon[j].address, MAXBLOCK))
					return;
				if (!Collision::PolygonPolygon(GroundConvexPolygon[i], GroundConvexPolygon[j], depth))
					return;
				Collision::CollisionResponse(
					GroundConvexPolygon[i], GroundConvexPolygon[j],
					depth);
				});
			//if (!Collision::RoughCollision(GroundConvexPolygon[i].address, GroundConvexPolygon[j].address, MAXBLOCK))
			//	continue;
			//if (!Collision::PolygonPolygon(GroundConvexPolygon[i], GroundConvexPolygon[j], depth))
			//	continue;
			//Collision::CollisionResponse(
			//	GroundConvexPolygon[i], GroundConvexPolygon[j],
			//	depth);
		}
	}
	for (int i = 0; i < circleSize; i++)
	{
		for (int j = 0; j < rectSize; j++)
		{
			pool->AddTask([i, j, this]() {
				BasicGraphics::VectorF depth;
				std::scoped_lock lock(GroundCircle[i].mtx.mtx, GroundRectangle[j].mtx.mtx);
				if (!Collision::RoughCollision(GroundCircle[i].address, GroundRectangle[j].address, MAXBLOCK))
					return;
				if (!Collision::CircleRect(GroundCircle[i], GroundRectangle[j], depth))
					return;
				Collision::CollisionResponse(
					GroundCircle[i], GroundRectangle[j],
					depth);
				});
			//if (!Collision::RoughCollision(GroundCircle[i].address, GroundRectangle[j].address, MAXBLOCK))
			//	continue;
			//if (!Collision::CircleRect(GroundCircle[i], GroundRectangle[j], depth))
			//	continue;
			//Collision::CollisionResponse(
			//	GroundCircle[i], GroundRectangle[j],
			//	depth);
		}
	}
	for (int i = 0; i < circleSize; i++)
	{
		for (int j = 0; j < polygonSize; j++)
		{
			pool->AddTask([i, j, this]() {
				BasicGraphics::VectorF depth;
				std::scoped_lock lock(GroundCircle[i].mtx.mtx, GroundConvexPolygon[j].mtx.mtx);
				if (!Collision::RoughCollision(GroundCircle[i].address, GroundConvexPolygon[j].address, MAXBLOCK))
					return;
				if (!Collision::CirclePolygon(GroundCircle[i], GroundConvexPolygon[j], depth))
					return;
				Collision::CollisionResponse(
					GroundCircle[i], GroundConvexPolygon[j],
					depth);
				});
			//if (!Collision::RoughCollision(GroundCircle[i].address, GroundConvexPolygon[j].address, MAXBLOCK))
			//	continue;
			//if (!Collision::CirclePolygon(GroundCircle[i], GroundConvexPolygon[j], depth))
			//	continue;
			//Collision::CollisionResponse(
			//	GroundCircle[i], GroundConvexPolygon[j],
			//	depth);
		}
	}
	for (int i = 0; i < polygonSize; i++)
	{
		for (int j = 0; j < rectSize; j++)
		{
			pool->AddTask([i, j, this]() {
				BasicGraphics::VectorF depth;
				std::scoped_lock lock(GroundConvexPolygon[i].mtx.mtx, GroundRectangle[j].mtx.mtx);
				if (!Collision::RoughCollision(GroundConvexPolygon[i].address, GroundRectangle[j].address, MAXBLOCK))
					return;
				if (!Collision::PolygonRect(GroundConvexPolygon[i], GroundRectangle[j], depth))
					return;
				Collision::CollisionResponse(
					GroundConvexPolygon[i], GroundRectangle[j],
					depth);
				});
			//if (!Collision::RoughCollision(GroundConvexPolygon[i].address, GroundRectangle[j].address, MAXBLOCK))
			//	continue;
			//if (!Collision::PolygonRect(GroundConvexPolygon[i], GroundRectangle[j], depth))
			//	continue;
			//Collision::CollisionResponse(
			//	GroundConvexPolygon[i], GroundRectangle[j],
			//	depth);
		}
	}
	pool->WaitAll();
}