#include "ScriptPCH.h"
#include "Sweet_TeleportNPC.h"
#include "InstanceSaveMgr.h"

#define GoBack "[뒤로]"

#include "ObjectMgr.h"
#include "ScriptMgr.h"
#include "ScriptedCreature.h"

#include "DatabaseEnv.h"
#include "DatabaseWorker.h"
#include "SQLOperation.h"
#include "MySQLConnection.h"
#include "MySQLThreading.h"

#include "Common.h"
#include "ObjectMgr.h"
#include "ArenaTeamMgr.h"
#include "World.h"
#include "WorldPacket.h"

#include "ArenaTeam.h"
#include "BattlegroundMgr.h"
#include "BattlegroundAV.h"
#include "BattlegroundAB.h"
#include "BattlegroundEY.h"
#include "BattlegroundWS.h"
#include "BattlegroundNA.h"
#include "BattlegroundBE.h"
#include "BattlegroundRL.h"
#include "BattlegroundSA.h"
#include "BattlegroundDS.h"
#include "BattlegroundRV.h"
#include "BattlegroundIC.h"
#include "Chat.h"
#include "Map.h"
#include "MapInstanced.h"
#include "MapManager.h"
#include "Player.h"
#include "GameEventMgr.h"
#include "SharedDefines.h"
#include "Formulas.h"
#include "DisableMgr.h"
#include "Opcodes.h"


//DB 설정파일
#define DB_HOST "14.43.111.195"
#define DB_USER "esotar"
#define DB_PASS "sys790721"
#define DB_NAME "characters"
#define _PORT 3306
#define ACCOUNT "$account"

enum Magni
{
   SPELL_AVATAR			= 19135,
   SPELL_KNOCK_AWAY		= 20686,
   SPELL_STORM_BOLT		= 20685,
   SPELL_CHARGE			= 69069,	//뼈의방패
   SPELL_LAY_ON_HANDS		= 633,  // may the wrong spell - but works fine in this case 
   SPELL_PANIC         = 19408,
   SPELL_SHADOW_BOLT   = 69148, //볼트공격
   SPELL_BOLT          = 69633, //치유효과 감소
};


// 전문기술 엔피시
#include "ScriptPCH.h"
#include "Config.h"

class professionnpc : public CreatureScript
{
public:
    professionnpc() : CreatureScript("professionnpc") {}


bool OnGossipHello(Player* pPlayer, Creature* pCreature)
{
	if(sConfigMgr->GetBoolDefault("ProfessionNPC.OnlyGMs", false)) // If ProfessionNPC.OnlyGMs is enabled in trinitycore.conf
		if (pPlayer->GetSession()->GetSecurity() == SEC_PLAYER)

		{
			return true;
		}

	bool EnableProfessions = sConfigMgr->GetBoolDefault("ProfessionNPC.EnableProfessions", true);
	bool EnableSecondarySkills = sConfigMgr->GetBoolDefault("ProfessionNPC.EnableSecondarySkills", true);

	// Main Menu for Alliance
    if (pPlayer->GetTeam() == ALLIANCE)
	{
	// Check config if "Professions" is enabled or not
	if(EnableProfessions)
		pPlayer->ADD_GOSSIP_ITEM( 7, "전문기술 "        , GOSSIP_SENDER_MAIN, 1000);
	// Check config if "Secondary Skills" is enabled or not
	if(EnableSecondarySkills)
		pPlayer->ADD_GOSSIP_ITEM( 7, "보조기술"   , GOSSIP_SENDER_MAIN, 2000);
	}
	else // Main Menu for Horde
	{
	// Check config if "Professions" is enabled or not
	if(EnableProfessions)
		pPlayer->ADD_GOSSIP_ITEM( 7, "전문기술 "        , GOSSIP_SENDER_MAIN, 1000);
	// Check config if "Secondary Skills" is enabled or not
	if(EnableSecondarySkills)
		pPlayer->ADD_GOSSIP_ITEM( 7, "보조기술"   , GOSSIP_SENDER_MAIN, 2000);
	}

    pPlayer->SEND_GOSSIP_MENU(DEFAULT_GOSSIP_MESSAGE,pCreature->GetGUID());

return true;
}

void SendDefaultMenu(Player* pPlayer, Creature* pCreature, uint32 uiAction)
{

// Not allow in combat
if (pPlayer->IsInCombat())
{
    pPlayer->CLOSE_GOSSIP_MENU();
	return;
}

	bool EnableProfessions = sConfigMgr->GetBoolDefault("ProfessionNPC.EnableProfessions", true);
	bool EnableSecondarySkills = sConfigMgr->GetBoolDefault("ProfessionNPC.EnableSecondarySkills", true);

//Mony Check
if (pPlayer->GetMoney() < (sConfigMgr->GetFloatDefault("SkillGoldCost",0)))
{
    pPlayer->CLOSE_GOSSIP_MENU();
 //   pCreature->MonsterWhisper("골드가 부족합니다.", pPlayer->GetGUID());
	return;
}

switch(uiAction)
{

case 1000: //Profession
            pPlayer->ADD_GOSSIP_ITEM( 5, "연금술."              , GOSSIP_SENDER_MAIN, 1001);
            pPlayer->ADD_GOSSIP_ITEM( 5, "대장장이."        , GOSSIP_SENDER_MAIN, 1002);
            pPlayer->ADD_GOSSIP_ITEM( 5, "마법부여."           , GOSSIP_SENDER_MAIN, 1003);
            pPlayer->ADD_GOSSIP_ITEM( 5, "기계공학."          , GOSSIP_SENDER_MAIN, 1004);
            pPlayer->ADD_GOSSIP_ITEM( 5, "약초채집."            , GOSSIP_SENDER_MAIN, 1005);
            pPlayer->ADD_GOSSIP_ITEM( 5, "주문각인."          , GOSSIP_SENDER_MAIN, 1006);
            pPlayer->ADD_GOSSIP_ITEM( 5, "보석세공."        , GOSSIP_SENDER_MAIN, 1007);
            pPlayer->ADD_GOSSIP_ITEM( 5, "가죽세공."       , GOSSIP_SENDER_MAIN, 1008);
            pPlayer->ADD_GOSSIP_ITEM( 5, "채광."               , GOSSIP_SENDER_MAIN, 1009);
            pPlayer->ADD_GOSSIP_ITEM( 5, "무두."             , GOSSIP_SENDER_MAIN, 1010);
            pPlayer->ADD_GOSSIP_ITEM( 5, "재봉술."            , GOSSIP_SENDER_MAIN, 1011);
            pPlayer->ADD_GOSSIP_ITEM( 7, "<- 뒤로"          , GOSSIP_SENDER_MAIN, 3000);

	pPlayer->SEND_GOSSIP_MENU(DEFAULT_GOSSIP_MESSAGE,pCreature->GetGUID());
break;

case 2000: //Secondary Skills
            pPlayer->ADD_GOSSIP_ITEM( 5, "요리."              , GOSSIP_SENDER_MAIN, 2001);
            pPlayer->ADD_GOSSIP_ITEM( 5, "응급치료."            , GOSSIP_SENDER_MAIN, 2002);
            pPlayer->ADD_GOSSIP_ITEM( 5, "낚시."              , GOSSIP_SENDER_MAIN, 2003);
          //pPlayer->ADD_GOSSIP_ITEM( 5, "Riding."               , GOSSIP_SENDER_MAIN, 2004);
            pPlayer->ADD_GOSSIP_ITEM( 7, "<- 뒤로"          , GOSSIP_SENDER_MAIN, 3000);

	pPlayer->SEND_GOSSIP_MENU(DEFAULT_GOSSIP_MESSAGE,pCreature->GetGUID());
break;

case 3000: //Back To Main Menu
	// Main Menu for Alliance
    if (pPlayer->GetTeam() == ALLIANCE)
	{
	// Check config if "Professions" is enabled or not
	if(EnableProfessions)
		pPlayer->ADD_GOSSIP_ITEM( 7, "전문기술 "        , GOSSIP_SENDER_MAIN, 1000);
	// Check config if "Secondary Skills" is enabled or not
	if(EnableSecondarySkills)
		pPlayer->ADD_GOSSIP_ITEM( 7, "보조기술"   , GOSSIP_SENDER_MAIN, 2000);
	}
	else // Main Menu for Horde
	{
	// Check config if "Professions" is enabled or not
	if(EnableProfessions)
		pPlayer->ADD_GOSSIP_ITEM( 7, "전문기술 "        , GOSSIP_SENDER_MAIN, 1000);
	// Check config if "Secondary Skills" is enabled or not
	if(EnableSecondarySkills)
		pPlayer->ADD_GOSSIP_ITEM( 7, "보조기술"   , GOSSIP_SENDER_MAIN, 2000);
	}

	pPlayer->SEND_GOSSIP_MENU(DEFAULT_GOSSIP_MESSAGE,pCreature->GetGUID());
break;

//////////////////////////////////////////////////Professions///////////////////////////////////////////////////////////////
/*
case 1001: // Alchemy
	if(!pPlayer->UpdateSkill(171,0))
      {
		pCreature->MonsterWhisper("해당기술을 가지고 있지않습니다.. $C.", pPlayer->GetGUID());
		OnGossipHello(pPlayer, pCreature);
		return;
	}

	pPlayer->UpdateSkill(171,(sConfigMgr->GetFloatDefault("SkillPoints",0)));
	pPlayer->ModifyMoney(-(sConfigMgr->GetFloatDefault("SkillGoldCost",0)));
	pPlayer->CLOSE_GOSSIP_MENU();
break;

case 1002: // Blacksmithing
	if(!pPlayer->UpdateSkill(164,0))
      {
		pCreature->MonsterWhisper("해당기술을 가지고 있지않습니다.. $C.", pPlayer->GetGUID());
		OnGossipHello(pPlayer, pCreature);
		return;
	}

	pPlayer->UpdateSkill(164,(sConfigMgr->GetFloatDefault("SkillPoints",0)));
	pPlayer->ModifyMoney(-(sConfigMgr->GetFloatDefault("SkillGoldCost",0)));
	pPlayer->CLOSE_GOSSIP_MENU();
break;

case 1003: // Enchanting
	if(!pPlayer->UpdateSkill(333,0))
      {
		pCreature->MonsterWhisper("해당기술을 가지고 있지않습니다.. $C.", pPlayer->GetGUID());
		OnGossipHello(pPlayer, pCreature);
		return;
	}

	pPlayer->UpdateSkill(333,(sConfigMgr->GetFloatDefault("SkillPoints",0)));
	pPlayer->ModifyMoney(-(sConfigMgr->GetFloatDefault("SkillGoldCost",0)));
	pPlayer->CLOSE_GOSSIP_MENU();
break;

case 1004: // Engineering
	if(!pPlayer->UpdateSkill(202,0))
      {
		pCreature->MonsterWhisper("해당기술을 가지고 있지않습니다.. $C.", pPlayer->GetGUID());
		OnGossipHello(pPlayer, pCreature);
		return;
	}

	pPlayer->UpdateSkill(202,(sConfigMgr->GetFloatDefault("SkillPoints",0)));
	pPlayer->ModifyMoney(-(sConfigMgr->GetFloatDefault("SkillGoldCost",0)));
	pPlayer->CLOSE_GOSSIP_MENU();
break;

case 1005: // Herbalism
	if(!pPlayer->UpdateSkill(182,0))
      {
		pCreature->MonsterWhisper("해당기술을 가지고 있지않습니다.. $C.", pPlayer->GetGUID());
		OnGossipHello(pPlayer, pCreature);
		return;
	}

	pPlayer->UpdateSkill(182,(sConfigMgr->GetFloatDefault("SkillPoints",0)));
	pPlayer->ModifyMoney(-(sConfigMgr->GetFloatDefault("SkillGoldCost",0)));
	pPlayer->CLOSE_GOSSIP_MENU();
break;

case 1006: // Inscription
	if(!pPlayer->UpdateSkill(773,0))
      {
		pCreature->MonsterWhisper("해당기술을 가지고 있지않습니다.. $C.", pPlayer->GetGUID());
		OnGossipHello(pPlayer, pCreature);
		return;
	}

	pPlayer->UpdateSkill(773,(sConfigMgr->GetFloatDefault("SkillPoints",0)));
	pPlayer->ModifyMoney(-(sConfigMgr->GetFloatDefault("SkillGoldCost",0)));
	pPlayer->CLOSE_GOSSIP_MENU();
break;

case 1007: // Jewelcrafting
	if(!pPlayer->UpdateSkill(755,0))
      {
		pCreature->MonsterWhisper("해당기술을 가지고 있지않습니다.. $C.", pPlayer->GetGUID());
		OnGossipHello(pPlayer, pCreature);
		return;
	}

	pPlayer->UpdateSkill(755,(sConfigMgr->GetFloatDefault("SkillPoints",0)));
	pPlayer->ModifyMoney(-(sConfigMgr->GetFloatDefault("SkillGoldCost",0)));
	pPlayer->CLOSE_GOSSIP_MENU();
break;

case 1008: // Leatherworking
	if(!pPlayer->UpdateSkill(165,0))
      {
		pCreature->MonsterWhisper("해당기술을 가지고 있지않습니다.. $C.", pPlayer->GetGUID());
		OnGossipHello(pPlayer, pCreature);
		return;
	}

	pPlayer->UpdateSkill(165,(sConfigMgr->GetFloatDefault("SkillPoints",0)));
	pPlayer->ModifyMoney(-(sConfigMgr->GetFloatDefault("SkillGoldCost",0)));
	pPlayer->CLOSE_GOSSIP_MENU();
break;

case 1009: // Mining
	if(!pPlayer->UpdateSkill(186,0))
      {
		pCreature->MonsterWhisper("해당기술을 가지고 있지않습니다.. $C.", pPlayer->GetGUID());
		OnGossipHello(pPlayer, pCreature);
		return;
	}

	pPlayer->UpdateSkill(186,(sConfigMgr->GetFloatDefault("SkillPoints",0)));
	pPlayer->ModifyMoney(-(sConfigMgr->GetFloatDefault("SkillGoldCost",0)));
	pPlayer->CLOSE_GOSSIP_MENU();
break;

case 1010: // Skinning
	if(!pPlayer->UpdateSkill(393,0))
      {
		pCreature->MonsterWhisper("해당기술을 가지고 있지않습니다.. $C.", pPlayer->GetGUID());
		OnGossipHello(pPlayer, pCreature);
		return;
	}

	pPlayer->UpdateSkill(393,(sConfigMgr->GetFloatDefault("SkillPoints",0)));
	pPlayer->ModifyMoney(-(sConfigMgr->GetFloatDefault("SkillGoldCost",0)));
	pPlayer->CLOSE_GOSSIP_MENU();
break;

case 1011: // Tailoring
	if(!pPlayer->UpdateSkill(197,0))
      {
		pCreature->MonsterWhisper("해당기술을 가지고 있지않습니다.. $C.", pPlayer->GetGUID());
		OnGossipHello(pPlayer, pCreature);
		return;
	}

	pPlayer->UpdateSkill(197,(sConfigMgr->GetFloatDefault("SkillPoints",0)));
	pPlayer->ModifyMoney(-(sConfigMgr->GetFloatDefault("SkillGoldCost",0)));
	pPlayer->CLOSE_GOSSIP_MENU();
break;

//////////////////////////////////////////////////Secondary Skills///////////////////////////////////////////////////////////////

case 2001: // Cooking
	if(!pPlayer->UpdateSkill(185,0))
      {
		pCreature->MonsterWhisper("해당기술을 가지고 있지않습니다.. $C.", pPlayer->GetGUID());
		OnGossipHello(pPlayer, pCreature);
		return;
	}

	pPlayer->UpdateSkill(185,(sConfigMgr->GetFloatDefault("SkillPoints",0)));
	pPlayer->ModifyMoney(-(sConfigMgr->GetFloatDefault("SkillGoldCost",0)));
	pPlayer->CLOSE_GOSSIP_MENU();
break;

case 2002: // First Aid
	if(!pPlayer->UpdateSkill(129,0))
      {
		pCreature->MonsterWhisper("해당기술을 가지고 있지않습니다.. $C.", pPlayer->GetGUID());
		OnGossipHello(pPlayer, pCreature);
		return;
	}

	pPlayer->UpdateSkill(129,(sConfigMgr->GetFloatDefault("SkillPoints",0)));
	pPlayer->ModifyMoney(-(sConfigMgr->GetFloatDefault("SkillGoldCost",0)));
	pPlayer->CLOSE_GOSSIP_MENU();
break;

case 2003: // Fishing
	if(!pPlayer->UpdateSkill(356,0))
      {
		pCreature->MonsterWhisper("해당기술을 가지고 있지않습니다.. $C.", pPlayer->GetGUID());
		OnGossipHello(pPlayer, pCreature);
		return;
	}

	pPlayer->UpdateSkill(356,(sConfigMgr->GetFloatDefault("SkillPoints",0)));
	pPlayer->ModifyMoney(-(sConfigMgr->GetFloatDefault("SkillGoldCost",0)));
	pPlayer->CLOSE_GOSSIP_MENU();
break;

//case 2004: // Riding
//	if(!pPlayer->UpdateSkill(,0))
//    {
//		pCreature->MonsterWhisper("해당기술을 가지고 있지않습니다.. $C.", pPlayer->GetGUID());
//		OnGossipHello(pPlayer, pCreature);
//		return;
//	}
//
//	pPlayer->UpdateSkill(,(sConfigMgr->GetFloatDefault("SkillPoints",0)));
//	pPlayer->ModifyMoney(-(sConfigMgr->GetFloatDefault("SkillGoldCost",0)));
//	pPlayer->CLOSE_GOSSIP_MENU();
//break;
*/
 pPlayer->CLOSE_GOSSIP_MENU();

} // end of switch
} //end of function

bool OnGossipSelect(Player* pPlayer, Creature* pCreature, uint32 uiSender, uint32 uiAction)
{
	// Main menu
	pPlayer->PlayerTalkClass->ClearMenus();
	if (uiSender == GOSSIP_SENDER_MAIN)
	SendDefaultMenu(pPlayer, pCreature, uiAction);

return true;
}
};

//레벨엔피씨
#include "ScriptPCH.h"
#include "Config.h"

#define GOSSIP_SENDER_MAIN      1000
#define SPELL_RESURRECTION_SICKNESS_15007  15007

