/*
 * Copyright (C) 2008-2014 TrinityCore <http://www.trinitycore.org/>
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

/// @todo Implement proper support for vehicle+player teleportation
/// @todo Use spell victory/defeat in wg instead of RewardMarkOfHonor() && RewardHonor
/// @todo Add proper implement of achievement

#include "BattlefieldWG.h"
#include "AchievementMgr.h"
#include "MapManager.h"
#include "ObjectMgr.h"
#include "Opcodes.h"
#include "Player.h"
#include "SpellAuras.h"
#include "TemporarySummon.h"
#include "Vehicle.h"
#include "WorldSession.h"
#include "Language.h"

BattlefieldWG::~BattlefieldWG()
{
    for (Workshop::const_iterator itr = WorkshopsList.begin(); itr != WorkshopsList.end(); ++itr)
        delete *itr;

    for (GameObjectBuilding::const_iterator itr = BuildingsInZone.begin(); itr != BuildingsInZone.end(); ++itr)
        delete *itr;
}

bool BattlefieldWG::SetupBattlefield()
{
    InitStalker(BATTLEFIELD_WG_NPC_STALKER, WintergraspStalkerPos[0], WintergraspStalkerPos[1], WintergraspStalkerPos[2], WintergraspStalkerPos[3]);

    m_TypeId = BATTLEFIELD_WG;                              // See enum BattlefieldTypes
    m_BattleId = BATTLEFIELD_BATTLEID_WG;
    m_ZoneId = BATTLEFIELD_WG_ZONEID;
    m_MapId = BATTLEFIELD_WG_MAPID;
    m_Map = sMapMgr->FindMap(m_MapId, 0);

    m_MaxPlayer = sWorld->getIntConfig(CONFIG_WINTERGRASP_PLR_MAX);
    m_IsEnabled = sWorld->getBoolConfig(CONFIG_WINTERGRASP_ENABLE);
    m_MinPlayer = sWorld->getIntConfig(CONFIG_WINTERGRASP_PLR_MIN);
    m_MinLevel = sWorld->getIntConfig(CONFIG_WINTERGRASP_PLR_MIN_LVL);
    m_BattleTime = sWorld->getIntConfig(CONFIG_WINTERGRASP_BATTLETIME) * MINUTE * IN_MILLISECONDS;
    m_NoWarBattleTime = sWorld->getIntConfig(CONFIG_WINTERGRASP_NOBATTLETIME) * MINUTE * IN_MILLISECONDS;
    m_RestartAfterCrash = sWorld->getIntConfig(CONFIG_WINTERGRASP_RESTART_AFTER_CRASH) * MINUTE * IN_MILLISECONDS;

    m_TimeForAcceptInvite = 20;
    m_StartGroupingTimer = 15 * MINUTE * IN_MILLISECONDS;
    m_StartGrouping = false;

    m_tenacityStack = 0;

    KickPosition.Relocate(5728.117f, 2714.346f, 697.733f, 0);
    KickPosition.m_mapId = m_MapId;

    RegisterZone(m_ZoneId);

    m_Data32.resize(BATTLEFIELD_WG_DATA_MAX);

    m_saveTimer = 60000;

	announcetime = 5 * MINUTE * IN_MILLISECONDS;

    // Init GraveYards
    SetGraveyardNumber(BATTLEFIELD_WG_GRAVEYARD_MAX);

    // Load from db
    if ((sWorld->getWorldState(BATTLEFIELD_WG_WORLD_STATE_ACTIVE) == 0) && (sWorld->getWorldState(BATTLEFIELD_WG_WORLD_STATE_DEFENDER) == 0)
            && (sWorld->getWorldState(ClockWorldState[0]) == 0))
    {
        sWorld->setWorldState(BATTLEFIELD_WG_WORLD_STATE_ACTIVE, uint64(false));
        sWorld->setWorldState(BATTLEFIELD_WG_WORLD_STATE_DEFENDER, uint64(urand(0, 1)));
        sWorld->setWorldState(ClockWorldState[0], uint64(m_NoWarBattleTime));
    }

    m_isActive = bool(sWorld->getWorldState(BATTLEFIELD_WG_WORLD_STATE_ACTIVE));
    m_DefenderTeam = TeamId(sWorld->getWorldState(BATTLEFIELD_WG_WORLD_STATE_DEFENDER));

    m_Timer = sWorld->getWorldState(ClockWorldState[0]);
    if (m_isActive)
    {
        m_isActive = false;
        m_Timer = m_RestartAfterCrash;
    }

    for (uint8 i = 0; i < BATTLEFIELD_WG_GRAVEYARD_MAX; i++)
    {
        BfGraveyardWG* graveyard = new BfGraveyardWG(this);

        // When between games, the graveyard is controlled by the defending team
        if (WGGraveYard[i].startcontrol == TEAM_NEUTRAL)
            graveyard->Initialize(m_DefenderTeam, WGGraveYard[i].gyid);
        else
            graveyard->Initialize(WGGraveYard[i].startcontrol, WGGraveYard[i].gyid);

        graveyard->SetTextId(WGGraveYard[i].textid);
        m_GraveyardList[i] = graveyard;
    }

	for (uint8 i = 0; i < WG_MAX_TOWER_GUARD; i++)
    {
        // Horde npc
        if (Creature* creature = SpawnCreature(WGtowerguard[i].entryHorde, WGtowerguard[i].x, WGtowerguard[i].y, WGtowerguard[i].z, WGtowerguard[i].o, TEAM_HORDE))
		{
            TowerCreature[TEAM_HORDE].insert(creature->GetGUID());
			HideNpc(creature);
		}
        // Alliance npc
        if (Creature* creature = SpawnCreature(WGtowerguard[i].entryAlliance, WGtowerguard[i].x, WGtowerguard[i].y, WGtowerguard[i].z, WGtowerguard[i].o, TEAM_ALLIANCE))
		{
            TowerCreature[TEAM_ALLIANCE].insert(creature->GetGUID());
			HideNpc(creature);
		}
    }

    // Spawn workshop creatures and gameobjects
    for (uint8 i = 0; i < WG_MAX_WORKSHOP; i++)
    {
        WGWorkshop* workshop = new WGWorkshop(this, i);
        if (i < BATTLEFIELD_WG_WORKSHOP_KEEP_WEST)
            workshop->GiveControlTo(GetAttackerTeam(), true);
        else
            workshop->GiveControlTo(GetDefenderTeam(), true);

        // Note: Capture point is added once the gameobject is created.
        WorkshopsList.insert(workshop);
    }

    // Spawn NPCs in the defender's keep, both Horde and Alliance
    for (uint8 i = 0; i < WG_MAX_KEEP_NPC; i++)
    {
        // Horde npc
        if (Creature* creature = SpawnCreature(WGKeepNPC[i].entryHorde, WGKeepNPC[i].x, WGKeepNPC[i].y, WGKeepNPC[i].z, WGKeepNPC[i].o, TEAM_HORDE))
            KeepCreature[TEAM_HORDE].insert(creature->GetGUID());
        // Alliance npc
        if (Creature* creature = SpawnCreature(WGKeepNPC[i].entryAlliance, WGKeepNPC[i].x, WGKeepNPC[i].y, WGKeepNPC[i].z, WGKeepNPC[i].o, TEAM_ALLIANCE))
            KeepCreature[TEAM_ALLIANCE].insert(creature->GetGUID());
    }

    // Hide NPCs from the Attacker's team in the keep
    for (GuidSet::const_iterator itr = KeepCreature[GetAttackerTeam()].begin(); itr != KeepCreature[GetAttackerTeam()].end(); ++itr)
        if (Creature* creature = GetCreature(*itr))
            HideNpc(creature);

    // Spawn Horde NPCs outside the keep
    for (uint8 i = 0; i < WG_OUTSIDE_ALLIANCE_NPC; i++)
        if (Creature* creature = SpawnCreature(WGOutsideNPC[i].entryHorde, WGOutsideNPC[i].x, WGOutsideNPC[i].y, WGOutsideNPC[i].z, WGOutsideNPC[i].o, TEAM_HORDE))
            OutsideCreature[TEAM_HORDE].insert(creature->GetGUID());

    // Spawn Alliance NPCs outside the keep
    for (uint8 i = WG_OUTSIDE_ALLIANCE_NPC; i < WG_MAX_OUTSIDE_NPC; i++)
        if (Creature* creature = SpawnCreature(WGOutsideNPC[i].entryAlliance, WGOutsideNPC[i].x, WGOutsideNPC[i].y, WGOutsideNPC[i].z, WGOutsideNPC[i].o, TEAM_ALLIANCE))
            OutsideCreature[TEAM_ALLIANCE].insert(creature->GetGUID());

    // Hide units outside the keep that are defenders
    for (GuidSet::const_iterator itr = OutsideCreature[GetDefenderTeam()].begin(); itr != OutsideCreature[GetDefenderTeam()].end(); ++itr)
        if (Creature* creature = GetCreature(*itr))
            HideNpc(creature);

    // Spawn turrets and hide them per default
/*    for (uint8 i = 0; i < WG_MAX_TURRET; i++)
    {
        Position towerCannonPos = WGTurret[i].GetPosition();
        if (Creature* creature = SpawnCreature(NPC_WINTERGRASP_TOWER_CANNON, towerCannonPos, TEAM_ALLIANCE))
        {
            CanonList.insert(creature->GetGUID());
            HideNpc(creature);
        }
    }
*/
    // Spawn all gameobjects
    for (uint8 i = 0; i < WG_MAX_OBJ; i++)
    {
        if (GameObject* go = SpawnGameObject(WGGameObjectBuilding[i].entry, WGGameObjectBuilding[i].x, WGGameObjectBuilding[i].y, WGGameObjectBuilding[i].z, WGGameObjectBuilding[i].o))
        {
            BfWGGameObjectBuilding* b = new BfWGGameObjectBuilding(this);
            b->Init(go, WGGameObjectBuilding[i].type, WGGameObjectBuilding[i].WorldState, WGGameObjectBuilding[i].nameId);
            if (!IsEnabled() && go->GetEntry() == GO_WINTERGRASP_VAULT_GATE)
                go->SetDestructibleState(GO_DESTRUCTIBLE_DESTROYED);
            BuildingsInZone.insert(b);
        }
    }

    // Spawning portal defender
    for (uint8 i = 0; i < WG_MAX_TELEPORTER; i++)
    {
        if (GameObject* go = SpawnGameObject(WGPortalDefenderData[i].entry, WGPortalDefenderData[i].x, WGPortalDefenderData[i].y, WGPortalDefenderData[i].z, WGPortalDefenderData[i].o))
        {
            DefenderPortalList.insert(go->GetGUID());
            go->SetFaction(WintergraspFaction[GetDefenderTeam()]);
        }
    }

    UpdateCounterVehicle(true);
    return true;
}

bool BattlefieldWG::Update(uint32 diff)
{
	uint8 workshopId = 0;
	bool phasenw =  false;
	bool phasene =  false;
	bool phasesw =  false;
	bool phasese =  false;

    bool m_return = Battlefield::Update(diff);
    if (m_saveTimer <= diff)
    {
        sWorld->setWorldState(BATTLEFIELD_WG_WORLD_STATE_ACTIVE, m_isActive);
        sWorld->setWorldState(BATTLEFIELD_WG_WORLD_STATE_DEFENDER, m_DefenderTeam);
        sWorld->setWorldState(ClockWorldState[0], m_Timer);
        m_saveTimer = 60 * IN_MILLISECONDS;
    }
    else
        m_saveTimer -= diff;

    if ((m_Timer > 1800000) && (m_Timer < 1801000) && (m_isActive == false))
	sWorld->SendWorldText(LANG_BF_WG_WORLD_ANNOUNCE_30);
	else if ((m_Timer > 600000) && (m_Timer < 601000) && (m_isActive == false))
	sWorld->SendWorldText(LANG_BF_WG_WORLD_ANNOUNCE_10);
	else if ((m_Timer > 300000) && (m_Timer < 301000) && (m_isActive == false))
	sWorld->SendWorldText(LANG_BF_WG_WORLD_ANNOUNCE_5);
	else if ((m_Timer > 120000) && (m_Timer < 121000) && (m_isActive == false))
	sWorld->SendWorldText(LANG_BF_WG_WORLD_ANNOUNCE_2);
	else if ((m_Timer > 60000) && (m_Timer < 61000) && (m_isActive == false))
	sWorld->SendWorldText(LANG_BF_WG_WORLD_ANNOUNCE_1);

	for (uint8 team = 0; team < 2; ++team)
		for (GuidSet::const_iterator itr = m_players[team].begin(); itr != m_players[team].end(); ++itr)
			if (Player* player = sObjectAccessor->FindPlayer(*itr))
			{
				if (player->HasAura(8326))
					if (player->getRace() == RACE_NIGHTELF)
						if (!player->HasAura(55173))
							player->CastSpell(player, 55173, true);

				/*if (player->isAFK())
					player->TeleportTo(KickPosition);*/
			}
	
	if (!IsWarTime())
    {
		for (uint8 team = 0; team < 2; ++team)
		{
			for (GuidSet::const_iterator itr = m_players[team].begin(); itr != m_players[team].end(); ++itr)
				if (Player* player = sObjectAccessor->FindPlayer(*itr))
				{
					player->RemoveAurasDueToSpell(58729);
					player->RemoveAurasDueToSpell(50494);
				}
		}
	}

	for (uint8 team = 0; team < 2; ++team)
		for (GuidSet::const_iterator itr = m_players[team].begin(); itr != m_players[team].end(); ++itr)
			if (Player* player = sObjectAccessor->FindPlayer(*itr))
			{
				if(player->HasAura(58730))
					if(!player->HasAura(45472))
						player->CastSpell(player, 58601, true);

				if (player->GetVehicleCreatureBase())
					if  (player->GetVehicleBase()->GetEntry() == 28366)
					{
						player->CastSpell(player, 42716, true);

						if (Creature* creature = player->FindNearestCreature(28366, 3.0f, true))
						{
							creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_DISABLE_MOVE);
							creature->CastSpell(creature, 42716, true);
						}
					}

				if (!player->GetVehicle())
					player->RemoveAurasDueToSpell(42716);

				player->RemoveAurasDueToSpell(65635);
				player->RemoveAurasDueToSpell(16612);
				player->RemoveAurasDueToSpell(17013);
				player->RemoveAurasDueToSpell(10767);
				player->RemoveAurasDueToSpell(48161);
				player->RemoveAurasDueToSpell(42995);
				player->RemoveAurasDueToSpell(15366);
				player->RemoveAurasDueToSpell(16609);
				player->RemoveAurasDueToSpell(24425);
				player->RemoveAurasDueToSpell(22888);
				player->RemoveAurasDueToSpell(34410);
				player->RemoveAurasDueToSpell(29235);
				player->RemoveAurasDueToSpell(24865);
				player->RemoveAurasDueToSpell(24998);
				player->RemoveAurasDueToSpell(36935);
			}

