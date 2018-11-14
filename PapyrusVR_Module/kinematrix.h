#pragma once
#include "api/PapyrusVRTypes.h"
#include "api/OpenVRTypes.h"

namespace Kinematrix
{
	void GameLoaded();
	void OnVRButtonEvent(PapyrusVR::VREventType eventType, PapyrusVR::EVRButtonId buttonID, PapyrusVR::VRDevice device);
	void OnPoseUpdate(float deltaTime);
}
