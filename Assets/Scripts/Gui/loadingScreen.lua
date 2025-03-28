initPos = {x = 850, y = 100, z = 0}
Size = {x = 200, y = 20}

gui = {
	{
		guiType = GuiType.TEXT,
		text = 'Loading...',
		name = 'loading',
		pos = initPos,
		scale = {x = 0.5, y = 0.5},
		font = 'batang.ttf',
		fontFirstChar = 0,
		fontLastChar = 256,
		color = {x = 1, y = 1, z = 1, w = 1}
	},
	{
		guiType = GuiType.GUI_RECTANGLE,
		pos = {x = initPos.x, y = initPos.y + Size.y, z = initPos.z},
		size = Size,
		color = {x = 0, y = 0, z = 0, w = 1}
	},
	{
		guiType = GuiType.GUI_RECTANGLE,
		pos = {x = initPos.x + 1, y = initPos.y + Size.y + 1, z = initPos.z + .1},
		size = {x = 30, y = Size.y - 2},
		name = '_loadingBar',
		color = {x = 0, y = 0, z = 1, w = 1}
	},
}
