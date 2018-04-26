#pragma once
#include <vector>
#include <functional>
#include "../PapyrusVR_Module/VRManager.h"

namespace PapyrusVR
{
	typedef std::function<void(float)> OnPoseUpdateCallback;

	extern "C" __declspec(dllimport) void RegisterPoseUpdateListener(OnPoseUpdateCallback callback);
	extern "C" __declspec(dllimport) VRManager* g_vrmanager;
}