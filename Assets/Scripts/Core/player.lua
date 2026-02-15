Player.numStartEngis = 3
Player.baseDir = nil 
Player.numDefWarMechs = 1
Player.behaviour = {}
Player.pointDefenseTrans = {}

Player.unitClassMappings = {
	{UnitId.ACS_MECH, UnitId.AINC_MECH, UnitId.ER_MECH},
	{UnitId.ACS_TANK, UnitId.AINC_TANK, UnitId.ER_TANK},
	{UnitId.ACS_ARTILLERY, UnitId.AINC_ARTILLERY, UnitId.ER_ARTILLERY},
	{UnitId.ACS_ROBO_ENGINEER, UnitId.AINC_ROBO_ENGINEER, UnitId.ER_ROBO_ENGINEER},
	{UnitId.ACS_CYBORG_ENGINEER, UnitId.AINC_CYBORG_ENGINEER, UnitId.ER_CYBORG_ENGINEER},
	{UnitId.ACS_MECH, UnitId.AINC_MECH, UnitId.ER_MECH},
	{UnitId.ACS_CARGO_SHIP, UnitId.AINC_CARGO_SHIP, UnitId.ER_CARGO_SHIP},
	{UnitId.ACS_CRUISER, UnitId.AINC_CRUISER, UnitId.ER_CRUISER},
	{UnitId.ACS_ANTI_SUB_CRUISER, UnitId.AINC_ANTI_SUB_CRUISER, UnitId.ER_ANTI_SUB_CRUISER},
	{nil, nil, nil},
	{UnitId.ACS_SUBMARINE, UnitId.AINC_SUBMARINE, UnitId.ER_SUBMARINE},
	{UnitId.ACS_MISSILE_SUBMARINE, UnitId.AINC_MISSILE_SUBMARINE, UnitId.ER_MISSILE_SUBMARINE},
	{UnitId.ACS_ICEBREAKER, UnitId.AINC_ICEBREAKER, UnitId.ER_ICEBREAKER},
	{nil, UnitId.FREEZER, nil},
	{nil, UnitId.EMP_BOAT, nil},
	{UnitId.ACS_LAND_FACTORY, UnitId.AINC_LAND_FACTORY, UnitId.ER_LAND_FACTORY},
	{UnitId.ACS_NAVAL_FACTORY, UnitId.AINC_NAVAL_FACTORY, UnitId.ER_NAVAL_FACTORY},
	{UnitId.ACS_TRADE_CENTER, UnitId.AINC_TRADE_CENTER, UnitId.ER_TRADE_CENTER},
	{UnitId.ACS_LAB, UnitId.AINC_LAB, UnitId.ER_LAB},
	{UnitId.POINT_DEFENSE, UnitId.POINT_DEFENSE, UnitId.POINT_DEFENSE},
	{UnitId.EXTRACTOR, UnitId.EXTRACTOR, UnitId.EXTRACTOR},
	{UnitId.ACS_REFINERY, UnitId.AINC_REFINERY, UnitId.ER_REFINERY},
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
	radius = 100
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
			return i - 1
		end
	end

	return nil
end

--TODO simplify building construction
function Player:buildLandFactory(arguments)
	factory = self:getUnitsByClass(UnitClass.LAND_FACTORY, 1)[1]

	if factory then
		return factory:toStructure():isComplete() and BTNodeResult.SUCCESS or BTNodeResult.RUNNING
	end

	factId = self.unitClassMappings[UnitClass.LAND_FACTORY + 1][self:getFaction() + 1]
	sp = Map.getSingleton():getSpawnPoint(self:getSpawnPointId())
	self:buildStructure(self:findIdleEngineer(), factId, sp, angle)

	return BTNodeResult.RUNNING
end

function Player:trainEngineers(arguments)
	engis = self:getUnitsByClass(UnitClass.CYBORG_ENGINEER, -1)

	if #engis >= self.numStartEngis then return BTNodeResult.SUCCESS end

	factory = self:getUnitsByClass(UnitClass.LAND_FACTORY, -1)[1]:toFactory()

	if #factory:getQueue() > 0 then return BTNodeResult.RUNNING end

	engiBuildId = self.unitClassMappings[UnitClass.CYBORG_ENGINEER + 1][self:getFaction() + 1]
	numBuildEngis = self.numStartEngis - #engis

	for i = 1, numBuildEngis do
		factory:appendToQueue(self:getBuildableUnitSlotId(factory, engiBuildId))
	end

	return BTNodeResult.RUNNING
end

function Player:buildRefinery(arguments)
	refinery = self:getUnitsByClass(UnitClass.REFINERY, 1)
	if #refinery > 0 then
		return (refinery[1]:toStructure():isComplete() and BTNodeResult.SUCCESS or BTNodeResult.RUNNING)
	end

	buildPos = Map.getSingleton():getSpawnPoint(self:getSpawnPointId()):add(self.baseDir:mult(20))
	factId = self.unitClassMappings[UnitClass.REFINERY + 1][self:getFaction() + 1]
	self:buildStructure(self:findIdleEngineer(), factId, buildPos, -.0)

	return BTNodeResult.RUNNING
end

-- TODO optimize deposit position check
function Player:buildExtractor(arguments)
	extractor = self:getUnitsByClass(UnitClass.EXTRACTOR, -1)

	if #extractor > 0 then
		return (extractor[1]:toStructure():isComplete() and BTNodeResult.SUCCESS or BTNodeResult.RUNNING)
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

function Player:buildLandForce(arguments)
	landFactory = self:getUnitsByClass(UnitClass.LAND_FACTORY, 1)

	if #landFactory == 0 then return BTNodeResult.FAILURE end

	landFactory = landFactory[1]:toFactory()
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
	else return BTNodeResult.RUNNING end

	for i = 1, numAttackableSpawnPoints do
		for j = 1, #self.taskForceData do
			for k = 1, self.taskForceData[j].numUnits do
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

function Player:formTaskForces(arguments)
	numSpawnPoints = Map.getSingleton():getNumSpawnPoints()

	if #self.taskForces == numSpawnPoints - 1 then return BTNodeResult.SUCCESS end

	unitGroups = {}

	for i = 1, #self.taskForceData do
		unitGroups[i] = self:getUnitsByClass(self.taskForceData[i].class, -1)
		enoughGroupUnits = (#unitGroups[i] >= (numSpawnPoints - 1) * self.taskForceData[i].numUnits)

		if not enoughGroupUnits then return BTNodeResult.FAILURE end
	end

	tfId = 1

	for i = 1, numSpawnPoints do
		if i - 1 == self:getSpawnPointId() then goto continue end

		self.taskForces[tfId] = {units = {}, moving = false, arrived = false, spawnPointId = i - 1}

		for j = 1, #self.taskForceData do
			subArrId = (tfId - 1) * self.taskForceData[j].numUnits + 1
			unitTbl = table.move(
				unitGroups[j], 
				subArrId, 
				subArrId + self.taskForceData[j].numUnits, 
				#self.taskForces[tfId].units + 1, 
				self.taskForces[tfId].units
			)
			self.taskForces[tfId].units = unitTbl
		end

		tfId = tfId + 1
		::continue::
	end

	return BTNodeResult.SUCCESS
end

function Player:isLandRouteToSpawnpoint(arguments)
	return BTNodeResult.SUCCESS
end

function Player:occupySpawnPoint(arguments)
	tfId = arguments.taskForceId

	if #self.taskForces[tfId].units > 0  then
		if self.taskForces[tfId].moving then
			return BTNodeResult.RUNNING
		elseif self.taskForces[tfId].arrived then
			return BTNodeResult.SUCCESS
		end
	elseif #self.taskForces[tfId].units == 0 then
		return BTNodeResult.FAILURE
	end

	spawnPoint = Map.getSingleton():getSpawnPoint(self.taskForces[tfId].spawnPointId)
	minDist = 10

	for i = 1, #self.taskForces[tfId].units do
		if self.taskForces[tfId].units[i]:getPos():getDistanceFrom(spawnPoint) < minDist then
			self.taskForces[tfId].moving = false
			self.taskForces[tfId].arrived = true
			return BTNodeResult.SUCCESS
		end
	end

	if not self.taskForces[tfId].moving then
		self.taskForces[tfId].moving = true
		self:deselectUnits()
		self:selectUnits(self.taskForces[tfId].units)
		self:issueOrder(OrderType.MOVE, Vector3:new(0, 0, 0), {Target:new(nil, spawnPoint)}, false)
	end

	return BTNodeResult.RUNNING
end

function Player:generateTaskForceActions()
	children = {}
	tfId = 1
	numSpawnPoints = Map.getSingleton():getNumSpawnPoints()

	for i = 1, numSpawnPoints do
		if i - 1 == self:getSpawnPointId() then goto continue end

		arguments = {taskForceId = tfId}
		children[#children + 1] = {
			type = BTNodeType.SEQUENCE,
			children = {
				{
					type = BTNodeType.SELECTOR, 
					children = {
						{type = BTNodeType.FUNCTION, func = 'isLandRouteToSpawnpoint', args = arguments},
						{
							type = BTNodeType.SEQUENCE, 
							children = {
								{type = BTNodeType.FUNCTION, func = 'boardTransports', args = arguments},
								{type = BTNodeType.FUNCTION, func = 'moveTransports', args = arguments},
								{type = BTNodeType.FUNCTION, func = 'unloadTransports', args = arguments},
							}
						}
					}
				},
				{type = BTNodeType.FUNCTION, func = 'occupySpawnPoint', args = arguments}
			}
		}

		tfId = tfId + 1
		::continue::
	end

	return children
end
