music = {
	loop = true,
	shuffle = false,
	delay = 2000,
	tracks = {'victory.ogg'}
}

res = graphics.resolution
overlaySize = {x = 600, y = 450}
overlayPos = {x = .5 * (res.x - overlaySize.x), y = 200, z = 0}
buttonSize = {x = 150, y = 40}
buttonInitPos = {x = .5 * (res.x - buttonSize.x), y = 350, z = .1}

gui = {
	{
		guiType = GuiType.TEXT,
		text = 'VICTORY!!!',
		name = 'V',
		pos = {x = overlayPos.x + 20, y = overlayPos.y + 75, z = .1},
		scale = 1,
		font = 'batang.ttf',
		fontFirstChar = 0,
		fontLastChar = 256,
		color = {x = 1, y = 1, z = 1, w = 1}
	},
	{
		guiType = GuiType.GUI_RECTANGLE,
		pos = overlayPos,
		size = overlaySize,
		color = {x = 0, y = 0, z = 0, w = 0.1}
	},
	{
		pos = buttonInitPos,
		size = buttonSize,
		guiType = GuiType.BUTTON,
		name = 'Resume',
		buttonType = ButtonType.RESUME
	},
	{
		pos = {x = buttonInitPos.x, y = buttonInitPos.y + buttonSize.y + 10, z = buttonInitPos.z},
		size = buttonSize,
		guiType = GuiType.BUTTON,
		name = 'Leave',
		buttonType = ButtonType.STATISTICS,
		trigger = -1
	}
}
