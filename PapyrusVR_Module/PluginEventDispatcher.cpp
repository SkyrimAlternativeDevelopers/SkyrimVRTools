#include "PluginEventDispatcher.h"
namespace PapyrusVR
{

	PoseUpdateListeners* PluginEventDispatcher::GetListensers()
	{
		return &g_poseUpdateListeners;
	}

	void PluginEventDispatcher::RegisterListener(OnPoseUpdateCallback callback)
	{
		g_poseUpdateListeners.push_back(callback);
	}
}