#pragma once

#include <JCGenCore.h>

#ifndef EXPORT_NAME
#define EXPORT_NAME __declspec(dllexport)
#endif

#ifndef DECL_PREFIX
#define DECL_PREFIX __cdecl
#endif

namespace jcgen
{
	EXPORT_NAME struct JCGenLua : public SerialFormat
	{
	protected:
		FunctionSpec_Declaration gen_deserialize_decl(const TypeSpec& _type) const final;
		FunctionBody gen_serialize_body(const TypeSpec& _type) final;
		FunctionBody gen_deserialize_body(const TypeSpec& _type) final;
	
	public:
		JCGenLua();
	};

	namespace parse
	{
		EXPORT_NAME ParseResult DECL_PREFIX parse_lua(std::istream& _istr);
	};
};
