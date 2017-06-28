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

#ifndef BATTLEFIELD_WG_
#define BATTLEFIELD_WG_

#include "ObjectAccessor.h"
#include "WorldPacket.h"
#include "Battlefield.h"
#include "World.h"

class Group;
class BattlefieldWG;
class WintergraspCapturePoint;

struct BfWGGameObjectBuilding;
struct WGWorkshop;

typedef std::set<GameObject*> GameObjectSet;
typedef std::set<BfWGGameObjectBuilding*> GameObjectBuilding;
typedef std::set<WGWorkshop*> Workshop;
typedef std::set<Group*> GroupSet;
//typedef std::set<WintergraspCapturePoint *> CapturePointSet; unused ?

enum WintergrastData
{
    BATTLEFIELD_WG_ZONEID                        = 4197,             // Wintergrasp
    BATTLEFIELD_WG_MAPID                         = 571               // Northrend
};

enum WintergraspSpells
{
    // Wartime auras
    SPELL_RECRUIT                               = 37795,
    SPELL_CORPORAL                              = 33280,
    SPELL_LIEUTENANT                            = 55629,
    SPELL_TENACITY                              = 58549,
    SPELL_TENACITY_VEHICLE                      = 59911,
    SPELL_TOWER_CONTROL                         = 62064,
    SPELL_SPIRITUAL_IMMUNITY                    = 58729,
	SPELL_HONOR                           = 58553,
    SPELL_GREAT_HONOR                           = 58555,
    SPELL_GREATER_HONOR                         = 58556,
    SPELL_GREATEST_HONOR                        = 58557,
    SPELL_ALLIANCE_FLAG                         = 14268,
    SPELL_HORDE_FLAG                            = 14267,
    SPELL_GRAB_PASSENGER                        = 61178,

    // Reward spells
    SPELL_VICTORY_REWARD                        = 56902,
    SPELL_DEFEAT_REWARD                         = 58494,
    SPELL_DAMAGED_TOWER                         = 59135,
    SPELL_DESTROYED_TOWER                       = 59136,
    SPELL_DAMAGED_BUILDING                      = 59201,
    SPELL_INTACT_BUILDING                       = 59203,

    SPELL_TELEPORT_BRIDGE                       = 59096,
    SPELL_TELEPORT_FORTRESS                     = 60035,

    SPELL_TELEPORT_DALARAN                      = 53360,
    SPELL_VICTORY_AURA                          = 60044,

    // Other spells
    SPELL_WINTERGRASP_WATER                     = 36444,
    SPELL_ESSENCE_OF_WINTERGRASP                = 58045,
    SPELL_WINTERGRASP_RESTRICTED_FLIGHT_AREA    = 58730,

    // Phasing spells
    SPELL_HORDE_CONTROLS_FACTORY_PHASE_SHIFT    = 56618, // ADDS PHASE 16
    SPELL_ALLIANCE_CONTROLS_FACTORY_PHASE_SHIFT = 56617, // ADDS PHASE 32

    SPELL_HORDE_CONTROL_PHASE_SHIFT             = 55773, // ADDS PHASE 64
    SPELL_ALLIANCE_CONTROL_PHASE_SHIFT          = 55774  // ADDS PHASE 128
};

enum WintergraspData
{
    BATTLEFIELD_WG_DATA_DAMAGED_TOWER_DEF,
    BATTLEFIELD_WG_DATA_BROKEN_TOWER_DEF,
    BATTLEFIELD_WG_DATA_DAMAGED_TOWER_ATT,
    BATTLEFIELD_WG_DATA_BROKEN_TOWER_ATT,
    BATTLEFIELD_WG_DATA_MAX_VEHICLE_A,
    BATTLEFIELD_WG_DATA_MAX_VEHICLE_H,
    BATTLEFIELD_WG_DATA_VEHICLE_A,
    BATTLEFIELD_WG_DATA_VEHICLE_H,
    BATTLEFIELD_WG_DATA_MAX
};

enum WintergraspAchievements
{
    ACHIEVEMENTS_WIN_WG                          = 1717,
    ACHIEVEMENTS_WIN_WG_100                      = 1718, /// @todo: Has to be implemented
    ACHIEVEMENTS_WG_GNOMESLAUGHTER               = 1723, /// @todo: Has to be implemented
    ACHIEVEMENTS_WG_TOWER_DESTROY                = 1727,
    ACHIEVEMENTS_DESTRUCTION_DERBY_A             = 1737, /// @todo: Has to be implemented
    ACHIEVEMENTS_WG_TOWER_CANNON_KILL            = 1751, /// @todo: Has to be implemented
    ACHIEVEMENTS_WG_MASTER_A                     = 1752, /// @todo: Has to be implemented
    ACHIEVEMENTS_WIN_WG_TIMER_10                 = 1755,
    ACHIEVEMENTS_STONE_KEEPER_50                 = 2085, /// @todo: Has to be implemented
    ACHIEVEMENTS_STONE_KEEPER_100                = 2086, /// @todo: Has to be implemented
    ACHIEVEMENTS_STONE_KEEPER_250                = 2087, /// @todo: Has to be implemented
    ACHIEVEMENTS_STONE_KEEPER_500                = 2088, /// @todo: Has to be implemented
    ACHIEVEMENTS_STONE_KEEPER_1000               = 2089, /// @todo: Has to be implemented
    ACHIEVEMENTS_WG_RANGER                       = 2199, /// @todo: Has to be implemented
    ACHIEVEMENTS_DESTRUCTION_DERBY_H             = 2476, /// @todo: Has to be implemented
    ACHIEVEMENTS_WG_MASTER_H                     = 2776  /// @todo: Has to be implemented
};

enum WintergraspWorldStates
{
    BATTLEFIELD_WG_WORLD_STATE_VEHICLE_H         = 3490,
    BATTLEFIELD_WG_WORLD_STATE_MAX_VEHICLE_H     = 3491,
    BATTLEFIELD_WG_WORLD_STATE_VEHICLE_A         = 3680,
    BATTLEFIELD_WG_WORLD_STATE_MAX_VEHICLE_A     = 3681,
    BATTLEFIELD_WG_WORLD_STATE_ACTIVE            = 3801,
    BATTLEFIELD_WG_WORLD_STATE_DEFENDER          = 3802,
    BATTLEFIELD_WG_WORLD_STATE_ATTACKER          = 3803,
    BATTLEFIELD_WG_WORLD_STATE_SHOW_WORLDSTATE   = 3710
};

enum WintergraspAreaIds
{
    AREA_WINTERGRASP_FORTRESS       = 4575,
    AREA_THE_SUNKEN_RING            = 4538,
    AREA_THE_BROKEN_TEMPLATE        = 4539,
    AREA_WESTPARK_WORKSHOP          = 4611,
    AREA_EASTPARK_WORKSHOP          = 4612,
    AREA_WINTERGRASP                = 4197,
    AREA_THE_CHILLED_QUAGMIRE       = 4589
};

/*#########################
 *####### Graveyards ######
 *#########################*/

class BfGraveyardWG : public BfGraveyard
{
    public:
        BfGraveyardWG(BattlefieldWG* Bf);

        void SetTextId(int32 textid) { m_GossipTextId = textid; }
        int32 GetTextId() { return m_GossipTextId; }
    protected:
        int32 m_GossipTextId;
};

enum WGGraveyardId
{
    BATTLEFIELD_WG_GY_WORKSHOP_NE,
    BATTLEFIELD_WG_GY_WORKSHOP_NW,
    BATTLEFIELD_WG_GY_WORKSHOP_SE,
    BATTLEFIELD_WG_GY_WORKSHOP_SW,
    BATTLEFIELD_WG_GY_KEEP,
    BATTLEFIELD_WG_GY_HORDE,
    BATTLEFIELD_WG_GY_ALLIANCE,
    BATTLEFIELD_WG_GRAVEYARD_MAX
};

enum WGGossipText
{
    BATTLEFIELD_WG_GOSSIPTEXT_GY_NE              = 20071,
    BATTLEFIELD_WG_GOSSIPTEXT_GY_NW              = 20072,
    BATTLEFIELD_WG_GOSSIPTEXT_GY_SE              = 20074,
    BATTLEFIELD_WG_GOSSIPTEXT_GY_SW              = 20073,
    BATTLEFIELD_WG_GOSSIPTEXT_GY_KEEP            = 20070,
    BATTLEFIELD_WG_GOSSIPTEXT_GY_HORDE           = 20075,
    BATTLEFIELD_WG_GOSSIPTEXT_GY_ALLIANCE        = 20076
};

enum WintergraspNpcs
{
    BATTLEFIELD_WG_NPC_GUARD_H                      = 30739,
    BATTLEFIELD_WG_NPC_GUARD_A                      = 30740,
    BATTLEFIELD_WG_NPC_STALKER                      = 15214,
	BATTLEFILED_WG_NPC_WORKSHOP_H					= 30400,
	BATTLEFILED_WG_NPC_WORKSHOP_A					= 30499,

    BATTLEFIELD_WG_NPC_VIERON_BLAZEFEATHER          = 31102,
    BATTLEFIELD_WG_NPC_STONE_GUARD_MUKAR            = 32296, // <WINTERGRASP QUARTERMASTER>
    BATTLEFIELD_WG_NPC_HOODOO_MASTER_FU_JIN         = 31101, // <MASTER HEXXER>
    BATTLEFIELD_WG_NPC_CHAMPION_ROS_SLAI            = 39173, // <WINTERGRASP QUARTERMASTER>
    BATTLEFIELD_WG_NPC_COMMANDER_DARDOSH            = 31091,
    BATTLEFIELD_WG_NPC_TACTICAL_OFFICER_KILRATH     = 31151,
    BATTLEFIELD_WG_NPC_SIEGESMITH_STRONGHOOF        = 31106,
    BATTLEFIELD_WG_NPC_PRIMALIST_MULFORT            = 31053,
    BATTLEFIELD_WG_NPC_LIEUTENANT_MURP              = 31107,

    BATTLEFIELD_WG_NPC_BOWYER_RANDOLPH              = 31052,
    BATTLEFIELD_WG_NPC_KNIGHT_DAMERON               = 32294, // <WINTERGRASP QUARTERMASTER>
    BATTLEFIELD_WG_NPC_SORCERESS_KAYLANA            = 31051, // <ENCHANTRESS>
    BATTLEFIELD_WG_NPC_MARSHAL_MAGRUDER             = 39172, // <WINTERGRASP QUARTERMASTER>
    BATTLEFIELD_WG_NPC_COMMANDER_ZANNETH            = 31036,
    BATTLEFIELD_WG_NPC_TACTICAL_OFFICER_AHBRAMIS    = 31153,
    BATTLEFIELD_WG_NPC_SIEGE_MASTER_STOUTHANDLE     = 31108,
    BATTLEFIELD_WG_NPC_ANCHORITE_TESSA              = 31054,
    BATTLEFIELD_WG_NPC_SENIOR_DEMOLITIONIST_LEGOSO  = 31109,
	Multivendor_h				= 7000006,
	Multivendor_a				= 7000005,


    NPC_TAUNKA_SPIRIT_GUIDE                         = 31841, // Horde spirit guide for Wintergrasp
    NPC_DWARVEN_SPIRIT_GUIDE                        = 31842, // Alliance spirit guide for Wintergrasp

    NPC_WINTERGRASP_SIEGE_ENGINE_ALLIANCE           = 28312,
    NPC_WINTERGRASP_SIEGE_ENGINE_HORDE              = 32627,
    NPC_WINTERGRASP_CATAPULT                        = 27881,
    NPC_WINTERGRASP_DEMOLISHER                      = 28094,
    NPC_WINTERGRASP_TOWER_CANNON                    = 28366,
	NPC_WINTERGRASP_GLACIAL_SPIRIT					= 30846,
	NPC_WINTERGRASP_GLACIAL_WR						= 30877,
	NPC_WINTERGRASP_GLACIAL_CEE						= 30849,
	NPC_WINTERGRASP_GLACIAL_ER						= 30876,
};

struct BfWGCoordGY
{
    float x;
    float y;
    float z;
    float o;
    uint32 gyid;
    uint8 type;
    int32 textid;              // for gossip menu
    TeamId startcontrol;
};

uint32 const WGQuest[2][6] =
{
    { 13186, 13181, 13222, 13538, 13177, 13179 },
    { 13185, 13183, 13223, 13539, 13178, 13180 },
};
// 7 in sql, 7 in header
BfWGCoordGY const WGGraveYard[BATTLEFIELD_WG_GRAVEYARD_MAX] =
{
    { 5104.750f, 2300.940f, 368.579f, 0.733038f, 1329, BATTLEFIELD_WG_GY_WORKSHOP_NE, BATTLEFIELD_WG_GOSSIPTEXT_GY_NE, TEAM_NEUTRAL },
    { 5099.120f, 3466.036f, 368.484f, 5.317802f, 1330, BATTLEFIELD_WG_GY_WORKSHOP_NW, BATTLEFIELD_WG_GOSSIPTEXT_GY_NW, TEAM_NEUTRAL },
    { 4314.648f, 2408.522f, 392.642f, 6.268125f, 1333, BATTLEFIELD_WG_GY_WORKSHOP_SE, BATTLEFIELD_WG_GOSSIPTEXT_GY_SE, TEAM_NEUTRAL },
    { 4331.716f, 3235.695f, 390.251f, 0.008500f, 1334, BATTLEFIELD_WG_GY_WORKSHOP_SW, BATTLEFIELD_WG_GOSSIPTEXT_GY_SW, TEAM_NEUTRAL },
    { 5537.986f, 2897.493f, 517.057f, 4.819249f, 1285, BATTLEFIELD_WG_GY_KEEP, BATTLEFIELD_WG_GOSSIPTEXT_GY_KEEP, TEAM_NEUTRAL },
    { 5032.454f, 3711.382f, 372.468f, 3.971623f, 1331, BATTLEFIELD_WG_GY_HORDE, BATTLEFIELD_WG_GOSSIPTEXT_GY_HORDE, TEAM_HORDE },
    { 5140.790f, 2179.120f, 390.950f, 1.972220f, 1332, BATTLEFIELD_WG_GY_ALLIANCE, BATTLEFIELD_WG_GOSSIPTEXT_GY_ALLIANCE, TEAM_ALLIANCE },
};

/* ######################### *
 *  WintergraspCapturePoint  *
 * ######################### */

class WintergraspCapturePoint : public BfCapturePoint
{
    public:
        WintergraspCapturePoint(BattlefieldWG* battlefield, TeamId teamInControl);

        void LinkToWorkshop(WGWorkshop* workshop) { m_Workshop = workshop; }

        void ChangeTeam(TeamId oldteam);
        TeamId GetTeam() const { return m_team; }

    protected:
        WGWorkshop* m_Workshop;
};

/* ######################### *
 * WinterGrasp Battlefield   *
 * ######################### */

