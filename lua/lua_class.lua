Account={balance=0,
withdraw=function (self,v)
	self.balance =self.balance -v
end}
function Account:New(extension)
	local t=setmetatable(extension or {},self)
	self.__index=self
	return t
end

acc=Account:New({name="fucan"})
print(acc.balance)
