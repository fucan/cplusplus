a={}
b={x=1,['hello,']="world!"}
a.astring ="ni,hao!"
a[100]=100
a["a table"]=b

function foo()
end
function bar()
end
a[foo]=bar
--�ֱ���ٱ�a��b
for k,v in next,a do
print(k,"=>",v)
end
print("-------------------")
for k,v in pairs(b) do
print(k,"=>",v)
end