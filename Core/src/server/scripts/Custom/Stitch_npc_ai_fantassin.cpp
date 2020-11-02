////#########################################################################################################################################################################################################################################
// Copyright (C) Juillet 2020 Stitch pour Aquayoup
// AI generique npc par classe : Fantassin Ver 2020-11-02 (warrior simple, combat au corp a corp)
// Il est possible d'influencer le temp entre 2 cast avec `BaseAttackTime` & `RangeAttackTime` 
// Necessite dans Creature_Template :
// Minimun  : UPDATE `creature_template` SET `ScriptName` = 'Stitch_npc_ai_fantassin',`AIName` = '' WHERE (entry = 15100007);
// Optionel : UPDATE `creature_template` SET `HealthModifier` = 2, `ManaModifier` = 3, `ArmorModifier` = 1, `DamageModifier` = 2,`BaseAttackTime` = 2000, `RangeAttackTime` = 2000 WHERE(entry = 15100007);
// REPLACE INTO `creature_template` (`entry`, `difficulty_entry_1`, `difficulty_entry_2`, `difficulty_entry_3`, `KillCredit1`, `KillCredit2`, `modelid1`, `modelid2`, `modelid3`, `modelid4`, `name`, `femaleName`, `subname`, `IconName`, `gossip_menu_id`, `minlevel`, `maxlevel`, `exp`, `exp_unk`, `faction`, `npcflag`, `speed_walk`, `speed_run`, `scale`, `rank`, `dmgschool`, `BaseAttackTime`, `RangeAttackTime`, `BaseVariance`, `RangeVariance`, `unit_class`, `unit_flags`, `unit_flags2`, `dynamicflags`, `family`, `trainer_type`, `trainer_class`, `trainer_race`, `type`, `type_flags`, `type_flags2`, `lootid`, `pickpocketloot`, `skinloot`, `resistance1`, `resistance2`, `resistance3`, `resistance4`, `resistance5`, `resistance6`, `spell1`, `spell2`, `spell3`, `spell4`, `spell5`, `spell6`, `spell7`, `spell8`, `VehicleId`, `mingold`, `maxgold`, `AIName`, `MovementType`, `InhabitType`, `HoverHeight`, `HealthModifier`, `HealthModifierExtra`, `ManaModifier`, `ManaModifierExtra`, `ArmorModifier`, `DamageModifier`, `ExperienceModifier`, `RacialLeader`, `movementId`, `RegenHealth`, `mechanic_immune_mask`, `flags_extra`, `ScriptName`, `VerifiedBuild`) VALUES
// (15100012, 0, 0, 0, 0, 0, 45640, 0, 0, 0, 'npc_ai_fantassin', '', '', '', 0, 90, 90, 0, 0, 11, 0, 1.01, 1.01, 1, 0, 0, 2000, 2000, 1, 1, 1, 0, 2048, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '', 0, 3, 1, 2, 1, 1, 1, 2, 2, 1, 0, 144, 1, 0, 0, 'Stitch_npc_ai_fantassin', 20173);
// REPLACE INTO `creature_equip_template` (`CreatureID`, `ID`, `ItemID1`, `ItemID2`, `ItemID3`, `VerifiedBuild`) VALUES
// (15100012, 1, 88857, 0, 0, 0);	## Lance
//###########################################################################################################################################################################################################################################



//################################################################################################
//StitchAI AI Fantassin
//################################################################################################

class Stitch_npc_ai_fantassin : public CreatureScript
{
public: Stitch_npc_ai_fantassin() : CreatureScript("Stitch_npc_ai_fantassin") { }

		struct Stitch_npc_ai_fantassinAI : public ScriptedAI
		{
			Stitch_npc_ai_fantassinAI(Creature* creature) : ScriptedAI(creature) { }

			uint32 Random;
			uint32 DistanceDeCast = 30;												// Distance max a laquelle un npc attaquera , au dela il quite le combat
			uint32 ResteADistance = 5;												// Distance max a laquelle un npc s'approchera
			uint32 Dist;															// Distance entre le npc et sa cible
			Unit* victim = me->GetVictim();										 
			
			// Definitions des variables Cooldown et le 1er lancement
			uint32 Cooldown_Spell1 = 500;
			uint32 Cooldown_Spell2 = 2000;
			uint32 Cooldown_Spell_Heal = 3000;
			uint32 Cooldown_ResteADistance = 4000;									// Test si en contact
			uint32 Cooldown_ResteAuContact;
			uint32 Cooldown_Anti_Bug_Figer = 2000;
			uint32 Cooldown_Npc_Emotes = urand(5000, 8000);
			uint32 Cooldown_Trop_Loin = 3000;

