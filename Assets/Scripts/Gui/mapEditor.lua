music = {tracks = {}}
numGui = 6

Size = {x = 140, y = 20}
startX = 100
gui = {
	{
		pos = {x = startX, y = 400, z = 0},
		size = Size,
		guiType = GuiType.BUTTON,
		buttonType = ButtonType.EXPORT,
		name = 'Export'
	},
	{
		pos = {x = startX + Size.x + 10, y = 400, z = 0},
		size = Size,
		guiType = GuiType.LISTBOX,
		listboxType = ListboxType.VEHICLES,
		numMaxDisplay = 5
	},
	{
		pos = {x = startX + 2 * (Size.x + 10), y = 400, z = 0},
		size = Size,
		guiType = GuiType.LISTBOX,
		listboxType = ListboxType.STRUCTURES,
		numMaxDisplay = 5
	},
	{
		pos = {x = startX + 3 * (Size.x + 10), y = 400, z = 0},
		size = Size,
		guiType = GuiType.LISTBOX,
		listboxType = ListboxType.RESOURCE_DEPOSITS,
		numMaxDisplay = 5
	},
	{
		pos = {x = startX + 4 * (Size.x + 10), y = 400, z = 0},
		size = Size,
		guiType = GuiType.LISTBOX,
		listboxType = ListboxType.SKYBOX_TEXTURES,
		numMaxDisplay = 5
	},
	{
		pos = {x = startX + 5 * (Size.x + 10), y = 400, z = 0},
		size = Size,
		guiType = GuiType.LISTBOX,
		listboxType = ListboxType.LAND_TEXTURES,
		numMaxDisplay = 5
	}
}
