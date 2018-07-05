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
#include "player.h"





void do_guide( CHAR_DATA *ch, char *argument )


{


    CHAR_DATA *victim;


    int ch_age = years_old(ch);


    char      arg [MAX_INPUT_LENGTH];





    argument = one_argument( argument, arg );





    if (IS_NPC(ch)) return;





    if (!IS_CLASS(ch, CLASS_MONK) )


    {


	send_to_char("Huh?\n\r",ch);


	return;


    }





    if ( arg[0] == '\0' )


    {


	send_to_char( "Guide whom?\n\r", ch );


	return;


    }





    if ( ch_age < 50 )


    {


       send_to_char("They must be 50 to become a monk!\n\r", ch);


       return;


    }


    


    if ( ( victim = get_char_room( ch, arg ) ) == NULL )


    {


	send_to_char( "They aren't here.\n\r", ch );


	return;


    }





    if ( IS_NPC(victim) )


    {


	send_to_char( "Not on NPC's.\n\r", ch );


	return;


    }





    if ( IS_IMMORTAL(victim) )


    {


	send_to_char( "Not on Immortals's.\n\r", ch );


	return;


    }





    if ( ch == victim )


    {


	send_to_char( "You cannot guide yourself.\n\r", ch );


	return;


    }





    if (IS_CLASS(victim, CLASS_MONK))


    {


	send_to_char( "They are already guided.\n\r", ch );


	return;


    }





    if ( victim->level != LEVEL_AVATAR && !IS_IMMORTAL(victim) )


    {


	send_to_char( "You can only guide avatars.\n\r", ch );


	return;


    }





    if (IS_CLASS(victim, CLASS_VAMPIRE) ||


IS_SET(victim->pcdata->stats[UNI_AFF], VAM_MORTAL))


    {


	send_to_char( "You are unable to guide vampires!\n\r", ch );


	return;


    }





    if (IS_CLASS(victim, CLASS_WEREWOLF))


    {


	send_to_char( "You are unable to guide werewolves!\n\r", ch );


	return;


    }





    if (IS_CLASS(victim, CLASS_DEMON) || IS_SET(victim->special,


SPC_CHAMPION))


    {


	send_to_char( "You are unable to guide demons!\n\r", ch );


	return;


    }








    if (IS_CLASS(victim, CLASS_HIGHLANDER))


    {


	send_to_char( "You are unable to guide highlanders.\n\r", ch );


	return;


    }


    


/*


    if (IS_CLASS(victim, CLASS_PALADIN))


    {


    send_to_char( "You are unable to guide paladins.\n\r", ch);


    return;


    }





	if (IS_CLASS(victim, CLASS_NINJA))


	{


	send_to_char( "You are unable to guide ninjas.\n\r", ch);


	return;


	}


	


*/


	if (IS_CLASS(victim, CLASS_DROW))


	{


	send_to_char( "You are unable to guide drow.\n\r", ch);


	return;


	}


	


	





    if (!IS_IMMUNE(victim,IMM_VAMPIRE))


    {


	send_to_char( "You cannot guide an unwilling person.\n\r", ch );


	return;


    }





    if (ch->exp < 50000)


    {


	send_to_char("You cannot afford the 50000 exp required to guide


them.\n\r",ch);


	return;


    }





    if (victim->exp < 50000)


    {


	send_to_char("They cannot afford the 50000 exp required to be


guided from you.\n\r",ch);


	return;


    }





    ch->exp -= 50000;


    victim->exp -= 50000;





    act("You guide $N in the ways of god.", ch, NULL, victim, TO_CHAR);


    act("$n guide $N in the ways of god.", ch, NULL, victim, TO_NOTVICT);


    act("$n guide you in the way of god.", ch, NULL, victim, TO_VICT);


    victim->level = LEVEL_ARCHMAGE;


    if (victim->trust > 3)


    victim->trust = victim->trust;


    else


    victim->trust = LEVEL_AVATAR;


    send_to_char( "You are now a monk.\n\r", victim );


    free_string(victim->lord);


    victim->lord = str_dup(ch->name);


    victim->class    = CLASS_MONK;


    save_char_obj(ch);


    save_char_obj(victim);


    return;


}





void do_mantra( CHAR_DATA *ch, char *argument )


{


    char arg1[MAX_INPUT_LENGTH];


    char arg2[MAX_INPUT_LENGTH];


    char buf[MAX_STRING_LENGTH];





    argument = one_argument( argument, arg1 );


    argument = one_argument( argument, arg2 );





    if (IS_NPC(ch)) return;





    if (!IS_CLASS(ch, CLASS_MONK))


    {


	send_to_char("Huh?\n\r",ch);


	return;


    }





    if (arg1[0] == '\0' && arg2[0] == '\0')


    {


	sprintf(buf,"Mantras Learned (%d).\n\r", ch->pcdata->powers[PMONK]);


	send_to_char(buf,ch);


	send_to_char("Powers you have:\n\r", ch);


	  if ( ch->pcdata->powers[PMONK] < 1 )


	   send_to_char("You have no knowledge of any Mantras.\n\r", ch);


	  if ( ch->pcdata->powers[PMONK] >= 1 )


	   send_to_char("Eyes of God.\n\r", ch);


	  if ( ch->pcdata->powers[PMONK] >= 2 )


	   send_to_char("Shield/Read Aura/Scry\n\r", ch);


	  if ( ch->pcdata->powers[PMONK] >= 3 )


	   send_to_char("Sacred Invisibility\n\r", ch);


	  if ( ch->pcdata->powers[PMONK] >= 4 )


	   send_to_char("Heart of the Oak\n\r", ch);


	  if ( ch->pcdata->powers[PMONK] >= 5 )


	   send_to_char("Adamantium Hands\n\r", ch);


	  if ( ch->pcdata->powers[PMONK] >= 6 )


	   send_to_char("Skin of Steel\n\r", ch);


	  if ( ch->pcdata->powers[PMONK] >= 7 )


	   send_to_char("The Almighty Favor\n\r", ch);


	  if ( ch->pcdata->powers[PMONK] >= 8 )


	   send_to_char("Dark Blaze\n\r", ch);


	  if ( ch->pcdata->powers[PMONK] >= 9 )


	   send_to_char("Celestial Path\n\r", ch);


	  if ( ch->pcdata->powers[PMONK] >= 10 )


	   send_to_char("Prayer of the Ages\n\r", ch);


	  if ( ch->pcdata->powers[PMONK] >= 11 )


	   send_to_char("Cloak of Life\n\r", ch);


	  if ( ch->pcdata->powers[PMONK] >= 12 )


	   send_to_char("Gods Heal.\n\r", ch);
	   
	   if ( ch->pcdata->powers[PMONK] >= 13 )


	   send_to_char("Gods Hold.\n\r", ch);
	   
	   if ( ch->pcdata->powers[PMONK] >= 14 )


	   send_to_char("Will Be In Soon\n\r", ch);
	   
	   if ( ch->pcdata->powers[PMONK] >= 15 )


	   send_to_char("You have completed your Mantra training.\n\r", ch);


	return;


    }





	if (arg2[0] == '\0')


    {


	if (!str_cmp(arg1,"power"))


	{


	  if ( ch->pcdata->powers[PMONK] < 1 )


	   send_to_char("You have no knowledge of any Mantras.\n\r", ch);


	  if ( ch->pcdata->powers[PMONK] >= 1 )


	   send_to_char("Eyes of God.\n\r", ch);


	  if ( ch->pcdata->powers[PMONK] >= 2 )


	   send_to_char("Shield/Read Aura/Scry\n\r", ch);


	  if ( ch->pcdata->powers[PMONK] >= 3 )


	   send_to_char("Sacred Invisibility\n\r", ch);


	  if ( ch->pcdata->powers[PMONK] >= 4 )


	   send_to_char("Heart of the Oak\n\r", ch);


	  if ( ch->pcdata->powers[PMONK] >= 5 )


	   send_to_char("Adamantium Hands\n\r", ch);


	  if ( ch->pcdata->powers[PMONK] >= 6 )


	   send_to_char("Skin of Steel\n\r", ch);


	  if ( ch->pcdata->powers[PMONK] >= 7 )


	   send_to_char("The Almighty Favor\n\r", ch);


	  if ( ch->pcdata->powers[PMONK] >= 8 )


	   send_to_char("Dark Blaze\n\r", ch);


	  if ( ch->pcdata->powers[PMONK] >= 9 )


	   send_to_char("Celestial Path\n\r", ch);


	  if ( ch->pcdata->powers[PMONK] >= 10 )


	   send_to_char("Prayer of the Ages\n\r", ch);


	  if ( ch->pcdata->powers[PMONK] >= 11 )


	   send_to_char("Cloak of Life\n\r", ch);


	  if ( ch->pcdata->powers[PMONK] >= 12 )


	   send_to_char("Gods Heal.\n\r", ch);
	   
	   if ( ch->pcdata->powers[PMONK] >= 13 )


	   send_to_char("Gods Hold.\n\r", ch);
	   
	   if ( ch->pcdata->powers[PMONK] >= 14 )


	   send_to_char("Will Be In Soon.\n\r", ch);
	   
	   if ( ch->pcdata->powers[PMONK] >= 15 )


	   send_to_char("You have learned all the mantras.\n\r", ch);


	  return;


	}


	sprintf(buf,"Mantras Learned (%d).\n\r",


ch->pcdata->powers[PMONK]);


	send_to_char(buf,ch);


	return;


    }


    if (!str_cmp(arg2,"improve"))


    {


	int improve;


	int cost;


	int max;





	if (!str_cmp(arg1,"power"   )) 


	{


	improve = PMONK;   


	max=14;

	}


	else


	{


	    send_to_char("________________________________\n\r",ch);


	    send_to_char("|                               |\n\r",ch);


	    send_to_char("|             Mantras           |\n\r",ch);


	    send_to_char("|_______________________________|\n\r",ch);


	    send_to_char("| 1.        Eye of God          |\n\r",ch);


	    send_to_char("| 2.   Shield/Read Aura/Scry    |\n\r",ch);


	    send_to_char("| 3.    Sacred Invisibility     |\n\r",ch);


	    send_to_char("| 4.     Adamantium Hands       |\n\r",ch);


	    send_to_char("| 5.       Flaming Hands        |\n\r",ch);


	    send_to_char("| 6.       Skin of Steel        |\n\r",ch);


	    send_to_char("| 7.    The Almighty's Favor    |\n\r",ch);


	    send_to_char("| 8.        Dark Blaze          |\n\r",ch);


	    send_to_char("| 9.      Celestial Path        |\n\r",ch);


	    send_to_char("| 10.   Pray of the Ages        |\n\r",ch);


	    send_to_char("| 11.     Cloak of Life         |\n\r",ch);


	    send_to_char("|_______________________________|\n\r",ch);


	    return;


	}							


	cost = (ch->pcdata->powers[improve]+1) * 10;


	arg1[0] = UPPER(arg1[0]);


	if ( ch->pcdata->powers[improve] >= max )


	{


	    sprintf(buf,"You have already gained all the known Mantras.\n\r");


	    send_to_char(buf,ch);


	    return;


	}


	if ( cost > ch->practice )


	{


	    sprintf(buf,"It costs you %d primal learn the Mantra.\n\r",


cost);


	    send_to_char(buf,ch);


	    return;


	}


	ch->pcdata->powers[improve] += 1;


	ch->practice -= cost;


	sprintf(buf,"You learn a Mantra.\n\r");


	send_to_char(buf,ch);


    }


    else send_to_char("To learn a Mantra, type: Mantra power improve.\n\r",ch);


	return;


}





void do_cloak( CHAR_DATA *ch, char *argument )


{


	if ( IS_NPC(ch) )


     return;





    if (!IS_CLASS(ch, CLASS_MONK))


    {


	  send_to_char("Huh?\n\r",ch);


	  return;


    }





    if (ch->pcdata->powers[PMONK] < 11 )


    {


	  send_to_char("You do not have the mantra of the Cloak of Life.\n\r",ch);


	  return;


    }





    if (ch->move < 1000)


    {


	 send_to_char("You do not have enough vitality to do that.\n\r",ch);


	 return;


    }





    if (IS_SET(ch->newbits, NEW_MONKCLOAK))


    {


       REMOVE_BIT(ch->newbits, NEW_MONKCLOAK);


       send_to_char("You are no longer protected by the Almighty's cloak.\n\r",ch);


       act("$n is no longer protected by the Almighty's cloak of life.\n\r",ch,NULL,NULL,TO_ROOM);


       return;


    }


    if (!IS_SET(ch->newbits,NEW_MONKCLOAK))


    {


       SET_BIT(ch->newbits, NEW_MONKCLOAK);


	 send_to_char("You are protected by the Almighty's cloak.\n\r",ch);


	 act("$n is protected by the Almighty's cloak of life.\n\r",ch,NULL,NULL,TO_ROOM);


	 ch->move = ch->move - 1000;


	 return;


    }





}





void do_prayofages( CHAR_DATA *ch, char *argument )


{


    char      arg [MAX_INPUT_LENGTH];


    CHAR_DATA *victim;





    argument = one_argument( argument, arg );





    if (IS_NPC(ch)) return;





    if (IS_CLASS(ch, CLASS_MONK))


    {


	if (ch->pcdata->powers[PMONK] < 10)


	{


	    send_to_char("You haven't gotten the Pray of Ages mantra.\n\r",ch);


	    return;


	}


    }


    else


    {


	send_to_char("Huh?\n\r",ch);


	return;


    }





    if (arg[0] == '\0')


    {


	send_to_char("Who do you wish to be teleported to?\n\r",ch);


	return;


    }





    if ( ( victim = get_char_world( ch, arg ) ) == NULL )


    {


	send_to_char( "Nobody by that name.\n\r", ch );


	return;


    }





    if ( IS_NPC(victim) )


    {


	send_to_char( "Not on NPC's.\n\r", ch );


	return;


    }


    if ( victim == ch ) 


    {


		send_to_char( "Nothing happens.\n\r", ch);


		return; 


    }





    if ( victim->level != LEVEL_AVATAR && !IS_CLASS(victim, CLASS_MONK))


    {


	send_to_char( "Nothing happens.\n\r", ch );


	return;


    }





    if (victim->in_room == NULL)


    {


	send_to_char( "Nothing happens.\n\r", ch );


	return;


    }





    if ( victim->position != POS_STANDING )


    {


	send_to_char( "You are unable to focus on their location.\n\r", ch );


	return;


    }





    send_to_char("You pray to god and vanish in a flash of light.\n\r",ch);


    act("$n utters a prayer to god and vanishes in a flash of light.",ch,NULL,NULL,TO_ROOM);


    char_from_room(ch);


    char_to_room(ch,victim->in_room);


    do_look(ch,"");


    send_to_char("You emerge from a portal in the sky.\n\r",ch);


    act("$n appears from a portal in the sky.",ch,NULL,NULL,TO_ROOM);


    return;


}





void do_sacredinvis( CHAR_DATA *ch, char *argument)


{


    if ( IS_NPC(ch) )


     return;





    if (!IS_CLASS(ch, CLASS_MONK))


    {


	  send_to_char("Huh?\n\r",ch);


	  return;


    }





    if (ch->pcdata->powers[PMONK] < 3)


    {


	  send_to_char("You do not have the mantra of Sacred Invisibility.\n\r",ch);


	  return;


    }





        if ( IS_SET(ch->affected_by, AFF_HIDE) )


    {


	REMOVE_BIT(ch->affected_by, AFF_HIDE);


	send_to_char( "You emerge from your cloak of invisibility.\n\r", ch );


	act("$n slowly emerges from a cloak of holy invisibility.",ch,NULL,NULL,TO_ROOM);


    }


    else


    {
	if (ch->move < 500) {
	send_to_char("You do not have enough movement to do this.\n\r", ch );
	return;}



	send_to_char( "You slowly fade into a cloak of invisibility.\n\r", ch );


	act("$n slowly fades into a cloak of holy invisibility..",ch,NULL,NULL,TO_ROOM);


	SET_BIT(ch->affected_by, AFF_HIDE);


	ch->move -= 500;
    }


    return;


}








void do_flaminghands( CHAR_DATA *ch, char *argument)


{


    if ( IS_NPC(ch) )


     return;





    if (!IS_CLASS(ch, CLASS_MONK))


    {


	  send_to_char("Huh?\n\r",ch);


	  return;


    }





    if (ch->pcdata->powers[PMONK] <5)


    {


	  send_to_char("You do not have the mantra of Flaming Hands.\n\r",ch);


	  return;


    }





    if (IS_SET(ch->newbits, NEW_MONKFLAME))


    {


	  REMOVE_BIT(ch->newbits, NEW_MONKFLAME);


        send_to_char("Your hands are no longer engulfed by flames.\n\r",ch);


	  act("$n's hands are no longer engulfed by flames.\n\r",ch,NULL,NULL,TO_ROOM);


	  return;


    }


    if (!IS_SET(ch->newbits, NEW_MONKFLAME))


    {


	  SET_BIT(ch->newbits, NEW_MONKFLAME);


	  send_to_char("Your hands are engulfed by flames!\n\r",ch);


	  act("$n's hands are engulfed by flames!\n\r",ch,NULL,NULL,TO_ROOM);


	  return;


    }


}





void do_adamantium( CHAR_DATA *ch, char *argument)
{
     if ( IS_NPC(ch) )
     return;

    if (!IS_CLASS(ch, CLASS_MONK))
    {
	  send_to_char("Huh?\n\r",ch);
	  return;
    }

    if (ch->monkab[BODY] < 1)
    {
	  send_to_char("You have not learned this ability yet.\n\r",ch);
	  return;
    }

    if (IS_SET(ch->newbits, NEW_MONKADAM))
    {
	  REMOVE_BIT(ch->newbits, NEW_MONKADAM);
        send_to_char("Your hands resume their normal toughness.\n\r",ch);
	  act("$n's hands resume their normal toughness.\n\r",ch,NULL,NULL,TO_ROOM);
	  return;
    }

    if (!IS_SET(ch->newbits, NEW_MONKADAM))
    {
	  SET_BIT(ch->newbits, NEW_MONKADAM);
	  send_to_char("Your hands turn as hard as adamantium!\n\r",ch);
	  act("$n's hands turn as hard as adamantium!\n\r",ch,NULL,NULL,TO_ROOM);
	  return;
    }
}



void do_celestial( CHAR_DATA *ch, char *argument)
{
    CHAR_DATA *victim;
    char buf[MAX_STRING_LENGTH];
    char arg[MAX_INPUT_LENGTH];
    argument = one_argument(argument, arg);

    if ( IS_NPC(ch) )
	return;

    if (!IS_CLASS(ch, CLASS_MONK))
	{
         send_to_char("Huh?\n\r",ch);
	   return;
	}

    if (ch->monkab[SPIRIT] < 4)
	{
	   send_to_char("You must obtain level 4 in Spirit to use Celestial Path.\n\r",ch);
	   return;
	}

    if (ch->move < 250)
	{
	    stc("You do not have enough vitality to do that!\n\r",ch);
	    return;
	}

    if ( arg[0] == '\0' )
    {
	send_to_char( "Open a celestial path to who?\n\r", ch );
	return;
    }

    if ( ( victim = get_char_world( ch, arg ) ) == NULL )
    {
	send_to_char( "They aren't here.\n\r", ch );
	return;
    }
 
	if  (!IS_NPC(victim) && !IS_IMMUNE(victim, IMM_SUMMON) )
	{
	send_to_char("There summon is off!\n\r",ch);
	return;
}
   
 if (ch == victim)
    {
        send_to_char("But you're already at yourself!\n\r",ch);
	  return;
    }

    if (victim->in_room->vnum == ch->in_room->vnum)
    {
	  send_to_char("But you're already there!\n\r",ch);
        return;
    }

    char_from_room(ch);
    char_to_room(ch,victim->in_room);
    if (IS_NPC(victim))
    {
    sprintf(buf,"You concentrate and open a Celestial path to %s!\n\r",victim->short_descr);
    send_to_char(buf, ch);
    }

    if (!IS_NPC(victim))
    {
    sprintf(buf,"You concentrate and open a Celestial path to %s!\n\r",victim->name);
    send_to_char(buf, ch);
    }
    act("$n appears before you through a Celestial portal!\n\r",ch,NULL,NULL,TO_ROOM);
    do_look(ch, "auto");
    ch->move = ch->move - 250;
    return;
}





void do_godseye( CHAR_DATA *ch, char *argument )


{


    if ( IS_NPC(ch) )


	return;


  


  	if ( !IS_CLASS(ch, CLASS_MONK) )


  	{


  	send_to_char("Huh?\n\r", ch);


  	return;


  	}


  	


  	if ( ch->pcdata->powers[PMONK] < 1 )


  	{


  	  send_to_char("You have not learned the Eyes of God Mantra.\n\r", ch);


  	  return;


  	}


  


    if ( IS_SET(ch->act, PLR_HOLYLIGHT) )


    {


	REMOVE_BIT(ch->act, PLR_HOLYLIGHT);


	send_to_char( "Your view shimmers into mortal vision.\n\r", ch );


    }


    else


    {


	send_to_char( "God answers your prayers and gives you his vision.\n\r", ch );


	SET_BIT(ch->act, PLR_HOLYLIGHT);


    }


    return;


}





void do_steelskin( CHAR_DATA *ch, char *argument )


{


    if ( IS_NPC(ch) )


	return;


  


  	if (!IS_CLASS(ch, CLASS_MONK) )


  	{


  	send_to_char("Huh?\n\r", ch);


  	return;


  	}


  	


  	if ( ch->pcdata->powers[PMONK] < 6 )


  	{


  	  send_to_char("You have not learned the Steel of Skin Mantra.\n\r", ch);


  	  return;


  	}





/* Remove steel shield in handler.c */


  


    if ( IS_SET(ch->newbits, NEW_MONKSKIN) )


    {


	REMOVE_BIT( ch->newbits, NEW_MONKSKIN );


	send_to_char( "Your steel shield disappears.\n\r", ch );


    }


    else


    {


	send_to_char( "You are surrounded by a steel shield\n\r", ch );


	SET_BIT(ch->newbits, NEW_MONKSKIN);


    }


    return;


}





/* Monk Targeting in ninja.c */





void do_godsbless( CHAR_DATA *ch, char *argument )


{





    int sn;


    int level;


    int spelltype;





    if ( IS_NPC(ch) )


	return;


  


  	if (!IS_CLASS(ch, CLASS_MONK))


  	{


  	send_to_char("Huh?\n\r", ch);


  	return;


  	}


  	


  	if ( IS_CLASS(ch, CLASS_MONK) && ch->pcdata->powers[PMONK] < 7 )


  	{


  	send_to_char("You have not learned the Almighty Favor Mantra.\n\r", ch);


  	return;


  	}


  	


  	if (ch->mana < 1000) 


  	{


	send_to_char("You don't have enough mana.\n\r", ch );


	return;


	}


	


    if ( ( sn = skill_lookup( "godbless" ) ) < 0 ) return;


    spelltype = skill_table[sn].target;


    level = 500;


    (*skill_table[sn].spell_fun) ( sn, level, ch, ch );


    WAIT_STATE( ch, 12 );


	ch->mana = ch->mana - 3000;


    return;


}





void do_godsfavor( CHAR_DATA *ch, char *argument )


{


	


	if ( IS_NPC(ch) )


	return; 





   if (!IS_CLASS(ch, CLASS_MONK))


      {


	send_to_char("Huh?\n\r",ch);


	return;


	}





   if ( IS_CLASS(ch, CLASS_MONK) && ch->pcdata->powers[PMONK] < 8 )


  	{


  	send_to_char("You have not learned the Almighty's Favor Mantra.\n\r", ch);


  	return;


  	}





   if (ch->move < 1500)


	{


	send_to_char("You do not have the vitality to do that!\n\r",ch);


	return;


	}


 if (!IS_SET(ch->newbits, NEW_MONKFAVOR))


   {


     SET_BIT(ch->newbits, NEW_MONKFAVOR);


     send_to_char("God smiles upon you and you feel strengthened.\n\r",ch);


     act("$n is blessed by the Almighty.",ch,NULL,NULL,TO_ROOM);


     ch->move = ch->move - 1500;


     WAIT_STATE(ch, 4);


     return;


   }


 if (IS_SET(ch->newbits, NEW_MONKFAVOR))


   {


     REMOVE_BIT(ch->newbits, NEW_MONKFAVOR);


     send_to_char("You feel weaker as The Almighty's favor leaves you.\n\r",ch);


     act("$n no longer looks so holy.",ch,NULL,NULL,TO_ROOM);


     return;


   }


}













/*
void do_darkblaze( CHAR_DATA *ch, char *argument )


{



	CHAR_DATA *och;


	


    if ( IS_NPC(ch) )


	return;


  


  	if (!IS_CLASS(ch, CLASS_MONK) )


  	{


  	send_to_char("Huh?\n\r", ch);


  	return;


  	}


  	


  	if ( IS_CLASS(ch, CLASS_MONK) && ch->pcdata->powers[PMONK] > 8 )


  	{


  	send_to_char("You have not learned the Dark Blaze Mantra.\n\r", ch);


  	return;


  	}


  	


  	if (ch->move < 1000) 


  	{


	send_to_char("You don't have enough moves.\n\r", ch );


	return;


	}


	


	if ((och->in_room == ch->in_room))


	{


		act("$n prays to God and a flash of light blinds you.", ch , argument , och, TO_VICT);


		SET_BIT(och->flag3, AFF_TOTALBLIND);



		continue;


	} 


	ch->move = ch->move - 1000;


    return;


} 
*/
void do_godsheal( CHAR_DATA *ch, char *argument )
{
    if (IS_NPC(ch))
	return;

     if ( IS_CLASS(ch, CLASS_MONK) && ch->pcdata->powers[PMONK] < 12 )

    {
	send_to_char("You need to obtain heal to use heal.\n\r",ch);
	return;
    }

    if (ch->mana < 300)
    {
	send_to_char("You don't have enough energy to use heal.\n\r",ch);
	return;
    }

    if (ch->fighting != NULL)
    {
	send_to_char("You are surrounded in a glowing halo of energy.\n\r",ch);
	act("$n is surrounded by a glowing halo, and looks very relaxed.\n\r",ch,NULL,NULL,TO_ROOM);
        ch->hit += 150;
        ch->mana -= 400;
	WAIT_STATE(ch,12);
	return;
    }

    if (ch->fighting == NULL)
    {
	send_to_char("You feel a mighty force lay his hands on you.\n\r",ch);
	act("$n is surrounded by a glowing halo, and looks very relaxed.\n\r",ch,NULL,NULL,TO_ROOM);
	ch->hit += 500;
	ch->mana -= 400;
	WAIT_STATE(ch,8);
	return;
    }
  }


void do_ghold( CHAR_DATA *ch,char *argument )
{
  if (IS_NPC(ch)) return;
  
  if (!IS_CLASS(ch,CLASS_MONK))
  {
  send_to_char("Huh?\n\r",ch);
  return;
  }
   if ( IS_CLASS(ch, CLASS_MONK) && ch->pcdata->powers[PMONK] < 13 )

    {
  send_to_char("You need Mantra 13 to use gods hold.\n\r",ch);
  return;
  }
  if (!IS_SET(ch->newbits, NEW_JAWLOCK))
  {
  send_to_char("God joins you at your side to hold your foe.\n\r",ch);
  SET_BIT(ch->newbits, NEW_JAWLOCK);
  return;
  }
  else if (IS_SET(ch->newbits, NEW_JAWLOCK))
  { 
  send_to_char("God leaves your side.\n\r",ch);
  REMOVE_BIT(ch->newbits,NEW_JAWLOCK);
  return;
  }
}
