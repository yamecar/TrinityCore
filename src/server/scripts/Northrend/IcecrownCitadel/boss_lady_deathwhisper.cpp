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

#include "ObjectMgr.h"
#include "ScriptMgr.h"
#include "ScriptedCreature.h"
#include "PoolMgr.h"
#include "Group.h"
#include "icecrown_citadel.h"
#include "SpellInfo.h"
#include "Player.h"

enum ScriptTexts
{
    // Lady Deathwhisper
    SAY_INTRO_1                 = 0,
    SAY_INTRO_2                 = 1,
    SAY_INTRO_3                 = 2,
    SAY_INTRO_4                 = 3,
    SAY_INTRO_5                 = 4,
    SAY_INTRO_6                 = 5,
    SAY_INTRO_7                 = 6,
    SAY_AGGRO                   = 7,
    SAY_PHASE_2                 = 8,
    EMOTE_PHASE_2               = 9,
    SAY_DOMINATE_MIND           = 10,
    SAY_DARK_EMPOWERMENT        = 11,
    SAY_DARK_TRANSFORMATION     = 12,
    SAY_ANIMATE_DEAD            = 13,
    SAY_KILL                    = 14,
    SAY_BERSERK                 = 15,
    SAY_DEATH                   = 16,

    // Darnavan
    SAY_DARNAVAN_AGGRO          = 0,
    SAY_DARNAVAN_RESCUED        = 1,
};

enum Spells
{
    // Lady Deathwhisper
    SPELL_MANA_BARRIER              = 70842,
    SPELL_SHADOW_BOLT               = 71254,
    SPELL_DEATH_AND_DECAY           = 71001,
    SPELL_DOMINATE_MIND_H           = 71289,
    SPELL_FROSTBOLT                 = 71420,
    SPELL_FROSTBOLT_VOLLEY          = 72905,
    SPELL_TOUCH_OF_INSIGNIFICANCE   = 71204,
    SPELL_SUMMON_SHADE              = 71363,
    SPELL_SHADOW_CHANNELING         = 43897, // Prefight, during intro
    SPELL_DARK_TRANSFORMATION_T     = 70895,
    SPELL_DARK_EMPOWERMENT_T        = 70896,
    SPELL_DARK_MARTYRDOM_T          = 70897,

    // Achievement
    SPELL_FULL_HOUSE                = 72827, // does not exist in dbc but still can be used for criteria check

    // Both Adds
    SPELL_TELEPORT_VISUAL           = 41236,

    // Fanatics
    SPELL_DARK_TRANSFORMATION       = 70900,
    SPELL_NECROTIC_STRIKE           = 70659,
    SPELL_SHADOW_CLEAVE             = 70670,
    SPELL_VAMPIRIC_MIGHT            = 70674,
    SPELL_FANATIC_S_DETERMINATION   = 71235,
    SPELL_DARK_MARTYRDOM_FANATIC    = 71236,

    //  Adherents
    SPELL_DARK_EMPOWERMENT          = 70901,
    SPELL_FROST_FEVER               = 67767,
    SPELL_DEATHCHILL_BOLT           = 70594,
    SPELL_DEATHCHILL_BLAST          = 70906,
    SPELL_CURSE_OF_TORPOR           = 71237,
    SPELL_SHORUD_OF_THE_OCCULT      = 70768,
    SPELL_ADHERENT_S_DETERMINATION  = 71234,
    SPELL_DARK_MARTYRDOM_ADHERENT   = 70903,

    // Vengeful Shade
    SPELL_VENGEFUL_BLAST            = 71544,
    SPELL_VENGEFUL_BLAST_PASSIVE    = 71494,
    SPELL_VENGEFUL_BLAST_25N        = 72010,
    SPELL_VENGEFUL_BLAST_10H        = 72011,
    SPELL_VENGEFUL_BLAST_25H        = 72012,

    // Darnavan
    SPELL_BLADESTORM                = 65947,
    SPELL_CHARGE                    = 65927,
    SPELL_INTIMIDATING_SHOUT        = 65930,
    SPELL_MORTAL_STRIKE             = 65926,
    SPELL_SHATTERING_THROW          = 65940,
    SPELL_SUNDER_ARMOR              = 65936,
};

enum EventTypes
{
    // Lady Deathwhisper
    EVENT_INTRO_2                       = 1,
    EVENT_INTRO_3                       = 2,
    EVENT_INTRO_4                       = 3,
    EVENT_INTRO_5                       = 4,
    EVENT_INTRO_6                       = 5,
    EVENT_INTRO_7                       = 6,
    EVENT_BERSERK                       = 7,
    EVENT_DEATH_AND_DECAY               = 8,
    EVENT_DOMINATE_MIND_H               = 9,

    // Phase 1 only
    EVENT_P1_SUMMON_WAVE                = 10,
    EVENT_P1_SHADOW_BOLT                = 11,
    EVENT_P1_EMPOWER_CULTIST            = 12,
    EVENT_P1_REANIMATE_CULTIST          = 13,

    // Phase 2 only
    EVENT_P2_SUMMON_WAVE                = 14,
    EVENT_P2_FROSTBOLT                  = 15,
    EVENT_P2_FROSTBOLT_VOLLEY           = 16,
    EVENT_P2_TOUCH_OF_INSIGNIFICANCE    = 17,
    EVENT_P2_SUMMON_SHADE               = 18,

    // Shared adds events
    EVENT_CULTIST_DARK_MARTYRDOM        = 19,

