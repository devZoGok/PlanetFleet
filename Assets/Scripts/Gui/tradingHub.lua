res = graphics.resolution

HubSize = {x = res.x * .3, y = res.y * .8}
HubPos = {x = .5 * (res.x - HubSize.x), y = 10, z = .1}
HubMargin = {top = 10, left = 10, right = 10, bottom = 10}

buttonSize = {x = 100, y = 20}
textboxSize = {x = 200, y = 25}

gui = {
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
