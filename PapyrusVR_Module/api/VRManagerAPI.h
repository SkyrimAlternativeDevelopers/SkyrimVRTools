#pragma once
#include "PapyrusVRTypes.h"
#include "OpenVRTypes.h"

namespace PapyrusVR
{
	typedef void(*OnVRButtonEvent)(VREventType, EVRButtonId, VRDevice);

	class VRManagerAPI
	{
	public:
		virtual void UpdatePoses() = 0;

		virtual void RegisterVRButtonListener(OnVRButtonEvent listener) = 0;
		virtual void UnregisterVRButtonListener(OnVRButtonEvent listener) = 0;

		virtual TrackedDevicePose* GetHMDPose(bool renderPose = true) = 0;
		virtual TrackedDevicePose* GetRightHandPose(bool renderPose = true) = 0;
		virtual TrackedDevicePose* GetLeftHandPose(bool renderPose = true) = 0;
		virtual TrackedDevicePose* GetPoseByDeviceEnum(VRDevice device) = 0;
	};
}