res = graphics.resolution
sz = 210

resTextScale = {x = .5, y = .5}
buttonInitPos = {x = sz, y = res.y - .65 * sz, z = 0}
buttonSize = {x = 50, y = 50}
buttonSpace = {x = 10, y = 10}

function generateButton(numGui, buttonData)
	xOffset = math.floor(.5 * numGui) * (buttonSize.x + buttonSpace.x)
	yOffset = (numGui + 1) % 2 == 0 and (buttonSize.y + buttonSpace.y) or 0

	button = {
		pos = {x = buttonInitPos.x + xOffset, y = buttonInitPos.y + yOffset, z = .5},
		size = buttonSize,
		guiType = GuiType.BUTTON,
		name = buttonData.name,
		buttonType = buttonData.buttonType,
		trigger = (buttonData.trigger or -1),
		orderType = buttonData.orderType,
		factoryId = buttonData.factoryId,
		engineerId = buttonData.engineerId,
		guiScreen = buttonData.guiScreen,
		slotId = buttonData.slotId
	}

	return button
end

function generateGui(unitId)
	local gui = {}
	vehicleButtonData = {
		{buttonType = ButtonType.UNIT_STATE, name = 'Change unit state'},
		{buttonType = ButtonType.ORDER, orderType = OrderType.GARRISON, name = 'Garrison'},
		{buttonType = ButtonType.ORDER, orderType = OrderType.PATROL, name = 'Patrol'},
		{buttonType = ButtonType.ORDER, orderType = OrderType.HALT, name = 'Halt'}
	}
	structureButtonData = {
		{buttonType = ButtonType.UNIT_STATE, name = 'Change unit state'},
		{buttonType = ButtonType.ORDER, orderType = OrderType.HALT, name = 'Halt'}
	}

	mainUnit = units[unitId + 1]
	unitButtonData = mainUnit.isVehicle and vehicleButtonData or structureButtonData

	if mainUnit.garrisonCapacity then
		unitButtonData[#unitButtonData + 1] = {
			buttonType = ButtonType.ORDER, 
			orderType = OrderType.EJECT, 
			name = 'Eject'
		}
	end

	for i = 1, #unitButtonData do
		gui[#gui + 1] = generateButton(#gui, unitButtonData[i])
	end

	if mainUnit.abilityButtons then
		for i = 1, #mainUnit.abilityButtons do
			abilButton = mainUnit.abilityButtons[i]

			buttonData = {
				trigger = abilButton.trigger,
				name = abilButton.name,
				buttonType = abilButton.buttonType,
				guiScreen = abilButton.guiScreen,
				orderType = abilButton.orderType
			}
			gui[#gui + 1] = generateButton(#gui, buttonData)
		end
	end

	if mainUnit.buildableUnits then
		for i = 1, #mainUnit.buildableUnits do
			buildUnit = mainUnit.buildableUnits[i]

			if not buildUnit.buildable then goto continue end

			isFactory = (
				mainUnit.unitClass == UnitClass.LAND_FACTORY or
				mainUnit.unitClass == UnitClass.NAVAL_FACTORY or
				mainUnit.unitClass == UnitClass.FORT
			)

			buttonData = {
				trigger = buildUnit.trigger,
				name = units[buildUnit.id + 1].name,
				buttonType = (mainUnit.isVehicle and ButtonType.BUILD or ButtonType.TRAIN),
				factoryId = (isFactory and unitId or nil),
				engineerId = (mainUnit.unitClass == UnitClass.ENGINEER and unitId or nil),
				slotId = (ButtonType.BUILD and i - 1 or nil)
			}
			gui[#gui + 1] = generateButton(#gui, buttonData)

			::continue::
		end
	end

	return gui
end

gui = generateGui(_mainUnitId)
