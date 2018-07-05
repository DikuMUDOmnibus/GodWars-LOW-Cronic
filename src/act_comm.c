
/***************************************************************************
 *  Original Diku Mud copyright (C) 1990, 1991 by Sebastian Hammer,        *
 *  Michael Seifert, Hans Henrik St{rfeldt, Tom Madsen, and Katja Nyboe.   *
 *                                                                         *
 *  Merc Diku Mud improvments copyright (C) 1992, 1993 by Michael          *
 *  Chastain, Michael Quan, and Mitchell Tse.                              *
 *                                                                         *
 *  In order to use any part of this Merc Diku Mud, you must comply with   *
 *  both the original Diku license in 'license.doc' as well the Merc       *
 *  license in 'license.txt'.  In particular, you may not remove either of *
 *  these copyright notices.                                               *
 *                                                                         *
 *  Much time and thought has gone into this software and you are          *
 *  benefitting.  We hope that you share your changes too.  What goes      *
 *  around, comes around.                                                  *
 ***************************************************************************/

#if defined(macintosh)
#include <types.h>
#else
#include <sys/types.h>
#endif
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "merc.h"



/*
 * Local functions.
 */
void	talk_channel	args( ( CHAR_DATA *ch, char *argument,
			    int channel, const char *verb ) );
bool is_in		args( (char *, char *) );
bool all_in		args( (char *, char *) );
char * oldelang		args( ( CHAR_DATA *ch, char *argument ) );
char * badlang		args( ( CHAR_DATA *ch, char *argument ) );
char * darktongue	args( ( CHAR_DATA *ch, char *argument ) );
char * drunktalk	args( ( CHAR_DATA *ch, char *argument ) );
char * socialc		args( ( CHAR_DATA *ch, char *argument, char *you, char *them ) );
char * socialv		args( ( CHAR_DATA *ch, char *argument, char *you, char *them ) );
char * socialn		args( ( CHAR_DATA *ch, char *argument, char *you, char *them ) );

void game_in_play(CHAR_DATA *ch, char *argument);

/* Trace's Bounty code */
void do_bounty( CHAR_DATA *ch, char *argument )
{
    char arg1 [MAX_INPUT_LENGTH];
    char arg2 [MAX_INPUT_LENGTH];
    char buf[MAX_STRING_LENGTH];
    CHAR_DATA *victim;

    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );
        
    
        if ( arg1[0] == '\0' || arg2[0] == '\0' )
        {
             send_to_char( "Place a bounty on who's head?\n\rSyntax:  Bounty <victim> <amount>\n\r", ch );
             return;
        }
        
        if ( ( victim = get_char_world( ch, arg1 ) ) == NULL)
        {
           send_to_char( "They are currently not logged in!", ch );
           return;
        }
  
      if (IS_NPC(victim))
      {
        send_to_char( "You cannot put a bounty on NPCs!", ch );
        return;
      }
      
      if (victim->pcdata->bounty > 0)
      {
        send_to_char( "They already have a bounty on their head.",ch);
        return;
      }
     
      if (victim->level >= 7)
      {
        send_to_char( "You can't put a bounty on an immortal.",ch);
        return;
      }
 
	if ( is_number( arg2 ) )
        {
        int amount;
        amount   = atoi(arg2);
        if (ch->pcdata->quest < amount)
        {
                send_to_char( "You don't have that many QPs!", ch );
                return;
        }
        ch->pcdata->quest -= amount;
        victim->pcdata->bounty +=amount;
        sprintf( buf, "You have placed a %d QP bounty on %s.\n\r",
        amount,victim->name );
        send_to_char(buf,ch);
        sprintf(buf, "%s now has a bounty of %d QPs on their head.\n\r",
	victim->name, amount);
	do_info(ch, buf);        
        return;
        }
}



/*
 * Generic channel function.
 */
void talk_channel( CHAR_DATA *ch, char *argument, int channel, const char *verb )
{
    char buf[MAX_STRING_LENGTH];
    char buf2[MAX_STRING_LENGTH];
    DESCRIPTOR_DATA *d;
	int pos;
	bool ignored;
    int position;
	if (IS_NPC(ch)) return;

    if (RTIMER(ch->in_room, RTIMER_SILENCE) != 0)
    {
     send_to_char("Something prevents you from speaking in this room.\n\r", ch);
     return;
    }
    if ( argument[0] == '\0' )
    {
	sprintf( buf, "%s what?\n\r", verb );
	buf[0] = UPPER(buf[0]);
	return;
    }

    if ( IS_HEAD(ch, LOST_TONGUE) )
    {
	sprintf( buf, "You can't %s without a tongue!\n\r", verb );
	send_to_char( buf, ch );
	return;
    }
    /*
    if (IS_POLYAFF(ch, POLY_ZULOFORM))
    {
         sprintf(buf, "Beasts can't speak!\n\r");
         send_to_char( buf, ch );
         return;
    
    }
*/
    
    if ( IS_EXTRA(ch, GAGGED) )
    {
	sprintf( buf, "You can't %s with a gag on!\n\r", verb );
	send_to_char( buf, ch );
	return;
    }

    if ( !IS_NPC(ch) && IS_SET(ch->act, PLR_SILENCE) )
    {
	sprintf( buf, "You can't %s.\n\r", verb );
	send_to_char( buf, ch );
	return;
    }

    REMOVE_BIT(ch->deaf, channel);

    switch ( channel )
    {
    default:
	sprintf( buf, "You %s '#1%s#n'.\n\r", verb, argument );
	send_to_char( buf, ch );

	sprintf( buf, "$n %ss '#1$t#n'.",     verb );
	sprintf( buf2, "$n %ss '#1$t#n'.",     verb );
	break;

    case CHANNEL_IMMTALK:
	sprintf( buf, "#5%s: $t.#n",ch->pcdata->switchname );
	sprintf( buf2, "#5%s: $t.#n",ch->pcdata->switchname );
	position	= ch->position;
	ch->position	= POS_STANDING;
	act( buf, ch, argument, NULL, TO_CHAR );
	ch->position	= position;
	break;

    case CHANNEL_PRAY:
	sprintf( buf, "#1You pray '$t'.#n" );
	sprintf( buf2, "#1(#4%s#1) prays '$t'.#n", ch->pcdata->switchname );
	position	= ch->position;
	ch->position	= POS_STANDING;
	act( buf, ch, argument, NULL, TO_CHAR );
	ch->position	= position;
	break;

    case CHANNEL_MAGETALK:
	sprintf( buf, "#4{$n} '$t'.#n" );
	sprintf( buf2, "#4{$n} '$t'.#n" );
	position	= ch->position;
	ch->position	= POS_STANDING;
	act( buf, ch, argument, NULL, TO_CHAR );
	ch->position	= position;
	break;

    case CHANNEL_HOWL:
	sprintf( buf, "#0You howl '$t'.#n" );
	sprintf( buf2, "#0$n howls '$t'.#n" );
	position	= ch->position;
	ch->position	= POS_STANDING;
	act( buf, ch, argument, NULL, TO_CHAR );
	ch->position	= position;
	break;

	case CHANNEL_MIKTALK:
		sprintf(buf, "#3**$n** '$t'.#n" );
		sprintf(buf2, "#3**$n** '$t'.#n" );
		position = ch->position;
		act(buf, ch, argument, NULL, TO_CHAR);
		ch->position = position;
	break;

	case CHANNEL_HIGHTALK:
		sprintf( buf, "#3-=$n=-  '$t'.#n" );
		sprintf(buf2, "#3-=$n=-  '$t'.#n" );
		position = ch->position;
		act( buf, ch, argument, NULL, TO_CHAR );
		ch->position = position;
		break;

    case CHANNEL_SIGN:
        sprintf( buf, "#2.o0%s0o. '$t'.#n", ch->pcdata->switchname );
        sprintf( buf2, "#2.o0%s0o. '$t'.#n", ch->pcdata->switchname );
        position        = ch->position;
        ch->position    = POS_STANDING;
        act( buf, ch, argument, NULL, TO_CHAR );
        ch->position    = position;
        break;

    case CHANNEL_KTALK:
	sprintf( buf, "#4.x[%s]x. '$t'.#n", ch->pcdata->switchname);
	sprintf(buf2, "#4.x[%s]x. '$t'.#n", ch->pcdata->switchname);
	position = ch->position;
	ch->position = POS_STANDING;
	act(buf, ch, argument, NULL, TO_CHAR);
	ch->position = position;
	break;

    case CHANNEL_MONK:
        sprintf( buf, "$n hums '$t'." );
        sprintf( buf2, "$n hums '$t'." );
        position        = ch->position;
        ch->position    = POS_STANDING;
        act( buf, ch, argument, NULL, TO_CHAR );
        ch->position    = position;
        break; 

    case CHANNEL_VAMPTALK:
	if (!IS_NPC(ch) && (ch->pcdata->stats[UNI_GEN] == 1 || IS_SET(ch->special, SPC_ANARCH)))
	{
	    sprintf( buf, "#7<[$n]> $t.#n" );
	    sprintf( buf2, "#7<[$n]> $t.#n" );
	}
	else if (!IS_NPC(ch) && ch->pcdata->stats[UNI_GEN] == 2)
	{
	    sprintf( buf, "#7<<$n>> $t.#n" );
	    sprintf( buf2, "#7[[$n]] $t.#n" );
	}
	else
	{
	    sprintf( buf, "#7<$n> $t.#n" );
	    sprintf( buf2, "#7[$n] $t.#n" );
	}

	position	= ch->position;
	ch->position	= POS_STANDING;
	act( buf, ch, argument, NULL, TO_CHAR );
	ch->position	= position;
	break;
    }

    for ( d = descriptor_list; d != NULL; d = d->next )
    {
	CHAR_DATA *och;
	CHAR_DATA *vch;

	och = d->original ? d->original : d->character;
	vch = d->character;

	ignored = FALSE;

	if ( d->connected == CON_PLAYING
	&&   vch != ch
	&&  !IS_SET(och->deaf, channel) )
	{
	for (pos = 0; pos < MAX_FORGET; pos++) {
	if (och->pcdata->forget[pos] == NULL) break;
	if (!str_cmp(ch->name, och->pcdata->forget[pos]))
	ignored = TRUE;
	}
	if (ignored) continue;
	
	    if ( channel == CHANNEL_IMMTALK && !IS_IMMORTAL(och) )
		continue;
	if (channel== CHANNEL_MIKTALK && (!IS_NPC(och) && !IS_CLASS(och,
CLASS_NINJA) && !IS_IMMORTAL(och)))
	continue;
	    if ( channel == CHANNEL_HIGHTALK && (!IS_NPC(och) &&		
		!IS_CLASS(och, CLASS_HIGHLANDER) && !IS_IMMORTAL(och)))
		continue;
	if (channel == CHANNEL_KTALK && (!IS_NPC(och) && 
	!IS_IMMORTAL(och) && ch->pcdata->kingdom != och->pcdata->kingdom))
	continue;
	    if ( channel == CHANNEL_SIGN && (!IS_NPC(och) &&
                !IS_CLASS(och, CLASS_DROW) && !IS_IMMORTAL(och)))
                continue;
            if ( channel == CHANNEL_MONK && (!IS_NPC(och) &&
                !IS_CLASS(och, CLASS_MONK) && !IS_IMMORTAL(och)))
                continue; 
	    if ( channel == CHANNEL_VAMPTALK && (!IS_NPC(och) && 
		!IS_CLASS(och, CLASS_VAMPIRE) && !IS_IMMORTAL(och)))
		continue;
            if ( channel == CHANNEL_MAGETALK && (!IS_NPC(och) &&
	        !IS_CLASS(och, CLASS_MAGE) && !IS_IMMORTAL(och)))
		continue;
            if ( channel == CHANNEL_PRAY && (!IS_NPC(och) &&
	        !IS_CLASS(och, CLASS_DEMON) && !IS_IMMORTAL(och)))
		continue;
            if ( channel == CHANNEL_HOWL && (!IS_NPC(och) &&
	        !IS_CLASS(och, CLASS_WEREWOLF) && !IS_POLYAFF(och,POLY_WOLF)
		&& !IS_IMMORTAL(och)))
	    {
	        if ((och->in_room) && (ch->in_room))
	    	{
			if ((och->in_room == ch->in_room))
			{
		    		act("$n throws back $s head and howls loudly.", ch , argument , och, TO_VICT);
		    		continue;
			} 
			else if ((och->in_room->area == ch->in_room->area ))
			{
		    		act("You hear a loud howl nearby.", ch , NULL , och, TO_VICT);
		    		continue;
			}
			else 
			{
		    		act("You hear a loud howl in the distance.", ch , NULL , och, TO_VICT);
	            		continue;
			}
	    	}
	    }
            
	    if ( channel == CHANNEL_YELL
	    &&   vch->in_room->area != ch->in_room->area )
		continue;

	    position		= vch->position;
	    if ( channel != CHANNEL_SHOUT && channel != CHANNEL_YELL )
		vch->position	= POS_STANDING;

	    if (!IS_NPC(vch) && IS_CLASS(vch, CLASS_VAMPIRE))
	    {
	    	act( buf2, ch, argument, vch, TO_VICT );
	    	vch->position	= position;
		continue;
	    }
	    if (!IS_NPC(vch) && IS_CLASS(vch, CLASS_WEREWOLF)) 
	    {
	    	act( buf2, ch, argument, vch, TO_VICT );
	    	vch->position	= position;
		continue;
	    }
            if (!IS_NPC(vch) && IS_CLASS(vch, CLASS_MONK))
            {
                act( buf2, ch, argument, vch, TO_VICT );
                vch->position   = position;
                continue;
            }
	    if (!IS_NPC(vch) && (IS_CLASS(vch, CLASS_DEMON)||IS_IMMORTAL(vch)))
	    {
	    	act( buf2, ch, argument, vch, TO_VICT );
	    	vch->position	= position;
		continue;
	    }
	    if (!IS_NPC(vch) && (IS_CLASS(vch,CLASS_MAGE)||IS_IMMORTAL(vch)))
	    {
	    	act( buf2, ch, argument, vch, TO_VICT );
	    	vch->position	= position;
		continue;
	    }
	  if (!IS_NPC(vch) && (IS_CLASS(vch, CLASS_HIGHLANDER)||IS_IMMORTAL(vch))) {
		act(buf2, ch, argument, vch, TO_VICT);
		vch->position = position;
		continue; 
	    }
	    act( buf, ch, argument, vch, TO_VICT );
	    vch->position	= position;
	}
    }

    return;
}


