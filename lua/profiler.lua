local Counters = {}
local Names = {}

local function hook()
	local f=debug.getinfo(2,"f").func
	if Counters[f] == nil then
		Counters[f]=1
		Names[f] = debug.getinfo(2,"Sn")
	else
		Counters[f]=Counters[f]+1
	end
end

local f = assert(loadfile(arg[1]))
debug.sethook(hook,"c")
f()
debug.sethook()

function getname(func)
	local n = Names[func]
	if n.what == "C" then
		return n.name
	end
	local lc = string.format("[%s]:%s",n.short_src,n.linedefined)
	if n.namewhat ~= "" then	--不等于
		return string.format("%s (%s)",lc,n.name)
	else
		return lc
	end
end

for func,count in pairs(Counters) do
	--io.write(getname(func)," Called ",count," Num\n") getname返回nil
	print(getname(func)," Called ",count," Num\n")
end