#pragma once
#include "openvr.h"

//TODO: Trackers
enum VRDevice
{
	HMD = 0,
	RightController = 1,
	LeftController = 2
};

enum VREvent
{
	Negative = -1,
	None = 0,
	Positive = 1
};

enum VREventType
{
	Touched = 0,
	Untouched = 1,
	Pressed = 2,
	Released = 3
};

typedef void(*OnVRButtonEvent)(VREventType, vr::EVRButtonId);

class VRManagerAPI
{
public:
	virtual void UpdatePoses() = 0;

	virtual void RegisterVRButtonListener(OnVRButtonEvent listener) = 0;
	virtual void UnregisterVRButtonListener(OnVRButtonEvent listener) = 0;

	virtual vr::TrackedDevicePose_t* GetHMDPose(bool renderPose = true) = 0;
	virtual vr::TrackedDevicePose_t* GetRightHandPose(bool renderPose = true) = 0;
	virtual vr::TrackedDevicePose_t* GetLeftHandPose(bool renderPose = true) = 0;
	virtual vr::TrackedDevicePose_t* GetPoseByDeviceEnum(VRDevice device) = 0;
};