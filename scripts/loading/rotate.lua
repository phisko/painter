if not self:has_on_click() then
    math.randomseed(os.time())
    local me = self
    self:emplace_on_click().func = function()
        local color = me:get_drawable().color
        color.r = math.random()
        color.g = math.random()
        color.b = math.random()
    end
end

local transform = self:get_transform()
transform.roll = transform.roll + delta_time
transform.roll = math.fmod(transform.roll, 2 * math.pi)

transform.yaw = transform.yaw + delta_time
transform.yaw = math.fmod(transform.yaw, 2 * math.pi)
