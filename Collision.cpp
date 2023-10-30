#include"Collision.h"

BasicGraphics::VectorF GetNormalVector(BasicGraphics::PointF p1, BasicGraphics::PointF p2)
{
	float dx(p2.y - p1.y), dy(p2.x - p1.x);
	BasicGraphics::VectorF result{ -dy,dx };
	return result / BasicGraphics::GetLength(result);
}

bool GetOverlappinglength(const std::vector<float>& p1, const std::vector<float>& p2, float& length)
{
	float p1max(p1[0]), p1min(p1[0]), p2max(p2[0]), p2min(p2[0]);
	for (auto pointIter : p1)
	{
		if (pointIter < p1min)
			p1min = pointIter;
		else if (pointIter > p1max)
			p1max = pointIter;
	}
	for (auto pointIter : p2)
	{
		if (pointIter < p2min)
			p2min = pointIter;
		else if (pointIter > p2max)
			p2max = pointIter;
	}

	if (p1max <= p2min || p2max <= p1min)
	{
		return false;
	}
	else if (p1min <= p2min && p2max <= p1max)
	{
		if (p2min - p1min < p1max - p2max)
			length = p1min - p2max;
		else
			length = p1max - p2min;
	}
	else if (p2min <= p1min && p1max <= p2max)
	{
		if (p2min - p1min < p1max - p2max)
			length = p2max - p1min;
		else
			length = p1min - p1max;
	}
	else
	{
		if (p1max < p2max)
			length = p1max - p2min;
		else
			length = p1min - p2max;
	}
	return true;
}

std::vector<float> ProjectCircle(const BasicGraphics::Circle& c, const BasicGraphics::VectorF& v, const BasicGraphics::VectorF& delta)
{
	float temp((c.address + delta + c.radius) * v);
	std::vector<float> result;
	result.push_back(temp + c.radius);
	result.push_back(temp - c.radius);
	return result;
}

std::vector<float> ProjectRectangle(const BasicGraphics::Rectangle& r, const BasicGraphics::VectorF& v, const BasicGraphics::VectorF& delta)
{
	std::vector<float> result(4);
	BasicGraphics::VectorF deltaAddr(r.address + delta);
	BasicGraphics::PointF temp{ deltaAddr.x + r.left,deltaAddr.y + r.top };
	result[0] = temp * v;
	
	temp.x = deltaAddr.x + r.right;
	temp.y = deltaAddr.y + r.top;
	result[1] = temp * v;

	temp.x = deltaAddr.x + r.right;
	temp.y = deltaAddr.y + r.bottom;
	result[2] = temp * v;

	temp.x = deltaAddr.x + r.left;
	temp.y = deltaAddr.y + r.bottom;
	result[3] = temp * v;

	return result;
}

void ProjectRectangleSp(const BasicGraphics::Rectangle& r, std::vector<float>& rX, std::vector<float>& rY, const BasicGraphics::VectorF& delta)
{
	if (rX.size() != 2)
	{
		rX.resize(2);
	}
	if (rY.size() != 2)
	{
		rY.resize(2);
	}
	rX[0] = r.address.x + delta.x + r.left;
	rX[1] = r.address.x + delta.x + r.right;
	rY[0] = r.address.y + delta.y + r.top;
	rY[1] = r.address.y + delta.y + r.bottom;
}

std::vector<float> ProjectPolygon(const BasicGraphics::ConvexPolygon& p, const BasicGraphics::VectorF& v, const BasicGraphics::VectorF& delta)
{
	std::vector<float> result;
	BasicGraphics::VectorF deltaAddr(p.address + delta);
	for (auto& iter : p.vertex)
	{
		result.push_back((iter + deltaAddr) * v);
	}
	return result;
}

bool Collision::CircleCircle(const BasicGraphics::Circle& c1, const BasicGraphics::Circle& c2, BasicGraphics::VectorF& depth)
{
	BasicGraphics::VectorF deltaCenter((c2.address + c2.radius) - (c1.address + c1.radius));
	double d(BasicGraphics::GetLength(deltaCenter));

	if (d >= c1.radius + c2.radius)
		return false;

	if (d == 0.0)
	{
		depth = { 0.0,c1.radius + c2.radius };
		return true;
	}

	depth = deltaCenter * ((c1.radius + c2.radius) / d - 1.0);
	return true;
}

