#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include "lua.h"
#include "lauxlib.h"
#include "lualib.h"

void error(lua_State *L,const char *fmt, ...) {
	va_list argp;
	va_start(argp,fmt);	//初始化
	vfprintf(stderr,fmt,argp);//输出到标准错误输出
	va_end(argp);	//关闭argp
	lua_close(L);
	exit(EXIT_FAILURE);
}

static void stackDump(lua_State *L) {
	int i;
	int top =lua_gettop(L);
	for (i=1;i<=top;i++) {	/*遍历所有层*/
		int t= lua_type(L,i);
		switch (t) {
			case LUA_TSTRING: {
				printf("'%s'",lua_tostring(L,i));
				break;
			}
			case LUA_TBOOLEAN: {
				printf(lua_toboolean(L,i) ? "true":"false");
				break;
			}
			case LUA_TNUMBER: {
				printf("%g",lua_tonumber(L,i));
				break;
			}
			default: {
				printf("'%s'",lua_typename(L,i));
				break;
			}
		}
		printf("	");
	}
	printf("\n");
}

int main(void) {
	char buff[256];
	int error;
	lua_State *L =luaL_newwtate();	/*打开Lua*/
	luaL_openlibs(L);				/*打开标准库*/
	
	while(fgets(buff,sizeof(buff),stdin) != NULL) {
		error = luaL_loadbuffer(L,buff,strlen(buff),"line") ||
				lua_pcalll(L,0,0,0);
		if (error) {
			fprintf(stderr,"%s",lua_tostring(L,-1));
			lua_pop(L,1);	/*从栈中弹出错误信息*/
		}
	}
	lua_close(L);
	return 0;
}