class levelnpc : public CreatureScript
{
public:
    levelnpc() : CreatureScript("levelnpc") {}


bool OnGossipHello(Player* pPlayer, Creature* pCreature)
{
/*	if(sWorld->getBoolConfig(CONFIG_LEVELNPC_ONLYGMS)) // If LevelNPC.OnlyGMs is enabled in trinitycore.conf
		if (pPlayer->GetSession()->GetSecurity() == SEC_PLAYER)
		{
			pCreature->MonsterWhisper("Sorry ich kann nur EliteAccounts Level hinzuf?en.", pPlayer->GetGUID());
			return true;
		}

	bool EnableLevel80 = sWorld->getBoolConfig(CONFIG_LEVELNPC_ENABLELEVEL80);
	bool EnableLevel100 = sWorld->getBoolConfig(CONFIG_LEVELNPC_ENABLELEVEL100);
	bool EnableLevel150 = sWorld->getBoolConfig(CONFIG_LEVELNPC_ENABLELEVEL150);
	bool EnableLevel200 = sWorld->getBoolConfig(CONFIG_LEVELNPC_ENABLELEVEL200);
	bool EnableLevel255 = sWorld->getBoolConfig(CONFIG_LEVELNPC_ENABLELEVEL255);
	bool UseTokens 		= sWorld->getBoolConfig(CONFIG_LEVELNPC_ENABLELEVEL255);	
*/
    if (pPlayer->GetTeam() == ALLIANCE)
	{
		pPlayer->ADD_GOSSIP_ITEM( 7, "Instant Levels ->"        , GOSSIP_SENDER_MAIN, 1000);
		pPlayer->ADD_GOSSIP_ITEM( 10, "Todenachwirkungen entfernen" , GOSSIP_SENDER_MAIN, 5000);
	}
	else
	{
		pPlayer->ADD_GOSSIP_ITEM( 7, "Instant Levels ->"        , GOSSIP_SENDER_MAIN, 1000);
		pPlayer->ADD_GOSSIP_ITEM( 10, "Todenachwirkungen entfernen" , GOSSIP_SENDER_MAIN, 5000);
	}
    pPlayer->SEND_GOSSIP_MENU(DEFAULT_GOSSIP_MESSAGE,pCreature->GetGUID());

return true;
}

void SendDefaultMenu(Player* pPlayer, Creature* pCreature, uint32 uiAction)
{

// Not allow in combat
if (pPlayer->IsInCombat())
{
    pPlayer->CLOSE_GOSSIP_MENU();
	return;
}
/*
	bool EnableLevel80 = sWorld->getBoolConfig(CONFIG_LEVELNPC_ENABLELEVEL80);
	bool EnableLevel100 = sWorld->getBoolConfig(CONFIG_LEVELNPC_ENABLELEVEL100);
	bool EnableLevel150 = sWorld->getBoolConfig(CONFIG_LEVELNPC_ENABLELEVEL150);
	bool EnableLevel200 = sWorld->getBoolConfig(CONFIG_LEVELNPC_ENABLELEVEL200);
	bool EnableLevel255 = sWorld->getBoolConfig(CONFIG_LEVELNPC_ENABLELEVEL255);
	bool UseTokens 		= sWorld->getBoolConfig(CONFIG_LEVELNPC_ENABLELEVEL255);	
	
//Mony Check
if(sWorld->getBoolConfig(CONFIG_LEVELNPC_USETOKENS))
{
if (!pPlayer->HasItemCount((sWorld->getIntConfig(CONFIG_LEVELNPC_ITEMENTRYNUM)), 0))
{
    pPlayer->CLOSE_GOSSIP_MENU();
    pCreature->MonsterWhisper("Du hast keine Tokens.", pPlayer->GetGUID());
	return;
}
}
else if(pPlayer->GetMoney() < (sWorld->getIntConfig(CONFIG_LEVELNPC_SKILLGOLDCOST)))
{
    pPlayer->CLOSE_GOSSIP_MENU();
    pCreature->MonsterWhisper("Du hast nicht genug Gold.", pPlayer->GetGUID());
	return;
}
*/
switch(uiAction)
{

//////////////////////////////////////////////////Leveling///////////////////////////////////////////////////////////////
case 1000: //Leveling
	if(pPlayer->getLevel() < 80 )
		pPlayer->ADD_GOSSIP_ITEM( 7, "(1 Donation Chip) Instant 80 ->"         , GOSSIP_SENDER_MAIN, 1001);
/*
	if(EnableLevel80 && pPlayer->getLevel() < 80 && !UseTokens)
		pPlayer->ADD_GOSSIP_ITEM( 7, "(100 Gold) Instant 80 ->"         , GOSSIP_SENDER_MAIN, 1001);	
	if(EnableLevel100 && pPlayer->getLevel() < 100 && pPlayer->getLevel() >= 80 && UseTokens)
		pPlayer->ADD_GOSSIP_ITEM( 7, "(2 Donation Chips) Instant 100 ->"        , GOSSIP_SENDER_MAIN, 1002);
	if(EnableLevel100 && pPlayer->getLevel() < 100 && pPlayer->getLevel() >= 80 && !UseTokens)
		pPlayer->ADD_GOSSIP_ITEM( 7, "(2 Donation Chips) Instant 100 ->"        , GOSSIP_SENDER_MAIN, 1002);
	if(EnableLevel150 && pPlayer->getLevel() < 150 && pPlayer->getLevel() >= 100 && UseTokens)
		pPlayer->ADD_GOSSIP_ITEM( 7, "(5 Donation Chips) Instant 150 ->"        , GOSSIP_SENDER_MAIN, 1003);
	if(EnableLevel150 && pPlayer->getLevel() < 150 && pPlayer->getLevel() >= 100 && !UseTokens)
		pPlayer->ADD_GOSSIP_ITEM( 7, "(5 Donation Chips) Instant 150 ->"        , GOSSIP_SENDER_MAIN, 1003);
	if(EnableLevel200 && pPlayer->getLevel() < 200 && pPlayer->getLevel() >= 150 && UseTokens)
		pPlayer->ADD_GOSSIP_ITEM( 7, "(10 Donation Chips) Instant 200 ->"       , GOSSIP_SENDER_MAIN, 1004);
	if(EnableLevel200 && pPlayer->getLevel() < 200 && pPlayer->getLevel() >= 150 && !UseTokens)
		pPlayer->ADD_GOSSIP_ITEM( 7, "(10 Donation Chips) Instant 200 ->"       , GOSSIP_SENDER_MAIN, 1004);
	if(EnableLevel255 && pPlayer->getLevel() < 255 && pPlayer->getLevel() >= 200 && UseTokens)
		pPlayer->ADD_GOSSIP_ITEM( 7, "(20 Donation Chips) Instant 255 ->"       , GOSSIP_SENDER_MAIN, 1005);
	if(EnableLevel255 && pPlayer->getLevel() < 255 && pPlayer->getLevel() >= 200 && !UseTokens)
		pPlayer->ADD_GOSSIP_ITEM( 7, "(20 Donation Chips) Instant 255 ->"       , GOSSIP_SENDER_MAIN, 1005);		
		pPlayer->ADD_GOSSIP_ITEM( 7, "<- Main Menu"                       , GOSSIP_SENDER_MAIN, 3000);
	pPlayer->SEND_GOSSIP_MENU(DEFAULT_GOSSIP_MESSAGE,pCreature->GetGUID());
*/
break;
case 3000: //Leveling
	if(pPlayer->getLevel() < 80)
		pPlayer->ADD_GOSSIP_ITEM( 7, "Instant Levels ->"        , GOSSIP_SENDER_MAIN, 1000);
    pPlayer->SEND_GOSSIP_MENU(DEFAULT_GOSSIP_MESSAGE,pCreature->GetGUID());
break;

case 1001: // Leveling
	pPlayer->GiveLevel(80);
/*	if(sWorld->getBoolConfig(CONFIG_LEVELNPC_USETOKENS))
	pPlayer->DestroyItemCount(99998, 1, true);
	else
	pPlayer->ModifyMoney(-(sWorld->getIntConfig(CONFIG_LEVELNPC_SKILLGOLDCOST)));
	pPlayer->CLOSE_GOSSIP_MENU();
*/
break;
/*
case 1002: // Leveling
	pPlayer->GiveLevel(100);
	if(sWorld->getBoolConfig(CONFIG_LEVELNPC_USETOKENS))
	pPlayer->DestroyItemCount(99998, 2, true);
	else
	pPlayer->ModifyMoney(-(sWorld->getIntConfig(CONFIG_LEVELNPC_SKILLGOLDCOST)));
	pPlayer->CLOSE_GOSSIP_MENU();
break;
case 1003: // Leveling
	pPlayer->GiveLevel(150);
	if(sWorld->getBoolConfig(CONFIG_LEVELNPC_USETOKENS))
	pPlayer->DestroyItemCount(99998, 5, true);
	else
	pPlayer->ModifyMoney(-(sWorld->getIntConfig(CONFIG_LEVELNPC_SKILLGOLDCOST)));
	pPlayer->CLOSE_GOSSIP_MENU();
break;
case 1004: // Leveling
	pPlayer->GiveLevel(200);
	if(sWorld->getBoolConfig(CONFIG_LEVELNPC_USETOKENS))
	pPlayer->DestroyItemCount(99998, 10, true);
	else
	pPlayer->ModifyMoney(-(sWorld->getIntConfig(CONFIG_LEVELNPC_SKILLGOLDCOST)));
	pPlayer->CLOSE_GOSSIP_MENU();
break;
case 1005: // Leveling
	pPlayer->GiveLevel(255);
	if(sWorld->getBoolConfig(CONFIG_LEVELNPC_USETOKENS))
	pPlayer->DestroyItemCount(99998, 20, true);
	else
	pPlayer->ModifyMoney(-(sWorld->getIntConfig(CONFIG_LEVELNPC_SKILLGOLDCOST)));
	pPlayer->CLOSE_GOSSIP_MENU();
break;
case 5000://Remove Res Sickness
	if(!pPlayer->HasAura(SPELL_RESURRECTION_SICKNESS_15007,0))
	{
		pCreature->MonsterWhisper("Du hast keine Todesnachwirkungen", pPlayer->GetGUID());
		OnGossipHello(pPlayer, pCreature);
		return;
	}

	pCreature->CastSpell(pPlayer,38588,false); // Healing effect
	pPlayer->RemoveAurasDueToSpell(SPELL_RESURRECTION_SICKNESS_15007,0);
	pPlayer->CLOSE_GOSSIP_MENU();
break;
*/
 pPlayer->CLOSE_GOSSIP_MENU();

} // end of switch
} //end of function

bool OnGossipSelect(Player* pPlayer, Creature* pCreature, uint32 uiSender, uint32 uiAction)
{
	// Main menu
	pPlayer->PlayerTalkClass->ClearMenus();
	if (uiSender == GOSSIP_SENDER_MAIN)
	SendDefaultMenu(pPlayer, pCreature, uiAction);

return true;
}
};

//레벨아이템 보상
#include "ScriptPCH.h"

enum LevelRewards
{
    EMBLEM_OF_CONQUEST  = 47241
};

class level_rewards : public PlayerScript
{
public:
    level_rewards() : PlayerScript("level_rewards") {}

    void OnLevelChanged(Player* player, uint8 oldLevel)
    {
        if (player->IsGameMaster())
            return;

        uint32 money = player->GetMoney();

		if(oldLevel <= 19)
		{
			if(player->getLevel() >=20)
			{
				player->SetMoney(money + 50000);
			}
		}

/*        MailSender sender(MAIL_NORMAL, 0, MAIL_STATIONERY_GM);

        MailDraft draft("얼라이언스를 위하여", "얼라이언스의 일원이 된것을 축하드립니다.)");

        SQLTransaction trans = CharacterDatabase.BeginTransaction();

		if ( player->getLevel() >= 80 && player->GetTeam() == ALLIANCE)
		{
			uint32 accId = player->GetSession()->GetAccountId();     // get account id
			QueryResult result = CharacterDatabase.PQuery("SELECT COUNT(guid) FROM characters WHERE account = '%u' and level = '80'",accId);
			Field *fields = result->Fetch();

			uint32 count = fields[0].GetUInt32();

			if(count ==0)
			{
	            Item* item = Item::CreateItem(49426, 50, 0);
				Item* item2 = Item::CreateItem(6489, 1, 0);
		        item->SaveToDB(trans);
				item2->SaveToDB(trans);
				draft.AddItem(item);
				draft.AddItem(item2);
				draft.AddMoney(50000000);

				draft.SendMailTo(trans, MailReceiver(player), sender);
				CharacterDatabase.CommitTransaction(trans); 
			}
		}

		std::string message;
		if(player->GetTeam() == ALLIANCE)
		{
			message += "얼라이언스 연합의 \n\n ";
			message += player->GetName();
			message += "님에게 추천을 받으셨습니다.\n\n";
			message += "추천보상 엽전한냥은 우클릭으로 1도넛을\n\n획득하실수있습니다.\n\n 앞으로도 재미있는 서버생활되시기 바랍니다.";
		}else
		{
			message += "호드 연합의 \n\n ";
			message += player->GetName();
			message += "님에게 추천을 받으셨습니다.\n\n";
			message += "추천보상 엽전한냥은 우클릭으로 1도넛을\n\n획득하실수있습니다.\n\n앞으로도 재미있는 서버생활되시기 바랍니다.";
		}

        MailDraft draft1("추천 보상", message);

        SQLTransaction trans1 = CharacterDatabase.BeginTransaction();

		if ( player->getLevel() >= 80)
		{
			uint32 accId = player->GetSession()->GetAccountId();
			QueryResult result_vote_check = LoginDatabase.PQuery("SELECT vote_check,vote_account FROM account WHERE id = '%u'",accId);
			Field *fields_vote_check = result_vote_check->Fetch();

			if(fields_vote_check[0].GetUInt32() == 1)
			{
				uint64 receiverGuid = fields_vote_check[1].GetUInt64();
				Item* item = Item::CreateItem(23189, 1, 0);
				item->SaveToDB(trans1);
				draft1.AddItem(item);

				draft1.SendMailTo(trans1, MailReceiver(receiverGuid), sender);
				CharacterDatabase.CommitTransaction(trans1); 

				QueryResult result_vote_del = LoginDatabase.PQuery("UPDATE account SET vote_check = 0 WHERE id = '%u'",accId);
			}
		} */
    }
};

//판매상인
#include "ScriptPCH.h"
#include "InstanceSaveMgr.h"

#define GoBack "[뒤로]"
#define SEND_NOTIFICATION(text)				pPlayer->GetSession()->SendNotification(text)
#define GOSSIP_MENU(textID, objectGUID)			pPlayer->PlayerTalkClass->SendGossipMenu(textID, objectGUID)
#define GOSSIP_CLOSE()					pPlayer->PlayerTalkClass->CloseGossip()
#define GOSSIP_CLEAR()					pPlayer->PlayerTalkClass->ClearMenus()

enum PageSenderSell
{
	SEND_MAIN_SELL,		// Pages accessed via Main Menu
	SEND_SUPPORT_SELL,	// Character Support
	PG_SUPPORT_SELL,		// Support Page
};

// Teleport NPC GossipHello
class SellNPC : public CreatureScript
{
public:
	SellNPC() : CreatureScript("SellNPC") {}
	
	bool OnGossipHello(Player * pPlayer, Creature * pCreature)
	{
		if (pPlayer->IsInCombat() || pPlayer->HasInvisibilityAura() || pPlayer->HasStealthAura())
		{
			SEND_NOTIFICATION("당신은 전투 중입니다.");
			pPlayer->CLOSE_GOSSIP_MENU();
			return true;
		}
		pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_VENDOR, "[재료상인소환]",		SEND_MAIN_SELL, PG_SUPPORT_SELL);
		/*if (pPlayer->HasItemCount(9888, 1))
		{
			pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_VENDOR, "[재료상인소환]",		SEND_MAIN, PG_SUPPORT);  
		}
		else
			SEND_NOTIFICATION("빛의 수호자 징표가 부족합니다.");
*/
		GOSSIP_MENU(700000, pCreature->GetGUID());
		return true;
	}

	// Teleport NPC GossipSelect
	bool OnGossipSelect(Player * pPlayer, Creature * pCreature, uint32 iSender, uint32 iAction)
	{
		if (pPlayer->IsInCombat() || pPlayer->HasInvisibilityAura() || pPlayer->HasStealthAura())
		{
			SEND_NOTIFICATION("당신은 전투 중입니다.");
			pPlayer->CLOSE_GOSSIP_MENU();
			return true;
		}
		
		uint32 money = pPlayer->GetMoney();
		
		// Pages accessed via Main Menu
		if (iSender == SEND_MAIN_SELL)
		{
			GOSSIP_CLEAR();

			if (iAction == PG_SUPPORT_SELL)		// Support Page
			/* 직업 1:전사 2:기사 3:사냥꾼 4:도적 5:사제 6:죽기 7:술사 8:법사 9:흑마 11:드루 */
			{	
				pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_VENDOR, "[보석]",			SEND_SUPPORT_SELL, 0);
				pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_VENDOR, "[마법부여]",	SEND_SUPPORT_SELL, 1);
				pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_VENDOR, "[천]",	SEND_SUPPORT_SELL, 2);
				pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_VENDOR, "[제작]",			SEND_SUPPORT_SELL, 3);
				pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_VENDOR, "[원소]",	SEND_SUPPORT_SELL, 4);
				pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_VENDOR, "[금속 & 암석]",	SEND_SUPPORT_SELL, 5);
				pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_VENDOR, "[마법재료]",			SEND_SUPPORT_SELL, 6);
				pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_VENDOR, "[요리]",	SEND_SUPPORT_SELL, 7); 
				pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_VENDOR, "[가죽]",	SEND_SUPPORT_SELL, 8);
				pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_VENDOR, "[약초]",	SEND_SUPPORT_SELL, 9);
				pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_VENDOR, "[주문]",	SEND_SUPPORT_SELL, 10);
				pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_VENDOR, "[보석세공]",	SEND_SUPPORT_SELL, 11);
			}

			GOSSIP_MENU(700000, pCreature->GetGUID());
		}
		// Support
		if (iSender == SEND_SUPPORT_SELL)
		{
			switch (iAction)
			{
			case 0:	
				{
					pPlayer->SummonCreature(5000040,5674.805f,2364.345f,798.421f,4.237229f);
				}
				break;

			case 1:	
				{
					pPlayer->SummonCreature(5000031,5679.269f,2360.493f,798.463f,3.857096f);
				}
				break;

			case 2:	
				{
					pPlayer->SummonCreature(5000018,5681.230f,2354.837f,798.510f,3.236631f);
				}
				break;

			case 3:	
				{
					pPlayer->SummonCreature(5000022,5680.181f,2348.897f,798.492f,2.642085f);
				}
				break;

			case 4:	
				{
					pPlayer->SummonCreature(5000028,5676.462f,2344.571f,798.525f,2.27760f);
				}
				break;

			case 5:	
				{
					pPlayer->SummonCreature(5000060,5671.001f,2342.238f,798.404f,1.703534f);
				}
				break;

			case 6:	
				{
					pPlayer->SummonCreature(5000091,5664.6450f,2343.077f,798.295f,1.185171f);
				}
				break;

			case 7:	
				{
					pPlayer->SummonCreature(5000058,5660.011f,2347.605f,798.417f,0.626752f);
				}
				break;

			case 8:	
				{
					pPlayer->SummonCreature(5000054,5657.872f,2352.873f,798.288f,0.012571f);
				}
				break;

			case 9:	
				{
					pPlayer->SummonCreature(5000043,5658.887f,2358.130f,798.333f,5.966674f);
				}
				break;

			case 10:	
				{
					pPlayer->SummonCreature(5000048,5663.266f,2363.452f,798.437f,5.360347f);
				}
				break;

			case 11:	
				{
					pPlayer->SummonCreature(5000052,5668.519f,2365.502f,798.369f,4.923665f);
				}
				break;

			}

			pPlayer->CLOSE_GOSSIP_MENU(); // sweet
//			return OnGossipHello(pPlayer, pCreature);
			return true;
		}
		return true;
	}
};

