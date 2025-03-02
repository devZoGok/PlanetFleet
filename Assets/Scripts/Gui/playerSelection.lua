Pos = {x = playerListboxPos.x, y = playerListboxPos.y, z = playerListboxPos.z} 
Size = {x = 100, y = 20}
difficulties = {"Easy", "Medium", "Hard"}
space = 10

gui = {
	{
		pos = {x = Pos.x, y = Pos.y + (#factions * Size.y + space) * (lineId + 1), z = Pos.z},
		size = Size,
		guiType = GuiType.LISTBOX,
		numMaxDisplay = 3,
		lines = factions,
		listboxType = ListboxType.FACTIONS
	},
	{
		pos = {x = Pos.x + Size.x + space, y = Pos.y + (#difficulties * Size.y + space) * (lineId + 1), z = Pos.z},
		size = Size,
		guiType = GuiType.LISTBOX,
		numMaxDisplay = 3,
		lines = difficulties,
		listboxType = ListboxType.CPU_DIFFICULTIES
	},
}
