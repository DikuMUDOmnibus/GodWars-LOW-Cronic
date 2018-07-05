#if defined(macintosh)
#include <types.h>
#else
#include <sys/types.h>
#endif
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "merc.h"
#include "garou.h"

void do_gifts( CHAR_DATA *ch, char *argument )
{
    char arg1[MAX_INPUT_LENGTH];
    char arg2[MAX_INPUT_LENGTH];
    char buf [MAX_STRING_LENGTH];
    char lin [MAX_STRING_LENGTH];

    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );

    if (IS_NPC(ch)) return;

    if (!IS_CLASS(ch, CLASS_WEREWOLF))
    {
	send_to_char("Huh?\n\r",ch);
	return;
    }

    if (arg1[0] == '\0' && arg2[0] == '\0')
    { 
 	sprintf(lin,"============================================================================");
	stc(lin, ch);
	stc("                                     Breed\n\r",ch);
	stc(lin, ch);
	sprintf(buf,"        Homid     [ %d ]         Metis    [ %d ]         Lupus       [ %d ]\n\r",
			ch->gifts[HOMID], ch->gifts[METIS], ch->gifts[LUPUS] );
	stc(buf, ch);
        stc(lin, ch);
	stc("                                    Auspice\n\r",ch);
	stc(lin, ch);
	sprintf(buf,"       Ragabash   [ %d ]        Theurge   [ %d ]        Philodox     [ %d ]\n\r",
		      ch->gifts[RAGABASH], ch->gifts[THEURGE], ch->gifts[PHILODOX] );
	stc(buf, ch);
	sprintf(buf,"          Galliard   [ %d ]        Ahroun      [ %d ]\n\r",
			ch->gifts[GALLIARD], ch->gifts[AHROUN] );
	stc(buf, ch);
	stc(lin, ch);
	 /* stc("                                     Tribe\n\r",ch);
	stc(lin, ch);
	sprintf(buf,"     Black Furies [ %d ]     Bone Gnawers [ %d ]    Children of Gaia [ %d ]\n\r",
			ch->gifts[BLACKFURIES], ch->gifts[BONEGNAWERS], ch->gifts[CHILDREN] );
	stc(buf, ch);
	sprintf(buf,"        Fianna    [ %d ]    Get of Fenris [ %d ]      Glass Walkers  [ %d ]\n\r",
			ch->gifts[FIANNA], ch->gifts[GETOFFENRIS], ch->gifts[GLASSWALKERS] );
	stc(buf, ch);
	sprintf(buf,"      Red Talons  [ %d ]     Shadow Lords [ %d ]     Silent Striders [ %d ]\n\r",
			ch->gifts[REDTALONS], ch->gifts[SHADOWLORDS], ch->gifts[SILENTSTRIDERS] );
	stc(buf, ch);
	sprintf(buf,"     Silver Fangs [ %d ]      StarGazers  [ %d ]          Uktena     [ %d ]\n\r",
			ch->gifts[SILVERFANGS], ch->gifts[STARGAZERS], ch->gifts[UKTENA] );
	stc(buf, ch);
	sprintf(buf,"                              Wendigo    [ %d ]\n\r",
			ch->gifts[WENDIGO] );
	stc(buf, ch);
	stc(lin, ch);*/
	return;
    }
    if (arg2[0] == '\0')
    {
      /* Temporary, until gifts are in */
      do_gifts(ch, "");
      return;
    }

    if (!str_cmp(arg2,"learn"))
    {
	int improve;
	int cost;
	int max = 5;

	     if (!str_cmp(arg1,"homid"            )) improve = HOMID;
	else if (!str_cmp(arg1,"metis"            )) improve = METIS;
	else if (!str_cmp(arg1,"lupus"            )) improve = LUPUS;
	else if (!str_cmp(arg1,"ragabash"         )) improve = RAGABASH;
	else if (!str_cmp(arg1,"theurge"          )) improve = THEURGE;
	else if (!str_cmp(arg1,"philodox"         )) improve = PHILODOX;
	else if (!str_cmp(arg1,"galliard"         )) improve = GALLIARD;
	else if (!str_cmp(arg1,"ahroun"           )) improve = AHROUN;
	else if (!str_cmp(arg1,"black furies"     )) improve = BLACKFURIES;
        else if (!str_cmp(arg1,"children of gaia" )) improve = CHILDREN;
        else if (!str_cmp(arg1,"bone gnawers"     )) improve = BONEGNAWERS;
        else if (!str_cmp(arg1,"fianna"           )) improve = FIANNA;
	else if (!str_cmp(arg1,"get of fenris"    )) improve = GETOFFENRIS;
	else if (!str_cmp(arg1,"glass walkers"    )) improve = GLASSWALKERS;
        else if (!str_cmp(arg1,"red talons"       )) improve = REDTALONS;
	else if (!str_cmp(arg1,"shadow lords"     )) improve = SHADOWLORDS;
	else if (!str_cmp(arg1,"silent striders"  )) improve = SILENTSTRIDERS;
	else if (!str_cmp(arg1,"silver fangs"     )) improve = SILVERFANGS;
	else if (!str_cmp(arg1,"stargazers"       )) improve = STARGAZERS;
	else if (!str_cmp(arg1,"uktena"           )) improve = UKTENA;
	else if (!str_cmp(arg1,"wendigo"          )) improve = WENDIGO;
        else
	{
	    do_gifts(ch,"");
	    return;
	}

	cost = (ch->gifts[improve]+1) * 10;
	arg1[0] = UPPER(arg1[0]);
	if ( ch->gifts[improve] >= max )
	{
	    sprintf(buf,"You have already gained all of the %s gifts.\n\r", arg1);
	    send_to_char(buf,ch);
	    return;
	}

	if ( cost > ch->practice )
	{
	    sprintf(buf,"It costs you %d primal to learn a new %s gift.\n\r", cost, arg1);
	    send_to_char(buf,ch);
	    return;
	}

	ch->gifts[improve] += 1;
	ch->practice -= cost;
	sprintf(buf,"You have learned a new %s gift.\n\r", arg1);
	send_to_char(buf,ch);
	return;
    }

    else send_to_char("To learn a new gift, type: gifts '<name>' learn.\n\r",ch);
    return;
}


