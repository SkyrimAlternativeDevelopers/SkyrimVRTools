#include "PapyrusVR.h"

namespace PapyrusVR 
{
	//OpenVR Hook
	RelocAddr <uintptr_t> OpenVR_Call(0xC50C69);
	BranchTrampoline l_LocalBranchTrampoline;

	//Custom Pose Event
	RegistrationSetHolder<TESForm*>				g_posesUpdateEventRegs;
	EventDispatcher<PapyrusVRPosesUpdateEvent>	g_posesUpdateEventDispatcher;
	PosesUpdateUpdateEventHandler				g_posesUpdateEventHandler;
	PapyrusVRPosesUpdateEvent					ev;
	

	#pragma region CustomPose Event
	class EventQueueFunctor0 : public IFunctionArguments
	{
	public:
		EventQueueFunctor0(BSFixedString & a_eventName)
			: eventName(a_eventName.data) {}

		virtual bool	Copy(Output * dst)
		{
			//dst->Resize(1);
			//SetVMValue(dst->Get(0), args1);

			return true;
		}

		void			operator() (const EventRegistration<TESForm*> & reg)
		{
			VMClassRegistry * registry = (*g_skyrimVM)->GetClassRegistry();
			registry->QueueEvent(reg.handle, &eventName, this);
		}

	private:
		BSFixedString	eventName;
	};

	EventResult PosesUpdateUpdateEventHandler::ReceiveEvent(PapyrusVRPosesUpdateEvent * evn, EventDispatcher<PapyrusVRPosesUpdateEvent> * dispatcher)
	{
		g_posesUpdateEventRegs.ForEach(
			EventQueueFunctor0(BSFixedString("OnPosesUpdate"))
		);

		return kEvent_Continue;
	}

	#pragma endregion

	#pragma region Papyrus Native Functions

	void GetTrackedDevicePoseByID(StaticFunctionTag *base, UInt32 deviceEnum, VMArray<float> returnValues)
	{
		_MESSAGE("GetTrackedDevicePoseByID");
		TrackedDevicePose_t* requestedPose;
		switch (deviceEnum)
		{
			case TrackedDevice::HMD:
				requestedPose = VRManager::GetInstance().GetHMDPose();
				break;
			case TrackedDevice::RightController:
				requestedPose = VRManager::GetInstance().GetRightHandPose();
				break;
			case TrackedDevice::LeftController:
				requestedPose = VRManager::GetInstance().GetLeftHandPose();
				break;
		}
		
		//Pose exists
		if (requestedPose)
		{
			HmdQuaternion_t deviceRotation = OpenVRUtils::GetRotation(requestedPose->mDeviceToAbsoluteTracking);
			HmdVector3_t devicePosition = OpenVRUtils::GetPosition(requestedPose->mDeviceToAbsoluteTracking);

			float number;

			//Position
			//Set X
			returnValues.Get(&number, 0);
			number = devicePosition.v[0];
			returnValues.Set(&number, 0);

			//Set Y
			returnValues.Get(&number, 1);
			number = devicePosition.v[1];
			returnValues.Set(&number, 1);

			//Set Z
			returnValues.Get(&number, 2);
			number = devicePosition.v[2];
			returnValues.Set(&number, 2);

			//Rotation
			//Set X
			returnValues.Get(&number, 3);
			number = deviceRotation.x;
			returnValues.Set(&number, 3);

			//Set Y
			returnValues.Get(&number, 4);
			number = deviceRotation.y;
			returnValues.Set(&number, 4);

			//Set Z
			returnValues.Get(&number, 5);
			number = deviceRotation.z;
			returnValues.Set(&number, 5);

			//Set W
			returnValues.Get(&number, 6);
			number = deviceRotation.w;
			returnValues.Set(&number, 6);
		}
	}

	void RegisterForPoseUpdates(StaticFunctionTag *base,TESForm * thisForm)
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

	void UnregisterForPoseUpdates(StaticFunctionTag *base,TESForm * thisForm)
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

	#pragma endregion

	//SkyrimVR+0xC50C69
	void OnVRUpdate()
	{
		VRManager::GetInstance().UpdatePoses();

		//Notify Papyrus scripts
		if (g_posesUpdateEventRegs.m_data.size() > 0)
		{
			//Crashes here
			//g_posesUpdateEventDispatcher.SendEvent(&ev);
		}
	}

	bool RegisterFuncs(VMClassRegistry* registry) {
		_MESSAGE("Registering native functions...");
		registry->RegisterFunction(new NativeFunction2 <StaticFunctionTag, void, UInt32, VMArray<float>>("GetTrackedDevicePoseByIDNative", "PapyrusVR", PapyrusVR::GetTrackedDevicePoseByID, registry));
		registry->RegisterFunction(new NativeFunction1 <StaticFunctionTag, void, TESForm*>("RegisterForPoseUpdates", "PapyrusVR", PapyrusVR::RegisterForPoseUpdates, registry));
		registry->RegisterFunction(new NativeFunction1 <StaticFunctionTag, void, TESForm*>("UnregisterForPoseUpdates", "PapyrusVR", PapyrusVR::UnregisterForPoseUpdates, registry));
		
		_MESSAGE("Creating event sink");
		g_posesUpdateEventDispatcher.AddEventSink(&g_posesUpdateEventHandler);
		
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