/*
void do_auction( CHAR_DATA *ch, char *argument )
{
    talk_channel( ch, argument, CHANNEL_AUCTION, "auction" );
    return;
}
*/


void room_message(ROOM_INDEX_DATA *room, char *message)
{
    CHAR_DATA *rch;

    if ( (rch = room->people) == NULL) return;

    act(message, rch, NULL, NULL, TO_ROOM);
    act(message, rch, NULL, NULL, TO_CHAR);
}

void do_chat( CHAR_DATA *ch, char *argument )
{
 
   talk_channel( ch, argument, CHANNEL_CHAT, "chat" );
    return;
 
}



/*
 * Alander's new channels.
 */
void do_music( CHAR_DATA *ch, char *argument )
{
    talk_channel( ch, argument, CHANNEL_MUSIC, "sing" );

    return;
}



void do_question( CHAR_DATA *ch, char *argument )
{
    talk_channel( ch, argument, CHANNEL_QUESTION, "question" );

    return;
}



void do_answer( CHAR_DATA *ch, char *argument )
{
    talk_channel( ch, argument, CHANNEL_QUESTION, "answer" );

    return;
}



void do_shout( CHAR_DATA *ch, char *argument )
{

    talk_channel( ch, argument, CHANNEL_SHOUT, "shout" );

    WAIT_STATE( ch, 12 );
    return;
}



void do_yell( CHAR_DATA *ch, char *argument )
{

    talk_channel( ch, argument, CHANNEL_YELL, "yell" );

    return;
}



void do_immtalk( CHAR_DATA *ch, char *argument )
{
    talk_channel( ch, argument, CHANNEL_IMMTALK, "immtalk" );
    return;
}

void do_monktalk( CHAR_DATA *ch, char *argument ) {
	if (IS_NPC(ch) || (!IS_IMMORTAL(ch) && !IS_CLASS(ch, CLASS_MONK))) {
	send_to_char("Huh?\n\r", ch);
	return;}
	talk_channel(ch, argument, CHANNEL_MONK, "monk");
	return;
}

void do_miktalk( CHAR_DATA *ch, char *argument )
{

	if (IS_NPC(ch) || (!IS_IMMORTAL(ch) && !IS_CLASS(ch, CLASS_NINJA)))
	{send_to_char("Huh?\n\r", ch);
	return;}
	talk_channel(ch, argument, CHANNEL_MIKTALK, "miktalk");
	return;
}

void do_hightalk( CHAR_DATA *ch, char *argument )
{
	
       if (IS_NPC(ch) || (!IS_IMMORTAL(ch) && !IS_CLASS(ch,CLASS_HIGHLANDER)))
	{
		send_to_char("Huh?\n\r",ch);
		return;
	}
	talk_channel( ch, argument, CHANNEL_HIGHTALK, "hightalk" );
	return;
}
void do_sign( CHAR_DATA *ch, char *argument )
{

    if (IS_NPC(ch) || (!IS_IMMORTAL(ch) && !IS_CLASS(ch,CLASS_DROW)))
    {
        send_to_char("Huh?\n\r",ch);
        return;
    }
    talk_channel( ch, argument, CHANNEL_SIGN, "sign" );
    return;
}

void do_ktalk( CHAR_DATA *ch, char *argument ) 
{
	if (IS_NPC(ch)) return;
	if (ch->pcdata->kingdom == 0) {
	send_to_char("Huh?\n\r", ch );
	return;}
	talk_channel(ch, argument, CHANNEL_KTALK, "ktalk" );
}


void do_vamptalk( CHAR_DATA *ch, char *argument )
{

    if (IS_NPC(ch) || (!IS_IMMORTAL(ch) && !IS_CLASS(ch,CLASS_VAMPIRE)))
    {
	send_to_char("Huh?\n\r",ch);
	return;
    }
    talk_channel( ch, argument, CHANNEL_VAMPTALK, "vamptalk" );
    return;
}

void do_magetalk( CHAR_DATA *ch, char *argument )
{
    if (IS_NPC( ch ) || (!IS_IMMORTAL(ch) && !IS_CLASS(ch,CLASS_MAGE)))
    {
	send_to_char("Huh?\n\r",ch);
	return;
    }
    talk_channel( ch, argument, CHANNEL_MAGETALK, "magetalk" );
    return;
}

void do_monk( CHAR_DATA *ch, char *argument )
{
    if (IS_NPC( ch ) || (!IS_IMMORTAL(ch) && !IS_CLASS(ch,CLASS_MONK)))
    {
        send_to_char("Huh?\n\r",ch);
        return;
    }
    talk_channel( ch, argument, CHANNEL_MONK, "monk" );
    return;
}

void do_pray( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *victim;
    char buf [MAX_STRING_LENGTH];
    
    act("You mutter a few prayers.",ch,NULL,NULL,TO_CHAR);
    act("$n mutters a quick prayer.",ch,NULL,NULL,TO_ROOM);

    if (IS_NPC(ch) ||(!IS_CLASS(ch,CLASS_DEMON) && !IS_IMMORTAL(ch))) 
	return;

    if ( argument[0] == '\0' && IS_SET(ch->special, SPC_DEMON_LORD))
    {
	send_to_char("What do you wish to pray?\n\r",ch);
	return;
    }
    else if ( argument[0] == '\0' )
    {
	if (ch->pcdata->stats[DEMON_CURRENT] < 1)
	{
	    send_to_char("Nothing happens.\n\r",ch);
	    return;
	}
	if ( ( victim = get_char_world(ch, ch->lord) ) == NULL )
	{
	    send_to_char("Nothing happens.\n\r",ch);
	    return;
	}
	act("You hear $n's prayers in your mind.",ch,NULL,victim,TO_VICT);
	send_to_char("You feel energy pour into your body.\n\r",victim);
	if (ch->pcdata->stats[DEMON_CURRENT] == 1)
	    sprintf(buf,"You receive a single point of energy.\n\r");
	else
	    sprintf(buf,"You receive %d points of energy.\n\r",
		ch->pcdata->stats[DEMON_CURRENT]);
	send_to_char(buf,victim);
	act("$n is briefly surrounded by a halo of energy.",victim,NULL,NULL,TO_ROOM);
	victim->pcdata->stats[DEMON_CURRENT] += ch->pcdata->stats[DEMON_CURRENT];
	victim->pcdata->stats[DEMON_TOTAL] += ch->pcdata->stats[DEMON_CURRENT];
	ch->pcdata->stats[DEMON_CURRENT] = 0;
	return;
    }
    if (IS_SET(ch->deaf, CHANNEL_PRAY))
    {
	send_to_char("But you're not even on the channel!\n\r",ch);
	return;
    }

    talk_channel( ch, argument, CHANNEL_PRAY, "pray" );
    return;
}

void do_howl( CHAR_DATA *ch, char *argument )
{
    if (IS_NPC( ch ) || (!IS_IMMORTAL(ch) && !IS_CLASS(ch,CLASS_WEREWOLF) 
	&& !IS_POLYAFF(ch, POLY_WOLF)))
    {
	send_to_char("Huh?\n\r",ch);
	return;
    }
    talk_channel( ch, argument, CHANNEL_HOWL, "howls" );
    return;
}



void do_say( CHAR_DATA *ch, char *argument )
{
    char name   [80];
    char poly   [MAX_STRING_LENGTH];
    char speak  [10];
    char speaks [10];
    char endbit [2];
    char secbit [2];
    CHAR_DATA *to;
    bool is_ok;

    if (RTIMER(ch->in_room, RTIMER_SILENCE) != 0)
    {
     send_to_char("Something prevents you from speaking in this room.\n\r", ch);
     return;
    }
    if ( IS_HEAD(ch, LOST_TONGUE) )
    {
	send_to_char( "You can't speak without a tongue!\n\r", ch );
	return;
    }
    if ( IS_EXTRA(ch, GAGGED) )
    {
	send_to_char( "You can't speak with a gag on!\n\r", ch );
	return;
    }

    if (strlen(argument) > MAX_INPUT_LENGTH)
    {
	send_to_char( "Line too long.\n\r", ch );
	return;
    }

    if ( argument[0] == '\0' )
    {
	send_to_char( "Say what?\n\r", ch );
	return;
    }

    endbit[0] = argument[strlen(argument)-1];
    endbit[1] = '\0';

    if (strlen(argument) > 1) secbit[0] = argument[strlen(argument)-2];
	else secbit[0] = '\0';
    secbit[1] = '\0';

    if (IS_BODY(ch,CUT_THROAT))
    {
	sprintf(speak,"rasp");
	sprintf(speaks,"rasps");
    }
    else if (!IS_NPC(ch) && 
	(IS_SET(ch->special,SPC_WOLFMAN) || IS_POLYAFF(ch, POLY_WOLF) 
	|| (IS_CLASS(ch, CLASS_VAMPIRE) && ch->pcdata->stats[UNI_RAGE] > 0)))
    {
	if (number_percent() > 50)
	{
	    sprintf(speak,"growl");
	    sprintf(speaks,"growls");
	}
	else
	{
	    sprintf(speak,"snarl");
	    sprintf(speaks,"snarls");
	}
    }
    else if (!IS_NPC(ch) && IS_POLYAFF(ch, POLY_BAT))
    {
	sprintf(speak,"squeak");
	sprintf(speaks,"squeaks");
    }
    else if (!IS_NPC(ch) && IS_POLYAFF(ch, POLY_SERPENT))
    {
	sprintf(speak,"hiss");
	sprintf(speaks,"hisses");
    }
    else if (!IS_NPC(ch) && IS_POLYAFF(ch, POLY_FROG))
    {
	sprintf(speak,"croak");
	sprintf(speaks,"croaks");
    }
    else if (!IS_NPC(ch) && IS_POLYAFF(ch, POLY_RAVEN))
    {
	sprintf(speak,"squark");
	sprintf(speaks,"squarks");
    }
    else if (IS_NPC(ch) && ch->pIndexData->vnum == MOB_VNUM_FROG)
    {
	sprintf(speak,"croak");
	sprintf(speaks,"croaks");
    }
    else if (IS_NPC(ch) && ch->pIndexData->vnum == MOB_VNUM_RAVEN)
    {
	sprintf(speak,"squark");
	sprintf(speaks,"squarks");
    }
    else if (IS_NPC(ch) && ch->pIndexData->vnum == MOB_VNUM_CAT)
    {
	sprintf(speak,"purr");
	sprintf(speaks,"purrs");
    }
    else if (IS_NPC(ch) && ch->pIndexData->vnum == MOB_VNUM_DOG)
    {
	sprintf(speak,"bark");
	sprintf(speaks,"barks");
    }
    else if (!str_cmp(endbit,"!"))
    {
	sprintf(speak,"exclaim");
	sprintf(speaks,"exclaims");
    }
    else if (!str_cmp(endbit,"?"))
    {
	sprintf(speak,"ask");
	sprintf(speaks,"asks");
    }
    else if (secbit[0] != '\0' && str_cmp(secbit,".") && !str_cmp(endbit,"."))
    {
	sprintf(speak,"state");
	sprintf(speaks,"states");
    }
    else if (secbit[0] != '\0' && !str_cmp(secbit,".") && !str_cmp(endbit,"."))
    {
	sprintf(speak,"mutter");
	sprintf(speaks,"mutters");
    }
    else if (!IS_NPC(ch) && ch->pcdata->condition[COND_DRUNK] > 10)
    {
	sprintf(speak,"slur");
	sprintf(speaks,"slurs");
    }
    else
    {
	sprintf(speak,"say");
	sprintf(speaks,"says");
    }
    sprintf(poly,"You %s '#3$T#n'.", speak);
    if (!IS_NPC(ch))
    {
	if (ch->pcdata->condition[COND_DRUNK] > 10)
	    act( poly, ch, NULL, drunktalk(ch, argument), TO_CHAR );
	else if (IS_SPEAKING(ch,DIA_OLDE))
	    act( poly, ch, NULL, oldelang(ch, argument), TO_CHAR );
	else if (IS_SPEAKING(ch,DIA_BAD))
	    act( poly, ch, NULL, badlang(ch, argument), TO_CHAR );
	else if (IS_SPEAKING(ch,LANG_DARK))
	    act( poly, ch, NULL, darktongue(ch, argument), TO_CHAR );
	else
	    act( poly, ch, NULL, argument, TO_CHAR );
    }
    else
	act( poly, ch, NULL, argument, TO_CHAR );

    sprintf(poly,"$n %s '#3$T#n'.", speaks);
    if (ch->in_room->vnum != ROOM_VNUM_IN_OBJECT)
    {
	if (!IS_NPC(ch))
	{
/*
	    if (ch->pcdata->condition[COND_DRUNK] > 10)
		act( poly, ch, NULL, drunktalk(ch, argument), TO_ROOM );
	    else if (IS_SPEAKING(ch,DIA_OLDE))
		act( poly, ch, NULL, oldelang(ch, argument), TO_ROOM );
	    else if (IS_SPEAKING(ch,DIA_BAD))
		act( poly, ch, NULL, badlang(ch, argument), TO_ROOM );
	    else if (IS_SPEAKING(ch,LANG_DARK))
		act( poly, ch, NULL, darktongue(ch, argument), TO_ROOM );
	    else
*/
		act( poly, ch, NULL, argument, TO_ROOM );
	}
	else
	    act( poly, ch, NULL, argument, TO_ROOM );
    	room_text( ch, strlower(argument) );
	return;
    }

    to = ch->in_room->people;
    for ( ; to != NULL; to = to->next_in_room )
    {
	is_ok = FALSE;

	if ( to->desc == NULL || !IS_AWAKE(to) )
	    continue;

	if ( ch == to )
	    continue;

	if (!IS_NPC(ch) && ch->pcdata->chobj != NULL && 
	    ch->pcdata->chobj->in_room != NULL &&
	    !IS_NPC(to) && to->pcdata->chobj != NULL && 
	    to->pcdata->chobj->in_room != NULL &&
	    ch->in_room == to->in_room)
		is_ok = TRUE; else is_ok = FALSE;

	if (!IS_NPC(ch) && ch->pcdata->chobj != NULL && 
	    ch->pcdata->chobj->in_obj != NULL &&
	    !IS_NPC(to) && to->pcdata->chobj != NULL && 
	    to->pcdata->chobj->in_obj != NULL &&
	    ch->pcdata->chobj->in_obj == to->pcdata->chobj->in_obj)
		is_ok = TRUE; else is_ok = FALSE;

	if (!is_ok) continue;

	if (IS_NPC(ch))
	    sprintf(name, ch->short_descr);
	else if (!IS_NPC(ch) && IS_AFFECTED(ch,AFF_POLYMORPH))
	    sprintf(name, ch->morph);
        else if (!IS_NPC(ch) && IS_AFFECTED(ch,AFF_SHIFT))
	    sprintf(name, ch->morph);


        else
	    sprintf(name, ch->name);
	name[0]=UPPER(name[0]);
	sprintf(poly,"%s %s '%s'.\n\r", name,speaks,argument);
	send_to_char(poly,to);
    }

    room_text( ch, strlower(argument) );
    return;
}