class BattlefieldWG : public Battlefield
{
    public:
        ~BattlefieldWG();
        /**
         * \brief Called when the battle start
         * - Spawn relic and turret
         * - Rebuild tower and wall
         * - Invite player to war
         */
        void OnBattleStart();

        /**
         * \brief Called when battle end
         * - Remove relic and turret
         * - Change banner/npc in keep if it needed
         * - Saving battlestate
         * - Reward honor/mark to player
         * - Remove vehicle
         * \param endByTimer : true if battle ended when timer is at 00:00, false if battle ended by clicking on relic
         */
        void OnBattleEnd(bool endByTimer);

        /**
         * \brief Called when grouping starts (15 minutes before battlestart)
         * - Invite all player in zone to join queue
         */
        void OnStartGrouping();

        /**
         * \brief Called when player accept invite to join battle
         * - Update aura
         * - Teleport if it needed
         * - Update worldstate
         * - Update tenacity
         * \param player: Player who accepted invite
         */
        void OnPlayerJoinWar(Player* player);

        /**
         * \brief Called when player left the battle
         * - Update player aura
         * \param player : Player who left the battle
         */
        void OnPlayerLeaveWar(Player* player);

        /**
         * \brief Called when player left the WG zone
         * \param player : Player who left the zone
         */
        void OnPlayerLeaveZone(Player* player);

        /**
         * \brief Called when player enters in WG zone
         * - Update aura
         * - Update worldstate
         * \param player : Player who enters the zone
         */
        void OnPlayerEnterZone(Player* player);

        /**
         * \brief Called for update battlefield data
         * - Save battle timer in database every minutes
         * - Update imunity aura from graveyard
         * \param diff : time elapsed since the last call (in ms)
         */
        bool Update(uint32 diff);

        /**
         * \brief Called when a creature is created
         * - Update vehicle count
         */
        void OnCreatureCreate(Creature* creature);

        /**
         * \brief Called when a creature is removed
         * - Update vehicle count
         */
        void OnCreatureRemove(Creature* creature);

        /**
         * \brief Called when a gameobject is created
         */
        void OnGameObjectCreate(GameObject* go);

        /**
         * \brief Called when a wall/tower is broken
         * - Update quest
         */
        void BrokenWallOrTower(TeamId team);

        /**
         * \brief Called when a tower is damaged
         * - Update tower count (for reward calcul)
         */
        void UpdateDamagedTowerCount(TeamId team);

        /**
         * \brief Called when tower is broken
         * - Update tower buff
         * - check if three south tower is down for remove 10 minutes to wg
         */
        void UpdatedDestroyedTowerCount(TeamId team);

        void DoCompleteOrIncrementAchievement(uint32 achievement, Player* player, uint8 incrementNumber = 1);

        void RemoveAurasFromPlayer(Player* player);

        /**
         * \brief Called when battlefield is setup, at server start
         */
        bool SetupBattlefield();

        /// Return pointer to relic object
        GameObject* GetRelic() { return GetGameObject(m_titansRelicGUID); }

        /// Define relic object
        void SetRelic(uint64 relicGUID) { m_titansRelicGUID = relicGUID; }

        /// Check if players can interact with the relic (Only if the last door has been broken)
        bool CanInteractWithRelic() { return m_isRelicInteractible; }

        /// Define if player can interact with the relic
        void SetRelicInteractible(bool allow) { m_isRelicInteractible = allow; }

        void UpdateVehicleCountWG();
        void UpdateCounterVehicle(bool init);

        void SendInitWorldStatesTo(Player* player);
        void SendInitWorldStatesToAll();
        void FillInitialWorldStates(WorldPacket& data);

        void HandleKill(Player* killer, Unit* victim);
        void OnUnitDeath(Unit* unit);
        void PromotePlayer(Player* killer);

        void UpdateTenacity();
        void ProcessEvent(WorldObject* obj, uint32 eventId);

        bool FindAndRemoveVehicleFromList(Unit* vehicle);

        // returns the graveyardId in the specified area.
        uint8 GetSpiritGraveyardId(uint32 areaId) const;

        uint32 GetData(uint32 data) const;

    protected:
        bool m_isRelicInteractible;

        Workshop WorkshopsList;

        GuidSet DefenderPortalList;
        GuidSet m_KeepGameObject[2];
        GameObjectBuilding BuildingsInZone;

        GuidSet m_vehicles[2];
        GuidSet CanonList;
		GuidSet TowerCanonList;
		GuidSet gsList;
		GuidSet wrList;
		GuidSet ceeList;
		GuidSet erList;
        GuidSet KeepCreature[2];
        GuidSet OutsideCreature[2];
		GuidSet shopneCreature[2];
		GuidSet shopnwCreature[2];
		GuidSet shopseCreature[2];
		GuidSet shopswCreature[2];
		GuidSet TowerCreature[2];

        uint32 m_tenacityStack;
        uint32 m_saveTimer;
		uint32 announcetime;

        uint64 m_titansRelicGUID;
		uint64 m_bannernwGUID;
		uint64 m_bannerneGUID;
		uint64 m_bannerswGUID;
		uint64 m_bannerseGUID;
		uint64 factoryri_GUID;
		uint64 factoryle_GUID;
		uint64 bassGY_GUID;
		uint64 bassGY_GUID_H;
		uint64 bassGY_GUID_A;

		uint64 landmineGUID;
		uint64 landmineGUID1;
		uint64 landmineGUID2;
		uint64 landmineGUID3;
		uint64 landmineGUID4;
		uint64 landmineGUID5;
		uint64 landmineGUID6;
		uint64 landmineGUID7;
		uint64 landmineGUID8;
		uint64 landmineGUID9;
};

uint32 const VehNumWorldState[]        = { 3680, 3490 };
uint32 const MaxVehNumWorldState[]     = { 3681, 3491 };
uint32 const ClockWorldState[]         = { 3781, 4354 };
uint32 const WintergraspFaction[]      = { 1732, 1735, 35 };
float const WintergraspStalkerPos[]    = { 4948.985f, 2937.789f, 550.5172f, 1.815142f };

uint8 const WG_MAX_OBJ              = 32;
uint8 const WG_MAX_TURRET           = 24;
uint8 const WG_MAX_TURRET_TOWER		=  24;
uint8 const WG_MAX_KEEP_NPC         = 42;
uint8 const WG_MAX_OUTSIDE_NPC      = 14;
uint8 const WG_OUTSIDE_ALLIANCE_NPC = 7;
uint8 const WG_MAX_TELEPORTER       = 12;
uint8 const WG_MAX_WORKSHOP = 6;
uint8 const WG_MAX_GS           = 71;
uint8 const WG_MAX_WR           = 20;
uint8 const WG_MAX_CEE           = 54;
uint8 const WG_MAX_ER           = 15;
uint8 const WG_MAX_WORKSHOP_NE_NPC         = 6;
uint8 const WG_MAX_WORKSHOP_NW_NPC         = 6;
uint8 const WG_MAX_WORKSHOP_SE_NPC         = 6;
uint8 const WG_MAX_WORKSHOP_SW_NPC         = 6;
uint8 const WG_MAX_TOWER_GUARD         = 24;


enum WintergraspGameObjectBuildingType
{
    BATTLEFIELD_WG_OBJECTTYPE_DOOR,
    BATTLEFIELD_WG_OBJECTTYPE_TITANRELIC,
    BATTLEFIELD_WG_OBJECTTYPE_WALL,
    BATTLEFIELD_WG_OBJECTTYPE_DOOR_LAST,
    BATTLEFIELD_WG_OBJECTTYPE_KEEP_TOWER,
    BATTLEFIELD_WG_OBJECTTYPE_TOWER
};

enum WintergraspGameObjectState
{
    BATTLEFIELD_WG_OBJECTSTATE_NONE,
    BATTLEFIELD_WG_OBJECTSTATE_NEUTRAL_INTACT,
    BATTLEFIELD_WG_OBJECTSTATE_NEUTRAL_DAMAGE,
    BATTLEFIELD_WG_OBJECTSTATE_NEUTRAL_DESTROY,
    BATTLEFIELD_WG_OBJECTSTATE_HORDE_INTACT,
    BATTLEFIELD_WG_OBJECTSTATE_HORDE_DAMAGE,
    BATTLEFIELD_WG_OBJECTSTATE_HORDE_DESTROY,
    BATTLEFIELD_WG_OBJECTSTATE_ALLIANCE_INTACT,
    BATTLEFIELD_WG_OBJECTSTATE_ALLIANCE_DAMAGE,
    BATTLEFIELD_WG_OBJECTSTATE_ALLIANCE_DESTROY
};

enum WintergraspWorkshopIds
{
    BATTLEFIELD_WG_WORKSHOP_NE,
    BATTLEFIELD_WG_WORKSHOP_NW,
    BATTLEFIELD_WG_WORKSHOP_SE,
    BATTLEFIELD_WG_WORKSHOP_SW,
    BATTLEFIELD_WG_WORKSHOP_KEEP_WEST,
    BATTLEFIELD_WG_WORKSHOP_KEEP_EAST
};

enum WintergraspWorldstates
{
    WORLDSTATE_WORKSHOP_NE      = 3701,
    WORLDSTATE_WORKSHOP_NW      = 3700,
    WORLDSTATE_WORKSHOP_SE      = 3703,
    WORLDSTATE_WORKSHOP_SW      = 3702,
    WORLDSTATE_WORKSHOP_K_W     = 3698,
    WORLDSTATE_WORKSHOP_K_E     = 3699
};

enum WintergraspTeamControl
{
    BATTLEFIELD_WG_TEAM_ALLIANCE,
    BATTLEFIELD_WG_TEAM_HORDE,
    BATTLEFIELD_WG_TEAM_NEUTRAL
};

/// @todo: Can this be handled with creature_text or SmartAI ?
enum WintergraspText
{
    BATTLEFIELD_WG_TEXT_WORKSHOP_NAME_NE         = 12055,
    BATTLEFIELD_WG_TEXT_WORKSHOP_NAME_NW         = 12052,
    BATTLEFIELD_WG_TEXT_WORKSHOP_NAME_SE         = 12053,
    BATTLEFIELD_WG_TEXT_WORKSHOP_NAME_SW         = 12054,
    BATTLEFIELD_WG_TEXT_WORKSHOP_ATTACK          = 12051,
    BATTLEFIELD_WG_TEXT_WORKSHOP_TAKEN           = 12050,
    BATTLEFIELD_WG_TEXT_ALLIANCE                 = 12057,
    BATTLEFIELD_WG_TEXT_HORDE                    = 12056,
    BATTLEFIELD_WG_TEXT_WILL_START               = 12058,
    BATTLEFIELD_WG_TEXT_START                    = 12067,
    BATTLEFIELD_WG_TEXT_FIRSTRANK                = 12059,
    BATTLEFIELD_WG_TEXT_SECONDRANK               = 12060,
    BATTLEFIELD_WG_TEXT_KEEPTOWER_NAME_NE        = 12062,
    BATTLEFIELD_WG_TEXT_KEEPTOWER_NAME_NW        = 12064,
    BATTLEFIELD_WG_TEXT_KEEPTOWER_NAME_SE        = 12061,
    BATTLEFIELD_WG_TEXT_KEEPTOWER_NAME_SW        = 12063,
    BATTLEFIELD_WG_TEXT_TOWER_DAMAGE             = 12065,
    BATTLEFIELD_WG_TEXT_TOWER_DESTROY            = 12066,
    BATTLEFIELD_WG_TEXT_TOWER_NAME_S             = 12069,
    BATTLEFIELD_WG_TEXT_TOWER_NAME_E             = 12070,
    BATTLEFIELD_WG_TEXT_TOWER_NAME_W             = 12071,
    BATTLEFIELD_WG_TEXT_DEFEND_KEEP              = 12068,
    BATTLEFIELD_WG_TEXT_WIN_KEEP                 = 12072,
	BATTLEFIELD_WG_TEXT_WORKSHOP_NAME_NE_TEAM_NEUTRAL         = 12073,
	BATTLEFIELD_WG_TEXT_WORKSHOP_NAME_NW_TEAM_NEUTRAL         = 12074,
	BATTLEFIELD_WG_TEXT_WORKSHOP_NAME_SE_TEAM_NEUTRAL         = 12075,
	BATTLEFIELD_WG_TEXT_WORKSHOP_NAME_SW_TEAM_NEUTRAL         = 12076,
};

enum WintergraspGameObject
{
    GO_WINTERGRASP_FACTORY_BANNER_NE             = 190475,
    GO_WINTERGRASP_FACTORY_BANNER_NW             = 190487,
    GO_WINTERGRASP_FACTORY_BANNER_SE             = 194959,
    GO_WINTERGRASP_FACTORY_BANNER_SW             = 194962,

    GO_WINTERGRASP_TITAN_S_RELIC                 = 192829,

    GO_WINTERGRASP_FORTRESS_TOWER_1              = 190221,
    GO_WINTERGRASP_FORTRESS_TOWER_2              = 190373,
    GO_WINTERGRASP_FORTRESS_TOWER_3              = 190377,
    GO_WINTERGRASP_FORTRESS_TOWER_4              = 190378,

    GO_WINTERGRASP_SHADOWSIGHT_TOWER             = 190356,
    GO_WINTERGRASP_WINTER_S_EDGE_TOWER           = 190357,
    GO_WINTERGRASP_FLAMEWATCH_TOWER              = 190358,

    GO_WINTERGRASP_FORTRESS_GATE                 = 190375,
    GO_WINTERGRASP_VAULT_GATE                    = 191810,

    GO_WINTERGRASP_KEEP_COLLISION_WALL           = 194323
};

struct WintergraspObjectPositionData
{
    float x;
    float y;
    float z;
    float o;
    uint32 entryHorde;
    uint32 entryAlliance;
};

// *****************************************************
// ************ Destructible (Wall, Tower..) ***********
// *****************************************************

struct WintergraspBuildingSpawnData
{
    uint32 entry;
    uint32 WorldState;
    float x;
    float y;
    float z;
    float o;
    uint32 type;
    uint32 nameId;
};

struct WintergraspRebuildableBuildingData
{
    uint64 Guid;
    uint32 entry;
    uint32 WorldState;
    float x;
    float y;
    float z;
    float o;
    uint32 type;
    uint32 nameId;
};

