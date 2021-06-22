#pragma once

#include "CppGen.h"

namespace pugi
{
	class xml_node;
};

namespace PROJECT_NAMESPACE
{
	struct GenerationSpec;
	
	result<GenerationSpecHandle> parse_xml(pugi::xml_node _root);
	result<GenerationSpecHandle> parse_xml(std::filesystem::path _path);
};
