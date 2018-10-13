#include "VRManager.h"
#include "DirUtils.h"

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

	void VRManager::RegisterInputActions()
	{
		if (IsInitialized())
		{
			_MESSAGE("[VRManager] Registering custom input actions to SteamVR Input");

			//TODO: SteamVR Input
			//DEBUG CODE
			/*const char* bindingFileDir = GetCurrentWorkingDir().append("\\Data\\SKSE\\Plugins\\PapyrusVR\\bindings.json").c_str();
			_MESSAGE("[VRManager] Loading binding file from %s", bindingFileDir);
			vr::EVRInputError error = vr::VRInput()->SetActionManifestPath(bindingFileDir);

			if (error != vr::EVRInputError::VRInputError_None)
				_MESSAGE("Error in registering action manifest to SteamVR Input (%d)", error);*/

			//DEBUG CODE, Don't enable you'll lose every controller binding
			/*vr::VRActionHandle_t test1;
			vr::VRActionHandle_t test2;
			vr::VRActionHandle_t test3;
			vr::VRActionHandle_t test4;
			vr::VRActionHandle_t test5;
			vr::VRActionHandle_t test6;
			vr::VRActionSetHandle_t set_test1;
			vr::VRActionSetHandle_t set_test2;

			error = vr::VRInput()->GetActionSetHandle("/actions/main/in/OpenInventory", &test1);
			error = vr::VRInput()->GetActionSetHandle("/actions/driving/in/HonkHorn", &test2);
			error = vr::VRInput()->GetActionSetHandle("/actions/driving/out/SpeedBump", &test3);
			error = vr::VRInput()->GetActionSetHandle("/actions/driving/in/Throttle", &test4);
			error = vr::VRInput()->GetActionSetHandle("/actions/main/in/RightHand", &test5);
			error = vr::VRInput()->GetActionSetHandle("/actions/main/in/RightHand_Anim", &test6);

			error = vr::VRInput()->GetActionSetHandle("/actions/main", &set_test1);
			error = vr::VRInput()->GetActionSetHandle("/actions/driving", &set_test2);

			if (error != vr::EVRInputError::VRInputError_None)
				_MESSAGE("Error in registering actions to SteamVR Input (%d)", error);
			*/
			
		}
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

			//TODO: SteamVR Input
			//Updates SteamVR Input Actions
			//vr::VRInput()->UpdateActionState(...);

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

	void VRManager::ProcessHapticEvents(vr::TrackedDeviceIndex_t unControllerDeviceIndex, uint32_t unAxisId, unsigned short usDurationMicroSec)
	{
		if (IsInitialized())
		{
			//DEBUG Code
			vr::TrackedDeviceIndex_t leftHandIndex = _vr->GetTrackedDeviceIndexForControllerRole(vr::ETrackedControllerRole::TrackedControllerRole_LeftHand);
			vr::TrackedDeviceIndex_t rightHandIndex = _vr->GetTrackedDeviceIndexForControllerRole(vr::ETrackedControllerRole::TrackedControllerRole_RightHand);

			//Finds device (better map the values somewhere instead of checking each time)
			VRDevice eventDevice =
				(unControllerDeviceIndex == leftHandIndex) ? VRDevice::VRDevice_LeftController :
				(unControllerDeviceIndex == rightHandIndex) ? VRDevice::VRDevice_RightController :
				VRDevice::VRDevice_Unknown;

			DispatchVRHapticEvent(unAxisId, usDurationMicroSec, eventDevice);
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