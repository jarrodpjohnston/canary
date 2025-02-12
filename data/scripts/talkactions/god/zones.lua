local zones = TalkAction("/zones")

local function randomZonePosition(zone)
	local positions = zone:getPositions()
	local destination = positions[math.random(1, #positions)]
	local tile = destination:getTile()
	while not tile or not tile:isWalkable(false, false, false, false, true) do
		destination = positions[math.random(1, #positions)]
		tile = destination:getTile()
	end
	return destination
end

function zones.onSay(player, words, param)
	local params = string.split(param, ",")
	local cmd = params[1]
	if not cmd then
		player:sendTextMessage(MESSAGE_STATUS_CONSOLE_BLUE, "Command not found.")
		return false
	end

	if cmd == "list" then
		local list = {}
		for _, zone in ipairs(Zone.getAll()) do
			table.insert(list, zone:getName())
		end
		player:sendTextMessage(MESSAGE_HEALED, "Zones: " .. table.concat(list, ", "))
		return true
	end

	local function zoneFromParam()
		local zoneName = params[2]:trim()
		if not zoneName then
			player:sendTextMessage(MESSAGE_HEALED, "Zone not found.")
			return false
		end
		local zone = Zone.getByName(zoneName)
		if not zone then
			player:sendTextMessage(MESSAGE_HEALED, "Zone not found.")
			return false
		end
		return zone
	end

	local commands = {
		["goto"] = function(zone)
			local pos = randomZonePosition(zone)
			if not pos then
				player:sendTextMessage(MESSAGE_HEALED, "No position found.")
				return false
			end
			player:teleportTo(pos)
			player:sendTextMessage(MESSAGE_HEALED, "You have been teleported to " .. zone:getName() .. ".")
		end,
		removeMonsters = function(zone)
			zone:removeMonsters()
			player:sendTextMessage(MESSAGE_HEALED, "Monsters removed from " .. zone:getName() .. ".")
		end,
		countMonsters = function(zone)
			local monsters = zone:getMonsters()
			player:sendTextMessage(MESSAGE_HEALED, "Zone " .. zone:getName() .. " monsters: " .. #monsters .. ".")
		end,
		removeNpcs = function(zone)
			zone:removeNpcs()
			player:sendTextMessage(MESSAGE_HEALED, "NPCs removed from " .. zone:getName() .. ".")
		end,
		countNpcs = function(zone)
			local npcs = zone:getNpcs()
			player:sendTextMessage(MESSAGE_HEALED, "Zone " .. zone:getName() .. " NPCs: " .. #npcs .. ".")
		end,
		kickPlayers = function(zone)
			local players = zone:getPlayers()
			for _, player in ipairs(players) do
				player:teleportTo(player:getTown():getTemplePosition())
				player:sendTextMessage(MESSAGE_HEALED, "You have been kicked from " .. zone:getName() .. ".")
			end
			player:sendTextMessage(MESSAGE_HEALED, "Players kicked from " .. zone:getName() .. ".")
		end,
		listPlayers = function(zone)
			local players = zone:getPlayers()
			local list = {}
			for _, player in ipairs(players) do
				table.insert(list, player:getName())
			end
			player:sendTextMessage(MESSAGE_HEALED, "Zone " .. zone:getName() .. " players: " .. table.concat(list, ", ") .. ".")
		end,
		countPlayers = function(zone)
			local players = zone:getPlayers()
			player:sendTextMessage(MESSAGE_HEALED, "Zone " .. zone:getName() .. " players: " .. #players .. ".")
		end,
		size = function(zone)
			local positions = zone:getPositions()
			player:sendTextMessage(MESSAGE_HEALED, "Zone " .. zone:getName() .. " size: " .. #positions .. ".")
		end,
	}

	local command = commands[cmd]
	if not command then
		player:sendTextMessage(MESSAGE_HEALED, "Command not found.")
		return false
	end
	local zone = zoneFromParam()
	if not zone then return false end
	return command(zone)
end

zones:separator(" ")
zones:groupType("god")
zones:register()
