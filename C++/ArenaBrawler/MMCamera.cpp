// Copyright 2019 Team Switcheroos, All Rights Reserved.

#include "MMCamera.h"
#include "MMCharacter.h"

#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"

#include "DrawDebugHelpers.h"
#include "EngineUtils.h"

#include "Engine/Classes/Kismet/GameplayStatics.h"
#include "Engine/Classes/Engine/World.h"

#include "Runtime/Engine/Classes/Engine/Engine.h"
#include "Runtime/Engine/Classes/Engine/GameViewportClient.h"
#include "Runtime/Engine/Classes/GameFramework/PlayerController.h"
#include "Runtime/Engine/Public/TimerManager.h"


AMMCameraActor::AMMCameraActor()
{
	PrimaryActorTick.bCanEverTick = true;

	// Create a camera boom
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 3000.0f; // The camera follows at this distance behind the character
	CameraBoom->bDoCollisionTest = false; // The camera does not care about collisions
	CameraBoom->bUsePawnControlRotation = false; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->FieldOfView = 50.f;
	FollowCamera->bUsePawnControlRotation = false; // Camera rotates relative to arm (though it probably won't ever rotate)

	BufferModifier = 1.f;
	Buffer = 250.f;
	BufferModifier2Players = 0.675f;
	BufferModifier3Players = 0.7f;
	BufferModifier4Players = 1.f;
	ZoomLerpAmount = 0.1f;
	MovementLerpAmount = 0.3f;
	WinnerLerpAmount = 0.05f;
	NegativeXBuffer = 700.f;
	MaxZoom = 5850.f;
	MinZoom = 1750.f;
	ZIgnoreThreshold = -1000.f;

    // Defaults to intro, which will be changed to Dynamic after the Intro camera is finished executing
	CameraMode = ECameraMode::CameraModeIntro;

	bDrawCameraMidpoint = false;
	bDrawPlayerLines = false;
	bDrawPlayerXYLimits = false;
	bPrintDebug = false;
	bConsiderAllPlayers = false;

	PlayersMidpoint = FVector(0.f);
	TheoreticalMinimumZoom = 1000.f;
	TheoreticalMaximumZoom = 5000.f;
	FurthestPlayerDistance = 0.f;
	TopLeft = FVector(0.f);
	BottomRight = FVector(0.f);
	FurthestPlayerX = 0.f;
	FurthestPlayerY = 0.f;
	AspectRatio = 0.f;
	BufferMultiplier = 0.f;
	LastXDirection = 0.f;
	Zoom = 0.f;

	MatchStarted = false;
}


void AMMCameraActor::BeginPlay()
{
	Super::BeginPlay();

	if (!ZoomCurve)
	{
        	UE_LOG(LogMMCamera, Error, TEXT("ERROR: Zoom curve not set in camera."));
	}

	for (TActorIterator<APlayerController> ActorItr(GetWorld()); ActorItr; ++ActorItr)
	{
		PlayerControllers.Add(*ActorItr);

        	UE_LOG(LogMMCamera, Warning, TEXT("Adding player controller."));
	}

	if (bConsiderAllPlayers)
	{
		FTimerHandle TimerHandle = FTimerHandle();
		GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &AMMCameraActor::GetAllPlayers, 0.2f, false);
	}
}


void AMMCameraActor::SetBufferModifier()
{
	switch (Players.Num())
	{
	case 2:
		BufferModifier = BufferModifier2Players;
		break;
	case 3:
		BufferModifier = BufferModifier3Players;
		break;
	case 4:
		BufferModifier = BufferModifier4Players;
		break;
	default:
		BufferModifier = 1.f;
		break;
	}
}


