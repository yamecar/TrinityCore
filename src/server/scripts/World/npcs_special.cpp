/*
 * Copyright (C) 2008-2014 TrinityCore <http://www.trinitycore.org/>
 * Copyright (C) 2006-2009 ScriptDev2 <https://scriptdev2.svn.sourceforge.net/>
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

/* ScriptData
SDName: Npcs_Special
SD%Complete: 100
SDComment: To be used for special NPCs that are located globally.
SDCategory: NPCs
EndScriptData
*/

/* ContentData
npc_air_force_bots       80%    support for misc (invisible) guard bots in areas where player allowed to fly. Summon guards after a preset time if tagged by spell
npc_lunaclaw_spirit      80%    support for quests 6001/6002 (Body and Heart)
npc_chicken_cluck       100%    support for quest 3861 (Cluck!)
npc_dancing_flames      100%    midsummer event NPC
npc_guardian            100%    guardianAI used to prevent players from accessing off-limits areas. Not in use by SD2
npc_garments_of_quests   80%    NPC's related to all Garments of-quests 5621, 5624, 5625, 5648, 565
npc_injured_patient     100%    patients for triage-quests (6622 and 6624)
npc_doctor              100%    Gustaf Vanhowzen and Gregory Victor, quest 6622 and 6624 (Triage)
npc_mount_vendor        100%    Regular mount vendors all over the world. Display gossip if player doesn't meet the requirements to buy
npc_sayge               100%    Darkmoon event fortune teller, buff player based on answers given
npc_snake_trap_serpents  80%    AI for snakes that summoned by Snake Trap
npc_shadowfiend         100%   restore 5% of owner's mana when shadowfiend die from damage
npc_locksmith            75%    list of keys needs to be confirmed
npc_firework            100%    NPC's summoned by rockets and rocket clusters, for making them cast visual
EndContentData */

#include "ScriptMgr.h"
#include "ScriptedCreature.h"
#include "ScriptedGossip.h"
#include "ScriptedEscortAI.h"
#include "ObjectMgr.h"
#include "ScriptMgr.h"
#include "World.h"
#include "PassiveAI.h"
#include "GameEventMgr.h"
#include "GridNotifiers.h"
#include "GridNotifiersImpl.h"
#include "Cell.h"
#include "CellImpl.h"
#include "SpellAuras.h"
#include "Pet.h"
#include "CreatureTextMgr.h"
#include "BattlegroundAV.h"
#include "BattlefieldMgr.h"
#include "BattlefieldWG.h"
#include "Battlefield.h"
#include "GameObjectAI.h"
#include "SmartAI.h"
#include "Vehicle.h"
#include "MoveSplineInit.h"

/*########
# npc_air_force_bots
#########*/

enum SpawnType
{
    SPAWNTYPE_TRIPWIRE_ROOFTOP,                             // no warning, summon Creature at smaller range
    SPAWNTYPE_ALARMBOT,                                     // cast guards mark and summon npc - if player shows up with that buff duration < 5 seconds attack
};

struct SpawnAssociation
{
    uint32 thisCreatureEntry;
    uint32 spawnedCreatureEntry;
    SpawnType spawnType;
};

enum AirFoceBots
{
    SPELL_GUARDS_MARK               = 38067,
    AURA_DURATION_TIME_LEFT         = 5000
};

float const RANGE_TRIPWIRE          = 15.0f;
float const RANGE_GUARDS_MARK       = 50.0f;

SpawnAssociation spawnAssociations[] =
{
    {2614,  15241, SPAWNTYPE_ALARMBOT},                     //Air Force Alarm Bot (Alliance)
    {2615,  15242, SPAWNTYPE_ALARMBOT},                     //Air Force Alarm Bot (Horde)
    {21974, 21976, SPAWNTYPE_ALARMBOT},                     //Air Force Alarm Bot (Area 52)
    {21993, 15242, SPAWNTYPE_ALARMBOT},                     //Air Force Guard Post (Horde - Bat Rider)
    {21996, 15241, SPAWNTYPE_ALARMBOT},                     //Air Force Guard Post (Alliance - Gryphon)
    {21997, 21976, SPAWNTYPE_ALARMBOT},                     //Air Force Guard Post (Goblin - Area 52 - Zeppelin)
    {21999, 15241, SPAWNTYPE_TRIPWIRE_ROOFTOP},             //Air Force Trip Wire - Rooftop (Alliance)
    {22001, 15242, SPAWNTYPE_TRIPWIRE_ROOFTOP},             //Air Force Trip Wire - Rooftop (Horde)
    {22002, 15242, SPAWNTYPE_TRIPWIRE_ROOFTOP},             //Air Force Trip Wire - Ground (Horde)
    {22003, 15241, SPAWNTYPE_TRIPWIRE_ROOFTOP},             //Air Force Trip Wire - Ground (Alliance)
    {22063, 21976, SPAWNTYPE_TRIPWIRE_ROOFTOP},             //Air Force Trip Wire - Rooftop (Goblin - Area 52)
    {22065, 22064, SPAWNTYPE_ALARMBOT},                     //Air Force Guard Post (Ethereal - Stormspire)
    {22066, 22067, SPAWNTYPE_ALARMBOT},                     //Air Force Guard Post (Scryer - Dragonhawk)
    {22068, 22064, SPAWNTYPE_TRIPWIRE_ROOFTOP},             //Air Force Trip Wire - Rooftop (Ethereal - Stormspire)
    {22069, 22064, SPAWNTYPE_ALARMBOT},                     //Air Force Alarm Bot (Stormspire)
    {22070, 22067, SPAWNTYPE_TRIPWIRE_ROOFTOP},             //Air Force Trip Wire - Rooftop (Scryer)
    {22071, 22067, SPAWNTYPE_ALARMBOT},                     //Air Force Alarm Bot (Scryer)
    {22078, 22077, SPAWNTYPE_ALARMBOT},                     //Air Force Alarm Bot (Aldor)
    {22079, 22077, SPAWNTYPE_ALARMBOT},                     //Air Force Guard Post (Aldor - Gryphon)
    {22080, 22077, SPAWNTYPE_TRIPWIRE_ROOFTOP},             //Air Force Trip Wire - Rooftop (Aldor)
    {22086, 22085, SPAWNTYPE_ALARMBOT},                     //Air Force Alarm Bot (Sporeggar)
    {22087, 22085, SPAWNTYPE_ALARMBOT},                     //Air Force Guard Post (Sporeggar - Spore Bat)
    {22088, 22085, SPAWNTYPE_TRIPWIRE_ROOFTOP},             //Air Force Trip Wire - Rooftop (Sporeggar)
    {22090, 22089, SPAWNTYPE_ALARMBOT},                     //Air Force Guard Post (Toshley's Station - Flying Machine)
    {22124, 22122, SPAWNTYPE_ALARMBOT},                     //Air Force Alarm Bot (Cenarion)
    {22125, 22122, SPAWNTYPE_ALARMBOT},                     //Air Force Guard Post (Cenarion - Stormcrow)
    {22126, 22122, SPAWNTYPE_ALARMBOT}                      //Air Force Trip Wire - Rooftop (Cenarion Expedition)
};

class npc_air_force_bots : public CreatureScript
{
public:
    npc_air_force_bots() : CreatureScript("npc_air_force_bots") { }

    struct npc_air_force_botsAI : public ScriptedAI
    {
        npc_air_force_botsAI(Creature* creature) : ScriptedAI(creature)
        {
            SpawnAssoc = NULL;
            SpawnedGUID = 0;

            // find the correct spawnhandling
            static uint32 entryCount = sizeof(spawnAssociations) / sizeof(SpawnAssociation);

            for (uint8 i = 0; i < entryCount; ++i)
            {
                if (spawnAssociations[i].thisCreatureEntry == creature->GetEntry())
                {
                    SpawnAssoc = &spawnAssociations[i];
                    break;
                }
            }

            if (!SpawnAssoc)
                TC_LOG_ERROR("sql.sql", "TCSR: Creature template entry %u has ScriptName npc_air_force_bots, but it's not handled by that script", creature->GetEntry());
            else
            {
                CreatureTemplate const* spawnedTemplate = sObjectMgr->GetCreatureTemplate(SpawnAssoc->spawnedCreatureEntry);

                if (!spawnedTemplate)
                {
                    TC_LOG_ERROR("sql.sql", "TCSR: Creature template entry %u does not exist in DB, which is required by npc_air_force_bots", SpawnAssoc->spawnedCreatureEntry);
                    SpawnAssoc = NULL;
                    return;
                }
            }
        }

        SpawnAssociation* SpawnAssoc;
        uint64 SpawnedGUID;

        void Reset() override { }

        Creature* SummonGuard()
        {
            Creature* summoned = me->SummonCreature(SpawnAssoc->spawnedCreatureEntry, 0.0f, 0.0f, 0.0f, 0.0f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 300000);

            if (summoned)
                SpawnedGUID = summoned->GetGUID();
            else
            {
                TC_LOG_ERROR("sql.sql", "TCSR: npc_air_force_bots: wasn't able to spawn Creature %u", SpawnAssoc->spawnedCreatureEntry);
                SpawnAssoc = NULL;
            }

            return summoned;
        }

        Creature* GetSummonedGuard()
        {
            Creature* creature = ObjectAccessor::GetCreature(*me, SpawnedGUID);

            if (creature && creature->IsAlive())
                return creature;

            return NULL;
        }

        void MoveInLineOfSight(Unit* who) override
        {
            if (!SpawnAssoc)
                return;

            if (me->IsValidAttackTarget(who))
            {
                Player* playerTarget = who->ToPlayer();

                // airforce guards only spawn for players
                if (!playerTarget)
                    return;

                Creature* lastSpawnedGuard = SpawnedGUID == 0 ? NULL : GetSummonedGuard();

                // prevent calling Unit::GetUnit at next MoveInLineOfSight call - speedup
                if (!lastSpawnedGuard)
                    SpawnedGUID = 0;

                switch (SpawnAssoc->spawnType)
                {
                    case SPAWNTYPE_ALARMBOT:
                    {
                        if (!who->IsWithinDistInMap(me, RANGE_GUARDS_MARK))
                            return;

                        Aura* markAura = who->GetAura(SPELL_GUARDS_MARK);
                        if (markAura)
                        {
                            // the target wasn't able to move out of our range within 25 seconds
                            if (!lastSpawnedGuard)
                            {
                                lastSpawnedGuard = SummonGuard();

                                if (!lastSpawnedGuard)
                                    return;
                            }

                            if (markAura->GetDuration() < AURA_DURATION_TIME_LEFT)
                                if (!lastSpawnedGuard->GetVictim())
                                    lastSpawnedGuard->AI()->AttackStart(who);
                        }
                        else
                        {
                            if (!lastSpawnedGuard)
                                lastSpawnedGuard = SummonGuard();

                            if (!lastSpawnedGuard)
                                return;

                            lastSpawnedGuard->CastSpell(who, SPELL_GUARDS_MARK, true);
                        }
                        break;
                    }
                    case SPAWNTYPE_TRIPWIRE_ROOFTOP:
                    {
                        if (!who->IsWithinDistInMap(me, RANGE_TRIPWIRE))
                            return;

                        if (!lastSpawnedGuard)
                            lastSpawnedGuard = SummonGuard();

                        if (!lastSpawnedGuard)
                            return;

                        // ROOFTOP only triggers if the player is on the ground
                        if (!playerTarget->IsFlying() && !lastSpawnedGuard->GetVictim())
                            lastSpawnedGuard->AI()->AttackStart(who);

                        break;
                    }
                }
            }
        }
    };

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new npc_air_force_botsAI(creature);
    }
};

/*######
## npc_lunaclaw_spirit
######*/

enum LunaclawSpirit
{
    QUEST_BODY_HEART_A      = 6001,
    QUEST_BODY_HEART_H      = 6002,

    TEXT_ID_DEFAULT         = 4714,
    TEXT_ID_PROGRESS        = 4715
};

#define GOSSIP_ITEM_GRANT   "You have thought well, spirit. I ask you to grant me the strength of your body and the strength of your heart."

class npc_lunaclaw_spirit : public CreatureScript
{
public:
    npc_lunaclaw_spirit() : CreatureScript("npc_lunaclaw_spirit") { }

    bool OnGossipHello(Player* player, Creature* creature) override
    {
        if (player->GetQuestStatus(QUEST_BODY_HEART_A) == QUEST_STATUS_INCOMPLETE || player->GetQuestStatus(QUEST_BODY_HEART_H) == QUEST_STATUS_INCOMPLETE)
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_GRANT, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);

        player->SEND_GOSSIP_MENU(TEXT_ID_DEFAULT, creature->GetGUID());
        return true;
    }

    bool OnGossipSelect(Player* player, Creature* creature, uint32 /*sender*/, uint32 action) override
    {
        player->PlayerTalkClass->ClearMenus();
        if (action == GOSSIP_ACTION_INFO_DEF + 1)
        {
            player->SEND_GOSSIP_MENU(TEXT_ID_PROGRESS, creature->GetGUID());
            player->AreaExploredOrEventHappens(player->GetTeam() == ALLIANCE ? QUEST_BODY_HEART_A : QUEST_BODY_HEART_H);
        }
        return true;
    }
};

/*########
# npc_chicken_cluck
#########*/

enum ChickenCluck
{
    EMOTE_HELLO_A       = 0,
    EMOTE_HELLO_H       = 1,
    EMOTE_CLUCK_TEXT    = 2,

    QUEST_CLUCK         = 3861,
    FACTION_FRIENDLY    = 35,
    FACTION_CHICKEN     = 31
};

class npc_chicken_cluck : public CreatureScript
{
public:
    npc_chicken_cluck() : CreatureScript("npc_chicken_cluck") { }

    struct npc_chicken_cluckAI : public ScriptedAI
    {
        npc_chicken_cluckAI(Creature* creature) : ScriptedAI(creature) { }

        uint32 ResetFlagTimer;

        void Reset() override
        {
            ResetFlagTimer = 120000;
            me->setFaction(FACTION_CHICKEN);
            me->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_QUESTGIVER);
        }

        void EnterCombat(Unit* /*who*/) override { }

        void UpdateAI(uint32 diff) override
        {
            // Reset flags after a certain time has passed so that the next player has to start the 'event' again
            if (me->HasFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_QUESTGIVER))
            {
                if (ResetFlagTimer <= diff)
                {
                    EnterEvadeMode();
                    return;
                }
                else
                    ResetFlagTimer -= diff;
            }

            if (UpdateVictim())
                DoMeleeAttackIfReady();
        }

        void ReceiveEmote(Player* player, uint32 emote) override
        {
            switch (emote)
            {
                case TEXT_EMOTE_CHICKEN:
                    if (player->GetQuestStatus(QUEST_CLUCK) == QUEST_STATUS_NONE && rand() % 30 == 1)
                    {
                        me->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_QUESTGIVER);
                        me->setFaction(FACTION_FRIENDLY);
                        Talk(player->GetTeam() == HORDE ? EMOTE_HELLO_H : EMOTE_HELLO_A);
                    }
                    break;
                case TEXT_EMOTE_CHEER:
                    if (player->GetQuestStatus(QUEST_CLUCK) == QUEST_STATUS_COMPLETE)
                    {
                        me->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_QUESTGIVER);
                        me->setFaction(FACTION_FRIENDLY);
                        Talk(EMOTE_CLUCK_TEXT);
                    }
                    break;
            }
        }
    };

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new npc_chicken_cluckAI(creature);
    }

    bool OnQuestAccept(Player* /*player*/, Creature* creature, Quest const* quest) override
    {
        if (quest->GetQuestId() == QUEST_CLUCK)
            CAST_AI(npc_chicken_cluck::npc_chicken_cluckAI, creature->AI())->Reset();

        return true;
    }

    bool OnQuestComplete(Player* /*player*/, Creature* creature, Quest const* quest) override
    {
        if (quest->GetQuestId() == QUEST_CLUCK)
            CAST_AI(npc_chicken_cluck::npc_chicken_cluckAI, creature->AI())->Reset();

        return true;
    }
};

/*######
## npc_dancing_flames
######*/

enum DancingFlames
{
    SPELL_BRAZIER           = 45423,
    SPELL_SEDUCTION         = 47057,
    SPELL_FIERY_AURA        = 45427
};

class npc_dancing_flames : public CreatureScript
{
public:
    npc_dancing_flames() : CreatureScript("npc_dancing_flames") { }

    struct npc_dancing_flamesAI : public ScriptedAI
    {
        npc_dancing_flamesAI(Creature* creature) : ScriptedAI(creature) { }

        bool Active;
        uint32 CanIteract;

        void Reset() override
        {
            Active = true;
            CanIteract = 3500;
            DoCast(me, SPELL_BRAZIER, true);
            DoCast(me, SPELL_FIERY_AURA, false);
            float x, y, z;
            me->GetPosition(x, y, z);
            me->Relocate(x, y, z + 0.94f);
            me->SetDisableGravity(true);
            me->HandleEmoteCommand(EMOTE_ONESHOT_DANCE);
            WorldPacket data;                       //send update position to client
            me->BuildHeartBeatMsg(&data);
            me->SendMessageToSet(&data, true);
        }

        void UpdateAI(uint32 diff) override
        {
            if (!Active)
            {
                if (CanIteract <= diff)
                {
                    Active = true;
                    CanIteract = 3500;
                    me->HandleEmoteCommand(EMOTE_ONESHOT_DANCE);
                }
                else
                    CanIteract -= diff;
            }
        }

        void EnterCombat(Unit* /*who*/) override { }

        void ReceiveEmote(Player* player, uint32 emote) override
        {
            if (me->IsWithinLOS(player->GetPositionX(), player->GetPositionY(), player->GetPositionZ()) && me->IsWithinDistInMap(player, 30.0f))
            {
                me->SetInFront(player);
                Active = false;

                WorldPacket data;
                me->BuildHeartBeatMsg(&data);
                me->SendMessageToSet(&data, true);
                switch (emote)
                {
                    case TEXT_EMOTE_KISS:
                        me->HandleEmoteCommand(EMOTE_ONESHOT_SHY);
                        break;
                    case TEXT_EMOTE_WAVE:
                        me->HandleEmoteCommand(EMOTE_ONESHOT_WAVE);
                        break;
                    case TEXT_EMOTE_BOW:
                        me->HandleEmoteCommand(EMOTE_ONESHOT_BOW);
                        break;
                    case TEXT_EMOTE_JOKE:
                        me->HandleEmoteCommand(EMOTE_ONESHOT_LAUGH);
                        break;
                    case TEXT_EMOTE_DANCE:
                        if (!player->HasAura(SPELL_SEDUCTION))
                            DoCast(player, SPELL_SEDUCTION, true);
                        break;
                }
            }
        }
    };

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new npc_dancing_flamesAI(creature);
    }
};

/*######
## Triage quest
######*/

enum Doctor
{
    SAY_DOC             = 0,

    DOCTOR_ALLIANCE     = 12939,
    DOCTOR_HORDE        = 12920,
    ALLIANCE_COORDS     = 7,
    HORDE_COORDS        = 6
};

struct Location
{
    float x, y, z, o;
};

static Location AllianceCoords[]=
{
    {-3757.38f, -4533.05f, 14.16f, 3.62f},                      // Top-far-right bunk as seen from entrance
    {-3754.36f, -4539.13f, 14.16f, 5.13f},                      // Top-far-left bunk
    {-3749.54f, -4540.25f, 14.28f, 3.34f},                      // Far-right bunk
    {-3742.10f, -4536.85f, 14.28f, 3.64f},                      // Right bunk near entrance
    {-3755.89f, -4529.07f, 14.05f, 0.57f},                      // Far-left bunk
    {-3749.51f, -4527.08f, 14.07f, 5.26f},                      // Mid-left bunk
    {-3746.37f, -4525.35f, 14.16f, 5.22f},                      // Left bunk near entrance
};

//alliance run to where
#define A_RUNTOX -3742.96f
#define A_RUNTOY -4531.52f
#define A_RUNTOZ 11.91f

static Location HordeCoords[]=
{
    {-1013.75f, -3492.59f, 62.62f, 4.34f},                      // Left, Behind
    {-1017.72f, -3490.92f, 62.62f, 4.34f},                      // Right, Behind
    {-1015.77f, -3497.15f, 62.82f, 4.34f},                      // Left, Mid
    {-1019.51f, -3495.49f, 62.82f, 4.34f},                      // Right, Mid
    {-1017.25f, -3500.85f, 62.98f, 4.34f},                      // Left, front
    {-1020.95f, -3499.21f, 62.98f, 4.34f}                       // Right, Front
};

//horde run to where
#define H_RUNTOX -1016.44f
#define H_RUNTOY -3508.48f
#define H_RUNTOZ 62.96f

uint32 const AllianceSoldierId[3] =
{
    12938,                                                  // 12938 Injured Alliance Soldier
    12936,                                                  // 12936 Badly injured Alliance Soldier
    12937                                                   // 12937 Critically injured Alliance Soldier
};

uint32 const HordeSoldierId[3] =
{
    12923,                                                  //12923 Injured Soldier
    12924,                                                  //12924 Badly injured Soldier
    12925                                                   //12925 Critically injured Soldier
};

/*######
## npc_doctor (handles both Gustaf Vanhowzen and Gregory Victor)
######*/
class npc_doctor : public CreatureScript
{
public:
    npc_doctor() : CreatureScript("npc_doctor") { }

    struct npc_doctorAI : public ScriptedAI
    {
        npc_doctorAI(Creature* creature) : ScriptedAI(creature) { }

        uint64 PlayerGUID;

        uint32 SummonPatientTimer;
        uint32 SummonPatientCount;
        uint32 PatientDiedCount;
        uint32 PatientSavedCount;

        bool Event;

        std::list<uint64> Patients;
        std::vector<Location*> Coordinates;

