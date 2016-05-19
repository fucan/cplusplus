require "socket"

host = "www.w3.org"
file = "/Help/Webmaster.html"

c = assert(socket.connect(host,80))

c:send("GET" .. file .. "HTTP/1.0\r\n\r\n")

while true do
	local s,status,partial = c:receive(2^10)
	io.write(s or partial)
	if status == "Closed" then break end
end

c:close()