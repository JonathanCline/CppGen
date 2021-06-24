#define TEST_GEN false

#if TEST_GEN

#include <CppGen.h>
#include "output.h"
#include <pugixml.hpp>
#include <iostream>

int main()
{
	pugi::xml_document _xml{};
	auto _root = _xml.append_child("root");

	building _sadFactory{};
	_sadFactory.height = 69;
	_sadFactory.price = 1000000;
	_sadFactory.texture = "sad_factory.png";

	std::cout << "Serialized:\n";
	const auto _serializedBuilding = building::serialize(_root.append_child(), _sadFactory);
	_xml.print(std::cout);

	std::cout << "\nAfter Cycle:\n";
	auto _desbdg = building::deserialize(_serializedBuilding);
	auto _resbdg = building::serialize(_root.append_child(), *_desbdg);
	_xml.print(std::cout);

	return 0;
};

#else

#include <JCGenXML.h>
#include <CppGen.h>

#include <fstream>

namespace ns = PROJECT_NAMESPACE;

int main()
{
	ns::load_serial_format(std::unique_ptr<ns::SerialFormat>{ new ns::JCGenXML{} });

	const auto _input = SOURCE_ROOT "input.xml";
	const auto _output = SOURCE_ROOT "output.h";

	std::ifstream _ifstr{ _input };
	auto _spec = ns::parse::parse_xml(_ifstr);
	if (!_spec)
	{
		return (int)_spec.err;
	};

	auto _proc = ns::process(_spec.value);
	if (!_proc)
	{
		return (int)_proc.err;
	};

	return (int)ns::generate(*_proc.value, _output);
};

#endif