    // Cult Fanatic
    EVENT_FANATIC_NECROTIC_STRIKE       = 20,
    EVENT_FANATIC_SHADOW_CLEAVE         = 21,
    EVENT_FANATIC_VAMPIRIC_MIGHT        = 22,

    // Cult Adherent
    EVENT_ADHERENT_FROST_FEVER          = 23,
    EVENT_ADHERENT_DEATHCHILL           = 24,
    EVENT_ADHERENT_CURSE_OF_TORPOR      = 25,
    EVENT_ADHERENT_SHORUD_OF_THE_OCCULT = 26,

    // Darnavan
    EVENT_DARNAVAN_BLADESTORM           = 27,
    EVENT_DARNAVAN_CHARGE               = 28,
    EVENT_DARNAVAN_INTIMIDATING_SHOUT   = 29,
    EVENT_DARNAVAN_MORTAL_STRIKE        = 30,
    EVENT_DARNAVAN_SHATTERING_THROW     = 31,
    EVENT_DARNAVAN_SUNDER_ARMOR         = 32,
};

enum Phases
{
    PHASE_ALL       = 0,
    PHASE_INTRO     = 1,
    PHASE_ONE       = 2,
    PHASE_TWO       = 3
};

enum DeprogrammingData
{
    NPC_DARNAVAN_10         = 38472,
    NPC_DARNAVAN_25         = 38485,
    NPC_DARNAVAN_CREDIT_10  = 39091,
    NPC_DARNAVAN_CREDIT_25  = 39092,

    ACTION_COMPLETE_QUEST   = -384720,
    POINT_DESPAWN           = 384721,
};

enum Actions
{
    ACTION_START_INTRO
};

#define NPC_DARNAVAN        RAID_MODE<uint32>(NPC_DARNAVAN_10, NPC_DARNAVAN_25, NPC_DARNAVAN_10, NPC_DARNAVAN_25)
#define NPC_DARNAVAN_CREDIT RAID_MODE<uint32>(NPC_DARNAVAN_CREDIT_10, NPC_DARNAVAN_CREDIT_25, NPC_DARNAVAN_CREDIT_10, NPC_DARNAVAN_CREDIT_25)
#define QUEST_DEPROGRAMMING RAID_MODE<uint32>(QUEST_DEPROGRAMMING_10, QUEST_DEPROGRAMMING_25, QUEST_DEPROGRAMMING_10, QUEST_DEPROGRAMMING_25)

uint32 const SummonEntries[2] = {NPC_CULT_FANATIC, NPC_CULT_ADHERENT};

#define GUID_CULTIST    1

Position const SummonPositions[7] =
{
    {-578.7066f, 2154.167f, 51.01529f, 1.692969f}, // 1 Left Door 1 (Cult Fanatic)
    {-598.9028f, 2155.005f, 51.01530f, 1.692969f}, // 2 Left Door 2 (Cult Adherent)
    {-619.2864f, 2154.460f, 51.01530f, 1.692969f}, // 3 Left Door 3 (Cult Fanatic)
    {-578.6996f, 2269.856f, 51.01529f, 4.590216f}, // 4 Right Door 1 (Cult Adherent)
    {-598.9688f, 2269.264f, 51.01529f, 4.590216f}, // 5 Right Door 2 (Cult Fanatic)
    {-619.4323f, 2268.523f, 51.01530f, 4.590216f}, // 6 Right Door 3 (Cult Adherent)
    {-524.2480f, 2211.920f, 62.90960f, 3.141592f}, // 7 Upper (Random Cultist)
};

class DaranavanMoveEvent : public BasicEvent
{
    public:
        DaranavanMoveEvent(Creature& darnavan) : _darnavan(darnavan) { }

        bool Execute(uint64 /*time*/, uint32 /*diff*/)
        {
            _darnavan.GetMotionMaster()->MovePoint(POINT_DESPAWN, SummonPositions[6]);
            return true;
        }

    private:
        Creature& _darnavan;
};

class boss_lady_deathwhisper : public CreatureScript
{
    public:
        boss_lady_deathwhisper() : CreatureScript("boss_lady_deathwhisper") { }

        struct boss_lady_deathwhisperAI : public BossAI
        {
            boss_lady_deathwhisperAI(Creature* creature) : BossAI(creature, DATA_LADY_DEATHWHISPER),
                _dominateMindCount(RAID_MODE<uint8>(0, 1, 1, 3)), _introDone(false)
            {
            }

            void Reset() override
            {
                _Reset();
                me->SetPower(POWER_MANA, me->GetMaxPower(POWER_MANA));
                events.SetPhase(PHASE_ONE);
                _waveCounter = 0;
                _nextVengefulShadeTargetGUID = 0;
                _darnavanGUID = 0;
                DoCast(me, SPELL_SHADOW_CHANNELING);
                me->RemoveAurasDueToSpell(SPELL_BERSERK);
                me->RemoveAurasDueToSpell(SPELL_MANA_BARRIER);
                me->ApplySpellImmune(0, IMMUNITY_STATE, SPELL_AURA_MOD_TAUNT, false);
                me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_ATTACK_ME, false);
            }

            void DoAction(int32 action) override
            {
                switch (action)
                {
                    case ACTION_START_INTRO:
                        if (!_introDone)
                        {
                            _introDone = true;
                            Talk(SAY_INTRO_1);
                            events.SetPhase(PHASE_INTRO);
                            events.ScheduleEvent(EVENT_INTRO_2, 11000, 0, PHASE_INTRO);
                            events.ScheduleEvent(EVENT_INTRO_3, 21000, 0, PHASE_INTRO);
                            events.ScheduleEvent(EVENT_INTRO_4, 31500, 0, PHASE_INTRO);
                            events.ScheduleEvent(EVENT_INTRO_5, 39500, 0, PHASE_INTRO);
                            events.ScheduleEvent(EVENT_INTRO_6, 48500, 0, PHASE_INTRO);
                            events.ScheduleEvent(EVENT_INTRO_7, 58000, 0, PHASE_INTRO);
                        }
                        break;
                    default:
                        break;
                }
            }

