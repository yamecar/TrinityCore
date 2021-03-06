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
SDName: Eastern_Plaguelands
SD%Complete: 100
SDComment: Quest support: 5211, 5742. Special vendor Augustus the Touched
SDCategory: Eastern Plaguelands
EndScriptData */

/* ContentData
npc_ghoul_flayer
npc_augustus_the_touched
npc_darrowshire_spirit
npc_tirion_fordring
EndContentData */

#include "ScriptMgr.h"
#include "ScriptedCreature.h"
#include "ScriptedGossip.h"
#include "Player.h"
#include "WorldSession.h"

class npc_ghoul_flayer : public CreatureScript
{
public:
    npc_ghoul_flayer() : CreatureScript("npc_ghoul_flayer") { }

    struct npc_ghoul_flayerAI : public ScriptedAI
    {
        npc_ghoul_flayerAI(Creature* creature) : ScriptedAI(creature) { }

        void Reset() override { }

        void EnterCombat(Unit* /*who*/) override { }

        void JustDied(Unit* killer) override
        {
            if (killer->GetTypeId() == TYPEID_PLAYER)
                me->SummonCreature(11064, 0.0f, 0.0f, 0.0f, 0.0f, TEMPSUMMON_TIMED_DESPAWN, 60000);
        }
    };

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new npc_ghoul_flayerAI(creature);
    }
};

/*######
## npc_augustus_the_touched
######*/

class npc_augustus_the_touched : public CreatureScript
{
public:
    npc_augustus_the_touched() : CreatureScript("npc_augustus_the_touched") { }

    bool OnGossipSelect(Player* player, Creature* creature, uint32 /*sender*/, uint32 action) override
    {
        player->PlayerTalkClass->ClearMenus();
        if (action == GOSSIP_ACTION_TRADE)
            player->GetSession()->SendListInventory(creature->GetGUID());
        return true;
    }

    bool OnGossipHello(Player* player, Creature* creature) override
    {
        if (creature->IsQuestGiver())
            player->PrepareQuestMenu(creature->GetGUID());

        if (creature->IsVendor() && player->GetQuestRewardStatus(6164))
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_VENDOR, GOSSIP_TEXT_BROWSE_GOODS, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_TRADE);

        player->SEND_GOSSIP_MENU(player->GetGossipTextId(creature), creature->GetGUID());
        return true;
    }
};

/*######
## npc_darrowshire_spirit
######*/

enum DarrowshireSpirit
{
    SPELL_SPIRIT_SPAWNIN    = 17321
};

class npc_darrowshire_spirit : public CreatureScript
{
public:
    npc_darrowshire_spirit() : CreatureScript("npc_darrowshire_spirit") { }

    bool OnGossipHello(Player* player, Creature* creature) override
    {
        player->SEND_GOSSIP_MENU(3873, creature->GetGUID());
        player->TalkedToCreature(creature->GetEntry(), creature->GetGUID());
        creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
        return true;
    }

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new npc_darrowshire_spiritAI(creature);
    }

    struct npc_darrowshire_spiritAI : public ScriptedAI
    {
        npc_darrowshire_spiritAI(Creature* creature) : ScriptedAI(creature) { }

        void Reset() override
        {
            DoCast(me, SPELL_SPIRIT_SPAWNIN);
            me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
        }

        void EnterCombat(Unit* /*who*/) override { }
    };
};

/*######
## npc_tirion_fordring
######*/

#define GOSSIP_HELLO    "I am ready to hear your tale, Tirion."
#define GOSSIP_SELECT1  "Thank you, Tirion.  What of your identity?"
#define GOSSIP_SELECT2  "That is terrible."
#define GOSSIP_SELECT3  "I will, Tirion."

class npc_tirion_fordring : public CreatureScript
{
public:
    npc_tirion_fordring() : CreatureScript("npc_tirion_fordring") { }

