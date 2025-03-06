playerListboxPos = {x = 110, y = 10, z = 0}
Pos = {x = playerListboxPos.x, y = playerListboxPos.y, z = playerListboxPos.z} 
Size = {x = 150, y = 20}
gap = 10
gapFactor = 3

factions = {'None', "America inc.", "European Republic", "Asian Co-Prosperity Sphere"}
difficulties = {"Easy", "Medium", "Hard"}
colors = {'White', 'Black', 'Red'}
teams = {'0', '1', '2', '3', '4'}

gui = {
	{
		pos = {x = Pos.x, y = Pos.y + (gapFactor * Size.y + gap) * (lineId + 1), z = Pos.z},
		size = Size,
		guiType = GuiType.LISTBOX,
		name = 'factions',
		numMaxDisplay = 3,
		lines = factions,
		listboxType = ListboxType.FACTIONS
	},
	{
		pos = {x = Pos.x + Size.x + gap, y = Pos.y + (gapFactor * Size.y + gap) * (lineId + 1), z = Pos.z},
		size = Size,
		guiType = GuiType.LISTBOX,
		name = 'difficulties',
		numMaxDisplay = 3,
		lines = difficulties,
		listboxType = ListboxType.CPU_DIFFICULTIES
	},
	{
		pos = {x = Pos.x + 2 * (Size.x + gap), y = Pos.y + (gapFactor * Size.y + gap) * (lineId + 1), z = Pos.z},
		size = Size,
		guiType = GuiType.LISTBOX,
		name = 'colors',
		numMaxDisplay = 3,
		lines = colors,
		listboxType = ListboxType.COLORS
	},
	{
		pos = {x = Pos.x + 3 * (Size.x + gap), y = Pos.y + (gapFactor * Size.y + gap) * (lineId + 1), z = Pos.z},
		size = Size,
		guiType = GuiType.LISTBOX,
		name = 'teams',
		numMaxDisplay = 3,
		lines = teams,
		listboxType = ListboxType.TEAMS
	},
}
