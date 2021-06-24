#pragma once

// generated on 2021-06-24 02:34:47.4830398 CDT

#include <string>
#include <optional>
struct lua_State;
namespace pugi{ class xml_node; };
struct person 
{
	int age;
	std::string name{peter};
	static int serialize(lua_State* _ctx, person _val);
	static ::std::optional<person> deserialize(lua_State* _ctx, int _idx);
	static ::pugi::xml_node serialize(::pugi::xml_node _ctx, person _val);
	static ::std::optional<person> deserialize(::pugi::xml_node _ctx);
};
struct building 
{
	std::string texture;
	int height{16};
	int price{1000};
	static int serialize(lua_State* _ctx, building _val);
	static ::std::optional<building> deserialize(lua_State* _ctx, int _idx);
	static ::pugi::xml_node serialize(::pugi::xml_node _ctx, building _val);
	static ::std::optional<building> deserialize(::pugi::xml_node _ctx);
};
struct your_mom 
{
	int weight{10000000};
	static int serialize(lua_State* _ctx, your_mom _val);
	static ::std::optional<your_mom> deserialize(lua_State* _ctx, int _idx);
	static ::pugi::xml_node serialize(::pugi::xml_node _ctx, your_mom _val);
	static ::std::optional<your_mom> deserialize(::pugi::xml_node _ctx);
};