//미니보스
/*########################################################
# Mini Bosses #
# Case di Gilda - BloodyWars #
# <https://www.bloodywars.net/> #
# <https://github.com/A-Metaphysical-Drama/BloodyCore> #
########################################################*/

/* ScriptData
SDName: BloodyWars Mini Bosses
SDAuthor: PrinceCreed
SDComment: Likantropo, Lava Boost, Quel'Nagas, Might of Sylvanas, Trollzilla, Perotharn e Uznam
SDCategory: Mini-Boss Case di Gilda
EndScriptData */

#include "ScriptPCH.h"

enum LikantropoSpells
{
    ENRAGE = 48138,
    FRENZY = 48142,
    MORTAL_WOUND = 59265,
    WORGEN_BITE = 55266,
    WORGEN_CALL = 53095,
    DECREPIT_FEVER = 54098,
    SUMMON_WORGEN = 27020,
    SPELL_AVATAR1			= 19135,
   SPELL_KNOCK_AWAY1		= 20686,
   SPELL_STORM_BOLT1		= 20685,
   SPELL_CHARGE1			= 69069,	//뼈의방패
   SPELL_LAY_ON_HANDS1		= 633,  // may the wrong spell - but works fine in this case 
   SPELL_PANIC1         = 19408,
   SPELL_SHADOW_BOLT1   = 69148, //볼트공격
   SPELL_BOLT1          = 69633, //치유효과 감소
   SPELL_BLOOD_MIRROR_DAMAGE               = 70821,
   SPELL_BLOOD_MIRROR_DUMMY                = 70838,
   SPELL_BLOODBOLT_WHIRL                   = 71772,
  SUMMON_ATT			= 530418,
  SUMMON_ATT2			= 535176,
  };

// Likantropo yells
#define SAY_LIKANTROPO_AGGRO "나의 영혼으로 난 리치왕이 될것이다.....여기가 너희들의 무덤이다..!!"
#define SAY_LIKANTROPO_SLAY "보았느냐? 나의 영혼 리치왕을....ㅋㅋ 병신들."
#define SAY_LIKANTROPO_DEATH "아오! 시바 나죽었다......노스랜드에서 보자 이놈들....."
#define SAY_LIKANTROPO_MORPH "Das werdet ihr bereuen"

enum LavaBoostSpells
{
    HEALING_WAVE = 51586,
    LIGHTNING_BOLT = 59024,
    BLOODLUST = 41185,
    LIGHTNING_SHIELD = 59025,
    SUMMON_AIR_ELEMENTAL = 30418,
    LAVA_BURST = 59519,
    CHAIN_LIGHTNING = 54531,
    FLAME_SHOCK = 43303,
    EARTH_SHOCK = 43305,
    LIGHTNING_RING = 59848
};

// Lava Boost yells
#define SAY_LAVA_AGGRO "Ihr wagt es der Wut der Elemente zu trotzen ?!"
#define SAY_LAVA_SLAY "Schockierend!"
#define SAY_LAVA_DEATH "Natur - Warum hast du mich verlassen ?"
#define SAY_LAVA_BLOOD "Bloodlust!"

enum QuelNagasSpells
{
    SPELL_MAGIC_BARRIER = 38112,
    SPELL_TOXIC_SPORES = 38575,
    TOXIC_SPORES_TRIGGER = 22207,
    SUMMONED_MURLOC = 27649,
    SPELL_POISON_BOLT_VOLLEY = 54098,
    SPELL_ENTANGLE = 38316,
    SPELL_WRATH = 20698
};

// Quel'Nagas yells
#define SAY_NAGAS_AGGRO "Ihr werdet dem Abgrund entgegenblicken!"
#define SAY_NAGAS_SLAY "Knnt ihr mithalten?!"
#define SAY_NAGAS_DEATH "Ich kann es nicht glauben!"
#define SAY_NAGAS_SUMMON "Murlocs!"

enum SylvanasSpells
{
    SPELL_MULTISHOT = 40872,
    SPELL_VOLLEY = 38633,
    SUMMONED_SNAKE = 29444,
    SPELL_BOLT_VOLLEY = 34780,
    SPELL_BEWITCHING_AURA = 29486,
    SPELL_REJUVENATION = 71142,
    SPELL_ICE_TRAP = 71249,
    SPELL_AURA_ROT = 25818
};

// Might of Sylvanas yells
#define SAY_SYLVANAS_AGGRO "Die Quel'Dorei werden niemals untergehen!"
#define SAY_SYLVANAS_SLAY "Shorel'aran!"
#define SAY_SYLVANAS_DEATH "Ich werde zurckkehren"
#define SAY_SYLVANAS_ICE "Ein wenig Tod gefllig?"

enum TrollzillaSpells
{
    SPELL_FRENZY = 28131,
    SPELL_CLEAVE = 33480,
    SPELL_FIRE_SHIELD = 30513,
    SPELL_WAR_STOMP = 56427,
    SUMMON_FIRE_ELEMENTAL = 30416
};

// Trollzilla yells
#define SAY_TROLLZILLA_AGGRO "Ich... euch... zerstren..."
#define SAY_TROLLZILLA_SLAY "Hakkar... Fr euch..."
#define SAY_TROLLZILLA_DEATH "Hakk... aaaa...rrr...."
#define SAY_TROLLZILLA_FRENZY "Ich... langsam... WTEND!"

enum PerotharnSpells
{
    STORM_OF_GRIEF = 59772,
    SHADOW_BURST = 34436,
    PIERCING_SHADOW = 36698,
    SHADOW_FLAME = 22539,
    HARVEST_SOUL = 28679,
    SHADOW_BOLT = 41957
};

// Perotharn yells
#define SAY_PEROTHARN_AGGRO "Ich werde euch als Geschenk ansehen und tten!"
#define SAY_PEROTHARN_SLAY "Bald wirst du einer von uns sein!"
#define SAY_PEROTHARN_DEATH "Meister... vergib mir..."
#define SAY_PEROTHARN_SOUL "Meister, hier kommen eure Seelen!"

enum UznamSpells
{
    EARTHQUAKE = 46240,
    WHIRLWIND = 52027,
    SHOCK_OF_SORROW = 50760,
    ENTROPIC_AURA = 36784,
    AURAL_SHOCK = 14538,
};

// Uznam yells
#define SAY_UZNAM_AGGRO "Wer entweiht mein Heiligtum?"
#define SAY_UZNAM_SLAY "Ihr whrt eine wrdige Trophe."
#define SAY_UZNAM_DEATH "Auferstanden? Mir drstet nach Blut"
#define SAY_UZNAM_SPECIAL "Ich werde etwas mde."
#define EMOTE_WHIRL "Mgen die Strme beginnen!"


/*#########
# Likantropo
#########*/

enum likantropoPhases
{
    PHASE_START                 = 1,
    PHASE_BREATH                = 2,
    PHASE_END                   = 3,
    PHASE_MODEL		=4
};

enum data
{
    BOSS_BEASTS                 = 0,
 };

class npc_likantropo : public CreatureScript
{
public:
    npc_likantropo() : CreatureScript("npc_likantropo") { }

    CreatureAI* GetAI(Creature* pCreature) const
    {
        return new npc_likantropoAI (pCreature);
    }

    bool OnGossipSelect(Player* player, Creature* creature, uint32 /*uiSender*/, uint32 uiAction)
    {
        player->PlayerTalkClass->ClearMenus();
        creature->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
	creature->setFaction(14);
        return true;
    }

bool OnGossipHello(Player* player, Creature* creature)
    {
        if (creature->IsQuestGiver())
            player->PrepareQuestMenu(creature->GetGUID());

        player->ADD_GOSSIP_ITEM( 0, "나는 노스랜드에 고립됀후 나의 영혼을 악마에게 팔았며,  나의 영혼으로 내왕국을 새롭게 건설하것이다! 너의 영혼을 나에게 바칠것텐가?", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+1);

        player->SEND_GOSSIP_MENU(DEFAULT_GOSSIP_MESSAGE, creature->GetGUID());
        return true;
    }


    struct npc_likantropoAI : public BossAI
    {
        npc_likantropoAI(Creature *pCreature) : BossAI(pCreature, BOSS_BEASTS){}

        uint32 EnrageTimer;
        uint32 WoundTimer;
        uint32 BiteTimer;
        uint32 FeverTimer;
	uint32 avatarTimer;
	uint32 knockawayTimer;
	uint32 stormboltTimer;
	uint32 chargeTimer;
	uint32 layonhandsTimer;
	uint32 panicTimer;
	uint32 shadowboltTimer;
	uint32 boltTimer;
	uint32 Phase;
        bool Enraged;
        bool Worgen;
	bool Healed;
	bool lana;
	bool sa;

        void Reset()
        {
	    Phase = PHASE_START; 
            me->RestoreDisplayId();
            EnrageTimer = 15000;
            WoundTimer = 5000;
            BiteTimer = 45000;
            FeverTimer = 12000;
            Enraged = false;
            Worgen = false;
	    Healed = false;
	    lana = false;
	    sa =false;
	    avatarTimer = 20000;
            knockawayTimer = 15000;
            stormboltTimer = urand(5000, 7000);
            chargeTimer = urand(8000, 17000) ;
            layonhandsTimer = 100000;
	    panicTimer = urand(20000, 40000);
	    shadowboltTimer = urand(5000, 7000);
	    boltTimer = urand(7000, 9000);
	   summons.DespawnAll();   
	    me->setFaction(35);
           me->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
        }

        void EnterCombat(Unit* /*who*/)
        {
            me->MonsterYell(SAY_LIKANTROPO_AGGRO, LANG_UNIVERSAL, 0);
        }

        void KilledUnit(Unit* victim)
        {
            me->MonsterYell(SAY_LIKANTROPO_SLAY, LANG_UNIVERSAL, 0);
            // In caso di morte di un pg, evoca un worgen dal suo corpo <3 ale
            if (victim->GetTypeId() == TYPEID_PLAYER)
                victim->SummonCreature(SUMMON_WORGEN, victim->GetPositionX(), victim->GetPositionY(), victim->GetPositionZ(), 0, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 60000);
        }

        void JustDied(Unit* /*killer*/)
        {
            me->MonsterYell(SAY_LIKANTROPO_DEATH, LANG_UNIVERSAL, 0);
            me->RestoreDisplayId();
	    summons.DespawnAll();
        }

	void UpdateAI(const uint32 uiDiff)
        {
            if (!UpdateVictim())
                return;

 if (Phase == PHASE_START)
{
	if (HealthBelowPct(30))
	{
		if (HealthBelowPct(15) && !Enraged)
	            {
                DoCast(me, FRENZY);
                Enraged = true;
	            }

		Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0);
       		if (target)
        	       {
			me->GetMotionMaster()->MoveCharge(target->GetPositionX(), target->GetPositionY(), target->GetPositionZ(), 0,me->GetSpeed(MOVE_RUN), 0);
			}



	}else{
        if (HealthBelowPct(80))
         {
              Phase = PHASE_BREATH;
              return;
	}else
	{
	   if (stormboltTimer <= uiDiff)
           {
               Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 1);
               if (target)
               {
                   DoCast(target, SPELL_STORM_BOLT1);
                   stormboltTimer = urand(7000, 9000);
               }
           }
           else
               stormboltTimer -= uiDiff;

           if (shadowboltTimer <= uiDiff)
           {
                    DoCastVictim(SPELL_SHADOW_BOLT1);
                   shadowboltTimer = urand(5000, 7000) ;
           }
           else
               shadowboltTimer -= uiDiff;

           if (panicTimer <= uiDiff)
           {
               DoCastVictim(SPELL_PANIC1);
               panicTimer = urand(20000, 40000);
           }
           else
               panicTimer -= uiDiff;

           if (boltTimer <= uiDiff)
           {
               Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 1);
               if (target)
               {
                   DoCast(target, SPELL_BOLT1);
                   boltTimer = urand(7000, 9000) ;
               }
           }
           else
               boltTimer -= uiDiff;


           if (avatarTimer <= uiDiff)
           {
               DoCast(me, SPELL_AVATAR1);
               avatarTimer = 20000 ;
           }
           else
               avatarTimer -= uiDiff;

           if (knockawayTimer <= uiDiff)
           {
               DoResetThreat();									// Threat Reset
               DoCastVictim(SPELL_KNOCK_AWAY1);
               knockawayTimer = 10000;
           }
           else
               knockawayTimer -= uiDiff;

           if (chargeTimer <= uiDiff)
           {
                   DoCast(me, SPELL_CHARGE1);
                  chargeTimer = urand(8000, 17000) ;
           }
           else 
               chargeTimer -= uiDiff;


           if (layonhandsTimer <= uiDiff)
           {
		DoCast(me, SPELL_LAY_ON_HANDS1);
                 layonhandsTimer = 100000 ;
           }
           else 
               layonhandsTimer -= uiDiff;

            // Piccoli Enrage (durata 8 secondi) che raddoppiano il danno ogni 20 sec circa
            if (EnrageTimer <= uiDiff)
            {
                DoCast(me, ENRAGE);
                EnrageTimer = urand(20000,25000);
            } else EnrageTimer -= uiDiff;

            // Ogni 5 sec applica uno stack di wound che riduce del 5% le cure (max 15 stack)
            if (WoundTimer <= uiDiff)
            {
                DoCast(me->GetVictim(), MORTAL_WOUND);
                WoundTimer = 5000;
            } else WoundTimer -= uiDiff;

            // Ogni 20 sec applica un disease che riduce del 50% gli hp e fa 3k ogni 3 sec per 21 sec
            if (FeverTimer <= uiDiff)
            {
                DoCastAOE(DECREPIT_FEVER);
                FeverTimer = 20000;
            } else FeverTimer -= uiDiff;

            // Ogni 45 secondi Whirlwind che colpisce ad area facendo il danno normale
            // e contemporaneamente trasforma un target a caso in Worgen, disattivandogli le spell
            if (BiteTimer <= uiDiff)
            {
                DoCast(me, WORGEN_BITE);
                if (Unit *pTarget = SelectTarget(SELECT_TARGET_RANDOM,1))
                    me->AddAura(WORGEN_CALL, pTarget);
                BiteTimer = 45000;
            } else BiteTimer -= uiDiff;
	 }
	}
 }else
 {
            // Ad 80% hp si trasforma in Worgen
            if (HealthBelowPct(80) && !Healed)
            {
		uint32 model;
		switch(rand()%50)
		{
			case 0:
				model = 30790;  
				Healed = true;
				sa =true;
				break;
			case 1:
				model = 31093;
				Healed = true;
				lana = true;
				break;
		}
                me->SetDisplayId(model);
                 DoCast(me, ENRAGE);
                EnrageTimer = urand(25000,35000);
                Worgen = true;
		return;
	    }

	if(lana && !sa)
            {
		me->SetCanFly(true);
		Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0);
       		if (target)
        	       {
			me->GetMotionMaster()->MoveCharge(target->GetPositionX(), target->GetPositionY(), target->GetPositionZ()+20, 0,me->GetSpeed(MOVE_RUN), 0);
			}

		if (boltTimer <= uiDiff)
 			{
				Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0);
       			        if (target)
        		       {
        		           DoCast(target, 71446);
				   DoCast(me, 71445);
				   boltTimer = urand(2000, 4000) ;
					return;
				}
        		  }
  		else
              		boltTimer -= uiDiff;

		if (stormboltTimer <= uiDiff)
 			{
				Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 0.0f, true);
       			        if (target)
        		       {
        		           target->SummonCreature(SUMMON_ATT, target->GetPositionX(), target->GetPositionY(), target->GetPositionZ(), 0, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 30000);
				   stormboltTimer = urand(13000, 16000) ;
				}
        		  }
  		else
             		stormboltTimer -= uiDiff;

		if (FeverTimer <= uiDiff)
 		       {
				Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 0.0f, true);
       			        if (target)
        		       {
			   target->SummonCreature(SUMMON_ATT2, target->GetPositionX(), target->GetPositionY(), target->GetPositionZ(),0, TEMPSUMMON_TIMED_DESPAWN, 60000);
			   FeverTimer = 30000 ;
	     		  }
			}
  		else
              		FeverTimer -= uiDiff;

		if (HealthBelowPct(30))
        	 {
			Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0);
       			        if (target)
        		       {
					me->GetMotionMaster()->MoveCharge(target->GetPositionX(), target->GetPositionY(), target->GetPositionZ(), target->GetOrientation(),me->GetSpeed(MOVE_RUN), 0);
				}
		 	Phase = PHASE_START;
			me->SetDisplayId(30721);
            	   return;
		}
       }

       if(sa && !lana)
            {
		if (boltTimer <= uiDiff)
 			{
				   DoCastVictim(71623);
				   boltTimer = urand(2000, 4000) ;
        		  }
  		else
              		boltTimer -= uiDiff;

		if (shadowboltTimer <= uiDiff)
 			{
				DoCast(me, 70358);
        		        shadowboltTimer = urand(10000,12000) ;
	      		  }
  		else
              		shadowboltTimer -= uiDiff;

		if (BiteTimer <= uiDiff)
 		       {
				Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0);
       			        if (target)
        		       {
				   DoCast(target,74525);
				   BiteTimer = 45000 ;
				}

        		  }
  		else
              		BiteTimer -= uiDiff;

		if (stormboltTimer <= uiDiff)
 			{
				DoCast(me,70372);
         		   stormboltTimer = urand(17000, 23000) ;
	       		  }
  		else
             		stormboltTimer -= uiDiff;

		if (FeverTimer <= uiDiff)
 		       {
				Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 0.0f, true);
       			        if (target)
        		       {
			   target->SummonCreature(SUMMON_ATT2, target->GetPositionX(), target->GetPositionY(), target->GetPositionZ(), 0, TEMPSUMMON_TIMED_DESPAWN, 60000);
			   FeverTimer = 30000 ;
	     		  }
      		  }
  		else
              		FeverTimer -= uiDiff;

		if (HealthBelowPct(30))
        	 {
			Phase = PHASE_START;
			me->SetDisplayId(30721);
            	   return;
		}
       }
}
           DoMeleeAttackIfReady();
        }

     };

};