const WintergraspBuildingSpawnData WGGameObjectBuilding[WG_MAX_OBJ] =
{
    // Wall (Not spawned in db)
    // Entry  WS    X        Y        Z        O         type                          NameID
    { 190219, 3749, 5371.46f, 3047.47f, 407.571f, 3.14159f, BATTLEFIELD_WG_OBJECTTYPE_WALL, 0 },
    { 190220, 3750, 5331.26f, 3047.1f, 407.923f, 0.052359f, BATTLEFIELD_WG_OBJECTTYPE_WALL, 0 },
    { 191795, 3764, 5385.84f, 2909.49f, 409.713f, 0.00872f, BATTLEFIELD_WG_OBJECTTYPE_WALL, 0 },
    { 191796, 3772, 5384.45f, 2771.84f, 410.27f, 3.14159f, BATTLEFIELD_WG_OBJECTTYPE_WALL, 0 },
    { 191799, 3762, 5371.44f, 2630.61f, 408.816f, 3.13286f, BATTLEFIELD_WG_OBJECTTYPE_WALL, 0 },
    { 191800, 3766, 5301.84f, 2909.09f, 409.866f, 0.008724f, BATTLEFIELD_WG_OBJECTTYPE_WALL, 0 },
    { 191801, 3770, 5301.06f, 2771.41f, 409.901f, 3.14159f, BATTLEFIELD_WG_OBJECTTYPE_WALL, 0 },
    { 191802, 3751, 5280.2f, 2995.58f, 408.825f, 1.61443f, BATTLEFIELD_WG_OBJECTTYPE_WALL, 0 },
    { 191803, 3752, 5279.14f, 2956.02f, 408.604f, 1.5708f, BATTLEFIELD_WG_OBJECTTYPE_WALL, 0 },
    { 191804, 3767, 5278.69f, 2882.51f, 409.539f, 1.5708f, BATTLEFIELD_WG_OBJECTTYPE_WALL, 0 },
    { 191806, 3769, 5279.5f, 2798.94f, 409.998f, 1.5708f, BATTLEFIELD_WG_OBJECTTYPE_WALL, 0 },
    { 191807, 3759, 5279.94f, 2724.77f, 409.945f, 1.56207f, BATTLEFIELD_WG_OBJECTTYPE_WALL, 0 },
    { 191808, 3760, 5279.6f, 2683.79f, 409.849f, 1.55334f, BATTLEFIELD_WG_OBJECTTYPE_WALL, 0 },
    { 191809, 3761, 5330.96f, 2630.78f, 409.283f, 3.13286f, BATTLEFIELD_WG_OBJECTTYPE_WALL, 0 },
    { 190369, 3753, 5256.08f, 2933.96f, 409.357f, 3.13286f, BATTLEFIELD_WG_OBJECTTYPE_WALL, 0 },
    { 190370, 3758, 5257.46f, 2747.33f, 409.743f, -3.13286f, BATTLEFIELD_WG_OBJECTTYPE_WALL, 0 },
    { 190371, 3754, 5214.96f, 2934.09f, 409.19f, -0.008724f, BATTLEFIELD_WG_OBJECTTYPE_WALL, 0 },
    { 190372, 3757, 5215.82f, 2747.57f, 409.188f, -3.13286f, BATTLEFIELD_WG_OBJECTTYPE_WALL, 0 },
    { 190374, 3755, 5162.27f, 2883.04f, 410.256f, 1.57952f, BATTLEFIELD_WG_OBJECTTYPE_WALL, 0 },
    { 190376, 3756, 5163.72f, 2799.84f, 409.227f, 1.57952f, BATTLEFIELD_WG_OBJECTTYPE_WALL, 0 },

    // Tower of keep (Not spawned in db)
    { 190221, 3711, 5281.15f, 3044.59f, 407.843f, 3.11539f, BATTLEFIELD_WG_OBJECTTYPE_KEEP_TOWER, BATTLEFIELD_WG_TEXT_KEEPTOWER_NAME_NW },
    { 190373, 3713, 5163.76f, 2932.23f, 409.19f, 3.12412f, BATTLEFIELD_WG_OBJECTTYPE_KEEP_TOWER, BATTLEFIELD_WG_TEXT_KEEPTOWER_NAME_SW },
    { 190377, 3714, 5166.4f, 2748.37f, 409.188f, -1.5708f, BATTLEFIELD_WG_OBJECTTYPE_KEEP_TOWER, BATTLEFIELD_WG_TEXT_KEEPTOWER_NAME_SE },
    { 190378, 3712, 5281.19f, 2632.48f, 409.099f, -1.58825f, BATTLEFIELD_WG_OBJECTTYPE_KEEP_TOWER, BATTLEFIELD_WG_TEXT_KEEPTOWER_NAME_NE },

    // Wall (with passage) (Not spawned in db)
    { 191797, 3765, 5343.29f, 2908.86f, 409.576f, 0.008724f, BATTLEFIELD_WG_OBJECTTYPE_WALL, 0 },
    { 191798, 3771, 5342.72f, 2771.39f, 409.625f, 3.14159f, BATTLEFIELD_WG_OBJECTTYPE_WALL, 0 },
    { 191805, 3768, 5279.13f, 2840.8f, 409.783f, 1.57952f, BATTLEFIELD_WG_OBJECTTYPE_WALL, 0 },

    // South tower (Not spawned in db)
    { 190356, 3704, 4557.17f, 3623.94f, 395.883f, 1.67552f, BATTLEFIELD_WG_OBJECTTYPE_TOWER, BATTLEFIELD_WG_TEXT_TOWER_NAME_W },
    { 190357, 3705, 4398.17f, 2822.5f, 405.627f, -3.12412f, BATTLEFIELD_WG_OBJECTTYPE_TOWER, BATTLEFIELD_WG_TEXT_TOWER_NAME_S },
    { 190358, 3706, 4459.1f, 1944.33f, 434.991f, -2.00276f, BATTLEFIELD_WG_OBJECTTYPE_TOWER, BATTLEFIELD_WG_TEXT_TOWER_NAME_E },

    // Door of forteress (Not spawned in db)
    { GO_WINTERGRASP_FORTRESS_GATE, 3763, 5162.99f, 2841.23f, 410.162f, -3.13286f, BATTLEFIELD_WG_OBJECTTYPE_DOOR, 0 },

    // Last door (Not spawned in db)
    { GO_WINTERGRASP_VAULT_GATE, 3773, 5397.11f, 2841.54f, 425.899f, 3.14159f, BATTLEFIELD_WG_OBJECTTYPE_DOOR_LAST, 0 },
};

const Position WGTurret[WG_MAX_TURRET] =
{
    { 5391.19f, 3060.8f,  419.616f, 1.69557f },
    { 5266.75f, 2976.5f,  421.067f, 3.20354f },
    { 5234.86f, 2948.8f,  420.88f,  1.61311f },
    { 5323.05f, 2923.7f,  421.645f, 1.5817f },
    { 5363.82f, 2923.87f, 421.709f, 1.60527f },
    { 5264.04f, 2861.34f, 421.587f, 3.21142f },
    { 5264.68f, 2819.78f, 421.656f, 3.15645f },
    { 5322.16f, 2756.69f, 421.646f, 4.69978f },
    { 5363.78f, 2756.77f, 421.629f, 4.78226f },
    { 5236.2f,  2732.68f, 421.649f, 4.72336f },
    { 5265.02f, 2704.63f, 421.7f,   3.12507f },
    { 5350.87f, 2616.03f, 421.243f, 4.72729f },
    { 5390.95f, 2615.5f,  421.126f, 4.6409f },
    { 5148.8f,  2820.24f, 421.621f, 3.16043f },
    { 5147.98f, 2861.93f, 421.63f,  3.18792f },
	{ 5278.2f,  2607.22f, 439.75f,  4.71944f},
	{ 5163.91f, 2722.96f, 439.84f,  4.672097f},
	{ 5139.15f, 2747.33f, 439.84f,  3.125082f},
	{ 5138.58f, 2935.15f, 439.84f,  3.11723f},
	{ 5163.06f, 2959.52f, 439.84f,  1.47258f},
	{ 5255.87f, 3047.62f, 438.49f,  3.13677f},
	{ 5280.89f, 3071.32f, 438.49f,  1.62879f},
	{ 5352.05f, 3061.48f, 421.1f,   1.518156f},
	{ 5253.59f, 2631.82f, 439.75f,  3.136081f},
};

const Position WGTurretTower[WG_MAX_TURRET_TOWER] =
{
    { 4469.448f, 1966.623f, 465.647f, 1.153573f },
    { 4481.996f, 1933.658f, 465.647f, 5.873029f },
    { 4449.092f, 1922.207f, 465.647f, 4.291998f },
    { 4436.631f, 1954.867f, 465.648f, 2.696869f },
    { 4470.156f, 1914.79f, 441.994f, 5.11902f },
    { 4429.163f, 1933.224f, 441.994f, 3.461045f },
    { 4448.138f, 1974.998f, 441.995f, 1.967238f },
    { 4489.115f, 1955.358f, 441.994f, 0.397988f },

    { 4398.814f, 2797.266f, 436.283f, 4.703747f },
    { 4423.43f, 2822.762f, 436.283f, 6.223487f },
    { 4397.825f, 2847.629f, 436.283f, 1.57943f },
    { 4373.589f, 2822.052f, 436.282f, 3.119393f },
    { 4376.496f, 2799.816f, 412.63f, 3.95349f },
    { 4421.64f, 2799.935f, 412.63f, 5.459298f },
    { 4420.263f, 2845.34f, 412.63f, 0.742197f },
	{ 4376.615f, 2800.122f, 412.631f, 3.92993f },

	{ 4559.634f, 3599.451f, 426.539f, 4.862201f },
	{ 4581.895f, 3626.438f, 426.539f, 0.117806f },
	{ 4554.788f, 3648.292f, 426.539f, 1.635787f },
	{ 4532.671f, 3621.284f, 426.539f, 3.211295f },
	{ 4537.38f, 3599.531f, 402.886f, 3.998462f },
	{ 4581.497f, 3604.087f, 402.886f, 5.651723f },
	{ 4576.89f, 3648.47f, 402.887f, 0.930496f },
	{ 4532.483f, 3644.01f, 402.887f, 2.438461f },
};

const Position GlacialSpirit[WG_MAX_GS] =
{
	{ 4689.55f, 	2588.1f, 	335.327f, 	2.0499f },
	{ 4877.99f, 	3215.59f, 	349.572f, 	4.59316f },
	{ 4583.73f, 	3069.43f, 	335.79f, 	2.00935f },
	{ 4694.42f, 	2660.07f, 	336.139f, 	0.255026f },
	{ 5182.14f, 	3261.53f, 	352.746f, 	0.590342f },
	{ 4827.06f, 	3218.72f, 	352.66f, 	5.46865f },
	{ 5096.51f, 	3637.21f, 	352.623f, 	0.282101f },
	{ 4675.14f, 	2707.97f, 	338.059f, 	1.84271f },
	{ 4572.26f, 	3003.85f, 	334.634f, 	5.14655f },
	{ 4925.61f, 	2077.11f, 	414.309f, 	5.9837f },
	{ 5171.25f, 	3580.06f, 	352.622f, 	4.79287f },
	{ 5111.87f, 	3512.72f, 	354.518f, 	4.49388f },
	{ 4686.07f, 	3358.93f, 	336.39f, 	3.03004f },
	{ 4622.56f, 	3177.89f, 	343.194f, 	1.79115f },
	{ 5204.71f, 	3337.12f, 	352.883f, 	2.28206f },
	{ 5274.75f, 	3370.94f, 	352.893f, 	3.17938f },
	{ 4738.18f, 	3378.69f, 	335.586f, 	2.04301f },
	{ 4908.1f, 	1996.07f, 	420.287f, 	0.153202f },
	{ 4680.19f, 	2515.12f, 	338.027f, 	0.6831f },
	{ 4978.86f, 	3576.16f, 	352.683f, 	3.47471f },
	{ 4630.58f, 	3005.83f, 	335.74f, 	1.16629f },
	{ 5216.07f, 	3425.03f, 	352.86f, 	2.27177f },
	{ 4775.79f, 	3473.27f, 	352.465f, 	5.36047f },
	{ 5289.42f, 	3314.61f, 	353.495f, 	3.59508f },
	{ 4621.66f, 	3046.16f, 	337.014f, 	1.08866f },
	{ 4661.27f, 	3323.91f, 	334.64f, 	1.44487f },
	{ 4854.33f, 	1971.22f, 	417.521f, 	5.1201f },
	{ 4691.28f, 	3238.81f, 	340.525f, 	2.24532f },
	{ 4628.97f, 	3104.99f, 	340.128f, 	4.05496f },
	{ 4524.78f, 	2963.2f, 	347.633f, 	0.586103f },
	{ 5142.44f, 	3348.53f, 	353.525f, 	0.612001f },
	{ 5105.44f, 	3410.52f, 	353.705f, 	1.43943f },
	{ 4888.21f, 	3497.29f, 	348.279f, 	3.44531f },
	{ 4873.92f, 	3538.88f, 	337.848f, 	5.16005f },
	{ 4986.15f, 	3698.07f, 	346.102f, 	2.30063f },
	{ 4889.77f, 	3171.95f, 	352.573f, 	5.52568f },
	{ 4945.13f, 	3211.14f, 	345.899f, 	1.78028f },
	{ 4925.16f, 	3637.69f, 	336.189f, 	5.30131f },
	{ 4822.67f, 	3180.47f, 	352.591f, 	3.38911f },
	{ 4996.46f, 	3220.24f, 	345.899f, 	1.05557f },
	{ 4821.73f, 	3501.55f, 	333.29f, 	4.15202f },
	{ 4755.39f, 	3552.49f, 	349.398f, 	1.55471f },
	{ 4803.15f, 	3134.7f, 	350.018f, 	1.82873f },
	{ 4589.63f, 	2329.17f, 	340.5f, 	1.96046f },
	{ 4683.87f, 	3557.53f, 	347.166f, 	3.53641f },
	{ 4573.76f, 	2376.27f, 	334.806f, 	4.24246f },
	{ 4717.7f, 	2342.11f, 	338.149f, 	3.0678f },
	{ 4682.79f, 	2311.82f, 	337.047f, 	0.868671f },
	{ 4773.65f, 	3184.28f, 	346.345f, 	2.55019f },
	{ 4489.78f, 	2410.96f, 	352.259f, 	5.05479f },
	{ 4787.87f, 	3227.57f, 	352.543f, 	0.538954f },
	{ 4608.34f, 	2428.72f, 	336.872f, 	3.13573f },
	{ 4817.9f, 	2141.85f, 	336.087f, 	2.45904f },
	{ 4531.39f, 	2451.69f, 	350.649f, 	5.59499f },
	{ 4775.26f, 	2194.86f, 	334.122f, 	0.492956f },
	{ 4826.78f, 	2215.43f, 	333.711f, 	1.86415f },
	{ 4782.11f, 	2248.57f, 	349.133f, 	5.45306f },
	{ 4853.58f, 	2053.08f, 	394.042f, 	3.04679f },
	{ 4817.47f, 	2014.95f, 	401.938f, 	5.16164f },
	{ 4725.69f, 	2261.02f, 	337.54f, 	2.76032f },
	{ 4670.61f, 	3187.49f, 	343.136f, 	3.25079f },
	{ 4748.96f, 	3180.21f, 	349.524f, 	2.92986f },
	{ 4627.8f, 	2362.98f, 	333.347f, 	4.74339f },
	{ 4672.68f, 	2779.12f, 	338.747f, 	1.49849f },
	{ 4840.01f, 	2831.54f, 	346.966f, 	3.50606f },
	{ 4615.34f, 	2738.8f, 	345.254f, 	5.2002f },
	{ 4603.27f, 	2788.44f, 	334.735f, 	5.32646f },
	{ 4656.99f, 	2460.82f, 	335.597f, 	1.52435f },
	{ 4567.8f, 	2836.47f, 	348.893f, 	4.80579f },
	{ 5039.58f, 	3589.58f, 	353.197f, 	2.98176f },
	{ 5131.98f, 	3605.52f, 	352.892f, 	4.03332f },
};

