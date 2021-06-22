#pragma once

#include "GenSpec.h"

#include <pugixml.hpp>

#include <unordered_map>
#include <format>


namespace PROJECT_NAMESPACE
{
	using SerialGenFunction = FunctionBody(*)(const TypeSpec&);

	struct SerialFormat
	{
	protected:
		VariableSpec context_param() const noexcept
		{
			return VariableSpec
			{
				.name = "_ctx",
				.type = this->serial_context_type
			};
		};

		virtual FunctionSpec_Declaration gen_serialize_decl(const TypeSpec& _type) const
		{
			FunctionSpec_Declaration _decl;
			_decl.is_static = true;
			_decl.name = "serialize";
			_decl.return_type = this->serial_type;
			_decl.parameters =
			{
				this->context_param(),
				VariableSpec
				{
					.name = "_val",
					.type = _type.type
				}
			};
			return _decl;
		};
		virtual FunctionSpec_Declaration gen_deserialize_decl(const TypeSpec& _type) const
		{
			FunctionSpec_Declaration _decl{};
			_decl.is_static = true;
			_decl.name = "deserialize";
			_decl.return_type = std::format("::std::optional<{}>", _type.type);
			_decl.parameters =
			{
				this->context_param()
			};
			return _decl;
		};
		virtual FunctionBody gen_serialize_body(const TypeSpec&) = 0;
		virtual FunctionBody gen_deserialize_body(const TypeSpec&) = 0;

	public:
		const std::string& name() const noexcept { return this->name_; };

		FunctionSpec gen_serialize(const TypeSpec& _type)
		{
			FunctionSpec _out{};
			_out.decl = this->gen_serialize_decl(_type);
			_out.body = this->gen_serialize_body(_type);
			return _out;
		};
		FunctionSpec gen_deserialize(const TypeSpec& _type)
		{
			FunctionSpec _out{};
			_out.decl = this->gen_deserialize_decl(_type);
			_out.body = this->gen_deserialize_body(_type);
			return _out;
		};

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





	struct SerialFormat_XML : public SerialFormat
	{
	protected:

		static std::string serialize_direct(const std::string_view& _node, const std::string_view& _var)
		{
			return std::format("{}.text() = {};", _node, _var);
		};
		static std::string serialize_std_string(const std::string_view& _node, const std::string_view& _var)
		{
			return serialize_direct(_node, std::format("{}.c_str()", _var));
		};

		using SerialGen = std::string(*)(const std::string_view&, const std::string_view&);
		static const inline std::unordered_map<std::string, SerialGen> serial_gen
		{
			{ "std::string", &serialize_std_string },
			{ "int", &serialize_direct },
			{ "unsigned", &serialize_direct },
			{ "bool", &serialize_direct },
		};

		static std::string deserialize_int(const std::string_view& _node, const std::string_view& _var)
		{
			return std::format("{} = {}.text().as_int();", _var, _node);
		};
		static std::string deserialize_std_string(const std::string_view& _node, const std::string_view& _var)
		{
			return std::format("{} = {}.text().as_string();", _var, _node);
		};

		using DeserialGen = std::string(*)(const std::string_view&, const std::string_view&);
		static const inline std::unordered_map<std::string, DeserialGen> deserial_gen
		{
			{ "std::string", &deserialize_std_string },
			{ "int", &deserialize_int }
		};


		FunctionBody gen_serialize_body(const TypeSpec& _type) final
		{
			const std::string_view _valueName = "_val";
			const std::string_view _nodeName = "_node";
			const std::string_view _aliasName = "v";
			const std::string_view _memberNodeName = "m";

			FunctionBody _lines
			{
				std::format("auto& {} = _ctx;", _nodeName),
				std::format("{}.set_name(\"{}\");", _nodeName, _type.type)
			};
			for (auto& m : _type.members)
			{
				auto it = serial_gen.find(m.type);
				if (it != serial_gen.end())
				{
					_lines.push_back("{");
					_lines.push_back(std::format("\tconst auto& {} = {}.{};", _aliasName, _valueName, m.name));
					_lines.push_back(std::format("\tauto {} = {}.append_child(\"{}\");", _memberNodeName, _nodeName, m.name));
					_lines.push_back(std::format("\t{}", (*it->second)(_memberNodeName, _aliasName)));
					_lines.push_back("};");
				};
			};

			_lines.push_back(std::format("return {};", _nodeName));
			return _lines;
		};

		FunctionBody gen_deserialize_body(const TypeSpec& _type) final
		{
			const std::string_view _nodeName = "_node";
			const std::string_view _aliasName = "v";
			const std::string_view _memberName = "m";
			const std::string_view _outputName = "_out";
			FunctionBody _lines
			{
				std::format("auto& {} = _ctx;", _nodeName),
				std::format("if (std::string{{{}.name()}} != \"{}\")", _nodeName, _type.type),
				std::format("{{"),
				std::format("\treturn std::nullopt;"),
				std::format("}};"),
				{},
				std::format("{} {}{{}};", _type.type, _outputName)
			};

			for (auto& m : _type.members)
			{
				auto it = this->deserial_gen.find(m.type);
				if (it != this->deserial_gen.end())
				{
					_lines.push_back("{");
					_lines.push_back(std::format("\tauto {} = {}.child(\"{}\");", _aliasName, _nodeName, m.name));
					_lines.push_back(std::format("\tauto& {} = {}.{};", _memberName, _outputName, m.name));
					_lines.push_back(std::format("\t{}", (*it->second)(_aliasName, _memberName)));
					_lines.push_back("};");
				};
			};
			_lines.push_back(std::format("return {};", _outputName));
			return _lines;
		};

