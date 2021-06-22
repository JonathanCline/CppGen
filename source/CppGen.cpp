#include "CppGen.h"

#include "Parse/XMLParse.h"
#include "Parse/LuaParse.h"

#include "Generate/SerialFormat.h"
#include "Generate/GenSpec.h"

#include "Util/Format.h"

#include <pugixml.hpp>

#include <fstream>
#include <variant>
#include <string>
#include <string_view>
#include <unordered_map>
#include <array>
#include <ranges>
#include <numeric>
#include <algorithm>
#include <cassert>
#include <memory>

namespace PROJECT_NAMESPACE::impl
{
	void GenerationSpecDeleter::operator()(GenerationSpec* _genspec)
	{
		delete _genspec;
	};
};

namespace PROJECT_NAMESPACE
{
	error generate_header(const GenerationSpec& _spec, std::ostream& _ostr, Tabber& _tabber)
	{
		auto& _tablevel = _tabber.level;

		for (const auto& sf : _spec.serial.formats)
		{
			for (const auto& i : sf->include_header)
			{
				print(_ostr, "{}", i);
			};
		};

		for (const auto& i : _spec.include)
		{
			print(_ostr, "{}", i);
		};

		for (const auto& sf : _spec.serial.formats)
		{
			for (const auto& fwd : sf->forward_decl)
			{
				print(_ostr, "{};\n", fwd);
			};
		};

		for (const auto& s : _spec.types)
		{
			print(_ostr, "{0}struct {1} \n{0}{{\n", _tabber, s.type);
			++_tablevel;
			
			for (const auto& m : s.members)
			{
				print(_ostr, "{}{};\n", _tabber, m);
			};

			for (const auto& f : s.functions)
			{
				print(_ostr, "{}{};\n", _tabber, f.decl);
			};

			--_tablevel;
			print(_ostr, "{}}};\n", _tabber);

		};
		return error::none;
	};
	error generate_source(const GenerationSpec& _spec, std::ostream& _ostr, Tabber& _tabber)
	{
		for (const auto& sf : _spec.serial.formats)
		{
			for (const auto& i : sf->include_source)
			{
				print(_ostr, "{}", i);
			};
		};

		for (const auto& t : _spec.types)
		{
			for (const auto& m : t.functions)
			{
				_ostr << format_function_definition(m, _tabber, &t);
			};
		};

		return error::none;
	};
};

namespace PROJECT_NAMESPACE
{
	error generate(const GenerationSpec& _spec, path _output)
	{
		_output.replace_extension("");

		auto _headerPath = path{ _output }.replace_extension(".h");
		auto _sourcePath = path{ _output }.replace_extension(".cpp");

		std::ofstream _header{ _headerPath };
		std::ofstream _source{ _sourcePath };

		Tabber _tabber{};

		const auto _timestampRaw = std::chrono::system_clock::now();
		const auto _timestamp = std::chrono::zoned_time(std::chrono::current_zone(), _timestampRaw);

		_header << std::format("#pragma once\n\n// generated on {}\n\n", _timestamp);
		generate_header(_spec, _header, _tabber);

		_tabber.level = 0;
		_source << std::format("#include \"{}\"\n\n// generated on {}\n\n", _headerPath.filename().string(), _timestamp);
		generate_source(_spec, _source, _tabber);

		return error::none;
	};
};
