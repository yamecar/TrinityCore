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
SDName: Stormwind_City
SD%Complete: 100
SDComment: Quest support: 1640, 1447, 4185, 11223, 434.
SDCategory: Stormwind City
EndScriptData */

/* ContentData
npc_archmage_malin
npc_bartleby
npc_lady_katrana_prestor
npc_tyrion
npc_tyrion_spybot
npc_marzon_silent_blade
npc_lord_gregor_lescovar
EndContentData */

#include "ScriptMgr.h"
#include "ScriptedCreature.h"
#include "ScriptedGossip.h"
#include "ScriptedEscortAI.h"
#include "Player.h"
#include "Language.h"
#include "ObjectMgr.h"

/*######
## npc_archmage_malin
######*/

#define GOSSIP_ITEM_MALIN "Can you send me to Theramore? I have an urgent message for Lady Jaina from Highlord Bolvar."

class npc_archmage_malin : public CreatureScript
{
public:
    npc_archmage_malin() : CreatureScript("npc_archmage_malin") { }

    bool OnGossipSelect(Player* player, Creature* creature, uint32 /*sender*/, uint32 action) override
    {
        player->PlayerTalkClass->ClearMenus();
        if (action == GOSSIP_ACTION_INFO_DEF)
        {
            player->CLOSE_GOSSIP_MENU();
            creature->CastSpell(player, 42711, true);
        }

        return true;
    }

    bool OnGossipHello(Player* player, Creature* creature) override
    {
        if (creature->IsQuestGiver())
            player->PrepareQuestMenu(creature->GetGUID());

        if (player->GetQuestStatus(11223) == QUEST_STATUS_COMPLETE)
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_MALIN, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF);

        player->SEND_GOSSIP_MENU(player->GetGossipTextId(creature), creature->GetGUID());

        return true;
    }
};

/*######
## npc_bartleby
######*/

enum Bartleby
{
    FACTION_ENEMY       = 168,
    QUEST_BEAT          = 1640
};

class npc_bartleby : public CreatureScript
{
public:
    npc_bartleby() : CreatureScript("npc_bartleby") { }

    bool OnQuestAccept(Player* player, Creature* creature, Quest const* quest) override
    {
        if (quest->GetQuestId() == QUEST_BEAT)
        {
            creature->setFaction(FACTION_ENEMY);
            creature->AI()->AttackStart(player);
        }
        return true;
    }

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new npc_bartlebyAI(creature);
    }

    struct npc_bartlebyAI : public ScriptedAI
    {
        npc_bartlebyAI(Creature* creature) : ScriptedAI(creature)
        {
            m_uiNormalFaction = creature->getFaction();
        }

        uint32 m_uiNormalFaction;

        void Reset() override
        {
            if (me->getFaction() != m_uiNormalFaction)
                me->setFaction(m_uiNormalFaction);
        }

        void AttackedBy(Unit* pAttacker) override
        {
            if (me->GetVictim())
                return;

            if (me->IsFriendlyTo(pAttacker))
                return;

            AttackStart(pAttacker);
        }

        void DamageTaken(Unit* pDoneBy, uint32 &uiDamage) override
        {
            if (uiDamage > me->GetHealth() || me->HealthBelowPctDamaged(15, uiDamage))
            {
                //Take 0 damage
                uiDamage = 0;

                if (Player* player = pDoneBy->ToPlayer())
                    player->AreaExploredOrEventHappens(QUEST_BEAT);
                EnterEvadeMode();
            }
        }
    };
};

/*######
## npc_lady_katrana_prestor
######*/

#define GOSSIP_ITEM_KAT_1 "Pardon the intrusion, Lady Prestor, but Highlord Bolvar suggested that I seek your advice."
#define GOSSIP_ITEM_KAT_2 "My apologies, Lady Prestor."
#define GOSSIP_ITEM_KAT_3 "Begging your pardon, Lady Prestor. That was not my intent."
#define GOSSIP_ITEM_KAT_4 "Thank you for your time, Lady Prestor."

class npc_lady_katrana_prestor : public CreatureScript
{
public:
    npc_lady_katrana_prestor() : CreatureScript("npc_lady_katrana_prestor") { }

    bool OnGossipSelect(Player* player, Creature* creature, uint32 /*sender*/, uint32 action) override
    {
        player->PlayerTalkClass->ClearMenus();
        switch (action)
        {
            case GOSSIP_ACTION_INFO_DEF:
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_KAT_2, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
                player->SEND_GOSSIP_MENU(2694, creature->GetGUID());
                break;
            case GOSSIP_ACTION_INFO_DEF+1:
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_KAT_3, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 2);
                player->SEND_GOSSIP_MENU(2695, creature->GetGUID());
                break;
            case GOSSIP_ACTION_INFO_DEF+2:
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_KAT_4, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 3);
                player->SEND_GOSSIP_MENU(2696, creature->GetGUID());
                break;
            case GOSSIP_ACTION_INFO_DEF+3:
                player->CLOSE_GOSSIP_MENU();
                player->AreaExploredOrEventHappens(4185);
                break;
        }
        return true;
    }

    bool OnGossipHello(Player* player, Creature* creature) override
    {
        if (creature->IsQuestGiver())
            player->PrepareQuestMenu(creature->GetGUID());

        if (player->GetQuestStatus(4185) == QUEST_STATUS_INCOMPLETE)
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_KAT_1, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF);

        player->SEND_GOSSIP_MENU(2693, creature->GetGUID());

        return true;
    }
};