	public:
		SerialFormat_XML() :
			SerialFormat{ "xml" }
		{
			auto& _fmt = *this;
			IncludeSpec _include{};
			_include.name = "pugixml.hpp";
			_include.type = _include.External;
			_fmt.include_source.push_back(_include);

			ForwardDeclSpec _fwd{};
			_fwd.type = "namespace pugi{ class xml_node; }";
			_fmt.forward_decl.push_back(_fwd);

			_fmt.serial_type = "::pugi::xml_node";
			_fmt.serial_context_type = _fmt.serial_type;
		};
	};
	struct SerialFormat_Lua : public SerialFormat
	{
	protected:

		static std::string push_int(const std::string_view& _lua, const std::string_view& _var)
		{
			return std::format("lua_pushinteger({}, {})", _lua, _var);
		};
		static std::string push_string(const std::string_view& _lua, const std::string_view& _var)
		{
			return std::format("lua_pushstring({}, {}.c_str())", _lua, _var);
		};

		using PushGen = std::string(*)(const std::string_view&, const std::string_view&);
		static const inline std::unordered_map<std::string, PushGen> push_gen
		{
			{ "int", &push_int },
			{ "std::string", &push_string },
		};

		static std::string pull_int(const std::string_view& _lua, const std::string_view& _var)
		{
			return std::format("{} = luaL_checkinteger({}, -1)", _var, _lua);
		};
		static std::string pull_std_string(const std::string_view& _lua, const std::string_view& _var)
		{
			return std::format("{} = luaL_checkstring({}, -1)", _var, _lua);
		};

		using PullGen = std::string(*)(const std::string_view&, const std::string_view&);
		static const inline std::unordered_map<std::string, PullGen> pull_gen
		{
			{ "int", &pull_int },
			{ "std::string", &pull_std_string }
		};


		FunctionSpec_Declaration gen_deserialize_decl(const TypeSpec& _type) const final
		{
			auto _decl = SerialFormat::gen_deserialize_decl(_type);
			_decl.parameters.push_back(VariableSpec
				{
					.name = "_idx",
					.type = "int"
				});
			return _decl;
		};

		FunctionBody gen_serialize_body(const TypeSpec& _type) final
		{
			constexpr std::string_view _luaName = "_lua";
			constexpr std::string_view _valueName = "_val";
			constexpr std::string_view _tableIndexName = "_idx";
			constexpr std::string_view _aliasName = "v";

			FunctionBody _lines
			{
				std::format("const auto& {} = _ctx;", _luaName),
				std::format("lua_newtable({});", _luaName),
				std::format("const auto {} = lua_gettop({});", _tableIndexName, _luaName),
				"",
				std::format("lua_pushstring({}, \"{}\");", _luaName, _type.type),
				std::format("lua_setfield({}, {}, \"name\");", _luaName, _tableIndexName),
				"",
				std::format("lua_newtable({});", _luaName),
			};

			for (auto& m : _type.members)
			{
				auto it = this->push_gen.find(m.type);
				if (it != this->push_gen.end())
				{
					_lines.push_back("{");
					_lines.push_back(std::format("\tconst auto& {} = {}.{};", _aliasName, _valueName, m.name));
					_lines.push_back(std::format("\t{};", this->push_gen.at(m.type)(_luaName, _aliasName)));
					_lines.push_back(std::format("\tlua_setfield({}, {} + 1, \"{}\");", _luaName, _tableIndexName, m.name));
					_lines.push_back("};");
				};
			};

			_lines.push_back("");
			_lines.push_back(std::format("lua_setfield({}, {}, \"fields\")", _luaName, _tableIndexName));
			_lines.push_back("return 1;");
			return _lines;
		};
		FunctionBody gen_deserialize_body(const TypeSpec& _type) final
		{
			FunctionBody _lines{};
			return _lines;
		};

	public:
		SerialFormat_Lua() :
			SerialFormat{ "lua" }
		{
			auto& _fmt = *this;
			IncludeSpec _include{};
			_include.name = "lua.hpp";
			_include.type = _include.External;
			_fmt.include_source.push_back(_include);

			ForwardDeclSpec _fwd{};
			_fwd.type = "struct lua_State";
			_fmt.forward_decl.push_back(_fwd);

			_fmt.serial_type = "int";
			_fmt.serial_context_type = "lua_State*";
		};
	};

	using serial_format_typelist = std::tuple
		<
		SerialFormat_XML,
		SerialFormat_Lua
		>;

	namespace impl
	{

		template <typename T, typename MapT>
		inline void insert_genpair(MapT& _map)
		{
			std::unique_ptr<T> _fmt{ new T{} };
			const auto _name = _fmt->name();
			_map.insert(std::pair{ _name, std::move(_fmt) });
		};
		template <typename MapT, typename... Ts>
		inline void insert_format_genpairs(MapT& _map, std::tuple<Ts...>* _tup)
		{
			(insert_genpair<Ts>(_map), ...);
		};
		inline auto make_format_table()
		{
			std::unordered_map<std::string, std::unique_ptr<SerialFormat>> _formats{};
			serial_format_typelist* _formatTypes = nullptr;
			insert_format_genpairs(_formats, _formatTypes);
			return _formats;
		};
	};
	inline SerialFormat* get_serial_format(const std::string& _name)
	{
		static auto formats = impl::make_format_table();
		const auto _it = formats.find(_name);
		return (_it != formats.end()) ? _it->second.get() : nullptr;
	};




};
