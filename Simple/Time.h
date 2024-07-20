#pragma once

#ifndef _TIME_H_
#define _TIME_H_

#include "Module.h"
#include "TimeFunctions.h"
#include "Functions.h"
#include "Variables.h"

#include "Simple_defines.h"

namespace Simple {
	class Time : public Module_Base {
		void InitVars();
		void InitFuncs();
	public:
		void Init() override;
	};
}

#endif // _TIME_H_