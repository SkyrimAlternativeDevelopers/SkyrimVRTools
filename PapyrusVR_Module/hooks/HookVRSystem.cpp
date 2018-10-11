#include "HookVRSystem.h"
#include "../VRManager.h"

using namespace vr;
using namespace PapyrusVR;

void* HookVRSystemFactory::Build(void* realInterface)
{
	if (realInterface != nullptr)
	{
		_MESSAGE("Hooked IVRSystem");
		VRManager::GetInstance().InitVRSystem((vr::IVRSystem*)realInterface);
		return (void*)(new HookVRSystem((vr::IVRSystem*)realInterface));
	}
	return nullptr;
}

std::string HookVRSystemFactory::GetWrappedVersion()
{
	return std::string(PapyrusVR_IVRSystem_Version);
}

HookVRSystem::HookVRSystem(vr::IVRSystem* wrappedSystem) : vr_system(wrappedSystem) { }


void HookVRSystem::GetRecommendedRenderTargetSize(uint32_t * pnWidth, uint32_t * pnHeight)
{
	vr_system->GetRecommendedRenderTargetSize(pnWidth, pnHeight);
}

HmdMatrix44_t HookVRSystem::GetProjectionMatrix(EVREye eEye, float fNearZ, float fFarZ)
{
	return vr_system->GetProjectionMatrix(eEye, fNearZ, fFarZ);
}

void HookVRSystem::GetProjectionRaw(EVREye eEye, float * pfLeft, float * pfRight, float * pfTop, float * pfBottom)
{
	vr_system->GetProjectionRaw(eEye, pfLeft, pfRight, pfTop, pfBottom);
}

bool HookVRSystem::ComputeDistortion(EVREye eEye, float fU, float fV, DistortionCoordinates_t * pDistortionCoordinates)
{
	return vr_system->ComputeDistortion(eEye, fU, fV, pDistortionCoordinates);
}

HmdMatrix34_t HookVRSystem::GetEyeToHeadTransform(EVREye eEye)
{
	return vr_system->GetEyeToHeadTransform(eEye);
}

bool HookVRSystem::GetTimeSinceLastVsync(float *pfSecondsSinceLastVsync, uint64_t *pulFrameCounter)
{
	return vr_system->GetTimeSinceLastVsync(pfSecondsSinceLastVsync, pulFrameCounter);
}

int32_t HookVRSystem::GetD3D9AdapterIndex()
{
	return vr_system->GetD3D9AdapterIndex();
}

void HookVRSystem::GetDXGIOutputInfo(int32_t * pnAdapterIndex)
{
	vr_system->GetDXGIOutputInfo(pnAdapterIndex);
}

void HookVRSystem::GetOutputDevice(uint64_t * pnDevice, ETextureType textureType, VkInstance_T * pInstance)
{
	vr_system->GetOutputDevice(pnDevice, textureType, pInstance);
}

bool HookVRSystem::IsDisplayOnDesktop()
{
	return vr_system->IsDisplayOnDesktop();
}

bool HookVRSystem::SetDisplayVisibility(bool bIsVisibleOnDesktop)
{
	return vr_system->SetDisplayVisibility(bIsVisibleOnDesktop);
}

void HookVRSystem::GetDeviceToAbsoluteTrackingPose(ETrackingUniverseOrigin eOrigin, float fPredictedSecondsToPhotonsFromNow, VR_ARRAY_COUNT(unTrackedDevicePoseArrayCount)TrackedDevicePose_t * pTrackedDevicePoseArray, uint32_t unTrackedDevicePoseArrayCount)
{
	vr_system->GetDeviceToAbsoluteTrackingPose(eOrigin, fPredictedSecondsToPhotonsFromNow, pTrackedDevicePoseArray, unTrackedDevicePoseArrayCount);
}

void HookVRSystem::ResetSeatedZeroPose()
{
	vr_system->ResetSeatedZeroPose();
}

HmdMatrix34_t HookVRSystem::GetSeatedZeroPoseToStandingAbsoluteTrackingPose()
{
	return vr_system->GetSeatedZeroPoseToStandingAbsoluteTrackingPose();
}

HmdMatrix34_t HookVRSystem::GetRawZeroPoseToStandingAbsoluteTrackingPose()
{
	return vr_system->GetRawZeroPoseToStandingAbsoluteTrackingPose();
}