/*#########
# Lava Boost
#########*/

class npc_lava_boost : public CreatureScript
{
public:
    npc_lava_boost() : CreatureScript("npc_lava_boost") { }

    CreatureAI* GetAI(Creature* pCreature) const
    {
        return new npc_lava_boostAI (pCreature);
    }

    struct npc_lava_boostAI : public ScriptedAI
    {
        npc_lava_boostAI(Creature *pCreature) : ScriptedAI(pCreature){}

        uint32 ChainTimer;
        uint32 LavaTimer;
        uint32 FlameTimer;
        uint32 ShockTimer;
        bool Haste;
        bool Healed;

        void Reset()
        {
            ChainTimer = 8000;
            LavaTimer = 15000;
            FlameTimer = 4000;
            ShockTimer = 20000;
            Haste = false;
            Healed = false;
        }

        void EnterCombat(Unit* /*who*/)
        {
            me->MonsterYell(SAY_LAVA_AGGRO, LANG_UNIVERSAL, 0);
        }

        void KilledUnit(Unit* victim)
        {
            me->MonsterYell(SAY_LAVA_SLAY, LANG_UNIVERSAL, 0);
        }

        void JustDied(Unit* /*killer*/)
        {
            me->MonsterYell(SAY_LAVA_DEATH, LANG_UNIVERSAL, 0);
        }

        void UpdateAI(const uint32 uiDiff)
        {
            if (!UpdateVictim())
                return;

            // Chain Lightning ogni 10/15 sec
            if (ChainTimer <= uiDiff)
            {
                me->InterruptNonMeleeSpells(false);
                DoCast(me->GetVictim(), CHAIN_LIGHTNING);
                ChainTimer = urand(10000,15000);
            } else ChainTimer -= uiDiff;

            // Ogni 15 sec casta Lava Burst su un target random da 8/10k
            if (LavaTimer <= uiDiff)
            {
                me->InterruptNonMeleeSpells(false);
                if (Unit *pTarget = SelectTarget(SELECT_TARGET_RANDOM,1))
                    DoCast(pTarget, LAVA_BURST);
                DoCast(me, LIGHTNING_SHIELD);
                LavaTimer = 15000;
            } else LavaTimer -= uiDiff;

            // Ogni 20 sec casta Earth Shock su un target random
            if (ShockTimer <= uiDiff)
            {
                me->InterruptNonMeleeSpells(false);
                if (Unit *pTarget = SelectTarget(SELECT_TARGET_RANDOM,1))
                    DoCast(pTarget, EARTH_SHOCK);
                DoCast(me, LIGHTNING_RING);
                ShockTimer = 20000;
            } else ShockTimer -= uiDiff;

            // Ogni 12 sec applica Flame Shock sul tank
            if (FlameTimer <= uiDiff)
            {
                me->InterruptNonMeleeSpells(false);
                DoCast(me->GetVictim(), FLAME_SHOCK);
                FlameTimer = 12000;
            } else FlameTimer -= uiDiff;

            // A 25% hp si cura con Healing Wave (solo una volta)
            if (HealthBelowPct(25) && !Healed)
            {
                me->InterruptNonMeleeSpells(false);
                DoCast(me, HEALING_WAVE);
                ChainTimer = 8000;
                LavaTimer = 10000;
                FlameTimer = 4000;
                ShockTimer = 15000;
                Healed = true;
            }

            // Bloodlust a 50% di vita e Summon Air Elemental
            if (HealthBelowPct(50) && !Haste)
            {
                me->InterruptNonMeleeSpells(false);
                DoCast(me, BLOODLUST);
                me->MonsterYell(SAY_LAVA_BLOOD, LANG_UNIVERSAL, 0);
                me->SummonCreature(SUMMON_AIR_ELEMENTAL, me->GetPositionX(), me->GetPositionY(), me->GetPositionZ(), 0, TEMPSUMMON_TIMED_DESPAWN, 60000);
                Haste = true;
            }

            DoSpellAttackIfReady(LIGHTNING_BOLT);
        }
    };

};


/*#########
# Quel'Nagas
#########*/

class npc_quel_nagas : public CreatureScript
{
public:
    npc_quel_nagas() : CreatureScript("npc_quel_nagas") { }

    CreatureAI* GetAI(Creature* pCreature) const
    {
        return new npc_quel_nagasAI (pCreature);
    }

    struct npc_quel_nagasAI : public ScriptedAI
    {
        npc_quel_nagasAI(Creature *pCreature) : ScriptedAI(pCreature){}

        uint32 Phase2Timer;
        uint32 Phase1Timer;
        uint32 SporeTimer;
        uint32 PoisonTimer;
        uint32 RootTimer;
        bool Phase1;

        void Reset()
        {
            Phase2Timer = 60000;
            Phase1Timer = 90000;
            SporeTimer = 6000;
            PoisonTimer = 15000;
            RootTimer = 25000;
            Phase1 = true;
        }

        void EnterCombat(Unit* /*who*/)
        {
            me->MonsterYell(SAY_NAGAS_AGGRO, LANG_UNIVERSAL, 0);
        }

        void KilledUnit(Unit* victim)
        {
            me->MonsterYell(SAY_NAGAS_SLAY, LANG_UNIVERSAL, 0);
        }

        void JustDied(Unit* /*killer*/)
        {
            me->MonsterYell(SAY_NAGAS_DEATH, LANG_UNIVERSAL, 0);
        }

        void UpdateAI(const uint32 uiDiff)
        {
            if (!UpdateVictim())
                return;

            // Summon Phase
            if (Phase2Timer <= uiDiff)
            {
                Phase1 = false;
                me->InterruptNonMeleeSpells(false);
                me->SetReactState(REACT_PASSIVE);
                me->AttackStop();
                me->RemoveAllAuras();
                // In Fase 2 evoca 3 Murloc
                if (Unit *pTarget = SelectTarget(SELECT_TARGET_RANDOM,0))
                    for (uint8 i = 1; i <= 3; i++)
                    {
                        me->SummonCreature(SUMMONED_MURLOC, pTarget->GetPositionX(), pTarget->GetPositionY(), pTarget->GetPositionZ(), 0, TEMPSUMMON_TIMED_DESPAWN, 60000);
                    }
                me->AddAura(SPELL_MAGIC_BARRIER, me);
                me->MonsterYell(SAY_NAGAS_SUMMON, LANG_UNIVERSAL, 0);
                Phase2Timer = 90000;
            } else Phase2Timer -= uiDiff;

            // Normal Phase
            if (Phase1Timer <= uiDiff)
            {
                Phase1 = true;
                me->InterruptNonMeleeSpells(false);
                me->SetReactState(REACT_AGGRESSIVE);
                me->RemoveAurasDueToSpell(SPELL_MAGIC_BARRIER);
                DoZoneInCombat();
                if (me->getThreatManager().isThreatListEmpty())
                    EnterEvadeMode();
                Phase1Timer = 90000;
            } else Phase1Timer -= uiDiff;

            if (Phase1)
            {
                // Ogni 15 sec lancia una spora velenosa ai piedi di un player random
                if (SporeTimer <= uiDiff)
                {
                    me->InterruptNonMeleeSpells(false);
                    if (Unit *pTarget = SelectTarget(SELECT_TARGET_RANDOM, 0))
                    {
                        Creature* trig = me->SummonCreature(TOXIC_SPORES_TRIGGER, pTarget->GetPositionX(), pTarget->GetPositionY(), pTarget->GetPositionZ(), 0, TEMPSUMMON_TIMED_DESPAWN, 30000);
                        if (trig)
                        {
                            trig->setFaction(14);
                            trig->CastSpell(trig, SPELL_TOXIC_SPORES,true);
                        }
                    }
                    SporeTimer = 15000;
                } else SporeTimer -= uiDiff;

                // Poison ad area ogni 10 sec
                if (PoisonTimer <= uiDiff)
                {
                    me->InterruptNonMeleeSpells(false);
                    DoCastAOE(SPELL_POISON_BOLT_VOLLEY);
                    PoisonTimer = 10000;
                } else PoisonTimer -= uiDiff;

                // Radici su un target random ogni 25 sec
                if (RootTimer <= uiDiff)
                {
                    me->InterruptNonMeleeSpells(false);
                    if (Unit *pTarget = SelectTarget(SELECT_TARGET_RANDOM, 0))
                        DoCast(pTarget, SPELL_ENTANGLE);
                    RootTimer = 25000;
                } else RootTimer -= uiDiff;
            }

            DoSpellAttackIfReady(SPELL_WRATH);
        }
    };

};


/*#########
# Might of Sylvanas
#########*/

class npc_might_of_sylvanas : public CreatureScript
{
public:
    npc_might_of_sylvanas() : CreatureScript("npc_might_of_sylvanas") { }

    CreatureAI* GetAI(Creature* pCreature) const
    {
        return new npc_might_of_sylvanasAI (pCreature);
    }

    struct npc_might_of_sylvanasAI : public ScriptedAI
    {
        npc_might_of_sylvanasAI(Creature *pCreature) : ScriptedAI(pCreature){}

        uint32 MultiTimer;
        uint32 VolleyTimer;
        uint32 SnakeTimer;
        uint32 PoisonTimer;
        uint32 TrapTimer;
        uint32 HealTimer;
        bool Haste;
        bool Healed;

        void Reset()
        {
            MultiTimer = 2000;
            VolleyTimer = 15000;
            SnakeTimer = 5000;
            PoisonTimer = 12000;
            TrapTimer = 20000;
            HealTimer = 25000;
            Haste = false;
            Healed = false;
        }

        void EnterCombat(Unit* /*who*/)
        {
            me->MonsterYell(SAY_SYLVANAS_AGGRO, LANG_UNIVERSAL, 0);
            me->AddAura(SPELL_BEWITCHING_AURA, me);
            me->AddAura(SPELL_AURA_ROT, me);
        }

        void KilledUnit(Unit* victim)
        {
            me->MonsterYell(SAY_SYLVANAS_SLAY, LANG_UNIVERSAL, 0);
        }

        void JustDied(Unit* /*killer*/)
        {
            me->MonsterYell(SAY_SYLVANAS_DEATH, LANG_UNIVERSAL, 0);
        }

        void UpdateAI(const uint32 uiDiff)
        {
            if (!UpdateVictim())
                return;

            if (me->HasUnitState(UNIT_STATE_CASTING))
                return;

            // Multishot ogni 2 sec
            if (MultiTimer <= uiDiff)
            {
                if (Unit *pTarget = SelectTarget(SELECT_TARGET_RANDOM, 0))
                    DoCast(pTarget, SPELL_MULTISHOT);
                MultiTimer = 4000;
            } else MultiTimer -= uiDiff;

            // Ogni 10 sec casta Volley su un target random
            if (VolleyTimer <= uiDiff)
            {
                DoCastAOE(SPELL_VOLLEY);
                VolleyTimer = 15000;
            } else VolleyTimer -= uiDiff;

            // Ogni 5 sec casta Snake Trap su un target random
            if (SnakeTimer <= uiDiff)
            {
                if (Unit *pTarget = SelectTarget(SELECT_TARGET_RANDOM,0))
                    for (uint8 i = 1; i <= 3; i++)
                    {
                        me->SummonCreature(SUMMONED_SNAKE, pTarget->GetPositionX(), pTarget->GetPositionY(), pTarget->GetPositionZ(), 0, TEMPSUMMON_TIMED_DESPAWN, 60000);
                    }
                SnakeTimer = 5000;
            } else SnakeTimer -= uiDiff;

            // Poison ad area ogni 10 sec
            if (PoisonTimer <= uiDiff)
            {
                DoCastAOE(SPELL_BOLT_VOLLEY);
                PoisonTimer = 12000;
            } else PoisonTimer -= uiDiff;

            // Ogni 20 sec casta Frost Trap sul tank
            if (TrapTimer <= uiDiff)
            {
                me->MonsterYell(SAY_SYLVANAS_ICE, LANG_UNIVERSAL, 0);
                DoCast(me->GetVictim(), SPELL_ICE_TRAP);
                TrapTimer = 20000;
            } else TrapTimer -= uiDiff;

            // Ogni 25 sec casta Rejuvenation
            if (HealTimer <= uiDiff)
            {
                DoCast(me, SPELL_REJUVENATION);
                HealTimer = 25000;
            } else HealTimer -= uiDiff;

            DoMeleeAttackIfReady();
        }
    };

};


/*#########
# Trollzilla
#########*/

class npc_trollzilla : public CreatureScript
{
public:
    npc_trollzilla() : CreatureScript("npc_trollzilla") { }

    CreatureAI* GetAI(Creature* pCreature) const
    {
        return new npc_trollzillaAI (pCreature);
    }

    struct npc_trollzillaAI : public ScriptedAI
    {
        npc_trollzillaAI(Creature *pCreature) : ScriptedAI(pCreature){}

        uint32 CleaveTimer;
        uint32 StompTimer;
        uint32 FireTimer;
        bool Enraged;

        void Reset()
        {
            CleaveTimer = 5000;
            StompTimer = 20000;
            FireTimer = 30000;
            Enraged = false;
        }

        void EnterCombat(Unit* /*who*/)
        {
            me->MonsterYell(SAY_TROLLZILLA_AGGRO, LANG_UNIVERSAL, 0);
            me->AddAura(SPELL_FIRE_SHIELD, me);
        }

        void KilledUnit(Unit* victim)
        {
            me->MonsterYell(SAY_TROLLZILLA_SLAY, LANG_UNIVERSAL, 0);
        }

        void JustDied(Unit* /*killer*/)

        {
            me->MonsterYell(SAY_TROLLZILLA_DEATH, LANG_UNIVERSAL, 0);
        }

        void UpdateAI(const uint32 uiDiff)
        {
            if (!UpdateVictim())
                return;

            // Ogni 5 sec fa Cleave

            if (CleaveTimer <= uiDiff)
            {
                DoCast(me->GetVictim(), SPELL_CLEAVE);
                CleaveTimer = 5000;
            } else CleaveTimer -= uiDiff;

            // Ogni 20 sec fa un War Stomp che stunna ad area
            if (StompTimer <= uiDiff)
            {
                DoCastAOE(SPELL_WAR_STOMP);
                StompTimer = 20000;
            } else StompTimer -= uiDiff;

            // Summon Fire Elemental ogni 30 sec
            if (FireTimer <= uiDiff)
            {
                if (Unit *pTarget = SelectTarget(SELECT_TARGET_RANDOM,0))
                    me->SummonCreature(SUMMON_FIRE_ELEMENTAL, pTarget->GetPositionX(), pTarget->GetPositionY(), pTarget->GetPositionZ(), 0, TEMPSUMMON_TIMED_DESPAWN, 60000);
                FireTimer = 30000;
            } else FireTimer -= uiDiff;

            // Enrage a 25% di vita (aumenta 40% attack speed e 25% damage)
            if (HealthBelowPct(25) && !Enraged)
            {
                me->MonsterYell(SAY_TROLLZILLA_FRENZY, LANG_UNIVERSAL, 0);
                DoCast(me, SPELL_FRENZY);
                Enraged = true;
            }

            DoMeleeAttackIfReady();
        }
    };

};


/*#########
# Perotharn
#########*/

class npc_perotharn : public CreatureScript
{
public:
    npc_perotharn() : CreatureScript("npc_perotharn") { }

    CreatureAI* GetAI(Creature* pCreature) const
    {
        return new npc_perotharnAI (pCreature);
    }

    struct npc_perotharnAI : public ScriptedAI
    {
        npc_perotharnAI(Creature *pCreature) : ScriptedAI(pCreature){}

        uint32 BurstTimer;
        uint32 GriefTimer;
        uint32 FlameTimer;
        uint32 PiercingTimer;
        uint32 HarvestTimer;

        void Reset()
        {
            BurstTimer = 30000;
            GriefTimer = 20000;
            FlameTimer = 15000;
            PiercingTimer = 10000;
            HarvestTimer = 45000;
        }

        void EnterCombat(Unit* /*who*/)
        {
            me->MonsterYell(SAY_PEROTHARN_AGGRO, LANG_UNIVERSAL, 0);
        }

        void KilledUnit(Unit* victim)
        {
            me->MonsterYell(SAY_PEROTHARN_SLAY, LANG_UNIVERSAL, 0);
        }

        void JustDied(Unit* /*killer*/)
        {
            me->MonsterYell(SAY_PEROTHARN_DEATH, LANG_UNIVERSAL, 0);
        }

        void UpdateAI(const uint32 uiDiff)
        {
            if (!UpdateVictim())
                return;

            // Shadow Burst ad area e reset threat
            if (BurstTimer <= uiDiff)
            {
                me->InterruptNonMeleeSpells(false);
                me->DeleteThreatList();
                DoCast(me->GetVictim(), SHADOW_BURST);
                BurstTimer = urand(25000,35000);
            } else BurstTimer -= uiDiff;

            // Storm of Grief ogni 20 sec colpisce ad area
            if (GriefTimer <= uiDiff)
            {
                me->InterruptNonMeleeSpells(false);
                if (Unit *pTarget = SelectTarget(SELECT_TARGET_RANDOM,0))
                    DoCast(pTarget, STORM_OF_GRIEF);
                GriefTimer = 20000;
            } else GriefTimer -= uiDiff;

            // Ogni 10 sec casta Piercing Shadow sul target a maggior aggro dopo il tank
            if (PiercingTimer <= uiDiff)
            {
                me->InterruptNonMeleeSpells(false);
                if (Unit *pTarget = SelectTarget(SELECT_TARGET_TOPAGGRO,0))
                    DoCast(pTarget, PIERCING_SHADOW);
                PiercingTimer = 10000;
            } else PiercingTimer -= uiDiff;

            // Ogni 15 sec casta Shadow Flame che fa danno a cono
            if (FlameTimer <= uiDiff)
            {
                me->InterruptNonMeleeSpells(false);
                DoCastAOE(SHADOW_FLAME);
                FlameTimer = 15000;
            } else FlameTimer -= uiDiff;

            // Ogni 45 sec casta Harvest Soul su tutto il raid
            if (HarvestTimer <= uiDiff)
            {
                me->InterruptNonMeleeSpells(false);
                me->MonsterYell(SAY_PEROTHARN_SOUL, LANG_UNIVERSAL, 0);
                DoCastAOE(HARVEST_SOUL);
                HarvestTimer = 45000;
            } else HarvestTimer -= uiDiff;

            DoSpellAttackIfReady(SHADOW_BOLT);
        }
    };

};


/*#########
# Uznam the Watcher
#########*/

class npc_uznam_the_watcher : public CreatureScript
{
public:
    npc_uznam_the_watcher() : CreatureScript("npc_uznam_the_watcher") { }

    CreatureAI* GetAI(Creature* pCreature) const
    {
        return new npc_uznam_the_watcherAI (pCreature);
    }