bool Collision::CirclePolygon(const BasicGraphics::Circle& c1, const BasicGraphics::ConvexPolygon& p2, BasicGraphics::VectorF& depth)
{
	BasicGraphics::VectorF deltaVector(c1.address);

	float depthLength(0.0), tempDepthLength(0.0);
	BasicGraphics::VectorF depthVector{ 0.0 }, tempDepthVector{ 0.0 };
	BasicGraphics::PointF startPoint(p2.vertex[0]), endPoint(p2.vertex[p2.vertex.size() - 1]);
	std::vector<float> c1Project, p2Project;

	depthVector = GetNormalVector(startPoint, endPoint);
	c1Project = ProjectCircle(c1, depthVector, deltaVector);
	p2Project = ProjectPolygon(p2, depthVector, deltaVector);
	if (!GetOverlappinglength(c1Project, p2Project, depthLength))
		return false;
	
	for (int i = 0; i < p2.vertex.size() - 1; i++)
	{
		startPoint = p2.vertex[i];
		endPoint = p2.vertex[i + 1];
		tempDepthVector = GetNormalVector(startPoint, endPoint);
		c1Project = ProjectCircle(c1, tempDepthVector, deltaVector);
		p2Project = ProjectPolygon(p2, tempDepthVector, deltaVector);
		if (!GetOverlappinglength(c1Project, p2Project, tempDepthLength))
			return false;
		if (fabs(tempDepthLength) < fabs(depthLength))
		{
			depthLength = tempDepthLength;
			depthVector = tempDepthVector;
		}
	}

	float pointLenght(FLT_MAX),tempPointLenght(0.0);
	int vertexIndex(0), tempVertexIndex(0);
	BasicGraphics::PointF c1Center(c1.address + c1.radius);
	for (auto& iter : p2.vertex)
	{
		tempPointLenght = BasicGraphics::GetLength(c1Center - (p2.address + iter));
		if (tempPointLenght < pointLenght)
		{
			vertexIndex = tempVertexIndex;
			pointLenght = tempDepthLength;
		}
		tempVertexIndex++;
	}

	tempDepthVector = GetNormalVector(c1Center - deltaVector, p2.vertex[vertexIndex]);
	c1Project = ProjectCircle(c1, tempDepthVector, deltaVector);
	p2Project = ProjectPolygon(p2, tempDepthVector, deltaVector);
	if (!GetOverlappinglength(c1Project, p2Project, tempDepthLength))
		return false;
	if (fabs(tempDepthLength) < fabs(depthLength))
	{
		depthLength = tempDepthLength;
		depthVector = tempDepthVector;
	}
	
	depth = depthVector * depthLength;
	return true;
}

bool Collision::CircleRect(const BasicGraphics::Circle& c1, const BasicGraphics::Rectangle& r2, BasicGraphics::VectorF& depth)
{
	BasicGraphics::VectorF deltaVector(c1.address * -1.0);
	std::vector<float> c1X(2), c1Y(2), r2X(2), r2Y(2);

	c1X[0] = 0;
	c1X[1] = 2 * c1.radius;
	c1Y[0] = 0;
	c1Y[1] = 2 * c1.radius;
	ProjectRectangleSp(r2, r2X, r2Y, deltaVector);

	BasicGraphics::VectorF normalVector{ 1.0,0.0 };
	float length(0.0), tempLenght(0.0);
	if (!GetOverlappinglength(c1X, r2X, length))
		return false;
	if (!GetOverlappinglength(c1Y, r2Y, tempLenght))
		return false;
	if (tempLenght < length)
	{
		normalVector.x = 0.0;
		normalVector.y = 1.0;
		length = tempLenght;
	}

	BasicGraphics::PointF cCenter{ c1.radius };
	if ((r2X[0] <= cCenter.x && cCenter.x <= r2X[1]) || (r2Y[0] <= cCenter.y && cCenter.y <= r2Y[1]))
	{
		depth = normalVector * length;
		return true;
	}

	BasicGraphics::VectorF tempVector;
	if (cCenter.x < r2X[0] && cCenter.y < r2Y[0])
	{
		tempVector.x = r2X[0] - cCenter.x;
		tempVector.y = r2Y[0] - cCenter.y;
	}
	else if (cCenter.x > r2X[1] && cCenter.y > r2Y[1])
	{
		tempVector.x = r2X[1] - cCenter.x;
		tempVector.y = r2Y[1] - cCenter.y;
	}
	else if (cCenter.x < r2X[0])
	{
		tempVector.x = r2X[0] - cCenter.x;
		tempVector.y = r2Y[1] - cCenter.y;
	}
	else
	{
		tempVector.x = r2X[1] - cCenter.x;
		tempVector.y = r2Y[0] - cCenter.y;
	}

	tempVector = tempVector / BasicGraphics::GetLength(tempVector);
	std::vector<float> c1Project(ProjectCircle(c1, tempVector, deltaVector)), r2Project(ProjectRectangle(r2, tempVector, deltaVector));
	if (!GetOverlappinglength(c1Project, r2Project, tempLenght))
		return false;
	if (tempLenght < length)
	{
		depth = tempVector * tempLenght;
		return true;
	}
	else
	{
		depth = normalVector * length;
		return true;
	}
}