void room_text( CHAR_DATA *ch, char *argument)
{
    CHAR_DATA     *vch;
    CHAR_DATA     *vch_next;
    CHAR_DATA     *mob;
    OBJ_DATA      *obj;
    ROOMTEXT_DATA *rt;
    bool           mobfound;
    bool           hop;

    char           arg  [MAX_INPUT_LENGTH];
    char           arg1 [MAX_INPUT_LENGTH];
    char           arg2 [MAX_INPUT_LENGTH];

    for ( rt = ch->in_room->roomtext; rt != NULL; rt = rt->next )
    {
	if (!strcmp(argument,rt->input)
	 || is_in(argument, rt->input)
	 || all_in(argument, rt->input)) 
	{
	    if ( rt->name != NULL         && rt->name != '\0'
	    &&   str_cmp(rt->name,"all")  && str_cmp(rt->name,"|all*") ) 
	    	if (!is_in(ch->name, rt->name) ) continue;
	    mobfound = TRUE;
	    if (rt->mob != 0)
	    {
		mobfound = FALSE;
	    	for ( vch = char_list; vch != NULL; vch = vch_next )
	    	{
		    vch_next	= vch->next;
		    if ( vch->in_room == NULL ) continue;
		    if ( !IS_NPC(vch)         ) continue;
		    if ( vch->in_room == ch->in_room
			&& vch->pIndexData->vnum == rt->mob )
		    {
			mobfound = TRUE;
		    	break;
		    }
	    	}
	    }
	    if (!mobfound) continue;
	    hop = FALSE;
	    switch(rt->type % RT_RETURN)
	    {
		case RT_SAY:
		    break;
		case RT_LIGHTS:
		    do_changelight(ch,"");
		    break;
		case RT_LIGHT:
		    REMOVE_BIT(ch->in_room->room_flags, ROOM_DARK);
		    break;
		case RT_DARK:
		    SET_BIT(ch->in_room->room_flags, ROOM_DARK);
		    break;
		case RT_OBJECT:
		    if ( get_obj_index(rt->power) == NULL ) return;
		    obj = create_object(get_obj_index(rt->power), ch->level);
		    if (IS_SET(rt->type, RT_TIMER) ) obj->timer = 1;
		    if (CAN_WEAR(obj,ITEM_TAKE)) obj_to_char(obj,ch);
		    else obj_to_room(obj,ch->in_room);
		    if (!str_cmp(rt->choutput,"copy"))
		    	act( rt->output, ch, obj, NULL, TO_CHAR );
		    else
		    	act( rt->choutput, ch, obj, NULL, TO_CHAR );
		    if (!IS_SET(rt->type, RT_PERSONAL) ) 
			act( rt->output, ch, obj, NULL, TO_ROOM );
		    hop = TRUE;
		    break;
		case RT_MOBILE:
		    if ( get_mob_index(rt->power) == NULL ) return;
		    mob = create_mobile(get_mob_index(rt->power));
		    char_to_room(mob,ch->in_room);
		    if (!str_cmp(rt->choutput,"copy"))
		    	act( rt->output, ch, NULL, mob, TO_CHAR );
		    else
		    	act( rt->choutput, ch, NULL, mob, TO_CHAR );
		    if (!IS_SET(rt->type, RT_PERSONAL) ) 
			act( rt->output, ch, NULL, mob, TO_ROOM );
		    hop = TRUE;
		    break;
		case RT_SPELL:
		    (*skill_table[rt->power].spell_fun) ( rt->power, number_range(20,30), ch, ch );
		    break;
		case RT_PORTAL:
		    if ( get_obj_index(OBJ_VNUM_PORTAL) == NULL ) return;
		    obj = create_object(get_obj_index(OBJ_VNUM_PORTAL), 0);
		    obj->timer = 1;
		    obj->value[0] = rt->power;
		    obj->value[1] = 1;
		    obj_to_room(obj,ch->in_room);
		    break;
		case RT_TELEPORT:
		    if ( get_room_index(rt->power) == NULL ) return;
		    if (!str_cmp(rt->choutput,"copy"))
		    	act( rt->output, ch, NULL, NULL, TO_CHAR );
		    else
		    	act( rt->choutput, ch, NULL, NULL, TO_CHAR );
		    if (!IS_SET(rt->type, RT_PERSONAL) ) 
			act( rt->output, ch, NULL, NULL, TO_ROOM );
		    char_from_room(ch);
		    char_to_room(ch,get_room_index(rt->power));
		    act("$n appears in the room.",ch,NULL,NULL,TO_ROOM);
		    do_look(ch,"auto");
		    hop = TRUE;
		    break;
		case RT_ACTION:
		    sprintf(arg,argument);
		    argument = one_argument( arg, arg1 );
		    argument = one_argument( arg, arg2 );
		    if ( (mob = get_char_room(ch, arg2) ) == NULL ) continue;
		    interpret( mob, rt->output );
		    break;
		case RT_OPEN_LIFT:
		    open_lift(ch);
		    break;
		case RT_CLOSE_LIFT:
		    close_lift(ch);
		    break;
		case RT_MOVE_LIFT:
		    move_lift(ch,rt->power);
		    break;
		default:
		    break;
	    }
	    if (hop && IS_SET(rt->type, RT_RETURN) ) return;
	    else if (hop) continue;
	    if (!str_cmp(rt->choutput,"copy") && !IS_SET(rt->type, RT_ACTION))
	    	act( rt->output, ch, NULL, NULL, TO_CHAR );
	    else if (!IS_SET(rt->type, RT_ACTION))
	    	act( rt->choutput, ch, NULL, NULL, TO_CHAR );
	    if (!IS_SET(rt->type, RT_PERSONAL) && !IS_SET(rt->type, RT_ACTION) )
		act( rt->output, ch, NULL, NULL, TO_ROOM );
	    if (IS_SET(rt->type, RT_RETURN) ) return;
	}
    }
    return;
}

char *strlower(char *ip)
{
	static char buffer[MAX_INPUT_LENGTH];
	int pos;

	for (pos = 0; pos < (MAX_INPUT_LENGTH - 1) && ip[pos] != '\0'; pos++) {
		buffer[pos] = tolower(ip[pos]);
	}	
	buffer[pos] = '\0';
	return buffer;
}

bool is_in(char *arg, char *ip)
{
	char *lo_arg;
	char cmp[MAX_INPUT_LENGTH];
	int fitted;

	if (ip[0] != '|')
		return FALSE;
	cmp[0] = '\0';
	lo_arg = strlower(arg);
	do {
		ip += strlen(cmp) + 1;
		fitted = sscanf(ip, "%[^*]", cmp);
		if (strstr(lo_arg, cmp) != NULL) {
			return TRUE;
		}
	} while (fitted > 0);
	return FALSE;
}

bool all_in(char *arg, char *ip)
{
	char *lo_arg;
	char cmp[MAX_INPUT_LENGTH];
	int fitted;

	if (ip[0] != '&')
		return FALSE;
	cmp[0] = '\0';
	lo_arg = strlower(arg);
	do {
		ip += strlen(cmp) + 1;
		fitted = sscanf(ip, "%[^*]", cmp);
		if (strstr(lo_arg, cmp) == NULL) {
			return FALSE;
		}
	} while (fitted > 0);
	return TRUE;
}	

void do_tell( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    char poly [MAX_STRING_LENGTH];
    CHAR_DATA *victim;
    int position;

    if ( !IS_NPC(ch) && IS_SET(ch->act, PLR_SILENCE) )
    {
	send_to_char( "Your message didn't get through.\n\r", ch );
	return;
    }
    if (IS_EXTRA(ch, GAGGED))
    {
	send_to_char( "Your message didn't get through.\n\r", ch );
	return;
    }

    argument = one_argument( argument, arg );

    if ( arg[0] == '\0' || argument[0] == '\0' )
    {
	send_to_char( "Tell whom what?\n\r", ch );
	return;
    }

    /*
     * Can tell to PC's anywhere, but NPC's only in same room.
     * -- Furey
     */
    if ( ( victim = get_char_world( ch, arg ) ) == NULL
    || ( IS_NPC(victim) && victim->in_room != ch->in_room ) )
    {
	send_to_char( "They aren't here.\n\r", ch );
	return;
    }

    if ( !IS_IMMORTAL(ch) && !IS_AWAKE(victim) )
    {
	act( "$E can't hear you.", ch, 0, victim, TO_CHAR );
	return;
    }

    if ( !IS_NPC(victim) && victim->desc == NULL )
    {
	act( "$E is currently link dead.", ch, 0, victim, TO_CHAR );
	return;
    }

    if ( IS_SET(victim->deaf, CHANNEL_TELL) && !IS_IMMORTAL(ch) )
    {
	if (IS_NPC(victim) || IS_NPC(ch) || strlen(victim->pcdata->marriage) < 2
	    || str_cmp(ch->name, victim->pcdata->marriage))
	{
	    act( "$E can't hear you.", ch, 0, victim, TO_CHAR );
	    return;
	}
    }
    sprintf(poly,"You tell $N '#6$t#n'.");
    act(poly, ch, argument, victim, TO_CHAR );

    position		= victim->position;
    victim->position	= POS_STANDING;

    sprintf(poly,"$n tells you '#6$t#n'.");
    act( poly, ch, argument, victim, TO_VICT );

    victim->position	= position;
    victim->reply	= ch;

    return;
}



void do_whisper( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;

    if (IS_EXTRA(ch, GAGGED))
    {
	send_to_char( "Not with a gag on!\n\r", ch );
	return;
    }

    argument = one_argument( argument, arg );

    if ( arg[0] == '\0' || argument[0] == '\0' )
    {
	send_to_char( "Syntax: whisper <person> <message>\n\r", ch );
	return;
    }

    if ( ( victim = get_char_world( ch, arg ) ) == NULL
    || ( victim->in_room != ch->in_room ) )
    {
	send_to_char( "They aren't here.\n\r", ch );
	return;
    }

    if ( !IS_AWAKE(victim) )
    {
	act( "$E cannot hear you.", ch, 0, victim, TO_CHAR );
	return;
    }

    if ( !IS_NPC(victim) && victim->desc == NULL )
    {
	act( "$E is currently link dead.", ch, 0, victim, TO_CHAR );
	return;
    }

    act( "You whisper to $N '$t'.", ch, argument, victim, TO_CHAR );
    act( "$n whispers to you '$t'.", ch, argument, victim, TO_VICT );
    act( "$n whispers something to $N.", ch, NULL, victim, TO_NOTVICT );

    return;
}



void do_reply( CHAR_DATA *ch, char *argument )
{
    char poly [MAX_STRING_LENGTH];
    CHAR_DATA *victim;
    int position;

    if ( !IS_NPC(ch) && IS_SET(ch->act, PLR_SILENCE) )
    {
	send_to_char( "Your message didn't get through.\n\r", ch );
	return;
    }
    if (IS_EXTRA(ch, GAGGED))
    {
	send_to_char( "Your message didn't get through.\n\r", ch );
	return;
    }

    if ( ( victim = ch->reply ) == NULL )
    {
	send_to_char( "They aren't here.\n\r", ch );
	return;
    }

    if ( !IS_IMMORTAL(ch) && !IS_AWAKE(victim) )
    {
	act( "$E can't hear you.", ch, 0, victim, TO_CHAR );
	return;
    }

    if ( !IS_NPC(victim) && victim->desc == NULL )
    {
	act( "$E is currently link dead.", ch, 0, victim, TO_CHAR );
	return;
    }
    sprintf(poly,"You reply to $N '#6$t#n'.");
    act( poly, ch, argument, victim, TO_CHAR );
    position		= victim->position;
    victim->position	= POS_STANDING;

    sprintf(poly,"$n replies to you '#6$t#n'.");
    act( poly, ch, argument, victim, TO_VICT );

    victim->position	= position;
    victim->reply	= ch;

    return;
}



