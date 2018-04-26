#pragma once
#include <vector>
#include <functional>
#include "VRManagerAPI.h"

typedef std::function<void(float)> OnPoseUpdateCallback;
static const UInt32 kPapyrusVR_Message_Init = 40008;
struct PapyrusVRAPI
{
	//Functions
	std::function<void(OnPoseUpdateCallback)> RegisterPoseUpdateListener;
	std::function<VRManagerAPI*(void)> GetVRManager;
};