    bool OnGossipSelect(Player* player, Creature* creature, uint32 /*sender*/, uint32 action) override
    {
        player->PlayerTalkClass->ClearMenus();
        switch (action)
        {
            case GOSSIP_ACTION_INFO_DEF+1:
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_SELECT1, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 2);
                player->SEND_GOSSIP_MENU(4493, creature->GetGUID());
                break;
            case GOSSIP_ACTION_INFO_DEF+2:
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_SELECT2, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 3);
                player->SEND_GOSSIP_MENU(4494, creature->GetGUID());
                break;
            case GOSSIP_ACTION_INFO_DEF+3:
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_SELECT3, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 4);
                player->SEND_GOSSIP_MENU(4495, creature->GetGUID());
                break;
            case GOSSIP_ACTION_INFO_DEF+4:
                player->CLOSE_GOSSIP_MENU();
                player->AreaExploredOrEventHappens(5742);
                break;
        }
        return true;
    }

    bool OnGossipHello(Player* player, Creature* creature) override
    {
        if (creature->IsQuestGiver())
            player->PrepareQuestMenu(creature->GetGUID());

        if (player->GetQuestStatus(5742) == QUEST_STATUS_INCOMPLETE && player->getStandState() == UNIT_STAND_STATE_SIT)
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_HELLO, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+1);

        player->SEND_GOSSIP_MENU(player->GetGossipTextId(creature), creature->GetGUID());

        return true;
    }
};

/*######
## npc_eris_heavenfire
######*/

enum BalanceOfLightAndShadow
{
    SPELL_BLESSING_OF_NORDRASSIL      = 23108,
    SPELL_DEATH_DOOR                  = 23127,

    NPC_INJURED_PEASANT               = 14484,
    // NPC_PLAGUED_PEASANT               = 14485,
    NPC_SCOURGE_ARCHER                = 14489,
    NPC_SCOURGE_FOOTSOLDIER           = 14486,

    QUEST_BALANCE_OF_LIGHT_AND_SHADOW = 7622
};

float bolas_coords[13][4] =
{
    // Peasant Spawn Rectangle UpperLeft
    {3350.240234, -3049.189941, 164.775314, 2.03},
    // Peasant Spawn Rectangle LowerRight
    {3368.810059, -3053.790039, 166.264008, 0.0},
    // Peasant Destination Area Center
    {3330.746826, -2974.629150, 160.388611, 0.0},
    // Footsoldiers
    {3347.603271, -3045.536377, 164.029877, 1.814429},
    {3363.609131, -3037.187256, 163.541885, 2.277649},
    {3349.105469, -3056.500977, 168.079468, 1.857460},
    // Archer
    {3347.865234, -3070.707275, 177.881882, 1.645396},
    {3357.144287, -3063.327637, 172.499222, 1.841747},
    {3371.682373, -3067.965332, 175.233582, 2.144123},
    {3379.904053, -3059.370117, 181.981873, 2.646778},
    {3334.646973, -3053.084717, 174.101074, 0.400536},
    {3368.005371, -3022.475830, 171.617966, 4.268625},
    {3327.000244, -3021.307861, 170.578796, 5.584163}
};

#define SAY_SPAWN1   "The Scourge is uppon us! Run! Run for your lives!"
#define SAY_SPAWN2   "Please help us! The Prince has gone mad!"
#define SAY_SPAWN3   "Seek sanctuary in Hearthglen! It is our only hope!"
// #define SAY_SPAWN4 ""
// #define SAY_SPAWN5 ""
#define SAY_SAVED1   "Thank you, kind stranger. May your heroism never be forgotten."
#define SAY_SAVED2   "The power of the light is truly great and merciful."
#define SAY_SAVED3   "Stranger, find the fallen Prince Menethil and end his reign of terror."
#define SAY_DEATH1   "Death take me! I cannot go on! I have nothing left..."
#define SAY_DEATH2   "Should I live through this, I shall make it my live's sole ambition to destroy Arthas..."
#define SAY_DEATH3   "The pain is unbearable!"
#define SAY_DEATH4   "I won't make it... go... go on without me..."
#define SAY_EMPOWER  "Be healed!"
#define SAY_COMPLETE "We are saved! The peasants have escaped the Scourge!"
// #define SAY_FAIL ""

