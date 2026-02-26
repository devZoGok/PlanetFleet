Player.numStartEngis = 3
Player.baseQuat = nil 
Player.baseRadius = 180
Player.numDefWarMechs = 1
Player.behaviour = {}
Player.pointDefenseTrans = {}
Player.canReachSpawnPoint = {}
Player.navalFactoryCellId = nil

Player.unitClassMappings = {
	{UnitId.ACS_MECH, UnitId.AINC_MECH, UnitId.ER_MECH},
	{UnitId.ACS_TANK, UnitId.AINC_TANK, UnitId.ER_TANK},
	{UnitId.ACS_ARTILLERY, UnitId.AINC_ARTILLERY, UnitId.ER_ARTILLERY},
	{UnitId.ACS_ROBO_ENGINEER, UnitId.AINC_ROBO_ENGINEER, UnitId.ER_ROBO_ENGINEER},
	{UnitId.ACS_CYBORG_ENGINEER, UnitId.AINC_CYBORG_ENGINEER, UnitId.ER_CYBORG_ENGINEER},
	{UnitId.ACS_TRANSPORT, UnitId.AINC_TRANSPORT, UnitId.ER_TRANSPORT},
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

Player.landTaskForceData = {
	{class = UnitClass.MECH, numUnits = 2},
	{class = UnitClass.TANK, numUnits = 1},
	{class = UnitClass.ARTILLERY, numUnits = 1}
}

Player.navalTaskForceData = {
	{class = UnitClass.TRANSPORT, numUnits = 1},
	--{class = UnitClass.CRUISER, numUnits = 1},
	--{class = UnitClass.ANTI_SUB_CRUISER, numUnits = 1}
}

Player.taskForces = {}

function Player:initBaseQuat()
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

	dir = map:getSpawnPoint(minDistId):subtr(sp):norm()
	left = (Vector3:new(1, 0, 0):getAngleBetween(dir) < .5 * math.pi)
	angle = dir:getAngleBetween(Vector3:new(0, 0, 1)) * (left and 1 or -1)

	self.baseQuat = Quaternion:new(angle, Vector3:new(0, 1, 0))
end

function Player:initPdSpots()
	pdId = self.unitClassMappings[UnitClass.POINT_DEFENSE + 1][self:getFaction() + 1]
	pdFrame = GameObjectFrame:new(pdId, 0, self, nil, Vector3:new(0, 0, 0), Quaternion:new(1, 0, 0, 0))
	baseDir = self.baseQuat:multVec(Vector3:new(0, 0, 1))
	frameCtrl = GameObjectFrameController.getSingleton()

	numPd = 12

	for i = 1, numPd do
		angle = (i - 1) * 2 * math.pi / numPd
		rotDir = Quaternion:new(angle, Vector3:new(0, 1, 0)):multVec(baseDir)
		pdPos = Map.getSingleton():getSpawnPoint(self:getSpawnPointId()):add(rotDir:norm():mult(self.baseRadius))
		pdFrame:placeAt(pdPos)

		frameCtrl:checkPlacement(pdFrame)

		if pdFrame.status == 0 then
			self.pointDefenseTrans[#self.pointDefenseTrans + 1] = {pos = pdPos, angle = angle}
		end
	end

	pdFrame:destroy()
end

--TODO replace this method with more robust checks to determine the possibility of building on a given spot
function Player:updatePdSpots()
	pdId = self.unitClassMappings[UnitClass.POINT_DEFENSE + 1][self:getFaction() + 1]
	pdFrame = GameObjectFrame:new(pdId, 0, self, nil, Vector3:new(0, 0, 0), Quaternion:new(1, 0, 0, 0))

	gofCtrl = GameObjectFrameController.getSingleton()
	delPdTrans = {}

	for i = 1, #self.pointDefenseTrans do
		pdFrame:placeAt(self.pointDefenseTrans[i].pos)
		gofCtrl:checkPlacement(pdFrame)

		if pdFrame.status == 1 then
			table.insert(delPdTrans, i)
		end
	end

	for i = #delPdTrans, 1, -1 do
		table.remove(self.pointDefenseTrans, i)
	end

	pdFrame:destroy()
end

--TODO remove heuristics
function Player:landRouteToSpawnpoint_(mapPoint)
	map = Map.getSingleton()
	dest = map:getCellId(mapPoint, false)
	source = map:getCellId(map:getSpawnPoint(self:getSpawnPointId()), false)

	pf = Pathfinder.getSingleton()
	cells = map:getCells()
	heurs = pf:calcHeuristics(cells, dest)
	path = pf:findPath(cells, heurs, source, dest, nil)

	for i = 1, #path do
		if cells[path[i]].type == 1 then
			if not self.navalFactoryCellId then self.navalFactoryCellId = path[i] end
			return false
		end
	end

	return true
end

function Player:landRouteToSpawnpoint(arguments)
	spId = self.taskForces[arguments.taskForceId].spawnPointId + 1
	return self.canReachSpawnPoint[spId] and BTNodeType.SUCCESS or BTNodeType.FAILURE
end

function Player:init()
	self:initBaseQuat()
	self:initPdSpots()

	for i = 1, Map.getSingleton():getNumSpawnPoints() do
		if i - 1 == self:getSpawnPointId() then self.canReachSpawnPoint[i] = true end

		spawnPoint = Map.getSingleton():getSpawnPoint(i - 1)
		self.canReachSpawnPoint[i] = self:landRouteToSpawnpoint_(spawnPoint)
	end
end

function Player:findSuitableSpot(buildingId, idealPos)
	structFrame = GameObjectFrame:new(buildingId, 0, self, nil, idealPos, self.baseQuat)
	numYPos = math.floor(2 * self.baseRadius / structFrame:getLength())
	numXPos = math.floor(2 * self.baseRadius / structFrame:getWidth())

	baseDir = self.baseQuat:multVec(Vector3:new(0, 0, 1))
	baseLeft = self.baseQuat:multVec(Vector3:new(1, 0, 0))

	frameCtrl = GameObjectFrameController.getSingleton()
	frameCtrl:checkPlacement(structFrame)

	if structFrame.status == 0 then
		structFrame:destroy()
		return idealPos
	end

	startPos = idealPos:add(baseDir:mult(self.baseRadius)):add(baseLeft:mult(self.baseRadius))

	for i = 1, numXPos do
		for j = 1, numYPos do
			point = startPos:add(baseLeft:neg():mult(i - 1)):add(baseDir:neg():mult(j - 1))
			structFrame:placeAt(point)
			frameCtrl:checkPlacement(structFrame)

			if structFrame.status == 0 then
				structFrame:destroy()
				return point
			end
		end
	end

	structFrame:destroy()
	return nil
end

function Player:buildStructure(engineer, buildingId, buildPos, buildAngle)
	if not engineer then return BTNodeResult.FAILURE end
	print(buildAngle)

	self:deselectUnits()
	self:selectUnits({engineer})
	
	building = GameObjectFactory.createUnit(self, buildingId, buildPos, Quaternion:new(-buildAngle, Vector3:new(0, 1, 0)), 0)
	self:issueOrder(OrderType.BUILD, Vector3:new(0, 0, 0), {Target:new(building:toGameObject(), Vector3:new(0, 0, 0))}, false)
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

function Player:buildLandFactory(arguments)
	factory = self:getUnitsByClass(UnitClass.LAND_FACTORY, 1)[1]

	if factory then
		return factory:toStructure():isComplete() and BTNodeResult.SUCCESS or BTNodeResult.RUNNING
	end

	factId = self.unitClassMappings[UnitClass.LAND_FACTORY + 1][self:getFaction() + 1]
	rd = self.baseQuat:multVec(Vector3:new(0, 0, 1)):mult(self.baseRadius)
	sp = Map.getSingleton():getSpawnPoint(self:getSpawnPointId())
	pos = self:findSuitableSpot(factId, sp)

	if pos then
		self:buildStructure(self:findIdleEngineer(), factId, pos, self.baseQuat:getAngle())
		return BTNodeResult.RUNNING
	else return BTNodeResult.FAILURE end
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
	refinery = self:getUnitsByClass(UnitClass.REFINERY, 1)[1]
	if refinery then
		return (refinery:toStructure():isComplete() and BTNodeResult.SUCCESS or BTNodeResult.RUNNING)
	end

	refId = self.unitClassMappings[UnitClass.REFINERY + 1][self:getFaction() + 1]
	factId = self.unitClassMappings[UnitClass.LAND_FACTORY + 1][self:getFaction() + 1]

	rd = self.baseQuat:multVec(Vector3:new(0, 0, 1)):mult(-.5 * (units[factId + 1].size.z + units[refId + 1].size.z))
	sp = Map.getSingleton():getSpawnPoint(self:getSpawnPointId())
	buildPos = sp:add(rd)

	pos = self:findSuitableSpot(refId, buildPos)

	if pos then
		buildAngle = (self.baseQuat:getAngle() + math.pi)
		self:buildStructure(self:findIdleEngineer(), refId, buildPos, buildAngle)
		return BTNodeResult.RUNNING
	else return BTNodeResult.FAILURE end
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

function Player:canReachAllSpawnPoints(arguments)
	return self.navalFactoryCellId and BTNodeResult.FAILURE or BTNodeResult.SUCCESS
end

function Player:buildNavalFactory(arguments)
	navalFactory = self:getUnitsByClass(UnitClass.NAVAL_FACTORY, 1)[1]

	if navalFactory then
		return (navalFactory:toStructure():isComplete() and BTNodeResult.SUCCESS or BTNodeResult.RUNNING)
	end

	navalFactId = self.unitClassMappings[UnitClass.NAVAL_FACTORY + 1][self:getFaction() + 1]
	pos = self:findSuitableSpot(navalFactId, Map.getSingleton():getCell(self.navalFactoryCellId).pos)

	if pos then
		self:buildStructure(self:findIdleEngineer(), navalFactId, pos, self.baseQuat:getAngle())
		return BTNodeResult.RUNNING
	else return BTNodeResult.FAILURE end
end

function Player:buildNavalForce(arguments)
	transports = self:getUnitsByClass(UnitClass.TRANSPORT, -1)

	if #transports >= #self.taskForces then return BTNodeResult.SUCCESS end

	factory = self:getUnitsByClass(UnitClass.NAVAL_FACTORY, 1)[1]:toFactory()

	if #factory:getQueue() > 0 then return BTNodeResult.RUNNING end

	transportBuildId = self.unitClassMappings[UnitClass.TRANSPORT + 1][self:getFaction() + 1]
	numBuildTransports = #self.taskForces - #transports

	for i = 1, numBuildTransports do
		factory:appendToQueue(self:getBuildableUnitSlotId(factory, transportBuildId))
	end

	return BTNodeResult.RUNNING
end

function Player:buildTaskForces(arguments)
	factory = self:getUnitsByClass(arguments.factoryClass, 1)

	if #factory == 0 then return BTNodeResult.FAILURE end

	factory = factory[1]:toFactory()
	numTaskForces = arguments.numTaskForces or 1
	taskForceData = arguments.taskForceData

	if #factory:getQueue() == 0 then
		enoughUnits = true

		for i = 1, #taskForceData do
			tfUnits = self:getUnitsByClass(taskForceData[i].class, -1)

			if #tfUnits < numTaskForces * taskForceData[i].numUnits then
				enoughUnits = false
				break
			end
		end

		if enoughUnits then return BTNodeResult.SUCCESS end
	else return BTNodeResult.RUNNING end

	for i = 1, numTaskForces do
		for j = 1, #taskForceData do
			for k = 1, taskForceData[j].numUnits do
				unitId = self.unitClassMappings[taskForceData[j].class + 1][self:getFaction() + 1]
				slotId = self:getBuildableUnitSlotId(factory, unitId)
				factory:appendToQueue(slotId)
			end
		end
	end

	return BTNodeResult.RUNNING
end

function Player:buildPointDefenseRing(arguments)
	self:updatePdSpots()
	pointDefs = self:getUnitsByClass(UnitClass.POINT_DEFENSE, -1)

	if #pointDefs >= #self.pointDefenseTrans then return BTNodeResult.SUCCESS end

	pdId = self.unitClassMappings[UnitClass.POINT_DEFENSE + 1][self:getFaction() + 1]
	pdTrans = self.pointDefenseTrans[#pointDefs + 1]
	self:buildStructure(self:findIdleEngineer(), pdId, pdTrans.pos, pdTrans.angle)

	return BTNodeResult.RUNNING
end

function Player:formLandTaskForces(arguments)
	numSpawnPoints = Map.getSingleton():getNumSpawnPoints()

	if #self.taskForces == numSpawnPoints - 1 then return BTNodeResult.SUCCESS end

	unitGroups = {}

	for i = 1, #self.landTaskForceData do
		unitGroups[i] = self:getUnitsByClass(self.landTaskForceData[i].class, -1)
		enoughGroupUnits = (#unitGroups[i] >= (numSpawnPoints - 1) * self.landTaskForceData[i].numUnits)

		if not enoughGroupUnits then return BTNodeResult.FAILURE end
	end

	tfId = 1

	for i = 1, numSpawnPoints do
		if i - 1 == self:getSpawnPointId() then goto continue end

		self.taskForces[tfId] = {units = {}, moving = false, arrived = false, spawnPointId = i - 1}

		for j = 1, #self.landTaskForceData do
			subArrId = (tfId - 1) * self.landTaskForceData[j].numUnits + 1
			unitTbl = table.move(
				unitGroups[j], 
				subArrId, 
				subArrId + self.landTaskForceData[j].numUnits, 
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

function Player:boardTransports(arguments)
	return BTNodeResult.FAILURE
end

function Player:moveTransports(arguments)
	return BTNodeResult.FAILURE
end

function Player:unloadTransports(arguments)
	return BTNodeResult.FAILURE
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
						{type = BTNodeType.FUNCTION, func = self.landRouteToSpawnpoint, args = arguments},
						{
							type = BTNodeType.SEQUENCE, 
							children = {
								{type = BTNodeType.FUNCTION, func = self.boardTransports, args = arguments},
								{type = BTNodeType.FUNCTION, func = self.moveTransports, args = arguments},
								{type = BTNodeType.FUNCTION, func = self.unloadTransports, args = arguments},
							}
						}
					}
				},
				{type = BTNodeType.FUNCTION, func = self.occupySpawnPoint, args = arguments}
			}
		}

		tfId = tfId + 1
		::continue::
	end

	return children
end