			// Spells
			uint32 Buf_1 = 12712;													
			uint32 liste_Buf[5] = { 12712, 6673, 1160, 159362, 8599 };										// Soldat aguerri 12712 (2 mains= dmg+15%), Cri de guerre 6673, Cri d�moralisant 1160 (8s 10m Soi-m�me), Folie sanguinaire 159362 (pv 1%/3s), Enrager 8599
			uint32 Spell_agro = 0;
			uint32 liste_agro[3] = { 100, 355, 145763 };													// Charge 100, Provocation 355, Bondir 8-40m 145763
			uint32 Spell_1;
			uint32 liste_spell_1[8] = { 29426, 126799, 118326, 172851, 38742, 99409, 100431, 115530 };		// Frappe h�ro�que 29426/78, Frappe tranchante 126799, Attaque vicieuse 118326, Encha�nement 172851, Encha�nement gangren� 38742, Encha�nement noir 99409, Encha�nement enflamm� 100431, Fendoir spirituel 115530
			uint32 Spell_2;
			uint32 liste_spell_2[9] = { 127171, 131662, 8147, 118532, 125436, 36991, 36991, 8147, 8147 };	// Fendoir vicieux 15/lvl + 2/lvl/1s cumulable 5 fois 127171, Coups de couteau 131662, Entaille infect�e 118532, D�coupe d'os 125436, Pourfendre 36991, Coup de tonnerre 8147
			uint32 Spell_evade = 108843;																	// Vitesse flamboyante 6s 150% 108843
			uint32 Spell_trop_Loin = 100;																	// Charge 100


			// Emotes
			uint32 Npc_Emotes[22] = { 1,3,7,11,15,16,19,21,22,23,24,53,66,71,70,153,254,274,381,401,462,482 };

			uint32 Start_Agro = 0;

			void Init_AI()
			{

				// ################################################################################################################################################
				// Tirages al�atoires des spells
				// ################################################################################################################################################
				// Si m_spells[0] != 0 alors affectation al�atoire de tous les spells , sinon utilisera m_spells[x]
				// m_spells[0] : Attaque principale						- Correspond a spell1 de creature_template
				// m_spells[1] : Dot									- Correspond a spell2 de creature_template
				// m_spells[2] : spell lanc� a l'agro					- Correspond a spell3 de creature_template
				// m_spells[3] : spell lanc� a l'�vade ou respawn		- Correspond a spell4 de creature_template
				// m_spells[4] : Buf									- Correspond a spell5 de creature_template

				if (me->m_spells[0] == 0) 
				{ 
					Spell_1 = liste_spell_1[urand(0, 7)];
					Spell_2 = liste_spell_2[urand(0, 8)];
					Spell_agro = liste_agro[urand(0, 2)];
				} else 
				{ 
					Spell_1 = me->m_spells[0]; 
					if (me->m_spells[1] != 0) { Spell_2 = me->m_spells[1]; }
					if (me->m_spells[2] != 0) { Spell_agro = me->m_spells[2]; }
					if (me->m_spells[3] != 0) { Spell_evade = me->m_spells[3]; }
					if (me->m_spells[4] != 0) { Buf_1 = me->m_spells[4]; }
				}


				// Spell a lancer a l'agro ------------------------------------------------------------------------------------------------------------------------

				Random = urand(1, 2);
					if (Random == 1) { me->CastSpell(me, Buf_1, true); }

				// ################################################################################################################################################
				// Divers
				// ################################################################################################################################################
				me->SetSheath(SHEATH_STATE_MELEE);												// S'equipe de l'arme au contact
				me->SetReactState(REACT_AGGRESSIVE);
				// ################################################################################################################################################
			}
			void JustRespawned() override
			{
				me->GetMotionMaster()->MoveTargetedHome();								// Retour home pour rafraichir client
				me->SetSpeedRate(MOVE_RUN, 1.01f);
				me->SetReactState(REACT_AGGRESSIVE);

				Init_AI();
				Random = urand(1, 2);
				if (Random == 1 && Spell_evade != 0) { me->CastSpell(me, Spell_evade, true); }		// 1/2 Chance de lancer le sort au respawn ou evade
			}

