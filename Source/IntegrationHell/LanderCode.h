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

#include "LanderCode.generated.h"

UCLASS(Blueprintable, BlueprintType)
class INTEGRATIONHELL_API ALanderCode : public APawn
{
	GENERATED_BODY()
public:
	ALanderCode();
	//~ALanderCode();
	/** Please add a function description */
	UFUNCTION(BlueprintCallable)
	void FuelDecrease(float DeltaTime);	
	UFUNCTION(BlueprintCallable)
		void adjustComponents(float DeltaTime);
	UFUNCTION(BlueprintCallable)
		void landerTick(float DeltaTime);
	UFUNCTION(BlueprintCallable)
		float roundToFloatPrecision(float number, float precision);

	UPROPERTY(BlueprintReadOnly)
		bool isConnectionOn = false;

	UFUNCTION(BlueprintCallable)
		bool TryConnection();
	UFUNCTION(BlueprintCallable)
		bool TestSendMessage();
	UPROPERTY(EditAnywhere)
		FString PythonServerIP = TEXT("127.0.0.1");

	UFUNCTION(BlueprintCallable)
		FVector thrustToApply();
	UFUNCTION(BlueprintCallable)
		FVector landerAcceleration();
	UFUNCTION(BlueprintCallable)
		FVector calcTargetPlanetGravitationalForce();
	UFUNCTION(BlueprintCallable)
		float calcGravitationalAcc(float dist, float mass);
	UFUNCTION(BlueprintCallable)
		FVector getRandomTarget();
	UFUNCTION(BlueprintCallable)
		FVector getRandomStart();	

	UFUNCTION(BlueprintCallable)
		float getScore();
	UFUNCTION(BlueprintCallable)
		void restartGame();
	UFUNCTION(BlueprintCallable)
		void missionTimeOver();
		float calcSpeedGainFromHeight(float h, float startvel);
	UFUNCTION(BlueprintCallable)
		void onSurfaceHit();
	UFUNCTION(BlueprintCallable)
		void applyNoise();
	UFUNCTION(BlueprintCallable)
		FVector calcVelocity(float DeltaTime);
	UFUNCTION(BlueprintCallable)
		FVector calcPosition(float DeltaTime);
	UFUNCTION(BlueprintCallable)
		float getReward();
	UFUNCTION(BlueprintCallable)
		FString getFormattedLog();
	UPROPERTY(BlueprintReadWrite)
	TArray<float> DataToSend;
	UPROPERTY(BlueprintReadWrite)
	TArray<float> DataReceivedFloat;

	uint8* DataReceived = new uint8[3*sizeof(float)];
	FSocket* PythonSocket;
	int32 BytesToRead = 0;
	uint32 ByteToRead = 0;
	int32 BytesSent = 0;
	UPROPERTY(BlueprintReadWrite)
		int32 tBytesSent=0;
	UFUNCTION(BlueprintCallable)
		FString floatArrayToString(TArray<float> arr, int len);
	UFUNCTION(BlueprintCallable)
		FString boolToString(bool boolean = true, int mode = 0);
	bool IsBytesToSend = true;
	UPROPERTY(BlueprintReadWrite)
		bool is_over;
	UPROPERTY(BlueprintReadWrite)
		bool on_surface;
	UPROPERTY(BlueprintReadWrite)
		int landing_count = 0;
	UPROPERTY(BlueprintReadWrite)
		int surfacehit_count = 0;
	UPROPERTY(BlueprintReadWrite)
		int timeover_count = 0;
	UPROPERTY(BlueprintReadWrite)
		bool is_over_sent = false;
	UPROPERTY(BlueprintReadWrite)
		bool wantsAnotherRide=false;
	UPROPERTY(BlueprintReadWrite)
		int etapcounter = 0;
	UPROPERTY(BlueprintReadWrite)
		float runtime = 0;
	UPROPERTY(BlueprintReadWrite)
		float simspeed = 1;
	UPROPERTY(BlueprintReadOnly)
		float simDeltaTime = 0;
	UPROPERTY(BlueprintReadOnly)
		float sensorSamplingTime = 0.001;
	UPROPERTY(BlueprintReadOnly)
		float timeSinceLastSampling = 0;
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
		float max_mission_time = 600; //1200;
	UPROPERTY(BlueprintReadWrite)
		float min_throttle_change = 0.1;
	UPROPERTY(BlueprintReadWrite)
		float throttle_pid_accuracy = 0.05;
	UPROPERTY(BlueprintReadWrite)
		float min_gimball_change = 0.01;
	UPROPERTY(BlueprintReadWrite)
		float giball_pid_accuracy = 0.003;
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
		float drymass=15102.1;
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "Default")
		bool noisySensor = false;
	UFUNCTION(BlueprintCallable)
		float getCurrentFuelMass();
	UFUNCTION(BlueprintCallable)
		float addGaussianNoise(float f=0.5, float dev=0.01);
	UFUNCTION(BlueprintCallable)
		float roundToGivenDecimal(float f = 0.0, int decimal = 4);
	UFUNCTION(BlueprintCallable)
		FVector calcAccVectorCelestialBody(FVector dir, float acc);
	UFUNCTION(BlueprintCallable)
		FVector calcAccVectorCelestialBodyFromMass(float mass, FVector dist);
	UPROPERTY(BlueprintReadWrite)
		float celestialBodyMass = 7.34767309 * FMath::Pow(10.0, 22.0);
	UPROPERTY(BlueprintReadWrite)
		float celestialBodyRadius = 1764000;
	UPROPERTY(BlueprintReadOnly)
		float gravConst = 6.674 * FMath::Pow(10.0, -11.0);
	UPROPERTY(BlueprintReadWrite)
		float inhomConst=0;
	UFUNCTION(BlueprintCallable)
		void setinhomConst();
	UPROPERTY(BlueprintReadOnly)
		float equatorialRadius = 6378136.6;
	UPROPERTY(BlueprintReadOnly)
		float polarRadius = 6356751.9;
	UFUNCTION(BlueprintCallable)
		float getPlanetEllipsoidRadius(FVector polarcoords);
	UFUNCTION(BlueprintCallable)
		float getGeopontentialAltitude(FVector polarcoords);
	UFUNCTION(BlueprintCallable)
		FVector cartesianToPolar(FVector cartesiancoords);
	UFUNCTION(BlueprintCallable)
		FVector polarToCartersian(FVector polarcoords);
	UPROPERTY(BlueprintReadOnly)
		float atm_temperature;
	UPROPERTY(BlueprintReadWrite)
		float atm_density;
	UPROPERTY(BlueprintReadWrite)
		float atm_pressure;
	UPROPERTY(BlueprintReadWrite)
		float atm_kinematicViscosity;
	UPROPERTY(BlueprintReadWrite)
		float atm_speedOfSound;
	UFUNCTION(BlueprintCallable)
		void getISAData(float geoPotH, float p_ref, float t_ref, float h_ref, float rho_ref, float lapse_rate);
	UFUNCTION(BlueprintCallable)
		void getAthmosphericData(float geopotAlt);
	UPROPERTY(BlueprintReadOnly)
		float maxComponentSurfaceHitVel = 7;

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