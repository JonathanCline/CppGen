#include "JCGenXML.h"

#include <unordered_map>
#include <format>

namespace jcgen
{
	namespace
	{
		std::string serialize_direct(const std::string_view& _node, const std::string_view& _var)
		{
			return std::format("{}.text() = {};", _node, _var);
		};
		std::string serialize_std_string(const std::string_view& _node, const std::string_view& _var)
		{
			return serialize_direct(_node, std::format("{}.c_str()", _var));
		};

		using SerialGen = std::string(*)(const std::string_view&, const std::string_view&);
		const inline std::unordered_map<std::string, SerialGen> serial_gen
		{
			{ "std::string", &serialize_std_string },
			{ "int", &serialize_direct },
			{ "unsigned", &serialize_direct },
			{ "bool", &serialize_direct },
		};


		std::string deserialize_int(const std::string_view& _node, const std::string_view& _var)
		{
			return std::format("{} = {}.text().as_int();", _var, _node);
		};
		std::string deserialize_std_string(const std::string_view& _node, const std::string_view& _var)
		{
			return std::format("{} = {}.text().as_string();", _var, _node);
		};

		using DeserialGen = std::string(*)(const std::string_view&, const std::string_view&);
		const inline std::unordered_map<std::string, DeserialGen> deserial_gen
		{
			{ "std::string", &deserialize_std_string },
			{ "int", &deserialize_int }
		};
	};



	FunctionBody SerialFormat_XML::gen_serialize_body(const TypeSpec& _type)
	{
		const std::string_view _valueName = "_val";
		const std::string_view _nodeName = "_node";
		const std::string_view _aliasName = "v";
		const std::string_view _memberNodeName = "m";

		FunctionBody _lines
		{
			std::format("auto& {} = _ctx;", _nodeName),
			std::format("{}.set_name(\"{}\");", _nodeName, _type.type)
		};

		const auto& _serializeGenerator = serial_gen;
		for (auto& m : _type.members)
		{
			auto it = _serializeGenerator.find(m.type);
			if (it != _serializeGenerator.end())
			{
				_lines.push_back("{");
				_lines.push_back(std::format("\tconst auto& {} = {}.{};", _aliasName, _valueName, m.name));
				_lines.push_back(std::format("\tauto {} = {}.append_child(\"{}\");", _memberNodeName, _nodeName, m.name));
				_lines.push_back(std::format("\t{}", (*it->second)(_memberNodeName, _aliasName)));
				_lines.push_back("};");
			};
		};

		_lines.push_back(std::format("return {};", _nodeName));
		return _lines;
	};
	FunctionBody SerialFormat_XML::gen_deserialize_body(const TypeSpec& _type)
	{
		const std::string_view _nodeName = "_node";
		const std::string_view _aliasName = "v";
		const std::string_view _memberName = "m";
		const std::string_view _outputName = "_out";
		FunctionBody _lines
		{
			std::format("auto& {} = _ctx;", _nodeName),
			std::format("if (std::string{{{}.name()}} != \"{}\")", _nodeName, _type.type),
			std::format("{{"),
			std::format("\treturn std::nullopt;"),
			std::format("}};"),
			{},
			std::format("{} {}{{}};", _type.type, _outputName)
		};

		const auto& _deserializeGenerator = deserial_gen;
		for (auto& m : _type.members)
		{
			auto it = _deserializeGenerator.find(m.type);
			if (it != _deserializeGenerator.end())
			{
				_lines.push_back("{");
				_lines.push_back(std::format("\tauto {} = {}.child(\"{}\");", _aliasName, _nodeName, m.name));
				_lines.push_back(std::format("\tauto& {} = {}.{};", _memberName, _outputName, m.name));
				_lines.push_back(std::format("\t{}", (*it->second)(_aliasName, _memberName)));
				_lines.push_back("};");
			};
		};
		_lines.push_back(std::format("return {};", _outputName));
		return _lines;
	};

	SerialFormat_XML::SerialFormat_XML() :
		SerialFormat{ "xml" }
	{
		auto& _fmt = *this;
		IncludeSpec _include{};
		_include.name = "pugixml.hpp";
		_include.type = _include.External;
		_fmt.include_source.push_back(_include);

		ForwardDeclSpec _fwd{};
		_fwd.type = "namespace pugi{ class xml_node; }";
		_fmt.forward_decl.push_back(_fwd);

		_fmt.serial_type = "::pugi::xml_node";
		_fmt.serial_context_type = _fmt.serial_type;
	};
};
