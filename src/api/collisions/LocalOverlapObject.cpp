#include "LocalOverlapObject.h"
#include "../utils/OpenVRUtils.h"
#include "../PapyrusVRTypes.h"

namespace PapyrusVR
{
	VROverlapEvent LocalOverlapObject::ComputeOverlapEvent(bool previousState, bool currentState)
	{
		if(previousState && !currentState)
			return VROverlapEvent::VROverlapEvent_OnExit;

		if(currentState && !previousState)
			return VROverlapEvent::VROverlapEvent_OnEnter;

		return VROverlapEvent::VROverlapEvent_None;
	}

	VROverlapEvent LocalOverlapObject::CheckOverlapWithPose(VRDevice device, TrackedDevicePose* otherPose, bool selfCollisions)
	{
		//Currently working only on spheres (need to account for quaternion rotation too)
		if (otherPose && transform && (selfCollisions || !attachedDevicePose || otherPose != (*attachedDevicePose)))
		{
			// Local Rotate -> Local Translate -> World Rotate -> World Translate
			Vector3 localTransformedPosition = (*transform) * Vector3();
			Vector3 attachedTransformedPosition;
				
			if (attachedDevicePose)
				attachedTransformedPosition = (*attachedDevicePose)->mDeviceToAbsoluteTracking * localTransformedPosition;
			else
				attachedTransformedPosition = localTransformedPosition;

			Vector3 devicePosition = OpenVRUtils::GetPosition(&(otherPose->mDeviceToAbsoluteTracking));
			Vector3 distanceFromOrigin = devicePosition - attachedTransformedPosition;

			//Compute event type and save state
			bool overlapping = shape->CheckForOverlap(distanceFromOrigin);
			VROverlapEvent returnEvent = ComputeOverlapEvent(_prevStates[device], overlapping);
			_prevStates[device] = overlapping;
			return returnEvent;
		}

		return VROverlapEvent::VROverlapEvent_None;
	}
}