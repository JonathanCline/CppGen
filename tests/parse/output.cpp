#include "output.h"

// generated on 2021-06-24 00:11:21.7481377 CDT

#include <pugixml.hpp>
::pugi::xml_node building::serialize(::pugi::xml_node _ctx, building _val)
{
	auto& _node = _ctx;
	_node.set_name("building");
	{
		const auto& v = _val.price;
		auto m = _node.append_child("price");
		m.text() = v;
	};
	{
		const auto& v = _val.texture;
		auto m = _node.append_child("texture");
		m.text() = v.c_str();
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
		auto v = _node.child("price");
		auto& m = _out.price;
		m = v.text().as_int();
	};
	{
		auto v = _node.child("texture");
		auto& m = _out.texture;
		m = v.text().as_string();
	};
	{
		auto v = _node.child("height");
		auto& m = _out.height;
		m = v.text().as_int();
	};
	return _out;
};