void do_emote( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_STRING_LENGTH];
    char *plast;

    char name   [80];
    char poly   [MAX_INPUT_LENGTH];
    CHAR_DATA *to;
    bool is_ok;


    if ( IS_HEAD(ch, LOST_TONGUE) || IS_HEAD(ch, LOST_HEAD) || IS_EXTRA(ch, GAGGED))
    {
	send_to_char( "You can't show your emotions.\n\r", ch );
	return;
    }

    if ( argument[0] == '\0' )
    {
	send_to_char( "Pose what?\n\r", ch );
	return;
    }

    for ( plast = argument; *plast != '\0'; plast++ )
	;

    strcpy( buf, argument );
    if ( isalpha(plast[-1]) )
	strcat( buf, "." );

    act( "$n $T", ch, NULL, buf, TO_CHAR );

    if (ch->in_room->vnum != ROOM_VNUM_IN_OBJECT)
    {
    	act( "$n $T", ch, NULL, buf, TO_ROOM );
	return;
    }

    to = ch->in_room->people;
    for ( ; to != NULL; to = to->next_in_room )
    {
	is_ok = FALSE;

	if ( to->desc == NULL || !IS_AWAKE(to) )
	    continue;

	if ( ch == to )
	    continue;

	if (!IS_NPC(ch) && ch->pcdata->chobj != NULL && 
	    ch->pcdata->chobj->in_room != NULL &&
	    !IS_NPC(to) && to->pcdata->chobj != NULL && 
	    to->pcdata->chobj->in_room != NULL &&
	    ch->in_room == to->in_room)
		is_ok = TRUE; else is_ok = FALSE;

	if (!IS_NPC(ch) && ch->pcdata->chobj != NULL && 
	    ch->pcdata->chobj->in_obj != NULL &&
	    !IS_NPC(to) && to->pcdata->chobj != NULL && 
	    to->pcdata->chobj->in_obj != NULL &&
	    ch->pcdata->chobj->in_obj == to->pcdata->chobj->in_obj)
		is_ok = TRUE; else is_ok = FALSE;

	if (!is_ok) continue;

	if (IS_NPC(ch))
	    sprintf(name, ch->short_descr);
	else if (!IS_NPC(ch) && IS_AFFECTED(ch,AFF_POLYMORPH))
	    sprintf(name, ch->morph);
        else if (!IS_NPC(ch) && IS_AFFECTED(ch,AFF_SHIFT))
	    sprintf(name, ch->morph);

        else
	    sprintf(name, ch->name);
	name[0]=UPPER(name[0]);
	sprintf(poly,"%s %s\n\r", name,buf);
	send_to_char(poly,to);
    }
    return;
}



void do_xemote( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_STRING_LENGTH];
    char buf2[MAX_STRING_LENGTH];
    char oldarg[MAX_STRING_LENGTH];
    char *plast;

    char name   [80];
    char you    [80];
    char them   [80];
    char poly   [MAX_INPUT_LENGTH];
    char arg    [MAX_INPUT_LENGTH];
    CHAR_DATA *to;
    CHAR_DATA *victim;
    bool is_ok;

    argument = one_argument( argument, arg );


    if ( IS_HEAD(ch, LOST_TONGUE) || IS_HEAD(ch, LOST_HEAD) || IS_EXTRA(ch, GAGGED))
    {
	send_to_char( "You can't show your emotions.\n\r", ch );
	return;
    }

    if (strlen(argument) > MAX_INPUT_LENGTH)
    {
	send_to_char( "Line too long.\n\r", ch );
	return;
    }

    if ( argument[0] == '\0' || arg[0] == '\0' )
    {
	send_to_char( "Syntax: emote <person> <sentence>\n\r", ch );
	return;
    }

    if ( ( victim = get_char_room( ch, arg ) ) == NULL )
    {
	send_to_char( "They aren't here.\n\r", ch );
	return;
    }

    if (IS_NPC(ch)) strcpy(you, ch->short_descr);
	else strcpy(you, ch->name);
    if (IS_NPC(victim)) strcpy(you, victim->short_descr);
	else strcpy(you, victim->name);
/*
oldarg = argument;
*/
    strcpy( oldarg,argument );
    strcpy( buf, argument );
    for ( plast = argument; *plast != '\0'; plast++ )
	;

    if ( isalpha(plast[-1]) )
	strcat( buf, "." );
    argument = socialc(ch, buf, you, them);

    strcpy( buf, argument );
    strcpy( buf2, "You ");
    buf[0] = LOWER(buf[0]);
    strcat( buf2, buf );
    capitalize( buf2 );
    act( buf2, ch, NULL, victim, TO_CHAR );

    if (ch->in_room->vnum != ROOM_VNUM_IN_OBJECT)
    {
	strcpy( buf, oldarg );
	for ( plast = argument; *plast != '\0'; plast++ )
	    ;
	if ( isalpha(plast[-1]) )
	    strcat( buf, "." );

	argument = socialn(ch, buf, you, them);

	strcpy( buf, argument );
	strcpy( buf2, "$n ");
	buf[0] = LOWER(buf[0]);
	strcat( buf2, buf );
	capitalize( buf2 );
    	act( buf2, ch, NULL, victim, TO_NOTVICT );

	strcpy( buf, oldarg );
	for ( plast = argument; *plast != '\0'; plast++ )
	    ;
	if ( isalpha(plast[-1]) )
	    strcat( buf, "." );

	argument = socialv(ch, buf, you, them);

	strcpy( buf, argument );
	strcpy( buf2, "$n ");
	buf[0] = LOWER(buf[0]);
	strcat( buf2, buf );
	capitalize( buf2 );
    	act( buf2, ch, NULL, victim, TO_VICT );
	return;
    }

    to = ch->in_room->people;
    for ( ; to != NULL; to = to->next_in_room )
    {
	is_ok = FALSE;

	if ( to->desc == NULL || !IS_AWAKE(to) )
	    continue;

	if ( ch == to )
	    continue;

	if (!IS_NPC(ch) && ch->pcdata->chobj != NULL && 
	    ch->pcdata->chobj->in_room != NULL &&
	    !IS_NPC(to) && to->pcdata->chobj != NULL && 
	    to->pcdata->chobj->in_room != NULL &&
	    ch->in_room == to->in_room)
		is_ok = TRUE; else is_ok = FALSE;

	if (!IS_NPC(ch) && ch->pcdata->chobj != NULL && 
	    ch->pcdata->chobj->in_obj != NULL &&
	    !IS_NPC(to) && to->pcdata->chobj != NULL && 
	    to->pcdata->chobj->in_obj != NULL &&
	    ch->pcdata->chobj->in_obj == to->pcdata->chobj->in_obj)
		is_ok = TRUE; else is_ok = FALSE;

	if (!is_ok) continue;

	if (IS_NPC(ch))
	    sprintf(name, ch->short_descr);
	else if (!IS_NPC(ch) && IS_AFFECTED(ch,AFF_POLYMORPH))
	    sprintf(name, ch->morph);
        else if (!IS_NPC(ch) && IS_AFFECTED(ch,AFF_SHIFT))
	    sprintf(name, ch->morph);

        else
	    sprintf(name, ch->name);
	name[0]=UPPER(name[0]);
	sprintf(poly,"%s %s\n\r", name,buf);
	send_to_char(poly,to);
    }
    return;
}



void do_bug( CHAR_DATA *ch, char *argument )
{
    append_file( ch, BUG_FILE, argument );
    send_to_char( "Ok.  Thanks.\n\r", ch );
    return;
}



void do_idea( CHAR_DATA *ch, char *argument )
{
    append_file( ch, IDEA_FILE, argument );
    send_to_char( "Ok.  Thanks.\n\r", ch );
    return;
}



void do_typo( CHAR_DATA *ch, char *argument )
{
    append_file( ch, TYPO_FILE, argument );
    send_to_char( "Ok.  Thanks.\n\r", ch );
    return;
}



void do_rent( CHAR_DATA *ch, char *argument )
{
    send_to_char( "There is no rent here.  Just save and quit.\n\r", ch );
    return;
}



void do_qui( CHAR_DATA *ch, char *argument )
{
    send_to_char( "If you want to QUIT, you have to spell it out.\n\r", ch );
    return;
}



