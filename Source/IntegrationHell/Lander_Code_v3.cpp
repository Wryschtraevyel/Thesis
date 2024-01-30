// Fill out your copyright notice in the Description page of Project Settings.


#include "Lander_Code_v3.h"

// Sets default values
ALander_Code_v3::ALander_Code_v3()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bTickEvenWhenPaused = true;
	PrimaryActorTick.TickGroup = TG_PrePhysics;
	DataToSend.Init(0.0, 17);
	DataReceivedFloat.Init(0.0, 3);

}

// Called when the game starts or when spawned
void ALander_Code_v3::BeginPlay()
{
	is_using_monopropellant = true;
	remaining_mission_time = max_mission_time;
	score = 0;
	is_over = false;
	number_of_restarts = 3;
	fuel = 50000;
	oxidizer = 50000;
	monopropellant = 8200;
	rcs_fuel = 500;
	gimball_pos_x = 0;
	gimball_pos_y = 0;
	engine_throttle = 60;
	fuel_valve = 50;
	oxidizer_valve = 50;
	battery_charge = 500000;
	target_throttle = 0;
	target_ox_valve = 50;
	target_fuel_valve = 50;
	target_gimball_pos_x = 0;
	target_gimball_pos_y = 0;
	target_coords = getRandomTarget();
	starting_coords = getRandomStart();
	pointerToTarget = (target_coords - starting_coords).GetUnsafeNormal();
	unitpos = starting_coords;
	velocity = FVector(0, 0, 0);
	Super::BeginPlay();
	
}
void ALander_Code_v3::restartGame() {

	//if (is_over) {
	//	if (PythonSocket && PythonSocket->GetConnectionState() == SCS_Connected) {

	//		PrepareData();
	//		float* data = DataToSend.GetData();
	//		uint8* B_DataToSend = reinterpret_cast<uint8*>(data);

	//		PythonSocket->Send(B_DataToSend, 17 * sizeof(float), BytesSent);
	//		tBytesSent = BytesSent;


	//		if (DataToSend[0] == 1.0) is_over_sent = true;
	//		IsBytesToSend = false;
	//	}
	//}

	is_using_monopropellant = true;
	remaining_mission_time = max_mission_time;
	score = 0;
	is_over = false;
	number_of_restarts = 3;
	fuel = 50000;
	oxidizer = 50000;
	monopropellant = 8200;
	rcs_fuel = 500;
	gimball_pos_x = 0;
	gimball_pos_y = 0;
	engine_throttle = 60;
	fuel_valve = 50;
	oxidizer_valve = 50;
	battery_charge = 500000;
	target_throttle = 0;
	target_ox_valve = 50;
	target_fuel_valve = 50;
	target_gimball_pos_x = 0;
	target_gimball_pos_y = 0;
	target_coords = getRandomTarget();
	starting_coords = getRandomStart();
	unitpos = starting_coords;
	pointerToTarget = target_coords - unitpos;
	velocity = FVector(0, 0, 0);

	/*if (PythonSocket && PythonSocket->GetConnectionState() == SCS_Connected) {
		PrepareData();
		float* data = DataToSend.GetData();
		uint8* B_DataToSend = reinterpret_cast<uint8*>(data);

		PythonSocket->Send(B_DataToSend, 17 * sizeof(float), BytesSent);
		tBytesSent = BytesSent;


		if (DataToSend[0] == 1.0) is_over_sent = true;
		IsBytesToSend = false;
	}*/

}