/*######
## npc_lord_gregor_lescovar
######*/

enum LordGregorLescovar
{
    SAY_GUARD_2    = 0,
    SAY_LESCOVAR_2 = 0,
    SAY_LESCOVAR_3 = 1,
    SAY_LESCOVAR_4 = 2,
    SAY_MARZON_1   = 0,
    SAY_MARZON_2   = 1,
    SAY_TYRION_2   = 1,

    NPC_STORMWIND_ROYAL = 1756,
    NPC_MARZON_BLADE    = 1755,
    NPC_TYRION          = 7766,

    QUEST_THE_ATTACK    = 434
};

class npc_lord_gregor_lescovar : public CreatureScript
{
public:
    npc_lord_gregor_lescovar() : CreatureScript("npc_lord_gregor_lescovar") { }

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new npc_lord_gregor_lescovarAI(creature);
    }

    struct npc_lord_gregor_lescovarAI : public npc_escortAI
    {
        npc_lord_gregor_lescovarAI(Creature* creature) : npc_escortAI(creature) { }

        uint32 uiTimer;
        uint32 uiPhase;

        uint64 MarzonGUID;

        void Reset() override
        {
            uiTimer = 0;
            uiPhase = 0;

            MarzonGUID = 0;
        }

        void EnterEvadeMode() override
        {
            me->DisappearAndDie();

            if (Creature* pMarzon = ObjectAccessor::GetCreature(*me, MarzonGUID))
            {
                if (pMarzon->IsAlive())
                    pMarzon->DisappearAndDie();
            }
        }

        void EnterCombat(Unit* who) override
        {
            if (Creature* pMarzon = ObjectAccessor::GetCreature(*me, MarzonGUID))
            {
                if (pMarzon->IsAlive() && !pMarzon->IsInCombat())
                    pMarzon->AI()->AttackStart(who);
            }
        }

        void WaypointReached(uint32 waypointId) override
        {
            switch (waypointId)
            {
                case 14:
                    SetEscortPaused(true);
                    Talk(SAY_LESCOVAR_2);
                    uiTimer = 3000;
                    uiPhase = 1;
                    break;
                case 16:
                    SetEscortPaused(true);
                    if (Creature* pMarzon = me->SummonCreature(NPC_MARZON_BLADE, -8411.360352f, 480.069733f, 123.760895f, 4.941504f, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 1000))
                    {
                        pMarzon->GetMotionMaster()->MovePoint(0, -8408.000977f, 468.611450f, 123.759903f);
                        MarzonGUID = pMarzon->GetGUID();
                    }
                    uiTimer = 2000;
                    uiPhase = 4;
                    break;
            }
        }
        //TO-DO: We don't have movemaps, also we can't make 2 npcs walks to one point propperly (and we can not use escort ai, because they are 2 different spawns and with same entry), because of it we make them, disappear.
        void DoGuardsDisappearAndDie()
        {
            std::list<Creature*> GuardList;
            me->GetCreatureListWithEntryInGrid(GuardList, NPC_STORMWIND_ROYAL, 8.0f);
            if (!GuardList.empty())
            {
                for (std::list<Creature*>::const_iterator itr = GuardList.begin(); itr != GuardList.end(); ++itr)
                {
                    if (Creature* pGuard = *itr)
                        pGuard->DisappearAndDie();
                }
            }
        }

        void UpdateAI(uint32 uiDiff) override
        {
            if (uiPhase)
            {
                if (uiTimer <= uiDiff)
                {
                    switch (uiPhase)
                    {
                        case 1:
                            if (Creature* pGuard = me->FindNearestCreature(NPC_STORMWIND_ROYAL, 8.0f, true))
                                pGuard->AI()->Talk(SAY_GUARD_2);
                            uiTimer = 3000;
                            uiPhase = 2;
                            break;
                        case 2:
                            DoGuardsDisappearAndDie();
                            uiTimer = 2000;
                            uiPhase = 3;
                            break;
                        case 3:
                            SetEscortPaused(false);
                            uiTimer = 0;
                            uiPhase = 0;
                            break;
                        case 4:
                            Talk(SAY_LESCOVAR_3);
                            uiTimer = 0;
                            uiPhase = 0;
                            break;
                        case 5:
                            if (Creature* pMarzon = ObjectAccessor::GetCreature(*me, MarzonGUID))
                                pMarzon->AI()->Talk(SAY_MARZON_1);
                            uiTimer = 3000;
                            uiPhase = 6;
                            break;
                        case 6:
                            Talk(SAY_LESCOVAR_4);
                            if (Player* player = GetPlayerForEscort())
                                player->AreaExploredOrEventHappens(QUEST_THE_ATTACK);
                            uiTimer = 2000;
                            uiPhase = 7;
                            break;
                        case 7:
                            if (Creature* pTyrion = me->FindNearestCreature(NPC_TYRION, 20.0f, true))
                                pTyrion->AI()->Talk(SAY_TYRION_2);
                            if (Creature* pMarzon = ObjectAccessor::GetCreature(*me, MarzonGUID))
                                pMarzon->setFaction(14);
                            me->setFaction(14);
                            uiTimer = 0;
                            uiPhase = 0;
                            break;
                    }
                } else uiTimer -= uiDiff;
            }
            npc_escortAI::UpdateAI(uiDiff);

            if (!UpdateVictim())
                return;

            DoMeleeAttackIfReady();
        }
    };
};