void do_quit( CHAR_DATA *ch, char *argument )
{
    DESCRIPTOR_DATA *d;
    char buf[MAX_STRING_LENGTH];
    OBJ_DATA *obj;
    OBJ_DATA *obj_next;
    CHAR_DATA *mount;

    if ( IS_NPC(ch) )
	return;

    if ( ch->position == POS_FIGHTING )
    {
	send_to_char( "No way! You are fighting.\n\r", ch );
	return;
    }

    if ( ch->position  < POS_SLEEPING )
    {
	send_to_char( "You're not DEAD yet.\n\r", ch );
	return;
    }

	if (ch->fight_timer >0) {
	send_to_char("Not until your fight timer expires.\n\r", ch );
	return;}

	save_char_obj_backup(ch);

    if (ch->gladiator  != NULL)
    ch->gladiator       = NULL; /* set player to bet on to NULL */
    if (ch->challenger != NULL)
    ch->challenger = NULL;
    if (ch->challenged != NULL)
    ch->challenged = NULL;
    if (IS_SET(ch->act,PLR_CHALLENGER))
    REMOVE_BIT(ch->act,PLR_CHALLENGER);
    if (IS_SET(ch->act,PLR_CHALLENGED))
    REMOVE_BIT(ch->act,PLR_CHALLENGED);

    if ( (mount = ch->mount) != NULL ) do_dismount(ch,"");

    send_to_char( "     Lifes a Bitch...then you die or marry one 
\n\r",ch);
/*    send_to_char( "   Extraordinary Claimes require Extraordinary
evidence.\n\r",ch);
    send_to_char( "               -Carl Sagan      \n\r",ch);
    send_to_char( "                          \n\r",ch);
    send_to_char( "                 \n\r\n\r",ch );*/
/*
    send_to_char( "\n\r           I'm a lean dog, a keen dog, a wild dog, and lone;\n\r",ch);
    send_to_char( "           I'm a rough dog, a tough dog, hunting on my own;\n\r",ch);
    send_to_char( "           I'm a bad dog, a mad dog, teasing silly sheep;\n\r",ch);
    send_to_char( "           I love to sit and bay the moon, to keep fat souls from sleep.\n\r\n\r",ch);
*/  
  /*
     * After extract_char the ch is no longer valid!
     */

	if (ch->pcdata->in_progress)
		free_note(ch->pcdata->in_progress);

    d = ch->desc;
    for ( obj = ch->carrying; obj != NULL; obj = obj_next )
    {
	obj_next = obj->next_content;
	if (obj->wear_loc == WEAR_NONE) continue;
    	if ( !IS_NPC(ch) && ((obj->chobj != NULL && !IS_NPC(obj->chobj) &&
	    obj->chobj->pcdata->obj_vnum != 0) || obj->item_type ==ITEM_KEY))
	    unequip_char(ch,obj);
    }

    for ( obj = ch->carrying; obj != NULL; obj = obj->next_content )
    {
        if ( IS_SET(obj->quest, QUEST_ARTIFACT) || obj->item_type == ITEM_KINGDOM_POWER 
	|| obj->item_type == ITEM_CORPSE_NPC)
	{
	  obj_from_char( obj );
	  obj_to_room( obj, ch->in_room );
	}
    }

    save_char_obj( ch );
    if (ch->pcdata->obj_vnum != 0)
    	act( "$n slowly fades out of existance.", ch, NULL, NULL, TO_ROOM );
    else
    	act( "$n has left the game.", ch, NULL, NULL, TO_ROOM );
    
    if( IS_SET(ch->act, PLR_TAG) )
    {
      REMOVE_BIT(ch->act, PLR_TAG);
      sprintf(buf, "TAG Game update:- %s has quit the TAG Game so no one is IT.\n\r", ch->name );
      do_info(ch, buf);
    }

    if ( d != NULL )
	close_socket2( d, FALSE );

    if (ch->in_room != NULL) char_from_room(ch);
    char_to_room(ch,get_room_index(3));

    sprintf( log_buf, "%s has quit.", ch->name );
    log_string( log_buf );

    if (ch->pcdata->obj_vnum == 0)
    {
	sprintf( buf, "#2%s #7has fled from #2The Cronic#7.#n",
ch->pcdata->switchname );
	do_info(ch,buf);
    }
    if (ch->pcdata->chobj != NULL) extract_obj(ch->pcdata->chobj);
    extract_char( ch, TRUE );
    return;
}



void do_save( CHAR_DATA *ch, char *argument )
{
    if ( IS_NPC(ch) )
	return;

    if ( ch->level < 2 )
    {
	send_to_char( "You must kill at least 5 mobs before you can save.\n\r", ch );
	return;
    }

    save_char_obj_backup( ch );
    save_char_obj( ch );
    send_to_char( "Saved.\n\r", ch );
    return;
}



void do_autosave( CHAR_DATA *ch, char *argument )
{
    if ( IS_NPC(ch) ) return;
    if ( ch->level < 2 ) return;
    save_char_obj( ch );
    return;
}



void do_follow( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;

    one_argument( argument, arg );

	
    if ( arg[0] == '\0' )
    {
	send_to_char( "Follow whom?\n\r", ch );
	return;
    }
    if ( ( victim = get_char_room( ch, arg ) ) == NULL )
    {
	send_to_char( "They aren't here.\n\r", ch );
	return;
    }
/*
	if ( ch->played  6789;
	return;
}
*/
    if ( IS_AFFECTED(ch, AFF_CHARM) && ch->master != NULL )
    {
	act( "But you'd rather follow $N!", ch, NULL, ch->master, TO_CHAR );
	return;
    }
    if ( victim == ch )
    {
	if ( ch->master == NULL )
	{
	    send_to_char( "You already follow yourself.\n\r", ch );
	    return;
	}
	stop_follower( ch );
	return;
    }
    
    if ( ch->master != NULL )
	stop_follower( ch );

    add_follower( ch, victim );
    return;
}



void add_follower( CHAR_DATA *ch, CHAR_DATA *master )
{
    if ( ch->master != NULL )
    {
	bug( "Add_follower: non-null master.", 0 );
	return;
    }

    ch->master        = master;
    ch->leader        = NULL;

    if ( can_see( master, ch ) )
	act( "$n now follows you.", ch, NULL, master, TO_VICT );

    act( "You now follow $N.",  ch, NULL, master, TO_CHAR );

    return;
}



void stop_follower( CHAR_DATA *ch )
{
    if ( ch->master == NULL )
    {
	bug( "Stop_follower: null master.", 0 );
	return;
    }

    if ( IS_AFFECTED(ch, AFF_CHARM) )
    {
	REMOVE_BIT( ch->affected_by, AFF_CHARM );
	affect_strip( ch, gsn_charm_person );
    }

    if ( can_see( ch->master, ch ) )
	act( "$n stops following you.", ch, NULL, ch->master, TO_VICT );
    act( "You stop following $N.", ch, NULL, ch->master, TO_CHAR    );

    ch->master = NULL;
    ch->leader = NULL;
    return;
}



void die_follower( CHAR_DATA *ch )
{
    CHAR_DATA *fch;

    if ( ch->master != NULL )
	stop_follower( ch );

    ch->leader = NULL;

    for ( fch = char_list; fch != NULL; fch = fch->next )
    {
	if ( fch->master == ch )
	    stop_follower( fch );
	if ( fch->leader == ch )
	    fch->leader = fch;
    }

    return;
}



void do_order( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;
    CHAR_DATA *och;
    CHAR_DATA *och_next;
    bool found;
    bool fAll;

    argument = one_argument( argument, arg );

    if ( arg[0] == '\0' || argument[0] == '\0' )
    {
	send_to_char( "Order whom to do what?\n\r", ch );
	return;
    }

    if ( IS_AFFECTED( ch, AFF_CHARM ) )
    {
	send_to_char( "You feel like taking, not giving, orders.\n\r", ch );
	return;
    }

    if ( !str_cmp( arg, "all" ) )
    {
      send_to_char("Ordering 'all' has been disabled.\n\r",ch);
      return;
    }
    else
    {
	fAll   = FALSE;
	if ( ( victim = get_char_room( ch, arg ) ) == NULL )
	{
	    send_to_char( "They aren't here.\n\r", ch );
	    return;
	}

	if ( victim == ch )
	{
	    send_to_char( "Aye aye, right away!\n\r", ch );
	    return;
	}

	if ( (!IS_AFFECTED(victim, AFF_CHARM) || victim->master != ch) )

	{
	    send_to_char( "Do it yourself!\n\r", ch );
	    return;
	}

	if ( IS_CLASS(ch, CLASS_VAMPIRE) && IS_CLASS(victim, CLASS_VAMPIRE)
	&& ((ch->pcdata->stats[UNI_GEN] != 2) || ch->pcdata->kingdom != victim->pcdata->kingdom))
	{
	    act( "$N ignores your order.", ch, NULL, victim, TO_CHAR );
	    act( "You ignore $n's order.", ch, NULL, victim, TO_VICT );
	    return;
	}

    }

    found = FALSE;
    for ( och = ch->in_room->people; och != NULL; och = och_next )
    {
	och_next = och->next_in_room;
	if ( och == ch ) continue;

	if ((IS_AFFECTED(och, AFF_CHARM)
	&&   och->master == ch
	&& ( fAll || och == victim ) )
	|| (ch->pcdata->stats[UNI_GEN] == 2 && (fAll || och == victim) &&
	ch->pcdata->kingdom == och->pcdata->kingdom))
	{
	    found = TRUE;
	    act( "$n orders you to '$t'.", ch, argument, och, TO_VICT );
	    interpret( och, argument );
	}
	else if ( !IS_NPC(ch) && !IS_NPC(och) && (fAll || och == victim)
	&& IS_CLASS(ch, CLASS_VAMPIRE) && IS_CLASS(och, CLASS_VAMPIRE)
	&& ch->pcdata->stats[UNI_GEN] < och->pcdata->stats[UNI_GEN] && 
	ch->pcdata->kingdom == och->pcdata->kingdom)
	{
	    found = TRUE;
	    act( "$n orders you to '$t'.", ch, argument, och, TO_VICT );
	    interpret( och, argument );
	}
    }

    if ( found ) 
	
	send_to_char( "Ok.\n\r", ch );
    else
	send_to_char( "You have no followers here.\n\r", ch );
	WAIT_STATE( ch, 12 );    
return;
}



void do_command( CHAR_DATA *ch, char *argument )
{
    char arg1[MAX_INPUT_LENGTH];
    char arg2[MAX_INPUT_LENGTH];
    char buf[MAX_STRING_LENGTH];
    char buffy[MAX_STRING_LENGTH];
    CHAR_DATA *victim;
    int awe;
    

    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );

    if (IS_NPC(ch)) return;
    if (!IS_CLASS(ch, CLASS_VAMPIRE))
    {
	send_to_char( "Huh?\n\r", ch );
	return;
    }

    if (ch->power[DISC_VAMP_DOMI] < 1)
    {
        send_to_char( "You must obtain at least level 1 in Dominate to use Command.\n\r", ch );
	return;
    }

    if (ch->spl[RED_MAGIC] < 1)
    {
	send_to_char( "Your mind is too weak.\n\r", ch );
	return;
    }

    if ( arg1[0] == '\0' || arg2[0] == '\0' )
    {
	send_to_char( "Command whom to do what?\n\r", ch );
	return;
    }

    if ( ( victim = get_char_room( ch, arg1 ) ) == NULL )
    {
	send_to_char( "They aren't here.\n\r", ch );
	return;
    }

    if ( victim == ch )
    {
	send_to_char( "How can you command yourself??\n\r", ch );
	return;
    }

  {
    if (ch->power[DISC_VAMP_DOMI] > 2)
	awe = 50;
    else if (ch->power[DISC_VAMP_DOMI] > 3)
	awe = 75;
    else if (ch->power[DISC_VAMP_DOMI] > 4)
	awe = 100;
    else awe = 25;
  }

    if (IS_EXTRA(ch, EXTRA_AWE))
    {
      awe += 75;
    }

    if ( !IS_NPC(victim) && victim->level != 3 )
    {
	send_to_char( "You can only command other avatars.\n\r", ch );
	return;
    }

  if ( ch->power[DISC_VAMP_DOMI] > 1 )
  {
    sprintf( buffy, "%s %s", arg2, argument );
    if (IS_NPC(victim))
    	sprintf( buf, "I think %s wants to %s",victim->short_descr,buffy);
    else if (!IS_NPC(victim) && IS_AFFECTED(victim, AFF_POLYMORPH))
    	sprintf( buf, "I think %s wants to %s", victim->morph,buffy);
    else
    	sprintf( buf, "I think %s wants to %s", victim->name,buffy);
    do_say(ch,buf);
  }
  else
  {
    if (IS_NPC(victim))
        sprintf( buf, "I think %s wants to %s", victim->short_descr, arg2 );
    else if (!IS_NPC(victim) && IS_AFFECTED(victim, AFF_POLYMORPH))
        sprintf( buf, "I think %s wants to %s", victim->morph, arg2 );
    else
        sprintf( buf, "I think %s wants to %s", victim->name, arg2 );
    do_say(ch,buf); 
  }

    if ( IS_NPC(victim) && victim->level >= awe )
    {
    	act("You shake off $N's suggestion.",victim,NULL,ch,TO_CHAR);
    	act("$n shakes off $N's suggestion.",victim,NULL,ch,TO_NOTVICT);
    	act("$n shakes off your suggestion.",victim,NULL,ch,TO_VICT);
    	act("$s mind is too strong to overcome.",victim,NULL,ch,TO_VICT);
	return;
    }

    else if ( victim->spl[BLUE_MAGIC] >= (ch->spl[RED_MAGIC]/2) )
    {
    	act("You shake off $N's suggestion.",victim,NULL,ch,TO_CHAR);
    	act("$n shakes off $N's suggestion.",victim,NULL,ch,TO_NOTVICT);
    	act("$n shakes off your suggestion.",victim,NULL,ch,TO_VICT);
    	act("$s mind is too strong to overcome.",victim,NULL,ch,TO_VICT);
	return;
    }

    act("You blink in confusion.",victim,NULL,NULL,TO_CHAR);
    act("$n blinks in confusion.",victim,NULL,NULL,TO_ROOM);
    strcpy(buf,"Yes, you're right, I do...");
    do_say(victim,buf);
    if ( ch->power[DISC_VAMP_DOMI] > 1 )
      interpret( victim, buffy );
    else interpret( victim, arg2 );
    return;
  
}



void do_group( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_STRING_LENGTH];
    char arg[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;

    one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
	CHAR_DATA *gch;
	CHAR_DATA *leader;

	
	leader = (ch->leader != NULL) ? ch->leader : ch;
	sprintf( buf, "%s's group:\n\r", PERS(leader, ch) );
	send_to_char( buf, ch );
	
	for ( gch = char_list; gch != NULL; gch = gch->next )
	{
	    if ( is_same_group( gch, ch ) )
	    {
		sprintf( buf,
		"[%-16s] %4d/%4d hp %4d/%4d mana %4d/%4d mv %5d xp\n\r",
		    capitalize( PERS(gch, ch) ),
		    gch->hit,   gch->max_hit,
		    gch->mana,  gch->max_mana,
		    gch->move,  gch->max_move,
		    gch->exp    );
		send_to_char( buf, ch );
	    }
	}
	return;
    }

    if ( ( victim = get_char_room( ch, arg ) ) == NULL )
    {
	send_to_char( "They aren't here.\n\r", ch );
	return;
    }

    if ( ch->master != NULL || ( ch->leader != NULL && ch->leader != ch ) )
    {
	send_to_char( "But you are following someone else!\n\r", ch );
	return;
    }

    if ( victim->master != ch && ch != victim )
    {
	act( "$N isn't following you.", ch, NULL, victim, TO_CHAR );
	return;
    }

    if ( is_same_group( victim, ch ) && ch != victim )
    {
	victim->leader = NULL;
	act( "$n removes $N from $s group.",   ch, NULL, victim, TO_NOTVICT );
	act( "$n removes you from $s group.",  ch, NULL, victim, TO_VICT    );
	act( "You remove $N from your group.", ch, NULL, victim, TO_CHAR    );
	return;
    }

    victim->leader = ch;
    act( "$N joins $n's group.", ch, NULL, victim, TO_NOTVICT );
    act( "You join $n's group.", ch, NULL, victim, TO_VICT    );
    act( "$N joins your group.", ch, NULL, victim, TO_CHAR    );
    return;
}



/*
 * 'Split' originally by Gnort, God of Chaos.
 */
void do_split( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_STRING_LENGTH];
    char arg[MAX_INPUT_LENGTH];
    CHAR_DATA *gch;
    int members;
    int amount;
    int share;
    int extra;

    one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
	send_to_char( "Split how much?\n\r", ch );
	return;
    }
    
    amount = atoi( arg );

    if ( amount < 0 )
    {
	send_to_char( "Your group wouldn't like that.\n\r", ch );
	return;
    }

    if ( amount == 0 )
    {
	send_to_char( "You hand out zero coins, but no one notices.\n\r", ch );
	return;
    }

    if ( ch->gold < amount )
    {
	send_to_char( "You don't have that much gold.\n\r", ch );
	return;
    }
  
    members = 0;
    for ( gch = ch->in_room->people; gch != NULL; gch = gch->next_in_room )
    {
	if ( is_same_group( gch, ch ) )
	    members++;
    }

    if ( members < 2 )
    {
	send_to_char( "Just keep it all.\n\r", ch );
	return;
    }
	    
    share = amount / members;
    extra = amount % members;

    if ( share == 0 )
    {
	send_to_char( "Don't even bother, cheapskate.\n\r", ch );
	return;
    }

    ch->gold -= amount;
    ch->gold += share + extra;

    sprintf( buf,
	"You split %d gold coins.  Your share is %d gold coins.\n\r",
	amount, share + extra );
    send_to_char( buf, ch );

    sprintf( buf, "$n splits %d gold coins.  Your share is %d gold coins.",
	amount, share );

    for ( gch = ch->in_room->people; gch != NULL; gch = gch->next_in_room )
    {
	if ( gch != ch && is_same_group( gch, ch ) )
	{
	    act( buf, ch, NULL, gch, TO_VICT );
	    gch->gold += share;
	}
    }

    return;
}



