#pragma once

#include "Generate/SerialFormat.h"

#include <unordered_map>
#include <string>
#include <format>

namespace PROJECT_NAMESPACE
{
	struct SerialFormat_Lua : public SerialFormat
	{
	protected:

		static std::string push_int(const std::string_view& _lua, const std::string_view& _var)
		{
			return std::format("lua_pushinteger({}, {})", _lua, _var);
		};
		static std::string push_string(const std::string_view& _lua, const std::string_view& _var)
		{
			return std::format("lua_pushstring({}, {}.c_str())", _lua, _var);
		};

		using PushGen = std::string(*)(const std::string_view&, const std::string_view&);
		static const inline std::unordered_map<std::string, PushGen> push_gen
		{
			{ "int", &push_int },
			{ "std::string", &push_string },
		};

		static std::string pull_int(const std::string_view& _lua, const std::string_view& _var)
		{
			return std::format("{} = luaL_checkinteger({}, -1)", _var, _lua);
		};
		static std::string pull_std_string(const std::string_view& _lua, const std::string_view& _var)
		{
			return std::format("{} = luaL_checkstring({}, -1)", _var, _lua);
		};

		using PullGen = std::string(*)(const std::string_view&, const std::string_view&);
		static const inline std::unordered_map<std::string, PullGen> pull_gen
		{
			{ "int", &pull_int },
			{ "std::string", &pull_std_string }
		};


		FunctionSpec_Declaration gen_deserialize_decl(const TypeSpec& _type) const final
		{
			auto _decl = SerialFormat::gen_deserialize_decl(_type);
			_decl.parameters.push_back(VariableSpec
				{
					.name = "_idx",
					.type = "int"
				});
			return _decl;
		};

		FunctionBody gen_serialize_body(const TypeSpec& _type) final
		{
			constexpr std::string_view _luaName = "_lua";
			constexpr std::string_view _valueName = "_val";
			constexpr std::string_view _tableIndexName = "_idx";
			constexpr std::string_view _aliasName = "v";

			FunctionBody _lines
			{
				std::format("const auto& {} = _ctx;", _luaName),
				std::format("lua_newtable({});", _luaName),
				std::format("const auto {} = lua_gettop({});", _tableIndexName, _luaName),
				"",
				std::format("lua_pushstring({}, \"{}\");", _luaName, _type.type),
				std::format("lua_setfield({}, {}, \"name\");", _luaName, _tableIndexName),
				"",
				std::format("lua_newtable({});", _luaName),
			};

			for (auto& m : _type.members)
			{
				auto it = this->push_gen.find(m.type);
				if (it != this->push_gen.end())
				{
					_lines.push_back("{");
					_lines.push_back(std::format("\tconst auto& {} = {}.{};", _aliasName, _valueName, m.name));
					_lines.push_back(std::format("\t{};", this->push_gen.at(m.type)(_luaName, _aliasName)));
					_lines.push_back(std::format("\tlua_setfield({}, {} + 1, \"{}\");", _luaName, _tableIndexName, m.name));
					_lines.push_back("};");
				};
			};

			_lines.push_back("");
			_lines.push_back(std::format("lua_setfield({}, {}, \"fields\")", _luaName, _tableIndexName));
			_lines.push_back("return 1;");
			return _lines;
		};
		FunctionBody gen_deserialize_body(const TypeSpec& _type) final
		{
			FunctionBody _lines{};
			return _lines;
		};

	public:
		SerialFormat_Lua() :
			SerialFormat{ "lua" }
		{
			auto& _fmt = *this;
			IncludeSpec _include{};
			_include.name = "lua.hpp";
			_include.type = _include.External;
			_fmt.include_source.push_back(_include);

			ForwardDeclSpec _fwd{};
			_fwd.type = "struct lua_State";
			_fmt.forward_decl.push_back(_fwd);

			_fmt.serial_type = "int";
			_fmt.serial_context_type = "lua_State*";
		};
	};
};
