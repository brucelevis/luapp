#pragma once

#include "luapp/LuaAPI.hpp"
#include "luapp/Table.hpp"

namespace lua{


//------------------------------------------------------------------------------
inline void CheckVarFromLua(lua::NativeState hLua,lua::Bool *t,int i)
{
	*t=(lua::Bool)lua::CheckBoolean(hLua,i);
}
//------------------------------------------------------------------------------
inline void CheckVarFromLua(lua::NativeState hLua,lua::Int *t,int i)
{
	*t=(lua::Int)lua::CheckInteger(hLua,i);
}
//------------------------------------------------------------------------------
inline void CheckVarFromLua(lua::NativeState hLua,lua::Num *t,int i)
{
	*t=(lua::Num)lua::CheckNumber(hLua,i);
}
//------------------------------------------------------------------------------
inline void CheckVarFromLua(lua::NativeState hLua,lua::Str *t,int i)
{
	*t=lua::CheckString(hLua,i);
}
//------------------------------------------------------------------------------
inline void CheckVarFromLua(lua::NativeState hLua,lua::Ptr *t,int i)
{
	*t=lua::CheckPointer(hLua,i);
}
//------------------------------------------------------------------------------
inline void _VisitTable(lua::NativeState hLua,lua::Table *table);
template<typename T>
inline void _SaveTableValue(lua::NativeState hLua,lua::Table *table,T key)
{
	// ... [value]

	int   type = lua_type(hLua, -1);

	if ( type==LUA_TSTRING )
	{
		lua::Str   _value;
		CheckVarFromLua(hLua,&_value,-1);
		lua::Var   value(_value);
		(*table)[key] = value;
	}
	else if ( type==LUA_TTABLE )
	{
		lua::Table   subTable;
		_VisitTable(hLua,&subTable);

		lua::Var   value = subTable;
		(*table)[key] = value;
	}
	// Just in case.
	else if ( type==LUA_TNIL )
	{
		// This element is not exist.
	}
	else if ( type==LUA_TNONE )
	{
		lua::log::Cout<<"luapp:No one know what type is it"<<lua::log::End;
	}
	else if ( type==LUA_TLIGHTUSERDATA )
	{
		lua::Ptr   value;
		CheckVarFromLua(hLua,&value,-1);
		(*table)[key] = value;
	}
	else if ( type==LUA_TBOOLEAN )
	{
		lua::Bool   value;
		CheckVarFromLua(hLua,&value,-1);
		(*table)[key] = value;
	}
	else if ( type==LUA_TFUNCTION || type==LUA_TUSERDATA || type==LUA_TTHREAD )
	{
		lua::RestType   value;
		(*table)[key] = value;
	}
	else if ( lua_isinteger(hLua, -1) )
	{
		lua::Int   value;
		CheckVarFromLua(hLua,&value,-1);
		(*table)[key] = value;
	}
	else if ( lua_isnumber(hLua, -1) )
	{
		lua::Num   value;
		CheckVarFromLua(hLua,&value,-1);
		(*table)[key] = value;
	}
	else
	{
		// drop else value.
	}

	// ... [value]
}
//------------------------------------------------------------------------------
inline void _SwitchTableKey(lua::NativeState hLua,lua::Table *table)
{
	                                      // ... [T] [key] [value] [key]

	if ( lua_type(hLua, -1)==LUA_TSTRING )
	{
		lua::Str   key;
		lua::CheckVarFromLua(hLua,&key,-1);
		lua_pop(hLua, 1);                 // ... [T] [key] [value]
		_SaveTableValue(hLua,table,key);
	}
	/* Not implement yet. Maybe I will ignore it ever.
	else if ( lua_type(hLua, -1)==LUA_TBOOLEAN )
	{
		lua::Bool  key;
		lua::CheckVarFromLua(hLua,&key,-1);
		lua_pop(hLua, 1);                 // ... [T] [key] [value]
		_SaveTableValue(hLua,table,key);
	}
	*/
	else if ( lua_isinteger(hLua, -1) )
	{
		lua::Int   key;
		lua::CheckVarFromLua(hLua,&key,-1);
		lua_pop(hLua, 1);                 // ... [T] [key] [value]
		_SaveTableValue(hLua,table,key);
	}
	else if ( lua_isnumber(hLua, -1) )
	{
		lua::Num   key;
		lua::CheckVarFromLua(hLua,&key,-1);
		lua_pop(hLua, 1);                 // ... [T] [key] [value]
		_SaveTableValue(hLua,table,key);
	}
	else// Just in case.
	{
		lua_pop(hLua, 1);                 // ... [T] [key] [value]
	}

	lua_pop(hLua, 1);                     // ... [T] [key]
}
//------------------------------------------------------------------------------
inline void _VisitTable(lua::NativeState hLua,lua::Table *table)
{
	                                    // ... [T]
	lua_pushnil(hLua);                  // ... [T] [nil]

	while ( lua_next(hLua, -2) != 0 )
	{
		                                // ... [T] [key] [value]
		/*
		 * I have to copy the key,
		 * because lua_isinteger may be modify key data.
		 * The modified key data will make lua_next crash.
		 */
		lua_pushvalue(hLua,-2);         // ... [T] [key] [value] [key]
		_SwitchTableKey(hLua,table);    // ... [T] [key]
	}

	// ... [T]
}
//------------------------------------------------------------------------------
inline void CheckVarFromLua(lua::NativeState hLua,lua::Table *table,int i)
{
	                                 // ...
	lua_pushvalue(hLua,i);           // ... [T]
	::lua::_VisitTable(hLua,table);  // ... [T]
	lua_pop(hLua, 1);                // ...
}
//------------------------------------------------------------------------------
inline void CheckVarFromLua(lua::NativeState hLua,lua::Var *t,int i)
{
	int   type = lua_type(hLua, i);

	if ( type==LUA_TSTRING )
	{
		lua::Str   var;
		CheckVarFromLua(hLua,&var,i);
		*t = var;
	}
	else if ( type==LUA_TBOOLEAN )
	{
		lua::Bool   var;
		CheckVarFromLua(hLua,&var,i);
		*t = var;
	}
	else if ( type==LUA_TNIL )
	{
		*t = lua::Var();
	}
	else if ( type==LUA_TNONE )
	{
		lua::log::Cout<<"luapp:No one know what type is it. That's new"<<lua::log::End;
	}
	else if ( type==LUA_TLIGHTUSERDATA )
	{
		lua::Ptr   var;
		CheckVarFromLua(hLua,&var,i);
		*t = var;
	}
	else if ( type==LUA_TTABLE )
	{
		lua::Table   table;
		CheckVarFromLua(hLua,&table,i);
		*t = table;
	}
	else if ( type==LUA_TFUNCTION || type==LUA_TUSERDATA || type==LUA_TTHREAD )
	{
		lua::RestType   var;
		// Just for detect type.
		*t = var;
	}
	else if ( lua_isinteger(hLua, i) )
	{
		lua::Int   var;
		CheckVarFromLua(hLua,&var,i);
		*t = var;
	}
	else if ( lua_isnumber(hLua, i) )
	{
		lua::Num   var;
		CheckVarFromLua(hLua,&var,i);
		*t = var;
	}
	else
	{
		lua::log::Cout<<"luapp:you get something luapp can't handle with"<<lua::log::End;
		*t = lua::Var();
	}
}
//------------------------------------------------------------------------------

}
