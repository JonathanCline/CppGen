#pragma once

// generated on 2021-06-22 01:56:03.9822781 CDT

#include "CppGen.h"
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
