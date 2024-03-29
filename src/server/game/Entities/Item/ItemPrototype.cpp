/*
* Copyright (C) 2008-2016 TrinityCore <http://www.trinitycore.org/>
*
* This program is free software; you can redistribute it and/or modify it
* under the terms of the GNU General Public License as published by the
* Free Software Foundation; either version 2 of the License, or (at your
* option) any later version.
*
* This program is distributed in the hope that it will be useful, but WITHOUT
* ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
* FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
* more details.
*
* You should have received a copy of the GNU General Public License along
* with this program. If not, see <http://www.gnu.org/licenses/>.
*/

#include "ItemPrototype.h"

#include "SpellInfo.h"
#include "SpellMgr.h"

bool ItemTemplate::CanChangeEquipStateInCombat() const
{
    switch (InventoryType)
    {
    case INVTYPE_RELIC:
    case INVTYPE_SHIELD:
    case INVTYPE_HOLDABLE:
        return true;
    }

    switch (Class)
    {
    case ITEM_CLASS_WEAPON:
    case ITEM_CLASS_PROJECTILE:
        return true;
    }

    return false;
}


float ItemTemplate::getDPS() const
{
    if (Delay == 0)
        return 0;
    float temp = 0;
    for (int i = 0; i < MAX_ITEM_PROTO_DAMAGES; ++i)
        temp += Damage[i].DamageMin + Damage[i].DamageMax;
    return temp * 500 / Delay;
}


int32 ItemTemplate::getFeralBonus(int32 extraDPS /*= 0*/) const
{
    // 0x02A5F3 - is mask for Melee weapon from ItemSubClassMask.dbc
    if (Class == ITEM_CLASS_WEAPON && (1 << SubClass) & 0x02A5F3)
    {
        int32 bonus = int32((extraDPS + getDPS())*14.0f) - 767;
        if (bonus < 0)
            return 0;
        return bonus;
    }
    return 0;
}

float ItemTemplate::GetItemLevelIncludingQuality() const
{
    float itemLevel = (float)ItemLevel;
    switch (Quality)
    {
    case ITEM_QUALITY_POOR:
    case ITEM_QUALITY_NORMAL:
    case ITEM_QUALITY_UNCOMMON:
    case ITEM_QUALITY_ARTIFACT:
    case ITEM_QUALITY_HEIRLOOM:
        itemLevel -= 13; // leaving this as a separate statement since we do not know the real behavior in this case
        break;
    case ITEM_QUALITY_RARE:
        itemLevel -= 13;
        break;
    case ITEM_QUALITY_EPIC:
    case ITEM_QUALITY_LEGENDARY:
    default:
        break;
    }
    return std::max<float>(0.f, itemLevel);
}

uint32 ItemTemplate::GetSkill() const
{
    const static uint32 item_weapon_skills[MAX_ITEM_SUBCLASS_WEAPON] =
    {
        SKILL_AXES,     SKILL_2H_AXES,  SKILL_BOWS,          SKILL_GUNS,      SKILL_MACES,
        SKILL_2H_MACES, SKILL_POLEARMS, SKILL_SWORDS,        SKILL_2H_SWORDS, 0,
        SKILL_STAVES,   0,              0,                   SKILL_FIST_WEAPONS,   0,
        SKILL_DAGGERS,  SKILL_THROWN,   SKILL_ASSASSINATION, SKILL_CROSSBOWS, SKILL_WANDS,
        SKILL_FISHING
    };

    const static uint32 item_armor_skills[MAX_ITEM_SUBCLASS_ARMOR] =
    {
        0, SKILL_CLOTH, SKILL_LEATHER, SKILL_MAIL, SKILL_PLATE_MAIL, 0, SKILL_SHIELD, 0, 0, 0, 0
    };

    switch (Class)
    {
    case ITEM_CLASS_WEAPON:
        if (SubClass >= MAX_ITEM_SUBCLASS_WEAPON)
            return 0;
        else
            return item_weapon_skills[SubClass];

    case ITEM_CLASS_ARMOR:
        if (SubClass >= MAX_ITEM_SUBCLASS_ARMOR)
            return 0;
        else
            return item_armor_skills[SubClass];

    default:
        return 0;
    }
}

void ItemTemplate::_LoadTotalAP()
{
    int32 totalAP = 0;
    for (uint32 i = 0; i < StatsCount; ++i)
        if (ItemStat[i].ItemStatType == ITEM_MOD_ATTACK_POWER)
            totalAP += ItemStat[i].ItemStatValue;

    // some items can have equip spells with +AP
    for (uint32 i = 0; i < MAX_ITEM_PROTO_SPELLS; ++i)
        if (Spells[i].SpellId > 0 && Spells[i].SpellTrigger == ITEM_SPELLTRIGGER_ON_EQUIP)
            if (SpellInfo const* spellInfo = sSpellMgr->GetSpellInfo(Spells[i].SpellId))
                for (uint8 j = 0; j < MAX_SPELL_EFFECTS; ++j)
                    if (spellInfo->Effects[j].IsAura(SPELL_AURA_MOD_ATTACK_POWER))
                        totalAP += spellInfo->Effects[j].CalcValue();

    _totalAP = totalAP;
}