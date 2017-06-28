#include "Log.h"
#include "Group.h"
#include "ObjectMgr.h"
#include "Opcodes.h"
#include "Player.h"
#include "SpellMgr.h"
#include "SpellInfo.h"
#include "WorldPacket.h"
#include "ScriptPCH.h"
#include "ChannelMgr.h"
#include "Channel.h"
#include "Language.h"

class System_OnKill : public PlayerScript
{
	public:
		System_OnKill() : PlayerScript("System_OnKill") {}

		void OnPVPKill(Player * Killer, Player * Victim)
		{
			uint64 KillerGUID = Killer->GetGUID();
			uint64 VictimGUID = Victim->GetGUID();
			std::ostringstream ss;
			struct KillStreak_Info{uint32 killstreak; uint64 lastkill;};
			static std::map<uint64, KillStreak_Info> KillStreakData;
			//Double Kill Check
			if (KillerGUID == VictimGUID || KillStreakData[KillerGUID].lastkill == VictimGUID) 
				return;

			if (!Victim->InBattleground())
			{
				KillStreakData[KillerGUID].killstreak = 0;
				return;
			}

			if (Victim->InArena())
			{
				KillStreakData[KillerGUID].killstreak = 0;
				return; 
			}
			
			//Update Information
			KillStreakData[KillerGUID].killstreak++; 
			KillStreakData[KillerGUID].lastkill = VictimGUID;
			KillStreakData[VictimGUID].killstreak = 0; 
			KillStreakData[VictimGUID].lastkill = 0;

			ChatHandler(Killer->GetSession()).PSendSysMessage(LANG_ONKILL_SYSTEM_1, KillStreakData[KillerGUID].killstreak);

			if (KillStreakData[KillerGUID].killstreak > 50)
				KillStreakData[KillerGUID].killstreak = 60;

			//Killstreak Check
			switch (KillStreakData[KillerGUID].killstreak)
			{
				case 5: 
					if (Group* group = Killer->GetGroup())
						for (GroupReference* itr = group->GetFirstMember(); itr != NULL; itr = itr->next())
							if(Player* target = itr->GetSource())
								if (Killer->GetZoneId() == target->GetZoneId() && target->getLevel() >= 80)
								{
									target->AddItem(7168, 1);
									ChatHandler(target->GetSession()).PSendSysMessage(LANG_ONKILL_SYSTEM_2, Killer->GetName().c_str(),KillStreakData[KillerGUID].killstreak);
								}
					break;
				case 10: 
					if (Group* group = Killer->GetGroup())
						for (GroupReference* itr = group->GetFirstMember(); itr != NULL; itr = itr->next())
							if(Player* target = itr->GetSource())
								if (Killer->GetZoneId() == target->GetZoneId() && target->getLevel() >= 80)
								{
									target->AddItem(7168, 1);
									ChatHandler(target->GetSession()).PSendSysMessage(LANG_ONKILL_SYSTEM_2, Killer->GetName().c_str(),KillStreakData[KillerGUID].killstreak);
								}
					break;
				case 20: 
					if (Group* group = Killer->GetGroup())
						for (GroupReference* itr = group->GetFirstMember(); itr != NULL; itr = itr->next())
							if(Player* target = itr->GetSource())
								if (Killer->GetZoneId() == target->GetZoneId() && target->getLevel() >= 80)
								{
									target->AddItem(7168, 2);
									ChatHandler(target->GetSession()).PSendSysMessage(LANG_ONKILL_SYSTEM_2, Killer->GetName().c_str(),KillStreakData[KillerGUID].killstreak);
								}
					break;
				case 30: 
					if (Group* group = Killer->GetGroup())
						for (GroupReference* itr = group->GetFirstMember(); itr != NULL; itr = itr->next())
							if(Player* target = itr->GetSource())
								if (Killer->GetZoneId() == target->GetZoneId() && target->getLevel() >= 80)
								{
									target->AddItem(7168, 3);
									ChatHandler(target->GetSession()).PSendSysMessage(LANG_ONKILL_SYSTEM_2, Killer->GetName().c_str(),KillStreakData[KillerGUID].killstreak);
								}
					break;
				case 40: 
					if (Group* group = Killer->GetGroup())
						for (GroupReference* itr = group->GetFirstMember(); itr != NULL; itr = itr->next())
							if(Player* target = itr->GetSource())
								if (Killer->GetZoneId() == target->GetZoneId() && target->getLevel() >= 80)
								{
									target->AddItem(7168, 4);
									ChatHandler(target->GetSession()).PSendSysMessage(LANG_ONKILL_SYSTEM_2, Killer->GetName().c_str(),KillStreakData[KillerGUID].killstreak);
								}
					break;
				case 50:
					if (Group* group = Killer->GetGroup())
						for (GroupReference* itr = group->GetFirstMember(); itr != NULL; itr = itr->next())
							if(Player* target = itr->GetSource())
								if (Killer->GetZoneId() == target->GetZoneId() && target->getLevel() >= 80)
								{
									target->AddItem(7168, 5);
									ChatHandler(target->GetSession()).PSendSysMessage(LANG_ONKILL_SYSTEM_2, Killer->GetName().c_str(),KillStreakData[KillerGUID].killstreak);
								}
					//Server Broadcast
					ss << "|cFFFF0000[전장알림]|cFF81CF42[" << Killer->GetName() << "]|r 연속 50킬!";
					sWorld->SendServerMessage(SERVER_MSG_STRING, ss.str().c_str());
					break;
				case 60:
					if (Group* group = Killer->GetGroup())
						for (GroupReference* itr = group->GetFirstMember(); itr != NULL; itr = itr->next())
							if(Player* target = itr->GetSource())
								if (Killer->GetZoneId() == target->GetZoneId() && target->getLevel() >= 80)
									target->AddItem(7168, 1);
					break;
				default :
					break;
			}

			if (urand(0, 99) < 5)
			{
				uint32 rewarditem;
				switch (rand()%3)
				{
					case 0:
						rewarditem = 20559;
						break;
					case 1:
						rewarditem = 29024;
						break;
					case 2:
						rewarditem = 42425;
						break;
					case 3:
						rewarditem = 20558;
						break;
					default:
						break;
				}
				Killer->AddItem(rewarditem, 1);
			}
		}
};

