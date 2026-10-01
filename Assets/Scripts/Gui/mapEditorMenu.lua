Size = {x = 150, y = 40}

gui = {
	{
		guiType = GuiType.TEXT,
		name = '',
		text = 'Map editor',
		pos = {x = 20, y = 50, z = 0},
		scale = .5,
		font = 'batang.ttf',
		fontFirstChar = 0,
		fontLastChar = 256,
		color = {x = 1, y = 1, z = 1, w = 1}
	},
	{
		pos = {x = 20, y = 60, z = 0},
		size = Size,
		guiType = GuiType.BUTTON,
		name = 'New map',
		buttonType = ButtonType.NEW_MAP
	},
	{
		pos = {x = 20, y = 110, z = 0},
		size = Size,
		guiType = GuiType.BUTTON,
		name = 'Load map',
		buttonType = ButtonType.LOAD_MAP
	},
	{
		pos = {x = 20, y = 160, z = 0},
		size = Size,
		guiType = GuiType.BUTTON,
		name = 'Back',
		buttonType = ButtonType.BACK,
		screen = 'mainMenu.lua'
	}
}
