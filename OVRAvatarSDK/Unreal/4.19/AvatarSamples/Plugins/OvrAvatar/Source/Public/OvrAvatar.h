#pragma once

#include "GameFramework/Pawn.h"
#include "OVR_Avatar.h"
#include "Set.h"
#include "OVR_Microphone.h"
#include "Components/ActorComponent.h"
#include "OVR_Plugin.h"
#include "OVR_Plugin_Types.h"


#include "OvrAvatar.generated.h"

class UPoseableMeshComponent;

UCLASS()
class OVRAVATAR_API UOvrAvatar : public UActorComponent
{
	GENERATED_BODY()

public:
	enum class ePlayerType
	{
		Local,
		Remote
	};

	enum HandType
	{
		HandType_Left,
		HandType_Right,
		HandType_Count
	};

	UOvrAvatar();

	virtual void BeginPlay() override;
	virtual void BeginDestroy() override;

	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction) override;

	void RequestAvatar(uint64_t userID);


	void HandleAvatarSpecification(const ovrAvatarMessage_AvatarSpecification* message);
	void HandleAssetLoaded(const ovrAvatarMessage_AssetLoaded* message);

	void SetVisibilityType(ovrAvatarVisibilityFlags flag) { VisibilityMask = flag; }
	void SetPlayerType(ePlayerType type) { PlayerType = type; }
	void SetPlayerHeightOffset(float offset) { PlayerHeightOffset = offset; }

	USceneComponent* DetachHand(HandType hand);
	void ReAttachHand(HandType hand);

	void SetRightHandPose(ovrAvatarHandGesture pose);
	void SetLeftHandPose(ovrAvatarHandGesture pose);
	void SetCustomGesture(HandType hand, ovrAvatarTransform* joints, uint32_t numJoints);
	void SetControllerVisibility(HandType hand, bool visible);

	void StartPacketRecording();
	ovrAvatarPacket* EndPacketRecording();
	void UpdateFromPacket(ovrAvatarPacket* packet, const float time);

protected:
	static FString HandNames[HandType_Count];

	void InitializeMaterials();

	void UpdateTransforms(float DeltaTime);

	void DebugDrawSceneComponents();

	void AddMeshComponent(ovrAvatarAssetID id, UPoseableMeshComponent* mesh);
	void AddDepthMeshComponent(ovrAvatarAssetID id, UPoseableMeshComponent* mesh);

	UPoseableMeshComponent* GetMeshComponent(ovrAvatarAssetID id) const;
	UPoseableMeshComponent* GetDepthMeshComponent(ovrAvatarAssetID id) const;

	UPoseableMeshComponent* CreateMeshComponent(USceneComponent* parent, ovrAvatarAssetID assetID, const FString& name);
	UPoseableMeshComponent* CreateDepthMeshComponent(USceneComponent* parent, ovrAvatarAssetID assetID, const FString& name);

	void LoadMesh(USkeletalMesh* SkeletalMesh, const ovrAvatarMeshAssetData* data);

	void UpdateSDK(float DeltaTime);
	void UpdatePostSDK();
	void UpdateVoiceData(float DeltaTime);

	void UpdateMeshComponent(USceneComponent& mesh, const ovrAvatarTransform& transform);
	void UpdateMaterial(UMeshComponent& mesh, const ovrAvatarMaterialState& material);
	void UpdateMaterialPBR(UPoseableMeshComponent& mesh, const ovrAvatarRenderPart_SkinnedMeshRenderPBS& data);
	void UpdateMaterialProjector(UPoseableMeshComponent& mesh, const ovrAvatarRenderPart_ProjectorRender& data, const USceneComponent& OvrComponent);
	void UpdateMaterialPBRV2(UPoseableMeshComponent& mesh, const ovrAvatarRenderPart_SkinnedMeshRenderPBS_V2& data);
	
	void UpdateSkeleton(UPoseableMeshComponent& mesh, const ovrAvatarSkinnedMeshPose& pose);

	void DebugLogAvatarSDKTransforms(const FString& wrapper);
	void DebugLogMaterialData(const ovrAvatarMaterialState& material, const FString& name);

	uint64_t OnlineUserID = 0;

	TSet<ovrAvatarAssetID> AssetIds;
	TMap<ovrAvatarAssetID, TWeakObjectPtr<UPoseableMeshComponent>> MeshComponents;
	TMap<ovrAvatarAssetID, TWeakObjectPtr<UPoseableMeshComponent>> DepthMeshComponents;

	ovrAvatar* Avatar = nullptr;

	TMap<FString, TWeakObjectPtr<USceneComponent>> RootAvatarComponents;

	ovrAvatarHandInputState HandInputState[HandType_Count];
	ovrAvatarTransform BodyTransform;

	bool LeftControllerVisible = false;
	bool RightControllerVisible = false;
	uint32_t VisibilityMask = ovrAvatarVisibilityFlag_ThirdPerson;

	ePlayerType PlayerType = ePlayerType::Local;
	float PlayerHeightOffset = 0.f;

	ovrMicrophoneHandle Microphone = nullptr;

	ovrAvatarAssetID ProjectorMeshID = 0;
	TWeakObjectPtr<UPoseableMeshComponent> ProjectorMeshComponent;

	TWeakObjectPtr<USceneComponent> AvatarHands[HandType_Count];

	ovrAvatarLookAndFeelVersion LookAndFeel = ovrAvatarLookAndFeelVersion_Two;
};