            void AttackStart(Unit* victim) override
            {
                if (me->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE))
                    return;

                if (victim && me->Attack(victim, true) && !events.IsInPhase(PHASE_ONE))
                    me->GetMotionMaster()->MoveChase(victim);
            }

            void EnterCombat(Unit* who) override
            { /*
                if (!instance->CheckRequiredBosses(DATA_LADY_DEATHWHISPER, who->ToPlayer()))
                {
                    EnterEvadeMode();
                    instance->DoCastSpellOnPlayers(LIGHT_S_HAMMER_TELEPORT);
                    return;
                }
*/
                me->setActive(true);
                DoZoneInCombat();

                events.Reset();
                events.SetPhase(PHASE_ONE);
                // phase-independent events
                events.ScheduleEvent(EVENT_BERSERK, 600000);
                events.ScheduleEvent(EVENT_DEATH_AND_DECAY, 10000);
                // phase one only
                events.ScheduleEvent(EVENT_P1_SUMMON_WAVE, 5000, 0, PHASE_ONE);
                events.ScheduleEvent(EVENT_P1_SHADOW_BOLT, urand(5500, 6000), 0, PHASE_ONE);
                events.ScheduleEvent(EVENT_P1_EMPOWER_CULTIST, urand(20000, 30000), 0, PHASE_ONE);
				events.ScheduleEvent(EVENT_P1_REANIMATE_CULTIST, urand(10000, 20000), 0, PHASE_ONE);
                if (GetDifficulty() != RAID_DIFFICULTY_10MAN_NORMAL)
                    events.ScheduleEvent(EVENT_DOMINATE_MIND_H, 27000);

                Talk(SAY_AGGRO);
                DoStartNoMovement(who);
                me->RemoveAurasDueToSpell(SPELL_SHADOW_CHANNELING);
                DoCast(me, SPELL_MANA_BARRIER, true);

                instance->SetBossState(DATA_LADY_DEATHWHISPER, IN_PROGRESS);
            }

            void JustDied(Unit* killer) override
            {
                Talk(SAY_DEATH);

                std::set<uint32> livingAddEntries;
                // Full House achievement
                for (SummonList::iterator itr = summons.begin(); itr != summons.end(); ++itr)
                    if (Unit* unit = ObjectAccessor::GetUnit(*me, *itr))
                        if (unit->IsAlive() && unit->GetEntry() != NPC_VENGEFUL_SHADE)
                            livingAddEntries.insert(unit->GetEntry());

                if (livingAddEntries.size() >= 5)
                    instance->DoUpdateAchievementCriteria(ACHIEVEMENT_CRITERIA_TYPE_BE_SPELL_TARGET, SPELL_FULL_HOUSE, 0, me);

                if (Creature* darnavan = ObjectAccessor::GetCreature(*me, _darnavanGUID))
                {
                    if (darnavan->IsAlive())
                    {
                        darnavan->setFaction(35);
                        darnavan->CombatStop(true);
                        darnavan->GetMotionMaster()->MoveIdle();
                        darnavan->SetReactState(REACT_PASSIVE);
                        darnavan->m_Events.AddEvent(new DaranavanMoveEvent(*darnavan), darnavan->m_Events.CalculateTime(10000));
                        darnavan->AI()->Talk(SAY_DARNAVAN_RESCUED);
                        if (Player* owner = killer->GetCharmerOrOwnerPlayerOrPlayerItself())
                        {
                            if (Group* group = owner->GetGroup())
                            {
                                for (GroupReference* itr = group->GetFirstMember(); itr != NULL; itr = itr->next())
                                    if (Player* member = itr->GetSource())
										if (member->IsInMap(owner))
											member->KilledMonsterCredit(NPC_DARNAVAN_CREDIT, 0);
                            }
                            else
                                owner->KilledMonsterCredit(NPC_DARNAVAN_CREDIT, 0);
                        }
                    }
                }

                _JustDied();
            }

            void JustReachedHome() override
            {
                _JustReachedHome();
                instance->SetBossState(DATA_LADY_DEATHWHISPER, FAIL);

                summons.DespawnAll();
                if (Creature* darnavan = ObjectAccessor::GetCreature(*me, _darnavanGUID))
                {
                    darnavan->DespawnOrUnsummon();
                    _darnavanGUID = 0;
                }
            }

            void KilledUnit(Unit* victim) override
            {
                if (victim->GetTypeId() == TYPEID_PLAYER)
                    Talk(SAY_KILL);
            }

            void DamageTaken(Unit* /*damageDealer*/, uint32& damage) override
            {
                // phase transition
                if (events.IsInPhase(PHASE_ONE) && damage > me->GetPower(POWER_MANA))
                {
                    Talk(SAY_PHASE_2);
                    Talk(EMOTE_PHASE_2);
                    DoStartMovement(me->GetVictim());
                    damage -= me->GetPower(POWER_MANA);
                    me->SetPower(POWER_MANA, 0);
                    me->RemoveAurasDueToSpell(SPELL_MANA_BARRIER);
                    events.SetPhase(PHASE_TWO);
                    events.ScheduleEvent(EVENT_P2_FROSTBOLT, urand(10000, 12000), 0, PHASE_TWO);
                    events.ScheduleEvent(EVENT_P2_FROSTBOLT_VOLLEY, urand(19000, 21000), 0, PHASE_TWO);
                    events.ScheduleEvent(EVENT_P2_TOUCH_OF_INSIGNIFICANCE, urand(6000, 9000), 0, PHASE_TWO);
                    events.ScheduleEvent(EVENT_P2_SUMMON_SHADE, urand(12000, 15000), 0, PHASE_TWO);
                    // on heroic mode Lady Deathwhisper is immune to taunt effects in phase 2 and continues summoning adds
                    if (IsHeroic())
                    {
                        me->ApplySpellImmune(0, IMMUNITY_STATE, SPELL_AURA_MOD_TAUNT, true);
                        me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_ATTACK_ME, true);
                        events.ScheduleEvent(EVENT_P2_SUMMON_WAVE, 45000, 0, PHASE_TWO);
                    }
                }
            }

            void JustSummoned(Creature* summon) override
            {
                if (summon->GetEntry() == NPC_DARNAVAN)
                    _darnavanGUID = summon->GetGUID();
                else
                    summons.Summon(summon);

                Unit* target = NULL;
                if (summon->GetEntry() == NPC_VENGEFUL_SHADE)
                {
                    target = ObjectAccessor::GetUnit(*me, _nextVengefulShadeTargetGUID);   // Vengeful Shade
                    _nextVengefulShadeTargetGUID = 0;
                }
                else
                    target = SelectTarget(SELECT_TARGET_RANDOM);                        // Wave adds

                summon->AI()->AttackStart(target);                                      // CAN be NULL
            }

            void UpdateAI(uint32 diff) override
            {
                if ((!UpdateVictim() && !events.IsInPhase(PHASE_INTRO)) || !CheckInRoom())
                    return;

                events.Update(diff);

                if (me->HasUnitState(UNIT_STATE_CASTING) && !events.IsInPhase(PHASE_INTRO))
                    return;

                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_INTRO_2:
                            Talk(SAY_INTRO_2);
                            break;
                        case EVENT_INTRO_3:
                            Talk(SAY_INTRO_3);
                            break;
                        case EVENT_INTRO_4:
                            Talk(SAY_INTRO_4);
                            break;
                        case EVENT_INTRO_5:
                            Talk(SAY_INTRO_5);
                            break;
                        case EVENT_INTRO_6:
                            Talk(SAY_INTRO_6);
                            break;
                        case EVENT_INTRO_7:
                            Talk(SAY_INTRO_7);
                            break;
                        case EVENT_DEATH_AND_DECAY:
                            if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM))
                                DoCast(target, SPELL_DEATH_AND_DECAY);
                            events.ScheduleEvent(EVENT_DEATH_AND_DECAY, urand(22000, 30000));
                            break;
                        case EVENT_DOMINATE_MIND_H:
                            Talk(SAY_DOMINATE_MIND);
                            for (uint8 i = 0; i < _dominateMindCount; i++)
                                if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 1, 0.0f, true, -SPELL_DOMINATE_MIND_H))
                                    DoCast(target, SPELL_DOMINATE_MIND_H);
                            events.ScheduleEvent(EVENT_DOMINATE_MIND_H, urand(40000, 45000));
                            break;
                        case EVENT_P1_SUMMON_WAVE:
                            SummonWaveP1();
                            events.ScheduleEvent(EVENT_P1_SUMMON_WAVE, IsHeroic() ? 45000 : 60000, 0, PHASE_ONE);
                            break;
                        case EVENT_P1_SHADOW_BOLT:
                            if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM))
                                DoCast(target, SPELL_SHADOW_BOLT);
                            events.ScheduleEvent(EVENT_P1_SHADOW_BOLT, urand(5000, 8000), 0, PHASE_ONE);
                            break;
                        case EVENT_P1_REANIMATE_CULTIST:
                            ReanimateCultist();
							events.ScheduleEvent(EVENT_P1_REANIMATE_CULTIST, urand(6000, 25000), 0, PHASE_ONE);
                            break;
                        case EVENT_P1_EMPOWER_CULTIST:
                            EmpowerCultist();
                            events.ScheduleEvent(EVENT_P1_EMPOWER_CULTIST, urand(18000, 25000), 0, PHASE_ONE);
                            break;
                        case EVENT_P2_FROSTBOLT:
							me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_INTERRUPT_CAST, false);
                            DoCastVictim(SPELL_FROSTBOLT);
                            events.ScheduleEvent(EVENT_P2_FROSTBOLT, urand(10000, 11000), 0, PHASE_TWO);
                            break;
                        case EVENT_P2_FROSTBOLT_VOLLEY:
                            DoCastAOE(SPELL_FROSTBOLT_VOLLEY);
                            events.ScheduleEvent(EVENT_P2_FROSTBOLT_VOLLEY, urand(13000, 15000), 0, PHASE_TWO);
                            break;
                        case EVENT_P2_TOUCH_OF_INSIGNIFICANCE:
                            DoCastVictim(SPELL_TOUCH_OF_INSIGNIFICANCE);
                            events.ScheduleEvent(EVENT_P2_TOUCH_OF_INSIGNIFICANCE, urand(9000, 13000), 0, PHASE_TWO);
                            break;
                        case EVENT_P2_SUMMON_SHADE:
                            if (Unit* shadeTarget = SelectTarget(SELECT_TARGET_RANDOM, 1))
                            {
                                _nextVengefulShadeTargetGUID = shadeTarget->GetGUID();
                                DoCast(shadeTarget, SPELL_SUMMON_SHADE);
                            }
                            events.ScheduleEvent(EVENT_P2_SUMMON_SHADE, urand(18000, 23000), 0, PHASE_TWO);
                            break;
                        case EVENT_P2_SUMMON_WAVE:
                            SummonWaveP2();
                            events.ScheduleEvent(EVENT_P2_SUMMON_WAVE, 45000, 0, PHASE_TWO);
                            break;
                        case EVENT_BERSERK:
                            DoCast(me, SPELL_BERSERK);
                            Talk(SAY_BERSERK);
                            break;
                    }
                }

                // We should not melee attack when barrier is up
                if (me->HasAura(SPELL_MANA_BARRIER))
                    return;

                DoMeleeAttackIfReady();
            }

            // summoning function for first phase
            void SummonWaveP1()
            {
                uint8 addIndex = _waveCounter & 1;
                uint8 addIndexOther = uint8(addIndex ^ 1);

                // Summon first add, replace it with Darnavan if weekly quest is active
                if (_waveCounter || !sPoolMgr->IsSpawnedObject<Quest>(QUEST_DEPROGRAMMING))
                    Summon(SummonEntries[addIndex], SummonPositions[addIndex * 3]);
                else
                    Summon(NPC_DARNAVAN, SummonPositions[addIndex * 3]);

                Summon(SummonEntries[addIndexOther], SummonPositions[addIndex * 3 + 1]);
                Summon(SummonEntries[addIndex], SummonPositions[addIndex * 3 + 2]);
                if (Is25ManRaid())
                {
                    Summon(SummonEntries[addIndexOther], SummonPositions[addIndexOther * 3]);
                    Summon(SummonEntries[addIndex], SummonPositions[addIndexOther * 3 + 1]);
                    Summon(SummonEntries[addIndexOther], SummonPositions[addIndexOther * 3 + 2]);
                    Summon(SummonEntries[urand(0, 1)], SummonPositions[6]);
                }

                ++_waveCounter;
            }

            // summoning function for second phase
            void SummonWaveP2()
            {
                if (Is25ManRaid())
                {
                    uint8 addIndex = _waveCounter & 1;
                    Summon(SummonEntries[addIndex], SummonPositions[addIndex * 3]);
                    Summon(SummonEntries[addIndex ^ 1], SummonPositions[addIndex * 3 + 1]);
                    Summon(SummonEntries[addIndex], SummonPositions[addIndex * 3+ 2]);
                }
                else
                    Summon(SummonEntries[urand(0, 1)], SummonPositions[6]);

                ++_waveCounter;
            }

            // helper for summoning wave mobs
            void Summon(uint32 entry, const Position& pos)
            {
                if (TempSummon* summon = me->SummonCreature(entry, pos, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 10000))
                    summon->CastSpell(summon, SPELL_TELEPORT_VISUAL);
            }

            void ReanimateCultist()
            {
                if (summons.empty())
                    return;

                std::list<Creature*> temp;
                for (SummonList::iterator itr = summons.begin(); itr != summons.end(); ++itr)
                    if (Creature* cre = ObjectAccessor::GetCreature(*me, *itr))
                        if (cre->IsAlive() && (cre->GetEntry() == NPC_CULT_FANATIC || cre->GetEntry() == NPC_CULT_ADHERENT))
                            temp.push_back(cre);

				if (temp.empty())
                    return;

                // select random cultist
                Creature* cultist = Trinity::Containers::SelectRandomContainerElement(temp);
                DoCast(cultist, SPELL_DARK_MARTYRDOM_T, true);
				cultist->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_INTERRUPT_CAST, true);
            }

            void EmpowerCultist()
            {
                if (summons.empty())
                    return;

                std::list<Creature*> temp;
                for (SummonList::iterator itr = summons.begin(); itr != summons.end(); ++itr)
                    if (Creature* cre = ObjectAccessor::GetCreature(*me, *itr))
                        if (cre->IsAlive() && (cre->GetEntry() == NPC_CULT_FANATIC || cre->GetEntry() == NPC_CULT_ADHERENT))
                            temp.push_back(cre);

                // noone to empower
                if (temp.empty())
                    return;

                // select random cultist
                Creature* cultist = Trinity::Containers::SelectRandomContainerElement(temp);
                DoCast(cultist, cultist->GetEntry() == NPC_CULT_FANATIC ? SPELL_DARK_TRANSFORMATION_T : SPELL_DARK_EMPOWERMENT_T, true);
                Talk(uint8(cultist->GetEntry() == NPC_CULT_FANATIC ? SAY_DARK_TRANSFORMATION : SAY_DARK_EMPOWERMENT));
            }

        private:
            uint64 _nextVengefulShadeTargetGUID;
            uint64 _darnavanGUID;
            uint32 _waveCounter;
            uint8 const _dominateMindCount;
            bool _introDone;
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return GetIcecrownCitadelAI<boss_lady_deathwhisperAI>(creature);
        }
};

