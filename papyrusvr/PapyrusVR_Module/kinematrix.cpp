#include "Kinematrix.h"
#include "GameAPI.h"
#include "GameTypes.h"
#include "InternalTasks.h"
#include "GameReferences.h"
#include "NiTypes.h"
#include "NiNodes.h"
#include "VRManager.h"
#include "api/utils/OpenVRUtils.h"
#define DEBUG_CONVERSION false

using namespace PapyrusVR;

namespace Kinematrix
{
	static BSFixedString NPC_Right_Hand("NPC R Hand [RHnd]");
	static BSFixedString RoomNode("RoomNode");
	static BSFixedString RightWandNode("RightWandNode");
	static BSFixedString NPC_LBody("CME LBody [LBody]");
	static BSFixedString NPC_LFoot("CME L Foot [Lft ]");
	static BSFixedString NPC_RFoot("CME R Foot [Rft ]");
	static NiAVObject* rootNode = nullptr;
	static NiNode* rightWandNode = nullptr;
	static NiNode* rootNodeTP = nullptr;
	static NiNode* rootNodeFP = nullptr;
	static NiNode* roomNode = nullptr;
	static NiNode* leftFoot = nullptr;
	static NiNode* rightFoot = nullptr;
	static NiNode* rightHandNode = nullptr;

	static PapyrusVR::Matrix34 roomMatrix;
	static PapyrusVR::Matrix34 rightMatrix;
	static NiTransform localRightTransform;
	static NiTransform worldRightTransform;
	static NiTransform worldParentInvRightTransform;
	static PapyrusVR::Matrix34 leftMatrix;
	static NiTransform localLeftTransform;
	static NiTransform worldLeftTransform;
	static NiTransform worldParentInvLeftTransform;

	#if DEBUG_CONVERSION
	static std::mutex debugMutex;
	#endif

	void GameLoaded()
	{
		_MESSAGE("Getting root node");
		rootNode = (*g_thePlayer)->loadedState->node->m_parent;
		if (!rootNode)
			_MESSAGE("Error while getting root node");
		else
			_MESSAGE("Root node scale %f", rootNode->m_worldTransform.scale);

		rootNodeTP = (*g_thePlayer)->GetNiRootNode(0);
		if (!rootNodeTP)
			_MESSAGE("Error while getting rootNodeTP node");

		rootNodeFP = (*g_thePlayer)->GetNiRootNode(1);
		if (!rootNodeFP)
			_MESSAGE("Error while getting rootNodeFP node");

		NiNode* mostInterestingRoot = (rootNodeFP != nullptr) ? rootNodeFP : rootNodeTP;
		if (mostInterestingRoot == rootNodeFP)
			_MESSAGE("First person node existing....");
		else
			_MESSAGE("No first person node, falling back to third person....");

		_MESSAGE("Getting hand node");
		rightHandNode = ni_cast(mostInterestingRoot->GetObjectByName(&NPC_Right_Hand.data), NiNode);

		if (!rightHandNode)
			_MESSAGE("Error while getting hand node");

		_MESSAGE("Getting RoomNode node");
		roomNode = ni_cast(rootNode->GetObjectByName(&RoomNode.data), NiNode);
		if (!roomNode)
			_MESSAGE("Error while getting RoomNode node");
		else
			_MESSAGE("Room Node scale %f", roomNode->m_worldTransform.scale);

		_MESSAGE("Getting Right Wand node");
		rightWandNode = ni_cast(rootNode->GetObjectByName(&RightWandNode.data), NiNode);
		if (!rightWandNode)
			_MESSAGE("Error while getting RightWandNode node");
		else
			_MESSAGE("RightWandNode scale %f", rightWandNode->m_worldTransform.scale);

		leftFoot = ni_cast(mostInterestingRoot->GetObjectByName(&NPC_LFoot.data), NiNode);
		if (!leftFoot)
			_MESSAGE("Error while getting NPC_LFoot node");
		else
			_MESSAGE("NPC_LFoot scale %f", leftFoot->m_worldTransform.scale);

		rightFoot = ni_cast(mostInterestingRoot->GetObjectByName(&NPC_RFoot.data), NiNode);
		if (!rightFoot)
			_MESSAGE("Error while getting NPC_RFoot node");
		else
			_MESSAGE("NPC_RFoot scale %f", rightFoot->m_worldTransform.scale);

		VRManager::GetInstance().RegisterVRUpdateListener(OnPoseUpdate);
		
		#if DEBUG_CONVERSION
		VRManager::GetInstance().RegisterVRButtonListener(OnVRButtonEvent);
		#endif

		localRightTransform.scale = 1;
		localLeftTransform.scale = 1;
	}

