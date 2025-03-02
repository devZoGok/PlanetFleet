numGui = 3
gui = {
	{
		pos = {x = 110, y = 190, z = 0},
		size = {x = 360, y = 20},
		guiType = GuiType.LISTBOX,
		addPlayerGui = false,
		listboxType = ListboxType.MAPS,
		numMaxDisplay = 5
	},
	{
		pos = {x = 110, y = 250, z = 0},
		size = {x = 140, y = 40},
		guiType = GuiType.BUTTON,
		name = 'Ok',
		buttonType = ButtonType.LOAD_MAP_OK,
		numDependencies = 1,
		dependencies = {
			{id = 0}
		}
	},
	{
		pos = {x = 420, y = 400, z = 0},
		size = {x = 150, y = 50},
		guiType = GuiType.BUTTON,
		name = 'Back',
		buttonType = ButtonType.BACK,
		screen = 'mapEditorMenu.lua'
	}
}
