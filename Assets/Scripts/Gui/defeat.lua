music = {
	loop = true,
	shuffle = false,
	delay = 2000,
	tracks = {'defeat.ogg'}
}

res = graphics.resolution
overlaySize = {x = 600, y = 450}
overlayPos = {x = .5 * (res.x - overlaySize.x), y = 100, z = .2}
buttonSize = {x = 150, y = 40}
buttonInitPos = {x = .5 * (res.x - buttonSize.x), y = 250, z = .3}

gui = {
	{
		guiType = GuiType.TEXT,
		text = 'Defeat',
		name = 'D',
		pos = {x = overlayPos.x + 150, y = overlayPos.y + 75, z = .3},
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
		name = 'Leave',
		buttonType = ButtonType.STATISTICS,
		trigger = -1
	},
}