    struct npc_uznam_the_watcherAI : public ScriptedAI
    {
        npc_uznam_the_watcherAI(Creature *pCreature) : ScriptedAI(pCreature){}

        uint32 EarthquakeTimer;
        uint32 WhirlwindTimer;
        uint32 ShockTimer;
        uint32 AuralShockTimer;

        void Reset()
        {
            EarthquakeTimer = 8000;
            WhirlwindTimer = 20000;
            ShockTimer = 30000;
            AuralShockTimer = 50000;
        }

        void EnterCombat(Unit* /*who*/)
        {
            me->MonsterYell(SAY_UZNAM_AGGRO, LANG_UNIVERSAL, 0);
            DoCast(me, ENTROPIC_AURA);
        }

        void KilledUnit(Unit* victim)
        {
            me->MonsterYell(SAY_UZNAM_SLAY, LANG_UNIVERSAL, 0);
        }

        void JustDied(Unit* /*killer*/)
        {
            me->MonsterYell(SAY_UZNAM_DEATH, LANG_UNIVERSAL, 0);
        }

        void UpdateAI(const uint32 uiDiff)
        {
            if (!UpdateVictim())
                return;

            // Earthquake Ogni 20 secondi.
            if (EarthquakeTimer <= uiDiff)
            {
                DoCast(EARTHQUAKE);
                EarthquakeTimer = 20000;
            } else EarthquakeTimer -= uiDiff;

            // Whirlwind Ogni 30 sec
            if (WhirlwindTimer <= uiDiff)
            {
                me->MonsterYell(SAY_UZNAM_SPECIAL, LANG_UNIVERSAL, 0);
                me->MonsterTextEmote(EMOTE_WHIRL, 0, true);
                DoCast(me, WHIRLWIND);
                WhirlwindTimer = 30000;
            } else WhirlwindTimer -= uiDiff;

            // Shock of sorrow Ogni 40 sec.
            if (ShockTimer <= uiDiff)
            {
                if (Unit *pTarget = SelectTarget(SELECT_TARGET_RANDOM,0))
                    DoCast(pTarget, SHOCK_OF_SORROW);
                ShockTimer = 40000;
            } else ShockTimer -= uiDiff;
            
            // Aural Shock Ogni minuto
            if (AuralShockTimer <= uiDiff)
            {
                DoCast(AURAL_SHOCK);
                AuralShockTimer = 60000;
            } else AuralShockTimer -= uiDiff;

            DoMeleeAttackIfReady();
        }
    };

};



// Teleport NPC GossipHello
class Pandaren : public CreatureScript
{
public:
	Pandaren() : CreatureScript("Pandaren") {}

   struct PandarenAI : public ScriptedAI
   {
       PandarenAI(Creature* Creature) : ScriptedAI(Creature) {}

       void Reset()
       {
           _avatarTimer = 20 *IN_MILLISECONDS;
           _knockawayTimer = 15 *IN_MILLISECONDS;
           _stormboltTimer = urand(5, 7) *IN_MILLISECONDS;
           _chargeTimer = urand(8, 17) *IN_MILLISECONDS;
           _layonhandsTimer = 100 *IN_MILLISECONDS;
		   _panicTimer = urand(20, 40) *IN_MILLISECONDS;
		   _shadowboltTimer = urand(5, 7) *IN_MILLISECONDS;
		   _boltTimer = urand(7, 9) *IN_MILLISECONDS;
       }
  
	    void EnterCombat(Unit* /*who*/)
       {
       }

       void UpdateAI(uint32 const diff)
       {
           if (!UpdateVictim())
               return;

		   if (_stormboltTimer <= diff)
           {
               Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 1);
               if (target)
               {
                   DoCast(target, SPELL_STORM_BOLT);
                   _stormboltTimer = urand(7, 9) *IN_MILLISECONDS;
               }
           }
           else
               _stormboltTimer -= diff;

           if (_shadowboltTimer <= diff)
           {
                    DoCastVictim(SPELL_SHADOW_BOLT);
                   _shadowboltTimer = urand(5, 7) *IN_MILLISECONDS;
           }
           else
               _shadowboltTimer -= diff;

           if (_panicTimer <= diff)
           {
               DoCastVictim(SPELL_PANIC);
               _panicTimer = urand(20, 40) *IN_MILLISECONDS;
           }
           else
               _panicTimer -= diff;

           if (_boltTimer <= diff)
           {
               Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 1);
               if (target)
               {
                   DoCast(target, SPELL_BOLT);
                   _boltTimer = urand(7, 9) *IN_MILLISECONDS;
               }
           }
           else
               _boltTimer -= diff;


           if (_avatarTimer <= diff)
           {
               DoCast(me, SPELL_AVATAR);
               _avatarTimer = 20 *IN_MILLISECONDS;
           }
           else
               _avatarTimer -= diff;

           if (_knockawayTimer <= diff)
           {
               DoResetThreat();									// Threat Reset
               DoCastVictim(SPELL_KNOCK_AWAY);
               _knockawayTimer = 10 *IN_MILLISECONDS;
           }
           else
               _knockawayTimer -= diff;

           if (_chargeTimer <= diff)
           {
                   DoCast(me, SPELL_CHARGE);
                   _chargeTimer = urand(8, 17) *IN_MILLISECONDS;
           }
           else 
               _chargeTimer -= diff;


           if (_layonhandsTimer <= diff)
           {
               DoCast(me, SPELL_LAY_ON_HANDS);
               _layonhandsTimer = 100 *IN_MILLISECONDS;
           }
           else 
               _layonhandsTimer -= diff;
			
