#pragma once
#include "openvr.h"
#include "skse64/ScaleformAPI.h"
#include "skse64/ScaleformValue.h"

namespace ScaleformVR
{
	void DispatchControllerState(vr::ETrackedControllerRole controllerHand, vr::VRControllerState_t controllerState);
	bool RegisterFuncs(GFxMovieView * view, GFxValue * root);
}