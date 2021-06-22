#include <CppGen.h>

#include <vector>
#include <string_view>
#include <iostream>
#include <filesystem>
#include <ranges>
#include <algorithm>
#include <array>

using namespace PROJECT_NAMESPACE;



int main(int _nargs, char* _args[])
{
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

	fs::path _input{ _args[1] };
	fs::path _output{ _args[2] };
	


	return 0;
};
