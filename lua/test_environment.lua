--��������
function foo()
	print(g or "No g defined!")
end

foo()

setfenv(foo,{g=100,print=print})	--���ú���foo�Ļ���Ϊ��{g=100,...}
foo()
print(g or "No g defined!")

--�����������Ǻ�����ִ��ʱ������ȫ�ֱ����ļ��ϣ���һ���������ء�

--�������������úܶ࣬����������ʵ�ֺ���ִ�еġ���ȫɳ�䡱������Lua
--�İ���ʵ��Ҳ��������