typedef boss_lady_deathwhisper::boss_lady_deathwhisperAI DeathwisperAI;

class npc_cult_fanatic : public CreatureScript
{
    public:
        npc_cult_fanatic() : CreatureScript("npc_cult_fanatic") { }

        struct npc_cult_fanaticAI : public ScriptedAI
        {
            npc_cult_fanaticAI(Creature* creature) : ScriptedAI(creature) { }

            void Reset() override
            {
                Events.Reset();
                Events.ScheduleEvent(EVENT_FANATIC_NECROTIC_STRIKE, urand(10000, 12000));
                Events.ScheduleEvent(EVENT_FANATIC_SHADOW_CLEAVE, urand(14000, 16000));
                Events.ScheduleEvent(EVENT_FANATIC_VAMPIRIC_MIGHT, urand(20000, 27000));
            }

            void SpellHit(Unit* /*caster*/, SpellInfo const* spell) override
            {
                switch (spell->Id)
                {
                    case SPELL_DARK_TRANSFORMATION:
                        me->UpdateEntry(NPC_DEFORMED_FANATIC);
						me->SetReactState(REACT_AGGRESSIVE);
                        AttackStart(SelectTarget(SELECT_TARGET_RANDOM));
                        break;
                    case SPELL_DARK_TRANSFORMATION_T:
                        if (me->HasFlag(UNIT_DYNAMIC_FLAGS, UNIT_DYNFLAG_DEAD))
                            break;
                        me->InterruptNonMeleeSpells(true);
                        DoCast(me, SPELL_DARK_TRANSFORMATION);
                        break;
                    case SPELL_DARK_MARTYRDOM_T:
                        me->InterruptNonMeleeSpells(true);
                        DoCast(me, SPELL_DARK_MARTYRDOM_FANATIC);
                        break;
                    case SPELL_DARK_MARTYRDOM_FANATIC: // 10nm
                    case 72495: // 25nm
                    case 72496: // 10hc
                    case 72497: // 25hc
                        me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_DISABLE_MOVE | UNIT_FLAG_PACIFIED | UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_UNK_29);
                        me->SetFlag(UNIT_DYNAMIC_FLAGS, UNIT_DYNFLAG_DEAD);
                        me->SetFlag(UNIT_FIELD_FLAGS_2, UNIT_FLAG2_FEIGN_DEATH);
                        me->SetReactState(REACT_PASSIVE);
                        me->AttackStop();
                        Events.ScheduleEvent(EVENT_CULTIST_DARK_MARTYRDOM, 4000);
                        break;
                    default:
                        break;
                }
            }

