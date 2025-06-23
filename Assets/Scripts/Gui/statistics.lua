headerHeight = 150
statsInitHeight = 120
statsSpace = 100
fontName = 'batang.ttf'

gui = {
	{
		guiType = GuiType.TEXT,
		text = 'Statistics',
		name = 'stat',
		pos = {x = 850, y = 100, z = 0},
		zIndex = 0,
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
		pos = {x = 50, y = headerHeight, z = 0},
		zIndex = 0,
		scale = 0.3,
		font = fontName,
		fontFirstChar = 0,
		fontLastChar = 256,
		color = {x = 1, y = 1, z = 1, w = 1}
	},
	{
		guiType = GuiType.TEXT,
		text = 'Vehicles built',
		name = 'vb',
		pos = {x = 150, y = headerHeight, z = 0},
		zIndex = 0,
		scale = 0.3,
		font = fontName,
		fontFirstChar = 0,
		fontLastChar = 256,
		color = {x = 1, y = 1, z = 1, w = 1}
	},
	{
		guiType = GuiType.TEXT,
		text = 'Vehicles destroyed',
		name = 'vd',
		pos = {x = 450, y = headerHeight, z = 0},
		zIndex = 0,
		scale = 0.3,
		font = fontName,
		fontFirstChar = 0,
		fontLastChar = 256,
		color = {x = 1, y = 1, z = 1, w = 1}
	},
	{
		guiType = GuiType.TEXT,
		text = 'Vehicles lost',
		name = 'vl',
		pos = {x = 850, y = headerHeight, z = 0},
		zIndex = 0,
		scale = 0.3,
		font = fontName,
		fontFirstChar = 0,
		fontLastChar = 256,
		color = {x = 1, y = 1, z = 1, w = 1}
	},
	{
		guiType = GuiType.TEXT,
		text = 'Structures built',
		name = 'sb',
		pos = {x = 1050, y = headerHeight, z = 0},
		zIndex = 0,
		scale = 0.3,
		font = fontName,
		fontFirstChar = 0,
		fontLastChar = 256,
		color = {x = 1, y = 1, z = 1, w = 1}
	},
	{
		guiType = GuiType.TEXT,
		text = 'Structures destroyed',
		name = 'sd',
		pos = {x = 1350, y = headerHeight, z = 0},
		zIndex = 0,
		scale = 0.3,
		font = fontName,
		fontFirstChar = 0,
		fontLastChar = 256,
		color = {x = 1, y = 1, z = 1, w = 1}
	},
	{
		guiType = GuiType.TEXT,
		text = 'Structures lost',
		name = 'sl',
		pos = {x = 1720, y = headerHeight, z = 0},
		zIndex = 0,
		scale = 0.3,
		font = 'batang.ttf',
		fontFirstChar = 0,
		fontLastChar = 256,
		color = {x = 1, y = 1, z = 1, w = 1}
	},
	{
		pos = {x = 1700, y = 1000, z = 0},
		size = Size,
		guiType = GuiType.BUTTON,
		name = 'Back',
		screen = 'mainMenu.lua',
		buttonType = ButtonType.BACK
	}
}
