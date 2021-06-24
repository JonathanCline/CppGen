#include <CppGen.h>

#include <JCGenXML.h>
#include <JCGenLua.h>

#include <vector>
#include <string_view>
#include <iostream>
#include <filesystem>
#include <ranges>
#include <algorithm>
#include <array>
#include <fstream>
#include <unordered_map>
#include <span>
#include <string>
#include <cassert>


namespace ns = PROJECT_NAMESPACE;

using format_typelist = std::tuple<
	ns::JCGenLua,
	ns::JCGenXML
>;







using namespace PROJECT_NAMESPACE;

struct usage_t
{
	constexpr static auto value = "<input/file.path> [<output/file/path>] [-<options>...]";
	friend inline std::ostream& operator<<(std::ostream& _ostr, const usage_t& _val)
	{
		return _ostr << _val.value;
	};
};
constexpr inline usage_t usage{};





template <typename T> struct array_size;

template <typename T, size_t N>
struct array_size<T[N]> : std::integral_constant<size_t, N> {};
template <template <typename T, size_t N> typename ArrayT, typename T, size_t N>
struct array_size<ArrayT<T, N>> : std::integral_constant<size_t, N> {};

template <typename T>
constexpr inline size_t array_size_v = array_size<T>::value;






struct ParsedCommandLine
{
	std::filesystem::path execpath;
	std::vector<std::string> args;
	std::unordered_map<std::string, std::string> ops;
};

struct ArgGenerator
{
	using span_type = std::span<char*, std::dynamic_extent>;
	using iterator = typename span_type::iterator;;

	constexpr const char* next() noexcept
	{
		auto& _at = this->at_;
		if (_at != this->span_.end())
		{
			return *_at++;
		}
		else
		{
			return "";
		};
	};
	constexpr auto operator()() noexcept
	{
		return this->next();
	};

	constexpr ArgGenerator(int _max, char* _args[]) :
		span_{ _args, _max }, at_{ this->span_.begin() }
	{};

	span_type span_;
	iterator at_;
};



std::string substr_after_char(const std::string& _str, const std::string_view& _after, size_t _offset = 0)
{
	const auto _pos = _str.find_first_not_of(_after, _offset);
	if (_pos != _str.npos)
	{
		return _str.substr(_pos, _str.size() - _pos);
	}
	else
	{
		return std::string{};
	};
};


std::string tolower(const std::string& _str)
{
	using string_type = std::string;
	using traits_type = typename string_type::traits_type;

	string_type _out{ _str };
	for (auto& o : _out)
	{
		const auto _lower = traits_type::to_char_type(std::tolower(traits_type::to_int_type(o)));
		o = _lower;
	};
	return _out;
};




ParsedCommandLine parse_cli(int _nargs, char* _rawArgArray[])
{
	ParsedCommandLine _out{};
	auto _argin = ArgGenerator{ _nargs, _rawArgArray };
	_out.execpath = _argin();

	std::string _arg = _argin();
	while(!_arg.empty())
	{
		if (_arg.starts_with('-'))
		{
			const auto _op = substr_after_char(_arg, "-");
			_arg = _argin();
			if (!_arg.starts_with('-'))
			{
				_out.ops.insert({ tolower(_op), _arg });
			}
			else
			{
				_out.ops.insert({ tolower(_op), {} });
				continue;
			};
		}
		else
		{
			_out.args.push_back(_arg);
		};
		_arg = _argin();
	};

	return _out;
};

ns::parse::ParseFunction get_parse_function(std::string _str)
{
	const static std::unordered_map<std::string, ns::parse::ParseFunction> ledger
	{
		{ "lua", ns::parse::parse_lua },
		{ "xml", ns::parse::parse_xml }
	};
	const auto it = ledger.find(_str);
	return (it != ledger.end()) ? it->second : nullptr;
};



int invoke_cppgen(ns::parse::ParseFunction _parser, const ns::path& _input, const ns::path& _output)
{
	assert(_parser);

	std::ifstream _file{ _input };
	const auto _parsed = (*_parser)(_file);
	if (!_parsed) [[unlikely]]
	{
		std::cout << "Failed to parse input file";
		return (int)_parsed.err;
	};

	const auto _processed = ns::process(_parsed.value);
	if (!_processed) [[unlikely]]
	{
		std::cout << "Failed to process parsed input spec";
		return (int)_processed.err;
	};

	const auto _result = ns::generate(*_processed.value, _output);
	if (_result != ns::error::none)
	{
		std::cout << "Failed to generate spec";
	};
	return (int)_result;
};

template <std::derived_from<ns::SerialFormat> T>
bool load_format()
{
	return ns::load_serial_format(std::unique_ptr<ns::SerialFormat>{ new T{} });
};

template <template <typename... Ts> typename TupT, typename... Ts>
size_t load_formats_impl(TupT<Ts...>* _tup)
{
	return ((size_t)load_format<Ts>() + ...);
};

size_t load_formats()
{
	format_typelist* _ptr = nullptr;
	return load_formats_impl(_ptr);
};

int rmain(int _nargs, char* _argsv[])
{
	load_formats();

	const auto _cliArgs = parse_cli(_nargs, _argsv);
	const auto [_exec, _args, _ops] = _cliArgs;

	if (_ops.contains("h") || _ops.contains("help"))
	{
		std::cout << usage;
		return 0;
	};

	if (_args.empty())
	{
		std::cout << "No input path specified. Use -h or -help to see usage.";
		return 1;
	};
	
	const ns::path _input = _args.front();
	ns::path _output{};

	ns::parse::ParseFunction _parseFun = nullptr;
	{
		const auto _end = _ops.end();
		auto _it = _end;
		
		if ((_it = _ops.find("f")) != _end || (_it = _ops.find("format")) != _end)
		{
			const auto _format = tolower(_it->second);
			_parseFun = get_parse_function(_format);
		};
	};

	// Try to determine the format using the input path
	if (!_parseFun)
	{
		auto _extension = _input.extension().string();
		if (!_extension.empty())
		{
			_extension.erase(_extension.begin());
			_parseFun = get_parse_function(_extension);
		};
	};

	// Ensure a parser was set
	if (!_parseFun)
	{
		std::cout << "Input format could not be determined, set it using -f <formatName> or -format <formatName>";
		return 2;
	};

	// Check for output path provided
	if (_args.size() >= 2)
	{
		_output = _args.at(1);
	};

	// If no output path, autoset based on input path
	if (_output.empty())
	{
		_output = _input.parent_path();
		_output.append("output");
	};

	// Final invoke
	return invoke_cppgen(_parseFun, _input, _output);
};



int main(int _nargs, char* _argsv[])
{
	using namespace std::string_literals;

	ns::path _path{ SOURCE_ROOT "/tests/parse_lua/input.lua" };
	auto _args = std::array
	{
		std::string{ _argsv[0] },
		_path.string(),
		std::string{ SOURCE_ROOT "/output" }
	};

	std::array<char*, array_size_v<decltype(_args)>> _carrays{ };
	auto it = _carrays.begin();
	for (auto& a : _args)
	{
		*it++ = a.data();
	};

	return rmain((int)_carrays.size(), _carrays.data());
};