/*######
## npc_marzon_silent_blade
######*/

class npc_marzon_silent_blade : public CreatureScript
{
public:
    npc_marzon_silent_blade() : CreatureScript("npc_marzon_silent_blade") { }

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new npc_marzon_silent_bladeAI(creature);
    }

    struct npc_marzon_silent_bladeAI : public ScriptedAI
    {
        npc_marzon_silent_bladeAI(Creature* creature) : ScriptedAI(creature)
        {
            me->SetWalk(true);
        }

        void Reset() override
        {
            me->RestoreFaction();
        }

        void EnterCombat(Unit* who) override
        {
            Talk(SAY_MARZON_2);

            if (me->IsSummon())
            {
                if (Unit* summoner = me->ToTempSummon()->GetSummoner())
                {
                    if (summoner->GetTypeId() == TYPEID_UNIT && summoner->IsAlive() && !summoner->IsInCombat())
                        summoner->ToCreature()->AI()->AttackStart(who);
                }
            }
        }

        void EnterEvadeMode() override
        {
            me->DisappearAndDie();

            if (me->IsSummon())
            {
                if (Unit* summoner = me->ToTempSummon()->GetSummoner())
                {
                    if (summoner->GetTypeId() == TYPEID_UNIT && summoner->IsAlive())
                        summoner->ToCreature()->DisappearAndDie();
                }
            }
        }

        void MovementInform(uint32 uiType, uint32 /*uiId*/) override
        {
            if (uiType != POINT_MOTION_TYPE)
                return;

            if (me->IsSummon())
            {
                Unit* summoner = me->ToTempSummon()->GetSummoner();
                if (summoner && summoner->GetTypeId() == TYPEID_UNIT && summoner->IsAIEnabled)
                {
                    npc_lord_gregor_lescovar::npc_lord_gregor_lescovarAI* ai =
                        CAST_AI(npc_lord_gregor_lescovar::npc_lord_gregor_lescovarAI, summoner->GetAI());
                    if (ai)
                    {
                        ai->uiTimer = 2000;
                        ai->uiPhase = 5;
                    }
                    //me->ChangeOrient(0.0f, summoner);
                }
            }
        }

        void UpdateAI(uint32 /*diff*/) override
        {
            if (!UpdateVictim())
                return;

            DoMeleeAttackIfReady();
        }
    };
};

/*######
## npc_tyrion_spybot
######*/

enum TyrionSpybot
{
    SAY_QUEST_ACCEPT_ATTACK  = 0,
    SAY_SPYBOT_1             = 1,
    SAY_SPYBOT_2             = 2,
    SAY_SPYBOT_3             = 3,
    SAY_SPYBOT_4             = 4,
    SAY_TYRION_1             = 0,
    SAY_GUARD_1              = 1,
    SAY_LESCOVAR_1           = 3,

    NPC_PRIESTESS_TYRIONA    = 7779,
    NPC_LORD_GREGOR_LESCOVAR = 1754,
};

