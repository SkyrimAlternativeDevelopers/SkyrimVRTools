#pragma once
#include "../PapyrusVRTypes.h"

namespace PapyrusVR
{
	class IShape
	{
	public:
		virtual bool CheckForOverlap(Vector3 point) = 0;
	};
}