           DoMeleeAttackIfReady();
       }
   
   private:
		uint32	_avatarTimer;
        uint32	_knockawayTimer;
        uint32	_stormboltTimer;
		uint32	_chargeTimer;
		uint32  _layonhandsTimer;
		uint32	_panicTimer;
		uint32	_shadowboltTimer;
		uint32	_boltTimer;
   };

   CreatureAI* GetAI(Creature* Creature) const
   {
       return new PandarenAI (Creature);
   }

	
	bool OnGossipHello(Player * pPlayer, Creature * pCreature)
	{
		if (pPlayer->IsInCombat() || pPlayer->HasInvisibilityAura() || pPlayer->HasStealthAura() || pPlayer->InBattleground() || pPlayer->InArena())
		{
			SEND_NOTIFICATION("당신은 전투 중입니다.");
			pPlayer->CLOSE_GOSSIP_MENU();
			return true;
		}

		pPlayer->ADD_GOSSIP_ITEM(MI_WNG, "[이동 하기]",		SEND_MAIN, PG_TRAVEL);
	/*	if(pPlayer->GetMoney() > 4000000)
		{
		pPlayer->ADD_GOSSIP_ITEM(MI_WNG, "[버프,장비수리,부활후유증제거]",	SEND_BUFF, 0);
		}
	*/	GOSSIP_MENU(700000, pCreature->GetGUID());
		return true;
	}

	// Teleport NPC GossipSelect
	bool OnGossipSelect(Player * pPlayer, Creature * pCreature, uint32 iSender, uint32 iAction)
	{
		if (pPlayer->IsInCombat() || pPlayer->HasInvisibilityAura() || pPlayer->HasStealthAura() || pPlayer->InBattleground() || pPlayer->InArena())
		{
			SEND_NOTIFICATION("당신은 전투 중입니다.");
			pPlayer->CLOSE_GOSSIP_MENU();
			return true;
		}
		
		uint32 money = pPlayer->GetMoney();
		
		// Pages accessed via Main Menu
		if (iSender == SEND_MAIN)
		{
			GOSSIP_CLEAR();
			if (iAction != PG_MAIN)
				pPlayer->ADD_GOSSIP_ITEM(MI_CHT, GoBack,	SEND_MAIN, PG_MAIN);

			if (iAction == PG_MAIN)			// Return to Main Page
				return OnGossipHello(pPlayer, pCreature);

			if (iAction == PG_TRAVEL)		// Travel Page
			{
				pPlayer->ADD_GOSSIP_ITEM(MI_WNG,"[대도시]",			SEND_TRAVEL, PG_CITY_T);
				pPlayer->ADD_GOSSIP_ITEM(MI_WNG,"[마을]",			SEND_TRAVEL, PG_TOWN_T);
			//	pPlayer->ADD_GOSSIP_ITEM(MI_WNG,"[PVP & 투기장 퀘스트 지역]",	SEND_TRAVEL, PG_PVP_T);
				pPlayer->ADD_GOSSIP_ITEM(MI_WNG,"[인던 & 공격대]",		SEND_TRAVEL, PG_D_AND_R);
			}

			if (iAction == PG_BUFF)		// Buff Page
			{
 		         pPlayer->ADD_GOSSIP_ITEM(MI_WNG, "[버프,장비수리,부활후유증제거]",	SEND_BUFF, 0);
			}

			GOSSIP_MENU(700000, pCreature->GetGUID());
		}
		
		/*
		Teleport
		=================================================================================================================================================================
		=================================================================================================================================================================
		=================================================================================================================================================================
		*/

		// City Teleport
		if (iSender == SEND_TE_CITY)
		{
			pPlayer->CLOSE_GOSSIP_MENU();
			if(pPlayer->getLevel() == 80)
				{
					if(money > 250000)
				 	{ pPlayer->SetMoney(money - 250000);
					TELEPORT_TO(travelCity[iAction].map, travelCity[iAction].posx, travelCity[iAction].posy, travelCity[iAction].posz, travelCity[iAction].poso);
					}
					else{
						SEND_NOTIFICATION("이동에 필요한 골드가 부족합니다.");
					}
				}
			else{
			TELEPORT_TO(travelCity[iAction].map, travelCity[iAction].posx, travelCity[iAction].posy, travelCity[iAction].posz, travelCity[iAction].poso);
			}
		}

		// Azeroth Towns Teleport
		if (iSender == SEND_TE_T_AZ)
		{
			pPlayer->CLOSE_GOSSIP_MENU();
			if(pPlayer->getLevel() == 80)
				{
				   	if(money > 250000)
				 	{ pPlayer->SetMoney(money - 250000);
					TELEPORT_TO(townAZ[iAction].map, townAZ[iAction].posx, townAZ[iAction].posy, townAZ[iAction].posz, townAZ[iAction].poso);
					}
					else{
						SEND_NOTIFICATION("이동에 필요한 골드가 부족합니다.");
					}
				}
			else{
			TELEPORT_TO(townAZ[iAction].map, townAZ[iAction].posx, townAZ[iAction].posy, townAZ[iAction].posz, townAZ[iAction].poso);
			}
		}

		// Outland Towns Teleport
		if (iSender == SEND_TE_T_OL)
		{
			pPlayer->CLOSE_GOSSIP_MENU();
			if(pPlayer->getLevel() == 80)
				{
				   	if(money > 250000)
				 	{ pPlayer->SetMoney(money - 250000);
					TELEPORT_TO(townOL[iAction].map, townOL[iAction].posx, townOL[iAction].posy, townOL[iAction].posz, townOL[iAction].poso);					
					}
					else{
						SEND_NOTIFICATION("이동에 필요한 골드가 부족합니다.");
					}
				}
			else{
			TELEPORT_TO(townOL[iAction].map, townOL[iAction].posx, townOL[iAction].posy, townOL[iAction].posz, townOL[iAction].poso);
			}
		}

		// Northrend Towns Teleport
		if (iSender == SEND_TE_T_NR)
		{
			pPlayer->CLOSE_GOSSIP_MENU();
			if(pPlayer->getLevel() == 80)
				{
   					if(money > 250000)
				 	{ pPlayer->SetMoney(money - 250000);
					TELEPORT_TO(townNR[iAction].map, townNR[iAction].posx, townNR[iAction].posy, townNR[iAction].posz, townNR[iAction].poso);
					}
					else{
						SEND_NOTIFICATION("이동에 필요한 골드가 부족합니다.");
					}
				}
			else{
			TELEPORT_TO(townNR[iAction].map, townNR[iAction].posx, townNR[iAction].posy, townNR[iAction].posz, townNR[iAction].poso);
			}
		}
        //대격변지역 이동
		if (iSender == SEND_TE_T_CY)
		{
			pPlayer->CLOSE_GOSSIP_MENU();
			if(pPlayer->getLevel() == 80)
				{
   					if(money > 250000)
				 	{ pPlayer->SetMoney(money - 250000);
					TELEPORT_TO(travelCity[iAction].map, travelCity[iAction].posx, travelCity[iAction].posy, travelCity[iAction].posz, travelCity[iAction].poso);
					}
					else{
						SEND_NOTIFICATION("이동에 필요한 골드가 부족합니다.");
					}
				}
			else{
			TELEPORT_TO(travelCity[iAction].map, travelCity[iAction].posx, travelCity[iAction].posy, travelCity[iAction].posz, travelCity[iAction].poso);
			}
		}

		// Azeroth Dungeons Teleport
		if (iSender == SEND_TE_D_AZ)
		{
			pPlayer->CLOSE_GOSSIP_MENU();
			if(pPlayer->getLevel() == 80)
				{
   					if(money > 250000)
				 	{ pPlayer->SetMoney(money - 250000);
					TELEPORT_TO(dungAZ[iAction].map, dungAZ[iAction].posx, dungAZ[iAction].posy, dungAZ[iAction].posz, dungAZ[iAction].poso);
					}
					else{
						SEND_NOTIFICATION("이동에 필요한 골드가 부족합니다.");
					}
				}
			else{
			TELEPORT_TO(dungAZ[iAction].map, dungAZ[iAction].posx, dungAZ[iAction].posy, dungAZ[iAction].posz, dungAZ[iAction].poso);
			}
		}

		// Outland Dungeons Teleport
		if (iSender == SEND_TE_D_OL)
		{
			pPlayer->CLOSE_GOSSIP_MENU();
			if(pPlayer->getLevel() == 80)
				{
   					if(money > 250000)
				 	{ pPlayer->SetMoney(money - 250000);
					TELEPORT_TO(dungOL[iAction].map, dungOL[iAction].posx, dungOL[iAction].posy, dungOL[iAction].posz, dungOL[iAction].poso);
					}
					else{
						SEND_NOTIFICATION("이동에 필요한 골드가 부족합니다.");
					}
				}
			else{
			TELEPORT_TO(dungOL[iAction].map, dungOL[iAction].posx, dungOL[iAction].posy, dungOL[iAction].posz, dungOL[iAction].poso);
			}
		}

		// Northrend Dungeons Teleport
		if (iSender == SEND_TE_D_NR)
		{
			pPlayer->CLOSE_GOSSIP_MENU();
			if(pPlayer->getLevel() == 80)
				{
   					if(money > 250000)
				 	{ pPlayer->SetMoney(money - 250000);
					TELEPORT_TO(dungNR[iAction].map, dungNR[iAction].posx, dungNR[iAction].posy, dungNR[iAction].posz, dungNR[iAction].poso);
					}
					else{
						SEND_NOTIFICATION("이동에 필요한 골드가 부족합니다.");
					}
				}
			else{
			TELEPORT_TO(dungNR[iAction].map, dungNR[iAction].posx, dungNR[iAction].posy, dungNR[iAction].posz, dungNR[iAction].poso);
			}
		}

		// 대격변 Teleport
		if (iSender == SEND_TE_D_CY)
		{
			pPlayer->CLOSE_GOSSIP_MENU();
			if(pPlayer->getLevel() == 80)
				{
   					if(money > 250000)
				 	{ pPlayer->SetMoney(money - 250000);
					TELEPORT_TO(travelCity[iAction].map, travelCity[iAction].posx, travelCity[iAction].posy, travelCity[iAction].posz, travelCity[iAction].poso);
					}
					else{
						SEND_NOTIFICATION("이동에 필요한 골드가 부족합니다.");
					}
				}
			else{
			TELEPORT_TO(travelCity[iAction].map, travelCity[iAction].posx, travelCity[iAction].posy, travelCity[iAction].posz, travelCity[iAction].poso);
			}
		}


		// PvP Teleport
		if (iSender == SEND_TE_PVP)
		{
			pPlayer->CLOSE_GOSSIP_MENU();
			if(pPlayer->getLevel() == 80)
				{
   					if(money > 250000)
				 	{ pPlayer->SetMoney(money - 250000);
					TELEPORT_TO(travelPVP[iAction].map, travelPVP[iAction].posx, travelPVP[iAction].posy, travelPVP[iAction].posz, travelPVP[iAction].poso);
					}
					else{
						SEND_NOTIFICATION("이동에 필요한 골드가 부족합니다.");
					}
				}
			else{
			TELEPORT_TO(travelPVP[iAction].map, travelPVP[iAction].posx, travelPVP[iAction].posy, travelPVP[iAction].posz, travelPVP[iAction].poso);
			}
		}


		// Buff
		/*
		Travel
		=================================================================================================================================================================
		=================================================================================================================================================================
		=================================================================================================================================================================
		*/
		if (iSender == SEND_BUFF)
		{
			switch (iAction)
			{
			case 0:		// 버프
				{
					if(pPlayer->getLevel() == 80)
					{
	     			   pPlayer->SetMoney(money - 10000000);
					pPlayer->CastSpell(pPlayer, 16612, true);
							pPlayer->CastSpell(pPlayer, 17013, true);
							pPlayer->CastSpell(pPlayer, 10767, true);
							pPlayer->CastSpell(pPlayer, 48161, true);
							pPlayer->CastSpell(pPlayer, 42995, true);
							pPlayer->CastSpell(pPlayer, 48469, true);
							pPlayer->CastSpell(pPlayer, 15366, true);
							pPlayer->CastSpell(pPlayer, 16609, true);
							pPlayer->CastSpell(pPlayer, 24425, true);
							pPlayer->CastSpell(pPlayer, 22888, true);
							pPlayer->CastSpell(pPlayer, 34410, true);
							pPlayer->CastSpell(pPlayer, 29235, true);
							pPlayer->DurabilityRepairAll(false, 0.0f, false);
							pPlayer->learnSpell(DEATHSICK, false);
							pPlayer->removeSpell(DEATHSICK);
	
							
							SEND_NOTIFICATION("버프,장비수리,부활 후유증제거 지원이 완료되었습니다.");
					}
					else
					{
							SEND_NOTIFICATION("80레벨 이전에는 이용하실수없습니다.");
					}
				}
			}
			pPlayer->CLOSE_GOSSIP_MENU(); // sweet
			return true;
		}

		if (iSender == SEND_TRAVEL)
		{
			// Pages accessed via Travel Menu
			if (iAction == PG_CITY_T || iAction == PG_TOWN_T || iAction == PG_PVP_T || iAction == PG_D_AND_R)
			{
				GOSSIP_CLEAR();
				pPlayer->ADD_GOSSIP_ITEM(MI_CHT, GoBack,	SEND_MAIN, PG_TRAVEL);

				switch (iAction)
				{
				case PG_CITY_T:			// City Travel Menu
					{
						if (PLAYER_TEAM() == HORDE || pPlayer->IsGameMaster() )
						{
							pPlayer->ADD_GOSSIP_ITEM(MI_WNG, "[오그리마]",	SEND_TE_CITY, 0);
							pPlayer->ADD_GOSSIP_ITEM(MI_WNG, "[언더시티]",	SEND_TE_CITY, 1);
							pPlayer->ADD_GOSSIP_ITEM(MI_WNG, "[썬더 블러프]",	SEND_TE_CITY, 2);
							pPlayer->ADD_GOSSIP_ITEM(MI_WNG, "[실버문]",	SEND_TE_CITY, 3);
						}
						if (PLAYER_TEAM() == ALLIANCE || pPlayer->IsGameMaster() )
						{
							pPlayer->ADD_GOSSIP_ITEM(MI_WNG, "[스톰윈드]",	SEND_TE_CITY, 4);
							pPlayer->ADD_GOSSIP_ITEM(MI_WNG, "[아이언포지]",	SEND_TE_CITY, 5);
							pPlayer->ADD_GOSSIP_ITEM(MI_WNG, "[다르나서스]",	SEND_TE_CITY, 6);
							pPlayer->ADD_GOSSIP_ITEM(MI_WNG, "[엑소다르]",	SEND_TE_CITY, 7);
						}

						pPlayer->ADD_GOSSIP_ITEM(MI_WNG, "[샤트라스]",		SEND_TE_CITY, 8);
						pPlayer->ADD_GOSSIP_ITEM(MI_WNG, "[달라란]",		SEND_TE_CITY, 9);
						//pPlayer->ADD_GOSSIP_ITEM(MI_WNG, "[GM의 안식처]",		SEND_TE_CITY, 10);
					}
					break;
		
				case PG_TOWN_T:			// Town Travel Menu
					{
						pPlayer->ADD_GOSSIP_ITEM(MI_WNG, "[아제로스]",		SEND_TRAVEL, PG_AZ_T);
						pPlayer->ADD_GOSSIP_ITEM(MI_WNG, "[아웃랜드]",		SEND_TRAVEL, PG_OL_T);
						pPlayer->ADD_GOSSIP_ITEM(MI_WNG, "[노스렌드]",		SEND_TRAVEL, PG_NR_T);
						/* pPlayer->ADD_GOSSIP_ITEM(MI_WNG, "[대격변지역]",		SEND_TRAVEL, PG_CY_T); */
					}
					break;

				case PG_PVP_T:			// PvP Travel Menu
					{
						pPlayer->ADD_GOSSIP_ITEM(MI_WNG, "[구루바시 투기장]",SEND_TE_PVP, PLAYER_TEAM() == HORDE ? 0 : 9);
						pPlayer->ADD_GOSSIP_ITEM(MI_WNG, "[혈투의 전장]",SEND_TE_PVP, PLAYER_TEAM() == HORDE ? 1 : 10);

						/*if (PLAYER_TEAM() == HORDE)
						{
							pPlayer->ADD_GOSSIP_ITEM(MI_WNG, "[스톰윈드 침공]",	SEND_TE_PVP, 2);
							pPlayer->ADD_GOSSIP_ITEM(MI_WNG, "[아이언포지 침공]",		SEND_TE_PVP, 3);
							pPlayer->ADD_GOSSIP_ITEM(MI_WNG, "[다르나서스 침공]",		SEND_TE_PVP, 4);
							pPlayer->ADD_GOSSIP_ITEM(MI_WNG, "[엑소다르 침공]",		SEND_TE_PVP, 5);
						}
						if (PLAYER_TEAM() == ALLIANCE)
						{
							pPlayer->ADD_GOSSIP_ITEM(MI_WNG, "[오그리마 침공]",		SEND_TE_PVP, 11);
							pPlayer->ADD_GOSSIP_ITEM(MI_WNG, "[언더시티 침공]",		SEND_TE_PVP, 12);
							pPlayer->ADD_GOSSIP_ITEM(MI_WNG, "[썬더 블러프 침공]",	SEND_TE_PVP, 13);
							pPlayer->ADD_GOSSIP_ITEM(MI_WNG, "[실버문 침공]",	SEND_TE_PVP, 14);
						}*/
						
						pPlayer->ADD_GOSSIP_ITEM(MI_WNG, "[나그란드: 피의 투기장]",	SEND_TE_PVP, PLAYER_TEAM() == HORDE ? 6 : 15);
						pPlayer->ADD_GOSSIP_ITEM(MI_WNG, "[줄드락: 고뇌의 투기장]",	SEND_TE_PVP, PLAYER_TEAM() == HORDE ? 7 : 16);
						pPlayer->ADD_GOSSIP_ITEM(MI_WNG, "[겨울손아귀 호수]",	SEND_TE_PVP, PLAYER_TEAM() == HORDE ? 8 : 17);
					}
					break;

				case PG_D_AND_R:		// Dungeons And Raids Menu
					{
						pPlayer->ADD_GOSSIP_ITEM(MI_WNG, "[아제로스 인던]",	SEND_TRAVEL, PG_AZ_D_P1);
						pPlayer->ADD_GOSSIP_ITEM(MI_WNG, "[아제로스 공격대]",	SEND_TRAVEL, PG_AZ_R);
						pPlayer->ADD_GOSSIP_ITEM(MI_WNG, "[아웃랜드 인던]",	SEND_TRAVEL, PG_OL_D_P1);
						pPlayer->ADD_GOSSIP_ITEM(MI_WNG, "[아웃랜드 공격대]",	SEND_TRAVEL, PG_OL_R);
						pPlayer->ADD_GOSSIP_ITEM(MI_WNG, "[노스렌드 인던]",	SEND_TRAVEL, PG_NR_D_P1);
						pPlayer->ADD_GOSSIP_ITEM(MI_WNG, "[노스렌드 공격대]",	SEND_TRAVEL, PG_NR_R);
						/* pPlayer->ADD_GOSSIP_ITEM(MI_WNG, "[대격변 인던]",	SEND_TRAVEL, PG_CY_D);
						pPlayer->ADD_GOSSIP_ITEM(MI_WNG, "[대격변 공격대]",	SEND_TRAVEL, PG_CY_R); */
					}
					break;
				}
			}

			// Pages accessed via Town Travel Menu
			if (iAction == PG_AZ_T || iAction == PG_OL_T || iAction == PG_NR_T || iAction == PG_CY_T)
			{
				GOSSIP_CLEAR();
				pPlayer->ADD_GOSSIP_ITEM(MI_CHT, GoBack,	SEND_TRAVEL, PG_TOWN_T);

				if (iAction == PG_AZ_T)
				{
					if (PLAYER_TEAM() == HORDE || pPlayer->IsGameMaster() )
						pPlayer->ADD_GOSSIP_ITEM(MI_WNG, "[호드 마을]",	SEND_TRAVEL, PG_AZ_F_P1);
					if (PLAYER_TEAM() == ALLIANCE || pPlayer->IsGameMaster() )
						pPlayer->ADD_GOSSIP_ITEM(MI_WNG, "[얼라이언스 마을]",SEND_TRAVEL, PG_AZ_F_P1);

					pPlayer->ADD_GOSSIP_ITEM(MI_WNG, "[중립 마을]",		SEND_TRAVEL, PG_AZ_N);
				}

				if (iAction == PG_OL_T)
				{
					if (PLAYER_TEAM() == HORDE || pPlayer->IsGameMaster() )
						pPlayer->ADD_GOSSIP_ITEM(MI_WNG, "[호드 마을]",	SEND_TRAVEL, PG_OL_F);
					if (PLAYER_TEAM() == ALLIANCE || pPlayer->IsGameMaster() )
						pPlayer->ADD_GOSSIP_ITEM(MI_WNG, "[얼라이언스 마을]",SEND_TRAVEL, PG_OL_F);

					pPlayer->ADD_GOSSIP_ITEM(MI_WNG, "[중립 마을]",		SEND_TRAVEL, PG_OL_N);
				}

				if (iAction == PG_NR_T)
				{
					if (PLAYER_TEAM() == HORDE || pPlayer->IsGameMaster() )
						pPlayer->ADD_GOSSIP_ITEM(MI_WNG, "[호드 마을]",	SEND_TRAVEL, PG_NR_F_P1);
					if (PLAYER_TEAM() == ALLIANCE || pPlayer->IsGameMaster() )
						pPlayer->ADD_GOSSIP_ITEM(MI_WNG, "[얼라이언스 마을]",SEND_TRAVEL, PG_NR_F_P1);

					pPlayer->ADD_GOSSIP_ITEM(MI_WNG, "[중립 마을]",		SEND_TRAVEL, PG_NR_N_P1);
				}
				if (iAction == PG_CY_T)
				{
					pPlayer->ADD_GOSSIP_ITEM(MI_WNG, "[놀드랏실 : 하이잘 산]", SEND_TE_T_CY, 0);
					pPlayer->ADD_GOSSIP_ITEM(MI_WNG, "[골드린의 제단 : 하이잘 산]", SEND_TE_T_CY, 1);
					pPlayer->ADD_GOSSIP_ITEM(MI_WNG, "[이바아나의 제단  : 하이잘 산]", SEND_TE_T_CY, 2);
					pPlayer->ADD_GOSSIP_ITEM(MI_WNG, "[말로른의 성역 : 하이잘 산]", SEND_TE_T_CY, 3);
					pPlayer->ADD_GOSSIP_ITEM(MI_WNG, "[탄원의 용광로 : 하이잘 산]", SEND_TE_T_CY, 4);
					pPlayer->ADD_GOSSIP_ITEM(MI_WNG, "[소단관문 : 하이잘 산]", SEND_TE_T_CY, 5);
					pPlayer->ADD_GOSSIP_ITEM(MI_WNG, "[대지의 사원 : 심원의 영지]", SEND_TE_T_CY, 6);
					pPlayer->ADD_GOSSIP_ITEM(MI_WNG, "[테라제인의 옥좌 : 심원의 영지]", SEND_TE_T_CY, 7);
					pPlayer->ADD_GOSSIP_ITEM(MI_WNG, "[람카헨 : 울둠]", SEND_TE_T_CY, 8);
					pPlayer->ADD_GOSSIP_ITEM(MI_WNG, "[슈노츠 비행기지 : 울둠]", SEND_TE_T_CY, 9);
				}
			}

			// Pages accessed via Azeroth Town Travel Menu
			if (iAction == PG_AZ_F_P1 || iAction == PG_AZ_F_P2 || iAction == PG_AZ_N)
			{
				GOSSIP_CLEAR();
				pPlayer->ADD_GOSSIP_ITEM(MI_CHT, GoBack,	SEND_TRAVEL, PG_AZ_T);

				if (iAction == PG_AZ_F_P1)
				{
					pPlayer->ADD_GOSSIP_ITEM(MI_CHD, "[다음 페이지] -->",		SEND_TRAVEL, PG_AZ_F_P2);

					if (PLAYER_TEAM() == HORDE || pPlayer->IsGameMaster() )
					{
						pPlayer->ADD_GOSSIP_ITEM(MI_WNG, "[피멍울 초소]",			SEND_TE_T_AZ, 0);
						pPlayer->ADD_GOSSIP_ITEM(MI_WNG, "[담쟁이 마을]",			SEND_TE_T_AZ, 1);
						pPlayer->ADD_GOSSIP_ITEM(MI_WNG, "[페랄라스: 모자케 야영지]",		SEND_TE_T_AZ, 2);
						pPlayer->ADD_GOSSIP_ITEM(MI_WNG, "[불모의 땅: 타우라조 야영지]",		SEND_TE_T_AZ, 3);
						pPlayer->ADD_GOSSIP_ITEM(MI_WNG, "[화염 마루]",				SEND_TE_T_AZ, 4);
						pPlayer->ADD_GOSSIP_ITEM(MI_WNG, "[버섯구름 봉우리: 높새바람 봉우리]",	SEND_TE_T_AZ, 5);
						pPlayer->ADD_GOSSIP_ITEM(MI_WNG, "[가시덤불 골짜기: 그롬골 주둔지]",	SEND_TE_T_AZ, 6);
						pPlayer->ADD_GOSSIP_ITEM(MI_WNG, "[아라시 고원: 해머폴]",			SEND_TE_T_AZ, 7);
						pPlayer->ADD_GOSSIP_ITEM(MI_WNG, "[황야의 땅: 카르가스]",			SEND_TE_T_AZ, 8);
						pPlayer->ADD_GOSSIP_ITEM(MI_WNG, "[동부 내륙지: 레반터스크 마을]",		SEND_TE_T_AZ, 9);
						pPlayer->ADD_GOSSIP_ITEM(MI_WNG, "[잊혀진 땅: 그늘수렵 마을]",		SEND_TE_T_AZ, 10);
						pPlayer->ADD_GOSSIP_ITEM(MI_WNG, "[잿빛 골짜기: 토막나무 주둔지]",		SEND_TE_T_AZ, 11);
						pPlayer->ADD_GOSSIP_ITEM(MI_WNG, "[슬픔의 늪: 스토나드]",			SEND_TE_T_AZ, 12);
					}
					if (PLAYER_TEAM() == ALLIANCE || pPlayer->IsGameMaster() )
					{
						pPlayer->ADD_GOSSIP_ITEM(MI_WNG, "[동부내륙지: 맹금의 봉우리]",		SEND_TE_T_AZ, 20);
						pPlayer->ADD_GOSSIP_ITEM(MI_WNG, "[잿빛골짜기: 아스트라나르]",		SEND_TE_T_AZ, 21);
						pPlayer->ADD_GOSSIP_ITEM(MI_WNG, "[어둠의 해안: 아우버다인]",		SEND_TE_T_AZ, 22);
						pPlayer->ADD_GOSSIP_ITEM(MI_WNG, "[핏빛안개 섬: 핏빛 감시초소]",		SEND_TE_T_AZ, 23);
						pPlayer->ADD_GOSSIP_ITEM(MI_WNG, "[서리바람 야영지]",			SEND_TE_T_AZ, 24);
						pPlayer->ADD_GOSSIP_ITEM(MI_WNG, "[그늘 숲: 다크샤이어]",			SEND_TE_T_AZ, 25);
						pPlayer->ADD_GOSSIP_ITEM(MI_WNG, "[페랄라스: 페더문 요새]",		SEND_TE_T_AZ, 26);
						pPlayer->ADD_GOSSIP_ITEM(MI_WNG, "[붉은마루 산맥: 레이크샤이어]",		SEND_TE_T_AZ, 27);
						pPlayer->ADD_GOSSIP_ITEM(MI_WNG, "[저습지: 메네실 항구]",			SEND_TE_T_AZ, 28);
						pPlayer->ADD_GOSSIP_ITEM(MI_WNG, "[저주받은 땅: 네더가드 요새]",			SEND_TE_T_AZ, 29);
						pPlayer->ADD_GOSSIP_ITEM(MI_WNG, "[잊혀진 땅: 나이젤의 야영지]",		SEND_TE_T_AZ, 30);
						pPlayer->ADD_GOSSIP_ITEM(MI_WNG, "[반란군 야영지]",			SEND_TE_T_AZ, 31);
						pPlayer->ADD_GOSSIP_ITEM(MI_WNG, "[임시 주둔지]",			SEND_TE_T_AZ, 32);
					}
				}

				if (iAction == PG_AZ_F_P2)
				{
					pPlayer->ADD_GOSSIP_ITEM(MI_CHD, "<-- [이전 페이지]",		SEND_TRAVEL, PG_AZ_F_P1);

					if (PLAYER_TEAM() == HORDE || pPlayer->IsGameMaster() )
					{
						pPlayer->ADD_GOSSIP_ITEM(MI_WNG, "[돌발톱 산맥: 해바위 야영지]",		SEND_TE_T_AZ, 13);
						pPlayer->ADD_GOSSIP_ITEM(MI_WNG, "[힐스브래드 구릉지: 타렌 밀농장]",	SEND_TE_T_AZ, 14);
						pPlayer->ADD_GOSSIP_ITEM(MI_WNG, "[불모의 땅: 크로스로드]",		SEND_TE_T_AZ, 15);
						pPlayer->ADD_GOSSIP_ITEM(MI_WNG, "[은빛소나무 숲: 공동묘지]",		SEND_TE_T_AZ, 16);
						pPlayer->ADD_GOSSIP_ITEM(MI_WNG, "[유령의 땅: 트랜퀼리엔]",		SEND_TE_T_AZ, 17);
						pPlayer->ADD_GOSSIP_ITEM(MI_WNG, "[아즈샤라: 발로르모크]",			SEND_TE_T_AZ, 18);
						pPlayer->ADD_GOSSIP_ITEM(MI_WNG, "[잿빛 골짜기: 조람가르 전초기지]",	SEND_TE_T_AZ, 19);
					}
					if (PLAYER_TEAM() == ALLIANCE || pPlayer->IsGameMaster() )
					{
						pPlayer->ADD_GOSSIP_ITEM(MI_WNG, "[서부 몰락지대: 감시의 언덕]",		SEND_TE_T_AZ, 33);
						pPlayer->ADD_GOSSIP_ITEM(MI_WNG, "[힐스브래드 구릉지: 사우스쇼어]",		SEND_TE_T_AZ, 34);
						pPlayer->ADD_GOSSIP_ITEM(MI_WNG, "[돌발톱 산맥: 돌발톱 봉우리]",		SEND_TE_T_AZ, 35);
						pPlayer->ADD_GOSSIP_ITEM(MI_WNG, "[갈퀴가지 숲]",			SEND_TE_T_AZ, 36);
						pPlayer->ADD_GOSSIP_ITEM(MI_WNG, "[탈렌드리스 초소]",			SEND_TE_T_AZ, 37);
						pPlayer->ADD_GOSSIP_ITEM(MI_WNG, "[탈라나르]",				SEND_TE_T_AZ, 38);
						pPlayer->ADD_GOSSIP_ITEM(MI_WNG, "[모단 호수: 텔사마]",			SEND_TE_T_AZ, 39);
						pPlayer->ADD_GOSSIP_ITEM(MI_WNG, "[먼지진흙 습지대: 테라모아 섬]",		SEND_TE_T_AZ, 40);
					}
				}

				if (iAction == PG_AZ_N)
				{
					pPlayer->ADD_GOSSIP_ITEM(MI_WNG, "[가시덤불 골짜기: 무법항]",			SEND_TE_T_AZ, 41);
					pPlayer->ADD_GOSSIP_ITEM(MI_WNG, "[실리더스: 세나리온 요새]",			SEND_TE_T_AZ, 42);
					pPlayer->ADD_GOSSIP_ITEM(MI_WNG, "[에메랄드 성소]",				SEND_TE_T_AZ, 43);
					pPlayer->ADD_GOSSIP_ITEM(MI_WNG, "[여명의 설원: 눈망루 마을]",			SEND_TE_T_AZ, 44);
					pPlayer->ADD_GOSSIP_ITEM(MI_WNG, "[타나리스: 가젯잔]",				SEND_TE_T_AZ, 45);
					pPlayer->ADD_GOSSIP_ITEM(MI_WNG, "[쿠엘다나스 섬]",				SEND_TE_T_AZ, 46);
					pPlayer->ADD_GOSSIP_ITEM(MI_WNG, "[동부 역병지대: 희망의 빛 예배당]",		SEND_TE_T_AZ, 47);
					pPlayer->ADD_GOSSIP_ITEM(MI_WNG, "[마샬의 야영지]",				SEND_TE_T_AZ, 48);
					pPlayer->ADD_GOSSIP_ITEM(MI_WNG, "[달의 숲]",					SEND_TE_T_AZ, 49);
					pPlayer->ADD_GOSSIP_ITEM(MI_WNG, "[진흙톱니 거점]",				SEND_TE_T_AZ, 50);
					pPlayer->ADD_GOSSIP_ITEM(MI_WNG, "[불모의 땅: 톱니항]",				SEND_TE_T_AZ, 51);
					pPlayer->ADD_GOSSIP_ITEM(MI_WNG, "[토륨 조합 거점]",				SEND_TE_T_AZ, 52);
				}
			}

			// Pages accessed via Outland Town Travel Menu
			if (iAction == PG_OL_F || iAction == PG_OL_N)
			{
				GOSSIP_CLEAR();
				pPlayer->ADD_GOSSIP_ITEM(MI_CHT, GoBack,	SEND_TRAVEL, PG_OL_T);

				if (iAction == PG_OL_F)
				{
					if (PLAYER_TEAM() == HORDE || pPlayer->IsGameMaster() )
					{
						pPlayer->ADD_GOSSIP_ITEM(MI_WNG, "[지옥불 반도: 매의 감시탑]",		SEND_TE_T_OL, 0);
						pPlayer->ADD_GOSSIP_ITEM(MI_WNG, "[나그란드: 가라다르]",			SEND_TE_T_OL, 1);
						pPlayer->ADD_GOSSIP_ITEM(MI_WNG, "[칼날 산맥: 모크나탈 마을]",		SEND_TE_T_OL, 2);
						pPlayer->ADD_GOSSIP_ITEM(MI_WNG, "[어둠달 골짜기: 어둠달 마을]",		SEND_TE_T_OL, 3);
						pPlayer->ADD_GOSSIP_ITEM(MI_WNG, "[지옥불 반도: 해골망치 초소]",		SEND_TE_T_OL, 4);
						pPlayer->ADD_GOSSIP_ITEM(MI_WNG, "[테로카르 숲: 돌망치 요새]",		SEND_TE_T_OL, 5);	
						pPlayer->ADD_GOSSIP_ITEM(MI_WNG, "[장가르 습지대: 늪쥐 감시초소]",		SEND_TE_T_OL, 6);
						pPlayer->ADD_GOSSIP_ITEM(MI_WNG, "[지옥불 반도: 스랄마]",			SEND_TE_T_OL, 7);
						pPlayer->ADD_GOSSIP_ITEM(MI_WNG, "[칼날 산맥: 천둥군주 요새]",		SEND_TE_T_OL, 8);
						pPlayer->ADD_GOSSIP_ITEM(MI_WNG, "[장가르 습지대: 자브라진]",		SEND_TE_T_OL, 9);
					}
					if (PLAYER_TEAM() == ALLIANCE || pPlayer->IsGameMaster() )
					{
						pPlayer->ADD_GOSSIP_ITEM(MI_WNG, "[알레리아 성채]",			SEND_TE_T_OL, 10);
						pPlayer->ADD_GOSSIP_ITEM(MI_WNG, "[명예의 요새]",			SEND_TE_T_OL, 11);
						pPlayer->ADD_GOSSIP_ITEM(MI_WNG, "[오레보르 피난처]",			SEND_TE_T_OL, 12);
						pPlayer->ADD_GOSSIP_ITEM(MI_WNG, "[징검다리 거점]",			SEND_TE_T_OL, 13);
						pPlayer->ADD_GOSSIP_ITEM(MI_WNG, "[실바나르]",				SEND_TE_T_OL, 14);
						pPlayer->ADD_GOSSIP_ITEM(MI_WNG, "[텔라아르]",				SEND_TE_T_OL, 15);
						pPlayer->ADD_GOSSIP_ITEM(MI_WNG, "[텔레도르]",				SEND_TE_T_OL, 16);
						pPlayer->ADD_GOSSIP_ITEM(MI_WNG, "[텔하마트 사원]",			SEND_TE_T_OL, 17);
						pPlayer->ADD_GOSSIP_ITEM(MI_WNG, "[토쉴리의 연구기지]",			SEND_TE_T_OL, 18);
						pPlayer->ADD_GOSSIP_ITEM(MI_WNG, "[와일드해머 성채]",			SEND_TE_T_OL, 19);
					}
				}

				if (iAction == PG_OL_N)
				{
					pPlayer->ADD_GOSSIP_ITEM(MI_WNG, "[샤타르 제단]",				SEND_TE_T_OL, 20);
					pPlayer->ADD_GOSSIP_ITEM(MI_WNG, "[52 구역]",					SEND_TE_T_OL, 21);
					pPlayer->ADD_GOSSIP_ITEM(MI_WNG, "[검은바람 비행기지]",				SEND_TE_T_OL, 22);
					pPlayer->ADD_GOSSIP_ITEM(MI_WNG, "[세나리온 야영지]",				SEND_TE_T_OL, 23);
					pPlayer->ADD_GOSSIP_ITEM(MI_WNG, "[코스모렌치]",					SEND_TE_T_OL, 24);
					pPlayer->ADD_GOSSIP_ITEM(MI_WNG, "[영원의 숲]",					SEND_TE_T_OL, 25);
					pPlayer->ADD_GOSSIP_ITEM(MI_WNG, "[오그릴라]",					SEND_TE_T_OL, 26);
					pPlayer->ADD_GOSSIP_ITEM(MI_WNG, "[별의 성소]",					SEND_TE_T_OL, 27);
					pPlayer->ADD_GOSSIP_ITEM(MI_WNG, "[폭풍 첨탑]",					SEND_TE_T_OL, 28);
					pPlayer->ADD_GOSSIP_ITEM(MI_WNG, "[운명의 계단]",				SEND_TE_T_OL, 29);
				}
			}

			// Pages accessed via Northrend Town Travel Menu
			if (iAction == PG_NR_F_P1 || iAction == PG_NR_F_P2 || iAction == PG_NR_N_P1 || iAction == PG_NR_N_P2)
			{
				GOSSIP_CLEAR();
				pPlayer->ADD_GOSSIP_ITEM(MI_CHT, GoBack,	SEND_TRAVEL, PG_NR_T);

				if (iAction == PG_NR_F_P1)
				{
					if (PLAYER_TEAM() == HORDE || pPlayer->IsGameMaster() )
					{
						pPlayer->ADD_GOSSIP_ITEM(MI_CHD, "[다음 페이지] -->",	SEND_TRAVEL, PG_NR_F_P2);

						pPlayer->ADD_GOSSIP_ITEM(MI_WNG, "[울부짖는 협만: 연금술사 야영지]",	SEND_TE_T_NR, 1);
						pPlayer->ADD_GOSSIP_ITEM(MI_WNG, "[울부짖는 협만: 겨울발굽 야영지]",	SEND_TE_T_NR, 5);
						pPlayer->ADD_GOSSIP_ITEM(MI_WNG, "[울부짖는 협만: 신 아가만드]",		SEND_TE_T_NR, 10);
						pPlayer->ADD_GOSSIP_ITEM(MI_WNG, "[울부짖는 협만: 복수의 상륙지)]",		SEND_TE_T_NR, 13);
						pPlayer->ADD_GOSSIP_ITEM(MI_WNG, "[북풍의 땅: 보르고로크 전초기지]",	SEND_TE_T_NR, 2);
						pPlayer->ADD_GOSSIP_ITEM(MI_WNG, "[북풍의 땅: 타운카르 마을]",		SEND_TE_T_NR, 12);
						pPlayer->ADD_GOSSIP_ITEM(MI_WNG, "[북풍의 땅: 전쟁노래부족 요새]",		SEND_TE_T_NR, 15);
						pPlayer->ADD_GOSSIP_ITEM(MI_WNG, "[용의 안식처: 아그마르의 망치]",		SEND_TE_T_NR, 0);
						pPlayer->ADD_GOSSIP_ITEM(MI_WNG, "[용의 안식처: 코르크론 선봉기지]",	SEND_TE_T_NR, 9);
						pPlayer->ADD_GOSSIP_ITEM(MI_WNG, "[용의 안식처: 원한의 초소]",		SEND_TE_T_NR, 14);
						pPlayer->ADD_GOSSIP_ITEM(MI_WNG, "[회색 구릉지: 원크와 야영지]",		SEND_TE_T_NR, 3);
						pPlayer->ADD_GOSSIP_ITEM(MI_WNG, "[회색 구릉지: 정복의 요새]",		SEND_TE_T_NR, 6);
						pPlayer->ADD_GOSSIP_ITEM(MI_WNG, "[줄드락: 두브라진]",			SEND_TE_T_NR, 7);
					}
					if (PLAYER_TEAM() == ALLIANCE || pPlayer->IsGameMaster() )
					{
						pPlayer->ADD_GOSSIP_ITEM(MI_WNG, "[울부짖는 협만: 빌더바르 요새]",		SEND_TE_T_NR, 19);
						pPlayer->ADD_GOSSIP_ITEM(MI_WNG, "[울부짖는 협만: 발가드]",		SEND_TE_T_NR, 22);
						pPlayer->ADD_GOSSIP_ITEM(MI_WNG, "[울부짖는 협만: 서부경비대 성채]",	SEND_TE_T_NR, 25);
						pPlayer->ADD_GOSSIP_ITEM(MI_WNG, "[북풍의 땅: 피즈크랭크 비행장]",		SEND_TE_T_NR, 17);
						pPlayer->ADD_GOSSIP_ITEM(MI_WNG, "[북풍의 땅: 용맹의 성채]",		SEND_TE_T_NR, 23);
						pPlayer->ADD_GOSSIP_ITEM(MI_WNG, "[용의 안식처: 폴드라곤 요새]",		SEND_TE_T_NR, 18);
						pPlayer->ADD_GOSSIP_ITEM(MI_WNG, "[용의 안식처: 별의 쉼터]",		SEND_TE_T_NR, 21);
						pPlayer->ADD_GOSSIP_ITEM(MI_WNG, "[용의 안식처: 윈터가드 성채]",		SEND_TE_T_NR, 27);
						pPlayer->ADD_GOSSIP_ITEM(MI_WNG, "[회색 구릉지: 호박빛소나무 오두막]",	SEND_TE_T_NR, 16);
						pPlayer->ADD_GOSSIP_ITEM(MI_WNG, "[회색 구릉지: 서부 몰락지대 여단 야영지]",	SEND_TE_T_NR, 24);
						pPlayer->ADD_GOSSIP_ITEM(MI_WNG, "[퐁풍우 봉우리: 서리요새]",		SEND_TE_T_NR, 20);
						pPlayer->ADD_GOSSIP_ITEM(MI_WNG, "[수정노래 숲: 윈드러너 전망대]",			SEND_TE_T_NR, 26);
					}
				}

				if (iAction == PG_NR_F_P2)
				{
					pPlayer->ADD_GOSSIP_ITEM(MI_CHD, "<-- [이전 페이지]",		SEND_TRAVEL, PG_NR_F_P1);

					if (PLAYER_TEAM() == HORDE || pPlayer->IsGameMaster() )
					{
						pPlayer->ADD_GOSSIP_ITEM(MI_WNG, "[폭풍우 봉우리: 그롬아쉬 추락 지점]",	SEND_TE_T_NR, 8);
						pPlayer->ADD_GOSSIP_ITEM(MI_WNG, "[폭풍우 봉우리: 툰카로 야영지]",		SEND_TE_T_NR, 4);
						pPlayer->ADD_GOSSIP_ITEM(MI_WNG, "[수정노래 숲: 선리버 지휘초소]",			SEND_TE_T_NR, 11);
					}
					if (PLAYER_TEAM() == ALLIANCE)
					{
					}
				}

				if (iAction == PG_NR_N_P1)
				{
					pPlayer->ADD_GOSSIP_ITEM(MI_CHD, "[다음 페이지] -->",		SEND_TRAVEL, PG_NR_N_P2);

					pPlayer->ADD_GOSSIP_ITEM(MI_WNG, "[울부짖는 협만: 카마구아]",			SEND_TE_T_NR, 36);
					pPlayer->ADD_GOSSIP_ITEM(MI_WNG, "[북풍의 땅: 호박석 절벽]",			SEND_TE_T_NR, 28);
					pPlayer->ADD_GOSSIP_ITEM(MI_WNG, "[북풍의 땅: 변위의 보호막]",			SEND_TE_T_NR, 44);
					pPlayer->ADD_GOSSIP_ITEM(MI_WNG, "[북풍의 땅: 우누페]",				SEND_TE_T_NR, 45);
					pPlayer->ADD_GOSSIP_ITEM(MI_WNG, "[용의 안식처: 모아키 항구]",			SEND_TE_T_NR, 39);
					pPlayer->ADD_GOSSIP_ITEM(MI_WNG, "[용의 안식처: 고룡쉼터 사원]",			SEND_TE_T_NR, 46);
					pPlayer->ADD_GOSSIP_ITEM(MI_WNG, "[줄드락: 빛의 틈)]",				SEND_TE_T_NR, 38);
					pPlayer->ADD_GOSSIP_ITEM(MI_WNG, "[줄드락: 칠흑의 감시초소]",			SEND_TE_T_NR, 34);
					pPlayer->ADD_GOSSIP_ITEM(MI_WNG, "[줄드락: 은빛십자군 격전지)]",			SEND_TE_T_NR, 41);
					pPlayer->ADD_GOSSIP_ITEM(MI_WNG, "[줄드락: 짐토르가]",				SEND_TE_T_NR, 47);
					pPlayer->ADD_GOSSIP_ITEM(MI_WNG, "[숄라자르 분지: 호반의 착륙장]",			SEND_TE_T_NR, 37);
					pPlayer->ADD_GOSSIP_ITEM(MI_WNG, "[숄라자르 분지: 네싱워리 주둔지]",		SEND_TE_T_NR, 40);
					pPlayer->ADD_GOSSIP_ITEM(MI_WNG, "[폭풍우 봉우리: 던 니펠렘]",			SEND_TE_T_NR, 33);
				}

				if (iAction == PG_NR_N_P2)
				{
					pPlayer->ADD_GOSSIP_ITEM(MI_CHD, "<-- [이전 페이지]",		SEND_TRAVEL, PG_NR_N_P1);

					pPlayer->ADD_GOSSIP_ITEM(MI_WNG, "[폭풍우 봉우리: K3]",				SEND_TE_T_NR, 35);
					pPlayer->ADD_GOSSIP_ITEM(MI_WNG, "[폭풍우 봉우리: 바울더크랙의 은거처]",		SEND_TE_T_NR, 30);
					pPlayer->ADD_GOSSIP_ITEM(MI_WNG, "[얼음왕관: 은빛십자군 선봉기지]",		SEND_TE_T_NR, 42);
					pPlayer->ADD_GOSSIP_ITEM(MI_WNG, "[얼음왕관: 어둠의 무기고]",			SEND_TE_T_NR, 43);
					pPlayer->ADD_GOSSIP_ITEM(MI_WNG, "[얼음왕관: 은빛십자군 마상시합 광장]",		SEND_TE_T_NR, 29);
					pPlayer->ADD_GOSSIP_ITEM(MI_WNG, "[얼음왕관: 십자군 봉우리]",			SEND_TE_T_NR, 31);
					pPlayer->ADD_GOSSIP_ITEM(MI_WNG, "[얼음왕관: 죽음의 마루]",			SEND_TE_T_NR, 32);
				}
			}

			// Pages accessed via Dungeons And Raids Menu
			if (iAction == PG_AZ_D_P1 || iAction == PG_AZ_D_P2 || iAction == PG_AZ_R ||
				iAction == PG_OL_D_P1 || iAction == PG_OL_D_P2 || iAction == PG_OL_R ||
				iAction == PG_NR_D_P1 || iAction == PG_NR_D_P2 || iAction == PG_NR_R ||
				iAction == PG_CY_D || iAction == PG_CY_R)
			{
				GOSSIP_CLEAR();
				pPlayer->ADD_GOSSIP_ITEM(MI_CHT, GoBack,	SEND_TRAVEL, PG_D_AND_R);

				if (iAction == PG_AZ_D_P1)
				{
					pPlayer->ADD_GOSSIP_ITEM(MI_CHD, "[다음 페이지] -->",		SEND_TRAVEL, PG_AZ_D_P2);

					pPlayer->ADD_GOSSIP_ITEM(MI_WNG, "[죽음의 폐광]",		SEND_TE_D_AZ, 13);
					pPlayer->ADD_GOSSIP_ITEM(MI_WNG, "[통곡의 동굴]",		SEND_TE_D_AZ, 15);
					pPlayer->ADD_GOSSIP_ITEM(MI_WNG, "[그림자송곳니 성채]",		SEND_TE_D_AZ, 10);
					pPlayer->ADD_GOSSIP_ITEM(MI_WNG, "[검은심연의 나락]",		SEND_TE_D_AZ, 0);
					pPlayer->ADD_GOSSIP_ITEM(MI_WNG, "[가시덩굴 우리]",		SEND_TE_D_AZ, 7);	
					pPlayer->ADD_GOSSIP_ITEM(MI_WNG, "[놈리건]",			SEND_TE_D_AZ, 4);
					pPlayer->ADD_GOSSIP_ITEM(MI_WNG, "[붉은십자군 수도원]",		SEND_TE_D_AZ, 8);
					pPlayer->ADD_GOSSIP_ITEM(MI_WNG, "[울다만]",			SEND_TE_D_AZ, 14);
					pPlayer->ADD_GOSSIP_ITEM(MI_WNG, "[가시덩굴 구릉]",		SEND_TE_D_AZ, 6);  
					pPlayer->ADD_GOSSIP_ITEM(MI_WNG, "[마라우돈]",			SEND_TE_D_AZ, 5);
					pPlayer->ADD_GOSSIP_ITEM(MI_WNG, "[줄파락]",			SEND_TE_D_AZ, 16);
					pPlayer->ADD_GOSSIP_ITEM(MI_WNG, "[아탈학카르 신전]",		SEND_TE_D_AZ, 12);
					pPlayer->ADD_GOSSIP_ITEM(MI_WNG, "[스트라솔름]",		SEND_TE_D_AZ, 11);
				}

				if (iAction == PG_AZ_D_P2)
				{
					pPlayer->ADD_GOSSIP_ITEM(MI_CHD, "<-- [이전 페이지]",		SEND_TRAVEL, PG_AZ_D_P1);

					pPlayer->ADD_GOSSIP_ITEM(MI_WNG, "[검은바위 첨탑하층]",		SEND_TE_D_AZ, 1);
					pPlayer->ADD_GOSSIP_ITEM(MI_WNG, "[검은바위 첨탑상층]",		SEND_TE_D_AZ, 2);
					pPlayer->ADD_GOSSIP_ITEM(MI_WNG, "[혈투의 전장]",		SEND_TE_D_AZ, 3);
					pPlayer->ADD_GOSSIP_ITEM(MI_WNG, "[스칼로맨스]",		SEND_TE_D_AZ, 9);
				}

				if (iAction == PG_AZ_R)
				{
					pPlayer->ADD_GOSSIP_ITEM(MI_WNG, "[검은날개 둥지(10인)]",			SEND_TE_D_AZ, 17);
					pPlayer->ADD_GOSSIP_ITEM(MI_WNG, "[화산심장부(10인)]",			SEND_TE_D_AZ, 18);
					pPlayer->ADD_GOSSIP_ITEM(MI_WNG, "[안퀴라즈 폐허(5인)]",			SEND_TE_D_AZ, 19);
					pPlayer->ADD_GOSSIP_ITEM(MI_WNG, "[안퀴라즈 사원(10인)]",			SEND_TE_D_AZ, 20);
					pPlayer->ADD_GOSSIP_ITEM(MI_WNG, "[줄구룹(5인)]",			SEND_TE_D_AZ, 21);
				}

				if (iAction == PG_OL_D_P1)
				{
					pPlayer->ADD_GOSSIP_ITEM(MI_CHD, "[다음 페이지] -->",		SEND_TRAVEL, PG_OL_D_P2);

					pPlayer->ADD_GOSSIP_ITEM(MI_WNG, "[지옥불 성채: 지옥불 성루]",		SEND_TE_D_OL, 9);
					pPlayer->ADD_GOSSIP_ITEM(MI_WNG, "[지옥불 성채: 피의 용광로]",		SEND_TE_D_OL, 10);
					pPlayer->ADD_GOSSIP_ITEM(MI_WNG, "[갈퀴송곳니 저수지: 강제 노역소]",	SEND_TE_D_OL, 6);
					pPlayer->ADD_GOSSIP_ITEM(MI_WNG, "[갈퀴송곳니 저수지: 지하수렁]",		SEND_TE_D_OL, 8);
					pPlayer->ADD_GOSSIP_ITEM(MI_WNG, "[아킨돈: 마나 무덤]",			SEND_TE_D_OL, 1);
					pPlayer->ADD_GOSSIP_ITEM(MI_WNG, "[시간의 동굴: 옛 힐스브래드 구릉지]",	SEND_TE_D_OL, 4);
					pPlayer->ADD_GOSSIP_ITEM(MI_WNG, "[아킨돈: 아키나이 납골당]",		SEND_TE_D_OL, 0);
					pPlayer->ADD_GOSSIP_ITEM(MI_WNG, "[아킨돈: 세테크 전당]",			SEND_TE_D_OL, 2);
					pPlayer->ADD_GOSSIP_ITEM(MI_WNG, "[아킨돈: 어둠의 미궁]",			SEND_TE_D_OL, 3);
					pPlayer->ADD_GOSSIP_ITEM(MI_WNG, "[시간의 동굴: 검은 늪]",			SEND_TE_D_OL, 5);
					pPlayer->ADD_GOSSIP_ITEM(MI_WNG, "[갈퀴송곳니 저수지: 증기 저장고]",		SEND_TE_D_OL, 7);
					pPlayer->ADD_GOSSIP_ITEM(MI_WNG, "[지옥불 성채: 으스러진 손의 전당]",	SEND_TE_D_OL, 11);
					pPlayer->ADD_GOSSIP_ITEM(MI_WNG, "[마법학자의 정원]",			SEND_TE_D_OL, 12);
				}

				if (iAction == PG_OL_D_P2)
				{
					pPlayer->ADD_GOSSIP_ITEM(MI_CHD, "<-- [이전 페이지]",		SEND_TRAVEL, PG_OL_D_P1);

					pPlayer->ADD_GOSSIP_ITEM(MI_WNG, "[폭풍우 요새: 알카트라즈]",	SEND_TE_D_OL, 13);
					pPlayer->ADD_GOSSIP_ITEM(MI_WNG, "[폭풍우 요새: 신록의 정원]",	SEND_TE_D_OL, 14);
					pPlayer->ADD_GOSSIP_ITEM(MI_WNG, "[폭풍우 요새: 메카나르]",		SEND_TE_D_OL, 15);
				}

				if (iAction == PG_OL_R)
				{	
					pPlayer->ADD_GOSSIP_ITEM(MI_WNG, "[카라잔]",				SEND_TE_D_OL, 20);
					pPlayer->ADD_GOSSIP_ITEM(MI_WNG, "[줄아만]",				SEND_TE_D_OL, 24);
					pPlayer->ADD_GOSSIP_ITEM(MI_WNG, "[지옥불 성채: 마그테리돈의 둥지]",	SEND_TE_D_OL, 19);
					pPlayer->ADD_GOSSIP_ITEM(MI_WNG, "[그룰의 둥지]",			SEND_TE_D_OL, 18);
					pPlayer->ADD_GOSSIP_ITEM(MI_WNG, "[불뱀 제단]",				SEND_TE_D_OL, 21);
					pPlayer->ADD_GOSSIP_ITEM(MI_WNG, "[폭풍우 요새: 폭풍우 요새]",		SEND_TE_D_OL, 23);
					pPlayer->ADD_GOSSIP_ITEM(MI_WNG, "[시간의 동굴: 하이잘 산의 전투]",		SEND_TE_D_OL, 17);
					pPlayer->ADD_GOSSIP_ITEM(MI_WNG, "[검은 사원]",				SEND_TE_D_OL, 16);
					pPlayer->ADD_GOSSIP_ITEM(MI_WNG, "[태양샘 고원]",			SEND_TE_D_OL, 22);
				}
				
				if (iAction == PG_NR_D_P1)
				{
					pPlayer->ADD_GOSSIP_ITEM(MI_CHD, "[다음 페이지] -->",		SEND_TRAVEL, PG_NR_D_P2);

					pPlayer->ADD_GOSSIP_ITEM(MI_WNG, "[마력의 탑: 마력의 탑]",		SEND_TE_D_NR, 8);
					pPlayer->ADD_GOSSIP_ITEM(MI_WNG, "[우트가드 성채: 우트가드 성채]",		SEND_TE_D_NR, 14);
					pPlayer->ADD_GOSSIP_ITEM(MI_WNG, "[아졸네룹: 아졸네룹]",			SEND_TE_D_NR, 1);
					pPlayer->ADD_GOSSIP_ITEM(MI_WNG, "[안카헤트: 고대 왕국]",			SEND_TE_D_NR, 0);
					pPlayer->ADD_GOSSIP_ITEM(MI_WNG, "[드락타론 성채]",			SEND_TE_D_NR, 3);
					pPlayer->ADD_GOSSIP_ITEM(MI_WNG, "[보랏빛 요새]",			SEND_TE_D_NR, 10);
					pPlayer->ADD_GOSSIP_ITEM(MI_WNG, "[군드락]",				SEND_TE_D_NR, 4);
					pPlayer->ADD_GOSSIP_ITEM(MI_WNG, "[울두아르: 돌의 전당]",			SEND_TE_D_NR, 13);
					pPlayer->ADD_GOSSIP_ITEM(MI_WNG, "[우트가드 성채: 우트가드 첨탑]",		SEND_TE_D_NR, 15);
					pPlayer->ADD_GOSSIP_ITEM(MI_WNG, "[시간의 동굴: 옛 스트라솔름]",		SEND_TE_D_NR, 2);
					pPlayer->ADD_GOSSIP_ITEM(MI_WNG, "[마력의 탑: 마력의 눈]",			SEND_TE_D_NR, 9);
					pPlayer->ADD_GOSSIP_ITEM(MI_WNG, "[울두아르: 번개의 전당]",			SEND_TE_D_NR, 12);
					pPlayer->ADD_GOSSIP_ITEM(MI_WNG, "[용사의 시험장]",			SEND_TE_D_NR, 11);
				}

				if (iAction == PG_NR_D_P2)
				{
					pPlayer->ADD_GOSSIP_ITEM(MI_CHD, "<-- [이전 페이지]",		SEND_TRAVEL, PG_NR_D_P1);

					pPlayer->ADD_GOSSIP_ITEM(MI_WNG, "[얼음왕관 성채: 투영의 전당",		SEND_TE_D_NR, 5);
					pPlayer->ADD_GOSSIP_ITEM(MI_WNG, "[얼음왕관 성채: 사론의 구덩이]",		SEND_TE_D_NR, 6);
					pPlayer->ADD_GOSSIP_ITEM(MI_WNG, "[얼음왕관 성채: 영혼의 제련소]",		SEND_TE_D_NR, 7);
				}

				if (iAction == PG_NR_R)
				{
					//pPlayer->ADD_GOSSIP_ITEM(MI_WNG, "[아카본 석실]",				SEND_TE_D_NR, 24);
					pPlayer->ADD_GOSSIP_ITEM(MI_WNG, "[흑요석 성소]",				SEND_TE_D_NR, 20);
					pPlayer->ADD_GOSSIP_ITEM(MI_WNG, "[영원의 눈]",					SEND_TE_D_NR, 19);
					pPlayer->ADD_GOSSIP_ITEM(MI_WNG, "[낙스라마스]",					SEND_TE_D_NR, 17);
					pPlayer->ADD_GOSSIP_ITEM(MI_WNG, "[울두아르]",					SEND_TE_D_NR, 23);
					pPlayer->ADD_GOSSIP_ITEM(MI_WNG, "[십자군의 시험장]",				SEND_TE_D_NR, 22);
					pPlayer->ADD_GOSSIP_ITEM(MI_WNG, "[오닉시아의 둥지]",				SEND_TE_D_NR, 18);
					pPlayer->ADD_GOSSIP_ITEM(MI_WNG, "[얼음왕관 성채]",				SEND_TE_D_NR, 16);
					pPlayer->ADD_GOSSIP_ITEM(MI_WNG, "[루비 성소]",					SEND_TE_D_NR, 21);
				}
				if (iAction == PG_CY_D)
				{
					pPlayer->ADD_GOSSIP_ITEM(MI_WNG, "[검은바위 동굴]",				SEND_TE_D_CY, 0);
					pPlayer->ADD_GOSSIP_ITEM(MI_WNG, "[파도의 왕좌]",				SEND_TE_D_CY, 1);
					pPlayer->ADD_GOSSIP_ITEM(MI_WNG, "[바위심장부]",					SEND_TE_D_CY, 2);
					pPlayer->ADD_GOSSIP_ITEM(MI_WNG, "[소용돌이 누각]",					SEND_TE_D_CY, 3);
					pPlayer->ADD_GOSSIP_ITEM(MI_WNG, "[시초의 전당]",					SEND_TE_D_CY, 4);
					pPlayer->ADD_GOSSIP_ITEM(MI_WNG, "[돌비르의 잃어버린 도시]",				SEND_TE_D_CY, 5);
					pPlayer->ADD_GOSSIP_ITEM(MI_WNG, "[그림 바툴]",					SEND_TE_D_CY, 6);
					pPlayer->ADD_GOSSIP_ITEM(MI_WNG, "[죽음의 폐광(영)]",				SEND_TE_D_CY, 7);
					pPlayer->ADD_GOSSIP_ITEM(MI_WNG, "[그림자송곳니 성채(영)]",				SEND_TE_D_CY, 8);
					pPlayer->ADD_GOSSIP_ITEM(MI_WNG, "[줄구룹(영)]",					SEND_TE_D_CY, 9);
					pPlayer->ADD_GOSSIP_ITEM(MI_WNG, "[줄아만(영)]",					SEND_TE_D_CY, 10);

				}
				if (iAction == PG_CY_R)
				{
					pPlayer->ADD_GOSSIP_ITEM(MI_WNG, "[검은날개 강림지]",				SEND_TE_D_CY, 11);
					pPlayer->ADD_GOSSIP_ITEM(MI_WNG, "[네 바람의 왕좌]",				SEND_TE_D_CY, 12);
					pPlayer->ADD_GOSSIP_ITEM(MI_WNG, "[황혼의 요새]",					SEND_TE_D_CY, 13);
					pPlayer->ADD_GOSSIP_ITEM(MI_WNG, "[바란딘 요새]",			SEND_TE_D_CY, 14);
				}
			}

			GOSSIP_MENU(700000, pCreature->GetGUID());
		}
		return true;
	};
};

