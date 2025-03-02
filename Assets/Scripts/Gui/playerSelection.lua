--playerListboxPos = {x = 210, y = 100, z = 0}
Pos = {x = playerListboxPos.x, y = playerListboxPos.y, z = playerListboxPos.z} 
Size = {x = 100, y = 20}

gui = {
	{
		pos = {x = Pos.x, y = Pos.y + (#factions * Size.y + 10) * (lineId + 1), z = Pos.z},
		size = Size,
		guiType = GuiType.LISTBOX,
		numMaxDisplay = 3,
		lines = factions,
		listboxType = ListboxType.FACTIONS
	},
}