uint32_t HookVRSystem::GetSortedTrackedDeviceIndicesOfClass(ETrackedDeviceClass eTrackedDeviceClass, VR_ARRAY_COUNT(unTrackedDeviceIndexArrayCount)vr::TrackedDeviceIndex_t * punTrackedDeviceIndexArray, uint32_t unTrackedDeviceIndexArrayCount, vr::TrackedDeviceIndex_t unRelativeToTrackedDeviceIndex)
{
	return vr_system->GetSortedTrackedDeviceIndicesOfClass(eTrackedDeviceClass, punTrackedDeviceIndexArray, unTrackedDeviceIndexArrayCount, unRelativeToTrackedDeviceIndex);
}

EDeviceActivityLevel HookVRSystem::GetTrackedDeviceActivityLevel(vr::TrackedDeviceIndex_t unDeviceId)
{
	return vr_system->GetTrackedDeviceActivityLevel(unDeviceId);
}

void HookVRSystem::ApplyTransform(TrackedDevicePose_t * pOutputPose, const TrackedDevicePose_t * pTrackedDevicePose, const HmdMatrix34_t * pTransform)
{
	vr_system->ApplyTransform(pOutputPose, pTrackedDevicePose, pTransform);
}

vr::TrackedDeviceIndex_t HookVRSystem::GetTrackedDeviceIndexForControllerRole(vr::ETrackedControllerRole unDeviceType)
{
	return vr_system->GetTrackedDeviceIndexForControllerRole(unDeviceType);
}

vr::ETrackedControllerRole HookVRSystem::GetControllerRoleForTrackedDeviceIndex(vr::TrackedDeviceIndex_t unDeviceIndex)
{
	return vr_system->GetControllerRoleForTrackedDeviceIndex(unDeviceIndex);
}

ETrackedDeviceClass HookVRSystem::GetTrackedDeviceClass(vr::TrackedDeviceIndex_t unDeviceIndex)
{
	return vr_system->GetTrackedDeviceClass(unDeviceIndex);
}

bool HookVRSystem::IsTrackedDeviceConnected(vr::TrackedDeviceIndex_t unDeviceIndex)
{
	return vr_system->IsTrackedDeviceConnected(unDeviceIndex);
}

bool HookVRSystem::GetBoolTrackedDeviceProperty(vr::TrackedDeviceIndex_t unDeviceIndex, ETrackedDeviceProperty prop, ETrackedPropertyError * pError)
{
	return vr_system->GetBoolTrackedDeviceProperty(unDeviceIndex, prop, pError);
}

float HookVRSystem::GetFloatTrackedDeviceProperty(vr::TrackedDeviceIndex_t unDeviceIndex, ETrackedDeviceProperty prop, ETrackedPropertyError * pError)
{
	return vr_system->GetFloatTrackedDeviceProperty(unDeviceIndex, prop, pError);
}

int32_t HookVRSystem::GetInt32TrackedDeviceProperty(vr::TrackedDeviceIndex_t unDeviceIndex, ETrackedDeviceProperty prop, ETrackedPropertyError * pError)
{
	return vr_system->GetInt32TrackedDeviceProperty(unDeviceIndex, prop, pError);
}

uint64_t HookVRSystem::GetUint64TrackedDeviceProperty(vr::TrackedDeviceIndex_t unDeviceIndex, ETrackedDeviceProperty prop, ETrackedPropertyError * pError)
{
	return vr_system->GetUint64TrackedDeviceProperty(unDeviceIndex, prop, pError);
}

HmdMatrix34_t HookVRSystem::GetMatrix34TrackedDeviceProperty(vr::TrackedDeviceIndex_t unDeviceIndex, ETrackedDeviceProperty prop, ETrackedPropertyError * pError)
{
	return vr_system->GetMatrix34TrackedDeviceProperty(unDeviceIndex, prop, pError);
}

uint32_t HookVRSystem::GetArrayTrackedDeviceProperty(vr::TrackedDeviceIndex_t unDeviceIndex, ETrackedDeviceProperty prop, PropertyTypeTag_t propType, void * pBuffer, uint32_t unBufferSize, ETrackedPropertyError * pError)
{
	return vr_system->GetArrayTrackedDeviceProperty(unDeviceIndex, prop, propType, pBuffer, unBufferSize, pError);
}