enum eRanks
{
  /* Alliance */
	PRIVATE = 1,
	CORPORAL = 2,
	SERGEANT = 3,
	MASTER_SERGEANT = 4,
	SERGEANT_MAJOR = 5,
	KNIGHT = 6,
	KNIGHT_LIEUTENANT = 7,
	KNIGHT_CAPTAIN = 8,
	KNIGHT_CHAMPION = 9,
	LIEUTENANT_COMMANDER = 10,
	COMMANDER = 11,
	MARSHAL = 12,
	FIELD_MARSHAL = 13,
	GRAND_MARSHAL = 14,
	OF_THE_ALLIANCE = 126,
  /* Horde */
    SCOUT = 15,
	GRUNT = 16,
	SERGEANT_H = 17,
	SENIOR_SERGEANT = 18,
	FIRST_SERGEANT = 19,
	STONE_GUARD = 20,
	BLOOD_GUARD = 21,
	LEGIONNAIRE = 22,
	CENTURION = 23,
	CHAMPION = 24,
	LIEUTENANT_GENERAL = 25,
	GENERAL = 26,
	WARLORD = 27,
	HIGH_WARLORD = 28,
	OF_THE_HORDE = 127
};

enum eKills
{
	KILLS_1 = 100,
	KILLS_2 = 500,
	KILLS_3 = 1000,
	KILLS_4 = 2000,
	KILLS_5 = 4000,
	KILLS_6 = 5000,
	KILLS_7 = 6000,
	KILLS_8 = 8000,
	KILLS_9 = 10000,
	KILLS_10 = 15000,
	KILLS_11 = 25000,
	KILLS_12 = 40000,
	KILLS_13 = 45000,
	KILLS_14 = 50000,
	KILLS_15 = 75000
};

class PvPTitles : public PlayerScript
{
     public:
		 PvPTitles()
			 : PlayerScript("PvPTitles")
		 {
		 }

