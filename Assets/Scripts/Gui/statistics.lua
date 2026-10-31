headerHeight = 100
statsInitLeft = .5 * res.x - 400
statsInitHeight = 40
statsSpace = 20
fontName = 'batang.ttf'

gui = {
	{
		guiType = GuiType.TEXT,
		text = 'Statistics',
		name = 'stat',
		pos = {x = .4 * res.x, y = 50, z = 0},
		scale = 0.5,
		font = fontName,
		fontFirstChar = 0,
		fontLastChar = 256,
		color = {x = 1, y = 1, z = 1, w = 1}
	},
	{
		guiType = GuiType.TEXT,
		text = 'Player',
		name = 'pl',
		pos = {x = statsInitLeft + 10, y = headerHeight, z = 0},
		scale = 0.18,
		font = fontName,
		fontFirstChar = 0,
		fontLastChar = 256,
		color = {x = 1, y = 1, z = 1, w = 1}
	},
	{
		guiType = GuiType.TEXT,
		text = 'Vehicles built',
		name = 'vb',
		pos = {x = statsInitLeft + 110, y = headerHeight, z = 0},
		scale = 0.18,
		font = fontName,
		fontFirstChar = 0,
		fontLastChar = 256,
		color = {x = 1, y = 1, z = 1, w = 1}
	},
	{
		guiType = GuiType.TEXT,
		text = 'Vehicles destroyed',
		name = 'vd',
		pos = {x = statsInitLeft + 220, y = headerHeight, z = 0},
		scale = 0.18,
		font = fontName,
		fontFirstChar = 0,
		fontLastChar = 256,
		color = {x = 1, y = 1, z = 1, w = 1}
	},
	{
		guiType = GuiType.TEXT,
		text = 'Vehicles lost',
		name = 'vl',
		pos = {x = statsInitLeft + 370, y = headerHeight, z = 0},
		scale = 0.18,
		font = fontName,
		fontFirstChar = 0,
		fontLastChar = 256,
		color = {x = 1, y = 1, z = 1, w = 1}
	},
	{
		guiType = GuiType.TEXT,
		text = 'Structs built',
		name = 'sb',
		pos = {x = statsInitLeft + 480, y = headerHeight, z = 0},
		scale = 0.18,
		font = fontName,
		fontFirstChar = 0,
		fontLastChar = 256,
		color = {x = 1, y = 1, z = 1, w = 1}
	},
	{
		guiType = GuiType.TEXT,
		text = 'Structs destroyed',
		name = 'sd',
		pos = {x = statsInitLeft + 580, y = headerHeight, z = 0},
		scale = 0.18,
		font = fontName,
		fontFirstChar = 0,
		fontLastChar = 256,
		color = {x = 1, y = 1, z = 1, w = 1}
	},
	{
		guiType = GuiType.TEXT,
		text = 'Structs lost',
		name = 'sl',
		pos = {x = statsInitLeft + 720, y = headerHeight, z = 0},
		scale = 0.18,
		font = 'batang.ttf',
		fontFirstChar = 0,
		fontLastChar = 256,
		color = {x = 1, y = 1, z = 1, w = 1}
	},
	{
		pos = {x = res.x - Size.x - 10, y = res.y - Size.y - 10, z = 0},
		size = Size,
		guiType = GuiType.BUTTON,
		name = 'Back',
		screen = 'mainMenu.lua',
		buttonType = ButtonType.BACK
	}
}
