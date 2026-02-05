Player.numStartEngis = 3
Player.baseDir = nil 
Player.numDefWarMechs = 1
Player.behaviour = {}

Player.taskForceData = {
	{class = UnitClass.WAR_MECH, buildId = 0, numUnits = 2},
	{class = UnitClass.TANK, buildId = 1, numUnits = 1},
	{class = UnitClass.ARTILLERY, buildId = 2, numUnits = 1}
}

Player.taskForces = {}

function Player:getBuildableUnitIdFromClass(unit, unitClass)
	buildableUnits = unit:getBuildableUnits()

	for i = 1, #buildableUnits do
		if buildableUnits[i].buildable and units[buildableUnits[i].id + 1].unitClass == unitClass then
			return i
		end
	end

	return -1
end

function Player:getFactionUnitIdFromClass(unitClass)
	faction = self:getFaction()

	if unitClass == UnitClass.LAND_FACTORY then
		if faction == 0 then 
			return UnitId.AINC_LAND_FACTORY
		elseif faction == 1 then 
			return UnitId.ER_LAND_FACTORY
		elseif faction == 2 then 
			return UnitId.ACS_LAND_FACTORY
		end
	elseif unitClass == UnitClass.REFINERY then
		if faction == 0 then 
			return UnitId.AINC_REFINERY
		elseif faction == 1 then 
			return UnitId.ER_REFINERY
		elseif faction == 2 then 
			return UnitId.ACS_REFINERY
		end
	elseif unitClass == UnitClass.LAB then
		if faction == 0 then 
			return UnitId.AINC_LAB
		elseif faction == 1 then 
			return UnitId.ER_LAB
		elseif faction == 2 then 
			return UnitId.ACS_LAB
		end
	elseif unitClass == UnitClass.ENGINEER then
		if faction == 0 then 
			return UnitId.AINC_ROBO_ENGINEER
		elseif faction == 1 then 
			return UnitId.ER_ROBO_ENGINEER
		elseif faction == 2 then 
			return UnitId.ACS_ROBO_ENGINEER
		end
	elseif unitClass == UnitClass.MECH then
		if faction == 0 then 
			return UnitId.AINC_MECH
		elseif faction == 1 then 
			return UnitId.ER_MECH
		elseif faction == 2 then 
			return UnitId.ACS_MECH
		end
	elseif unitClass == UnitClass.TANK then
		if faction == 0 then 
			return UnitId.AINC_TANK
		elseif faction == 1 then 
			return UnitId.ER_TANK
		elseif faction == 2 then 
			return UnitId.ACS_TANK
		end
	elseif unitClass == UnitClass.ARTILLERY then
		if faction == 0 then 
			return UnitId.AINC_ARTILLERY
		elseif faction == 1 then 
			return UnitId.ER_ARTILLERY
		elseif faction == 2 then 
			return UnitId.ACS_ARTILLERY
		end
	end

	return -1
end

--TODO simplify building construction
function Player:buildFort(arguments)
	forts = self:getUnitsByClass(UnitClass.LAND_FACTORY, 1)

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
	fort = GameObjectFactory.createUnit(self, self:getFactionUnitIdFromClass(UnitClass.LAND_FACTORY), sp, Quaternion:new(angle, Vector3:new(0, 1, 0)), 0)
	self:addUnit(fort)
	self:issueOrder(OrderType.BUILD, Vector3:new(0, 0, 0), {Target:new(fort:toGameObject(), Vector3:new(0, 0, 0))}, false)

	return BTNodeResult.RUNNING
end

