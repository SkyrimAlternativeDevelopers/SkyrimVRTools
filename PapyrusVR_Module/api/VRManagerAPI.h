#pragma once
#include "openvr.h"

class VRManagerAPI
{
public:
	virtual void UpdatePoses() = 0;
	virtual vr::TrackedDevicePose_t* GetHMDPose(bool renderPose = true) = 0;
	virtual vr::TrackedDevicePose_t* GetRightHandPose(bool renderPose = true) = 0;
	virtual vr::TrackedDevicePose_t* GetLeftHandPose(bool renderPose = true) = 0;
};