#pragma once

// generated on 2021-06-24 00:11:21.7481377 CDT

#include <string>
#include <optional>
namespace pugi{ class xml_node; };
struct building 
{
	int price{1000};
	std::string texture;
	int height{16};
	static ::pugi::xml_node serialize(::pugi::xml_node _ctx, building _val);
	static ::std::optional<building> deserialize(::pugi::xml_node _ctx);
};
