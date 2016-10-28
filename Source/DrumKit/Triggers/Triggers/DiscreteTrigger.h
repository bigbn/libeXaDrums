/*
 * Conditioner.h
 *
 *  Created on: 4 May 2015
 *      Author: jeremy
 */

#ifndef RASPIDRUMS_SOURCE_DRUMKIT_TRIGGER_H_
#define RASPIDRUMS_SOURCE_DRUMKIT_TRIGGER_H_

#include "Trigger.h"
#include "../TriggerParameters.h"

#include <chrono>
#include <iostream>
#include <vector>
#include <memory>

#include <cmath>


namespace DrumKit
{

	class DiscreteTrigger : public Trigger
	{

	public:

		DiscreteTrigger(TriggerParameters triggerParams);
		virtual ~DiscreteTrigger();

		virtual void Refresh();
		//virtual bool Trig(short value, float& strength);
		//bool GetState() const { return trig; }


	private:

		//mutable std::mutex triggerMutex;

	};



}

#endif /* RASPIDRUMS_SOURCE_DRUMKIT_TRIGGER_H_ */
