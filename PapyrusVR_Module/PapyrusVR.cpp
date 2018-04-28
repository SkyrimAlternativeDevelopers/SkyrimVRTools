#include "PapyrusVR.h"

namespace PapyrusVR 
{
	//OpenVR Hook
	RelocAddr <uintptr_t>	OpenVR_Call(0xC50C69);
	BranchTrampoline		l_LocalBranchTrampoline;

	//Custom Pose Event
	RegistrationSetHolder<TESForm*>				g_posesUpdateEventRegs;
	PoseUpdateListeners							g_poseUpdateListeners;

	//API
	std::mutex					listenersMutex; 
	SKSEMessagingInterface*		g_messagingInterface;
	PluginHandle*				g_pluginHandle;
	PapyrusVRAPI apiMessage;

	#pragma region Papyrus Events
		class EventQueueFunctor0 : public IFunctionArguments
		{
		public:
			EventQueueFunctor0(BSFixedString & a_eventName)
				: eventName(a_eventName.data) {}

			virtual bool Copy(Output * dst) { return true; }

			void operator() (const EventRegistration<TESForm*> & reg)
			{
				VMClassRegistry * registry = (*g_skyrimVM)->GetClassRegistry();
				registry->QueueEvent(reg.handle, &eventName, this);
			}

		private:
			BSFixedString	eventName;
		};
	#pragma endregion

	#pragma region Papyrus Native Functions
		#pragma region SteamVR Coordinates
			void GetSteamVRDeviceRotation(StaticFunctionTag *base, UInt32 deviceEnum, VMArray<float> returnValues)
			{
				_MESSAGE("GetSteamVRDeviceRotation");
				CopyPoseToVMArray(deviceEnum, &returnValues, PoseParam::Rotation);
			}

			void GetSteamVRDeviceQRotation(StaticFunctionTag *base, UInt32 deviceEnum, VMArray<float> returnValues)
			{
				_MESSAGE("GetSteamVRDeviceQRotation");
				CopyPoseToVMArray(deviceEnum, &returnValues, PoseParam::QRotation);
			}

			void GetSteamVRDevicePosition(StaticFunctionTag *base, UInt32 deviceEnum, VMArray<float> returnValues)
			{
				_MESSAGE("GetSteamVRDevicePosition");
				CopyPoseToVMArray(deviceEnum, &returnValues, PoseParam::Position);

			}
		#pragma endregion

		#pragma region Skyrim Coordinates
			void GetSkyrimDeviceRotation(StaticFunctionTag *base, UInt32 deviceEnum, VMArray<float> returnValues)
			{
				_MESSAGE("GetSkyrimDeviceRotation");
				CopyPoseToVMArray(deviceEnum, &returnValues, PoseParam::Rotation, true);
			}

			void GetSkyrimDeviceQRotation(StaticFunctionTag *base, UInt32 deviceEnum, VMArray<float> returnValues)
			{
				_MESSAGE("GetSkyrimDeviceQRotation");
				CopyPoseToVMArray(deviceEnum, &returnValues, PoseParam::QRotation, true);
			}

			void GetSkyrimDevicePosition(StaticFunctionTag *base, UInt32 deviceEnum, VMArray<float> returnValues)
			{
				_MESSAGE("GetSkyrimDevicePosition");
				CopyPoseToVMArray(deviceEnum, &returnValues, PoseParam::Position, true);
			}
		#pragma endregion

		void RegisterForPoseUpdates(StaticFunctionTag *base,	TESForm* thisForm)
		{
			_MESSAGE("RegisterForPoseUpdates");
			if (!thisForm)
			{
				_MESSAGE("Called RegisterForPoseUpdates from NULL form!");
				return;
			}

			g_posesUpdateEventRegs.Register(thisForm->GetFormType(), thisForm);


			if (thisForm->formID)
				_MESSAGE("%d registered for PoseUpdates", thisForm->formID);
		}