class npc_tyrion_spybot : public CreatureScript
{
public:
    npc_tyrion_spybot() : CreatureScript("npc_tyrion_spybot") { }

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new npc_tyrion_spybotAI(creature);
    }

    struct npc_tyrion_spybotAI : public npc_escortAI
    {
        npc_tyrion_spybotAI(Creature* creature) : npc_escortAI(creature) { }

        uint32 uiTimer;
        uint32 uiPhase;

        void Reset() override
        {
            uiTimer = 0;
            uiPhase = 0;
        }

        void WaypointReached(uint32 waypointId) override
        {
            switch (waypointId)
            {
                case 1:
                    SetEscortPaused(true);
                    uiTimer = 2000;
                    uiPhase = 1;
                    break;
                case 5:
                    SetEscortPaused(true);
                    Talk(SAY_SPYBOT_1);
                    uiTimer = 2000;
                    uiPhase = 5;
                    break;
                case 17:
                    SetEscortPaused(true);
                    Talk(SAY_SPYBOT_3);
                    uiTimer = 3000;
                    uiPhase = 8;
                    break;
            }
        }

        void UpdateAI(uint32 uiDiff) override
        {
            if (uiPhase)
            {
                if (uiTimer <= uiDiff)
                {
                    switch (uiPhase)
                    {
                        case 1:
                            Talk(SAY_QUEST_ACCEPT_ATTACK);
                            uiTimer = 3000;
                            uiPhase = 2;
                            break;
                        case 2:
                            if (Creature* pTyrion = me->FindNearestCreature(NPC_TYRION, 10.0f))
                                pTyrion->AI()->Talk(SAY_TYRION_1);
                            uiTimer = 3000;
                            uiPhase = 3;
                            break;
                        case 3:
                            me->UpdateEntry(NPC_PRIESTESS_TYRIONA);
                            uiTimer = 2000;
                            uiPhase = 4;
                            break;
                        case 4:
                           SetEscortPaused(false);
                           uiPhase = 0;
                           uiTimer = 0;
                           break;
                        case 5:
                            if (Creature* pGuard = me->FindNearestCreature(NPC_STORMWIND_ROYAL, 10.0f, true))
                                pGuard->AI()->Talk(SAY_GUARD_1);
                            uiTimer = 3000;
                            uiPhase = 6;
                            break;
                        case 6:
                            Talk(SAY_SPYBOT_2);
                            uiTimer = 3000;
                            uiPhase = 7;
                            break;
                        case 7:
                            SetEscortPaused(false);
                            uiTimer = 0;
                            uiPhase = 0;
                            break;
                        case 8:
                            if (Creature* pLescovar = me->FindNearestCreature(NPC_LORD_GREGOR_LESCOVAR, 10.0f))
                                pLescovar->AI()->Talk(SAY_LESCOVAR_1);
                            uiTimer = 3000;
                            uiPhase = 9;
                            break;
                        case 9:
                            Talk(SAY_SPYBOT_4);
                            uiTimer = 3000;
                            uiPhase = 10;
                            break;
                        case 10:
                            if (Creature* pLescovar = me->FindNearestCreature(NPC_LORD_GREGOR_LESCOVAR, 10.0f))
                            {
                                if (Player* player = GetPlayerForEscort())
                                {
                                    CAST_AI(npc_lord_gregor_lescovar::npc_lord_gregor_lescovarAI, pLescovar->AI())->Start(false, false, player->GetGUID());
                                    CAST_AI(npc_lord_gregor_lescovar::npc_lord_gregor_lescovarAI, pLescovar->AI())->SetMaxPlayerDistance(200.0f);
                                }
                            }
                            me->DisappearAndDie();
                            uiTimer = 0;
                            uiPhase = 0;
                            break;
                    }
                } else uiTimer -= uiDiff;
            }
            npc_escortAI::UpdateAI(uiDiff);

            if (!UpdateVictim())
                return;

            DoMeleeAttackIfReady();
        }
    };
};

/*######
## npc_tyrion
######*/

enum Tyrion
{
    NPC_TYRION_SPYBOT = 8856
};

class npc_tyrion : public CreatureScript
{
public:
    npc_tyrion() : CreatureScript("npc_tyrion") { }

    bool OnQuestAccept(Player* player, Creature* creature, Quest const* quest) override
    {
        if (quest->GetQuestId() == QUEST_THE_ATTACK)
        {
            if (Creature* pSpybot = creature->FindNearestCreature(NPC_TYRION_SPYBOT, 5.0f, true))
            {
                CAST_AI(npc_tyrion_spybot::npc_tyrion_spybotAI, pSpybot->AI())->Start(false, false, player->GetGUID());
                CAST_AI(npc_tyrion_spybot::npc_tyrion_spybotAI, pSpybot->AI())->SetMaxPlayerDistance(200.0f);
            }
            return true;
        }
        return false;
    }
};

enum FactionLeadersNPC
{
    // Alliance
    BOSS_VARIAN                     = 29611, // King Varian Wrynn
    BOSS_MAGNI                      = 2784,  // King Magni Bronzebeard
    BOSS_MEKKATORQUE                = 7937,  // High Tinker Mekkatorque
    BOSS_TYRANDE                    = 7999,  // Tyrande Whisperwind
    BOSS_VELEN                      = 17468, // Prophet Velen

    NPC_STORMWIND_GUARD             = 68,    // 75 lvl
    NPC_STORMWIND_ELITE_GUARD       = 1756,  // 80 lvl elite
    NPC_IRONFORGE_GUARD             = 5595,  // 75 lvl
    NPC_DARNASSUS_GUARD             = 4262,  // 75 lvl
    NPC_EXODAR_GUARD                = 16733, // 75 lvl
    NPC_EXODAR_ELITE_GUARD          = 20674, // 80 lvl elite

