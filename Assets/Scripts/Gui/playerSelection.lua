playerListboxPos = {x = 20, y = 30, z = 0}
Pos = {x = playerListboxPos.x, y = playerListboxPos.y, z = playerListboxPos.z} 
FactionListboxSize = {x = 270, y = 20}
Size = {x = 150, y = 20}
gap = 10
gapFactor = 3

colors = {
	{name = 'White', code = {r = 1, g = 1, b = 1}}, 
	{name = 'Red', code = {r = 1, g = 0, b = 0}}, 
	{name = 'Green', code = {r = 0, g = 1, b = 0}}, 
	{name = 'Blue', code = {r = 0, g = 0, b = 1}}, 
	{name = 'Yellow', code = {r = 1, g = 1, b = 0}}, 
	{name = 'Orange', code = {r = 1, g = .5, b = 0}}, 
	{name = 'Brown', code = {r = .5, g = .25, b = 0}}, 
	{name = 'Purple', code = {r = .7, g = 0, b = .7}}, 
	{name = 'Pink', code = {r = 1, g = .25, b = .5}}, 
}

function generateColorTable(colors, fieldName)
	tbl = {}

	for i = 1, #colors do
		table.insert(tbl, colors[i][fieldName])
	end

	return tbl
end

function generateGui(humanPlayer)
	factions = {'Random', 'Asian Co-Prosperity Sphere', 'America inc.', 'European Republic'}
	difficulties = {'None', "Easy AI", "Medium AI", "Hard AI"}
	teams = {'0', '1', '2', '3', '4'}

	firstGui = nil

	if humanPlayer then
		table.remove(difficulties, 1)

		firstGui = {
			guiType = GuiType.TEXT,
			name = '_mainPlayer',
			text = 'Player',
			pos = {x = 20, y = 120, z = 0},
			scale = .2,
			font = 'batang.ttf',
			fontFirstChar = 0,
			fontLastChar = 256,
			color = {x = 1, y = 1, z = 1, w = 1}
		}
	else
		firstGui = {
			pos = {x = Pos.x, y = Pos.y + (gapFactor * Size.y + gap) * (lineId + 1), z = Pos.z},
			size = Size,
			guiType = GuiType.LISTBOX,
			name = 'difficulties',
			numMaxDisplay = 3,
			lines = difficulties,
			listboxType = ListboxType.CPU_DIFFICULTIES
		}
	end

	local gui = {
		firstGui,
		{
			pos = {x = Pos.x + Size.x + gap, y = Pos.y + (gapFactor * Size.y + gap) * (lineId + 1), z = Pos.z},
			size = FactionListboxSize,
			guiType = GuiType.LISTBOX,
			name = 'factions',
			numMaxDisplay = 3,
			lines = factions,
			listboxType = ListboxType.FACTIONS
		},
		{
			pos = {x = Pos.x + Size.x + FactionListboxSize.x + 2 * gap, y = Pos.y + (gapFactor * Size.y + gap) * (lineId + 1), z = Pos.z},
			size = Size,
			guiType = GuiType.LISTBOX,
			name = 'colors',
			numMaxDisplay = 3,
			lines = generateColorTable(colors, 'name'),
			listboxType = ListboxType.COLORS
		},
		{
			pos = {x = Pos.x + FactionListboxSize.x + 2 * Size.x + 3 * gap, y = Pos.y + (gapFactor * Size.y + gap) * (lineId + 1), z = Pos.z},
			size = {x = 50, y = 20},
			guiType = GuiType.LISTBOX,
			name = 'teams',
			numMaxDisplay = 3,
			lines = teams,
			listboxType = ListboxType.TEAMS
		},
	}

	return gui
end

gui = generateGui(lineId == MAIN_PLAYER_ID)
