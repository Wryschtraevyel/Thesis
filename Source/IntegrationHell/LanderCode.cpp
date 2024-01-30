// Fill out your copyright notice in the Description page of Project Settings.


#include "LanderCode.h"
#include <random>

// Sets default values
ALanderCode::ALanderCode()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bTickEvenWhenPaused = true;
	PrimaryActorTick.TickGroup = TG_PrePhysics;
	DataToSend.Init(0.0, 17);
	DataReceivedFloat.Init(0.0, 3);
	/*StaticMesh1 = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("<Lander>"));
	StaticMesh1->SetStaticMesh();
	StaticMesh1->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);*/
	//DefaultSceneRoot = CreateDefaultScreenRoot
	/** Please add a variable description */
//	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Default")
//		TObjectPtr<UParticleSystemComponent> P_Explosion_Big_A;

	/** Please add a variable description */
//	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Pawn")
//		TObjectPtr<UMotionControllerComponent> MotionController;

	/** Please add a variable description */
//	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Default")
//		TObjectPtr<UStaticMeshComponent> StaticMesh1;

	/** Please add a variable description */
//	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Default")
//		TObjectPtr<UAIPerceptionComponent> AIPerception;

	/** Please add a variable description */
//	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Default")
//		TObjectPtr<USceneComponent> DefaultSceneRoot;

}
void ALanderCode::FuelDecrease(float DeltaTime) {
	monopropellant -= (engine_throttle / 100) * (8200 / 311) * DeltaTime;
	if (monopropellant < 0) monopropellant = 0;
	battery_charge = battery_charge > DeltaTime * 0.1 ? battery_charge - DeltaTime * 0.1 : 0;
	if (battery_charge < 0) battery_charge = 0;
}
bool ALanderCode::TryConnection() {
	bool ConnectionSuccess = false;
	TSharedRef<FInternetAddr> PythonServerAddr = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->CreateInternetAddr();
	FIPv4Address ip;
	FIPv4Address::Parse(PythonServerIP, ip);
	PythonServerAddr->SetIp(ip.Value);
	PythonServerAddr->SetPort(7001);
	PythonSocket = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->CreateSocket(NAME_Stream, TEXT("Default"), false);
		if (PythonSocket->Connect(*PythonServerAddr))
	{
		GLog->Logf(TEXT("Connection Successful"));
		ConnectionSuccess = true;
	}
	else {
		GLog->Logf(TEXT("Connection failed."));
	}

	isConnectionOn = ConnectionSuccess;

	return ConnectionSuccess;
}
bool ALanderCode::TestSendMessage() {
	bool Sended = false;
	if (PythonSocket && PythonSocket->GetConnectionState() == SCS_Connected)
	{
		uint8 PLUTO = 4;

		PythonSocket->Send(&PLUTO, 1, BytesSent);
		UE_LOG(LogTemp, Warning, TEXT("Something went wrong."));
		Sended = true;
	}
	return Sended;
}
void ALanderCode::setinhomConst() {
	FRandomStream rs;
	rs.GenerateNewSeed();
	inhomConst = rs.FRandRange(-0.0126, 0.0126);
}
void ALanderCode::adjustComponents(float DeltaTime) {
	double powerreq_valve = 8;
	double powerreq_actuator = 20;
	double powerreq_reignite = 200;
	bool engineoff = engine_throttle < 10;
	if (is_using_monopropellant ) {
		
		if (engine_throttle != target_throttle) {
			if (target_throttle == 0) {
				engine_throttle = 0;
				battery_charge -= powerreq_valve;
			}
			else {
				if (abs(engine_throttle - target_throttle) > throttle_pid_accuracy) {
					engine_throttle += engine_throttle < target_throttle ? DeltaTime / 3 : DeltaTime / (-3);
				}
				battery_charge -= powerreq_valve * DeltaTime;
			}
			
			
		}
		if (engineoff && engine_throttle >= 10 && number_of_restarts>0 && target_throttle >=10) {
			--number_of_restarts;
			battery_charge -= powerreq_reignite;
			engineoff = false;
		}
	}
	else {
		//future plan
	}

	if (gimball_pos_x != target_gimball_pos_x) {
		if (abs(gimball_pos_x - target_gimball_pos_x) > giball_pid_accuracy) {
			gimball_pos_x += gimball_pos_x < target_gimball_pos_x ? 0.5 * DeltaTime : -0.5 * DeltaTime;
		}
		battery_charge -= powerreq_actuator * DeltaTime;
	}

	if (gimball_pos_y != target_gimball_pos_y) {
		if (abs(gimball_pos_y - target_gimball_pos_y) > giball_pid_accuracy){ 
			gimball_pos_y += gimball_pos_y < target_gimball_pos_y ? 0.5 * DeltaTime : -0.5 * DeltaTime;
		}
		battery_charge -= powerreq_actuator * DeltaTime;
	}

}
float ALanderCode::addGaussianNoise(float f, float dev) {
	std::random_device rd{};
	std::mt19937 gen{ rd() };
	std::normal_distribution<float> d{ f, dev };
	return d(gen);
}
float ALanderCode::getScore() {

	float mission_time = max_mission_time - remaining_mission_time;
	float landing_speed = velocity.Size();
	float final_distance = FVector::Distance(target_coords, unitpos);
	float final_distance_xy = FVector::DistXY(target_coords, unitpos);
	

	if (remaining_mission_time <= 0) {
		landing_speed += calcSpeedGainFromHeight(unitpos.Component(2), landing_speed);
		score = -10000;
	}
	else {
		score = 0;
	}

	if (landing_speed > 3) {
		score -= landing_speed * speed_weight * speed_weight;
		score -= 250000;
	}
	else {
		score -= landing_speed * speed_weight;
		score += remaining_mission_time * time_weight;
		score += monopropellant * fuel_weight;
		score -= final_distance_xy * distance_weight;
	}
	
	return score;
}
float ALanderCode::getReward() {
	float reward = 0;
	if (
		FVector::DistXY(target_coords, unitpos) > 100
		&& FVector::DistXY(starting_coords, unitpos) > 100
		&& FMath::Abs(1500 - unitpos.Component(2) / 100) > 500
		)
	{
		score -= FMath::Abs(1500 - unitpos.Component(2) / 100);
	}

	if ((target_throttle <= 10 || engine_throttle <=10) && (unitpos[2] > 10 || velocity.Size() > 5)) {
		reward -= speed_weight * 900;
	}

	reward -=
		remaining_mission_time < 600000
		? 600000 - remaining_mission_time
		: 0;
	
	if (FVector::DistXY(target_coords, unitpos) < FVector::DistXY(target_coords, last_checked_pos)) 
	{
		reward += distance_weight *
			(FVector::DistXY(target_coords, unitpos) > 100
			? FVector::DistXY(last_checked_pos, unitpos)
			: FVector::Distance(last_checked_pos, unitpos));
	}
	else 
	{
		reward -= 
			2 * distance_weight *
			(
				FVector::DistXY(target_coords, unitpos) > 100
				? FVector::DistXY(last_checked_pos, unitpos)
				: FVector::Distance(last_checked_pos, unitpos)
			);
	}
	float starting_distance = FVector::Distance(starting_coords, target_coords) * 1.2;
	float expected_total_fuel_usage = starting_distance / maxdistance * 8200;
	float expected_max_fuel_usage = FVector::Distance(starting_coords, unitpos) / starting_distance * expected_total_fuel_usage * 2;
	reward -=
		expected_max_fuel_usage < (8200 - monopropellant)
		? fuel_weight * ((8200 - monopropellant) - expected_max_fuel_usage)
		: 0;

	return reward;
}
float ALanderCode::calcSpeedGainFromHeight(float h, float startvel) {
	return sqrt((startvel * startvel) + 2 * 6.6743 * 7.34767309 * (10 ^ 11)*(1/(1737400) - 1/(1737400+h)));
}
void ALanderCode::missionTimeOver() {

}
FVector ALanderCode::getRandomTarget() {

	FRandomStream rs;
	rs.GenerateNewSeed();
	float x = rs.FRandRange(-500,500);
	float y = rs.FRandRange(-500,500);
	FVector pos(x, y, 0);
	return pos;

}
float ALanderCode::roundToFloatPrecision(float number, float precision) {
	float digits = -FMath::LogX(10, precision)-1;
	float result = roundToGivenDecimal(number, (int)digits);
	while (abs(result - number) > precision) {
		result += precision;
	}
	if ((result + precision / 2) < number) {
		result += precision;
	}

	return result;
}
FVector ALanderCode::getRandomStart() {

	FRandomStream rs;
	rs.GenerateNewSeed();
	float x = rs.FRandRange(-500, 500);
	float y = rs.FRandRange(-500, 500);
	float z = rs.FRandRange(2700, 3900);
	FVector pos(x, y, z);
	return pos;
}
FVector ALanderCode::thrustToApply() {
	float thrust = 0;
	if (monopropellant <= 0) {
		thrust = 0;
	}
	else if (isengineoff && number_of_restarts==0) {
		thrust = 47 * engine_throttle/10;
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
	

	FVector force = FVector(x,y,z);
	return force;

}
float ALanderCode::getCurrentFuelMass() {
	if (is_using_monopropellant) {
		return monopropellant;
	}
	else {
		return oxidizer + fuel;
	}
	return 0;
}
FVector ALanderCode::calcTargetPlanetGravitationalForce() {
	float height = unitpos.Component(2);
	float grav_force =
		(gravConst *
			(getCurrentFuelMass() + drymass) * celestialBodyMass)
		/ (FMath::Square(celestialBodyRadius +height));

	return FVector(0, 0, grav_force);


}
FVector ALanderCode::landerAcceleration() {	
	if (on_surface) {
		return FVector(0, 0, 0);
	}
	else {
		FVector totalforce = -calcTargetPlanetGravitationalForce() + thrustToApply();
		float mass = monopropellant + drymass;
		FVector acceleration = totalforce / mass;
		//generic directions, these should change based on the location of the landing site
		acceleration += calcAccVectorCelestialBody(FVector(1, 1, 0.01), (float)0.00593);
		acceleration += calcAccVectorCelestialBody(FVector(1, -1, -2), (float)0.0027);
		//acceleration[2] += inhomConst;
		return acceleration;
	}
	return FVector(0, 0, 0);
}
FString ALanderCode::boolToString(bool boolean, int mode) {
	FString str = FString();
	switch (mode) {
	case 0: str = boolean ? "1" : "0"; break;
	case 1: str = boolean ? "True" : "False"; break;
	case 2: str = boolean ? "Yes" : "No"; break;
	case 3: str = boolean ? "Sure" : "Nope"; break;
	case 4: str = boolean ? "Valid" : "Invalid"; break;
	default: str = boolean ? "T" : "F"; break;
	}
	return str;
}
FString ALanderCode::getFormattedLog() {
	FString str = FString();
	str.Append("****************************\n");
	str.Append("Is connected: " + boolToString(PythonSocket && PythonSocket->GetConnectionState() == SCS_Connected, 2) + '\n');
	if (timeSinceLastSampling == 0 || timeSinceLastSampling >= sensorSamplingTime) {
		str.Append("Data to be sent: " + floatArrayToString(DataToSend, 17));
		str.Append("Curr data received: " + floatArrayToString(DataReceivedFloat, 3));
	}
	else {
		str.Append("Last data  sent: " + floatArrayToString(DataToSend, 17));
		str.Append("Last data received: " + floatArrayToString(DataReceivedFloat, 3));
	}
	str.Append("Current Acceleration: " + landerAcceleration().ToString() + '\n');
	str.Append("Grav Accel: " + (calcTargetPlanetGravitationalForce()/(monopropellant+drymass)).ToString() + '\n');
	str.Append("Current Position: " + unitpos.ToString() + '\n');
	str.Append("Target Position:" + target_coords.ToString() + '\n');
	str.Append("Current Speed: " + velocity.ToString() + '\n');
	str.Append("Engine throttle: " + FString::SanitizeFloat(engine_throttle) + " => " + FString::SanitizeFloat(target_throttle) + '\n');
	str.Append("Gimball Polar Angle: " + FString::SanitizeFloat(gimball_pos_x) + " => " + FString::SanitizeFloat(target_gimball_pos_x) + '\n');
	str.Append("Gimball Inclination Angle: " + FString::SanitizeFloat(gimball_pos_y) + " => " + FString::SanitizeFloat(target_gimball_pos_y) + '\n');
	str.Append("Etap count: " + FString::FromInt(etapcounter) + '\n');
	str.Append("Delta Time: " + FString::SanitizeFloat(simDeltaTime) + '\n');
	str.Append("Time since last sampling: " + FString::SanitizeFloat(timeSinceLastSampling) + '\n');
	str.Append("Remaining Mission Time: " + FString::SanitizeFloat(remaining_mission_time) + '\n');
	str.Append("Total runtime: " + FString::SanitizeFloat(runtime) + '\n');
	str.Append("Timeouts: " + FString::FromInt(timeover_count) + '\n');
	str.Append("Surface hit " + FString::FromInt(surfacehit_count) + " times\n");
	str.Append("Succesfully landed " + FString::FromInt(landing_count) + "times\n");
	str.Append("------------------------------\n");

	return str;
}
// Called when the game starts or when spawned
void ALanderCode::BeginPlay()
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
	engine_throttle = 40;
	fuel_valve = 50;
	oxidizer_valve = 50;
	battery_charge = 500000;
	target_throttle =60;
	target_ox_valve = 50;
	target_fuel_valve = 50;
	target_gimball_pos_x = 0;
	target_gimball_pos_y = 0;
	target_coords = getRandomTarget();
	starting_coords = getRandomStart();
	pointerToTarget = (target_coords - starting_coords).GetUnsafeNormal();
	unitpos = starting_coords;
	velocity = FVector(0, 0, 0);
	setinhomConst();
	TryConnection();
	Super::BeginPlay();	
}
void ALanderCode::restartGame() {


	if (PythonSocket && PythonSocket->GetConnectionState() == SCS_Connected) {

			PrepareData();
			float* data = DataToSend.GetData();
			uint8* B_DataToSend = reinterpret_cast<uint8*>(data);

			PythonSocket->Send(B_DataToSend, 17 * sizeof(float), BytesSent);
			tBytesSent = BytesSent;


			if (DataToSend[0] == 1.0) is_over_sent = true;
			IsBytesToSend = false;
	}

	on_surface = false;
	etapcounter += 1;
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
	engine_throttle = 40;
	fuel_valve = 50;
	oxidizer_valve = 50;
	battery_charge = 500000;
	target_throttle = 40;
	target_ox_valve = 50;
	target_fuel_valve = 50;
	target_gimball_pos_x = 0;
	target_gimball_pos_y = 0;
	target_coords = getRandomTarget();
	starting_coords = getRandomStart();
	unitpos = starting_coords;
	pointerToTarget = target_coords - unitpos;
	velocity = FVector(0, 0, 0);
	setinhomConst();
	if (PythonSocket && PythonSocket->GetConnectionState() == SCS_Connected) {
		PrepareData();
		float* data = DataToSend.GetData();
		uint8* B_DataToSend = reinterpret_cast<uint8*>(data);

		PythonSocket->Send(B_DataToSend, 17 * sizeof(float), BytesSent);
		tBytesSent = BytesSent;


		if (DataToSend[0] == 1.0) is_over_sent = true;
		IsBytesToSend = false;
	}

}
int ALanderCode::ParseArrivedBytes(uint8* msg) {
	for (int i = 0; i < 3; i++) {
		DataReceivedFloat[i] = *(reinterpret_cast<float*>(msg + 4*i));
	}

	//float read_status = DataReceivedFloat[0]; //if the AI thinks we've landed, it can send a 1, otherwise 0
	float read_throttle = DataReceivedFloat[0]; //the throttle it wants
	float read_gimball_x = DataReceivedFloat[1]; //target gimball x
	float read_gimball_y = DataReceivedFloat[2]; //target gimball y



	/*if (read_status == 1) {
		is_over = true;
	}
	else {*/
		target_throttle = abs(read_throttle - engine_throttle) > min_throttle_change ? read_throttle : engine_throttle;
		target_gimball_pos_x = abs(read_gimball_x - gimball_pos_x) > min_gimball_change ? read_gimball_x : gimball_pos_x;
		target_gimball_pos_y = abs(read_gimball_y - gimball_pos_y) > min_gimball_change ? read_gimball_y : gimball_pos_y;
	//}

	return 0;
}
float ALanderCode::calcGravitationalAcc(float dist, float mass) {
	return gravConst * mass / (pow(dist, 2));
}
FVector ALanderCode::calcAccVectorCelestialBody(FVector dir, float acc) {
	FVector res = dir;
	res.Normalize();
	res *= acc;
	return res;
}
FVector ALanderCode::calcAccVectorCelestialBodyFromMass(float mass, FVector dist) {
	FVector res;
	res[0] = calcGravitationalAcc(dist[0], mass);
	res[1] = calcGravitationalAcc(dist[1], mass);
	res[2] = calcGravitationalAcc(dist[2], mass);
	return res;
}
void ALanderCode::applyNoise() {
	DataToSend[2] = addGaussianNoise(DataToSend[2], DataToSend[2] / 100);
	DataToSend[3] = addGaussianNoise(DataToSend[3], DataToSend[3] / 100 + 0.01);
	DataToSend[4] = addGaussianNoise(DataToSend[4], DataToSend[4] / 100 + 0.01);
	DataToSend[5] = addGaussianNoise(DataToSend[5], DataToSend[5] / 100 + 0.01);
	DataToSend[6] = addGaussianNoise(DataToSend[6], DataToSend[6] / 50);
	DataToSend[7] = addGaussianNoise(DataToSend[7], DataToSend[7] / 100 + 0.01);
	DataToSend[8] = addGaussianNoise(DataToSend[8], DataToSend[8] / 100 + 0.01);

	DataToSend[9] = addGaussianNoise(DataToSend[9], 0.01);

	DataToSend[11] = addGaussianNoise(DataToSend[11], 0.005);
	DataToSend[12] = addGaussianNoise(DataToSend[12], 0.005);

	DataToSend[15] = addGaussianNoise(DataToSend[15], DataToSend[15] / 100);

}
float ALanderCode::roundToGivenDecimal(float f, int decimal) {
	return FMath::DivideAndRoundNearest(f * FMath::Pow(10, decimal), FMath::Pow(10, decimal));
}
void ALanderCode::PrepareData() {
	
	FVector accel = landerAcceleration();
	DataToSend[0] = (is_over ? 1.0 : 0.0); //gamestate: 1=over 0=running
	DataToSend[1] = (is_over ? getScore() : getReward()); //reward for last action (feedback)
	DataToSend[2] = unitpos.Component(2); //height
	DataToSend[3] = accel.Component(0); //acceleration along the x axis
	DataToSend[4] = accel.Component(1); //acceleration along the y axis
	DataToSend[5] = accel.Component(2); //acceleration
	DataToSend[6] = FMath::RoundHalfToZero(FVector::Distance(target_coords, unitpos)); //Distance from the target
	DataToSend[7] = FMath::Acos(pointerToTarget.Component(2)); //inclination and azimuth towards the target
	DataToSend[8] = FMath::Sign(pointerToTarget.Component(1))*FMath::Acos(pointerToTarget.Component(0)/FMath::Sqrt(FMath::Square(pointerToTarget.Component(0))+FMath::Square(pointerToTarget.Component(1))));
	DataToSend[9] = engine_throttle;
	DataToSend[10] = target_throttle;
	DataToSend[11] = gimball_pos_x;
	DataToSend[12] = gimball_pos_y;
	DataToSend[13] = target_gimball_pos_x;
	DataToSend[14] = target_gimball_pos_y;
	DataToSend[15] = monopropellant;
	DataToSend[16] = remaining_mission_time;
	
	if (noisySensor) {
		applyNoise();
	}

	last_checked_pos = unitpos;

	return;
}
FVector ALanderCode::calcVelocity(float DeltaTime){
	velocity += DeltaTime * landerAcceleration();
	return velocity;
}
FVector ALanderCode::calcPosition(float DeltaTime) {
	unitpos += DeltaTime * velocity;
	return unitpos;
}
void ALanderCode::landerTick(float DeltaTime) {

	simDeltaTime = DeltaTime * simspeed;
	timeSinceLastSampling += simDeltaTime;

	if (PythonSocket && PythonSocket->GetConnectionState() == SCS_Connected)
	{

		if (IsBytesToSend && timeSinceLastSampling >= sensorSamplingTime)
		{
			PrepareData();
			timeSinceLastSampling = 0;
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


	}

	
	if (!is_over) {
		pointerToTarget = target_coords - unitpos;
		runtime += simDeltaTime;
		FuelDecrease(simDeltaTime);
		adjustComponents(simDeltaTime);
		calcVelocity(simDeltaTime);
		calcPosition(simDeltaTime);
		if (unitpos[2] < -2) {
			onSurfaceHit();
		}
		else if (unitpos[2] > 5500) {
			is_over = true;
		}
		remaining_mission_time -= simDeltaTime;
		if (remaining_mission_time <= 0) {
			is_over = true;
			++timeover_count;
			remaining_mission_time = 0;
		}
	}
}
// Called every frame
void ALanderCode::Tick(float DeltaTime)
{
	//landerTick(DeltaTime);
	Super::Tick(DeltaTime);
	
}
void ALanderCode::onSurfaceHit() {
	if (velocity.Size() < maxComponentSurfaceHitVel) {
		++landing_count;
	}
	//temporary solution until the AI learns what landing means
	is_over = true;
	velocity = FVector(0, 0, 0);
	++surfacehit_count;
}
FVector ALanderCode::cartesianToPolar(FVector cartesiancoords) {
	FVector polarcoords;
	polarcoords[0] = cartesiancoords.Length();
	polarcoords[1] = FMath::Acos(cartesiancoords[2] / polarcoords[0]);
	polarcoords[2] = FMath::Sign(cartesiancoords[1]) *
		FMath::Acos(
			cartesiancoords[0]
			/ FMath::Sqrt(
				FMath::Square(cartesiancoords[0])
				+ FMath::Square(cartesiancoords[0])
			)
		);
	return polarcoords;
}
FVector ALanderCode::polarToCartersian(FVector polarcoords) {
	FVector cartesiancoords;
	cartesiancoords[0] = polarcoords[0] * FMath::Sin(polarcoords[1]) * FMath::Cos(polarcoords[2]);
	cartesiancoords[1] = polarcoords[0] * FMath::Sin(polarcoords[1]) * FMath::Sin(polarcoords[2]);
	cartesiancoords[2] = polarcoords[0] * FMath::Cos(polarcoords[1]);
	return cartesiancoords;
}
// Called to bind functionality to input
void ALanderCode::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}
FString ALanderCode::floatArrayToString(TArray<float> arr, int len) {
	FString str = "[ ";

	for (int i = 0; i < len; i++) {
		str += FString::SanitizeFloat(arr[i]);
		str += "; ";
	}
	str += "]\n";
	return str;
}
float ALanderCode::getPlanetEllipsoidRadius(FVector polarcoords) {
	float R = (polarRadius * equatorialRadius)
		/ (FMath::Sqrt(
			FMath::Square(
				equatorialRadius * FMath::Sin(polarcoords[2])
			) +
			FMath::Square(
				polarRadius * FMath::Sin(polarcoords[2])
			)
		)
	);
	return R;
}
float ALanderCode::getGeopontentialAltitude(FVector polarcoords) {
	float h = polarcoords[0];
	float r = getPlanetEllipsoidRadius(polarcoords);
	return r * h / (r + h);
}
void ALanderCode::getISAData(float geoPotH, float p_ref, float t_ref, float h_ref, float rho_ref, float lapse_rate) {
	atm_temperature = t_ref + (geoPotH - h_ref) * lapse_rate;

	if (lapse_rate != 0)
	{
		atm_pressure = p_ref * FMath::Pow(atm_temperature / t_ref, -rho_ref * 9.80665 / (lapse_rate * p_ref) * t_ref);
	}
	else
	{
		atm_pressure = p_ref * FMath::Exp(-rho_ref * 9.80665 / p_ref * (geoPotH - h_ref));
	}

	atm_density = rho_ref * t_ref / atm_temperature * atm_pressure / p_ref;
	atm_speedOfSound = FMath::Sqrt(1.4 * atm_pressure / atm_density);

	//derived from two-parameter Sutherland's formula
	float dynamicViscosity = 0.0075117105 * atm_temperature / (t_ref * (atm_temperature + 120)) * FMath::Sqrt(atm_temperature / t_ref);
	atm_kinematicViscosity = dynamicViscosity / atm_density;
}
void ALanderCode::getAthmosphericData(float geopotAlt) {
	float h = geopotAlt;

	//reference values
	float p_ref, t_ref, h_ref, rho_ref, lapse_rate;

	//TODO: make it a proper model, if else is not good
	if (h < 11000)
	{
		h_ref = 0;
		p_ref = 101325;
		t_ref = 288.15;
		rho_ref = 1.225;
		lapse_rate = -0.0065;
	}
	else if (h < 20000)
	{
		h_ref = 11000;
		p_ref = 22632.0405963472;
		t_ref = 216.65;
		rho_ref = 0.363917651;
		lapse_rate = 0;
	}
	else if (h < 32000)
	{
		h_ref = 20000;
		p_ref = 5474.87766066002;
		t_ref = 216.65;
		rho_ref = 8.8034687287099E-2;
		lapse_rate = 0.001;
	}
	else if (h < 47000)
	{
		h_ref = 32000;
		p_ref = 868.015837749373;
		t_ref = 228.65;
		rho_ref = 1.3224965380505E-2;
		lapse_rate = 0.0028;
	}
	else if (h < 51000)
	{
		h_ref = 47000;
		p_ref = 110.905784553924;
		t_ref = 270.65;
		rho_ref = 1.427526789758E-3;
		lapse_rate = 0;
	}
	else if (h < 71000)
	{
		h_ref = 51000;
		p_ref = 66.9385353730403;
		t_ref = 270.65;
		rho_ref = 8.61601158946E-4;
		lapse_rate = -0.0028;
	}
	else if (h < 84852)
	{
		h_ref = 71000;
		p_ref = 3.95639275458239;
		t_ref = 214.65;
		rho_ref = 6.4210581837E-5;
		lapse_rate = -0.002;
	}
	else
	{
		//unofficial extrapolation from here...
		//This extrapolation approximates the 
		//NRLMSISE Standard Atmosphere Model
		//temperature data to at least 150km.
		//Note that at this altitude the composition and
		//thus the average molecular weight
		//of air changes considerably, so at some point
		//the ISA atmospheric model calculations become ivalid.
		h_ref = 84852;
		p_ref = 0.373380371004539;
		t_ref = 186.946;
		rho_ref = 6.957823471E-6;
		lapse_rate = 0.008;
	}

	getISAData(h, p_ref, t_ref, h_ref, rho_ref, lapse_rate);
}