void do_gtell( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_STRING_LENGTH];
    CHAR_DATA *gch;

    if ( argument[0] == '\0' )
    {
	send_to_char( "Tell your group what?\n\r", ch );
	return;
    }

    if ( IS_SET( ch->act, PLR_NO_TELL ) )
    {
	send_to_char( "Your message didn't get through!\n\r", ch );
	return;
    }

    /*
     * Note use of send_to_char, so gtell works on sleepers.
     */
    sprintf( buf, "%s tells the group '%s'.\n\r", ch->name, argument );
    for ( gch = char_list; gch != NULL; gch = gch->next )
    {
	if ( is_same_group( gch, ch ) )
	    send_to_char( buf, gch );
    }

    return;
}



/*
 * It is very important that this be an equivalence relation:
 * (1) A ~ A
 * (2) if A ~ B then B ~ A
 * (3) if A ~ B  and B ~ C, then A ~ C
 */
bool is_same_group( CHAR_DATA *ach, CHAR_DATA *bch )
{
    if ( ach->leader != NULL ) ach = ach->leader;
    if ( bch->leader != NULL ) bch = bch->leader;
    return ach == bch;
}

void do_changelight(CHAR_DATA *ch, char *argument )
{
    if (IS_SET(ch->in_room->room_flags, ROOM_DARK))
    {
	REMOVE_BIT(ch->in_room->room_flags, ROOM_DARK);
	act("The room is suddenly filled with light!",ch,NULL,NULL,TO_CHAR);
	act("The room is suddenly filled with light!",ch,NULL,NULL,TO_ROOM);
	return;
    }
    SET_BIT(ch->in_room->room_flags, ROOM_DARK);
    act("The lights in the room suddenly go out!",ch,NULL,NULL,TO_CHAR);
    act("The lights in the room suddenly go out!",ch,NULL,NULL,TO_ROOM);
    return;
}

void open_lift( CHAR_DATA *ch )
{
    ROOM_INDEX_DATA *location;
    int in_room;

    in_room  = ch->in_room->vnum;
    location = get_room_index(in_room);

    if (is_open(ch)) return;

    act("The doors open.",ch,NULL,NULL,TO_CHAR);
    act("The doors open.",ch,NULL,NULL,TO_ROOM);
    move_door(ch);
    if (is_open(ch)) act("The doors close.",ch,NULL,NULL,TO_ROOM);
    if (!same_floor(ch,in_room)) act("The lift judders suddenly.",ch,NULL,NULL,TO_ROOM);
    if (is_open(ch)) act("The doors open.",ch,NULL,NULL,TO_ROOM);
    move_door(ch);
    open_door(ch,FALSE);
    char_from_room(ch);
    char_to_room(ch,location);
    open_door(ch,TRUE);
    move_door(ch);
    open_door(ch,TRUE);
    thru_door(ch,in_room);
    char_from_room(ch);
    char_to_room(ch,location);
    return;
}

void close_lift( CHAR_DATA *ch )
{
    ROOM_INDEX_DATA *location;
    int in_room;

    in_room  = ch->in_room->vnum;
    location = get_room_index(in_room);

    if (!is_open(ch)) return;
    act("The doors close.",ch,NULL,NULL,TO_CHAR);
    act("The doors close.",ch,NULL,NULL,TO_ROOM);
    open_door(ch,FALSE);
    move_door(ch);
    open_door(ch,FALSE);
    char_from_room(ch);
    char_to_room(ch,location);
    return;
}

void move_lift( CHAR_DATA *ch, int to_room )
{
    ROOM_INDEX_DATA *location;
    int in_room;

    in_room  = ch->in_room->vnum;
    location = get_room_index(in_room);

    if (is_open(ch)) act("The doors close.",ch,NULL,NULL,TO_CHAR);
    if (is_open(ch)) act("The doors close.",ch,NULL,NULL,TO_ROOM);
    if (!same_floor(ch,to_room)) act("The lift judders suddenly.",ch,NULL,NULL,TO_CHAR);
    if (!same_floor(ch,to_room)) act("The lift judders suddenly.",ch,NULL,NULL,TO_ROOM);
    move_door(ch);
    open_door(ch,FALSE);
    char_from_room(ch);
    char_to_room(ch,location);
    open_door(ch,FALSE);
    thru_door(ch,to_room);
    return;
}

bool same_floor( CHAR_DATA *ch, int cmp_room )
{
    OBJ_DATA *obj;
    OBJ_DATA *obj_next;

    for ( obj = ch->in_room->contents; obj != NULL; obj = obj_next )
    {
	obj_next = obj->next_content;
	if ( obj->item_type != ITEM_PORTAL ) continue;
	if ( obj->pIndexData->vnum == 30001 ) continue;
	if ( obj->value[0] == cmp_room ) return TRUE;
	else return FALSE;
    }
    return FALSE;
}

bool is_open( CHAR_DATA *ch )
{
    OBJ_DATA *obj;
    OBJ_DATA *obj_next;

    for ( obj = ch->in_room->contents; obj != NULL; obj = obj_next )
    {
	obj_next = obj->next_content;
	if ( obj->item_type != ITEM_PORTAL ) continue;
	if ( obj->pIndexData->vnum == 30001 ) continue;
	if ( obj->value[2] == 0 ) return TRUE;
	else return FALSE;
    }
    return FALSE;
}

void move_door( CHAR_DATA *ch )
{
    OBJ_DATA *obj;
    OBJ_DATA *obj_next;
    ROOM_INDEX_DATA *pRoomIndex;

    for ( obj = ch->in_room->contents; obj != NULL; obj = obj_next )
    {
	obj_next = obj->next_content;
	if ( obj->item_type != ITEM_PORTAL ) continue;
	if ( obj->pIndexData->vnum == 30001 ) continue;
	pRoomIndex = get_room_index(obj->value[0]);
	char_from_room(ch);
	char_to_room(ch,pRoomIndex);
	return;
    }
    return;
}

void thru_door( CHAR_DATA *ch, int doorexit )
{
    OBJ_DATA *obj;
    OBJ_DATA *obj_next;

    for ( obj = ch->in_room->contents; obj != NULL; obj = obj_next )
    {
	obj_next = obj->next_content;
	if ( obj->item_type != ITEM_PORTAL ) continue;
	if ( obj->pIndexData->vnum == 30001 ) continue;
	obj->value[0] = doorexit;
	return;
    }
    return;
}

void open_door( CHAR_DATA *ch, bool be_open )
{
    OBJ_DATA *obj;
    OBJ_DATA *obj_next;

    for ( obj = ch->in_room->contents; obj != NULL; obj = obj_next )
    {
	obj_next = obj->next_content;
	if ( obj->item_type != ITEM_PORTAL ) continue;
	if ( obj->pIndexData->vnum == 30001 ) continue;
	if ( obj->value[2] == 0 && !be_open ) obj->value[2] = 3;
	else if ( obj->value[2] == 3 && be_open ) obj->value[2] = 0;
	return;
    }
    return;
}

void do_speak( CHAR_DATA *ch, char *argument )
{
    char arg [MAX_INPUT_LENGTH];
    char buf [MAX_INPUT_LENGTH];

    argument = one_argument( argument, arg );

    if (IS_NPC(ch)) return;

    if (arg[0] == '\0')
    {
	if (IS_SPEAKING(ch,LANG_DARK)) 
	    send_to_char("You are speaking the Dark tongue.\n\r",ch);
	else if (IS_SPEAKING(ch,DIA_OLDE)) 
	    send_to_char("You are speaking Olde Worlde.\n\r",ch);
	else if (IS_SPEAKING(ch,DIA_BAD)) 
	    send_to_char("You are speaking very badly.\n\r",ch);
	else
	    send_to_char("You are speaking the common language.\n\r",ch);
	strcpy(buf,"You can speak the following languages:");
	strcat(buf," Common Olde Bad");
	if (CAN_SPEAK(ch, LANG_DARK)) strcat(buf," Dark");
	strcat(buf,".\n\r");
	send_to_char(buf,ch);
	return;
    }

    if (!str_cmp(arg,"dark"))
    {
	if (!CAN_SPEAK(ch,LANG_DARK))
	{
	    send_to_char("You cannot speak the Dark tongue.\n\r",ch);
	    return;
	}
	if (IS_SPEAKING(ch,LANG_DARK))
	{
	    send_to_char("But you are already speaking the Dark tongue!\n\r",ch);
	    return;
	}
	ch->pcdata->language[0] = LANG_DARK;
	send_to_char("Ok.\n\r",ch);
	return;
    }
    else if (!str_cmp(arg,"common"))
    {
	if (ch->pcdata->language[0] == LANG_COMMON)
	{
	    send_to_char("But you are already speaking the common tongue!\n\r",ch);
	    return;
	}
	ch->pcdata->language[0] = LANG_COMMON;
	send_to_char("Ok.\n\r",ch);
	return;
    }
    else if (!str_cmp(arg,"olde"))
    {
	if (ch->pcdata->language[0] == DIA_OLDE)
	{
	    send_to_char("But you are already speaking Olde Worlde!\n\r",ch);
	    return;
	}
	ch->pcdata->language[0] = DIA_OLDE;
	send_to_char("Ok.\n\r",ch);
	return;
    }
    else if (!str_cmp(arg,"bad"))
    {
	if (ch->pcdata->language[0] == DIA_BAD)
	{
	    send_to_char("But you are already speaking badly!\n\r",ch);
	    return;
	}
	ch->pcdata->language[0] = DIA_BAD;
	send_to_char("Ok.\n\r",ch);
	return;
    }
/*
    else if (!str_cmp(arg,"common"))
    {
	if (ch->pcdata->language[0] < LANG_DARK)
	{
	    send_to_char("But you are already speaking the common tongue!\n\r",ch);
	    return;
	}
	ch->pcdata->language[0] = LANG_COMMON;
	if (CAN_SPEAK(ch,DIA_OLDE)) ch->pcdata->language[0] = DIA_OLDE;
	else if (CAN_SPEAK(ch,DIA_BAD)) ch->pcdata->language[0] = DIA_BAD;
	send_to_char("Ok.\n\r",ch);
	return;
    }
*/
    else
    {
	strcpy(buf,"You can speak the following languages:");
	strcat(buf," Common Olde Bad");
	if (CAN_SPEAK(ch, LANG_DARK)) strcat(buf," Dark");
	strcat(buf,".\n\r");
	send_to_char(buf,ch);
	return;
    }

    return;
}

char *badlang( CHAR_DATA *ch, char *argument )
{
    char buf  [MAX_STRING_LENGTH];
    char *pName;
    int iSyl;
    int length;

    struct spk_type
    {
	char *	old;
	char *	new;
    };

    static const struct spk_type spk_table[] =
    {
	{ " ",		" "		},
	{ "my name is",	"i calls meself"},
	{ "are not",	"aint"		},
	{ "have",	"'av"		},
	{ "my",		"me"		},
	{ "hello",	"oy"		},
	{ "hi ",	"oy "		},
	{ "i am",	"im"		},
	{ "it is",	"tis"		},
	{ "the ",	"da "		},
	{ " the",	" da"		},
	{ "thank",	"fank"		},
	{ "that",	"dat"		},
	{ "with",	"wiv"		},
	{ "they",	"day"		},
	{ "this",	"dis"		},
	{ "then",	"den"		},
	{ "there",	"ver"		},
	{ "their",	"ver"		},
	{ "thing",	"fing"		},
	{ "think",	"fink"		},
	{ "was",	"woz"		},
	{ "would",	"wud"		},
	{ "what",	"wot"		},
	{ "where",	"weer"		},
	{ "when",	"wen"		},
	{ "are",	"is"		},
	{ "you",	"ya"		},
	{ "your",	"yer"		},
	{ "dead",	"ded"		},
	{ "kill",	"stomp"		},
	{ "food",	"nosh"		},
	{ "blood",	"blud"		},
	{ "vampire",	"sucker"	},
	{ "kindred",	"suckers"	},
	{ "fire",	"hot"		},
	{ "dwarf",	"stunty"	},
	{ "dwarves",	"stunties"	},
	{ "goblin",	"gobbo"		},
	{ "death",	"def"		},
	{ "immune",	"mune"		},
	{ "immunit",	"munit"		},
	{ "childer",	"nippers"	},
	{ "childe",	"nipper"	},
	{ "child",	"nipper"	},
	{ "tradition",	"wassname"	},
	{ "generation",	"batch"		},
	{ "founded",	"made"		},
	{ "sired",	"nipped"	},
	{ "sire",	"dad"		},
	{ "lineage",	"istory"	},
	{ "recognize",	"dats"		},
	{ "recognize",	"dats"		},
	{ "decapitate",	"headchop"	},
	{ "decap",	"chop"		},
	{ "recites",	"sez"		},
	{ "recite",	"sez"		},
	{ "a", "a" }, { "b", "b" }, { "c", "c" }, { "d", "d" },
	{ "e", "e" }, { "f", "f" }, { "g", "g" }, { "h", "h" },
	{ "i", "i" }, { "j", "j" }, { "k", "k" }, { "l", "l" },
	{ "m", "m" }, { "n", "n" }, { "o", "o" }, { "p", "p" },
	{ "q", "q" }, { "r", "r" }, { "s", "s" }, { "t", "t" },
	{ "u", "u" }, { "v", "v" }, { "w", "w" }, { "x", "x" },
	{ "y", "y" }, { "z", "z" }, { ",", "," }, { ".", "." },
	{ ";", ";" }, { ":", ":" }, { "(", "(" }, { ")", ")" },
	{ ")", ")" }, { "-", "-" }, { "!", "!" }, { "?", "?" },
	{ "1", "1" }, { "2", "2" }, { "3", "3" }, { "4", "4" },
	{ "5", "5" }, { "6", "6" }, { "7", "7" }, { "8", "8" },
	{ "9", "9" }, { "0", "0" }, { "%", "%" }, {  "",  "" }
    };
    buf[0]	= '\0';

    if ( argument[0] == '\0' ) return argument;

    for ( pName = str_dup(argument); *pName != '\0'; pName += length )
    {
	for ( iSyl = 0; (length = strlen(spk_table[iSyl].old)) != 0; iSyl++ )
	{
	    if ( !str_prefix( spk_table[iSyl].old, pName ) )
	    {
		strcat( buf, spk_table[iSyl].new );
		break;
	    }
	}

	if ( length == 0 )
	    length = 1;
    }

    argument[0] = '\0';
    strcpy(argument,buf);
    argument[0] = UPPER(argument[0]);
    return argument;
}

