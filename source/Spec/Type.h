#pragma once

#include <string>
#include <unordered_map>

namespace PROJECT_NAMESPACE
{
	class TypeDef
	{
	public:
		const std::string name;
	};

	class TypeDefLedger
	{
	public:
		bool contains(const std::string& _name) const noexcept;
		TypeDef* define(const std::string& _name);
		void clear() noexcept;
	private:
		std::unordered_map<std::string, TypeDef> ledger_;
	};

};
