resolutions = {{x = 640, y = 480}, {x = 800, y = 600}, {x = 1920, y = 1080}}

-- a resolution set by hand in options.lua stays selectable
if getOptionIndex('graphics.resolution', resolutions) == -1 then
	table.insert(resolutions, getOption('graphics.resolution'))
end

resolutionLines = {}

for i = 1, #resolutions do
	table.insert(resolutionLines, resolutions[i].x .. ' x ' .. resolutions[i].y)
end

gui = {
	{
		guiType = GuiType.TEXT,
		name = '',
		text = 'Video',
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
		text = 'Resolution',
		pos = {x = 20, y = 100, z = 0},
		scale = .2,
		font = 'batang.ttf',
		fontFirstChar = 0,
		fontLastChar = 256,
		color = {x = 1, y = 1, z = 1, w = 1}
	},
	{
		guiType = GuiType.TEXT,
		name = '',
		text = 'Fullscreen',
		pos = {x = 170, y = 110, z = 0},
		scale = .2,
		font = 'batang.ttf',
		fontFirstChar = 0,
		fontLastChar = 256,
		color = {x = 1, y = 1, z = 1, w = 1}
	},
	{
		pos = {x = 20, y = 100, z = 0},
		size = {x = 120, y = 20},
		guiType = GuiType.LISTBOX,
		listboxType = ListboxType.RESOLUTION,
		numMaxDisplay = 5,
		lines = resolutionLines,
		option = {path = 'graphics.resolution', values = resolutions}
	},
	{
		pos = {x = 260, y = 100, z = 0},
		guiType = GuiType.CHECKBOX,
		option = {path = 'graphics.fullscreen'}
	},
	{
		pos = {x = 20, y = 310, z = 0},
		size = {x = 150, y = 20},
		guiType = GuiType.BUTTON,
		name = 'Ok',
		buttonType = ButtonType.OK,
		dependencies = {
			{id = 3},
			{id = 4}
		}
	},
	{
		pos = {x = 180, y = 310, z = 0},
		size = {x = 150, y = 20},
		guiType = GuiType.BUTTON,
		name = 'Restore defaults',
		buttonType = ButtonType.DEFAULTS,
		dependencies = {
			{id = 3},
			{id = 4}
		}
	},
	{
		pos = {x = 340, y = 310, z = 0},
		size = {x = 150, y = 20},
		guiType = GuiType.BUTTON,
		name = 'Back',
		screen = 'options.lua',
		buttonType = ButtonType.BACK
	}
}
