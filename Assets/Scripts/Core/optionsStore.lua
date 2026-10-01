-- Reads and writes the option tables of options.lua (mappings, graphics, audio, ...).
-- Options are addressed by dotted paths such as 'graphics.resolution'.

local OPTIONS_FILE = 'Scripts/Core/options.lua'
local DEFAULTS_FILE = 'Scripts/Core/defaultOptions.lua'

-- keys written in this order, all others alphabetically after them
local KEY_ORDER = {'mappings', 'bind', 'trigger', 'action', 'bindType', 'inOptions'}
local keyRanks = {}

for i = 1, #KEY_ORDER do
	keyRanks[KEY_ORDER[i]] = i
end

-- runs an options script without touching the live option tables
local function loadSandboxed(file)
	local env = {}
	assert(loadfile(PATH .. file, 't', env))()

	return env
end

local function deepCopy(value)
	if type(value) ~= 'table' then return value end

	local copy = {}

	for k, v in pairs(value) do
		copy[k] = deepCopy(v)
	end

	return copy
end

local function deepEquals(a, b)
	if type(a) ~= 'table' or type(b) ~= 'table' then return a == b end

	for k, v in pairs(a) do
		if not deepEquals(v, b[k]) then return false end
	end

	for k in pairs(b) do
		if a[k] == nil then return false end
	end

	return true
end

local function splitPath(path)
	local keys = {}

	for key in path:gmatch('[^.]+') do
		table.insert(keys, key)
	end

	return keys
end

local function lookup(root, path)
	local value = root

	for _, key in ipairs(splitPath(path)) do
		if type(value) ~= 'table' then return nil end

		value = value[key]
	end

	return value
end

local function sortedKeys(tbl)
	local keys = {}

	for k in pairs(tbl) do
		if type(k) == 'string' then table.insert(keys, k) end
	end

	table.sort(keys, function(a, b)
		local rankA, rankB = keyRanks[a], keyRanks[b]

		if rankA and rankB then return rankA < rankB end
		if rankA or rankB then return rankA ~= nil end

		return a < b
	end)

	return keys
end

-- nested tables of plain values are written on one line, e.g. a mapping or a resolution
local function serialize(value, indent)
	if type(value) == 'number' then return string.format('%.14g', value) end
	if type(value) == 'string' then return string.format('%q', value) end
	if type(value) ~= 'table' then return tostring(value) end

	local entries, multiline = {}, indent == ''

	for i = 1, #value do
		multiline = multiline or type(value[i]) == 'table'
		table.insert(entries, serialize(value[i], indent .. '\t'))
	end

	for _, key in ipairs(sortedKeys(value)) do
		multiline = multiline or type(value[key]) == 'table'
		table.insert(entries, key .. ' = ' .. serialize(value[key], indent .. '\t'))
	end

	if not multiline then return '{' .. table.concat(entries, ', ') .. '}' end

	return '{\n' .. indent .. '\t' .. table.concat(entries, ',\n' .. indent .. '\t') .. '\n' .. indent .. '}'
end

-- the live value, or the default one if options.lua doesn't have it
function getOption(path)
	local value = lookup(_G, path)

	if value == nil then value = lookup(loadSandboxed(DEFAULTS_FILE), path) end

	return value
end

function setOption(path, value)
	local keys = splitPath(path)
	local parent = _G

	for i = 1, #keys - 1 do
		if type(parent[keys[i]]) ~= 'table' then parent[keys[i]] = {} end

		parent = parent[keys[i]]
	end

	parent[keys[#keys]] = deepCopy(value)
end

-- 0-based position of the option's value among the given values, -1 if it's not one of them
function getOptionIndex(path, values)
	local current = getOption(path)

	for i = 1, #values do
		if deepEquals(values[i], current) then return i - 1 end
	end

	return -1
end

function restoreDefaultOption(path)
	setOption(path, lookup(loadSandboxed(DEFAULTS_FILE), path))
end

-- rewrites options.lua from the live option tables
function saveOptions()
	local defaults = loadSandboxed(DEFAULTS_FILE)
	local sections = {}

	for name in pairs(loadSandboxed(OPTIONS_FILE)) do sections[name] = true end
	for name in pairs(defaults) do sections[name] = true end

	local chunks = {}

	for _, name in ipairs(sortedKeys(sections)) do
		local value = _G[name]

		if value == nil then value = defaults[name] end

		table.insert(chunks, name .. ' = ' .. serialize(value, ''))
	end

	local source = table.concat(chunks, '\n') .. '\n'

	-- never replace options.lua with something the game can't load
	assert(load(source, 'options', 't', {}))()

	local file = assert(io.open(PATH .. OPTIONS_FILE, 'w'))
	file:write(source)
	file:close()
end