// Called every frame
void ALander_Code_v3::Tick(float DeltaTime)
{
	landerTick(DeltaTime);
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ALander_Code_v3::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void ALander_Code_v3::landerTick(float DeltaTime) {

	/*if (PythonSocket && PythonSocket->GetConnectionState() == SCS_Connected)
	{

		if (IsBytesToSend)
		{
			PrepareData();
			float* data = DataToSend.GetData();
			uint8* B_DataToSend = reinterpret_cast<uint8*>(data);

			PythonSocket->Send(B_DataToSend, 17 * sizeof(float), BytesSent);
			tBytesSent = BytesSent;


			if (DataToSend[0] == 1.0) is_over_sent = true;
			IsBytesToSend = false;

		}

		if (PythonSocket->HasPendingData(ByteToRead)) {



			if (PythonSocket->Recv(DataReceived, ByteToRead, BytesToRead))
			{

				ParseArrivedBytes(DataReceived);
				IsBytesToSend = true;

			}

		}


	}*/


	if (!is_over) {
		pointerToTarget = target_coords - unitpos;
		FuelDecrease(DeltaTime);
		adjustComponents(DeltaTime);
		remaining_mission_time -= DeltaTime;
		if (remaining_mission_time <= 0) {
			is_over = true;
			remaining_mission_time = 0;
		}
	}
}
FVector ALander_Code_v3::getRandomTarget() {

	FRandomStream rs;
	rs.GenerateNewSeed();
	float x = rs.FRandRange(-500, 500);
	float y = rs.FRandRange(-500, 500);
	FVector pos(x, y, 0);
	return pos;

}
FVector ALander_Code_v3::getRandomStart() {

	FRandomStream rs;
	rs.GenerateNewSeed();
	float x = rs.FRandRange(-500, 500);
	float y = rs.FRandRange(-500, 500);
	float z = rs.FRandRange(2700, 3900);
	FVector pos(x, y, z);
	return pos;
}

void ALander_Code_v3::FuelDecrease(float DeltaTime) {
	monopropellant -= (engine_throttle / 100) * (8200 / 311) * (DeltaTime / 1000);
	if (monopropellant < 0) monopropellant = 0;
	battery_charge = battery_charge > (DeltaTime / 1000) * 0.1 ? battery_charge - (DeltaTime / 1000) * 0.1 : 0;
	if (battery_charge < 0) battery_charge = 0;
}
void ALander_Code_v3::adjustComponents(float DeltaTime) {
	double powerreq_valve = 8;
	double powerreq_actuator = 20;
	double powerreq_reignite = 200;
	bool engineoff = engine_throttle < 10;
	if (is_using_monopropellant) {

		if (engine_throttle != target_throttle) {
			if (target_throttle == 0) {
				engine_throttle = 0;
				battery_charge -= powerreq_valve;
			}
			else {
				if (abs(engine_throttle - target_throttle) < DeltaTime / 30) {
					engine_throttle = target_throttle;
				}
				else {
					engine_throttle += engine_throttle < target_throttle ? DeltaTime / 30 : DeltaTime / (-30);
				}
				battery_charge -= powerreq_valve * DeltaTime / 1000;
			}


		}
		if (engineoff && engine_throttle >= 10 && number_of_restarts > 0 && target_throttle >= 10) {
			--number_of_restarts;
			battery_charge -= powerreq_reignite;
			engineoff = false;
		}
	}
	else {
		//future plan
	}

	if (gimball_pos_x != target_gimball_pos_x) {
		if (abs(gimball_pos_x - target_gimball_pos_x) < 0.5 * DeltaTime / 1000) {
			gimball_pos_x = target_gimball_pos_x;
		}
		else {
			gimball_pos_x += gimball_pos_x < target_gimball_pos_x ? 0.5 * DeltaTime / 1000 : -0.5 * DeltaTime / 1000;
		}
		battery_charge -= powerreq_actuator * DeltaTime / 1000;
	}

	if (gimball_pos_y != target_gimball_pos_y) {
		if (abs(gimball_pos_y - target_gimball_pos_y) < 0.5 * DeltaTime / 1000) {
			gimball_pos_y = target_gimball_pos_y;
		}
		else {
			gimball_pos_y += gimball_pos_y < target_gimball_pos_y ? 0.5 * DeltaTime / 1000 : -0.5 * DeltaTime / 1000;
		}
		battery_charge -= powerreq_actuator * DeltaTime / 1000;
	}

}

FVector ALander_Code_v3::thrustToApply() {
	float thrust = 0;
	if (monopropellant <= 0) {
		thrust = 0;
	}
	else if (isengineoff && number_of_restarts == 0) {
		thrust = 47 * engine_throttle / 10;
	}
	else {
		if (engine_throttle >= 95) {
			thrust = 47000 * engine_throttle / 100;
		}
		else if (engine_throttle > 60) {
			thrust = 30360 + (engine_throttle - 60) * 100;
		}
		else if (engine_throttle >= 10) {
			thrust = 4700 + (engine_throttle - 10) * (30360 - 4700) / (60 - 10);
		}
		else {
			thrust = 47 * engine_throttle / 10;
		}
	}
	float pos_x_rad = FMath::DegreesToRadians(gimball_pos_x);
	float pos_y_rad = FMath::DegreesToRadians(gimball_pos_y);
	float z =
		FMath::Sqrt(
			FMath::Square(thrust)
			/ (
				FMath::Square(FMath::Tan(pos_x_rad))
				+ FMath::Square(FMath::Tan(pos_y_rad)) + 1
				)
		);
	float x = z * FMath::Tan(pos_x_rad);
	float y = z * FMath::Tan(pos_y_rad);


	FVector force = FVector(x, y, z);
	return force;

}
FVector ALander_Code_v3::calcGravitationalForce() {
	float height = unitpos.Component(2);
	float grav_force =
		(6.674 * FMath::Pow(10.0, -11.0) *
			(monopropellant + drymass) * 7.34767309 * FMath::Pow(10.0, 22.0))
		/ (FMath::Square(1737400 + height));

	return FVector(0, 0, grav_force);


}
FVector ALander_Code_v3::landerAcceleration() {
	FVector totalforce = -calcGravitationalForce() + thrustToApply();
	float mass = monopropellant + drymass;
	FVector acceleration = totalforce / mass;
	return acceleration;
}