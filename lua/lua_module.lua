local modname = ...
local M={}
_G[modname] = M		--模块名
package.loaded[modname]=M
--local _G=_G
--setmetatable(M,{__index = _G})
--导入段：
--声明这个模块从外界所需的所有东西
local sqrt =math.sqrt
local io = io
setfenv(1,M)	--设置当前模块的环境，声明函数不再需要前缀了

--以上代码可以使用module函数代替
--module(...)
-- setmetatable(M,{__index=_G})
--else 
-- module(...,package.seeall)

M.i = {r=0,i=1}
function new (r,i)
	return {r=r,i=i}
end

function add(c1,c2)
	return new(c1.r+c2.r,c1.i+c2.i)
end

function sub(c1,c2)
	return new(c1.r-c2.r,c1.i-c2.i)
end

function mul(c1,c2)
	return new(c1.r*c2.r-c1.i*c2.i,c1.r*c2.i+c1.i*c2.r)
end

local function inv(c)
	local n = c.r^2+c.i^2
	return new(c.r/n,-c1.i/n)
end

function div (c1,c2)
	return mul(c1,inv(c2))
end

--return M
--如果一个模块无返回值的话，require就会返回package.loaded[modname]的当前值