		 void OnPVPKill(Player * killer, Player * killed)
		 {
			 if(killer == NULL)
				 return;

			 if(killer->getLevel() > killed->getLevel())
				 return;
			
			if (killer->GetUInt32Value(PLAYER_FIELD_LIFETIME_HONORABLE_KILLS)  >= 100)
			{
				 if(killer->GetOTeam() == ALLIANCE)
						killer->SetTitle(sCharTitlesStore.LookupEntry(PRIVATE));
					 else if(killer->GetOTeam() == HORDE)
						killer->SetTitle(sCharTitlesStore.LookupEntry(SCOUT));
			}

			if (killer->GetUInt32Value(PLAYER_FIELD_LIFETIME_HONORABLE_KILLS)  >= 500)
			{
				 if(killer->GetOTeam() == ALLIANCE)
						killer->SetTitle(sCharTitlesStore.LookupEntry(CORPORAL));
					 else if(killer->GetOTeam() == HORDE)
						killer->SetTitle(sCharTitlesStore.LookupEntry(GRUNT));
			}

			if (killer->GetUInt32Value(PLAYER_FIELD_LIFETIME_HONORABLE_KILLS)  >= 1000)
			{
				 if(killer->GetOTeam() == ALLIANCE)
				  killer->SetTitle(sCharTitlesStore.LookupEntry(SERGEANT));
				 else if(killer->GetOTeam() == HORDE)
				  killer->SetTitle(sCharTitlesStore.LookupEntry(SERGEANT_H));
			}

			 if (killer->GetUInt32Value(PLAYER_FIELD_LIFETIME_HONORABLE_KILLS)  >= 2000)
			{
				 if(killer->GetOTeam() == ALLIANCE)
				  killer->SetTitle(sCharTitlesStore.LookupEntry(MASTER_SERGEANT));
				 else if(killer->GetOTeam() == HORDE)
				  killer->SetTitle(sCharTitlesStore.LookupEntry(SENIOR_SERGEANT));
			}

			 if (killer->GetUInt32Value(PLAYER_FIELD_LIFETIME_HONORABLE_KILLS)  >= 4000)
			{
				 if(killer->GetOTeam() == ALLIANCE)
				  killer->SetTitle(sCharTitlesStore.LookupEntry(SERGEANT_MAJOR));
				 else if(killer->GetOTeam() == HORDE)
				  killer->SetTitle(sCharTitlesStore.LookupEntry(FIRST_SERGEANT));
			}

			 if (killer->GetUInt32Value(PLAYER_FIELD_LIFETIME_HONORABLE_KILLS)  >= 5000)
			{
				 if(killer->GetOTeam() == ALLIANCE)
					 killer->SetTitle(sCharTitlesStore.LookupEntry(KNIGHT));
				 else if(killer->GetOTeam() == HORDE)
				  killer->SetTitle(sCharTitlesStore.LookupEntry(STONE_GUARD));
			}

			 if (killer->GetUInt32Value(PLAYER_FIELD_LIFETIME_HONORABLE_KILLS)  >= 6000)
			{
				 if(killer->GetOTeam() == ALLIANCE)
					 killer->SetTitle(sCharTitlesStore.LookupEntry(KNIGHT_LIEUTENANT));
				 else if(killer->GetOTeam() == HORDE)
				  killer->SetTitle(sCharTitlesStore.LookupEntry(BLOOD_GUARD));
			}

			 if (killer->GetUInt32Value(PLAYER_FIELD_LIFETIME_HONORABLE_KILLS)  >= 8000)
			{
				 if(killer->GetOTeam() == ALLIANCE)
				  killer->SetTitle(sCharTitlesStore.LookupEntry(KNIGHT_CAPTAIN));
				 else if(killer->GetOTeam() == HORDE)
				  killer->SetTitle(sCharTitlesStore.LookupEntry(LEGIONNAIRE));
			}

			 if (killer->GetUInt32Value(PLAYER_FIELD_LIFETIME_HONORABLE_KILLS)  >= 10000)
			{
				 if(killer->GetOTeam() == ALLIANCE)
				  killer->SetTitle(sCharTitlesStore.LookupEntry(KNIGHT_CHAMPION));
				 else if(killer->GetOTeam() == HORDE)
				  killer->SetTitle(sCharTitlesStore.LookupEntry(CENTURION));
			}

			 if (killer->GetUInt32Value(PLAYER_FIELD_LIFETIME_HONORABLE_KILLS)  >= 15000)
			{
				 if(killer->GetOTeam() == ALLIANCE)
					 killer->SetTitle(sCharTitlesStore.LookupEntry(LIEUTENANT_COMMANDER));
				 else if(killer->GetOTeam() == HORDE)
				  killer->SetTitle(sCharTitlesStore.LookupEntry(CHAMPION));
			}

			 if (killer->GetUInt32Value(PLAYER_FIELD_LIFETIME_HONORABLE_KILLS)  >= 25000)
			{
				 if(killer->GetOTeam() == ALLIANCE)
				  killer->SetTitle(sCharTitlesStore.LookupEntry(COMMANDER));
				 else if(killer->GetOTeam() == HORDE)
				  killer->SetTitle(sCharTitlesStore.LookupEntry(LIEUTENANT_GENERAL));
			}

			 if (killer->GetUInt32Value(PLAYER_FIELD_LIFETIME_HONORABLE_KILLS)  >= 40000)
			{
				 if(killer->GetOTeam() == ALLIANCE)
					 killer->SetTitle(sCharTitlesStore.LookupEntry(MARSHAL));
				 else if(killer->GetOTeam() == HORDE)
				  killer->SetTitle(sCharTitlesStore.LookupEntry(GENERAL));
			}

			 if (killer->GetUInt32Value(PLAYER_FIELD_LIFETIME_HONORABLE_KILLS)  >= 45000)
			{
				 if(killer->GetOTeam() == ALLIANCE)
				  killer->SetTitle(sCharTitlesStore.LookupEntry(FIELD_MARSHAL));
				 else if(killer->GetOTeam() == HORDE)
				  killer->SetTitle(sCharTitlesStore.LookupEntry(WARLORD));
			}

			 if (killer->GetUInt32Value(PLAYER_FIELD_LIFETIME_HONORABLE_KILLS)  >= 50000)
			{
				 if(killer->GetOTeam() == ALLIANCE)
				  killer->SetTitle(sCharTitlesStore.LookupEntry(GRAND_MARSHAL));
				 else if(killer->GetOTeam() == HORDE)
				  killer->SetTitle(sCharTitlesStore.LookupEntry(HIGH_WARLORD));
			}

			 if (killer->GetUInt32Value(PLAYER_FIELD_LIFETIME_HONORABLE_KILLS)  >= 70000)
			{
				 if(killer->GetOTeam() == ALLIANCE)
					 killer->SetTitle(sCharTitlesStore.LookupEntry(OF_THE_ALLIANCE));
				 else if(killer->GetOTeam() == HORDE)
				  killer->SetTitle(sCharTitlesStore.LookupEntry(OF_THE_HORDE));
			}
		 }
};