//울부짖는 협만 마음이가는대로
/*############
# Quest 11472
#############*/

enum AttractedReefBullData
{
    NPC_FEMALE_REEF_COW = 24797,
    SPELL_ANUNIAQS_NET = 21014,
    SPELL_TASTY_REEF_FISH = 44454,
    SPELL_LOVE_COSMETIC = 52148,
    ITEM_TASTY_REEF_FISH = 34127,
    QUEST_THE_WAY_TO_HIS_HEART = 11472
};

class npc_attracted_reef_bull : public CreatureScript
{
    public:

        npc_attracted_reef_bull() : CreatureScript("npc_attracted_reef_bull") {}

        struct npc_attracted_reef_bullAI : public ScriptedAI
        {
            npc_attracted_reef_bullAI(Creature* creature) : ScriptedAI(creature) {}

            uint64 playerGUID;
            uint8 point;

            void Reset()
            {
                playerGUID = 0;
                point = 0;
            }

            void UpdateAI(const uint32 diff) {}

            void SpellHit(Unit* caster, const SpellInfo* spell)
            {
                if (!caster->ToPlayer())
                    return;

                if (spell->Id == SPELL_TASTY_REEF_FISH)
                {
                    if (playerGUID == 0)
                        playerGUID = caster->GetGUID();

                    me->GetMotionMaster()->MovePoint(point, caster->GetPositionX(), caster->GetPositionY(), caster->GetPositionZ());
                    ++point;
                }

                if (Creature* female = me->FindNearestCreature(NPC_FEMALE_REEF_COW, 5.0f, true))
                {
                 /*   if (Player* player = me->GetPlayer(*me, playerGUID))
                    {
                        DoCast(me, SPELL_LOVE_COSMETIC);
                        female->AI()->DoCast(female, SPELL_LOVE_COSMETIC);
                        player->GroupEventHappens(QUEST_THE_WAY_TO_HIS_HEART, me);
                        me->DespawnOrUnsummon(5000);
                    }
                */}
            }
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return new npc_attracted_reef_bullAI(creature);
        }
};