/*
 * Homid Gifts
 * Level One  - Persuasion    - Like Awe for Vampires.  Automatic
 * Level Two  - Staredown     - Mobs flee
 * Level Three- Disquiet      - Spell effect, -hit\dam in combat for opponent.
 * Level Four - Reshape Object- Rename objects
 * Level Five - Cocoon        - Damage Reduction
 */

/*
 * Homid - 2 - Staredown 
 */

void do_staredown( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *victim;
    char arg[MAX_INPUT_LENGTH];
    char buf[MAX_STRING_LENGTH];

    argument = one_argument( argument, arg );

    if ( IS_NPC(ch) ) return;

    if ( !IS_CLASS(ch, CLASS_WEREWOLF) )
    {
	stc("Huh?\n\r",ch);
	return;
    }

    if ( ch->gifts[HOMID] < 2 )
    {
      stc("You have not learned the Homid gift of Staredown.\n\r",ch);
      return;
    }

    if ( ( victim = get_char_room( ch, arg) ) == NULL )
    {
      stc("They aren't here.\n\r",ch);
      return;
    }

    arg[0] = UPPER(arg[0]);    

    if ( ch->fighting == NULL )
    {
      stc("You must be fighting to use the gift of Staredown.\n\r",ch);
      return;
    }

    if ( victim->fighting == NULL )
    {
      sprintf(buf, "%s is not fighting anyone.", arg);
      stc(buf, ch);
      return;
    }  

    WAIT_STATE( ch, 16 );

    if ( IS_NPC(victim) )
    {
      if ( ch->gifts[HOMID] < 4 )
      {
	  if ( number_range( 1, 3 ) != 1 )
        {
          act("You stare deeply into $N's eyes, but nothing happens.",ch,NULL,victim,TO_CHAR);
          act("$n stares deeply into your eyes.",ch,NULL,victim,TO_VICT);
          act("$n stares deeply into $N's eyes, but nothing happens.",ch,NULL,victim,TO_NOTVICT);
	    return;
        }
      }

      act("You stare into $N's eyes, and $E flees in terror.",ch,NULL,victim,TO_CHAR);
      act("$n stares deeply into your eyes, and you are filled with terror.",ch,NULL,victim,TO_VICT);
      act("$n stares deeply into $N's eyes, and $E flees in terror.",ch,NULL,victim,TO_NOTVICT);
      do_flee( victim, "" );
      return;
    }

    else
    {
      if ( ch->gifts[HOMID] < 5 )
      {
        if ( number_range( 1, 4 ) != 2 )
        {
          act("You stare deeply into $N's eyes, but nothing happens.",ch,NULL,victim,TO_CHAR);
          act("$n stares deeply into your eyes.",ch,NULL,victim,TO_VICT);
          act("$n stares deeply into $N's eyes, but nothing happens.",ch,NULL,victim,TO_NOTVICT);
	    return;
        }
      }

      act("You stare into $N's eyes, and $E flees in terror.",ch,NULL,victim,TO_CHAR);
      act("$n stares deeply into your eyes, and you are filled with terror.",ch,NULL,victim,TO_VICT);
      act("$n stares deeply into $N's eyes, and $E flees in terror.",ch,NULL,victim,TO_NOTVICT);
      do_flee( victim, "" );
      return;
    }

  return;

}