class npc_fleeing_peasant : public CreatureScript
{
public:
    npc_fleeing_peasant() : CreatureScript("npc_fleeing_peasant") { }

    struct npc_fleeing_peasantAI : public ScriptedAI
    {
    public:
        npc_fleeing_peasantAI(Creature *c) : ScriptedAI(c) {}

        uint16 sayTimer;
        uint8  sayStep;

        void Reset()
        {
            sayTimer = 0;
            me->SetUnitMovementFlags(MOVEMENTFLAG_WALKING);
            me->SetSpeed(MOVE_WALK, 0.45f);
            me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PVP_ATTACKABLE);
            me->SetReactState(REACT_PASSIVE);
            me->GetMotionMaster()->MovePoint(0, bolas_coords[2][0] + irand(-3, 3),  bolas_coords[2][1] + irand(-3, 3), bolas_coords[2][2]);
        }

        void StartOutro()
        {
            sayStep = 1;
        }

        void UpdateAI(const uint32 diff)
        {
			if (Creature* ch = me->FindNearestCreature(234721, 5.0f))
				if (Creature* e = me->FindNearestCreature(14494, 200.0f))
				{
					e->AI()->DoAction(0);
					me->DespawnOrUnsummon();
				}

            if (!sayStep)
                return;

            if (sayTimer > diff)
            {
                sayTimer -= diff;
                return;
            }
            else
                sayTimer = 3000;

            switch (sayStep)
            {
                case 1: me->MonsterSay(SAY_SAVED1, LANG_UNIVERSAL, 0); sayStep++; break;
                case 2: me->MonsterSay(SAY_SAVED2, LANG_UNIVERSAL, 0); sayStep++; break;
                case 3: me->MonsterSay(SAY_SAVED3, LANG_UNIVERSAL, 0); sayStep++; break;
                default:                                                          break;
            }
        }

        void DamageTaken(Unit* /*pKiller*/, uint32 &damage)
        {
            if (damage < me->GetHealth())
                return;

            if (Creature* e = me->FindNearestCreature(14494, 200.0f))
                e->AI()->DoAction(1);

            switch (urand(0, 6))
            {
                case 0: me->MonsterSay(SAY_DEATH1, LANG_UNIVERSAL, 0); break;
                case 1: me->MonsterSay(SAY_DEATH2, LANG_UNIVERSAL, 0); break;
                case 2: me->MonsterSay(SAY_DEATH3, LANG_UNIVERSAL, 0); break;
                case 3: me->MonsterSay(SAY_DEATH4, LANG_UNIVERSAL, 0); break;
                default:                                               break;
            }
        }
    };

    CreatureAI* GetAI(Creature* pCreature) const
    {
        return new npc_fleeing_peasantAI(pCreature);
    }
};

class npc_eris_havenfire : public CreatureScript
{
public:
    npc_eris_havenfire() : CreatureScript("npc_eris_havenfire") { }

    struct npc_eris_havenfireAI : public ScriptedAI
    {
    public:
        npc_eris_havenfireAI(Creature *c) : ScriptedAI(c) {}

        bool    eventActive;
        uint32  waveTimer;
        uint8   waveCount;
        uint8   saveCount;
        uint8   killCount;
        uint32  soldierTimer1;
		uint32  soldierTimer2;
		uint32  soldierTimer3;
        Player* pPriest;

        void Reset()
        {
            pPriest = NULL;
            eventActive = false;
            soldierTimer1 = 0;
			soldierTimer2 = 0;
			soldierTimer3 = 0;
            waveTimer = 0;
            waveCount = 0;
            saveCount = 0;
            killCount = 0;
        }

