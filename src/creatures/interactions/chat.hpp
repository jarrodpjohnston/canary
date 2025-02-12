/**
 * Canary - A free and open-source MMORPG server emulator
 * Copyright (©) 2019-2022 OpenTibiaBR <opentibiabr@outlook.com>
 * Repository: https://github.com/opentibiabr/canary
 * License: https://github.com/opentibiabr/canary/blob/main/LICENSE
 * Contributors: https://github.com/opentibiabr/canary/graphs/contributors
 * Website: https://docs.opentibiabr.com/
 */

#pragma once

#include "utils/utils_definitions.hpp"
#include "lua/scripts/luascript.hpp"

class Party;
class Player;

using UsersMap = phmap::btree_map<uint32_t, Player*>;
using InvitedMap = phmap::btree_map<uint32_t, const Player*>;

class ChatChannel {
public:
	ChatChannel() = default;
	ChatChannel(uint16_t channelId, std::string channelName) :
		name(std::move(channelName)),
		id(channelId) { }

	virtual ~ChatChannel() = default;

	bool addUser(Player &player);
	bool removeUser(const Player &player);
	bool hasUser(const Player &player);

	bool talk(const Player &fromPlayer, SpeakClasses type, const std::string &text);
	void sendToAll(const std::string &message, SpeakClasses type) const;

	const std::string &getName() const {
		return name;
	}
	uint16_t getId() const {
		return id;
	}
	const UsersMap &getUsers() const {
		return users;
	}
	virtual const InvitedMap* getInvitedUsers() const {
		return nullptr;
	}

	virtual uint32_t getOwner() const {
		return 0;
	}

	bool isPublicChannel() const {
		return publicChannel;
	}

	bool executeOnJoinEvent(const Player &player);
	bool executeCanJoinEvent(const Player &player);
	bool executeOnLeaveEvent(const Player &player);
	bool executeOnSpeakEvent(const Player &player, SpeakClasses &type, const std::string &message);

protected:
	UsersMap users;

	std::string name;

	int32_t canJoinEvent = -1;
	int32_t onJoinEvent = -1;
	int32_t onLeaveEvent = -1;
	int32_t onSpeakEvent = -1;

	uint16_t id;
	bool publicChannel = false;

	friend class Chat;
};

class PrivateChatChannel final : public ChatChannel {
public:
	PrivateChatChannel(uint16_t channelId, std::string channelName) :
		ChatChannel(channelId, channelName) { }

	uint32_t getOwner() const override {
		return owner;
	}
	void setOwner(uint32_t newOwner) {
		this->owner = newOwner;
	}

	bool isInvited(uint32_t guid) const;

	void invitePlayer(const Player &player, Player &invitePlayer);
	void excludePlayer(const Player &player, Player &excludePlayer);

	bool removeInvite(uint32_t guid);

	void closeChannel() const;

	const InvitedMap* getInvitedUsers() const override {
		return &invites;
	}

private:
	InvitedMap invites;
	uint32_t owner = 0;
};

using ChannelList = std::list<ChatChannel*>;

class Chat {
public:
	Chat();

	// non-copyable
	Chat(const Chat &) = delete;
	Chat &operator=(const Chat &) = delete;

	static Chat &getInstance() {
		return inject<Chat>();
	}

	bool load();

	ChatChannel* createChannel(const Player &player, uint16_t channelId);
	bool deleteChannel(const Player &player, uint16_t channelId);

	ChatChannel* addUserToChannel(Player &player, uint16_t channelId);
	bool removeUserFromChannel(const Player &player, uint16_t channelId);
	void removeUserFromAllChannels(const Player &player);

	bool talkToChannel(const Player &player, SpeakClasses type, const std::string &text, uint16_t channelId);

	ChannelList getChannelList(const Player &player);

	ChatChannel* getChannel(const Player &player, uint16_t channelId);
	ChatChannel* getChannelById(uint16_t channelId);
	ChatChannel* getGuildChannelById(uint32_t guildId);
	PrivateChatChannel* getPrivateChannel(const Player &player);

	LuaScriptInterface* getScriptInterface() {
		return &scriptInterface;
	}

private:
	phmap::btree_map<uint16_t, ChatChannel> normalChannels;
	phmap::btree_map<uint16_t, PrivateChatChannel> privateChannels;
	phmap::btree_map<Party*, ChatChannel> partyChannels;
	phmap::btree_map<uint32_t, ChatChannel> guildChannels;

	LuaScriptInterface scriptInterface;

	PrivateChatChannel dummyPrivate;
};

constexpr auto g_chat = Chat::getInstance;