void do_disquiet( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *victim;
    AFFECT_DATA af;
    char arg [MAX_INPUT_LENGTH];

    argument = one_argument( argument, arg );

    if (IS_NPC(ch)) return;

    if (!IS_CLASS(ch, CLASS_WEREWOLF))
    {
      send_to_char("Huh?\n\r", ch);
      return;
    }

    if ( ch->gifts[HOMID] < 3 )
    {
      send_to_char("You have not learned the Homid gift of Disquiet.\n\r", ch);
      return;
    }

    if ( ( victim = get_char_room(ch, arg) ) == NULL ) 
    {
      stc("They are not here.\n\r",ch);
      return;
    }

    if ( is_safe( ch, victim) ) return;

    if ( ch->gnosis[GCURRENT] < 1 )
    {
      stc("You need one point of gnosis power to use the gift of Disquiet.\n\r",ch);
      return;
    }

    af.duration  = ch->gifts[HOMID];
    af.location  = APPLY_DAMROLL;
    af.modifier  = -( ch->gifts[HOMID] * 5 );
    af.bitvector = 0;
    affect_to_char( victim, &af );

    af.duration  = ch->gifts[HOMID];
    af.location  = APPLY_HITROLL;
    af.modifier  = -( ch->gifts[HOMID] * 5 );
    af.bitvector = 0;
    affect_to_char( victim, &af );

    ch->gnosis[CURRENT]--;
    act("Your eyes glow a sinister red as you glare at $N.",ch,NULL,victim,TO_CHAR);
    act("$n's eyes glow a sinister red as $e glares at you.\n\rYou feel odd.",ch,NULL,victim,TO_VICT);
    act("$n's eyes glow a sinister red as $e glares at $N.",ch,NULL,victim,TO_NOTVICT);
    
    return;

}

