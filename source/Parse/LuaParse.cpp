#include "LuaParse.h"

#include "Generate/GenSpec.h"
#include "Generate/SerialFormat.h"

#include <lua.hpp>

#include <type_traits>
#include <concepts>

namespace PROJECT_NAMESPACE
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



	result<GenerationSpecHandle> parse_lua(lua_State* _lua, int _rawIndex)
	{
		result<GenerationSpecHandle> _outResult{};
		_outResult.err = _outResult.none;
		_outResult.value = GenerationSpecHandle{ new GenerationSpec{} };

		const auto _idx = lua_absindex(_lua, _rawIndex);
		auto& _out = *_outResult.value;

		lua_getfield(_lua, _idx, "serial");
		if (lua_istable(_lua, -1))
		{
			lua_foreach(_lua, -1, [&_out](lua_State* _lua) -> int
				{
					const auto _serial = get_serial_format(luaL_checkstring(_lua, -1));
					_out.serial.formats.push_back(_serial);
					return 0;
				});
		};
		std::erase(_out.serial.formats, nullptr);
		lua_settop(_lua, _idx);

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

		lua_getfield(_lua, _idx, "define");
		if (lua_istable(_lua, -1))
		{
			const auto _tableIndex = lua_gettop(_lua);

			lua_pushnil(_lua);
			while (lua_next(_lua, _tableIndex) != 0)
			{
				const auto _name = luaL_checkstring(_lua, -2);
				TypeSpec _type{};
				_type.type = _name;
				
				const auto _fieldsIndex = lua_gettop(_lua);

				lua_pushnil(_lua);
				while (lua_next(_lua, _fieldsIndex) != 0)
				{
					VariableSpec _var{};
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

					_type.members.push_back(std::move(_var));
					lua_pop(_lua, 1);
				};
				lua_settop(_lua, _fieldsIndex - 1);

				for (auto sf : _out.serial.formats)
				{
					_type.functions.push_back(sf->gen_serialize(_type));
					_type.functions.push_back(sf->gen_deserialize(_type));
				};

				_out.types.push_back(std::move(_type));
			};
		};
		lua_settop(_lua, _idx);

		return _outResult;
	};
	result<GenerationSpecHandle> parse_lua(std::filesystem::path _path)
	{
		const auto _pathStr = _path.string();
		auto _lua = luaL_newstate();

		result<GenerationSpecHandle> _out{};
		_out.err = _out.none;

		const auto _error = luaL_dofile(_lua, _pathStr.c_str());
		if (_error != LUA_OK)
		{
			_out.err = _out.bad_source_path;
			lua_close(_lua);
			return _out;
		}
		else if (!lua_istable(_lua, -1))
		{
			_out.err = _out.bad_source_data;
			lua_close(_lua);
			return _out;
		}
		else
		{
			_out = parse_lua(_lua, -1);
			lua_close(_lua);
			return _out;
		};
	};
};
