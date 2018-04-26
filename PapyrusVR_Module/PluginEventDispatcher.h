#pragma once
#include <ctime>
#include <algorithm>
#include <functional>
#include <vector>

namespace PapyrusVR
{
	typedef std::function<void(float)> OnPoseUpdateCallback;
	typedef std::vector<OnPoseUpdateCallback> PoseUpdateListeners;

	class PluginEventDispatcher
	{
	public:
		static PluginEventDispatcher& GetInstance()
		{
			static PluginEventDispatcher instance;

			return instance;
		}
	private:
		PluginEventDispatcher() { }
		PoseUpdateListeners g_poseUpdateListeners;

	public:
		PluginEventDispatcher(PluginEventDispatcher const&) = delete;
		void operator=(PluginEventDispatcher const&) = delete;
		PoseUpdateListeners* GetListensers();
		void RegisterListener(OnPoseUpdateCallback callback);
	};
}