bool Collision::PolygonPolygon(const BasicGraphics::ConvexPolygon& p1, const BasicGraphics::ConvexPolygon& p2, BasicGraphics::VectorF& depth)
{
	BasicGraphics::VectorF deltaVector(p1.address);
	std::vector<float> visitedDepthVector;

	std::vector<float> p1Project, p2Project;
	BasicGraphics::VectorF depthVector{ 0.0 }, tempDepthVector{ 0.0 };
	float depthLength(0.0), tempDepthLength(0.0);

	BasicGraphics::PointF startPoint(p1.vertex[0]), endPoint(p1.vertex[p1.vertex.size() - 1]);
	depthVector = GetNormalVector(startPoint, endPoint);
	p1Project = ProjectPolygon(p1, depthVector, deltaVector);
	p2Project = ProjectPolygon(p2, depthVector, deltaVector);
	if (!GetOverlappinglength(p1Project, p2Project, depthLength))
		return false;
	visitedDepthVector.push_back(depthVector.x);

	startPoint = p2.vertex[0];
	endPoint = p2.vertex[p2.vertex.size() - 1];
	tempDepthVector = GetNormalVector(startPoint, endPoint);
	if (tempDepthVector.x != depthVector.x)
	{
		p1Project = ProjectPolygon(p1, tempDepthVector, deltaVector);
		p2Project = ProjectPolygon(p2, tempDepthVector, deltaVector);
		if (!GetOverlappinglength(p1Project, p2Project, tempDepthLength))
			return false;
		visitedDepthVector.push_back(tempDepthVector.x);

		if (fabs(tempDepthLength) < fabs(depthLength))
		{
			depthVector = tempDepthVector;
			depthLength = tempDepthLength;
		}
	}

	for (int i = 0; i < p1.vertex.size() - 1; i++)
	{
		startPoint = p1.vertex[i];
		endPoint = p1.vertex[i + 1];
		tempDepthVector = GetNormalVector(startPoint, endPoint);
		if (std::any_of(visitedDepthVector.begin(), visitedDepthVector.end(), [tempDepthVector](const float i) {
			return i == tempDepthVector.x;
			}))
			continue;
		p1Project = ProjectPolygon(p1, tempDepthVector, deltaVector);
		p2Project = ProjectPolygon(p2, tempDepthVector, deltaVector);
		if (!GetOverlappinglength(p1Project, p2Project, tempDepthLength))
			return false;
		visitedDepthVector.push_back(tempDepthVector.x);
		if (fabs(tempDepthLength) < fabs(depthLength))
		{
			depthVector = tempDepthVector;
			depthLength = tempDepthLength;
		}
	}
	for (int i = 0; i < p2.vertex.size() - 1; i++)
	{
		startPoint = p2.vertex[i];
		endPoint = p2.vertex[i + 1];
		tempDepthVector = GetNormalVector(startPoint, endPoint);
		if (std::any_of(visitedDepthVector.begin(), visitedDepthVector.end(), [tempDepthVector](const float i) {
			return i == tempDepthVector.x;
			}))
			continue;
		p1Project = ProjectPolygon(p1, tempDepthVector, deltaVector);
		p2Project = ProjectPolygon(p2, tempDepthVector, deltaVector);
		if (!GetOverlappinglength(p1Project, p2Project, tempDepthLength))
			return false;
		visitedDepthVector.push_back(tempDepthVector.x);
		if (fabs(tempDepthLength) < fabs(depthLength))
		{
			depthVector = tempDepthVector;
			depthLength = tempDepthLength;
		}
	}
	depth = depthVector * depthLength;
	return true;
}

