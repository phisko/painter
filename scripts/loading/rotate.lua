if not self:hasLuaTableComponent() then
    self:attachLuaTableComponent()

    math.randomseed(os.time())
    local id = self.id -- used in lambda
    self:attachOnClickComponent().onClick = function()
        local color = getEntity(id):getGraphicsComponent().color
        color.r = math.random()
        color.g = math.random()
        color.b = math.random()
    end
end

local transform = self:getTransformComponent()
transform.roll = transform.roll + deltaTime
transform.roll = math.fmod(transform.roll, 2 * math.pi)

transform.yaw = transform.yaw + deltaTime
transform.yaw = math.fmod(transform.yaw, 2 * math.pi)