        void Reset() override
        {
            PlayerGUID = 0;

            SummonPatientTimer = 10000;
            SummonPatientCount = 0;
            PatientDiedCount = 0;
            PatientSavedCount = 0;

            Patients.clear();
            Coordinates.clear();

            Event = false;

            me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
        }

        void BeginEvent(Player* player)
        {
            PlayerGUID = player->GetGUID();

            SummonPatientTimer = 10000;
            SummonPatientCount = 0;
            PatientDiedCount = 0;
            PatientSavedCount = 0;

            switch (me->GetEntry())
            {
                case DOCTOR_ALLIANCE:
                    for (uint8 i = 0; i < ALLIANCE_COORDS; ++i)
                        Coordinates.push_back(&AllianceCoords[i]);
                    break;
                case DOCTOR_HORDE:
                    for (uint8 i = 0; i < HORDE_COORDS; ++i)
                        Coordinates.push_back(&HordeCoords[i]);
                    break;
            }

            Event = true;
            me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
        }

        void PatientDied(Location* point)
        {
            Player* player = ObjectAccessor::GetPlayer(*me, PlayerGUID);
            if (player && ((player->GetQuestStatus(6624) == QUEST_STATUS_INCOMPLETE) || (player->GetQuestStatus(6622) == QUEST_STATUS_INCOMPLETE)))
            {
                ++PatientDiedCount;

                if (PatientDiedCount > 5 && Event)
                {
                    if (player->GetQuestStatus(6624) == QUEST_STATUS_INCOMPLETE)
                        player->FailQuest(6624);
                    else if (player->GetQuestStatus(6622) == QUEST_STATUS_INCOMPLETE)
                        player->FailQuest(6622);

                    Reset();
                    return;
                }

                Coordinates.push_back(point);
            }
            else
                // If no player or player abandon quest in progress
                Reset();
        }

        void PatientSaved(Creature* /*soldier*/, Player* player, Location* point)
        {
            if (player && PlayerGUID == player->GetGUID())
            {
                if ((player->GetQuestStatus(6624) == QUEST_STATUS_INCOMPLETE) || (player->GetQuestStatus(6622) == QUEST_STATUS_INCOMPLETE))
                {
                    ++PatientSavedCount;

                    if (PatientSavedCount == 15)
                    {
                        if (!Patients.empty())
                        {
                            std::list<uint64>::const_iterator itr;
                            for (itr = Patients.begin(); itr != Patients.end(); ++itr)
                            {
                                if (Creature* patient = ObjectAccessor::GetCreature((*me), *itr))
                                    patient->setDeathState(JUST_DIED);
                            }
                        }

                        if (player->GetQuestStatus(6624) == QUEST_STATUS_INCOMPLETE)
                            player->AreaExploredOrEventHappens(6624);
                        else if (player->GetQuestStatus(6622) == QUEST_STATUS_INCOMPLETE)
                            player->AreaExploredOrEventHappens(6622);

                        Reset();
                        return;
                    }

                    Coordinates.push_back(point);
                }
            }
        }

        void UpdateAI(uint32 diff) override;

        void EnterCombat(Unit* /*who*/) override { }
    };

    bool OnQuestAccept(Player* player, Creature* creature, Quest const* quest) override
    {
        if ((quest->GetQuestId() == 6624) || (quest->GetQuestId() == 6622))
            CAST_AI(npc_doctor::npc_doctorAI, creature->AI())->BeginEvent(player);

        return true;
    }

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new npc_doctorAI(creature);
    }
};

/*#####
## npc_injured_patient (handles all the patients, no matter Horde or Alliance)
#####*/

class npc_injured_patient : public CreatureScript
{
public:
    npc_injured_patient() : CreatureScript("npc_injured_patient") { }

    struct npc_injured_patientAI : public ScriptedAI
    {
        npc_injured_patientAI(Creature* creature) : ScriptedAI(creature) { }

        uint64 DoctorGUID;
        Location* Coord;

        void Reset() override
        {
            DoctorGUID = 0;
            Coord = NULL;

            //no select
            me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);

            //no regen health
            me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IN_COMBAT);

            //to make them lay with face down
            me->SetUInt32Value(UNIT_FIELD_BYTES_1, UNIT_STAND_STATE_DEAD);

            uint32 mobId = me->GetEntry();

            switch (mobId)
            {                                                   //lower max health
                case 12923:
                case 12938:                                     //Injured Soldier
                    me->SetHealth(me->CountPctFromMaxHealth(75));
                    break;
                case 12924:
                case 12936:                                     //Badly injured Soldier
                    me->SetHealth(me->CountPctFromMaxHealth(50));
                    break;
                case 12925:
                case 12937:                                     //Critically injured Soldier
                    me->SetHealth(me->CountPctFromMaxHealth(25));
                    break;
            }
        }

        void EnterCombat(Unit* /*who*/) override { }

        void SpellHit(Unit* caster, SpellInfo const* spell) override
        {
            Player* player = caster->ToPlayer();
            if (!player || !me->IsAlive() || spell->Id != 20804)
                return;

            if (player->GetQuestStatus(6624) == QUEST_STATUS_INCOMPLETE || player->GetQuestStatus(6622) == QUEST_STATUS_INCOMPLETE)
                if (DoctorGUID)
                    if (Creature* doctor = ObjectAccessor::GetCreature(*me, DoctorGUID))
                        CAST_AI(npc_doctor::npc_doctorAI, doctor->AI())->PatientSaved(me, player, Coord);

            //make not selectable
            me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);

            //regen health
            me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IN_COMBAT);

            //stand up
            me->SetUInt32Value(UNIT_FIELD_BYTES_1, UNIT_STAND_STATE_STAND);

            Talk(SAY_DOC);

            uint32 mobId = me->GetEntry();
            me->SetWalk(false);

            switch (mobId)
            {
                case 12923:
                case 12924:
                case 12925:
                    me->GetMotionMaster()->MovePoint(0, H_RUNTOX, H_RUNTOY, H_RUNTOZ);
                    break;
                case 12936:
                case 12937:
                case 12938:
                    me->GetMotionMaster()->MovePoint(0, A_RUNTOX, A_RUNTOY, A_RUNTOZ);
                    break;
            }
        }

        void UpdateAI(uint32 /*diff*/) override
        {
            //lower HP on every world tick makes it a useful counter, not officlone though
            if (me->IsAlive() && me->GetHealth() > 6)
                me->ModifyHealth(-5);

            if (me->IsAlive() && me->GetHealth() <= 6)
            {
                me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IN_COMBAT);
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                me->setDeathState(JUST_DIED);
                me->SetFlag(UNIT_DYNAMIC_FLAGS, 32);

                if (DoctorGUID)
                    if (Creature* doctor = ObjectAccessor::GetCreature((*me), DoctorGUID))
                        CAST_AI(npc_doctor::npc_doctorAI, doctor->AI())->PatientDied(Coord);
            }
        }
    };

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new npc_injured_patientAI(creature);
    }
};

void npc_doctor::npc_doctorAI::UpdateAI(uint32 diff)
{
    if (Event && SummonPatientCount >= 20)
    {
        Reset();
        return;
    }

    if (Event)
    {
        if (SummonPatientTimer <= diff)
        {
            if (Coordinates.empty())
                return;

            std::vector<Location*>::iterator itr = Coordinates.begin() + rand() % Coordinates.size();
            uint32 patientEntry = 0;

            switch (me->GetEntry())
            {
                case DOCTOR_ALLIANCE:
                    patientEntry = AllianceSoldierId[rand() % 3];
                    break;
                case DOCTOR_HORDE:
                    patientEntry = HordeSoldierId[rand() % 3];
                    break;
                default:
                    TC_LOG_ERROR("scripts", "Invalid entry for Triage doctor. Please check your database");
                    return;
            }

            if (Location* point = *itr)
            {
                if (Creature* Patient = me->SummonCreature(patientEntry, point->x, point->y, point->z, point->o, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 5000))
                {
                    //303, this flag appear to be required for client side item->spell to work (TARGET_SINGLE_FRIEND)
                    Patient->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PVP_ATTACKABLE);

                    Patients.push_back(Patient->GetGUID());
                    CAST_AI(npc_injured_patient::npc_injured_patientAI, Patient->AI())->DoctorGUID = me->GetGUID();
                    CAST_AI(npc_injured_patient::npc_injured_patientAI, Patient->AI())->Coord = point;

                    Coordinates.erase(itr);
                }
            }
            SummonPatientTimer = 10000;
            ++SummonPatientCount;
        }
        else
            SummonPatientTimer -= diff;
    }
}

/*######
## npc_garments_of_quests
######*/

/// @todo get text for each NPC

enum Garments
{
    SPELL_LESSER_HEAL_R2    = 2052,
    SPELL_FORTITUDE_R1      = 1243,

    QUEST_MOON              = 5621,
    QUEST_LIGHT_1           = 5624,
    QUEST_LIGHT_2           = 5625,
    QUEST_SPIRIT            = 5648,
    QUEST_DARKNESS          = 5650,

    ENTRY_SHAYA             = 12429,
    ENTRY_ROBERTS           = 12423,
    ENTRY_DOLF              = 12427,
    ENTRY_KORJA             = 12430,
    ENTRY_DG_KEL            = 12428,

    // used by 12429, 12423, 12427, 12430, 12428, but signed for 12429
    SAY_THANKS              = 0,
    SAY_GOODBYE             = 1,
    SAY_HEALED              = 2,
};

class npc_garments_of_quests : public CreatureScript
{
public:
    npc_garments_of_quests() : CreatureScript("npc_garments_of_quests") { }

    struct npc_garments_of_questsAI : public npc_escortAI
    {
        npc_garments_of_questsAI(Creature* creature) : npc_escortAI(creature)
        {
            Reset();
        }

        uint64 CasterGUID;

        bool IsHealed;
        bool CanRun;

        uint32 RunAwayTimer;

        void Reset() override
        {
            CasterGUID = 0;

            IsHealed = false;
            CanRun = false;

            RunAwayTimer = 5000;

            me->SetStandState(UNIT_STAND_STATE_KNEEL);
            // expect database to have RegenHealth=0
            me->SetHealth(me->CountPctFromMaxHealth(70));
        }

        void EnterCombat(Unit* /*who*/) override { }

        void SpellHit(Unit* caster, SpellInfo const* spell) override
        {
            if (spell->Id == SPELL_LESSER_HEAL_R2 || spell->Id == SPELL_FORTITUDE_R1)
            {
                //not while in combat
                if (me->IsInCombat())
                    return;

                //nothing to be done now
                if (IsHealed && CanRun)
                    return;

                if (Player* player = caster->ToPlayer())
                {
                    switch (me->GetEntry())
                    {
                        case ENTRY_SHAYA:
                            if (player->GetQuestStatus(QUEST_MOON) == QUEST_STATUS_INCOMPLETE)
                            {
                                if (IsHealed && !CanRun && spell->Id == SPELL_FORTITUDE_R1)
                                {
                                    Talk(SAY_THANKS, caster);
                                    CanRun = true;
                                }
                                else if (!IsHealed && spell->Id == SPELL_LESSER_HEAL_R2)
                                {
                                    CasterGUID = caster->GetGUID();
                                    me->SetStandState(UNIT_STAND_STATE_STAND);
                                    Talk(SAY_HEALED, caster);
                                    IsHealed = true;
                                }
                            }
                            break;
                        case ENTRY_ROBERTS:
                            if (player->GetQuestStatus(QUEST_LIGHT_1) == QUEST_STATUS_INCOMPLETE)
                            {
                                if (IsHealed && !CanRun && spell->Id == SPELL_FORTITUDE_R1)
                                {
                                    Talk(SAY_THANKS, caster);
                                    CanRun = true;
                                }
                                else if (!IsHealed && spell->Id == SPELL_LESSER_HEAL_R2)
                                {
                                    CasterGUID = caster->GetGUID();
                                    me->SetStandState(UNIT_STAND_STATE_STAND);
                                    Talk(SAY_HEALED, caster);
                                    IsHealed = true;
                                }
                            }
                            break;
                        case ENTRY_DOLF:
                            if (player->GetQuestStatus(QUEST_LIGHT_2) == QUEST_STATUS_INCOMPLETE)
                            {
                                if (IsHealed && !CanRun && spell->Id == SPELL_FORTITUDE_R1)
                                {
                                    Talk(SAY_THANKS, caster);
                                    CanRun = true;
                                }
                                else if (!IsHealed && spell->Id == SPELL_LESSER_HEAL_R2)
                                {
                                    CasterGUID = caster->GetGUID();
                                    me->SetStandState(UNIT_STAND_STATE_STAND);
                                    Talk(SAY_HEALED, caster);
                                    IsHealed = true;
                                }
                            }
                            break;
                        case ENTRY_KORJA:
                            if (player->GetQuestStatus(QUEST_SPIRIT) == QUEST_STATUS_INCOMPLETE)
                            {
                                if (IsHealed && !CanRun && spell->Id == SPELL_FORTITUDE_R1)
                                {
                                    Talk(SAY_THANKS, caster);
                                    CanRun = true;
                                }
                                else if (!IsHealed && spell->Id == SPELL_LESSER_HEAL_R2)
                                {
                                    CasterGUID = caster->GetGUID();
                                    me->SetStandState(UNIT_STAND_STATE_STAND);
                                    Talk(SAY_HEALED, caster);
                                    IsHealed = true;
                                }
                            }
                            break;
                        case ENTRY_DG_KEL:
                            if (player->GetQuestStatus(QUEST_DARKNESS) == QUEST_STATUS_INCOMPLETE)
                            {
                                if (IsHealed && !CanRun && spell->Id == SPELL_FORTITUDE_R1)
                                {
                                    Talk(SAY_THANKS, caster);
                                    CanRun = true;
                                }
                                else if (!IsHealed && spell->Id == SPELL_LESSER_HEAL_R2)
                                {
                                    CasterGUID = caster->GetGUID();
                                    me->SetStandState(UNIT_STAND_STATE_STAND);
                                    Talk(SAY_HEALED, caster);
                                    IsHealed = true;
                                }
                            }
                            break;
                    }

                    // give quest credit, not expect any special quest objectives
                    if (CanRun)
                        player->TalkedToCreature(me->GetEntry(), me->GetGUID());
                }
            }
        }

        void WaypointReached(uint32 /*waypointId*/) override
        {

        }

        void UpdateAI(uint32 diff) override
        {
            if (CanRun && !me->IsInCombat())
            {
                if (RunAwayTimer <= diff)
                {
                    if (Unit* unit = ObjectAccessor::GetUnit(*me, CasterGUID))
                    {
                        switch (me->GetEntry())
                        {
                            case ENTRY_SHAYA:
                            case ENTRY_ROBERTS:
                            case ENTRY_DOLF:
                            case ENTRY_KORJA:
                            case ENTRY_DG_KEL:
                                Talk(SAY_GOODBYE, unit);
                                break;
                        }

                        Start(false, true, true);
                    }
                    else
                        EnterEvadeMode();                       //something went wrong

                    RunAwayTimer = 30000;
                }
                else
                    RunAwayTimer -= diff;
            }

            npc_escortAI::UpdateAI(diff);
        }
    };

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new npc_garments_of_questsAI(creature);
    }
};

/*######
## npc_guardian
######*/

enum GuardianSpells
{
    SPELL_DEATHTOUCH            = 5
};

class npc_guardian : public CreatureScript
{
public:
    npc_guardian() : CreatureScript("npc_guardian") { }

    struct npc_guardianAI : public ScriptedAI
    {
        npc_guardianAI(Creature* creature) : ScriptedAI(creature) { }

        void Reset() override
        {
            me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
        }

        void EnterCombat(Unit* /*who*/) override
        {
        }

        void UpdateAI(uint32 /*diff*/) override
        {
            if (!UpdateVictim())
                return;

            if (me->isAttackReady())
            {
                DoCastVictim(SPELL_DEATHTOUCH, true);
                me->resetAttackTimer();
            }
        }
    };

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new npc_guardianAI(creature);
    }
};

/*######
## npc_mount_vendor
######*/

class npc_mount_vendor : public CreatureScript
{
public:
    npc_mount_vendor() : CreatureScript("npc_mount_vendor") { }

    bool OnGossipHello(Player* player, Creature* creature) override
    {
        if (creature->IsQuestGiver())
            player->PrepareQuestMenu(creature->GetGUID());

        bool canBuy = false;
        uint32 vendor = creature->GetEntry();
        uint8 race = player->getRace();

        switch (vendor)
        {
            case 384:                                           //Katie Hunter
            case 1460:                                          //Unger Statforth
            case 2357:                                          //Merideth Carlson
            case 4885:                                          //Gregor MacVince
                if (player->GetReputationRank(72) != REP_EXALTED && race != RACE_HUMAN)
                    player->SEND_GOSSIP_MENU(5855, creature->GetGUID());
                else canBuy = true;
                break;
            case 1261:                                          //Veron Amberstill
                if (player->GetReputationRank(47) != REP_EXALTED && race != RACE_DWARF)
                    player->SEND_GOSSIP_MENU(5856, creature->GetGUID());
                else canBuy = true;
                break;
            case 3362:                                          //Ogunaro Wolfrunner
                if (player->GetReputationRank(76) != REP_EXALTED && race != RACE_ORC)
                    player->SEND_GOSSIP_MENU(5841, creature->GetGUID());
                else canBuy = true;
                break;
            case 3685:                                          //Harb Clawhoof
                if (player->GetReputationRank(81) != REP_EXALTED && race != RACE_TAUREN)
                    player->SEND_GOSSIP_MENU(5843, creature->GetGUID());
                else canBuy = true;
                break;
            case 4730:                                          //Lelanai
                if (player->GetReputationRank(69) != REP_EXALTED && race != RACE_NIGHTELF)
                    player->SEND_GOSSIP_MENU(5844, creature->GetGUID());
                else canBuy = true;
                break;
            case 4731:                                          //Zachariah Post
                if (player->GetReputationRank(68) != REP_EXALTED && race != RACE_UNDEAD_PLAYER)
                    player->SEND_GOSSIP_MENU(5840, creature->GetGUID());
                else canBuy = true;
                break;
            case 7952:                                          //Zjolnir
                if (player->GetReputationRank(530) != REP_EXALTED && race != RACE_TROLL)
                    player->SEND_GOSSIP_MENU(5842, creature->GetGUID());
                else canBuy = true;
                break;
            case 7955:                                          //Milli Featherwhistle
                if (player->GetReputationRank(54) != REP_EXALTED && race != RACE_GNOME)
                    player->SEND_GOSSIP_MENU(5857, creature->GetGUID());
                else canBuy = true;
                break;
            case 16264:                                         //Winaestra
                if (player->GetReputationRank(911) != REP_EXALTED && race != RACE_BLOODELF)
                    player->SEND_GOSSIP_MENU(10305, creature->GetGUID());
                else canBuy = true;
                break;
            case 17584:                                         //Torallius the Pack Handler
                if (player->GetReputationRank(930) != REP_EXALTED && race != RACE_DRAENEI)
                    player->SEND_GOSSIP_MENU(10239, creature->GetGUID());
                else canBuy = true;
                break;
        }

        if (canBuy)
        {
            if (creature->IsVendor())
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_VENDOR, GOSSIP_TEXT_BROWSE_GOODS, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_TRADE);
            player->SEND_GOSSIP_MENU(player->GetGossipTextId(creature), creature->GetGUID());
        }
        return true;
    }

    bool OnGossipSelect(Player* player, Creature* creature, uint32 /*sender*/, uint32 action) override
    {
        player->PlayerTalkClass->ClearMenus();
        if (action == GOSSIP_ACTION_TRADE)
            player->GetSession()->SendListInventory(creature->GetGUID());

        return true;
    }
};


/*######
## npc_sayge
######*/

enum Sayge
{
    SPELL_DMG      = 23768, // dmg
    SPELL_RES      = 23769, // res
    SPELL_ARM      = 23767, // arm
    SPELL_SPI      = 23738, // spi
    SPELL_INT      = 23766, // int
    SPELL_STM      = 23737, // stm
    SPELL_STR      = 23735, // str
    SPELL_AGI      = 23736, // agi
    SPELL_FORTUNE  = 23765  // faire fortune
};

#define GOSSIP_HELLO_SAYGE          "Yes"
#define GOSSIP_SENDACTION_SAYGE1    "Slay the Man"
#define GOSSIP_SENDACTION_SAYGE2    "Turn him over to liege"
#define GOSSIP_SENDACTION_SAYGE3    "Confiscate the corn"
#define GOSSIP_SENDACTION_SAYGE4    "Let him go and have the corn"
#define GOSSIP_SENDACTION_SAYGE5    "Execute your friend painfully"
#define GOSSIP_SENDACTION_SAYGE6    "Execute your friend painlessly"
#define GOSSIP_SENDACTION_SAYGE7    "Let your friend go"
#define GOSSIP_SENDACTION_SAYGE8    "Confront the diplomat"
#define GOSSIP_SENDACTION_SAYGE9    "Show not so quiet defiance"
#define GOSSIP_SENDACTION_SAYGE10   "Remain quiet"
#define GOSSIP_SENDACTION_SAYGE11   "Speak against your brother openly"
#define GOSSIP_SENDACTION_SAYGE12   "Help your brother in"
#define GOSSIP_SENDACTION_SAYGE13   "Keep your brother out without letting him know"
#define GOSSIP_SENDACTION_SAYGE14   "Take credit, keep gold"
#define GOSSIP_SENDACTION_SAYGE15   "Take credit, share the gold"
#define GOSSIP_SENDACTION_SAYGE16   "Let the knight take credit"
#define GOSSIP_SENDACTION_SAYGE17   "Thanks"

class npc_sayge : public CreatureScript
{
public:
    npc_sayge() : CreatureScript("npc_sayge") { }