uint32_t HookVRSystem::GetStringTrackedDeviceProperty(vr::TrackedDeviceIndex_t unDeviceIndex, ETrackedDeviceProperty prop, VR_OUT_STRING() char * pchValue, uint32_t unBufferSize, ETrackedPropertyError * pError)
{
	return vr_system->GetStringTrackedDeviceProperty(unDeviceIndex, prop, pchValue, unBufferSize, pError);
}

const char * HookVRSystem::GetPropErrorNameFromEnum(ETrackedPropertyError error)
{
	return vr_system->GetPropErrorNameFromEnum(error);
}

bool HookVRSystem::PollNextEvent(VREvent_t * pEvent, uint32_t uncbVREvent)
{
	return vr_system->PollNextEvent(pEvent, uncbVREvent);
}

bool HookVRSystem::PollNextEventWithPose(ETrackingUniverseOrigin eOrigin, VREvent_t * pEvent, uint32_t uncbVREvent, vr::TrackedDevicePose_t * pTrackedDevicePose)
{
	return vr_system->PollNextEventWithPose(eOrigin, pEvent, uncbVREvent, pTrackedDevicePose);
}

const char * HookVRSystem::GetEventTypeNameFromEnum(EVREventType eType)
{
	return vr_system->GetEventTypeNameFromEnum(eType);
}

HiddenAreaMesh_t HookVRSystem::GetHiddenAreaMesh(EVREye eEye, EHiddenAreaMeshType type)
{
	return vr_system->GetHiddenAreaMesh(eEye, type);
}

bool HookVRSystem::GetControllerState(vr::TrackedDeviceIndex_t unControllerDeviceIndex, vr::VRControllerState_t * pControllerState, uint32_t unControllerStateSize)
{
	return vr_system->GetControllerState(unControllerDeviceIndex, pControllerState, unControllerStateSize);
}

bool HookVRSystem::GetControllerStateWithPose(ETrackingUniverseOrigin eOrigin, vr::TrackedDeviceIndex_t unControllerDeviceIndex, vr::VRControllerState_t * pControllerState, uint32_t unControllerStateSize, TrackedDevicePose_t * pTrackedDevicePose)
{
	return vr_system->GetControllerStateWithPose(eOrigin, unControllerDeviceIndex, pControllerState, unControllerStateSize, pTrackedDevicePose);
}

void HookVRSystem::TriggerHapticPulse(vr::TrackedDeviceIndex_t unControllerDeviceIndex, uint32_t unAxisId, unsigned short usDurationMicroSec)
{
	vr_system->TriggerHapticPulse(unControllerDeviceIndex, unAxisId, usDurationMicroSec);

	//Usefull for haptic suits mods
	VRManager::GetInstance().ProcessHapticEvents(unControllerDeviceIndex, unAxisId, usDurationMicroSec);
}

const char * HookVRSystem::GetButtonIdNameFromEnum(vr::EVRButtonId eButtonId)
{
	return vr_system->GetButtonIdNameFromEnum(eButtonId);
}

const char * HookVRSystem::GetControllerAxisTypeNameFromEnum(EVRControllerAxisType eAxisType)
{
	return vr_system->GetControllerAxisTypeNameFromEnum(eAxisType);
}

bool HookVRSystem::IsInputAvailable()
{
	return vr_system->IsInputAvailable();
}

bool HookVRSystem::IsSteamVRDrawingControllers()
{
	return vr_system->IsSteamVRDrawingControllers();
}

bool HookVRSystem::ShouldApplicationPause()
{
	return vr_system->ShouldApplicationPause();
}

bool HookVRSystem::ShouldApplicationReduceRenderingWork()
{
	return vr_system->ShouldApplicationReduceRenderingWork();
}

uint32_t HookVRSystem::DriverDebugRequest(vr::TrackedDeviceIndex_t unDeviceIndex, const char * pchRequest, VR_OUT_STRING() char * pchResponseBuffer, uint32_t unResponseBufferSize)
{
	return vr_system->DriverDebugRequest(unDeviceIndex, pchRequest, pchResponseBuffer, unResponseBufferSize);
}

vr::EVRFirmwareError HookVRSystem::PerformFirmwareUpdate(vr::TrackedDeviceIndex_t unDeviceIndex)
{
	return vr_system->PerformFirmwareUpdate(unDeviceIndex);
}

void HookVRSystem::AcknowledgeQuit_Exiting()
{
	vr_system->AcknowledgeQuit_Exiting();
}

void HookVRSystem::AcknowledgeQuit_UserPrompt()
{
	vr_system->AcknowledgeQuit_UserPrompt();
}