            void UpdateAI(uint32 diff) override
            {
                if (!UpdateVictim())
                    return;

                Events.Update(diff);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                while (uint32 eventId = Events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_FANATIC_NECROTIC_STRIKE:
                            DoCastVictim(SPELL_NECROTIC_STRIKE);
                            Events.ScheduleEvent(EVENT_FANATIC_NECROTIC_STRIKE, urand(11000, 13000));
                            break;
                        case EVENT_FANATIC_SHADOW_CLEAVE:
                            DoCastVictim(SPELL_SHADOW_CLEAVE);
                            Events.ScheduleEvent(EVENT_FANATIC_SHADOW_CLEAVE, urand(9500, 11000));
                            break;
                        case EVENT_FANATIC_VAMPIRIC_MIGHT:
                            DoCast(me, SPELL_VAMPIRIC_MIGHT);
                            Events.ScheduleEvent(EVENT_FANATIC_VAMPIRIC_MIGHT, urand(20000, 27000));
                            break;
                        case EVENT_CULTIST_DARK_MARTYRDOM:
                            if (me->IsSummon())
                                if (Unit* owner = me->ToTempSummon()->GetSummoner())
                                    if (owner->ToCreature())
                                        owner->ToCreature()->AI()->Talk(SAY_ANIMATE_DEAD);
                            me->UpdateEntry(NPC_REANIMATED_FANATIC);
                            me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_DISABLE_MOVE | UNIT_FLAG_PACIFIED | UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_UNK_29);
                            me->RemoveFlag(UNIT_DYNAMIC_FLAGS, UNIT_DYNFLAG_DEAD);
                            me->RemoveFlag(UNIT_FIELD_FLAGS_2, UNIT_FLAG2_FEIGN_DEATH);
                            DoCast(me, SPELL_FANATIC_S_DETERMINATION);
                            me->SetReactState(REACT_AGGRESSIVE);
                            AttackStart(SelectTarget(SELECT_TARGET_RANDOM));
                            break;
                    }
                }

                DoMeleeAttackIfReady();
            }

        protected:
            EventMap Events;
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return GetIcecrownCitadelAI<npc_cult_fanaticAI>(creature);
        }
};

