Size = {x = 150, y = 40}

numGui = 4
music = {
	loop = true,
	shuffle = false,
	tracks = {'intro.ogg'}
}
gui = {
	{
		pos = {x = 110, y = 40, z = 0},
		size = Size,
		guiType = GuiType.BUTTON,
		name = 'Single player',
		buttonType = ButtonType.SINGLE_PLAYER
	},
	{
		pos = {x = 110, y = 90, z = 0},
		size = Size,
		guiType = GuiType.BUTTON,
		name = 'Map editor',
		buttonType = ButtonType.MAP_EDITOR
	},
	{
		pos = {x = 110, y = 140, z = 0},
		size = Size,
		guiType = GuiType.BUTTON,
		name = 'Options',
		buttonType = ButtonType.OPTIONS
	},
	{
		pos = {x = 110, y = 190, z = 0},
		size = Size,
		guiType = GuiType.BUTTON,
		name = 'Exit',
		buttonType = ButtonType.EXIT
	}
}
