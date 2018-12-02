#include "ScaleformVR.h"

#include "skse64/ScaleformMovie.h"
#include "skse64/Hooks_Scaleform.h"
#include "skse64/ScaleformCallbacks.h"
#include <list>
#include "hooks/openvr_hook.h"

namespace ScaleformVR
{
	bool equalGFxValue(const GFxValue& lhs, const GFxValue& rhs)
	{
		if (rhs.type == lhs.type) {
			switch (rhs.GetType()) {
			case GFxValue::kType_Null:
				return true;
			case GFxValue::kType_Bool:
				return lhs.GetBool() == rhs.GetBool();
			case GFxValue::kType_Number:
				return lhs.GetNumber() == rhs.GetNumber();
			case GFxValue::kType_String:
				return 0 == strcmp(lhs.GetString(), rhs.GetString());
			case GFxValue::kType_WideString:
				return 0 == wcscmp(lhs.GetWideString(), rhs.GetWideString());
			case GFxValue::kType_Object:
				return lhs.data.obj == rhs.data.obj;
			case GFxValue::kType_Array:
				// Not implemented =(
				return false;
			case GFxValue::kType_DisplayObject:
				return lhs.data.obj == rhs.data.obj;
			default:
				return false;
			}
		}
		return false;
	}

	struct ScaleformCallback {
		GFxMovieView *	movieView;
		GFxValue		object;
		const char *	methodName;

		bool operator==(const ScaleformCallback& rhs)
		{
			return equalGFxValue(this->object, rhs.object) && (0 == strcmp(this->methodName, rhs.methodName));
		}
	};

	typedef std::list <ScaleformCallback> ScaleformCallbackList;
	static ScaleformCallbackList g_scaleformInputHandlers;

	void DispatchControllerState(vr::ETrackedControllerRole controllerHand, vr::VRControllerState_t controllerState) {
		for (ScaleformCallbackList::iterator iter = g_scaleformInputHandlers.begin(); iter != g_scaleformInputHandlers.end(); ++iter)
		{
			GFxMovieView* movieView = (*iter).movieView;
			GFxValue* object = &(*iter).object;
			const char* methodName = (*iter).methodName;

			GFxValue result, args[5];
			args[0].SetNumber(controllerHand);
			args[1].SetNumber(controllerState.unPacketNum);
			args[2].SetNumber(controllerState.ulButtonPressed);
			args[3].SetNumber(controllerState.ulButtonTouched);

			GFxValue axisData;
			movieView->CreateArray(&args[4]);

			for (int i=0; i < vr::k_unControllerStateAxisCount; i++) {
				GFxValue x, y;
				x.SetNumber(controllerState.rAxis[i].x);
				y.SetNumber(controllerState.rAxis[i].y);

				args[4].PushBack(&x);
				args[4].PushBack(&y);
			}

			object->Invoke(methodName, &result, args, 5);
		}
	}

	class VRInputScaleform_ShutoffButtonEventsToGame : public GFxFunctionHandler
	{
	public:
		virtual void	Invoke(Args * args)
		{
			//_MESSAGE("ShutoffButtonEventsToGame dll fn called!");

			ASSERT(args->numArgs >= 1);

			bool shutoff = args->args[0].GetBool();

			setControllerStateUpdateShutoff(shutoff);

			//_MESSAGE("ShutoffButtonEventsToGame: %d", shutoff);
		}
	};

	class VRInputScaleform_RegisterInputHandler : public GFxFunctionHandler
	{
	public:
		virtual void	Invoke(Args * args)
		{
			//_MESSAGE("RegisterInputHandler dll fn called!");

			ASSERT(args->numArgs == 2);
			ASSERT(args->args[0].GetType() == GFxValue::kType_DisplayObject);
			ASSERT(args->args[1].GetType() == GFxValue::kType_String);

			ScaleformCallback callback;
			callback.movieView = args->movie;
			callback.object = args->args[0];
			callback.methodName = args->args[1].GetString();

			// If the same callback has already been registered, do not add it again.
			// This will cause the callback to triggered twice when input events are sent.
			for (ScaleformCallbackList::iterator iter = g_scaleformInputHandlers.begin(); iter != g_scaleformInputHandlers.end(); ++iter)
			{
				if (*iter == callback) {
					//_MESSAGE("Duplicate callback found, aborting register...");
					return;
				}
			}

			//_MESSAGE("Registering: %s", callback.methodName);
			g_scaleformInputHandlers.push_back(callback);
		}
	};

	class VRInputScaleform_UnregisterInputHandler : public GFxFunctionHandler
	{
	public:
		virtual void	Invoke(Args * args)
		{
			//_MESSAGE("VRInputScaleform_UnregisterInputHandler dll fn called!");
			ASSERT(args->numArgs == 2);
			ASSERT(args->args[0].GetType() == GFxValue::kType_DisplayObject);
			ASSERT(args->args[1].GetType() == GFxValue::kType_String);

			ScaleformCallback callback;
			callback.object = args->args[0];
			callback.methodName = args->args[1].GetString();

			g_scaleformInputHandlers.remove(callback);
			//_MESSAGE("Unregistering: %s | remaining: %d", callback.methodName, g_scaleformInputHandlers.size());
		}
	};

	bool RegisterFuncs(GFxMovieView * view, GFxValue * plugin) {
		//_MESSAGE("Registering scaleform functions");

		RegisterFunction<VRInputScaleform_ShutoffButtonEventsToGame>(plugin, view, "ShutoffButtonEventsToGame");
		RegisterFunction<VRInputScaleform_RegisterInputHandler>(plugin, view, "RegisterInputHandler");
		RegisterFunction<VRInputScaleform_UnregisterInputHandler>(plugin, view, "UnregisterInputHandler");

		return true;
	}
}