/*		for (uint8 team = 0; team < 2; ++team)
			for (GuidSet::const_iterator itr = KeepCreature[team].begin(); itr != KeepCreature[team].end(); ++itr)
				if (Creature* creature = GetCreature(*itr))
					if (!creature->IsInCombat() && creature->IsAlive())
						creature->GetMotionMaster()->MoveTargetedHome();
		
		for (uint8 team = 0; team < 2; ++team)
			for (GuidSet::const_iterator itr = OutsideCreature[team].begin(); itr != OutsideCreature[team].end(); ++itr)
				if (Creature* creature = GetCreature(*itr))
					if (!creature->IsInCombat() && creature->IsAlive())
						creature->GetMotionMaster()->MoveTargetedHome(); */

	if (!m_isActive)
		return m_return;

	for (GuidSet::const_iterator itr = m_PlayersInWar[TEAM_ALLIANCE].begin(); itr != m_PlayersInWar[TEAM_ALLIANCE].end(); ++itr)
    {
        if (Player* player = sObjectAccessor->FindPlayer(*itr))
        {
			if (player->FindNearestCreature(NPC_DWARVEN_SPIRIT_GUIDE, 20))
			{
				player->CastSpell(player, 50494, true);
				player->SetFullHealth();
			}

			if (!player->FindNearestCreature(NPC_DWARVEN_SPIRIT_GUIDE, 20))
			{
				player->RemoveAurasDueToSpell(58729);
				player->RemoveAurasDueToSpell(50494);
			}
		}
    }

	for (GuidSet::const_iterator itr = m_PlayersInWar[TEAM_HORDE].begin(); itr != m_PlayersInWar[TEAM_HORDE].end(); ++itr)
    {
        if (Player* player = sObjectAccessor->FindPlayer(*itr))
        {
			if (player->FindNearestCreature(NPC_TAUNKA_SPIRIT_GUIDE, 20))
			{
				player->CastSpell(player, 50494, true);
				player->SetFullHealth();
			}

			if (!player->FindNearestCreature(NPC_TAUNKA_SPIRIT_GUIDE, 20))
			{
				player->RemoveAurasDueToSpell(58729);
				player->RemoveAurasDueToSpell(50494);
			}
		}
    }

	uint32 alliancePlayers = m_PlayersInWar[TEAM_ALLIANCE].size();
    uint32 hordePlayers = m_PlayersInWar[TEAM_HORDE].size();

	if (diff >= announcetime)
    {
		sWorld->SendWorldText(LANG_BF_WG_WORLD_ANNOUNCE_START,alliancePlayers,hordePlayers);
		announcetime = 5 * MINUTE * IN_MILLISECONDS;
	} else
        announcetime -= diff;

	//ºÒ±¼ÀÇ ÀÇÁö
	if (alliancePlayers == hordePlayers || alliancePlayers < 1 || hordePlayers < 1)
	{
		for (uint8 team = 0; team < 2; ++team)
			for (GuidSet::const_iterator itr = m_players[team].begin(); itr != m_players[team].end(); ++itr)
				if (Player* player = sObjectAccessor->FindPlayer(*itr))
				{
					player->RemoveAurasDueToSpell(SPELL_TENACITY);
					player->RemoveAurasDueToSpell(SPELL_GREATEST_HONOR);
					player->RemoveAurasDueToSpell(SPELL_GREATER_HONOR);
					player->RemoveAurasDueToSpell(SPELL_GREAT_HONOR);
					player->RemoveAurasDueToSpell(SPELL_HONOR);
				}

		for (uint8 team = 0; team < 2; ++team)
			for (GuidSet::const_iterator itr = m_players[team].begin(); itr != m_players[team].end(); ++itr)
				if (Creature* creature = GetCreature(*itr))
				{
					creature->RemoveAurasDueToSpell(SPELL_TENACITY_VEHICLE);
					creature->RemoveAurasDueToSpell(SPELL_GREATEST_HONOR);
					creature->RemoveAurasDueToSpell(SPELL_GREATER_HONOR);
					creature->RemoveAurasDueToSpell(SPELL_GREAT_HONOR);
					creature->RemoveAurasDueToSpell(SPELL_HONOR);
				}
	}

	if (alliancePlayers > 0 && hordePlayers > 0)
	{
		if (alliancePlayers > hordePlayers)
		{
			uint32 playercount_check =  alliancePlayers / hordePlayers;
			uint32 newStack = 8;
			if (playercount_check <= 4)
				newStack = 6;
			if (playercount_check <= 3)
				newStack = 4;
			if (playercount_check <= 2)
				newStack = 2;

			uint32 crenewStack = 1;

			for (GuidSet::const_iterator itr = m_players[TEAM_HORDE].begin(); itr != m_players[TEAM_HORDE].end(); ++itr)
				if (Player* player = sObjectAccessor->FindPlayer(*itr))
					player->SetAuraStack(SPELL_TENACITY, player, newStack);

			for (GuidSet::const_iterator itr = m_vehicles[TEAM_HORDE].begin(); itr != m_vehicles[TEAM_HORDE].end(); ++itr)
				if (Creature* creature = GetCreature(*itr))
					creature->SetAuraStack(SPELL_TENACITY_VEHICLE, creature, crenewStack);

			uint32 buff_honor = SPELL_GREATEST_HONOR; //100%
			if (playercount_check <= 4)
				buff_honor = SPELL_GREATER_HONOR;
			if (playercount_check <= 3)
				buff_honor = SPELL_GREAT_HONOR;
			if (playercount_check <= 2)
				buff_honor = SPELL_HONOR;

			if (hordePlayers > 0)
			{
				for (GuidSet::const_iterator itr = m_players[TEAM_ALLIANCE].begin(); itr != m_players[TEAM_ALLIANCE].end(); ++itr)
					if (Player* player = sObjectAccessor->FindPlayer(*itr))
					{
						if (!player->HasAura(buff_honor))
						{
							player->RemoveAurasDueToSpell(SPELL_GREATEST_HONOR);
							player->RemoveAurasDueToSpell(SPELL_GREATER_HONOR);
							player->RemoveAurasDueToSpell(SPELL_GREAT_HONOR);
							player->RemoveAurasDueToSpell(SPELL_HONOR);
							player->CastSpell(player, buff_honor, true);
						}
					}
				for (GuidSet::const_iterator itr = m_vehicles[TEAM_ALLIANCE].begin(); itr != m_vehicles[TEAM_ALLIANCE].end(); ++itr)
					if (Creature* creature = GetCreature(*itr))
					{
						if (!creature->HasAura(buff_honor))
						{
							creature->RemoveAurasDueToSpell(SPELL_GREATEST_HONOR);
							creature->RemoveAurasDueToSpell(SPELL_GREATER_HONOR);
							creature->RemoveAurasDueToSpell(SPELL_GREAT_HONOR);
							creature->RemoveAurasDueToSpell(SPELL_HONOR);
							creature->CastSpell(creature, buff_honor, true);
						}
					 }
			}
		}

		if (alliancePlayers < hordePlayers)
		{
			uint32 playercount_check =  hordePlayers / alliancePlayers;
			uint32 newStack = 8;
			if (playercount_check <= 4)
				newStack = 6;
			if (playercount_check <= 3)
				newStack = 4;
			if (playercount_check <= 2)
				newStack = 2;

			uint32 crenewStack = 1;

			for (GuidSet::const_iterator itr = m_players[TEAM_ALLIANCE].begin(); itr != m_players[TEAM_ALLIANCE].end(); ++itr)
				if (Player* player = sObjectAccessor->FindPlayer(*itr))
					player->SetAuraStack(SPELL_TENACITY, player, newStack);

			for (GuidSet::const_iterator itr = m_vehicles[TEAM_ALLIANCE].begin(); itr != m_vehicles[TEAM_ALLIANCE].end(); ++itr)
				if (Creature* creature = GetCreature(*itr))
					creature->SetAuraStack(SPELL_TENACITY_VEHICLE, creature, crenewStack);

			uint32 buff_honor = SPELL_GREATEST_HONOR; //100%
			if (playercount_check <= 4)
				buff_honor = SPELL_GREATER_HONOR;
			if (playercount_check <= 3)
				buff_honor = SPELL_GREAT_HONOR;
			if (playercount_check <= 2)
				buff_honor = SPELL_HONOR;

			if (alliancePlayers > 0)
			{
				for (GuidSet::const_iterator itr = m_players[TEAM_HORDE].begin(); itr != m_players[TEAM_HORDE].end(); ++itr)
					if (Player* player = sObjectAccessor->FindPlayer(*itr))
					{
						if (!player->HasAura(buff_honor))
						{
							player->RemoveAurasDueToSpell(SPELL_GREATEST_HONOR);
							player->RemoveAurasDueToSpell(SPELL_GREATER_HONOR);
							player->RemoveAurasDueToSpell(SPELL_GREAT_HONOR);
							player->RemoveAurasDueToSpell(SPELL_HONOR);
							player->CastSpell(player, buff_honor, true);
						}
					}
				for (GuidSet::const_iterator itr = m_vehicles[TEAM_HORDE].begin(); itr != m_vehicles[TEAM_HORDE].end(); ++itr)
					if (Creature* creature = GetCreature(*itr))
					{
						if (!creature->HasAura(buff_honor))
						{
							creature->RemoveAurasDueToSpell(SPELL_GREATEST_HONOR);
							creature->RemoveAurasDueToSpell(SPELL_GREATER_HONOR);
							creature->RemoveAurasDueToSpell(SPELL_GREAT_HONOR);
							creature->RemoveAurasDueToSpell(SPELL_HONOR);
							creature->CastSpell(creature, buff_honor, true);
						}
					 }
			}
		}
	}
	/*if (alliancePlayers == hordePlayers || alliancePlayers < 1 || hordePlayers < 1)
	{
		for (uint8 team = 0; team < 2; ++team)
			for (GuidSet::const_iterator itr = m_players[team].begin(); itr != m_players[team].end(); ++itr)
				if (Player* player = sObjectAccessor->FindPlayer(*itr))
				{
					player->RemoveAurasDueToSpell(SPELL_TENACITY);
					player->RemoveAurasDueToSpell(SPELL_GREATEST_HONOR);
					player->RemoveAurasDueToSpell(SPELL_GREATER_HONOR);
					player->RemoveAurasDueToSpell(SPELL_GREAT_HONOR);
					player->RemoveAurasDueToSpell(SPELL_HONOR);
				}

		for (uint8 team = 0; team < 2; ++team)
			for (GuidSet::const_iterator itr = m_players[team].begin(); itr != m_players[team].end(); ++itr)
				if (Creature* creature = GetCreature(*itr))
				{
					creature->RemoveAurasDueToSpell(SPELL_TENACITY_VEHICLE);
					creature->RemoveAurasDueToSpell(SPELL_GREATEST_HONOR);
					creature->RemoveAurasDueToSpell(SPELL_GREATER_HONOR);
					creature->RemoveAurasDueToSpell(SPELL_GREAT_HONOR);
					creature->RemoveAurasDueToSpell(SPELL_HONOR);
				}
	}

	if (alliancePlayers > hordePlayers)
    {
		uint32 playercounf = alliancePlayers - hordePlayers;
		uint32 newStack = 16;
		if (playercounf < 22)
			newStack = 14;
		if (playercounf < 19)
			newStack = 12;
		if (playercounf < 16)
			newStack = 10;
		if (playercounf < 13)
			newStack = 8;
		if (playercounf < 10)
			newStack = 6;
		if (playercounf < 7)
			newStack = 4;
		if (playercounf < 4)
			newStack = 2;

		uint32 crenewStack = newStack / 2;

		for (GuidSet::const_iterator itr = m_players[TEAM_HORDE].begin(); itr != m_players[TEAM_HORDE].end(); ++itr)
			if (Player* player = sObjectAccessor->FindPlayer(*itr))
				player->SetAuraStack(SPELL_TENACITY, player, newStack);

		for (GuidSet::const_iterator itr = m_vehicles[TEAM_HORDE].begin(); itr != m_vehicles[TEAM_HORDE].end(); ++itr)
			if (Creature* creature = GetCreature(*itr))
				creature->SetAuraStack(SPELL_TENACITY_VEHICLE, creature, crenewStack);

		uint32 buff_honor = SPELL_GREATEST_HONOR; //100%
        if (newStack < 13)
            buff_honor = SPELL_GREATER_HONOR;
        if (newStack < 9)
            buff_honor = SPELL_GREAT_HONOR;
        if (newStack < 5)
            buff_honor = SPELL_HONOR;

		if (hordePlayers > 0)
		{
			for (GuidSet::const_iterator itr = m_players[TEAM_ALLIANCE].begin(); itr != m_players[TEAM_ALLIANCE].end(); ++itr)
				if (Player* player = sObjectAccessor->FindPlayer(*itr))
				{
					if (!player->HasAura(buff_honor))
					{
						player->RemoveAurasDueToSpell(SPELL_GREATEST_HONOR);
						player->RemoveAurasDueToSpell(SPELL_GREATER_HONOR);
						player->RemoveAurasDueToSpell(SPELL_GREAT_HONOR);
						player->RemoveAurasDueToSpell(SPELL_HONOR);
						player->CastSpell(player, buff_honor, true);
					}
				}
			for (GuidSet::const_iterator itr = m_vehicles[TEAM_ALLIANCE].begin(); itr != m_vehicles[TEAM_ALLIANCE].end(); ++itr)
				if (Creature* creature = GetCreature(*itr))
				{
					if (!creature->HasAura(buff_honor))
					{
						creature->RemoveAurasDueToSpell(SPELL_GREATEST_HONOR);
						creature->RemoveAurasDueToSpell(SPELL_GREATER_HONOR);
						creature->RemoveAurasDueToSpell(SPELL_GREAT_HONOR);
						creature->RemoveAurasDueToSpell(SPELL_HONOR);
						creature->CastSpell(creature, buff_honor, true);
					}
				 }
		}
	}

	if (alliancePlayers < hordePlayers)
    {
		uint32 playercounf = hordePlayers - alliancePlayers;
		uint32 newStack = 16;
		if (playercounf < 22)
			newStack = 14;
		if (playercounf < 19)
			newStack = 12;
		if (playercounf < 16)
			newStack = 10;
		if (playercounf < 13)
			newStack = 8;
		if (playercounf < 10)
			newStack = 6;
		if (playercounf < 7)
			newStack = 4;
		if (playercounf < 4)
			newStack = 2;

		uint32 crenewStack = newStack / 2;

		for (GuidSet::const_iterator itr = m_players[TEAM_ALLIANCE].begin(); itr != m_players[TEAM_ALLIANCE].end(); ++itr)
			if (Player* player = sObjectAccessor->FindPlayer(*itr))
				player->SetAuraStack(SPELL_TENACITY, player, newStack);

		for (GuidSet::const_iterator itr = m_vehicles[TEAM_ALLIANCE].begin(); itr != m_vehicles[TEAM_ALLIANCE].end(); ++itr)
			if (Creature* creature = GetCreature(*itr))
				creature->SetAuraStack(SPELL_TENACITY_VEHICLE, creature, crenewStack);

		uint32 buff_honor = SPELL_GREATEST_HONOR; //100%
        if (newStack < 13)
            buff_honor = SPELL_GREATER_HONOR;
        if (newStack < 9)
            buff_honor = SPELL_GREAT_HONOR;
        if (newStack < 5)
            buff_honor = SPELL_HONOR;

		if (alliancePlayers > 0)
		{
			for (GuidSet::const_iterator itr = m_players[TEAM_HORDE].begin(); itr != m_players[TEAM_HORDE].end(); ++itr)
				if (Player* player = sObjectAccessor->FindPlayer(*itr))
				{
					if (!player->HasAura(buff_honor))
					{
						player->RemoveAurasDueToSpell(SPELL_GREATEST_HONOR);
						player->RemoveAurasDueToSpell(SPELL_GREATER_HONOR);
						player->RemoveAurasDueToSpell(SPELL_GREAT_HONOR);
						player->RemoveAurasDueToSpell(SPELL_HONOR);
						player->CastSpell(player, buff_honor, true);
					}
				}
			for (GuidSet::const_iterator itr = m_vehicles[TEAM_HORDE].begin(); itr != m_vehicles[TEAM_HORDE].end(); ++itr)
				if (Creature* creature = GetCreature(*itr))
				{
					if (!creature->HasAura(buff_honor))
					{
						creature->RemoveAurasDueToSpell(SPELL_GREATEST_HONOR);
						creature->RemoveAurasDueToSpell(SPELL_GREATER_HONOR);
						creature->RemoveAurasDueToSpell(SPELL_GREAT_HONOR);
						creature->RemoveAurasDueToSpell(SPELL_HONOR);
						creature->CastSpell(creature, buff_honor, true);
					}
				 }
		}
	}   
*/
	for (GuidSet::const_iterator itr = CanonList.begin(); itr != CanonList.end(); ++itr)
    {
        if (Creature* creature = GetCreature(*itr))
			 if(creature->GetHomePosition().GetPositionX() != creature->GetPositionX())
				creature->RemoveFromWorld();
    }

	for (GuidSet::const_iterator itr = TowerCanonList.begin(); itr != TowerCanonList.end(); ++itr)
    {
        if (Creature* creature = GetCreature(*itr))
			 if(creature->GetHomePosition().GetPositionX() != creature->GetPositionX())
				creature->RemoveFromWorld();
    }

	for (Workshop::const_iterator itr = WorkshopsList.begin(); itr != WorkshopsList.end(); ++itr)
	 {
		BattlefieldWG* m_WG;

	    if (WGWorkshop* workshop = (*itr))
	    {
			switch (workshop->workshopId)
			{
			    case BATTLEFIELD_WG_WORKSHOP_NE:
					if (workshop->teamControl == TEAM_NEUTRAL)
					{
						    for (GuidSet::const_iterator itr = shopneCreature[TEAM_HORDE].begin(); itr != shopneCreature[TEAM_HORDE].end(); ++itr)
								if (Creature* creature = GetCreature(*itr))
									 HideNpc(creature);

							 for (GuidSet::const_iterator itr = shopneCreature[TEAM_ALLIANCE].begin(); itr != shopneCreature[TEAM_ALLIANCE].end(); ++itr)
								if (Creature* creature = GetCreature(*itr))
									 HideNpc(creature);
					}

					for (GuidSet::const_iterator itr = shopneCreature[TEAM_HORDE].begin(); itr != shopneCreature[TEAM_HORDE].end(); ++itr)
						if (Creature* creature = GetCreature(*itr))
							if(creature->GetPhaseMask() == 1)
								 phasene = true;

					for (GuidSet::const_iterator itr = shopneCreature[TEAM_ALLIANCE].begin(); itr != shopneCreature[TEAM_ALLIANCE].end(); ++itr)
						if (Creature* creature = GetCreature(*itr))
							if(creature->GetPhaseMask() == 1)
								 phasene = true;
					
					if (phasene)
					break;

					if (workshop->teamControl == TEAM_HORDE)
						for (GuidSet::const_iterator itr = shopneCreature[TEAM_HORDE].begin(); itr != shopneCreature[TEAM_HORDE].end(); ++itr)
							if (Creature* creature = GetCreature(*itr))
								ShowNpc(creature, true);

					if (workshop->teamControl == TEAM_ALLIANCE)
						 for (GuidSet::const_iterator itr = shopneCreature[TEAM_ALLIANCE].begin(); itr != shopneCreature[TEAM_ALLIANCE].end(); ++itr)
							if (Creature* creature = GetCreature(*itr))
								 ShowNpc(creature, true);
			        break;
			    case BATTLEFIELD_WG_WORKSHOP_NW:
					if (workshop->teamControl == TEAM_NEUTRAL)
					{
						    for (GuidSet::const_iterator itr = shopnwCreature[TEAM_HORDE].begin(); itr != shopnwCreature[TEAM_HORDE].end(); ++itr)
								if (Creature* creature = GetCreature(*itr))
									 HideNpc(creature);

							 for (GuidSet::const_iterator itr = shopnwCreature[TEAM_ALLIANCE].begin(); itr != shopnwCreature[TEAM_ALLIANCE].end(); ++itr)
								if (Creature* creature = GetCreature(*itr))
									 HideNpc(creature);
					}

					for (GuidSet::const_iterator itr = shopnwCreature[TEAM_HORDE].begin(); itr != shopnwCreature[TEAM_HORDE].end(); ++itr)
						if (Creature* creature = GetCreature(*itr))
							if(creature->GetPhaseMask() == 1)
								 phasenw = true;

					for (GuidSet::const_iterator itr = shopnwCreature[TEAM_ALLIANCE].begin(); itr != shopnwCreature[TEAM_ALLIANCE].end(); ++itr)
						if (Creature* creature = GetCreature(*itr))
							if(creature->GetPhaseMask() == 1)
								 phasenw = true;

					if (phasenw)
					break;

					if (workshop->teamControl == TEAM_HORDE)
						for (GuidSet::const_iterator itr = shopnwCreature[TEAM_HORDE].begin(); itr != shopnwCreature[TEAM_HORDE].end(); ++itr)
							if (Creature* creature = GetCreature(*itr))
								ShowNpc(creature, true);

					if (workshop->teamControl == TEAM_ALLIANCE)
						 for (GuidSet::const_iterator itr = shopnwCreature[TEAM_ALLIANCE].begin(); itr != shopnwCreature[TEAM_ALLIANCE].end(); ++itr)
							if (Creature* creature = GetCreature(*itr))
								 ShowNpc(creature, true);
			        break;
			    case BATTLEFIELD_WG_WORKSHOP_SE:
					if (workshop->teamControl == TEAM_NEUTRAL)
					{
						    for (GuidSet::const_iterator itr = shopseCreature[TEAM_HORDE].begin(); itr != shopseCreature[TEAM_HORDE].end(); ++itr)
								if (Creature* creature = GetCreature(*itr))
									 HideNpc(creature);

							 for (GuidSet::const_iterator itr = shopseCreature[TEAM_ALLIANCE].begin(); itr != shopseCreature[TEAM_ALLIANCE].end(); ++itr)
								if (Creature* creature = GetCreature(*itr))
									 HideNpc(creature);
					}

					for (GuidSet::const_iterator itr = shopseCreature[TEAM_HORDE].begin(); itr != shopseCreature[TEAM_HORDE].end(); ++itr)
						if (Creature* creature = GetCreature(*itr))
							if(creature->GetPhaseMask() == 1)
								 phasese = true;

					for (GuidSet::const_iterator itr = shopseCreature[TEAM_ALLIANCE].begin(); itr != shopseCreature[TEAM_ALLIANCE].end(); ++itr)
						if (Creature* creature = GetCreature(*itr))
							if(creature->GetPhaseMask() == 1)
								 phasese = true;

					if (phasese)
					break;

					if (workshop->teamControl == TEAM_HORDE)
						for (GuidSet::const_iterator itr = shopseCreature[TEAM_HORDE].begin(); itr != shopseCreature[TEAM_HORDE].end(); ++itr)
							if (Creature* creature = GetCreature(*itr))
								ShowNpc(creature, true);

					if (workshop->teamControl == TEAM_ALLIANCE)
						 for (GuidSet::const_iterator itr = shopseCreature[TEAM_ALLIANCE].begin(); itr != shopseCreature[TEAM_ALLIANCE].end(); ++itr)
							if (Creature* creature = GetCreature(*itr))
								 ShowNpc(creature, true);
			        break;
			    case BATTLEFIELD_WG_WORKSHOP_SW:
					if (workshop->teamControl == TEAM_NEUTRAL)
					{
						    for (GuidSet::const_iterator itr = shopswCreature[TEAM_HORDE].begin(); itr != shopswCreature[TEAM_HORDE].end(); ++itr)
								if (Creature* creature = GetCreature(*itr))
									 HideNpc(creature);

							 for (GuidSet::const_iterator itr = shopswCreature[TEAM_ALLIANCE].begin(); itr != shopswCreature[TEAM_ALLIANCE].end(); ++itr)
								if (Creature* creature = GetCreature(*itr))
									 HideNpc(creature);
					}

					for (GuidSet::const_iterator itr = shopswCreature[TEAM_HORDE].begin(); itr != shopswCreature[TEAM_HORDE].end(); ++itr)
						if (Creature* creature = GetCreature(*itr))
							if(creature->GetPhaseMask() == 1)
								 phasesw = true;

					for (GuidSet::const_iterator itr = shopswCreature[TEAM_ALLIANCE].begin(); itr != shopswCreature[TEAM_ALLIANCE].end(); ++itr)
						if (Creature* creature = GetCreature(*itr))
							if(creature->GetPhaseMask() == 1)
								 phasesw = true;

					if (phasesw)
					break;

					if (workshop->teamControl == TEAM_HORDE)
						for (GuidSet::const_iterator itr = shopswCreature[TEAM_HORDE].begin(); itr != shopswCreature[TEAM_HORDE].end(); ++itr)
							if (Creature* creature = GetCreature(*itr))
								ShowNpc(creature, true);

					if (workshop->teamControl == TEAM_ALLIANCE)
						 for (GuidSet::const_iterator itr = shopswCreature[TEAM_ALLIANCE].begin(); itr != shopswCreature[TEAM_ALLIANCE].end(); ++itr)
							if (Creature* creature = GetCreature(*itr))
								 ShowNpc(creature, true);
			        break;
			}
		}
	}

    return m_return;
}

