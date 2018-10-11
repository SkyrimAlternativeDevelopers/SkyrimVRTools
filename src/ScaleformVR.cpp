#include "ScaleformVR.h"

#include "skse64/ScaleformMovie.h"
#include "skse64/Hooks_Scaleform.h"
#include "skse64/ScaleformCallbacks.h"

namespace ScaleformVR
{
	class VRInputScaleform_ShutoffButtonEventsToGame : public GFxFunctionHandler
	{
	public:
		virtual void	Invoke(Args * args)
		{
			_MESSAGE("ShutoffButtonEventsToGame dll fn called!");

			ASSERT(args->numArgs >= 1);

			bool		shutoff = args->args[0].GetBool();

			_MESSAGE("ShutoffButtonEventsToGame: %d", shutoff);
		}
	};

	class VRInputScaleform_RegisterInputHandler : public GFxFunctionHandler
	{
	public:
		virtual void	Invoke(Args * args)
		{
			_MESSAGE("RegisterInputHandler dll fn called!");
		}
	};

	class VRInputScaleform_UnregisterInputHandler : public GFxFunctionHandler
	{
	public:
		virtual void	Invoke(Args * args)
		{
			_MESSAGE("VRInputScaleform_UnregisterInputHandler dll fn called!");
		}
	};

	bool RegisterFuncs(GFxMovieView * view, GFxValue * plugin) {
		_MESSAGE("Registering scaleform functions");

		RegisterFunction<VRInputScaleform_ShutoffButtonEventsToGame>(plugin, view, "ShutoffButtonEventsToGame");
		RegisterFunction<VRInputScaleform_RegisterInputHandler>(plugin, view, "RegisterInputHandler");
		RegisterFunction<VRInputScaleform_UnregisterInputHandler>(plugin, view, "UnregisterInputHandler");

		return true;
	}
}