gui = {
	{
		guiType = GuiType.TEXT,
		name = '',
		text = 'Audio',
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
		text = 'Volume',
		pos = {x = 20, y = 110, z = 0},
		scale = .2,
		font = 'batang.ttf',
		fontFirstChar = 0,
		fontLastChar = 256,
		color = {x = 1, y = 1, z = 1, w = 1}
	},
	{
		pos = {x = 370, y = 100, z = 0},
		size = {x = 60, y = 30},
		guiType = GuiType.TEXTBOX,
	},
	{
		pos = {x = 120, y = 100, z = 0},
		size = {x = 200, y = 10},
		guiType = GuiType.SLIDER,
		minValue = 0,
		maxValue = 100,
		numDependencies = 1,
		dependencies = {
			{id = 1}
		}
	},
	{
		pos = {x = 20, y = 400, z = 0},
		size = {x = 150, y = 20},
		guiType = GuiType.BUTTON,
		name = 'Ok',
		buttonType = ButtonType.OK
	},
	{
		pos = {x = 180, y = 400, z = 0},
		size = {x = 150, y = 20},
		guiType = GuiType.BUTTON,
		name = 'Restore defaults',
		buttonType = ButtonType.DEFAULTS
	},
	{
		pos = {x = 340, y = 400, z = 0},
		size = {x = 150, y = 20},
		guiType = GuiType.BUTTON,
		name = 'Back',
		screen = 'options.lua',
		buttonType = ButtonType.BACK
	}

}
