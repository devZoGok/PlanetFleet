Player.numStartEngis = 3
Player.baseDir = nil 
Player.numDefWarMechs = 1
Player.behaviour = {}
Player.pointDefenseTrans = {}

Player.unitClassMappings = {
	{UnitId.ACS_MECH, UnitId.AINC_MECH, UnitId.ER_MECH}, --WAR_MECH = 0,
	{UnitId.ACS_TANK, UnitId.AINC_TANK, UnitId.ER_TANK}, --TANK = 1,
	{UnitId.ACS_ARTILLERY, UnitId.AINC_ARTILLERY, UnitId.ER_ARTILLERY}, --ARTILLERY = 2,
	{UnitId.ACS_ROBO_ENGINEER, UnitId.AINC_ROBO_ENGINEER, UnitId.ER_ROBO_ENGINEER}, --ENGINEER = 3,
	{UnitId.ACS_CYBORG_ENGINEER, UnitId.AINC_CYBORG_ENGINEER, UnitId.ER_CYBORG_ENGINEER}, --ENGINEER = 3,
	{UnitId.ACS_MECH, UnitId.AINC_MECH, UnitId.ER_MECH}, --TRANSPORT = 4,
	{UnitId.ACS_CARGO_SHIP, UnitId.AINC_CARGO_SHIP, UnitId.ER_CARGO_SHIP}, --RESOURCE_ROVER = 5,
	{UnitId.ACS_CRUISER, UnitId.AINC_CRUISER, UnitId.ER_CRUISER}, --CRUISER = 6,
	{UnitId.ACS_ANTI_SUB_CRUISER, UnitId.AINC_ANTI_SUB_CRUISER, UnitId.ER_ANTI_SUB_CRUISER}, --CRUISER = 6,
	{nil, nil, nil}, --CARRIER = 8,
	{UnitId.ACS_SUBMARINE, UnitId.AINC_SUBMARINE, UnitId.ER_SUBMARINE}, --SUBMARINE = 9,
	{UnitId.ACS_MISSILE_SUBMARINE, UnitId.AINC_MISSILE_SUBMARINE, UnitId.ER_MISSILE_SUBMARINE}, --SUBMARINE = 9,
	{UnitId.ACS_ICEBREAKER, UnitId.AINC_ICEBREAKER, UnitId.ER_ICEBREAKER}, --ICEBREAKER = 10,
	{nil, UnitId.FREEZER, nil}, --FREEZER = 11,
	{nil, UnitId.EMP_BOAT, nil}, --EMP_BOAT = 12,
	{UnitId.ACS_LAND_FACTORY, UnitId.AINC_LAND_FACTORY, UnitId.ER_LAND_FACTORY}, --LAND_FACTORY = 13,
	{UnitId.ACS_NAVAL_FACTORY, UnitId.AINC_NAVAL_FACTORY, UnitId.ER_NAVAL_FACTORY}, --NAVAL_FACTORY = 14,
	{UnitId.ACS_TRADE_CENTER, UnitId.AINC_TRADE_CENTER, UnitId.ER_TRADE_CENTER}, --TRADE_CENTER = 15,
	{UnitId.ACS_LAB, UnitId.AINC_LAB, UnitId.ER_LAB}, --LAB = 16,
	{UnitId.POINT_DEFENSE, UnitId.POINT_DEFENSE, UnitId.POINT_DEFENSE}, --POINT_DEFENSE = 17,
	{UnitId.EXTRACTOR, UnitId.EXTRACTOR, UnitId.EXTRACTOR}, --EXTRACTOR = 18,
	{UnitId.ACS_REFINERY, UnitId.AINC_REFINERY, UnitId.ER_REFINERY}, --REFINERY = 19,
	--{UnitId.ACS_MECH, UnitId.AINC_MECH, UnitId.ER_MECH}, --ICE_SHEET = 20,
}

Player.taskForceData = {
	{class = UnitClass.MECH, numUnits = 2},
	{class = UnitClass.TANK, numUnits = 1},
	{class = UnitClass.ARTILLERY, numUnits = 1}
}

Player.taskForces = {}

function Player:initBaseDir()
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
end

