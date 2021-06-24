#include "output.h"

// generated on 2021-06-24 01:52:52.0015336 CDT

#include <lua.hpp>
int your_mom::serialize(lua_State* _ctx, your_mom _val)
{
	const auto& _lua = _ctx;
	lua_newtable(_lua);
	const auto _idx = lua_gettop(_lua);
	
	lua_pushstring(_lua, "your_mom");
	lua_setfield(_lua, _idx, "name");
	
	lua_newtable(_lua);
	{
		const auto& v = _val.weight;
		lua_pushinteger(_lua, v);
		lua_setfield(_lua, _idx + 1, "weight");
	};
	
	lua_setfield(_lua, _idx, "fields")
	return 1;
};
::std::optional<your_mom> your_mom::deserialize(lua_State* _ctx, int _idx)
{
};
int building::serialize(lua_State* _ctx, building _val)
{
	const auto& _lua = _ctx;
	lua_newtable(_lua);
	const auto _idx = lua_gettop(_lua);
	
	lua_pushstring(_lua, "building");
	lua_setfield(_lua, _idx, "name");
	
	lua_newtable(_lua);
	{
		const auto& v = _val.height;
		lua_pushinteger(_lua, v);
		lua_setfield(_lua, _idx + 1, "height");
	};
	{
		const auto& v = _val.price;
		lua_pushinteger(_lua, v);
		lua_setfield(_lua, _idx + 1, "price");
	};
	{
		const auto& v = _val.texture;
		lua_pushstring(_lua, v.c_str());
		lua_setfield(_lua, _idx + 1, "texture");
	};
	
	lua_setfield(_lua, _idx, "fields")
	return 1;
};
::std::optional<building> building::deserialize(lua_State* _ctx, int _idx)
{
};
int person::serialize(lua_State* _ctx, person _val)
{
	const auto& _lua = _ctx;
	lua_newtable(_lua);
	const auto _idx = lua_gettop(_lua);
	
	lua_pushstring(_lua, "person");
	lua_setfield(_lua, _idx, "name");
	
	lua_newtable(_lua);
	{
		const auto& v = _val.name;
		lua_pushstring(_lua, v.c_str());
		lua_setfield(_lua, _idx + 1, "name");
	};
	{
		const auto& v = _val.age;
		lua_pushinteger(_lua, v);
		lua_setfield(_lua, _idx + 1, "age");
	};
	
	lua_setfield(_lua, _idx, "fields")
	return 1;
};
::std::optional<person> person::deserialize(lua_State* _ctx, int _idx)
{
};
