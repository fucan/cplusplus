--Э���߳�
function instream()
	return coroutine.wrap(function()
			print("instream")
			while true do
				print("instream wait")
				local line=io.read("*l")
				if line then
					coroutine.yield(line)--���ص�����filter,�������ȴ�
					print("instream end")
				else
					break
				end
			end
		end
	)
end

function filter(ins)
	return coroutine.wrap(function()
		print("filter")
		while true do
			print("filter wait")
			local line=ins()--����instream�ķ���ֵ,����Э��resume
			if line then
				line="** " .. line .. " **"
				coroutine.yield(line)--���ص�����outstream,�Լ���ִͣ��
				print("filter end")
			else
				break
			end
		end
	end
	)
end

function outstream(ins)
	while true do
		local line =ins()--����filter�ķ���ֵ������Э��resume
		if line then
			print(line)
		else
			break
		end
	end
end
local ins=instream()
print(ins)
local fil=filter(ins)
print(fil)
outstream(fil)--����Э����

--����Э��
--ͨ��coroutine.wrap���Դ���һ��Э���̣߳��ú�������һ���������͵�
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