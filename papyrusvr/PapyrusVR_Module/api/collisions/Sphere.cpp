#include "Sphere.h"
#include <cmath>

namespace PapyrusVR
{
	bool Sphere::CheckForOverlap(Vector3 distanceFromOrigin)
	{
		//Use square distances (more performance friendly)

        return distanceFromOrigin.lengthSquared() < pow( radius, 2 );
	}
}