#pragma once

#include <JCGenCore.h>

#include <filesystem>
#include <memory>
#include <optional>

namespace PROJECT_NAMESPACE
{
	using path = std::filesystem::path;


	struct GenerationSpec;
	namespace impl
	{
		struct GenerationSpecDeleter
		{
			void operator()(GenerationSpec* _genspec);
		};
	};

	using GenerationSpecHandle = std::unique_ptr<GenerationSpec, impl::GenerationSpecDeleter>;
	
	bool load_serial_format(std::unique_ptr<SerialFormat> _format);

	result<GenerationSpecHandle> process(const parse::ParseSpec& _spec);

	error generate(const GenerationSpec& _spec, path _output);
	
};