void BattlefieldWG::OnBattleStart()
{
	for (GuidSet::const_iterator itr = shopnwCreature[TEAM_ALLIANCE].begin(); itr != shopnwCreature[TEAM_ALLIANCE].end(); ++itr)
		if (Creature* creature = GetCreature(*itr))
			creature->RemoveFromWorld();

	for (GuidSet::const_iterator itr = shopneCreature[TEAM_ALLIANCE].begin(); itr != shopneCreature[TEAM_ALLIANCE].end(); ++itr)
		if (Creature* creature = GetCreature(*itr))
			creature->RemoveFromWorld();

	for (GuidSet::const_iterator itr = shopseCreature[TEAM_ALLIANCE].begin(); itr != shopseCreature[TEAM_ALLIANCE].end(); ++itr)
		if (Creature* creature = GetCreature(*itr))
			creature->RemoveFromWorld();

	for (GuidSet::const_iterator itr = shopswCreature[TEAM_ALLIANCE].begin(); itr != shopswCreature[TEAM_ALLIANCE].end(); ++itr)
		if (Creature* creature = GetCreature(*itr))
			creature->RemoveFromWorld();

	for (GuidSet::const_iterator itr = shopnwCreature[TEAM_HORDE].begin(); itr != shopnwCreature[TEAM_HORDE].end(); ++itr)
		if (Creature* creature = GetCreature(*itr))
			creature->RemoveFromWorld();

	for (GuidSet::const_iterator itr = shopneCreature[TEAM_HORDE].begin(); itr != shopneCreature[TEAM_HORDE].end(); ++itr)
		if (Creature* creature = GetCreature(*itr))
			creature->RemoveFromWorld();

	for (GuidSet::const_iterator itr = shopseCreature[TEAM_HORDE].begin(); itr != shopseCreature[TEAM_HORDE].end(); ++itr)
		if (Creature* creature = GetCreature(*itr))
		   creature->RemoveFromWorld();

	for (GuidSet::const_iterator itr = shopswCreature[TEAM_HORDE].begin(); itr != shopswCreature[TEAM_HORDE].end(); ++itr)
		if (Creature* creature = GetCreature(*itr))
			creature->RemoveFromWorld();

		for (uint8 i = 0; i < WG_MAX_WORKSHOP_NE_NPC; i++)
    {
        // Horde npc
        if (Creature* creature = SpawnCreature(WGshopneNPC[i].entryHorde, WGshopneNPC[i].x, WGshopneNPC[i].y, WGshopneNPC[i].z, WGshopneNPC[i].o, TEAM_HORDE))
		{
            shopneCreature[TEAM_HORDE].insert(creature->GetGUID());
			HideNpc(creature);
		}
        // Alliance npc
        if (Creature* creature = SpawnCreature(WGshopneNPC[i].entryAlliance, WGshopneNPC[i].x, WGshopneNPC[i].y, WGshopneNPC[i].z, WGshopneNPC[i].o, TEAM_ALLIANCE))
		{
            shopneCreature[TEAM_ALLIANCE].insert(creature->GetGUID());
			HideNpc(creature);
		}
    }

	for (uint8 i = 0; i < WG_MAX_WORKSHOP_NW_NPC; i++)
    {
        // Horde npc
        if (Creature* creature = SpawnCreature(WGshopnwNPC[i].entryHorde, WGshopnwNPC[i].x, WGshopnwNPC[i].y, WGshopnwNPC[i].z, WGshopnwNPC[i].o, TEAM_HORDE))
		{
            shopnwCreature[TEAM_HORDE].insert(creature->GetGUID());
			HideNpc(creature);
		}
        // Alliance npc
        if (Creature* creature = SpawnCreature(WGshopnwNPC[i].entryAlliance, WGshopnwNPC[i].x, WGshopnwNPC[i].y, WGshopnwNPC[i].z, WGshopnwNPC[i].o, TEAM_ALLIANCE))
		{
            shopnwCreature[TEAM_ALLIANCE].insert(creature->GetGUID());
			HideNpc(creature);
		}
    }

	for (uint8 i = 0; i < WG_MAX_WORKSHOP_SE_NPC; i++)
    {
        // Horde npc
        if (Creature* creature = SpawnCreature(WGshopseNPC[i].entryHorde, WGshopseNPC[i].x, WGshopseNPC[i].y, WGshopseNPC[i].z, WGshopseNPC[i].o, TEAM_HORDE))
		{
            shopseCreature[TEAM_HORDE].insert(creature->GetGUID());
			HideNpc(creature);
		}
        // Alliance npc
        if (Creature* creature = SpawnCreature(WGshopseNPC[i].entryAlliance, WGshopseNPC[i].x, WGshopseNPC[i].y, WGshopseNPC[i].z, WGshopseNPC[i].o, TEAM_ALLIANCE))
		{
            shopseCreature[TEAM_ALLIANCE].insert(creature->GetGUID());
			HideNpc(creature);
		}
    }

	for (uint8 i = 0; i < WG_MAX_WORKSHOP_SW_NPC; i++)
    {
        // Horde npc
        if (Creature* creature = SpawnCreature(WGshopswNPC[i].entryHorde, WGshopswNPC[i].x, WGshopswNPC[i].y, WGshopswNPC[i].z, WGshopswNPC[i].o, TEAM_HORDE))
		{
            shopswCreature[TEAM_HORDE].insert(creature->GetGUID());
			HideNpc(creature);
		}
        // Alliance npc
        if (Creature* creature = SpawnCreature(WGshopswNPC[i].entryAlliance, WGshopswNPC[i].x, WGshopswNPC[i].y, WGshopswNPC[i].z, WGshopswNPC[i].o, TEAM_ALLIANCE))
		{
            shopswCreature[TEAM_ALLIANCE].insert(creature->GetGUID());
			HideNpc(creature);
		}
    }

	for (GuidSet::const_iterator itr = TowerCreature[TEAM_HORDE].begin(); itr != TowerCreature[TEAM_HORDE].end(); ++itr)
    {
        if (Creature* creature = GetCreature(*itr))
        {
            creature->RemoveFromWorld();
        }
    }

	for (GuidSet::const_iterator itr = TowerCreature[TEAM_ALLIANCE].begin(); itr != TowerCreature[TEAM_ALLIANCE].end(); ++itr)
    {
        if (Creature* creature = GetCreature(*itr))
        {
            creature->RemoveFromWorld();
        }
    }

	 if(GetAttackerTeam() == TEAM_ALLIANCE)
	{
		 for (uint8 i = 0; i < WG_MAX_TOWER_GUARD; i++)
		{
			if (Creature* creature = SpawnCreature(WGtowerguard[i].entryAlliance, WGtowerguard[i].x, WGtowerguard[i].y, WGtowerguard[i].z, WGtowerguard[i].o, TEAM_ALLIANCE))
			{
				TowerCreature[TEAM_ALLIANCE].insert(creature->GetGUID());
				ShowNpc(creature, true);
			}
		}
	}

	for (Workshop::const_iterator itr = WorkshopsList.begin(); itr != WorkshopsList.end(); ++itr)
        if (*itr) 
            (*itr)->GiveControlTo(TEAM_NEUTRAL,  false);

	if(GetAttackerTeam() == TEAM_HORDE)
	{
		 for (uint8 i = 0; i < WG_MAX_TOWER_GUARD; i++)
		{
			if (Creature* creature = SpawnCreature(WGtowerguard[i].entryHorde, WGtowerguard[i].x, WGtowerguard[i].y, WGtowerguard[i].z, WGtowerguard[i].o, TEAM_HORDE))
			{
				TowerCreature[TEAM_HORDE].insert(creature->GetGUID());
				ShowNpc(creature, true);
			}
		}
	}

/*	QueryResult result = WorldDatabase.PQuery("SELECT * FROM battlefield_point WHERE id = 1");  ÇÚµðÄÎ
	if(result)
	{
		int32 al_point = (*result)[1].GetInt32();
		int32 ho_point = (*result)[2].GetInt32();

		if(GetAttackerTeam() == TEAM_ALLIANCE)
		{
			if(al_point <= 300)
			{
				if (GameObject* bannerne = SpawnGameObject(GO_WINTERGRASP_FACTORY_BANNER_NE, 4944.539f, 2439.993f, 320.206f, 0))
				{
					m_bannerneGUID = bannerne->GetGUID();
				}
			}else
			{
				for (Workshop::const_iterator itr = WorkshopsList.begin(); itr != WorkshopsList.end(); ++itr)
					if (WGWorkshop* workshop = (*itr))
						if (workshop->workshopId == BATTLEFIELD_WG_WORKSHOP_NE)
							workshop->GiveControlTo(GetAttackerTeam(), true);
			}

			if (GameObject* bannernw = SpawnGameObject(GO_WINTERGRASP_FACTORY_BANNER_NW, 4941.4765f, 3340.4765f, 376.875f, 0))
			{
				m_bannernwGUID = bannernw->GetGUID();
			}
		}

		if(GetAttackerTeam() == TEAM_HORDE)
		{
			if (ho_point <= 300)
			{
				if (GameObject* bannernw = SpawnGameObject(GO_WINTERGRASP_FACTORY_BANNER_NW, 4941.4765f, 3340.4765f, 376.875f, 0))
				{
					m_bannernwGUID = bannernw->GetGUID();
				}
			}else
			{
				for (Workshop::const_iterator itr = WorkshopsList.begin(); itr != WorkshopsList.end(); ++itr)
					if (WGWorkshop* workshop = (*itr))
						if (workshop->workshopId == BATTLEFIELD_WG_WORKSHOP_NW)
							workshop->GiveControlTo(GetAttackerTeam(), true);
			}

			if (GameObject* bannerne = SpawnGameObject(GO_WINTERGRASP_FACTORY_BANNER_NE, 4944.539f, 2439.993f, 320.206f, 0))
			{
				m_bannerneGUID = bannerne->GetGUID();
			}
		}
	}
*/
	if (GameObject* bannernw = SpawnGameObject(GO_WINTERGRASP_FACTORY_BANNER_NW, 4941.4765f, 3340.4765f, 376.875f, 0))
    {
        m_bannernwGUID = bannernw->GetGUID();
    }

	if (GameObject* bannerne = SpawnGameObject(GO_WINTERGRASP_FACTORY_BANNER_NE, 4944.539f, 2439.993f, 320.206f, 0))
    {
        m_bannerneGUID = bannerne->GetGUID();
    }

	if (GameObject* bannerse = SpawnGameObject(GO_WINTERGRASP_FACTORY_BANNER_SE, 4404.298f, 2350.885f, 375.536f, 0))
    {
        m_bannerseGUID = bannerse->GetGUID();
    }

	if (GameObject* bannersw = SpawnGameObject(GO_WINTERGRASP_FACTORY_BANNER_SW, 4397.052f, 3289.444f, 372.428f, 0))
    {
        m_bannerswGUID = bannersw->GetGUID();
    }

	//Áö·Ú
	if (Creature* landmine = SpawnCreature(343620, 5397.585f,	2620.169f,	413.804f,	2.7737f, TEAM_ALLIANCE))
    {
		landmineGUID = landmine->GetGUID();
		landmine->setFaction(WintergraspFaction[GetDefenderTeam()]);
	}

	if (Creature* landmine1 = SpawnCreature(343620, 5397.47f,	3058.769f,	413.911f,	5.48254f, TEAM_ALLIANCE))
    {
		landmineGUID1 = landmine1->GetGUID();
		landmine1->setFaction(WintergraspFaction[GetDefenderTeam()]);
	}

	if (Creature* landmine2 = SpawnCreature(2347201, 5193.2f,	2742.23f,	413.013f,	2.51322f, TEAM_ALLIANCE))
    {
		landmineGUID2 = landmine2->GetGUID();
		landmine2->setFaction(WintergraspFaction[GetDefenderTeam()]);
	}

	if (Creature* landmine3 = SpawnCreature(2347201, 5307.93f,	2625.85f,	412.9f,	1.77966f, TEAM_ALLIANCE))
    {
		landmineGUID3 = landmine3->GetGUID();
		landmine3->setFaction(WintergraspFaction[GetDefenderTeam()]);
	}

	if (Creature* landmine4 = SpawnCreature(2347201, 5273.8f,	2661.08f,	412.852f,	5.97526f, TEAM_ALLIANCE))
    {
		landmineGUID4 = landmine4->GetGUID();
		landmine4->setFaction(WintergraspFaction[GetDefenderTeam()]);
	}

	if (Creature* landmine5 = SpawnCreature(2347201, 5309.92f,	3051.52f,	411.823f,	4.73747f, TEAM_ALLIANCE))
    {
		landmineGUID5 = landmine5->GetGUID();
		landmine5->setFaction(WintergraspFaction[GetDefenderTeam()]);
	}

	if (Creature* landmine6 = SpawnCreature(2347201, 5274.34f,	3017.88f,	411.7f,	0.741364f, TEAM_ALLIANCE))
    {
		landmineGUID6 = landmine6->GetGUID();
		landmine6->setFaction(WintergraspFaction[GetDefenderTeam()]);
	}

	if (Creature* landmine7 = SpawnCreature(2347201, 5192.43f,	2939.48f,	413.09f,	4.57725f, TEAM_ALLIANCE))
    {
		landmineGUID7 = landmine7->GetGUID();
		landmine7->setFaction(WintergraspFaction[GetDefenderTeam()]);
	}

	if (Creature* landmine8 = SpawnCreature(2347201, 5157.12f,	2905.42f,	412.978f,	0.296829f, TEAM_ALLIANCE))
    {
		landmineGUID8 = landmine8->GetGUID();
		landmine8->setFaction(WintergraspFaction[GetDefenderTeam()]);
	}

	if (Creature* landmine9 = SpawnCreature(2347201, 5158.68f,	2776.88f,	413.118f,	5.84959f, TEAM_ALLIANCE))
    {
		landmineGUID9 = landmine9->GetGUID();
		landmine9->setFaction(WintergraspFaction[GetDefenderTeam()]);
	}

	// Spawn turrets and hide them per default
    for (uint8 i = 0; i < WG_MAX_TURRET; i++)
    {
        Position towerCannonPos = WGTurret[i].GetPosition();
        if (Creature* creature = SpawnCreature(NPC_WINTERGRASP_TOWER_CANNON, towerCannonPos, TEAM_ALLIANCE))
        {
            CanonList.insert(creature->GetGUID());
            HideNpc(creature);
        }
    }
	
	for (uint8 i = 0; i < WG_MAX_TURRET_TOWER; i++)
    {
        Position towerCannonPostower = WGTurretTower[i].GetPosition();
        if (Creature* creature = SpawnCreature(NPC_WINTERGRASP_TOWER_CANNON, towerCannonPostower, TEAM_ALLIANCE))
        {
            TowerCanonList.insert(creature->GetGUID());
            HideNpc(creature);
        }
    }
		// Rebuild all wall
    for (GameObjectBuilding::const_iterator itr = BuildingsInZone.begin(); itr != BuildingsInZone.end(); ++itr)
    {
        if (*itr)
        {
            (*itr)->Removebuild();
        }
    }

	for (uint8 i = 0; i < WG_MAX_OBJ; i++)
    {
        if (GameObject* go = SpawnGameObject(WGGameObjectBuilding[i].entry, WGGameObjectBuilding[i].x, WGGameObjectBuilding[i].y, WGGameObjectBuilding[i].z, WGGameObjectBuilding[i].o))
        {
            BfWGGameObjectBuilding* b = new BfWGGameObjectBuilding(this);
            b->Init(go, WGGameObjectBuilding[i].type, WGGameObjectBuilding[i].WorldState, WGGameObjectBuilding[i].nameId);
            if (!IsEnabled() && go->GetEntry() == GO_WINTERGRASP_VAULT_GATE)
                go->SetDestructibleState(GO_DESTRUCTIBLE_DESTROYED);
            BuildingsInZone.insert(b);
        }
    }

		// Rebuild all wall
    for (GameObjectBuilding::const_iterator itr = BuildingsInZone.begin(); itr != BuildingsInZone.end(); ++itr)
    {
        if (*itr)
        {
            (*itr)->Rebuild();
            (*itr)->UpdateTurretAttack(false);
        }
    }

	 // Spawn titan relic
    if (GameObject* relic = SpawnGameObject(GO_WINTERGRASP_TITAN_S_RELIC, 5440.0f, 2840.8f, 430.43f, 0))
    {
        // Update faction of relic, only attacker can click on
        relic->SetFaction(WintergraspFaction[GetAttackerTeam()]);
        // Set in use (not allow to click on before last door is broken)
        relic->SetFlag(GAMEOBJECT_FLAGS, GO_FLAG_IN_USE);
        m_titansRelicGUID = relic->GetGUID();
    }
    else
        TC_LOG_ERROR("bg.battlefield", "WG: Failed to spawn titan relic.");

	if (bassGY_GUID)
		if (Creature* bassgy = GetCreature(bassGY_GUID))
			bassgy->RemoveFromWorld();
	bassGY_GUID = 0;

	if(Creature* bassgy_A = SpawnCreature(NPC_DWARVEN_SPIRIT_GUIDE, 5140.79f, 2179.120117f, 390.950989f, 1.97222f ,TEAM_ALLIANCE))
			bassGY_GUID_A = bassgy_A->GetGUID();

	if(Creature* bassgy_H = SpawnCreature(NPC_TAUNKA_SPIRIT_GUIDE, 5030.31543f, 3709.800781f, 372.34671f, 3.892127f,TEAM_HORDE))
			bassGY_GUID_H = bassgy_H->GetGUID();

	if(GetDefenderTeam() == TEAM_ALLIANCE)
	{
		if(Creature* factoryri = SpawnCreature(30499, 5392.91f, 2975.26f, 415.222f,4.555f,TEAM_ALLIANCE))
			factoryri_GUID = factoryri->GetGUID();

		if(Creature* factoryle = SpawnCreature(30499, 5391.609f, 2707.71f, 415.05f, 4.555f,TEAM_ALLIANCE))
			factoryle_GUID = factoryle->GetGUID();

		if(Creature* bassgy = SpawnCreature(NPC_DWARVEN_SPIRIT_GUIDE, 5537.47998f, 2898.909912f, 517.258972f, 4.86947f ,TEAM_ALLIANCE))
			bassGY_GUID = bassgy->GetGUID();
	}else{
		if(Creature* factoryri = SpawnCreature(30400, 5392.91f, 2975.26f, 415.222f,4.555f,TEAM_HORDE))
			factoryri_GUID = factoryri->GetGUID();

		if(Creature* factoryle = SpawnCreature(30400, 5391.609f, 2707.71f, 415.05f, 4.555f,TEAM_HORDE))
			factoryle_GUID = factoryle->GetGUID();

		if(Creature* bassgy = SpawnCreature(NPC_TAUNKA_SPIRIT_GUIDE, 5537.47998f, 2898.909912f, 517.258972f, 4.86947f ,TEAM_HORDE))
			bassGY_GUID = bassgy->GetGUID();
	}

    // Update tower visibility and update faction
    for (GuidSet::const_iterator itr = CanonList.begin(); itr != CanonList.end(); ++itr)
    {
        if (Creature* creature = GetCreature(*itr))
        {
            ShowNpc(creature, true);
            creature->setFaction(WintergraspFaction[GetDefenderTeam()]);
			creature->CastSpell(creature, 42716, true);
        }
    }

	for (GuidSet::const_iterator itr = TowerCanonList.begin(); itr != TowerCanonList.end(); ++itr)
    {
        if (Creature* creature = GetCreature(*itr))
        {
            ShowNpc(creature, true);
            creature->setFaction(WintergraspFaction[GetAttackerTeam()]);
			creature->CastSpell(creature, 42716, true);
        }
    }

	for (GuidSet::const_iterator itr = gsList.begin(); itr != gsList.end(); ++itr)
    {
        if (Creature* creature = GetCreature(*itr))
        {
            creature->RemoveFromWorld();
        }
    }

	 for (GuidSet::const_iterator itr = wrList.begin(); itr != wrList.end(); ++itr)
    {
        if (Creature* creature = GetCreature(*itr))
        {
            creature->RemoveFromWorld();
        }
    }

	for (GuidSet::const_iterator itr = ceeList.begin(); itr != ceeList.end(); ++itr)
    {
        if (Creature* creature = GetCreature(*itr))
        {
            creature->RemoveFromWorld();
        }
    }

	for (GuidSet::const_iterator itr = erList.begin(); itr != erList.end(); ++itr)
    {
        if (Creature* creature = GetCreature(*itr))
        {
            creature->RemoveFromWorld();
        }
    }

    SetData(BATTLEFIELD_WG_DATA_BROKEN_TOWER_ATT, 0);
    SetData(BATTLEFIELD_WG_DATA_BROKEN_TOWER_DEF, 0);
    SetData(BATTLEFIELD_WG_DATA_DAMAGED_TOWER_ATT, 0);
    SetData(BATTLEFIELD_WG_DATA_DAMAGED_TOWER_DEF, 0);

    // Update graveyard (in no war time all graveyard is to deffender, in war time, depend of base)
    for (Workshop::const_iterator itr = WorkshopsList.begin(); itr != WorkshopsList.end(); ++itr)
        if (*itr)
            (*itr)->UpdateGraveyardAndWorkshop();

	if(GetAttackerTeam() == TEAM_ALLIANCE)
	{
		for (GuidSet::const_iterator itr = m_players[TEAM_ALLIANCE].begin(); itr != m_players[TEAM_ALLIANCE].end(); ++itr)
        {
            if (Player* player = sObjectAccessor->FindPlayer(*itr))
            {
				if(!player->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_DISABLE_MOVE | UNIT_FLAG_TAXI_FLIGHT))
				{
					if (!player->IsGameMaster())
					{
						player->TeleportTo(571, 5095.828f, 2180.385f, 373.789f, 2.303783f);
						SendInitWorldStatesTo(player);
					}
				}
            }
        }

		for (GuidSet::const_iterator itr = m_players[TEAM_HORDE].begin(); itr != m_players[TEAM_HORDE].end(); ++itr)
        {
            if (Player* player = sObjectAccessor->FindPlayer(*itr))
            {
				if(!player->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_DISABLE_MOVE | UNIT_FLAG_TAXI_FLIGHT))
				{
					if (!player->IsGameMaster())
					{
						player->TeleportTo(571, 5349.8686f, 2838.481f, 409.240f, 0.046328f);
						SendInitWorldStatesTo(player);
					}
				}
            }
        }
	}

	if(GetAttackerTeam() == TEAM_HORDE)
	{
		for (GuidSet::const_iterator itr = m_players[TEAM_ALLIANCE].begin(); itr != m_players[TEAM_ALLIANCE].end(); ++itr)
        {
            if (Player* player = sObjectAccessor->FindPlayer(*itr))
            {
				if(!player->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_DISABLE_MOVE | UNIT_FLAG_TAXI_FLIGHT))
				{
					if (!player->IsGameMaster())
					{
						player->TeleportTo(571, 5349.8686f, 2838.481f, 409.240f, 0.046328f);
						SendInitWorldStatesTo(player);
					}
				}
            }
        }

		for (GuidSet::const_iterator itr = m_players[TEAM_HORDE].begin(); itr != m_players[TEAM_HORDE].end(); ++itr)
        {
            if (Player* player = sObjectAccessor->FindPlayer(*itr))
            {
				if(!player->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_DISABLE_MOVE | UNIT_FLAG_TAXI_FLIGHT))
				{
					if (!player->IsGameMaster())
					{
						player->TeleportTo(571, 5026.993f, 3673.337f, 366.1f, 3.855737f);
						SendInitWorldStatesTo(player);
					}
				}
            }
        }
	}

 /*   for (uint8 team = 0; team < 2; ++team)
        for (GuidSet::const_iterator itr = m_players[team].begin(); itr != m_players[team].end(); ++itr)
        {
            // Kick player in orb room, TODO: offline player ?
            if (Player* player = sObjectAccessor->FindPlayer(*itr))
            {
                float x, y, z;
                player->GetPosition(x, y, z);
                if (5500 > x && x > 5392 && y < 2880 && y > 2800 && z < 480)
                    player->TeleportTo(571, 5349.8686f, 2838.481f, 409.240f, 0.046328f);
                SendInitWorldStatesTo(player);
            }
        }*/
    // Initialize vehicle counter
    UpdateCounterVehicle(true);
    // Send start warning to all players
    SendWarningToAllInZone(BATTLEFIELD_WG_TEXT_START);
}

