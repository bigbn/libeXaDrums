/*
 * Conditioner.h
 *
 *  Created on: 4 May 2015
 *      Author: jeremy
 */

#ifndef RASPIDRUMS_SOURCE_DRUMKIT_TRIGGER_H_
#define RASPIDRUMS_SOURCE_DRUMKIT_TRIGGER_H_

#include "../../Sound/Mixer.h"
#include "Trigger.h"
#include "TriggerParameters.h"

#include <chrono>
#include <iostream>
#include <vector>
#include <memory>

#include <cmath>

using namespace std::chrono;

namespace DrumKit
{

	class DrumTrigger : public Trigger
	{

	public:

		DrumTrigger(TriggerParameters triggerParams);

		virtual bool Trig(short value, float& strength);
		//bool GetState() const { return trig; }

		virtual ~DrumTrigger();

	private:



	};



}

#endif /* RASPIDRUMS_SOURCE_DRUMKIT_TRIGGER_H_ */