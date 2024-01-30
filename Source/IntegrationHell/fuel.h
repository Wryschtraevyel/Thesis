#pragma once

#include <vector>

enum Monopropellant {
	Hydrazine,
	Nitromethane,
	ADN,
	Ethylane_Oxide,
	HAN
};

enum Oxidizer {
	Hydrogen_peroxide,
	LOX,
	Dinitrogen_tetroxide, 
	Nitric_acid
};

enum Propellant {
	Hydrogen,
	Kerosane,
	RP1,
	Hydrazine,
	Aerozine,
	Dinitrogen_tetroxide,
	UDMH,
	MMH,
	Ethanol,
	Methane,
	Carbon_dioxide,
	Gasoline
};

static class PropellantInformation {
	static std::vector<std::vector<float>> mono = {};
	static std::vector<std::vector<std::vector<float>>> = {};
	static public std::vector<float> getInfo(Monopropellant mp) {
		return mono[mp];
	}
	static public std::vector<float> getInfo(Oxidizer ox, Propellant prop) {
		return mono[prop, ox];
	}

};
