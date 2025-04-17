local library = loadstring(game:HttpGet("https://raw.githubusercontent.com/Patskorn/GUI/main/Copy-SynapOver.lua"))()

local GUI = library:new("RaDka.CC","[ RightControl ]")
local Tab1 = GUI:Tap("ESP")
local Tab2 = GUI:Tap("COMBAT")
local Tab3 = GUI:Tap("VISUAL")

local ESP = {
    Enabled = false,
    ShowName = true,
    ShowBox = true,
    ShowLine = true,
    ShowTool = true,
    ShowHeadChams = false,
    TeamCheck = false,
    RainbowESP = false,
    MaxDistance = 10000,
    TeamColor = Color3.fromRGB(0, 255, 0),
    EnemyColor = Color3.fromRGB(255, 0, 0),
    ChamsTransparency = 0.5
}

local Players = game:GetService("Players")
local RunService = game:GetService("RunService")
local LocalPlayer = Players.LocalPlayer
local Camera = workspace.CurrentCamera

-- Функция для создания радужного цвета
local function getRainbowColor()
    local time = tick() % 5 / 5
    return Color3.fromHSV(time, 1, 1)
end

Tab1:Toggle("Enable ESP", false, function(value)
    ESP.Enabled = value
end)

Tab1:Toggle("Show Names", true, function(value)
    ESP.ShowName = value
end)

Tab1:Toggle("Show Box", true, function(value)
    ESP.ShowBox = value
end)

Tab1:Toggle("Show Tool", true, function(value)
    ESP.ShowTool = value
end)

Tab1:Toggle("Show Lines", true, function(value)
    ESP.ShowLine = value
end)

Tab1:Toggle("Show Head Chams", false, function(value)
    ESP.ShowHeadChams = value
    for _, player in pairs(Players:GetPlayers()) do
        if player ~= LocalPlayer and player.Character then
            ToggleChams(player.Character, value)
        end
    end
end)

Tab1:Toggle("Rainbow ESP", false, function(value)
    ESP.RainbowESP = value
end)

Tab1:Toggle("Team Colors", false, function(value)
    ESP.TeamCheck = value
end)

Tab1:Line()

Tab1:Slider("ESP Distance", 100, 10000, 2000, function(value)
    ESP.MaxDistance = value
end)

local function GetPlayerColor(player)
    if ESP.RainbowESP then
        return getRainbowColor()
    elseif ESP.TeamCheck then
        return player.Team == LocalPlayer.Team and ESP.TeamColor or ESP.EnemyColor
    end
    return ESP.EnemyColor
end

local function ToggleChams(character, enabled)
    if character then
        local head = character:FindFirstChild("Head")
        if head then
            if enabled and ESP.ShowHeadChams then
                if not head:FindFirstChild("HeadHighlight") then
                    local highlight = Instance.new("Highlight")
                    highlight.Name = "HeadHighlight"
                    highlight.FillColor = ESP.EnemyColor
                    highlight.OutlineColor = Color3.new(0, 0, 0)
                    highlight.FillTransparency = ESP.ChamsTransparency
                    highlight.OutlineTransparency = 0
                    highlight.DepthMode = Enum.HighlightDepthMode.AlwaysOnTop
                    highlight.Parent = head
                end
            else
                local highlight = head:FindFirstChild("HeadHighlight")
                if highlight then
                    highlight:Destroy()
                end
            end
        end
    end
end

local function GetEquippedTool(player)
    if player.Character then
        local tool = player.Character:FindFirstChildOfClass("Tool")
        if tool then
            return tool.Name
        end
    end
    return "None"
end

local function CreateESP(player)
    local esp = {
        BoxTop = Drawing.new("Line"),
        BoxBottom = Drawing.new("Line"),
        BoxLeft = Drawing.new("Line"),
        BoxRight = Drawing.new("Line"),
        Name = Drawing.new("Text"),
        Tool = Drawing.new("Text"),
        Line = Drawing.new("Line")
    }
    
    for _, line in pairs({esp.BoxTop, esp.BoxBottom, esp.BoxLeft, esp.BoxRight}) do
        line.Thickness = 1
        line.Color = GetPlayerColor(player)
        line.Transparency = 1
        line.Visible = false
    end
    
    esp.Name.Size = 13
    esp.Name.Center = true
    esp.Name.Outline = true
    esp.Name.Color = GetPlayerColor(player)
    esp.Name.Visible = false
    
    esp.Tool.Size = 13
    esp.Tool.Center = true
    esp.Tool.Outline = true
    esp.Tool.Color = GetPlayerColor(player)
    esp.Tool.Visible = false
    
    esp.Line.Thickness = 1
    esp.Line.Transparency = 1
    esp.Line.Color = GetPlayerColor(player)
    esp.Line.Visible = false
    
    return esp
