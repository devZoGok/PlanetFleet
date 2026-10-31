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
		pos = {x = buttonInitPos.x + xOffset, y = buttonInitPos.y + yOffset, z = buttonInitPos.z},
		size = buttonSize,
		guiType = GuiType.BUTTON,
		name = buttonData.name,
		imagePath = buttonData.imagePath,
		buttonType = buttonData.buttonType,
		trigger = (buttonData.trigger or -1),
		orderType = buttonData.orderType,
		factoryId = buttonData.factoryId,
		engineerId = buttonData.engineerId,
		guiScreen = buttonData.guiScreen,
		tooltip = buttonData.tooltip,
		amount = buttonData.amount,
		slotId = buttonData.slotId
	}

	return button
end

function generateGui(unitId)
	local gui = {}
	haltOrder = {
		buttonType = ButtonType.ORDER, 
		orderType = OrderType.HALT, 
		name = 'Halt',
		tooltip = {
			offset = {x = buttonSize.x, y = -buttonSize.y, z = .1},
			size = {x = 210, y = 20},
			lines = {{entry = {text = "Cancel all unit\'s orders"}}}
		}
	}
	vehicleButtonData = {
		{
			buttonType = ButtonType.UNIT_STATE, 
			imagePath = 'Icons/Buttons/chase.png',
			tooltip = {
				offset = {x = buttonSize.x, y = -buttonSize.y, z = .1},
				size = {x = 340, y = 120},
				lines = {
					{entry = {text = 'Toggle unit state'}},
					{entry = {text = '(C)hase: automatically attacks and'}},
					{entry = {text = 'pursues enemies'}},
					{entry = {text = '(S)tand ground: automatically fires on'}},
					{entry = {text = 'enemies but does not move'}},
					{entry = {text = '(H)old fire: does not react to enemies'}},
				}
			}
		},
		{
			buttonType = ButtonType.ORDER, 
			orderType = OrderType.GARRISON, 
			name = 'Garrison',
			tooltip = {
				offset = {x = buttonSize.x, y = -buttonSize.y, z = .1},
				size = {x = 300, y = 60},
				lines = {
					{entry = {text = 'Garrison'}},
					{entry = {text = 'Enter a garrisonable unit, such as a'}},
					{entry = {text = 'transport ship'}},
				}
			}
		},
		haltOrder
	}
	structureButtonData = {haltOrder}

	mainUnit = units[unitId + 1]
	unitButtonData = mainUnit.isVehicle and vehicleButtonData or structureButtonData

	if mainUnit.garrisonCapacity then
		unitButtonData[#unitButtonData + 1] = {
			buttonType = ButtonType.ORDER, 
			orderType = OrderType.EJECT, 
			name = 'Eject',
			tooltip = {
				offset = {x = buttonSize.x, y = -buttonSize.y, z = .1},
				size = {x = 220, y = 40},
				lines = {
					{entry = {text = 'Eject'}},
					{entry = {text = 'Eject all garrisoned units'}},
				}
			}
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
				orderType = abilButton.orderType,
				amount = abilButton.amount,
				tooltip = {
					offset = {x = buttonSize.x, y = -buttonSize.y, z = .1},
					size = abilButton.tooltip.size,
					lines = abilButton.tooltip.lines
				}
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

			isEng = (
				mainUnit.unitClass == UnitClass.ENGINEER or 
				mainUnit.unitClass == UnitClass.FREEZER
			)
			costStr = '' .. units[buildUnit.id + 1].cost
			buildTimeStr = '' .. units[buildUnit.id + 1].buildTime

			buttonData = {
				trigger = buildUnit.trigger,
				name = units[buildUnit.id + 1].name,
				buttonType = (mainUnit.isVehicle and ButtonType.BUILD or ButtonType.TRAIN),
				factoryId = (isFactory and unitId or nil),
				engineerId = (isEng and unitId or nil),
				imagePath = buildUnit.iconPath,
				slotId = (ButtonType.BUILD and i - 1 or nil),
				tooltip = {
					offset = {x = buttonSize.x, y = -buttonSize.y, z = .1},
					size = {x = 220, y = 60},
					lines = {
						{entry = {text = units[buildUnit.id + 1].name}},
						{
							entry = {text = 'Cost: ' .. costStr .. ' ' .. buildTimeStr}, 
							icons = {
								{charId = 0, path = 'Resources/refineds.png'},
								{charId = 9 + #costStr, path = 'Resources/time.png'}
							}
						},
					}
				}
			}
			gui[#gui + 1] = generateButton(#gui, buttonData)

			::continue::
		end
	end

	return gui
end

gui = generateGui(_mainUnitId)
