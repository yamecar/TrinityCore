#include "ScriptPCH.h"
#include "ScriptedCreature.h"
#include "ObjectMgr.h"
#include "Group.h"

enum ZOMBIE_DATA
{
	NPC_START_GAME = 125,
	NPC_START_NEXT_WAVE = 126,
	NPC_START_NEXT_ROUND = 127,
	NPC_WAIT_FOR_END_ROUND = 128,
	NPC_WAIT_FOR_CHEST_LOOT = 151,
	NPC_END_GAME = 152,
	NPC_END_GAME_INUSE = 154,
	NPC_END_GAME_GOSSIP = 157,

	TIME_BTW_EACH_WAVE  = -1000350,
	TIME_TO  = -1000350,
	ZOMBIE_NPC_3_SPELL  = -1000350,
	ZOMBIE_NPC_4_SPELL  = -1000350,
	ZOMBIE_NPC_5_SPELL  = -1000350,
};

uint32 ZOMBIE_NPCS [7] = {0, 434,435,436,437,440,441};
uint32 TREASURE_CHEST [7] = {0, 510000,520000,530000,540000,550000,560000};

const Position Spawn_C1 ={-10953.3662f, -1997.1374f, 79.9735f, 3.0059f};
const Position Spawn_C2 ={-10974.728516f, -1972.700073f, 79.973518f, 5.158750f};
const Position Spawn_C3 ={-10957.954102f, -1973.366821f, 79.973518f, 3.991647f};
const Position Spawn_C4 ={-10985.388672f, -1992.934814f, 79.973984f, 6.138140f};
const Position Spawn_C5 ={-10980.184570f, -2016.144897f, 79.973984f, 0.798218f};
const Position Spawn_C6 ={-10962.931641f, -2019.476074f, 79.974602f, 2.163240f};

bool zombie_inUse = false;

bool closeGossipNotify(Player *player, std::string message)
{
	player->PlayerTalkClass->SendCloseGossip();
	std::string str = "|cFFFFFC00" + std::string(message); // make message yellow
    WorldPacket data(SMSG_NOTIFICATION, (str.size() + 1));
    data << str;
    player->GetSession()->SendPacket(&data);
	return true;
}

class zombie_bg_master : public CreatureScript
{
public:
	zombie_bg_master() : CreatureScript("zombie_bg_master") { }

	bool OnGossipHello(Player* player, Creature* creature)
	{
		if(player->IsInCombat())
		{
			return closeGossipNotify(player, "전투중에는 좀비 이벤트를 시작할수없습니다.");
		}

		if(!player->GetGroup() || player->GetGroup() == NULL)
		{
			return closeGossipNotify(player, "You must be the group leader in a party/raid with 5-10 players.");
		} 

		Group* grp = player->GetGroup();

		if(grp->GetMembersCount() > 5 || grp->GetMembersCount() < 1)
		{
			return closeGossipNotify(player, "You must be the group leader in a party/raid with 5-10 players.");
		}

		if(grp->GetLeaderGUID() != player->GetGUID())
		{
			return closeGossipNotify(player, "파티 리더만 좀비 이벤트를 시작할수있습니다.");
		}

		player->ADD_GOSSIP_ITEM(GOSSIP_ICON_BATTLE, "좀비 이벤트로 이동시켜주세요!", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+1);
		player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "괜찬습니다.", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+2);
		player->PlayerTalkClass->SendGossipMenu(1, creature->GetGUID());
		return true;
	}

	bool OnGossipSelect(Player* player, Creature* creature, uint32 /*uiSender*/, uint32 uiAction)
	{
		player->PlayerTalkClass->ClearMenus();
		Group* grp = player->GetGroup();
		switch(uiAction)
		{
		case GOSSIP_ACTION_INFO_DEF+1:
			grp->ConvertToRaid();
			player->TeleportTo(532, -10966.888f, -2011.3127f, 79.98f, 1.37f);
			zombie_inUse = true;
			break;

		case GOSSIP_ACTION_INFO_DEF+2:
			player->PlayerTalkClass->SendCloseGossip();
			break;
		}
		return true;
	}


};


class zombie_bg_master_endgame : public CreatureScript
{
public:
	zombie_bg_master_endgame() : CreatureScript("zombie_bg_master_endgame") { }