void do_reshape(CHAR_DATA *ch, char *argument)
{
    char arg1[MAX_STRING_LENGTH];
    char arg2[MAX_STRING_LENGTH];
    OBJ_DATA *obj; 
 
    argument=one_argument(argument,arg1);
    strcpy(arg2,argument);

    if (IS_NPC(ch)) return;

    if (!IS_CLASS(ch,CLASS_WEREWOLF))
    {
      send_to_char("Huh?\n\r",ch);
      return;
    }

    if (ch->gifts[HOMID] < 4)
    {
      send_to_char("You have not achieved the Homid gift of Reshape Object.\n\r",ch);
      return;
    }

    if (arg1 == NULL || arg2 == NULL)
    {
       send_to_char("Syntax: Reshape (Item) (Description).\n\r",ch);
       return;
    }

    if ( ( obj = get_obj_carry( ch,arg1 ) ) == NULL )
    {
      send_to_char("You dont have that item.\n\r",ch);
      return;
    }

    if (strlen(arg2) > 40 || strlen(arg2) < 3)
    {
      send_to_char("The name should be between 3 and 40 characters.\n\r",ch);
      return;
    }

    free_string(obj->name);
    obj->name = str_dup(arg2);
    free_string(obj->short_descr);
    obj->short_descr=str_dup(arg2);
    send_to_char("Ok.\n\r",ch);
    return;
}

void do_cocoon( CHAR_DATA *ch, char *argument )
{
    if ( IS_NPC(ch) ) return;

    if ( !IS_CLASS(ch, CLASS_WEREWOLF) )
    {
	stc("Huh?\n\r",ch);
	return;
    }

    if ( ch->gifts[HOMID] < 5 )
    {
      stc("You haven't learned the Homid gift of Cocoon.\n\r",ch);
      return;
    }


    if ( IS_GAR1(ch, WOLF_COCOON) )
    {
      stc("Your cocoon breaks and falls to the ground.\n\r",ch);
      act("$n's cocoon breaks and falls to the ground.",ch,NULL,NULL,TO_ROOM);
      REMOVE_BIT(ch->garou1, WOLF_COCOON);
      return;
    } 

    if ( ch->gnosis[GCURRENT] < 2 )
    {
      stc("You do not have enough gnosis to use Cocoon.\n\r",ch);
      return;
    }

    else
    {
      stc("Your body is surrounded by a thick, opaque epidermis.\n\r",ch);
      act("$n's body is surrounded by a thick, opaque epidermis.",ch,NULL,NULL,TO_ROOM);
      SET_BIT(ch->garou1, WOLF_COCOON);
      ch->gnosis[GCURRENT]--;
      return;
    }
  return;
}

/*
 * Metis Gifts
 * Level One  - Gift of the Porcupine - Quills
 * Level Two  - Burrow          - Like Demon Travel
 * Level Three- Eyes of the Cat - Nightsight
 * Level Four - Wither Limb	- Withers a limb, easier on NPCs
 * Level Five - Totem Gift	- Random, Special Power. Ooo.
 */

void do_quills( CHAR_DATA *ch, char *argument )
{
    if ( IS_NPC(ch) ) return;

    if ( !IS_CLASS(ch, CLASS_WEREWOLF) )
    {
   	stc("Huh?\n\r",ch);
	return;
    }

    if ( ch->gifts[METIS] < 1 )
    {
        stc("You haven't learned the Gift of the Porcupine.\n\r",ch);
	return;
    }

    if ( IS_SET(ch->newbits, NEW_QUILLS) )
    {
        stc("Your quills soften and twist into fur.\n\r",ch);
	act("$n's quills soften and revert back to fur.",ch,NULL,NULL,TO_ROOM);
        REMOVE_BIT(ch->newbits, NEW_QUILLS);
	return;
    }

    if ( !IS_SET(ch->newbits, NEW_QUILLS) )
    {
        stc("Your fur becomes bristly and sharp.\n\r",ch);
	act("$n's fur becomes bristly and sharp.",ch,NULL,NULL,TO_ROOM);
	SET_BIT(ch->newbits, NEW_QUILLS);
	return;
    }
  return;
}

