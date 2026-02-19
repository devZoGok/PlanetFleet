for i = 1, #game.cpuPlayers do
	game.cpuPlayers[i]:init()

	game.cpuPlayers[i].behaviour = {
		type = BTNodeType.SEQUENCE,
		children = {
			{type = BTNodeType.FUNCTION, func = 'buildLandFactory'},
			{type = BTNodeType.FUNCTION, func = 'trainEngineers'},
			{
				type = BTNodeType.PARALLEL,
				children = {
					{type = BTNodeType.FUNCTION, func = 'buildRefinery'},
					{
						type = BTNodeType.SEQUENCE, 
						children = {
							{type = BTNodeType.FUNCTION, func = 'buildExtractor'},
							{type = BTNodeType.FUNCTION, func = 'buildHarvester'},
						}
					}
				}
			},
			{
				type = BTNodeType.PARALLEL, 
				children = {
					{
						type = BTNodeType.SEQUENCE, 
						children = {
							{type = BTNodeType.FUNCTION, func = 'buildLandForce'},
							{type = BTNodeType.FUNCTION, func = 'formTaskForces'},
						}
					},
					{type = BTNodeType.FUNCTION, func = 'buildPointDefenseRing'},
				}
			},
			{
				type = BTNodeType.PARALLEL, 
				children = game.cpuPlayers[i]:generateTaskForceActions()
			}
			--[[
			]]--
		}
	}
end
