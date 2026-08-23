music = {tracks = {}}

Size = {x = 140, y = 20}
startX = 20
gui = {
	{
		guiType = GuiType.TEXT,
		name = 'radius_',
		text = '',
		pos = {x = 0, y = 40, z = 0},
		scale = .5,
		font = 'batang.ttf',
		fontFirstChar = 0,
		fontLastChar = 256,
		color = {x = 1, y = 1, z = 1, w = 1}
	},
	{
		pos = {x = startX, y = 300, z = 0},
		size = {x = 80, y = 20},
		guiType = GuiType.BUTTON,
		buttonType = ButtonType.EXPORT,
		name = 'Export'
	},
	{
		pos = {x = startX + 110, y = 300, z = 0},
		size = {x = 170, y = 20},
		guiType = GuiType.BUTTON,
		buttonType = ButtonType.BACK,
		screen = 'mapEditorMenu.lua',
		name = 'Back to map editor'
	},
	{
		guiType = GuiType.TEXT,
		name = '',
		text = 'Vehicles',
		pos = {x = startX, y = 400, z = 0},
		scale = .25,
		font = 'batang.ttf',
		fontFirstChar = 0,
		fontLastChar = 256,
		color = {x = 1, y = 1, z = 1, w = 1}
	},
	{
		pos = {x = startX, y = 400, z = 0},
		size = Size,
		guiType = GuiType.LISTBOX,
		listboxType = ListboxType.VEHICLES,
		numMaxDisplay = 5
	},
	{
		guiType = GuiType.TEXT,
		name = '',
		text = 'Structures',
		pos = {x = startX + 150, y = 400, z = 0},
		scale = .25,
		font = 'batang.ttf',
		fontFirstChar = 0,
		fontLastChar = 256,
		color = {x = 1, y = 1, z = 1, w = 1}
	},
	{
		pos = {x = startX + 150, y = 400, z = 0},
		size = Size,
		guiType = GuiType.LISTBOX,
		listboxType = ListboxType.STRUCTURES,
		numMaxDisplay = 5
	},
	{
		guiType = GuiType.TEXT,
		name = '',
		text = 'Resources',
		pos = {x = startX + 2 * (Size.x + 10), y = 400, z = 0},
		scale = .25,
		font = 'batang.ttf',
		fontFirstChar = 0,
		fontLastChar = 256,
		color = {x = 1, y = 1, z = 1, w = 1}
	},
	{
		pos = {x = startX + 2 * (Size.x + 10), y = 400, z = 0},
		size = Size,
		guiType = GuiType.LISTBOX,
		listboxType = ListboxType.RESOURCE_DEPOSITS,
		numMaxDisplay = 5
	},
	{
		guiType = GuiType.TEXT,
		name = '',
		text = 'Skyboxes',
		pos = {x = startX + 3 * (Size.x + 10), y = 400, z = 0},
		scale = .25,
		font = 'batang.ttf',
		fontFirstChar = 0,
		fontLastChar = 256,
		color = {x = 1, y = 1, z = 1, w = 1}
	},
	{
		pos = {x = startX + 3 * (Size.x + 10), y = 400, z = 0},
		size = Size,
		guiType = GuiType.LISTBOX,
		listboxType = ListboxType.SKYBOX_TEXTURES,
		numMaxDisplay = 5
	},
	{
		guiType = GuiType.TEXT,
		name = '',
		text = 'Land textures',
		pos = {x = startX + 4 * (Size.x + 10), y = 400, z = 0},
		scale = .25,
		font = 'batang.ttf',
		fontFirstChar = 0,
		fontLastChar = 256,
		color = {x = 1, y = 1, z = 1, w = 1}
	},
	{
		pos = {x = startX + 4 * (Size.x + 10), y = 400, z = 0},
		size = {x = 170, y = 20},
		guiType = GuiType.LISTBOX,
		listboxType = ListboxType.LAND_TEXTURES,
		numMaxDisplay = 5
	}
}