void BattlefieldWG::UpdateCounterVehicle(bool init)
{
    if (init)
    {
        SetData(BATTLEFIELD_WG_DATA_VEHICLE_H, 0);
        SetData(BATTLEFIELD_WG_DATA_VEHICLE_A, 0);
    }
    SetData(BATTLEFIELD_WG_DATA_MAX_VEHICLE_H, 0);
    SetData(BATTLEFIELD_WG_DATA_MAX_VEHICLE_A, 0);

    for (Workshop::const_iterator itr = WorkshopsList.begin(); itr != WorkshopsList.end(); ++itr)
    {
        if (WGWorkshop* workshop = (*itr))
        {
            if (workshop->teamControl == TEAM_ALLIANCE)
                UpdateData(BATTLEFIELD_WG_DATA_MAX_VEHICLE_A, 4);
            else if (workshop->teamControl == TEAM_HORDE)
                UpdateData(BATTLEFIELD_WG_DATA_MAX_VEHICLE_H, 4);
        }
    }

    UpdateVehicleCountWG();
}

void BattlefieldWG::OnBattleEnd(bool endByTimer)
{
	uint32 alliancePlayers = m_PlayersInWar[TEAM_ALLIANCE].size();
    uint32 hordePlayers = m_PlayersInWar[TEAM_HORDE].size();

	sWorld->SendWorldText(LANG_BF_WG_WORLD_ANNOUNCE_END,alliancePlayers,hordePlayers);

	for (uint8 i = 0; i < WG_MAX_GS; i++)
    {
        Position gsPos = GlacialSpirit[i].GetPosition();
        if (Creature* creature = SpawnCreature(NPC_WINTERGRASP_GLACIAL_SPIRIT, gsPos, TEAM_NEUTRAL))
        {
            gsList.insert(creature->GetGUID());
			creature->setFaction(834);
        }
    }

	for (uint8 i = 0; i < WG_MAX_WR; i++)
    {
        Position wrPos = WateRevenant[i].GetPosition();
        if (Creature* creature = SpawnCreature(NPC_WINTERGRASP_GLACIAL_WR, wrPos, TEAM_NEUTRAL))
        {
            wrList.insert(creature->GetGUID());
			creature->setFaction(834);
        }
    }

	for (uint8 i = 0; i < WG_MAX_CEE; i++)
    {
        Position ceePos = EarthboundRevenant[i].GetPosition();
        if (Creature* creature = SpawnCreature(NPC_WINTERGRASP_GLACIAL_CEE, ceePos, TEAM_NEUTRAL))
        {
            ceeList.insert(creature->GetGUID());
			creature->setFaction(834);
        }
    }

	for (uint8 i = 0; i < WG_MAX_ER; i++)
    {
        Position erPos = ChilledEarthElemental[i].GetPosition();
        if (Creature* creature = SpawnCreature(NPC_WINTERGRASP_GLACIAL_ER, erPos, TEAM_NEUTRAL))
        {
            erList.insert(creature->GetGUID());
			creature->setFaction(834);
        }
    }

			// Rebuild all wall
    for (GameObjectBuilding::const_iterator itr = BuildingsInZone.begin(); itr != BuildingsInZone.end(); ++itr)
    {
        if (*itr)
        {
            (*itr)->Removebuild();
        }
    }

	for (uint8 i = 0; i < WG_MAX_OBJ; i++)
    {
        if (GameObject* go = SpawnGameObject(WGGameObjectBuilding[i].entry, WGGameObjectBuilding[i].x, WGGameObjectBuilding[i].y, WGGameObjectBuilding[i].z, WGGameObjectBuilding[i].o))
        {
            BfWGGameObjectBuilding* b = new BfWGGameObjectBuilding(this);
            b->Init(go, WGGameObjectBuilding[i].type, WGGameObjectBuilding[i].WorldState, WGGameObjectBuilding[i].nameId);
            if (!IsEnabled() && go->GetEntry() == GO_WINTERGRASP_VAULT_GATE)
                go->SetDestructibleState(GO_DESTRUCTIBLE_DESTROYED);
            BuildingsInZone.insert(b);
        }
    }

		// Rebuild all wall
    for (GameObjectBuilding::const_iterator itr = BuildingsInZone.begin(); itr != BuildingsInZone.end(); ++itr)
    {
        if (*itr)
        {
            (*itr)->Rebuild();
            (*itr)->UpdateTurretAttack(false);
        }
    }

	for (Workshop::const_iterator itr = WorkshopsList.begin(); itr != WorkshopsList.end(); ++itr)
        if (*itr) 
            (*itr)->GiveControlTo(TEAM_NEUTRAL,  false);

	if (m_bannernwGUID)
        if (GameObject* bannernw = GetGameObject(m_bannernwGUID))
            bannernw->RemoveFromWorld();
    m_bannernwGUID = 0;

	if (m_bannerneGUID)
        if (GameObject* bannerne = GetGameObject(m_bannerneGUID))
            bannerne->RemoveFromWorld();
    m_bannerneGUID = 0;

	if (m_bannerswGUID)
        if (GameObject* bannersw = GetGameObject(m_bannerswGUID))
            bannersw->RemoveFromWorld();
    m_bannerswGUID = 0;

	if (m_bannerseGUID)
        if (GameObject* bannerse = GetGameObject(m_bannerseGUID))
            bannerse->RemoveFromWorld();
    m_bannerseGUID = 0;

	//Áö·Ú »èÁ¦
	if (landmineGUID)
        if (Creature* creature = GetCreature(landmineGUID))
            creature->RemoveFromWorld();
    landmineGUID = 0;

	if (landmineGUID1)
        if (Creature* creature = GetCreature(landmineGUID1))
            creature->RemoveFromWorld();
    landmineGUID1 = 0;

	if (landmineGUID2)
        if (Creature* creature = GetCreature(landmineGUID2))
            creature->RemoveFromWorld();
    landmineGUID2 = 0;

	if (landmineGUID3)
        if (Creature* creature = GetCreature(landmineGUID3))
            creature->RemoveFromWorld();
    landmineGUID3 = 0;

	if (landmineGUID4)
        if (Creature* creature = GetCreature(landmineGUID4))
            creature->RemoveFromWorld();
    landmineGUID4 = 0;

	if (landmineGUID5)
        if (Creature* creature = GetCreature(landmineGUID5))
            creature->RemoveFromWorld();
    landmineGUID5 = 0;

	if (landmineGUID6)
        if (Creature* creature = GetCreature(landmineGUID6))
            creature->RemoveFromWorld();
    landmineGUID6 = 0;

	if (landmineGUID7)
        if (Creature* creature = GetCreature(landmineGUID7))
            creature->RemoveFromWorld();
    landmineGUID7 = 0;

	if (landmineGUID8)
        if (Creature* creature = GetCreature(landmineGUID8))
            creature->RemoveFromWorld();
    landmineGUID8 = 0;

	if (landmineGUID9)
        if (Creature* creature = GetCreature(landmineGUID9))
            creature->RemoveFromWorld();
    landmineGUID9 = 0;

	for (GuidSet::const_iterator itr = TowerCreature[TEAM_ALLIANCE].begin(); itr != TowerCreature[TEAM_ALLIANCE].end(); ++itr)
    {
        if (Creature* creature = GetCreature(*itr))
        {
            creature->RemoveFromWorld();
        }
    }

	for (GuidSet::const_iterator itr = TowerCreature[TEAM_HORDE].begin(); itr != TowerCreature[TEAM_HORDE].end(); ++itr)
    {
        if (Creature* creature = GetCreature(*itr))
        {
            creature->RemoveFromWorld();
        }
    }

	for (GuidSet::const_iterator itr = shopnwCreature[TEAM_ALLIANCE].begin(); itr != shopnwCreature[TEAM_ALLIANCE].end(); ++itr)
		if (Creature* creature = GetCreature(*itr))
			creature->RemoveFromWorld();

	for (GuidSet::const_iterator itr = shopneCreature[TEAM_ALLIANCE].begin(); itr != shopneCreature[TEAM_ALLIANCE].end(); ++itr)
		if (Creature* creature = GetCreature(*itr))
			creature->RemoveFromWorld();

	for (GuidSet::const_iterator itr = shopseCreature[TEAM_ALLIANCE].begin(); itr != shopseCreature[TEAM_ALLIANCE].end(); ++itr)
		if (Creature* creature = GetCreature(*itr))
			creature->RemoveFromWorld();

	for (GuidSet::const_iterator itr = shopswCreature[TEAM_ALLIANCE].begin(); itr != shopswCreature[TEAM_ALLIANCE].end(); ++itr)
		if (Creature* creature = GetCreature(*itr))
			creature->RemoveFromWorld();

	for (GuidSet::const_iterator itr = shopnwCreature[TEAM_HORDE].begin(); itr != shopnwCreature[TEAM_HORDE].end(); ++itr)
		if (Creature* creature = GetCreature(*itr))
			creature->RemoveFromWorld();

	for (GuidSet::const_iterator itr = shopneCreature[TEAM_HORDE].begin(); itr != shopneCreature[TEAM_HORDE].end(); ++itr)
		if (Creature* creature = GetCreature(*itr))
			creature->RemoveFromWorld();

	for (GuidSet::const_iterator itr = shopseCreature[TEAM_HORDE].begin(); itr != shopseCreature[TEAM_HORDE].end(); ++itr)
		if (Creature* creature = GetCreature(*itr))
		   creature->RemoveFromWorld();

	for (GuidSet::const_iterator itr = shopswCreature[TEAM_HORDE].begin(); itr != shopswCreature[TEAM_HORDE].end(); ++itr)
		if (Creature* creature = GetCreature(*itr))
			creature->RemoveFromWorld();

	 if(GetAttackerTeam() == TEAM_ALLIANCE)
	{
		 for (uint8 i = 0; i < WG_MAX_TOWER_GUARD; i++)
		{
			if (Creature* creature = SpawnCreature(WGtowerguard[i].entryAlliance, WGtowerguard[i].x, WGtowerguard[i].y, WGtowerguard[i].z, WGtowerguard[i].o, TEAM_ALLIANCE))
			{
				TowerCreature[TEAM_ALLIANCE].insert(creature->GetGUID());
				ShowNpc(creature, true);
			}
		}
	}

	if(GetAttackerTeam() == TEAM_HORDE)
	{
		for (uint8 i = 0; i < WG_MAX_TOWER_GUARD; i++)
		{
			// Horde npc
			if (Creature* creature = SpawnCreature(WGtowerguard[i].entryHorde, WGtowerguard[i].x, WGtowerguard[i].y, WGtowerguard[i].z, WGtowerguard[i].o, TEAM_HORDE))
			{
				TowerCreature[TEAM_HORDE].insert(creature->GetGUID());
				ShowNpc(creature, true);
			}
		}
	}

	if (bassGY_GUID)
		if (Creature* bassgy = GetCreature(bassGY_GUID))
			bassgy->RemoveFromWorld();
	bassGY_GUID = 0;

	if (bassGY_GUID_A)
		if (Creature* bassgy_A = GetCreature(bassGY_GUID_A))
			bassgy_A->RemoveFromWorld();
	bassGY_GUID_A = 0;

	if (bassGY_GUID_H)
		if (Creature* bassgy_H = GetCreature(bassGY_GUID_H))
			bassgy_H->RemoveFromWorld();
	bassGY_GUID_H = 0;

	if (factoryle_GUID)
        if (Creature* factoryle = GetCreature(factoryle_GUID))
            factoryle->RemoveFromWorld();
    factoryle_GUID = 0;

	if (factoryri_GUID)
        if (Creature* factoryri = GetCreature(factoryri_GUID))
            factoryri->RemoveFromWorld();
    factoryri_GUID = 0;

	//end

	SetData(BATTLEFIELD_WG_DATA_BROKEN_TOWER_ATT, 0);
    SetData(BATTLEFIELD_WG_DATA_BROKEN_TOWER_DEF, 0);
    SetData(BATTLEFIELD_WG_DATA_DAMAGED_TOWER_ATT, 0);
    SetData(BATTLEFIELD_WG_DATA_DAMAGED_TOWER_DEF, 0);
	
    // Remove relic
    if (m_titansRelicGUID)
        if (GameObject* relic = GetGameObject(m_titansRelicGUID))
            relic->RemoveFromWorld();
    m_titansRelicGUID = 0;

    // Remove turret
    for (GuidSet::const_iterator itr = CanonList.begin(); itr != CanonList.end(); ++itr)
    {
        if (Creature* creature = GetCreature(*itr))
        {
            if (!endByTimer)
                creature->setFaction(WintergraspFaction[GetDefenderTeam()]);
            creature->RemoveFromWorld();
        }
    }

	for (GuidSet::const_iterator itr = TowerCanonList.begin(); itr != TowerCanonList.end(); ++itr)
    {
        if (Creature* creature = GetCreature(*itr))
        {
            if (!endByTimer)
                creature->setFaction(WintergraspFaction[GetAttackerTeam()]);
            creature->RemoveFromWorld();
        }
    }

    if (!endByTimer) // One player triggered the relic
    {
        // Change all npc in keep
        for (GuidSet::const_iterator itr = KeepCreature[GetAttackerTeam()].begin(); itr != KeepCreature[GetAttackerTeam()].end(); ++itr)
            if (Creature* creature = GetCreature(*itr))
                HideNpc(creature);

        for (GuidSet::const_iterator itr = KeepCreature[GetDefenderTeam()].begin(); itr != KeepCreature[GetDefenderTeam()].end(); ++itr)
            if (Creature* creature = GetCreature(*itr))
                ShowNpc(creature, true);

        // Change all npc out of keep
        for (GuidSet::const_iterator itr = OutsideCreature[GetDefenderTeam()].begin(); itr != OutsideCreature[GetDefenderTeam()].end(); ++itr)
            if (Creature* creature = GetCreature(*itr))
                HideNpc(creature);

        for (GuidSet::const_iterator itr = OutsideCreature[GetAttackerTeam()].begin(); itr != OutsideCreature[GetAttackerTeam()].end(); ++itr)
            if (Creature* creature = GetCreature(*itr))
                ShowNpc(creature, true);
    }

    // Update all graveyard, control is to defender when no wartime
    for (uint8 i = 0; i < BATTLEFIELD_WG_GY_HORDE; i++)
        if (BfGraveyard* graveyard = GetGraveyardById(i))
            graveyard->GiveControlTo(GetDefenderTeam());

    for (GuidSet::const_iterator itr = m_KeepGameObject[GetDefenderTeam()].begin(); itr != m_KeepGameObject[GetDefenderTeam()].end(); ++itr)
        if (GameObject* object = GetGameObject(*itr))
            object->SetRespawnTime(RESPAWN_IMMEDIATELY);

    for (GuidSet::const_iterator itr = m_KeepGameObject[GetAttackerTeam()].begin(); itr != m_KeepGameObject[GetAttackerTeam()].end(); ++itr)
        if (GameObject* object = GetGameObject(*itr))
            object->SetRespawnTime(RESPAWN_ONE_DAY);

    // Update portal defender faction
    for (GuidSet::const_iterator itr = DefenderPortalList.begin(); itr != DefenderPortalList.end(); ++itr)
        if (GameObject* portal = GetGameObject(*itr))
            portal->SetFaction(WintergraspFaction[GetDefenderTeam()]);

	// ½Â¸®ÆÀ º¯°æ
/*	QueryResult result = WorldDatabase.PQuery("SELECT * FROM battlefield_point WHERE id = 1");
	if(result)
	{
		int32 win_team = (*result)[3].GetInt32();
		
		if(GetDefenderTeam() == TEAM_ALLIANCE)
			if (win_team != 1)
			{
				QueryResult result_insert = CharacterDatabase.PQuery("DELETE FROM battlefieldwg_account");
				QueryResult result_ch_point = WorldDatabase.PQuery("UPDATE battlefield_point SET  win_team = 1 WHERE id = 1");
			}

		if(GetDefenderTeam() == TEAM_HORDE)
			if (win_team != 2)
			{
				QueryResult result_insert = CharacterDatabase.PQuery("DELETE FROM battlefieldwg_account");
				QueryResult result_ch_point = WorldDatabase.PQuery("UPDATE battlefield_point SET  win_team =  2 WHERE id = 1");
			}
	}
*/
    // Saving data
    for (GameObjectBuilding::const_iterator itr = BuildingsInZone.begin(); itr != BuildingsInZone.end(); ++itr)
        (*itr)->Save();
    for (Workshop::const_iterator itr = WorkshopsList.begin(); itr != WorkshopsList.end(); ++itr)
        (*itr)->Save();

	for (GuidSet::const_iterator itr = m_PlayersInWar[GetDefenderTeam()].begin(); itr != m_PlayersInWar[GetDefenderTeam()].end(); ++itr)
    {
        if (Player* player = sObjectAccessor->FindPlayer(*itr))
        {
            player->CastSpell(player, SPELL_ESSENCE_OF_WINTERGRASP, true);
            player->CastSpell(player, SPELL_VICTORY_REWARD, true);
            // Send Wintergrasp victory achievement
            DoCompleteOrIncrementAchievement(ACHIEVEMENTS_WIN_WG, player);
            // Award achievement for succeeding in Wintergrasp in 10 minutes or less
            if (!endByTimer && GetTimer() <= 10000)
                DoCompleteOrIncrementAchievement(ACHIEVEMENTS_WIN_WG_TIMER_10, player);

			if (player->GetQuestStatus(13183) == QUEST_STATUS_INCOMPLETE && player->HasAura(33280) || player->HasAura(55629))
				player->CompleteQuest(13183);

			if (player->GetQuestStatus(13181) == QUEST_STATUS_INCOMPLETE && player->HasAura(33280) || player->HasAura(55629))
				player->CompleteQuest(13181);

			if (player->HasAura(SPELL_LIEUTENANT))
				player->KilledMonsterCredit(129997);

			if (player->HasAura(SPELL_HONOR))
			player->ModifyHonorPoints(750);
			else if (player->HasAura(SPELL_GREAT_HONOR))
				player->ModifyHonorPoints(1500);
			else if (player->HasAura(SPELL_GREATER_HONOR))
				player->ModifyHonorPoints(2250);
			else if (player->HasAura(SPELL_GREATEST_HONOR))
				player->ModifyHonorPoints(3000);
        }
    }

    for (GuidSet::const_iterator itr = m_PlayersInWar[GetAttackerTeam()].begin(); itr != m_PlayersInWar[GetAttackerTeam()].end(); ++itr)
        if (Player* player = sObjectAccessor->FindPlayer(*itr))
		{
            player->CastSpell(player, SPELL_DEFEAT_REWARD, true);

			if (player->HasAura(SPELL_LIEUTENANT))
				player->KilledMonsterCredit(129997);

			if (player->HasAura(SPELL_HONOR))
				player->ModifyHonorPoints(312);
			else if (player->HasAura(SPELL_GREAT_HONOR))
				player->ModifyHonorPoints(625);
			else if (player->HasAura(SPELL_GREATER_HONOR))
				player->ModifyHonorPoints(937);
			else if (player->HasAura(SPELL_GREATEST_HONOR))
				player->ModifyHonorPoints(1250);
		}

    for (uint8 team = 0; team < 2; ++team)
    {
        for (GuidSet::const_iterator itr = m_PlayersInWar[team].begin(); itr != m_PlayersInWar[team].end(); ++itr)
            if (Player* player = sObjectAccessor->FindPlayer(*itr))
			{
                RemoveAurasFromPlayer(player);

				if (player->FindNearestCreature(30400, 1))
					player->InterruptNonMeleeSpells(true);

				if (player->FindNearestCreature(30499, 1))
					player->InterruptNonMeleeSpells(true);

				player->InterruptNonMeleeSpells(true);

				// Ä¿½ºÅÒ½ÃÀÛ	
				if (!player->IsAlive()) //À¯Á®°¡ »ì¾ÆÀÖÁö¾ÊÀ»¶§
				{
					if(player->GetTeamId() == GetAttackerTeam())
					{
						player->AddItem(43589,1);
						player->ResurrectPlayer(0.5f);
						player->SpawnCorpseBones();
						player->SaveToDB();

						if (player->HasAura(SPELL_HONOR))
							player->ModifyHonorPoints(1562);
						else if (player->HasAura(SPELL_GREAT_HONOR))
							player->ModifyHonorPoints(1875);
						else if (player->HasAura(SPELL_GREATER_HONOR))
							player->ModifyHonorPoints(2187);
						else if (player->HasAura(SPELL_GREATEST_HONOR))
							player->ModifyHonorPoints(2500);
						else
							player->ModifyHonorPoints(1250);

						if (player->GetTeamId() == TEAM_HORDE)
							player->TeleportTo(571, 5025.857422f, 3674.628906f, 362.737122f, 4.135169f);
						else
							player->TeleportTo(571, 5101.284f, 2186.564f, 373.549f, 3.812f);
					}else
					{
						if (player->HasAura(SPELL_HONOR))
							player->ModifyHonorPoints(3750);
						else if (player->HasAura(SPELL_GREAT_HONOR))
							player->ModifyHonorPoints(4500);
						else if (player->HasAura(SPELL_GREATER_HONOR))
							player->ModifyHonorPoints(5250);
						else if (player->HasAura(SPELL_GREATEST_HONOR))
							player->ModifyHonorPoints(6000);
						else
							player->ModifyHonorPoints(3000);

						player->AddItem(43589,3);
						player->ResurrectPlayer(0.5f);
						player->SpawnCorpseBones();
						player->SaveToDB();
						player->TeleportTo(571, 5345, 2842, 410, 3.14f);
					}
				}
				// Ä¿½ºÅÒÁ¾·á */
			}

        m_PlayersInWar[team].clear();

        for (GuidSet::const_iterator itr = m_vehicles[team].begin(); itr != m_vehicles[team].end(); ++itr)
            if (Creature* creature = GetCreature(*itr))
                if (creature->IsVehicle())
				{
					creature->DisappearAndDie();
                    creature->DespawnOrUnsummon();
					creature->RemoveFromWorld();
				}

        m_vehicles[team].clear();
    }

    if (!endByTimer)
    {
        for (uint8 team = 0; team < 2; ++team)
        {
            for (GuidSet::const_iterator itr = m_players[team].begin(); itr != m_players[team].end(); ++itr)
            {
                if (Player* player = sObjectAccessor->FindPlayer(*itr))
                {
                    player->RemoveAurasDueToSpell(m_DefenderTeam == TEAM_ALLIANCE ? SPELL_HORDE_CONTROL_PHASE_SHIFT : SPELL_ALLIANCE_CONTROL_PHASE_SHIFT, player->GetGUID());
                    player->AddAura(m_DefenderTeam == TEAM_HORDE ? SPELL_HORDE_CONTROL_PHASE_SHIFT : SPELL_ALLIANCE_CONTROL_PHASE_SHIFT, player);
                }
            }
        }
    }

    if (!endByTimer) // win alli/horde
        SendWarningToAllInZone((GetDefenderTeam() == TEAM_ALLIANCE) ? BATTLEFIELD_WG_TEXT_WIN_KEEP : BATTLEFIELD_WG_TEXT_WIN_KEEP + 1);
    else // defend alli/horde
        SendWarningToAllInZone((GetDefenderTeam() == TEAM_ALLIANCE) ? BATTLEFIELD_WG_TEXT_DEFEND_KEEP : BATTLEFIELD_WG_TEXT_DEFEND_KEEP + 1);
}

