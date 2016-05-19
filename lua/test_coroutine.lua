--Э���߳�
function producer()
	return coroutine.create(
		function(salt)
			local t={1,2,3}
			for i=1,#t do
				salt=coroutine.yield(t[i]+salt)
			end
		end
	)
end

function consumer(prod)
	local salt=10
	while true do
		local running,product=coroutine.resume(prod,salt)
		--print(running,product)
		salt=salt*salt
		if running then
			print(product or "end!")
		else
			break
		end
	end
end

consumer(producer())

--����Э��
--ͨ��coroutine.create���Դ���һ��Э���̣߳��ú�������һ���������͵�
--������Ϊ�̵߳�ִ���壬����һ���̶߳���

--����Э��
--ͨ��coroutine.resume��������һ���̻߳��߼���һ��������̡߳��ú���
--����һ���̶߳����Լ�������Ҫ���ݸ����̵߳Ĳ������߳̿���ͨ���̺߳�
--���Ĳ�������coroutine.yield���õķ���ֵ����ȡ��Щ���������̳߳���
--ִ��ʱ��resume���ݵĲ���ͨ���̺߳����Ĳ������ݸ��̣߳��̴߳��̺߳�
--����ʼִ�У����߳��ɹ���תΪִ��ʱ��resume���ݵĲ�����yield���÷�
--��ֵ����ʽ���ݸ��̣߳��̴߳�yield���ú����ִ�С�
--��Э�̲����ڷ���false

--�̷߳�������
--�̵߳���coroutine.yield��ͣ�Լ���ִ�У�����ִ��Ȩ���ظ�����/������
--���̣߳��̻߳�������yield����һЩֵ�����ߣ���Щֵ��resume���õķ�
--��ֵ����ʽ���ء�

function testResume()
	c=coroutine.create(function()
	local i=0
		while 1 do 
			i=i+1
			if i==2 then
			coroutine.yield()
			end
		end
	end
	)
	print(coroutine.resume(c))	--�������õ�,resume ��yield
	print(coroutine.resume(c))
	print(coroutine.resume(c))
end

testResume()