    // Horde
    BOSS_THRALL                     = 4949,  // Thrall
    BOSS_VOLJIN                     = 10540, // Vol'Jin
    BOSS_SYLVANAS                   = 10181, // Lady Sylvanas Windrunner
    BOSS_CAIRNE                     = 3057,  // Cairne Bloodhoof
    BOSS_THERON                     = 16802, // Lor'themar Theron

    // Theron guard
    BOSS_ROMMATH                    = 16800, // Grand Magister Rommath

    NPC_ORGRIMMAR_GUARD             = 3296,  // 75 lvl
    NPC_ORGRIMMAR_ELITE_GUARD       = 14304, // 80 lvl elite
    NPC_UNDERCITY_GUARD             = 36213, // 75 lvl
    NPC_UNDERCITY_ELITE_GUARD       = 13839, // 80 lvl elite
    NPC_THUNDERBLUFF_GUARD          = 3084,  // 75 lvl
    NPC_THUNDERBLUFF_ELITE_GUARD    = 3083,  // 80 lvl elite
    NPC_SILVERMOON_GUARD            = 16222, // 75 lvl
    NPC_SILVERMOON_ELITE_GUARD      = 36864, // 80 lvl elite
    NPC_SILVERMOON_BOSS_BODYGUARD   = 16801, // Halduron Brightwing
};

enum Events
{
    // Shared event for all bosses
    EVENT_CALL_GUARDS               = 1,
    // Tyrande, Cairne, Theron
    EVENT_CLEAVE                    = 2,
    // Vol'Jin, Sylvanas
    EVENT_SHOOT                     = 3,

    // King Magni Bronzebeard
    EVENT_AVATAR                    = 4,
    EVENT_STORM_BOLT                = 5,
    EVENT_KNOCK_AWAY                = 6,

    // King Varian Wrynn
    EVENT_WHIRLWIND                 = 7,
    EVENT_HEROIC_LEAP               = 8,

    // High Tinker Mekkatorque
    EVENT_BOMB                      = 9,
    EVENT_GOBLIN_DRAGON_GUN         = 10,
    EVENT_SUPER_SHRING_RAY          = 11,

    // Tyrande Whisperwind
    EVENT_MOONFIRE                  = 12,
    EVENT_SEARING_ARROW             = 13,
    EVENT_STARFALL                  = 14,

    // Prophet Velen
    EVENT_HOLY_BLAST                = 15,
    EVENT_HOLY_NOVA                 = 16,
    EVENT_HOLY_SMITE                = 17,
    EVENT_PRAYER_OF_HEALING         = 18,
    EVENT_STAFF_STRIKE              = 19,

    // Thrall
    EVENT_CHAIN_LIGHTNING           = 20,
    EVENT_SHOCK                     = 21,

    // Vol'Jin
    EVENT_HEX                       = 22,
    EVENT_SHADOW_SHOCK              = 23,
    EVENT_SHADOW_WORD_PAIN          = 24,
    EVENT_VEIL_OF_SHADOW            = 25,

    // Lady Sylvanas Windrunner
    EVENT_BLACK_ARROW               = 26,
    EVENT_FADE                      = 27,
    EVENT_MULTISHOT                 = 28,
    EVENT_SUMMON_SKELETON           = 29,

    // Cairne Bloodhoof
    EVENT_MORTAL_STRIKE             = 30,
    EVENT_THUNDERCLAP               = 31,
    EVENT_UPPERCUT                  = 32,
    EVENT_WAR_STOMP                 = 33,

    // Lor'themar Theron
    EVENT_ARCANE_SHOCK              = 34,
    EVENT_MANA_BURN                 = 35,
    EVENT_MASS_CHARM                = 36,
};

// predicate function to select player between given distances
struct InRangePlayerSelector : public std::unary_function<Unit*, bool>
{
    InRangePlayerSelector(Creature* me, float minRange, float maxRange) : _me(me), _minRange(minRange), _maxRange(maxRange) {}

    bool operator()(Unit const* target) const
    {
        return (target->GetTypeId() == TYPEID_PLAYER && target->GetDistance2d(_me) >= _minRange && target->GetDistance2d(_me) <= _maxRange);
    }
 
    Creature* _me;
    float _minRange, _maxRange;
};

// predicate function to select mana user player in range
struct ManaUserPlayerSelector : public std::unary_function<Unit*, bool>
{
    ManaUserPlayerSelector(Creature* me, float range) : _me(me), _range(range) {}

    bool operator()(Unit const* target) const
    {
        return (target->GetTypeId() == TYPEID_PLAYER && target->ToPlayer()->getPowerType() == POWER_MANA && target->GetDistance2d(_me) <= _range);
    }
 
    Creature* _me;
    float _range;
};

/*######
## King_Varian_Wrynn
######*/

enum VarianSpells
{
    SPELL_WHIRLWIND                 = 41056,
    SPELL_HEROIC_LEAP               = 59688,
};