enum LandrosTexts
{
	SAY_WRONG = 1,
	SAY_CORRECT = 2,
	SAY_REPET = 4
};

enum LandroMenus
{
	WELCOME							=	8855,
	PROMOTION_MENU_TEXT				=	8856,
	PROMOTION						=	9197,
	HEROES_OF_AZEROTH				=	9192,
	THROUGH_THE_DARK_PORTAL			=	9194,
	FIRES_OF_OUTLAND				=	9195,
	MARCH_OF_THE_LEGION				=	9196,
	SERVANTS_OF_THE_BETRAYER		=	9191,
	HUNT_FOR_ILLIDAN				=	9629,
	DRUMS_OF_WAR					=	9682,
	BLOOD_OF_GLADIATORS				=	10330,
	FIELDS_OF_HONOR					=	10533,
	SCOURGEWAR						=	10810,
	WRATHGATE						=	11342,
	ICECROWN						=	11343,
	POINTS_REDEMPTION				=	9193
};

class landro_longshot : public CreatureScript
{
    public:

        landro_longshot(): CreatureScript("landro_longshot") { }

		int SelectedReward;

		bool OnGossipHello(Player* player, Creature* creature)
		{
			player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "뭘 취급하싶니까?", GOSSIP_SENDER_MAIN, PROMOTION);
			player->SEND_GOSSIP_MENU(WELCOME, creature->GetGUID());
			return true;
		}

		bool OnGossipSelect(Player* player, Creature* creature, uint32 sender, uint32 action)
		{
			player->PlayerTalkClass->ClearMenus();
			
			if (action != 0)
			{
				SelectedReward = action;
			}

			uint32 number = 1;


			QueryResult GetGossipFields = WorldDatabase.PQuery("SELECT option_text, action_menu_id FROM gossip_menu_option WHERE menu_id = %u", action);
			do{
				Field* fields = GetGossipFields->Fetch();

				std::string OptionText		=	fields[0].GetString();
				uint32	ActionMenuID		=	fields[1].GetUInt32();
				if (ActionMenuID != 0)
				{
					player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, OptionText, GOSSIP_SENDER_MAIN, ActionMenuID);
				}
				else
				{
					if (number == 1)
						player->ADD_GOSSIP_ITEM_EXTENDED(0, OptionText, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1, "", 0, true);

					if (number == 2)
						player->ADD_GOSSIP_ITEM_EXTENDED(0, OptionText, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 2, "", 0, true);

					if (number == 3)
						player->ADD_GOSSIP_ITEM_EXTENDED(0, OptionText, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 3, "", 0, true);
				}
				number++;
			} while (GetGossipFields->NextRow());

			player->SEND_GOSSIP_MENU(PROMOTION_MENU_TEXT, creature->GetGUID());
			return true;
		}

		bool OnGossipSelectCode(Player* player, Creature* creature, uint32 sender, uint32 action, char const* code)
		{
			player->PlayerTalkClass->ClearMenus();
			std::string codeUINT = code;
			uint32 num = 0;
			if (!*code)
				return false;

			if (action == GOSSIP_ACTION_INFO_DEF + 1)
				num = 0;

			if (action == GOSSIP_ACTION_INFO_DEF + 2)
				num = 1;

			if (action == GOSSIP_ACTION_INFO_DEF + 3)
				num = 2;

			QueryResult SearchForCode = WorldDatabase.PQuery("SELECT item FROM promotion_codes WHERE code = '%s' AND collection = %u AND used = 0 and id = %d", codeUINT.c_str(), SelectedReward, num);
			if (!SearchForCode)
			{
				QueryResult SearchForUseCode = WorldDatabase.PQuery("SELECT item FROM promotion_codes WHERE code = '%s' AND collection = %u AND used = 1 and id = %d", codeUINT.c_str(), SelectedReward, num);
				if (SearchForUseCode)
					creature->AI()->Talk(SAY_REPET);
				else
					creature->AI()->Talk(SAY_WRONG);
			}
			else
			{
				creature->AI()->Talk(SAY_CORRECT);
				do
				{
					Field *fields = SearchForCode->Fetch();
					player->AddItem(fields[0].GetUInt32(), 1);
					WorldDatabase.PQuery("Update promotion_codes SET USED = 1 WHERE code = '%s' and collection = %u and id = %d", codeUINT.c_str(), SelectedReward, num);
				} while (SearchForCode->NextRow());
			}

			player->PlayerTalkClass->SendCloseGossip();
			return true;
		}
};