    bool OnGossipHello(Player* player, Creature* creature) override
    {
        if (creature->IsQuestGiver())
            player->PrepareQuestMenu(creature->GetGUID());

        if (player->HasSpellCooldown(SPELL_INT) ||
            player->HasSpellCooldown(SPELL_ARM) ||
            player->HasSpellCooldown(SPELL_DMG) ||
            player->HasSpellCooldown(SPELL_RES) ||
            player->HasSpellCooldown(SPELL_STR) ||
            player->HasSpellCooldown(SPELL_AGI) ||
            player->HasSpellCooldown(SPELL_STM) ||
            player->HasSpellCooldown(SPELL_SPI))
            player->SEND_GOSSIP_MENU(7393, creature->GetGUID());
        else
        {
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_HELLO_SAYGE, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
            player->SEND_GOSSIP_MENU(7339, creature->GetGUID());
        }

        return true;
    }

    void SendAction(Player* player, Creature* creature, uint32 action)
    {
        switch (action)
        {
            case GOSSIP_ACTION_INFO_DEF + 1:
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_SENDACTION_SAYGE1,            GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 2);
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_SENDACTION_SAYGE2,            GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 3);
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_SENDACTION_SAYGE3,            GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 4);
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_SENDACTION_SAYGE4,            GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 5);
                player->SEND_GOSSIP_MENU(7340, creature->GetGUID());
                break;
            case GOSSIP_ACTION_INFO_DEF + 2:
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_SENDACTION_SAYGE5,            GOSSIP_SENDER_MAIN + 1, GOSSIP_ACTION_INFO_DEF);
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_SENDACTION_SAYGE6,            GOSSIP_SENDER_MAIN + 2, GOSSIP_ACTION_INFO_DEF);
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_SENDACTION_SAYGE7,            GOSSIP_SENDER_MAIN + 3, GOSSIP_ACTION_INFO_DEF);
                player->SEND_GOSSIP_MENU(7341, creature->GetGUID());
                break;
            case GOSSIP_ACTION_INFO_DEF + 3:
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_SENDACTION_SAYGE8,            GOSSIP_SENDER_MAIN + 4, GOSSIP_ACTION_INFO_DEF);
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_SENDACTION_SAYGE9,            GOSSIP_SENDER_MAIN + 5, GOSSIP_ACTION_INFO_DEF);
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_SENDACTION_SAYGE10,           GOSSIP_SENDER_MAIN + 2, GOSSIP_ACTION_INFO_DEF);
                player->SEND_GOSSIP_MENU(7361, creature->GetGUID());
                break;
            case GOSSIP_ACTION_INFO_DEF + 4:
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_SENDACTION_SAYGE11,           GOSSIP_SENDER_MAIN + 6, GOSSIP_ACTION_INFO_DEF);
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_SENDACTION_SAYGE12,           GOSSIP_SENDER_MAIN + 7, GOSSIP_ACTION_INFO_DEF);
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_SENDACTION_SAYGE13,           GOSSIP_SENDER_MAIN + 8, GOSSIP_ACTION_INFO_DEF);
                player->SEND_GOSSIP_MENU(7362, creature->GetGUID());
                break;
            case GOSSIP_ACTION_INFO_DEF + 5:
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_SENDACTION_SAYGE14,           GOSSIP_SENDER_MAIN + 5, GOSSIP_ACTION_INFO_DEF);
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_SENDACTION_SAYGE15,           GOSSIP_SENDER_MAIN + 4, GOSSIP_ACTION_INFO_DEF);
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_SENDACTION_SAYGE16,           GOSSIP_SENDER_MAIN + 3, GOSSIP_ACTION_INFO_DEF);
                player->SEND_GOSSIP_MENU(7363, creature->GetGUID());
                break;
            case GOSSIP_ACTION_INFO_DEF:
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_SENDACTION_SAYGE17,           GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 6);
                player->SEND_GOSSIP_MENU(7364, creature->GetGUID());
                break;
            case GOSSIP_ACTION_INFO_DEF + 6:
                creature->CastSpell(player, SPELL_FORTUNE, false);
                player->SEND_GOSSIP_MENU(7365, creature->GetGUID());
                break;
        }
    }

    bool OnGossipSelect(Player* player, Creature* creature, uint32 sender, uint32 action) override
    {
        player->PlayerTalkClass->ClearMenus();
        switch (sender)
        {
            case GOSSIP_SENDER_MAIN:
                SendAction(player, creature, action);
                break;
            case GOSSIP_SENDER_MAIN + 1:
                creature->CastSpell(player, SPELL_DMG, false);
                player->AddSpellCooldown(SPELL_DMG, 0, time(NULL) + 7200);
                SendAction(player, creature, action);
                break;
            case GOSSIP_SENDER_MAIN + 2:
                creature->CastSpell(player, SPELL_RES, false);
                player->AddSpellCooldown(SPELL_RES, 0, time(NULL) + 7200);
                SendAction(player, creature, action);
                break;
            case GOSSIP_SENDER_MAIN + 3:
                creature->CastSpell(player, SPELL_ARM, false);
                player->AddSpellCooldown(SPELL_ARM, 0, time(NULL) + 7200);
                SendAction(player, creature, action);
                break;
            case GOSSIP_SENDER_MAIN + 4:
                creature->CastSpell(player, SPELL_SPI, false);
                player->AddSpellCooldown(SPELL_SPI, 0, time(NULL) + 7200);
                SendAction(player, creature, action);
                break;
            case GOSSIP_SENDER_MAIN + 5:
                creature->CastSpell(player, SPELL_INT, false);
                player->AddSpellCooldown(SPELL_INT, 0, time(NULL) + 7200);
                SendAction(player, creature, action);
                break;
            case GOSSIP_SENDER_MAIN + 6:
                creature->CastSpell(player, SPELL_STM, false);
                player->AddSpellCooldown(SPELL_STM, 0, time(NULL) + 7200);
                SendAction(player, creature, action);
                break;
            case GOSSIP_SENDER_MAIN + 7:
                creature->CastSpell(player, SPELL_STR, false);
                player->AddSpellCooldown(SPELL_STR, 0, time(NULL) + 7200);
                SendAction(player, creature, action);
                break;
            case GOSSIP_SENDER_MAIN + 8:
                creature->CastSpell(player, SPELL_AGI, false);
                player->AddSpellCooldown(SPELL_AGI, 0, time(NULL) + 7200);
                SendAction(player, creature, action);
                break;
        }
        return true;
    }
};

class npc_steam_tonk : public CreatureScript
{
public:
    npc_steam_tonk() : CreatureScript("npc_steam_tonk") { }

    struct npc_steam_tonkAI : public ScriptedAI
    {
        npc_steam_tonkAI(Creature* creature) : ScriptedAI(creature) { }

        void Reset() override { }
        void EnterCombat(Unit* /*who*/) override { }

        void OnPossess(bool apply)
        {
            if (apply)
            {
                // Initialize the action bar without the melee attack command
                me->InitCharmInfo();
                me->GetCharmInfo()->InitEmptyActionBar(false);

                me->SetReactState(REACT_PASSIVE);
            }
            else
                me->SetReactState(REACT_AGGRESSIVE);
        }
    };

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new npc_steam_tonkAI(creature);
    }
};

enum TonkMine
{
    SPELL_TONK_MINE_DETONATE    = 25099
};

class npc_tonk_mine : public CreatureScript
{
public:
    npc_tonk_mine() : CreatureScript("npc_tonk_mine") { }

    struct npc_tonk_mineAI : public ScriptedAI
    {
        npc_tonk_mineAI(Creature* creature) : ScriptedAI(creature)
        {
            me->SetReactState(REACT_PASSIVE);
        }

        uint32 ExplosionTimer;

        void Reset() override
        {
            ExplosionTimer = 3000;
        }

        void EnterCombat(Unit* /*who*/) override { }
        void AttackStart(Unit* /*who*/) override { }
        void MoveInLineOfSight(Unit* /*who*/) override { }


        void UpdateAI(uint32 diff) override
        {
            if (ExplosionTimer <= diff)
            {
                DoCast(me, SPELL_TONK_MINE_DETONATE, true);
                me->setDeathState(DEAD); // unsummon it
            }
            else
                ExplosionTimer -= diff;
        }
    };

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new npc_tonk_mineAI(creature);
    }
};

/*####
## npc_brewfest_reveler
####*/

enum BrewfestReveler
{
    SPELL_BREWFEST_TOAST = 41586
};

class npc_brewfest_reveler : public CreatureScript
{
    public:
        npc_brewfest_reveler() : CreatureScript("npc_brewfest_reveler") { }

        struct npc_brewfest_revelerAI : public ScriptedAI
        {
            npc_brewfest_revelerAI(Creature* creature) : ScriptedAI(creature) { }

            void ReceiveEmote(Player* player, uint32 emote) override
            {
                if (!IsHolidayActive(HOLIDAY_BREWFEST))
                    return;

                if (emote == TEXT_EMOTE_DANCE)
                    me->CastSpell(player, SPELL_BREWFEST_TOAST, false);
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_brewfest_revelerAI(creature);
        }
};

enum TrainingDummy
{
    NPC_ADVANCED_TARGET_DUMMY                  = 2674,
    NPC_TARGET_DUMMY                           = 2673,

    EVENT_TD_CHECK_COMBAT                      = 1,
    EVENT_TD_DESPAWN                           = 2
};

class npc_training_dummy : public CreatureScript
{
public:
    npc_training_dummy() : CreatureScript("npc_training_dummy") { }

    struct npc_training_dummyAI : ScriptedAI
    {
        npc_training_dummyAI(Creature* creature) : ScriptedAI(creature)
        {
            SetCombatMovement(false);
        }

        EventMap _events;
        std::unordered_map<uint64, time_t> _damageTimes;

        void Reset() override
        {
            me->SetControlled(true, UNIT_STATE_STUNNED);//disable rotate
            me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);//imune to knock aways like blast wave

            _events.Reset();
            _damageTimes.clear();
            if (me->GetEntry() != NPC_ADVANCED_TARGET_DUMMY && me->GetEntry() != NPC_TARGET_DUMMY)
                _events.ScheduleEvent(EVENT_TD_CHECK_COMBAT, 1000);
            else
                _events.ScheduleEvent(EVENT_TD_DESPAWN, 15000);
        }

        void EnterEvadeMode() override
        {
            if (!_EnterEvadeMode())
                return;

            Reset();
        }

        void DamageTaken(Unit* doneBy, uint32& damage) override
        {
            me->AddThreat(doneBy, float(damage));    // just to create threat reference
            _damageTimes[doneBy->GetGUID()] = time(NULL);
            damage = 0;
        }

        void UpdateAI(uint32 diff) override
        {
            if (!me->IsInCombat())
                return;

            if (!me->HasUnitState(UNIT_STATE_STUNNED))
                me->SetControlled(true, UNIT_STATE_STUNNED);//disable rotate

            _events.Update(diff);

            if (uint32 eventId = _events.ExecuteEvent())
            {
                switch (eventId)
                {
                    case EVENT_TD_CHECK_COMBAT:
                    {
                        time_t now = time(NULL);
                        for (std::unordered_map<uint64, time_t>::iterator itr = _damageTimes.begin(); itr != _damageTimes.end();)
                        {
                            // If unit has not dealt damage to training dummy for 5 seconds, remove him from combat
                            if (itr->second < now - 5)
                            {
                                if (Unit* unit = ObjectAccessor::GetUnit(*me, itr->first))
                                    unit->getHostileRefManager().deleteReference(me);

                                itr = _damageTimes.erase(itr);
                            }
                            else
                                ++itr;
                        }
                        _events.ScheduleEvent(EVENT_TD_CHECK_COMBAT, 1000);
                        break;
                    }
                    case EVENT_TD_DESPAWN:
                        me->DespawnOrUnsummon(1);
                        break;
                    default:
                        break;
                }
            }
        }

        void MoveInLineOfSight(Unit* /*who*/) override { }
    };

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new npc_training_dummyAI(creature);
    }
};

/*######
# npc_wormhole
######*/

#define GOSSIP_ENGINEERING1   "Borean Tundra"
#define GOSSIP_ENGINEERING2   "Howling Fjord"
#define GOSSIP_ENGINEERING3   "Sholazar Basin"
#define GOSSIP_ENGINEERING4   "Icecrown"
#define GOSSIP_ENGINEERING5   "Storm Peaks"
#define GOSSIP_ENGINEERING6   "Underground..."

enum WormholeSpells
{
    SPELL_BOREAN_TUNDRA         = 67834,
    SPELL_SHOLAZAR_BASIN        = 67835,
    SPELL_ICECROWN              = 67836,
    SPELL_STORM_PEAKS           = 67837,
    SPELL_HOWLING_FJORD         = 67838,
    SPELL_UNDERGROUND           = 68081,

    TEXT_WORMHOLE               = 907,

    DATA_SHOW_UNDERGROUND       = 1,
};

class npc_wormhole : public CreatureScript
{
    public:
        npc_wormhole() : CreatureScript("npc_wormhole") { }

        struct npc_wormholeAI : public PassiveAI
        {
            npc_wormholeAI(Creature* creature) : PassiveAI(creature) { }

            void InitializeAI() override
            {
                _showUnderground = urand(0, 100) == 0; // Guessed value, it is really rare though
            }

            uint32 GetData(uint32 type) const override
            {
                return (type == DATA_SHOW_UNDERGROUND && _showUnderground) ? 1 : 0;
            }

        private:
            bool _showUnderground;
        };

        bool OnGossipHello(Player* player, Creature* creature) override
        {
            if (creature->IsSummon())
            {
                if (player == creature->ToTempSummon()->GetSummoner())
                {
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ENGINEERING1, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ENGINEERING2, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 2);
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ENGINEERING3, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 3);
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ENGINEERING4, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 4);
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ENGINEERING5, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 5);

                    if (creature->AI()->GetData(DATA_SHOW_UNDERGROUND))
                        player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ENGINEERING6, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 6);

                    player->PlayerTalkClass->SendGossipMenu(TEXT_WORMHOLE, creature->GetGUID());
                }
            }

            return true;
        }

        bool OnGossipSelect(Player* player, Creature* creature, uint32 /*sender*/, uint32 action) override
        {
            player->PlayerTalkClass->ClearMenus();

            switch (action)
            {
                case GOSSIP_ACTION_INFO_DEF + 1: // Borean Tundra
                    player->CLOSE_GOSSIP_MENU();
                    creature->CastSpell(player, SPELL_BOREAN_TUNDRA, false);
                    break;
                case GOSSIP_ACTION_INFO_DEF + 2: // Howling Fjord
                    player->CLOSE_GOSSIP_MENU();
                    creature->CastSpell(player, SPELL_HOWLING_FJORD, false);
                    break;
                case GOSSIP_ACTION_INFO_DEF + 3: // Sholazar Basin
                    player->CLOSE_GOSSIP_MENU();
                    creature->CastSpell(player, SPELL_SHOLAZAR_BASIN, false);
                    break;
                case GOSSIP_ACTION_INFO_DEF + 4: // Icecrown
                    player->CLOSE_GOSSIP_MENU();
                    creature->CastSpell(player, SPELL_ICECROWN, false);
                    break;
                case GOSSIP_ACTION_INFO_DEF + 5: // Storm peaks
                    player->CLOSE_GOSSIP_MENU();
                    creature->CastSpell(player, SPELL_STORM_PEAKS, false);
                    break;
                case GOSSIP_ACTION_INFO_DEF + 6: // Underground
                    player->CLOSE_GOSSIP_MENU();
                    creature->CastSpell(player, SPELL_UNDERGROUND, false);
                    break;
            }

            return true;
        }

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_wormholeAI(creature);
        }
};

/*######
## npc_pet_trainer
######*/

enum PetTrainer
{
    TEXT_ISHUNTER               = 5838,
    TEXT_NOTHUNTER              = 5839,
    TEXT_PETINFO                = 13474,
    TEXT_CONFIRM                = 7722
};

#define GOSSIP_PET1             "How do I train my pet?"
#define GOSSIP_PET2             "I wish to untrain my pet."
#define GOSSIP_PET_CONFIRM      "Yes, please do."

class npc_pet_trainer : public CreatureScript
{
public:
    npc_pet_trainer() : CreatureScript("npc_pet_trainer") { }

    bool OnGossipHello(Player* player, Creature* creature) override
    {
        if (creature->IsQuestGiver())
            player->PrepareQuestMenu(creature->GetGUID());

        if (player->getClass() == CLASS_HUNTER)
        {
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_PET1, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
            if (player->GetPet() && player->GetPet()->getPetType() == HUNTER_PET)
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_PET2, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 2);

            player->PlayerTalkClass->SendGossipMenu(TEXT_ISHUNTER, creature->GetGUID());
            return true;
        }
        player->PlayerTalkClass->SendGossipMenu(TEXT_NOTHUNTER, creature->GetGUID());
        return true;
    }

    bool OnGossipSelect(Player* player, Creature* creature, uint32 /*sender*/, uint32 action) override
    {
        player->PlayerTalkClass->ClearMenus();
        switch (action)
        {
            case GOSSIP_ACTION_INFO_DEF + 1:
                player->PlayerTalkClass->SendGossipMenu(TEXT_PETINFO, creature->GetGUID());
                break;
            case GOSSIP_ACTION_INFO_DEF + 2:
                {
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_PET_CONFIRM, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 3);
                    player->PlayerTalkClass->SendGossipMenu(TEXT_CONFIRM, creature->GetGUID());
                }
                break;
            case GOSSIP_ACTION_INFO_DEF + 3:
                {
                    player->ResetPetTalents();
                    player->CLOSE_GOSSIP_MENU();
                }
                break;
        }
        return true;
    }
};

/*######
## npc_locksmith
######*/

enum LockSmith
{
    QUEST_HOW_TO_BRAKE_IN_TO_THE_ARCATRAZ = 10704,
    QUEST_DARK_IRON_LEGACY                = 3802,
    QUEST_THE_KEY_TO_SCHOLOMANCE_A        = 5505,
    QUEST_THE_KEY_TO_SCHOLOMANCE_H        = 5511,
    QUEST_HOTTER_THAN_HELL_A              = 10758,
    QUEST_HOTTER_THAN_HELL_H              = 10764,
    QUEST_RETURN_TO_KHAGDAR               = 9837,
    QUEST_CONTAINMENT                     = 13159,
    QUEST_ETERNAL_VIGILANCE               = 11011,
    QUEST_KEY_TO_THE_FOCUSING_IRIS        = 13372,
    QUEST_HC_KEY_TO_THE_FOCUSING_IRIS     = 13375,

    ITEM_ARCATRAZ_KEY                     = 31084,
    ITEM_SHADOWFORGE_KEY                  = 11000,
    ITEM_SKELETON_KEY                     = 13704,
    ITEM_SHATTERED_HALLS_KEY              = 28395,
    ITEM_THE_MASTERS_KEY                  = 24490,
    ITEM_VIOLET_HOLD_KEY                  = 42482,
    ITEM_ESSENCE_INFUSED_MOONSTONE        = 32449,
    ITEM_KEY_TO_THE_FOCUSING_IRIS         = 44582,
    ITEM_HC_KEY_TO_THE_FOCUSING_IRIS      = 44581,

    SPELL_ARCATRAZ_KEY                    = 54881,
    SPELL_SHADOWFORGE_KEY                 = 54882,
    SPELL_SKELETON_KEY                    = 54883,
    SPELL_SHATTERED_HALLS_KEY             = 54884,
    SPELL_THE_MASTERS_KEY                 = 54885,
    SPELL_VIOLET_HOLD_KEY                 = 67253,
    SPELL_ESSENCE_INFUSED_MOONSTONE       = 40173,
};

#define GOSSIP_LOST_ARCATRAZ_KEY                "I've lost my key to the Arcatraz."
#define GOSSIP_LOST_SHADOWFORGE_KEY             "I've lost my key to the Blackrock Depths."
#define GOSSIP_LOST_SKELETON_KEY                "I've lost my key to the Scholomance."
#define GOSSIP_LOST_SHATTERED_HALLS_KEY         "I've lost my key to the Shattered Halls."
#define GOSSIP_LOST_THE_MASTERS_KEY             "I've lost my key to the Karazhan."
#define GOSSIP_LOST_VIOLET_HOLD_KEY             "I've lost my key to the Violet Hold."
#define GOSSIP_LOST_ESSENCE_INFUSED_MOONSTONE   "I've lost my Essence-Infused Moonstone."
#define GOSSIP_LOST_KEY_TO_THE_FOCUSING_IRIS    "I've lost my Key to the Focusing Iris."
#define GOSSIP_LOST_HC_KEY_TO_THE_FOCUSING_IRIS "I've lost my Heroic Key to the Focusing Iris."

class npc_locksmith : public CreatureScript
{
public:
    npc_locksmith() : CreatureScript("npc_locksmith") { }

    bool OnGossipHello(Player* player, Creature* creature) override
    {
        // Arcatraz Key
        if (player->GetQuestRewardStatus(QUEST_HOW_TO_BRAKE_IN_TO_THE_ARCATRAZ) && !player->HasItemCount(ITEM_ARCATRAZ_KEY, 1, true))
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_LOST_ARCATRAZ_KEY, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);

