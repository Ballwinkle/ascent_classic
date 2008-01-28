/*
 * Ascent MMORPG Server
 * Copyright (C) 2005-2007 Ascent Team <http://www.ascentemu.com/>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#ifndef WOWSERVER_ITEM_H
#define WOWSERVER_ITEM_H

#include "Skill.h"

struct EnchantmentInstance
{
	EnchantEntry * Enchantment;
	bool BonusApplied;
	uint32 Slot;
	time_t ApplyTime;
	uint32 Duration;
	bool RemoveAtLogout;
	uint32 RandomSuffix;
};

typedef map<uint32, EnchantmentInstance> EnchantmentMap;

#define APPLY true
#define REMOVE false

#define RANDOM_SUFFIX_MAGIC_CALCULATION(__suffix, __scale) int32(float((float((__suffix)) * float((__scale)))) / 13340.0f);

class SERVER_DECL Item : public Object
{
public:
	Item ( );
	Item(uint32 high,uint32 low);
	virtual ~Item();

	void Create( uint32 itemid, Player* owner );

	ASCENT_INLINE ItemPrototype* GetProto() const { return m_itemProto; }
	ASCENT_INLINE void SetProto(ItemPrototype* pr) { m_itemProto = pr; }

	ASCENT_INLINE Player* GetOwner() const { return m_owner; }
	void SetOwner(Player *owner);

	ASCENT_INLINE bool IsContainer(){return (m_objectTypeId == TYPEID_CONTAINER) ? true : false; }
	
	//! DB Serialization
	void LoadFromDB(Field *fields, Player* plr, bool light);
	void SaveToDB(int8 containerslot, int8 slot, bool firstsave, QueryBuffer * buf);
	bool LoadAuctionItemFromDB(uint64 guid);
	void DeleteFromDB();
	
	ASCENT_INLINE void SoulBind()
	{
		this->SetFlag( ITEM_FIELD_FLAGS, ITEM_FLAG_SOULBOUND );
	}

	ASCENT_INLINE bool IsSoulbound()
	{
		return this->HasFlag( ITEM_FIELD_FLAGS, ITEM_FLAG_QUEST | ITEM_FLAG_SOULBOUND );
	}

	ASCENT_INLINE uint32 GetChargesLeft()
	{
		for(uint32 x=0;x<5;x++)
			if(m_itemProto->Spells[x].Id)
				return GetUInt32Value(ITEM_FIELD_SPELL_CHARGES+x);

		return 0;
	}

	ASCENT_INLINE time_t GetEnchantmentApplytime(uint32 slot)
	{
		EnchantmentMap::iterator itr = Enchantments.find(slot);
		if(itr == Enchantments.end()) return 0;
		else return itr->second.ApplyTime;
	}

	//! Adds an enchantment to the item.
	int32 AddEnchantment(EnchantEntry * Enchantment, uint32 Duration, bool Perm = false,
		bool apply = true, bool RemoveAtLogout = false,uint32 Slot_ = 0, uint32 RandomSuffix=0);
	uint32 GetSocketsCount();


	//! Removes an enchantment from the item.
	void RemoveEnchantment(uint32 EnchantmentSlot);
	
	// Removes related temporary enchants
	void RemoveRelatedEnchants(EnchantEntry * newEnchant);

	//! Adds the bonus on an enchanted item.
	void ApplyEnchantmentBonus(uint32 Slot, bool Apply);

	//! Applies all enchantment bonuses (use on equip)
	void ApplyEnchantmentBonuses();

	//! Removes all enchantment bonuses (use on dequip)
	void RemoveEnchantmentBonuses();

	//! Event to remove an enchantment.
	void EventRemoveEnchantment(uint32 Slot);

	//! Check if we have an enchantment of this id?
	int32 HasEnchantment(uint32 Id);

	//! Modify the time of an existing enchantment.
	void ModifyEnchantmentTime(uint32 Slot, uint32 Duration);

	//! Find free enchantment slot.
	int32 FindFreeEnchantSlot(EnchantEntry * Enchantment, uint32 random_type);

	//! Removes all enchantments.
	void RemoveAllEnchantments(bool OnlyTemporary);

	//! Sends SMSG_ITEM_UPDATE_ENCHANT_TIME
	void SendEnchantTimeUpdate(uint32 Slot, uint32 Duration);

	//! Applies any random properties the item has.
	void ApplyRandomProperties(bool apply);

	void RemoveProfessionEnchant();
	void RemoveSocketBonusEnchant();

	ASCENT_INLINE void SetCount(uint32 amt) { SetUInt32Value(ITEM_FIELD_STACK_COUNT,amt); }
	ASCENT_INLINE void SetDurability(uint32 Value) { SetUInt32Value(ITEM_FIELD_DURABILITY,Value); };
	ASCENT_INLINE void SetDurabilityToMax() { SetUInt32Value(ITEM_FIELD_DURABILITY,GetUInt32Value(ITEM_FIELD_MAXDURABILITY)); }
	ASCENT_INLINE uint32 GetDurability() { return GetUInt32Value(ITEM_FIELD_DURABILITY); }
	ASCENT_INLINE uint32 GetDurabilityMax() { return GetUInt32Value(ITEM_FIELD_MAXDURABILITY); }
	ASCENT_INLINE bool IsAmmoBag() { return (m_itemProto->Class==ITEM_CLASS_QUIVER); }

	void RemoveFromWorld();

	Loot *loot;
	bool locked;
	bool m_isDirty;

	EnchantmentInstance * GetEnchantment(uint32 slot);
	bool IsGemRelated(EnchantEntry * Enchantment);

	ASCENT_INLINE uint32 GetItemRandomSuffixFactor() { return m_uint32Values[ITEM_FIELD_PROPERTY_SEED]; }
	static uint32 GenerateRandomSuffixFactor(ItemPrototype * m_itemProto);

	ASCENT_INLINE void SetRandomProperty(uint32 id)
	{
		SetUInt32Value(ITEM_FIELD_RANDOM_PROPERTIES_ID, id);
		random_prop = id;
	}

	ASCENT_INLINE void SetRandomSuffix(uint32 id)
	{
		int32 r_id = -(int32(id));
		uint32 v = Item::GenerateRandomSuffixFactor(m_itemProto);
		SetUInt32Value(ITEM_FIELD_RANDOM_PROPERTIES_ID, (uint32)r_id);
		SetUInt32Value(ITEM_FIELD_PROPERTY_SEED, v);
		random_suffix = id;
	}

	bool HasEnchantments() { return (Enchantments.size() > 0) ? true : false; }

	uint32 wrapped_item_id;

protected:

	ItemPrototype *m_itemProto;
	EnchantmentMap Enchantments;
	uint32 _fields[ITEM_END];//this mem is wasted in case of container... but this will be fixed in future

	Player *m_owner; // let's not bother the manager with unneeded requests
	uint32 random_prop;
	uint32 random_suffix;
};

uint32 GetSkillByProto(uint32,uint32);

uint32 GetSellPriceForItem(ItemPrototype *proto, uint32 count);
uint32 GetBuyPriceForItem(ItemPrototype *proto, uint32 count, Player * plr, Creature * vendor);

uint32 GetSellPriceForItem(uint32 itemid, uint32 count);
uint32 GetBuyPriceForItem(uint32 itemid, uint32 count, Player * plr, Creature * vendor);

#endif