res = windowSize
Size = {x = 70, y = 70}
margin = 10
sz = {x = Size.x - margin, y = Size.y - margin}

gui = {
	{
		pos = {x = res.x - Size.x + .5 * margin, y = res.y - Size.y + .5 * margin, z = 0},
		size = sz,
		guiType = GuiType.BUTTON,
		buttonType = ButtonType.FORT_TRAIN,
		trigger = 82
	},
}
