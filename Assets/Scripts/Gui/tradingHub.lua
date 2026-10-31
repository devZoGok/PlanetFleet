res = graphics.resolution

HubSize = {x = 500, y = res.y * .8}
HubPos = {x = .5 * (res.x - HubSize.x), y = 10, z = .1}
HubMargin = {top = 10, left = 10, right = 10, bottom = 10}

buttonSize = {x = 100, y = 20}
textboxSize = {x = 200, y = 25}

warIcon = 'Icons/PlayerStates/war.png'
allianceIcon = 'Icons/PlayerStates/alliance.png'

gui = {
	{
		guiType = GuiType.TEXT,
		text = 'Trading hub',
		name = '',
		pos = {x = HubPos.x + .25 * HubSize.x, y = HubPos.y + HubMargin.top + 25, z = .11},
		scale = 0.5,
		font = 'batang.ttf',
		fontFirstChar = 0,
		fontLastChar = 256,
		color = {x = 1, y = 1, z = 1, w = 1}
	},
	{
		guiType = GuiType.TEXT,
		text = 'Name',
		name = '',
		pos = {x = HubPos.x + HubMargin.left, y = HubPos.y + HubMargin.top + 60, z = .11},
		scale = 0.25,
		font = 'batang.ttf',
		fontFirstChar = 0,
		fontLastChar = 256,
		color = {x = 1, y = 1, z = 1, w = 1}
	},
	{
		guiType = GuiType.TEXT,
		text = 'Status',
		name = '',
		pos = {x = HubPos.x + HubMargin.left + 130, y = HubPos.y + HubMargin.top + 60, z = .11},
		scale = 0.25,
		font = 'batang.ttf',
		fontFirstChar = 0,
		fontLastChar = 256,
		color = {x = 1, y = 1, z = 1, w = 1}
	},
	{
		guiType = GuiType.TEXT,
		text = 'Trade offers',
		name = '',
		pos = {x = HubPos.x + HubMargin.right + 200, y = HubPos.y + HubMargin.top + 60, z = .11},
		scale = 0.25,
		font = 'batang.ttf',
		fontFirstChar = 0,
		fontLastChar = 256,
		color = {x = 1, y = 1, z = 1, w = 1}
	},
	{
		guiType = GuiType.GUI_RECTANGLE,
		pos = HubPos,
		size = HubSize,
		color = {x = 0, y = 0, z = 0, w = .6}
	},
	{
		guiType = GuiType.BUTTON,
		buttonType = ButtonType.ACTIVE_STATE_BACK,
		name = 'Back',
		pos = {x = HubPos.x + HubSize.x - buttonSize.x - HubMargin.right, y = HubPos.y + HubSize.y - buttonSize.y - HubMargin.bottom, z = .11},
		size = buttonSize,
		trigger = 10
	},
}