const Position WateRevenant[WG_MAX_WR] =
{
	{ 4650.11f, 	3216.19f, 	336.037f, 	0.462086f },
	{ 4704.64f, 	3453.58f, 	353.475f, 	5.79484f },
	{ 4555.87f, 	2976.34f, 	335.639f, 	0.399384f },
	{ 5141.5f, 	3416.66f, 	353.632f, 	5.51932f },
	{ 4972.7f, 	3210.85f, 	346.899f, 	2.70996f },
	{ 4781.65f, 	3148.91f, 	346.899f, 	5.31853f },
	{ 4715.96f, 	3519.97f, 	344.724f, 	0.793317f },
	{ 4653.8f, 	2342.48f, 	334.472f, 	1.18402f },
	{ 4576.24f, 	2360.43f, 	334.347f, 	3.19344f },
	{ 4539.82f, 	2426.31f, 	345.484f, 	1.58316f },
	{ 4757.16f, 	2220.46f, 	335.432f, 	2.83542f },
	{ 4788.23f, 	1975.37f, 	420.174f, 	0.598566f },
	{ 4902.13f, 	2235.1f, 	347.502f, 	0.766064f },
	{ 4883.96f, 	1985.27f, 	410.708f, 	3.55263f },
	{ 4822.82f, 	2041.69f, 	404.181f, 	2.35956f },
	{ 4903.36f, 	2049.29f, 	402.789f, 	2.98618f },
	{ 4595.95f, 	2848.9f, 	335.764f, 	0.52775f },
	{ 4639.69f, 	2769.04f, 	335.584f, 	3.56645f },
	{ 4612.82f, 	2909.1f, 	335.637f, 	3.27896f },
	{ 4811.98f, 	2179.59f, 	334.711f, 	0.231615f },
};

const Position ChilledEarthElemental[WG_MAX_CEE] =
{
	{ 4724.14f, 	3023.03f, 	357.07f, 	2.07377f },
	{ 4402.46f, 	2536.18f, 	368.144f, 	4.05318f },
	{ 4758.89f, 	2759.37f, 	355.925f, 	0.616189f },
	{ 5075.68f, 	2543.87f, 	359.455f, 	6.11773f },
	{ 4585.57f, 	2656.52f, 	379.22f, 	5.2706f },
	{ 4661.45f, 	2209.93f, 	359.827f, 	1.71621f },
	{ 4819.77f, 	2525.28f, 	360.236f, 	0.172158f },
	{ 5115.13f, 	2335.43f, 	357.932f, 	3.94158f },
	{ 5129.76f, 	2545.37f, 	366.021f, 	4.42132f },
	{ 5248.6f, 	2410.12f, 	365.864f, 	1.87671f },
	{ 5072.47f, 	2397.25f, 	358.265f, 	0.518365f },
	{ 5122.14f, 	2485.9f, 	357.412f, 	1.73522f },
	{ 4581.11f, 	2479.83f, 	356.676f, 	1.38399f },
	{ 5113.11f, 	2419.79f, 	357.294f, 	2.15784f },
	{ 5107.38f, 	3244.32f, 	356.531f, 	1.36192f },
	{ 4952.88f, 	2650.97f, 	357.348f, 	2.33141f },
	{ 4783.14f, 	2987.15f, 	356.366f, 	5.51822f },
	{ 4511.95f, 	2580.17f, 	356.539f, 	3.80415f },
	{ 4920.75f, 	3132.85f, 	357.325f, 	1.01521f },
	{ 4602.48f, 	2194.29f, 	358.938f, 	4.62069f },
	{ 4972.42f, 	2752.24f, 	374.293f, 	4.1734f },
	{ 4715.46f, 	3135.49f, 	357.418f, 	3.3733f },
	{ 4989.06f, 	3088.64f, 	357.16f, 	5.51382f },
	{ 4374.41f, 	3051.63f, 	357.185f, 	3.2587f },
	{ 5077.62f, 	2586.14f, 	363.294f, 	2.19018f },
	{ 4752.59f, 	2686.37f, 	356.93f, 	1.60445f },
	{ 4417.34f, 	2588.77f, 	359.225f, 	4.21174f },
	{ 4982.31f, 	3150.53f, 	355.821f, 	0.621733f },
	{ 5198.47f, 	2400.87f, 	357.811f, 	6.09233f },
	{ 5148.02f, 	3075.18f, 	378.945f, 	0.491059f },
	{ 4954.05f, 	2578.04f, 	359.531f, 	3.22827f },
	{ 4816.06f, 	2569.97f, 	358.079f, 	4.14084f },
	{ 5090.98f, 	2449.11f, 	357.927f, 	0.121174f },
	{ 4741.97f, 	2610.15f, 	353.628f, 	3.50084f },
	{ 4917.1f, 	2568.43f, 	361.781f, 	2.03022f },
	{ 4864.81f, 	2609.88f, 	356.589f, 	0.178893f },
	{ 5042.38f, 	2332.73f, 	359.42f, 	1.91498f },
	{ 4807.91f, 	3071.82f, 	357.644f, 	5.31699f },
	{ 4877.42f, 	2685.97f, 	357.254f, 	4.7469f },
	{ 4344.59f, 	3015.37f, 	357.139f, 	3.03624f },
	{ 5092.04f, 	3184.08f, 	356.584f, 	5.30346f },
	{ 5093.37f, 	3097.77f, 	364.962f, 	1.20752f },
	{ 4721.82f, 	2479.68f, 	354.079f, 	0.637859f },
	{ 4620.45f, 	2258.97f, 	357.687f, 	2.12215f },
	{ 5152.93f, 	2242.71f, 	367.194f, 	1.63981f },
	{ 4537.05f, 	2534.57f, 	359.304f, 	3.99622f },
	{ 4754.27f, 	2520.92f, 	361.039f, 	0.819296f },
	{ 4816.85f, 	2748.6f, 	357.007f, 	1.73854f },
	{ 4912.62f, 	2687.42f, 	364.903f, 	3.0498f },
	{ 5048.15f, 	2243.37f, 	356.483f, 	4.6813f },
	{ 5227.51f, 	2345.33f, 	368.245f, 	2.0103f },
	{ 4816.63f, 	2624.93f, 	355.758f, 	2.76636f },
	{ 4833.85f, 	2789.67f, 	358.741f, 	0.017142f },
	{ 5142.73f, 	3186.17f, 	362.655f, 	2.23846f },
};

const Position EarthboundRevenant[WG_MAX_ER] =
{
	{ 4873.42f, 	2652.43f, 	357.654f, 	0.76254f },
	{ 4389.34f, 	2508.53f, 	370.818f, 	2.11255f },
	{ 5173.06f, 	2249.31f, 	371.722f, 	2.83934f },
	{ 5216.38f, 	2383.63f, 	361.764f, 	4.19153f },
	{ 4712.17f, 	3103.18f, 	357.862f, 	2.56202f },
	{ 5093.51f, 	2531.22f, 	362.62f, 	4.29335f },
	{ 4737.18f, 	2548.74f, 	368.521f, 	1.63842f },
	{ 4948.96f, 	3113.54f, 	358.345f, 	4.24874f },
	{ 4629.28f, 	2217.87f, 	360.606f, 	6.04854f },
	{ 4781.8f, 	3025.19f, 	358.45f, 	6.1112f },
	{ 4609.38f, 	2548.96f, 	375.02f, 	4.64568f },
	{ 4749.34f, 	2724.37f, 	359.097f, 	1.52935f },
	{ 5123.66f, 	3225.15f, 	357.609f, 	0.978378f },
	{ 5086.53f, 	2422.25f, 	358.788f, 	4.05321f },
	{ 4906.22f, 	2612.16f, 	360.119f, 	2.36181f },
};

const WintergraspObjectPositionData WGshopneNPC[WG_MAX_WORKSHOP_NE_NPC] =
{
	{ 4939.7597f, 2389.060f, 326.153f, 3.263f, BATTLEFILED_WG_NPC_WORKSHOP_H, BATTLEFILED_WG_NPC_WORKSHOP_A },
	{ 4966.939f, 2439.462f, 321.478f, 0.964506f, BATTLEFIELD_WG_NPC_GUARD_H, BATTLEFIELD_WG_NPC_GUARD_A },
	{ 4945.465f, 2459.222f, 320.879f, 0.680192f, BATTLEFIELD_WG_NPC_GUARD_H, BATTLEFIELD_WG_NPC_GUARD_A },
	{ 4858.262f, 2422.081f, 320.939f, 2.916222f, BATTLEFIELD_WG_NPC_GUARD_H, BATTLEFIELD_WG_NPC_GUARD_A },
	{ 4853.701f, 2402.188f, 321.909f, 2.916222f, BATTLEFIELD_WG_NPC_GUARD_H, BATTLEFIELD_WG_NPC_GUARD_A },
	{ 5104.75f, 2300.949951f, 368.567993f, 0.733038f, NPC_TAUNKA_SPIRIT_GUIDE, NPC_DWARVEN_SPIRIT_GUIDE },
};

const WintergraspObjectPositionData WGshopnwNPC[WG_MAX_WORKSHOP_NW_NPC] =
{
	{ 4964.890f, 3383.060f, 382.911f, 6.126f, BATTLEFILED_WG_NPC_WORKSHOP_H, BATTLEFILED_WG_NPC_WORKSHOP_A },
	{ 5005.068f, 3314.081f, 376.382f, 5.061142f, BATTLEFIELD_WG_NPC_GUARD_H, BATTLEFIELD_WG_NPC_GUARD_A },
	{ 4989.147f, 3308.292f, 376.872f, 5.218222f, BATTLEFIELD_WG_NPC_GUARD_H, BATTLEFIELD_WG_NPC_GUARD_A },
	{ 4899.35f, 3348.08f, 375.286f, 3.242918f, BATTLEFIELD_WG_NPC_GUARD_H, BATTLEFIELD_WG_NPC_GUARD_A },
	{ 4899.893f, 3318.662f, 375.668f, 3.352115f, BATTLEFIELD_WG_NPC_GUARD_H, BATTLEFIELD_WG_NPC_GUARD_A },
	{ 5103.129883f, 3462.129883f, 368.567993f, 5.2709f, NPC_TAUNKA_SPIRIT_GUIDE, NPC_DWARVEN_SPIRIT_GUIDE },
};

const WintergraspObjectPositionData WGshopseNPC[WG_MAX_WORKSHOP_SE_NPC] =
{
	{ 4357.669f, 2357.989f, 382.006f, 1.675f, BATTLEFILED_WG_NPC_WORKSHOP_H, BATTLEFILED_WG_NPC_WORKSHOP_A },
	{ 4348.698f, 2411.460f, 374.660f, 1.647803f, BATTLEFIELD_WG_NPC_GUARD_H, BATTLEFIELD_WG_NPC_GUARD_A },
	{ 4404.674f, 2366.434f, 376.208f, 6.042891f, BATTLEFIELD_WG_NPC_GUARD_H, BATTLEFIELD_WG_NPC_GUARD_A },
	{ 4349.663f, 2299.645f, 374.660f, 4.724179f, BATTLEFIELD_WG_NPC_GUARD_H, BATTLEFIELD_WG_NPC_GUARD_A },
	{ 4395.391f, 2328.546f, 376.306f, 6.042891f, BATTLEFIELD_WG_NPC_GUARD_H, BATTLEFIELD_WG_NPC_GUARD_A },
	{ 4318.439941f, 2408.070068f, 392.675995f, 6.23083f, NPC_TAUNKA_SPIRIT_GUIDE, NPC_DWARVEN_SPIRIT_GUIDE },
};

const WintergraspObjectPositionData WGshopswNPC[WG_MAX_WORKSHOP_SW_NPC] =
{
	{ 4354.149f, 3312.82f, 377.962f, 1.675f, BATTLEFILED_WG_NPC_WORKSHOP_H, BATTLEFILED_WG_NPC_WORKSHOP_A },
	{ 4363.983f, 3247.447f, 373.146f, 4.888357f, BATTLEFIELD_WG_NPC_GUARD_H, BATTLEFIELD_WG_NPC_GUARD_A },
	{ 4401.490f, 3247.698f, 373.074f, 4.643313f, BATTLEFIELD_WG_NPC_GUARD_H, BATTLEFIELD_WG_NPC_GUARD_A },
	{ 4411.466f, 3291.84f, 372.16f, 0.084045f, BATTLEFIELD_WG_NPC_GUARD_H, BATTLEFIELD_WG_NPC_GUARD_A },
	{ 4412.113f, 3315.646f, 371.281f, 0.006323f, BATTLEFIELD_WG_NPC_GUARD_H, BATTLEFIELD_WG_NPC_GUARD_A },
	{ 4336.25f, 3235.52002f, 390.334015f, 0.628319f, NPC_TAUNKA_SPIRIT_GUIDE, NPC_DWARVEN_SPIRIT_GUIDE },
};

