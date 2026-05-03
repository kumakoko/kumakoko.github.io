local print_coroutine_function = function(coroutine_obj)
	local t = {nil,nil,nil,nil}
	t[1] = "Current state of variable "
	t[2] = tostring(coroutine_obj)
	t[3] = " is "
	t[4] = coroutine.status(coroutine_obj)
	print(table.concat(t))
end

local coroutine_func = function()
	print("Long long ago,")
	coroutine.yield()
	print("there was an old man who was telling us a story.")
	coroutine.yield()
	print("What does long long ago mean?")
	coroutine.yield()
	print("What does it mean when we say Long long ago?")
	coroutine.yield()
end


--[[-- 打印出当前协程的状态，协程对象一旦被创建，状态即为挂起
print_coroutine_function(co)

-- 让协程从挂起到执行
coroutine.resume(co)
print_coroutine_function(co)
print_coroutine_function(co)--]]

function main()
	local co = coroutine.create(coroutine_func)
	print("Type of variable co is "..type(co))
	
	local answer = nil
	repeat
		print("Press 'y' to exit")
		io.flush()
		answer = io.read()
		coroutine.resume(co)
	until answer== "y"
end

main()