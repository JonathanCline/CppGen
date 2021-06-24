#include "SerialFormat.h"

#include <unordered_map>
#include <string>
#include <memory>

namespace PROJECT_NAMESPACE
{
	using FormatLedger = std::unordered_map<std::string, std::unique_ptr<SerialFormat>>;

	auto& serial_format_ledger()
	{
		static FormatLedger ledger{};
		return ledger;
	};

	SerialFormat* get_serial_format(const std::string& _name)
	{
		const auto& _ledger = serial_format_ledger();
		const auto _it = _ledger.find(_name);
		return (_it != _ledger.end()) ? _it->second.get() : nullptr;
	};

	bool load_serial_format(std::unique_ptr<SerialFormat> _format)
	{
		if (!_format)
		{
			return false;
		};

		auto& _ledger = serial_format_ledger();
		const auto _name = _format->name();
		const auto [_iter, _good] = _ledger.insert({ _name, std::move(_format) });
		return _good;
	};
};