void do_burrow( CHAR_DATA *ch, char *argument )
{
    char      arg [MAX_INPUT_LENGTH];
    CHAR_DATA *victim;

    argument = one_argument( argument, arg );

    if (IS_NPC(ch)) return;

    if ( !IS_CLASS(ch, CLASS_WEREWOLF) )
    {
        send_to_char("Huh?\n\r",ch);
        return;
    }

    if ( ch->gifts[METIS] < 2 )
    {
        send_to_char("You haven't learned the Metis gift of Burrow.\n\r", ch);
        return;
    }

    if ( arg[0] == '\0' )
    {
        send_to_char("Burrow to who?\n\r", ch);
        return;
    }

    if ( ( victim = get_char_world( ch, arg ) ) == NULL )
    {
        send_to_char( "They are not here.\n\r", ch );
        return;
    }

    if ( IS_NPC(victim) )
    {
        send_to_char( "Not on NPC's.\n\r", ch );
        return;
    }

    if (IS_IMMUNE(victim, IMM_TRAVEL))
    {
        send_to_char("I don't think they want you to do that.\n\r",ch);
        return;
    }        

    if ( victim == ch )
    {
        send_to_char( "You cannot burrow to yourself.\n\r", ch);
        return;
    }

    if ( victim->level != LEVEL_AVATAR || !IS_CLASS(victim, CLASS_WEREWOLF))
    {
        send_to_char( "Nothing happens.\n\r", ch );
        return;
    }

    if (victim->in_room == NULL)
    {
        send_to_char( "Nothing happens.\n\r", ch );
        return;
    }

    send_to_char("You burrow into the ground.\n\r",ch);
    act("$n burrows into the ground.",ch,NULL,NULL,TO_ROOM);
    char_from_room(ch);
    char_to_room(ch,victim->in_room);
    do_look(ch,"");
    send_to_char("You burrow out of the ground.\n\r",ch);
    act("$n burrows out of the ground.",ch,NULL,NULL,TO_ROOM);
    return;
}
  
// Eyes of the Cat - Nightsight.  clan.c