class npc_cult_adherent : public CreatureScript
{
    public:
        npc_cult_adherent() : CreatureScript("npc_cult_adherent") { }

        struct npc_cult_adherentAI : public ScriptedAI
        {
            npc_cult_adherentAI(Creature* creature) : ScriptedAI(creature) { }

            void Reset() override
            {
                Events.Reset();
                Events.ScheduleEvent(EVENT_ADHERENT_FROST_FEVER, urand(10000, 12000));
                Events.ScheduleEvent(EVENT_ADHERENT_DEATHCHILL, urand(14000, 16000));
                Events.ScheduleEvent(EVENT_ADHERENT_CURSE_OF_TORPOR, urand(14000, 16000));
                Events.ScheduleEvent(EVENT_ADHERENT_SHORUD_OF_THE_OCCULT, urand(3000, 5000));
            }

            void SpellHit(Unit* /*caster*/, SpellInfo const* spell) override
            {
                switch (spell->Id)
                {
                    case SPELL_DARK_EMPOWERMENT:
                        me->UpdateEntry(NPC_EMPOWERED_ADHERENT);
						me->SetReactState(REACT_AGGRESSIVE);
                        AttackStart(SelectTarget(SELECT_TARGET_RANDOM));
                        break;
                    case SPELL_DARK_EMPOWERMENT_T:
                        if (me->HasFlag(UNIT_DYNAMIC_FLAGS, UNIT_DYNFLAG_DEAD))
                            break;
                        me->InterruptNonMeleeSpells(true);
                        DoCast(me, SPELL_DARK_EMPOWERMENT);
                        break;
                    case SPELL_DARK_MARTYRDOM_T:
                        me->InterruptNonMeleeSpells(true);
                        DoCast(me, SPELL_DARK_MARTYRDOM_ADHERENT);
                        break;
                    case SPELL_DARK_MARTYRDOM_ADHERENT:
                    case 72498:
                    case 72499:
                    case 72500:
                        me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_DISABLE_MOVE | UNIT_FLAG_PACIFIED | UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_UNK_29);
                        me->SetFlag(UNIT_DYNAMIC_FLAGS, UNIT_DYNFLAG_DEAD);
                        me->SetFlag(UNIT_FIELD_FLAGS_2, UNIT_FLAG2_FEIGN_DEATH);
                        me->SetReactState(REACT_PASSIVE);
                        me->AttackStop();
                        Events.ScheduleEvent(EVENT_CULTIST_DARK_MARTYRDOM, 4000);
                        break;
                    default:
                        break;
                }
            }

