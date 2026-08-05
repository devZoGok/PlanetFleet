res = graphics.resolution
HubMargin.top = 100

resTrayPos = {x = HubPos.x + HubMargin.left, y = HubPos.y + HubMargin.top}
iconSize = {x = 25, y = 25}
textboxSize = {x = 150, y = 25}

buttonSize = {x = 100, y = 20}

ResourceTrayGuiId = {ICON = 0, TEXTBOX = 1, INCREMENT = 2, DECREMENT = 3}
IconPath = {WEALTH = 'Icons/Resources/wealth.png', RESEARCH = 'Icons/Resources/research.png', REFINEDS = 'Icons/Resources/refineds.png', TIME = 'Icons/Resources/time.png'}

function createResourceTrayGui(cpuPlayer, lineId, guiId, imgPath)
	height = HubMargin.top + lineId * iconSize.y

	if guiId == ResourceTrayGuiId.ICON then
		guiElPos = {x = HubPos.x + (cpuPlayer and HubMargin.left or HubSize.x -(HubMargin.right + 3 * iconSize.x + textboxSize.x)), y = height, z = .2}

		return {
			guiType = GuiType.GUI_RECTANGLE,
			pos = guiElPos,
			size = iconSize,
			imagePath = imgPath
		}
	elseif guiId == ResourceTrayGuiId.TEXTBOX then
		guiElPos = {x = HubPos.x + (cpuPlayer and HubMargin.left + iconSize.x or HubSize.x -(HubMargin.right + 2 * iconSize.x + textboxSize.x)), y = height, z = .2}

		return {
			pos = guiElPos,
			size = textboxSize,
			guiType = GuiType.TEXTBOX,
		}
	elseif guiId == ResourceTrayGuiId.INCREMENT then
		guiElPos = {x = HubPos.x + (cpuPlayer and HubMargin.left + iconSize.x + textboxSize.x or HubSize.x -(HubMargin.right + 2 * iconSize.x)), y = height, z = .2}

		return {
			guiType = GuiType.BUTTON,
			buttonType = ButtonType.RESOURCE_AMMOUNT,
			ammount = 10,
			name = '+',
			pos = guiElPos,
			size = iconSize,
			trigger = 10
		}
	elseif guiId == ResourceTrayGuiId.DECREMENT then
		guiElPos = {x = HubPos.x + (cpuPlayer and HubMargin.left + 2 * iconSize.x + textboxSize.x or HubSize.x -(HubMargin.right + iconSize.x)), y = height, z = .2}

		return {
			guiType = GuiType.BUTTON,
			buttonType = ButtonType.RESOURCE_AMMOUNT,
			ammount = -10,
			name = '-',
			pos = guiElPos,
			size = iconSize,
			trigger = 10
		}
	end
end

gui = {
	{
		guiType = GuiType.GUI_RECTANGLE,
		pos = HubPos,
		size = HubSize,
		color = {x = 0, y = 0, z = 0, w = .6}
	},
	{
		guiType = GuiType.TEXT,
		name = 'P0',
		text = 'P0',
		pos = {x = HubPos.x + HubMargin.left, y = HubPos.y + HubMargin.top - 20, z = .2},
		scale = {x = .5, y = .5},
		font = 'batang.ttf',
		fontFirstChar = 0,
		fontLastChar = 256,
		color = {x = 1, y = 1, z = 1, w = 1}
	},
	{
		guiType = GuiType.TEXT,
		name = 'You',
		text = 'You',
		pos = {x = HubPos.x + HubSize.x -(HubMargin.right + textboxSize.x + iconSize.x + 50), y = HubPos.y + HubMargin.top - 20, z = .2},
		scale = {x = .5, y = .5},
		font = 'batang.ttf',
		fontFirstChar = 0,
		fontLastChar = 256,
		color = {x = 1, y = 1, z = 1, w = 1}
	},

	createResourceTrayGui(true, 0, ResourceTrayGuiId.ICON, IconPath.REFINEDS),
	createResourceTrayGui(true, 0, ResourceTrayGuiId.TEXTBOX),
	createResourceTrayGui(true, 0, ResourceTrayGuiId.INCREMENT),
	createResourceTrayGui(true, 0, ResourceTrayGuiId.DECREMENT),
	createResourceTrayGui(false, 0, ResourceTrayGuiId.ICON, IconPath.REFINEDS),
	createResourceTrayGui(false, 0, ResourceTrayGuiId.TEXTBOX),
	createResourceTrayGui(false, 0, ResourceTrayGuiId.INCREMENT),
	createResourceTrayGui(false, 0, ResourceTrayGuiId.DECREMENT),
	createResourceTrayGui(true, 1, ResourceTrayGuiId.ICON, IconPath.WEALTH),
	createResourceTrayGui(true, 1, ResourceTrayGuiId.TEXTBOX),
	createResourceTrayGui(true, 1, ResourceTrayGuiId.INCREMENT),
	createResourceTrayGui(true, 1, ResourceTrayGuiId.DECREMENT),
	createResourceTrayGui(false, 1, ResourceTrayGuiId.ICON, IconPath.WEALTH),
	createResourceTrayGui(false, 1, ResourceTrayGuiId.TEXTBOX),
	createResourceTrayGui(false, 1, ResourceTrayGuiId.INCREMENT),
	createResourceTrayGui(false, 1, ResourceTrayGuiId.DECREMENT),
	createResourceTrayGui(true, 2, ResourceTrayGuiId.ICON, IconPath.RESEARCH),
	createResourceTrayGui(true, 2, ResourceTrayGuiId.TEXTBOX),
	createResourceTrayGui(true, 2, ResourceTrayGuiId.INCREMENT),
	createResourceTrayGui(true, 2, ResourceTrayGuiId.DECREMENT),
	createResourceTrayGui(false, 2, ResourceTrayGuiId.ICON, IconPath.RESEARCH),
	createResourceTrayGui(false, 2, ResourceTrayGuiId.TEXTBOX),
	createResourceTrayGui(false, 2, ResourceTrayGuiId.INCREMENT),
	createResourceTrayGui(false, 2, ResourceTrayGuiId.DECREMENT),

	{
		guiType = GuiType.BUTTON,
		buttonType = ButtonType.PLAYER_TRADE,
		name = 'Back',
		guiScreen = 'tradingHub.lua',
		pos = {x = HubPos.x + HubSize.x - 2 * buttonSize.x - 2 * HubMargin.right, y = HubPos.y + HubSize.y - buttonSize.y - HubMargin.bottom, z = .2},
		size = buttonSize,
		trigger = 10
	},
	{
		guiType = GuiType.BUTTON,
		buttonType = ButtonType.TRADE_OFFER,
		name = 'Offer',
		pos = {x = HubPos.x + HubSize.x - buttonSize.x - HubMargin.right, y = HubPos.y + HubSize.y - buttonSize.y - HubMargin.bottom, z = .2},
		size = buttonSize,
		trigger = 10
	},
	{
		pos = {x = HubPos.x + HubMargin.left, y = HubPos.y + HubSize.y - textboxSize.y - HubMargin.bottom, z = .2},
		size = textboxSize,
		guiType = GuiType.TEXTBOX,
	}
}
