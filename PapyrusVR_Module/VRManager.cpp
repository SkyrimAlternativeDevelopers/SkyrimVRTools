#include "VRManager.h"

namespace PapyrusVR
{
	void VRManager::InitVRCompositor(vr::IVRCompositor* compositor)
	{
		_MESSAGE("[VRManager] Setting VRManager compositor to address %p", compositor);
		if (compositor != nullptr)
			_compositor = compositor;
	}

	void VRManager::InitVRSystem(vr::IVRSystem* vrSystem)
	{
		_MESSAGE("[VRManager] Setting VRManager system to address %p", vrSystem);
		if (vrSystem != nullptr)
			_vr = vrSystem;
	}
          
	bool VRManager::IsInitialized()
	{
		return _compositor != nullptr && _vr != nullptr;
	}

	clock_t lastFrame = clock();
	clock_t thisFrame;
	double deltaTime = 0.0f;
	void VRManager::UpdatePoses()
	{
		if (IsInitialized())
		{
			//Calculate deltaTime
			thisFrame = clock();
			deltaTime = double(thisFrame - lastFrame) / CLOCKS_PER_SEC;
			lastFrame = thisFrame;

			vr::VRCompositorError error = _compositor->GetLastPoses((vr::TrackedDevicePose_t*)_renderPoses, k_unMaxTrackedDeviceCount, (vr::TrackedDevicePose_t*)_gamePoses, k_unMaxTrackedDeviceCount);
			if (error && error != vr::EVRCompositorError::VRCompositorError_None)
				_MESSAGE("[VRManager] Error while retriving game poses!");

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

			//Dispatch update event
			DispatchVRUpdateEvent(deltaTime);
		}
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
		if (IsInitialized())
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

        std::lock_guard<std::mutex> lock( _vrLocalOverlapObjectMapMutex );

		for (auto const& element : _localOverlapObjects)
		{
			
			evt = element.second->CheckOverlapWithPose(currentDevice, GetPoseByDeviceEnum(currentDevice));

			//Notify listeners of event
			if (evt != VROverlapEvent::VROverlapEvent_None)
				DispatchVROverlapEvent(evt, element.first, currentDevice);
		}
	}

	//Notifies all listeners that an event has occured
	void VRManager::DispatchVRUpdateEvent(float deltaTime)
	{
		std::lock_guard<std::mutex> lock(_vrUpdateListenersMutex);

		for (OnVRUpdateEvent& listener : _vrUpdateListeners)
		{
			if (listener)
				(*listener)(deltaTime);
		}
	}

	//Notifies all listeners that an event has occured
	void VRManager::DispatchVRButtonEvent(VREventType eventType, EVRButtonId button, VRDevice device)
	{
        std::lock_guard<std::mutex> lock( _vrButtonEventsListenersMutex );

		for (OnVRButtonEvent& listener : _vrButtonEventsListeners)
		{
			if (listener)
				(*listener)(eventType, button, device);
		}
	}

	//Notifies all listeners that an overlap has occured
	void VRManager::DispatchVROverlapEvent(VROverlapEvent eventType, UInt32 objectHandle, VRDevice device)
	{
		//TODO: Filter events?
		_MESSAGE("[VRManager] Dispatching overlap event %d from device %d in handle %d", eventType, device, objectHandle);

        std::lock_guard<std::mutex> lock( _vrOverlapEventsListenersMutex );

		for (OnVROverlapEvent& listener : _vrOverlapEventsListeners)
			(*listener)(eventType, objectHandle, device);
	}

	UInt32 VRManager::CreateLocalOverlapSphere(float radius, Matrix34* transform, VRDevice attachedDevice)
	{
		_MESSAGE("[VRManager] CreateLocalOverlapSphere");

		if (!transform)
			return 0; //handle = 0, ERROR

		Sphere* overlapSphere = new Sphere(radius);
		_MESSAGE("[VRManager] Radius %f", radius);
		_MESSAGE("[VRManager] Transform size %d", sizeof(*transform));
		_MESSAGE("[VRManager] Attached to deviceID %d", attachedDevice);

		TrackedDevicePose** attachedTo = NULL;
		if (attachedDevice != VRDevice_Unknown)
			attachedTo = _orderedTrackedDevicesMap[false]+attachedDevice;

		LocalOverlapObject* overlapObject = new LocalOverlapObject(overlapSphere, transform, attachedTo);

        std::lock_guard<std::mutex> lock( _vrLocalOverlapObjectMapMutex );

		UInt32 handle = _nextLocalOverlapObjectHandle++;
		_localOverlapObjects[handle] = overlapObject;

		return handle;
	}

	void VRManager::DestroyLocalOverlapObject(UInt32 overlapObjectHandle)
	{
		std::lock_guard<std::mutex> lock( _vrLocalOverlapObjectMapMutex );

		if (_localOverlapObjects.count(overlapObjectHandle))
		{
			delete _localOverlapObjects[overlapObjectHandle];
			_localOverlapObjects.erase(overlapObjectHandle);
		}
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