end

local ESPObjects = {}

local function UpdateESP()
    for _, player in pairs(Players:GetPlayers()) do
        if player ~= LocalPlayer then
            if not ESPObjects[player] then
                ESPObjects[player] = CreateESP(player)
            end
            
            local esp = ESPObjects[player]
            local playerColor = GetPlayerColor(player)
            
            -- Обновляем цвет для чамсов головы
            if player.Character then
                if ESP.ShowHeadChams then
                    local head = player.Character:FindFirstChild("Head")
                    if head then
                        local headHighlight = head:FindFirstChild("HeadHighlight")
                        if headHighlight then
                            headHighlight.FillColor = playerColor
                        end
                    end
                end
            end
            
            if ESP.Enabled and player.Character and 
               player.Character:FindFirstChild("HumanoidRootPart") and 
               player.Character:FindFirstChild("Humanoid") and 
               player.Character.Humanoid.Health > 0 then
                
                local character = player.Character
                local humanoidRootPart = character.HumanoidRootPart
                local head = character:FindFirstChild("Head")
                
                if head then
                    local pos = humanoidRootPart.Position
                    local distance = (pos - Camera.CFrame.Position).Magnitude
                    
                    if distance <= ESP.MaxDistance then
                        local screenPos, onScreen = Camera:WorldToViewportPoint(pos)
                        
                        if onScreen then
                            -- Box ESP
                            if ESP.ShowBox then
                                local topPos = head.Position + Vector3.new(0, 1, 0)
                                local bottomPos = humanoidRootPart.Position - Vector3.new(0, 3, 0)
                                
                                local topScreen = Camera:WorldToViewportPoint(topPos)
                                local bottomScreen = Camera:WorldToViewportPoint(bottomPos)
                                local boxHeight = math.abs(topScreen.Y - bottomScreen.Y)
                                local boxWidth = boxHeight / 2
                                
                                local topLeft = Vector2.new(screenPos.X - boxWidth/2, topScreen.Y)
                                local topRight = Vector2.new(screenPos.X + boxWidth/2, topScreen.Y)
                                local bottomLeft = Vector2.new(screenPos.X - boxWidth/2, bottomScreen.Y)
                                local bottomRight = Vector2.new(screenPos.X + boxWidth/2, bottomScreen.Y)
                                
                                esp.BoxTop.From = topLeft
                                esp.BoxTop.To = topRight
                                esp.BoxTop.Color = playerColor
                                esp.BoxTop.Visible = true
                                
                                esp.BoxBottom.From = bottomLeft
                                esp.BoxBottom.To = bottomRight
                                esp.BoxBottom.Color = playerColor
                                esp.BoxBottom.Visible = true
                                
                                esp.BoxLeft.From = topLeft
                                esp.BoxLeft.To = bottomLeft
                                esp.BoxLeft.Color = playerColor
                                esp.BoxLeft.Visible = true
                                
                                esp.BoxRight.From = topRight
                                esp.BoxRight.To = bottomRight
                                esp.BoxRight.Color = playerColor
                                esp.BoxRight.Visible = true
                            else
                                esp.BoxTop.Visible = false
                                esp.BoxBottom.Visible = false
                                esp.BoxLeft.Visible = false
                                esp.BoxRight.Visible = false
                            end
                            
                            -- Name ESP
                            if ESP.ShowName then
                                esp.Name.Text = string.format("%s\n[%d]", player.Name, math.floor(distance))
                                esp.Name.Position = Vector2.new(screenPos.X, screenPos.Y - 40)
                                esp.Name.Color = playerColor
                                esp.Name.Visible = true
                            else
                                esp.Name.Visible = false
                            end
                            
                            -- Tool ESP
                            if ESP.ShowTool then
                                esp.Tool.Text = GetEquippedTool(player)
                                esp.Tool.Position = Vector2.new(screenPos.X, screenPos.Y + 20)
                                esp.Tool.Color = playerColor
                                esp.Tool.Visible = true
                            else
                                esp.Tool.Visible = false
                            end
                            
                            -- Line ESP
                            if ESP.ShowLine then
                                esp.Line.From = Vector2.new(Camera.ViewportSize.X/2, Camera.ViewportSize.Y)
                                esp.Line.To = Vector2.new(screenPos.X, screenPos.Y)
                                esp.Line.Color = playerColor
                                esp.Line.Visible = true
                            else
                                esp.Line.Visible = false
                            end
                        else
                            esp.BoxTop.Visible = false
                            esp.BoxBottom.Visible = false
                            esp.BoxLeft.Visible = false
                            esp.BoxRight.Visible = false
                            esp.Name.Visible = false
                            esp.Tool.Visible = false
                            esp.Line.Visible = false
                        end
                    else
                        esp.BoxTop.Visible = false
                        esp.BoxBottom.Visible = false
                        esp.BoxLeft.Visible = false
                        esp.BoxRight.Visible = false
                        esp.Name.Visible = false
                        esp.Tool.Visible = false
                        esp.Line.Visible = false
                    end
                end
            else
                esp.BoxTop.Visible = false
                esp.BoxBottom.Visible = false
                esp.BoxLeft.Visible = false
                esp.BoxRight.Visible = false
                esp.Name.Visible = false
                esp.Tool.Visible = false
                esp.Line.Visible = false
            end
        end
    end
