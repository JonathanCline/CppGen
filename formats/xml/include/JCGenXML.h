#pragma once

#include <JCGenCore.h>

#define EXPORT_NAME __declspec(dllexport)

namespace jcgen
{
	struct EXPORT_NAME SerialFormat_XML : public SerialFormat
	{
	protected:
		FunctionBody gen_serialize_body(const TypeSpec& _type) final;
		FunctionBody gen_deserialize_body(const TypeSpec& _type) final;
	public:
		SerialFormat_XML();
	};
};
