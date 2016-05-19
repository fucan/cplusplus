a = 2 --在全局变量下赋值
local newgt={}
setmetatable(newgt,{__index=_G})
setfenv(1,newgt)
b =3	--在新环境下赋值
a=4
print(newgt.a)
print(newgt.b)
print(_G['b'])
print(_G['a'])