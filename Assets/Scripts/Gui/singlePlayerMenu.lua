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
		guiType = GuiType.TEXT,
		name = '',
		text = 'Singleplayer',
		pos = {x = 20, y = 50, z = 0},
		scale = .5,
		font = 'batang.ttf',
		fontFirstChar = 0,
		fontLastChar = 256,
		color = {x = 1, y = 1, z = 1, w = 1}
	},
	{
		pos = {x = 20, y = 310, z = 0},
		size = Size,
		guiType = GuiType.LISTBOX,
		addPlayerGui = true,
		listboxType = ListboxType.MAPS,
		numMaxDisplay = 5
	},
	{
		pos = {x = 20, y = 460, z = 0},
		size = Size,
		guiType = GuiType.BUTTON,
		buttonType = ButtonType.PLAY,
		name = 'Play',
		numDependencies = 4,
		dependencies = {{id = 1}}
	},
	{
		pos = {x = 20 + Size.x + 10, y = 460, z = 0},
		size = Size,
		guiType = GuiType.BUTTON,
		buttonType = ButtonType.BACK,
		name = 'Back',
		screen = 'mainMenu.lua'
	}
}
