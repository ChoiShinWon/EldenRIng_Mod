// Fill out your copyright notice in the Description page of Project Settings.


#include "EldenRing_Mod/AnimModifiers/AnimModifier_FlattenPelvis.h"
#include "Animation/AnimSequence.h"
#include "Animation/AnimData/IAnimationDataModel.h"
#include "Animation/AnimData/IAnimationDataController.h"

void UAnimModifier_FlattenPelvis::OnApply_Implementation(UAnimSequence* AnimationSequence)
{
	if (!AnimationSequence) return;

	const IAnimationDataModel* Model = AnimationSequence->GetDataModel();
	if (!Model) return;

	// GetBoneAnimationTracks/FindBoneTrackByName 등은 UE 5.2부터 deprecated돼서
	// IsValidBoneTrackName + GetBoneTrackTransforms 조합으로 대체
	if (!Model->IsValidBoneTrackName(PelvisBoneName))
	{
		UE_LOG(LogTemp, Error, TEXT("[FlattenPelvis] '%s' 본을 못 찾음!"), *PelvisBoneName.ToString());

		TArray<FName> AllNames;
		Model->GetBoneTrackNames(AllNames);
		for (const FName& Name : AllNames)
		{
			UE_LOG(LogTemp, Error, TEXT(" - %s"), *Name.ToString());
		}
		return;
	}

	// 해당 본의 프레임별 실제 트랜스폼(위치/회전/스케일)을 통째로 읽어옴
	TArray<FTransform> Transforms;
	Model->GetBoneTrackTransforms(PelvisBoneName, Transforms);

	// 수평(X, Y) 이동값만 0으로 밀고, 높낮이(Z)와 회전/스케일은 그대로 유지
	TArray<FVector3f> NewPosKeys;
	TArray<FQuat4f> NewRotKeys;
	TArray<FVector3f> NewScaleKeys;
	NewPosKeys.Reserve(Transforms.Num());
	NewRotKeys.Reserve(Transforms.Num());
	NewScaleKeys.Reserve(Transforms.Num());

	for (const FTransform& T : Transforms)
	{
		FVector Pos = T.GetLocation();
		Pos.X = 0.0f;
		Pos.Y = 0.0f;

		NewPosKeys.Add(FVector3f(Pos));
		NewRotKeys.Add(FQuat4f(T.GetRotation()));
		NewScaleKeys.Add(FVector3f(T.GetScale3D()));
	}

	// 수정한 키를 실제 애니메이션 데이터에 반영
	IAnimationDataController& Controller = AnimationSequence->GetController();
	Controller.OpenBracket(FText::FromString(TEXT("Flatten Pelvis Horizontal Motion")));
	Controller.SetBoneTrackKeys(PelvisBoneName, NewPosKeys, NewRotKeys, NewScaleKeys, true);
	Controller.CloseBracket();
}
