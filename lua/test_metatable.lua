t={}
m={a="and",b='Li Lei',c="Han Meimei"}
setmetatable(t,{__index=m})	--��{__index=m}��Ϊ��t��Ԫ��

--��ٱ�t
for k,v in pairs(t) do
print(k,v)
end
print("-------------------")
print(t)
print("-------------------")
print(t.b,t.a,t.c)

--
function add(t1,t2)
	--'#'�����ȡ����
	assert(#t1 == #t2)
	local length= #t1
	for i=1,length do
		t1[i]=t1[i]+t2[i]
	end
	return t1
end

--setmetatable ���ر����õı�
t1={1,2,3}
t2={10,20,30}
t1=setmetatable(t1,{__add=add})
t2=setmetatable(t2,{__add=add})

t1=t1+t2
for i=1, #t1 do
	print(t1[i])
end

--��ٱ�t2
for k,v in pairs(t1) do
print(k,v)
end