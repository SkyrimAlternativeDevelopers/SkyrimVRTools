#include "openvr_hook.h"
#include "../VRManager.h"
#include "HookVRSystem.h"
#include "HookVRCompositor.h"
#include "skse64_common/SafeWrite.h"
#include "skse64_common/Utilities.h"

using namespace vr;
using namespace PapyrusVR;

static void *VR_CALLTYPE Hook_VR_GetGenericInterface(const char *pchInterfaceVersion, EVRInitError *peError)
{
	//If we have a translation for our interface, return the value
	if (interfaceTranslationMap.count(pchInterfaceVersion) != 0)
	{
		IHookInterfaceFactory* translatedVersion = interfaceTranslationMap[pchInterfaceVersion];
		_MESSAGE("[OpenVR_Hook] Custom interface %s defined for original interface %s", translatedVersion->GetWrappedVersion().c_str(), pchInterfaceVersion);
		void* requestedInterface = OpenVR_VR_GetGenericInterface(translatedVersion->GetWrappedVersion().c_str(), peError);
		void* builtInterface = translatedVersion->Build(requestedInterface);
		if (builtInterface == nullptr)
		{
			_MESSAGE("[OpenVR_Hook] Hooked cannot cast interface %s to anything...", typeid(requestedInterface).name());
			return requestedInterface;
		}
		else
			return builtInterface;
	}
	else
	{
		_MESSAGE("[OpenVR_Hook] No custom interface defined for version: %s", pchInterfaceVersion);
		return OpenVR_VR_GetGenericInterface(pchInterfaceVersion, peError);
	}
}

//Tries to write our fake wrapper interface
bool DoHook()
{
	uintptr_t* pVR_GetGenericInterface = (uintptr_t*)GetIATAddr((UInt8 *)GetModuleHandle(NULL), "openvr_api.dll", "VR_GetGenericInterface");

	//Can't find VR_GetGenericInterface
	if (pVR_GetGenericInterface == nullptr)
		return false;


	//Creates translation map to wrap older OpenVR versions to new ones
	interfaceTranslationMap.emplace(std::string(SkyrimVR_IVRSystem_Version), (IHookInterfaceFactory*) new HookVRSystemFactory());
	interfaceTranslationMap.emplace(std::string(SkyrimVR_IVRCompositor_Version), (IHookInterfaceFactory*)new HookVRCompositorFactory());

	OpenVR_VR_GetGenericInterface = (void*(*)(const char *pchInterfaceVersion, EVRInitError *peError))*pVR_GetGenericInterface;

	SafeWrite64((uintptr_t)pVR_GetGenericInterface, (UInt64)Hook_VR_GetGenericInterface);
	return true;
}