class boss_varian : public CreatureScript
{
    public:
        boss_varian() : CreatureScript("boss_varian") { }

        struct boss_varianAI : public WorldBossAI
        {
            boss_varianAI(Creature* creature) : WorldBossAI(creature) { }

            void EnterCombat(Unit* /*who*/)
            {
				sWorld->SendWorldText(LANG_ANNOUNCE_5);
                _EnterCombat();
                events.ScheduleEvent(EVENT_WHIRLWIND, 10000);
                events.ScheduleEvent(EVENT_HEROIC_LEAP, 45000); // does he start fight with jump?
                events.ScheduleEvent(EVENT_CALL_GUARDS, 1000);
            }

            void ExecuteEvent(uint32 const eventId)
            {
                switch (eventId)
                {
                    case EVENT_WHIRLWIND:
                        DoCastVictim(SPELL_WHIRLWIND);
                        events.ScheduleEvent(EVENT_WHIRLWIND, urand(9000, 18000));
                        break;
                    case EVENT_HEROIC_LEAP:
                        if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, InRangePlayerSelector(me, 8, 25)))
                            DoCast(target, SPELL_HEROIC_LEAP);
                        events.ScheduleEvent(EVENT_HEROIC_LEAP, urand(45000, 52000));
                        break;
                    case EVENT_CALL_GUARDS:
                        me->CallForHelp(VISIBLE_RANGE);
                        events.ScheduleEvent(EVENT_CALL_GUARDS, 5000);
                        break;
                    default:
                        break;
                }
            }

            void UpdateAI(uint32 const diff)
            {
                if (!UpdateVictim())
                    return;

                events.Update(diff);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                while (uint32 eventId = events.ExecuteEvent())
                    ExecuteEvent(eventId);

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return new boss_varianAI(creature);
        }
};

enum MagniTexts
{
    SAY_MAGNI_AGGRO               = 0, // "For Khaz'Modan!" or "Feel the fury of the mountain!"
};

enum MagniSpells
{
    SPELL_AVATAR                    = 19135,
    SPELL_STORM_BOLT                = 20685,
    SPELL_KNOCK_AWAY                = 20686,
};

class boss_magni : public CreatureScript
{
    public:
        boss_magni() : CreatureScript("boss_magni") { }

        struct boss_magniAI : public WorldBossAI
        {
            boss_magniAI(Creature* creature) : WorldBossAI(creature) { }

            void EnterCombat(Unit* /*who*/)
            {
				sWorld->SendWorldText(LANG_ANNOUNCE_6);
                _EnterCombat();
                Talk(SAY_MAGNI_AGGRO); // is there other way to random say?
                events.ScheduleEvent(EVENT_AVATAR, 30000);
                events.ScheduleEvent(EVENT_STORM_BOLT, 9000);
                events.ScheduleEvent(EVENT_KNOCK_AWAY, 25000);
                events.ScheduleEvent(EVENT_CALL_GUARDS, 1000);
            }

            void ExecuteEvent(uint32 const eventId)
            {
                switch (eventId)
                {
                    case EVENT_AVATAR:
                        DoCast(me, SPELL_AVATAR);
                        events.ScheduleEvent(EVENT_AVATAR, 30000);
                        break;
                    case EVENT_STORM_BOLT:
                        // cast SPELL_STORM_BOLT at random non-tank target if more than one target
                        if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 1, 40, true))
                            DoCast(target, SPELL_STORM_BOLT);
                        else
                            DoCastVictim(SPELL_STORM_BOLT);
                        events.ScheduleEvent(EVENT_STORM_BOLT, 9000);
                        break;
                    case EVENT_KNOCK_AWAY:
                        DoCastVictim(SPELL_KNOCK_AWAY);
                        DoResetThreat();
                        events.ScheduleEvent(EVENT_KNOCK_AWAY, urand(15000, 30000));
                        break;
                    case EVENT_CALL_GUARDS:
                        me->CallForHelp(VISIBLE_RANGE);
                        events.ScheduleEvent(EVENT_CALL_GUARDS, 5000);
                        break;
                    default:
                        break;
                }
            }

            void UpdateAI(uint32 const diff)
            {
                if (!UpdateVictim())
                    return;

                events.Update(diff);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                while (uint32 eventId = events.ExecuteEvent())
                    ExecuteEvent(eventId);

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return new boss_magniAI(creature);
        }
};

enum TyrandeTexts
{
    // all of them have sounds
    SAY_TYRANDE_AGGRO               = 0, // "Anu'dora!" or "For Kalimdor!" or "Endu'di Rifa!"
};

enum TyrandeSpells
{
    SPELL_CLEAVE                    = 20691,
    SPELL_MOONFIRE                  = 20690,
    SPELL_SEARING_ARROW             = 20688,
    SPELL_STARFALL                  = 20687,
};