bool Collision::PolygonRect(const BasicGraphics::ConvexPolygon& p1, const BasicGraphics::Rectangle& r2, BasicGraphics::VectorF& depth)
{
	BasicGraphics::VectorF deltaVector(p1.address);
	std::vector<float> visitedDepthVector;

	std::vector<float> p1Project, r2Project, r2ProjectSp;
	BasicGraphics::VectorF depthVector{ 1.0,0.0 }, tempDepthVector{ 0.0,1.0 };
	float depthLength(0.0), tempDepthLength(0.0);

	ProjectRectangleSp(r2, r2Project, r2ProjectSp, deltaVector);
	p1Project = ProjectPolygon(p1, depthVector, deltaVector);
	if (!GetOverlappinglength(p1Project, r2Project, depthLength))
		return false;

	p1Project = ProjectPolygon(p1, tempDepthVector, deltaVector);
	if (!GetOverlappinglength(p1Project, r2ProjectSp, tempDepthLength))
		return false;

	if (fabs(tempDepthLength) < fabs(depthLength))
	{
		depthVector = tempDepthVector;
		depthLength = tempDepthLength;
	}
	visitedDepthVector.push_back(0.0);
	visitedDepthVector.push_back(1.0);

	BasicGraphics::PointF startPoint(p1.vertex[0]), endPoint(p1.vertex[p1.vertex.size() - 1]);
	for (int i = 0; i < p1.vertex.size(); i++)
	{
		if (i != 0)
		{
			startPoint = p1.vertex[i - 1];
			endPoint = p1.vertex[i];
		}
		tempDepthVector = GetNormalVector(startPoint, endPoint);
		if (std::any_of(visitedDepthVector.begin(), visitedDepthVector.end(), [tempDepthVector](const float i) {
			return i == tempDepthVector.x;
			}))
			continue;
		p1Project = ProjectPolygon(p1, tempDepthVector, deltaVector);
		r2Project = ProjectRectangle(r2, tempDepthVector, deltaVector);
		if (!GetOverlappinglength(p1Project, r2Project, tempDepthLength))
			return false;
		visitedDepthVector.push_back(tempDepthVector.x);
		if (fabs(tempDepthLength) < fabs(depthLength))
		{
			depthVector = tempDepthVector;
			depthLength = tempDepthLength;
		}
	}

	depth = depthVector * depthLength;
	return true;
}

bool Collision::RectRect(const BasicGraphics::Rectangle& r1, const BasicGraphics::Rectangle& r2, BasicGraphics::VectorF& depth)
{
	BasicGraphics::VectorF deltaVector(r1.address);
	std::vector<float> r1X(2), r1Y(2), r2X(2), r2Y(2);
	ProjectRectangleSp(r1, r1X, r1Y, deltaVector);
	ProjectRectangleSp(r2, r2X, r2Y, deltaVector);

	float lengthX(0.0), lengthY(0.0);
	if (!GetOverlappinglength(r1X, r2X, lengthX))
		return false;
	if (!GetOverlappinglength(r1Y, r2Y, lengthY))
		return false;

	if (lengthX < lengthY)
	{
		depth.x = lengthX;
		depth.y = 0.0;
		return true;
	}
	else
	{
		depth.x = 0.0;
		depth.y = lengthY;
		return true;
	}
}

void Collision::CollisionResponse(BasicGraphics::PointF& g1Addr, BasicGraphics::Physics& g1Phy, BasicGraphics::PointF& g2Addr, BasicGraphics::Physics& g2Phy, const BasicGraphics::VectorF depth)
{
	if (g1Phy.stationary && g2Phy.stationary)
	{
		return;
	}
	else if (g1Phy.stationary)
	{
		g2Addr = g2Addr + depth;
	}
	else if (g2Phy.stationary)
	{
		g1Addr = g1Addr - depth;
	}
	else
	{
		g1Addr = g1Addr - depth / 2.0;
		g2Addr = g2Addr + depth / 2.0;
	}

	BasicGraphics::VectorF relativeVelocity(g2Phy.v - g1Phy.v);
	if (relativeVelocity * depth > 0)
		return;

	float e(0.5);
	BasicGraphics::VectorF normal(depth / BasicGraphics::GetLength(depth));
	float j(-(1.0 - e) * (relativeVelocity * normal));
	j = j * (g1Phy.m + g2Phy.m);
	BasicGraphics::VectorF impulse(normal * j);
	g1Phy.v = g1Phy.v - impulse / g1Phy.m;
	g2Phy.v = g2Phy.v - impulse / g2Phy.m;
}