end

RunService.RenderStepped:Connect(UpdateESP)

Players.PlayerRemoving:Connect(function(player)
    if ESPObjects[player] then
        for _, obj in pairs(ESPObjects[player]) do
            obj:Remove()
        end
        ESPObjects[player] = nil
        
        if player.Character then
            local head = player.Character:FindFirstChild("Head")
            if head then
                local headHighlight = head:FindFirstChild("HeadHighlight")
                if headHighlight then
                    headHighlight:Destroy()
                end
            end
        end
    end
end)

Players.PlayerAdded:Connect(function(player)
    player.CharacterAdded:Connect(function(character)
        if ESP.ShowHeadChams then
            ToggleChams(character, true)
        end
    end)
end)

for _, player in pairs(Players:GetPlayers()) do
    if player ~= LocalPlayer then
        player.CharacterAdded:Connect(function(character)
            if ESP.ShowHeadChams then
                ToggleChams(character, true)
            end
        end)
    end
end

Tab2:Button("Hitbox",function()
    _G.HeadSize = 15
_G.Disabled = true

game:GetService('RunService').RenderStepped:connect(function()
    if _G.Disabled then
        for i,v in next, game:GetService('Players'):GetPlayers() do
            if v.Name ~= game:GetService('Players').LocalPlayer.Name then
                pcall(function()
                    v.Character.Head.Size = Vector3.new(_G.HeadSize,_G.HeadSize,_G.HeadSize)
                    v.Character.Head.Transparency = 0.4
                    v.Character.Head.BrickColor = BrickColor.new("Red")
                    v.Character.Head.Material = "Glass"
                    v.Character.Head.CanCollide = false
                    v.Character.Head.Massless = true
                end)
            end
        end
    end
end)
end)

Tab2:Button("Aimbot T",function()
    local UserInputService = game:GetService("UserInputService")
local Players = game:GetService("Players")
local LocalPlayer = Players.LocalPlayer
local Mouse = LocalPlayer:GetMouse()

local function AimAt(target)
    if target and target.Character then
        local head = target.Character:FindFirstChild("Head")
        if head then
            local camera = workspace.CurrentCamera
            camera.CFrame = CFrame.new(camera.CFrame.Position, head.Position)
        end
    end
end

local function GetClosestPlayer()
    local closestPlayer = nil
    local shortestDistance = 1000 -- Максимальная дистанция 1000 studs
    
    for _, player in pairs(Players:GetPlayers()) do
        if player ~= LocalPlayer and player.Character and player.Character:FindFirstChild("Head") then
            local distance = (LocalPlayer.Character.Head.Position - player.Character.Head.Position).magnitude
            if distance < shortestDistance then
                closestPlayer = player
                shortestDistance = distance
            end
        end
    end
    
    return closestPlayer
end

local aimLock = false

UserInputService.InputBegan:Connect(function(input)
    if input.KeyCode == Enum.KeyCode.T then
        aimLock = true
        while aimLock do
            local target = GetClosestPlayer()
            if target then
                AimAt(target)
            end
            wait()
        end
    end
end)

UserInputService.InputEnded:Connect(function(input)
    if input.KeyCode == Enum.KeyCode.T then
        aimLock = false
    end
end)
end)