class motd_message_player : public PlayerScript
{
   public:
	   motd_message_player() : PlayerScript("motd_message_player") { }

	   void OnLogin(Player* player)
		{
			if (ChannelMgr* cMgr = ChannelMgr::forTeam(player->GetTeam()))
			{
				cMgr->setTeam(player->GetTeam());
	 
				if (Channel* channel = cMgr->GetJoinChannel("빛의수호자", 0))
				{
					channel->JoinChannel(player->GetSession()->GetPlayer(), "");
					channel->Invite(player->GetSession()->GetPlayer(), player->GetSession()->GetPlayer()->GetName());
					channel->LeaveChannel(player->GetSession()->GetPlayer(), true);
				}
			}
		}

		void OnLogout(Player* player)
		{
			if (!player->IsGameMaster())
			{
				QueryResult result = CharacterDatabase.PQuery("SELECT * FROM daily_players_reports WHERE guid = %d and activ = 0",player->GetGUIDLow());
				if (result)
				{
					uint32 accId = player->GetSession()->GetAccountId();
					QueryResult result_insert = CharacterDatabase.PQuery("REPLACE INTO auth.account_banned (id,realm,bandate,unbandate,bannedby,banreason,active) values (%d,0,0,0,'server-script','Server-AutoScript : Movement hack use',1);",accId);
					QueryResult result_update = CharacterDatabase.PQuery("UPDATE daily_players_reports set activ = 1 WHERE guid = %d",player->GetGUIDLow());
				}
			}
		}
};

