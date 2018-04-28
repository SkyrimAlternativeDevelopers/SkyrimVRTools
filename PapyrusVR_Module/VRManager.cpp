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
			vr::VRCompositorError error = _compositor->GetLastPoses((vr::TrackedDevicePose_t*)_renderPoses, vr::k_unMaxTrackedDeviceCount, (vr::TrackedDevicePose_t*)_gamePoses, vr::k_unMaxTrackedDeviceCount);
			if (error && error != vr::EVRCompositorError::VRCompositorError_None)
				_MESSAGE("Error while retriving game poses!");

			//Updates poses
			_leftHandID = _vr->GetTrackedDeviceIndexForControllerRole(vr::ETrackedControllerRole::TrackedControllerRole_LeftHand);
			_rightHandID = _vr->GetTrackedDeviceIndexForControllerRole(vr::ETrackedControllerRole::TrackedControllerRole_RightHand);
			for (int i = 0; i < vr::k_unMaxTrackedDeviceCount; i++)
			{
				switch (_vr->GetTrackedDeviceClass(i))
				{
					case vr::ETrackedDeviceClass::TrackedDeviceClass_HMD:
						_hmdID = i;
					break;
					case vr::ETrackedDeviceClass::TrackedDeviceClass_GenericTracker:
						//TODO: Trackers
						break;
				}
			}

			//Process Events
			ProcessControllerEvents(_leftHandID);
			ProcessControllerEvents(_rightHandID);
		}
		else
			Init();
	}
	
	VREvent VRManager::CheckStatesForMask(UInt64 previousEvt, UInt64 newEvt, UInt64 mask)
	{
		
		if ((newEvt & mask) && !(previousEvt & mask))
			return VREvent::Positive;
		else if(!(newEvt & mask) && (previousEvt & mask))
			return VREvent::Negative;

		return VREvent::None;
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

	void VRManager::ProcessControllerEvents(int controllerID)
	{
		if (_vr)
		{
			VRDevice currentDevice = controllerID == _leftHandID ? VRDevice::LeftController : VRDevice::RightController;
			vr::VRControllerState_t newState;
			VREvent eventResult;
			_vr->GetControllerState(controllerID, &newState, sizeof(vr::VRControllerState_t));
			if (newState.unPacketNum != _controllerStates[controllerID].unPacketNum)
			{
				//We need to implement events here, using PollNextEvent could lead to skyrim losing events

				//Button Pressed/Release
				if (_controllerStates[controllerID].ulButtonPressed != newState.ulButtonPressed)
				{
					for (vr::EVRButtonId& button : _possibleButtonsIds)
					{
						eventResult = CheckStatesForMask(_controllerStates[controllerID].ulButtonPressed, newState.ulButtonPressed, vr::ButtonMaskFromId(button));
						if (eventResult == VREvent::Positive)
							DispatchVRButtonEvent(VREventType::Pressed, (EVRButtonId)button, currentDevice);

						if (eventResult == VREvent::Negative)
							DispatchVRButtonEvent(VREventType::Released, (EVRButtonId)button, currentDevice);
					}
				}

				//Button Touched/Untouched
				if (_controllerStates[controllerID].ulButtonTouched != newState.ulButtonTouched)
				{
					for (vr::EVRButtonId& button : _possibleButtonsIds)
					{
						eventResult = CheckStatesForMask(_controllerStates[controllerID].ulButtonTouched, newState.ulButtonTouched, vr::ButtonMaskFromId(button));
						if (eventResult == VREvent::Positive)
							DispatchVRButtonEvent(VREventType::Touched, (EVRButtonId)button, currentDevice);

						if (eventResult == VREvent::Negative)
							DispatchVRButtonEvent(VREventType::Untouched, (EVRButtonId)button, currentDevice);
					}
				}

				_controllerStates[controllerID] = newState;
			}
		}
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

	void VRManager::RegisterVRButtonListener(OnVRButtonEvent listener)
	{
		if (listener)
		{
			_vrButtonEventsListenersMutex.lock();
			_vrButtonEventsListeners.remove(listener);
			_vrButtonEventsListeners.push_back(listener);
			_vrButtonEventsListenersMutex.unlock();
		}
	}

	void VRManager::UnregisterVRButtonListener(OnVRButtonEvent listener)
	{
		if (listener)
		{
			_vrButtonEventsListenersMutex.lock();
			_vrButtonEventsListeners.remove(listener);
			_vrButtonEventsListenersMutex.unlock();
		}
	}

	TrackedDevicePose* VRManager::GetHMDPose(bool renderPose)
	{ 
		if (_hmdID < 0)
			return NULL;
		return renderPose ? &_renderPoses[_hmdID] : &_gamePoses[_hmdID];
	}
	TrackedDevicePose* VRManager::GetRightHandPose(bool renderPose)
	{
		if (_rightHandID < 0)
			return NULL;
		return renderPose ? &_renderPoses[_rightHandID] : &_gamePoses[_rightHandID];
	}

	TrackedDevicePose* VRManager::GetLeftHandPose(bool renderPose)
	{
		if (_leftHandID < 0)
			return NULL; 
		return renderPose ? &_renderPoses[_leftHandID] : &_gamePoses[_leftHandID]; 
	}

	TrackedDevicePose* VRManager::GetPoseByDeviceEnum(VRDevice device)
	{
		TrackedDevicePose* requestedPose = NULL;
		switch (device)
		{
			case VRDevice::HMD:
				requestedPose = GetHMDPose();
				break;
			case VRDevice::RightController:
				requestedPose = GetRightHandPose();
				break;
			case VRDevice::LeftController:
				requestedPose = GetLeftHandPose();
				break;
		}
		return requestedPose;
	}
}