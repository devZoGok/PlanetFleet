res = windowSize
Size = {x = 70, y = 70}

gui = {
	{
		pos = {x = res.x - Size.x, y = res.y - Size.y, z = 0},
		size = Size,
		guiType = GuiType.BUTTON,
		buttonType = ButtonType.NAVAL_FACTORY_TRAIN,
		trigger = 66
	}
}
