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
				type = BTNodeType.SEQUENCE, 
				children = {
					{type = BTNodeType.FUNCTION, func = 'buildLandDefForce'},
					{type = BTNodeType.FUNCTION, func = 'buildPointDefenseRing'}
				}
			},
					--[[
					]]--
			--[[
			{
				type = BTNodeType.SELECTOR, 
				children = {
					{type = BTNodeType.FUNCTION, func = 'hasLandRouteToAllSpawnPoints'},
					{
						type = BTNodeType.SEQUENCE, 
						children = {
							{type = BTNodeType.FUNCTION, func = 'buildNavalFactory'},
							{type = BTNodeType.FUNCTION, func = 'buildNavalForce'}
						}
					},
				}
			},
			{
				type = BTNodeType.SELECTOR, 
				children = {
					{type = BTNodeType.FUNCTION, func = 'hasLandRouteToSpawnPoint'},
					{
						type = BTNodeType.SEQUENCE, 
						children = {
							{type = BTNodeType.FUNCTION, func = 'boardTransports'},
							{type = BTNodeType.FUNCTION, func = 'moveTransports'}
							{type = BTNodeType.FUNCTION, func = 'unloadTransports'}
						}
					},
				}
			},
			{type = BTNodeType.FUNCTION, func = 'occupySpawnpoint'},
			{
				type = BTNodeType.PARALLEL, 
				children = {
					{type = BTNodeType.FUNCTION, func = 'buildTaskForces'},
					{
						type = BTNodeType.PARALLEL, 
						numMinSuccesses = 2,
						children = game.cpuPlayers[i]:generateTaskforceActions()
					},
				}
			},
			]]--
		}
	}
end
