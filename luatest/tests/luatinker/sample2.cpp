// sample2.cpp : C++ 와 Lua 상호간의 변수 접근을 알아본다.
//

extern "C" 
{
	#include "lua.h"
	#include "lualib.h"
	#include "lauxlib.h"
};

#include "luatinker.h"

static int cpp_int = 100;

//int main()
//{
//	// Lua 를 초기화 한다.
//	lua_State* L = luaL_newstate();
//
//	// Lua 기본 함수들을 로드한다.- print() 사용
//    luaL_openlibs(L);
//
//	// LuaTinker 를 이용해서 cpp_int 를 Lua에 전달
//    luatinker::set(L, "cpp_int", cpp_int);
//
//	// sample1.lua 파일을 로드/실행한다.
//    luatinker::dofile(L, "sample2.lua");
//
//	// sample1.lua 의 함수를 호출한다.
//	int lua_int = luatinker::get<int>(L, "lua_int");
//
//	// lua_int 값 출력
//	printf("lua_int = %d\n", lua_int);
//
//	// 프로그램 종료
//	lua_close(L);
//
//	return 0;
//}
