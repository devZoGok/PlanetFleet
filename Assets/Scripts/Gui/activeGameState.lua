res = graphics.resolution
Size = {x = 70, y = 70}
sz = 210
s = 40

function generateFactionMusic(factionId)
	asianTracks = {'ACS/asian_theme_1.ogg', 'ACS/asian_theme_2.ogg'}
	americanTracks = {'AInc/american_theme_1.ogg', 'AInc/american_theme_2.ogg'}
	europeanTracks = {'ER/european_theme_1.ogg', 'ER/european_theme_2.ogg'}

	factionTracks = {}

	if factionId == 0 then 
		factionTracks = americanTracks
	elseif factionId == 1 then 
		factionTracks = europeanTracks
	elseif factionId == 2 then 
		factionTracks = asianTracks
	end

	return {
		loop = true,
		shuffle = true,
		delay = 2000,
		tracks = factionTracks
	}
end

eyeIcon = 'eye.png'
refIcon = 'refineds.png'
minimapSize = {x = 200, y = 200}
minimapPos = {x = 0, y = res.y - minimapSize.y, z = .9}

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
		pos = {x = s, y = 2 * s, z = 0},
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
		pos = {x = s, y = 3 * s, z = 0},
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
		pos = {x = 0, y = 1 * s, z = 0},
		size = {x = s, y = s},
		imagePath = resIconBasePath .. 'wealth.png',
	},
	{
		guiType = GuiType.GUI_RECTANGLE,
		name = 'research',
		pos = {x = 0, y = 2 * s, z = 0},
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
		pos = {x = minimapSize.x, y = res.y - minimapSize.y, z = 0},
		size = {x = Size.x, y = minimapSize.y - .75 * sz},
		trigger = 10
	},
	{
		guiType = GuiType.GUI_RECTANGLE,
		pos = {x = minimapPos.x, y = res.y - .75 * sz, z = 0},
		size = {x = res.x - minimapPos.x, y = sz * .75},
		color = {x = .6, y = .6, z = .6, w = .4}
	},
}
