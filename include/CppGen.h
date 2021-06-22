#pragma once

#include <ranges>
#include <vector>
#include <filesystem>
#include <memory>

/*
	Raw Input (XML, Lua, ...)
		-> parsed
	Input Specification
		-> processed
	Generation Specification
		-> generated
	Final Output (C++)
*/

namespace PROJECT_NAMESPACE
{
	using path = std::filesystem::path;

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
		using error_t = PROJECT_NAMESPACE::error;
		using enum error;
		
		constexpr explicit operator bool() const noexcept
		{
			return this->err == none;
		};

		T value;
		error_t err;
	};


	struct GenerationSpec;
	namespace impl
	{
		struct GenerationSpecDeleter
		{
			void operator()(GenerationSpec* _genspec);
		};
	};

	using GenerationSpecHandle = std::unique_ptr<GenerationSpec, impl::GenerationSpecDeleter>;
	
	result<GenerationSpecHandle> parse_xml(path _input);
	result<GenerationSpecHandle> parse_lua(path _input);

	error generate(const GenerationSpec& _spec, path _output);

};
