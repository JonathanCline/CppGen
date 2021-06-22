#include <CppGen.h>

#include <JCGenXML.h>


#include <vector>
#include <string_view>
#include <iostream>
#include <filesystem>
#include <ranges>
#include <algorithm>
#include <array>

#define IMPORT_NAME __declspec(dllimport)

struct IMPORT_NAME PROJECT_NAMESPACE::SerialFormat_XML;



using namespace PROJECT_NAMESPACE;

int main(int _nargs, char* _args[])
{
	{
		std::unique_ptr<SerialFormat_XML> _xmlFormat{ new SerialFormat_XML{} };
		load_serial_format(std::move(_xmlFormat));
	};

	path _input = SOURCE_ROOT "tests/parse/input.xml";
	path _output = SOURCE_ROOT "output.h";

	const auto _spec = parse_xml(_input);
	if (!_spec)
	{
		return (int)_spec.err;
	};
	return (int)generate(*_spec.value, _output);







	const auto _cmdArgs = std::views::counted(_args, _nargs) | std::views::drop(1);
	for (auto a : _cmdArgs)
	{
		std::cout << a << '\n';
	};

	if (_nargs < 3)
	{
		std::cout << R"(
usage:
	<inputPath> <outputPath>
)";
		return 0;
	};

	namespace fs = std::filesystem;


	return 0;
};
