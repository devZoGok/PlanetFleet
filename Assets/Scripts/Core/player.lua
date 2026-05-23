Player.baseQuat = nil 
Player.baseRadius = 180
Player.numDefWarMechs = 1
Player.behaviour = {}
Player.pointDefenseTrans = {}
Player.spawnPointData = {}
Player.navalFactoryCellId = nil
Player.navalRallyPoint = nil
Player.suitableBuildingPosIds = nil

Player.unitClassMappings = {
	{UnitId.ACS_MECH, UnitId.AINC_MECH, UnitId.ER_MECH},
	{UnitId.ACS_TANK, UnitId.AINC_TANK, UnitId.ER_TANK},
	{UnitId.ACS_ARTILLERY, UnitId.AINC_ARTILLERY, UnitId.ER_ARTILLERY},
	{UnitId.ACS_ROBO_ENGINEER, UnitId.AINC_ROBO_ENGINEER, UnitId.ER_ROBO_ENGINEER},
	{UnitId.ACS_CYBORG_ENGINEER, UnitId.AINC_CYBORG_ENGINEER, UnitId.ER_CYBORG_ENGINEER},
	{UnitId.ACS_TRANSPORT, UnitId.AINC_TRANSPORT, UnitId.ER_TRANSPORT},
	{UnitId.ACS_CARGO_SHIP, UnitId.AINC_CARGO_SHIP, UnitId.ER_CARGO_SHIP},
	{UnitId.ACS_ASSAULT_CRUISER, UnitId.AINC_ASSAULT_CRUISER, UnitId.ER_ASSAULT_CRUISER},
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
	{class = UnitClass.MECH, numUnits = 3},
	{class = UnitClass.TANK, numUnits = 2},
	{class = UnitClass.ARTILLERY, numUnits = 1}
}

