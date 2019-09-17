if not __init_done__ then
	__init_done__ = true
	local behavior = self:attachBehaviorComponent()
	behavior.func = function()
		print("PUTE")
	end
end

local scripts = self:getLuaComponent().scripts
for i = 0, scripts:size() - 1 do
	local s = scripts:get(i)
	print(s)
end