class boss_tyrande : public CreatureScript
{
    public:
        boss_tyrande() : CreatureScript("boss_tyrande") { }

        struct boss_tyrandeAI : public WorldBossAI
        {
            boss_tyrandeAI(Creature* creature) : WorldBossAI(creature) { }

            void EnterCombat(Unit* /*who*/)
            {
				sWorld->SendWorldText(LANG_ANNOUNCE_7);
                _EnterCombat();
                Talk(SAY_TYRANDE_AGGRO);
                events.ScheduleEvent(EVENT_CLEAVE, 9000);
                events.ScheduleEvent(EVENT_MOONFIRE, 10000);
                events.ScheduleEvent(EVENT_SEARING_ARROW, 15000);
                events.ScheduleEvent(EVENT_STARFALL, 45000);
                events.ScheduleEvent(EVENT_CALL_GUARDS, 1000);
            }

            void ExecuteEvent(uint32 const eventId)
            {
                switch (eventId)
                {
                    case EVENT_CLEAVE:
                        DoCastVictim(SPELL_CLEAVE);
                        events.ScheduleEvent(EVENT_CLEAVE, urand(7000, 12000));
                        break;
                    case EVENT_MOONFIRE:
                        if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 50, true))
                            DoCast(target, SPELL_MOONFIRE);
                        events.ScheduleEvent(EVENT_MOONFIRE, urand(8000, 19000));
                        break;
                    case EVENT_SEARING_ARROW:
                        if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 50, true))
                            DoCast(target, SPELL_SEARING_ARROW);
                        events.ScheduleEvent(EVENT_SEARING_ARROW, urand(6000, 30000)); // max value rounded, prev was 42 from #4
                        break;
                    case EVENT_STARFALL:
                        DoCastAOE(SPELL_STARFALL);
                        events.ScheduleEvent(EVENT_STARFALL, urand(30000, 60000)); // rounded, prev values was 34-54 from #4
                        break;
                    case EVENT_CALL_GUARDS:
                        me->CallForHelp(VISIBLE_RANGE);
                        events.ScheduleEvent(EVENT_CALL_GUARDS, 5000);
                        break;
                    default:
                        break;
                }
            }

            void UpdateAI(uint32 const diff)
            {
                if (!UpdateVictim())
                    return;

                events.Update(diff);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                while (uint32 eventId = events.ExecuteEvent())
                    ExecuteEvent(eventId);

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return new boss_tyrandeAI(creature);
        }
};

enum VelenTexts
{
    SAY_VELEN_AGGRO                = 0, // "An unwise decision." or "I foresaw this." or "How unfortunate...for you."
};

enum VelenSpells
{
    SPELL_HOLY_BLAST                = 59700,
    SPELL_HOLY_NOVA                 = 59701,
    SPELL_HOLY_SMITE                = 59703,
    SPELL_PRAYER_OF_HEALING         = 59698, // problematic spell, he should heal guards
    SPELL_STAFF_STRIKE              = 33542,
};

class boss_velen : public CreatureScript
{
    public:
        boss_velen() : CreatureScript("boss_velen") { }

        struct boss_velenAI : public WorldBossAI
        {
            boss_velenAI(Creature* creature) : WorldBossAI(creature) { }

            void Reset()
            {
                _secondPhase = false;
                _Reset();
            }

            void EnterCombat(Unit* /*who*/)
            {
				sWorld->SendWorldText(LANG_ANNOUNCE_8);
                _EnterCombat();
                Talk(SAY_VELEN_AGGRO);
                events.ScheduleEvent(EVENT_HOLY_NOVA, 40000);
                events.ScheduleEvent(EVENT_HOLY_SMITE, 8000);
                events.ScheduleEvent(EVENT_PRAYER_OF_HEALING, 30000);
                events.ScheduleEvent(EVENT_CALL_GUARDS, 1000);
            }

            void DamageTaken(Unit* /*attacker*/, uint32& /*damage*/)
            {
                // If below 80% health he will change a tactic
                if (!_secondPhase && HealthBelowPct(80))
                {
                    _secondPhase = true;
                    events.ScheduleEvent(EVENT_HOLY_BLAST, 10000);
                    events.ScheduleEvent(EVENT_STAFF_STRIKE, 15000);
                    events.CancelEvent(EVENT_HOLY_NOVA); // stop repeating this one according info #5
                }
            }

