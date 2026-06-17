Size = {x = 100, y = 20}
music = {
	loop = true,
	shuffle = false,
	delay = 2000,
	tracks = {'intro.ogg'}
}

MAIN_PLAYER_ID = 0

gui = {
	{
		pos = {x = 110, y = 310, z = 0},
		size = Size,
		guiType = GuiType.LISTBOX,
		addPlayerGui = true,
		listboxType = ListboxType.MAPS,
		numMaxDisplay = 5
	},
	{
		pos = {x = 110, y = 460, z = 0},
		size = Size,
		guiType = GuiType.BUTTON,
		buttonType = ButtonType.PLAY,
		name = 'Play',
		numDependencies = 4,
		dependencies = {{id = 0}}
	},
	{
		pos = {x = 110 + Size.x + 10, y = 460, z = 0},
		size = Size,
		guiType = GuiType.BUTTON,
		buttonType = ButtonType.BACK,
		name = 'Back',
		screen = 'mainMenu.lua'
	}
}
