--函数环境
function foo()
	print(g or "No g defined!")
end

foo()

setfenv(foo,{g=100,print=print})	--设置函数foo的环境为表{g=100,...}
foo()
print(g or "No g defined!")

--函数环境就是函数在执行时所见的全局变量的集合，以一个表来承载。

--函数环境的作用很多，利用它可以实现函数执行的“安全沙箱”；另外Lua
--的包的实现也依赖它。