#pragma once
#include <memory>
#include "IShape.h"
#include "../OpenVRTypes.h"
#include "../PapyrusVRTypes.h"

namespace PapyrusVR
{

	class LocalOverlapObject
	{
	public:
		LocalOverlapObject(IShape* aShape, Matrix34* aTransform, TrackedDevicePose** aAttachedDevicePose = NULL) :
			shape(aShape),
			transform(aTransform),
			attachedDevicePose(aAttachedDevicePose) { };
	private:
		std::unique_ptr<IShape> shape;
		std::unique_ptr<Matrix34> transform;

		TrackedDevicePose** attachedDevicePose;
		bool _prevStates[k_unMaxTrackedDeviceCount];
		VROverlapEvent ComputeOverlapEvent(bool previousState, bool currentState);
	public:
		//Checks if an overlap occured with the given pose, selfCollisions enables checks on the attached pose as well 
		VROverlapEvent CheckOverlapWithPose(VRDevice device, TrackedDevicePose* otherPose, bool selfCollisions = false);

		//Attaches to the given device, NULL to detach
		void AttachToPose(TrackedDevicePose** devicePose) { attachedDevicePose = devicePose; }
	};
}