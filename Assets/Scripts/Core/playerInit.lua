for i = 1, #game.players do
	game.players[i].behaviour = {
		type = BTNodeType.SEQUENCE,
		children = {
			{type = BTNodeType.FUNCTION, func = 'buildFort'},
			{type = BTNodeType.FUNCTION, func = 'trainEngineers'},
			{
				type = BTNodeType.PARALLEL,
				children = {
					{
						type = BTNodeType.SEQUENCE, 
						children = {
							{type = BTNodeType.FUNCTION, func = 'buildHarvester'},
						}
					},
					{type = BTNodeType.FUNCTION, func = 'buildRefinery'},
					{
						type = BTNodeType.SEQUENCE, 
						children = {
							{type = BTNodeType.FUNCTION, func = 'buildExtractor'},
							{type = BTNodeType.FUNCTION, func = 'startHarvesting'},
						}
					}
				}
			},
			{
				type = BTNodeType.PARALLEL, 
				children = {
					{type = BTNodeType.FUNCTION, func = 'buildTaskForces'},
					{
						type = BTNodeType.PARALLEL, 
						numMinSuccesses = 2,
						children = game.players[i]:generateTaskforceActions()
					},
				}
			},
			--[[
			]]--
		}
	}
end
