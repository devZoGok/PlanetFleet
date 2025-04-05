res = graphics.resolution
Size = {x = 70, y = 70}
sz = 210
s = 40

eyeIcon = 'eye.png'
refIcon = 'refineds.png'
minimapSize = {x = 200, y = 200}
minimapPos = {x = res.x - minimapSize.x, y = 0, z = .9}

resTextScale = {x = .5, y = .5}
resIconBasePath = 'Icons/Resources/'
pointerTex = 'pointer.png'
attackTex = 'attack.png'
garrisonTex = 'garrison.png'
supplyTex = 'supply.png'
hackTex = 'hack.png'
noGarrisonTex = 'noGarrison.png'
noSupplyTex = 'noSupply.png'
noHackTex = 'noHack.png'

gui = {
	{
		guiType = GuiType.BUTTON,
		buttonType = ButtonType.MINIMAP,
		name = 'minimap',
		pos = minimapPos,
		size = minimapSize,
		color = {x = .5, y = .5, z = .5, w = 1}
	},
	{
		guiType = GuiType.TEXT,
		name = 'refineds',
		text = '',
		pos = {x = s, y = s, z = 0},
		scale = resTextScale,
		font = 'batang.ttf',
		fontFirstChar = 0,
		fontLastChar = 256,
		color = {x = 1, y = 1, z = 1, w = 1}
	},
	{
		guiType = GuiType.TEXT,
		name = 'wealth',
		text = '',
		pos = {x = 200 + s, y = s, z = 0},
		scale = resTextScale,
		font = 'batang.ttf',
		fontFirstChar = 0,
		fontLastChar = 256,
		color = {x = 1, y = 1, z = 1, w = 1}
	},
	{
		guiType = GuiType.TEXT,
		name = 'research',
		text = '',
		pos = {x = 400 + s, y = s, z = 0},
		scale = resTextScale,
		font = 'batang.ttf',
		fontFirstChar = 0,
		fontLastChar = 256,
		color = {x = 1, y = 1, z = 1, w = 1}
	},
	{
		guiType = GuiType.GUI_RECTANGLE,
		name = 'refineds',
		pos = {x = 0, y = 0, z = 0},
		size = {x = s, y = s},
		imagePath = resIconBasePath .. 'refineds.png',
	},
	{
		guiType = GuiType.GUI_RECTANGLE,
		name = 'wealth',
		pos = {x = 200, y = 0, z = 0},
		size = {x = s, y = s},
		imagePath = resIconBasePath .. 'wealth.png',
	},
	{
		guiType = GuiType.GUI_RECTANGLE,
		name = 'research',
		pos = {x = 400, y = 0, z = 0},
		size = {x = s, y = s},
		imagePath = resIconBasePath .. 'research.png',
	},
	{
		guiType = GuiType.TEXT,
		name = 'depth',
		text = '',
		pos = {x = 0, y = 400, z = 0},
		scale = {x = 0, y = 1},
		font = 'batang.ttf',
		fontFirstChar = 0,
		fontLastChar = 256,
		color = {x = 1, y = 1, z = 1, w = 1}
	},
	{
		guiType = GuiType.BUTTON,
		buttonType = ButtonType.PLAYER_TRADE,
		name = 'Trade',
		guiScreen = 'tradingHub.lua',
		pos = {x = res.x - Size.x, y = 200, z = 0},
		size = Size,
		trigger = 10
	},
	{
		guiType = GuiType.GUI_RECTANGLE,
		pos = {x = res.x - sz, y = res.y - sz, z = 0},
		size = {x = sz, y = sz},
		color = {x = .6, y = .6, z = .6, w = .4}
	},
	{
		guiType = GuiType.GUI_RECTANGLE,
		pos = {x = 0, y = res.y - sz, z = 0},
		size = {x = sz, y = sz},
		color = {x = .6, y = .6, z = .6, w = .4}
	},
	{
		guiType = GuiType.GUI_RECTANGLE,
		pos = {x = sz, y = res.y - .75 * sz, z = 0},
		size = {x = res.x - 2 * sz, y = sz * .75},
		color = {x = .6, y = .6, z = .6, w = .4}
	},
}
