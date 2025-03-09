Player.numStartEngis = 3
Player.baseDir = nil 

Player.numDefWarMechs = 1

Player.numTaskForceWarMechs = 5
Player.numTaskForceTanks = 1
Player.numTaskForceArtillery = 1
Player.taskForceClearing = false

Player.movingToHostileSpawnPoint = false

--TODO use enum-like values instead of literals for order types
--TODO simplify building construction
function Player:buildFort()
	forts = self:getUnitsByClass(UnitClass.FORT, 1)

	if #forts > 0 then
		return forts[1]:toStructure():getBuildStatus() == 100 and BTNodeResult.SUCCESS or BTNodeResult.RUNNING
	end

	engis = self:getUnitsByClass(UnitClass.ENGINEER, 1)
	if #engis == 0 then return BTNodeResult.FAILURE end

	self:deselectUnits()
	self:selectUnits({engis[1]})

	map = Map.getSingleton()
	sp = map:getSpawnPoint(self:getSpawnPointId())
	minDistId = nil 

	for i = 0, map:getNumSpawnPoints() - 1 do
		if i == self:getSpawnPointId() then goto continue end

		if minDistId == nil then minDistId = i end

		if map:getSpawnPoint(i):getDistanceFrom(sp) < map:getSpawnPoint(minDistId):getDistanceFrom(sp) then
			minDistId = i
		end

		::continue::
	end

	self.baseDir = map:getSpawnPoint(minDistId):subtr(sp):norm()
	right = (self.baseDir:getAngleBetween(Vector3:new(1, 0, 0)) > 1.57)
	angle = self.baseDir:getAngleBetween(Vector3:new(0, 0, 1)) * (right and -1 or 1)
	fort = GameObjectFactory.createUnit(self, UnitId.FORT, sp, Quaternion:new(angle, Vector3:new(0, 1, 0)), 0)
	self:addUnit(fort)
	self:issueOrder(1, Vector3:new(0, 0, 0), {Target:new(fort, Vector3:new(0, 0, 0))}, false)

	return BTNodeResult.RUNNING
end