void do_wither( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *victim;
    OBJ_DATA *obj;
    char arg[MAX_INPUT_LENGTH];
    bool blah = FALSE;
    int chance = 0;

    argument = one_argument( argument, arg );

    if ( IS_NPC(ch) ) return;

    if ( !IS_CLASS(ch, CLASS_WEREWOLF) )
    {
	stc("Huh?\n\r",ch);
	return;
    }

    if ( ch->gifts[METIS] < 4 )
    {
	stc("You haven't learned the Metis gift of Wither Limb.\n\r",ch);
	return;
    }

    if ( ch->gnosis[GCURRENT] < 3 )
    {
	stc("You do not have enough gnosis power stored to use the gift of Wither Limb.\n\r",ch);
	return;
    }

    if ( arg[0] == '\0' )
    {
	stc("Who's limb do you wish to wither?\n\r",ch);
	return;
    }

    if ( ( victim = get_char_room( ch, arg ) ) == NULL )
    {
	stc("They aren't here.\n\r",ch);
	return;
    }

    if ( IS_ARM_R(victim, LOST_ARM) && IS_ARM_L(victim, LOST_ARM) )
    {
	stc("They have already lost both their arms.\n\r",ch);
	return;
    }
 
    WAIT_STATE(ch, 32);
    ch->gnosis[CURRENT] -= 3;
    if ( IS_NPC( victim ) )
    {
      chance = number_range( 45, 55 );

      if ( number_percent( ) > chance )
      {
	stc("Nothing happened.\n\r",ch);
	return;
      }

       if ( !IS_ARM_R(victim, LOST_ARM) )
         blah = FALSE;
       else blah = TRUE;

	 if ( !blah )
         {
 	  if (!IS_ARM_R(victim,LOST_ARM))
	      SET_BIT(victim->loc_hp[3],LOST_ARM);
	  if (!IS_BLEEDING(victim,BLEEDING_ARM_R))
	      SET_BIT(victim->loc_hp[6],BLEEDING_ARM_R);
	  if (IS_BLEEDING(victim,BLEEDING_HAND_R))
	      REMOVE_BIT(victim->loc_hp[6],BLEEDING_HAND_R);
	  act("You concentrate deeply and $N's arm starts to wither!",ch,NULL,victim,TO_CHAR);
	  act("$n's eyes glow dark red as $e gazes at you, and your arm begins withering!",ch,NULL,victim,TO_NOTVICT);
	  act("$n gazes evilly at $N, and $S arm begins withering!",ch,NULL,victim,TO_VICT);
	  make_part(victim,"arm");
	  if (IS_ARM_L(victim,LOST_ARM) && IS_ARM_R(victim,LOST_ARM))
	  {
	      if ((obj = get_eq_char( victim, WEAR_ARMS )) != NULL)
		  take_item(victim,obj);
	  }
	  if ((obj = get_eq_char( victim, WEAR_WIELD )) != NULL)
	      take_item(victim,obj);
	  if ((obj = get_eq_char( victim, WEAR_HANDS )) != NULL)
	      take_item(victim,obj);
	  if ((obj = get_eq_char( victim, WEAR_WRIST_R )) != NULL)
	      take_item(victim,obj);
	  if ((obj = get_eq_char( victim, WEAR_FINGER_R )) != NULL)
	      take_item(victim,obj);
	  return;
         }
       
	 else if ( blah )
	 {
 	  if (!IS_ARM_L(victim,LOST_ARM))
	      SET_BIT(victim->loc_hp[2],LOST_ARM);
	  else blah = TRUE;
	  if (!IS_BLEEDING(victim,BLEEDING_ARM_L))
	      SET_BIT(victim->loc_hp[6],BLEEDING_ARM_L);
	  if (IS_BLEEDING(victim,BLEEDING_HAND_L))
	      REMOVE_BIT(victim->loc_hp[6],BLEEDING_HAND_L);
	  act("You concentrate deeply and $N's arm starts to wither!",ch,NULL,victim,TO_CHAR);
	  act("$n's eyes glow dark red as $e gazes at you, and your arm begins withering!",ch,NULL,victim,TO_NOTVICT);
	  act("$n gazes evilly at $N, and $S arm begins withering!",ch,NULL,victim,TO_VICT);
	  make_part(victim,"arm");
	  if (IS_ARM_L(victim,LOST_ARM) && IS_ARM_R(victim,LOST_ARM))
	  {
	      if ((obj = get_eq_char( victim, WEAR_ARMS )) != NULL)
		  take_item(victim,obj);
	  }
	  if ((obj = get_eq_char( victim, WEAR_WIELD )) != NULL)
	      take_item(victim,obj);
	  if ((obj = get_eq_char( victim, WEAR_HANDS )) != NULL)
	      take_item(victim,obj);
	  if ((obj = get_eq_char( victim, WEAR_WRIST_L )) != NULL)
	      take_item(victim,obj);
	  if ((obj = get_eq_char( victim, WEAR_FINGER_L )) != NULL)
	      take_item(victim,obj);
	  return;
        }
    }

    else if ( !IS_NPC(victim) )
    {
	chance = 15;
      if ( number_percent( ) > chance )
      {
	act("You gaze intently at $N.\n\rNothing happens.",ch,NULL,victim,TO_CHAR);
	act("$n gazes eerily at you.\n\rNothing happens.",ch,NULL,victim,TO_VICT);
	act("$n gazes eerily at $N.",ch,NULL,victim,TO_NOTVICT);
	return;
      }


       if ( !IS_ARM_R(victim, LOST_ARM) )
         blah = FALSE;
       else blah = TRUE;

	 if ( !blah )
         {
 	  if (!IS_ARM_R(victim,LOST_ARM))
	      SET_BIT(victim->loc_hp[3],LOST_ARM);
	  if (!IS_BLEEDING(victim,BLEEDING_ARM_R))
	      SET_BIT(victim->loc_hp[6],BLEEDING_ARM_R);
	  if (IS_BLEEDING(victim,BLEEDING_HAND_R))
	      REMOVE_BIT(victim->loc_hp[6],BLEEDING_HAND_R);
	  act("You concentrate deeply and $N's arm starts to wither!",ch,NULL,victim,TO_CHAR);
	  act("$n's eyes glow dark red as $e gazes at you, and your arm begins withering!",ch,NULL,victim,TO_NOTVICT);
	  act("$n gazes evilly at $N, and $S arm begins withering!",ch,NULL,victim,TO_VICT);
	  make_part(victim,"arm");
	  if (IS_ARM_L(victim,LOST_ARM) && IS_ARM_R(victim,LOST_ARM))
	  {
	      if ((obj = get_eq_char( victim, WEAR_ARMS )) != NULL)
		  take_item(victim,obj);
	  }
	  if ((obj = get_eq_char( victim, WEAR_WIELD )) != NULL)
	      take_item(victim,obj);
	  if ((obj = get_eq_char( victim, WEAR_HANDS )) != NULL)
	      take_item(victim,obj);
	  if ((obj = get_eq_char( victim, WEAR_WRIST_R )) != NULL)
	      take_item(victim,obj);
	  if ((obj = get_eq_char( victim, WEAR_FINGER_R )) != NULL)
	      take_item(victim,obj);
	  return;
	 }

	 else if ( blah )
	 {
 	  if (!IS_ARM_L(victim,LOST_ARM))
	      SET_BIT(victim->loc_hp[2],LOST_ARM);
	  else blah = TRUE;
	  if (!IS_BLEEDING(victim,BLEEDING_ARM_L))
	      SET_BIT(victim->loc_hp[6],BLEEDING_ARM_L);
	  if (IS_BLEEDING(victim,BLEEDING_HAND_L))
	      REMOVE_BIT(victim->loc_hp[6],BLEEDING_HAND_L);
	  act("You concentrate deeply and $N's arm starts to wither!",ch,NULL,victim,TO_CHAR);
	  act("$n's eyes glow dark red as $e gazes at you, and your arm begins withering!",ch,NULL,victim,TO_NOTVICT);
	  act("$n gazes evilly at $N, and $S arm begins withering!",ch,NULL,victim,TO_VICT);
	  make_part(victim,"arm");
	  if (IS_ARM_L(victim,LOST_ARM) && IS_ARM_R(victim,LOST_ARM))
	  {
	      if ((obj = get_eq_char( victim, WEAR_ARMS )) != NULL)
		  take_item(victim,obj);
	  }
	  if ((obj = get_eq_char( victim, WEAR_WIELD )) != NULL)
	      take_item(victim,obj);
	  if ((obj = get_eq_char( victim, WEAR_HANDS )) != NULL)
	      take_item(victim,obj);
	  if ((obj = get_eq_char( victim, WEAR_WRIST_L )) != NULL)
	      take_item(victim,obj);
	  if ((obj = get_eq_char( victim, WEAR_FINGER_L )) != NULL)
	      take_item(victim,obj);
	  return;
        }
      }
  return;
}

/*
 * Ahroun Gifts
 */

void do_razorclaws(CHAR_DATA *ch, char *argument)
{

   if (IS_CLASS(ch, CLASS_WEREWOLF))
   {
	if ( ch->gifts[AHROUN] <= 1 )
        {
	    stc("Huh?\n\r",ch);
	    return;
	}
   }

   if ( !IS_VAMPAFF(ch, VAM_CLAWS) )
   {
	stc("Your talons aren't even extended!\n\r",ch);
	return;
   }

   if ( IS_GAR1(ch, WOLF_RAZORCLAWS ) )
   {
      stc("Your claws lose their razor-sharp edge.\n\r",ch);
      act("$n's claws lose their razor-sharp edge.",ch,NULL,NULL,TO_ROOM);
      REMOVE_BIT(ch->garou1, WOLF_RAZORCLAWS);
   }
   else
   {
	send_to_char("You hone your claws to a razor sharpness.\n\r",ch);
	act("$n hones $s claws to a razor sharpness.",ch, NULL, NULL, TO_ROOM);
	SET_BIT( ch->garou1, WOLF_RAZORCLAWS );
   }

  return;

}

