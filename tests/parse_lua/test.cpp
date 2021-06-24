#include <JCGenLua.h>
#include <JCGenXML.h>

#include <CppGen.h>

#include <fstream>

namespace ns = PROJECT_NAMESPACE;

int main()
{
	ns::load_serial_format(std::unique_ptr<ns::SerialFormat>{ new ns::JCGenLua{} });
	ns::load_serial_format(std::unique_ptr<ns::SerialFormat>{ new ns::JCGenXML{} });

	const auto _input = SOURCE_ROOT "input.lua";
	const auto _output = SOURCE_ROOT "output.h";

	std::fstream _ifstr{ _input };
	auto _spec = ns::parse::parse_lua(_ifstr);
	if (!_spec)
	{
		return (int)_spec.err;
	};

	auto _proc = ns::process(_spec.value);
	if (!_proc)
	{
		return (int)_proc.err;
	};

	return (int)ns::generate(*_proc.value, _output);
};
