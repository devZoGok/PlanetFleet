gui = {
	{
		guiType = GuiType.TEXT,
		name = 't0',
		text = 'Load map',
		pos = {x = 20, y = 50, z = 0},
		scale = .5,
		font = 'batang.ttf',
		fontFirstChar = 0,
		fontLastChar = 256,
		color = {x = 1, y = 1, z = 1, w = 1}
	},
	{
		pos = {x = 20, y = 100, z = 0},
		size = {x = 360, y = 20},
		guiType = GuiType.LISTBOX,
		addPlayerGui = false,
		listboxType = ListboxType.MAPS,
		numMaxDisplay = 5
	},
	{
		pos = {x = 20, y = 210, z = 0},
		size = {x = 160, y = 40},
		guiType = GuiType.BUTTON,
		name = 'Ok',
		buttonType = ButtonType.LOAD_MAP_OK,
		numDependencies = 1,
		dependencies = {{id = 1}}
	},
	{
		pos = {x = 220, y = 210, z = 0},
		size = {x = 160, y = 40},
		guiType = GuiType.BUTTON,
		name = 'Back',
		buttonType = ButtonType.BACK,
		screen = 'mapEditorMenu.lua'
	}
}