const WintergraspObjectPositionData WGtowerguard[WG_MAX_TOWER_GUARD] =
{
      { 4452.859863f, 2808.870117f, 402.604004f, 6.056290f, BATTLEFIELD_WG_NPC_GUARD_H, BATTLEFIELD_WG_NPC_GUARD_A },     // Standing Guard
      { 4455.899902f, 2835.958008f, 401.122559f, 0.034907f, BATTLEFIELD_WG_NPC_GUARD_H, BATTLEFIELD_WG_NPC_GUARD_A },     // Standing Guard
      { 4408.094238f, 2864.732178f, 404.808563f, 1.605943f, BATTLEFIELD_WG_NPC_GUARD_H, BATTLEFIELD_WG_NPC_GUARD_A },     // Roaming Guard
      { 4362.089844f, 2811.510010f, 407.337006f, 3.193950f, BATTLEFIELD_WG_NPC_GUARD_H, BATTLEFIELD_WG_NPC_GUARD_A },     // Standing Guard
      { 4412.290039f, 2753.790039f, 401.015015f, 5.829400f, BATTLEFIELD_WG_NPC_GUARD_H, BATTLEFIELD_WG_NPC_GUARD_A },     // Standing Guard
      { 4421.939941f, 2773.189941f, 400.894989f, 5.707230f, BATTLEFIELD_WG_NPC_GUARD_H, BATTLEFIELD_WG_NPC_GUARD_A },     // Standing Guard
      { 4501.060059f, 1990.280029f, 431.157013f, 1.029740f, BATTLEFIELD_WG_NPC_GUARD_H, BATTLEFIELD_WG_NPC_GUARD_A },     // Standing Guard
      { 4463.830078f, 2015.180054f, 430.299988f, 1.431170f, BATTLEFIELD_WG_NPC_GUARD_H, BATTLEFIELD_WG_NPC_GUARD_A },     // Standing Guard
      { 4494.580078f, 1943.760010f, 435.627014f, 6.195920f, BATTLEFIELD_WG_NPC_GUARD_H, BATTLEFIELD_WG_NPC_GUARD_A },     // Standing Guard
      { 4450.149902f, 1897.579956f, 435.045013f, 4.398230f, BATTLEFIELD_WG_NPC_GUARD_H, BATTLEFIELD_WG_NPC_GUARD_A },     // Standing Guard
      { 4428.870117f, 1906.869995f, 432.648010f, 3.996800f, BATTLEFIELD_WG_NPC_GUARD_H, BATTLEFIELD_WG_NPC_GUARD_A },     // Standing Guard
		{ 4361.204102f, 2832.31958f, 407.525909f, 3.110168f, BATTLEFIELD_WG_NPC_GUARD_H, BATTLEFIELD_WG_NPC_GUARD_A },     // Standing Guard
	{ 4426.628418f, 1971.30125f, 434.514832f, 2.72532f, BATTLEFIELD_WG_NPC_GUARD_H, BATTLEFIELD_WG_NPC_GUARD_A },     // Standing Guard
	{ 4417.341309f, 1951.666504f, 434.61322f, 2.74574f, BATTLEFIELD_WG_NPC_GUARD_H, BATTLEFIELD_WG_NPC_GUARD_A },     // Standing Guard
	{ 4572.979004f, 3580.555908f, 393.777161f, 4.864748f, BATTLEFIELD_WG_NPC_GUARD_H, BATTLEFIELD_WG_NPC_GUARD_A },     // Standing Guard
	{ 4551.098145f, 3580.176025f, 394.370361f, 4.717878f, BATTLEFIELD_WG_NPC_GUARD_H, BATTLEFIELD_WG_NPC_GUARD_A },     // Standing Guard
	{ 4597.944824f, 3617.586182f, 397.559418f, 6.278465f, BATTLEFIELD_WG_NPC_GUARD_H, BATTLEFIELD_WG_NPC_GUARD_A },     // Standing Guard
	{ 4595.674805f, 3638.637451f, 397.749115f, 0.102094f, BATTLEFIELD_WG_NPC_GUARD_H, BATTLEFIELD_WG_NPC_GUARD_A },     // Standing Guard
	{ 4563.566406f, 3662.35498f, 397.881958f, 1.643045f, BATTLEFIELD_WG_NPC_GUARD_H, BATTLEFIELD_WG_NPC_GUARD_A },     // Standing Guard
	{ 4542.328613f, 3660.675049f, 397.658813f, 1.639904f, BATTLEFIELD_WG_NPC_GUARD_H, BATTLEFIELD_WG_NPC_GUARD_A },     // Standing Guard
	{ 4512.175293f, 3629.484863f, 393.887482f, 3.270988f, BATTLEFIELD_WG_NPC_GUARD_H, BATTLEFIELD_WG_NPC_GUARD_A },     // Standing Guard
	{ 4514.929688f, 3607.600586f, 394.090149f, 3.256065f, BATTLEFIELD_WG_NPC_GUARD_H, BATTLEFIELD_WG_NPC_GUARD_A },     // Standing Guard
	{ 4387.198242f, 2864.363037f, 404.427979f, 1.605943f, BATTLEFIELD_WG_NPC_GUARD_H, BATTLEFIELD_WG_NPC_GUARD_A },     // Standing Guard
	{ 4485.327148f, 1915.732544f, 434.533142f, 5.731636f, BATTLEFIELD_WG_NPC_GUARD_H, BATTLEFIELD_WG_NPC_GUARD_A },     // Standing Guard
};

// Here there is all npc keeper spawn point
const WintergraspObjectPositionData WGKeepNPC[WG_MAX_KEEP_NPC] =
{
    // X          Y            Z           O         horde                          alliance
    // North East
    { 5326.203125f, 2660.026367f, 409.100891f, 2.543383f, BATTLEFIELD_WG_NPC_GUARD_H, BATTLEFIELD_WG_NPC_GUARD_A },       // Roaming Guard
    { 5298.430176f, 2738.760010f, 409.316010f, 3.971740f, BATTLEFIELD_WG_NPC_VIERON_BLAZEFEATHER, BATTLEFIELD_WG_NPC_BOWYER_RANDOLPH }, // Vieron Blazefeather
    { 5335.310059f, 2764.110107f, 409.274994f, 4.834560f, BATTLEFIELD_WG_NPC_GUARD_H, BATTLEFIELD_WG_NPC_GUARD_A },       // Standing Guard
    { 5349.810059f, 2763.629883f, 409.333008f, 4.660030f, BATTLEFIELD_WG_NPC_GUARD_H, BATTLEFIELD_WG_NPC_GUARD_A },       // Standing Guard
    // North
    { 5373.470215f, 2789.060059f, 409.322998f, 2.600540f, BATTLEFIELD_WG_NPC_STONE_GUARD_MUKAR, BATTLEFIELD_WG_NPC_KNIGHT_DAMERON }, // Stone Guard Mukar
    { 5296.560059f, 2789.870117f, 409.274994f, 0.733038f, BATTLEFIELD_WG_NPC_HOODOO_MASTER_FU_JIN, BATTLEFIELD_WG_NPC_SORCERESS_KAYLANA }, // Voodoo Master Fu'jin
    { 5372.670000f, 2786.740000f, 409.442000f, 2.809980f, BATTLEFIELD_WG_NPC_CHAMPION_ROS_SLAI, BATTLEFIELD_WG_NPC_MARSHAL_MAGRUDER }, // Wintergrasp Quartermaster
    { 5368.709961f, 2856.360107f, 409.322998f, 2.949610f, BATTLEFIELD_WG_NPC_GUARD_H, BATTLEFIELD_WG_NPC_GUARD_A },       // Standing Guard
    { 5367.910156f, 2826.520020f, 409.322998f, 3.333580f, BATTLEFIELD_WG_NPC_GUARD_H, BATTLEFIELD_WG_NPC_GUARD_A },       // Standing Guard
    { 5389.270020f, 2847.370117f, 418.759003f, 3.106690f, BATTLEFIELD_WG_NPC_GUARD_H, BATTLEFIELD_WG_NPC_GUARD_A },       // Standing Guard
    { 5388.560059f, 2834.770020f, 418.759003f, 3.071780f, BATTLEFIELD_WG_NPC_GUARD_H, BATTLEFIELD_WG_NPC_GUARD_A },       // Standing Guard
    { 5359.129883f, 2837.989990f, 409.364014f, 4.698930f, BATTLEFIELD_WG_NPC_COMMANDER_DARDOSH, BATTLEFIELD_WG_NPC_COMMANDER_ZANNETH }, // Commander Dardosh
    { 5366.129883f, 2833.399902f, 409.322998f, 3.141590f, BATTLEFIELD_WG_NPC_TACTICAL_OFFICER_KILRATH, BATTLEFIELD_WG_NPC_TACTICAL_OFFICER_AHBRAMIS }, // Tactical Officer Kilrath
    // X          Y            Z           O         horde  alliance
    // North West
    { 5350.680176f, 2917.010010f, 409.274994f, 1.466080f, BATTLEFIELD_WG_NPC_GUARD_H, BATTLEFIELD_WG_NPC_GUARD_A },       // Standing Guard
    { 5335.120117f, 2916.800049f, 409.444000f, 1.500980f, BATTLEFIELD_WG_NPC_GUARD_H, BATTLEFIELD_WG_NPC_GUARD_A },       // Standing Guard
    { 5295.560059f, 2926.669922f, 409.274994f, 0.872665f, BATTLEFIELD_WG_NPC_SIEGESMITH_STRONGHOOF, BATTLEFIELD_WG_NPC_SIEGE_MASTER_STOUTHANDLE }, // Stronghoof
    { 5371.399902f, 3026.510010f, 409.205994f, 3.250030f, BATTLEFIELD_WG_NPC_PRIMALIST_MULFORT, BATTLEFIELD_WG_NPC_ANCHORITE_TESSA }, // Primalist Mulfort
    { 5392.123535f, 3031.110352f, 409.187683f, 3.677212f, BATTLEFIELD_WG_NPC_GUARD_H, BATTLEFIELD_WG_NPC_GUARD_A },       // Roaming Guard
    // South
    { 5270.060059f, 2847.550049f, 409.274994f, 3.071780f, BATTLEFIELD_WG_NPC_GUARD_H, BATTLEFIELD_WG_NPC_GUARD_A },       // Standing Guard
    { 5270.160156f, 2833.479980f, 409.274994f, 3.124140f, BATTLEFIELD_WG_NPC_GUARD_H, BATTLEFIELD_WG_NPC_GUARD_A },       // Standing Guard
    { 5179.109863f, 2837.129883f, 409.274994f, 3.211410f, BATTLEFIELD_WG_NPC_GUARD_H, BATTLEFIELD_WG_NPC_GUARD_A },       // Standing Guard
    { 5179.669922f, 2846.600098f, 409.274994f, 3.089230f, BATTLEFIELD_WG_NPC_GUARD_H, BATTLEFIELD_WG_NPC_GUARD_A },       // Standing Guard
    { 5234.970215f, 2883.399902f, 409.274994f, 4.293510f, BATTLEFIELD_WG_NPC_LIEUTENANT_MURP, BATTLEFIELD_WG_NPC_SENIOR_DEMOLITIONIST_LEGOSO }, // Lieutenant Murp
    // X          Y            Z           O         horde  alliance
    // Portal guards (from around the fortress)
    { 5319.209473f, 3055.947754f, 409.176636f, 1.020201f, BATTLEFIELD_WG_NPC_GUARD_H, BATTLEFIELD_WG_NPC_GUARD_A },       // Standing Guard
    { 5311.612305f, 3061.207275f, 408.734161f, 0.965223f, BATTLEFIELD_WG_NPC_GUARD_H, BATTLEFIELD_WG_NPC_GUARD_A },       // Standing Guard
    { 5264.713379f, 3017.283447f, 408.479706f, 3.482424f, BATTLEFIELD_WG_NPC_GUARD_H, BATTLEFIELD_WG_NPC_GUARD_A },       // Standing Guard
    { 5269.096191f, 3008.315918f, 408.826294f, 3.843706f, BATTLEFIELD_WG_NPC_GUARD_H, BATTLEFIELD_WG_NPC_GUARD_A },       // Standing Guard
    { 5201.414551f, 2945.096924f, 409.190735f, 0.945592f, BATTLEFIELD_WG_NPC_GUARD_H, BATTLEFIELD_WG_NPC_GUARD_A },       // Standing Guard
    { 5193.386230f, 2949.617188f, 409.190735f, 1.145859f, BATTLEFIELD_WG_NPC_GUARD_H, BATTLEFIELD_WG_NPC_GUARD_A },       // Standing Guard
    { 5148.116211f, 2904.761963f, 409.193756f, 3.368532f, BATTLEFIELD_WG_NPC_GUARD_H, BATTLEFIELD_WG_NPC_GUARD_A },       // Standing Guard
    { 5153.355957f, 2895.501465f, 409.199310f, 3.549174f, BATTLEFIELD_WG_NPC_GUARD_H, BATTLEFIELD_WG_NPC_GUARD_A },       // Standing Guard
    { 5154.353027f, 2787.349365f, 409.250183f, 2.555644f, BATTLEFIELD_WG_NPC_GUARD_H, BATTLEFIELD_WG_NPC_GUARD_A },       // Standing Guard
    { 5150.066406f, 2777.876953f, 409.343903f, 2.708797f, BATTLEFIELD_WG_NPC_GUARD_H, BATTLEFIELD_WG_NPC_GUARD_A },       // Standing Guard
    { 5193.706543f, 2732.882812f, 409.189514f, 4.845073f, BATTLEFIELD_WG_NPC_GUARD_H, BATTLEFIELD_WG_NPC_GUARD_A },       // Standing Guard
    { 5202.126953f, 2737.570557f, 409.189514f, 5.375215f, BATTLEFIELD_WG_NPC_GUARD_H, BATTLEFIELD_WG_NPC_GUARD_A },       // Standing Guard
    { 5269.181152f, 2671.174072f, 409.098999f, 2.457459f, BATTLEFIELD_WG_NPC_GUARD_H, BATTLEFIELD_WG_NPC_GUARD_A },       // Standing Guard
    { 5264.960938f, 2662.332520f, 409.098999f, 2.598828f, BATTLEFIELD_WG_NPC_GUARD_H, BATTLEFIELD_WG_NPC_GUARD_A },       // Standing Guard
    { 5307.111816f, 2616.006836f, 409.095734f, 5.355575f, BATTLEFIELD_WG_NPC_GUARD_H, BATTLEFIELD_WG_NPC_GUARD_A },       // Standing Guard
    { 5316.770996f, 2619.430176f, 409.027740f, 5.363431f, BATTLEFIELD_WG_NPC_GUARD_H, BATTLEFIELD_WG_NPC_GUARD_A },        // Standing Guard
	{ 5153.485f, 2849.541f, 409.18f, 3.389767f,  BATTLEFIELD_WG_NPC_GUARD_H, BATTLEFIELD_WG_NPC_GUARD_A },        // Standing Guard
	{ 5153.17f, 2832.09f, 409.185f, 2.867477f,  BATTLEFIELD_WG_NPC_GUARD_H, BATTLEFIELD_WG_NPC_GUARD_A },        // Standing Guard
	{ 5372.76f, 2794.66f, 409.239f, 2.833772f,  Multivendor_h, Multivendor_a },        // Standing Guard
};

