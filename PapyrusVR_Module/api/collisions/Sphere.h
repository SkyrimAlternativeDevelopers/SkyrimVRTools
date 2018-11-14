#pragma once
#include "IShape.h"
#include "../PapyrusVRTypes.h"

namespace PapyrusVR
{
	class Sphere : public IShape
	{
	public:
		Sphere(float a_radius) : radius(a_radius) {};
	private:
		float radius = 5.0f;
	public:
		//Checks if point(expressed in distance from center) is inside the object
		bool CheckForOverlap(Vector3 distanceFromOrigin);
	};
}