function permgen(a,n)
	n = n or #a		--默认n为a的大小
	if n <= 1 then
		--printResult(a)
		coroutine.yield(a)	--
	else
		for i=1,n do
		--将第i个元素放到数组末尾
		a[n],a[i] = a[i],a[n]
		--生成其余元素的排列
		permgen(a,n-1)
		--恢复第i个元素
		a[n],a[i] = a[i],a[n]
		end
	end
end

function printResult(a)
	for i=1,#a do
		io.write(a[i]," ")
	end
	io.write("\n")
end

function permutations(a)
	--[[local co = coroutine.create(function()
		permgen(a)
	end
	)
	return function() --迭代器
		local code,res = coroutine.resume(co)
		return res
	end]]
	return coroutine.wrap(function() permgen(a) end)
end
for p in permutations({"a","b","c"}) do
	printResult(p)
end