void AMMCameraActor::FindPlayerSquare()
{
	TopLeft = FVector(-9999.f, 9999.f, 300.f);
	BottomRight = FVector(9999.f, -9999.f, 300.f);

	for (int32 i = 0; i < Players.Num(); i++)
	{
		FVector ActorLocation = Players[i]->GetActorLocation();

		TopLeft.X = ActorLocation.X > TopLeft.X ? ActorLocation.X : TopLeft.X;
		TopLeft.Y = ActorLocation.Y < TopLeft.Y ? ActorLocation.Y : TopLeft.Y;

		BottomRight.X = ActorLocation.X < BottomRight.X ? ActorLocation.X : BottomRight.X;
		BottomRight.Y = ActorLocation.Y > BottomRight.Y ? ActorLocation.Y : BottomRight.Y;
	}

	TopLeft.X += 90.f;
	TopLeft.Y -= 90.f;

	BottomRight.X -= 90.f;
	BottomRight.Y += 90.f;

	if (bDrawPlayerXYLimits)
	{
		DrawDebugSphere(GetWorld(), TopLeft, 40.f, 12, FColor::White);
		DrawDebugSphere(GetWorld(), BottomRight, 40.f, 12, FColor::White);
	}
}


void AMMCameraActor::WinnerCamera()
{
	SetActorLocation(FMath::Lerp(GetActorLocation(), Players[0]->GetActorLocation(), WinnerLerpAmount));
}


void AMMCameraActor::MidpointCamera()
{
	FVector FurthestPlayersMidpoint;
	PlayersMidpoint = FVector(0.f);

	FurthestPlayerDistance = 0.f;
	FurthestPlayerX = 0.f;
	FurthestPlayerY = 0.f;

	float FurthestX1 = 0.f;
	float FurthestX2 = 0.f;

	for (int32 i = 0; i < Players.Num(); i++)
	{
        // Current actor location.
		FVector loc = Players[i]->GetActorLocation();

		// Sum player locations to find midpoint
		PlayersMidpoint += loc;

		for (int32 j = 0; j < Players.Num(); j++)
		{
			if (i != j)
			{
				FVector JLocation = Players[j]->GetActorLocation();

				// Calculate the furthest distance between two players in both the X and Y directions
				const float DistanceX = FMath::Abs(JLocation.X - loc.X);

				if (DistanceX > FurthestPlayerX)
				{
					FurthestPlayerX = DistanceX;
					// Store the midpoint and X position of the two furthest players in the X direction for later use
					FurthestPlayersMidpoint = (loc + JLocation) / 2.f;
					FurthestX1 = JLocation.X;
					FurthestX2 = loc.X;
				}

				const float DistanceY = FMath::Abs(JLocation.Y - loc.Y);

				if (DistanceY > FurthestPlayerY)
				{
					FurthestPlayerY = DistanceY;
				}
			}
		}
	}

	// Average the player positions to get the midpoint
	PlayersMidpoint /= Players.Num();
	// Account for the fact that the screen is wider than it is high
	FurthestPlayerX *= AspectRatio;

	FurthestPlayerDistance = FurthestPlayerX > FurthestPlayerY ? FurthestPlayerX : FurthestPlayerY;

	// Set the buffer modifier based on the number of players
	SetBufferModifier();

	// Calculate how much the camera should be weighted towards players that are far away from a group of players (in the X direction)
	float XDirection = 0.f;
	if (FurthestPlayerX != 0.f)
	{
		XDirection = (FMath::Abs(FurthestPlayersMidpoint.X - PlayersMidpoint.X) / FMath::Abs(FurthestX1 - FurthestX2)) * 4.f;
	}

	if (FurthestPlayersMidpoint.X > PlayersMidpoint.X)
	{
		XDirection = FMath::Lerp(LastXDirection, XDirection * 0.55f, 0.005f);
	} else
	{
		XDirection = FMath::Lerp(LastXDirection, XDirection * -1.f, 0.007f);
	}
	LastXDirection = XDirection;

	// Lerp to the midpoint of players, including the weighted extra in the X direction
	FVector Target = PlayersMidpoint;
	const float ZoomPercent = (CameraBoom->TargetArmLength - TheoreticalMinimumZoom) / (TheoreticalMaximumZoom - TheoreticalMinimumZoom);
	Target.X += XDirection * ZoomPercent * NegativeXBuffer * BufferModifier;

	SetActorLocation(FMath::Lerp(GetActorLocation(), Target, MovementLerpAmount));

	// Calculate and set the zoom level (target spring arm length)
	BufferMultiplier = ZoomCurve->GetFloatValue(ZoomPercent) * BufferModifier;

	// Lerp the zoom level smoothly
	Zoom = FMath::Lerp(CameraBoom->TargetArmLength, FurthestPlayerDistance + (Buffer * BufferMultiplier), ZoomLerpAmount);
	Zoom = FMath::Clamp(Zoom, MinZoom, MaxZoom);
	CameraBoom->TargetArmLength = Zoom;

	// Debug draw/prints
	if (bDrawCameraMidpoint)
	{
		DrawDebugSphere(GetWorld(), PlayersMidpoint, 40.f, 12, FColor::White);
	}

	if (bDrawPlayerLines)
	{
		for (int32 i = 0; i < Players.Num(); i++)
		{
			for (int32 j = 0; j < Players.Num(); j++)
			{
				if (i != j)
				{
					DrawDebugLine(GetWorld(), Players[i]->GetActorLocation(), Players[j]->GetActorLocation(), FColor::White);
				}
			}
		}
	}

	if (bDrawPlayerXYLimits)
	{
		FindPlayerSquare();
	}

	if (bPrintDebug)
	{
		DebugPrintString += "Zoom " + FString::SanitizeFloat(Zoom);
		DebugPrintString += "ZoomPercent " + FString::SanitizeFloat(ZoomPercent);

        UE_LOG(LogMMCamera, Warning, TEXT("Debugging: %s"), *DebugPrintString)

		DebugPrintString.Empty();
	}

} // MidpointCamera.

