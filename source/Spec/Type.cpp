#include "Type.h"

#include <cassert>

namespace PROJECT_NAMESPACE
{
	bool TypeDefLedger::contains(const std::string& _name) const noexcept
	{
		return this->ledger_.contains(_name);
	};
	TypeDef* TypeDefLedger::define(const std::string& _name)
	{
		auto it = this->ledger_.find(_name);
		if (it == this->ledger_.end())
		{
			const auto [_iter, _good] = this->ledger_.insert({ _name, TypeDef{ _name } });
			assert(_good);
			it = _iter;
		};
		return &it->second;
	};
	void TypeDefLedger::clear() noexcept
	{
		this->ledger_.clear();
	};

};