// *******************************************************
// ******************* Reward System *********************
// *******************************************************
void BattlefieldWG::DoCompleteOrIncrementAchievement(uint32 achievement, Player* player, uint8 /*incrementNumber*/)
{
    AchievementEntry const* achievementEntry = sAchievementMgr->GetAchievement(achievement);

    if (!achievementEntry)
        return;

    switch (achievement)
    {
        case ACHIEVEMENTS_WIN_WG_100:
        {
            // player->UpdateAchievementCriteria();
        }
        default:
        {
            if (player)
                player->CompletedAchievement(achievementEntry);
            break;
        }
    }

}

void BattlefieldWG::OnStartGrouping()
{
    SendWarningToAllInZone(BATTLEFIELD_WG_TEXT_WILL_START);
}

uint8 BattlefieldWG::GetSpiritGraveyardId(uint32 areaId) const
{
    switch (areaId)
    {
        case AREA_WINTERGRASP_FORTRESS:
            return BATTLEFIELD_WG_GY_KEEP;
        case AREA_THE_SUNKEN_RING:
            return BATTLEFIELD_WG_GY_WORKSHOP_NE;
        case AREA_THE_BROKEN_TEMPLATE:
            return BATTLEFIELD_WG_GY_WORKSHOP_NW;
        case AREA_WESTPARK_WORKSHOP:
            return BATTLEFIELD_WG_GY_WORKSHOP_SW;
        case AREA_EASTPARK_WORKSHOP:
            return BATTLEFIELD_WG_GY_WORKSHOP_SE;

        case AREA_WINTERGRASP:
            return BATTLEFIELD_WG_GY_ALLIANCE;
        case AREA_THE_CHILLED_QUAGMIRE:
            return BATTLEFIELD_WG_GY_HORDE; 
        default:
            TC_LOG_ERROR("bg.battlefield", "BattlefieldWG::GetSpiritGraveyardId: Unexpected Area Id %u", areaId);
            break;
    }

    return 0;
}

