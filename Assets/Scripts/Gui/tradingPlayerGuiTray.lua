res = graphics.resolution

buttonSize = {x = 100, y = 20}
textboxSize = {x = 200, y = 20}

height = HubPos.y + HubMargin.top + lineId * 30

gui = {
	{
		guiType = GuiType.TEXT,
		name = 'playerName',
		text = '',
		pos = {x = HubPos.x + HubMargin.left, y = height + 35, z = .11},
		scale = {x = .2, y = .2},
		font = 'batang.ttf',
		fontFirstChar = 0,
		fontLastChar = 256,
		color = {x = 1, y = 1, z = 1, w = 1}
	},
	{
		guiType = GuiType.GUI_RECTANGLE,
		pos = {x = HubPos.x + HubMargin.left + 100, y = height, z = .11},
		size = {x = 50, y = 50},
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
		pos = {x = HubPos.x + HubSize.x - buttonSize.x - HubMargin.right, y = height + 25, z = .11},
		size = buttonSize,
		trigger = 10,
		dependencies = {{id = 2}}
	}
}
