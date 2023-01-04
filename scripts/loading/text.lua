local str = "Loading"

local count = math.floor(os.clock()) % 4
for i = 1, count do
    str = str .. "."
end

local text = self:get_text_2d()
text.value:assign(str)