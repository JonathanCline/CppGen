#pragma once

#include <JCGenCore.h>

namespace PROJECT_NAMESPACE
{
	using SerialGenFunction = FunctionBody(*)(const TypeSpec&);
	SerialFormat* get_serial_format(const std::string& _name);
};
