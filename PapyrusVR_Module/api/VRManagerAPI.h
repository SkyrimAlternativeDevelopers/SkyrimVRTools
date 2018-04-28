#pragma once
#include "openvr.h"

//TODO: Trackers
enum VRDevice
{
	HMD = 0,
	RightController = 1,
	LeftController = 2
};

class VRManagerAPI
{
public:
	virtual void UpdatePoses() = 0;
	virtual vr::TrackedDevicePose_t* GetHMDPose(bool renderPose = true) = 0;
	virtual vr::TrackedDevicePose_t* GetRightHandPose(bool renderPose = true) = 0;
	virtual vr::TrackedDevicePose_t* GetLeftHandPose(bool renderPose = true) = 0;
	virtual vr::TrackedDevicePose_t* GetPoseByDeviceEnum(VRDevice device) = 0;
};