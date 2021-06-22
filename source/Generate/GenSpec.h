#pragma once

#include "Spec/Type.h"

#include <string>
#include <vector>

namespace PROJECT_NAMESPACE
{
	struct SerialFormat;



	struct IncludeSpec
	{
		enum IncludeType : int
		{
			External = 0,
			Local = 1
		};

		std::string name;
		IncludeType type;
	};

	struct SerialSpec
	{
		std::vector<SerialFormat*> formats;
	};

	struct VariableSpec
	{
		std::string name;
		std::string type;
		std::string value;
	};

	struct FunctionSpec_Declaration
	{
		bool is_static = false;
		std::string return_type;
		std::string name;
		std::vector<VariableSpec> parameters;
	};

	using FunctionBody = std::vector<std::string>;
	struct FunctionSpec
	{
		FunctionSpec_Declaration decl;
		FunctionBody body;
	};

	struct TypeSpec
	{
		std::string type;
		std::vector<VariableSpec> members;
		std::vector<FunctionSpec> functions{};
	};

	struct GenerationSpec
	{
		std::vector<IncludeSpec> include{};
		std::vector<TypeSpec> types{};

		SerialSpec serial{};

		TypeDefLedger typedefs;
	};

	struct ForwardDeclSpec
	{
		std::string type;
	};

};