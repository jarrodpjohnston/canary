local startup = GlobalEvent("Startup")

function startup.onStartup()
	math.randomseed(systemTime())

	db.query("TRUNCATE TABLE `players_online`")
	db.asyncQuery("DELETE FROM `guild_wars` WHERE `status` = 0")
	db.asyncQuery("DELETE FROM `players` WHERE `deletion` != 0 AND `deletion` < " .. os.time())
	db.asyncQuery("DELETE FROM `ip_bans` WHERE `expires_at` != 0 AND `expires_at` <= " .. os.time())
	db.asyncQuery("DELETE FROM `market_history` WHERE `inserted` <= " .. (os.time() - configManager.getNumber(configKeys.MARKET_OFFER_DURATION)))

	-- reset familiars message storage
	db.query('DELETE FROM `player_storage` WHERE `key` = '..Global.Storage.FamiliarSummonEvent10)
	db.query('DELETE FROM `player_storage` WHERE `key` = '..Global.Storage.FamiliarSummonEvent60)

	-- Move expired bans to ban history
	local resultId = db.storeQuery("SELECT * FROM `account_bans` WHERE `expires_at` != 0 AND `expires_at` <= " .. os.time())
	if resultId ~= false then
		repeat
			local accountId = Result.getNumber(resultId, "account_id")
			db.asyncQuery("INSERT INTO `account_ban_history` (`account_id`, `reason`, `banned_at`, `expired_at`, `banned_by`) VALUES (" .. accountId .. ", " .. db.escapeString(Result.getString(resultId, "reason")) .. ", " .. Result.getNumber(resultId, "banned_at") .. ", " .. Result.getNumber(resultId, "expires_at") .. ", " .. Result.getNumber(resultId, "banned_by") .. ")")
			db.asyncQuery("DELETE FROM `account_bans` WHERE `account_id` = " .. accountId)
		until not Result.next(resultId)
		Result.free(resultId)
	end

	-- Check house auctions
	local resultId = db.storeQuery("SELECT `id`, `highest_bidder`, `last_bid`, (SELECT `balance` FROM `players` WHERE `players`.`id` = `highest_bidder`) AS `balance` FROM `houses` WHERE `owner` = 0 AND `bid_end` != 0 AND `bid_end` < " .. os.time())
	if resultId ~= false then
		repeat
			local house = House(Result.getNumber(resultId, "id"))
			if house then
				local highestBidder = Result.getNumber(resultId, "highest_bidder")
				local balance = Result.getNumber(resultId, "balance")
				local lastBid = Result.getNumber(resultId, "last_bid")
				if balance >= lastBid then
					db.query("UPDATE `players` SET `balance` = " .. (balance - lastBid) .. " WHERE `id` = " .. highestBidder)
					house:setOwnerGuid(highestBidder)
				end
				db.asyncQuery("UPDATE `houses` SET `last_bid` = 0, `bid_end` = 0, `highest_bidder` = 0, `bid` = 0 WHERE `id` = " .. house:getId())
			end
		until not Result.next(resultId)
		Result.free(resultId)
	end

	-- store towns in database
	db.query("TRUNCATE TABLE `towns`")
	for i, town in ipairs(Game.getTowns()) do
		local position = town:getTemplePosition()
		db.query("INSERT INTO `towns` (`id`, `name`, `posx`, `posy`, `posz`) VALUES (" .. town:getId() .. ", " .. db.escapeString(town:getName()) .. ", " .. position.x .. ", " .. position.y .. ", " .. position.z .. ")")
	end

	do -- Event Schedule rates
		local lootRate = EventsScheduler.getEventSLoot()
		if lootRate ~= 100 then
			SCHEDULE_LOOT_RATE = lootRate
		end

		local expRate = EventsScheduler.getEventSExp()
		if expRate ~= 100 then
			SCHEDULE_EXP_RATE = expRate
		end

		local skillRate = EventsScheduler.getEventSSkill()
		if skillRate ~= 100 then
			SCHEDULE_SKILL_RATE = skillRate
		end

		local spawnRate = EventsScheduler.getSpawnMonsterSchedule()
		if spawnRate ~= 100 then
			SCHEDULE_SPAWN_RATE = spawnRate
		end

		if expRate ~= 100 or lootRate ~= 100 or spawnRate ~= 100 or skillRate ~= 100 then
			logger.info("[Events] Exp: {}%, loot: {}%, Spawn: {}%, Skill: {}%", expRate, lootRate, spawnRate, skillRate)
		end
	end
end

startup:register()
