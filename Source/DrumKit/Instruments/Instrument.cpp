/*
 * Instrument.cpp
 *
 *  Created on: 15 Nov 2015
 *      Author: jeremy
 */


#include "Instrument.h"

namespace DrumKit
{

	Instrument::Instrument(InstrumentParameters parameters, std::shared_ptr<Sound::SoundBank> sb)
	: parameters(parameters), soundBank(sb)
	{


		return;
	}

	Instrument::~Instrument()
	{

		return;
	}






}

