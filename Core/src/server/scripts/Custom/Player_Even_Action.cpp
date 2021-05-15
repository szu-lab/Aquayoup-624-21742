//###################################################################################################################################################################
//Stitch Avril 2021 - Player_Even_Action_handler - version 1.0 , Wod 6.2.4-21742 
// Action (Apprentissage, ajout d'item,etc) suite a un �v�nement Joueur (connexion,levelup,zone, cr�ation) suivant la classe ou la race
//###################################################################################################################################################################

//#include "Config.h"
#include "ScriptMgr.h"
#include "Player.h"
#include "Map.h"
#include "WorldSession.h"
#include "GameEventMgr.h"
#include "Channel.h"
#include "ChannelMgr.h"
#include "Chat.h"
#include "ChatPackets.h"
#include "ChatTextBuilder.h"
#include "Config.h"
#include "Unit.h"
#include "World.h"
#include "WorldPacket.h"
#include "WorldSession.h"
#include "Guild.h"
#include "GuildFinderMgr.h"
#include "GuildMgr.h"
#include "DatabaseEnv.h"
#include "SpellAuraEffects.h"
#include "SpellHistory.h"
#include "SpellMgr.h"
#include "SpellScript.h"

uint32 SPELL_POUR_VISUEL = 14867;
uint32 PERTE_DE_DURABILITE = 45317;         //  Perte de 10 % de la durabilit�
uint32 DISPARITION6S = 35205;

namespace {

class Player_Even_Action_handler : public PlayerScript {

public:
    Player_Even_Action_handler() : PlayerScript("Player_Even_Action_handler") {}

	// ################################################################################################################################################
    // Routines de test d"evenements 
	// ################################################################################################################################################

    // Connexion d'un joueur ou 1ere connexion joueur
    void OnLogin(Player* player, bool firstLogin) override
    {
        Apprentissage_Ou_Additem_Suivant_classes_races(player);

        // 1ere connexion joueur
        if (firstLogin)
        {

        }

    }

    // Au level up
    virtual void OnLevelChanged(Player* player, uint8 /*oldLevel*/) 
    {
        Apprentissage_Ou_Additem_Suivant_classes_races(player);
    }





    // Sans action pour le moment
    void OnPlayerEnterZone(Player* player, uint32 newZone, uint32 newArea)
    {
		//VOIR Stitch fatigue forc� pour interdire une zone - Vashj'ir
    }
    void OnPlayerUpdate(Player* player, uint32 p_time)
    {
        if (!player->IsInWorld()) { return; }
    }

    ////// TMP a voir/v�rifier : public: /* CreatureScript */
    // 
    // void OnPlayerEnter(map, player) {}
    // void OnPlayerLogin(Player* player, bool firstLogin) {}
    // void OnPlayerRepop(Player* player) {}
    // void OnPlayerKilledByCreature(Creature* /*killer*/, Player* player/*killed*/) {}
    // void OnCreatureKill(Player* /*killer*/, Creature* /*killed*/) { }
    // void OnLogout(Player* /*player*/) { }
    // void OnCreate(Player* /*player*/) { }
    // void OnMapChanged(Player* /*player*/) { }
    // void Player::Update(uint32 p_time)
    // void Player::KillPlayer()
    // void UpdateAI(uint32 diff) override ---> if (CanSpawn)
    // void Player::OnCombatExit()
    // void OnPlayerSpellCast(Player* player, Spell* spell, bool skipCheck) {}
    // void OnAccountLogin(uint32 /*accountId*/) {}

	// ################################################################################################################################################
    // Routines d'actions secondaires suite a un �v�nement 
	// ################################################################################################################################################

