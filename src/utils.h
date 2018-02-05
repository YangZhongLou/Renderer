/**
* Author ZhongLou Yang, 2018-2-5
*/
#pragma once

#include <string>
#include <iostream>
#include <assert.h>
#include "defines.h"

namespace Concise
{
	namespace Utils
	{
		void ExitFatal(std::string message, std::string caption, bool silent = false);
	}
}