            void ExecuteEvent(uint32 const eventId)
            {
                switch (eventId)
                {
                    case EVENT_HOLY_BLAST:
                        DoCastVictim(SPELL_HOLY_BLAST);
                        DoModifyThreatPercent(me->GetVictim(), -100);
                        events.ScheduleEvent(EVENT_HOLY_BLAST, urand(7000, 27000));
                        break;
                    case EVENT_HOLY_NOVA:
                        if (SelectTarget(SELECT_TARGET_RANDOM, 0, 15, true))
                            DoCastAOE(SPELL_HOLY_NOVA);
                        events.ScheduleEvent(EVENT_HOLY_NOVA, urand(32000, 49000)); // need more info about timers
                        break;
                    case EVENT_HOLY_SMITE:
                        DoCastVictim(SPELL_HOLY_SMITE);
                        events.ScheduleEvent(EVENT_HOLY_SMITE, urand(4000, 12000));
                        break;
                    case EVENT_PRAYER_OF_HEALING:
                        // how to handle this? timers?
                        DoCastAOE(SPELL_PRAYER_OF_HEALING);
                        events.ScheduleEvent(EVENT_PRAYER_OF_HEALING, 12000);
                        break;
                    case EVENT_STAFF_STRIKE:
                        DoCastVictim(SPELL_STAFF_STRIKE);
                        events.ScheduleEvent(EVENT_STAFF_STRIKE, urand(7000, 17000));
                        break;
                    case EVENT_CALL_GUARDS:
                        // think this boss not only calls all nearby guards but summon them
                        me->CallForHelp(VISIBLE_RANGE);
                        events.ScheduleEvent(EVENT_CALL_GUARDS, 5000);
                        break;
                    default:
                        break;
                }
            }

            void UpdateAI(uint32 const diff)
            {
                if (!UpdateVictim())
                    return;

                events.Update(diff);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                while (uint32 eventId = events.ExecuteEvent())
                    ExecuteEvent(eventId);

                DoMeleeAttackIfReady();
            }

            private:
                bool _secondPhase;
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return new boss_velenAI(creature);
        }
};

enum TheronTexts
{
    SAY_THERON_KILL_PlAYER          = 0, // "It was inevitable." or "You die in vain!"
};

enum TheronSpells
{
    SPELL_THERON_CLEAVE             = 15284,
    SPELL_ARCANE_SHOCK              = 59715, // 2.1.3 wowhead: 12-13 sec cooldown
    SPELL_MANA_BURN                 = 33385,
    SPELL_MASS_CHARM                = 33384,
};

class boss_theron : public CreatureScript
{
    public:
        boss_theron() : CreatureScript("boss_theron") { }

        struct boss_theronAI : public WorldBossAI
        {
            boss_theronAI(Creature* creature) : WorldBossAI(creature) { }

            void EnterCombat(Unit* /*who*/)
            {
				sWorld->SendWorldText(LANG_ANNOUNCE_4);
                _EnterCombat();
                /*events.ScheduleEvent(EVENT_CLEAVE, 10000);
                events.ScheduleEvent(EVENT_ARCANE_SHOCK, 15000);
                events.ScheduleEvent(EVENT_MANA_BURN, 15000);
                events.ScheduleEvent(EVENT_MASS_CHARM, 15000);*/
                events.ScheduleEvent(EVENT_CALL_GUARDS, 1000);
            }

            void KilledUnit(Unit* /*who*/)
            {
                Talk(SAY_THERON_KILL_PlAYER);
            }

            void ExecuteEvent(uint32 const eventId)
            {
                switch (eventId)
                {
                    // TODO: find info about all these spells, for now here just placeholders
                    /*case EVENT_CLEAVE:
                        DoCastVictim(SPELL_THERON_CLEAVE);
                        events.ScheduleEvent(EVENT_CLEAVE, urand(7000, 14000));
                        break;
                    case EVENT_ARCANE_SHOCK:
                        if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 20, true))
                            DoCast(target, SPELL_ARCANE_SHOCK);
                        events.ScheduleEvent(EVENT_ARCANE_SHOCK, urand(8000, 19000));
                        break;
                    case EVENT_MANA_BURN:
                        if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, ManaUserPlayerSelector(me, 30)))
                            DoCast(target, SPELL_MANA_BURN);
                        events.ScheduleEvent(EVENT_MANA_BURN, urand(7000, 14000));
                        break;
                    case EVENT_MASS_CHARM:
                        DoCastAOE(SPELL_MASS_CHARM);
                        events.ScheduleEvent(EVENT_MASS_CHARM, urand(7000, 14000));
                        break;*/
                    case EVENT_CALL_GUARDS:
                        me->CallForHelp(VISIBLE_RANGE);
                        events.ScheduleEvent(EVENT_CALL_GUARDS, 5000);
                        break;
                    default:
                        break;
                }
            }

            void UpdateAI(uint32 const diff)
            {
                if (!UpdateVictim())
                    return;

                events.Update(diff);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                while (uint32 eventId = events.ExecuteEvent())
                    ExecuteEvent(eventId);

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return new boss_theronAI(creature);
        }
};

void AddSC_stormwind_city()
{
    new npc_archmage_malin();
    new npc_bartleby();
    new npc_lady_katrana_prestor();
    new npc_tyrion();
    new npc_tyrion_spybot();
    new npc_lord_gregor_lescovar();
    new npc_marzon_silent_blade();
	new boss_varian();
	new boss_magni();
	new boss_tyrande();
	new boss_velen();
	new boss_theron();
}