const WintergraspObjectPositionData WGOutsideNPC[WG_MAX_OUTSIDE_NPC] =
{
    { 5032.04f, 3681.79f, 362.980f, 4.210f, BATTLEFIELD_WG_NPC_VIERON_BLAZEFEATHER, 0 },
    { 5020.71f, 3626.19f, 360.150f, 4.640f, BATTLEFIELD_WG_NPC_HOODOO_MASTER_FU_JIN, 0 },
    { 4994.85f, 3660.51f, 359.150f, 2.260f, BATTLEFIELD_WG_NPC_COMMANDER_DARDOSH, 0 },
    { 5015.46f, 3677.11f, 362.970f, 6.009f, BATTLEFIELD_WG_NPC_TACTICAL_OFFICER_KILRATH, 0 },
    { 5031.12f, 3663.77f, 363.500f, 3.110f, BATTLEFIELD_WG_NPC_SIEGESMITH_STRONGHOOF, 0 },
    { 5042.74f, 3675.82f, 363.060f, 3.358f, BATTLEFIELD_WG_NPC_PRIMALIST_MULFORT, 0 },
    { 5014.45f, 3640.87f, 361.390f, 3.280f, BATTLEFIELD_WG_NPC_LIEUTENANT_MURP, 0 },
    { 5100.07f, 2168.89f, 365.779f, 1.972f, 0, BATTLEFIELD_WG_NPC_BOWYER_RANDOLPH },
    { 5081.70f, 2173.73f, 365.878f, 0.855f, 0, BATTLEFIELD_WG_NPC_SORCERESS_KAYLANA },
    { 5078.28f, 2183.70f, 365.029f, 1.466f, 0, BATTLEFIELD_WG_NPC_COMMANDER_ZANNETH },
    { 5088.49f, 2188.18f, 365.647f, 5.253f, 0, BATTLEFIELD_WG_NPC_TACTICAL_OFFICER_AHBRAMIS },
    { 5095.67f, 2193.28f, 365.924f, 4.939f, 0, BATTLEFIELD_WG_NPC_SIEGE_MASTER_STOUTHANDLE },
    { 5088.61f, 2167.66f, 365.689f, 0.680f, 0, BATTLEFIELD_WG_NPC_ANCHORITE_TESSA },
    { 5080.40f, 2199.00f, 359.489f, 2.967f, 0, BATTLEFIELD_WG_NPC_SENIOR_DEMOLITIONIST_LEGOSO },
};

struct WintergraspTeleporterData
{
    uint32 entry;
    float x;
    float y;
    float z;
    float o;
};

const WintergraspTeleporterData WGPortalDefenderData[WG_MAX_TELEPORTER] =
{
    // Player teleporter
    { 190763, 5153.41f, 2901.35f, 409.191f, -0.069f },
    { 190763, 5268.70f, 2666.42f, 409.099f, -0.715f },
    { 190763, 5197.05f, 2944.81f, 409.191f, 2.3387f },
    { 190763, 5196.67f, 2737.34f, 409.189f, -2.932f },
    { 190763, 5314.58f, 3055.85f, 408.862f, 0.5410f },
    { 190763, 5391.28f, 2828.09f, 418.675f, -2.164f },
    { 190763, 5153.93f, 2781.67f, 409.246f, 1.6580f },
    { 190763, 5311.44f, 2618.93f, 409.092f, -2.373f },
    { 190763, 5269.21f, 3013.84f, 408.828f, -1.762f },
    { 190763, 5401.62f, 2853.66f, 418.674f, 2.6354f },
    // Vehicle teleporter
    { 192951, 5314.51f, 2703.69f, 408.550f, -0.890f },
    { 192951, 5316.25f, 2977.04f, 408.539f, -0.820f },
};

// *********************************************************
// **********Tower Element(GameObject, Creature)************
// *********************************************************

struct WintergraspTowerData
{
    uint32 towerEntry;                  // Gameobject id of tower
    uint8 nbObject;                     // Number of gameobjects spawned on this point
    WintergraspObjectPositionData GameObject[6];   // Gameobject position and entry (Horde/Alliance)

    // Creature: Turrets and Guard /// @todo: Killed on Tower destruction ? Tower damage ? Requires confirming
    uint8 nbCreatureBottom;
    WintergraspObjectPositionData CreatureBottom[14];
    uint8 nbCreatureTop;
    WintergraspObjectPositionData CreatureTop[5];
};

uint8 const WG_MAX_ATTACKTOWERS = 3;
// 192414 : 0 in sql, 1 in header
// 192278 : 0 in sql, 3 in header
const WintergraspTowerData AttackTowers[WG_MAX_ATTACKTOWERS] =
{
    // West tower
    {
        190356,
        6,
        {
            { 4559.109863f, 3606.219971f, 419.998993f, -1.483530f, 192488, 192501 },    // Flag on tower
            { 4539.419922f, 3622.489990f, 420.033997f, -3.071770f, 192488, 192501 },    // Flag on tower
            { 4555.259766f, 3641.649902f, 419.973999f, 1.675510f, 192488, 192501 },     // Flag on tower
            { 4574.870117f, 3625.909912f, 420.079010f, 0.080117f, 192488, 192501 },     // Flag on tower
            { 4433.899902f, 3534.139893f, 360.274994f, -1.850050f, 192269, 192278 },    // Flag near workshop
            { 4572.930176f, 3475.520020f, 363.009003f, 1.42240f, 192269, 192278 }       // Flag near bridge
        },
        0,
        {
			{ 0, 0, 0, 0, 0, 0 },
			{ 0, 0, 0, 0, 0, 0 },
			{ 0, 0, 0, 0, 0, 0 },
			{ 0, 0, 0, 0, 0, 0 },
			{ 0, 0, 0, 0, 0, 0 },
			{ 0, 0, 0, 0, 0, 0 },
			{ 0, 0, 0, 0, 0, 0 },
			{ 0, 0, 0, 0, 0, 0 },
			{ 0, 0, 0, 0, 0, 0 },
			{ 0, 0, 0, 0, 0, 0 },
			{ 0, 0, 0, 0, 0, 0 },
        },

        0,
        {
            { 0, 0, 0, 0, 0, 0 },
            { 0, 0, 0, 0, 0, 0 },
            { 0, 0, 0, 0, 0, 0 },
            { 0, 0, 0, 0, 0, 0 },
            { 0, 0, 0, 0, 0, 0 },
        }
    },

    // South Tower
    {
        190357,
        5,
        {
            { 4416.000000f, 2822.669922f, 429.851013f, -0.017452f, 192488, 192501 },    // Flag on tower
            { 4398.819824f, 2804.699951f, 429.791992f, -1.588250f, 192488, 192501 },    // Flag on tower
            { 4387.620117f, 2719.570068f, 389.934998f, -1.544620f, 192366, 192414 },    // Flag near tower
            { 4464.120117f, 2855.449951f, 406.110992f, 0.829032f, 192366, 192429 },     // Flag near tower
            { 4526.459961f, 2810.179932f, 391.200012f, -2.993220f, 192269, 192278 },    // Flag near bridge
            { 0, 0, 0, 0, 0, 0 },
        },
        0,
        {
			{ 0, 0, 0, 0, 0, 0 },
			{ 0, 0, 0, 0, 0, 0 },
			{ 0, 0, 0, 0, 0, 0 },
			{ 0, 0, 0, 0, 0, 0 },
			{ 0, 0, 0, 0, 0, 0 },
			{ 0, 0, 0, 0, 0, 0 },
			{ 0, 0, 0, 0, 0, 0 },
			{ 0, 0, 0, 0, 0, 0 },
			{ 0, 0, 0, 0, 0, 0 },
			{ 0, 0, 0, 0, 0, 0 },
			{ 0, 0, 0, 0, 0, 0 },
			{ 0, 0, 0, 0, 0, 0 },
			{ 0, 0, 0, 0, 0, 0 },
			{ 0, 0, 0, 0, 0, 0 },
        },
        0,
        {
            { 0, 0, 0, 0, 0, 0 },
            { 0, 0, 0, 0, 0, 0 },
            { 0, 0, 0, 0, 0, 0 },
            { 0, 0, 0, 0, 0, 0 },
            { 0, 0, 0, 0, 0, 0 },
        },
    },

    // East Tower
    {
        190358,
        4,
        {
            { 4466.790039f, 1960.420044f, 459.144012f, 1.151920f, 192488, 192501 },     // Flag on tower
            { 4475.350098f, 1937.030029f, 459.070007f, -0.43633f, 192488, 192501 },     // Flag on tower
            { 4451.759766f, 1928.099976f, 459.075989f, -2.00713f, 192488, 192501 },     // Flag on tower
            { 4442.990234f, 1951.900024f, 459.092987f, 2.740160f, 192488, 192501 },     // Flag on tower
            { 0, 0, 0, 0, 0, 0 },
            { 0, 0, 0, 0, 0, 0 },
        },
        0,
        {
			{ 0, 0, 0, 0, 0, 0 },
			{ 0, 0, 0, 0, 0, 0 },
			{ 0, 0, 0, 0, 0, 0 },
			{ 0, 0, 0, 0, 0, 0 },
			{ 0, 0, 0, 0, 0, 0 },
			{ 0, 0, 0, 0, 0, 0 },
			{ 0, 0, 0, 0, 0, 0 },
            { 0, 0, 0, 0, 0, 0 },
            { 0, 0, 0, 0, 0, 0 },
			{ 0, 0, 0, 0, 0, 0 },
			{ 0, 0, 0, 0, 0, 0 },
			{ 0, 0, 0, 0, 0, 0 },
        },
        0,
        {
            { 0, 0, 0, 0, 0, 0 },
            { 0, 0, 0, 0, 0, 0 },
            { 0, 0, 0, 0, 0, 0 },
            { 0, 0, 0, 0, 0, 0 },
            { 0, 0, 0, 0, 0, 0 },
        },
    },
};

struct WintergraspTowerCannonData
{
    uint32 towerEntry;
    uint8 nbTowerCannonBottom;
    Position TowerCannonBottom[5];
    uint8 nbTurretTop;
    Position TurretTop[5];
};

const uint8 WG_MAX_TOWER_CANNON = 7;

const WintergraspTowerCannonData TowerCannon[WG_MAX_TOWER_CANNON] =
{
    {
        190221,
        0,
        {
            { 0, 0, 0, 0 },
            { 0, 0, 0, 0 },
            { 0, 0, 0, 0 },
            { 0, 0, 0, 0 },
            { 0, 0, 0, 0 },
        },
        0,
        {
            { 0, 0, 0, 0 },
            { 0, 0, 0, 0 },
            { 0, 0, 0, 0 },
            { 0, 0, 0, 0 },
            { 0, 0, 0, 0 },
        },
    },
    {
        190373,
        0,
        {
            { 0, 0, 0, 0 },
            { 0, 0, 0, 0 },
            { 0, 0, 0, 0 },
            { 0, 0, 0, 0 },
            { 0, 0, 0, 0 },
        },
        0,
        {
            { 0, 0, 0, 0 },
            { 0, 0, 0, 0 },
            { 0, 0, 0, 0 },
            { 0, 0, 0, 0 },
            { 0, 0, 0, 0 },
        },
    },
    {
        190377,
        0,
        {
            { 0, 0, 0, 0 },
            { 0, 0, 0, 0 },
            { 0, 0, 0, 0 },
            { 0, 0, 0, 0 },
            { 0, 0, 0, 0 },
        },
        0,
        {
            { 0, 0, 0, 0 },
            { 0, 0, 0, 0 },
            { 0, 0, 0, 0 },
            { 0, 0, 0, 0 },
            { 0, 0, 0, 0 },
        },
    },
    {
        190378,
        0,
        {
            { 0, 0, 0, 0 },
            { 0, 0, 0, 0 },
            { 0, 0, 0, 0 },
            { 0, 0, 0, 0 },
            { 0, 0, 0, 0 },
        },
        0,
        {
            { 0, 0, 0, 0 },
            { 0, 0, 0, 0 },
            { 0, 0, 0, 0 },
            { 0, 0, 0, 0 },
            { 0, 0, 0, 0 },
        },
    },
    {
        190356,
        0,
        {
            {0, 0, 0, 0},
            {0, 0, 0, 0},
            {0, 0, 0, 0},
            {0, 0, 0, 0},
            {0, 0, 0, 0},
        },
        0,
        {
            {0, 0, 0, 0},
            {0, 0, 0, 0},
            {0, 0, 0, 0},
            {0, 0, 0, 0},
            {0, 0, 0, 0},
        },
    },
    {
        190357,
        0,
        {
            {0, 0, 0, 0},
            {0, 0, 0, 0},
            { 0, 0, 0, 0 },
            { 0, 0, 0, 0 },
            { 0, 0, 0, 0 },
        },
        0,
        {
            {0, 0, 0, 0},
            {0, 0, 0, 0},
            {0, 0, 0, 0},
            { 0, 0, 0, 0 },
            { 0, 0, 0, 0 },
        },
    },
    {
        190358,
        0,
        {
            {0, 0, 0, 0},
            {0, 0, 0, 0},
            { 0, 0, 0, 0 },
            { 0, 0, 0, 0 },
            { 0, 0, 0, 0 },
        },
        0,
        {
            {0, 0, 0, 0},
            {0, 0, 0, 0},
            { 0, 0, 0, 0 },
            { 0, 0, 0, 0 },
            { 0, 0, 0, 0 },
        },
    },
};

// *********************************************************
// *****************WorkShop Data & Element*****************
// *********************************************************

struct WGWorkshopData
{
    uint8 id;
    uint32 worldstate;
    uint32 text;
};

const WGWorkshopData WorkshopsData[WG_MAX_WORKSHOP] =
{
    // NE
    {BATTLEFIELD_WG_WORKSHOP_NE, WORLDSTATE_WORKSHOP_NE, BATTLEFIELD_WG_TEXT_WORKSHOP_NAME_NE},
    // NW
    {BATTLEFIELD_WG_WORKSHOP_NW, WORLDSTATE_WORKSHOP_NW, BATTLEFIELD_WG_TEXT_WORKSHOP_NAME_NW},
    // SE
    {BATTLEFIELD_WG_WORKSHOP_SE, WORLDSTATE_WORKSHOP_SE, BATTLEFIELD_WG_TEXT_WORKSHOP_NAME_SE},
    // SW
    {BATTLEFIELD_WG_WORKSHOP_SW, WORLDSTATE_WORKSHOP_SW, BATTLEFIELD_WG_TEXT_WORKSHOP_NAME_SW},
    // KEEP WEST - It can't be taken, so it doesn't have a textid
    {BATTLEFIELD_WG_WORKSHOP_KEEP_WEST, WORLDSTATE_WORKSHOP_K_W, 0},
    // KEEP EAST - It can't be taken, so it doesn't have a textid
    {BATTLEFIELD_WG_WORKSHOP_KEEP_EAST, WORLDSTATE_WORKSHOP_K_E, 0}
};

// ********************************************************************
// *         Structs using for Building, Graveyard, Workshop          *
// ********************************************************************
// Structure for different buildings that can be destroyed during battle
struct BfWGGameObjectBuilding
{
    BfWGGameObjectBuilding(BattlefieldWG* WG)
    {
        m_WG = WG;
        m_Team = 0;
        m_BuildGUID = 0;
        m_Type = 0;
        m_WorldState = 0;
        m_State = 0;
        m_NameId = 0;
    }

    // the team that controls this point
    uint8 m_Team;

    // WG object
    BattlefieldWG* m_WG;

    // Linked gameobject
    uint64 m_BuildGUID;