			void EnterCombat(Unit* /*who*/) override
			{
				ResteADistance = urand(4, 6);
				Init_AI();
			}
			void EnterEvadeMode(EvadeReason /*why*/) override
			{
				Start_Agro = 0;
				RetireBugDeCombat();
				me->AddUnitState(UNIT_STATE_EVADE);
				//me->SetSpeedRate(MOVE_RUN, 1.5f);													// Vitesse de d�placement
				me->GetMotionMaster()->MoveTargetedHome();											// Retour home
				me->RemoveAllControlled();															// renvois pet

				me->RemoveAura(Buf_1);

				Random = urand(1, 3);
				if (Random == 1 && Spell_evade != 0) { me->CastSpell(me, Spell_evade, true); }		// 1/3 Chance de lancer le sort au respawn ou evade
			}
			void JustReachedHome() override
			{
				me->SetReactState(REACT_AGGRESSIVE);
				me->SetSpeedRate(MOVE_RUN, 1.01f);										// Vitesse par defaut d�finit a 1.01f puisque le patch modification par type,famille test si 1.0f
			}
			void UpdateAI(uint32 diff) override
			{
				// ################################################################################################################################################
				// Emotes hors combat & mouvement #################################################################################################################
				// ################################################################################################################################################
				if ((Cooldown_Npc_Emotes <= diff) && (!me->isMoving()) && (!me->IsInCombat()))
				{
					uint32 Npc_Play_Emotes = Npc_Emotes[urand(0, 21)];
					me->HandleEmoteCommand(Npc_Play_Emotes);
					Cooldown_Npc_Emotes = urand(10000, 20000);
				}
				else
					Cooldown_Npc_Emotes -= diff;

				// ################################################################################################################################################
				// En Combat ######################################################################################################################################
				// ################################################################################################################################################
				if (UpdateVictim() /*&& !me->HasUnitState(UNIT_STATE_MOVE) || me->HasUnitState(UNIT_STATE_CASTING)*/)
				{
					Unit* victim = me->GetVictim();
					Dist = me->GetDistance(victim);

					if (Start_Agro == 0)
					{
						Start_Agro = 1;

						// ########################################################################################################################################
						// Spell a lancer a l'agro 
						// ########################################################################################################################################

							Random = urand(1, 2);
							if (Random == 1 && Spell_agro != 0) { me->CastSpell(victim, Spell_agro, true); }	// 1/2 Chance de lancer le sort d'agro

							// ####################################################################################################################################
					}


					// ############################################################################################################################################
					// Combat 

					if (Dist < 6)
					{ 

						// Spell1 sur la cible
						if (Cooldown_Spell1 <= diff && Spell_1 !=0)
						{
							me->CastSpell(victim, Spell_1, true);
							DoMeleeAttackIfReady();																// Combat en m�l�e
							Cooldown_Spell1 = 3000;
						}
						else Cooldown_Spell1 -= diff;

						// Spell2 sur la cible
						if (Cooldown_Spell2 <= diff && Spell_2 != 0)
						{
							me->CastSpell(victim, Spell_2, true);
							Cooldown_Spell2 = urand(6000, 8000);
						}
						else Cooldown_Spell2 -= diff;
					}

					Mouvement_Contact(diff);

					// ############################################################################################################################################
				}
				// ################################################################################################################################################
				Mouvement_All();
			}