void BattlefieldWG::OnCreatureCreate(Creature* creature)
{
    // Accessing to db spawned creatures
    switch (creature->GetEntry())
    {
        case NPC_DWARVEN_SPIRIT_GUIDE:
        case NPC_TAUNKA_SPIRIT_GUIDE:
        {
            TeamId teamId = (creature->GetEntry() == NPC_DWARVEN_SPIRIT_GUIDE ? TEAM_ALLIANCE : TEAM_HORDE);
            uint8 graveyardId = GetSpiritGraveyardId(creature->GetAreaId());
            if (m_GraveyardList[graveyardId])
                m_GraveyardList[graveyardId]->SetSpirit(creature, teamId);
            break;
        }
    }

    // untested code - not sure if it is valid.
    if (IsWarTime())
    {
        switch (creature->GetEntry())
        {
            case NPC_WINTERGRASP_SIEGE_ENGINE_ALLIANCE:
            case NPC_WINTERGRASP_SIEGE_ENGINE_HORDE:
            case NPC_WINTERGRASP_CATAPULT:
            case NPC_WINTERGRASP_DEMOLISHER:
            {
                if (!creature->ToTempSummon() || !creature->ToTempSummon()->GetSummonerGUID() || !sObjectAccessor->FindPlayer(creature->ToTempSummon()->GetSummonerGUID()))
                {
                    creature->DespawnOrUnsummon();
                    return;
                }

                Player* creator = sObjectAccessor->FindPlayer(creature->ToTempSummon()->GetSummonerGUID());
                TeamId team = creator->GetTeamId();

                if (team == TEAM_HORDE)
                {
                    if (GetData(BATTLEFIELD_WG_DATA_VEHICLE_H) < GetData(BATTLEFIELD_WG_DATA_MAX_VEHICLE_H))
                    {
                        UpdateData(BATTLEFIELD_WG_DATA_VEHICLE_H, 1);
                        creature->AddAura(SPELL_HORDE_FLAG, creature);
                        m_vehicles[team].insert(creature->GetGUID());
                        UpdateVehicleCountWG();

						if(GetDefenderTeam() == TEAM_HORDE)
							creature->setFaction(WintergraspFaction[GetDefenderTeam()]);
                    }
                    else
                    {
                        creature->DespawnOrUnsummon();
                        return;
                    }
                }
                else
                {
                    if (GetData(BATTLEFIELD_WG_DATA_VEHICLE_A) < GetData(BATTLEFIELD_WG_DATA_MAX_VEHICLE_A))
                    {
                        UpdateData(BATTLEFIELD_WG_DATA_VEHICLE_A, 1);
                        creature->AddAura(SPELL_ALLIANCE_FLAG, creature);
                        m_vehicles[team].insert(creature->GetGUID());
                        UpdateVehicleCountWG();

						if(GetDefenderTeam() == TEAM_ALLIANCE)
							creature->setFaction(WintergraspFaction[GetDefenderTeam()]);
                    }
                    else
                    {
                        creature->DespawnOrUnsummon();
                        return;
                    }
                }

                creature->CastSpell(creator, SPELL_GRAB_PASSENGER, true);
                break;
            }
        }
    }
}

