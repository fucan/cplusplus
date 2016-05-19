--协作线程
function instream()
	return coroutine.wrap(function()
			print("instream")
			while true do
				print("instream wait")
				local line=io.read("*l")
				if line then
					coroutine.yield(line)--返回调用者filter,并继续等待
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
			local line=ins()--接收instream的返回值,启动协程resume
			if line then
				line="** " .. line .. " **"
				coroutine.yield(line)--返回调用者outstream,自己暂停执行
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
		local line =ins()--接收filter的返回值，启动协程resume
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
outstream(fil)--两个协程吗？

--创建协程
--通过coroutine.wrap可以创建一个协作线程，该函数接收一个函数类型的
--参数作为线程的执行体，返回一个线程对象。

--启动协程
--通过coroutine.resume可以启动一个线程或者继续一个挂起的线程。该函数
--接收一个线程对象以及其他需要传递给该线程的参数。线程可以通过线程函
--数的参数或者coroutine.yield调用的返回值来获取这些参数。当线程初次
--执行时，resume传递的参数通过线程函数的参数传递给线程，线程从线程函
--数开始执行；当线程由挂起转为执行时，resume传递的参数以yield调用返
--回值的形式传递给线程，线程从yield调用后继续执行。
--当协程不存在返回false

--线程放弃调度
--线程调用coroutine.yield暂停自己的执行，并把执行权返回给启动/继续它
--的线程；线程还可利用yield返回一些值给后者，这些值以resume调用的返
--回值的形式返回。