            void UpdateAI(uint32 diff) override
            {
                if (!UpdateVictim())
                    return;

				if (me->GetEntry() == NPC_EMPOWERED_ADHERENT || me->GetEntry() == NPC_REANIMATED_ADHERENT)
					me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_INTERRUPT_CAST, true);

                Events.Update(diff);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                while (uint32 eventId = Events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_ADHERENT_FROST_FEVER:
                            DoCastVictim(SPELL_FROST_FEVER);
                            Events.ScheduleEvent(EVENT_ADHERENT_FROST_FEVER, urand(9000, 13000));
                            break;
                        case EVENT_ADHERENT_DEATHCHILL:
                            if (me->GetEntry() == NPC_EMPOWERED_ADHERENT || me->GetEntry() == NPC_REANIMATED_ADHERENT)
                                DoCastVictim(SPELL_DEATHCHILL_BLAST);
                            else
                                DoCastVictim(SPELL_DEATHCHILL_BOLT);
                            Events.ScheduleEvent(EVENT_ADHERENT_DEATHCHILL, urand(9000, 13000));
                            break;
                        case EVENT_ADHERENT_CURSE_OF_TORPOR:
                            if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 1))
                                DoCast(target, SPELL_CURSE_OF_TORPOR);
                            Events.ScheduleEvent(EVENT_ADHERENT_CURSE_OF_TORPOR, urand(9000, 13000));
                            break;
                        case EVENT_ADHERENT_SHORUD_OF_THE_OCCULT:
                            DoCast(me, SPELL_SHORUD_OF_THE_OCCULT);
                            Events.ScheduleEvent(EVENT_ADHERENT_SHORUD_OF_THE_OCCULT, urand(27000, 32000));
                            break;
                        case EVENT_CULTIST_DARK_MARTYRDOM:
                            if (me->IsSummon())
                                if (Unit* owner = me->ToTempSummon()->GetSummoner())
                                    if (owner->ToCreature())
                                        owner->ToCreature()->AI()->Talk(SAY_ANIMATE_DEAD);
                            me->UpdateEntry(NPC_REANIMATED_ADHERENT);
                            me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_DISABLE_MOVE | UNIT_FLAG_PACIFIED | UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_UNK_29);
                            me->RemoveFlag(UNIT_FIELD_FLAGS_2, UNIT_FLAG2_FEIGN_DEATH);
                            me->RemoveFlag(UNIT_DYNAMIC_FLAGS, UNIT_DYNFLAG_DEAD);
                            DoCast(me, SPELL_ADHERENT_S_DETERMINATION);
                            me->SetReactState(REACT_AGGRESSIVE);
                            AttackStart(SelectTarget(SELECT_TARGET_RANDOM));
                            break;
                    }
                }

                DoMeleeAttackIfReady();
            }

        protected:
            EventMap Events;
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return GetIcecrownCitadelAI<npc_cult_adherentAI>(creature);
        }
};

class npc_vengeful_shade : public CreatureScript
{
    public:
        npc_vengeful_shade() : CreatureScript("npc_vengeful_shade") { }

        struct npc_vengeful_shadeAI : public ScriptedAI
        {
            npc_vengeful_shadeAI(Creature* creature) : ScriptedAI(creature)
            {
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
            }

            void Reset() override
            {
                me->AddAura(SPELL_VENGEFUL_BLAST_PASSIVE, me);
            }

            void SpellHitTarget(Unit* /*target*/, SpellInfo const* spell) override
            {
                switch (spell->Id)
                {
                    case SPELL_VENGEFUL_BLAST:
                    case SPELL_VENGEFUL_BLAST_25N:
                    case SPELL_VENGEFUL_BLAST_10H:
                    case SPELL_VENGEFUL_BLAST_25H:
                        me->Kill(me);
                        break;
                    default:
                        break;
                }
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return GetIcecrownCitadelAI<npc_vengeful_shadeAI>(creature);
        }
};

class npc_darnavan : public CreatureScript
{
    public:
        npc_darnavan() : CreatureScript("npc_darnavan") { }

        struct npc_darnavanAI : public ScriptedAI
        {
            npc_darnavanAI(Creature* creature) : ScriptedAI(creature)
            {
            }

