#include "XMLParse.h"

#include "Generate/SerialFormat.h"
#include "Generate/GenSpec.h"

#include <pugixml.hpp>

namespace PROJECT_NAMESPACE
{
	result<GenerationSpecHandle> parse_xml(pugi::xml_node _root)
	{
		result<GenerationSpecHandle> _outResult{};
		_outResult.value = GenerationSpecHandle{ new GenerationSpec{} };
		auto& _out = *_outResult.value;
		{
			const auto _serialNode = _root.child("serial");
			auto& _serial = _out.serial;
			for (auto n : _serialNode)
			{
				_serial.formats.push_back(get_serial_format(n.name()));
			};
			std::erase(_serial.formats, nullptr);
		};

		for (auto n : _root)
		{
			std::string _name = n.name();
			if (_name == "define")
			{
				TypeSpec _type{};
				_type.type = n.attribute("name").as_string();
				for (auto fnode : n)
				{
					std::string _fnodeName = fnode.name();
					if (_fnodeName == "var")
					{
						VariableSpec _var{};
						_var.type = fnode.attribute("type").as_string();
						_var.name = fnode.attribute("name").as_string();
						_var.value = fnode.text().as_string();
						_type.members.push_back(std::move(_var));
					};
				};

				for (auto sf : _out.serial.formats)
				{
					_type.functions.push_back(sf->gen_serialize(_type));
					_type.functions.push_back(sf->gen_deserialize(_type));
				};

				_out.types.push_back(std::move(_type));
			}
			else if (_name == "include")
			{
				IncludeSpec _include{};
				_include.name = n.text().as_string();
				_include.type = (IncludeSpec::IncludeType)!n.attribute("external").as_bool();
				_out.include.push_back(std::move(_include));
			};
		};
		return _outResult;
	};
	result<GenerationSpecHandle> parse_xml(std::filesystem::path _path)
	{
		result<GenerationSpecHandle> _out{};
		_out.err = _out.none;

		pugi::xml_document _xml{};
		const auto _result = _xml.load_file(_path.c_str());
		if (_result.status != pugi::xml_parse_status::status_ok)
		{
			_out.err = _out.bad_source_path;
			return _out;
		};

		_out = parse_xml(_xml.first_child());
		{
			IncludeSpec _include{};
			_include.name = "optional";
			_include.type = _include.External;
			_out.value->include.push_back(_include);
		};
		return _out;
	};
};
