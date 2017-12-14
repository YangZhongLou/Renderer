/*
* This code is licensed under the MIT license (MIT) (http://opensource.org/licenses/MIT)
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
		void ExitFatal(std::string message, std::string caption, bool silent);
	}
}
