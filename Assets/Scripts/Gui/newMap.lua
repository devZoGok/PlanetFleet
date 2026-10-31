Size = {x = 200, y = 40}

gui = {
	{
		guiType = GuiType.TEXT,
		name = 't0',
		text = 'New map',
		pos = {x = 20, y = 50, z = 0},
		scale = .5,
		font = 'batang.ttf',
		fontFirstChar = 0,
		fontLastChar = 256,
		color = {x = 1, y = 1, z = 1, w = 1}
	},
	{
		guiType = GuiType.TEXT,
		name = 't1',
		text = 'Map name',
		pos = {x = 20, y = 150, z = 0},
		scale = .25,
		font = 'batang.ttf',
		fontFirstChar = 0,
		fontLastChar = 256,
		color = {x = 1, y = 1, z = 1, w = 1}
	},
	{
		pos = {x = 20, y = 150, z = 0},
		size = Size,
		guiType = GuiType.TEXTBOX
	},
	{
		guiType = GuiType.TEXT,
		name = 't2',
		text = 'Width',
		pos = {x = 20, y = 220, z = 0},
		scale = .25,
		font = 'batang.ttf',
		fontFirstChar = 0,
		fontLastChar = 256,
		color = {x = 1, y = 1, z = 1, w = 1}
	},
	{
		pos = {x = 20, y = 220, z = 0},
		size = Size,
		guiType = GuiType.TEXTBOX
	},
	{
		guiType = GuiType.TEXT,
		name = 't3',
		text = 'Height',
		pos = {x = 20, y = 290, z = 0},
		scale = .25,
		font = 'batang.ttf',
		fontFirstChar = 0,
		fontLastChar = 256,
		color = {x = 1, y = 1, z = 1, w = 1}
	},
	{
		pos = {x = 20, y = 290, z = 0},
		size = Size,
		guiType = GuiType.TEXTBOX
	},
	{
		pos = {x = 20, y = 400, z = 0},
		size = {x = 150, y = 20},
		guiType = GuiType.BUTTON,
		name = 'Ok',
		buttonType = ButtonType.NEW_MAP_OK,
		numDependencies = 3,
		dependencies = {{id = 2}, {id = 4}, {id = 6}}
	},
	{
		pos = {x = 190, y = 400, z = 0},
		size = {x = 150, y = 20},
		guiType = GuiType.BUTTON,
		name = 'Back',
		buttonType = ButtonType.BACK,
		screen = 'mapEditorMenu.lua'
	}
}