void BattlefieldWG::OnCreatureRemove(Creature* /*creature*/)
{
/* possibly can be used later
    if (IsWarTime())
    {
        switch (creature->GetEntry())
        {
            case NPC_WINTERGRASP_SIEGE_ENGINE_ALLIANCE:
            case NPC_WINTERGRASP_SIEGE_ENGINE_HORDE:
            case NPC_WINTERGRASP_CATAPULT:
            case NPC_WINTERGRASP_DEMOLISHER:
            {
                uint8 team;
                if (creature->getFaction() == WintergraspFaction[TEAM_ALLIANCE])
                    team = TEAM_ALLIANCE;
                else if (creature->getFaction() == WintergraspFaction[TEAM_HORDE])
                    team = TEAM_HORDE;
                else
                    return;

                m_vehicles[team].erase(creature->GetGUID());
                if (team == TEAM_HORDE)
                    UpdateData(BATTLEFIELD_WG_DATA_VEHICLE_H, -1);
                else
                    UpdateData(BATTLEFIELD_WG_DATA_VEHICLE_A, -1);
                UpdateVehicleCountWG();

                break;
            }
        }
    }*/
}

void BattlefieldWG::OnGameObjectCreate(GameObject* go)
{
    uint8 workshopId = 0;

    switch (go->GetEntry())
    {
        case GO_WINTERGRASP_FACTORY_BANNER_NE:
            workshopId = BATTLEFIELD_WG_WORKSHOP_NE;
            break;
        case GO_WINTERGRASP_FACTORY_BANNER_NW:
            workshopId = BATTLEFIELD_WG_WORKSHOP_NW;
            break;
        case GO_WINTERGRASP_FACTORY_BANNER_SE:
            workshopId = BATTLEFIELD_WG_WORKSHOP_SE;
            break;
        case GO_WINTERGRASP_FACTORY_BANNER_SW:
            workshopId = BATTLEFIELD_WG_WORKSHOP_SW;
            break;
        default:
            return;
    }

    for (Workshop::const_iterator itr = WorkshopsList.begin(); itr != WorkshopsList.end(); ++itr)
    {
        if (WGWorkshop* workshop = (*itr))
        {
            if (workshop->workshopId == workshopId)
            {
                WintergraspCapturePoint* capturePoint = new WintergraspCapturePoint(this, TEAM_NEUTRAL);

                capturePoint->SetCapturePointData(go);
                capturePoint->LinkToWorkshop(workshop);
                AddCapturePoint(capturePoint);
                break;
            }
        }
    }
}

// Called when player kill a unit in wg zone
void BattlefieldWG::HandleKill(Player* killer, Unit* victim)
{
    if (killer == victim)
        return;

    bool again = false;
    TeamId killerTeam = killer->GetTeamId();

    if (victim->GetTypeId() == TYPEID_PLAYER)
    {
        for (GuidSet::const_iterator itr = m_PlayersInWar[killerTeam].begin(); itr != m_PlayersInWar[killerTeam].end(); ++itr)
            if (Player* player = sObjectAccessor->FindPlayer(*itr))
                if (player->GetDistance2d(killer) < 40)
                    PromotePlayer(player);
        return;
    }
	else
        killer->UpdateAchievementCriteria(ACHIEVEMENT_CRITERIA_TYPE_SPECIAL_PVP_KILL, 1, 0, victim);

    for (GuidSet::const_iterator itr = KeepCreature[GetOtherTeam(killerTeam)].begin();
         itr != KeepCreature[GetOtherTeam(killerTeam)].end(); ++itr)
    {
        if (Creature* creature = GetCreature(*itr))
        {
            if (victim->GetEntry() == creature->GetEntry() && !again)
            {
                again = true;
                for (GuidSet::const_iterator iter = m_PlayersInWar[killerTeam].begin(); iter != m_PlayersInWar[killerTeam].end(); ++iter)
                    if (Player* player = sObjectAccessor->FindPlayer(*iter))
                        if (player->GetDistance2d(killer) < 40.0f)
                            PromotePlayer(player);
            }
        }
    }
    /// @todoRecent PvP activity worldstate
}

bool BattlefieldWG::FindAndRemoveVehicleFromList(Unit* vehicle)
{
    for (uint32 itr = 0; itr < 2; ++itr)
    {
        if (m_vehicles[itr].find(vehicle->GetGUID()) != m_vehicles[itr].end())
        {
            m_vehicles[itr].erase(vehicle->GetGUID());
            if (itr == TEAM_HORDE)
                UpdateData(BATTLEFIELD_WG_DATA_VEHICLE_H, -1);
            else
                UpdateData(BATTLEFIELD_WG_DATA_VEHICLE_A, -1);
            return true;
        }
    }
    return false;
}

void BattlefieldWG::OnUnitDeath(Unit* unit)
{
    if (IsWarTime())
        if (unit->IsVehicle())
            if (FindAndRemoveVehicleFromList(unit))
                UpdateVehicleCountWG();
}

// Update rank for player
void BattlefieldWG::PromotePlayer(Player* killer)
{
	if (!m_isActive)
        return;
    // Updating rank of player
    if (Aura* auraRecruit = killer->GetAura(SPELL_RECRUIT))
    {
        if (auraRecruit->GetStackAmount() >= 5)
        {
            killer->RemoveAura(SPELL_RECRUIT);
            killer->CastSpell(killer, SPELL_CORPORAL, true);
            SendWarningToPlayer(killer, BATTLEFIELD_WG_TEXT_FIRSTRANK);
        }
        else
            killer->CastSpell(killer, SPELL_RECRUIT, true);
    }
    else if (Aura* auraCorporal = killer->GetAura(SPELL_CORPORAL))
    {
        if (auraCorporal->GetStackAmount() >= 5)
        {
            killer->RemoveAura(SPELL_CORPORAL);
            killer->CastSpell(killer, SPELL_LIEUTENANT, true);
            SendWarningToPlayer(killer, BATTLEFIELD_WG_TEXT_SECONDRANK);
        }
        else
            killer->CastSpell(killer, SPELL_CORPORAL, true);
    }
}

void BattlefieldWG::RemoveAurasFromPlayer(Player* player)
{
    player->RemoveAurasDueToSpell(SPELL_RECRUIT);
    player->RemoveAurasDueToSpell(SPELL_CORPORAL);
    player->RemoveAurasDueToSpell(SPELL_LIEUTENANT);
    player->RemoveAurasDueToSpell(SPELL_TOWER_CONTROL);
    player->RemoveAurasDueToSpell(SPELL_SPIRITUAL_IMMUNITY);
    player->RemoveAurasDueToSpell(SPELL_TENACITY);
    player->RemoveAurasDueToSpell(SPELL_ESSENCE_OF_WINTERGRASP);
    player->RemoveAurasDueToSpell(SPELL_WINTERGRASP_RESTRICTED_FLIGHT_AREA);
	player->RemoveAurasDueToSpell(50494);
	player->RemoveAurasDueToSpell(SPELL_GREATEST_HONOR);
	player->RemoveAurasDueToSpell(SPELL_GREATER_HONOR);
	player->RemoveAurasDueToSpell(SPELL_GREAT_HONOR);
	player->RemoveAurasDueToSpell(SPELL_HONOR);
}

void BattlefieldWG::OnPlayerJoinWar(Player* player)
{
    RemoveAurasFromPlayer(player);

    player->CastSpell(player, SPELL_RECRUIT, true);

    if (!player->IsGameMaster())
    {
        if (player->GetTeamId() == GetDefenderTeam())
            player->TeleportTo(571, 5345, 2842, 410, 3.14f);
        else
        {
            if (player->GetTeamId() == TEAM_HORDE)
                player->TeleportTo(571, 5025.857422f, 3674.628906f, 362.737122f, 4.135169f);
            else
                player->TeleportTo(571, 5101.284f, 2186.564f, 373.549f, 3.812f);
        }
    }

    if (player->GetTeamId() == GetAttackerTeam())
    {
        if (GetData(BATTLEFIELD_WG_DATA_BROKEN_TOWER_ATT) < 3)
            player->SetAuraStack(SPELL_TOWER_CONTROL, player, 3 - GetData(BATTLEFIELD_WG_DATA_BROKEN_TOWER_ATT));
    }
    else
    {
        if (GetData(BATTLEFIELD_WG_DATA_BROKEN_TOWER_ATT) > 0)
           player->SetAuraStack(SPELL_TOWER_CONTROL, player, GetData(BATTLEFIELD_WG_DATA_BROKEN_TOWER_ATT));
    }
    SendInitWorldStatesTo(player);
}

void BattlefieldWG::OnPlayerLeaveWar(Player* player)
{
    // Remove all aura from WG /// @todo false we can go out of this zone on retail and keep Rank buff, remove on end of WG
    if (!player->GetSession()->PlayerLogout())
    {
        if (Creature* vehicle = player->GetVehicleCreatureBase())   // Remove vehicle of player if he go out.
            vehicle->DespawnOrUnsummon();

        RemoveAurasFromPlayer(player);
    }

    player->RemoveAurasDueToSpell(SPELL_HORDE_CONTROLS_FACTORY_PHASE_SHIFT);
    player->RemoveAurasDueToSpell(SPELL_ALLIANCE_CONTROLS_FACTORY_PHASE_SHIFT);
    player->RemoveAurasDueToSpell(SPELL_HORDE_CONTROL_PHASE_SHIFT);
    player->RemoveAurasDueToSpell(SPELL_ALLIANCE_CONTROL_PHASE_SHIFT);
	player->RemoveAurasDueToSpell(58729);
	player->RemoveAurasDueToSpell(SPELL_RECRUIT);
    player->RemoveAurasDueToSpell(SPELL_CORPORAL);
    player->RemoveAurasDueToSpell(SPELL_LIEUTENANT);
    player->RemoveAurasDueToSpell(SPELL_TOWER_CONTROL);
    player->RemoveAurasDueToSpell(SPELL_SPIRITUAL_IMMUNITY);
    player->RemoveAurasDueToSpell(SPELL_TENACITY);
    player->RemoveAurasDueToSpell(SPELL_ESSENCE_OF_WINTERGRASP);
    player->RemoveAurasDueToSpell(SPELL_WINTERGRASP_RESTRICTED_FLIGHT_AREA);
	player->RemoveAurasDueToSpell(50494);
}

void BattlefieldWG::OnPlayerLeaveZone(Player* player)
{
    if (!m_isActive)
        RemoveAurasFromPlayer(player);

    player->RemoveAurasDueToSpell(SPELL_HORDE_CONTROLS_FACTORY_PHASE_SHIFT);
    player->RemoveAurasDueToSpell(SPELL_ALLIANCE_CONTROLS_FACTORY_PHASE_SHIFT);
    player->RemoveAurasDueToSpell(SPELL_HORDE_CONTROL_PHASE_SHIFT);
    player->RemoveAurasDueToSpell(SPELL_ALLIANCE_CONTROL_PHASE_SHIFT);
	player->RemoveAurasDueToSpell(58729);
	player->RemoveAurasDueToSpell(SPELL_RECRUIT);
    player->RemoveAurasDueToSpell(SPELL_CORPORAL);
    player->RemoveAurasDueToSpell(SPELL_LIEUTENANT);
    player->RemoveAurasDueToSpell(SPELL_TOWER_CONTROL);
    player->RemoveAurasDueToSpell(SPELL_SPIRITUAL_IMMUNITY);
    player->RemoveAurasDueToSpell(SPELL_TENACITY);
    player->RemoveAurasDueToSpell(SPELL_ESSENCE_OF_WINTERGRASP);
    player->RemoveAurasDueToSpell(SPELL_WINTERGRASP_RESTRICTED_FLIGHT_AREA);
	player->RemoveAurasDueToSpell(50494);
}

void BattlefieldWG::OnPlayerEnterZone(Player* player)
{
    if (!m_isActive)
        RemoveAurasFromPlayer(player);

    player->AddAura(m_DefenderTeam == TEAM_HORDE ? SPELL_HORDE_CONTROL_PHASE_SHIFT : SPELL_ALLIANCE_CONTROL_PHASE_SHIFT, player);
    // Send worldstate to player
    SendInitWorldStatesTo(player);
}

uint32 BattlefieldWG::GetData(uint32 data) const
{
    switch (data)
    {
        // Used to determine when the phasing spells must be cast
        // See: SpellArea::IsFitToRequirements
        case AREA_THE_SUNKEN_RING:
        case AREA_THE_BROKEN_TEMPLATE:
        case AREA_WESTPARK_WORKSHOP:
        case AREA_EASTPARK_WORKSHOP:
            // Graveyards and Workshops are controlled by the same team.
            if (BfGraveyard const* graveyard = GetGraveyardById(GetSpiritGraveyardId(data)))
                return graveyard->GetControlTeamId();
            break;
        default:
            break;
    }

    return Battlefield::GetData(data);
}


