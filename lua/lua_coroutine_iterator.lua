function permgen(a,n)
	n = n or #a		--Ĭ��nΪa�Ĵ�С
	if n <= 1 then
		--printResult(a)
		coroutine.yield(a)	--
	else
		for i=1,n do
		--����i��Ԫ�طŵ�����ĩβ
		a[n],a[i] = a[i],a[n]
		--��������Ԫ�ص�����
		permgen(a,n-1)
		--�ָ���i��Ԫ��
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
	return function() --������
		local code,res = coroutine.resume(co)
		return res
	end]]
	return coroutine.wrap(function() permgen(a) end)
end
for p in permutations({"a","b","c"}) do
	printResult(p)
end