for i = 1, #game.cpuPlayers do
	player = game.cpuPlayers[i]
	player:init()

	player.behaviour = {
		type = BTNodeType.SEQUENCE,
		children = {
			{type = BTNodeType.FUNCTION, func = player.buildLandFactory},
			{
				type = BTNodeType.FUNCTION, 
				func = player.buildTaskForces, 
				args = {
					factoryClass = UnitClass.LAND_FACTORY,
					taskForceData = {{class = UnitClass.CYBORG_ENGINEER, numUnits = 3}}
				}
			},
			{
				type = BTNodeType.PARALLEL,
				children = {
					{type = BTNodeType.FUNCTION, func = player.buildRefinery},
					{
						type = BTNodeType.SEQUENCE, 
						children = {
							{type = BTNodeType.FUNCTION, func = player.buildExtractor},
							{type = BTNodeType.FUNCTION, func = player.buildHarvester},
						}
					}
				}
			},
			{
				type = BTNodeType.SELECTOR, 
				children = {
					{type = BTNodeType.FUNCTION, func = player.canReachAllSpawnPoints},
					{
						type = BTNodeType.SEQUENCE, 
						children = {
							{type = BTNodeType.FUNCTION, func = player.buildNavalFactory},
							{
								type = BTNodeType.FUNCTION, 
								func = player.buildTaskForces, 
								args = {factoryClass = UnitClass.NAVAL_FACTORY, taskForceData = player.navalTaskForceData}
							},
						}
					},
				}
			},
			{
				type = BTNodeType.PARALLEL, 
				children = {
					{
						type = BTNodeType.SEQUENCE, 
						children = {
							{
								type = BTNodeType.FUNCTION, 
								func = player.buildTaskForces, 
								args = {
									factoryClass = UnitClass.LAND_FACTORY,
									numTaskForces = Map.getSingleton():getNumSpawnPoints() - 1,
									taskForceData = player.landTaskForceData
								}
							},
							{type = BTNodeType.FUNCTION, func = player.formLandTaskForces},
						}
					},
					{type = BTNodeType.FUNCTION, func = player.buildPointDefenseRing},
				}
			},
			{
				type = BTNodeType.PARALLEL, 
				children = player:generateTaskForceActions()
			}
		}
	}
end