char *oldelang( CHAR_DATA *ch, char *argument )
{
    char buf  [MAX_STRING_LENGTH];
    char *pName;
    int iSyl;
    int length;

    struct spk_type
    {
	char *	old;
	char *	new;
    };

    static const struct spk_type spk_table[] =
    {
	{ " ",		" "		},
	{ "have",	"hath"		},
	{ "hello",	"hail"		},
	{ "hi ",	"hail "		},
	{ " hi",	" hail"		},
	{ "are",	"art"		},
	{ "your",	"thy"		},
	{ "you",	"thou"		},
	{ "i think",	"methinks"	},
	{ "do ",	"doth "		},
	{ " do",	" doth"		},
	{ "it was",	"twas"		},
	{ "before",	"ere"		},
	{ "his",	"$s"		},
	{ "a", "a" }, { "b", "b" }, { "c", "c" }, { "d", "d" },
	{ "e", "e" }, { "f", "f" }, { "g", "g" }, { "h", "h" },
	{ "i", "i" }, { "j", "j" }, { "k", "k" }, { "l", "l" },
	{ "m", "m" }, { "n", "n" }, { "o", "o" }, { "p", "p" },
	{ "q", "q" }, { "r", "r" }, { "s", "s" }, { "t", "t" },
	{ "u", "u" }, { "v", "v" }, { "w", "w" }, { "x", "x" },
	{ "y", "y" }, { "z", "z" }, { ",", "," }, { ".", "." },
	{ ";", ";" }, { ":", ":" }, { "(", "(" }, { ")", ")" },
	{ ")", ")" }, { "-", "-" }, { "!", "!" }, { "?", "?" },
	{ "1", "1" }, { "2", "2" }, { "3", "3" }, { "4", "4" },
	{ "5", "5" }, { "6", "6" }, { "7", "7" }, { "8", "8" },
	{ "9", "9" }, { "0", "0" }, { "%", "%" }, {  "",  "" }
    };
    buf[0]	= '\0';

    if ( argument[0] == '\0' ) return argument;

    for ( pName = str_dup(argument); *pName != '\0'; pName += length )
    {
	for ( iSyl = 0; (length = strlen(spk_table[iSyl].old)) != 0; iSyl++ )
	{
	    if ( !str_prefix( spk_table[iSyl].old, pName ) )
	    {
		strcat( buf, spk_table[iSyl].new );
		break;
	    }
	}

	if ( length == 0 )
	    length = 1;
    }

    argument[0] = '\0';
    strcpy(argument,buf);
    argument[0] = UPPER(argument[0]);

    return argument;
}

char *darktongue( CHAR_DATA *ch, char *argument )
{
    char buf  [MAX_STRING_LENGTH];
    char *pName;
    int iSyl;
    int length;

    struct spk_type
    {
	char *	old;
	char *	new;
    };

    static const struct spk_type spk_table[] =
    {
	{ " ",		" "		},
	{ "a", "i" }, { "b", "t" }, { "c", "x" }, { "d", "j" },
	{ "e", "u" }, { "f", "d" }, { "g", "k" }, { "h", "z" },
	{ "i", "o" }, { "j", "s" }, { "k", "f" }, { "l", "h" },
	{ "m", "b" }, { "n", "c" }, { "o", "e" }, { "p", "r" },
	{ "q", "l" }, { "r", "v" }, { "s", "w" }, { "t", "q" },
	{ "u", "a" }, { "v", "n" }, { "w", "y" }, { "x", "g" },
	{ "y", "m" }, { "z", "p" }, { ",", "," }, { ".", "." },
	{ ";", ";" }, { ":", ":" }, { "(", "(" }, { ")", ")" },
	{ ")", ")" }, { "-", "-" }, { "!", "!" }, { "?", "?" },
	{ "", "" }
    };
    buf[0]	= '\0';

    if ( argument[0] == '\0' ) return argument;

    for ( pName = str_dup(argument); *pName != '\0'; pName += length )
    {
	for ( iSyl = 0; (length = strlen(spk_table[iSyl].old)) != 0; iSyl++ )
	{
	    if ( !str_prefix( spk_table[iSyl].old, pName ) )
	    {
		strcat( buf, spk_table[iSyl].new );
		break;
	    }
	}

	if ( length == 0 )
	    length = 1;
    }

    argument[0] = '\0';
    strcpy(argument,buf);
    argument[0] = UPPER(argument[0]);

    return argument;
}

char *drunktalk( CHAR_DATA *ch, char *argument )
{
    char buf  [MAX_STRING_LENGTH];
    char *pName;
    int iSyl;
    int length;
    int loop;

    struct spk_type
    {
	char *	old;
	char *	new;
    };

    static const struct spk_type spk_table[] =
    {
	{ " ",		" "		},
	{ "is",		"ish"		},
	{ "a", "a" }, { "b", "b" }, { "c", "c" }, { "d", "d" },
	{ "e", "e" }, { "f", "f" }, { "g", "g" }, { "h", "h" },
	{ "i", "i" }, { "j", "j" }, { "k", "k" }, { "l", "l" },
	{ "m", "m" }, { "n", "n" }, { "o", "o" }, { "p", "p" },
	{ "q", "q" }, { "r", "r" }, { "s", "s" }, { "t", "t" },
	{ "u", "u" }, { "v", "v" }, { "w", "w" }, { "x", "x" },
	{ "y", "y" }, { "z", "z" }, { ",", "," }, { ".", "." },
	{ ";", ";" }, { ":", ":" }, { "(", "(" }, { ")", ")" },
	{ ")", ")" }, { "-", "-" }, { "!", "!" }, { "?", "?" },
	{ "1", "1" }, { "2", "2" }, { "3", "3" }, { "4", "4" },
	{ "5", "5" }, { "6", "6" }, { "7", "7" }, { "8", "8" },
	{ "9", "9" }, { "0", "0" }, { "%", "%" }, {  "",  "" }
    };
    buf[0]	= '\0';

    if ( argument[0] == '\0' ) return argument;

    for ( pName = str_dup(argument); *pName != '\0'; pName += length )
    {
	for ( iSyl = 0; (length = strlen(spk_table[iSyl].old)) != 0; iSyl++ )
	{
	    if ( !str_prefix( spk_table[iSyl].old, pName ) )
	    {
		strcat( buf, spk_table[iSyl].new );
		if (number_range(1,5) == 1 && str_cmp(spk_table[iSyl].new," "))
		    strcat( buf, spk_table[iSyl].new );
		else if (!str_cmp(spk_table[iSyl].new," "))
		{
		    if (number_range(1,5) == 1 && strlen(buf) < MAX_INPUT_LENGTH)
			strcat( buf, "*hic* " );
		}
		break;
	    }
	}

	if ( length == 0 )
	    length = 1;
    }

    argument[0] = '\0';
    strcpy(argument,buf);
    argument[0] = UPPER(argument[0]);
    for (loop = 1; loop < strlen(argument); loop++ )
    {
	if (number_range(1,2) == 1)
	    argument[loop] = UPPER(argument[loop]);
    }

    return argument;
}

char *socialc( CHAR_DATA *ch, char *argument, char *you, char *them )
{
    char buf  [MAX_STRING_LENGTH];
    char *pName;
    int iSyl;
    int length;

    struct spk_type
    {
	char *	old;
	char *	new;
    };

    static const struct spk_type spk_table[] =
    {
	{ " ",		" "		},
	{ "you are",	"$E is"		},
	{ "you.",	"$M."		},
	{ "you,",	"$M,"		},
	{ "you ",	"$M "		},
	{ " you",	" $M"		},
	{ "your ",	"$S "		},
	{ " your",	" $S"		},
	{ "yours.",	"theirs."	},
	{ "yours,",	"theirs,"	},
	{ "yours ",	"theirs "	},
	{ " yours",	" theirs"	},
	{ "begins",	"begin"		},
	{ "caresses",	"caress"	},
	{ "gives",	"give"		},
	{ "glares",	"glare"		},
	{ "grins",	"grin"		},
	{ "licks",	"lick"		},
	{ "looks",	"look"		},
	{ "loves",	"love"		},
	{ "plunges",	"plunge"	},
	{ "presses",	"press"		},
	{ "pulls",	"pull"		},
	{ "runs",	"run"		},
	{ "slaps",	"slap"		},
	{ "slides",	"slide"		},
	{ "smashes",	"smash"		},
	{ "squeezes",	"squeeze"	},
	{ "stares",	"stare"		},
	{ "sticks",	"stick"		},
	{ "strokes",	"stroke"	},
	{ "tugs",	"tug"		},
	{ "thinks",	"think"		},
	{ "thrusts",	"thrust"	},
	{ "whistles",	"whistle"	},
	{ "wraps",	"wrap"		},
	{ "winks",	"wink"		},
	{ "wishes",	"wish"		},
	{ " winks",	" wink"		},
	{ " his",	" your"		},
	{ "his ",	"your "		},
	{ " her",	" your"		},
	{ "her ",	"your "		},
	{ " him",	" your"		},
	{ "him ",	"your "		},
	{ "the",	"the"		},
	{ " he",	" you"		},
	{ "he ",	"you "		},
	{ " she",	" you"		},
	{ "she ",	"you "		},
	{ "a", "a" }, { "b", "b" }, { "c", "c" }, { "d", "d" },
	{ "e", "e" }, { "f", "f" }, { "g", "g" }, { "h", "h" },
	{ "i", "i" }, { "j", "j" }, { "k", "k" }, { "l", "l" },
	{ "m", "m" }, { "n", "n" }, { "o", "o" }, { "p", "p" },
	{ "q", "q" }, { "r", "r" }, { "s", "s" }, { "t", "t" },
	{ "u", "u" }, { "v", "v" }, { "w", "w" }, { "x", "x" },
	{ "y", "y" }, { "z", "z" }, { ",", "," }, { ".", "." },
	{ ";", ";" }, { ":", ":" }, { "(", "(" }, { ")", ")" },
	{ ")", ")" }, { "-", "-" }, { "!", "!" }, { "?", "?" },
	{ "1", "1" }, { "2", "2" }, { "3", "3" }, { "4", "4" },
	{ "5", "5" }, { "6", "6" }, { "7", "7" }, { "8", "8" },
	{ "9", "9" }, { "0", "0" }, { "%", "%" }, {  "",  "" }
    };
    buf[0]	= '\0';

    if ( argument[0] == '\0' ) return argument;

    for ( pName = str_dup(argument); *pName != '\0'; pName += length )
    {
	for ( iSyl = 0; (length = strlen(spk_table[iSyl].old)) != 0; iSyl++ )
	{
	    if ( !str_prefix( spk_table[iSyl].old, pName ) )
	    {
		strcat( buf, spk_table[iSyl].new );
		break;
	    }
	}

	if ( length == 0 )
	    length = 1;
    }

    argument[0] = '\0';
    strcpy(argument,buf);
    argument[0] = UPPER(argument[0]);

    return argument;
}

char *socialv( CHAR_DATA *ch, char *argument, char *you, char *them )
{
    char buf  [MAX_STRING_LENGTH];
    char *pName;
    int iSyl;
    int length;

    struct spk_type
    {
	char *	old;
	char *	new;
    };

    static const struct spk_type spk_table[] =
    {
	{ " ",		" "		},
	{ " his",	" $s"		},
	{ "his ",	"$s "		},
	{ " her",	" $s"		},
	{ "her ",	"$s "		},
	{ " him",	" $m"		},
	{ "him ",	"$m "		},
	{ " he",	" $e"		},
	{ "he ",	"$e "		},
	{ " she",	" $e"		},
	{ "she ",	"$e "		},
	{ "a", "a" }, { "b", "b" }, { "c", "c" }, { "d", "d" },
	{ "e", "e" }, { "f", "f" }, { "g", "g" }, { "h", "h" },
	{ "i", "i" }, { "j", "j" }, { "k", "k" }, { "l", "l" },
	{ "m", "m" }, { "n", "n" }, { "o", "o" }, { "p", "p" },
	{ "q", "q" }, { "r", "r" }, { "s", "s" }, { "t", "t" },
	{ "u", "u" }, { "v", "v" }, { "w", "w" }, { "x", "x" },
	{ "y", "y" }, { "z", "z" }, { ",", "," }, { ".", "." },
	{ ";", ";" }, { ":", ":" }, { "(", "(" }, { ")", ")" },
	{ ")", ")" }, { "-", "-" }, { "!", "!" }, { "?", "?" },
	{ "1", "1" }, { "2", "2" }, { "3", "3" }, { "4", "4" },
	{ "5", "5" }, { "6", "6" }, { "7", "7" }, { "8", "8" },
	{ "9", "9" }, { "0", "0" }, { "%", "%" }, {  "",  "" }
    };
    buf[0]	= '\0';

    if ( argument[0] == '\0' ) return argument;

    for ( pName = str_dup(argument); *pName != '\0'; pName += length )
    {
	for ( iSyl = 0; (length = strlen(spk_table[iSyl].old)) != 0; iSyl++ )
	{
	    if ( !str_prefix( spk_table[iSyl].old, pName ) )
	    {
		strcat( buf, spk_table[iSyl].new );
		break;
	    }
	}

	if ( length == 0 )
	    length = 1;
    }

    argument[0] = '\0';
    strcpy(argument,buf);
    argument[0] = UPPER(argument[0]);

    return argument;
}

