#include "Ground.h"

Ground::Ground()
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

	BasicGraphics::ConvexPolygon temp{};
	temp.address = address;
	temp.vertex = tempVertex;
	temp.phy.stationary = stationary;
	temp.color = BasicGraphics::ColorF{ 0.5,0.5,0.5,1.0 };
	temp.phy.m = 1.0;
	temp.phy.a = { 0.0,0.0 };
	temp.phy.v = { 0.0,0.0 };
	GroundConvexPolygon.push_back(temp);
}

void Ground::AddRectangle(const BasicGraphics::PointF address, const bool stationary, const float left, const float top, const float right, const float bottom)
{
	BasicGraphics::Rectangle temp{};
	temp.address = address;
	temp.phy.stationary = stationary;
	temp.left = left;
	temp.top = top;
	temp.right = right;
	temp.bottom = bottom;
	temp.color = BasicGraphics::ColorF{ 0.5,0.5,0.5,1.0 };
	temp.phy.m = 1.0;
	temp.phy.a = { 0.0,0.0 };
	temp.phy.v = { 0.0,0.0 };
	GroundRectangle.push_back(temp);
}

void Ground::AddCircle(const BasicGraphics::PointF address, const bool stationary, const float radius)
{
	BasicGraphics::Circle temp{};
	temp.address = address;
	temp.phy.stationary = stationary;
	temp.radius = radius;
	temp.color = BasicGraphics::ColorF{ 0.5,0.5,0.5,1.0 };
	temp.phy.m = 1.0;
	temp.phy.a = { 0.0,0.0 };
	temp.phy.v = { 0.0,0.0 };
	GroundCircle.push_back(temp);
}

bool RoughCollision(const BasicGraphics::PointF& p1, const BasicGraphics::PointF& p2)
{
	float dx(p1.x - p2.x), dy(p1.y - p2.y);
	if (dx<-MAXBLOCK || dx>MAXBLOCK || dy<-MAXBLOCK || dy>MAXBLOCK)
		return false;
	else
		return true;
}

void Ground::CollisionStart()
{
	BasicGraphics::VectorF depth;
	int circleSize(GroundCircle.size()),
		polygonSize(GroundConvexPolygon.size()),
		rectSize(GroundRectangle.size());
	for (int i = 0; i < circleSize - 1; i++)
	{
		for (int j = i + 1; j < circleSize; j++)
		{
			if (!RoughCollision(GroundCircle[i].address, GroundCircle[j].address))
				continue;
			if (!Collision::CircleCircle(GroundCircle[i], GroundCircle[j], depth))
				continue;
			Collision::CollisionResponse(
				GroundCircle[i].address, GroundCircle[i].phy,
				GroundCircle[j].address, GroundCircle[j].phy,
				depth
			);
		}
	}
	for (int i = 0; i < rectSize - 1; i++)
	{
		for (int j = i + 1; j < rectSize; j++)
		{
			if (!RoughCollision(GroundRectangle[i].address, GroundRectangle[j].address))
				continue;
			if (!Collision::RectRect(GroundRectangle[i], GroundRectangle[j], depth))
				continue;
			Collision::CollisionResponse(
				GroundRectangle[i].address, GroundRectangle[i].phy,
				GroundRectangle[j].address, GroundRectangle[j].phy,
				depth
			);
		}
	}
	for (int i = 0; i < polygonSize - 1; i++)
	{
		for (int j = i + 1; j < polygonSize; j++)
		{
			if (!RoughCollision(GroundConvexPolygon[i].address, GroundConvexPolygon[j].address))
				continue;
			if (!Collision::PolygonPolygon(GroundConvexPolygon[i], GroundConvexPolygon[j], depth))
				continue;
			Collision::CollisionResponse(
				GroundConvexPolygon[i].address, GroundConvexPolygon[i].phy,
				GroundConvexPolygon[j].address, GroundConvexPolygon[j].phy,
				depth
			);
		}
	}
	for (int i = 0; i < circleSize; i++)
	{
		for (int j = 0; j < rectSize; j++)
		{
			if (!RoughCollision(GroundCircle[i].address, GroundRectangle[j].address))
				continue;
			if (!Collision::CircleRect(GroundCircle[i], GroundRectangle[j], depth))
				continue;
			Collision::CollisionResponse(
				GroundCircle[i].address, GroundCircle[i].phy,
				GroundRectangle[j].address, GroundRectangle[j].phy,
				depth
			);
		}
	}
	for (int i = 0; i < circleSize; i++)
	{
		for (int j = 0; j < polygonSize; j++)
		{
			if (!RoughCollision(GroundCircle[i].address, GroundConvexPolygon[j].address))
				continue;
			if (!Collision::CirclePolygon(GroundCircle[i], GroundConvexPolygon[j], depth))
				continue;
			Collision::CollisionResponse(
				GroundCircle[i].address, GroundCircle[i].phy,
				GroundConvexPolygon[j].address, GroundConvexPolygon[j].phy,
				depth
			);
		}
	}
	for (int i = 0; i < polygonSize; i++)
	{
		for (int j = 0; j < rectSize; j++)
		{
			if (!RoughCollision(GroundConvexPolygon[i].address, GroundRectangle[j].address))
				continue;
			if (!Collision::PolygonRect(GroundConvexPolygon[i], GroundRectangle[j], depth))
				continue;
			Collision::CollisionResponse(
				GroundConvexPolygon[i].address, GroundConvexPolygon[i].phy,
				GroundRectangle[j].address, GroundRectangle[j].phy,
				depth
			);
		}
	}
}