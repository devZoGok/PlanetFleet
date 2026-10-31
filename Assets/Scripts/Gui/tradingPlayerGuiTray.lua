res = graphics.resolution

buttonSize = {x = 60, y = 20}
textboxSize = {x = 200, y = 20}

height = HubPos.y + HubMargin.top + 90 + lineId * 70

gui = {
	{
		guiType = GuiType.TEXT,
		name = 'playerName',
		text = '',
		pos = {x = HubPos.x + HubMargin.left, y = height + 40, z = .11},
		scale = .2,
		font = 'batang.ttf',
		fontFirstChar = 0,
		fontLastChar = 256,
		color = {x = 1, y = 1, z = 1, w = 1}
	},
	{
		guiType = GuiType.GUI_RECTANGLE,
		pos = {x = HubPos.x + HubMargin.left + 130, y = height, z = .11},
		size = {x = 40, y = 40},
		imagePath = relationsIcon
	},
	{
		pos = {x = HubPos.x + HubMargin.left + 200, y = height + 25, z = .11},
		size = textboxSize,
		guiType = GuiType.LISTBOX,
		listboxType = ListboxType.TRADE_OFFERS,
		numMaxDisplay = 3
	},
	{
		guiType = GuiType.BUTTON,
		buttonType = ButtonType.TRADING_SCREEN,
		name = 'Trade',
		guiScreen = 'tradingScreen.lua',
		pos = {x = HubPos.x + HubSize.x - HubMargin.right - buttonSize.x, y = height + 25, z = .11},
		size = buttonSize,
		trigger = 10,
		dependencies = {{id = 2}}
	}
}