function Player:trainEngineers(arguments)
	fort = self:getUnitsByClass(UnitClass.LAND_FACTORY, 1)[1]:toFactory()
	engineers = self:getUnitsByClass(UnitClass.ENGINEER, -1)
	engineerDiffNum = self.numStartEngis - #engineers
	queueDiff = engineerDiffNum - #fort:getQueue()

	engiBuildId = self:getBuildableUnitIdFromClass(fort, UnitClass.ENGINEER)

	for i = 1, queueDiff do fort:appendToQueue(engiBuildId) end

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
		self:issueOrder(OrderType.BUILD, Vector3:new(0, 0, 0), {Target:new(building:toGameObject(), Vector3:new(0, 0, 0))}, false)
	end

	building = self:getUnitsByClass(unitClass, 1)[1]
	return (building:toStructure():getBuildStatus() == 100 and BTNodeResult.SUCCESS or BTNodeResult.RUNNING)
end

function Player:buildRefinery(arguments)
	buildPos = Map.getSingleton():getSpawnPoint(self:getSpawnPointId()):add(self.baseDir:mult(20))
	return self:buildStructure(self:findIdleEngineer(), self:getFactionUnitIdFromClass(UnitClass.REFINERY), buildPos, -.1)
end

-- TODO optimize deposit position check
function Player:buildExtractor(arguments)
	deposits = {}
	players = Game.getSingleton():getPlayers(true)

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

function Player:buildHarvester(arguments)
	if self:getUnitsByClass(UnitClass.RESOURCE_ROVER, 1)[1] then
		return BTNodeResult.SUCCESS
	end

	factory = self:getUnitsByClass(UnitClass.LAND_FACTORY, 1)[1]
	if not factory then return BTNodeResult.FAILURE end

	factory = factory:toFactory()

	if #factory:getQueue() == 0 then factory:appendToQueue(3) end

	return BTNodeResult.RUNNING
end

function Player:startHarvesting()
	harvesters = self:getUnitsByClass(UnitClass.RESOURCE_ROVER, -1)

	for i = 1, #harvesters do
		if harvesters[i]:getNumOrders() > 0 and harvesters[i]:getOrder(0).type == 7 then
			return BTNodeResult.SUCCESS
		end
	end

	extractor = self:getUnitsByClass(UnitClass.EXTRACTOR, 1)[1]
	if not extractor then return BTNodeResult.FAILURE end

	self:deselectUnits()
	self:selectUnits({harvesters[1]})
	self:issueOrder(OrderType.SUPPLY, Vector3:new(0, 0, 0), {Target:new(extractor:toGameObject(), Vector3:new(0, 0, 0))}, false)

	return BTNodeResult.SUCCESS
end

function Player:buildTaskForces(arguments)
	landFactory = self:getUnitsByClass(UnitClass.LAND_FACTORY, 1)[1]

	if not landFactory then return BTNodeResult.FAILURE end

	landFactory = landFactory:toFactory()
	numAttackableSpawnPoints = Map.getSingleton():getNumSpawnPoints() - 1

	if #landFactory:getQueue() == 0 then
		enoughUnits = true

		for i = 1, #self.taskForceData do
			tfUnits = self:getUnitsByClass(self.taskForceData[i].class, -1)

			if #tfUnits < numAttackableSpawnPoints * self.taskForceData[i].numUnits then
				enoughUnits = false
				break
			end
		end

		if enoughUnits then return BTNodeResult.SUCCESS end
	end

	if #landFactory:getQueue() > 0 then return BTNodeResult.RUNNING end

	for i = 1, numAttackableSpawnPoints do
		for j = 1, #self.taskForceData do
			for k = 1, self.taskForceData[j].numUnits do
				landFactory:appendToQueue(self.taskForceData[j].buildId)
			end
		end
	end

	return BTNodeResult.RUNNING
end

