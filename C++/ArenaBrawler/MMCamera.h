// Copyright 2019 Team Switcheroos, All Rights Reserved.
 
#pragma once
 
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Engine/Classes/Curves/CurveFloat.h"
#include "MMCamera.generated.h"
 
 
DECLARE_LOG_CATEGORY_CLASS(LogMMCamera, Log, All);
 
 
class APlayerController;
class AMMCharacter;
 
 
/**
 *  CameraModeStatic\n
 *  CameraModeDynamic\n
 */
UENUM(BlueprintType, Category="MuscleMagic|MMCamera")
enum class ECameraMode : uint8
{
	CameraModeStatic    UMETA(DisplayName="Static Camera"),
	CameraModeDynamic   UMETA(DisplayName="Dynamic Camera"),
	CameraModeIntro		UMETA(DisplayName="Intro Camera")
};
 
 
/**
 *  An actor that moves a camera component. Must be placed in the world.
 */
UCLASS()
class MMCORE_API AMMCameraActor : public AActor
{
	GENERATED_BODY()
	
	// Camera boom, positioning the camera.
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="MuscleMagic|MMCamera", meta=(AllowPrivateAccess="true"))
	class USpringArmComponent* CameraBoom;
 
	// Follow camera.
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="MuscleMagic|MMCamera", meta=(AllowPrivateAccess="true"))
	class UCameraComponent* FollowCamera;
 
public:
 
	AMMCameraActor();
 
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
 
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }
 
public:
 
	// Cameras current operation mode.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="MuscleMagic|MMCamera")
	ECameraMode CameraMode;
 
	// Determines the multiplier for the zoom buffer based on the percentage of zoom between the theoretical min/max zoom. */
	UPROPERTY(EditAnywhere, BlueprintReadonly, Category="MuscleMagic|MMCamera")
	UCurveFloat* ZoomCurve = nullptr;
 
	// Base value for the extra zoom added to the camera boom to keep all players on screen at once.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="MuscleMagic|MMCamera")
	float Buffer;
 
	// Buffer modifier for 2 players.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="MuscleMagic|MMCamera")
	float BufferModifier2Players;
 
	// Buffer modifier for 3 players.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="MuscleMagic|MMCamera")
	float BufferModifier3Players;
 
	// Buffer modifier for 4 players.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="MuscleMagic|MMCamera")
	float BufferModifier4Players;
 
	// Percentage value used in lerping from one zoom level to another (zoom speed).
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="MuscleMagic|MMCamera")
	float ZoomLerpAmount;
 
	// Percentage value used in lerping the movement in the X direction depending on player distance.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="MuscleMagic|MMCamera")
	float MovementLerpAmount;
 
	// Amount of movement lerp for winner camera (1 player).
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="MuscleMagic|MMCamera")
	float WinnerLerpAmount;
 
	// Distance the camera lerps to away from group to allow view of solo players.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="MuscleMagic|MMCamera")
	float NegativeXBuffer;
 
	// Maximum zoom level for the camera. The maximum target arm length.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="MuscleMagic|MMCamera")
	float MaxZoom;
 
	// Minimum zoom level for the camera. The minimum target arm length.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="MuscleMagic|MMCamera")
	float MinZoom;

	// Z height to stop considering players for camera
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MuscleMagic|MMCamera")
	float ZIgnoreThreshold;
 
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="MuscleMagic|MMCamera Debug")
	bool bDrawCameraMidpoint;
 
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="MuscleMagic|MMCamera Debug")
	bool bDrawPlayerLines;
 
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="MuscleMagic|MMCamera Debug")
	bool bDrawPlayerXYLimits;
 
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="MuscleMagic|MMCamera Debug")
	bool bPrintDebug;
 
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="MuscleMagic|MMCamera Debug")
	bool bConsiderAllPlayers;
 
protected:
 
    // ~ Begin AActor interface.
 
	void Tick(float DeltaTime) override;
 
	void BeginPlay() override;
 
    // ~ End AActor interface.
 
	UFUNCTION(BlueprintCallable)
	void SetBufferModifier();
 
	UFUNCTION(BlueprintCallable)
	void FindPlayerSquare();
 
	UFUNCTION(BlueprintCallable)
	void WinnerCamera();
 
	UFUNCTION(BlueprintCallable)
	void MidpointCamera();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void IntroCamera();
 
	UFUNCTION(BlueprintCallable)
	void MatchStart();
 
	UFUNCTION(BlueprintCallable)
	void CheckValidPlayers();
 
	UFUNCTION(BlueprintCallable)
	void GetAllPlayers();
 
protected:
 
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="MuscleMagic|MMCamera")
	TArray<APlayerController*> PlayerControllers;
 
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="MuscleMagic|MMCamera")
	TArray<AMMCharacter*> Players;
 
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="MuscleMagic|MMCamera")
	FVector PlayersMidpoint;
 
	// Arbitrary minimum zoom level used to determine buffer multiplier on curve. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="MuscleMagic|MMCamera")
	float TheoreticalMinimumZoom;
 
	// Arbitrary maximum zoom level used to determine buffer multiplier on curve. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="MuscleMagic|MMCamera")
	float TheoreticalMaximumZoom;
 
	// Distance between the two furthest players in either the X or Y direction. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="MuscleMagic|MMCamera")
	float FurthestPlayerDistance;
 
	// Signifies the top left corner of a rectangle containing the players. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="MuscleMagic|MMCamera")
	FVector TopLeft;
 
	// Signifies the bottom right corner of a rectangle containing the players. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="MuscleMagic|MMCamera")
	FVector BottomRight;
 
	// Largest distance between two players in the X direction. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="MuscleMagic|MMCamera")
	float FurthestPlayerX;
 
	// Largest distance between two players in the Y direction. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="MuscleMagic|MMCamera")
	float FurthestPlayerY;
 
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="MuscleMagic|MMCamera")
	float AspectRatio;
 
	// Combines all multipliers for the buffer to be added to the zoom level. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="MuscleMagic|MMCamera")
	float BufferMultiplier;
 
	// Used to change the buffer multiplier depending on the current number of players.*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="MuscleMagic|MMCamera")
	float BufferModifier;
 
	// The last direction on the X axis that the camera lerped to. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="MuscleMagic|MMCamera")
	float LastXDirection;
 
	// Whether the match has started or not.*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="MuscleMagic|MMCamera")
	bool MatchStarted;
 
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="MuscleMagic|MMCamera")
	float Zoom;
 
private:
 
	FString DebugPrintString;
 
};

