/*Coded by Xkilla and made by Xkilla*/
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

void do_makezerg( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *victim;
    char      arg [MAX_INPUT_LENGTH];

    argument = one_argument( argument, arg );

    if (IS_NPC(ch)) return;

    if (!IS_CLASS(ch, CLASS_ZERG) )
    {
	send_to_char("Huh?\n\r",ch);
	return;
    }

    if ( arg[0] == '\0' )
    {
	send_to_char( "Class whom?\n\r", ch );
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
	send_to_char( "You cannot class yourself.\n\r", ch );
	return;
    }

    if (IS_CLASS(victim, CLASS_MONK))
    {
	send_to_char( "They are Monk.\n\r", ch );
	return;
    }

    if ( victim->level != LEVEL_AVATAR && !IS_IMMORTAL(victim) )
    {
	send_to_char( "You can only guide avatars.\n\r", ch );
	return;
    }

    if (IS_CLASS(victim, CLASS_VAMPIRE) || IS_SET(victim->pcdata->stats[UNI_AFF], VAM_MORTAL))
    {
	send_to_char( "You are unable to class vampires!\n\r", ch );
	return;
    }

    if (IS_CLASS(victim, CLASS_WEREWOLF))
    {
	send_to_char( "You are unable to class werewolves!\n\r", ch );
	return;
    }

    if (IS_CLASS(victim, CLASS_DEMON) || IS_SET(victim->special, SPC_CHAMPION))
    {
	send_to_char( "You are unable to class demons!\n\r", ch );
	return;
    }

    if (IS_CLASS(victim, CLASS_HIGHLANDER))
    {
	send_to_char( "You are unable to class highlanders.\n\r", ch );
	return;
    }  

    if (IS_CLASS(victim, CLASS_ZERG))
    {
	send_to_char( "You are unable to class Zergs.\n\r", ch );
	return;
    }  

	if (IS_CLASS(victim, CLASS_NINJA))
	{
	send_to_char( "You are unable to class ninjas.\n\r", ch);
	return;
	}

    if (!IS_IMMUNE(victim,IMM_VAMPIRE))
    {
	send_to_char( "You cannot class an unwilling person.\n\r", ch );
	return;
    }

    if (ch->exp < 1000)
    {
	send_to_char("You cannot afford the 1000 exp required to guide them.\n\r",ch);
	return;
    }

    if (victim->exp < 1000)
    {
	send_to_char("They cannot afford the 1000 exp required to be guided from you.\n\r",ch);
	return;
    }

    ch->exp -= 1000;
    victim->exp -= 1000;

    act("You make $N a Zerg.", ch, NULL, victim, TO_CHAR);
    act("$n makes $N a Zerg.", ch, NULL, victim, TO_NOTVICT);
    act("$n makes you a Zerg.", ch, NULL, victim, TO_VICT);
    send_to_char( "You are now a Zerg.\n\r", victim );
    free_string(victim->lord);
    victim->lord = str_dup(ch->name);
    victim->class    = CLASS_ZERG;
    victim->generation = ch->generation+1;
    save_char_obj(ch);
    save_char_obj(victim);
    return;
}

