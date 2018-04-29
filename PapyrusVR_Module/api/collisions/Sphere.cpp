#include "Sphere.h"

namespace PapyrusVR
{
	bool Sphere::CheckForOverlap(Vector3 distanceFromOrigin)
	{
		//Use square distances (more performance friendly)
		return pow(distanceFromOrigin.x, 2) + pow(distanceFromOrigin.y, 2) + pow(distanceFromOrigin.z, 2) < (radius*radius);
	}
}