		void UnregisterForPoseUpdates(StaticFunctionTag *base,	TESForm* thisForm)
		{
			_MESSAGE("UnregisterForPoseUpdates");
			if (!thisForm)
			{
				_MESSAGE("Called UnregisterForPoseUpdates from NULL form!");
				return;
			}

			g_posesUpdateEventRegs.Unregister(thisForm->GetFormType(), thisForm);

			if (thisForm->formID)
				_MESSAGE("%d unregistered for PoseUpdates", thisForm->formID);
		}

		//Used for debugging
		std::clock_t start = clock();
		void TimeSinceLastCall(StaticFunctionTag* base)
		{
			clock_t end = clock();
			double elapsed_seconds = double(end - start) / CLOCKS_PER_SEC;
			_MESSAGE("90 events fired after %f seconds", elapsed_seconds);
			start = end;
		}

	#pragma endregion

	#pragma region OpenVR Hooks

		//SkyrimVR+0xC50C69
		BSFixedString eventName("OnPosesUpdate");
		clock_t lastFrame = clock();
		clock_t thisFrame;
		double deltaTime = 0.0f;
		void OnVRUpdate()
		{
			//Calculate deltaTime
			thisFrame = clock();
			deltaTime = double(thisFrame - lastFrame) / CLOCKS_PER_SEC;
			lastFrame = thisFrame;

			//Update Poses
			VRManager::GetInstance().UpdatePoses();

			//Notify Listeners
			listenersMutex.lock();
			for (OnPoseUpdateCallback& callback : g_poseUpdateListeners)
				callback(deltaTime);
			listenersMutex.unlock();

			//Notify Papyrus scripts
			//WARNING: Disabled cause this will currently freeze the game every 90 seconds
			//if (g_posesUpdateEventRegs.m_data.size() > 0)
			//	g_posesUpdateEventRegs.ForEach(
			//		EventQueueFunctor0(eventName)
			//	);
		}

	#pragma endregion

	#pragma region API
		void RegisterPoseUpdateListener(OnPoseUpdateCallback callback)
		{
			listenersMutex.lock();
			g_poseUpdateListeners.push_back(callback);
			listenersMutex.unlock();
		}

		//Returns the VRManager singleton instance
		VRManagerAPI* GetVRManager()
		{
			return &VRManager::GetInstance();
		}
	#pragma endregion

	#pragma region Messaging Interface

		// Listens for SKSE events
		void OnSKSEMessageRecived(SKSEMessagingInterface::Message* message)
		{
			if (message)
			{
				if (message->type == SKSEMessagingInterface::kMessage_PostPostLoad)
				{
					if (g_messagingInterface && g_pluginHandle)
					{
						_MESSAGE("Game Loaded, Dispatching Init messages to all listeners");
						apiMessage.GetVRManager = GetVRManager;
						apiMessage.RegisterPoseUpdateListener = RegisterPoseUpdateListener;

						//Sends pointers to API functions/classes
						g_messagingInterface->Dispatch(*g_pluginHandle, kPapyrusVR_Message_Init, &apiMessage, sizeof(apiMessage), NULL);
					}
				}
			}
		}

		void RegisterMessagingInterface(SKSEMessagingInterface* messagingInterface)
		{
			if (messagingInterface && g_pluginHandle)
			{
				g_messagingInterface = messagingInterface;
				_MESSAGE("Registering for plugin loaded message!");
				g_messagingInterface->RegisterListener(*g_pluginHandle, "SKSE", OnSKSEMessageRecived);
			}
		}

		void RegisterHandle(PluginHandle* handle)
		{
			if (handle)
				g_pluginHandle = handle;
		}
	#pragma endregion

