#pragma once
#include "openvr.h"

using namespace vr;
namespace PapyrusVR
{
	//Singleton to manage OpenVR calls (cache poses etc.)
	class VRManager
	{
	public:
		static VRManager& GetInstance()
		{
			static VRManager instance;

			return instance;
		}
	private:
		VRManager() { Init(); }

		IVRCompositor* _compositor;
		IVRSystem* _vr;

		TrackedDevicePose_t _renderPoses[vr::k_unMaxTrackedDeviceCount]; //Used to store available poses
		TrackedDevicePose_t _gamePoses[vr::k_unMaxTrackedDeviceCount]; //Used to store available poses

		int _hmdID = -1;
		int _rightHandID = -1;
		int _leftHandID = -1;

	public:
		VRManager(VRManager const&) = delete;
		void operator=(VRManager const&) = delete;
		bool Init();
		void UpdatePoses();
		TrackedDevicePose_t* GetHMDPose(bool renderPose = true);
		TrackedDevicePose_t* GetRightHandPose(bool renderPose = true);
		TrackedDevicePose_t* GetLeftHandPose(bool renderPose = true);
	};
}