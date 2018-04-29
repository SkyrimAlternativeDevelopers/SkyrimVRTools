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

	int _leftIndex;
	int _rightIndex;
	void VRManager::UpdatePoses()
	{
		if (_compositor)
		{
			vr::VRCompositorError error = _compositor->GetLastPoses((vr::TrackedDevicePose_t*)_renderPoses, vr::k_unMaxTrackedDeviceCount, (vr::TrackedDevicePose_t*)_gamePoses, vr::k_unMaxTrackedDeviceCount);
			if (error && error != vr::EVRCompositorError::VRCompositorError_None)
				_MESSAGE("Error while retriving game poses!");

			//HMD
			UpdateTrackedDevicesMapEntry(VRDevice::VRDevice_HMD, k_unTrackedDeviceIndex_Hmd);

			//Controllers
			UpdateTrackedDevicesMapEntry(VRDevice::VRDevice_LeftController, _vr->GetTrackedDeviceIndexForControllerRole(vr::ETrackedControllerRole::TrackedControllerRole_LeftHand));
			UpdateTrackedDevicesMapEntry(VRDevice::VRDevice_RightController, _vr->GetTrackedDeviceIndexForControllerRole(vr::ETrackedControllerRole::TrackedControllerRole_RightHand));

			/*
			//TODO: Trackers
			for (int i = 0; i < vr::k_unMaxTrackedDeviceCount; i++)
			{
				switch (_vr->GetTrackedDeviceClass(i))
				{
					case vr::ETrackedDeviceClass::TrackedDeviceClass_GenericTracker:
						...
						break;
				}
			}
			*/

			//Process Events
			ProcessControllerEvents(VRDevice::VRDevice_LeftController);
			ProcessControllerEvents(VRDevice::VRDevice_RightController);

			//Process Overlaps for every valid tracked object
			for (int i = 0; i < VRDevice::VRDevice_LeftController + 1; i++)
				ProcessOverlapEvents((VRDevice)i);
		}
		else
			Init();
	}

	void VRManager::UpdateTrackedDevicesMapEntry(VRDevice device, uint32_t newIndex)
	{
		_orderedTrackedDevicesMap[false][device] = _gamePoses + newIndex;
		_orderedTrackedDevicesMap[true][device] = _renderPoses + newIndex;
	}
	
	VREvent VRManager::CheckStatesForMask(UInt64 previousEvt, UInt64 newEvt, UInt64 mask)
	{
		
		if ((newEvt & mask) && !(previousEvt & mask))
			return VREvent::VREvent_Positive;
		else if(!(newEvt & mask) && (previousEvt & mask))
			return VREvent::VREvent_Negative;

		return VREvent::VREvent_None;
	}

	//Used to iterate through every possible unique button
	vr::EVRButtonId _possibleButtonsIds[15] = 
	{
		vr::EVRButtonId::k_EButton_System,
		vr::EVRButtonId::k_EButton_ApplicationMenu,
		vr::EVRButtonId::k_EButton_Grip,
		vr::EVRButtonId::k_EButton_DPad_Left,
		vr::EVRButtonId::k_EButton_DPad_Up,
		vr::EVRButtonId::k_EButton_DPad_Right,
		vr::EVRButtonId::k_EButton_DPad_Down,
		vr::EVRButtonId::k_EButton_A,
		vr::EVRButtonId::k_EButton_ProximitySensor,
		vr::EVRButtonId::k_EButton_Axis0,
		vr::EVRButtonId::k_EButton_Axis1,
		vr::EVRButtonId::k_EButton_Axis2,
		vr::EVRButtonId::k_EButton_Axis3,
		vr::EVRButtonId::k_EButton_Axis4,
		vr::EVRButtonId::k_EButton_Max
	};

	void VRManager::ProcessControllerEvents(VRDevice currentDevice)
	{
		if (_vr)
		{
			//If the device is mapped
			if (_orderedTrackedDevicesMap[false][currentDevice])
			{
				//Ugly but handy, gets the device index from our ordered map
				uint64_t controllerID = (_orderedTrackedDevicesMap[false][currentDevice]) - _gamePoses;

				vr::VRControllerState_t newState;
				VREvent eventResult;

				//Check controller index
				if (_vr->GetControllerState(controllerID, &newState, sizeof(vr::VRControllerState_t)))
				{
					if (newState.unPacketNum != _controllerStates[controllerID].unPacketNum)
					{
						//We need to implement events here, using PollNextEvent could lead to skyrim losing events

						//Button Pressed/Release
						if (_controllerStates[controllerID].ulButtonPressed != newState.ulButtonPressed)
						{
							for (vr::EVRButtonId& button : _possibleButtonsIds)
							{
								eventResult = CheckStatesForMask(_controllerStates[controllerID].ulButtonPressed, newState.ulButtonPressed, vr::ButtonMaskFromId(button));
								if (eventResult == VREvent::VREvent_Positive)
									DispatchVRButtonEvent(VREventType::VREventType_Pressed, (EVRButtonId)button, currentDevice);

								if (eventResult == VREvent::VREvent_Negative)
									DispatchVRButtonEvent(VREventType::VREventType_Released, (EVRButtonId)button, currentDevice);
							}
						}

						//Button Touched/Untouched
						if (_controllerStates[controllerID].ulButtonTouched != newState.ulButtonTouched)
						{
							for (vr::EVRButtonId& button : _possibleButtonsIds)
							{
								eventResult = CheckStatesForMask(_controllerStates[controllerID].ulButtonTouched, newState.ulButtonTouched, vr::ButtonMaskFromId(button));
								if (eventResult == VREvent::VREvent_Positive)
									DispatchVRButtonEvent(VREventType::VREventType_Touched, (EVRButtonId)button, currentDevice);

								if (eventResult == VREvent::VREvent_Negative)
									DispatchVRButtonEvent(VREventType::VREventType_Untouched, (EVRButtonId)button, currentDevice);
							}
						}

						_controllerStates[controllerID] = newState;
					}
				}
			}
		}
	}

	void VRManager::ProcessOverlapEvents(VRDevice currentDevice)
	{
		// O(_localOverlapObjects.len)
		// Iterate through every object and calculate local overlap events
		VROverlapEvent evt;
		_vrLocalOverlapObjectMapMutex.lock();
		for (UInt32 i = 1; i < _localOverlapObjectCount; i++)
		{
			if (_localOverlapObjects[i])
			{
				evt = _localOverlapObjects[i]->CheckOverlapWithPose(currentDevice, GetPoseByDeviceEnum(currentDevice));

				//Notify listeners of event
				if (evt != VROverlapEvent::VROverlapEvent_None)
					DispatchVROverlapEvent(evt, i, currentDevice);
			}
		}
		_vrLocalOverlapObjectMapMutex.unlock();
	}

	//Notifies all listeners that an event has occured
	void VRManager::DispatchVRButtonEvent(VREventType eventType, EVRButtonId button, VRDevice device)
	{
		_vrButtonEventsListenersMutex.lock();
		for (OnVRButtonEvent& listener : _vrButtonEventsListeners)
		{
			if (listener)
				(*listener)(eventType, button, device);
		}
		_vrButtonEventsListenersMutex.unlock();
	}

	//Notifies all listeners that an overlap has occured
	void VRManager::DispatchVROverlapEvent(VROverlapEvent eventType, UInt32 objectHandle, VRDevice device)
	{
		//TODO: Filter events?
		_MESSAGE("Dispatching overlap event %d from device %d in handle %d", eventType, device, objectHandle);
		_vrOverlapEventsListenersMutex.lock();
		for (OnVROverlapEvent& listener : _vrOverlapEventsListeners)
			(*listener)(eventType, objectHandle, device);
		_vrOverlapEventsListenersMutex.unlock();
	}

	UInt32 VRManager::CreateLocalOverlapSphere(float radius, Matrix34* transform, VRDevice attachedDevice)
	{
		_MESSAGE("CreateLocalOverlapSphere");

		if (!transform)
			return 0;
		Sphere* overlapSphere = new Sphere(radius);
		_MESSAGE("Radius %f", radius);
		_MESSAGE("Transform size %d", sizeof(*transform));
		_MESSAGE("Attached to deviceID %d", attachedDevice);
		_MESSAGE("CreateLocalOverlapSphere");

		TrackedDevicePose** attachedTo = NULL;
		if (attachedDevice != VRDevice_Unknown)
			attachedTo = _orderedTrackedDevicesMap[false]+attachedDevice;

		LocalOverlapObject* overlapObject = new LocalOverlapObject(overlapSphere, transform, attachedTo);

		//Finds unique handle (0,UINT32_MAX]

		_vrLocalOverlapObjectMapMutex.lock();
		UInt32 handle = _localOverlapObjectCount++;
		_localOverlapObjects[handle] = overlapObject; 
		_vrLocalOverlapObjectMapMutex.unlock();

		return 0;
	}

	void VRManager::DestroyLocalOverlapObject(UInt32 overlapObjectHandle)
	{
		_vrLocalOverlapObjectMapMutex.lock();
		if(_localOverlapObjects[overlapObjectHandle])
			_localOverlapObjects.erase(overlapObjectHandle);
		_vrLocalOverlapObjectMapMutex.unlock();
	}

	TrackedDevicePose* VRManager::GetHMDPose(bool renderPose)
	{ 
		return _orderedTrackedDevicesMap[renderPose][VRDevice::VRDevice_HMD];
	}
	TrackedDevicePose* VRManager::GetRightHandPose(bool renderPose)
	{
		return _orderedTrackedDevicesMap[renderPose][VRDevice::VRDevice_RightController];
	}

	TrackedDevicePose* VRManager::GetLeftHandPose(bool renderPose)
	{
		return _orderedTrackedDevicesMap[renderPose][VRDevice::VRDevice_LeftController];
	}

	TrackedDevicePose* VRManager::GetPoseByDeviceEnum(VRDevice device, bool renderPose)
	{
		return _orderedTrackedDevicesMap[renderPose][device];
	}
}