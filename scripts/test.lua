if not __init_done__ then
	__init_done__ = true
	local behavior = self:emplace_execute()
	behavior.func = function(delta_time)
		print("Test from Lua")
	end
end

local scripts = self:get_lua().scripts
for i = 0, scripts:size() - 1 do
	local s = scripts:get(i)
	print(s)
end