Player.navalTaskForceData = {
	{class = UnitClass.TRANSPORT, numUnits = 2},
	--{class = UnitClass.CRUISER, numUnits = 2},
	{class = UnitClass.ANTI_SUB_CRUISER, numUnits = 1}
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

		if pdFrame.status == GameObjectFrameStatus.PLACEABLE then
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

		if pdFrame.status ~= GameObjectFrameStatus.PLACEABLE then
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
	forwClampedDest = pf:clampDestToSourceRegion(source, dest)

	if dest ~= forwClampedDest then
		cells = map:getCells()

		embarkCellId = nil
		navalRallyCellId = nil
		embarkCellId = forwClampedDest

		for i = 1, #cells[embarkCellId + 1].edges do
			destCellId = cells[embarkCellId + 1].edges[i].destCellId

			if cells[destCellId + 1].type == 1 then
				if not self.navalFactoryCellId then self.navalFactoryCellId = destCellId end

				navalRallyCellId = destCellId
				break
			end
		end

		disembarkCellId = nil
		navalSupportCellId = nil
		backwClampedDest = pf:clampDestToSourceRegion(dest, embarkCellId)
		disembarkCellId = backwClampedDest

		for i = 1, #cells[disembarkCellId + 1].edges do
			destCellId = cells[disembarkCellId + 1].edges[i].destCellId

			if cells[destCellId + 1].type == 1 then
				navalSupportCellId = destCellId
				break
			end
		end
		
		
		return {
			reachable = false, 
			embarkCellId = embarkCellId, 
			navalRallyCellId = navalRallyCellId, 
			navalSupportCellId = navalSupportCellId, 
			disembarkCellId = disembarkCellId
		}
	else
		return {reachable = true}
	end
end

function Player:landRouteToSpawnpoint(arguments)
	spId = self.taskForces[arguments.taskForceId].spawnPointId + 1
	return self.spawnPointData[spId].reachable == true and BTNodeResult.SUCCESS or BTNodeResult.FAILURE
end

function Player:init()
	self:initBaseQuat()
	self:initPdSpots()

	for i = 1, Map.getSingleton():getNumSpawnPoints() do
		taskForce = {units = {}, moving = false, attacking = false, arrived = false, landed = false, spawnPointId = i - 1}

		if i - 1 == self:getSpawnPointId() then
			taskForce.arrived = true
			taskForce.landed = true
			self.spawnPointData[i] = {reachable = true}
		end

		self.taskForces[i] = taskForce
		spawnPoint = Map.getSingleton():getSpawnPoint(i - 1)
		self.spawnPointData[i] = self:landRouteToSpawnpoint_(spawnPoint)

	end
end

function Player:findSuitableSpot(buildingId, idealPos, radius)
	structFrame = GameObjectFrame:new(buildingId, 0, self, nil, idealPos, self.baseQuat)
	diffY = 10
	diffX = 10
	numYPos = math.floor(2 * radius / diffY)
	numXPos = math.floor(2 * radius / diffX)

	baseDir = self.baseQuat:multVec(Vector3:new(0, 0, 1))
	baseLeft = self.baseQuat:multVec(Vector3:new(1, 0, 0))

	frameCtrl = GameObjectFrameController.getSingleton()
	startX = 1
	startY = 1

	if not self.suitableBuildingPosIds then
		frameCtrl:checkPlacement(structFrame)
		
		if structFrame.status == GameObjectFrameStatus.PLACEABLE then
			structFrame:destroy()
			return idealPos
		end
	else
		startX = self.suitableBuildingPosIds.x
		startY = self.suitableBuildingPosIds.y
	end

	startPos = idealPos:add(baseDir:mult(self.baseRadius)):add(baseLeft:mult(self.baseRadius))

	for i = startX, numXPos do
		for j = startY, numYPos do
			point = startPos:add(baseLeft:neg():mult((i - 1) * diffX)):add(baseDir:neg():mult((j - 1) * diffY))
			structFrame:placeAt(point)
			frameCtrl:checkPlacement(structFrame)

			if structFrame.status == GameObjectFrameStatus.PLACEABLE then
				self.suitableBuildingPosIds = nil
				structFrame:destroy()
				return point
			elseif structFrame.status == GameObjectFrameStatus.BLOCKED_BY_FOG_OF_WAR then
				engis = self:getUnitsByClass(UnitClass.ROBO_ENGINEER, -1)

				for i = 1, #engis do
					if engis[i]:getNumOrders() > 0 and engis[i]:getOrder(0).targets[1].pos:getDistanceFrom(point) < engis[i]:getLineOfSight() then
						structFrame:destroy()
						return nil
					end
				end

				idleEngi = self:findIdleEngineer()

				if not idleEngi then
					structFrame:destroy()
					return nil
				end

				self:deselectUnits()
				self:selectUnits({idleEngi})
				self:issueOrder(OrderType.MOVE, Vector3:new(0, 0, 0), {Target:new(nil, point)}, false)

				self.suitableBuildingPosIds = {x = i, y = j}
				structFrame:destroy()
				return nil
			end
		end
	end

	structFrame:destroy()
	return nil
end

function Player:buildStructure(engineer, buildingId, buildPos, buildAngle)
	if not engineer then return BTNodeResult.FAILURE end

	self:deselectUnits()
	self:selectUnits({engineer})
	
	building = GameObjectFactory.createUnit(self, buildingId, buildPos, Quaternion:new(-buildAngle, Vector3:new(0, 1, 0)), 0)
	self:issueOrder(OrderType.BUILD, Vector3:new(0, 0, 0), {Target:new(building:toGameObject(), Vector3:new(0, 0, 0))}, false)
end

function Player:findIdleEngineer()
	engineers = self:getUnitsByClass(UnitClass.ROBO_ENGINEER, -1)

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

function Player:updateTaskForces(arguments)
	numUnits = self:getNumUnits()
	
	for i = 1, #self.taskForces do
		for j = #self.taskForces[i].units, 1, -1 do
			for k = 1, numUnits do
				if self.taskForces[i].units[j] == self:getUnit(k - 1) then goto continue end
			end

			table.remove(self.taskForces[i].units, j)

			::continue::
		end

		if #self.taskForces[i].units == 0 then
			self.taskForces[i].arrived = false
			self.taskForces[i].moving = false
			self.taskForces[i].attacking = false
			self.taskForces[i].landed = false
		end
	end

	return BTNodeResult.SUCCESS
end

function Player:buildLandFactory(arguments)
	factory = self:getUnitsByClass(UnitClass.LAND_FACTORY, 1)[1]

	if factory then
		return factory:toStructure():isComplete() and BTNodeResult.SUCCESS or BTNodeResult.RUNNING
	end

	engi = self:findIdleEngineer()

	if not engi then return BTNodeResult.FAILURE end

	factId = self.unitClassMappings[UnitClass.LAND_FACTORY + 1][self:getFaction() + 1]
	rd = self.baseQuat:multVec(Vector3:new(0, 0, 1)):mult(self.baseRadius)
	sp = Map.getSingleton():getSpawnPoint(self:getSpawnPointId())
	pos = self:findSuitableSpot(factId, sp, self.baseRadius, engi)

	if pos then
		self:buildStructure(engi, factId, pos, self.baseQuat:getAngle())
		factory = self:getUnitsByClass(UnitClass.LAND_FACTORY, 1)[1]:toFactory()
		factory:setRallyPoint(factory:getPos():add(factory:getDirVec():mult(100)))
		return BTNodeResult.RUNNING
	else return BTNodeResult.FAILURE end
end

function Player:buildRefinery(arguments)
	refinery = self:getUnitsByClass(UnitClass.REFINERY, 1)[1]
	if refinery then
		return (refinery:toStructure():isComplete() and BTNodeResult.SUCCESS or BTNodeResult.RUNNING)
	end

	refId = self.unitClassMappings[UnitClass.REFINERY + 1][self:getFaction() + 1]
	factId = self.unitClassMappings[UnitClass.LAND_FACTORY + 1][self:getFaction() + 1]

	rd = self.baseQuat:multVec(Vector3:new(0, 0, 1)):mult(-.6 * (units[factId + 1].size.z + units[refId + 1].size.z))
	fp = self:getUnitsByClass(UnitClass.LAND_FACTORY, 1)[1]:getPos()
	buildPos = fp:add(rd)

	idleEngi = self:findIdleEngineer()
	pos = self:findSuitableSpot(refId, buildPos, self.baseRadius, idleEngi)

	if pos then
		buildAngle = (self.baseQuat:getAngle() + math.pi)
		self:buildStructure(idleEngi, refId, buildPos, buildAngle)
		return BTNodeResult.RUNNING
	else return BTNodeResult.FAILURE end
end

function Player:getVisibleDeposits(nearBase)
	visibleDeposits = {}
	players = Game.getSingleton():getPlayers(true)
	friendlyUnits = self:getFriendlyUnits(true)
	sp = Map.getSingleton():getSpawnPoint(self:getSpawnPointId())

	for i = 1, #players do
		deposits = players[i]:getResourceDeposits()

		for j = 1, #deposits do
			if self:isObjectVisible(deposits[j]:toGameObject(), friendlyUnits) then
				if not nearBase or (nearBase and deposits[j]:getPos():getDistanceFrom(sp) < self.baseRadius) then
					visibleDeposits[#visibleDeposits + 1] = deposits[j]
				end
			end
		end
	end

	return visibleDeposits
end

function Player:buildExtractors(arguments)
	extractors = self:getUnitsByClass(UnitClass.EXTRACTOR, -1)

	if arguments and arguments.numWaitExtractors and #extractors >= arguments.numWaitExtractors then
		for i = 1, arguments.numWaitExtractors do
			if not extractors[i]:toStructure():isComplete() then return BTNodeResult.RUNNING end
		end
	end

	visibleDeposits = self:getVisibleDeposits()
	depId = nil

	for i = 1, #visibleDeposits do
		if not visibleDeposits[i]:getExtractor() then
			depId = i
			break
		end
	end

	if not depId then return BTNodeResult.SUCCESS end

	self:buildStructure(self:findIdleEngineer(), UnitId.EXTRACTOR, visibleDeposits[depId]:getPos(), 0)
	return BTNodeResult.RUNNING
end

function Player:startHarvesting()
	harvesters = self:getUnitsByClass(UnitClass.RESOURCE_ROVER, -1)
	harvester = nil

	for i = 1, #harvesters do
		order = (harvesters[i]:getNumOrders() > 0 and harvesters[i]:getOrder(0) or nil)

		if i == #harvesters and order and order.type == OrderType.SUPPLY then
			return BTNodeResult.SUCCESS
		elseif not order or (order and order.type ~= OrderType.SUPPLY) then
			harvester = harvesters[i]
			break
		end
	end

	extractors = self:getUnitsByClass(UnitClass.EXTRACTOR, -1)
	if #extractors == 0 then return BTNodeResult.FAILURE end

	for i = #extractors, 1, -1 do
		if extractors[i]:toExtractor():getDeposit():getAmmount() == 0 then
			table.remove(extractors, i)
		end
	end

	minDistId = 1
	hPos = harvester:getPos()

	for i = 1, #extractors do
		currDist = extractors[i]:getPos():getDistanceFrom(hPos)
		minDist = extractors[minDistId]:getPos():getDistanceFrom(hPos)

		if currDist < minDist then minDistId = i end
	end

	self:deselectUnits()
	self:selectUnits({harvester})
	self:issueOrder(OrderType.SUPPLY, Vector3:new(0, 0, 0), {Target:new(extractors[minDistId]:toGameObject(), Vector3:new(0, 0, 0))}, false)

	return BTNodeResult.RUNNING
end

function Player:canReachAllSpawnPoints(arguments)
	return self.navalFactoryCellId and BTNodeResult.FAILURE or BTNodeResult.SUCCESS
end

function Player:buildNavalFactory(arguments)
	navalFactory = self:getUnitsByClass(UnitClass.NAVAL_FACTORY, 1)[1]

	if navalFactory then
		if not self.navalRallyPoint then self.navalRallyPoint = navalFactory:toFactory():getRallyPoint() end

		return (navalFactory:toStructure():isComplete() and BTNodeResult.SUCCESS or BTNodeResult.RUNNING)
	end

	navalFactId = self.unitClassMappings[UnitClass.NAVAL_FACTORY + 1][self:getFaction() + 1]
	navalCellPos = Map.getSingleton():getCell(self.navalFactoryCellId).pos
	idleEngi = self:findIdleEngineer()
	pos = self:findSuitableSpot(navalFactId, navalCellPos, self.baseRadius, idleEngi)

	if pos then
		self:buildStructure(idleEngi, navalFactId, pos, self.baseQuat:getAngle())
		return BTNodeResult.RUNNING
	else return BTNodeResult.FAILURE end
end

function Player:getNumEmptyTaskForces()
	numEmpty = 0

	for i = 1, #self.taskForces do
		if #self.taskForces[i].units == 0 then numEmpty = numEmpty + 1 end
	end

	return numEmpty
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

	for i = 1, #self.taskForces do
		if i == #self.taskForces and #self.taskForces[i].units > 0 then return BTNodeResult.SUCCESS
		else break end
	end

	unitGroups = {}

	for i = 1, #self.landTaskForceData do
		unitGroups[i] = self:getUnitsByClass(self.landTaskForceData[i].class, -1)
		enoughGroupUnits = (#unitGroups[i] >= #self.taskForces * self.landTaskForceData[i].numUnits)

		if not enoughGroupUnits then return BTNodeResult.FAILURE end
	end

	tfId = 1

	for i = 1, #self.taskForces do
		if #self.taskForces[i].units > 0 then goto continue end

		for j = 1, #self.landTaskForceData do
			subArrId = #unitGroups[j] - (tfId * self.landTaskForceData[j].numUnits) + 1
			unitTbl = table.move(
				unitGroups[j], 
				subArrId, 
				subArrId + self.landTaskForceData[j].numUnits - 1,
				#self.taskForces[i].units + 1, 
				self.taskForces[i].units
			)
			self.taskForces[i].units = unitTbl
		end

		tfId = tfId + 1
		::continue::
	end

	return BTNodeResult.SUCCESS
end

function Player:getTransportData(transports)
	allIdle = true
	allInWater = true
	allInLand = true

	for i = 1, #transports do
		numOrders = transports[i]:getNumOrders()
		currCell = map:getCell(map:getCellId(transports[i]:getPos(), false))

		if numOrders > 0 then allIdle = false end

		if currCell.type ~= 1 then allInWater = false
		elseif currCell.type ~= 0 then allInLand = false end
	end

	return {allIdle = allIdle, allInWater = allInWater, allInLand = allInLand}
end

function Player:boardTransports(arguments)
	taskForce = self.taskForces[arguments.taskForceId]

	if taskForce.landed then return BTNodeResult.SUCCESS end

	map = Map.getSingleton()
	transports = self:getUnitsByClass(UnitClass.TRANSPORT, -1)
	tpData = self:getTransportData(transports)

	if tpData.allIdle and tpData.allInWater then
		self:deselectUnits()
		self:selectUnits(self:getUnitsByClass(UnitClass.CRUISER, -1))
		self:selectUnits(self:getUnitsByClass(UnitClass.ANTI_SUB_CRUISER, -1))

		rallyCell = map:getCell(self.spawnPointData[taskForce.spawnPointId + 1].navalRallyCellId)
		self:issueOrder(OrderType.MOVE, Vector3:new(0, 0, 0), {Target:new(nil, rallyCell.pos)}, false)

		self:deselectUnits()
		self:selectUnits(transports)

		embarkCell = map:getCell(self.spawnPointData[taskForce.spawnPointId + 1].embarkCellId)
		self:issueOrder(OrderType.MOVE, Vector3:new(0, 0, 0), {Target:new(nil, embarkCell.pos)}, false)
	end

	if tpData.allInWater and not tpData.allIdle then return BTNodeResult.RUNNING end

	if not taskForce.moving and tpData.allInLand and tpData.allIdle then
		usaId = 1

		for i = 1, #transports do
			numSlots = transports[i]:getNumGarrisonSlots()
			un = table.move(taskForce.units, usaId, usaId + numSlots - 1, 1, {})
			usaId = usaId + numSlots

			self:deselectUnits()
			self:selectUnits(un)
			self:issueOrder(OrderType.GARRISON, Vector3:new(0, 0, 0), {Target:new(transports[i]:toGameObject(), Vector3:new(0, 0, 0))}, false)
		end

		taskForce.moving = true
	end

	for i = 1, #taskForce.units do
		garrisoned = taskForce.units[i]:toVehicle():getGarrisonable()

		if i == #taskForce.units and garrisoned then 
			taskForce.moving = false
			return BTNodeResult.SUCCESS
		elseif not garrisoned then return BTNodeResult.RUNNING end
	end
end

function Player:moveTransports(arguments)
	if self.taskForces[arguments.taskForceId].landed then return BTNodeResult.SUCCESS end

	transports = self:getUnitsByClass(UnitClass.TRANSPORT, -1)

	for i = 1, #transports do
		empty = transports[i]:isGarrisonEmpty()

		if i == #transports and empty then return BTNodeResult.SUCCESS
		elseif not empty then break end
	end

	tpData = self:getTransportData(transports)

	if tpData.allInLand and tpData.allIdle then
		spId = self.taskForces[arguments.taskForceId].spawnPointId
		targPos = Map.getSingleton():getCell(self.spawnPointData[spId + 1].disembarkCellId).pos

		for i = 1, #transports do
			nearDisembarkPoint = (transports[i]:getPos():getDistanceFrom(targPos) < 10)

			if i == #transports and nearDisembarkPoint then
				self.taskForces[arguments.taskForceId].landed = true

				self:deselectUnits()
				self:selectUnits(transports)
				self:issueOrder(OrderType.EJECT, Vector3:new(0, 0, 0), {}, false)
				self:selectUnits(self:getUnitsByClass(UnitClass.CRUISER, -1))
				self:selectUnits(self:getUnitsByClass(UnitClass.ANTI_SUB_CRUISER, -1))
				self:issueOrder(OrderType.MOVE, Vector3:new(0, 0, 0), {Target:new(nil, self.navalRallyPoint)}, false)
				return BTNodeResult.SUCCESS
			elseif not nearDisembarkPoint then break end
		end

		self:deselectUnits()
		self:selectUnits(transports)
		self:issueOrder(OrderType.MOVE, Vector3:new(0, 0, 0), {Target:new(nil, targPos)}, false)
		self:deselectUnits()
		self:selectUnits(self:getUnitsByClass(UnitClass.CRUISER, -1))
		self:selectUnits(self:getUnitsByClass(UnitClass.ANTI_SUB_CRUISER, -1))

		targPos = Map.getSingleton():getCell(self.spawnPointData[spId + 1].navalSupportCellId).pos
		self:issueOrder(OrderType.MOVE, Vector3:new(0, 0, 0), {Target:new(nil, targPos)}, false)
	end

	if not tpData.allIdle then return BTNodeResult.RUNNING end
end

function Player:getNearestHostileUnits(friendlyUnits, hostileUnits)
end

function Player:clearShoreDefenses(arguments)
	if self.taskForces[arguments.taskForceId].landed then return BTNodeResult.SUCCESS end

	self:deselectUnits()
	self:selectUnits(self:getUnitsByClass(UnitClass.CRUISER, -1))
	self:selectUnits(self:getUnitsByClass(UnitClass.ANTI_SUB_CRUISER, -1))
	selUnits = self:getSelectedUnits()
	maxLineOfSight = nil

	for i = 1, #selUnits do
		if not maxLineOfSight then maxLineOfSight = selUnits[i]:getLineOfSight() end 

		if selUnits[i]:getLineOfSight() > maxLineOfSight then
			maxLineOfSight = selUnits[i]:getLineOfSight()
		end
	end

	if not selUnits[1] then return BTNodeResult.FAILURE end

	hostileUnits = self:getHostileUnits()
	order = (selUnits[1]:getNumOrders() > 0 and selUnits[1]:getOrder(0) or nil)

	for i = 1, #hostileUnits do
		for j = 1, #selUnits do
			if hostileUnits[i]:getPos():getDistanceFrom(selUnits[j]:getPos()) < maxLineOfSight then

				if not order or (order and (order.type ~= OrderType.ATTACK or (order.type == OrderType.ATTACK and order.targets[1].unit ~= hostileUnits[i]))) then
					self:issueOrder(OrderType.ATTACK, Vector3:new(0, 0, 0), {Target:new(hostileUnits[i]:toGameObject(), hostileUnits[i]:getPos())}, false)
				end

				return BTNodeResult.RUNNING
			end
		end
	end

	spawnPointId = self.taskForces[arguments.taskForceId].spawnPointId + 1
	supportCellId = self.spawnPointData[spawnPointId].navalSupportCellId
	map = Map.getSingleton()

	if map:getCellId(selUnits[1]:getPos(), true) ~= supportCellId and not order or (order and order.type ~= OrderType.MOVE) then
		self:issueOrder(OrderType.MOVE, Vector3:new(0, 0, 0), {Target:new(nil, map:getCell(supportCellId).pos)}, false)
	end

	return BTNodeResult.SUCCESS
end

function Player:occupySpawnPoint(arguments)
	tfId = arguments.taskForceId

	if not self.spawnPointData[self.taskForces[tfId].spawnPointId + 1].reachable and not self.taskForces[tfId].landed then
		return BTNodeResult.FAILURE
	end

	if #self.taskForces[tfId].units > 0 and self.taskForces[tfId].arrived then
		return BTNodeResult.SUCCESS
	elseif #self.taskForces[tfId].units == 0 then
		return BTNodeResult.FAILURE
	end

	hostileUnits = self:getHostileUnits()
	spawnPoint = Map.getSingleton():getSpawnPoint(self.taskForces[tfId].spawnPointId)
	minDist = 10

	if #hostileUnits == 0 then
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

		self.taskForces[tfId].attacking = false
		self.taskForces[tfId].moving = true
	elseif #hostileUnits > 0 and self.taskForces[tfId].units[1]:getNumOrders() == 0 then
		self:deselectUnits()
		self:selectUnits(self.taskForces[tfId].units)
		self:issueOrder(OrderType.ATTACK, Vector3:new(0, 0, 0), {Target:new(hostileUnits[1]:toGameObject(), Vector3:new(0, 0, 0))}, false)
		self.taskForces[tfId].attacking = true
		self.taskForces[tfId].movign = false
	end

	return BTNodeResult.RUNNING
end

function Player:generateTaskForceActions()
	children = {
		{type = BTNodeType.SEQUENCE, children = {}}, 
		{type = BTNodeType.PARALLEL, children = {}}, 
	}
	numSpawnPoints = Map.getSingleton():getNumSpawnPoints()
	tfId = 1

	for i = 1, numSpawnPoints do
		arguments = {taskForceId = i}
		table.insert(
			children[1].children,
			{
				type = BTNodeType.SELECTOR, 
				children = {
					{type = BTNodeType.FUNCTION, func = self.landRouteToSpawnpoint, args = arguments},
					{
						type = BTNodeType.SEQUENCE, 
						children = {
							{type = BTNodeType.FUNCTION, func = self.boardTransports, args = arguments},
							{
								type = BTNodeType.PARALLEL, 
								children = {
									{type = BTNodeType.FUNCTION, func = self.moveTransports, args = arguments},
									{type = BTNodeType.FUNCTION, func = self.clearShoreDefenses, args = arguments}
								}
							},
						}
					}
				}
			}
		)
		table.insert(children[2].children, {type = BTNodeType.FUNCTION, func = self.occupySpawnPoint, args = arguments})

		tfId = tfId + 1
		::continue::
	end

	return children
end