	void OnVRButtonEvent(VREventType eventType, EVRButtonId buttonID, VRDevice device)
	{
		#if DEBUG_CONVERSION
		if (eventType == VREventType::VREventType_Pressed && device == VRDevice::VRDevice_RightController && buttonID == EVRButtonId::k_EButton_SteamVR_Trigger)
		{
			std::lock_guard<std::mutex> lock(debugMutex);

			_MESSAGE("[DEBUG] -------------............---------------");
			_MESSAGE("[DEBUG] Debug information on matrixes");
			_MESSAGE("[DEBUG] ------------- Local Converted ---------------");
			_MESSAGE("[DEBUG] [ %f \t %f \t %f \t %f ]", rightMatrix.m[0][0], rightMatrix.m[0][1], rightMatrix.m[0][2], rightMatrix.m[0][3]);
			_MESSAGE("[DEBUG] [ %f \t %f \t %f \t %f ]", rightMatrix.m[1][0], rightMatrix.m[1][1], rightMatrix.m[1][2], rightMatrix.m[1][3]);
			_MESSAGE("[DEBUG] [ %f \t %f \t %f \t %f ]", rightMatrix.m[2][0], rightMatrix.m[2][1], rightMatrix.m[2][2], rightMatrix.m[2][3]);
			_MESSAGE("[DEBUG] Scale: %f", 1.0f);
			_MESSAGE("[DEBUG] ------------- Local Game WND ---------------");
			_MESSAGE("[DEBUG] [ %f \t %f \t %f \t %f ]", rightWandNode->m_localTransform.rot.data[0][0], rightWandNode->m_localTransform.rot.data[0][1], rightWandNode->m_localTransform.rot.data[0][2], rightWandNode->m_localTransform.pos.x);
			_MESSAGE("[DEBUG] [ %f \t %f \t %f \t %f ]", rightWandNode->m_localTransform.rot.data[1][0], rightWandNode->m_localTransform.rot.data[1][1], rightWandNode->m_localTransform.rot.data[1][2], rightWandNode->m_localTransform.pos.y);
			_MESSAGE("[DEBUG] [ %f \t %f \t %f \t %f ]", rightWandNode->m_localTransform.rot.data[2][0], rightWandNode->m_localTransform.rot.data[2][1], rightWandNode->m_localTransform.rot.data[2][2], rightWandNode->m_localTransform.pos.z);
			_MESSAGE("[DEBUG] Scale: %f", rightWandNode->m_localTransform.scale);
			_MESSAGE("[DEBUG] -------------............---------------");
			_MESSAGE("[DEBUG] -------------............---------------");
			_MESSAGE("[DEBUG] ------------- World Converted ---------------");
			_MESSAGE("[DEBUG] [ %f \t %f \t %f \t %f ]", worldRightTransform.rot.data[0][0], worldRightTransform.rot.data[0][1], worldRightTransform.rot.data[0][2], worldRightTransform.pos.x);
			_MESSAGE("[DEBUG] [ %f \t %f \t %f \t %f ]", worldRightTransform.rot.data[1][0], worldRightTransform.rot.data[1][1], worldRightTransform.rot.data[1][2], worldRightTransform.pos.y);
			_MESSAGE("[DEBUG] [ %f \t %f \t %f \t %f ]", worldRightTransform.rot.data[2][0], worldRightTransform.rot.data[2][1], worldRightTransform.rot.data[2][2], worldRightTransform.pos.z);
			_MESSAGE("[DEBUG] Scale: %f", worldRightTransform.scale);
			_MESSAGE("[DEBUG] ------------- World Game WND ---------------");
			_MESSAGE("[DEBUG] [ %f \t %f \t %f \t %f ]", rightWandNode->m_worldTransform.rot.data[0][0], rightWandNode->m_worldTransform.rot.data[0][1], rightWandNode->m_worldTransform.rot.data[0][2], rightWandNode->m_worldTransform.pos.x);
			_MESSAGE("[DEBUG] [ %f \t %f \t %f \t %f ]", rightWandNode->m_worldTransform.rot.data[1][0], rightWandNode->m_worldTransform.rot.data[1][1], rightWandNode->m_worldTransform.rot.data[1][2], rightWandNode->m_worldTransform.pos.y);
			_MESSAGE("[DEBUG] [ %f \t %f \t %f \t %f ]", rightWandNode->m_worldTransform.rot.data[2][0], rightWandNode->m_worldTransform.rot.data[2][1], rightWandNode->m_worldTransform.rot.data[2][2], rightWandNode->m_worldTransform.pos.z);
			_MESSAGE("[DEBUG] Scale: %f", rightWandNode->m_worldTransform.scale);
			_MESSAGE("[DEBUG] -------------............---------------");
		}
		#endif
	}

	void OnPoseUpdate(float deltaTime)
	{
		#if DEBUG_CONVERSION
		//If debugging is enabled, lock matrixes
		std::lock_guard<std::mutex> lock(debugMutex);
		#endif

		TrackedDevicePose* leftHandSVRPose = VRManager::GetInstance().GetLeftHandPose();
		TrackedDevicePose* rightHandSVRPose = VRManager::GetInstance().GetRightHandPose();
		rightMatrix = rightHandSVRPose->mDeviceToAbsoluteTracking;
		leftMatrix = leftHandSVRPose->mDeviceToAbsoluteTracking;

		//Do Offset conversions
		OpenVRUtils::SteamVRTransformToSkyrimTransform(&rightMatrix);
		OpenVRUtils::SteamVRTransformToSkyrimTransform(&leftMatrix);

		if (rightFoot != nullptr)
		{
			OpenVRUtils::CopyMatrix34ToNiTrasform(&rightMatrix, &localRightTransform);

			worldRightTransform = roomNode->m_worldTransform * localRightTransform;

			rightFoot->m_parent->m_worldTransform.Invert(worldParentInvRightTransform);
			rightFoot->m_localTransform = worldParentInvRightTransform * worldRightTransform;
			TaskInterface::UpdateWorldData(rightFoot);
		}

		if (leftFoot != nullptr)
		{
			OpenVRUtils::CopyMatrix34ToNiTrasform(&leftMatrix, &localLeftTransform);

			worldLeftTransform = roomNode->m_worldTransform * localLeftTransform;

			//leftFoot->m_worldTransform = worldLeftTransform;
			//We need to alter the local transform or skyrim will ignore the change
			leftFoot->m_parent->m_worldTransform.Invert(worldParentInvLeftTransform);
			leftFoot->m_localTransform = worldParentInvLeftTransform * worldLeftTransform;
			TaskInterface::UpdateWorldData(leftFoot);
		}
	}
}
