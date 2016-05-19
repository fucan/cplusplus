function createCountdownTimer(second)
	local ms=second*1000
	
	local function countDown()
		ms=ms-1
		return ms
	end
	
	return countDown
end

timer1 = createCountdownTimer(1)
for i=1,3 do
	print(timer1())
end

print("----------")
timer2 = createCountdownTimer(1)
for i=1,3 do
	print(timer2())
end