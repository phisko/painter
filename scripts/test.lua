print("pute")
if not __init_done__ then
	__init_done__ = true
	local behavior = self:emplace_execute()
	behavior.func = function(delta_time)
		print("Test from Lua")
	end
end

local files = self:get_lua_scripts().files
for i = 0, files:size() - 1 do
	local s = files:get(i)
	print(s)
end