	#pragma region Utility Methods
		void CopyPoseToVMArray(UInt32 deviceType, VMArray<float>* resultArray, PoseParam parameter, bool skyrimWorldSpace)
		{
			TrackedDevicePose_t* requestedPose = VRManager::GetInstance().GetPoseByDeviceEnum((VRDevice)deviceType);

			//Pose exists
			if (requestedPose)
			{
				HmdMatrix34_t matrix = requestedPose->mDeviceToAbsoluteTracking;

				if (parameter == PoseParam::Position)
				{
					//Position
					HmdVector3_t devicePosition = OpenVRUtils::GetPosition(&(requestedPose->mDeviceToAbsoluteTracking));

					//Really dumb way to do it, just for testing
					if (skyrimWorldSpace)
					{
						NiAVObject* playerNode = (*g_thePlayer)->GetNiNode();
						devicePosition.v[0] += playerNode->m_worldTransform.pos.x;
						devicePosition.v[1] += playerNode->m_worldTransform.pos.y;
						devicePosition.v[2] += playerNode->m_worldTransform.pos.z;
					}

					OpenVRUtils::CopyVector3ToVMArray(&devicePosition, resultArray);
				}
				else
				{
					HmdQuaternion_t quatRotation = OpenVRUtils::GetRotation(&(requestedPose->mDeviceToAbsoluteTracking));

					if (parameter == PoseParam::Rotation)
					{
						//Euler
						HmdVector3_t deviceRotation = OpenVRUtils::QuatToEuler(&quatRotation);
						OpenVRUtils::CopyVector3ToVMArray(&deviceRotation, resultArray);
					}
					else
					{
						//Quaternion
						OpenVRUtils::CopyQuaternionToVMArray(&quatRotation, resultArray);
					}
				}
			}
		}
	#pragma endregion

	//Entry Point
	bool RegisterFuncs(VMClassRegistry* registry) 
	{
		_MESSAGE("Registering native functions...");
		registry->RegisterFunction(new NativeFunction2 <StaticFunctionTag, void, UInt32, VMArray<float>>("GetSteamVRDeviceRotation_Native", "PapyrusVR", PapyrusVR::GetSteamVRDeviceRotation, registry));
		registry->RegisterFunction(new NativeFunction2 <StaticFunctionTag, void, UInt32, VMArray<float>>("GetSteamVRDeviceQRotation_Native", "PapyrusVR", PapyrusVR::GetSteamVRDeviceQRotation, registry));
		registry->RegisterFunction(new NativeFunction2 <StaticFunctionTag, void, UInt32, VMArray<float>>("GetSteamVRDevicePosition_Native", "PapyrusVR", PapyrusVR::GetSteamVRDevicePosition, registry));
		registry->RegisterFunction(new NativeFunction2 <StaticFunctionTag, void, UInt32, VMArray<float>>("GetSkyrimDeviceRotation_Native", "PapyrusVR", PapyrusVR::GetSkyrimDeviceRotation, registry));
		registry->RegisterFunction(new NativeFunction2 <StaticFunctionTag, void, UInt32, VMArray<float>>("GetSkyrimDeviceQRotation_Native", "PapyrusVR", PapyrusVR::GetSkyrimDeviceQRotation, registry));
		registry->RegisterFunction(new NativeFunction2 <StaticFunctionTag, void, UInt32, VMArray<float>>("GetSkyrimDevicePosition_Native", "PapyrusVR", PapyrusVR::GetSkyrimDevicePosition, registry));
		registry->RegisterFunction(new NativeFunction1 <StaticFunctionTag, void, TESForm*>("RegisterForPoseUpdates", "PapyrusVR", PapyrusVR::RegisterForPoseUpdates, registry));
		registry->RegisterFunction(new NativeFunction1 <StaticFunctionTag, void, TESForm*>("UnregisterForPoseUpdates", "PapyrusVR", PapyrusVR::UnregisterForPoseUpdates, registry));
		registry->RegisterFunction(new NativeFunction0 <StaticFunctionTag, void>("TimeSinceLastCall", "PapyrusVR", PapyrusVR::TimeSinceLastCall, registry)); //Debug function

		_MESSAGE("Creating trampoline");
		if (!l_LocalBranchTrampoline.Create(1024 * 64))
		{
			_MESSAGE("Can't create local branch trampoline!");
			return false;
		}

		_MESSAGE("Hooking into OpenVR calls");
		l_LocalBranchTrampoline.Write5Call(OpenVR_Call, GetFnAddr(&PapyrusVR::OnVRUpdate));

		return true;
	}
}
