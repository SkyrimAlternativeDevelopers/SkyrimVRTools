#include "VRManager.h"

namespace PapyrusVR
{
	bool VRManager::Init()
	{
		if ((_compositor = vr::VRCompositor()))
			_MESSAGE("Compositor is reachable, proceding with setup...");
		else
		{
			_MESSAGE("Failed to get compositor");
			return false;
		}

		if ((_vr = vr::VRSystem()))
			_MESSAGE("VR System is reachable, proceding with setup...");
		else
		{
			_MESSAGE("Failed to get VR System");
			return false;
		}
		
		return true;
	}

	void VRManager::UpdatePoses()
	{
		if (_compositor)
		{
			vr::VRCompositorError error = _compositor->GetLastPoses(_renderPoses, vr::k_unMaxTrackedDeviceCount, _gamePoses, vr::k_unMaxTrackedDeviceCount);
			if (error && error != vr::EVRCompositorError::VRCompositorError_None)
				_MESSAGE("Error while retriving game poses!");

			//Updates poses
			for (int i = 0; i < vr::k_unMaxTrackedDeviceCount; i++)
			{
				switch (_vr->GetTrackedDeviceClass(i))
				{
					case vr::ETrackedDeviceClass::TrackedDeviceClass_Controller:
						switch (_vr->GetControllerRoleForTrackedDeviceIndex(i))
						{
							case ETrackedControllerRole::TrackedControllerRole_LeftHand:
								_leftHandID = i;
							break;
							case ETrackedControllerRole::TrackedControllerRole_RightHand:
								_rightHandID = i;
							break;
						}
					break;
					case vr::ETrackedDeviceClass::TrackedDeviceClass_HMD:
						_hmdID = i;
					break;
				}
			}
		}
	}

	TrackedDevicePose_t* VRManager::GetHMDPose(bool renderPose) 
	{ 
		if (_hmdID < 0)
			return NULL;
		return renderPose ? &_renderPoses[_hmdID] : &_gamePoses[_hmdID];
	}
	TrackedDevicePose_t* VRManager::GetRightHandPose(bool renderPose)
	{
		if (_rightHandID < 0)
			return NULL;
		return renderPose ? &_renderPoses[_rightHandID] : &_gamePoses[_rightHandID];
	}

	TrackedDevicePose_t* VRManager::GetLeftHandPose(bool renderPose)
	{
		if (_leftHandID < 0)
			return NULL; 
		return renderPose ? &_renderPoses[_leftHandID] : &_gamePoses[_leftHandID]; 
	}
}