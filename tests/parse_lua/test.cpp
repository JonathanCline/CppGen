#include <CppGen.h>

namespace ns = PROJECT_NAMESPACE;

int main()
{
	const auto _input = SOURCE_ROOT "input.lua";
	const auto _output = SOURCE_ROOT "output.h";

	auto _spec = ns::parse_lua(_input);
	if (!_spec)
	{
		return (int)_spec.err;
	};

	return (int)ns::generate(*_spec.value, _output);
};