class exchange_statement : public CreatureScript
{
    public:

        exchange_statement(): CreatureScript("exchange_statement") { }

		bool OnGossipHello(Player* player, Creature* creature)
		{
			player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "빛의 문장을 다른 문장,휘장으로 교환가능합니다.", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF);
			if (player->GetTeam() == ALLIANCE)
				player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "빛의 문장을 결전의두루마리II로 교환가능합니다.", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 10);
			else
				player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "빛의 문장을 결전의두루마리II로 교환가능합니다.", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 20);

			player->SEND_GOSSIP_MENU(100000, creature->GetGUID());
			return true;
		}

		bool OnGossipSelect(Player* player, Creature* creature, uint32 sender, uint32 action)
		{
			player->PlayerTalkClass->ClearMenus();
			
			if (action == GOSSIP_ACTION_INFO_DEF)
				player->ADD_GOSSIP_ITEM_EXTENDED(0, "정의의 휘장이 필요합니다", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1, "", 0, true);

			if (action == GOSSIP_ACTION_INFO_DEF + 10)
				player->ADD_GOSSIP_ITEM_EXTENDED(0, "빛의 문장 1개 필요합니다", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 11, "", 0, true);

			if (action == GOSSIP_ACTION_INFO_DEF + 20)
				player->ADD_GOSSIP_ITEM_EXTENDED(0, "빛의 문장 10개 필요합니다", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 21, "", 0, true);

			player->SEND_GOSSIP_MENU(100001, creature->GetGUID());
			return true;
		}

		bool OnGossipSelectCode(Player* player, Creature* creature, uint32 sender, uint32 action, char const* code)
		{
			player->PlayerTalkClass->ClearMenus();
			if (!*code)
				return false;

			std::string codeUINT = code;

			uint32 value = atoi(codeUINT.c_str());
			uint32 money = (value * 200000.0f);

			if (player->GetMoney() < money)
			{
				player->GetSession()->SendNotification("골드가 부족합니다.");
				player->PlayerTalkClass->SendCloseGossip();
				return false;
			}

			if (action == GOSSIP_ACTION_INFO_DEF + 1)
				if(player->HasItemCount(7168, value))
				{
					player->ModifyMoney(-money);
					player->DestroyItemCount(7168, value, true);
					player->AddItem(29434, value);
					player->GetSession()->SendNotification("교환되었습니다.");
				}
				else
					player->GetSession()->SendNotification("빛의 문장이 부족합니다.");

			if (action == GOSSIP_ACTION_INFO_DEF + 11)
				if (value > 5)
				{
					player->GetSession()->SendNotification("해당아이템은 소지한도가 5개입니다.");
				}
				else
				{
					if(player->HasItemCount(7168, value))
					{
						player->DestroyItemCount(7168, value, true);
						player->AddItem(875, value);
						player->GetSession()->SendNotification("교환되었습니다.");
					}
					else
						player->GetSession()->SendNotification("빛의 문장이 부족합니다.");
				}

			if (action == GOSSIP_ACTION_INFO_DEF + 21)
				if (value > 5)
				{
					player->GetSession()->SendNotification("해당아이템은 소지한도가 5개입니다.");
				}
				else
				{
					 uint32 hascount = value * 10;
					if(player->HasItemCount(7168, hascount))
					{
						player->DestroyItemCount(7168, hascount, true);
						player->AddItem(5041, value);
						player->GetSession()->SendNotification("교환되었습니다.");
					}
					else
						player->GetSession()->SendNotification("빛의 문장이 부족합니다.");
				}
					

			player->PlayerTalkClass->SendCloseGossip();
			return true;
		}
};