function Player:formTaskForce(arguments)
	if self.taskForces[arguments.tfId] then return BTNodeResult.SUCCESS end

	unitGroups = {}

	for i = 1, #self.taskForceData do
		unitGroups[i] = self:getUnitsByClass(self.taskForceData[i].class, -1)
		enoughGroupUnits = (#unitGroups[i] - (arguments.tfId - 1) * self.taskForceData[i].numUnits >= self.taskForceData[i].numUnits)

		if not enoughGroupUnits then return BTNodeResult.FAILURE end
	end

	self.taskForces[arguments.tfId] = {units = {}, attacking = false, clearing = false, spawnPointId = arguments.spId}

	for i = 1, #self.taskForceData do
		unitGroup = {}

		subArrId = (arguments.tfId - 1) * self.taskForceData[i].numUnits + 1
		table.move(unitGroups[i], subArrId, subArrId + self.taskForceData[i].numUnits, 1, unitGroup)
		table.move(unitGroup, 1, #unitGroup, #self.taskForces[arguments.tfId].units + 1, self.taskForces[arguments.tfId].units)
	end

	return BTNodeResult.SUCCESS
end

function Player:clearNearEnemies(arguments)
	taskForce = self.taskForces[arguments.tfId]

	if taskForce.clearing then return BTNodeResult.SUCCESS end

	self:selectUnits(taskForce.units)

	targetUnits = {}
	players = Game.getSingleton():getPlayers(false)

	for i = 1, #players do
		if players[i] == self or players[i]:getTeam() == self:getTeam() then goto continue end

		plUnits = players[i]:getUnits()
		targetUnits = table.move(plUnits, 1, #plUnits, #targetUnits + 1, targetUnits)

		::continue::
	end

	targUnit = nil

	for i = 1, #targetUnits do
		for j = 1, #taskForce.units do
			tfUnit = taskForce.units[j]

			if tfUnit:getPos():getDistanceFrom(targetUnits[i]:getPos()) < tfUnit:getLineOfSight() then
				targUnit = targetUnits[i]
				break
			end
		end

		if targUnit then break end
	end

	if not taskForce.clearing and targUnit then
		self:deselectUnits()
		self:selectUnits(taskForce.units)
		self:issueOrder(OrderType.MOVE, Vector3:new(0, 0, 0), {Target:new(targUnit:toGameObject(), Vector3:new(0, 0, 0))}, false)

		self.taskForces[arguments.tfId].clearing = true
	elseif taskForce.clearing and not targUnit then
		self.taskForces[arguments.tfId].clearing = false
	end

	taskForce = self.taskForces[arguments.tfId]
	return taskForce.clearing and BTNodeResult.RUNNING or BTNodeResult.SUCCESS
end

function Player:attackSpawnPoint(arguments)
	taskForce = self.taskForces[arguments.tfId]

	if taskForce.attacking then return BTNodeResult.RUNNING end

	self:deselectUnits()
	self:selectUnits(taskForce.units)

	for i = 1, #taskForce.units do taskForce.units[i]:setState(0) end

	enemySpawnPoint = Map.getSingleton():getSpawnPoint(taskForce.spawnPointId)
	self:issueOrder(OrderType.MOVE, Vector3:new(0, 0, 0), {Target:new(nil, enemySpawnPoint)}, false)
	self.taskForces[arguments.tfId].attacking = true

	return BTNodeResult.RUNNING
end

function Player:generateTaskforceActions()
	numSpawnPoints = Map.getSingleton():getNumSpawnPoints()
	actionsTable = {}

	taskForceId = 1
	for i = 1, numSpawnPoints do
		if i - 1 == self:getSpawnPointId() then goto continue end

		actionsTable[#actionsTable + 1] = {
			type = BTNodeType.SEQUENCE,
			children = {
				{type = BTNodeType.FUNCTION, func = 'formTaskForce', args = {tfId = taskForceId, spId = i - 1}},
				{type = BTNodeType.FUNCTION, func = 'clearNearEnemies', args = {tfId = taskForceId}},
				{type = BTNodeType.FUNCTION, func = 'attackSpawnPoint', args = {tfId = taskForceId}},
			}
		}

		taskForceId = taskForceId + 1

		::continue::
	end

	return actionsTable
end
