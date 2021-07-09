#include "Bounds.h"
#include <algorithm>

BoundingRegion::BoundingRegion(BoundType type)
	: type(type)
{
}

BoundingRegion::BoundingRegion(glm::vec3 center, float radius)
	: type(BoundType::SPHERE), center(center), radius(radius)
{
}

BoundingRegion::BoundingRegion(glm::vec3 min, glm::vec3 max)
	: type(BoundType::AABB), min(min), max(max)
{
}

glm::vec3 BoundingRegion::calculateCenter()
{
	return (type == BoundType::AABB) ? (min + max) / 2.0f : center;
}

glm::vec3 BoundingRegion::calculateDimensions()
{
	return (type == BoundType::AABB) ? (max - min) : glm::vec3(2.0f * radius);
}

bool BoundingRegion::containsPoint(glm::vec3 pt)
{
	if (type == BoundType::AABB)
		// box - point must be larger than min and smaller than max
		return (pt.x >= min.x) && (pt.x <= max.x) &&
			(pt.y >= min.y) && (pt.y <= max.y) &&
			(pt.z>= min.z) && (pt.z <= max.z);
	else
	{
		// sphere - distance must be less than radius
		float distSquared = 0.0f;
		for (int i = 0; i < 3; i++)
			distSquared = (center[i] - pt[i]) * (center[i] - pt[i]);

		return distSquared <= (radius * radius);
	}
}

bool BoundingRegion::containsRegion(BoundingRegion br)
{
	if (br.type == BoundType::AABB)
		// if br is a box, just has to contains min and max
		return containsPoint(br.min) && containsPoint(br.max);
	else if (type == BoundType::SPHERE && br.type == BoundType::SPHERE)
		// if both sphere, combined distance from the centers and br.radius is less than radius
		return glm::length(center - br.center) + br.radius < radius;
	else
	{
		// if this is a box and br is a sphere
		if (!containsPoint(br.center))
			// center is outside of the box
			return false;

		// center is inside the box

		/*
			for each axis (x, y, z)
			- If distance to each side is smaller than the radius, return false
		*/
		for (int i = 0; i < 3; i++)
			if (abs(max[i] - br.center[i]) < br.radius || abs(br.center[i] - min[i]) < br.radius)
				return false;

		return true;
	}
}

bool BoundingRegion::intersectsWith(BoundingRegion br)
{
	// overlay on all axis

	if (type == BoundType::AABB && br.type == BoundType::AABB)
	{
		// both boxes
		glm::vec3 rad = calculateDimensions() / 2.0f;		// "radius" of this box
		glm::vec3 radBr = br.calculateDimensions() / 2.0f;	// "radius" of br

		glm::vec3 center = calculateCenter();				// "center" of this box
		glm::vec3 centerBr = br.calculateCenter();			// "center" of br

		glm::vec3 dist = abs(center - centerBr);

		for (int i = 0; i < 3; i++)
		{
			if (dist[i] > rad[i] + radBr[i])
				// no overlap on this axis
				return false;
		}

		return true;
	}
	else if (type == BoundType::SPHERE && br.type == BoundType::SPHERE)
		// both sphere, distances centers must be less than combined radius
		return glm::length(center - br.center) < (radius + br.radius);
	else if (type == BoundType::SPHERE)
	{
		// this is a sphere, br is a box

		// determine if sphere is above to, below bottom etc...
		// find distance(squared) to the closest plane
		float distSquared = 0.0f;
		for (int i = 0; i < 3; i++)
		{
			// determine closest side
			float closestPt = std::max(br.min[i], std::min(center[i], br.max[i]));
			// add distance
			distSquared += (closestPt - center[i]) * (closestPt - center[i]);
		}

		return distSquared < (radius* radius);
	}
	else
		// this is a box, br is a sphere
		// call algorithm for br (defined in preceding else if block)
		return br.intersectsWith(*this);
}

