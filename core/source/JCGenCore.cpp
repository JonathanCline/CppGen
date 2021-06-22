#include "JCGenCore.h"

#include <format>

namespace jcgen
{
	VariableSpec SerialFormat::context_param() const noexcept
	{
		return VariableSpec
		{
			.name = "_ctx",
			.type = this->serial_context_type
		};
	};

	FunctionSpec_Declaration SerialFormat::gen_serialize_decl(const TypeSpec& _type) const
	{
		FunctionSpec_Declaration _decl;
		_decl.is_static = true;
		_decl.name = "serialize";
		_decl.return_type = this->serial_type;
		_decl.parameters =
		{
			this->context_param(),
			VariableSpec
			{
				.name = "_val",
				.type = _type.type
			}
		};
		return _decl;
	};
	FunctionSpec_Declaration SerialFormat::gen_deserialize_decl(const TypeSpec& _type) const
	{
		FunctionSpec_Declaration _decl{};
		_decl.is_static = true;
		_decl.name = "deserialize";
		_decl.return_type = std::format("::std::optional<{}>", _type.type);
		_decl.parameters =
		{
			this->context_param()
		};
		return _decl;
	};

	FunctionSpec SerialFormat::gen_serialize(const TypeSpec& _type)
	{
		FunctionSpec _out{};
		_out.decl = this->gen_serialize_decl(_type);
		_out.body = this->gen_serialize_body(_type);
		return _out;
	};
	FunctionSpec SerialFormat::gen_deserialize(const TypeSpec& _type)
	{
		FunctionSpec _out{};
		_out.decl = this->gen_deserialize_decl(_type);
		_out.body = this->gen_deserialize_body(_type);
		return _out;
	};

};