    // eWGGameObjectBuildingType
    uint32 m_Type;

    // WorldState
    uint32 m_WorldState;

    // eWGGameObjectState
    uint32 m_State;

    // Name id for warning text
    uint32 m_NameId;

    // GameObject associations
    GuidSet m_GameObjectList[2];

    // Creature associations
    GuidSet m_CreatureBottomList[2];
    GuidSet m_CreatureTopList[2];
    GuidSet m_TowerCannonBottomList;
    GuidSet m_TurretTopList;

    void Rebuild()
    {
        switch (m_Type)
        {
            case BATTLEFIELD_WG_OBJECTTYPE_KEEP_TOWER:
            case BATTLEFIELD_WG_OBJECTTYPE_DOOR_LAST:
            case BATTLEFIELD_WG_OBJECTTYPE_DOOR:
            case BATTLEFIELD_WG_OBJECTTYPE_WALL:
                m_Team = m_WG->GetDefenderTeam();           // Objects that are part of the keep should be the defender's
                break;
            case BATTLEFIELD_WG_OBJECTTYPE_TOWER:
                m_Team = m_WG->GetAttackerTeam();           // The towers in the south should be the attacker's
                break;
            default:
                m_Team = TEAM_NEUTRAL;
                break;
        }

        if (GameObject* build = m_WG->GetGameObject(m_BuildGUID))
        {
			build->IsDestructibleBuilding();
            // Rebuild gameobject
            if (build->IsDestructibleBuilding())
            {
                build->SetDestructibleState(GO_DESTRUCTIBLE_REBUILDING, NULL, true);
                if (build->GetEntry() == GO_WINTERGRASP_VAULT_GATE)
                    if (GameObject* go = build->FindNearestGameObject(GO_WINTERGRASP_KEEP_COLLISION_WALL, 50.0f))
                        go->SetGoState(GO_STATE_READY);

                // Update worldstate
                m_State = BATTLEFIELD_WG_OBJECTSTATE_ALLIANCE_INTACT - (m_Team * 3);
                m_WG->SendUpdateWorldState(m_WorldState, m_State);
            }
            UpdateCreatureAndGo();
            build->SetFaction(WintergraspFaction[m_Team]);
        }
    }

	 void Removebuild()
    {
		 if (GameObject* build = m_WG->GetGameObject(m_BuildGUID))
        {
	        build->RemoveFromWorld();
		}
    }

    // Called when associated gameobject is damaged
    void Damaged()
    {
        // Update worldstate
        m_State = BATTLEFIELD_WG_OBJECTSTATE_ALLIANCE_DAMAGE - (m_Team * 3);
        m_WG->SendUpdateWorldState(m_WorldState, m_State);

        // Send warning message
        if (m_NameId)                                       // tower damage + name
            m_WG->SendWarningToAllInZone(m_NameId);

        for (GuidSet::const_iterator itr = m_CreatureTopList[m_WG->GetAttackerTeam()].begin(); itr != m_CreatureTopList[m_WG->GetAttackerTeam()].end(); ++itr)
            if (Creature* creature = m_WG->GetCreature(*itr))
                m_WG->HideNpc(creature);

        for (GuidSet::const_iterator itr = m_TurretTopList.begin(); itr != m_TurretTopList.end(); ++itr)
            if (Creature* creature = m_WG->GetCreature(*itr))
                m_WG->HideNpc(creature);

        if (m_Type == BATTLEFIELD_WG_OBJECTTYPE_KEEP_TOWER)
            m_WG->UpdateDamagedTowerCount(m_WG->GetDefenderTeam());
        else if (m_Type == BATTLEFIELD_WG_OBJECTTYPE_TOWER)
            m_WG->UpdateDamagedTowerCount(m_WG->GetAttackerTeam());
    }

    // Called when associated gameobject is destroyed
    void Destroyed()
    {
        // Update worldstate
        m_State = BATTLEFIELD_WG_OBJECTSTATE_ALLIANCE_DESTROY - (m_Team * 3);
        m_WG->SendUpdateWorldState(m_WorldState, m_State);

        // Warn players
        if (m_NameId)
            m_WG->SendWarningToAllInZone(m_NameId);

        switch (m_Type)
        {
            // Inform the global wintergrasp script of the destruction of this object
            case BATTLEFIELD_WG_OBJECTTYPE_TOWER:
            case BATTLEFIELD_WG_OBJECTTYPE_KEEP_TOWER:
                m_WG->UpdatedDestroyedTowerCount(TeamId(m_Team));
                break;
            case BATTLEFIELD_WG_OBJECTTYPE_DOOR_LAST:
                if (GameObject* build = m_WG->GetGameObject(m_BuildGUID))
                    if (GameObject* go = build->FindNearestGameObject(GO_WINTERGRASP_KEEP_COLLISION_WALL, 50.0f))
                        go->SetGoState(GO_STATE_ACTIVE);
                m_WG->SetRelicInteractible(true);
                if (m_WG->GetRelic())
                    m_WG->GetRelic()->RemoveFlag(GAMEOBJECT_FLAGS, GO_FLAG_IN_USE);
                else
                    TC_LOG_ERROR("misc", "BattlefieldWG: Relic not found.");
                break;
        }

        m_WG->BrokenWallOrTower(TeamId(m_Team));
    }

    void Init(GameObject* go, uint32 type, uint32 worldstate, uint32 nameid)
    {
        if (!go)
            return;

        // GameObject associated to object
        m_BuildGUID = go->GetGUID();

        // Type of building (WALL/TOWER/DOOR)
        m_Type = type;

        // WorldState for client (icon on map)
        m_WorldState = worldstate;

        // NameId for Warning text
        m_NameId = nameid;

        switch (m_Type)
        {
            case BATTLEFIELD_WG_OBJECTTYPE_KEEP_TOWER:
            case BATTLEFIELD_WG_OBJECTTYPE_DOOR_LAST:
            case BATTLEFIELD_WG_OBJECTTYPE_DOOR:
            case BATTLEFIELD_WG_OBJECTTYPE_WALL:
                m_Team = m_WG->GetDefenderTeam();           // Objects that are part of the keep should be the defender's
                break;
            case BATTLEFIELD_WG_OBJECTTYPE_TOWER:
                m_Team = m_WG->GetAttackerTeam();           // The towers in the south should be the attacker's
                break;
            default:
                m_Team = TEAM_NEUTRAL;
                break;
        }

        m_State = sWorld->getWorldState(m_WorldState);
        switch (m_State)
        {
            case BATTLEFIELD_WG_OBJECTSTATE_ALLIANCE_INTACT:
            case BATTLEFIELD_WG_OBJECTSTATE_HORDE_INTACT:
                go->SetDestructibleState(GO_DESTRUCTIBLE_REBUILDING, NULL, true);
                break;
            case BATTLEFIELD_WG_OBJECTSTATE_ALLIANCE_DESTROY:
            case BATTLEFIELD_WG_OBJECTSTATE_HORDE_DESTROY:
                go->SetDestructibleState(GO_DESTRUCTIBLE_DESTROYED);
                break;
            case BATTLEFIELD_WG_OBJECTSTATE_ALLIANCE_DAMAGE:
            case BATTLEFIELD_WG_OBJECTSTATE_HORDE_DAMAGE:
                go->SetDestructibleState(GO_DESTRUCTIBLE_DAMAGED);
                break;
        }

        int32 towerid = -1;
        switch (go->GetEntry())
        {
            case GO_WINTERGRASP_FORTRESS_TOWER_1:
                towerid = 0;
                break;
            case GO_WINTERGRASP_FORTRESS_TOWER_2:
                towerid = 1;
                break;
            case GO_WINTERGRASP_FORTRESS_TOWER_3:
                towerid = 2;
                break;
            case GO_WINTERGRASP_FORTRESS_TOWER_4:
                towerid = 3;
                break;
            case GO_WINTERGRASP_SHADOWSIGHT_TOWER:
                towerid = 4;
                break;
            case GO_WINTERGRASP_WINTER_S_EDGE_TOWER:
                towerid = 5;
                break;
            case GO_WINTERGRASP_FLAMEWATCH_TOWER:
                towerid = 6;
                break;
        }

        if (towerid > 3) // Attacker towers
        {
            // Spawn associate gameobjects
            for (uint8 i = 0; i < AttackTowers[towerid - 4].nbObject; i++)
            {
                WintergraspObjectPositionData gobData = AttackTowers[towerid - 4].GameObject[i];
                if (GameObject* go = m_WG->SpawnGameObject(gobData.entryHorde, gobData.x, gobData.y, gobData.z, gobData.o))
                    m_GameObjectList[TEAM_HORDE].insert(go->GetGUID());
                if (GameObject* go = m_WG->SpawnGameObject(gobData.entryAlliance, gobData.x, gobData.y, gobData.z, gobData.o))
                    m_GameObjectList[TEAM_ALLIANCE].insert(go->GetGUID());
            }

            // Spawn associate npc bottom
            for (uint8 i = 0; i < AttackTowers[towerid - 4].nbCreatureBottom; i++)
            {
                WintergraspObjectPositionData creatureData = AttackTowers[towerid - 4].CreatureBottom[i];
                if (Creature* creature = m_WG->SpawnCreature(creatureData.entryHorde, creatureData.x, creatureData.y, creatureData.z, creatureData.o, TEAM_HORDE))
                    m_CreatureBottomList[TEAM_HORDE].insert(creature->GetGUID());
                if (Creature* creature = m_WG->SpawnCreature(creatureData.entryAlliance, creatureData.x, creatureData.y, creatureData.z, creatureData.o, TEAM_ALLIANCE))
                    m_CreatureBottomList[TEAM_ALLIANCE].insert(creature->GetGUID());
            }

            // Spawn associate npc top
            for (uint8 i = 0; i < AttackTowers[towerid - 4].nbCreatureTop; i++)
            {
                WintergraspObjectPositionData creatureData = AttackTowers[towerid - 4].CreatureTop[i];
                if (Creature* creature = m_WG->SpawnCreature(creatureData.entryHorde, creatureData.x, creatureData.y, creatureData.z, creatureData.o, TEAM_HORDE))
                    m_CreatureTopList[TEAM_HORDE].insert(creature->GetGUID());
                if (Creature* creature = m_WG->SpawnCreature(creatureData.entryAlliance, creatureData.x, creatureData.y, creatureData.z, creatureData.o, TEAM_ALLIANCE))
                    m_CreatureTopList[TEAM_ALLIANCE].insert(creature->GetGUID());
            }
        }

        if (towerid >= 0)
        {
            // Spawn Turret bottom
            for (uint8 i = 0; i < TowerCannon[towerid].nbTowerCannonBottom; i++)
            {
                Position turretPos = TowerCannon[towerid].TowerCannonBottom[i].GetPosition();
                if (Creature* turret = m_WG->SpawnCreature(NPC_WINTERGRASP_TOWER_CANNON, turretPos, TEAM_ALLIANCE))
                {
                    m_TowerCannonBottomList.insert(turret->GetGUID());
                    switch (go->GetEntry())
                    {
                        case GO_WINTERGRASP_FORTRESS_TOWER_1:
                        case GO_WINTERGRASP_FORTRESS_TOWER_2:
                        case GO_WINTERGRASP_FORTRESS_TOWER_3:
                        case GO_WINTERGRASP_FORTRESS_TOWER_4:
                            turret->setFaction(WintergraspFaction[m_WG->GetDefenderTeam()]);
                            break;
                        case GO_WINTERGRASP_SHADOWSIGHT_TOWER:
                        case GO_WINTERGRASP_WINTER_S_EDGE_TOWER:
                        case GO_WINTERGRASP_FLAMEWATCH_TOWER:
                            turret->setFaction(WintergraspFaction[m_WG->GetAttackerTeam()]);
                            break;
                    }
                    m_WG->HideNpc(turret);
                }
            }

            // Spawn Turret top
            for (uint8 i = 0; i < TowerCannon[towerid].nbTurretTop; i++)
            {
                Position towerCannonPos = TowerCannon[towerid].TurretTop[i].GetPosition();
                if (Creature* turret = m_WG->SpawnCreature(NPC_WINTERGRASP_TOWER_CANNON, towerCannonPos, TeamId(0)))
                {
                    m_TurretTopList.insert(turret->GetGUID());
                    switch (go->GetEntry())
                    {
                        case GO_WINTERGRASP_FORTRESS_TOWER_1:
                        case GO_WINTERGRASP_FORTRESS_TOWER_2:
                        case GO_WINTERGRASP_FORTRESS_TOWER_3:
                        case GO_WINTERGRASP_FORTRESS_TOWER_4:
                            turret->setFaction(WintergraspFaction[m_WG->GetDefenderTeam()]);
                            break;
                        case GO_WINTERGRASP_SHADOWSIGHT_TOWER:
                        case GO_WINTERGRASP_WINTER_S_EDGE_TOWER:
                        case GO_WINTERGRASP_FLAMEWATCH_TOWER:
                            turret->setFaction(WintergraspFaction[m_WG->GetAttackerTeam()]);
                            break;
                    }
                    m_WG->HideNpc(turret);
                }
            }
            UpdateCreatureAndGo();
        }
    }

    void UpdateCreatureAndGo()
    {
        for (GuidSet::const_iterator itr = m_CreatureTopList[m_WG->GetDefenderTeam()].begin(); itr != m_CreatureTopList[m_WG->GetDefenderTeam()].end(); ++itr)
            if (Creature* creature = m_WG->GetCreature(*itr))
                m_WG->HideNpc(creature);

        for (GuidSet::const_iterator itr = m_CreatureTopList[m_WG->GetAttackerTeam()].begin(); itr != m_CreatureTopList[m_WG->GetAttackerTeam()].end(); ++itr)
            if (Creature* creature = m_WG->GetCreature(*itr))
                m_WG->ShowNpc(creature, true);

        for (GuidSet::const_iterator itr = m_CreatureBottomList[m_WG->GetDefenderTeam()].begin(); itr != m_CreatureBottomList[m_WG->GetDefenderTeam()].end(); ++itr)
            if (Creature* creature = m_WG->GetCreature(*itr))
                m_WG->HideNpc(creature);

        for (GuidSet::const_iterator itr = m_CreatureBottomList[m_WG->GetAttackerTeam()].begin(); itr != m_CreatureBottomList[m_WG->GetAttackerTeam()].end(); ++itr)
            if (Creature* creature = m_WG->GetCreature(*itr))
                m_WG->ShowNpc(creature, true);

        for (GuidSet::const_iterator itr = m_GameObjectList[m_WG->GetDefenderTeam()].begin(); itr != m_GameObjectList[m_WG->GetDefenderTeam()].end(); ++itr)
            if (GameObject* object = m_WG->GetGameObject(*itr))
                object->SetRespawnTime(RESPAWN_ONE_DAY);

        for (GuidSet::const_iterator itr = m_GameObjectList[m_WG->GetAttackerTeam()].begin(); itr != m_GameObjectList[m_WG->GetAttackerTeam()].end(); ++itr)
            if (GameObject* object = m_WG->GetGameObject(*itr))
                object->SetRespawnTime(RESPAWN_IMMEDIATELY);
    }