function Player:trainEngineers()
	fort = self:getUnitsByClass(UnitClass.FORT, 1)[1]:toFactory()
	engineers = self:getUnitsByClass(UnitClass.ENGINEER, -1)
	engineerDiffNum = self.numStartEngis - #engineers
	queueDiff = engineerDiffNum - #fort:getQueue()

	for i = 1, queueDiff do fort:appendToQueue(0) end

	engisStartNumBuilt = (#self:getUnitsByClass(UnitClass.ENGINEER, -1) == self.numStartEngis)
	return (engisStartNumBuilt and BTNodeResult.SUCCESS or BTNodeResult.RUNNING)
end

function Player:findIdleEngineer()
	engineers = self:getUnitsByClass(UnitClass.ENGINEER, -1)

	for i = 1, #engineers do
		if engineers[i]:getNumOrders() == 0 then return engineers[i] end
	end

	return nil
end

function Player:buildStructure(engineer, buildingId, buildPos, buildAngle)
	if not engineer then return BTNodeResult.FAILURE end

	unitClass = units[buildingId + 1].unitClass

	if #self:getUnitsByClass(unitClass, 1) == 0 then
		self:deselectUnits()
		self:selectUnits({engineer})
		
		building = GameObjectFactory.createUnit(self, buildingId, buildPos, Quaternion:new(1, 0, 0, 0), 0)
		self:addUnit(building)
		self:issueOrder(1, Vector3:new(0, 0, 0), {Target:new(building, Vector3:new(0, 0, 0))}, false)
	end

	building = self:getUnitsByClass(unitClass, 1)[1]
	return (building:toStructure():getBuildStatus() == 100 and BTNodeResult.SUCCESS or BTNodeResult.RUNNING)
end

-- TODO dispatch another engineer if the previous is destroyed before completion
function Player:buildLandFactory()
	buildPos = Map.getSingleton():getSpawnPoint(self:getSpawnPointId()):add(self.baseDir:mult(20))
	return self:buildStructure(self:findIdleEngineer(), UnitId.LAND_FACTORY, buildPos, .6)
end

function Player:buildRefinery()
	buildPos = Map.getSingleton():getSpawnPoint(self:getSpawnPointId()):add(self.baseDir:mult(20))
	return self:buildStructure(self:findIdleEngineer(), UnitId.REFINERY, buildPos, -.1)
end

-- TODO optimize deposit position check
function Player:buildExtractor()
	deposits = {}
	players = Game.getSingleton():getPlayers()

	for i = 1, #players do
		deps = players[i]:getResourceDeposits()
		table.move(deps, 1, #deps, #deposits + 1, deposits)
	end

	if #deposits == 0 then return BTNodeResult.FAILURE end

	depPos = deposits[1]:getPos()
	spawnPoint = Map.getSingleton():getSpawnPoint(self:getSpawnPointId())

	for i = 1, #deposits do
		if deposits[i]:getPos():getDistanceFrom(spawnPoint) < depPos:getDistanceFrom(spawnPoint) then
			depPos = deposits[i]:getPos()
		end
	end

	return self:buildStructure(self:findIdleEngineer(), UnitId.EXTRACTOR, depPos, 0)
end

function Player:buildHarvester()
	if #self:getUnitsByClass(UnitClass.RESOURCE_ROVER, 1) > 0 then
		return BTNodeResult.SUCCESS
	end

	factories = self:getUnitsByClass(UnitClass.LAND_FACTORY, 1)
	if #factories == 0 then return BTNodeResult.FAILURE end

	factory = factories[1]:toFactory()

	if #factory:getQueue() == 0 then
		factory:appendToQueue(3)
		return BTNodeResult.RUNNING
	end
end

function Player:startHarvesting()
	harvesters = self:getUnitsByClass(UnitClass.RESOURCE_ROVER, -1)

	for i = 1, #harvesters do
		if harvesters[i]:getNumOrders() > 0 and harvesters[i]:getOrder(0).type == 7 then
			return BTNodeResult.SUCCESS
		end
	end

	extractors = self:getUnitsByClass(UnitClass.EXTRACTOR, 1)
	if #extractors == 0 then return BTNodeResult.FAILURE end

	self:deselectUnits()
	self:selectUnits({harvesters[1]})
	self:issueOrder(7, Vector3:new(0, 0, 0), {Target:new(extractors[1], Vector3:new(0, 0, 0))}, false)

	return BTNodeResult.SUCCESS
end

function Player:buildTaskforceUnitGroup(numUnits, currNumUnits, factory, buId)
	if currNumUnits < numUnits then
		for i = 1, numUnits - currNumUnits do
			factory:appendToQueue(buId)
		end
	end
end

function Player:buildTaskforce()
	currNumBuiltWarMechs = #self:getUnitsByClass(UnitClass.WAR_MECH, -1)
	currNumBuiltTanks = #self:getUnitsByClass(UnitClass.TANK, -1)
	currNumBuiltArtillery = #self:getUnitsByClass(UnitClass.ARTILLERY, -1)
	currNumWarMechs = currNumBuiltWarMechs
	currNumTanks = currNumBuiltTanks
	currNumArtillery = currNumBuiltArtillery
	currLandFactories = self:getUnitsByClass(UnitClass.LAND_FACTORY, -1)

	for i = 1, #currLandFactories do
		currNumWarMechs = currNumWarMechs + currLandFactories[i]:toFactory():getNumQueueUnitsById(0)
		currNumTanks = currNumTanks + currLandFactories[i]:toFactory():getNumQueueUnitsById(1)
		currNumArtillery = currNumArtillery + currLandFactories[i]:toFactory():getNumQueueUnitsById(2)
	end

	numWarMechs = self.numDefWarMechs + self.numTaskForceWarMechs
	numTanks = self.numTaskForceTanks
	numArtillery = self.numTaskForceArtillery

	if currNumBuiltWarMechs >= numWarMechs and currNumBuiltTanks >= numTanks and currNumBuiltArtillery >= numArtillery then
		return true
	end

	landFactory = currLandFactories[1]:toFactory()

	self:buildTaskforceUnitGroup(numWarMechs, currNumWarMechs, landFactory, 0)
	self:buildTaskforceUnitGroup(numTanks, currNumTanks, landFactory, 1)
	self:buildTaskforceUnitGroup(numArtillery, currNumArtillery, landFactory, 2)

	return false
end

function Player:enemiesCloseBy()
	--if self.taskForceClearing then return true end

	self:selectUnits(self:getUnitsByClass(UnitClass.WAR_MECH, self.numTaskForceWarMechs))
	self:selectUnits(self:getUnitsByClass(UnitClass.TANK, self.numTaskForceTanks))
	self:selectUnits(self:getUnitsByClass(UnitClass.ARTILLERY, self.numTaskForceArtillery))
	taskForce = self:getSelectedUnits()

	uns = {}
	players = Game.getSingleton():getPlayers()

	for i = 1, #players do
		if players[i] == self or players[i]:getTeam() == self:getTeam() then goto continue end

		un = players[i]:getUnits()
		uns = table.move(un, 1, #un, #uns + 1, uns)

		::continue::
	end

	targUnit = nil

	for i = 1, #uns do
		for j = 1, #taskForce do
			if taskForce[j]:getPos():getDistanceFrom(uns[i]:getPos()) < taskForce[j]:getLineOfSight() then
				targUnit = uns[i]
				break
			end
		end

		if targUnit then break end
	end

	if not self.taskForceClearing and targUnit then
		print('clearing...')
		self.taskForceClearing = true

		self:deselectUnits()
		self:selectUnits(taskForce)
		self:issueOrder(2, Vector3:new(0, 0, 0), {Target:new(targUnit, Vector3:new(0, 0, 0))}, false)
	elseif self.taskForceClearing and not targUnit then
		print('clearing finished...')
		self.taskForceClearing = false
	end

	return self.taskForceClearing
end

function Player:attackSpawnPoint()
	if self.movingToHostileSpawnPoint then return true end
	print('advancing')

	taskForceMechs = self:getUnitsByClass(UnitClass.WAR_MECH, self.numTaskForceWarMechs)
	taskForceTanks = self:getUnitsByClass(UnitClass.TANK, self.numTaskForceTanks)
	taskForceArtillery = self:getUnitsByClass(UnitClass.ARTILLERY, self.numTaskForceArtillery)

	self:deselectUnits()
	self:selectUnits(taskForceMechs)
	self:selectUnits(taskForceTanks)
	self:selectUnits(taskForceArtillery)

	taskForce = self:getSelectedUnits()

	for i = 1, #taskForce do
		taskForce[i]:setState(0)
	end

	map = Map.getSingleton()
	numSpawnPoints = map:getNumSpawnPoints()
	enemySpawnPoint = nil
	players = Game.getSingleton():getPlayers()

	for i = 1, numSpawnPoints do
		if i - 1 ~= self:getSpawnPointId() then
			enemySpawnPoint = map:getSpawnPoint(players[i]:getSpawnPointId())
			break
		end
	end

	self:issueOrder(2, Vector3:new(0, 0, 0), {Target:new(nil, enemySpawnPoint)}, false)
	self.movingToHostileSpawnPoint = true

	return true
end

Player.behaviour = {
	type = BTNodeType.SEQUENCE,
	children = {
		{type = BTNodeType.FUNCTION, func = 'buildFort'},
		{type = BTNodeType.FUNCTION, func = 'trainEngineers'},
		{
			type = BTNodeType.PARALLEL,
			numMinSuccesses = 3,
			children = {
				{
					type = BTNodeType.SEQUENCE, 
					children = {
						{type = BTNodeType.FUNCTION, func = 'buildLandFactory'},
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
		--[[
		{type = BTNodeType.FUNCTION, func = 'startHarvesting'},
		{type = BTNodeType.FUNCTION, func = 'buildTaskforce'},
		{
			type = BTNodeType.SELECTOR, 
			children = {
				{type = BTNodeType.FUNCTION, func = 'enemiesCloseBy'},
				{type = BTNodeType.FUNCTION, func = 'attackSpawnPoint'}
			}
		},
		]]--
	}
}