	bool OnGossipHello(Player* player, Creature* creature)
	{
	/*	if(player->IsInCombat())
		        return false;
		if(player->GetGroup()->GetLeaderGUID() == player->GetGUID()){
		        player->ADD_GOSSIP_ITEM(GOSSIP_ICON_BATTLE, "이벤트를 종료합니다!", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+1);
		        player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "취소.", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+2);                
		        player->PlayerTalkClass->SendGossipMenu(1, creature->GetGUID());
		} */
		return true;
	}

	bool OnGossipSelect(Player* player, Creature* creature, uint32 /*uiSender*/, uint32 uiAction)
	{
		/*player->PlayerTalkClass->ClearMenus();
		Group* grp = player->GetGroup();
		switch(uiAction)
		{
		case GOSSIP_ACTION_INFO_DEF+1:
		                creature->MonsterYell("리더가 이벤트를 종료하엿습니다!", 0, 0);
		                creature->SummonCreature(NPC_END_GAME, -10989.9003f, -1940.1300f, 93.9107f, 4.8345f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 1*1000); 
		                creature->DespawnOrUnsummon();
		        break;

		case GOSSIP_ACTION_INFO_DEF+2:
		        player->PlayerTalkClass->SendCloseGossip();
		        break;
		}*/
		return true;
	}


};

class zombie_bg_master_final : public CreatureScript
{
public:
	zombie_bg_master_final() : CreatureScript("zombie_bg_master_final") { }

