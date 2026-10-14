game = {cpuPlayers = {}}

GameObjectFrameStatus = {
	PLACEABLE = 0, 
	BLOCKED_BY_FOG_OF_WAR = 1,
	BLOCKED_BY_UNIT = 2,
	BLOCKED_BY_MAP_BOUNDS = 3,
	BLOCKED_BY_DIFF_TERR = 4,
	BLOCKED_BY_BUMPY_TERR = 5
};

function cellIdsEqual(cid1, cid2)
	for i = 1, #cid1 do
		if cid1[i] ~= cid2[i] then return false end
	end

	return true
end
