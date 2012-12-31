#ifndef NIL_TIME_HPP
#define NIL_TIME_HPP

#include <string>

#include "types.hpp"

namespace nil
{
	ulong time();
	ullong milliseconds();
	std::string timestamp();
	ullong boot_time();
}

#endif
