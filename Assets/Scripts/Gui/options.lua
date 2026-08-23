Size = {x = 120, y = 20}

gui = {
	{
		guiType = GuiType.TEXT,
		name = '',
		text = 'Options',
		pos = {x = 20, y = 50, z = 0},
		scale = .5,
		font = 'batang.ttf',
		fontFirstChar = 0,
		fontLastChar = 256,
		color = {x = 1, y = 1, z = 1, w = 1}
	},
	{
		pos = {x = 20, y = 100, z = 0},
		size = Size,
		guiType = GuiType.BUTTON,
		name = 'Controls',
		buttonType = ButtonType.CONTROLS_TAB
	},
	{
		pos = {x = 180, y = 100, z = 0},
		size = Size,
		guiType = GuiType.BUTTON,
		name = 'Mouse',
		buttonType = ButtonType.MOUSE_TAB
	},
	{
		pos = {x = 340, y = 100, z = 0},
		size = Size,
		guiType = GuiType.BUTTON,
		name = 'Video',
		buttonType = ButtonType.VIDEO_TAB
	},
	{
		pos = {x = 500, y = 100, z = 0},
		size = Size,
		guiType = GuiType.BUTTON,
		name = 'Audio',
		buttonType = ButtonType.AUDIO_TAB
	},
	{
		pos = {x = 20, y = 160, z = 0},
		size = Size,
		guiType = GuiType.BUTTON,
		name = 'Back',
		screen = 'mainMenu.lua',
		buttonType = ButtonType.BACK
	}
}
