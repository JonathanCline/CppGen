#pragma once

#include <format>
#include <iosfwd>
#include <ranges>
#include <type_traits>
#include <string_view>

namespace PROJECT_NAMESPACE
{
	// Usage:
	// std::string _str = "foo";
	// const auto _cvec = range_cast<std::vector<char>>(_str);
	template <std::ranges::range OutT, std::ranges::range InT>
	requires (std::constructible_from<OutT, decltype(std::forward<InT>(std::declval<InT&&>()))> ||
		std::constructible_from<OutT, std::ranges::iterator_t<InT>, std::ranges::iterator_t<InT>>)
		constexpr inline OutT range_cast(InT&& _in) noexcept
	{
		if constexpr (std::constructible_from<OutT, decltype(std::forward<InT>(_in))>)
		{
			return OutT(std::forward<InT>(_in));
		}
		else
		{
			return OutT(std::ranges::begin(_in), std::ranges::end(_in));
		};
	};

	template <typename T, typename CharT>
	struct formatter;

	template <typename T>
	concept cx_parsing_formatter = requires(T a, std::format_parse_context _ctx)
	{
		a.parse(_ctx);
	};



	struct Tabber
	{
		friend inline std::ostream& operator<<(std::ostream& _ostr, const Tabber& _tab)
		{
			return _ostr << std::string_view{ _tab.buffer.data(), _tab.level };
		};

		using buffer_type = std::array<char, 32>;
		const buffer_type buffer = []()
		{
			buffer_type _buffer{};
			_buffer.fill('\t');
			return _buffer;
		}();
		size_t level = 0;
	};
	template <typename CharT>
	struct formatter<Tabber, CharT>
	{
		auto format(const Tabber& _tab)
		{
			const auto _tabs = std::string_view{ _tab.buffer.data(), _tab.level };
			return std::format("{}", _tabs);
		};
	};


};


namespace std
{
	template <typename T, typename CharT>
	requires requires
	{
		PROJECT_NAMESPACE::formatter<T, CharT>{};
	}
	struct formatter<T, CharT> : public formatter<std::string, CharT>
	{
		template <typename ContextT>
		auto format(const T& _value, ContextT& _ctx)
		{
			return std::formatter<std::string, CharT>::format(this->custom_.format(_value), _ctx);
		};
		PROJECT_NAMESPACE::formatter<T, CharT> custom_{};
	};
};

namespace PROJECT_NAMESPACE
{
	template <typename CharT>
	struct formatter<IncludeSpec, CharT>
	{
		auto format(const IncludeSpec& _spec)
		{
			constexpr std::array<std::string_view, 2> _formats
			{
				"#include <{}>\n",
				"#include \"{}\"\n"
			};
			return std::format(_formats.at((size_t)_spec.type), _spec.name);
		};
	};


	template <typename CharT>
	struct formatter<VariableSpec, CharT>
	{
		auto format(const VariableSpec& _spec)
		{
			const auto& [_name, _type, _value] = _spec;
			const std::string_view _fmt = [&_value]()
			{
				if (_value.empty())
				{
					// Omit initializer
					return "{} {}";
				}
				else
				{
					// Include initializer
					return "{} {}{{{}}}";
				};
			}();
			return std::format(_fmt, _type, _name, _value);
		};
	};




	template <typename CharT>
	struct formatter<FunctionSpec_Declaration, CharT>
	{
		auto format(const FunctionSpec_Declaration& _spec)
		{
			std::string_view _preSpecs = (_spec.is_static) ? "static " : "";

			std::string _paramStr{};
			for (auto& p : _spec.parameters)
			{
				_paramStr.append(std::format("{}, ", p));
			};
			if (!_paramStr.empty())
			{
				_paramStr.pop_back();
				_paramStr.pop_back();
			};

			return std::format("{}{} {}({})", _preSpecs, _spec.return_type, _spec.name, _paramStr);
		};
	};


	template <typename CharT>
	struct formatter<ForwardDeclSpec, CharT>
	{
		auto format(const ForwardDeclSpec& _spec)
		{
			return std::format("{}", _spec.type);
		};
	};





	constexpr auto wrap_format(std::string_view _fmt)
	{
		return [&_fmt](const auto& s) -> std::string
		{
			return std::format(_fmt, s);
		};
	};

	template <std::ranges::range T>
	constexpr inline auto format_fold(std::string_view _fmt, T&& _range)
	{
		namespace vw = std::views;
		auto _formatted = range_cast<std::vector<std::string>>(_range | vw::transform(wrap_format(_fmt)));
		return range_cast<std::string>(_formatted | vw::join);
	};

	// _class is optional
	inline auto format_function_definition(const FunctionSpec& _spec, Tabber& _tabber, const TypeSpec* _class)
	{
		auto& _decl = _spec.decl;
		std::string _fname{};
		if (_class)
		{
			_fname.append(_class->type + "::");
		};
		_fname.append(_decl.name);

		std::string _params = format_fold("{}, ", _decl.parameters);
		if (!_params.empty())
		{
			_params.pop_back();
			_params.pop_back();
		};
		auto _out = std::format("{}{} {}({})\n", _tabber, _decl.return_type, _fname, _params);
		_out.append(std::format("{}{{\n", _tabber));

		++_tabber.level;
		for (auto& line : _spec.body)
		{
			_out.append(std::format("{}{}\n", _tabber, line));
		};
		--_tabber.level;

		_out.append(std::format("{}}};\n", _tabber));
		return _out;
	};








	template <typename... Ts>
	inline std::ostream& print(std::ostream& _ostr, std::string_view _fmt, Ts&&... _vals)
	{
		return _ostr << std::format(_fmt, std::forward<Ts>(_vals)...);
	};
};
