/*
 * Ascent MMORPG Server
 * LUA Interface
 * Copyright (C) 2007 Burlex <burlex@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#ifndef __LUAENGINE_H
#define __LUAENGINE_H

/** Quest Events
 */
enum QuestEvents
{
	QUEST_EVENT_ON_COMPLETE		= 1,
	QUEST_EVENT_ON_ACCEPT		= 2,
	QUEST_EVENT_CAN_ACCEPT		= 3,
	QUEST_EVENT_COUNT,
};

/** Creature Events
 */
enum CreatureEvents
{
	CREATURE_EVENT_ON_ENTER_COMBAT		= 1,
	CREATURE_EVENT_ON_LEAVE_COMBAT		= 2,
	CREATURE_EVENT_ON_KILLED_TARGET		= 3,
	CREATURE_EVENT_ON_DIED				= 4,
	CREATURE_EVENT_AI_TICK				= 5,
	CREATURE_EVENT_ON_SPAWN				= 6,
	CREATURE_EVENT_ON_GOSSIP_TALK		= 7,
	CREATURE_EVENT_ON_REACH_WP			= 8,
	CREATURE_EVENT_ON_LEAVE_LIMBO		= 9,
	CREATURE_EVENT_PLAYER_ENTERS_RANGE	= 10,
	CREATURE_EVENT_COUNT,
};

/** GameObject Events
 */
enum GameObjectEvents
{
	GAMEOBJECT_EVENT_ON_SPAWN			= 1,
	GAMEOBJECT_EVENT_ON_USE				= 2,
	GAMEOBJECT_EVENT_COUNT,
};

class LuaEngine
{
private:
	lua_State * L;
	Mutex m_Lock;

public:
	LuaEngine();
	~LuaEngine();

	void LoadScripts();
	void Shutdown();
	void Restart();
	void RegisterCoreFunctions();
	inline Mutex& GetLock() { return m_Lock; }

	void OnUnitEvent(Unit * pUnit, uint32 EventType, Unit * pMiscUnit);
	void OnGameObjectEvent(GameObject * pGameObject, uint32 EventType, Unit * pMiscUnit);
};

struct LuaUnitBinding { const char * Functions[CREATURE_EVENT_COUNT]; };
struct LuaGameObjectBinding { const char * Functions[GAMEOBJECT_EVENT_COUNT]; };

class LuaEngineMgr : public Singleton<LuaEngineMgr>
{
private:
	typedef map<LuaEngine*, uint32> LuaEngineMap;
	LuaEngineMap m_engines;
	Mutex m_lock;

	typedef HM_NAMESPACE::hash_map<uint32, LuaUnitBinding> UnitBindingMap;
	typedef HM_NAMESPACE::hash_map<uint32, LuaGameObjectBinding> GameObjectBindingMap;
	UnitBindingMap m_unitBinding;
	GameObjectBindingMap m_gameobjectBinding;

public:
	void Startup();
	void Unload();
	LuaEngine* GetLuaEngine();
	void FinishedWithLuaEngine(LuaEngine * engine);

	void RegisterUnitEvent(uint32 Id, uint32 Event, const char * FunctionName);
	void RegisterGameObjectEvent(uint32 Id, uint32 Event, const char * FunctionName);

	inline const char * GetUnitEvent(uint32 Id, uint32 Event)
	{
		UnitBindingMap::iterator itr = m_unitBinding.find(Id);
		ASSERT(Event<CREATURE_EVENT_COUNT);
		return (itr == m_unitBinding.end()) ? NULL : itr->second.Functions[Event];
	}

	inline const char * GetGameObjectEvent(uint32 Id, uint32 Event)
	{
		UnitBindingMap::iterator itr = m_unitBinding.find(Id);
		ASSERT(Event<CREATURE_EVENT_COUNT);
		return (itr == m_unitBinding.end()) ? NULL : itr->second.Functions[Event];
	}

	void ReloadScripts();
};

#endif

