#include "HookVRCompositor.h"
#include "openvr_hook.h"
#include "../VRManager.h"

using namespace vr;
using namespace PapyrusVR;

#pragma region Factory Methods

void* HookVRCompositorFactory::Build(void * realInterface)
{
	if (realInterface != nullptr)
	{
		_MESSAGE("[OpenVR_Hook] Hooked IVRCompositor");
		VRManager::GetInstance().InitVRCompositor((IVRCompositor*)realInterface);
		OpenVRHookMgr::GetInstance()->SetVRCompositor((IVRCompositor*)realInterface);
		OpenVRHookMgr::GetInstance()->SetFakeVRCompositor(new HookVRCompositor((vr::IVRCompositor*)realInterface));
		return (void*)(OpenVRHookMgr::GetInstance()->GetFakeVRCompositor());
	}
	return nullptr;
}

std::string HookVRCompositorFactory::GetWrappedVersion()
{
	return std::string(SkyrimVR_IVRCompositor_Version);
}

#pragma endregion

#pragma region Hooked Methods

HookVRCompositor::HookVRCompositor(vr::IVRCompositor* wrappedCompositor) : vr_compositor(wrappedCompositor) { }

EVRCompositorError HookVRCompositor::WaitGetPoses(VR_ARRAY_COUNT(unRenderPoseArrayCount)TrackedDevicePose_t * pRenderPoseArray, uint32_t unRenderPoseArrayCount, 
													VR_ARRAY_COUNT(unGamePoseArrayCount)TrackedDevicePose_t * pGamePoseArray, uint32_t unGamePoseArrayCount)
{
	EVRCompositorError result = vr_compositor->WaitGetPoses(pRenderPoseArray, unRenderPoseArrayCount, pGamePoseArray, unGamePoseArrayCount);


	//Calls the VRManager for Internal Processing
	VRManager::GetInstance().UpdatePoses();

	// call all the registered callbacks for raw OpenVR events
	for (auto it = OpenVRHookMgr::GetInstance()->mWaitGetPosesCallbacks.begin(); it != OpenVRHookMgr::GetInstance()->mWaitGetPosesCallbacks.end(); ++it)
	{
		WaitGetPoses_CB cbfunc = *it;
		cbfunc(pRenderPoseArray, unRenderPoseArrayCount, pGamePoseArray, unGamePoseArrayCount);
	}

	return result;
}
#pragma endregion

#pragma region Interface Dummy Implementations
void HookVRCompositor::SetTrackingSpace(ETrackingUniverseOrigin eOrigin)
{
	vr_compositor->SetTrackingSpace(eOrigin);
}

ETrackingUniverseOrigin HookVRCompositor::GetTrackingSpace()
{
	return vr_compositor->GetTrackingSpace();
}

EVRCompositorError HookVRCompositor::GetLastPoses(VR_ARRAY_COUNT(unRenderPoseArrayCount)TrackedDevicePose_t * pRenderPoseArray, uint32_t unRenderPoseArrayCount, VR_ARRAY_COUNT(unGamePoseArrayCount)TrackedDevicePose_t * pGamePoseArray, uint32_t unGamePoseArrayCount)
{
	return vr_compositor->GetLastPoses(pRenderPoseArray, unRenderPoseArrayCount, pGamePoseArray, unGamePoseArrayCount);
}

EVRCompositorError HookVRCompositor::GetLastPoseForTrackedDeviceIndex(TrackedDeviceIndex_t unDeviceIndex, TrackedDevicePose_t * pOutputPose, TrackedDevicePose_t * pOutputGamePose)
{
	return vr_compositor->GetLastPoseForTrackedDeviceIndex(unDeviceIndex, pOutputPose, pOutputGamePose);
}

EVRCompositorError HookVRCompositor::Submit(EVREye eEye, const Texture_t * pTexture, const VRTextureBounds_t * pBounds, EVRSubmitFlags nSubmitFlags)
{
	return vr_compositor->Submit(eEye, pTexture, pBounds, nSubmitFlags);
}

void HookVRCompositor::ClearLastSubmittedFrame()
{
	vr_compositor->ClearLastSubmittedFrame();
}

void HookVRCompositor::PostPresentHandoff()
{
	vr_compositor->PostPresentHandoff();
}

bool HookVRCompositor::GetFrameTiming(Compositor_FrameTiming * pTiming, uint32_t unFramesAgo)
{
	return vr_compositor->GetFrameTiming(pTiming, unFramesAgo);
}

uint32_t HookVRCompositor::GetFrameTimings(Compositor_FrameTiming * pTiming, uint32_t nFrames)
{
	return vr_compositor->GetFrameTimings(pTiming, nFrames);
}

float HookVRCompositor::GetFrameTimeRemaining()
{
	return vr_compositor->GetFrameTimeRemaining();
}

void HookVRCompositor::GetCumulativeStats(Compositor_CumulativeStats * pStats, uint32_t nStatsSizeInBytes)
{
	vr_compositor->GetCumulativeStats(pStats, nStatsSizeInBytes);
}

void HookVRCompositor::FadeToColor(float fSeconds, float fRed, float fGreen, float fBlue, float fAlpha, bool bBackground)
{
	vr_compositor->FadeToColor(fSeconds, fRed, fGreen, fBlue, fAlpha, bBackground);
}