    void UpdateTurretAttack(bool disable)
    {
        for (GuidSet::const_iterator itr = m_TowerCannonBottomList.begin(); itr != m_TowerCannonBottomList.end(); ++itr)
        {
            if (Creature* creature = m_WG->GetCreature(*itr))
            {
                if (GameObject* build = m_WG->GetGameObject(m_BuildGUID))
                {
                    if (disable)
                        m_WG->HideNpc(creature);
                    else
                        m_WG->ShowNpc(creature, true);

                    switch (build->GetEntry())
                    {
                        case GO_WINTERGRASP_FORTRESS_TOWER_1:
                        case GO_WINTERGRASP_FORTRESS_TOWER_2:
                        case GO_WINTERGRASP_FORTRESS_TOWER_3:
                        case GO_WINTERGRASP_FORTRESS_TOWER_4:
                        {
                            creature->setFaction(WintergraspFaction[m_WG->GetDefenderTeam()]);
                            break;
                        }
                        case GO_WINTERGRASP_SHADOWSIGHT_TOWER:
                        case GO_WINTERGRASP_WINTER_S_EDGE_TOWER:
                        case GO_WINTERGRASP_FLAMEWATCH_TOWER:
                        {
                            creature->setFaction(WintergraspFaction[m_WG->GetAttackerTeam()]);
                            break;
                        }
                    }
                }
            }
        }

        for (GuidSet::const_iterator itr = m_TurretTopList.begin(); itr != m_TurretTopList.end(); ++itr)
        {
            if (Creature* creature = m_WG->GetCreature(*itr))
            {
                if (GameObject* build = m_WG->GetGameObject(m_BuildGUID))
                {
                    if (disable)
                        m_WG->HideNpc(creature);
                    else
                        m_WG->ShowNpc(creature, true);

                    switch (build->GetEntry())
                    {
                        case GO_WINTERGRASP_FORTRESS_TOWER_1:
                        case GO_WINTERGRASP_FORTRESS_TOWER_2:
                        case GO_WINTERGRASP_FORTRESS_TOWER_3:
                        case GO_WINTERGRASP_FORTRESS_TOWER_4:
                        {
                            creature->setFaction(WintergraspFaction[m_WG->GetDefenderTeam()]);
                            break;
                        }
                        case GO_WINTERGRASP_SHADOWSIGHT_TOWER:
                        case GO_WINTERGRASP_WINTER_S_EDGE_TOWER:
                        case GO_WINTERGRASP_FLAMEWATCH_TOWER:
                        {
                            creature->setFaction(WintergraspFaction[m_WG->GetAttackerTeam()]);
                            break;
                        }
                    }
                }
            }
        }
    }

    void Save()
    {
        sWorld->setWorldState(m_WorldState, m_State);
    }
};

struct WGWorkshop
{
    // pointer to the battlefield that the workshop belongs to
    BattlefieldWG* bf;
    // id of the workshop, useful to retrieve data of the WorkshopsData array
    uint8 workshopId;
    // team that controls the node
    uint8 teamControl;
    // for worldstate
    uint32 state;

    WGWorkshop(BattlefieldWG* _bf, uint8 _workshopId)
    {
        ASSERT(_bf || _workshopId < WG_MAX_WORKSHOP);

        bf = _bf;
        workshopId = _workshopId;
        teamControl = BATTLEFIELD_WG_TEAM_NEUTRAL;
        state = BATTLEFIELD_WG_OBJECTSTATE_NONE;
    }

    void GiveControlTo(uint8 team, bool init /* for first call in setup*/)
    {
        switch (team)
        {
            case BATTLEFIELD_WG_TEAM_NEUTRAL:
            {
                // Send warning message to all player to inform a faction attack to a workshop
                // alliance / horde attacking a workshop
				state = team == BATTLEFIELD_WG_TEAM_NEUTRAL ? BATTLEFIELD_WG_OBJECTSTATE_NEUTRAL_INTACT : BATTLEFIELD_WG_OBJECTSTATE_NEUTRAL_INTACT;
				bf->SendUpdateWorldState(WorkshopsData[workshopId].worldstate, state);

				bf->SendWarningToAllInZone(teamControl ? WorkshopsData[workshopId].text : WorkshopsData[workshopId].text + 1);

				// Found associate graveyard and update it
                if (workshopId < BATTLEFIELD_WG_WORKSHOP_KEEP_WEST)
                    if (bf->GetGraveyardById(workshopId))
                        bf->GetGraveyardById(workshopId)->GiveControlTo(team == BATTLEFIELD_WG_TEAM_NEUTRAL ? TEAM_NEUTRAL : TEAM_NEUTRAL);

				teamControl = team;
                break;
            }
            case BATTLEFIELD_WG_TEAM_ALLIANCE:
            case BATTLEFIELD_WG_TEAM_HORDE:
            {
                // Updating worldstate
                state = team == BATTLEFIELD_WG_TEAM_ALLIANCE ? BATTLEFIELD_WG_OBJECTSTATE_ALLIANCE_INTACT : BATTLEFIELD_WG_OBJECTSTATE_HORDE_INTACT;
                bf->SendUpdateWorldState(WorkshopsData[workshopId].worldstate, state);

                // Warning message
                if (!init)                              // workshop taken - alliance
                    bf->SendWarningToAllInZone(team == BATTLEFIELD_WG_TEAM_ALLIANCE ? WorkshopsData[workshopId].text : WorkshopsData[workshopId].text+1);

                // Found associate graveyard and update it
                if (workshopId < BATTLEFIELD_WG_WORKSHOP_KEEP_WEST)
                    if (bf->GetGraveyardById(workshopId))
                        bf->GetGraveyardById(workshopId)->GiveControlTo(team == BATTLEFIELD_WG_TEAM_ALLIANCE ? TEAM_ALLIANCE : TEAM_HORDE);

                teamControl = team;
                break;
            }
        }

        if (!init)
            bf->UpdateCounterVehicle(false);
    }

    void UpdateGraveyardAndWorkshop()
    {
        if (workshopId < BATTLEFIELD_WG_WORKSHOP_KEEP_WEST)
            bf->GetGraveyardById(workshopId)->GiveControlTo(TeamId(teamControl));
        else
            GiveControlTo(bf->GetDefenderTeam(), true);
    }

    void Save()
    {
        sWorld->setWorldState(WorkshopsData[workshopId].worldstate, state);
    }
};

// Structure for the 6 workshop
struct WintergraspWorkshopData
{
    BattlefieldWG* m_WG;                                    // Pointer to wintergrasp
    uint64 m_BuildGUID;
    uint32 m_Type;
    uint32 m_State;                                         // For worldstate
    uint32 m_WorldState;
    uint32 m_TeamControl;                                   // Team witch control the workshop
    GuidSet m_CreatureOnPoint[2];                           // Contain all Creature associate to this point
    GuidSet m_GameObjectOnPoint[2];                     // Contain all Gameobject associate to this point
    uint32 m_NameId;                                        // Id of trinity_string witch contain name of this node, using for alert message

    WintergraspWorkshopData(BattlefieldWG* WG)
    {
        m_WG = WG;
        m_BuildGUID = 0;
        m_Type = 0;
        m_State = 0;
        m_WorldState = 0;
        m_TeamControl = 0;
        m_NameId = 0;
    }

    // Spawning associate creature and store them
    void AddCreature(const WintergraspObjectPositionData& obj)
    {
        if (Creature* creature = m_WG->SpawnCreature(obj.entryHorde, obj.x, obj.y, obj.z, obj.o, TEAM_HORDE))
            m_CreatureOnPoint[TEAM_HORDE].insert(creature->GetGUID());

        if (Creature* creature = m_WG->SpawnCreature(obj.entryAlliance, obj.x, obj.y, obj.z, obj.o, TEAM_ALLIANCE))
            m_CreatureOnPoint[TEAM_ALLIANCE].insert(creature->GetGUID());
    }

    // Spawning Associate gameobject and store them
    void AddGameObject(const WintergraspObjectPositionData& obj)
    {
        if (GameObject* gameobject = m_WG->SpawnGameObject(obj.entryHorde, obj.x, obj.y, obj.z, obj.o))
            m_GameObjectOnPoint[TEAM_HORDE].insert(gameobject->GetGUID());
        if (GameObject* gameobject = m_WG->SpawnGameObject(obj.entryAlliance, obj.x, obj.y, obj.z, obj.o))
            m_GameObjectOnPoint[TEAM_ALLIANCE].insert(gameobject->GetGUID());
    }

    // Init method, setup variable
    void Init(uint32 worldstate, uint32 type, uint32 nameid)
    {
        m_WorldState = worldstate;
        m_Type = type;
        m_NameId = nameid;
    }

    // Called on change faction in CapturePoint class
    void GiveControlTo(uint8 team, bool init /* for first call in setup*/)
    {
        switch (team)
        {
            case BATTLEFIELD_WG_TEAM_NEUTRAL:
            {
                // Send warning message to all player for inform a faction attack a workshop
                // alliance / horde attacking workshop
                m_WG->SendWarningToAllInZone(m_TeamControl ? m_NameId : m_NameId + 1);

				for (GuidSet::const_iterator itr = m_CreatureOnPoint[TEAM_ALLIANCE].begin(); itr != m_CreatureOnPoint[TEAM_ALLIANCE].end(); ++itr)
                    if (Creature* creature = m_WG->GetCreature(*itr))
						m_WG->HideNpc(creature);

				// Hide Horde creature
                for (GuidSet::const_iterator itr = m_CreatureOnPoint[TEAM_HORDE].begin(); itr != m_CreatureOnPoint[TEAM_HORDE].end(); ++itr)
                    if (Creature* creature = m_WG->GetCreature(*itr))
						m_WG->HideNpc(creature);

                m_TeamControl = team;
                break;
            }
            case BATTLEFIELD_WG_TEAM_ALLIANCE:
            {
                // Show Alliance creature
                for (GuidSet::const_iterator itr = m_CreatureOnPoint[TEAM_ALLIANCE].begin(); itr != m_CreatureOnPoint[TEAM_ALLIANCE].end(); ++itr)
                    if (Creature* creature = m_WG->GetCreature(*itr))
                        m_WG->ShowNpc(creature, creature->GetEntry() != 30499);

                // Hide Horde creature
                for (GuidSet::const_iterator itr = m_CreatureOnPoint[TEAM_HORDE].begin(); itr != m_CreatureOnPoint[TEAM_HORDE].end(); ++itr)
                    if (Creature* creature = m_WG->GetCreature(*itr))
                        m_WG->HideNpc(creature);

                // Show Alliance gameobject
                for (GuidSet::const_iterator itr = m_GameObjectOnPoint[TEAM_ALLIANCE].begin(); itr != m_GameObjectOnPoint[TEAM_ALLIANCE].end(); ++itr)
                    if (GameObject* object = m_WG->GetGameObject(*itr))
                        object->SetRespawnTime(RESPAWN_IMMEDIATELY);

                // Hide Horde gameobject
                for (GuidSet::const_iterator itr = m_GameObjectOnPoint[TEAM_HORDE].begin(); itr != m_GameObjectOnPoint[TEAM_HORDE].end(); ++itr)
                    if (GameObject* object = m_WG->GetGameObject(*itr))
                        object->SetRespawnTime(RESPAWN_ONE_DAY);


                // Updating worldstate
                m_State = BATTLEFIELD_WG_OBJECTSTATE_ALLIANCE_INTACT;
                m_WG->SendUpdateWorldState(m_WorldState, m_State);

                // Warning message
                if (!init)                              // workshop taken - alliance
                    m_WG->SendWarningToAllInZone(m_NameId);

                // Found associate graveyard and update it
                if (m_Type < BATTLEFIELD_WG_WORKSHOP_KEEP_WEST)
                    if (m_WG->GetGraveyardById(m_Type))
                        m_WG->GetGraveyardById(m_Type)->GiveControlTo(TEAM_ALLIANCE);

                m_TeamControl = team;
                break;
            }
            case BATTLEFIELD_WG_TEAM_HORDE:
            {
                // Show Horde creature
                for (GuidSet::const_iterator itr = m_CreatureOnPoint[TEAM_HORDE].begin(); itr != m_CreatureOnPoint[TEAM_HORDE].end(); ++itr)
                    if (Creature* creature = m_WG->GetCreature(*itr))
                        m_WG->ShowNpc(creature, creature->GetEntry() != 30400);

                // Hide Alliance creature
                for (GuidSet::const_iterator itr = m_CreatureOnPoint[TEAM_ALLIANCE].begin(); itr != m_CreatureOnPoint[TEAM_ALLIANCE].end(); ++itr)
                    if (Creature* creature = m_WG->GetCreature(*itr))
                        m_WG->HideNpc(creature);

                // Hide Alliance gameobject
                for (GuidSet::const_iterator itr = m_GameObjectOnPoint[TEAM_ALLIANCE].begin(); itr != m_GameObjectOnPoint[TEAM_ALLIANCE].end(); ++itr)
                    if (GameObject* object = m_WG->GetGameObject(*itr))
                        object->SetRespawnTime(RESPAWN_ONE_DAY);

                // Show Horde gameobject
                for (GuidSet::const_iterator itr = m_GameObjectOnPoint[TEAM_HORDE].begin(); itr != m_GameObjectOnPoint[TEAM_HORDE].end(); ++itr)
                    if (GameObject* object = m_WG->GetGameObject(*itr))
                        object->SetRespawnTime(RESPAWN_IMMEDIATELY);

                // Update worlstate
                m_State = BATTLEFIELD_WG_OBJECTSTATE_HORDE_INTACT;
                m_WG->SendUpdateWorldState(m_WorldState, m_State);

                // Warning message
                if (!init)                              // workshop taken - horde
                    m_WG->SendWarningToAllInZone(m_NameId + 1);

                // Update graveyard control
                if (m_Type < BATTLEFIELD_WG_WORKSHOP_KEEP_WEST)
                    if (m_WG->GetGraveyardById(m_Type))
                        m_WG->GetGraveyardById(m_Type)->GiveControlTo(TEAM_HORDE);

                m_TeamControl = team;
                break;
            }
        }
        if (!init)
            m_WG->UpdateCounterVehicle(false);
    }

    void UpdateGraveyardAndWorkshop()
    {
        if (m_Type < BATTLEFIELD_WG_WORKSHOP_KEEP_WEST)
            m_WG->GetGraveyardById(m_Type)->GiveControlTo(TeamId(m_TeamControl));
        else
            GiveControlTo(m_WG->GetDefenderTeam(), true);
    }

    void Save()
    {
        sWorld->setWorldState(m_WorldState, m_State);
    }
};

#endif
