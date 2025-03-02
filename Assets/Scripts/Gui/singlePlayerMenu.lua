Size = {x = 100, y = 20}
playerListboxPos = {x = 110, y = 10, z = 0}
factions = {"America inc.", "European Republic", "Asian Co-Prosperity Sphere"}

numGui = 6
gui = {
	{
		pos = playerListboxPos,
		size = Size,
		guiType = GuiType.LISTBOX,
		numMaxDisplay = 3,
		lines = factions,
		listboxType = ListboxType.FACTIONS
	},
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
		dependencies = {
			{id = 0},
			{id = 1},
			{id = 2},
			{id = 3}
		}
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