	bool OnGossipHello(Player* player, Creature* creature)
	{
		if(!player->GetGroup())
			return false;
		if(player->GetGroup() == NULL)
			return false;
		if((player->GetGroup()->GetMembersCount() <= 5 && player->GetGroup()->GetMembersCount() >= 1) && player->GetGroup()->GetLeaderGUID() == player->GetGUID())
		{
			player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "좀비 이벤트 파티 소환해주세요!", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+1);
			if(player->GetGroup()->GetLeaderGUID() == player->GetGUID())
				player->ADD_GOSSIP_ITEM(GOSSIP_ICON_BATTLE, "이벤트 시작", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+2);
			player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "최소", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+4);
			player->PlayerTalkClass->SendGossipMenu(1, creature->GetGUID());
		} else 
		{
			ChatHandler(player->GetSession()).PSendSysMessage("You must be the group leader in a party/raid with 5-10 players");
			while(creature->SelectNearestPlayer(280.0f) != NULL)
			{
				Player* plr = creature->SelectNearestPlayer(280.0f);
				if(plr)
				{
					plr->ResurrectPlayer(60, false);
					plr->TeleportTo(534, 5541.770020f, -3783.469971f, 1594.910034f, 2.883010f);
				}
			}
		}

		return true;
	}

	bool OnGossipSelect(Player* player, Creature* creature, uint32 /*uiSender*/, uint32 uiAction)
	{

		player->PlayerTalkClass->ClearMenus();
		Group* grp = player->GetGroup();
		zombie_inUse = true;
		if(grp){
			switch(uiAction)
			{
			case GOSSIP_ACTION_INFO_DEF+1:

				for (GroupReference* itr = grp->GetFirstMember(); itr != NULL; itr = itr->next())
				{
					Player* groupMember = itr->GetSource();
					ChatHandler(player->GetSession()).PSendSysMessage("파티원 소환중......");
					if(!groupMember)
						continue;                
					if(groupMember->GetGUID() == player->GetGUID())
						break;
					player->SetSelection(groupMember->GetGUID());
					player->CastSpell(groupMember, 7720, true);
					std::ostringstream ss;
					ss << groupMember->GetName();
					ChatHandler(player->GetSession()).PSendSysMessage("%s", ss.str().c_str());
				}
				break;

			case GOSSIP_ACTION_INFO_DEF+2:
				creature->MonsterYell("30초후 라운드 1 시작!", 0, 0);

				/* spawn NPC_START_GAME to despawn after 30 seconds and start the game */
				creature->SummonCreature(NPC_START_GAME, -10989.9003f, -1940.1300f, 93.9107f, 4.8345f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 30*1000); 
				/* the main NPC will not be seen again by the group so teleport him away */
				creature->UpdatePosition(-10989.9003f, -1940.1300f, 93.9107f, 4.8345f, true);
				break;

			case GOSSIP_ACTION_INFO_DEF+3:                
				creature->MonsterYell("리더만 이벤트를 중지할수 있습니다!", 0, 0);
				creature->SummonCreature(NPC_END_GAME, -10989.9003f, -1940.1300f, 93.9107f, 4.8345f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 500); 
				break;

			case GOSSIP_ACTION_INFO_DEF+4:
				//NPC_END_GAME

				break;
			}
		}
		player->PlayerTalkClass->SendCloseGossip();
		return true;
	}

	struct zombie_bg_master_finalAI : public ScriptedAI
	{
		zombie_bg_master_finalAI(Creature* c) : ScriptedAI(c), Summons(me) {}

		SummonList Summons;
		//total number of zombies to spawn
		uint8 zombieCount;
		//current number of zombies spawned so far
		uint8 zombiesSpawned;
		//how many zombies have died in this round
		uint8 zombieDeathCount;
		//what round in the game you are on
		uint8 zombieRound;
		//how many waves of zombies per round
		uint8 zombieWaves;
		//the number of waves already spawned
		uint8 zombieWavesCount;
		//the number of zombies to be spawned per wave
		uint32 zombieSpawnPerWave;
		//stores how many players are in the group
		uint32 zombieGroupMembers;
		//stores number of zombies spawn per each person
		uint8 ZOMBIES_PER_PERSON;

		bool inZombieGame;

		Player* zombieLeader;
		Group* playerGroup;
		GameObject *zombie_chest_loot;

		void JustSummoned(Creature* summon) 
		{        
			if(summon) {
				uint32 entry = summon->GetEntry();
				Summons.Summon(summon);
				if(entry >= 125 && entry <= 128 || entry == 151 || entry == 152)
				{

				} else {
					Player* selPlr = summon->SelectNearestPlayer(200);
					if(selPlr)
						summon->Attack(selPlr, false);
					else
					{
						/* cant find a player to select */
					}
				}
			}
		}

		void SummonedCreatureDespawn(Creature* summon) 
		{
			uint32 summonID = summon->GetEntry();

			if(summonID == NPC_START_GAME)
			{
				/* called at the beg of the game */
				/* reset all vars because this is new instance of game */
				resetZombieEvent();

				Player* selPlr = me->SelectNearestPlayer(280.0f);
				if(!selPlr)
				{ reviveAndTeleportOut(); resetZombieEvent(); me->MonsterYell("!selPlr return", 0, 0);return; }
				if(selPlr == NULL)
				{        reviveAndTeleportOut(); resetZombieEvent(); me->MonsterYell("selPlr == NULL return", 0, 0);return; }
				if(!selPlr->GetGroup())
				{        reviveAndTeleportOut(); resetZombieEvent(); me->MonsterYell("!Group return", 0, 0);return; }
				if(selPlr->GetGroup() == NULL)
				{        reviveAndTeleportOut(); resetZombieEvent(); me->MonsterYell("Group == NULL return", 0, 0);return; }
				if(playerGroup = selPlr->GetGroup())
				{
					zombieGroupMembers = playerGroup->GetMembersCount();
					if(zombieLeader = sObjectAccessor->FindPlayer(playerGroup->GetLeaderGUID()))
					{
					} else {
						reviveAndTeleportOut(); resetZombieEvent(); me->MonsterYell("LeaderGUID == NULL return", 0, 0);return;
					}
				}
				else
				{
					reviveAndTeleportOut(); resetZombieEvent(); me->MonsterYell("playerGroup == NULL return", 0, 0);return;
				}

				/* Set the ZombieGame to true so they cant keep starting the game */
				inZombieGame = true;

				me->MonsterYell("라운드 1 시작!", 0, 0);
				sendMessageToGroup("라운드 1 시작!");
				me->MonsterYell("이벤트중 죽을 경우 라운드 종료시 자동 부할됩니다.", 0, 0);
				me->SetVisible(false);

				/* calc how many waves of zombies to send */
				zombieWaves = getNumberOfWaves();                
				/* calc how many zombies per wave */
				zombieSpawnPerWave = getZombieSpawnCountPerWave();        
				/* spawn */
				spawnZombies(me, zombieSpawnPerWave);        

				/* NPC_START_NEXT_WAVE is despanwed when the next wave of zombies is to be sent */
				me->SummonCreature(NPC_START_NEXT_WAVE, -10989.9003f, -1940.1300f, 93.9107f, 4.8345f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 30*1000);         
			}

			if(inZombieGame == false){ return; }
			char buffer [150];
			switch(summonID)
			{
			case NPC_START_GAME:
				/* called at the beg of the game */
				break;

				/* called when a wave of zombies needs to be spawned */
			case NPC_START_NEXT_WAVE:                                                                        
				if(zombieWavesCount == zombieWaves){                                        /* spawned all waves */
					if(isRoundOver()){                                                                        /* check if all zombies are dead */
						/* reset wave count to 1 for next round */
						zombieWavesCount = 1;                                
						/* add addtional zombies for next round */
						ZOMBIES_PER_PERSON = ZOMBIES_PER_PERSON + 2;        

						/* spawn treasure loot */
						spawnGoDrop(me);

						/* remove all add. zombies that are somehow left */
						Summons.RemoveNotExisting();

						/* revive the group for the next round */
						reviveGroup();
						me->SetVisible(true);

						sprintf (buffer, "총 15 라운드 중 라운드 %u 종료!, 다음 라운드 시작은 1분후 시작됩니다!", zombieRound);
						me->MonsterYell(buffer, 0 , 0);
						sendMessageToGroup(buffer);
						sprintf (buffer, "이벤트를 종료하시려면 그룹을 해산하시면 30초후 자동 이동됩니다.");
						me->MonsterYell(buffer, 0 , 0);
						sendMessageToGroup(buffer);

						/* NPC_START_NEXT_ROUND is spawned when all waves of zombies have been killed */
						me->SummonCreature(NPC_START_NEXT_ROUND, -10989.9003f, -1940.1300f, 93.9107f, 4.8345f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 60*1000);
						/* NPC_END_GAME_GOSSIP is spawned after every round incase players want to exit the game */
						me->SummonCreature(NPC_END_GAME_GOSSIP, -10969.819336f, -1995.319580f, 79.973022f, 1.467133f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 60*1000);
						zombieRound++;

					} else {                                                                                        /* not all zombies are dead wait for * sec and check again */
						if(isGroupDead())
						{
							reviveAndTeleportOut();
							me->SetVisible(true);
							Summons.DespawnAll();
							Summons.RemoveNotExisting();
							resetZombieEvent();
						}
						else {
							/* not all members are dead but there is still zombies left, respawn and check again in * sec */
							me->SummonCreature(NPC_WAIT_FOR_END_ROUND, -10989.9003f, -1940.1300f, 93.9107f, 4.8345f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 30*1000);
						}
					}
				}
				else{                                                                                                        /* not all waves spawned continue spawning zombies */
					if(!isGroupDead()) {
						spawnZombies(me, zombieSpawnPerWave);
						me->SummonCreature(NPC_START_NEXT_WAVE, -10989.9003f, -1940.1300f, 93.9107f, 4.8345f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 30*1000);
						zombieWavesCount++;
					} else {
						reviveAndTeleportOut();
						me->SetVisible(true);
						Summons.DespawnAll();
						resetZombieEvent();
						TC_LOG_ERROR("misc","Everyone is dead. Stopping....2");
					}
				}
				break;

			case NPC_START_NEXT_ROUND:                                                                        /* called when the all waves are spawned and next round needs to begin */
				if(zombieRound != 15) {
					sprintf (buffer, "총 15 라운드 중 라운드 %u 시작!", zombieRound);
					me->MonsterYell(buffer, 0 , 0);                
					sendMessageToGroup(buffer);
					me->SetVisible(false);
					zombieWaves = getNumberOfWaves();                                                /* calc how many waves of zombies to send */
					zombieSpawnPerWave = getZombieSpawnCountPerWave();                /* calc how many zombies per wave */
					spawnZombies(me, zombieSpawnPerWave);
					me->SummonCreature(NPC_START_NEXT_WAVE, -10989.9003f, -1940.1300f, 93.9107f, 4.8345f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 30*1000); 
				} 
				else 
				{
					me->MonsterYell("15 라운드 모두 완료하였습니다.", 0 , 0);        
					sendMessageToGroup("15 라운드 모두 완료하였습니다.");
					resetZombieEvent();
					Summons.RemoveNotExisting();
				}
				break;

			case NPC_WAIT_FOR_END_ROUND:                                                                /* called when the all waves are spawned but zombies are still left */
				if(isGroupDead())                                                                                /* check if the zombies killed all the players */
				{
					reviveAndTeleportOut();
					me->SetVisible(true);
					Summons.DespawnAll();
					resetZombieEvent();
					TC_LOG_ERROR("misc","Everyone is dead. Stopping....3");
				} else {                                                                                                
					if(isRoundOver()){                                                                        /* check if all zombies have been killed if not, wait another * secs*/                
						sprintf (buffer, "총 15 라운드 중 라운드 %u 종료, 다음라운드는 1분후에 시작됩니다!", zombieRound);
						me->SetVisible(true);
						me->MonsterYell(buffer, 0 , 0);        
						sendMessageToGroup(buffer);
						sprintf (buffer, "이벤트를 종료하시려면 그룹을 해산하시면 30초이후 자동이동됩니다.");
						me->MonsterYell(buffer, 0 , 0);
						sendMessageToGroup(buffer);

						zombieWavesCount = 1;
						ZOMBIES_PER_PERSON = ZOMBIES_PER_PERSON + 2;

						reviveGroup();
						Summons.RemoveNotExisting();
						spawnGoDrop(me);

						me->SummonCreature(NPC_START_NEXT_ROUND, -10989.9003f, -1940.1300f, 93.9107f, 4.8345f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 60*1000);
						me->SummonCreature(NPC_END_GAME_GOSSIP, -10969.819336f, -1995.319580f, 79.973022f, 1.467133f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 60*1000);
						zombieRound++;
					} else {
						me->SummonCreature(NPC_WAIT_FOR_END_ROUND, -10989.9003f, -1940.1300f, 93.9107f, 4.8345f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 30*1000);
					}
				}
				break;

			case NPC_WAIT_FOR_CHEST_LOOT:
				if(zombie_chest_loot)
				{
					zombie_chest_loot->SetRespawnTime(0);                                 
					zombie_chest_loot->Delete();
					zombie_chest_loot->DeleteFromDB();
				}
				break;

			case NPC_END_GAME:
				reviveAndTeleportOut();
				resetZombieEvent();        
				me->UpdatePosition(-10989.9003f, -1940.1300f, 93.9107f, 4.8345f, true);
				break;
			case NPC_END_GAME_INUSE:
				zombie_inUse = false;
				break;
			case NPC_END_GAME_GOSSIP:

				break;
			default: 
				if(summonID == 434 || summonID == 435 || summonID == 436 || summonID == 437 || summonID == 440 || summonID == 441)
					zombieDeathCount++;
				break;
			}
		}

		bool isGroupDead()
		{
			if(!zombieLeader->GetGroup())
				return true;
			if(zombieLeader->GetGroup() == NULL)
				return true;
			if(!playerGroup)
				return true;
			if(playerGroup == NULL)
				return true;
			if(playerGroup->GetMembersCount() > 5 || playerGroup->GetMembersCount() < 1)
				return true;

			uint32 playersDead = 0;
			Player* GroupMember;
			const Group::MemberSlotList members = playerGroup->GetMemberSlots();
			for (Group::member_citerator itr = members.begin(); itr!= members.end(); ++itr)
			{
				GroupMember = (ObjectAccessor::GetPlayer(*me, itr->guid));
				if (!GroupMember || !GroupMember->GetSession()) { playersDead++; continue; }
				//if (!GroupMember->IsWithinDistInMap(me, 100)){ playersDead++; continue; }
				if (GroupMember->isDead()){ playersDead++; continue; }

			}

			if(playersDead >= playerGroup->GetMembersCount())
				return true;

			if(zombieRound == 15)
				return true;        
			return false;
		}

		bool sendMessageToGroup(const char *message)
		{
			if(!zombieLeader->GetGroup())
				return true;
			if(zombieLeader->GetGroup() == NULL)
				return true;
			if(!playerGroup)
				return true;

			Player* GroupMember;
			const Group::MemberSlotList members = playerGroup->GetMemberSlots();
			for (Group::member_citerator itr = members.begin(); itr!= members.end(); ++itr)
			{
				GroupMember = (ObjectAccessor::GetPlayer(*me, itr->guid));
				if (GroupMember && GroupMember->GetSession())
				{
					if(!GroupMember)
						continue;
					
					std::string str = "|cFFFFFC00" + std::string(message); // make message yellow
					WorldPacket data(SMSG_NOTIFICATION, (str.size() + 1));
					data << str;
					GroupMember->GetSession()->SendPacket(&data);
				}
			}
			return true;
		}

		void reviveAndTeleportOut()
		{
			if(!zombieLeader || zombieLeader == NULL)
			{
				teleportPlayersOut();
				return;
			}

			if(!zombieLeader->GetGroup())
			{
				teleportPlayersOut();
				return;
			}
			if(zombieLeader->GetGroup() == NULL)
			{
				teleportPlayersOut();
				return;
			}
			if(!playerGroup)
			{
				teleportPlayersOut();
				return;
			}

			if(playerGroup){
				Player* GroupMember;
				const Group::MemberSlotList members = playerGroup->GetMemberSlots();
				for (Group::member_citerator itr = members.begin(); itr!= members.end(); ++itr)
				{
					GroupMember = (ObjectAccessor::GetPlayer(*me, itr->guid));
					if (GroupMember && GroupMember->GetSession())
					{
						//if (!GroupMember->IsWithinDistInMap(me, 100))
						//{
						if(GroupMember->isDead())
							GroupMember->ResurrectPlayer(60, false);
						GroupMember->TeleportTo(534, 5541.770020f, -3783.469971f, 1594.910034f, 2.883010f);
						//}
					}
				}
			}
		}

		bool teleportPlayersOut()
		{
			TC_LOG_ERROR("misc","teleportPlrsOut");
			while(me->SelectNearestPlayer(280.0f) != NULL)
			{
				Player* plr = me->SelectNearestPlayer(280.0f);
				if(plr)
				{
					plr->ResurrectPlayer(60, false);
					plr->TeleportTo(534, 5541.770020f, -3783.469971f, 1594.910034f, 2.883010f);
				}
			}
			return true;
		}

		void spawnZombies(Creature* creature, uint32 count)
		{
			zombiesSpawned = zombiesSpawned + count;
			for(uint32 i = 1; i <= count; i++){
				ACE_Based::Thread::Sleep(10);
				uint32 NPC_ID = rand() % 6 + 1;
				switch((rand() % 6 + 1))
				{
				case 1:  
					creature->SummonCreature(ZOMBIE_NPCS[NPC_ID], Spawn_C1);        
					break;
				case 2: 
					creature->SummonCreature(ZOMBIE_NPCS[NPC_ID], Spawn_C2);        
					break;
				case 3: 
					creature->SummonCreature(ZOMBIE_NPCS[NPC_ID], Spawn_C3);        
					break;
				case 4: 
					creature->SummonCreature(ZOMBIE_NPCS[NPC_ID], Spawn_C4);        
					break;
				case 5: 
					creature->SummonCreature(ZOMBIE_NPCS[NPC_ID], Spawn_C5);        
					break;
				case 6: 
					creature->SummonCreature(ZOMBIE_NPCS[NPC_ID], Spawn_C6);        
					break;
				default: TC_LOG_ERROR("misc","spawn default");break;
				}
			}
		}

		int getZombieSpawnCountPerWave()
		{
			return ((zombieGroupMembers * ZOMBIES_PER_PERSON) - 1 ) / zombieWaves + 1;
		}

		int getNumberOfWaves()
		{
			if(zombieRound <= 3)
				return 3;
			if(zombieRound > 4 && zombieRound <= 8)
				return 4;
			if(zombieRound > 8 && zombieRound <= 12)
				return 5;
			if(zombieRound > 12 && zombieRound <= 18)
				return 6;
			if(zombieRound >18 && zombieRound < 50)
				return 7;
			return 3;
		}

		int getZombieSpawnCount(Creature* creature)
		{
			return ((zombieGroupMembers * ZOMBIES_PER_PERSON) - 1 ) / 3 + 1;

		}

		bool spawnGoDrop(Creature* creature)
		{
			switch(zombieRound)
			{
			case 1: creature->SummonCreature(510000, -10970.494141f, -2001.188354f, 79.973495f, 1.433620f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 90*1000); break;
			case 2: creature->SummonCreature(510000, -10970.494141f, -2001.188354f, 79.973495f, 1.433620f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 90*1000); break;
			case 3: creature->SummonCreature(520000, -10970.494141f, -2001.188354f, 79.973495f, 1.433620f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 90*1000); break;
			case 4: creature->SummonCreature(520000, -10970.494141f, -2001.188354f, 79.973495f, 1.433620f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 90*1000); break;
			case 5: creature->SummonCreature(530000, -10970.494141f, -2001.188354f, 79.973495f, 1.433620f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 90*1000); break;
			case 6: creature->SummonCreature(530000, -10970.494141f, -2001.188354f, 79.973495f, 1.433620f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 90*1000); break;
			case 7: creature->SummonCreature(540000, -10970.494141f, -2001.188354f, 79.973495f, 1.433620f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 90*1000); break;
			case 8: creature->SummonCreature(540000, -10970.494141f, -2001.188354f, 79.973495f, 1.433620f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 90*1000); break;
			case 9: creature->SummonCreature(550000, -10970.494141f, -2001.188354f, 79.973495f, 1.433620f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 90*1000); break;
			case 10: creature->SummonCreature(550000, -10970.494141f, -2001.188354f, 79.973495f, 1.433620f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 90*1000); break;
			case 11: creature->SummonCreature(560000, -10970.494141f, -2001.188354f, 79.973495f, 1.433620f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 90*1000); break;
			case 12: creature->SummonCreature(560000, -10970.494141f, -2001.188354f, 79.973495f, 1.433620f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 90*1000); break;
			case 13: creature->SummonCreature(560000, -10970.494141f, -2001.188354f, 79.973495f, 1.433620f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 90*1000); break;
			case 14: creature->SummonCreature(560000, -10970.494141f, -2001.188354f, 79.973495f, 1.433620f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 90*1000); break;
			case 15: creature->SummonCreature(560000, -10970.494141f, -2001.188354f, 79.973495f, 1.433620f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 90*1000); break;
			}

			//creature->SummonCreature(NPC_WAIT_FOR_CHEST_LOOT, -10989.9003f, -1940.1300f, 93.9107f, 4.8345f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 100*1000);
			return true;
		}

		bool isRoundOver()
		{
			if((zombiesSpawned - zombieDeathCount) <= 0) {
				zombiesSpawned = 0;
				zombieDeathCount = 0;
				return true;
			}

			return false;
		}

		bool resetZombieEvent()
		{
			zombieCount = 0;
			zombiesSpawned = 0;
			zombieDeathCount = 0;
			zombieRound = 1;
			zombieWaves = 3;
			zombieWavesCount = 1;
			zombieSpawnPerWave = 0;
			zombieGroupMembers = 0;
			ZOMBIES_PER_PERSON = 3;

			playerGroup = NULL;
			zombieLeader = NULL;

			inZombieGame = false;
			me->UpdatePosition(-10989.9003f, -1940.1300f, 93.9107f, 4.8345f, true);
			return true;
		}

		void reviveGroup()
		{
			GroupReference* grpRef = playerGroup->GetFirstMember();
			for (grpRef; grpRef != NULL; grpRef = grpRef->next())
			{
				Player* groupMember = grpRef->GetSource();
				if(!groupMember)
					continue;

				if(!groupMember->IsInFlight() && !groupMember->IsFlying())
				{
					if(!groupMember->IsAlive() && groupMember->IsInWorld())
					{
						if(!groupMember->InBattleground() && !groupMember->InArena())
						{
							groupMember->ResurrectPlayer(60, false);
						}
					}
					groupMember->TeleportTo(532, -10966.888f, -2011.3127f, 79.98f, 1.37f);                
				}
			}
		}


	};

	CreatureAI* GetAI(Creature* creature) const
	{
		return new zombie_bg_master_finalAI(creature);
	}
};

/*
---------------
Zombie Creatures AI
---------------
*/

class mobs_normal_zombie : public CreatureScript
{
public:
	mobs_normal_zombie() : CreatureScript("mobs_normal_zombie") { }

	struct mobs_normal_zombieAI : public ScriptedAI
	{
		mobs_normal_zombieAI(Creature* c) : ScriptedAI(c) {}

		void JustDied(Unit* killer)
		{
			//zombieDeathCount++;
			if(me)
				me->DespawnOrUnsummon();
		}        
	};

	CreatureAI* GetAI(Creature* creature) const
	{
		return new mobs_normal_zombieAI (creature);
	}
};

class go_zombie_treasure : public GameObjectScript
{
public:
	go_zombie_treasure() : GameObjectScript("go_zombie_treasure") { }

	bool OnGossipHello(Player* player, GameObject* go)
	{
		TC_LOG_ERROR("misc","OnGossipHello");
		//player->SendLoot(go->GetGUID(), LOOT_CORPSE);
		return true;
	}
};

void AddSC_zombie_event()
{
	new zombie_bg_master();
	new zombie_bg_master_endgame();
	new zombie_bg_master_final();
	new mobs_normal_zombie();
	new go_zombie_treasure();
}