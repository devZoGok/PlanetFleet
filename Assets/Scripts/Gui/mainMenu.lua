Size = {x = 150, y = 40}
res = graphics.resolution

res = graphics.resolution
overlaySize = {x = 600, y = 450}
overlayPos = {x = .5 * (res.x - overlaySize.x), y = 100, z = 0}
buttonSize = {x = 150, y = 40}
buttonInitPos = {x = .5 * (res.x - buttonSize.x), y = 250, z = .1}

music = {
	loop = true,
	shuffle = false,
	delay = 2000,
	tracks = {'intro.ogg'}
}
gui = {
	{
		guiType = GuiType.TEXT,
		name = '',
		text = 'Planet Fleet',
		pos = {x = 20, y = 50, z = 0},
		scale = .75,
		font = 'batang.ttf',
		fontFirstChar = 0,
		fontLastChar = 256,
		color = {x = 0, y = 0, z = 1, w = 1}
	},
	{
		pos = {x = 20, y = 100, z = 0},
		size = Size,
		guiType = GuiType.BUTTON,
		name = 'Single player',
		buttonType = ButtonType.SINGLE_PLAYER,
		baseColor = {x = .6, y = .6, z = .6, w = 1},
		hoveredOnColor = {x = .8, y = .8, z = .8, w = 1},
		tooltip = {
			offset = {x = 100, y = -25, z = .1},
			size = {x = 270, y = 20},
			lines = {
				{
					entry = {text = 'Play with computer opponents'},
				},

			}
		}
	},
	{
		pos = {x = 20, y = 160, z = 0},
		size = Size,
		guiType = GuiType.BUTTON,
		name = 'Map editor',
		buttonType = ButtonType.MAP_EDITOR,
		tooltip = {
			offset = {x = 100, y = -25, z = .1},
			size = {x = 340, y = 20},
			lines = {
				{entry = {text = 'Create new maps or edit existing ones'}},
			}
		}
	},
	{
		pos = {x = 20, y = 220, z = 0},
		size = Size,
		guiType = GuiType.BUTTON,
		name = 'Options',
		buttonType = ButtonType.OPTIONS,
		tooltip = {
			offset = {x = 100, y = -25, z = .1},
			size = {x = 200, y = 20},
			lines = {
				{entry = {text = 'Configure Planet Fleet'}},
			}
		}
	},
	{
		pos = {x = 20, y = 280, z = 0},
		size = Size,
		guiType = GuiType.BUTTON,
		name = 'Exit',
		buttonType = ButtonType.EXIT,
		tooltip = {
			offset = {x = 100, y = -25, z = .1},
			size = {x = 150, y = 20},
			lines = {
				{entry = {text = 'Exit Planet Fleet'}},
			}
		}
	}
}
