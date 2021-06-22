#pragma once

#include "CppGen.h"

struct lua_State;

namespace PROJECT_NAMESPACE
{
	struct GenerationSpec;

	result<GenerationSpecHandle> parse_lua(lua_State* _lua, int _index);
	result<GenerationSpecHandle> parse_lua(std::filesystem::path _path);
};
