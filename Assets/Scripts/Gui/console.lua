gui = {
	{
		guiType = GuiType.TEXT,
		text = 'Console',
		name = '',
		pos = {x = 20, y = 40, z = 0},
		scale = 0.5,
		font = 'batang.ttf',
		fontFirstChar = 0,
		fontLastChar = 256,
		color = {x = 1, y = 1, z = 1, w = 1}
	},
	{
		pos = {x = 20, y = 40, z = 0},
		size = {x = 600, y = 20},
		guiType = GuiType.LISTBOX,
		listboxType = ListboxType.CONSOLE,
		numMaxDisplay = 10,
		closable = false
	},
	{
		pos = {x = 20, y = 510, z = 0},
		size = {x = 550, y = 20},
		guiType = GuiType.TEXTBOX
	},
	{
		pos = {x = 590, y = 510, z = 0},
		size = {x = 30, y = 20},
		guiType = GuiType.BUTTON,
		buttonType = ButtonType.CONSOLE_COMMAND_OK,
		name = 'OK',
		numDependencies = 2,
		dependencies = {
			{id = 1},
			{id = 2}
		}
	},
	{
		pos = {x = 630, y = 510, z = 0},
		size = {x = 40, y = 20},
		guiType = GuiType.BUTTON,
		buttonType = ButtonType.BACK,
		screen = 'gamePaused.lua',
		name = 'Back'
	}
}
