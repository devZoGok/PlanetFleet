res = graphics.resolution
Size = {x = res.x * .5, y = res.y * .5}
techButtonSize = {x = 50, y = 50}
Pos = {x = res.x * .3, y = res.y * .1, z = .1}
margin = {top = 10, left = 10, right = 10, bottom = 10}
baseTechPath = 'Icons/Technologies/'
heightOffset = techButtonSize.y + 10

gui = {
	{
		guiType = GuiType.GUI_RECTANGLE,
		pos = Pos,
		size = Size,
		color = {x = 0, y = 0, z = 0, w = .6}
	},
	{
		pos = {x = Pos.x + .5 * (Size.x - techButtonSize.x), y = Pos.y + margin.top, z = .11},
		size = techButtonSize,
		name = "ER APT",
		imagePath = baseTechPath .. 'apt.jpg',
		guiType = GuiType.BUTTON,
		buttonType = ButtonType.RESEARCH,
		techId = TechId.APT,
		trigger = 82,
	},
	{
		pos = {x = Pos.x + margin.left, y = Pos.y + margin.top + heightOffset, z = .11},
		size = techButtonSize,
		name = "FPT",
		imagePath = baseTechPath .. 'fpt.jpg',
		guiType = GuiType.BUTTON,
		buttonType = ButtonType.RESEARCH,
		techId = TechId.FPT,
		trigger = 82,
	},
	{
		pos = {x = Pos.x + margin.left, y = Pos.y + margin.top + 2 * heightOffset, z = .11},
		size = techButtonSize,
		name = "Cruisers",
		imagePath = baseTechPath .. 'cruisers.jpg',
		guiType = GuiType.BUTTON,
		buttonType = ButtonType.RESEARCH,
		techId = TechId.CRUISERS,
		trigger = 82,
	},
	{
		pos = {x = Pos.x + .5 * (Size.x - techButtonSize.x), y = Pos.y + margin.top + heightOffset, z = .11},
		size = techButtonSize,
		name = "EFT",
		imagePath = baseTechPath .. 'ept.jpg',
		guiType = GuiType.BUTTON,
		buttonType = ButtonType.RESEARCH,
		techId = TechId.EFT,
		trigger = 82,
	},
	{
		pos = {x = Pos.x + .5 * (Size.x - 3 * techButtonSize.x), y = Pos.y + margin.top + 2 * heightOffset, z = .11},
		size = techButtonSize,
		name = "Stealth",
		imagePath = baseTechPath .. 'stealth.jpg',
		guiType = GuiType.BUTTON,
		buttonType = ButtonType.RESEARCH,
		techId = TechId.ST_SUB,
		trigger = 82,
	},
	{
		pos = {x = Pos.x + .5 * (Size.x + 3 * techButtonSize.x), y = Pos.y + margin.top + 2 * heightOffset, z = .11},
		size = techButtonSize,
		name = "Robotics",
		imagePath = baseTechPath .. 'robo.jpg',
		guiType = GuiType.BUTTON,
		buttonType = ButtonType.RESEARCH,
		techId = TechId.ROBO,
		trigger = 82,
	},
	{
		pos = {x = Pos.x + Size.x - (margin.right + techButtonSize.x), y = Pos.y + margin.top + heightOffset, z = .11},
		size = techButtonSize,
		name = "PCT",
		imagePath = baseTechPath .. 'pct.jpg',
		guiType = GuiType.BUTTON,
		buttonType = ButtonType.RESEARCH,
		techId = TechId.PCT,
		trigger = 82,
	},
	{
		pos = {x = Pos.x + Size.x - (margin.right + 3 * techButtonSize.x), y = Pos.y + margin.top + 2 * heightOffset, z = .11},
		size = techButtonSize,
		name = "Carriers",
		imagePath = baseTechPath .. 'carriers.jpg',
		guiType = GuiType.BUTTON,
		buttonType = ButtonType.RESEARCH,
		techId = TechId.CARRIERS,
		trigger = 82,
	},
	{
		pos = {x = Pos.x + Size.x - (margin.right + techButtonSize.x), y = Pos.y + margin.top + 2 * heightOffset, z = .11},
		size = techButtonSize,
		name = "Tanks",
		imagePath = baseTechPath .. 'tanks.jpg',
		guiType = GuiType.BUTTON,
		buttonType = ButtonType.RESEARCH,
		techId = TechId.TANKS,
		trigger = 82,
	},
}
