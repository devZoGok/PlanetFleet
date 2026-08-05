gui = {
	{
		guiType = GuiType.TEXT,
		name = '',
		text = 'Video',
		pos = {x = 20, y = 50, z = 0},
		scale = .5,
		font = 'batang.ttf',
		fontFirstChar = 0,
		fontLastChar = 256,
		color = {x = 1, y = 1, z = 1, w = 1}
	},
	{
		guiType = GuiType.TEXT,
		name = '',
		text = 'Resolution',
		pos = {x = 20, y = 100, z = 0},
		scale = .2,
		font = 'batang.ttf',
		fontFirstChar = 0,
		fontLastChar = 256,
		color = {x = 1, y = 1, z = 1, w = 1}
	},
	{
		pos = {x = 20, y = 100, z = 0},
		size = {x = 120, y = 20},
		guiType = GuiType.LISTBOX,
		listboxType = ListboxType.RESOLUTION,
		numMaxDisplay = 5,
		numLines = 3,
		lines = {
			'640 x 480',
			'800 x 600',
			'1920 x 1080'
		}
	},
	{
		pos = {x = 20, y = 310, z = 0},
		size = {x = 150, y = 20},
		guiType = GuiType.BUTTON,
		name = 'Ok',
		buttonType = ButtonType.OK
	},
	{
		pos = {x = 180, y = 310, z = 0},
		size = {x = 150, y = 20},
		guiType = GuiType.BUTTON,
		name = 'Restore defaults',
		buttonType = ButtonType.DEFAULTS
	},
	{
		pos = {x = 340, y = 310, z = 0},
		size = {x = 150, y = 20},
		guiType = GuiType.BUTTON,
		name = 'Back',
		screen = 'options.lua',
		buttonType = ButtonType.BACK
	}
}