        // Shadowforge Key
        if (player->GetQuestRewardStatus(QUEST_DARK_IRON_LEGACY) && !player->HasItemCount(ITEM_SHADOWFORGE_KEY, 1, true))
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_LOST_SHADOWFORGE_KEY, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 2);

        // Skeleton Key
        if ((player->GetQuestRewardStatus(QUEST_THE_KEY_TO_SCHOLOMANCE_A) || player->GetQuestRewardStatus(QUEST_THE_KEY_TO_SCHOLOMANCE_H)) &&
            !player->HasItemCount(ITEM_SKELETON_KEY, 1, true))
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_LOST_SKELETON_KEY, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 3);

        // Shatered Halls Key
        if ((player->GetQuestRewardStatus(QUEST_HOTTER_THAN_HELL_A) || player->GetQuestRewardStatus(QUEST_HOTTER_THAN_HELL_H)) &&
            !player->HasItemCount(ITEM_SHATTERED_HALLS_KEY, 1, true))
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_LOST_SHATTERED_HALLS_KEY, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 4);

        // Master's Key
        if (player->GetQuestRewardStatus(QUEST_RETURN_TO_KHAGDAR) && !player->HasItemCount(ITEM_THE_MASTERS_KEY, 1, true))
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_LOST_THE_MASTERS_KEY, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 5);

        // Violet Hold Key
        if (player->GetQuestRewardStatus(QUEST_CONTAINMENT) && !player->HasItemCount(ITEM_VIOLET_HOLD_KEY, 1, true))
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_LOST_VIOLET_HOLD_KEY, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 6);

        // Essence-Infused Moonstone
        if (player->GetQuestRewardStatus(QUEST_ETERNAL_VIGILANCE) && !player->HasItemCount(ITEM_ESSENCE_INFUSED_MOONSTONE, 1, true))
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_LOST_ESSENCE_INFUSED_MOONSTONE, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 7);

        // Key to the Focusing Iris
        if (player->GetQuestRewardStatus(QUEST_KEY_TO_THE_FOCUSING_IRIS) && !player->HasItemCount(ITEM_KEY_TO_THE_FOCUSING_IRIS, 1, true))
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_LOST_KEY_TO_THE_FOCUSING_IRIS, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 8);

        // Heroic Key to the Focusing Iris
        if (player->GetQuestRewardStatus(QUEST_HC_KEY_TO_THE_FOCUSING_IRIS) && !player->HasItemCount(ITEM_HC_KEY_TO_THE_FOCUSING_IRIS, 1, true))
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_LOST_HC_KEY_TO_THE_FOCUSING_IRIS, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 9);

        player->SEND_GOSSIP_MENU(player->GetGossipTextId(creature), creature->GetGUID());

        return true;
    }

    bool OnGossipSelect(Player* player, Creature* /*creature*/, uint32 /*sender*/, uint32 action) override
    {
        player->PlayerTalkClass->ClearMenus();
        switch (action)
        {
            case GOSSIP_ACTION_INFO_DEF + 1:
                player->CLOSE_GOSSIP_MENU();
                player->CastSpell(player, SPELL_ARCATRAZ_KEY, false);
                break;
            case GOSSIP_ACTION_INFO_DEF + 2:
                player->CLOSE_GOSSIP_MENU();
                player->CastSpell(player, SPELL_SHADOWFORGE_KEY, false);
                break;
            case GOSSIP_ACTION_INFO_DEF + 3:
                player->CLOSE_GOSSIP_MENU();
                player->CastSpell(player, SPELL_SKELETON_KEY, false);
                break;
            case GOSSIP_ACTION_INFO_DEF + 4:
                player->CLOSE_GOSSIP_MENU();
                player->CastSpell(player, SPELL_SHATTERED_HALLS_KEY, false);
                break;
            case GOSSIP_ACTION_INFO_DEF + 5:
                player->CLOSE_GOSSIP_MENU();
                player->CastSpell(player, SPELL_THE_MASTERS_KEY, false);
                break;
            case GOSSIP_ACTION_INFO_DEF + 6:
                player->CLOSE_GOSSIP_MENU();
                player->CastSpell(player, SPELL_VIOLET_HOLD_KEY, false);
                break;
            case GOSSIP_ACTION_INFO_DEF + 7:
                player->CLOSE_GOSSIP_MENU();
                player->CastSpell(player, SPELL_ESSENCE_INFUSED_MOONSTONE, false);
                break;
            case GOSSIP_ACTION_INFO_DEF + 8:
                player->CLOSE_GOSSIP_MENU();
                player->AddItem(ITEM_KEY_TO_THE_FOCUSING_IRIS, 1);
                break;
            case GOSSIP_ACTION_INFO_DEF + 9:
                player->CLOSE_GOSSIP_MENU();
                player->AddItem(ITEM_HC_KEY_TO_THE_FOCUSING_IRIS, 1);
                break;
        }
        return true;
    }
};

/*######
## npc_experience
######*/

#define EXP_COST                100000 //10 00 00 copper (10golds)
#define GOSSIP_TEXT_EXP         14736
#define GOSSIP_XP_OFF           "I no longer wish to gain experience."
#define GOSSIP_XP_ON            "I wish to start gaining experience again."

class npc_experience : public CreatureScript
{
public:
    npc_experience() : CreatureScript("npc_experience") { }

    bool OnGossipHello(Player* player, Creature* creature) override
    {
        player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_XP_OFF, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
        player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_XP_ON, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 2);
        player->PlayerTalkClass->SendGossipMenu(GOSSIP_TEXT_EXP, creature->GetGUID());
        return true;
    }

    bool OnGossipSelect(Player* player, Creature* /*creature*/, uint32 /*sender*/, uint32 action) override
    {
        player->PlayerTalkClass->ClearMenus();
        bool noXPGain = player->HasFlag(PLAYER_FLAGS, PLAYER_FLAGS_NO_XP_GAIN);
        bool doSwitch = false;

        switch (action)
        {
            case GOSSIP_ACTION_INFO_DEF + 1://xp off
                {
                    if (!noXPGain)//does gain xp
                        doSwitch = true;//switch to don't gain xp
                }
                break;
            case GOSSIP_ACTION_INFO_DEF + 2://xp on
                {
                    if (noXPGain)//doesn't gain xp
                        doSwitch = true;//switch to gain xp
                }
                break;
        }
        if (doSwitch)
        {
            if (!player->HasEnoughMoney(EXP_COST))
                player->SendBuyError(BUY_ERR_NOT_ENOUGHT_MONEY, 0, 0, 0);
            else if (noXPGain)
            {
                player->ModifyMoney(-EXP_COST);
                player->RemoveFlag(PLAYER_FLAGS, PLAYER_FLAGS_NO_XP_GAIN);
            }
            else if (!noXPGain)
            {
                player->ModifyMoney(-EXP_COST);
                player->SetFlag(PLAYER_FLAGS, PLAYER_FLAGS_NO_XP_GAIN);
            }
        }
        player->PlayerTalkClass->SendCloseGossip();
        return true;
    }
};

enum Fireworks
{
    NPC_OMEN                = 15467,
    NPC_MINION_OF_OMEN      = 15466,
    NPC_FIREWORK_BLUE       = 15879,
    NPC_FIREWORK_GREEN      = 15880,
    NPC_FIREWORK_PURPLE     = 15881,
    NPC_FIREWORK_RED        = 15882,
    NPC_FIREWORK_YELLOW     = 15883,
    NPC_FIREWORK_WHITE      = 15884,
    NPC_FIREWORK_BIG_BLUE   = 15885,
    NPC_FIREWORK_BIG_GREEN  = 15886,
    NPC_FIREWORK_BIG_PURPLE = 15887,
    NPC_FIREWORK_BIG_RED    = 15888,
    NPC_FIREWORK_BIG_YELLOW = 15889,
    NPC_FIREWORK_BIG_WHITE  = 15890,

    NPC_CLUSTER_BLUE        = 15872,
    NPC_CLUSTER_RED         = 15873,
    NPC_CLUSTER_GREEN       = 15874,
    NPC_CLUSTER_PURPLE      = 15875,
    NPC_CLUSTER_WHITE       = 15876,
    NPC_CLUSTER_YELLOW      = 15877,
    NPC_CLUSTER_BIG_BLUE    = 15911,
    NPC_CLUSTER_BIG_GREEN   = 15912,
    NPC_CLUSTER_BIG_PURPLE  = 15913,
    NPC_CLUSTER_BIG_RED     = 15914,
    NPC_CLUSTER_BIG_WHITE   = 15915,
    NPC_CLUSTER_BIG_YELLOW  = 15916,
    NPC_CLUSTER_ELUNE       = 15918,

    GO_FIREWORK_LAUNCHER_1  = 180771,
    GO_FIREWORK_LAUNCHER_2  = 180868,
    GO_FIREWORK_LAUNCHER_3  = 180850,
    GO_CLUSTER_LAUNCHER_1   = 180772,
    GO_CLUSTER_LAUNCHER_2   = 180859,
    GO_CLUSTER_LAUNCHER_3   = 180869,
    GO_CLUSTER_LAUNCHER_4   = 180874,

    SPELL_ROCKET_BLUE       = 26344,
    SPELL_ROCKET_GREEN      = 26345,
    SPELL_ROCKET_PURPLE     = 26346,
    SPELL_ROCKET_RED        = 26347,
    SPELL_ROCKET_WHITE      = 26348,
    SPELL_ROCKET_YELLOW     = 26349,
    SPELL_ROCKET_BIG_BLUE   = 26351,
    SPELL_ROCKET_BIG_GREEN  = 26352,
    SPELL_ROCKET_BIG_PURPLE = 26353,
    SPELL_ROCKET_BIG_RED    = 26354,
    SPELL_ROCKET_BIG_WHITE  = 26355,
    SPELL_ROCKET_BIG_YELLOW = 26356,
    SPELL_LUNAR_FORTUNE     = 26522,

    ANIM_GO_LAUNCH_FIREWORK = 3,
    ZONE_MOONGLADE          = 493,
};

Position omenSummonPos = {7558.993f, -2839.999f, 450.0214f, 4.46f};

class npc_firework : public CreatureScript
{
public:
    npc_firework() : CreatureScript("npc_firework") { }

    struct npc_fireworkAI : public ScriptedAI
    {
        npc_fireworkAI(Creature* creature) : ScriptedAI(creature) { }

        bool isCluster()
        {
            switch (me->GetEntry())
            {
                case NPC_FIREWORK_BLUE:
                case NPC_FIREWORK_GREEN:
                case NPC_FIREWORK_PURPLE:
                case NPC_FIREWORK_RED:
                case NPC_FIREWORK_YELLOW:
                case NPC_FIREWORK_WHITE:
                case NPC_FIREWORK_BIG_BLUE:
                case NPC_FIREWORK_BIG_GREEN:
                case NPC_FIREWORK_BIG_PURPLE:
                case NPC_FIREWORK_BIG_RED:
                case NPC_FIREWORK_BIG_YELLOW:
                case NPC_FIREWORK_BIG_WHITE:
                    return false;
                case NPC_CLUSTER_BLUE:
                case NPC_CLUSTER_GREEN:
                case NPC_CLUSTER_PURPLE:
                case NPC_CLUSTER_RED:
                case NPC_CLUSTER_YELLOW:
                case NPC_CLUSTER_WHITE:
                case NPC_CLUSTER_BIG_BLUE:
                case NPC_CLUSTER_BIG_GREEN:
                case NPC_CLUSTER_BIG_PURPLE:
                case NPC_CLUSTER_BIG_RED:
                case NPC_CLUSTER_BIG_YELLOW:
                case NPC_CLUSTER_BIG_WHITE:
                case NPC_CLUSTER_ELUNE:
                default:
                    return true;
            }
        }

        GameObject* FindNearestLauncher()
        {
            GameObject* launcher = NULL;

            if (isCluster())
            {
                GameObject* launcher1 = GetClosestGameObjectWithEntry(me, GO_CLUSTER_LAUNCHER_1, 0.5f);
                GameObject* launcher2 = GetClosestGameObjectWithEntry(me, GO_CLUSTER_LAUNCHER_2, 0.5f);
                GameObject* launcher3 = GetClosestGameObjectWithEntry(me, GO_CLUSTER_LAUNCHER_3, 0.5f);
                GameObject* launcher4 = GetClosestGameObjectWithEntry(me, GO_CLUSTER_LAUNCHER_4, 0.5f);

                if (launcher1)
                    launcher = launcher1;
                else if (launcher2)
                    launcher = launcher2;
                else if (launcher3)
                    launcher = launcher3;
                else if (launcher4)
                    launcher = launcher4;
            }
            else
            {
                GameObject* launcher1 = GetClosestGameObjectWithEntry(me, GO_FIREWORK_LAUNCHER_1, 0.5f);
                GameObject* launcher2 = GetClosestGameObjectWithEntry(me, GO_FIREWORK_LAUNCHER_2, 0.5f);
                GameObject* launcher3 = GetClosestGameObjectWithEntry(me, GO_FIREWORK_LAUNCHER_3, 0.5f);

                if (launcher1)
                    launcher = launcher1;
                else if (launcher2)
                    launcher = launcher2;
                else if (launcher3)
                    launcher = launcher3;
            }

            return launcher;
        }

        uint32 GetFireworkSpell(uint32 entry)
        {
            switch (entry)
            {
                case NPC_FIREWORK_BLUE:
                    return SPELL_ROCKET_BLUE;
                case NPC_FIREWORK_GREEN:
                    return SPELL_ROCKET_GREEN;
                case NPC_FIREWORK_PURPLE:
                    return SPELL_ROCKET_PURPLE;
                case NPC_FIREWORK_RED:
                    return SPELL_ROCKET_RED;
                case NPC_FIREWORK_YELLOW:
                    return SPELL_ROCKET_YELLOW;
                case NPC_FIREWORK_WHITE:
                    return SPELL_ROCKET_WHITE;
                case NPC_FIREWORK_BIG_BLUE:
                    return SPELL_ROCKET_BIG_BLUE;
                case NPC_FIREWORK_BIG_GREEN:
                    return SPELL_ROCKET_BIG_GREEN;
                case NPC_FIREWORK_BIG_PURPLE:
                    return SPELL_ROCKET_BIG_PURPLE;
                case NPC_FIREWORK_BIG_RED:
                    return SPELL_ROCKET_BIG_RED;
                case NPC_FIREWORK_BIG_YELLOW:
                    return SPELL_ROCKET_BIG_YELLOW;
                case NPC_FIREWORK_BIG_WHITE:
                    return SPELL_ROCKET_BIG_WHITE;
                default:
                    return 0;
            }
        }

        uint32 GetFireworkGameObjectId()
        {
            uint32 spellId = 0;

            switch (me->GetEntry())
            {
                case NPC_CLUSTER_BLUE:
                    spellId = GetFireworkSpell(NPC_FIREWORK_BLUE);
                    break;
                case NPC_CLUSTER_GREEN:
                    spellId = GetFireworkSpell(NPC_FIREWORK_GREEN);
                    break;
                case NPC_CLUSTER_PURPLE:
                    spellId = GetFireworkSpell(NPC_FIREWORK_PURPLE);
                    break;
                case NPC_CLUSTER_RED:
                    spellId = GetFireworkSpell(NPC_FIREWORK_RED);
                    break;
                case NPC_CLUSTER_YELLOW:
                    spellId = GetFireworkSpell(NPC_FIREWORK_YELLOW);
                    break;
                case NPC_CLUSTER_WHITE:
                    spellId = GetFireworkSpell(NPC_FIREWORK_WHITE);
                    break;
                case NPC_CLUSTER_BIG_BLUE:
                    spellId = GetFireworkSpell(NPC_FIREWORK_BIG_BLUE);
                    break;
                case NPC_CLUSTER_BIG_GREEN:
                    spellId = GetFireworkSpell(NPC_FIREWORK_BIG_GREEN);
                    break;
                case NPC_CLUSTER_BIG_PURPLE:
                    spellId = GetFireworkSpell(NPC_FIREWORK_BIG_PURPLE);
                    break;
                case NPC_CLUSTER_BIG_RED:
                    spellId = GetFireworkSpell(NPC_FIREWORK_BIG_RED);
                    break;
                case NPC_CLUSTER_BIG_YELLOW:
                    spellId = GetFireworkSpell(NPC_FIREWORK_BIG_YELLOW);
                    break;
                case NPC_CLUSTER_BIG_WHITE:
                    spellId = GetFireworkSpell(NPC_FIREWORK_BIG_WHITE);
                    break;
                case NPC_CLUSTER_ELUNE:
                    spellId = GetFireworkSpell(urand(NPC_FIREWORK_BLUE, NPC_FIREWORK_WHITE));
                    break;
            }

            const SpellInfo* spellInfo = sSpellMgr->GetSpellInfo(spellId);

            if (spellInfo && spellInfo->Effects[0].Effect == SPELL_EFFECT_SUMMON_OBJECT_WILD)
                return spellInfo->Effects[0].MiscValue;

            return 0;
        }

        void Reset() override
        {
            if (GameObject* launcher = FindNearestLauncher())
            {
                launcher->SendCustomAnim(ANIM_GO_LAUNCH_FIREWORK);
                me->SetOrientation(launcher->GetOrientation() + M_PI/2);
            }
            else
                return;

            if (isCluster())
            {
                // Check if we are near Elune'ara lake south, if so try to summon Omen or a minion
                if (me->GetZoneId() == ZONE_MOONGLADE)
                {
                    if (!me->FindNearestCreature(NPC_OMEN, 100.0f) && me->GetDistance2d(omenSummonPos.GetPositionX(), omenSummonPos.GetPositionY()) <= 100.0f)
                    {
                        switch (urand(0, 9))
                        {
                            case 0:
                            case 1:
                            case 2:
                            case 3:
                                if (Creature* minion = me->SummonCreature(NPC_MINION_OF_OMEN, me->GetPositionX()+frand(-5.0f, 5.0f), me->GetPositionY()+frand(-5.0f, 5.0f), me->GetPositionZ(), 0.0f, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 20000))
                                    minion->AI()->AttackStart(me->SelectNearestPlayer(20.0f));
                                break;
                            case 9:
                                me->SummonCreature(NPC_OMEN, omenSummonPos);
                                break;
                        }
                    }
                }
                if (me->GetEntry() == NPC_CLUSTER_ELUNE)
                    DoCast(SPELL_LUNAR_FORTUNE);

                float displacement = 0.7f;
                for (uint8 i = 0; i < 4; i++)
                    me->SummonGameObject(GetFireworkGameObjectId(), me->GetPositionX() + (i%2 == 0 ? displacement : -displacement), me->GetPositionY() + (i > 1 ? displacement : -displacement), me->GetPositionZ() + 4.0f, me->GetOrientation(), 0.0f, 0.0f, 0.0f, 0.0f, 1);
            }
            else
                //me->CastSpell(me, GetFireworkSpell(me->GetEntry()), true);
                me->CastSpell(me->GetPositionX(), me->GetPositionY(), me->GetPositionZ(), GetFireworkSpell(me->GetEntry()), true);
        }
    };

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new npc_fireworkAI(creature);
    }
};

/*#####
# npc_spring_rabbit
#####*/

enum rabbitSpells
{
    SPELL_SPRING_FLING          = 61875,
    SPELL_SPRING_RABBIT_JUMP    = 61724,
    SPELL_SPRING_RABBIT_WANDER  = 61726,
    SPELL_SUMMON_BABY_BUNNY     = 61727,
    SPELL_SPRING_RABBIT_IN_LOVE = 61728,
    NPC_SPRING_RABBIT           = 32791
};

class npc_spring_rabbit : public CreatureScript
{
public:
    npc_spring_rabbit() : CreatureScript("npc_spring_rabbit") { }

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new npc_spring_rabbitAI(creature);
    }

    struct npc_spring_rabbitAI : public ScriptedAI
    {
        npc_spring_rabbitAI(Creature* creature) : ScriptedAI(creature) { }

        bool inLove;
        uint32 jumpTimer;
        uint32 bunnyTimer;
        uint32 searchTimer;
        uint64 rabbitGUID;

        void Reset() override
        {
            inLove = false;
            rabbitGUID = 0;
            jumpTimer = urand(5000, 10000);
            bunnyTimer = urand(10000, 20000);
            searchTimer = urand(5000, 10000);
            if (Unit* owner = me->GetOwner())
                me->GetMotionMaster()->MoveFollow(owner, PET_FOLLOW_DIST, PET_FOLLOW_ANGLE);
        }

        void EnterCombat(Unit* /*who*/) override { }

        void DoAction(int32 /*param*/) override
        {
            inLove = true;
            if (Unit* owner = me->GetOwner())
                owner->CastSpell(owner, SPELL_SPRING_FLING, true);
        }

        void UpdateAI(uint32 diff) override
        {
            if (inLove)
            {
                if (jumpTimer <= diff)
                {
                    if (Unit* rabbit = ObjectAccessor::GetUnit(*me, rabbitGUID))
                        DoCast(rabbit, SPELL_SPRING_RABBIT_JUMP);
                    jumpTimer = urand(5000, 10000);
                } else jumpTimer -= diff;

                if (bunnyTimer <= diff)
                {
                    DoCast(SPELL_SUMMON_BABY_BUNNY);
                    bunnyTimer = urand(20000, 40000);
                } else bunnyTimer -= diff;
            }
            else
            {
                if (searchTimer <= diff)
                {
                    if (Creature* rabbit = me->FindNearestCreature(NPC_SPRING_RABBIT, 10.0f))
                    {
                        if (rabbit == me || rabbit->HasAura(SPELL_SPRING_RABBIT_IN_LOVE))
                            return;

                        me->AddAura(SPELL_SPRING_RABBIT_IN_LOVE, me);
                        DoAction(1);
                        rabbit->AddAura(SPELL_SPRING_RABBIT_IN_LOVE, rabbit);
                        rabbit->AI()->DoAction(1);
                        rabbit->CastSpell(rabbit, SPELL_SPRING_RABBIT_JUMP, true);
                        rabbitGUID = rabbit->GetGUID();
                    }
                    searchTimer = urand(5000, 10000);
                } else searchTimer -= diff;
            }
        }
    };
};

class npc_imp_in_a_ball : public CreatureScript
{
private:
    enum
    {
        SAY_RANDOM,

        EVENT_TALK = 1,
    };

public:
    npc_imp_in_a_ball() : CreatureScript("npc_imp_in_a_ball") { }

    struct npc_imp_in_a_ballAI : public ScriptedAI
    {
        npc_imp_in_a_ballAI(Creature* creature) : ScriptedAI(creature)
        {
            summonerGUID = 0;
        }

