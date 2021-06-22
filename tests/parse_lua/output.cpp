#include "output.h"

// generated on 2021-06-22 02:46:51.2242405 CDT

#include <lua.hpp>
#include <pugixml.hpp>
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
::pugi::xml_node person::serialize(::pugi::xml_node _ctx, person _val)
{
	auto& _node = _ctx;
	_node.set_name("person");
	{
		const auto& v = _val.name;
		auto m = _node.append_child("name");
		m.text() = v.c_str();
	};
	{
		const auto& v = _val.age;
		auto m = _node.append_child("age");
		m.text() = v;
	};
	return _node;
};
::std::optional<person> person::deserialize(::pugi::xml_node _ctx)
{
	auto& _node = _ctx;
	if (std::string{_node.name()} != "person")
	{
		return std::nullopt;
	};
	
	person _out{};
	{
		auto v = _node.child("name");
		auto& m = _out.name;
		m = v.text().as_string();
	};
	{
		auto v = _node.child("age");
		auto& m = _out.age;
		m = v.text().as_int();
	};
	return _out;
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
		const auto& v = _val.texture;
		lua_pushstring(_lua, v.c_str());
		lua_setfield(_lua, _idx + 1, "texture");
	};
	{
		const auto& v = _val.price;
		lua_pushinteger(_lua, v);
		lua_setfield(_lua, _idx + 1, "price");
	};
	{
		const auto& v = _val.height;
		lua_pushinteger(_lua, v);
		lua_setfield(_lua, _idx + 1, "height");
	};
	
	lua_setfield(_lua, _idx, "fields")
	return 1;
};
::std::optional<building> building::deserialize(lua_State* _ctx, int _idx)
{
};
::pugi::xml_node building::serialize(::pugi::xml_node _ctx, building _val)
{
	auto& _node = _ctx;
	_node.set_name("building");
	{
		const auto& v = _val.texture;
		auto m = _node.append_child("texture");
		m.text() = v.c_str();
	};
	{
		const auto& v = _val.price;
		auto m = _node.append_child("price");
		m.text() = v;
	};
	{
		const auto& v = _val.height;
		auto m = _node.append_child("height");
		m.text() = v;
	};
	return _node;
};
::std::optional<building> building::deserialize(::pugi::xml_node _ctx)
{
	auto& _node = _ctx;
	if (std::string{_node.name()} != "building")
	{
		return std::nullopt;
	};
	
	building _out{};
	{
		auto v = _node.child("texture");
		auto& m = _out.texture;
		m = v.text().as_string();
	};
	{
		auto v = _node.child("price");
		auto& m = _out.price;
		m = v.text().as_int();
	};
	{
		auto v = _node.child("height");
		auto& m = _out.height;
		m = v.text().as_int();
	};
	return _out;
};
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
::pugi::xml_node your_mom::serialize(::pugi::xml_node _ctx, your_mom _val)
{
	auto& _node = _ctx;
	_node.set_name("your_mom");
	{
		const auto& v = _val.weight;
		auto m = _node.append_child("weight");
		m.text() = v;
	};
	return _node;
};
::std::optional<your_mom> your_mom::deserialize(::pugi::xml_node _ctx)
{
	auto& _node = _ctx;
	if (std::string{_node.name()} != "your_mom")
	{
		return std::nullopt;
	};
	
	your_mom _out{};
	{
		auto v = _node.child("weight");
		auto& m = _out.weight;
		m = v.text().as_int();
	};
	return _out;
};
