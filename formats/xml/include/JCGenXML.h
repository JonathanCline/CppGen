#pragma once

#include <JCGenCore.h>

#define EXPORT_NAME __declspec(dllexport)

namespace jcgen
{
	EXPORT_NAME struct JCGenXML : public SerialFormat
	{
	protected:
		FunctionBody gen_serialize_body(const TypeSpec& _type) final;
		FunctionBody gen_deserialize_body(const TypeSpec& _type) final;
	public:
		JCGenXML();
	};

	namespace parse
	{
		EXPORT_NAME ParseResult parse_xml(std::istream& _istr);
	};

};
