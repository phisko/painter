local str = "Loading"

local count = math.floor(os.clock()) % 4
for i = 1, count do
    str = str .. "."
end

local text = self:getTextComponent2D()
text.text:assign(str)