HmdColor_t HookVRCompositor::GetCurrentFadeColor(bool bBackground)
{
	return vr_compositor->GetCurrentFadeColor(bBackground);
}

void HookVRCompositor::FadeGrid(float fSeconds, bool bFadeIn)
{
	vr_compositor->FadeGrid(fSeconds, bFadeIn);
}

float HookVRCompositor::GetCurrentGridAlpha()
{
	return vr_compositor->GetCurrentGridAlpha();
}

EVRCompositorError HookVRCompositor::SetSkyboxOverride(VR_ARRAY_COUNT(unTextureCount) const Texture_t * pTextures, uint32_t unTextureCount)
{
	return vr_compositor->SetSkyboxOverride(pTextures, unTextureCount);
}

void HookVRCompositor::ClearSkyboxOverride()
{
	vr_compositor->ClearSkyboxOverride();
}

void HookVRCompositor::CompositorBringToFront()
{
	vr_compositor->CompositorBringToFront();
}

void HookVRCompositor::CompositorGoToBack()
{
	vr_compositor->CompositorGoToBack();
}

void HookVRCompositor::CompositorQuit()
{
	vr_compositor->CompositorQuit();
}

bool HookVRCompositor::IsFullscreen()
{
	return vr_compositor->IsFullscreen();
}

uint32_t HookVRCompositor::GetCurrentSceneFocusProcess()
{
	return vr_compositor->GetCurrentSceneFocusProcess();
}

uint32_t HookVRCompositor::GetLastFrameRenderer()
{
	return vr_compositor->GetLastFrameRenderer();
}

bool HookVRCompositor::CanRenderScene()
{
	return vr_compositor->CanRenderScene();
}

void HookVRCompositor::ShowMirrorWindow()
{
	vr_compositor->ShowMirrorWindow();
}

void HookVRCompositor::HideMirrorWindow()
{
	vr_compositor->HideMirrorWindow();
}

bool HookVRCompositor::IsMirrorWindowVisible()
{
	return vr_compositor->IsMirrorWindowVisible();
}

void HookVRCompositor::CompositorDumpImages()
{
	vr_compositor->CompositorDumpImages();
}

bool HookVRCompositor::ShouldAppRenderWithLowResources()
{
	return vr_compositor->ShouldAppRenderWithLowResources();
}

void HookVRCompositor::ForceInterleavedReprojectionOn(bool bOverride)
{
	vr_compositor->ForceInterleavedReprojectionOn(bOverride);
}

void HookVRCompositor::ForceReconnectProcess()
{
	vr_compositor->ForceReconnectProcess();
}

void HookVRCompositor::SuspendRendering(bool bSuspend)
{
	vr_compositor->SuspendRendering(bSuspend);
}

vr::EVRCompositorError HookVRCompositor::GetMirrorTextureD3D11(vr::EVREye eEye, void * pD3D11DeviceOrResource, void ** ppD3D11ShaderResourceView)
{
	return vr_compositor->GetMirrorTextureD3D11(eEye, pD3D11DeviceOrResource, ppD3D11ShaderResourceView);
}

void HookVRCompositor::ReleaseMirrorTextureD3D11(void * pD3D11ShaderResourceView)
{
	vr_compositor->ReleaseMirrorTextureD3D11(pD3D11ShaderResourceView);
}

vr::EVRCompositorError HookVRCompositor::GetMirrorTextureGL(vr::EVREye eEye, vr::glUInt_t * pglTextureId, vr::glSharedTextureHandle_t * pglSharedTextureHandle)
{
	return vr_compositor->GetMirrorTextureGL(eEye, pglTextureId, pglSharedTextureHandle);
}

bool HookVRCompositor::ReleaseSharedGLTexture(vr::glUInt_t glTextureId, vr::glSharedTextureHandle_t glSharedTextureHandle)
{
	return vr_compositor->ReleaseSharedGLTexture(glTextureId, glSharedTextureHandle);
}

void HookVRCompositor::LockGLSharedTextureForAccess(vr::glSharedTextureHandle_t glSharedTextureHandle)
{
	vr_compositor->LockGLSharedTextureForAccess(glSharedTextureHandle);
}

void HookVRCompositor::UnlockGLSharedTextureForAccess(vr::glSharedTextureHandle_t glSharedTextureHandle)
{
	vr_compositor->UnlockGLSharedTextureForAccess(glSharedTextureHandle);
}

uint32_t HookVRCompositor::GetVulkanInstanceExtensionsRequired(VR_OUT_STRING() char * pchValue, uint32_t unBufferSize)
{
	return vr_compositor->GetVulkanInstanceExtensionsRequired(pchValue, unBufferSize);
}

uint32_t HookVRCompositor::GetVulkanDeviceExtensionsRequired(VkPhysicalDevice_T * pPhysicalDevice, VR_OUT_STRING() char * pchValue, uint32_t unBufferSize)
{
	return vr_compositor->GetVulkanDeviceExtensionsRequired(pPhysicalDevice, pchValue, unBufferSize);
}

void HookVRCompositor::SetExplicitTimingMode(EVRCompositorTimingMode eTimingMode)
{
	vr_compositor->SetExplicitTimingMode(eTimingMode);
}

EVRCompositorError HookVRCompositor::SubmitExplicitTimingData()
{
	return vr_compositor->SubmitExplicitTimingData();
}

#pragma endregion