void do_zergpower( CHAR_DATA *ch, char *argument )
{
    char arg1 [MAX_INPUT_LENGTH];
    char buf[MAX_STRING_LENGTH];
    int improve;
    improve = PZERG; 
    
    argument = one_argument( argument, arg1 );
    
    if (IS_NPC(ch)) return;

    if (!IS_CLASS(ch, CLASS_ZERG))
    {
	send_to_char("Huh?\n\r",ch);
	return;
    }

    if (arg1[0] == '\0')
    {
	    send_to_char("     Zerg Powers.\n\r",ch);
	    send_to_char("- Remember to put single quotes ( ' ) around the power.\n\r", ch);
	    if ( ch->pcdata->powers[PZERG] < 1 ) send_to_char("Eyes of Zerg\n\r", ch);
	    if ( ch->pcdata->powers[PZERG] < 2 ) send_to_char("Speed
Power\n\r", ch);
	    if ( ch->pcdata->powers[PZERG] < 3 ) send_to_char("Super Invis\n\r", ch);
	    if ( ch->pcdata->powers[PZERG] < 4 ) send_to_char("Toughness\n\r", ch);
	    if ( ch->pcdata->powers[PZERG] < 5 ) send_to_char("Super 
Shield\n\r", ch);
	    if ( ch->pcdata->powers[PZERG] < 6 ) send_to_char("Travel\n\r", ch);

		return;
    }


	if (!str_cmp(arg1,"eyes of zerg")) 
    {
    	if ( ch->pcdata->powers[PZERG] > 0 )
    		send_to_char("You already have the Eyes of Zerg Power.\n\r", ch);
    	else if ( ch->practice < 10)
    		send_to_char("You don't have the 10 required primal.\n\r", ch);
    	else
    	{
    		ch->pcdata->powers[improve] += 1;
    		ch->practice -= 10;
    		send_to_char("You master the Eyes of Zerg Power.\n\r", ch);
    	}
    	return;
    }
    else if (!str_cmp(arg1,"speed power"))
    {
    	if ( ch->pcdata->powers[PZERG] > 1 )
    		send_to_char("You already have the Speed Power.\n\r", ch);
    	else if ( ch->pcdata->powers[PZERG] < 1 )
    		send_to_char("You need all the powers before this one to receive this power.\n\r", ch);
    	else if ( ch->practice < 20)
    		send_to_char("You don't have the 20 required primal.\n\r", ch);
    	else
    	{
    		ch->pcdata->powers[improve] += 1;
    		ch->practice -= 20;
    		send_to_char("You master the Speed Power.\n\r", ch);
    	}
    	return;
    }
    else if (!str_cmp(arg1,"super invis"))
    {
    	if ( ch->pcdata->powers[PZERG] > 2 )
    		send_to_char("You already have the Super Invis Power.\n\r", ch);
    	else if ( ch->pcdata->powers[PZERG] < 2 )
    		send_to_char("You need all the powers before this one to receive this power.\n\r", ch);
    	else if ( ch->practice < 30)
    		send_to_char("You don't have the 30 required primal.\n\r", ch);
    	else
    	{
    		ch->pcdata->powers[improve] += 1;
    		ch->practice -= 30;
    		send_to_char("You master the Super Invis Power.\n\r", ch);
    	}
    	return;
    }
    else if (!str_cmp(arg1,"toughness"))
    {
    	if ( ch->pcdata->powers[PZERG] > 3 )
    		send_to_char("You already have the Toughness Power.\n\r", ch);
    	else if ( ch->pcdata->powers[PZERG] < 3 )
    		send_to_char("You need all the power before this one to receive this power.\n\r", ch);
    	else if ( ch->practice < 40)
    		send_to_char("You don't have the 40 required primal.\n\r", ch);
    	else
    	{
    		ch->pcdata->powers[improve] += 1;
    		ch->practice -= 40;
    		send_to_char("You master the Toughness Power.\n\r", ch);
    	}
    	return;
    }
    else if (!str_cmp(arg1,"super shield"))
    {
    	if ( ch->pcdata->powers[PZERG] > 4 )
    		send_to_char("You already have the Super Shield power.\n\r", ch);
    	else if ( ch->pcdata->powers[PZERG] < 4 )
    		send_to_char("You need all the power before this one to receive this power.\n\r", ch);
    	else if ( ch->practice < 50)
    		send_to_char("You don't have the 50 required primal.\n\r", ch);
    	else
    	{
    		ch->pcdata->powers[improve] += 1;
    		ch->practice -= 50;
    		send_to_char("You master the Super Shield Power.\n\r", ch);
    	}
    	return;
    }
    else if (!str_cmp(arg1,"travel"))
    {
    	if ( ch->pcdata->powers[PZERG] > 5 )
    		send_to_char("You already have the travel power.\n\r", ch);
    	else if ( ch->pcdata->powers[PZERG] < 5 )
    		send_to_char("You need all the power before this one to receive this power.\n\r", ch);
    	else if ( ch->practice < 60)
    		send_to_char("You don't have the 60 required primal.\n\r", ch);
    	else
    	{
    		ch->pcdata->powers[improve] += 1;
    		ch->practice -= 60;
    		send_to_char("You master the travel power.\n\r", ch);
    	}
    	return;
    }
	sprintf(buf,"Powers Learned (%d).\n\r", ch->pcdata->powers[PZERG]);
	send_to_char(buf,ch);

	return;
}

void do_eyesofzerg( CHAR_DATA *ch, char *argument )
{
    if ( IS_NPC(ch) )
	return;
  
  	if ( !IS_CLASS(ch, CLASS_ZERG) )
  	{
  	send_to_char("Huh?\n\r", ch);
  	return;
  	}
  	
  	if ( ch->pcdata->powers[PZERG] < 1 )
  	{
  	  send_to_char("You have not learned the Eyes of Zerg Power.\n\r", ch);
  	  return;
  	}
  
    if ( IS_SET(ch->act, PLR_HOLYLIGHT) )
    {
	REMOVE_BIT(ch->act, PLR_HOLYLIGHT);
	send_to_char( "Your lose your Zergly sight.\n\r", ch );
    }
    else
    {
	send_to_char( "Xkilla gives you his eye vision.\n\r", ch );
	SET_BIT(ch->act, PLR_HOLYLIGHT);
    }
    return;
}

void do_supershield( CHAR_DATA *ch, char *argument )
{
    if ( IS_NPC(ch) )
	return;
  
  	if (!IS_CLASS(ch, CLASS_ZERG) )
  	{
  	send_to_char("Huh?\n\r", ch);
  	return;
  	}
  	
  	if ( ch->pcdata->powers[PZERG] < 5 )
  	{
  	  send_to_char("You have not learned the Super Shield Power.\n\r", ch);
  	  return;
  	}

/* Remove super shield in handler.c */
  
    if ( IS_AFFECTED(ch, AFF_STEELSHIELD) )
    {
	REMOVE_BIT( ch->affected_by, AFF_STEELSHIELD );
	send_to_char( "Your super shield disappears.\n\r", ch );
    }
    else
    {
	send_to_char( "You are surrounded by a super shield\n\r", ch );
	SET_BIT(ch->affected_by, AFF_STEELSHIELD);
    }
    return;
}

void do_ztravel( CHAR_DATA *ch, char *argument )
{
	char arg[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;
    CHAR_DATA *mount;

	one_argument( argument, arg );

	if ( !IS_CLASS(ch, CLASS_ZERG) )
	{
		send_to_char("Huh?\n\r", ch);
		return;
	}

	if (IS_CLASS(ch, CLASS_ZERG) && ch->pcdata->powers[PZERG] < 6 )
	{
	  send_to_char("You have not learned the travel power.\n\r", ch);
	  return;
	}

    if ( arg[0] == '\0' )
    {
	send_to_char( "Travel to where?\n\r", ch );
	return;
    }
    
    if ( ch->position == POS_FIGHTING )
    {
	send_to_char( "No way! You are fighting.\n\r", ch );
	return;
    }

    if ( ( victim = get_char_world( ch, arg ) ) == NULL )
    {
	send_to_char( "Nobody by that name.\n\r", ch );
	return;
    }
    if( IS_IMMUNE(victim, IMM_TRAVEL) && !IS_NPC(victim))
    {
        send_to_char( "They don't wish to be traveled to.\n\r", ch);
        return;
    }
	if ( !IS_IMMUNE(victim, IMM_SUMMON) && !IS_NPC(victim))
	{
		send_to_char("They do not wish to be astraled to!\n\r", ch);
		return;
	}
	
    act( "You travel.", ch, NULL, NULL, TO_CHAR );
	act( "$n travels.", ch, NULL, NULL, TO_ROOM );
	char_from_room( ch );
	char_to_room( ch, get_room_index(victim->in_room->vnum) );
    act( "You appear infront of $N", ch, NULL, victim, TO_CHAR );
	act( "$n appears infront of $N.", ch, NULL, victim, TO_ROOM );	

    do_look( ch, "auto" );
    if ( (mount = victim->mount) == NULL ) return;
    char_from_room( mount );
    char_to_room( mount, get_room_index(victim->in_room->vnum) );
    do_look( mount, "auto" );
    return;
}

void do_superinvis( CHAR_DATA *ch, char *argument )
{
    if (IS_NPC(ch)) return;
	
	if (!IS_CLASS(ch, CLASS_ZERG) )
	{
		send_to_char("Huh?\n\r", ch);
		return;
	}
	
	if (IS_CLASS(ch, CLASS_ZERG) && ch->pcdata->powers[PMONK] < 3 )
	{
	  send_to_char("You have not learned the Super Invis.\n\r", ch);
	  return;
	}

    if ( IS_SET(ch->act, PLR_WIZINVIS) )
    {
	REMOVE_BIT(ch->act, PLR_WIZINVIS);
	send_to_char( "You appear from a shroud of light.\n\r", ch );
	act( "$n appears from a shroud of light.", ch, NULL, NULL, TO_ROOM );
    }
    else
    {
	send_to_char( "You shield yourself in a shroud of light.\n\r", ch );
	act( "$n is shielded in a shroud of light.", ch, NULL, NULL, TO_ROOM );
	SET_BIT(ch->act, PLR_WIZINVIS);
    }
    return;
}

void do_zclan( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_STRING_LENGTH];
    CHAR_DATA *gch;
    char lord[MAX_INPUT_LENGTH];

    if (IS_NPC(ch)) return;

    if (!IS_CLASS(ch, CLASS_ZERG) )
    {
	send_to_char("Huh?\n\r",ch);
	return;
    }

    if( ch->generation < 3 )
      strcpy(lord,ch->name);
    else strcpy(lord,ch->lord);

    sprintf( buf, "%s's Zergs:\n\r", ch->lord );
    send_to_char( buf, ch );
    send_to_char("[      Name      ] [ Hits  % ] [ Mana  % ] [ Move  % ]\n\r", ch );
    for ( gch = char_list; gch != NULL; gch = gch->next )
    {
	if ( IS_NPC(gch) ) continue;
	if ( !IS_CLASS(gch, CLASS_ZERG) ) continue;
	if ( !str_cmp(gch->lord,lord)|| !str_cmp(gch->name,lord) )
	{
	    sprintf( buf,
	    "[%-16s] [%-6d%3d] [%-6d%3d] [%-6d%3d]\n\r",
		capitalize( gch->name ),
		gch->hit,  (gch->hit  * 100 / gch->max_hit ),
		gch->mana, (gch->mana * 100 / gch->max_mana),
		gch->move, (gch->move * 100 / gch->max_move) );
		send_to_char( buf, ch );
	}
    }
    return;
}

