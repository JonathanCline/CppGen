#include "CppGen.h"

#include "Generate/SerialFormat.h"
#include "Generate/GenSpec.h"

#include "Util/Format.h"

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

namespace PROJECT_NAMESPACE
{
	result<GenerationSpecHandle> process(const parse::ParseSpec& _spec)
	{
		using error = PROJECT_NAMESPACE::error;
		result<GenerationSpecHandle> _outResult{};
		_outResult.err = error::none;
		_outResult.value = GenerationSpecHandle{ new GenerationSpec{} };
		
		auto& _out = *_outResult.value;
		
		// This should be auto determined based on which types are used by the ParseSpec
		_out.include.push_back(IncludeSpec{ .name = "string", .type = IncludeSpec::External });
		_out.include.push_back(IncludeSpec{ .name = "optional", .type = IncludeSpec::External });
		// </comment>
		
		for (auto& f : _spec.formats)
		{
			auto _serial = get_serial_format(f.name);
			if (_serial)
			{
				_out.serial.formats.push_back(_serial);
			};
		};

		for (auto& t : _spec.types)
		{
			auto& _types = _out.types;
			
			TypeSpec _type{};
			_type.type = t.name;
			
			for (auto& v : t.variables)
			{
				VariableSpec _var{};
				_var.name = v.name;
				_var.type = v.type;
				_var.value = v.value;
				_type.members.push_back(std::move(_var));
			};

			for (auto& f : _out.serial.formats)
			{
				_type.functions.push_back(f->gen_serialize(_type));
				_type.functions.push_back(f->gen_deserialize(_type));
			};

			_types.push_back(std::move(_type));
		};

		return _outResult;
	};
};
