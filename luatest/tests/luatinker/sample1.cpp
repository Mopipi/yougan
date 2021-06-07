// sample1.cpp : C++ 와 Lua 상호간의 함수 실행을 알아본다.
//

#include <iostream>

extern "C" 
{
	#include "lua.h"
	#include "lualib.h"
	#include "lauxlib.h"
};

#include "luatinker.h"

int cpp_func(int arg1, int arg2)
{
	return arg1 + arg2;
}

//int main()
//{
//	// Lua 를 초기화 한다.
//	lua_State* L = luaL_newstate();
//
//	// Lua 기본 함수들을 로드한다.- print() 사용
//    luaL_openlibs(L);
//
//	// LuaTinker 를 이용해서 함수를 등록한다.
//    luatinker::def(L, "cpp_func", cpp_func);
//
//	// sample1.lua 파일을 로드/실행한다.
//    luatinker::dofile(L, "sample1.lua");
//
//	// sample1.lua 의 함수를 호출한다.
//	int result = luatinker::call<int>(L, "lua_func", 3, 4);
//
//	// lua_func(3,4) 의 결과물 출력
//	printf("lua_func(3,4) = %d\n", result);
//
//	// 프로그램 종료
//	lua_close(L);
//
//	return 0;
//}
