#pragma once

// generated on 2021-06-24 01:52:52.0015336 CDT

#include <string>
#include <optional>
struct lua_State;
struct your_mom 
{
	int weight{10000000};
	static int serialize(lua_State* _ctx, your_mom _val);
	static ::std::optional<your_mom> deserialize(lua_State* _ctx, int _idx);
};
struct building 
{
	int height{16};
	int price{1000};
	std::string texture;
	static int serialize(lua_State* _ctx, building _val);
	static ::std::optional<building> deserialize(lua_State* _ctx, int _idx);
};
struct person 
{
	std::string name{peter};
	int age;
	static int serialize(lua_State* _ctx, person _val);
	static ::std::optional<person> deserialize(lua_State* _ctx, int _idx);
};
