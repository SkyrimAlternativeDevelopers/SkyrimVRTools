#include "ScaleformVR.h"

#include "skse64/ScaleformMovie.h"
#include "skse64/Hooks_Scaleform.h"
#include "skse64/ScaleformCallbacks.h"
#include <list>

namespace ScaleformVR
{
	struct ScaleformCallback {
		const GFxValue	* object;
		const char		* methodName;

		bool operator==(const ScaleformCallback& rhs)
		{
			return (this->object && rhs.object) && (0 == strcmp(this->methodName, rhs.methodName));
		}
	};

	typedef std::list <ScaleformCallback> ScaleformCallbackList;
	static ScaleformCallbackList g_scaleformInputHandlers;

	class VRInputScaleform_ShutoffButtonEventsToGame : public GFxFunctionHandler
	{
	public:
		virtual void	Invoke(Args * args)
		{
			_MESSAGE("ShutoffButtonEventsToGame dll fn called!");

			ASSERT(args->numArgs >= 1);

			bool shutoff = args->args[0].GetBool();

			_MESSAGE("ShutoffButtonEventsToGame: %d", shutoff);
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
			callback.object = &args->args[0];
			callback.methodName = args->args[1].GetString();

			// If the same callback has already been registered, do not add it again.
			// This will cause the callback to triggered twice when input events are sent.
			for (ScaleformCallbackList::iterator iter = g_scaleformInputHandlers.begin(); iter != g_scaleformInputHandlers.end(); ++iter)
			{
				if (*iter == callback) {
					_MESSAGE("Duplicate callback found, aborting register...");
					return;
				}
			}

			_MESSAGE("Registering: %s", callback.methodName);
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
			callback.object = &args->args[0];
			callback.methodName = args->args[1].GetString();

			_MESSAGE("Unregistering: %s", callback.methodName);
			g_scaleformInputHandlers.remove(callback);
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