void AMMCameraActor::IntroCamera_Implementation()
{
}


void AMMCameraActor::Tick(const float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!bConsiderAllPlayers)
	{
		for (int32 i = 0; i < PlayerControllers.Num(); ++i)
		{
			auto *ControlledCharacter = Cast<AMMCharacter>(PlayerControllers[i]->GetPawn());

			if (ControlledCharacter != nullptr)
			{
				if (!MatchStarted)
				{
					MatchStart();
					MatchStarted = true;
				}

				Players.Add(ControlledCharacter);
				PlayerControllers.RemoveAt(i);

				--i; // TODO: This is bad practice.
			}
		}
	} 
	else
	{
		if (!MatchStarted)
		{
			MatchStart();
			MatchStarted = true;
		}
	}

    if (CameraMode == ECameraMode::CameraModeStatic)
    {
        // Do not zoom or update camera when static.
        return;
    }

	CheckValidPlayers();

	// Execute camera modes
	if (Players.Num() > 1)
	{
		switch (CameraMode)
		{
		case ECameraMode::CameraModeStatic:
			break;
		case ECameraMode::CameraModeDynamic:
			MidpointCamera();
			break;
		case ECameraMode::CameraModeIntro:
			IntroCamera();
		}
	}
	else if (Players.Num() > 0)
	{
		WinnerCamera();
	}
	else
	{
		SetActorLocation(FMath::Lerp(GetActorLocation(), FVector(0.f), WinnerLerpAmount));
	}

} // Tick.


void AMMCameraActor::MatchStart()
{
	// Set this camera to be the active camera
	PlayerControllers[0]->SetViewTargetWithBlend(this);

	FVector2D ViewportSize = FVector2D();
	GetWorld()->GetGameViewport()->GetViewportSize(ViewportSize);
	AspectRatio = ViewportSize.X / ViewportSize.Y;
}


void AMMCameraActor::CheckValidPlayers()
{
	for (int32 i = 0; i < Players.Num(); ++i)
	{
		bool bRemovePlayer = false;
		bool bIsPendingKill = Players[i]->IsPendingKill();
		bool bIsDead = Players[i]->GetHealthPoints() <= 0.f;
		bool bIsBelowZThreshold = Players[i]->GetActorLocation().Z <= ZIgnoreThreshold;

		if (!Players[i])
		{
			bRemovePlayer = true;
		} else if (bIsPendingKill || bIsDead || bIsBelowZThreshold)
		{
			bRemovePlayer = true;
		}

		if (bRemovePlayer)
		{
			Players.RemoveAt(i);

			UE_LOG(LogMMCamera, Warning, TEXT("Debugging: %d"), i)

			UGameplayStatics::GetPlayerController(GetWorld(), 0)->SetViewTargetWithBlend(this);

			--i;
		}
	}
}


void AMMCameraActor::GetAllPlayers()
{
	for (TActorIterator<AMMCharacter> ActorItr(GetWorld()); ActorItr; ++ActorItr)
	{
		Players.Add(*ActorItr);
	}
}