        void IsSummonedBy(Unit* summoner) override
        {
            if (summoner->GetTypeId() == TYPEID_PLAYER)
            {
                summonerGUID = summoner->GetGUID();
                events.ScheduleEvent(EVENT_TALK, 3000);
            }
        }

        void UpdateAI(uint32 diff) override
        {
            events.Update(diff);

            if (events.ExecuteEvent() == EVENT_TALK)
            {
                if (Player* owner = ObjectAccessor::GetPlayer(*me, summonerGUID))
                {
                    sCreatureTextMgr->SendChat(me, SAY_RANDOM, owner,
                        owner->GetGroup() ? CHAT_MSG_MONSTER_PARTY : CHAT_MSG_MONSTER_WHISPER, LANG_ADDON, TEXT_RANGE_NORMAL);
                }
            }
        }

    private:
        EventMap events;
        uint64 summonerGUID;
    };

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new npc_imp_in_a_ballAI(creature);
    }
};

class npc_risen_ally : public CreatureScript
{
public:
    npc_risen_ally() : CreatureScript("npc_risen_ally") { }

       CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_risen_allyAI(creature);
    }

    struct npc_risen_allyAI : ScriptedAI
    {
        npc_risen_allyAI(Creature* creature) : ScriptedAI(creature) {}

        void InitializeAI()
        {
            me->setPowerType(POWER_ENERGY);
            me->SetMaxPower(POWER_ENERGY, 100);
            me->SetPower(POWER_ENERGY, 100);
            me->SetSheath(SHEATH_STATE_MELEE);
            me->SetByteFlag(UNIT_FIELD_BYTES_2, 2, UNIT_CAN_BE_ABANDONED);
            me->SetUInt32Value(UNIT_FIELD_BYTES_0, 2048);
            me->SetUInt32Value(UNIT_FIELD_FLAGS, UNIT_FLAG_PVP_ATTACKABLE);
            me->SetUInt32Value(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_NONE);
            me->SetFloatValue(UNIT_FIELD_COMBATREACH, 1.5f);
        }

        void Reset()
        {
            me->setPowerType(POWER_ENERGY);
            me->SetMaxPower(POWER_ENERGY, 100);
        }

        void IsSummonedBy(Unit* owner)
        {
            me->setPowerType(POWER_ENERGY);
            me->SetMaxPower(POWER_ENERGY, 100);
            me->SetPower(POWER_ENERGY, 100);
        }

        void JustDied(Unit* /*killer*/)
        {
           if (me->GetOwner())
              {
                 me->GetOwner()->RemoveAurasDueToSpell(62218);
                 me->GetOwner()->RemoveAurasDueToSpell(46619);
              }
        }

        void UpdateAI(const uint32 diff)
        {
           if (!me->IsCharmed())
              me->DespawnOrUnsummon();

           if (me->IsInCombat())
              DoMeleeAttackIfReady();
        }
    };
};

enum TriggerMisc
{
    GO_SPELL_FOCUS              = 184750,
    NPC_DARK_CONCLAVE_RITUALIST = 22138
};

class npc_invis_arakkoa_target : public CreatureScript
{
public:
    npc_invis_arakkoa_target() : CreatureScript("npc_invis_arakkoa_target") { }

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_invis_arakkoa_targetAI (creature);
    }

    struct npc_invis_arakkoa_targetAI : public ScriptedAI
    {
        npc_invis_arakkoa_targetAI(Creature* creature) : ScriptedAI(creature){}

        uint32 checkTimer;

        void Reset()
        {
            checkTimer = 4*IN_MILLISECONDS;
        }

        void UpdateAI(uint32 diff)
        {
            if (checkTimer <= diff)
            {
                if (!me->FindNearestCreature(NPC_DARK_CONCLAVE_RITUALIST, 1000.0f))
                {
                    me->SummonGameObject(GO_SPELL_FOCUS, -4192.24f, 2005.37f, 76.79f, 0.80f, 0, 0, 0, 0, 300);
                    me->DespawnOrUnsummon();
                }

                checkTimer = 4*IN_MILLISECONDS;
            }
            else
            checkTimer -= diff;
         }    
    };
};

enum q11429Data
{
    QUEST_DROP_IT_ROCK_IT = 11429,
    NPC_WINTERSKORN_DEFENDER = 24015,
};

class npc_banner_q11429 : public CreatureScript
{
public:
    npc_banner_q11429() : CreatureScript("npc_banner_q11429") { }

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_banner_q11429AI(creature);
    }

    struct npc_banner_q11429AI : public ScriptedAI
    {
        npc_banner_q11429AI(Creature* c) : ScriptedAI(c) { }

        uint32 uiWaveTimer;
        uint8 killCounter;

        void Reset()
        {
            killCounter = 0;
            uiWaveTimer = 2000;
            me->SetReactState(REACT_PASSIVE);
            me->GetMotionMaster()->MoveIdle();
        }

        void JustDied(Unit* /*killer*/)
        {
            if (Player* player = me->GetOwner()->ToPlayer())
                player->FailQuest(QUEST_DROP_IT_ROCK_IT);
        }

        void UpdateAI(const uint32 diff)
        {
            if(uiWaveTimer < diff)
            {
                if(Creature* pVrykul = me->SummonCreature(NPC_WINTERSKORN_DEFENDER, (1476.85f + rand()%20), (-5327.56f + rand()%20), (194.8f  + rand()%2), 0.0f, TEMPSUMMON_CORPSE_DESPAWN))
                {
                    pVrykul->AI()->AttackStart(me);
                    pVrykul->GetMotionMaster()->Clear();
                    pVrykul->GetMotionMaster()->MoveChase(me);
                }
                uiWaveTimer = urand(8000, 16000);
            }
            else
                uiWaveTimer -= diff;
        }

        void SummonedCreatureDespawn(Creature* summon)
        {
            if (summon->GetEntry() == NPC_WINTERSKORN_DEFENDER)
                killCounter++;

            if(killCounter >= 3)
            {
                if (Player* player = me->GetOwner()->ToPlayer())
                    player->GroupEventHappens(QUEST_DROP_IT_ROCK_IT, me);

                me->DespawnOrUnsummon(2000);
            }
        }
    };
};

//Spells
#define SPELL_ICE_ARMOR	 			36881
#define SPELL_LIGHTNING_BOLT        548
#define SPELL_BLISTERING_COLD       71049
#define SPELL_DARKWINTER			74275
#define SPELL_SHADOW_SPIKE			46589
#define SPELL_BERSERK               45078
#define SPELL_SINBEAM				40827
#define SPELL_SOULSTORM             68872
#define SPELL_SOULSTORM_CHANNEL     69008
#define SPELL_SOULSTORM_VISUAL      68870
#define SPELL_FEAR					68950
#define SPELL_FROST_BEACON			70126
#define SPELL_FRENZY                8269
#define SPELL_WHITEOUT				72034	// 30 % Debuff
#define SPELL_FROZEN_MALLET			71993
#define SPELL_CHAIN_LIGHTNING		33665
#define SPELL_HARVEST_SOUL			68980
#define SPELL_FURY_ANTICHEAT		72350
#define SPELL_RAISE_DEAD			71769
#define SPELL_DRAIN_MANA			5138
#define SPELL_BLIZZARD				26607
#define SPELL_JUMP_TO_TARGET		64430
#define DEATH_GRIP					64431
#define MIRROR_IMAGE				55342
#define MIRROR_IMAGE_PREEFFECT		8677
#define MIRROR_IMAGE_TIMER			180000
// Orb spell info
#define SPELL_ORB_FLARE_PASSIVE  	30234

enum Events
{	
	// Global Events:
	EVENT_WHITEOUT              = 1,
	EVENT_BERSERK				= 2,
	EVENT_BLISTER				= 3,
	
	// Phase 1
	EVENT_SHADOW_SPIKE			= 4,
	EVENT_SOULSTORM				= 5,
	EVENT_FEAR					= 6,
	EVENT_FRENZY				= 7,
	EVENT_CHAIN_LIGHTNING		= 8,

	// Phase 2
	EVENT_BLISTERING_COLD		= 9,
	EVENT_DRAIN_MANA 			= 10,
	EVENT_FROZEN_ORB			= 11,
	EVENT_SINBEAM				= 12,
	EVENT_FROST_BEACON			= 13,

	// Common Events:
	EVENT_RANDOM_CHAT			= 14,
	EVENT_INTRO					= 15,
	EVENT_COMBAT				= 16,
	EVENT_INTROB				= 17,
	EVENT_INTROC				= 18,
	EVENT_PAYBACK				= 19,
	EVENT_HARVEST_SOUL			= 20,
	EVENT_REVIVE				= 21,
	EVENT_PLAYSOUND_CAST1A		= 22,
	EVENT_PLAYSOUND_CAST1B		= 23,
	EVENT_SOULFRAGMENT			= 24,
	EVENT_MIRROR_IMAGE			= 25,
	EVENT_MIRROR_IMAGE_MAINSPELL= 26,
	EVENT_GHOSTLY				= 27,
};

// FYI: You can have 1-8 phases max.
enum Phases
{
	PHASE_NONE	= 1,
    PHASE_ONE	= 2,
    PHASE_TWO	= 3,
	PHASE_OWNED	= 4,
	PHASE_OUTRO	= 5,
};

//Chats
enum Texts
{
    // The Lich King
    SAY_LK_INTRO_1                  = 0,
    SAY_LK_INTRO_2                  = 1,
    SAY_LK_INTRO_3                  = 2,
	SAY_LK_AGGRO					= 3,
    SAY_LK_REMORSELESS_WINTER       = 4,
    SAY_LK_QUAKE                    = 5,
    SAY_LK_SUMMON_VALKYR            = 6,
    SAY_LK_HARVEST_SOUL             = 7,
    SAY_LK_FROSTMOURNE_ESCAPE       = 8,
    SAY_LK_FROSTMOURNE_KILL         = 9,
    SAY_LK_KILL                     = 10,
    SAY_LK_BERSERK                  = 11,
    EMOTE_DEFILE_WARNING            = 12,
    EMOTE_NECROTIC_PLAGUE_WARNING   = 13,
    SAY_LK_OUTRO_1                  = 14,
    SAY_LK_OUTRO_2                  = 15,
    SAY_LK_OUTRO_3                  = 16,
    SAY_LK_OUTRO_4                  = 17,
    SAY_LK_OUTRO_5                  = 18,
    SAY_LK_OUTRO_6                  = 19,
    SAY_LK_OUTRO_7                  = 20,
    SAY_LK_OUTRO_8                  = 21,
};

enum MiscData
{
    MUSIC_FROZEN_THRONE         = 17457,
    MUSIC_SPECIAL               = 17458,
    MUSIC_FURY_OF_FROSTMOURNE   = 17459,
    MUSIC_FINAL                 = 17460,
    EQUIP_ASHBRINGER_GLOWING    = 50442,
    EQUIP_BROKEN_FROSTMOURNE    = 50840,
};

enum AuraSpells
{
	AURA_TWISTER				= 13913,
	AURA_SHADOW					= 16592,
	AURA_REFLECTION				= 10831,
	AURA_SOULFRAGMENT			= 71905,
	AURA_GHOSTLY				= 16713,
};

enum AuraSpells_Timers
{
	AURA_SOULFRAGMENT_timer = 6000,
};

// Adds Settings
#define ADD_ID_TOSPAWN			17096
#define ADD_MAX_ADDS			14
#define MIN_PAUSE_CHATTER		16000	// Timer of the random chatter (Do not set too low or you will get crazy).

// Declarations
bool _soulstormed;
bool _frenzied;
bool _berserking;	// The scripting for this is not compltete yet so I recommend nog using the according phase yet or write it yourself it is not hard.
bool _reflectdmg;	// Same --^
bool _has_orb_adds		= false;
bool _playerscheating	= false;

int _current_orbs;
int _ticks;
uint32 _inMeleeRange;
uint32 _lasthp;
uint32 _newhp,_hplost;
uint32 _totaldmgtaken;
uint32 _chatter_timer;
uint32 t;

//Modelsmorph
#define MODEL_DARKSAW		30721 // LichKing. In my case not needed, because my creature already has value 30721 as displayID in the database.
#define MODEL_CLONE_DIED	24641 // The spawned Arthas Mirages. We change their model @ death (mostly for visual enjoyment).

// The actual Boss Class:
class boss_darksaw : public CreatureScript
{
  public:
	boss_darksaw() : CreatureScript("boss_darksaw"){}
	
	CreatureAI* GetAI(Creature* pCreature) const
	{
		return new boss_darksawAI(pCreature);
	}

	struct boss_darksawAI : public ScriptedAI
	{
		boss_darksawAI(Creature *c) : ScriptedAI(c) {}
		
		// Class Inits (Occurs only once @ script init:
		EventMap events;
		
		uint32 addztimer;
		uint32 _last_lk_special;	// Also chatter.
		
		
		// What to reset @ first init, after boss died, players died or fled:
		void Reset()
		{
			// First, reset eventsmap
			events.Reset();
			// Walk Speed (Or set in in DB, but do NOT do that is you choose other Model ID than Lich King as I dids):
			me->SetSpeed(MOVE_WALK, 2.6f, false);
			// Act cool:
			me->SetWalk(false);

			// Remove all spells and auras from previous attempts
			me->RemoveAllAuras();
			
			// set some used variables
			_berserking = false;
			_frenzied = false;
			_soulstormed = false;
			_has_orb_adds = false;
			_playerscheating = false;
			_ticks = 0;
			
			events.SetPhase(PHASE_NONE);
			me->SetReactState(REACT_AGGRESSIVE);
			
			// Ghost Effect
			me->AddAura(AURA_GHOSTLY,me);
		}
		
		void EnterCombat(Unit* who)
		{
			// Stay relaxed:
			me->SetWalk(true);

			// Remove Ghost effect
			events.CancelEvent(EVENT_GHOSTLY);
			me->RemoveAura(AURA_GHOSTLY);
			
			// Some fool pulled,. (: start the war:
			events.SetPhase(PHASE_ONE);
			
			me->SendPlaySound(17458, false); // Some intromusic, does not interrupt current music and does not loop.
			me->AddThreat(who, 30.0f);
			me->AI()->AttackStart(me);
			me->setActive(true);
			
			events.ScheduleEvent(EVENT_INTRO, 100);
		}
		
		void DamageTaken(Unit* attacker, uint32& damage)
		{
			/* // Not Finished yet,.
			// Cannot lose 1 % HP in 1 tick.. I will cheat!
			if ((me->GetMaxHealth() / 100) <= damage) {
				events.ScheduleEvent(EVENT_PAYBACK, 2500);
				me->MonsterYell("Your end is nigh!",LANG_UNIVERSAL,NULL);
				_playerscheating = true;
			}
			*/


			if (damage > 0) {
				
				me->AddThreat(me->GetVictim(), 1.0f);

				if (_reflectdmg && attacker->HasAura(AURA_REFLECTION))
				{	// When reflect avtive, cast 10% of damage done back to boss. 
					attacker->DealDamage(attacker->GetVictim(), damage * 0.1 ,0,DIRECT_DAMAGE,SPELL_SCHOOL_MASK_ALL,0,true);
					// me->MonsterSay("Debug: Aura Powered!..",LANG_UNIVERSAL,NULL);
				}
				/*
				_totaldmgtaken = _totaldmgtaken + damage;
				if (_ticks == 10)
				{ 
					// After
				}
				*/
			}

			if (!_frenzied && !HealthAbovePct(75))
			{
				// add the shadowaura if not already present:
				if (!me->HasAura(AURA_REFLECTION)) { me->AddAura(AURA_SHADOW,me); }
			}
			if (!_frenzied && !HealthAbovePct(50))
			{
				// schedule the event that changes our phase
				_frenzied = true;
				events.ScheduleEvent(EVENT_FRENZY, 100);
			}
			if (!_berserking && !HealthAbovePct(24))
			{
				// schedule the event that also changes our phase:
				_berserking = true;
				// It works, but atm not recommended. It gives 900% damage buff :)
				// Just leave the boolean True at all times.
				// events.ScheduleEvent(EVENT_BERSERK, 100);
			}
			// we're losing health bad, go insane!
			if (!_soulstormed && !HealthAbovePct(15))
			{
				_soulstormed = true;
				events.ScheduleEvent(EVENT_SOULSTORM, 100);
			}
		}
		
		void JustDied(Unit* /*killer*/)
		{
			events.Reset();
			if (RAND(1,2)==1) { Talk(SAY_LK_FROSTMOURNE_ESCAPE); _chatter_timer = 0; } // 50 % chance he will comment on death.
			me->RemoveAllAuras();
		}
		
		void KilledUnit(Unit* victim)
		{
			if (victim->GetGUID() == 99001) // Wanneer een Clone.
			{ 
				victim->SetDisplayId(MODEL_CLONE_DIED);
			}
			if (victim && victim->GetCreatureType() == CREATURE_TYPE_HUMANOID) //GetTypeId() == TYPEID_PLAYER && me->IsValidAttackTarget(victim))
			{
				me->MonsterSay("하하하, ",LANG_UNIVERSAL,NULL);
				if (RAND(1,2) == 1) Talk(RAND(SAY_LK_FROSTMOURNE_KILL, SAY_LK_HARVEST_SOUL, SAY_LK_KILL)); // 50 % Kans dat er een opmerking komt.
				_chatter_timer = 0;
			}
		}

		//--

