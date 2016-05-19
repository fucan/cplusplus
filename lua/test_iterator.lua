--±Õ°ü
function enum(array)
	local index=1
	return function()
		local ret=array[index]
		print("index:",index)
		index=index+1
		return ret
	end
end

function foreach(array,action)
	for element in enum(array) do
		action(element)
	end
end

foreach({1,2,3},print)
--ÀàËÆ´úÂë
f=enum({4,5,6})
e=f()
while e do
	print(e)
	e=f()
end
