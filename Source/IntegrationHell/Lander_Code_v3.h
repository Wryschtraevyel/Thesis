// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Components/StaticMeshComponent.h"
#include "Particles/ParticleSystemComponent.h"
#include "Components/SceneComponent.h"
#include "Engine/World.h"

#include "Math/Vector.h"
#include "Kismet/KismetMathLibrary.h"
#include "Math/UnrealMathUtility.h"

#include "Math/RandomStream.h"
#include "Misc/DateTime.h"

#include "SocketSubsystem.h"
#include "Sockets.h"
#include "Networking.h"
#include "IPAddress.h"
#include "Lander_Code_v3.generated.h"

UCLASS()
class INTEGRATIONHELL_API ALander_Code_v3 : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	ALander_Code_v3();

	//variables
	UPROPERTY(BlueprintReadOnly)
		bool isConnectionOn = false;

	UPROPERTY(EditAnywhere)
		FString PythonServerIP = TEXT("127.0.0.1");
	UPROPERTY(BlueprintReadWrite)
		TArray<float> DataToSend;
	UPROPERTY(BlueprintReadWrite)
		TArray<float> DataReceivedFloat;
	uint8* DataReceived = new uint8[3 * sizeof(float)];
	FSocket* PythonSocket;
	int32 BytesToRead = 0;
	uint32 ByteToRead = 0;
	int32 BytesSent = 0;
	UPROPERTY(BlueprintReadWrite)
		int32 tBytesSent = 0;
	bool IsBytesToSend = true;
	UPROPERTY(BlueprintReadWrite)
		bool is_over;
	UPROPERTY(BlueprintReadWrite)
		bool is_over_sent = false;
	UPROPERTY(BlueprintReadWrite)
		bool wantsAnotherRide = false;
	UPROPERTY(BlueprintReadWrite)
		int timeout = 10000;
	UPROPERTY(BlueprintReadWrite)
		float score;
	UPROPERTY(BlueprintReadWrite)
		float value;
	UPROPERTY(BlueprintReadWrite)
		float speed_weight = 9;
	UPROPERTY(BlueprintReadWrite)
		float fuel_weight = 5;
	UPROPERTY(BlueprintReadWrite)
		float distance_weight = 3;
	UPROPERTY(BlueprintReadWrite)
		float time_weight = 0.02;
	UPROPERTY(BlueprintReadWrite)
		float max_mission_time = 1200000;
	UPROPERTY(BlueprintReadWrite)
		float min_throttle_change = 0.1;
	UPROPERTY(BlueprintReadWrite)
		float min_gimball_change = 0.01;
	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category = "Default")
		float remaining_mission_time;
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Default")
		TObjectPtr<UParticleSystemComponent> P_Explosion_Big_A;
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Default")
		TObjectPtr<UStaticMeshComponent> StaticMesh1;
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Default")
		TObjectPtr<USceneComponent> DefaultSceneRoot;
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "Default")
		FVector Actuatorsettings;
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "Default")
		FVector target_coords;
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "Default")
		FVector pointerToTarget;
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "Default")
		FVector starting_coords;
	UPROPERTY(BlueprintReadWrite)
		FVector velocity;
	UPROPERTY(BlueprintReadWrite)
		FVector unitpos;
	UPROPERTY(BlueprintReadWrite)
		FVector last_checked_pos;
	UPROPERTY(BlueprintReadWrite)
		float maxdistance = 420000;
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "Default")
		float fuel;
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "Default")
		double oxidizer;
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "Default")
		float monopropellant;
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "Default")
		float rcs_fuel;
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "Default")
		bool is_using_monopropellant;
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "Default")
		float gimball_pos_x;
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "Default")
		float gimball_pos_y;
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "Default")
		float engine_throttle;
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "Default")
		float fuel_valve;
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "Default")
		float oxidizer_valve;
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "Default")
		float battery_charge;
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "Default")
		float target_throttle;
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "Default")
		float target_ox_valve;
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "Default")
		float target_fuel_valve;
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "Default")
		float target_gimball_pos_x;
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "Default")
		float target_gimball_pos_y;
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "Default")
		int number_of_restarts;
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "Default")
		bool isengineoff;
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "Default")
		float drymass = 15102.1;

	//methods
	UFUNCTION(BlueprintCallable)
		void restartGame();

	UFUNCTION(BlueprintCallable)
		FVector getRandomTarget();

	UFUNCTION(BlueprintCallable)
		FVector getRandomStart();
	UFUNCTION(BlueprintCallable)
		void FuelDecrease(float DeltaTime);

	UFUNCTION(BlueprintCallable)
		void adjustComponents(float DeltaTime);

	UFUNCTION(BlueprintCallable)
		void landerTick(float DeltaTime);

	UFUNCTION(BlueprintCallable)
		FVector thrustToApply();

	UFUNCTION(BlueprintCallable)
		FVector landerAcceleration();

	UFUNCTION(BlueprintCallable)
		FVector calcGravitationalForce();
	
	/*
	UFUNCTION(BlueprintCallable)
		bool TryConnection();

	UFUNCTION(BlueprintCallable)
		bool TestSendMessage();





	UFUNCTION(BlueprintCallable)
		float getScore();



	UFUNCTION(BlueprintCallable)
		void missionTimeOver();

	float calcSpeedGainFromHeight(float h, float startvel);

	UFUNCTION(BlueprintCallable)
		void onSurfaceHit();

	UFUNCTION(BlueprintCallable)
		float getReward();
	
	UFUNCTION(BlueprintCallable)
		FString floatArrayToString(TArray<float> arr, int len);
	*/
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	int ParseArrivedBytes(uint8*);
	void PrepareData();

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

};