		void UpdateAI(const uint32 diff)
		{
		// Updates Out of Combat
			if (me->GetAuraCount(AURA_GHOSTLY) < 1)
			{
				if (!UpdateVictim()) { me->AddAura(AURA_GHOSTLY,me); }
			}

			if (!UpdateVictim()) return;
			
			
		// Updates IN Combat
			events.Update(diff);
			
			//Timers Updates/Reset:
			_ticks++;
			_current_orbs = 0;
			_last_lk_special += diff;
			_chatter_timer += diff;
			
			
			// Adds and other EVENTless stuff:
			if (_has_orb_adds == true && addztimer <= diff)
			{
				std::list<Creature*> IcicleAddsList;
				GetCreatureListWithEntryInGrid(IcicleAddsList, me, ADD_ID_TOSPAWN, 100.0f);
				for (std::list<Creature*>::iterator itr = IcicleAddsList.begin(); itr != IcicleAddsList.end(); ++itr)
				{
	/*(*itr)->*/	++_current_orbs;	// Optionally, you also can do stuff with (*itr)-> here.
				}
				
				if (_current_orbs < ADD_MAX_ADDS) 
				{
					if (RAND(1,4)==1) { events.ScheduleEvent(EVENT_PLAYSOUND_CAST1A, 10); _chatter_timer = 0; }
					
					//Summon Orb Add
					Creature* AstralOrb = DoSpawnCreature(17096, float(rand()%47), float(rand()%47), 0, 0, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 5000);
					Unit* target = NULL; target = SelectTarget(SELECT_TARGET_TOPAGGRO, 0);
					
					if (AstralOrb && target)
					{
						AstralOrb->CastSpell(AstralOrb, SPELL_ORB_FLARE_PASSIVE, false);
						AstralOrb->AI()->AttackStart(target);
					}
				}
				addztimer = 10000;
			} else addztimer -= diff;
			
			
			// return back to main code if we're still casting
			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;
			
			while (uint32 eventId = events.ExecuteEvent())
			{
				switch (eventId)
				{	
					// CasterSounds:
					case EVENT_PLAYSOUND_CAST1A:
						t = RAND(14803, 14771, 14751);
						if (t==14803) { me->MonsterYell("감동적이군..",LANG_UNIVERSAL,NULL); }
						if (t==14771) { me->MonsterYell("자비란 없다..",LANG_UNIVERSAL,NULL); }
						if (t==14751) { me->MonsterYell("너의 군대가..",LANG_UNIVERSAL,NULL); }
						events.ScheduleEvent(EVENT_PLAYSOUND_CAST1B, 2500);
						me->SendPlaySound(t, false); // Touchy.. / No Mercy. / Your.. Army..
					_chatter_timer = 0;
					break;
					case EVENT_PLAYSOUND_CAST1B:
					_chatter_timer = 0;
						t = RAND(14795, 14786, 14787);
						if (t==14795) { me->MonsterYell("붉은 십자군이여 너의 빛은 어디있는냐?",LANG_UNIVERSAL,NULL); }
						if (t==14786) { me->MonsterYell("살았건 죽었건, 넌 나를 섬길것이다.",LANG_UNIVERSAL,NULL); }
						if (t==14787) { me->MonsterYell("고통받아라 벌레들아!",LANG_UNIVERSAL,NULL); }
						me->SendPlaySound(t, false); // Where is your light now crusaders? / 14786 u will serve me ..
					_chatter_timer = 0;
					break;
					
					// Berserk Handler:
					case EVENT_BERSERK:
					_reflectdmg = true;	// Now reflecting big portion of damage back to LK.
						
						// Our positive BUFF:
						me->AddAura(AURA_REFLECTION,me->GetVictim());
						me->GetVictim()->BuildAuraStateUpdateForTarget(me->GetVictim());
						
						// Bosses Negative buff:
						me->RemoveAurasDueToSpell(SPELL_BERSERK);
						me->CastSpell(me,SPELL_BERSERK);


						me->SendPlaySound(13878, false); // Buff Sound
						me->SendPlaySound(14805, false); // LK Sound
					_chatter_timer = 0;
					break;
					
					// Cheaters payback! ^^ (Disabled)
					case EVENT_PAYBACK:
						/*
						_has_orb_adds = false; // No adds now.
						events.SetPhase(PHASE_OWNED);
						DoCastAOE(SPELL_FURY_ANTICHEAT,true); // Some more overkill :)
						me->MonsterYell("So, Too can play that game, nib! ;)",LANG_UNIVERSAL,NULL);
						_chatter_timer = 0;
						me->SetWalk(true);
						Talk(SAY_LK_OUTRO_2); // Fake outro,.
						_chatter_timer = 0;
						*/
					break;
					
					// Random Chatter:
					case EVENT_RANDOM_CHAT:
						if (_chatter_timer > MIN_PAUSE_CHATTER)
						{
						  _chatter_timer = 0;
							t = RAND(1,4);
							if (t==1){
							Talk(RAND(
								SAY_LK_OUTRO_5, 
								SAY_LK_KILL, 
								SAY_LK_FROSTMOURNE_KILL, 
								SAY_LK_AGGRO));
							}else{
								t = RAND(
								14738, // "You have crossed into the world of the dead in search of answers. You wish to save you ally, and have risked life and limb to be here. Allow me to help."
								14745, // "But.. It is not yet your time to serve the Lich King. Yes, a greater destiny awaits you. Power.. You must become more powerful before you are to serve me."
								14754, // "Fail me and it shall be your undoing.."
								14755, // "Succeed and even greater power shall be yours!"

								14760, // Random,. 
								//14770, // "Very well.. warriors of the frozen wastes, rise up! I command you to fight, kill, and die for your master. Let none survive.."
								14773,  // "Mercy is for the weak!"
								14792,  // "Let the destruction of this place, serve as a lesson.. To all those who would dare oppose the scourge.."
								
								//14799,  // "Come to me, crusaders. I will remake you!"
								14802,  // "Lay down your arms, and surrender your souls."
								14766); // "Be warned.."

								if (t==14738) { me->MonsterYell("너는 해답을 찾고자 죽은의 세계로 건너왔다. 동료를 돕겠다는 일념하에 이사지에 목숨을 내 던졌지. 내가 널 도와주마!",LANG_UNIVERSAL,NULL); }
								if (t==14745) { me->MonsterYell("하지만 아직 리치왕을 섬길때가 아니다..그래 더 큰 운명이 너를 기다린다.  힘이. 나를 섬기기전에 너는 더 큰힘을 길러야한다.",LANG_UNIVERSAL,NULL); }
								if (t==14754) { me->MonsterYell("날 실망시키면 넌 파멸하리라..",LANG_UNIVERSAL,NULL); }
								if (t==14755) { me->MonsterYell("성공하면 더욱더 강력한 힘을 맛보게 될것이다!",LANG_UNIVERSAL,NULL); }
							
								if (t==14760) { me->MonsterYell("할 일은 알고있겠지.",LANG_UNIVERSAL,NULL); }
								//if (t==14770) { me->MonsterYell("Very well.. warriors of the frozen wastes, rise up! I command you to fight, kill, and die for your master. Let none survive..",LANG_UNIVERSAL,NULL); }
								if (t==14773) { me->MonsterYell("자비란 나약함일뿐이다!",LANG_UNIVERSAL,NULL); }
								if (t==14792) { me->MonsterYell("이곳을 숙대밭으로 만들어라.. 감히 스컬지에게 맞서는자에게 뼈져린 교훈을 안겨주어라..",LANG_UNIVERSAL,NULL); }
							
								//if (t==14799) { me->MonsterYell("Come to me, crusaders. I will remake you!",LANG_UNIVERSAL,NULL); }
								if (t==14802) { me->MonsterYell("무의미한 저항은 그만두고, 나에게 영혼을 받쳐라.",LANG_UNIVERSAL,NULL); }
								if (t==14766) { me->MonsterYell("명심해라..",LANG_UNIVERSAL,NULL); }

								me->SendPlaySound(t, false); // Touchy.. / No Mercy. / Your.. Army..
							  _chatter_timer = 0;
							}
						}
						events.ScheduleEvent(EVENT_RANDOM_CHAT, urand(14000, 16000));
					break;

					// PHASE_ONE:
					case EVENT_CHAIN_LIGHTNING:
						if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM))
						//me->CastCustomSpell(SPELL_CHAIN_LIGHTNING, SPELLVALUE_MAX_TARGETS, 1, target, false);
						DoCast(me->GetVictim(), SPELL_CHAIN_LIGHTNING);
						events.ScheduleEvent(EVENT_CHAIN_LIGHTNING, urand(7000, 11000), 0, PHASE_ONE);
					break;

					case EVENT_WHITEOUT:
						Talk(RAND(SAY_LK_FROSTMOURNE_ESCAPE, SAY_LK_HARVEST_SOUL));	// Warning - Important.
						DoCast(me, SPELL_WHITEOUT);
						events.ScheduleEvent(EVENT_WHITEOUT, 120000, 0, PHASE_ONE);
						break;

					case EVENT_BLISTERING_COLD:
						_inMeleeRange = 0;
						for (uint8 i = 0; i < 10; ++i)
						{
							if (Unit* target = SelectTarget(SELECT_TARGET_TOPAGGRO, i))
								// check if target is within melee-distance
								if (me->IsWithinMeleeRange(target))
									++_inMeleeRange;
						}
						// trigger spellcast only if we have 1 or more targets to affect..
						if (_inMeleeRange >= 1)
						{	// ..and only if a frost add is near:
							if (Creature *SummonedAdd = GetClosestCreatureWithEntry(me, ADD_ID_TOSPAWN, 3.0f))
							{
								Talk(RAND(SAY_LK_OUTRO_5, SAY_LK_BERSERK));			// Warning - Important.
								DoCast(me->GetVictim(), SPELL_BLISTERING_COLD);
								_chatter_timer = 0;
							}
							else
							{
								//me->MonsterYell("<Blistering Cold Evaded>",LANG_UNIVERSAL,NULL); // Debug.
							}
							
						}
						events.ScheduleEvent(EVENT_BLISTERING_COLD, urand(10000, 20000));
					break;
						
					case EVENT_SHADOW_SPIKE:
						if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM))
							DoCast(target, SPELL_SHADOW_SPIKE);
						events.ScheduleEvent(EVENT_SHADOW_SPIKE, urand(4500, 6000), 0, PHASE_ONE);
					break;

					case EVENT_FEAR:
						//me->MonsterYell("Hahaha...",LANG_UNIVERSAL,NULL);
						me->SendPlaySound(14820, false); _chatter_timer = 0;
						if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM))
							me->CastCustomSpell(SPELL_FEAR, SPELLVALUE_MAX_TARGETS, 1, target, false);
						events.ScheduleEvent(EVENT_FEAR, urand(12000, 48000), 0, PHASE_ONE);
					break;
					
					case EVENT_DRAIN_MANA:
						DoCastAOE(SPELL_DRAIN_MANA);
						events.ScheduleEvent(EVENT_DRAIN_MANA, urand(10000, 12000), 0, PHASE_TWO);
					break;
						
					case EVENT_SINBEAM:
						DoCast(me->GetVictim(), SPELL_SINBEAM);
						events.ScheduleEvent(EVENT_SINBEAM, urand(4000, 6000), 0, PHASE_TWO);
					break;

					case EVENT_GHOSTLY:
						me->AddAura(AURA_GHOSTLY,me);
						events.ScheduleEvent(EVENT_GHOSTLY, 55);
					break;

					case EVENT_SOULSTORM:
						_has_orb_adds = false; // From now on, no more ads that cause Blistering Cold explosions.
						// Notice: Resetting all current auras on boss, if you dont, the Soulstorm will most likely crash the client.
						me->RemoveAura(SPELL_BERSERK); // Extra, might not be neccesary.
						me->RemoveAura(AURA_SHADOW); // Extra, better safe than sorry.
						me->RemoveAllAuras();
						
						Talk(SAY_LK_QUAKE); _chatter_timer = 0;
						me->CastSpell(me, SPELL_SOULSTORM_VISUAL, true);
						me->CastSpell(me, SPELL_SOULSTORM, false);
						break;
					
					case EVENT_FROST_BEACON:
						if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM))
							DoCast(target, SPELL_FROST_BEACON);
						events.ScheduleEvent(EVENT_FROST_BEACON, urand(40000, 75000), 0, PHASE_TWO);
					break;
					
					case EVENT_HARVEST_SOUL:
						Talk(SAY_LK_HARVEST_SOUL); _chatter_timer = 0;
						if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM))
							// DoCast(target, SPELL_HARVEST_SOUL);
							DoCastAOE(SPELL_HARVEST_SOUL);
						events.ScheduleEvent(EVENT_HARVEST_SOUL, urand(40000, 75000), 0, PHASE_TWO);
					break;
					
					case EVENT_SOULFRAGMENT:
						if (me->GetAuraCount(AURA_SOULFRAGMENT) < 9) 
						{
							events.CancelEvent(EVENT_SOULFRAGMENT);
							DoCast(me, AURA_SOULFRAGMENT);
						}
						events.ScheduleEvent(EVENT_SOULFRAGMENT, AURA_SOULFRAGMENT_timer); // Lasts untill cancelled :)
					break;
					
					case EVENT_MIRROR_IMAGE:
						DoCast(me, MIRROR_IMAGE_PREEFFECT);
						events.ScheduleEvent(EVENT_MIRROR_IMAGE_MAINSPELL, 4800);
					break;
					case EVENT_MIRROR_IMAGE_MAINSPELL:
						for (uint8 i = 0; i < 2; ++i)
						{
							// DoCast(me, MIRROR_IMAGE);
							//Summon Clone, will dissappear after a short while when out of combat.
							Creature* MyDarkClone = DoSpawnCreature(99001, float(rand()%47), float(rand()%47), 0, 0, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 5000);
							Unit* Clonetarget = NULL;
							Clonetarget = SelectTarget(SELECT_TARGET_TOPAGGRO, 0);
							
							if (MyDarkClone && Clonetarget)
							{
								// MyDarkClone->CastSpell(MyDarkClone, SPELL_ORB_FLARE_PASSIVE, false);
								MyDarkClone->AddAura(AURA_GHOSTLY,MyDarkClone);
								MyDarkClone->AI()->AttackStart(Clonetarget);
							}
						events.ScheduleEvent(EVENT_MIRROR_IMAGE, MIRROR_IMAGE_TIMER); // Lasts untill cancelled :)
						}
					break;
					
					//
					// Defining Phases:
					//
					
					// Phase 2 Events:
					case EVENT_FRENZY:
						events.SetPhase(PHASE_TWO);
						
						events.ScheduleEvent(EVENT_DRAIN_MANA, 5000, 0, PHASE_TWO);
						events.ScheduleEvent(EVENT_SINBEAM, 2000, 0, PHASE_TWO);
						events.ScheduleEvent(EVENT_FROZEN_ORB, 1500, 0, PHASE_TWO);
						events.ScheduleEvent(EVENT_HARVEST_SOUL, urand(40000, 75000), 0, PHASE_TWO);
						events.ScheduleEvent(EVENT_BLISTERING_COLD, 120000, 0, PHASE_TWO);
						
						t = RAND(14808, 14800, 14787);
						if (t==14808) { me->MonsterYell("최후의 날이다!!!",LANG_UNIVERSAL,NULL); }
						if (t==14800) { me->MonsterYell("내 끔찍한 피조물 앞에서 몸서리쳐라.",LANG_UNIVERSAL,NULL); }
						if (t==14787) { me->MonsterYell("고통받아라 벌레들아!",LANG_UNIVERSAL,NULL); }
						me->SendPlaySound(t, false); _chatter_timer = 0;

						DoCast(me, SPELL_FRENZY, true);
					break;
					
					// Phase 1 Events:
					case EVENT_COMBAT:
						me->SetWalk(false);
													
						// Always running events
						_has_orb_adds = true;
						addztimer = 10000;

						// Starten random chats:
						events.ScheduleEvent(EVENT_RANDOM_CHAT, 21000);

						// If you uncomment next line, you will most certainly die (fast).
						// DoCast(me, SPELL_FROZEN_MALLET); // Will go insane with stacks...

						events.ScheduleEvent(EVENT_SOULFRAGMENT, 100);

						// Phase one events (regular form)
						events.ScheduleEvent(EVENT_MIRROR_IMAGE, 4000);
						events.ScheduleEvent(EVENT_SHADOW_SPIKE, 5000, 0, PHASE_ONE);
						events.ScheduleEvent(EVENT_CHAIN_LIGHTNING, 9000, 0, PHASE_ONE);
						events.ScheduleEvent(EVENT_FEAR, 25000, 0, PHASE_ONE);
						events.ScheduleEvent(EVENT_BLISTERING_COLD, urand(10000, 20000), 0, PHASE_ONE);
						events.ScheduleEvent(EVENT_WHITEOUT, 120000, 0, PHASE_ONE);
					break;
					
					// Intro Events:
					case EVENT_INTRO:
						Talk(SAY_LK_INTRO_1);
						events.ScheduleEvent(EVENT_INTROB, 12500);
						break;
					
					case EVENT_INTROB:
						Talk(SAY_LK_INTRO_2);
						events.ScheduleEvent(EVENT_COMBAT, 12500);
						break;
					
					default:
						break;
					
				}
			}
		  DoMeleeAttackIfReady();
		}
	};
};

class ballad_of_oldie : public CreatureScript
{
public:
    ballad_of_oldie() : CreatureScript("ballad_of_oldie") { }

    bool OnGossipHello(Player* player, Creature* creature) override
    {
        if (player->GetQuestStatus(11064) == QUEST_STATUS_INCOMPLETE)
        {
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "용아귀부족 경주: 노장의 연가", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
        }

        if (creature->IsQuestGiver())
            player->PrepareQuestMenu(creature->GetGUID());

        player->TalkedToCreature(creature->GetEntry(), creature->GetGUID());
        player->SEND_GOSSIP_MENU(player->GetGossipTextId(creature), creature->GetGUID());
        return true;
    }

    bool OnGossipSelect(Player* player, Creature* creature, uint32 /*sender*/, uint32 action) override
    {
        player->PlayerTalkClass->ClearMenus();
        if (action == GOSSIP_ACTION_INFO_DEF + 1)
        {
            player->CompleteQuest(11064);
            player->CLOSE_GOSSIP_MENU();
            return true;
        }
        return true;
    }
};

/*#####
# npc_argent squire
#####*/

enum squire
{
    ACH_PONY_UP                     = 3736,

    NPC_ARGENT_GRUNT                = 33239,
    NPC_ARGENT_SQUIRE               = 33238,

    SPELL_SQUIRE_MOUNT_CHECK        = 67039,
    SPELL_STORMWIND_PENNANT         = 62727,
    SPELL_SENJIN_PENNANT            = 63446,
    SPELL_DARNASSUS_PENNANT         = 63443,
    SPELL_EXODAR_PENNANT            = 63439,
    SPELL_UNDERCITY_PENNANT         = 63441,
    SPELL_GNOMEREAGAN_PENNANT       = 63442,
    SPELL_IRONFORGE_PENNANT         = 63440,
    SPELL_ORGRIMMAR_PENNANT         = 63444,
    SPELL_SILVERMOON_PENNANT        = 63438,
    SPELL_THUNDERBLUFF_PENNANT      = 63445,
    SPELL_SQUIRE_TIRED              = 67401,
    SPELL_SQUIRE_BANK               = 67368,
    SPELL_SQUIRE_SHOP               = 67377,
    SPELL_SQUIRE_POSTMAN            = 67376,
    SPELL_PLAYER_TIRED              = 67334
};

class npc_argent_squire : public CreatureScript
{
public:
    npc_argent_squire() : CreatureScript("npc_argent_squire") { }

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new npc_argent_squireAI(creature);
    }

    struct npc_argent_squireAI : public ScriptedAI
    {
        npc_argent_squireAI(Creature* creature) : ScriptedAI(creature) { }

        bool Bank;
        bool Shop;
        bool Mail;

        uint32 curPennant;

        void Reset()
        {
            curPennant = 0;
            Bank=false;
            Shop=false;
            Mail=false;

            if (Aura* tired = me->GetOwner()->GetAura(SPELL_PLAYER_TIRED))
            {
                int32 duration = tired->GetDuration();
                tired = me->AddAura(SPELL_SQUIRE_TIRED, me);
                tired->SetDuration(duration);
            }
        }

        void UpdateAI(uint32 diff)
        {
            Player* player = me->GetOwner()->ToPlayer();

            if (player->HasAchieved(ACH_PONY_UP))
                if (!me->HasAura(SPELL_SQUIRE_MOUNT_CHECK))
                    me->AddAura(SPELL_SQUIRE_MOUNT_CHECK,me);

            if (me->HasAura(SPELL_SQUIRE_TIRED)) // Make sure Tired aura is applied before Unsummon
            {
                if (Bank || Shop || Mail)
                {
                    me->DespawnOrUnsummon();
                }
            }
        }

        void sGossipSelect (Player* player, uint32 /*sender*/, uint32 action)
        {
            switch (action)
            {
                case 0:
                    player->CLOSE_GOSSIP_MENU();
                    if (!Bank)
                    {
                        Bank = true;
                        me->AddAura(SPELL_SQUIRE_BANK,me);
                        player->AddAura(SPELL_PLAYER_TIRED, player);
                        me->SetFlag(UNIT_NPC_FLAGS,UNIT_NPC_FLAG_BANKER);
                    }
                    player->GetSession()->SendShowBank(player->GetGUID());
                    break;
                case 1:
                    player->CLOSE_GOSSIP_MENU();
                    if (!Shop)
                    {
                        Shop = true;
                        me->AddAura(SPELL_SQUIRE_SHOP,me);
                        player->AddAura(SPELL_PLAYER_TIRED, player);
                    }
                    player->GetSession()->SendListInventory(me->GetGUID());
                    break;
                case 2:
                    player->CLOSE_GOSSIP_MENU();
                    if (!Mail)
                    {
                        me->RemoveFlag(UNIT_NPC_FLAGS,UNIT_NPC_FLAG_VENDOR);
                        me->SetFlag(UNIT_NPC_FLAGS,UNIT_NPC_FLAG_MAILBOX);
                        Mail = true;
                        me->AddAura(SPELL_SQUIRE_POSTMAN,me);
                        player->AddAura(SPELL_PLAYER_TIRED, player);
                    }
                    player->GetSession()->SendShowMailBox(me->GetGUID());
                    break;
                case 3: //Darnassus/Darkspear Pennant
                    player->CLOSE_GOSSIP_MENU();
                    me->RemoveAurasDueToSpell(curPennant);
                    if (me->GetEntry()==NPC_ARGENT_SQUIRE)
                    {
                        me->AddAura(SPELL_DARNASSUS_PENNANT,me);
                        curPennant = SPELL_DARNASSUS_PENNANT;
                    }
                    else
                    {
                        me->AddAura(SPELL_SENJIN_PENNANT,me);
                        curPennant = SPELL_SENJIN_PENNANT;
                    }
                    break;
                case 4: //Exodar/Forsaken Pennant
                    player->CLOSE_GOSSIP_MENU();
                    me->RemoveAurasDueToSpell(curPennant);
                    if (me->GetEntry()==NPC_ARGENT_SQUIRE)
                    {
                        me->AddAura(SPELL_EXODAR_PENNANT,me);
                        curPennant = SPELL_EXODAR_PENNANT;
                    }
                    else
                    {
                        me->AddAura(SPELL_UNDERCITY_PENNANT,me);
                        curPennant = SPELL_UNDERCITY_PENNANT;
                    }
                    break;
                case 5: //Gnomereagan/Orgrimmar Pennant
                    player->CLOSE_GOSSIP_MENU();
                    me->RemoveAurasDueToSpell(curPennant);
                    if (me->GetEntry()==NPC_ARGENT_SQUIRE)
                    {
                        me->AddAura(SPELL_GNOMEREAGAN_PENNANT,me);
                        curPennant = SPELL_GNOMEREAGAN_PENNANT;
                    }
                    else
                    {
                        me->AddAura(SPELL_ORGRIMMAR_PENNANT,me);
                        curPennant = SPELL_ORGRIMMAR_PENNANT;
                    }
                    break;
                case 6: //Ironforge/Silvermoon Pennant
                    player->CLOSE_GOSSIP_MENU();
                    me->RemoveAurasDueToSpell(curPennant);
                    if (me->GetEntry()==NPC_ARGENT_SQUIRE)
                    {
                        me->AddAura(SPELL_IRONFORGE_PENNANT,me);
                        curPennant = SPELL_IRONFORGE_PENNANT;
                    }
                    else
                    {
                        me->AddAura(SPELL_SILVERMOON_PENNANT,me);
                        curPennant = SPELL_SILVERMOON_PENNANT;
                    }
                    break;
                case 7: //Stormwind/Thunder Bluff Pennant
                    player->CLOSE_GOSSIP_MENU();
                    me->RemoveAurasDueToSpell(curPennant);
                    if (me->GetEntry()==NPC_ARGENT_SQUIRE)
                    {
                        me->AddAura(SPELL_STORMWIND_PENNANT,me);
                        curPennant = SPELL_STORMWIND_PENNANT;
                    }
                    else
                    {
                        me->AddAura(SPELL_THUNDERBLUFF_PENNANT,me);
                        curPennant = SPELL_THUNDERBLUFF_PENNANT;
                    }
                    break;
            }
        }

    };
};

class npc_roach : public CreatureScript
{
public:
    npc_roach() : CreatureScript("npc_roach") { }

    struct npc_roachAI : CasterAI
    {
        npc_roachAI(Creature* c) : CasterAI(c) {}

		uint32 m_uiTimer1;
		uint32 m_uiTimer2;
		uint32 m_uiTimer3;
		uint32 m_uiTimer4;

		void InitializeAI()
        {
            CasterAI::InitializeAI();
			me->setActive(true);
        }

		void Reset()
		{
			m_uiTimer1 = 0;
			m_uiTimer2 = 10000;
			m_uiTimer3 = 20000;
			m_uiTimer4 = 30000;
			me->setActive(true);
		}