            void Reset() override
            {
                _events.Reset();
                _events.ScheduleEvent(EVENT_DARNAVAN_BLADESTORM, 10000);
                _events.ScheduleEvent(EVENT_DARNAVAN_INTIMIDATING_SHOUT, urand(20000, 25000));
                _events.ScheduleEvent(EVENT_DARNAVAN_MORTAL_STRIKE, urand(25000, 30000));
                _events.ScheduleEvent(EVENT_DARNAVAN_SUNDER_ARMOR, urand(5000, 8000));
                _canCharge = true;
                _canShatter = true;
            }

            void JustDied(Unit* killer) override
            {
                _events.Reset();
                if (Player* owner = killer->GetCharmerOrOwnerPlayerOrPlayerItself())
                {
                    if (Group* group = owner->GetGroup())
                    {
                        for (GroupReference* itr = group->GetFirstMember(); itr != NULL; itr = itr->next())
                            if (Player* member = itr->GetSource())
								if (member->IsInMap(owner))
									member->FailQuest(QUEST_DEPROGRAMMING);
                    }
                    else
                        owner->FailQuest(QUEST_DEPROGRAMMING);
                }
            }

            void MovementInform(uint32 type, uint32 id) override
            {
                if (type != POINT_MOTION_TYPE || id != POINT_DESPAWN)
                    return;

                me->DespawnOrUnsummon();
            }

            void EnterCombat(Unit* /*victim*/) override
            {
                Talk(SAY_DARNAVAN_AGGRO);
            }

            void UpdateAI(uint32 diff) override
            {
                if (!UpdateVictim())
                    return;

                _events.Update(diff);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                if (_canShatter && me->GetVictim() && me->EnsureVictim()->IsImmunedToDamage(SPELL_SCHOOL_MASK_NORMAL))
                {
                    DoCastVictim(SPELL_SHATTERING_THROW);
                    _canShatter = false;
                    _events.ScheduleEvent(EVENT_DARNAVAN_SHATTERING_THROW, 30000);
                    return;
                }

                if (_canCharge && !me->IsWithinMeleeRange(me->GetVictim()))
                {
                    DoCastVictim(SPELL_CHARGE);
                    _canCharge = false;
                    _events.ScheduleEvent(EVENT_DARNAVAN_CHARGE, 20000);
                    return;
                }

                while (uint32 eventId = _events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_DARNAVAN_BLADESTORM:
                            DoCast(SPELL_BLADESTORM);
                            _events.ScheduleEvent(EVENT_DARNAVAN_BLADESTORM, urand(90000, 100000));
                            break;
                        case EVENT_DARNAVAN_CHARGE:
                            _canCharge = true;
                            break;
                        case EVENT_DARNAVAN_INTIMIDATING_SHOUT:
                            DoCast(SPELL_INTIMIDATING_SHOUT);
                            _events.ScheduleEvent(EVENT_DARNAVAN_INTIMIDATING_SHOUT, urand(90000, 120000));
                            break;
                        case EVENT_DARNAVAN_MORTAL_STRIKE:
                            DoCastVictim(SPELL_MORTAL_STRIKE);
                            _events.ScheduleEvent(EVENT_DARNAVAN_MORTAL_STRIKE, urand(15000, 30000));
                            break;
                        case EVENT_DARNAVAN_SHATTERING_THROW:
                            _canShatter = true;
                            break;
                        case EVENT_DARNAVAN_SUNDER_ARMOR:
                            DoCastVictim(SPELL_SUNDER_ARMOR);
                            _events.ScheduleEvent(EVENT_DARNAVAN_SUNDER_ARMOR, urand(3000, 7000));
                            break;
                    }
                }

                DoMeleeAttackIfReady();
            }

        private:
            EventMap _events;
            bool _canCharge;
            bool _canShatter;
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return GetIcecrownCitadelAI<npc_darnavanAI>(creature);
        }
};

class spell_deathwhisper_mana_barrier : public SpellScriptLoader
{
    public:
        spell_deathwhisper_mana_barrier() : SpellScriptLoader("spell_deathwhisper_mana_barrier") { }

        class spell_deathwhisper_mana_barrier_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_deathwhisper_mana_barrier_AuraScript);

            void HandlePeriodicTick(AuraEffect const* /*aurEff*/)
            {
                PreventDefaultAction();
                if (Unit* caster = GetCaster())
                {
                    int32 missingHealth = int32(caster->GetMaxHealth() - caster->GetHealth());
                    caster->ModifyHealth(missingHealth);
                    caster->ModifyPower(POWER_MANA, -missingHealth);
                }
            }

            void Register() override
            {
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_deathwhisper_mana_barrier_AuraScript::HandlePeriodicTick, EFFECT_0, SPELL_AURA_PERIODIC_TRIGGER_SPELL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_deathwhisper_mana_barrier_AuraScript();
        }
};

class at_lady_deathwhisper_entrance : public AreaTriggerScript
{
    public:
        at_lady_deathwhisper_entrance() : AreaTriggerScript("at_lady_deathwhisper_entrance") { }

        bool OnTrigger(Player* player, AreaTriggerEntry const* /*areaTrigger*/) override
        {
            if (InstanceScript* instance = player->GetInstanceScript())
                if (Creature* ladyDeathwhisper = ObjectAccessor::GetCreature(*player, instance->GetData64(DATA_LADY_DEATHWHISPER)))
                    ladyDeathwhisper->AI()->DoAction(ACTION_START_INTRO);

            return true;
        }
};

void AddSC_boss_lady_deathwhisper()
{
    new boss_lady_deathwhisper();
    new npc_cult_fanatic();
    new npc_cult_adherent();
    new npc_vengeful_shade();
    new npc_darnavan();
    new spell_deathwhisper_mana_barrier();
	new at_lady_deathwhisper_entrance();
}