/**
 * Canary - A free and open-source MMORPG server emulator
 * Copyright (©) 2019-2022 OpenTibiaBR <opentibiabr@outlook.com>
 * Repository: https://github.com/opentibiabr/canary
 * License: https://github.com/opentibiabr/canary/blob/main/LICENSE
 * Contributors: https://github.com/opentibiabr/canary/graphs/contributors
 * Website: https://docs.opentibiabr.com/
 */

#pragma once

#include "declarations.hpp"

#include "config/configmanager.hpp"
#include "map/house/house.hpp"
#include "items/item.hpp"
#include "map/map.hpp"
#include "creatures/monsters/spawns/spawn_monster.hpp"
#include "creatures/npcs/spawns/spawn_npc.hpp"

class IOMapException : public std::exception {
public:
	IOMapException(const std::string &msg) :
		message(msg) { }

	const char* what() const noexcept override {
		return message.c_str();
	}

private:
	std::string message;
};

class IOMap {
public:
	static void loadMap(Map* map, const std::string &identifier, const Position &pos = Position(), bool unload = false);

	/**
	 * Load main map monsters
	 * \param map Is the map class
	 * \returns true if the monsters spawn map was loaded successfully
	 */
	static bool loadMonsters(Map* map) {
		if (map->monsterfile.empty()) {
			// OTBM file doesn't tell us about the monsterfile,
			// Lets guess it is mapname-monster.xml.
			map->monsterfile = g_configManager().getString(MAP_NAME);
			map->monsterfile += "-monster.xml";
		}

		return map->spawnsMonster.loadFromXML(map->monsterfile);
	}

	/**
	 * Load main map npcs
	 * \param map Is the map class
	 * \returns true if the npcs spawn map was loaded successfully
	 */
	static bool loadNpcs(Map* map) {
		if (map->npcfile.empty()) {
			// OTBM file doesn't tell us about the npcfile,
			// Lets guess it is mapname-npc.xml.
			map->npcfile = g_configManager().getString(MAP_NAME);
			map->npcfile += "-npc.xml";
		}

		return map->spawnsNpc.loadFromXml(map->npcfile);
	}

	/**
	 * Load main map houses
	 * \param map Is the map class
	 * \returns true if the main map houses was loaded successfully
	 */
	static bool loadHouses(Map* map) {
		if (map->housefile.empty()) {
			// OTBM file doesn't tell us about the housefile,
			// Lets guess it is mapname-house.xml.
			map->housefile = g_configManager().getString(MAP_NAME);
			map->housefile += "-house.xml";
		}

		return map->houses.loadHousesXML(map->housefile);
	}

	/**
	 * Load custom  map monsters
	 * \param map Is the map class
	 * \returns true if the monsters spawn map custom was loaded successfully
	 */
	static bool loadMonstersCustom(Map* map, const std::string &mapName, int customMapIndex) {
		if (map->monsterfile.empty()) {
			// OTBM file doesn't tell us about the monsterfile,
			// Lets guess it is mapname-monster.xml.
			map->monsterfile = mapName;
			map->monsterfile += "-monster.xml";
		}
		return map->spawnsMonsterCustomMaps[customMapIndex].loadFromXML(map->monsterfile);
	}

	/**
	 * Load custom map npcs
	 * \param map Is the map class
	 * \returns true if the npcs spawn map custom was loaded successfully
	 */
	static bool loadNpcsCustom(Map* map, const std::string &mapName, int customMapIndex) {
		if (map->npcfile.empty()) {
			// OTBM file doesn't tell us about the npcfile,
			// Lets guess it is mapname-npc.xml.
			map->npcfile = mapName;
			map->npcfile += "-npc.xml";
		}

		return map->spawnsNpcCustomMaps[customMapIndex].loadFromXml(map->npcfile);
	}

	/**
	 * Load custom map houses
	 * \param map Is the map class
	 * \returns true if the map custom houses was loaded successfully
	 */
	static bool loadHousesCustom(Map* map, const std::string &mapName, int customMapIndex) {
		if (map->housefile.empty()) {
			// OTBM file doesn't tell us about the housefile,
			// Lets guess it is mapname-house.xml.
			map->housefile = mapName;
			map->housefile += "-house.xml";
		}
		return map->housesCustomMaps[customMapIndex].loadHousesXML(map->housefile);
	}

private:
	static void parseMapDataAttributes(FileStream &stream, Map* map, const std::string &fileName);
	static void parseWaypoints(FileStream &stream, Map &map);
	static void parseTowns(FileStream &stream, Map &map);
	static void parseTileArea(FileStream &stream, Map &map, const Position &pos);
};