//Vyran, The Nature Caller by Mi'rael//
enum Spells
{
	SPELL_FIREBOLT = 20801,
	SPELL_CHAINLIGHTNING = 39945,
	SPELL_ENGRAGE = 8599,
	SPELL_SCREAM = 13704,
	SPELL_ACID = 33551,
	SPELL_SOUL = 32057,
	SPELL_CLEAVE = 31345
};

class boss_vyran : public CreatureScript
{
public:

	boss_vyran()
		: CreatureScript("boss_vyran"){}

	struct boss_vyranAI : public ScriptedAI
	{
		boss_vyranAI(Creature * c) : ScriptedAI(c){}
		uint32 FIREBOLT_Timer;
		uint32 CHAINLIGHTNING_Timer;
		uint32 ENGRAGE_Timer;
		uint32 SCREAM_Timer;
		uint32 ACID_Timer;
		uint32 SOUL_Timer;
		uint32 CLEAVE_Timer;

		void Reset()
		{
			FIREBOLT_Timer = 12000;
			CHAINLIGHTNING_Timer = 18000;
			ENGRAGE_Timer = 180000;
			SCREAM_Timer = 20000;
			ACID_Timer = 30000;
			SOUL_Timer = 15000;
			CLEAVE_Timer = 9000;
		}

		void KilledUnit(Unit * /* killed */)
		{
			me->MonsterSay("The Power of Nature...is unlimited!", LANG_UNIVERSAL, 0);
		}

		void JustDied(Unit * /* killer */)
		{
			me->MonsterYell("I can't...believe this...You won this time, but your journey is far from over!", LANG_UNIVERSAL, 0);
		}

		void EnterCombat(Unit* /* who */)
		{
			me->MonsterYell("Who are you? You've made a terrible mistake. Now you will meet your DEATH!...", LANG_UNIVERSAL, 0);
		}

		void UpdateAI(uint32 diff)
		{
			if (!UpdateVictim())
				return;

			if (FIREBOLT_Timer < diff)
			{
				DoCast(me->GetVictim(), SPELL_FIREBOLT);
				FIREBOLT_Timer = 10000;
			}
			else
				FIREBOLT_Timer -= diff;

			if (CHAINLIGHTNING_Timer < diff)
			{
				DoCast(me->GetVictim(), SPELL_CHAINLIGHTNING);
				CHAINLIGHTNING_Timer = 12000;
			}
			else
				CHAINLIGHTNING_Timer -= diff;

			if (ENGRAGE_Timer < diff)
			{
				DoCast(me->GetVictim(), SPELL_ENGRAGE);
				ENGRAGE_Timer = 180000;
			}
			else
				ENGRAGE_Timer -= diff;

			if (SCREAM_Timer < diff)
			{
				DoCast(me->GetVictim(), SPELL_SCREAM);
				SCREAM_Timer = 20000;
			}
			else
				SCREAM_Timer -= diff;

			if (ACID_Timer < diff)
			{
				DoCast(me->GetVictim(), SPELL_ACID);
				ACID_Timer = 30000;
			}
			else
				ACID_Timer -= diff;

			if (SOUL_Timer < diff)
			{
				DoCast(me->GetVictim(), SPELL_SOUL);
				SOUL_Timer = 15000;
			}
			else
				SOUL_Timer -= diff;

			if (CLEAVE_Timer < diff)
			{
				DoCast(me->GetVictim(), SPELL_CLEAVE);
				CLEAVE_Timer = 9000;
			}
			else
				CLEAVE_Timer -= diff;

			DoMeleeAttackIfReady();

		}
	};

	CreatureAI* GetAI(Creature* creature) const
	{
		return new boss_vyranAI(creature);
	}

};

void AddSC_PvP_System()
{
	new System_OnKill;
	new PvPTitles;
	new landro_longshot();
	new motd_message_player;
	new exchange_statement();

	new boss_vyran();
}