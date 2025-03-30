res = graphics.resolution
sz = 210

resTextScale = {x = .5, y = .5}
buttonInitPos = {x = sz, y = res.y - .75 * sz, z = 0}
buttonSize = {x = 20, y = 20}

function generateButton(numGui, buttonData)
	xOffset = math.floor(.5 * (numGui + 1)) * buttonSize.x
	yOffset = (numGui + 1) % 2 == 0 and 0 or buttonSize.y

	button = {
		pos = {x = buttonInitPos.x + xOffset, y = buttonInitPos.y + yOffset, z = .5},
		size = buttonSize,
		guiType = GuiType.BUTTON,
		buttonType = buttonData.buttonType,
		trigger = buttonData.trigger,
		factoryId = buttonData.factoryId
	}

	return button
end

function generateGui(unitId)
	gui = {}

	if units[unitId + 1].buildableUnits then
		for i = 1, #units[unitId + 1].buildableUnits do
			buildUnit = units[unitId + 1].buildableUnits[i]

			if not buildUnit.buildable then goto continue end

			isFactory = (
				buildUnit.unitClass == UnitClass.LAND_FACTORY or
				buildUnit.unitClass == UnitClass.NAVAL_FACTORY or
				buildUnit.unitClass == UnitClass.FORT
			)

			buttonData = {
				trigger = buildUnit.trigger,
				name = units[unitId + 1].name,
				factoryId = (isFactory and unitId or nil),
				buttonType = (buildUnit.isVehicle and ButtonType.TRAIN or ButtonType.BUILD)
			}
			gui[#gui + 1] = generateButton(#gui, buttonData)

			::continue::
		end
	end

	return gui
end

gui = generateGui(_mainUnitId)