        void UpdateAI(uint32 diff) override
        {
			CasterAI::UpdateAI(diff);
			std::ostringstream ss;

			uint32 guid  = me->GetGUIDLow();
			if (guid == 13761073)
			{
				if (m_uiTimer1 <= diff)
				{
					m_uiTimer1 = 30000;
					ss << "|cffff0000[바퀴벌레]:|r 동부왕국에 바퀴벌레가 나타 났어요!!!! 15분안에 안잡으면 그냥 죽어요!!!   1";
					sWorld->SendGlobalText(ss.str().c_str(), NULL);
				}
				else
					m_uiTimer1 -= diff;

				if (m_uiTimer2 <= diff)
				{
					m_uiTimer2 = 30000;
					ss << "|cffff0000[바퀴벌레]:|r 여기는 동굴 인것같아요!!!! ";
					sWorld->SendGlobalText(ss.str().c_str(), NULL);
				}
				else
					m_uiTimer2 -= diff;

				if (m_uiTimer3 <= diff)
				{
					m_uiTimer3 = 30000;
					ss << "|cffff0000[바퀴벌레]:|r 골드샤이어가 보여요!!!! ";
					sWorld->SendGlobalText(ss.str().c_str(), NULL);
				}
				else
					m_uiTimer3 -= diff;

				if (m_uiTimer4 <= diff)
				{
					me->SetVisible(false);
					me->DespawnOrUnsummon();
					ss << "|cffff0000[바퀴벌레]:|r 바이바이!!!! ";
					sWorld->SendGlobalText(ss.str().c_str(), NULL);
				}
				else
					m_uiTimer4 -= diff;
			}

			if (guid == 13761074)
			{
				if (m_uiTimer1 <= diff)
				{
					m_uiTimer1 = 30000;
					ss << "|cffff0000[바퀴벌레]:|r 동부왕국에 바퀴벌레가 나타 났어요!!!! 15분안에 안잡으면 그냥 죽어요!!!   1";
					sWorld->SendGlobalText(ss.str().c_str(), NULL);
				}

				if (m_uiTimer2 <= diff)
				{
					m_uiTimer2 = 30000;
					ss << "|cffff0000[바퀴벌레]:|r 여기는 동굴 인것같아요!!!! ";
					sWorld->SendGlobalText(ss.str().c_str(), NULL);
				}

				if (m_uiTimer3 <= diff)
				{
					m_uiTimer3 = 30000;
					ss << "|cffff0000[바퀴벌레]:|r 골드샤이어가 보여요!!!! ";
					sWorld->SendGlobalText(ss.str().c_str(), NULL);
				}

				if (m_uiTimer4 <= diff)
				{
					me->SetVisible(false);
					me->DespawnOrUnsummon();
					ss << "|cffff0000[바퀴벌레]:|r 바이바이!!!! ";
					sWorld->SendGlobalText(ss.str().c_str(), NULL);
				}
			}
		}
    };

    CreatureAI *GetAI(Creature* creature) const
    {
        return new npc_roachAI(creature);
    }
};

class npc_edana_hatetalon : public CreatureScript
{
    public:
        npc_edana_hatetalon() : CreatureScript("npc_edana_hatetalon") { }

        struct npc_edana_hatetalonAI : public ScriptedAI
        {
            npc_edana_hatetalonAI(Creature* c) : ScriptedAI(c) {}

             void Reset() override
            {
                me->GetMotionMaster()->MovePoint(0, -2772.457f, 2630.383f, 72.317f);
            }

        };

        CreatureAI *GetAI(Creature* creature) const
		{
			return new npc_edana_hatetalonAI(creature);
		}
};

class npc_voidwalker_minion : public CreatureScript
{
    public:
        npc_voidwalker_minion() : CreatureScript("npc_voidwalker_minion") { }

        struct npc_voidwalker_minionAI : public ScriptedAI
        {
            npc_voidwalker_minionAI(Creature* c) : ScriptedAI(c) {}

			void UpdateAI(uint32 diff) override
			{
				if (Unit* owner = me->GetOwner())
					if(owner->GetDistance(me) >= 40)
					{
						me->ClearInCombat();
						me->GetMotionMaster()->MoveTargetedHome();
					}
			}

        };

        CreatureAI *GetAI(Creature* creature) const
		{
			return new npc_voidwalker_minionAI(creature);
		}
};

class npc_q4041 : public CreatureScript
{
	private:
    enum
    {
		EVENT_WALK_INTRO1,
		EVENT_WALK_INTRO2,
		EVENT_WALK_INTRO3,
    };

    public:
        npc_q4041() : CreatureScript("npc_q4041") { }

        struct npc_q4041AI : public ScriptedAI
        {
            npc_q4041AI(Creature* c) : ScriptedAI(c) {}

			void Reset() override
            {
				_events.Reset();
            }

			void UpdateAI(uint32 diff) override
			{
				_events.Update(diff);

				if (GameObject* obj =  me->FindNearestGameObject(164758, 5))
				{
					_events.ScheduleEvent(EVENT_WALK_INTRO1, 3000);

				switch (_events.ExecuteEvent())
				{
					case EVENT_WALK_INTRO1:
						me->GetMotionMaster()->MovePoint(0, obj->GetPositionX(), obj->GetPositionY(), obj->GetPositionZ());
						_events.ScheduleEvent(EVENT_WALK_INTRO2, 30000);
						break;
					case EVENT_WALK_INTRO2:
						obj->RemoveFromWorld();
						me->GetMotionMaster()->MoveTargetedHome();
						_events.ScheduleEvent(EVENT_WALK_INTRO3, 3000);
						break;
					case EVENT_WALK_INTRO3:
						_events.Reset();
						break;
				}
				}
			}
		private:
            EventMap _events;
        };

        CreatureAI *GetAI(Creature* creature) const
		{
			return new npc_q4041AI(creature);
		}
};

class npc_relentless : public CreatureScript
{
public:
    npc_relentless() : CreatureScript("npc_relentless") { }

    bool OnGossipHello(Player* player, Creature* creature) override
    {
		Creature* rabbit = player->FindNearestCreature(5000091, 30.0f);
		Creature* rabbit1 = player->FindNearestCreature(5000060, 30.0f);
		Creature* rabbit2 = player->FindNearestCreature(5000058, 30.0f);
		Creature* rabbit3 = player->FindNearestCreature(5000054, 30.0f);
		Creature* rabbit4 = player->FindNearestCreature(5000052, 30.0f);

		if (!rabbit && !rabbit1 && !rabbit2 && !rabbit3 && !rabbit4 )
		{
			player->ADD_GOSSIP_ITEM(GOSSIP_ICON_VENDOR, "격노한 장비상인을 불러주세요!", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_TRADE);
			player->ADD_GOSSIP_ITEM(GOSSIP_ICON_VENDOR, "격노한 무기상인을 불러주세요!", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_TRADE + 1);
			player->ADD_GOSSIP_ITEM(GOSSIP_ICON_VENDOR, "냉혹한 장비상인을 불러주세요!", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_TRADE + 2);
			player->ADD_GOSSIP_ITEM(GOSSIP_ICON_VENDOR, "냉혹한 무기상인을 불러주세요!", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_TRADE + 3);
			player->ADD_GOSSIP_ITEM(GOSSIP_ICON_VENDOR, "잡상인을 불러주세요!", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_TRADE + 4);
		}

		player->TalkedToCreature(creature->GetEntry(), creature->GetGUID());
        player->SEND_GOSSIP_MENU(player->GetGossipTextId(creature), creature->GetGUID());
        return true;
    }

    bool OnGossipSelect(Player* player, Creature* creature, uint32 /*sender*/, uint32 action) override
    {
        player->PlayerTalkClass->ClearMenus();
        player->CLOSE_GOSSIP_MENU();

		uint32 faction = 35;
		uint32 displayID = 35;
		if (Battlefield* wg = sBattlefieldMgr->GetBattlefieldByBattleId(BATTLEFIELD_BATTLEID_WG))
		{
			if (wg->GetDefenderTeam() == TEAM_HORDE)
			{
				faction = 1981;
				switch (urand(0, 1))
				{
					case 0:
						displayID = 24074;
						break;
					case 1:
						displayID = 24076;
						break;
				}
			}
			else
			{
				faction = 1892;
				switch (urand(0, 1))
				{
					case 0:
						displayID = 25951;
						break;
					case 1:
						displayID = 25948;
						break;
				}
			}
		}

        switch (action)
        {
            case GOSSIP_ACTION_TRADE: 
				if (Creature* creature = player->SummonCreature(5000091, 5367.049f, 2851.83f, 409.23f, 3.190150f, TEMPSUMMON_TIMED_DESPAWN, 120000))
				{
					creature->setFaction(faction);
					creature->SetDisplayId(displayID);
				}
				break;
			case GOSSIP_ACTION_TRADE + 1: 
				if (Creature* creature = player->SummonCreature(5000060, 5367.049f, 2851.83f, 409.23f, 3.190150f, TEMPSUMMON_TIMED_DESPAWN, 120000))
				{
					creature->setFaction(faction);
					creature->SetDisplayId(displayID);
				}
				break;
			case GOSSIP_ACTION_TRADE + 2: 
				if (Creature* creature = player->SummonCreature(5000058, 5367.049f, 2851.83f, 409.23f, 3.190150f, TEMPSUMMON_TIMED_DESPAWN, 120000))
				{
					creature->setFaction(faction);
					creature->SetDisplayId(displayID);
				}
				break;
			case GOSSIP_ACTION_TRADE + 3: 
				if (Creature* creature = player->SummonCreature(5000054, 5367.049f, 2851.83f, 409.23f, 3.190150f, TEMPSUMMON_TIMED_DESPAWN, 120000))
				{
					creature->setFaction(faction);
					creature->SetDisplayId(displayID);
				}
				break;
			case GOSSIP_ACTION_TRADE + 4: 
				if (Creature* creature = player->SummonCreature(5000052, 5367.049f, 2851.83f, 409.23f, 3.190150f, TEMPSUMMON_TIMED_DESPAWN, 120000))
				{
					creature->setFaction(faction);
					creature->SetDisplayId(displayID);
				}
				break;
        }
        return true;
    }

	struct npc_relentlessAI : public ScriptedAI
    {
        npc_relentlessAI(Creature* c) : ScriptedAI(c) {}

		void UpdateAI(uint32 diff) override
		{
			uint32 displayID = 35;
			if (Battlefield* wg = sBattlefieldMgr->GetBattlefieldByBattleId(BATTLEFIELD_BATTLEID_WG))
			{
				if (wg->GetDefenderTeam() == TEAM_HORDE)
				{
					me->setFaction(1981);
					me->SetDisplayId(24074);
				}
				else
				{
					me->setFaction(1892);
					me->SetDisplayId(25951);
				}
			}
		}
    };

    CreatureAI *GetAI(Creature* creature) const
	{
		return new npc_relentlessAI(creature);
	}
};

class npc_om_point : public CreatureScript
{
    public:
       npc_om_point() : CreatureScript("npc_om_point") { }

        struct npc_om_pointAI : public ScriptedAI
        {
            npc_om_pointAI(Creature* c) : ScriptedAI(c) {}

			uint32 m_uiTimer;


			void Reset()
			{
				m_uiTimer = 120000;
				me->SetCanFly(true);
			}

		   void UpdateAI(const uint32 diff)
		   {
				if (m_uiTimer <= diff)
				{
					if (GameObject* obj =  me->FindNearestGameObject(1910090, 20))
					{
						obj->UseDoorOrButton();
						me->RemoveFromWorld();
					}
				}
				else
					m_uiTimer -= diff;
		   }
        };

        CreatureAI *GetAI(Creature* creature) const
		{
			return new npc_om_pointAI(creature);
		}
};

class npc_fun_and_games : public CreatureScript
{
public:
    npc_fun_and_games() : CreatureScript("npc_fun_and_games") { }

    bool OnGossipHello(Player* player, Creature* creature) override
    {
        if (player->GetQuestStatus(12887) == QUEST_STATUS_INCOMPLETE || player->GetQuestStatus(12892) == QUEST_STATUS_INCOMPLETE)
        {
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "즐거운 장난", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
        }

        if (creature->IsQuestGiver())
            player->PrepareQuestMenu(creature->GetGUID());

        player->TalkedToCreature(creature->GetEntry(), creature->GetGUID());
        player->SEND_GOSSIP_MENU(player->GetGossipTextId(creature), creature->GetGUID());
        return true;
    }

    bool OnGossipSelect(Player* player, Creature* creature, uint32 /*sender*/, uint32 action) override
    {
        player->PlayerTalkClass->ClearMenus();
        if (action == GOSSIP_ACTION_INFO_DEF + 1)
        {
			if (player->GetQuestStatus(12887) == QUEST_STATUS_INCOMPLETE)
				player->CompleteQuest(12887);

			if (player->GetQuestStatus(12892) == QUEST_STATUS_INCOMPLETE)
				player->CompleteQuest(12892);
            player->CLOSE_GOSSIP_MENU();
            return true;
        }
        return true;
    }
};

// Dark Iron Guzzler in the Brewfest achievement 'Down With The Dark Iron'
enum DarkIronGuzzler
{
    NPC_DARK_IRON_GUZZLER = 23709,
    NPC_DARK_IRON_HERALD = 24536,
    NPC_DARK_IRON_SPAWN_BUNNY = 23894,

    NPC_FESTIVE_KEG_1 = 23702, // Thunderbrew Festive Keg
    NPC_FESTIVE_KEG_2 = 23700, // Barleybrew Festive Keg
    NPC_FESTIVE_KEG_3 = 23706, // Gordok Festive Keg
    NPC_FESTIVE_KEG_4 = 24373, // T'chalis's Festive Keg
    NPC_FESTIVE_KEG_5 = 24372, // Drohn's Festive Keg

    SPELL_GO_TO_NEW_TARGET = 42498,
    SPELL_ATTACK_KEG = 42393,
    SPELL_RETREAT = 42341,
    SPELL_DRINK = 42436,

    SAY_RANDOM = 0,
};

class npc_dark_iron_guzzler : public CreatureScript
{
public:
    npc_dark_iron_guzzler() : CreatureScript("npc_dark_iron_guzzler") { }

    CreatureAI *GetAI(Creature* creature) const
    {
        return new npc_dark_iron_guzzlerAI(creature);
    }

    struct npc_dark_iron_guzzlerAI : public ScriptedAI
    {
        npc_dark_iron_guzzlerAI(Creature* creature) : ScriptedAI(creature) { }

        bool atKeg;
        bool playersLost;
        bool barleyAlive;
        bool thunderAlive;
        bool gordokAlive;
        bool drohnAlive;
        bool tchaliAlive;

        uint32 AttackKegTimer;
        uint32 TalkTimer;

        void Reset()
        {
            AttackKegTimer = 5000;
            TalkTimer = (urand(1000, 120000));
            me->AddUnitMovementFlag(MOVEMENTFLAG_WALKING);
        }

        void IsSummonedBy(Unit* summoner)
        {
            // Only cast the spell on spawn
            DoCast(me, SPELL_GO_TO_NEW_TARGET);
        }

        // These values are set through SAI - when a Festive Keg dies it will set data to all Dark Iron Guzzlers within 3 yards (the killers)
        void SetData(uint32 type, uint32 data)
        {
            if (type == 10 && data == 10)
            {
                DoCast(me, SPELL_GO_TO_NEW_TARGET);
                thunderAlive = false;
            }

            if (type == 11 && data == 11)
            {
                DoCast(me, SPELL_GO_TO_NEW_TARGET);
                barleyAlive = false;
            }

            if (type == 12 && data == 12)
            {
                DoCast(me, SPELL_GO_TO_NEW_TARGET);
                gordokAlive = false;
            }

            if (type == 13 && data == 13)
            {
                DoCast(me, SPELL_GO_TO_NEW_TARGET);
                drohnAlive = false;
            }

            if (type == 14 && data == 14)
            {
                DoCast(me, SPELL_GO_TO_NEW_TARGET);
                tchaliAlive = false;
            }
        }

        // As you can see here we do not have to use a spellscript for this
        void SpellHit(Unit* caster, const SpellInfo* spell)
        {
            if (spell->Id == SPELL_DRINK)
            {
                // Fake death - it's only visual!
                me->SetUInt32Value(UNIT_FIELD_BYTES_1, UNIT_STAND_STATE_DEAD);
                me->StopMoving();

                // Time based on information from videos
                me->DespawnOrUnsummon(7000);
            }

            // Retreat - run back
            if (spell->Id == SPELL_RETREAT)
            {
                // Remove walking flag so we start running
                me->RemoveUnitMovementFlag(MOVEMENTFLAG_WALKING);

                if (me->GetAreaId() == 1296)
                {
                    me->GetMotionMaster()->MovePoint(1, 1197.63f, -4293.571f, 21.243f);
                }
                else if (me->GetAreaId() == 1)
                {
                    me->GetMotionMaster()->MovePoint(2, -5152.3f, -603.529f, 398.356f);
                }
            }

            if (spell->Id == SPELL_GO_TO_NEW_TARGET)
            {
                // If we're at Durotar we target different kegs if we are at at Dun Morogh
                if (me->GetAreaId() == 1296)
                {
                    if (drohnAlive && gordokAlive && tchaliAlive)
                    {
                        switch (urand(0, 2))
                        {
                        case 0: // Gordok Festive Keg
                            me->GetMotionMaster()->MovePoint(4, 1220.86f, -4297.37f, 21.192f);
                            break;
                        case 1: // Drohn's Festive Keg
                            me->GetMotionMaster()->MovePoint(5, 1185.98f, -4312.98f, 21.294f);
                            break;
                        case 2: // Ti'chali's Festive Keg
                            me->GetMotionMaster()->MovePoint(6, 1184.12f, -4275.21f, 21.191f);
                            break;
                        }
                    }
                    else if (!drohnAlive)
                    {
                        switch (urand(0, 1))
                        {
                        case 0: // Gordok Festive Keg
                            me->GetMotionMaster()->MovePoint(4, 1220.86f, -4297.37f, 21.192f);
                            break;
                        case 1: // Ti'chali's Festive Keg
                            me->GetMotionMaster()->MovePoint(6, 1184.12f, -4275.21f, 21.191f);
                            break;
                        }
                    }
                    else if (!gordokAlive)
                    {
                        switch (urand(0, 1))
                        {
                        case 0: // Drohn's Festive Keg
                            me->GetMotionMaster()->MovePoint(5, 1185.98f, -4312.98f, 21.294f);
                            break;
                        case 1: // Ti'chali's Festive Keg
                            me->GetMotionMaster()->MovePoint(6, 1184.12f, -4275.21f, 21.191f);
                            break;
                        }
                    }
                    else if (!tchaliAlive)
                    {
                        switch (urand(0, 1))
                        {
                        case 0: // Gordok Festive Keg
                            me->GetMotionMaster()->MovePoint(4, 1220.86f, -4297.37f, 21.192f);
                            break;
                        case 1: // Drohn's Festive Keg
                            me->GetMotionMaster()->MovePoint(5, 1185.98f, -4312.98f, 21.294f);
                            break;
                        }
                    }
                }
                // If we're at Dun Morogh we target different kegs if we are at Durotar
                else if (me->GetAreaId() == 1)
                {
                    if (barleyAlive && gordokAlive && thunderAlive)
                    {
                        switch (urand(0, 2))
                        {
                        case 0: // Barleybrew Festive Keg
                            me->GetMotionMaster()->MovePoint(7, -5183.67f, -599.58f, 397.177f);
                            break;
                        case 1: // Thunderbrew Festive Keg
                            me->GetMotionMaster()->MovePoint(8, -5159.53f, -629.52f, 397.213f);
                            break;
                        case 2: // Gordok Festive Keg
                            me->GetMotionMaster()->MovePoint(9, -5148.01f, -578.34f, 397.177f);
                            break;
                        }
                    }
                    else if (!barleyAlive)
                    {
                        switch (urand(0, 1))
                        {
                        case 0: // Thunderbrew Festive Keg
                            me->GetMotionMaster()->MovePoint(8, -5159.53f, -629.52f, 397.213f);
                            break;
                        case 1: // Gordok Festive Keg
                            me->GetMotionMaster()->MovePoint(9, -5148.01f, -578.34f, 397.177f);
                            break;
                        }
                    }
                    else if (!gordokAlive)
                    {
                        switch (urand(0, 1))
                        {
                        case 0: // Barleybrew Festive Keg
                            me->GetMotionMaster()->MovePoint(7, -5183.67f, -599.58f, 397.177f);
                            break;
                        case 1: // Thunderbrew Festive Keg
                            me->GetMotionMaster()->MovePoint(8, -5159.53f, -629.52f, 397.213f);
                            break;
                        }
                    }
                    else if (!thunderAlive)
                    {
                        switch (urand(0, 1))
                        {
                        case 0: // Barleybrew Festive Keg
                            me->GetMotionMaster()->MovePoint(7, -5183.67f, -599.58f, 397.177f);
                            break;
                        case 1: // Gordok Festive Keg
                            me->GetMotionMaster()->MovePoint(9, -5148.01f, -578.34f, 397.177f);
                            break;
                        }
                    }
                }
                atKeg = false;
            }
        }

        void MovementInform(uint32 Type, uint32 PointId)
        {
            if (Type != POINT_MOTION_TYPE)
                return;

            // Arrived at the retreat spot, we should despawn
            if (PointId == 1 || PointId == 2)
                me->DespawnOrUnsummon(7000);

            // Arrived at the new keg - the spell has conditions in database
            if (PointId == 4 || PointId == 5 || PointId == 6 || PointId == 7 || PointId == 8 || PointId == 9)
            {
                DoCast(SPELL_ATTACK_KEG);
                me->SetByteFlag(UNIT_FIELD_BYTES_1, 1, 0x01); // Sit down
                atKeg = true;
            }
        }

