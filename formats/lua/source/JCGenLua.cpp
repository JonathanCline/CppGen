#include "JCGenLua.h"

#include <lua.hpp>

#include <unordered_map>
#include <string>
#include <format>
#include <ranges>
#include <algorithm>
#include <istream>

namespace jcgen
{

	namespace
	{
		std::string push_int(const std::string_view& _lua, const std::string_view& _var)
		{
			return std::format("lua_pushinteger({}, {})", _lua, _var);
		};
		std::string push_string(const std::string_view& _lua, const std::string_view& _var)
		{
			return std::format("lua_pushstring({}, {}.c_str())", _lua, _var);
		};

		using PushGen = std::string(*)(const std::string_view&, const std::string_view&);
		const inline std::unordered_map<std::string, PushGen> push_gen
		{
			{ "int", &push_int },
			{ "std::string", &push_string },
		};

		std::string pull_int(const std::string_view& _lua, const std::string_view& _var)
		{
			return std::format("{} = luaL_checkinteger({}, -1)", _var, _lua);
		};
		std::string pull_std_string(const std::string_view& _lua, const std::string_view& _var)
		{
			return std::format("{} = luaL_checkstring({}, -1)", _var, _lua);
		};

		using PullGen = std::string(*)(const std::string_view&, const std::string_view&);
		const inline std::unordered_map<std::string, PullGen> pull_gen
		{
			{ "int", &pull_int },
			{ "std::string", &pull_std_string }
		};
	};

	FunctionSpec_Declaration JCGenLua::gen_deserialize_decl(const TypeSpec& _type) const
	{
		auto _decl = SerialFormat::gen_deserialize_decl(_type);
		_decl.parameters.push_back(VariableSpec
			{
				.name = "_idx",
				.type = "int"
			});
		return _decl;
	};

	FunctionBody JCGenLua::gen_serialize_body(const TypeSpec& _type)
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
			auto it = push_gen.find(m.type);
			if (it != push_gen.end())
			{
				_lines.push_back("{");
				_lines.push_back(std::format("\tconst auto& {} = {}.{};", _aliasName, _valueName, m.name));
				_lines.push_back(std::format("\t{};", it->second(_luaName, _aliasName)));
				_lines.push_back(std::format("\tlua_setfield({}, {} + 1, \"{}\");", _luaName, _tableIndexName, m.name));
				_lines.push_back("};");
			};
		};

		_lines.push_back("");
		_lines.push_back(std::format("lua_setfield({}, {}, \"fields\")", _luaName, _tableIndexName));
		_lines.push_back("return 1;");
		return _lines;
	};
	FunctionBody JCGenLua::gen_deserialize_body(const TypeSpec& _type)
	{
		FunctionBody _lines{};
		return _lines;
	};

	JCGenLua::JCGenLua() :
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

namespace jcgen::parse
{
	template <typename T>
	concept lua_cfunction = std::invocable<T, lua_State*> &&
		std::same_as<std::invoke_result_t<T, lua_State*>, int>;

	size_t lua_foreach(lua_State* _lua, int _rawIndex, lua_cfunction auto&& _op)
	{
		const auto _idx = lua_absindex(_lua, _rawIndex);
		const auto _len = lua_rawlen(_lua, _idx);
		for (lua_Integer n = 1; n <= _len; ++n)
		{
			lua_rawgeti(_lua, _idx, n);
			std::invoke(_op, _lua);
		};
		return (size_t)_len;
	};

	ParseResult parse_lua(lua_State* _lua, int _rawIndex)
	{
		ParseResult _outResult{};
		_outResult.err = _outResult.none;
	
		const auto _idx = lua_absindex(_lua, _rawIndex);
		auto& _out = _outResult.value;

		lua_getfield(_lua, _idx, "serial");
		if (lua_istable(_lua, -1))
		{
			lua_foreach(_lua, -1, [&_out](lua_State* _lua) -> int
				{
					const auto _serial = luaL_checkstring(_lua, -1);
					_out.formats.push_back(SerialFormatDecl{ _serial });
					return 0;
				});
		};
		lua_settop(_lua, _idx);

		/*
		lua_getfield(_lua, _idx, "include");
		if (lua_istable(_lua, -1))
		{
			lua_foreach(_lua, -1, [&_out](lua_State* _lua) -> int
				{
					IncludeSpec _include{};

					lua_getfield(_lua, -1, "external");
					luaL_checktype(_lua, -1, LUA_TBOOLEAN);
					const auto _external = lua_toboolean(_lua, -1);
					lua_pop(_lua, 1);

					if (_external)
					{
						_include.type = _include.External;
					}
					else
					{
						_include.type = _include.Local;
					};

					lua_getfield(_lua, -1, "name");
					_include.name = luaL_checkstring(_lua, -1);
					lua_pop(_lua, 1);

					_out.include.push_back(std::move(_include));
					return 0;
				});
		};
		lua_settop(_lua, _idx);
		*/

		lua_getfield(_lua, _idx, "define");
		if (lua_istable(_lua, -1))
		{
			const auto _tableIndex = lua_gettop(_lua);

			lua_pushnil(_lua);
			while (lua_next(_lua, _tableIndex) != 0)
			{
				const auto _name = luaL_checkstring(_lua, -2);
				TypeDecl _type{};
				_type.name = _name;

				const auto _fieldsIndex = lua_gettop(_lua);

				lua_pushnil(_lua);
				while (lua_next(_lua, _fieldsIndex) != 0)
				{
					VariableDecl _var{};
					_var.name = luaL_checkstring(_lua, -2);

					lua_getfield(_lua, -1, "type");
					_var.type = luaL_checkstring(_lua, -1);
					lua_pop(_lua, 1);

					lua_getfield(_lua, -1, "value");
					if (!lua_isnil(_lua, -1))
					{
						_var.value = lua_tostring(_lua, -1);
					};
					lua_pop(_lua, 1);

					_type.variables.push_back(std::move(_var));
					lua_pop(_lua, 1);
				};
				lua_settop(_lua, _fieldsIndex - 1);
				_out.types.push_back(std::move(_type));
			};
		};
		lua_settop(_lua, _idx);

		return _outResult;
	};

	ParseResult parse_lua(std::istream& _istr)
	{
		ParseResult _out{};

		if (!_istr)
		{
			_out.err = _out.bad_source_path;
			return _out;
		};

		std::string _script{};
		{
			constexpr std::streamsize bufferlen = 512;
			char _buffer[bufferlen]{};
			std::ranges::fill(_buffer, 0);
			while (!_istr.eof() && _istr)
			{
				_istr.read(_buffer, bufferlen);
				_script.append(_buffer, _istr.gcount());
			};
			_script.push_back('\0');
		};

		auto _lua = luaL_newstate();
		const auto _result = luaL_dostring(_lua, _script.c_str());
		if (_result != LUA_OK || lua_gettop(_lua) == 0)
		{
			_out.err = _out.bad_source_data;
			const auto _msg = luaL_optstring(_lua, -1, "error");

			return _out;
		};

		return parse_lua(_lua, -1);
	};

};