function Player:initPdSpots()
	numPd = 6
	radius = 40
	map = Map.getSingleton()
	mapSize = map:getMapSize()

	for i = 1, numPd do
		angle = (i - 1) * 2 * math.pi / numPd
		rotDir = Quaternion:new(angle, Vector3:new(0, 1, 0)):multVec(self.baseDir)
		pdPos = map:getSpawnPoint(self:getSpawnPointId()):add(rotDir:norm():mult(radius))

		if math.abs(pdPos.x) < .5 * mapSize.x and math.abs(pdPos.z) < .5 * mapSize.z then
			self.pointDefenseTrans[#self.pointDefenseTrans + 1] = {pos = pdPos, angle = angle}
		end
	end

	print('num pd pos: ' .. #self.pointDefenseTrans)
end

function Player:init()
	self:initBaseDir()
	self:initPdSpots()
end

function Player:buildStructure(engineer, buildingId, buildPos, buildAngle)
	if not engineer then return BTNodeResult.FAILURE end

	self:deselectUnits()
	self:selectUnits({engineer})
	
	building = GameObjectFactory.createUnit(self, buildingId, buildPos, Quaternion:new(buildAngle, Vector3:new(0, 1, 0)), 0)
	self:issueOrder(OrderType.BUILD, Vector3:new(0, 0, 0), {Target:new(building:toGameObject(), Vector3:new(0, 0, 0))}, false)
end

function Player:findSuitableSpot()
	return Vector3:new(0, 0, 0)
end

function Player:findIdleEngineer()
	engineers = self:getUnitsByClass(UnitClass.CYBORG_ENGINEER, -1)

	for i = 1, #engineers do
		if engineers[i]:getNumOrders() == 0 then return engineers[i] end
	end

	return nil
end

function Player:getBuildableUnitSlotId(buildingUnit, buildableUnitId)
	for i = 1, #buildingUnit:getBuildableUnits() do
		bu = buildingUnit:getBuildableUnit(i - 1)

		if bu.buildable and bu.id == buildableUnitId then
			print('returning ' .. i - 1)
			return i - 1
		end
	end

	print('returning nil')
	return nil
end

--TODO simplify building construction
function Player:buildLandFactory(arguments)
	factory = self:getUnitsByClass(UnitClass.LAND_FACTORY, 1)[1]

	if factory then
		return factory:toStructure():isComplete() and BTNodeResult.SUCCESS or BTNodeResult.RUNNING
	end

	engis = self:getUnitsByClass(UnitClass.CYBORG_ENGINEER, 1)
	if #engis == 0 then return BTNodeResult.FAILURE end

	self:deselectUnits()
	self:selectUnits({engis[1]})

	right = (self.baseDir:getAngleBetween(Vector3:new(1, 0, 0)) > .5 * math.pi)
	angle = self.baseDir:getAngleBetween(Vector3:new(0, 0, 1)) * (right and -1 or 1)
	rot = Quaternion:new(angle, Vector3:new(0, 1, 0))

	factId = self.unitClassMappings[UnitClass.LAND_FACTORY + 1][self:getFaction() + 1]
	sp = Map.getSingleton():getSpawnPoint(self:getSpawnPointId())
	self:buildStructure(self:findIdleEngineer(), factId, sp, angle)

	return BTNodeResult.RUNNING
end

function Player:trainEngineers(arguments)
	factory = self:getUnitsByClass(UnitClass.LAND_FACTORY, 1)[1]:toFactory()
	engineers = self:getUnitsByClass(UnitClass.CYBORG_ENGINEER, -1)
	engineerDiffNum = self.numStartEngis - #engineers
	queueDiff = engineerDiffNum - #factory:getQueue()

	engiBuildId = self.unitClassMappings[UnitClass.CYBORG_ENGINEER + 1][self:getFaction() + 1]

	for i = 1, queueDiff do
		factory:appendToQueue(self:getBuildableUnitSlotId(factory, engiBuildId))
	end

	engisStartNumBuilt = (#self:getUnitsByClass(UnitClass.CYBORG_ENGINEER, -1) == self.numStartEngis)
	return (engisStartNumBuilt and BTNodeResult.SUCCESS or BTNodeResult.RUNNING)
end

function Player:buildRefinery(arguments)
	refinery = self:getUnitsByClass(UnitClass.REFINERY, 1)[1]

	if refinery then
		print('cpu player: ')
		print(self)
		print('ref1: ')
		print(refinery)
		print('ref2: ')
		print(refinery:toStructure())
		return (refinery:toStructure():isComplete() and BTNodeResult.SUCCESS or BTNodeResult.RUNNING)
	end

	buildPos = Map.getSingleton():getSpawnPoint(self:getSpawnPointId()):add(self.baseDir:mult(20))
	factId = self.unitClassMappings[UnitClass.REFINERY + 1][self:getFaction() + 1]
	self:buildStructure(self:findIdleEngineer(), factId, buildPos, -.0)

	return BTNodeResult.RUNNING
end

-- TODO optimize deposit position check
function Player:buildExtractor(arguments)
	extractor = self:getUnitsByClass(UnitClass.EXTRACTOR, -1)[1]

	if extractor then
		return (extractor:toStructure():isComplete() and BTNodeResult.SUCCESS or BTNodeResult.RUNNING)
	end

	visibleDeposits = {}
	players = Game.getSingleton():getPlayers(true)

	for i = 1, #players do
		deposits = players[i]:getResourceDeposits()

		for j = 1, #deposits do
			for k = 1, #self:getUnits() do
				unit = self:getUnit(k - 1)

				if unit:getPos():getDistanceFrom(deposits[j]:getPos()) <= unit:getLineOfSight() and not deposits[j]:getExtractor() then
					visibleDeposits[#visibleDeposits + 1] = deposits[i]
					goto continue
				end
			end

			::continue::
		end
	end

	if #visibleDeposits == 0 then return BTNodeResult.FAILURE end

	depPos = visibleDeposits[1]:getPos()
	spawnPoint = Map.getSingleton():getSpawnPoint(self:getSpawnPointId())

	for i = 1, #visibleDeposits do
		if visibleDeposits[i]:getPos():getDistanceFrom(spawnPoint) < depPos:getDistanceFrom(spawnPoint) then
			depPos = deposits[i]:getPos()
		end
	end

	self:buildStructure(self:findIdleEngineer(), UnitId.EXTRACTOR, depPos, 0)

	return BTNodeResult.RUNNING
end

function Player:buildHarvester(arguments)
	if #self:getUnitsByClass(UnitClass.RESOURCE_ROVER, 1) > 0 then
		return BTNodeResult.SUCCESS
	end

	factory = self:getUnitsByClass(UnitClass.LAND_FACTORY, 1)[1]
	if not factory then return BTNodeResult.FAILURE end

	factory = factory:toFactory()
	roverSlotId = self.unitClassMappings[UnitClass.RESOURCE_ROVER + 1][self:getFaction() + 1]

	if #factory:getQueue() == 0 then
		factory:appendToQueue(self:getBuildableUnitSlotId(factory, roverSlotId))
	end

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

function Player:buildLandDefForce(arguments)
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

		print('about to...')
		if enoughUnits then
			print('here it comes...')
			return BTNodeResult.SUCCESS
		end
	else
		return BTNodeResult.RUNNING
	end

	for i = 1, numAttackableSpawnPoints do
		for j = 1, #self.taskForceData do
			for k = 1, self.taskForceData[j].numUnits do
				print('queueing...')
				unitId = self.unitClassMappings[self.taskForceData[j].class + 1][self:getFaction() + 1]
				slotId = self:getBuildableUnitSlotId(landFactory, unitId)
				landFactory:appendToQueue(slotId)
			end
		end
	end

	return BTNodeResult.RUNNING
end

function Player:buildPointDefenseRing(arguments)
	pointDefs = self:getUnitsByClass(UnitClass.POINT_DEFENSE, -1)
	if #pointDefs == #self.pointDefenseTrans then return BTNodeResult.SUCCESS end

	pdId = self.unitClassMappings[UnitClass.POINT_DEFENSE + 1][self:getFaction() + 1]
	pdTrans = self.pointDefenseTrans[#pointDefs + 1]
	self:buildStructure(self:findIdleEngineer(), pdId, pdTrans.pos, pdTrans.angle)

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

--[[
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
]]--