        void DoAction(const int32 action)
        {
            if (!eventActive || !pPriest || !pPriest->IsInWorld())
            {
                pPriest = NULL;
                eventActive = false;
                return;
            }

            switch (action)
            {
                case 0: saveCount++; break;
                case 1: killCount++; break;
                default:             break;
            }

            if (killCount >= 15)
            {
                // FailQuest
                //me->MonsterYell(SAY_FAIL, LANG_UNIVERSAL, 0);
                pPriest->FailQuest(QUEST_BALANCE_OF_LIGHT_AND_SHADOW);
                eventActive = false;
                pPriest = NULL;
            }
            else if (saveCount >= 50)
            {
                // award Quest
                me->MonsterYell(SAY_COMPLETE, LANG_UNIVERSAL, 0);
                pPriest->CompleteQuest(QUEST_BALANCE_OF_LIGHT_AND_SHADOW);
                eventActive = false;
                pPriest = NULL;
            }
        }

        void StartEvent(Player* pPlayer)
        {
            if (eventActive)
                return;

            // init vars
            eventActive = true;
            soldierTimer1 =  72000;
			soldierTimer2 =  72000;
			soldierTimer3 =  72000;
            waveTimer = 10000;
            waveCount = 0;
            saveCount = 0;
            killCount = 0;
            pPriest = pPlayer;

            // spawn archer
            for (uint8 i = 6; i < 13; i++)
            {
                Position pos;
                pos.m_positionX = bolas_coords[i][0];
                pos.m_positionY = bolas_coords[i][1];
                pos.m_positionZ = bolas_coords[i][2];
                pos.m_orientation = bolas_coords[i][3];

                if (Creature* p = me->SummonCreature(NPC_SCOURGE_ARCHER, pos, TEMPSUMMON_TIMED_DESPAWN, 5*MINUTE*IN_MILLISECONDS))
                {
                    p->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_DISABLE_MOVE);
                    p->SetReactState(REACT_AGGRESSIVE);
                }
            }
        }

        void MoveInLineOfSight(Unit* who)
        {
            /// out of range or already registered
            if (!me->IsWithinDist(who, 10.0f) || !who->ToCreature() || who->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE))
                return;

            // irrelevant creature
            Creature* p = who->ToCreature();

            if (p->GetEntry() != NPC_INJURED_PEASANT)
                return;

            p->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
            p->DespawnOrUnsummon();

            // priest lost or event not active
            if (!eventActive || !pPriest || !pPriest->IsInWorld())
            {
                pPriest = NULL;
                eventActive = false;
                return;
            }

            // register survivor
            DoAction(0);