	void Apprentissage_Ou_Additem_Suivant_classes_races(Player* player)
    {
    uint8 _team = player->GetTeamId();      
    uint8 _class = player->getClass();
    uint8 _level = player->getLevel();
    uint8 _race = player->getRace();
    uint8 _GenderID = player->getGender();

    switch (_class)
{

case CLASS_WARRIOR:
	if (_level > 3) { player->LearnSpell(6343, false); }			//Coup de tonnerre 
    break;
case CLASS_PALADIN:
    break;
case CLASS_HUNTER:
	player->setPowerType(POWER_FOCUS);
	player->SetPower(POWER_FOCUS, 100);
    if (_level >= 5)
    {
        player->LearnSpell(883, true);      // Appel du familier 1
        player->LearnSpell(2641, true);     // Renvoyer le familier
        player->LearnSpell(9321, true);     // Contr�le du familier
        player->LearnSpell(6991, true);     // Nourrir le familier
        player->LearnSpell(136, true);      // Gu�rison du familier - ne s'apprend pas ?!
        player->LearnSpell(33976, true);    // Gu�rison du familier : temporaire : parce que 136 ne veux pas s'apprendre seul
        player->LearnSpell(982, true);      // Ressusciter le familier
        player->LearnSpell(1515, true);     // Apprivoiser une b�te
        player->LearnSpell(1462, true);     // Connaissance des b�tes
        player->GetSession()->SendNotification("|cffffffff[Vous avez appris de nouvelles competances]:|r");
    }
    if (_level >= 10)
    {
        player->LearnSpell(83242, true);      // Appel du familier 2
    }
    if (_level >= 17)
    {
        player->LearnSpell(83243, true);      // Appel du familier 3
    }
    if (_level >= 41)
    {
        player->LearnSpell(83244, true);      // Appel du familier 4
    }
    if (_level >= 48)
    {
        player->LearnSpell(83245, true);      // Appel du familier 5
    }
    break;
case CLASS_ROGUE:
	// Vampire ------
	if (player->GetUInt32Value(PLAYER_FIELD_CURRENT_SPEC_ID) == PLAYER_SPEC_ID_VAMPIRE) //Stitch Vampire - 538 correspond a ID de ChrSpecialization.dbc
	{
		if (player->HasAura(300124) || player->HasAura(300125))
		{
			player->setPowerType(POWER_DEMONIC_FURY);
			player->SetMaxPower(POWER_DEMONIC_FURY, 100);
			player->SetMaxPower(POWER_ENERGY, 0);
		}
		else
		{
			player->setPowerType(POWER_ENERGY);
			player->SetMaxPower(POWER_ENERGY, 100);
		}
	}
    break;
case CLASS_PRIEST:
	player->SetPower(POWER_SHADOW_ORBS, 0); // RAZ POWER_SHADOW_ORBS a la connexion sinon bug visuel
    break;
case CLASS_DEATH_KNIGHT:
    if (_level >= 6)
    {
        player->LearnSpell(53428, true); // Runeforge
    }
    if (_level >= 10)
    {
        player->LearnSpell(50977, true); // Porte de la mort
        player->LearnSpell(53428, true); // Destrier de la mort cramoisi
    }
    break;
case CLASS_SHAMAN:
    break;
case CLASS_MAGE:
    break;
case CLASS_WARLOCK:
	player->SetPower(POWER_SOUL_SHARDS, 0); // RAZ POWER_SOUL_SHARDS a la connexion
    break;
case CLASS_MONK:
    break;
case CLASS_DRUID:
    break;
}
	switch (_race)
{
case RACE_HUMAN:
    break;
case RACE_ORC:
    break;
case RACE_DWARF:
    break;
case RACE_NIGHTELF:
    break;
case RACE_UNDEAD_PLAYER:
    break;
case RACE_TAUREN:
    break;
case RACE_GNOME:
    break;
case RACE_TROLL:
    break;
case RACE_GOBLIN:
    break;
case RACE_BLOODELF:
    break;
case RACE_DRAENEI:
    break;
case RACE_WORGEN:
    player->LearnSpell(68978, true); // Ecorcheur
    player->LearnSpell(69270, true); // Langue (giln�en)
    player->LearnSpell(68976, true); // Aberration
    player->LearnSpell(68975, true); // Acharnement
	if (_level >= 5)	
	{		
		player->LearnSpell(68992, true); // Sombre course	
	}
	if (_level >= 10)	
	{		
		player->LearnSpell(68996, true); // Deux formes
		player->LearnSpell(94293, true); // Forme modifi�e
	}
	if (_level >= 20)
	{
		player->LearnSpell(87840, true); // Ventre � terre	
	}
    break;
case RACE_PANDAREN_NEUTRAL:
    break;
case RACE_PANDAREN_ALLIANCE:
    break;
case RACE_PANDAREN_HORDE:
    break;

}
    }

	/*
    void Apprentissage_Rez_sur_son_corp(Player* player)
    {
        uint8 rez = sConfigMgr->GetIntDefault("Rez_sur_son_corp", 1);
        if (rez == 1)
        {
            player->LearnSpell(SPELL_POUR_REZ, true);      // R�surrection de Pierre d'�me
        }
    }
    */




private:
	std::map<ObjectGuid, int> _unitDifficulty;
};

}

void AddSC_Player_Even_Action() {
	new Player_Even_Action_handler();
}