char *socialn( CHAR_DATA *ch, char *argument, char *you, char *them )
{
    char buf  [MAX_STRING_LENGTH];
    char *pName;
    int iSyl;
    int length;

    struct spk_type
    {
	char *	old;
	char *	new;
    };

    static const struct spk_type spk_table[] =
    {
	{ " ",		" "		},
	{ "you are",	"$N is"		},
	{ "you.",	"$N."		},
	{ "you,",	"$N,"		},
	{ "you ",	"$N "		},
	{ " you",	" $N"		},
	{ "your.",	"$N's."		},
	{ "your,",	"$N's,"		},
	{ "your ",	"$N's "		},
	{ " your",	" $N's"		},
	{ "yourself",	"$Mself"	},
	{ " his",	" $s"		},
	{ "his ",	"$s "		},
	{ " her",	" $s"		},
	{ "her ",	"$s "		},
	{ " him",	" $m"		},
	{ "him ",	"$m "		},
	{ " he",	" $e"		},
	{ "he ",	"$e "		},
	{ " she",	" $e"		},
	{ "she ",	"$e "		},
	{ "a", "a" }, { "b", "b" }, { "c", "c" }, { "d", "d" },
	{ "e", "e" }, { "f", "f" }, { "g", "g" }, { "h", "h" },
	{ "i", "i" }, { "j", "j" }, { "k", "k" }, { "l", "l" },
	{ "m", "m" }, { "n", "n" }, { "o", "o" }, { "p", "p" },
	{ "q", "q" }, { "r", "r" }, { "s", "s" }, { "t", "t" },
	{ "u", "u" }, { "v", "v" }, { "w", "w" }, { "x", "x" },
	{ "y", "y" }, { "z", "z" }, { ",", "," }, { ".", "." },
	{ ";", ";" }, { ":", ":" }, { "(", "(" }, { ")", ")" },
	{ ")", ")" }, { "-", "-" }, { "!", "!" }, { "?", "?" },
	{ "1", "1" }, { "2", "2" }, { "3", "3" }, { "4", "4" },
	{ "5", "5" }, { "6", "6" }, { "7", "7" }, { "8", "8" },
	{ "9", "9" }, { "0", "0" }, { "%", "%" }, {  "",  "" }
    };
    buf[0]	= '\0';

    if ( argument[0] == '\0' ) return argument;

    for ( pName = str_dup(argument); *pName != '\0'; pName += length )
    {
	for ( iSyl = 0; (length = strlen(spk_table[iSyl].old)) != 0; iSyl++ )
	{
	    if ( !str_prefix( spk_table[iSyl].old, pName ) )
	    {
		strcat( buf, spk_table[iSyl].new );
		break;
	    }
	}

	if ( length == 0 )
	    length = 1;
    }

    argument[0] = '\0';
    strcpy(argument,buf);
    argument[0] = UPPER(argument[0]);

    return argument;
}


void do_ignore(CHAR_DATA *ch, char *argument)
{
    CHAR_DATA *rch;
    char arg[MAX_INPUT_LENGTH],buf[MAX_STRING_LENGTH];
    DESCRIPTOR_DATA *d;
    int pos;
    bool found = FALSE;

    if (ch->desc == NULL)
	rch = ch;
    else
	rch = ch->desc->original ? ch->desc->original : ch;

    if (IS_NPC(rch))
	return;

    smash_tilde( argument );

    argument = one_argument(argument,arg);
    
    if (arg[0] == '\0')
    {
	if (rch->pcdata->forget[0] == NULL)
	{
	    send_to_char("You are not ignoring anyone.\n\r",ch);
	    return;
	}
	send_to_char("You are currently ignoring:\n\r",ch);

	for (pos = 0; pos < MAX_FORGET; pos++)
	{
	    if (rch->pcdata->forget[pos] == NULL)
		break;

	    sprintf(buf,"    %s\n\r",rch->pcdata->forget[pos]);
	    send_to_char(buf,ch);
	}
	return;
    }

    for (pos = 0; pos < MAX_FORGET; pos++)
    {
	if (rch->pcdata->forget[pos] == NULL)
	    break;

	if (!str_cmp(arg,rch->pcdata->forget[pos]))
	{
	    send_to_char("You have already ignored that person.\n\r",ch);
	    return;
	}
    }

    for (d = descriptor_list; d != NULL; d = d->next)
    {
	CHAR_DATA *wch;

 	if (d->connected != CON_PLAYING || !can_see(ch,d->character))
	    continue;
	
	wch = ( d->original != NULL ) ? d->original : d->character;

 	if (!can_see(ch,wch))
	    continue;

	if (!str_cmp(arg,wch->name))
	{
	    found = TRUE;
	    if (wch == ch)
	    {
		send_to_char("You can't ignore yourself!\n\r", ch );
		return;
	    }
	    if (wch->level >= LEVEL_IMMORTAL)
	    {
		send_to_char("That person is very hard to ignore.\n\r",ch);
		return;
	    }
	}
    }

    if (!found)
    {
	send_to_char("No one by that name is playing.\n\r",ch);
	return;
    }

    for (pos = 0; pos < MAX_FORGET; pos++)
    {
	if (rch->pcdata->forget[pos] == NULL)
	    break;
     }

     if (pos >= MAX_FORGET)
     {
	send_to_char("Sorry, you have reached the ignore limit.\n\r",ch);
	return;
     }
  
     /* make a new forget */
     rch->pcdata->forget[pos]		= str_dup(arg);
     sprintf(buf,"You are now ignoring %s.\n\r",arg);
     send_to_char(buf,ch);
}

void do_unignore(CHAR_DATA *ch, char *argument)
{
    CHAR_DATA *rch;
    char arg[MAX_INPUT_LENGTH],buf[MAX_STRING_LENGTH];
    int pos;
    bool found = FALSE;
 
    if (ch->desc == NULL)
	rch = ch;
    else
	rch = ch->desc->original ? ch->desc->original : ch;
 
    if (IS_NPC(rch))
	return;
 
    argument = one_argument(argument,arg);

    if (arg[0] == '\0')
    {
	if (rch->pcdata->forget[0] == NULL)
	{
	    send_to_char("You are not ignoring anyone.\n\r",ch);
	    return;
	}
	send_to_char("You are currently ignoring:\n\r",ch);

	for (pos = 0; pos < MAX_FORGET; pos++)
	{
	    if (rch->pcdata->forget[pos] == NULL)
		break;

	    sprintf(buf,"    %s\n\r",rch->pcdata->forget[pos]);
	    send_to_char(buf,ch);
	}
	return;
    }

    for (pos = 0; pos < MAX_FORGET; pos++)
    {
	if (rch->pcdata->forget[pos] == NULL)
	    break;

	if (found)
	{
	    rch->pcdata->forget[pos-1]		= rch->pcdata->forget[pos];
	    rch->pcdata->forget[pos]		= NULL;
	    continue;
	}

	if(!strcmp(arg,rch->pcdata->forget[pos]))
	{
	    send_to_char("Ignore removed.\n\r",ch);
	    free_string(rch->pcdata->forget[pos]);
	    rch->pcdata->forget[pos] = NULL;
	    found = TRUE;
	}
    }

    if (!found)
	send_to_char("No one by that name is ignored.\n\r",ch);
}

void do_tag( CHAR_DATA *ch, char *argument )
 {

   CHAR_DATA *victim;
   char      name[MAX_STRING_LENGTH];
   char      buf[MAX_STRING_LENGTH];

   name[0]='\0';
   game_in_play(ch, name);

   if (IS_NPC(ch))
      return;

   if ( argument[0] == '\0' )
      {
      send_to_char("Who do you want to tag?\n\r", ch);
      return;
      }

   if ( strcmp(argument,"show")==0 )
      {
      if ( name[0] == '\0' )
         {
	 send_to_char("There is no game of tag in progress, why not start one!\n\r", ch);
 	 return;
	 }
      else
	 {
	 sprintf(buf, "%s is currently IT!\n\r", name);
	 send_to_char(buf, ch);
	 return;
	 }
      }

    if ( strcmp(argument,"quit")==0 )
       {
	if (IS_SET(ch->act, PLR_TAG ))
	   {
	   REMOVE_BIT(ch->act, PLR_TAG);
	   send_to_char("You are nolonger playing TAG.\n\r", ch);
	   sprintf( buf, "TAG Game update:- %s has quit the TAG Game so no one is IT.\n\r", ch->name );
	   do_info(ch, buf );
	   return;
	   }
	else
	   {
	   send_to_char("But you aren't IT!\n\r", ch);
	   return;
	   }
	}
			
			
    victim = get_char_world(ch,argument);
		 
    if ( victim == NULL )
       {
       send_to_char( "They aren't playing!\n\r", ch);
       return;
       }

     if ( IS_NPC(victim) )
        {
        send_to_char("Sorry! NPC's can't play tag!\n\r", ch);
	return;
        }

     if (IS_SET(victim->act, PLR_NOTAG) )
        {
        send_to_char("They don't want to play tag... Spoil sports!\n\r", ch);
        return;
        }

     if ( (name[0] == '\0' ) ) /* No Game in progress... */
        {
        if ( ch->name == victim->name ) /* Player tags self... */
           {
	   SET_BIT(victim->act, PLR_TAG);
			
	   send_to_char("You tag yourself, Now go get someone!\n\r", ch);
	   sprintf( buf, "%s has started a game of TAG, and is IT!\n\r", ch->name ); /* Voltecs info channel */
	   do_info( ch, buf );
	   return;
	   }
        else /* Player tags someone else... */
	   {
	   if ((ch->in_room->vnum == victim->in_room->vnum) && can_see(ch, victim)) /* Players can see eachother... */
              {
	      SET_BIT(victim->act, PLR_TAG );
			
	      sprintf(buf, "%s Pounces on you shouting 'TAG! You're IT!'\n\r", ch->name);
	      send_to_char(buf, victim);
			
	      sprintf(buf, "You pounce on %s and shout 'TAG! You're IT!'\n\r", victim->name);
	      send_to_char(buf, ch);
			
	      sprintf(buf, "%s has started a game of TAG, and %s is IT!\n\r", ch->name, victim->name);
          do_info(ch, buf ); /* Voltecs info channel */
			
	      return;
              }
 	   else /* Players can't see eachother! */
	      {
	      send_to_char("You can't see anyone by that name...\n\r", ch);
	      return;
	      }
			   
	    }
          }
       else /* There is a game in play... */
	  {
	  if (!IS_SET(ch->act, PLR_TAG) ) /* The player calling tag isn't IT */
	     {
	     sprintf(buf, "There is allready a game in play, and %s is IT!\n\r", name);
	     send_to_char(buf, ch);
	     return;
	     }
	   else /* The player calling tag is IT */
             {
	      if (ch->name == victim->name ) /* Player tags self... */
	         {
		 send_to_char("Tagging yourself won't get you anywhere!\n\r", ch);
		 return;
	         }
	      else /* Player Tags someone else... */
	         {
	         if ((ch->in_room->vnum == victim->in_room->vnum) && can_see(ch, victim)) /* Players can see eachother... */
		    {
		    SET_BIT(victim->act, PLR_TAG );
		    REMOVE_BIT(ch->act, PLR_TAG );

		    sprintf(buf, "%s Pounces on you shouting 'TAG! You're IT!'\n\r", ch->name);
		    send_to_char(buf, victim);
		    sprintf(buf, "You pounce on %s and shout 'TAG! You're IT!'\n\r", victim->name);
		    send_to_char(buf, ch);
		    sprintf(buf, "TAG Game update:- %s has just tagged %s, so %s is IT!\n\r", ch->name, victim->name, victim->name);
		    do_info(ch, buf ); /* Voltecs info channel */
		    return;
		    } 
		 else	 /* Players can't see eachother... */
		    {
		    send_to_char("You can't see anyone by that name...\n\r", ch);
		    return;
		    }
		 } /* End of Player Tags someone else... */ 
	      } /* End of Player calling tag is IT */
	   } /* End of there is a game in play */
	} /* End of do_tag */
		
/********************************************************************/

void do_notag(CHAR_DATA *ch, char *argument) /*Removes a players TAG flag if set... */
{
        if (IS_SET (ch->act, PLR_NOTAG ) )
           {
           send_to_char("You now allow people to tag you. (YAY!)\n\r", ch);
           REMOVE_BIT( ch->act, PLR_NOTAG );
           return;
           }
        else
           {
           send_to_char("You nolonger let people tag you... (Spoil Sport!)\n\r", ch);
           SET_BIT( ch->act, PLR_NOTAG );
           return;
           }
}

/********************************************************************/

void game_in_play(CHAR_DATA *ch, char *name)
{
	CHAR_DATA       *on_it;
	DESCRIPTOR_DATA	*d;

	for ( d = descriptor_list; d; d = d->next )
	  {
	  if ( d->connected == CON_PLAYING && IS_SET(d->character->act, PLR_TAG) )
		  {
		  on_it = d->character;
		  strcpy(name, d->character->name);
		  }
	  }
	 return;
}