			void RetireBugDeCombat()
			{
				me->CombatStop(true);
				me->RemoveAllControlled();												// renvois pet
				me->DeleteThreatList();
				me->LoadCreaturesAddon();
				me->SetLootRecipient(NULL);
				me->ResetPlayerDamageReq();
				me->SetLastDamagedTime(0);
				me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_DISABLE_MOVE);				// UNROOT
				me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IN_COMBAT);					// Retire flag "en combat"
				me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);				// Retire flag "non attaquable"
			}
			void Mouvement_All()
			{
				if (me->IsAlive() && !me->IsInCombat() /*&& !UpdateVictim()*/ && !me->HasUnitState(UNIT_STATE_MOVE) && (me->GetDistance2d(me->GetHomePosition().GetPositionX(), me->GetHomePosition().GetPositionY()) > 1))
				{
					EnterEvadeMode(EVADE_REASON_SEQUENCE_BREAK);
				}

				if (!UpdateVictim())
					return;

				Dist = me->GetDistance(me->GetVictim());
				if ((Dist > DistanceDeCast) || (me->GetDistance2d(me->GetHomePosition().GetPositionX(), me->GetHomePosition().GetPositionY()) > 40))
				{
					RetireBugDeCombat();
					me->AddUnitState(UNIT_STATE_EVADE);
					EnterEvadeMode(EVADE_REASON_SEQUENCE_BREAK);						// Quite le combat si la cible > 30m (Caster & M�l�e) ou > 40m de home
				}
			}
			void Mouvement_Contact(uint32 diff)
			{
				if (!UpdateVictim())
					return;

				Unit* victim = me->GetVictim();
				Dist = me->GetDistance(victim);

				me->GetMotionMaster()->MoveChase(victim, 1);								// Pour suivre la cible
				//DoMeleeAttackIfReady();													// Combat en m�l�e

				// Si la cible >= 6m (pour �viter bug de rester fig�) ---------------------------------------------------------------------------------------------
				if ((Dist >= 6) && (Cooldown_Anti_Bug_Figer <= diff))
				{
					float x, y, z, mapid;
					x = (victim->GetPositionX() + urand(0, 2) - 1);
					y = (victim->GetPositionY() + urand(0, 2) - 1);
					z = victim->GetPositionZ();
					mapid = victim->GetMapId();
					//me->GetClosePoint(x, y, z, me->GetObjectSize() / 3, 3);
					me->GetMotionMaster()->MovePoint(mapid, x, y, z);
					Cooldown_Anti_Bug_Figer = 1000;
				}
				else Cooldown_Anti_Bug_Figer -= diff;

				// Si la cible < 8m --------------------------------------------------------------------------------------------------------------------------------
				if ((Dist < 8) && (Cooldown_ResteADistance <= diff) )
				{
					Random = urand(1, 5);
					if (Random == 1 || Random == 2)
					{
						Tourne_Au_Tour_En_Combat();											// 2 chances sur 5 tourne au tour de sa victime
					}
					else if (Random == 3)
					{
						Avance_3m_En_Combat();												// 1 chances sur 5 avance
					}
					Cooldown_ResteADistance = urand(7000, 10000);
				}
				else Cooldown_ResteADistance -= diff;

				// Si la cible >= 10m <25m : Charge ---------------------------------------------------------------------------------------------------------------
				if ((Dist >= 10 && Dist <= 25) && (Cooldown_Trop_Loin <= diff))
				{
					Random = urand(1, 4);
						if (Random = 1 && Spell_trop_Loin !=0)
						{
							DoCastVictim(Spell_trop_Loin);
						}
						Cooldown_Trop_Loin = urand(6000,10000);
				}
				else Cooldown_Trop_Loin -= diff;
			}
			void Tourne_Au_Tour_En_Combat()
			{
				if (!UpdateVictim())
					return;

				Unit* victim = me->GetVictim();
				Dist = me->GetDistance(victim);

				float x, y, z, mapid;
				x = (victim->GetPositionX() + urand(0, 4) - 2);
				y = (victim->GetPositionY() + urand(0, 4) - 2);
				z = victim->GetPositionZ();
				mapid = victim->GetMapId();
				//me->GetClosePoint(x, y, z, me->GetObjectSize() / 3, 3);
				me->GetMotionMaster()->MovePoint(mapid, x, y, z);
			}
			void Avance_3m_En_Combat()
			{
				if (!UpdateVictim())
					return;

				Unit* victim = me->GetVictim();
				Dist = me->GetDistance(victim);

				float x, y, z, mapid;
				x = (victim->GetPositionX());
				y = (victim->GetPositionY());
				z = victim->GetPositionZ();
				mapid = victim->GetMapId();
				me->GetClosePoint(x, y, z, me->GetObjectSize() / 3, 3);
				me->GetMotionMaster()->MovePoint(mapid, x, y, z);
			}

			void Bonus_Degat_Arme_Done(int val) // 
			{
				// +- Bonus en % de degat des armes inflig�es a victim
				me->HandleStatModifier(UNIT_MOD_ATTACK_POWER, TOTAL_PCT, val, true);
				me->HandleStatModifier(UNIT_MOD_DAMAGE_OFFHAND, TOTAL_PCT, val, true);
				me->HandleStatModifier(UNIT_MOD_DAMAGE_RANGED, TOTAL_PCT, val, true);
			}
			void Bonus_Armure(int val) // 
			{
				// +- Bonus d'armure 100% = pas de bonus/malus   ex : Bonus_Armure(115); // Bonus d'armure +15%
				me->SetModifierValue(UNIT_MOD_ARMOR, BASE_VALUE, me->GetArmor() * (val / 100));
				me->SetCanModifyStats(true);
				me->UpdateAllStats();
			}
		};




		CreatureAI* GetAI(Creature* creature) const override
		{
			return new Stitch_npc_ai_fantassinAI(creature);
		}
};

//#################################################################################################################################################################
void AddSC_npc_ai_fantassin()
{
	new Stitch_npc_ai_fantassin();
}















