#pragma once
#include <list>
#include <mutex>
#include "openvr.h"
#include "api/VRManagerAPI.h"
#include "api/PapyrusVRTypes.h"

namespace PapyrusVR
{
	//Singleton to manage OpenVR calls (cache poses etc.)
	class VRManager : public VRManagerAPI
	{
	public:
		static VRManager& GetInstance()
		{
			static VRManager instance;

			return instance;
		}
	private:
		VRManager() { }

		vr::IVRCompositor* _compositor;
		vr::IVRSystem* _vr;

		TrackedDevicePose _renderPoses[vr::k_unMaxTrackedDeviceCount]; //Used to store available poses
		TrackedDevicePose _gamePoses[vr::k_unMaxTrackedDeviceCount]; //Used to store available poses
		vr::VRControllerState_t _controllerStates[vr::k_unMaxTrackedDeviceCount]; //Used to store states

		std::mutex _vrButtonEventsListenersMutex;
		std::list<OnVRButtonEvent> _vrButtonEventsListeners;

		int _hmdID = -1;
		int _rightHandID = -1;
		int _leftHandID = -1;

		VREvent CheckStatesForMask(UInt64 previousEvt, UInt64 newEvt, UInt64 mask);
		void DispatchVRButtonEvent(VREventType eventType, EVRButtonId button, VRDevice device);
	public:
		VRManager(VRManager const&) = delete;
		void operator=(VRManager const&) = delete;
		bool Init();
		void UpdatePoses();

		void ProcessControllerEvents(int controllerID);

		void RegisterVRButtonListener(OnVRButtonEvent listener);
		void UnregisterVRButtonListener(OnVRButtonEvent listener);

		TrackedDevicePose* GetHMDPose(bool renderPose = true);
		TrackedDevicePose* GetRightHandPose(bool renderPose = true);
		TrackedDevicePose* GetLeftHandPose(bool renderPose = true);
		TrackedDevicePose* GetPoseByDeviceEnum(VRDevice device);
	};
}
