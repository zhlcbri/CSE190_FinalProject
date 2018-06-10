#pragma once

#include "Online.h"
#include "GameFramework/Pawn.h"
#include "OvrAvatar.h"
#include "LocalAvatar.generated.h"

UCLASS()
class AVATARSAMPLES_API ALocalAvatar : public APawn
{
	GENERATED_BODY()
public:
	ALocalAvatar();

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;

	void OnLoginComplete(int32 LocalUserNum, bool bWasSuccessful, const FUniqueNetId& UserId, const FString& Error);
	void SetupPlayerInputComponent(UInputComponent* Input) override;

private:
	void UpdatePacketRecording(float DeltaTime);

	void DriveHand(ovrHandType hand, float DeltaTime);
	void ToggleRecordPackets();
	void SpawnNewRemoteAvatar();
	void DestroyRemoteAvatar();
	void CycleRightHandPose();
	void CycleLeftHandPose();
	void CycleRightHandAttach();
	void CycleLeftHandAttach();

	void LeftThumbstickX_Value(float value);
	void LeftThumbstickY_Value(float value);
	void RightThumbstickX_Value(float value);
	void RightThumbstickY_Value(float value);

#define INPUT_ENTRY(entry, hand, flag) \
	void entry##_Pressed();\
	void entry##_Released();
	INPUT_COMMAND_TUPLE
#undef INPUT_ENTRY

#define AXIS_ENTRY(entry, hand, flag) \
	void entry##_Value( float value);
		AXIS_INPUT_TUPLE
#undef AXIS_ENTRY

#define CUSTOM_ENTRY(entry, hand, field, invert) \
	void entry##_Value( float value);
		CUSTOM_AXIS_TUPLE
#undef CUSTOM_ENTRY


	FDelegateHandle OnLoginCompleteDelegateHandle;

	enum class eHandPoseState
	{
		Default,
		Sphere,
		Cube,
		Custom,
		Controller,
		Detached,
	};

	eHandPoseState LeftHandPoseIndex = eHandPoseState::Default;
	eHandPoseState RightHandPoseIndex = eHandPoseState::Default;

	UOvrAvatar* AvatarComponent = nullptr;

	struct FPacketRecordSettings
	{
		bool Initialized = false;
		bool RecordingFrames = false;
		float UpdateRate = 1.0 / 5.f;  // Lower rate = bigger packets. Try to optimize against fidelity vs Network Overhead.
		float AccumulatedTime = 0.f;
	};

	TWeakObjectPtr<USceneComponent> AvatarHands[ovrHand_Count];
	FPacketRecordSettings PacketSettings;

	FVector2D StickPosition[ovrHand_Count];
};