Tab2:Button("ShowFov Y",function()
    -- Получаем необходимые сервисы
local Players = game:GetService("Players")
local RunService = game:GetService("RunService")
local UserInputService = game:GetService("UserInputService")

-- Создаем новый UI
local screenGui = Instance.new("ScreenGui")
screenGui.Name = "CrosshairGui"
screenGui.Parent = Players.LocalPlayer:WaitForChild("PlayerGui")
screenGui.ResetOnSpawn = false -- Чтобы не исчезал при смерти

-- Создаем круг используя Frame
local circle = Instance.new("Frame")
circle.Size = UDim2.new(0, 200, 0, 200)
circle.AnchorPoint = Vector2.new(0.5, 0.5)
circle.Position = UDim2.new(0.5, 0, 0.39, 0) -- Немного ниже
circle.BackgroundTransparency = 1
circle.Parent = screenGui
circle.Visible = false -- Изначально скрыт

-- Создаем UICorner для создания круглой формы
local uiCorner = Instance.new("UICorner")
uiCorner.CornerRadius = UDim.new(1, 0)
uiCorner.Parent = circle

-- Создаем UIStroke для обводки
local uiStroke = Instance.new("UIStroke")
uiStroke.Thickness = 2
uiStroke.Parent = circle

-- Создаем точку в центре
local centerDot = Instance.new("Frame")
centerDot.Size = UDim2.new(0, 4, 0, 4)
centerDot.AnchorPoint = Vector2.new(0.5, 0.5)
centerDot.Position = UDim2.new(0.5, 0, 0.39, 0) -- Немного ниже
centerDot.BackgroundColor3 = Color3.new(1, 1, 1)
centerDot.Parent = screenGui
centerDot.Visible = false -- Изначально скрыт

-- Создаем UICorner для точки
local dotCorner = Instance.new("UICorner")
dotCorner.CornerRadius = UDim.new(1, 0)
dotCorner.Parent = centerDot

-- Функция для обновления цвета
local function updateColor()
    local h = tick() % 1
    uiStroke.Color = Color3.fromHSV(h, 1, 1)
end

-- Переменная для отслеживания состояния прицела
local crosshairEnabled = false

-- Функция переключения видимости прицела
local function toggleCrosshair()
    crosshairEnabled = not crosshairEnabled
    circle.Visible = crosshairEnabled
    centerDot.Visible = crosshairEnabled
end

-- Обработка нажатия клавиши
UserInputService.InputBegan:Connect(function(input, gameProcessed)
    if not gameProcessed and input.KeyCode == Enum.KeyCode.Y then
        toggleCrosshair()
    end
end)

-- Обновляем цвет каждый кадр
RunService.RenderStepped:Connect(updateColor)
end)

Tab3:Button("float",function()
    local Players = game:GetService("Players")
local UserInputService = game:GetService("UserInputService")

local player = Players.LocalPlayer
local character = player.Character or player.CharacterAdded:Wait()
local humanoid = character:WaitForChild("Humanoid")
local rootPart = character:WaitForChild("HumanoidRootPart")

local isBoostJumping = false
local boostForce = 500
local canBoostJump = true

local function startBoostJump()
    if canBoostJump and not isBoostJumping then
        isBoostJumping = true
        humanoid:ChangeState(Enum.HumanoidStateType.Jumping)
        
        local lookVector = workspace.CurrentCamera.CFrame.LookVector
        local boostDirection = Vector3.new(lookVector.X, 0.5, lookVector.Z).Unit
        rootPart.Velocity = boostDirection * boostForce
        
        canBoostJump = false
        wait(0.3)
        canBoostJump = true
        isBoostJumping = false
    end
end

UserInputService.JumpRequest:Connect(startBoostJump)

player.CharacterAdded:Connect(function(newCharacter)
    character = newCharacter
    humanoid = character:WaitForChild("Humanoid")
    rootPart = character:WaitForChild("HumanoidRootPart")
    canBoostJump = true
end)
end)
