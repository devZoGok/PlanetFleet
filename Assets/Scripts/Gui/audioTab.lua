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
		text = 'Music',
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
		option = {path = 'audio.musicVolume', step = 1},
		numDependencies = 1,
		dependencies = {
			{id = 2}
		}
	},
	{
		guiType = GuiType.TEXT,
		name = '',
		text = 'Effects',
		pos = {x = 20, y = 170, z = 0},
		scale = .2,
		font = 'batang.ttf',
		fontFirstChar = 0,
		fontLastChar = 256,
		color = {x = 1, y = 1, z = 1, w = 1}
	},
	{
		pos = {x = 370, y = 160, z = 0},
		size = {x = 60, y = 30},
		guiType = GuiType.TEXTBOX,
	},
	{
		pos = {x = 120, y = 160, z = 0},
		size = {x = 200, y = 10},
		guiType = GuiType.SLIDER,
		minValue = 0,
		maxValue = 100,
		option = {path = 'audio.sfxVolume', step = 1},
		numDependencies = 1,
		dependencies = {
			{id = 5}
		}
	},
	{
		pos = {x = 20, y = 400, z = 0},
		size = {x = 150, y = 20},
		guiType = GuiType.BUTTON,
		name = 'Ok',
		buttonType = ButtonType.OK,
		dependencies = {
			{id = 3},
			{id = 6}
		}
	},
	{
		pos = {x = 180, y = 400, z = 0},
		size = {x = 150, y = 20},
		guiType = GuiType.BUTTON,
		name = 'Restore defaults',
		buttonType = ButtonType.DEFAULTS,
		dependencies = {
			{id = 3},
			{id = 6}
		}
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