class spell_anuniaqs_net : public SpellScriptLoader
{
public:
    spell_anuniaqs_net() : SpellScriptLoader("spell_anuniaqs_net") {}

    class spell_anuniaqs_net_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_anuniaqs_net_SpellScript);

        bool Validate(SpellInfo const* /*spellInfo*/)
        {
            if (!sSpellMgr->GetSpellInfo(SPELL_ANUNIAQS_NET))
                return false;
            return true;
        }

        void HandleDummy(SpellEffIndex /*effIndex*/)
        {
            if (Unit* caster = GetCaster())
                if (caster->ToPlayer())
                    caster->ToPlayer()->AddItem(ITEM_TASTY_REEF_FISH, urand(1,5));
        }

        void Register()
        {
            OnEffectHit += SpellEffectFn(spell_anuniaqs_net_SpellScript::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
        }
    };

    SpellScript* GetSpellScript() const
    {
        return new spell_anuniaqs_net_SpellScript();
    }
};

//체인지 레이스
enum  defines
{
 faction_token = 16086, // Faction Change Token
 race_token = 16086 // Race Change Token
};

class npc_changer : public CreatureScript
{
	public:
		npc_changer() : CreatureScript("npc_changer"){}

		bool OnGossipHello(Player * pPlayer, Creature * pCreature)
		{
			
			
			pPlayer->ADD_GOSSIP_ITEM(4, "|TInterface\\icons\\Ability_Creature_Disease_02:23|t종족변경을 원합니다! ", GOSSIP_SENDER_MAIN, 0);
		    pPlayer->ADD_GOSSIP_ITEM(4, "|TInterface\\icons\\Ability_Creature_Disease_02:23|t진영변경을 원합니다!", GOSSIP_SENDER_MAIN, 1);
			pPlayer->PlayerTalkClass->SendGossipMenu(9425, pCreature->GetGUID());
			return true;
		}

		bool OnGossipSelect(Player * Player, Creature * Creature, uint32 /*uiSender*/, uint32 uiAction)
		{
			if(!Player)
				return true;

			switch(uiAction)
			{
				case 0:
					if(Player->HasItemCount(race_token, 3))
					{
						Player->DestroyItemCount(race_token, 3, true, false);
						Player->SetAtLoginFlag(AT_LOGIN_CHANGE_RACE);
						CharacterDatabase.PExecute("UPDATE characters SET at_login = at_login | '128' WHERE guid = %u", Player->GetGUID());
						Player->GetSession()->SendNotification("케릭터접속창에서 정보를 수정하세요!");
						Player->PlayerTalkClass->SendCloseGossip();
					}
					else
					{
						Player->GetSession()->SendNotification("종족을 변경하기 위해서는 투기장징표 3개가 필요합니다.");
						Player->PlayerTalkClass->SendCloseGossip();
					} 
					break;
				case 1:
					if(Player->HasItemCount(faction_token, 10))
					{
						Player->DestroyItemCount(faction_token, 10, true, false);
						Player->SetAtLoginFlag(AT_LOGIN_CHANGE_FACTION);
						CharacterDatabase.PExecute("UPDATE characters SET at_login = at_login | '64' WHERE guid = %u", Player->GetGUID());
						Player->GetSession()->SendNotification("케릭터접속창에서 정보를 변경하세요");
						Player->PlayerTalkClass->SendCloseGossip();

					}

					else
					{
						Player->GetSession()->SendNotification("진영을 변경하기 위해서는 투기장징표 10개가 필요합니다.");
						Player->PlayerTalkClass->SendCloseGossip();
					} 
					break;
			}
			return true;
		}

};

// Register Script
void AddSC_Sweet_TeleportNPC()
{
	//new Sweet_TeleportNPC();
	//new professionnpc();
//	new levelnpc();
	new level_rewards();
//	new SellNPC();
//	new npc_likantropo();
//    new npc_lava_boost();
//    new npc_quel_nagas();
    new npc_might_of_sylvanas();
    new npc_trollzilla();
    new npc_perotharn();
    new npc_uznam_the_watcher();
	new Pandaren();
    new npc_attracted_reef_bull();
    new spell_anuniaqs_net();
	new npc_changer();
}