        void UpdateAI(const uint32 diff)
        {
            if (!IsHolidayActive(HOLIDAY_BREWFEST))
                return;

            // If all kegs are dead we should retreat because we have won
            if ((!gordokAlive && !thunderAlive && !barleyAlive) || (!gordokAlive && !drohnAlive && !tchaliAlive))
            {
                DoCast(me, SPELL_RETREAT);

                // We are doing this because we'll have to reset our scripts when we won
                if (Creature* herald = me->FindNearestCreature(NPC_DARK_IRON_HERALD, 100.0f))
                    herald->AI()->SetData(20, 20);

                // Despawn all summon bunnies so they will stop summoning guzzlers
                if (Creature* spawnbunny = me->FindNearestCreature(NPC_DARK_IRON_SPAWN_BUNNY, 100.0f))
                    spawnbunny->DespawnOrUnsummon();
            }

            if (TalkTimer <= diff)
            {
                me->AI()->Talk(SAY_RANDOM);
                TalkTimer = (urand(44000, 120000));
            } else TalkTimer -= diff;

            // Only happens if we're at keg
            if (atKeg)
            {
                if (AttackKegTimer <= diff)
                {
                    DoCast(SPELL_ATTACK_KEG);
                    AttackKegTimer = 5000;
                } else AttackKegTimer -= diff;
            }
        }
    };
};

enum TrainWrecker
{
    GO_TOY_TRAIN          = 193963,
    SPELL_TOY_TRAIN_PULSE =  61551,
    SPELL_WRECK_TRAIN     =  62943,
    ACTION_WRECKED        =      1,
    EVENT_DO_JUMP         =      1,
    EVENT_DO_FACING       =      2,
    EVENT_DO_WRECK        =      3,
    EVENT_DO_DANCE        =      4,
    MOVEID_CHASE          =      1,
    MOVEID_JUMP           =      2
};
class npc_train_wrecker : public CreatureScript
{
    public:
        npc_train_wrecker() : CreatureScript("npc_train_wrecker") { }

        struct npc_train_wreckerAI : public NullCreatureAI
        {
            npc_train_wreckerAI(Creature* creature) : NullCreatureAI(creature), _isSearching(true), _nextAction(0), _timer(1 * IN_MILLISECONDS) { }

            GameObject* VerifyTarget() const
            {
                if (GameObject* target = ObjectAccessor::GetGameObject(*me, _target))
                    return target;
                me->HandleEmoteCommand(EMOTE_ONESHOT_RUDE);
                me->DespawnOrUnsummon(3 * IN_MILLISECONDS);
                return nullptr;
            }

            void UpdateAI(uint32 diff) override
            {
                if (_isSearching)
                {
                    if (diff < _timer)
                        _timer -= diff;
                    else
                    {
                        if (GameObject* target = me->FindNearestGameObject(GO_TOY_TRAIN, 15.0f))
                        {
                            _isSearching = false;
                            _target = target->GetGUID();
                            me->SetWalk(true);
                            me->GetMotionMaster()->MovePoint(MOVEID_CHASE, target->GetNearPosition(3.0f, target->GetAngle(me)));
                        }
                        else
                            _timer = 3 * IN_MILLISECONDS;
                    }
                }
                else
                {
                    switch (_nextAction)
                    {
                        case EVENT_DO_JUMP:
                            if (GameObject* target = VerifyTarget())
                                me->GetMotionMaster()->MoveJump(*target, 5.0, 10.0, MOVEID_JUMP);
                            _nextAction = 0;
                            break;
                        case EVENT_DO_FACING:
                            if (GameObject* target = VerifyTarget())
                            {
                                me->SetFacingTo(target->GetOrientation());
                                me->HandleEmoteCommand(EMOTE_ONESHOT_ATTACK1H);
                                _timer = 1.5 * IN_MILLISECONDS;
                                _nextAction = EVENT_DO_WRECK;
                            }
                            else
                                _nextAction = 0;
                            break;
                        case EVENT_DO_WRECK:
                            if (diff < _timer)
                            {
                                _timer -= diff;
                                break;
                            }
                            if (GameObject* target = VerifyTarget())
                            {
                                me->CastSpell(target, SPELL_WRECK_TRAIN, false);
                                target->AI()->DoAction(ACTION_WRECKED);
                                _timer = 2 * IN_MILLISECONDS;
                                _nextAction = EVENT_DO_DANCE;
                            }
                            else
                                _nextAction = 0;
                            break;
                        case EVENT_DO_DANCE:
                            if (diff < _timer)
                            {
                                _timer -= diff;
                                break;
                            }
                            me->SetUInt32Value(UNIT_NPC_EMOTESTATE, EMOTE_ONESHOT_DANCE);
                            me->DespawnOrUnsummon(5 * IN_MILLISECONDS);
                            _nextAction = 0;
                            break;
                        default:
                            break;
                    }
                }
            }

            void MovementInform(uint32 /*type*/, uint32 id)
            {
                if (id == MOVEID_CHASE)
                    _nextAction = EVENT_DO_JUMP;
                else if (id == MOVEID_JUMP)
                    _nextAction = EVENT_DO_FACING;
            }

        private:
            bool _isSearching;
            uint8 _nextAction;
            uint32 _timer;
            uint64 _target;
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_train_wreckerAI(creature);
        }
};

/*######
## npc_torch_tossing_target_bunny_controller
######*/

enum TorchTossingTarget
{
    NPC_TORCH_TOSSING_TARGET_BUNNY      = 25535,
    SPELL_TARGET_INDICATOR              = 45723
};

class npc_torch_tossing_target_bunny_controller : public CreatureScript
{
public:
    npc_torch_tossing_target_bunny_controller() : CreatureScript("npc_torch_tossing_target_bunny_controller") { }

    struct npc_torch_tossing_target_bunny_controllerAI : public ScriptedAI
    {
        npc_torch_tossing_target_bunny_controllerAI(Creature* creature) : ScriptedAI(creature)
        {
            _targetTimer = 3000;
        }

        uint64 DoSearchForTargets(uint64 lastTargetGUID)
        {
            std::list<Creature*> targets;
            me->GetCreatureListWithEntryInGrid(targets, NPC_TORCH_TOSSING_TARGET_BUNNY, 60.0f);
            targets.remove_if([lastTargetGUID](Creature* creature) { return creature->GetGUID() == lastTargetGUID; });

            if (!targets.empty())
            {
                _lastTargetGUID = Trinity::Containers::SelectRandomContainerElement(targets)->GetGUID();

                return _lastTargetGUID;
            }
            return 0;
		}

        void UpdateAI(uint32 diff) override
        {
            if (_targetTimer < diff)
            {
                if (Unit* target = ObjectAccessor::GetUnit(*me, DoSearchForTargets(_lastTargetGUID)))
                    target->CastSpell(target, SPELL_TARGET_INDICATOR, true);

                _targetTimer = 3000;
            }
            else
                _targetTimer -= diff;
        }

    private:
        uint32 _targetTimer;
        uint64 _lastTargetGUID;
    };

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new npc_torch_tossing_target_bunny_controllerAI(creature);
    }
};

enum StableMasters
{
    SPELL_MINIWING                  = 54573,
    SPELL_JUBLING                   = 54611,
    SPELL_DARTER                    = 54619,
    SPELL_WORG                      = 54631,
    SPELL_SMOLDERWEB                = 54634,
    SPELL_CHIKEN                    = 54677,
    SPELL_WOLPERTINGER              = 54688,

    STABLE_MASTER_GOSSIP_SUB_MENU   = 9820
};

class npc_stable_master : public CreatureScript
{
    public:
        npc_stable_master() : CreatureScript("npc_stable_master") { }

        struct npc_stable_masterAI : public SmartAI
        {
            npc_stable_masterAI(Creature* creature) : SmartAI(creature) { }

            void sGossipSelect(Player* player, uint32 menuId, uint32 gossipListId) override
            {
                SmartAI::sGossipSelect(player, menuId, gossipListId);
                if (menuId != STABLE_MASTER_GOSSIP_SUB_MENU)
                    return;

                switch (gossipListId)
                {
                    case 0:
                        player->CastSpell(player, SPELL_MINIWING, false);
                        break;
                    case 1:
                        player->CastSpell(player, SPELL_JUBLING, false);
                        break;
                    case 2:
                        player->CastSpell(player, SPELL_DARTER, false);
                        break;
                    case 3:
                        player->CastSpell(player, SPELL_WORG, false);
                        break;
                    case 4:
                        player->CastSpell(player, SPELL_SMOLDERWEB, false);
                        break;
                    case 5:
                        player->CastSpell(player, SPELL_CHIKEN, false);
                        break;
                    case 6:
                        player->CastSpell(player, SPELL_WOLPERTINGER, false);
                        break;
                    default:
                        return;
                }

                player->PlayerTalkClass->SendCloseGossip();
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_stable_masterAI(creature);
        }
};

/*######
## npc_midsummer_bunny_pole
######*/

enum RibbonPoleData
{
    GO_RIBBON_POLE              = 181605,
    SPELL_RIBBON_DANCE_COSMETIC = 29726,
    SPELL_RED_FIRE_RING         = 46836,
    SPELL_BLUE_FIRE_RING        = 46842,
    EVENT_CAST_RED_FIRE_RING    = 1,
    EVENT_CAST_BLUE_FIRE_RING   = 2
};

class npc_midsummer_bunny_pole : public CreatureScript
{
public:
    npc_midsummer_bunny_pole() : CreatureScript("npc_midsummer_bunny_pole") { }

    struct npc_midsummer_bunny_poleAI : public ScriptedAI
    {
        npc_midsummer_bunny_poleAI(Creature* creature) : ScriptedAI(creature)
        {
            Initialize();
        }

        void Initialize()
        {
            events.Reset();
            running = false;
        }
        
        void Reset() override
        {
            Initialize();
        }

        void DoAction(int32 /*action*/) override
        {
            // Don't start event if it's already running.
            if (running)
                return;

            running = true;
            events.ScheduleEvent(EVENT_CAST_RED_FIRE_RING, 1);
        }

        bool checkNearbyPlayers()
        {
            // Returns true if no nearby player has aura "Test Ribbon Pole Channel".
            std::list<Player*> players;
            Trinity::UnitAuraCheck check(true, SPELL_RIBBON_DANCE_COSMETIC);
            Trinity::PlayerListSearcher<Trinity::UnitAuraCheck> searcher(me, players, check);
            me->VisitNearbyWorldObject(10.0f, searcher);

            return players.empty();
        }

        void UpdateAI(uint32 diff) override
        {
            if (!running)
                return;

            events.Update(diff);

            switch (events.ExecuteEvent())
            {
            case EVENT_CAST_RED_FIRE_RING:
            {
                if (checkNearbyPlayers())
                {
                    Reset();
                    return;
                }

                if (GameObject* go = me->FindNearestGameObject(GO_RIBBON_POLE, 10.0f))
                    me->CastSpell(go, SPELL_RED_FIRE_RING, true);

                events.ScheduleEvent(EVENT_CAST_BLUE_FIRE_RING, 5000);
            }
            break;
            case EVENT_CAST_BLUE_FIRE_RING:
            {
                if (checkNearbyPlayers())
                {
                    Reset();
                    return;
                }

                if (GameObject* go = me->FindNearestGameObject(GO_RIBBON_POLE, 10.0f))
                    me->CastSpell(go, SPELL_BLUE_FIRE_RING, true);

                events.ScheduleEvent(EVENT_CAST_RED_FIRE_RING, 5000);
            }
            break;
            }
        }

    private:
        EventMap events;
        bool running;
    };

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new npc_midsummer_bunny_poleAI(creature);
    }
};

enum BountifulTable
{
    SEAT_TURKEY_CHAIR                       = 0,
    SEAT_CRANBERRY_CHAIR                    = 1,
    SEAT_STUFFING_CHAIR                     = 2,
    SEAT_SWEET_POTATO_CHAIR                 = 3,
    SEAT_PIE_CHAIR                          = 4,
    SEAT_FOOD_HOLDER                        = 5,
    SEAT_PLATE_HOLDER                       = 6,
    NPC_THE_TURKEY_CHAIR                    = 34812,
    NPC_THE_CRANBERRY_CHAIR                 = 34823,
    NPC_THE_STUFFING_CHAIR                  = 34819,
    NPC_THE_SWEET_POTATO_CHAIR              = 34824,
    NPC_THE_PIE_CHAIR                       = 34822,
    SPELL_CRANBERRY_SERVER                  = 61793,
    SPELL_PIE_SERVER                        = 61794,
    SPELL_STUFFING_SERVER                   = 61795,
    SPELL_TURKEY_SERVER                     = 61796,
    SPELL_SWEET_POTATOES_SERVER             = 61797
};

typedef std::unordered_map<uint32 /*Entry*/, uint32 /*Spell*/> ChairSpells;
ChairSpells const _chairSpells =
{
    { NPC_THE_CRANBERRY_CHAIR, SPELL_CRANBERRY_SERVER },
    { NPC_THE_PIE_CHAIR, SPELL_PIE_SERVER },
    { NPC_THE_STUFFING_CHAIR, SPELL_STUFFING_SERVER },
    { NPC_THE_TURKEY_CHAIR, SPELL_TURKEY_SERVER },
    { NPC_THE_SWEET_POTATO_CHAIR, SPELL_SWEET_POTATOES_SERVER },
};

class CastFoodSpell : public BasicEvent
{
    public:
        CastFoodSpell(Unit* owner, uint32 spellId) : _owner(owner), _spellId(spellId) { }

        bool Execute(uint64 /*execTime*/, uint32 /*diff*/) override
        {
            _owner->CastSpell(_owner, _spellId, true);
            return false;
        }

    private:
        Unit* _owner;
        uint32 _spellId;
};

class npc_bountiful_table : public CreatureScript
{
public:
    npc_bountiful_table() : CreatureScript("npc_bountiful_table") { }

    struct npc_bountiful_tableAI : public PassiveAI
    {
        npc_bountiful_tableAI(Creature* creature) : PassiveAI(creature) { }

        void PassengerBoarded(Unit* who, int8 seatId, bool /*apply*/) override
        {
            float x = 0.0f;
            float y = 0.0f;
            float z = 0.0f;
            float o = 0.0f;

            switch (seatId)
            {
                case SEAT_TURKEY_CHAIR:
                    x = 3.87f;
                    y = 2.07f;
                    o = 3.700098f;
                    break;
                case SEAT_CRANBERRY_CHAIR:
                    x = 3.87f;
                    y = -2.07f;
                    o = 2.460914f;
                    break;
                case SEAT_STUFFING_CHAIR:
                    x = -2.52f;
                    break;
                case SEAT_SWEET_POTATO_CHAIR:
                    x = -0.09f;
                    y = -3.24f;
                    o = 1.186824f;
                    break;
                case SEAT_PIE_CHAIR:
                    x = -0.18f;
                    y = 3.24f;
                    o = 5.009095f;
                    break;
                case SEAT_FOOD_HOLDER:
                case SEAT_PLATE_HOLDER:
                    if (Vehicle* holders = who->GetVehicleKit())
                        holders->InstallAllAccessories(true);
                    return;
                default:
                    break;
            }

            Movement::MoveSplineInit init(who);
            init.DisableTransportPathTransformations();
            init.MoveTo(x, y, z, false);
            init.SetFacing(o);
            init.Launch();
            who->m_Events.AddEvent(new CastFoodSpell(who, _chairSpells.at(who->GetEntry())), who->m_Events.CalculateTime(1000));
            if (who->GetTypeId() == TYPEID_UNIT)
                who->SetDisplayId(who->ToCreature()->GetCreatureTemplate()->Modelid1);
        }
    };

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new npc_bountiful_tableAI(creature);
    }
};

/*######
## npc_torch_tossing_target_bunny_controller
######*/
class npc_wintergrasp_trab : public CreatureScript
{
public:
    npc_wintergrasp_trab() : CreatureScript("npc_wintergrasp_trab")	{}

    struct npc_wintergrasp_trabAI : public ScriptedAI
    {
        npc_wintergrasp_trabAI(Creature* creature) : ScriptedAI(creature)  
		{
			playerGUID = 0;
		}

		void MoveInLineOfSight(Unit* who) override
        {
			if (who->GetTypeId() == TYPEID_PLAYER)
                playerGUID = who->GetGUID();
		}

        void UpdateAI(uint32 diff) override
        {
			if (Player* target = ObjectAccessor::GetPlayer(*me, playerGUID))
				if (target->IsAlive() && !target->HasAura(27827))
					if (me->IsWithinDistInMap(target, 3.0f))
						if (!target->IsGameMaster())
							if (!target->IsFriendlyTo(me))
								target->Kill(target);
        }

	private:
        uint64 playerGUID;
    };

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new npc_wintergrasp_trabAI(creature);
    }
};

enum npcwintergrasptrab2
{
    EVENT_SHUT  = 1,
};

class npc_wintergrasp_trab2 : public CreatureScript
{
public:
    npc_wintergrasp_trab2() : CreatureScript("npc_wintergrasp_trab2") { }

    struct npc_wintergrasp_trab2AI : public ScriptedAI
    {
        npc_wintergrasp_trab2AI(Creature* creature) : ScriptedAI(creature)  
		{
			playerGUID = 0;
		}

		void MoveInLineOfSight(Unit* who) override
        {
			if (who->GetTypeId() == TYPEID_PLAYER)
			{
                playerGUID = who->GetGUID();
				events.ScheduleEvent(EVENT_SHUT, 2000);
			}
		}

        void UpdateAI(uint32 diff) override
        {
			if (Unit* target = me->GetVictim())
				if (me->GetDistance(target) >= 76)
				{
					me->DeleteThreatList();
					me->CombatStop(true);
				}

			events.Update(diff);

            if (me->HasUnitState(UNIT_STATE_CASTING))
                return;

            while (uint32 eventId = events.ExecuteEvent())
            {
                switch (eventId)
                {
					case EVENT_SHUT:
						if (Player* target = ObjectAccessor::GetPlayer(*me, playerGUID))
						me->CastSpell(target, 51421);
						events.ScheduleEvent(EVENT_SHUT, 2000);
					break;
				}
			}
        }

	private:
		EventMap events;
		uint64 playerGUID;
    };

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new npc_wintergrasp_trab2AI(creature);
    }
};

class npc_wintergrasp_trab3 : public CreatureScript
{
public:
    npc_wintergrasp_trab3() : CreatureScript("npc_wintergrasp_trab3") { }

    struct npc_wintergrasp_trab3AI : public ScriptedAI
    {
        npc_wintergrasp_trab3AI(Creature* creature) : ScriptedAI(creature)  
		{
			playerGUID = 0;
		}

		void MoveInLineOfSight(Unit* who) override
        {
	         if (who->GetTypeId() == TYPEID_PLAYER)
                playerGUID = who->GetGUID();
		}

        void UpdateAI(uint32 diff) override
        {
			if (Player* target = ObjectAccessor::GetPlayer(*me, playerGUID))
				if (target->IsAlive() && !target->HasAura(27827))
					if (me->IsWithinDistInMap(target, 1.0f))
						if (!target->IsGameMaster())
							if (!target->IsFriendlyTo(me))
								target->Kill(target);
        }

	private:
        uint64 playerGUID;
    };

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new npc_wintergrasp_trab3AI(creature);
    }
};

class npc_zevsbot : public CreatureScript
{
public:
    npc_zevsbot() : CreatureScript("npc_zevsbot") { }

    struct npc_zevsbotAI : public ScriptedAI
    {
        npc_zevsbotAI(Creature* creature) : ScriptedAI(creature)  
		{
			RemoveFromWorldtime = 540000;
		}

		void SpellHit(Unit* caster, SpellInfo const* spell) override
        {
            if (spell->Id == 68052)
				Talk(EMOTE_CLUCK_TEXT);
		}

        void UpdateAI(uint32 diff) override
        {
			Map::PlayerList const& lPlayers = me->GetMap()->GetPlayers();
			if (!lPlayers.isEmpty())
				for (Map::PlayerList::const_iterator i = lPlayers.begin(); i != lPlayers.end(); ++i)
					if (Player* player = i->GetSource())
						if(player->GetDistance(me) >= 60)
							me->RemoveFromWorld();

			if (RemoveFromWorldtime < diff)
            {
				Talk(0);
                me->RemoveFromWorld();
            }
            else
                RemoveFromWorldtime -= diff;
        }

	private:
        uint32 RemoveFromWorldtime;
    };

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new npc_zevsbotAI(creature);
    }
};

void AddSC_npcs_special()
{
    new npc_air_force_bots();
    new npc_lunaclaw_spirit();
    new npc_chicken_cluck();
    new npc_dancing_flames();
    new npc_doctor();
    new npc_injured_patient();
    new npc_garments_of_quests();
    new npc_guardian();
    new npc_mount_vendor();
    new npc_sayge();
    new npc_steam_tonk();
    new npc_tonk_mine();
    new npc_brewfest_reveler();
    new npc_training_dummy();
    new npc_wormhole();
    new npc_pet_trainer();
    new npc_locksmith();
    new npc_experience();
    new npc_firework();
    new npc_spring_rabbit();
    new npc_imp_in_a_ball();
	new npc_risen_ally();
	new npc_invis_arakkoa_target();
	new npc_banner_q11429();
	new boss_darksaw();
	new ballad_of_oldie();
	new npc_train_wrecker();
	new npc_torch_tossing_target_bunny_controller();
	new npc_stable_master();

	new npc_argent_squire();
	new npc_roach();

	//6월 25일
	new npc_edana_hatetalon();
	new npc_voidwalker_minion();
	new npc_q4041();
	new npc_relentless();

	//6월 26일
	new npc_om_point();

	//8월 6일
	new npc_fun_and_games();

	//가을축제 검은무쇠단 드워프 타도
	new npc_dark_iron_guzzler();

	new npc_midsummer_bunny_pole();

	new npc_bountiful_table();
	new npc_wintergrasp_trab();
	//new npc_wintergrasp_trab2();
	new npc_wintergrasp_trab3(); 

	new npc_zevsbot();
}