void BattlefieldWG::FillInitialWorldStates(WorldPacket& data)
{
    data << uint32(BATTLEFIELD_WG_WORLD_STATE_ATTACKER) << uint32(GetAttackerTeam());
    data << uint32(BATTLEFIELD_WG_WORLD_STATE_DEFENDER) << uint32(GetDefenderTeam());
    data << uint32(BATTLEFIELD_WG_WORLD_STATE_ACTIVE) << uint32(IsWarTime() ? 0 : 1); // Note: cleanup these two, their names look awkward
    data << uint32(BATTLEFIELD_WG_WORLD_STATE_SHOW_WORLDSTATE) << uint32(IsWarTime() ? 1 : 0);

    for (uint32 i = 0; i < 2; ++i)
        data << ClockWorldState[i] << uint32(time(NULL) + (m_Timer / 1000));

    data << uint32(BATTLEFIELD_WG_WORLD_STATE_VEHICLE_H) << uint32(GetData(BATTLEFIELD_WG_DATA_VEHICLE_H));
    data << uint32(BATTLEFIELD_WG_WORLD_STATE_MAX_VEHICLE_H) << GetData(BATTLEFIELD_WG_DATA_MAX_VEHICLE_H);
    data << uint32(BATTLEFIELD_WG_WORLD_STATE_VEHICLE_A) << uint32(GetData(BATTLEFIELD_WG_DATA_VEHICLE_A));
    data << uint32(BATTLEFIELD_WG_WORLD_STATE_MAX_VEHICLE_A) << GetData(BATTLEFIELD_WG_DATA_MAX_VEHICLE_A);

    for (GameObjectBuilding::const_iterator itr = BuildingsInZone.begin(); itr != BuildingsInZone.end(); ++itr)
        data << (*itr)->m_WorldState << (*itr)->m_State;

    for (Workshop::const_iterator itr = WorkshopsList.begin(); itr != WorkshopsList.end(); ++itr)
        if (*itr)
            data << WorkshopsData[(*itr)->workshopId].worldstate << (*itr)->state;
}

void BattlefieldWG::SendInitWorldStatesTo(Player* player)
{
    WorldPacket data(SMSG_INIT_WORLD_STATES, (4 + 4 + 4 + 2 + (BuildingsInZone.size() * 8) + (WorkshopsList.size() * 8)));

    data << uint32(m_MapId);
    data << uint32(m_ZoneId);
    data << uint32(0);
    data << uint16(10 + BuildingsInZone.size() + WorkshopsList.size()); // Number of fields

    FillInitialWorldStates(data);

    player->GetSession()->SendPacket(&data);
}

void BattlefieldWG::SendInitWorldStatesToAll()
{
    for (uint8 team = 0; team < 2; team++)
        for (GuidSet::iterator itr = m_players[team].begin(); itr != m_players[team].end(); ++itr)
            if (Player* player = sObjectAccessor->FindPlayer(*itr))
                SendInitWorldStatesTo(player);
}

void BattlefieldWG::BrokenWallOrTower(TeamId /*team*/)
{
// might be some use for this in the future. old code commented out below. KL
/*    if (team == GetDefenderTeam())
    {
        for (GuidSet::const_iterator itr = m_PlayersInWar[GetAttackerTeam()].begin(); itr != m_PlayersInWar[GetAttackerTeam()].end(); ++itr)
        {
            if (Player* player = sObjectAccessor->FindPlayer(*itr))
                IncrementQuest(player, WGQuest[player->GetTeamId()][2], true);
        }
    }*/
}

// Called when a tower is broke
void BattlefieldWG::UpdatedDestroyedTowerCount(TeamId team)
{
    // Destroy an attack tower
    if (team == GetAttackerTeam())
    {
        // Update counter
        UpdateData(BATTLEFIELD_WG_DATA_DAMAGED_TOWER_ATT, -1);
        UpdateData(BATTLEFIELD_WG_DATA_BROKEN_TOWER_ATT, 1);

        // Remove buff stack on attackers
        for (GuidSet::const_iterator itr = m_PlayersInWar[GetAttackerTeam()].begin(); itr != m_PlayersInWar[GetAttackerTeam()].end(); ++itr)
            if (Player* player = sObjectAccessor->FindPlayer(*itr))
                player->RemoveAuraFromStack(SPELL_TOWER_CONTROL);

        // Add buff stack to defenders
        for (GuidSet::const_iterator itr = m_PlayersInWar[GetDefenderTeam()].begin(); itr != m_PlayersInWar[GetDefenderTeam()].end(); ++itr)
            if (Player* player = sObjectAccessor->FindPlayer(*itr))
            {
                player->CastSpell(player, SPELL_TOWER_CONTROL, true);
                DoCompleteOrIncrementAchievement(ACHIEVEMENTS_WG_TOWER_DESTROY, player);
            }

        // If all three south towers are destroyed (ie. all attack towers), remove ten minutes from battle time
        if (GetData(BATTLEFIELD_WG_DATA_BROKEN_TOWER_ATT) == 3)
        {
            if (int32(m_Timer - 600000) < 0)
                m_Timer = 0;
            else
                m_Timer -= 600000;
            SendInitWorldStatesToAll();
        }
    }
    else
    {
        UpdateData(BATTLEFIELD_WG_DATA_DAMAGED_TOWER_DEF, -1);
        UpdateData(BATTLEFIELD_WG_DATA_BROKEN_TOWER_DEF, 1);
    }
}

void BattlefieldWG::ProcessEvent(WorldObject* obj, uint32 eventId)
{
    if (!obj || !IsWarTime())
        return;

    // We handle only gameobjects here
    GameObject* go = obj->ToGameObject();
    if (!go)
        return;

    // On click on titan relic
    if (go->GetEntry() == GO_WINTERGRASP_TITAN_S_RELIC)
    {
        if (CanInteractWithRelic())
            EndBattle(false);
        else if (GameObject* relic = GetRelic())
            relic->SetRespawnTime(RESPAWN_IMMEDIATELY);
    }

    // if destroy or damage event, search the wall/tower and update worldstate/send warning message
    for (GameObjectBuilding::const_iterator itr = BuildingsInZone.begin(); itr != BuildingsInZone.end(); ++itr)
    {
        if (GameObject* build = GetGameObject((*itr)->m_BuildGUID))
        {
            if (go->GetEntry() == build->GetEntry())
            {
                if (build->GetGOInfo()->building.damagedEvent == eventId)
                    (*itr)->Damaged();

                if (build->GetGOInfo()->building.destroyedEvent == eventId)
				{
                    (*itr)->Destroyed();

					// Add Support of Quests Toppling the Towers & Southern Sabotage
					if (go->GetEntry()==190356 || go->GetEntry()==190357 || go->GetEntry()==190358)
					{
						for (GuidSet::const_iterator itr = m_PlayersInWar[GetDefenderTeam()].begin(); itr != m_PlayersInWar[GetDefenderTeam()].end(); ++itr)
							if (Player* player = sObjectAccessor->FindPlayer(*itr))
								player->RewardPlayerAndGroupAtEvent(35074, go);
					}
				}
                break;
            }
        }
    }
}

// Called when a tower is damaged, used for honor reward calcul
void BattlefieldWG::UpdateDamagedTowerCount(TeamId team)
{
    if (team == GetAttackerTeam())
        UpdateData(BATTLEFIELD_WG_DATA_DAMAGED_TOWER_ATT, 1);
    else
        UpdateData(BATTLEFIELD_WG_DATA_DAMAGED_TOWER_DEF, 1);
}

// Update vehicle count WorldState to player
void BattlefieldWG::UpdateVehicleCountWG()
{
    SendUpdateWorldState(BATTLEFIELD_WG_WORLD_STATE_VEHICLE_H,     GetData(BATTLEFIELD_WG_DATA_VEHICLE_H));
    SendUpdateWorldState(BATTLEFIELD_WG_WORLD_STATE_MAX_VEHICLE_H, GetData(BATTLEFIELD_WG_DATA_MAX_VEHICLE_H));
    SendUpdateWorldState(BATTLEFIELD_WG_WORLD_STATE_VEHICLE_A,     GetData(BATTLEFIELD_WG_DATA_VEHICLE_A));
    SendUpdateWorldState(BATTLEFIELD_WG_WORLD_STATE_MAX_VEHICLE_A, GetData(BATTLEFIELD_WG_DATA_MAX_VEHICLE_A));
}

void BattlefieldWG::UpdateTenacity()
{
    TeamId team = TEAM_NEUTRAL;
    uint32 alliancePlayers = m_PlayersInWar[TEAM_ALLIANCE].size();
    uint32 hordePlayers = m_PlayersInWar[TEAM_HORDE].size();
    int32 newStack = 0;

    if (alliancePlayers && hordePlayers)
    {
        if (alliancePlayers < hordePlayers)
            newStack = int32((float(hordePlayers / alliancePlayers) - 1) * 4);  // positive, should cast on alliance
        else if (alliancePlayers > hordePlayers)
            newStack = int32((1 - float(alliancePlayers / hordePlayers)) * 4);  // negative, should cast on horde
    }

    if (newStack == int32(m_tenacityStack))
        return;

    if (m_tenacityStack > 0 && newStack <= 0)               // old buff was on alliance
        team = TEAM_ALLIANCE;
    else if (newStack >= 0)                                 // old buff was on horde
        team = TEAM_HORDE;

    m_tenacityStack = newStack;
    // Remove old buff
    if (team != TEAM_NEUTRAL)
    {
        for (GuidSet::const_iterator itr = m_players[team].begin(); itr != m_players[team].end(); ++itr)
            if (Player* player = sObjectAccessor->FindPlayer(*itr))
                player->RemoveAurasDueToSpell(SPELL_TENACITY);

        for (GuidSet::const_iterator itr = m_vehicles[team].begin(); itr != m_vehicles[team].end(); ++itr)
            if (Creature* creature = GetCreature(*itr))
                creature->RemoveAurasDueToSpell(SPELL_TENACITY_VEHICLE);
    }

    // Apply new buff
    if (newStack)
    {
		uint32 playercounf = abs(alliancePlayers - hordePlayers);
		if (playercounf >= 5 && playercounf < 9)
			newStack = 8;

		if (playercounf >= 10 && playercounf < 15)
			newStack = 12;

		if (playercounf >= 15)
			newStack = 16;

		if (newStack > 16)
            newStack = 16;

		int32 crenewStack = newStack / 2;

		if (alliancePlayers < hordePlayers)
		{
			for (GuidSet::const_iterator itr = m_PlayersInWar[TEAM_ALLIANCE].begin(); itr != m_PlayersInWar[TEAM_ALLIANCE].end(); ++itr)
				if (Player* player = sObjectAccessor->FindPlayer(*itr))
					player->SetAuraStack(SPELL_TENACITY, player, newStack);

			for (GuidSet::const_iterator itr = m_vehicles[TEAM_ALLIANCE].begin(); itr != m_vehicles[TEAM_ALLIANCE].end(); ++itr)
				if (Creature* creature = GetCreature(*itr))
					creature->SetAuraStack(SPELL_TENACITY_VEHICLE, creature, crenewStack);
		}

		if (alliancePlayers > hordePlayers)
		{
			for (GuidSet::const_iterator itr = m_PlayersInWar[TEAM_HORDE].begin(); itr != m_PlayersInWar[TEAM_HORDE].end(); ++itr)
				if (Player* player = sObjectAccessor->FindPlayer(*itr))
					player->SetAuraStack(SPELL_TENACITY, player, newStack);

			for (GuidSet::const_iterator itr = m_vehicles[TEAM_HORDE].begin(); itr != m_vehicles[TEAM_HORDE].end(); ++itr)
				if (Creature* creature = GetCreature(*itr))
					creature->SetAuraStack(SPELL_TENACITY_VEHICLE, creature, newStack);
		}

/*        team = newStack > 0 ? TEAM_ALLIANCE : TEAM_HORDE;

        if (newStack < 0)
            newStack = -newStack;
        if (newStack > 16)
            newStack = 16;

        uint32 buff_honor = SPELL_GREATEST_HONOR;
        if (newStack < 15)
            buff_honor = SPELL_GREATER_HONOR;
        if (newStack < 10)
            buff_honor = SPELL_GREAT_HONOR;
        if (newStack < 5)
            buff_honor = 0;

        for (GuidSet::const_iterator itr = m_PlayersInWar[team].begin(); itr != m_PlayersInWar[team].end(); ++itr)
            if (Player* player = sObjectAccessor->FindPlayer(*itr))
                player->SetAuraStack(SPELL_TENACITY, player, newStack);

        for (GuidSet::const_iterator itr = m_vehicles[team].begin(); itr != m_vehicles[team].end(); ++itr)
            if (Creature* creature = GetCreature(*itr))
                creature->SetAuraStack(SPELL_TENACITY_VEHICLE, creature, newStack);
*/
		uint32 buff_honor = SPELL_GREATEST_HONOR;
        if (newStack < 15)
            buff_honor = SPELL_GREATER_HONOR;
        if (newStack < 10)
            buff_honor = SPELL_GREAT_HONOR;
        if (newStack < 5)
            buff_honor = 0;

        if (buff_honor != 0)
	      {
            for (GuidSet::const_iterator itr = m_PlayersInWar[team].begin(); itr != m_PlayersInWar[team].end(); ++itr)
                if (Player* player = sObjectAccessor->FindPlayer(*itr))
				{
					player->RemoveAurasDueToSpell(SPELL_GREATEST_HONOR);
					player->RemoveAurasDueToSpell(SPELL_GREATER_HONOR);
					player->RemoveAurasDueToSpell(SPELL_GREAT_HONOR);
                    player->CastSpell(player, buff_honor, true);
				}
            for (GuidSet::const_iterator itr = m_vehicles[team].begin(); itr != m_vehicles[team].end(); ++itr)
                if (Creature* creature = GetCreature(*itr))
				{
					creature->RemoveAurasDueToSpell(SPELL_GREATEST_HONOR);
					creature->RemoveAurasDueToSpell(SPELL_GREATER_HONOR);
					creature->RemoveAurasDueToSpell(SPELL_GREAT_HONOR);
                    creature->CastSpell(creature, buff_honor, true);
				}
        }
   }
}

WintergraspCapturePoint::WintergraspCapturePoint(BattlefieldWG* battlefield, TeamId teamInControl) : BfCapturePoint(battlefield)
{
    m_Bf = battlefield;
    m_team = teamInControl;
    m_Workshop = NULL;
}

void WintergraspCapturePoint::ChangeTeam(TeamId /*oldTeam*/)
{
    ASSERT(m_Workshop);
    m_Workshop->GiveControlTo(m_team, false);
}

BfGraveyardWG::BfGraveyardWG(BattlefieldWG* battlefield) : BfGraveyard(battlefield)
{
    m_Bf = battlefield;
    m_GossipTextId = 0;
}
