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
					taskForceData = {{class = UnitClass.ROBO_ENGINEER, numUnits = 5}}
				}
			},
			{
				type = BTNodeType.PARALLEL,
				children = {
					{type = BTNodeType.FUNCTION, func = player.buildRefinery},
					{
						type = BTNodeType.SEQUENCE, 
						children = {
							{type = BTNodeType.FUNCTION, func = player.buildExtractors},
							{
								type = BTNodeType.FUNCTION, 
								func = player.buildTaskForces, 
								args = {
									factoryClass = UnitClass.LAND_FACTORY,
									taskForceData = {{class = UnitClass.RESOURCE_ROVER, numUnits = 4}}
								}
							},
						}
					}
				}
			},
			{type = BTNodeType.FUNCTION, func = player.startHarvesting},
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
									numTaskForces = player:getNumEmptyTaskForces(),
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
