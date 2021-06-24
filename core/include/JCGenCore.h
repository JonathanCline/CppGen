#pragma once

#include <string>
#include <vector>
#include <iosfwd>
#include <optional>

namespace jcgen
{
	enum class error : int
	{
		error = -1,
		none = 0,
		bad_source_path = 1,
		bad_source_data = 2
	};

	template <typename T>
	struct result
	{
		using error_t = jcgen::error;
		using enum error;

		constexpr explicit operator bool() const noexcept
		{
			return this->err == none;
		};

		T value;
		error_t err;
	};


	struct VariableSpec
	{
		std::string name;
		std::string type;
		std::string value;
	};

	struct FunctionSpec_Declaration
	{
		std::string name;
		std::string return_type;
		std::vector<VariableSpec> parameters{};
		bool is_static = false;
	};

	using FunctionBody = std::vector<std::string>;
	
	struct FunctionSpec
	{
		FunctionSpec_Declaration decl;
		FunctionBody body;
	};

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

	struct ForwardDeclSpec
	{
		std::string type;
	};

	struct TypeSpec
	{
		std::string type;
		std::vector<VariableSpec> members;
		std::vector<FunctionSpec> functions{};
	};





	struct SerialFormat
	{
	protected:
		VariableSpec context_param() const noexcept;

		virtual FunctionSpec_Declaration gen_serialize_decl(const TypeSpec& _type) const;
		virtual FunctionSpec_Declaration gen_deserialize_decl(const TypeSpec& _type) const;
		virtual FunctionBody gen_serialize_body(const TypeSpec&) = 0;
		virtual FunctionBody gen_deserialize_body(const TypeSpec&) = 0;

	public:
		const std::string& name() const noexcept { return this->name_; };

		FunctionSpec gen_serialize(const TypeSpec& _type);
		FunctionSpec gen_deserialize(const TypeSpec& _type);

		std::vector<IncludeSpec> include_header{};
		std::vector<IncludeSpec> include_source{};

		std::string serial_type;
		std::string serial_context_type;
		std::vector<ForwardDeclSpec> forward_decl{};

		SerialFormat(const std::string& _name) :
			name_{ _name }
		{};
		virtual ~SerialFormat() = default;

	private:
		std::string name_;
	};




	namespace parse
	{
		struct SerialFormatDecl
		{
			std::string name;
		};

		struct VariableDecl
		{
			std::string type;
			std::string name;
			std::string value; // leave empty if no default was provided
		};

		struct TypeDecl
		{
			std::string name;
			std::vector<VariableDecl> variables;
		};

		struct ParseSpec
		{
			std::vector<SerialFormatDecl> formats{};
			std::vector<TypeDecl> types{};
		};

		using ParseResult = result<ParseSpec>;

		using ParseFunction = ParseResult(*)(std::istream& _istr);

	};


};
