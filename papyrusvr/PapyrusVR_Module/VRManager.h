#pragma once
#include <list>
#include <mutex>
#include <map>
#include "openvr.h"
#include "EventHandling.h"
#include "api/VRManagerAPI.h"
#include "api/PapyrusVRTypes.h"
#include "api/collisions/LocalOverlapObject.h"
#include "api/collisions/IShape.h"
#include "api/collisions/Sphere.h"

//TODO: Implement OpenVRHook getters, setters and callbacks
namespace PapyrusVR
{
	//Singleton to manage OpenVR calls (cache poses etc.)
	class VRManager : public VRManagerAPI
	{
	public:
		//Singleton Logic
		static VRManager& GetInstance()
		{
			static VRManager instance;

			return instance;
		}
	
	#pragma region EventHandling
	private:
		//Mutex
		std::mutex _vrButtonEventsListenersMutex;
		std::mutex _vrOverlapEventsListenersMutex;
		std::mutex _vrHapticEventsListenersMutex;
		std::mutex _vrUpdateListenersMutex;

		//Listeners Lists
		std::list<OnVRButtonEvent> _vrButtonEventsListeners;
		std::list<OnVROverlapEvent> _vrOverlapEventsListeners;
		std::list<OnVRHapticEvent> _vrHapticEventsListeners;
		std::list<OnVRUpdateEvent> _vrUpdateListeners;

		//Dispatchers
		void DispatchVRButtonEvent(VREventType eventType, EVRButtonId button, VRDevice device) { GenericDispactchEvent(&_vrButtonEventsListeners, &_vrButtonEventsListenersMutex, eventType, button, device); }
		void DispatchVROverlapEvent(VROverlapEvent eventType, UInt32 objectHandle, VRDevice device) { GenericDispactchEvent(&_vrOverlapEventsListeners, &_vrOverlapEventsListenersMutex, eventType, objectHandle, device); }
		void DispatchVRHapticEvent(UInt32 axisId, UInt32 pulseDuration, VRDevice device) { GenericDispactchEvent(&_vrHapticEventsListeners, &_vrHapticEventsListenersMutex, axisId, pulseDuration, device); }
		void DispatchVRUpdateEvent(float deltaTime) { GenericDispactchEvent(&_vrUpdateListeners, &_vrUpdateListenersMutex, deltaTime); }

	public:
		//Events Register/Unregister
		void RegisterVRButtonListener(OnVRButtonEvent listener) { GenericRegisterForEvent(listener, &_vrButtonEventsListeners, &_vrButtonEventsListenersMutex); }
		void UnregisterVRButtonListener(OnVRButtonEvent listener) { GenericUnregisterForEvent(listener, &_vrButtonEventsListeners, &_vrButtonEventsListenersMutex); }
		void RegisterVROverlapListener(OnVROverlapEvent listener) { GenericRegisterForEvent(listener, &_vrOverlapEventsListeners, &_vrOverlapEventsListenersMutex); }
		void UnregisterVROverlapListener(OnVROverlapEvent listener) { GenericUnregisterForEvent(listener, &_vrOverlapEventsListeners, &_vrOverlapEventsListenersMutex); }
		void RegisterVRHapticListener(OnVRHapticEvent listener) { GenericRegisterForEvent(listener, &_vrHapticEventsListeners, &_vrHapticEventsListenersMutex); }
		void UnregisterVRHapticListener(OnVRHapticEvent listener) { GenericUnregisterForEvent(listener, &_vrHapticEventsListeners, &_vrHapticEventsListenersMutex); }
		void RegisterVRUpdateListener(OnVRUpdateEvent listener) { GenericRegisterForEvent(listener, &_vrUpdateListeners, &_vrUpdateListenersMutex); }
		void UnregisterVRUpdateListener(OnVRUpdateEvent listener) { GenericUnregisterForEvent(listener, &_vrUpdateListeners, &_vrUpdateListenersMutex); }
	#pragma endregion

	private:
		VRManager() : _nextLocalOverlapObjectHandle(1) { }

		vr::IVRCompositor* _compositor = nullptr;
		vr::IVRSystem* _vr = nullptr;

		TrackedDevicePose _renderPoses[k_unMaxTrackedDeviceCount]; //Used to store available poses
		TrackedDevicePose _gamePoses[k_unMaxTrackedDeviceCount]; //Used to store available poses
		vr::VRControllerState_t _controllerStates[k_unMaxTrackedDeviceCount]; //Used to store states

		std::mutex _vrLocalOverlapObjectMapMutex;
		

		UInt32 _nextLocalOverlapObjectHandle;
		std::map<UInt32, LocalOverlapObject*> _localOverlapObjects;

		TrackedDevicePose* _orderedTrackedDevicesMap[2][k_unMaxTrackedDeviceCount];
		void UpdateTrackedDevicesMapEntry(VRDevice device, uint32_t posePointer);

		VREvent CheckStatesForMask(UInt64 previousEvt, UInt64 newEvt, UInt64 mask);

		void ProcessControllerEvents(VRDevice currentDevice);
		void ProcessOverlapEvents(VRDevice currentDevice);

	public:
		VRManager(VRManager const&) = delete;
		void operator=(VRManager const&) = delete;
		void InitVRCompositor(vr::IVRCompositor* compositor);
		void InitVRSystem(vr::IVRSystem* vrSystem);
		void RegisterInputActions();

		void UpdatePoses();
		void ProcessHapticEvents(vr::TrackedDeviceIndex_t unControllerDeviceIndex, uint32_t unAxisId, unsigned short usDurationMicroSec);

		bool IsInitialized();

		//Overlap Engine Methods
		UInt32 CreateLocalOverlapSphere(float radius, Matrix34* transform, VRDevice attachedDevice = VRDevice::VRDevice_Unknown);
		void DestroyLocalOverlapObject(UInt32 overlapObjectHandle);

		TrackedDevicePose* GetHMDPose(bool renderPose = true);
		TrackedDevicePose* GetRightHandPose(bool renderPose = true);
		TrackedDevicePose* GetLeftHandPose(bool renderPose = true);
		TrackedDevicePose* GetPoseByDeviceEnum(VRDevice device, bool renderPose = true);
	};
}
