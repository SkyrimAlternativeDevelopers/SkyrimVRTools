#pragma once
#include <list>
#include <mutex>
#include <map>
#include "openvr.h"
#include "api/VRManagerAPI.h"
#include "api/PapyrusVRTypes.h"
#include "api/collisions/LocalOverlapObject.h"
#include "api/collisions/IShape.h"
#include "api/collisions/Sphere.h"

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
		VRManager() : _localOverlapObjectCount(1) { }

		vr::IVRCompositor* _compositor;
		vr::IVRSystem* _vr;

		TrackedDevicePose _renderPoses[k_unMaxTrackedDeviceCount]; //Used to store available poses
		TrackedDevicePose _gamePoses[k_unMaxTrackedDeviceCount]; //Used to store available poses
		vr::VRControllerState_t _controllerStates[k_unMaxTrackedDeviceCount]; //Used to store states

		std::mutex _vrLocalOverlapObjectMapMutex;
		std::mutex _vrButtonEventsListenersMutex;
		std::mutex _vrOverlapEventsListenersMutex;

		std::list<OnVRButtonEvent> _vrButtonEventsListeners;
		std::list<OnVROverlapEvent> _vrOverlapEventsListeners;

		UInt32 _localOverlapObjectCount;
		std::map<UInt32, LocalOverlapObject*> _localOverlapObjects;

		TrackedDevicePose* _orderedTrackedDevicesMap[2][k_unMaxTrackedDeviceCount];
		void UpdateTrackedDevicesMapEntry(VRDevice device, uint32_t posePointer);

		VREvent CheckStatesForMask(UInt64 previousEvt, UInt64 newEvt, UInt64 mask);

		void DispatchVRButtonEvent(VREventType eventType, EVRButtonId button, VRDevice device);
		void DispatchVROverlapEvent(VROverlapEvent eventType, UInt32 objectHandle, VRDevice device);

		void ProcessControllerEvents(VRDevice currentDevice);
		void ProcessOverlapEvents(VRDevice currentDevice);

		template <typename T1>
		void GenericRegisterForEvent(T1 listener, std::list<T1>* list, std::mutex* listMutex)
		{
			if (listener)
			{
				listMutex->lock();
				list->remove(listener);
				list->push_back(listener);
				listMutex->unlock();
			}
		};

		template <typename T1>
		void GenericUnregisterForEvent(T1 listener, std::list<T1>* list, std::mutex* listMutex)
		{
			if (listener)
			{
				listMutex->lock();
				list->remove(listener);
				listMutex->unlock();
			}
		};

	public:
		VRManager(VRManager const&) = delete;
		void operator=(VRManager const&) = delete;
		void Init();
		void UpdatePoses();

		bool IsInitialized();

		//Events Register/Unregister
		void RegisterVRButtonListener(OnVRButtonEvent listener) { GenericRegisterForEvent(listener, &_vrButtonEventsListeners, &_vrButtonEventsListenersMutex); }
		void UnregisterVRButtonListener(OnVRButtonEvent listener) { GenericUnregisterForEvent(listener, &_vrButtonEventsListeners, &_vrButtonEventsListenersMutex); }
		void RegisterVROverlapListener(OnVROverlapEvent listener) { GenericRegisterForEvent(listener, &_vrOverlapEventsListeners, &_vrOverlapEventsListenersMutex); }
		void UnregisterVROverlapListener(OnVROverlapEvent listener) { GenericUnregisterForEvent(listener, &_vrOverlapEventsListeners, &_vrOverlapEventsListenersMutex); }

		//Overlap Engine Methods
		UInt32 CreateLocalOverlapSphere(float radius, Matrix34* transform, VRDevice attachedDevice = VRDevice::VRDevice_Unknown);
		void DestroyLocalOverlapObject(UInt32 overlapObjectHandle);

		TrackedDevicePose* GetHMDPose(bool renderPose = true);
		TrackedDevicePose* GetRightHandPose(bool renderPose = true);
		TrackedDevicePose* GetLeftHandPose(bool renderPose = true);
		TrackedDevicePose* GetPoseByDeviceEnum(VRDevice device, bool renderPose = true);
	};
}