            if (saveCount == 50)
                CAST_AI(npc_fleeing_peasant::npc_fleeing_peasantAI, p->AI())->StartOutro();
        }

        void UpdateAI(const uint32 diff)
        {
            if (!eventActive || !pPriest || !pPriest->IsInWorld())
            {
                pPriest = NULL;
                eventActive = false;
                return;
            }

/* strange targeting
            if (!pPriest->HasAuraEffect(SPELL_BLESSING_OF_NORDRASSIL, EFFECT_0) && (100.0f * pPriest->GetPower(POWER_MANA) / pPriest->GetMaxPower(POWER_MANA)) < 35.0f)
            {
                me->CastSpell(pPriest, SPELL_BLESSING_OF_NORDRASSIL, true);
                me->MonsterYell(SAY_EMPOWER, LANG_UNIVERSAL, 0);
            }
*/

            if (soldierTimer1 <= diff)
            {
                Position pos;
                pos.m_positionX = bolas_coords[3][0];
                pos.m_positionY = bolas_coords[3][1];
                pos.m_positionZ = bolas_coords[3][2];
                pos.m_orientation = bolas_coords[3][3];

                for (uint8 i = 0; (waveCount - 1) >= i; i++)
                    if (Creature* s = me->SummonCreature(NPC_SCOURGE_FOOTSOLDIER, pos, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 5000))
                        s->getThreatManager().addThreat(pPriest, 1.0f);

                soldierTimer1 = 20000 + urand(0, 20000);
            }
            else
                soldierTimer1 -= diff;

            if (soldierTimer2 <= diff)
            {
                Position pos;
                pos.m_positionX = bolas_coords[4][0];
                pos.m_positionY = bolas_coords[4][1];
                pos.m_positionZ = bolas_coords[4][2];
                pos.m_orientation = bolas_coords[4][3];

                for (uint8 i = 0; (waveCount - 1) >= i; i++)
                    if (Creature* s = me->SummonCreature(NPC_SCOURGE_FOOTSOLDIER, pos, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 5000))
                        s->getThreatManager().addThreat(pPriest, 1.0f);

                soldierTimer2 = 20000 + urand(0, 20000);

            }
            else
                soldierTimer2 -= diff;

            if (soldierTimer3 <= diff)
            {
                Position pos;
                pos.m_positionX = bolas_coords[5][0];
                pos.m_positionY = bolas_coords[5][1];
                pos.m_positionZ = bolas_coords[5][2];
                pos.m_orientation = bolas_coords[5][3];

                for (uint8 i = 0; (waveCount - 1) >= i; i++)
                    if (Creature* s = me->SummonCreature(NPC_SCOURGE_FOOTSOLDIER, pos, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 5000))
                        s->getThreatManager().addThreat(pPriest, 1.0f);

                soldierTimer3 = 20000 + urand(0, 20000);
            }
            else
                soldierTimer3 -= diff;

            if (waveTimer <= diff)
            {
                // 6*12 => 72 Peasants sent, 14 may die => 58 are underway
                if (waveCount >= 6)
                    return;

                pPriest->getHostileRefManager().deleteReferences();

                for (uint8 i = 0; i <= 12; i++)
                {
                    Position pos;
                    pos.m_positionX = bolas_coords[0][0] + (bolas_coords[1][0] - bolas_coords[0][0]) * urand(0, 100) / 100.0f;
                    pos.m_positionY = bolas_coords[0][1] + (bolas_coords[1][1] - bolas_coords[0][1]) * urand(0, 100) / 100.0f;
                    pos.m_positionZ = bolas_coords[0][2];
                    pos.m_orientation = bolas_coords[0][3];

                    if (Creature* p = me->SummonCreature(NPC_INJURED_PEASANT, pos, TEMPSUMMON_MANUAL_DESPAWN, 0))
                    {
                        p->setFaction(pPriest->getFaction());

                        if (i == 0)
                        {
                            switch (waveCount)
                            {
                                case 0: p->MonsterYell(SAY_SPAWN1, LANG_UNIVERSAL, 0); break;
                                case 1: p->MonsterYell(SAY_SPAWN2, LANG_UNIVERSAL, 0); break;
                               //case 2:                                                break;
                                case 3: p->MonsterYell(SAY_SPAWN3, LANG_UNIVERSAL, 0); break;
                                //case 4:                                                break;
                                //case 5:                                                break;
                                default:                                               break;
                            }
                        }

                        if (!urand(0, 3))
                            p->CastSpell(p, SPELL_DEATH_DOOR, true);
                    }
                }
                waveCount++;
                waveTimer = 60000;
            }
            else
                waveTimer -= diff;
        }
    };

    bool OnQuestAccept(Player* pPlayer, Creature* pCreature, Quest const *quest)
    {
        if (quest->GetQuestId() == QUEST_BALANCE_OF_LIGHT_AND_SHADOW)
            CAST_AI(npc_eris_havenfire::npc_eris_havenfireAI, pCreature->AI())->StartEvent(pPlayer);
    }

    CreatureAI* GetAI(Creature* pCreature) const
    {
        return new npc_eris_havenfireAI(pCreature);
    }
};

void AddSC_eastern_plaguelands()
{
    new npc_ghoul_flayer();
    new npc_augustus_the_touched();
    new npc_darrowshire_spirit();
    new npc_tirion_fordring();
	new npc_eris_havenfire();
    new npc_fleeing_peasant();
}
