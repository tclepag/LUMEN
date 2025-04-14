#pragma once

#include "../framework.h"
#include <variant>

#include "lua_table.h"
#include "lua_function.h"

#ifdef LULUA_EXPORTS
#define LULUA_API __declspec(dllexport)
#else
#define LULUA_API __declspec(dllimport)
#endif

namespace lua {
	using Value = std::variant<
		std::nullptr_t, 
		bool, 
		lua_Integer, 
		lua_Number, 
		const char*,
		LuaFunction*,
		LuaTable*
	>;

	class LuaValue {
	public:
		LuaValue(Value val) : value(val) {}

		template <typename T>
		T get() const {
			return std::get<T>(value);
		}

		template <typename T>
		bool is() const {
			return std::holds_alternative<T>(value);
		}
	private:
		Value value;
	};
}