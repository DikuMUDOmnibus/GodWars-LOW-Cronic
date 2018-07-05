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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "merc.h"



/*
 * Local functions.
 */
int	hit_gain	args( ( CHAR_DATA *ch ) );
int	mana_gain	args( ( CHAR_DATA *ch ) );
int	move_gain	args( ( CHAR_DATA *ch ) );
void	mobile_update	args( ( void ) );
void	weather_update	args( ( void ) );
void    room_update     args( ( void ) );
void	char_update	args( ( void ) );
void	obj_update	args( ( void ) );
void	aggr_update	args( ( void ) );
void    ww_update       args( ( void ) );
void    embrace_update  args((void));

void gain_exp( CHAR_DATA *ch, int gain )
{
    CHAR_DATA *mount = NULL;
    CHAR_DATA *master = NULL;

    if ( IS_NPC(ch) && (mount = ch->mount) != NULL && !IS_NPC(mount))
    {
	if ( (master = ch->master) == NULL || master != mount )
	    mount->exp += gain;
    }

    if ( (( get_age(ch) - 17) * 2 ) < 5 )
      gain *= 3;

    if ( !IS_NPC(ch) && !IS_IMMORTAL(ch) )
	ch->exp += gain;
    return;
}



/*
 * Regeneration stuff.
 */
int hit_gain( CHAR_DATA *ch )
{
    int gain;
    int conamount;

    if ( IS_NPC(ch) )
    {
	gain = ch->level;
    }
    else
    {
	if (IS_CLASS(ch, CLASS_VAMPIRE) && ch->position != POS_MEDITATING) return 0;
	gain = number_range( 10, 20 );

	if ((conamount = (get_curr_con(ch)+1)) > 1)
	{
	    switch ( ch->position )
	    {
		case POS_MEDITATING: 
		if (IS_CLASS(ch, CLASS_VAMPIRE))
	        {
		gain *= conamount * 1.5;
	        }
	        else
		gain *= conamount * 3;	break;
		case POS_SLEEPING:   gain *= conamount;		break;
		case POS_RESTING:    gain *= conamount * 0.5;	break;
	    }
	}

	if ( ch->pcdata->condition[COND_FULL]   == 0 && !IS_HERO(ch) )
	    gain *= 0.5;

	if ( ch->pcdata->condition[COND_THIRST] == 0 && !IS_HERO(ch) )
	    gain *= 0.5;

    }

    if ( IS_AFFECTED(ch, AFF_POISON))
	gain *= 0.25;
    if ( IS_AFFECTED(ch, AFF_FLAMING))
	gain *= 0.01;


    return UMIN(gain, ch->max_hit - ch->hit);
}



int mana_gain( CHAR_DATA *ch )
{
    int gain;
    int intamount;

    if ( IS_NPC(ch) )
    {
	gain = ch->level;
    }
    else
    {
	if (IS_CLASS(ch, CLASS_VAMPIRE) && ch->position != POS_MEDITATING) return 0;
	gain = number_range( 10, 20 );

	if ((intamount = (get_curr_int(ch)+1)) > 1)
	{
	    switch ( ch->position )
	    {
		case POS_MEDITATING:
		if (IS_CLASS(ch, CLASS_VAMPIRE)) gain *= intamount * 1.5;
		else gain *= intamount * ch->level;	break;
		case POS_SLEEPING:   gain *= intamount * 1.5;break;
		case POS_RESTING:    gain *= intamount ;		break;
	    }
	}

	if ( !IS_HERO(ch) && ch->pcdata->condition[COND_THIRST] == 0 )
	    gain *= 0.5;

    }

    if ( IS_AFFECTED( ch, AFF_POISON ))
	gain *= 0.25;
    if ( IS_AFFECTED( ch, AFF_FLAMING ))
	gain *= 0.01;

    return UMIN(gain, ch->max_mana - ch->mana);
}



int move_gain( CHAR_DATA *ch )
{
    int gain;
    int dexamount;

    if ( IS_NPC(ch) )
    {
	gain = ch->level;
    }
    else
    {
	if (IS_CLASS(ch, CLASS_VAMPIRE) && ch->position != POS_MEDITATING) return 0;
	gain = number_range( 10, 20 );

	if ((dexamount = (get_curr_dex(ch)+1)) > 1)
	{
	    switch ( ch->position )
	    {
		case POS_MEDITATING:
	   if (IS_CLASS(ch, CLASS_MONK))
	        gain *= dexamount * 5;
		else if (IS_CLASS(ch, CLASS_VAMPIRE))
		gain *= dexamount * 1.5;
	   else 
		gain *= dexamount * 3;
		break;
		case POS_SLEEPING:   gain *= dexamount;		break;
		case POS_RESTING:    gain *= dexamount * 0.5;	break;
	    }
	}

	if ( !IS_HERO(ch) && ch->pcdata->condition[COND_THIRST] == 0 )
	    gain *= 0.5;
    }

    if ( IS_AFFECTED(ch, AFF_POISON))
	gain *= 0.25;
    if ( IS_AFFECTED(ch, AFF_FLAMING))
	gain *= 0.01;

    return UMIN(gain, ch->max_move - ch->move);
}



void gain_condition( CHAR_DATA *ch, int iCond, int value )
{
    int condition;

    if ( value == 0 || IS_NPC(ch) )
	return;

    if (!IS_NPC(ch) && IS_HERO(ch) && !IS_CLASS(ch, CLASS_VAMPIRE) 
	&& iCond != COND_DRUNK)
	return;

    condition				= ch->pcdata->condition[iCond];
    if (!IS_NPC(ch) && !IS_CLASS(ch, CLASS_VAMPIRE) )
	ch->pcdata->condition[iCond]	= URANGE( 0, condition + value, 48 );
    else
	ch->pcdata->condition[iCond]	= URANGE( 0, condition + value, 100 );

    if ( ch->pcdata->condition[iCond] == 0 )
    {
	switch ( iCond )
	{
	case COND_FULL:
	    if (!IS_CLASS(ch, CLASS_VAMPIRE))
	    {
		send_to_char( "You are REALLY hungry.\n\r",  ch );
		act( "You hear $n's stomach rumbling.", ch, NULL, NULL, TO_ROOM );
	    }
	    break;

	case COND_THIRST:
	    if (!IS_CLASS(ch, CLASS_VAMPIRE)) 
		send_to_char( "You are REALLY thirsty.\n\r", ch );
	    else if (ch->hit > 0)
	    {
		send_to_char( "You are DYING from lack of blood!\n\r", ch );
		act( "$n gets a hungry look in $s eyes.", ch, NULL, NULL, TO_ROOM );
		ch->hit = ch->hit - number_range(2,5);
		if (number_percent() <= ch->beast && ch->beast > 0) vamp_rage(ch);
		if (!IS_VAMPAFF(ch, VAM_FANGS)) do_fangs(ch,"");
	    }
	    break;

	case COND_DRUNK:
	    if ( condition != 0 )
		send_to_char( "You are sober.\n\r", ch );
	    break;
	}
    }
    else if ( ch->pcdata->condition[iCond] < 10 )
    {
	switch ( iCond )
	{
	case COND_FULL:
	    if (!IS_CLASS(ch, CLASS_VAMPIRE)) 
		send_to_char( "You feel hungry.\n\r",  ch );
	    break;

	case COND_THIRST:
	    if (!IS_CLASS(ch, CLASS_VAMPIRE)) 
		send_to_char( "You feel thirsty.\n\r", ch );
	    else
	    {
		send_to_char( "You crave blood.\n\r", ch );
		if (number_range(1,1000) <= ch->beast && ch->beast > 0) vamp_rage(ch);
		if (number_percent() > (ch->pcdata->condition[COND_THIRST]+75)
		    && !IS_VAMPAFF(ch, VAM_FANGS)) do_fangs(ch,"");
	    }
	    break;
	}
    }

    return;
}



/*
 * Mob autonomous action.
 * This function takes 25% to 35% of ALL Merc cpu time.
 * -- Furey
 */
void mobile_update( void )
{
    CHAR_DATA *ch;
    CHAR_DATA *ch_next;
    EXIT_DATA *pexit;
    int door;

    /* Examine all mobs. */
    for ( ch = char_list; ch != NULL; ch = ch_next )
    {
	ch_next = ch->next;

	if ( ch->in_room == NULL ) continue;

	if ( ch->hunting != NULL && ch->hunting != '\0' && 
	    strlen(ch->hunting) > 1 )
	{
	    check_hunt( ch );
	    continue;
	}

	if ( !IS_NPC(ch) )
	{
	    if (ch->pcdata->condition[COND_DRUNK] > 10 && number_range(1,10) == 1)
	    {
		send_to_char("You hiccup loudly.\n\r",ch);
		act("$n hiccups.",ch,NULL,NULL,TO_ROOM);
	    }
	    if (ch->pcdata->stage[0] > 0 || ch->pcdata->stage[2] > 0)
	    {
		CHAR_DATA *vch;
		if (ch->pcdata->stage[1] > 0 && ch->pcdata->stage[2] >= 225)
		{
		    ch->pcdata->stage[2] += 1;
		    if ( ( vch = ch->pcdata->partner ) != NULL &&
			!IS_NPC(vch) && vch->pcdata->partner == ch &&
			((vch->pcdata->stage[2] >= 200 && vch->sex == SEX_FEMALE) ||
			(ch->pcdata->stage[2] >= 200 && ch->sex == SEX_FEMALE)))
		    {
			if (ch->in_room != vch->in_room) continue;
			if (vch->pcdata->stage[2] >= 225 &&
			    ch->pcdata->stage[2] >= 225 &&
			    vch->pcdata->stage[2] < 240 &&
			    ch->pcdata->stage[2] < 240)
			{
			    ch->pcdata->stage[2] = 240;
			    vch->pcdata->stage[2] = 240;
			}
			if (ch->sex == SEX_MALE && vch->pcdata->stage[2] >= 240)
			{
			    act("You thrust deeply between $N's warm, damp thighs.",ch,NULL,vch,TO_CHAR);
			    act("$n thrusts deeply between your warm, damp thighs.",ch,NULL,vch,TO_VICT);
			    act("$n thrusts deeply between $N's warm, damp thighs.",ch,NULL,vch,TO_NOTVICT);
			    if (vch->pcdata->stage[2] > ch->pcdata->stage[2])
				ch->pcdata->stage[2] = vch->pcdata->stage[2];
			}
			else if (ch->sex == SEX_FEMALE && vch->pcdata->stage[2] >= 240)
			{
			    act("You squeeze your legs tightly around $N, moaning loudly.",ch,NULL,vch,TO_CHAR);
			    act("$n squeezes $s legs tightly around you, moaning loudly.",ch,NULL,vch,TO_VICT);
			    act("$n squeezes $s legs tightly around $N, moaning loudly.",ch,NULL,vch,TO_NOTVICT);
			    if (vch->pcdata->stage[2] > ch->pcdata->stage[2])
				ch->pcdata->stage[2] = vch->pcdata->stage[2];
			}
		    }
		    if (ch->pcdata->stage[2] >= 250)
		    {
			if ( ( vch = ch->pcdata->partner ) != NULL &&
			    !IS_NPC(vch) && vch->pcdata->partner == ch &&
			    ch->in_room == vch->in_room)
			{
			    vch->pcdata->stage[2] = 250;
			    if (ch->sex == SEX_MALE)
			    {
				stage_update(ch,vch,2);
				stage_update(vch,ch,2);
			    }
			    else
			    {
				stage_update(vch,ch,2);
				stage_update(ch,vch,2);
			    }
			    ch->pcdata->stage[0] = 0;
			    vch->pcdata->stage[0] = 0;
			    if (!IS_EXTRA(ch, EXTRA_EXP))
			    {
				send_to_char("Congratulations on achieving a simultanious orgasm!  Recieve 100000 exp!\n\r",ch);
				SET_BIT(ch->extra, EXTRA_EXP);
				ch->exp += 100000;
			    }
			    if (!IS_EXTRA(vch, EXTRA_EXP))
			    {
				send_to_char("Congratulations on achieving a simultanious orgasm!  Recieve 100000 exp!\n\r",vch);
				SET_BIT(vch->extra, EXTRA_EXP);
				vch->exp += 100000;
			    }
			}
		    }
		}
		else
		{
		    if (ch->pcdata->stage[0] > 0 && ch->pcdata->stage[2] < 1 &&
			ch->position != POS_RESTING) 
		    {
			if (ch->pcdata->stage[0] > 1)
			    ch->pcdata->stage[0] -= 1;
			else
			    ch->pcdata->stage[0] = 0;
		    }
		    else if (ch->pcdata->stage[2]>0 && ch->pcdata->stage[0] < 1)
		    {
			if (ch->pcdata->stage[2] > 10)
			    ch->pcdata->stage[2] -= 10;
			else
			    ch->pcdata->stage[2] = 0;
			if (ch->sex == SEX_MALE && ch->pcdata->stage[2] == 0)
			    send_to_char("You feel fully recovered.\n\r",ch);
		    }
		}
	    }

	    if (!IS_NPC(ch) && IS_CLASS(ch, CLASS_VAMPIRE) && IS_HERO(ch))
	    {
		if ( ch->position == POS_FIGHTING && ch->rage > 0 
		    && ch->rage < 25 && !IS_ITEMAFF(ch, ITEMA_RAGER) )
		    ch->rage += 1;
		else if (ch->rage > 0 && !IS_ITEMAFF(ch, ITEMA_RAGER))
		    ch->rage -= 1;
		if (ch->rage < 1) continue;
		if ( ch->hit < ch->max_hit || ch->mana < ch->max_mana || 
		    ch->move < ch->max_move )
		    werewolf_regen(ch);
		if (ch->loc_hp[6] > 0)
		{
		    int sn = skill_lookup( "clot" );
		    (*skill_table[sn].spell_fun) (sn,ch->level,ch,ch);
		}
		else
		{
		    if ((ch->loc_hp[0] + ch->loc_hp[1] + ch->loc_hp[2] +
			ch->loc_hp[3] + ch->loc_hp[4] + ch->loc_hp[5]) != 0)
		    reg_mend(ch);
		}
	    }

	    if (!IS_NPC(ch) && IS_CLASS(ch, CLASS_MONK) && IS_HERO(ch))
	    {
	      if ( ch->position != POS_FIGHTING )
	      {
		if ( ch->chi[CURRENT] > 0 )
	        {
		    if ( number_range( 1,4 ) == 2 )
		    {
			ch->chi[CURRENT]--;
			stc("You feel more relaxed.\n\r",ch);
			act("$n looks more relaxed.",ch,NULL,NULL,TO_ROOM);
		    }
		}
	      }
	    }

	    if (!IS_NPC(ch) && IS_CLASS(ch, CLASS_NINJA) && IS_HERO(ch))
	    {
		if ( ch->position == POS_FIGHTING && ch->rage > 0 
		    && ch->rage < 25 && !IS_ITEMAFF(ch, ITEMA_RAGER) )
		    ch->rage += 1;
		else if (ch->rage > 0 && !IS_ITEMAFF(ch, ITEMA_RAGER))
		    ch->rage -= 1;
		if (ch->rage < 1) continue;
		if ( ch->hit < ch->max_hit || ch->mana < ch->max_mana || 
		    ch->move < ch->max_move )
		    werewolf_regen(ch);
		if (ch->loc_hp[6] > 0)
		{
		    int sn = skill_lookup( "clot" );
		    (*skill_table[sn].spell_fun) (sn,ch->level,ch,ch);
		}
		else
		{
		    if ((ch->loc_hp[0] + ch->loc_hp[1] + ch->loc_hp[2] +
			ch->loc_hp[3] + ch->loc_hp[4] + ch->loc_hp[5]) != 0)
		    reg_mend(ch);
		}
	    }
	    else if (!IS_NPC(ch) && IS_CLASS(ch, CLASS_WEREWOLF) && IS_HERO(ch))
	    {
		if (ch->position == POS_FIGHTING && !IS_ITEMAFF(ch, ITEMA_RAGER))
		{
		    if (ch->rage < 200)
			ch->rage += number_range(5,10);
		    if (ch->rage < 200 &&
ch->pcdata->powers[DISC_WERE_WOLF] > 3)
			ch->rage += number_range(5,10);
		    if (!IS_SET(ch->special, SPC_WOLFMAN) && 
			ch->rage >= 100)
			do_werewolf(ch,"");
		}
		else if (ch->rage > 0 && !IS_ITEMAFF(ch, ITEMA_RAGER))
		{
		    ch->rage -= 1;
		    if (ch->rage < 100) do_unwerewolf(ch,"");
		}
		if ( ch->hit < ch->max_hit || ch->mana < ch->max_mana || 
		    ch->move < ch->max_move )
		    werewolf_regen(ch);

		if (IS_CLASS(ch,CLASS_WEREWOLF) && ch->position == POS_SLEEPING
		    && ch->power[DISC_WERE_BEAR] > 3 && ch->hit > 0)
		{
		    if ( ch->hit < ch->max_hit || ch->mana < ch->max_mana || 
			ch->move < ch->max_move )
			werewolf_regen(ch);
		    if ( ch->hit < ch->max_hit || ch->mana < ch->max_mana || 
			ch->move < ch->max_move )
			werewolf_regen(ch);
		    if ( ch->hit < ch->max_hit || ch->mana < ch->max_mana || 
			ch->move < ch->max_move )
			werewolf_regen(ch);
		    if ( ch->hit < ch->max_hit || ch->mana < ch->max_mana || 
			ch->move < ch->max_move )
			werewolf_regen(ch);
		}
		if (ch->loc_hp[6] > 0)
		{
		    int sn = skill_lookup( "clot" );
		    (*skill_table[sn].spell_fun) (sn,ch->level,ch,ch);
		}
		else
		{
		    if ((ch->loc_hp[0] + ch->loc_hp[1] + ch->loc_hp[2] +
			ch->loc_hp[3] + ch->loc_hp[4] + ch->loc_hp[5]) != 0)
		    reg_mend(ch);
		}
	    }
	    else if (IS_CLASS(ch, CLASS_VAMPIRE) &&
		(ch->power[DISC_VAMP_OBFU] > 8)
		&& ch->in_room != NULL
		&& ch->in_room->vnum == 27000)
	    {
		if ( ch->hit < ch->max_hit || ch->mana < ch->max_mana || 
		    ch->move < ch->max_move )
		    werewolf_regen(ch);
		    werewolf_regen(ch);
		if (ch->hit > 0)
		{
		    if ( ch->hit < ch->max_hit || ch->mana < ch->max_mana
|| 
			ch->move < ch->max_move )
			werewolf_regen(ch);
		    if ( ch->hit < ch->max_hit || ch->mana < ch->max_mana
|| 
			ch->move < ch->max_move )
			werewolf_regen(ch);
		    if ( ch->hit < ch->max_hit || ch->mana < ch->max_mana
|| 
			ch->move < ch->max_move )
			werewolf_regen(ch);
		    if ( ch->hit < ch->max_hit || ch->mana < ch->max_mana
|| 
			ch->move < ch->max_move )
			werewolf_regen(ch);
		      werewolf_regen(ch);
		}
		if (ch->loc_hp[6] > 0)
		{
		    int sn = skill_lookup( "clot" );
		    (*skill_table[sn].spell_fun) (sn,ch->level,ch,ch);
		}
		else
		{
		    if ((ch->loc_hp[0] + ch->loc_hp[1] + ch->loc_hp[2] +
		ch->loc_hp[3] + ch->loc_hp[4] + ch->loc_hp[5]) != 0)
		    reg_mend(ch);
		}
	    }
	else if ( ( ( IS_CLASS(ch, CLASS_DEMON)
	   && IS_HERO(ch)
	   && ch->in_room != NULL 
	   && ( ch->in_room->vnum == 30000 
	   ||   ch->in_room->vnum == 30001
	   ||   ch->in_room->vnum == 30002
	   ||   ch->in_room->vnum == 30003
	   ||   ch->in_room->vnum == 30004
	   ||   ch->in_room->vnum == 30005
	   ||   ch->in_room->vnum == 30006
	   ||   ch->in_room->vnum == 30007
	   ||   ch->in_room->vnum == 30008
	   ||   ch->in_room->vnum == 30009
	   ||   ch->in_room->vnum == 30010 ) )
	   || ( IS_CLASS(ch, CLASS_DROW)
	   && IS_HERO(ch) 
	   && ch->in_room != NULL 
	   && (ch->in_room->vnum == 26000 || ch->in_room->vnum == 26001) ) ) )
	   {
		if ( ch->hit < ch->max_hit)
		ch->hit += 300;
 		if ( ch->mana < ch->max_mana)
		ch->mana += 300; 
		if ( ch->move < ch->max_move)
		    ch->move += 300;
		if (ch->hit > 0)
		{
		    if ( ch->hit < ch->max_hit || ch->mana < ch->max_mana || 
			ch->move < ch->max_move )
			werewolf_regen(ch);
		    if ( ch->hit < ch->max_hit || ch->mana < ch->max_mana || 
			ch->move < ch->max_move )
			werewolf_regen(ch);
		    if ( ch->hit < ch->max_hit || ch->mana < ch->max_mana || 
			ch->move < ch->max_move )
			werewolf_regen(ch);
		    if ( ch->hit < ch->max_hit || ch->mana < ch->max_mana || 
			ch->move < ch->max_move )
			werewolf_regen(ch);
		}
		if (ch->loc_hp[6] > 0)
		{
		    int sn = skill_lookup( "clot" );
		    (*skill_table[sn].spell_fun) (sn,ch->level,ch,ch);
		}
		else
		{
		    if ((ch->loc_hp[0] + ch->loc_hp[1] + ch->loc_hp[2] +
			ch->loc_hp[3] + ch->loc_hp[4] + ch->loc_hp[5]) != 0)
		    reg_mend(ch);
		}
	    }

	    else if (IS_CLASS(ch, CLASS_HIGHLANDER) && (get_eq_char(ch, WEAR_WIELD) != NULL))
    	    {
               int wpn = (ch->wpn[1]/3);        
		if ( ch->hit < ch->max_hit)
		ch->hit += wpn;
 		if ( ch->mana < ch->max_mana)
		ch->mana += wpn; 
		if ( ch->move < ch->max_move)
		    ch->move += wpn;
		if (ch->hit > 0)
		{
		    if ( ch->hit < ch->max_hit || ch->mana < ch->max_mana || 
			ch->move < ch->max_move )
			werewolf_regen(ch);
		    if ( ch->hit < ch->max_hit || ch->mana < ch->max_mana || 
			ch->move < ch->max_move )
			werewolf_regen(ch);
		}
		if (ch->loc_hp[6] > 0)
		{
		    int sn = skill_lookup( "clot" );
		    (*skill_table[sn].spell_fun) (sn,ch->level,ch,ch);
		}
		else
		{
		    if ((ch->loc_hp[0] + ch->loc_hp[1] + ch->loc_hp[2] +
			ch->loc_hp[3] + ch->loc_hp[4] + ch->loc_hp[5]) != 0)
		    reg_mend(ch);
		}
	    }
	else if (!IS_NPC(ch) && IS_ITEMAFF(ch, ITEMA_REGENERATE)) {
                   if ( ch->hit < ch->max_hit || ch->mana < ch->max_mana ||
                        ch->move < ch->max_move )
                        werewolf_regen(ch);
                    if ( ch->hit < ch->max_hit || ch->mana < ch->max_mana ||
                        ch->move < ch->max_move )   
                        werewolf_regen(ch);
                    if ( ch->hit < ch->max_hit || ch->mana < ch->max_mana ||
                        ch->move < ch->max_move )
                        werewolf_regen(ch);
                    if ( ch->hit < ch->max_hit || ch->mana < ch->max_mana ||
                        ch->move < ch->max_move )
                        werewolf_regen(ch);
		reg_mend(ch);
		}	
continue;
	}

	if ( IS_AFFECTED(ch, AFF_CHARM) ) continue;

	/* Examine call for special procedure */
	if ( ch->spec_fun != 0 )
	{
	    if ( (*ch->spec_fun) ( ch ) )
		continue;
	    if (ch == NULL) continue;
	}

	/* That's all for sleeping / busy monster */
	if ( ch->position != POS_STANDING )
	    continue;

	/* Scavenge */
	if ( IS_SET(ch->act, ACT_SCAVENGER)
	&&   ch->in_room->contents != NULL
	&&   number_bits( 2 ) == 0 )
	{
	    OBJ_DATA *obj;
	    OBJ_DATA *obj_best;
	    int max;

	    max         = 1;
	    obj_best    = 0;
	    for ( obj = ch->in_room->contents; obj; obj = obj->next_content )
	    {
		if ( CAN_WEAR(obj, ITEM_TAKE) && obj->cost > max )
		{
		    obj_best    = obj;
		    max         = obj->cost;
		}
	    }

	    if ( obj_best )
	    {
		obj_from_room( obj_best );
		obj_to_char( obj_best, ch );
		act( "$n picks $p up.", ch, obj_best, NULL, TO_ROOM );
		act( "You pick $p up.", ch, obj_best, NULL, TO_CHAR );
	    }
	}

	/* Wander */
	if ( !IS_SET(ch->act, ACT_SENTINEL)
	&& ( door = number_bits( 5 ) ) <= 5
	&& ( pexit = ch->in_room->exit[door] ) != NULL
	&&   pexit->to_room != NULL
	&&   !IS_SET(pexit->exit_info, EX_CLOSED)
	&&   !IS_SET(pexit->to_room->room_flags, ROOM_NO_MOB)
	&& ( ch->hunting == NULL || strlen(ch->hunting) < 2 )
	&& ( (!IS_SET(ch->act, ACT_STAY_AREA) && ch->level < 900)
	||   pexit->to_room->area == ch->in_room->area ) )
	{
	    move_char( ch, door );
	}

	/* Flee */
	if ( ch->hit < ch->max_hit / 2
	&& ( door = number_bits( 3 ) ) <= 5
	&& ( pexit = ch->in_room->exit[door] ) != NULL
	&&   pexit->to_room != NULL
	&&   !IS_AFFECTED(ch, AFF_WEBBED)
	&&   ch->level < 900
	&&   !IS_SET(pexit->exit_info, EX_CLOSED)
	&&   !IS_SET(pexit->to_room->room_flags, ROOM_NO_MOB) 
	&&   !IS_AFFECTED(ch, AFF_TENDRILS))
        {
	    CHAR_DATA *rch;
	    bool found;

	    found = FALSE;
	    for ( rch  = pexit->to_room->people;
		  rch != NULL;
		  rch  = rch->next_in_room )
	    {
		if ( !IS_NPC(rch) )
		{
		    found = TRUE;
		    break;
		}
	    }
	    if ( !found )
		move_char( ch, door );
	}

    }

    return;
}



/*
 * Update the weather.
 */
void weather_update( void )
{
    char buf[MAX_STRING_LENGTH];
    DESCRIPTOR_DATA *d;
    CHAR_DATA *ch = NULL;
    int diff;
    bool char_up;

    buf[0] = '\0';

    switch ( ++time_info.hour )
    {
    case  5:
	weather_info.sunlight = SUN_LIGHT;
	strcat( buf, "The day has begun.\n\r" );
	break;

    case  6:
	weather_info.sunlight = SUN_RISE;
	strcat( buf, "The sun rises in the east.\n\r" );
	break;

    case 19:
	weather_info.sunlight = SUN_SET;
	strcat( buf, "The sun slowly disappears in the west.\n\r" );
	break;

    case 20:
        weather_info.sunlight = SUN_DARK;
	strcat( buf, "The night has begun.\n\r" );
	break;

    case 24:
	time_info.hour = 0;
	time_info.day++;
	for ( d = descriptor_list; d != NULL; d = d->next )
	{
	    char_up = FALSE;
	    if (( d->connected == CON_PLAYING
	    ||    d->connected == CON_EDITING )
	    &&   (ch = d->character) != NULL
	    &&  !IS_NPC(ch))
	    {
		if (ch->fighting == NULL && !IS_SET(ch->newbits, NEW_NATURAL)
		 && ch->monkab[SPIRIT] >= 2 )
		  SET_BIT(ch->newbits, NEW_NATURAL);

		send_to_char( "You hear a clock in the distance strike midnight.\n\r",ch);
		if (IS_EXTRA(ch, EXTRA_ROT))
                {
                        send_to_char("Your flesh feels better.\n\r", ch);
                        REMOVE_BIT(ch->extra, EXTRA_ROT);
                        
                }
                if (IS_SET(ch->in_room->room_flags, ROOM_SILENCE))
                {
                send_to_char("The silence leaves the room.\n\r",ch);
                act("The silence leaves the room.",ch,NULL,NULL,TO_ROOM);
                REMOVE_BIT(ch->in_room->room_flags,ROOM_SILENCE);
                
                }

                if (IS_SET(ch->in_room->room_flags,ROOM_FLAMING))
                {
                send_to_char("The flames in the room die down.\n\r",ch);
                act("The flames in the room die down.",ch,NULL,NULL,TO_ROOM);
                REMOVE_BIT(ch->in_room->room_flags,ROOM_FLAMING);
                }
                if ( IS_CLASS(ch, CLASS_VAMPIRE) )
		{
		    if (ch->hit < ch->max_hit)
			{ ch->hit = ch->max_hit; char_up = TRUE; }
		    if (ch->mana < ch->max_mana)
			{ ch->mana = ch->max_mana; char_up = TRUE; }
		    if (ch->move < ch->max_move)
			{ ch->move = ch->max_move; char_up = TRUE; }
		    if (char_up) send_to_char( "You feel the strength of the kindred flow through your veins!\n\r", ch );
		    ch->position = POS_STANDING;
		    if (ch->pcdata->gentick < 23-ch->generation)
			ch->pcdata->gentick += 1;

		    if (ch->pcdata->gentick == 23-ch->generation 
  			&& ch->generation > 3 && ch->pcdata->lwrgen == FALSE )
		    {
			stc("Your blood boils and bubbles with power.\n\r",ch);
			send_to_char("You become one generation closer to Trace!\n\r",ch);
			ch->generation -=1;
			ch->pcdata->gentick=0;
			ch->pcdata->lwrgen=TRUE;
		    }



		    if (IS_EXTRA(ch, EXTRA_POTENCY))
		    { 
			send_to_char("You feel your blood potency fade away.\n\r", ch);	
			ch->generation =ch->generation +1;
			REMOVE_BIT(ch->extra, EXTRA_POTENCY);
		    }

                    if (IS_SET(ch->newbits, NEW_TIDE))
 	            {
 		     REMOVE_BIT(ch->newbits, NEW_TIDE);
                     send_to_char("The tide of vitae leaves you.\n\r", ch);
		    }

		    if (IS_SET(ch->extra, EXTRA_BAAL))
                    {
                       send_to_char("The spirit of Baal escapes you.\n\r",ch);  
                       ch->power[DISC_VAMP_POTE] -= 2;
                       ch->power[DISC_VAMP_CELE] -= 2;
                       ch->power[DISC_VAMP_FORT] -= 2;
                       REMOVE_BIT(ch->extra, EXTRA_BAAL);
                    }   

                    if (IS_EXTRA(ch, EXTRA_FLASH))
                    {
                       send_to_char("Your speed slows.\n\r",ch);  
                       ch->power[DISC_VAMP_CELE] -= 2;
                       REMOVE_BIT(ch->extra, EXTRA_FLASH);
                    }   

                   }			
		  } 
                 
	    
	}
	break;
    }

    if ( time_info.day   >= 35 )
    {
	time_info.day = 0;
	time_info.month++;
    }

    if ( time_info.month >= 17 )
    {
	time_info.month = 0;
	time_info.year++;
    }

    /*
     * Weather change.
     */
    if ( time_info.month >= 9 && time_info.month <= 16 )
	diff = weather_info.mmhg >  985 ? -2 : 2;
    else
	diff = weather_info.mmhg > 1015 ? -2 : 2;

    weather_info.change   += diff * dice(1, 4) + dice(2, 6) - dice(2, 6);
    weather_info.change    = UMAX(weather_info.change, -12);
    weather_info.change    = UMIN(weather_info.change,  12);

    weather_info.mmhg += weather_info.change;
    weather_info.mmhg  = UMAX(weather_info.mmhg,  960);
    weather_info.mmhg  = UMIN(weather_info.mmhg, 1040);

    switch ( weather_info.sky )
    {
    default: 
	bug( "Weather_update: bad sky %d.", weather_info.sky );
	weather_info.sky = SKY_CLOUDLESS;
	break;

    case SKY_CLOUDLESS:
	if ( weather_info.mmhg <  990
	|| ( weather_info.mmhg < 1010 && number_bits( 2 ) == 0 ) )
	{
	    strcat( buf, "The sky is getting cloudy.\n\r" );
	    weather_info.sky = SKY_CLOUDY;
	}
	break;

    case SKY_CLOUDY:
	if ( weather_info.mmhg <  970
	|| ( weather_info.mmhg <  990 && number_bits( 2 ) == 0 ) )
	{
	    strcat( buf, "It starts to rain.\n\r" );
	    weather_info.sky = SKY_RAINING;
	}

	if ( weather_info.mmhg > 1030 && number_bits( 2 ) == 0 )
	{
	    strcat( buf, "The clouds disappear.\n\r" );
	    weather_info.sky = SKY_CLOUDLESS;
	}
	break;

    case SKY_RAINING:
	if ( weather_info.mmhg <  970 && number_bits( 2 ) == 0 )
	{
	    strcat( buf, "Lightning flashes in the sky.\n\r" );
	    weather_info.sky = SKY_LIGHTNING;
	}

	if ( weather_info.mmhg > 1030
	|| ( weather_info.mmhg > 1010 && number_bits( 2 ) == 0 ) )
	{
	    strcat( buf, "The rain stopped.\n\r" );
	    weather_info.sky = SKY_CLOUDY;
	}
	break;

    case SKY_LIGHTNING:
	if ( weather_info.mmhg > 1010
	|| ( weather_info.mmhg >  990 && number_bits( 2 ) == 0 ) )
	{
	    strcat( buf, "The lightning has stopped.\n\r" );
	    weather_info.sky = SKY_RAINING;
	    break;
	}
	break;
    }

    if ( buf[0] != '\0' )
    {
	for ( d = descriptor_list; d != NULL; d = d->next )
	{
	    if (( d->connected == CON_PLAYING
	    ||    d->connected == CON_EDITING )
	    &&   IS_OUTSIDE(d->character)
	    &&   IS_AWAKE(d->character) )
		send_to_char( buf, d->character );
	}
    }

    return;
}


int is_wall(EXIT_DATA *ex)
{

    if (IS_SET(ex->exit_info, EX_ICE_WALL)) return 1;
    if (IS_SET(ex->exit_info, EX_FIRE_WALL)) return 2;
    if (IS_SET(ex->exit_info, EX_SWORD_WALL)) return 3;
    if (IS_SET(ex->exit_info, EX_PRISMATIC_WALL)) return 4;
    if (IS_SET(ex->exit_info, EX_IRON_WALL)) return 5;
    if (IS_SET(ex->exit_info, EX_MUSHROOM_WALL)) return 6;
    if (IS_SET(ex->exit_info, EX_CALTROP_WALL)) return 7;
    if (IS_SET(ex->exit_info, EX_ASH_WALL)) return 8;
    return 0;
}

const char * wall[MAX_WALL+1] = 
{
    "","wall of ice", "wall of fire", "wall of swords",
	"prismatic wall", "wall or iron", "wall of mushrooms",
	"wall of caltrops", "wall of ash"
};

/*
 * Update all rooooooms, like gore, poison clouds etc....
 */
void room_update( void )
{
    int i;
    char buf[MAX_STRING_LENGTH];
    ROOM_INDEX_DATA *room;

    
    for ( room = room_list; room != NULL; room = room->next_room)
    {
        
        if (RTIMER(room, RTIMER_WALL_NORTH) == 1
	    && room->exit[DIR_NORTH] != NULL
	    && is_wall(room->exit[DIR_NORTH])!= 0)
        {
	    sprintf(buf, "The %s slowly fades away.", wall[is_wall(room->exit[DIR_NORTH])]);
	    room_message(room, buf);
	    make_wall(room, DIR_NORTH, 0);
        }

        if (RTIMER(room, RTIMER_WALL_SOUTH) == 1
	    && room->exit[DIR_SOUTH] != NULL
	    && is_wall(room->exit[DIR_SOUTH])!= 0)
        {
	    sprintf(buf, "The %s slowly fades away.", wall[is_wall(room->exit[DIR_SOUTH])]);
	    room_message(room, buf);
	    make_wall(room, DIR_SOUTH, 0);
        }

        if (RTIMER(room, RTIMER_WALL_EAST) == 1
	    && room->exit[DIR_EAST] != NULL
	    && is_wall(room->exit[DIR_EAST])!= 0)
        {
	    sprintf(buf, "The %s slowly fades away.", wall[is_wall(room->exit[DIR_EAST])]);
	    room_message(room, buf);
	    make_wall(room, DIR_EAST, 0);
        }

        if (RTIMER(room, RTIMER_WALL_WEST) == 1
	    && room->exit[DIR_WEST] != NULL
	    && is_wall(room->exit[DIR_WEST])!= 0)
        {
	    sprintf(buf, "The %s slowly fades away.", wall[is_wall(room->exit[DIR_WEST])]);
	    room_message(room, buf);
	    make_wall(room, DIR_WEST, 0);
        }

        if (RTIMER(room, RTIMER_WALL_UP) == 1
	    && room->exit[DIR_UP] != NULL
	    && is_wall(room->exit[DIR_UP])!= 0)
        {
	    sprintf(buf, "The %s slowly fades away.", wall[is_wall(room->exit[DIR_UP])]);
	    room_message(room, buf);
	    make_wall(room, DIR_UP, 0);
        }

        if (RTIMER(room, RTIMER_WALL_DOWN) == 1
	    && room->exit[DIR_DOWN] != NULL
	    && is_wall(room->exit[DIR_DOWN])!= 0)
        {
	    sprintf(buf, "The %s slowly fades away.", wall[is_wall(room->exit[DIR_DOWN])]);
	    room_message(room, buf);
	    make_wall(room, DIR_DOWN, 0);
        }


	if (RTIMER(room, RTIMER_STINKING_CLOUD) == 1)
	    room_message(room, "The poisonous vapours dissipate and clear.");

	if (RTIMER(room, RTIMER_HIDE_ROOM) == 1)
	    room_message(room, "The shroud leaves the room.");

	if (RTIMER(room, RTIMER_GHOST_LIGHT) == 2)
	    room_message(room, "The vapourous ghosts start howling insanely.");

	if (RTIMER(room, RTIMER_GHOST_LIGHT) == 3)
	    room_message(room, "The vapourous ghosts start moaning.");

	if (RTIMER(room, RTIMER_GHOST_LIGHT) == 1)
	    room_message(room, "The vapourous ghosts dissipate and vanish.");

	if (RTIMER(room, RTIMER_GLYPH_PROTECTION) == 1)
	    room_message(room, "The glyph of protection flares and vanishes.");

	if (RTIMER(room, RTIMER_SWARM_BEES) == 1)
	    room_message(room, "The bees fly away into the sky.");

	if (RTIMER(room, RTIMER_DISCORD) == 1)
	    room_message(room, "The banging and crashing stops.");

	if (RTIMER(room, RTIMER_SWARM_BATS) == 1)
	    room_message(room, "The bats flap away into the night.");

	if (RTIMER(room, RTIMER_SWARM_RATS) == 1)
	    room_message(room, "The rats scurry away into the floorboards.");
  
      if (RTIMER(room, RTIMER_SILENCE) == 1)
	    room_message(room, "The silence disappates.");

	for (i = 0 ; i < MAX_RTIMER ; i++)
	    room->tick_timer[i] = UMAX(room->tick_timer[i] - 1, 0);

    }

    return;

}



/*
 * Update all chars, including mobs.
 * This function is performance sensitive.
 */
void char_update( void )
{   
    CHAR_DATA *ch;
    CHAR_DATA *ch_next;
    CHAR_DATA *ch_save;
    CHAR_DATA *ch_quit;
    bool is_obj;
    bool drop_out = FALSE;
    time_t save_time;

    save_time	= current_time;
    ch_save	= NULL;
    ch_quit	= NULL;

    for ( ch = char_list; ch != NULL; ch = ch_next )
    {
	AFFECT_DATA *paf;
	AFFECT_DATA *paf_next;
	int i;
	ch_next = ch->next;
 

	if (!IS_NPC(ch) && (IS_HEAD(ch,LOST_HEAD) || IS_EXTRA(ch,EXTRA_OSWITCH)))
	    is_obj = TRUE;
	else if (!IS_NPC(ch) && ch->pcdata->obj_vnum != 0)
	    {is_obj = TRUE;SET_BIT(ch->extra, EXTRA_OSWITCH);}
	else
	    is_obj = FALSE;
	/*
	 * Find dude with oldest save time.
	 */
	if ( !IS_NPC(ch)
	&& ( ch->desc == NULL || ch->desc->connected == CON_PLAYING )
	&&   ch->level >= 2
	&&   ch->save_time < save_time )
	{
	    ch_save	= ch;
	    save_time	= ch->save_time;
	}
/*
	if (!IS_NPC(ch) && IS_CLASS(ch, CLASS_WEREWOLF) && !is_obj &&
	    ch->power[DISC_WERE_BEAR] > 3 && ch->position == POS_SLEEPING)
	{
	    if ( ch->hit  < ch->max_hit  ) ch->hit  = ch->max_hit;
	    if ( ch->mana < ch->max_mana ) ch->mana = ch->max_mana;
	    if ( ch->move < ch->max_move ) ch->move = ch->max_move;
	}
*/
	if (ch->fighting == NULL && !IS_NPC(ch)) 

	if ( !IS_NPC(ch) && IS_CLASS(ch, CLASS_WEREWOLF) 
	  && ch->gnosis[GMAXIMUM] > 0 && ch->gnosis[GMAXIMUM]
	   > ch->gnosis[GCURRENT] )
	{
	  if ( ch->position == POS_SLEEPING )
	    ch->gnosis[GCURRENT] += number_range(2,3);
	  else if ( ch->position == POS_RESTING )
	    ch->gnosis[GCURRENT] += number_range(1,3);
	  else if ( ch->position == POS_FIGHTING )
	    ch->gnosis[GCURRENT] += 0;
	  else
	    ch->gnosis[GCURRENT] += number_range( 1, 2 );
	}



	if ( IS_CLASS(ch, CLASS_VAMPIRE) && ch->beast > 0
	  && ch->pcdata->condition[COND_THIRST] <= 15 )
	{
	  act("You bare your fangs and scream in rage from lack of blood.",ch,NULL,NULL,TO_CHAR);
	  act("$n bares $s fangs and screams in rage.",ch,NULL,NULL,TO_ROOM);
	  do_berserk(ch,"");
	  do_beastlike(ch,"");
	}

	/* Character Tick Timers */

        if (ch->tick_timer[TIMER_CAN_CALL_WAR_HORSE] == 1)
	    send_to_char("You may now call your war horse again.\n\r", ch);

 	if (ch->tick_timer[TIMER_CAN_POLYMORPH] == 1)
	    send_to_char("You may now polymorph again.\n\r", ch);

 	if (ch->tick_timer[TIMER_MAKE_SNOWMAN] == 1)
	    send_to_char("You are ready to make another snowman now.\n\r", ch);

        if (ch->tick_timer[TIMER_CANMAJESTY] == 1)
            stc("You can once again use majesty.\n\r",ch);

	if (IS_CLASS(ch, CLASS_NINJA) && ch->pcdata->powers[NPOWER_CHIKYU] >= 6
	   && ch->pcdata->powers[HARA_KIRI] > 0) {
		if (ch->pcdata->powers[HARA_KIRI] == 1)
		send_to_char("Your HaraKiri wears off.\n\r", ch );
		ch->pcdata->powers[HARA_KIRI]--;	

	}

        if (ch->tick_timer[TIMER_MAJESTY] == 1)
        {
	    REMOVE_BIT(ch->affected_by, AFF_PEACE);
	    stc("You feel less imposing.\n\r",ch);
	    act("$n looks less imposing.",ch,NULL,NULL,TO_ROOM);
        }

 	if (ch->tick_timer[TIMER_SPHINX_ROAR] == 1)
	    send_to_char("You are ready to roar again.\n\r", ch);

 	if (ch->tick_timer[TIMER_NEWBIE_IMM] == 1)
	    send_to_char("You are no longer protected by the gods.\n\r", ch);

 	if (ch->tick_timer[TIMER_CAN_DO_NEXUS] == 1)
	    send_to_char("You may now create another nexus.\n\r", ch);

        if (ch->tick_timer[TIMER_INFERNO] == 1)
	    stc("You are able to use Inferno again.\n\r",ch);

 	if (ch->tick_timer[TIMER_CAN_SHRIEK] == 1)
	    send_to_char("Your voice has recovered.\n\r", ch);

 	if (ch->tick_timer[TIMER_CAN_CALL_ROCKS] == 1)
	    send_to_char("You may now call the Bane.\n\r", ch);

 	if (ch->tick_timer[TIMER_HELLFIRE_SUMMON] == 1)
	    send_to_char("You may now summon hellfire once more.\n\r", ch);

 	if (ch->tick_timer[TIMER_ENTOMB] == 1)
	    send_to_char("You may now use the entomb ability again.\n\r", ch);

 	if (ch->tick_timer[TIMER_CAN_BREATHE_FROST] == 1)
	    send_to_char("You are ready to breathe frost again.\n\r", ch);

 	if (ch->tick_timer[TIMER_CAN_FEATHER] == 1)
	    send_to_char("You are ready to make another feather.\n\r", ch);

	    if (ch->tick_timer[TIMER_THIRD_ARM_GROWING] == 1)
	    {
	 	send_to_char("A third arm bursts out of your stomach.\n\r", ch);
		act("An arm bursts out of $n's stomach, spraying blood everywhere.", ch, NULL, NULL, TO_ROOM);
		SET_BIT(ch->newbits, THIRD_HAND);
	    }

	    if (ch->tick_timer[TIMER_FOURTH_ARM_GROWING] == 1)
	    {
	 	send_to_char("Another arm bursts out of your stomach.\n\r", ch);
		act("An arm bursts out of $n's stomach, spraying blood everywhere.", ch, NULL, NULL, TO_ROOM);
		SET_BIT(ch->newbits, FOURTH_HAND);
	    }

	for (i = 0; i < MAX_TIMER; i++)
	    if (ch->tick_timer[i] > 0) ch->tick_timer[i] -= 1;

	if (ch->fighting == NULL
	&& !IS_SET(ch->newbits, NEW_CLOAK)
	&& !IS_NPC(ch)
	&& !is_obj
	&& IS_CLASS(ch, CLASS_MONK)
	&& ch->pcdata->powers[PMONK] > 10)
	{
	   SET_BIT(ch->newbits, NEW_CLOAK);
	   send_to_char("Your Cloak of Life is restored.\n\r",ch);
	}


	if ( ch->position > POS_STUNNED && !is_obj)
	{
	    if ( ch->hit  < ch->max_hit )
		ch->hit  += hit_gain(ch);

	    if ( ch->mana < ch->max_mana )
		ch->mana += mana_gain(ch);

	    if ( ch->move < ch->max_move )
		ch->move += move_gain(ch);
	}

	if ( ch->position == POS_STUNNED && !is_obj)
	{
            ch->hit = ch->hit + number_range(2,4);
	    update_pos( ch );
/*
            if (ch->position > POS_STUNNED)
            {
                act( "$n clambers back to $s feet.", ch, NULL, NULL, TO_ROOM );
                act( "You clamber back to your feet.", ch, NULL, NULL, TO_CHAR );
            }
*/
	}

	if ( !IS_NPC(ch) && ch->level < LEVEL_IMMORTAL && !is_obj)
	{
	    OBJ_DATA *obj;
	    int blood;

	    if ( ( ( obj = get_eq_char( ch, WEAR_WIELD ) ) != NULL
	    &&   obj->item_type == ITEM_LIGHT
	    &&   obj->value[2] > 0 )
	    || ( ( obj = get_eq_char( ch, WEAR_HOLD ) ) != NULL
	    &&   obj->item_type == ITEM_LIGHT
	    &&   obj->value[2] > 0 ) )
	    {
		if ( --obj->value[2] == 0 && ch->in_room != NULL )
		{
		    --ch->in_room->light;
		    act( "$p goes out.", ch, obj, NULL, TO_ROOM );
		    act( "$p goes out.", ch, obj, NULL, TO_CHAR );
		    extract_obj( obj );
		}
	    }

	    if ( ++ch->timer >= 12 )
	    {
		if ( ch->was_in_room == NULL && ch->in_room != NULL )
		{
		    ch->was_in_room = ch->in_room;
		    if ( ch->fighting != NULL )
			stop_fighting( ch, TRUE );
		    act( "$n disappears into the void.",
			ch, NULL, NULL, TO_ROOM );
		    send_to_char( "You disappear into the void.\n\r", ch );
		    save_char_obj( ch );
		    char_from_room( ch );
		    char_to_room( ch, get_room_index( ROOM_VNUM_LIMBO ) );
		}
	    }

	    if ( ch->timer > 30 ) ch_quit = ch;

	    gain_condition( ch, COND_DRUNK,  -1 );
	    if (!IS_CLASS(ch, CLASS_VAMPIRE))
	    {
	    	gain_condition( ch, COND_FULL, -1 );
	    	gain_condition( ch, COND_THIRST, -1 );
	    }
	    else
	    {
		blood = -1;
		if (ch->beast > 0)
		{
		    if (IS_VAMPAFF(ch, VAM_CLAWS)) blood -= number_range(1,3);
		    if (IS_VAMPAFF(ch, VAM_FANGS)) blood -= 1;
		    if (IS_VAMPAFF(ch, VAM_NIGHTSIGHT)) blood -= 1;
		    if (IS_VAMPAFF(ch, AFF_SHADOWSIGHT)) blood -= number_range(1,3);
		    if (IS_SET(ch->act, PLR_HOLYLIGHT)) blood -= number_range(1,5);
		    if (IS_VAMPAFF(ch, VAM_DISGUISED)) blood -= number_range(5,10);
		    if (IS_VAMPAFF(ch, VAM_CHANGED)) blood -= number_range(5,10);
		    if (IS_VAMPAFF(ch, IMM_SHIELDED)) blood -= number_range(1,3);
		    if (IS_POLYAFF(ch, POLY_SERPENT)) blood -= number_range(1,3);
		}
               ch->pcdata->condition[COND_THIRST] += blood;
               
               if (ch->pcdata->condition[COND_THIRST] <= 0)
               {
                ch->pcdata->condition[COND_THIRST] = 0;
               } 
	    }
	}

	for ( paf = ch->affected; paf != NULL; paf = paf_next )
	{
	    paf_next	= paf->next;
	    if ( paf->duration > 0 )
		paf->duration--;
	    else if ( paf->duration < 0 )
		;
	    else
	    {
		if ( paf_next == NULL
		||   paf_next->type != paf->type
		||   paf_next->duration > 0 )
		{
		    if ( paf->type > 0 && skill_table[paf->type].msg_off && !is_obj)
		    {
			send_to_char( skill_table[paf->type].msg_off, ch );
			send_to_char( "\n\r", ch );
		    }
		}
	  
		affect_remove( ch, paf );
	    }
	}

	/*
	 * Careful with the damages here,
	 *   MUST NOT refer to ch after damage taken,
	 *   as it may be lethal damage (on NPC).
	 */
	if ( ch->loc_hp[6] > 0 && !is_obj && ch->in_room != NULL )
	{
	    int dam = 0;
	    int minhit = 0;
	    if (!IS_NPC(ch)) minhit = -11;
	    if (IS_BLEEDING(ch,BLEEDING_HEAD) && (ch->hit-dam) > minhit )
	    {
		act( "A spray of blood shoots from the stump of $n's neck.", ch, NULL, NULL, TO_ROOM );
		send_to_char( "A spray of blood shoots from the stump of your neck.\n\r", ch );
		dam += number_range(20,50);
	    }
	    if (IS_BLEEDING(ch,BLEEDING_THROAT) && (ch->hit-dam) > minhit )
	    {
		act( "Blood pours from the slash in $n's throat.", ch, NULL, NULL, TO_ROOM );
		send_to_char( "Blood pours from the slash in your throat.\n\r", ch );
		dam += number_range(10,20);
	    }
	    if (IS_BLEEDING(ch,BLEEDING_ARM_L) && (ch->hit-dam) > minhit )
	    {
		act( "A spray of blood shoots from the stump of $n's left arm.", ch, NULL, NULL, TO_ROOM );
		send_to_char( "A spray of blood shoots from the stump of your left arm.\n\r", ch );
		dam += number_range(10,20);
	    }
	    else if (IS_BLEEDING(ch,BLEEDING_HAND_L) && (ch->hit-dam) > minhit )
	    {
		act( "A spray of blood shoots from the stump of $n's left wrist.", ch, NULL, NULL, TO_ROOM );
		send_to_char( "A spray of blood shoots from the stump of your left wrist.\n\r", ch );
		dam += number_range(5,10);
	    }
	    if (IS_BLEEDING(ch,BLEEDING_ARM_R) && (ch->hit-dam) > minhit )
	    {
		act( "A spray of blood shoots from the stump of $n's right arm.", ch, NULL, NULL, TO_ROOM );
		send_to_char( "A spray of blood shoots from the stump of your right arm.\n\r", ch );
		dam += number_range(10,20);
	    }
	    else if (IS_BLEEDING(ch,BLEEDING_HAND_R) && (ch->hit-dam) > minhit )
	    {
		act( "A spray of blood shoots from the stump of $n's right wrist.", ch, NULL, NULL, TO_ROOM );
		send_to_char( "A spray of blood shoots from the stump of your right wrist.\n\r", ch );
		dam += number_range(5,10);
	    }
	    if (IS_BLEEDING(ch,BLEEDING_LEG_L) && (ch->hit-dam) > minhit )
	    {
		act( "A spray of blood shoots from the stump of $n's left leg.", ch, NULL, NULL, TO_ROOM );
		send_to_char( "A spray of blood shoots from the stump of your left leg.\n\r", ch );
		dam += number_range(10,20);
	    }
	    else if (IS_BLEEDING(ch,BLEEDING_FOOT_L) && (ch->hit-dam) > minhit )
	    {
		act( "A spray of blood shoots from the stump of $n's left ankle.", ch, NULL, NULL, TO_ROOM );
		send_to_char( "A spray of blood shoots from the stump of your left ankle.\n\r", ch );
		dam += number_range(5,10);
	    }
	    if (IS_BLEEDING(ch,BLEEDING_LEG_R) && (ch->hit-dam) > minhit )
	    {
		act( "A spray of blood shoots from the stump of $n's right leg.", ch, NULL, NULL, TO_ROOM );
		send_to_char( "A spray of blood shoots from the stump of your right leg.\n\r", ch );
		dam += number_range(10,20);
	    }
	    else if (IS_BLEEDING(ch,BLEEDING_FOOT_R) && (ch->hit-dam) > minhit )
	    {
		act( "A spray of blood shoots from the stump of $n's right ankle.", ch, NULL, NULL, TO_ROOM );
		send_to_char( "A spray of blood shoots from the stump of your right ankle.\n\r", ch );
		dam += number_range(5,10);
	    }
	    if (IS_HERO(ch)) { ch->hit = ch->hit - dam;
		if (ch->hit < 1) ch->hit = 1; }
	    else ch->hit = ch->hit - dam;
	    update_pos(ch);
	    ch->in_room->blood += dam;
	    if (ch->in_room->blood > 1000) ch->in_room->blood = 1000;
	    if (ch->hit <=-11 || (IS_NPC(ch) && ch->hit < 1))
	    {
		do_killperson(ch,ch->name);
		drop_out = TRUE;
	    }
	}
	if (IS_EXTRA(ch,EXTRA_ROT) && !is_obj && !drop_out)
	{
	 int dam;
	 if (IS_NPC(ch)) break;
         act( "$n's flesh shrivels and tears.", ch, NULL, NULL, TO_ROOM);
	 send_to_char( "Your flesh shrivels and tears.\n\r", ch);
         dam = number_range(250,500);
         ch->hit = ch->hit - dam;
         update_pos(ch);
         if (ch->hit < -10)
         {
            do_killperson(ch,ch->name);
            drop_out = TRUE;
         }
        }
/*
	if (IS_SET(ch->in_room->room_flags,ROOM_FLAMING) && !IS_AFFECTED(ch,AFF_FLAMING))
        {
        send_to_char("You catch on fire!\n\r",ch);
        SET_BIT(ch->affected_by, AFF_FLAMING);
        sprintf(buf,"%s catches on fire!\n\r",ch->name);
        act(buf,ch,NULL,NULL,TO_ROOM);
        }
*/

     if ( IS_AFFECTED(ch, AFF_FLAMING) && !is_obj && !drop_out && ch->in_room != NULL )
	{
	    int dam;
	    if (IS_NPC(ch)) break;
	    act( "$n's flesh burns and crisps.", ch, NULL, NULL, TO_ROOM );
	    send_to_char( "Your flesh burns and crisps.\n\r", ch );
	    dam = number_range(250,300);
	    ch->hit = ch->hit - dam;
	    update_pos(ch);
	    if (ch->hit <=-9)
	    {
		update_pos(ch);
		drop_out = TRUE;
	    }
	}
	else if ( IS_CLASS(ch, CLASS_VAMPIRE) && (!IS_AFFECTED(ch,AFF_SHADOWPLANE)) &&
	    (!IS_NPC(ch) && !IS_IMMUNE(ch,IMM_SUNLIGHT)) && ch->in_room != NULL &&
	    (!ch->in_room->sector_type == SECT_INSIDE) && !is_obj &&
	    (!room_is_dark(ch->in_room)) && (weather_info.sunlight != SUN_DARK) )
	{
	    act( "$n's flesh smolders in the sunlight!", ch, NULL, NULL, TO_ROOM );
	    send_to_char( "Your flesh smolders in the sunlight!\n\r", ch );
	    /* This one's to keep Zarkas quiet ;) */
	    if (IS_POLYAFF(ch, POLY_SERPENT))
	    	ch->hit = ch->hit - number_range(2,4);
	    else
	    	ch->hit = ch->hit - number_range(5,10);
	    update_pos(ch);
	    if (ch->hit <=-11)
	    {
		do_killperson(ch,ch->name);
		drop_out = TRUE;
	    }
	}
	else if ( IS_AFFECTED(ch, AFF_POISON) && !is_obj && !drop_out )
	{
	    act( "$n shivers and suffers.", ch, NULL, NULL, TO_ROOM );
	    send_to_char( "You shiver and suffer.\n\r", ch );
	    damage( ch, ch, 2, gsn_poison );
	}/*
	else if ( !IS_NPC( ch ) && ch->paradox[1] > 0 )
	{
	    if ( ch->paradox[1] > 50 ) paradox( ch );
	    else if ( ch->paradox[2] == 0 && ch->paradox[1] > 0 )
	    {
		ch->paradox[1] --;
		ch->paradox[2] = PARADOX_TICK;
	    }
	    else ch->paradox[3] --;
	}*/
	else if ( ch->position == POS_INCAP && !is_obj && !drop_out )
	{
	    if (ch->level > 0)
                ch->hit = ch->hit + number_range(2,4);
	    else
                ch->hit = ch->hit - number_range(1,2);
	    update_pos( ch );
            if (ch->position > POS_INCAP)
            {
                act( "$n's wounds stop bleeding and seal up.", ch, NULL, NULL, TO_ROOM );
                send_to_char( "Your wounds stop bleeding and seal up.\n\r", ch );
            }
            if (ch->position > POS_STUNNED)
            {
                act( "$n clambers back to $s feet.", ch, NULL, NULL, TO_ROOM );
                send_to_char( "You clamber back to your feet.\n\r", ch );
            }
	}
	else if ( ch->position == POS_MORTAL && !is_obj && !drop_out )
	{
	    drop_out = FALSE;
	   /* if (ch->level > 0)*/
                ch->hit = ch->hit + number_range(2,4);
/*	    else
	    {
                ch->hit = ch->hit - number_range(1,2);
		if (!IS_NPC(ch) && (ch->hit <=-11))
		    do_killperson(ch,ch->name);
		drop_out = TRUE;
	    }*/
	    if (!drop_out)
	    {
	    	update_pos( ch );
            	if (ch->position == POS_INCAP)
            	{
                    act( "$n's wounds begin to close, and $s bones pop back into place.", ch, NULL, NULL, TO_ROOM );
                    send_to_char( "Your wounds begin to close, and your bones pop back into place.\n\r", ch );
            	}
	    }
	}
	else if ( ch->position == POS_DEAD && !is_obj && !drop_out )
	{
	   update_pos(ch);
	   if (!IS_NPC(ch))
		do_killperson(ch,ch->name);
	}
	drop_out = FALSE;
    }

    /*
     * Autosave and autoquit.
     * Check that these chars still exist.
     */
    if ( ch_save != NULL || ch_quit != NULL )
    {
	for ( ch = char_list; ch != NULL; ch = ch_next )
	{
	    ch_next = ch->next;
	    if ( ch == ch_save )
		save_char_obj( ch );
	    if ( ch == ch_quit )
		do_quit( ch, "" );
	}
    }

    return;
}



/*
 * Update all objs.
 * This function is performance sensitive.
 */
void obj_update( void )
{   
    OBJ_DATA *obj;
    OBJ_DATA *obj_next;

    for ( obj = object_list; obj != NULL; obj = obj_next )
    {
	CHAR_DATA *rch;
	char *message;

	obj_next = obj->next;
	if ( obj->timer <= 0 || --obj->timer > 0 )
	    continue;

	switch ( obj->item_type )
	{
	default:              message = "$p vanishes.";         	break;
	case ITEM_FOUNTAIN:   message = "$p dries up.";         	break;
	case ITEM_CORPSE_NPC: message = "$p decays into dust."; 	break;
	case ITEM_CORPSE_PC:  message = "$p decays into dust."; 	break;
	case ITEM_FOOD:       message = "$p decomposes.";		break;
	case ITEM_TRASH:      message = "$p crumbles into dust.";	break;
	case ITEM_EGG:        message = "$p cracks open.";		break;
	case ITEM_WEAPON:     message = "$p turns to fine dust and blows away."; break;
        case ITEM_WALL:       message = "$p flows back into the ground.";break;
	}

	if ( obj->carried_by != NULL && !IS_OBJ_STAT2(obj, ITEM_DAEMONSEED) )
	{
	    act( message, obj->carried_by, obj, NULL, TO_CHAR );
	}
	else if ( obj->in_room != NULL
	&&      ( rch = obj->in_room->people ) != NULL && !IS_OBJ_STAT2(obj, ITEM_DAEMONSEED) )
	{
	    act( message, rch, obj, NULL, TO_ROOM );
	    act( message, rch, obj, NULL, TO_CHAR );
	}

	/* If the item is an egg, we need to create a mob and shell!
	 * KaVir
	 */
	if ( obj->item_type == ITEM_EGG && !IS_OBJ_STAT2(obj, ITEM_DAEMONSEED))
	{
	    CHAR_DATA      *creature;
	    OBJ_DATA       *egg;
	    if ( get_mob_index( obj->value[0] ) != NULL )
	    {
		if (obj->carried_by != NULL && obj->carried_by->in_room != NULL)
		{
	    	    creature = create_mobile( get_mob_index( obj->value[0] ) );
		    char_to_room(creature,obj->carried_by->in_room);
		}
		else if (obj->in_room != NULL)
		{
	    	    creature = create_mobile( get_mob_index( obj->value[0] ) );
		    char_to_room(creature,obj->in_room);
		}
		else
		{
	    	    creature = create_mobile( get_mob_index( obj->value[0] ) );
		    char_to_room(creature,get_room_index(ROOM_VNUM_HELL));
		}
	    	egg = create_object( get_obj_index( OBJ_VNUM_EMPTY_EGG ), 0 );
	    	egg->timer = 2;
	    	obj_to_room( egg, creature->in_room );
	    	act( "$n clambers out of $p.", creature, obj, NULL, TO_ROOM );
	    }
	    else if (obj->in_room != NULL)
	    {
	    	egg = create_object( get_obj_index( OBJ_VNUM_EMPTY_EGG ), 0 );
	    	egg->timer = 2;
	    	obj_to_room( egg, obj->in_room );
	    }
	}

        if (IS_OBJ_STAT2(obj, ITEM_DAEMONSEED)
	    && obj != NULL
	    && obj->in_obj == NULL
	    && (locate_obj(obj))->people )
	{
	    char buf[MAX_STRING_LENGTH];
	    CHAR_DATA *vch;
	    int wdam;

	    sprintf(buf, "%s suddenly explodes in a ball of flame, incinerating you!\n\r",obj->short_descr);
	    buf[0] = UPPER(buf[0]);
	    if ((locate_obj(obj))->people == NULL) break;
	    for ( vch = (locate_obj(obj))->people; vch != NULL;vch=vch->next_in_room )
    	    {
		if (vch->class == 0 || (!IS_NPC(vch) && vch->level < 3)) continue;
	    if ( IS_SET(vch->in_room->room_flags,ROOM_SAFE) )
	    {
		stc("You are unaffected by the blast.\n\r",vch);
		continue;
	    }
	    wdam = obj->level + dice(12,50);
            damage(vch,vch,obj->level+ dice(12, 50),gsn_inferno); 
		send_to_char(buf, vch);
	    sprintf(buf,"The flames strike you incredibly hard![%d]\n\r",wdam);
		stc(buf,vch);
 	    }
	}

	if (obj != NULL) extract_obj( obj );
    }

    return;
}



/*
 * Aggress.
 *
 * for each mortal PC
 *     for each mob in room
 *         aggress on some random PC
 *
 * This function takes 25% to 35% of ALL Merc cpu time.
 * Unfortunately, checking on each PC move is too tricky,
 *   because we don't want the mob to just attack the first PC
 *   who leads the party into the room.
 *
 * -- Furey
 */
void aggr_update( void )
{
    CHAR_DATA *wch;
    CHAR_DATA *wch_next;
    CHAR_DATA *ch;
    CHAR_DATA *ch_next;
    CHAR_DATA *vch;
    CHAR_DATA *vch_next;
    CHAR_DATA *victim = NULL;

    OBJ_DATA *obj = NULL;
    OBJ_DATA *chobj = NULL;
    ROOM_INDEX_DATA *objroom = NULL;
    DESCRIPTOR_DATA *d;

    for ( d = descriptor_list; d != NULL; d = d->next )
    {
/*
	if ( d->connected == CON_PLAYING
	&& ( ch = d->character ) != NULL
	&&   !IS_NPC(ch)
	&&   ch->pcdata->stage[1] > 0
	&& ( victim = ch->pcdata->partner ) != NULL
	&&   !IS_NPC(victim)
	&&   ch->in_room != NULL
	&&   victim->in_room != NULL
	&&   victim->in_room != ch->in_room )
	{
	    ch->pcdata->stage[1] = 0;
	    victim->pcdata->stage[1] = 0;
	}
*/
	if (( d->connected == CON_PLAYING
	||    d->connected == CON_EDITING )
	&& ( ch = d->character ) != NULL
	&&   !IS_NPC(ch)
	&&   ch->pcdata != NULL
	&& ( obj = ch->pcdata->chobj ) != NULL )
	{
	    if (obj->in_room != NULL)
	    	objroom = obj->in_room;
	    else if (obj->in_obj != NULL)
	    	objroom = get_room_index(ROOM_VNUM_IN_OBJECT);
	    else if (obj->carried_by != NULL)
	    {
		if (obj->carried_by != ch && obj->carried_by->in_room != NULL)
		    objroom = obj->carried_by->in_room;
		else continue;
	    }
	    else continue;
	    if (ch->in_room != objroom && objroom != NULL)
	    {
	    	char_from_room(ch);
	    	char_to_room(ch,objroom);
		do_look(ch,"auto");
	    }
	}
	else if (( d->connected == CON_PLAYING
	||         d->connected == CON_EDITING )
	&& ( ch = d->character ) != NULL
	&&   !IS_NPC(ch)
	&&   ch->pcdata != NULL
	&&   (IS_HEAD(ch,LOST_HEAD) || IS_EXTRA(ch,EXTRA_OSWITCH) || ch->pcdata->obj_vnum != 0) )
	{
	    if (ch->pcdata->obj_vnum != 0)
	    {
		bind_char(ch);
		continue;
	    }
	    if (IS_HEAD(ch,LOST_HEAD))
	    {
	    	REMOVE_BIT(ch->loc_hp[0],LOST_HEAD);
	    	send_to_char("You are able to regain a body.\n\r",ch);
	    	ch->position = POS_RESTING;
	    	ch->hit = 1;
	    } else {
	    	send_to_char("You return to your body.\n\r",ch);
		REMOVE_BIT(ch->extra,EXTRA_OSWITCH);}
	    REMOVE_BIT(ch->affected_by,AFF_POLYMORPH);
	    free_string(ch->morph);
	    ch->morph = str_dup("");
	    char_from_room(ch);
	    char_to_room(ch,get_room_index(ROOM_VNUM_ALTAR));
	    if ( ( chobj = ch->pcdata->chobj ) != NULL )
		chobj->chobj = NULL;
	    ch->pcdata->chobj = NULL;
	    do_look(ch,"auto");
	}
	continue;
    }

    for ( wch = char_list; wch != NULL; wch = wch_next )
    {
	wch_next = wch->next;
	if ( IS_NPC(wch)
	|| ( wch->desc != NULL && wch->desc->connected != CON_PLAYING && wch->desc->connected != CON_EDITING )
	||   wch->position <= POS_STUNNED
	||   wch->level >= LEVEL_IMMORTAL
	||   wch->pcdata == NULL
	|| ( ( chobj = wch->pcdata->chobj ) != NULL )
	||   wch->in_room == NULL )
	    continue;

	if ( (IS_CLASS(wch, CLASS_WEREWOLF) && 
wch->power[DISC_WERE_LYNX] > 1 &&IS_HERO(wch)) ||
	    IS_ITEMAFF(wch, ITEMA_STALKER) || (IS_CLASS(wch, CLASS_NINJA) &&
wch->pcdata->powers[NPOWER_CHIKYU] >= 4))
	{
	    if ( wch->hunting != NULL && wch->hunting != '\0' && 
		strlen(wch->hunting) > 1 )
	    {
		ROOM_INDEX_DATA *old_room = wch->in_room;
		check_hunt( wch );
		if (wch->in_room == old_room)
		{
		    free_string(wch->hunting);
		    wch->hunting = str_dup( "" );
		    continue;
		}
		check_hunt( wch );
		if (wch->in_room == old_room)
		{
		    free_string(wch->hunting);
		    wch->hunting = str_dup( "" );
		}
		continue;
	    }
	}

	for ( ch = wch->in_room->people; ch != NULL; ch = ch_next )
	{
	    int count;

	    ch_next	= ch->next_in_room;

	    if ( !IS_NPC(ch)
	    ||   !IS_SET(ch->act, ACT_AGGRESSIVE)
	    ||   no_attack(ch, wch)
	    ||   ch->fighting != NULL
	    ||   IS_AFFECTED(ch, AFF_CHARM)
	    ||   !IS_AWAKE(ch)
	    ||   ( IS_SET(ch->act, ACT_WIMPY) && IS_AWAKE(wch) )
	    ||   !can_see( ch, wch ) )
		continue;

	    /*
	     * Ok we have a 'wch' player character and a 'ch' npc aggressor.
	     * Now make the aggressor fight a RANDOM pc victim in the room,
	     *   giving each 'vch' an equal chance of selection.
	     */
	    count	= 0;
	    victim	= NULL;
	    for ( vch = wch->in_room->people; vch != NULL; vch = vch_next )
	    {
		vch_next = vch->next_in_room;

		if ( !IS_NPC(vch)
		&&   !no_attack(ch, vch)
		&&   vch->pcdata != NULL
		&& ( ( chobj = vch->pcdata->chobj ) == NULL )
		&&   vch->level < LEVEL_IMMORTAL
		&&   vch->position > POS_STUNNED
		&&   ( !IS_SET(ch->act, ACT_WIMPY) || !IS_AWAKE(vch) )
		&&   can_see( ch, vch ) )
		{
		    if ( number_range( 0, count ) == 0 )
			victim = vch;
		    count++;
		}
	    }

	    if ( victim == NULL )
	    {
/*
		bug( "Aggr_update: null victim attempt by mob %d.", ch->pIndexData->vnum );
*/
		continue;
	    }

	    multi_hit( ch, victim, TYPE_UNDEFINED );
	}
    }

    return;
}


 
void embrace_update( void )
{
    DESCRIPTOR_DATA *d;
    CHAR_DATA       *ch;
    CHAR_DATA       *victim;
    int blpr;  /* variable to check for amout of blood sucked. Shakti */
    char buf[MAX_STRING_LENGTH];

    for ( d = descriptor_list; d != NULL; d = d->next )
    {
      
      if ( ( ch = d->character) == NULL )
	continue;

// Too lazy to make another update for monk stuff
// So I'm cheating and using embrace for monks. flaf.

      if ( IS_CLASS(ch, CLASS_MONK) && ch->fighting == NULL )
	ch->monkblock = 0;

      if ( !IS_NPC(ch) && IS_CLASS(ch, CLASS_MONK)
          && ch->focus[MAXIMUM] > 0 && ch->focus[MAXIMUM]
           > ch->focus[CURRENT] )
        {
          if ( ch->position == POS_SLEEPING )
            ch->focus[CURRENT] += number_range(2,3);
          else if ( ch->position == POS_MEDITATING )
            ch->focus[CURRENT] += number_range( 4, 5 );
          else if ( ch->position == POS_RESTING )
            ch->focus[CURRENT] += number_range(1,2);
          else if ( ch->position == POS_FIGHTING )
            ch->focus[CURRENT] += 0;
          else
            ch->focus[CURRENT] += number_range( 1, 2 );
        }

      if ( IS_NPC(ch) || ch->embracing == NULL )
        continue;

	if (!IS_CLASS(ch,CLASS_VAMPIRE))
	  stop_embrace(ch,NULL);

	victim=ch->embracing;

/* Fix for embracing mobs by Shakti.					*/

	if (IS_NPC(victim) )
	{
		if (victim->level > 75 )
		{
			stop_embrace(ch,victim);
			send_to_char("You cannot embrace someone so powerful!\n\r",ch);
			return;
		}
		
	/* To keep how much blood was sucked, and how much gained the same. Shakti */
	
		(blpr =number_range(30,40));
		victim->practice -=blpr;
		ch->pcdata->condition[COND_THIRST] +=blpr;
		
		sprintf(buf, "%s shudders in ecstacy as he drinks blood from %s's neck.\n\r", ch->name,victim->short_descr);
	    act(buf,ch,NULL,NULL,TO_ROOM);
	    sprintf(buf, "You shudder in ecstacy as you drink blood from %s's neck.\n\r",victim->short_descr);
	    send_to_char(buf,ch);
	    
		if (victim->practice < 0)
		{
			victim->practice = 0;
		}

		if (victim->practice == 0)
		
		{
	     raw_kill(victim);
	     sprintf(buf,"%s's body falls lifless to the ground!.\n\r",victim->short_descr);
	     act(buf,ch,NULL,NULL,TO_ROOM);
	     send_to_char(buf,ch);
	     stop_embrace(ch,victim);
		 return;
		}

		if ( ch->pcdata->condition[COND_THIRST] > (3000 /
ch->generation))
		{
	 	  ch->pcdata->condition[COND_THIRST] = (3000 /
ch->generation);
		  stc("Your bloodlust is sated.\n\r",ch);
		}

		continue;
	}



	if (get_char_world(ch,victim->name) == NULL)
	  {stop_embrace(ch,NULL);continue;}
	if (ch->in_room != victim->in_room)
	  stop_embrace(ch,victim);
	if (victim->pcdata->condition[COND_THIRST] < 0)
	  victim->pcdata->condition[COND_THIRST] = 0;
       
	ch->pcdata->condition[COND_THIRST] += number_range(30,40);
    sprintf(buf, "%s shudders in ecstacy as he drinks blood from %s's neck.\n\r", ch->name,victim->name);
	act(buf,ch,NULL,NULL,TO_ROOM);
	sprintf(buf, "You shudder in ecstacy as you drink blood from %s's neck.\n\r",victim->name);
	send_to_char(buf,ch);
	sprintf(buf, "You feel some of your life slip away as %s drinks from your neck.\n\r", ch->name);
	send_to_char(buf,victim);
	ch->pcdata->condition[COND_THIRST] += number_range(35,40);
	victim->pcdata->condition[COND_THIRST] -= number_range(40,42);
	if (ch->pcdata->condition[COND_THIRST] > 3000/ch->generation)
	{ch->pcdata->condition[COND_THIRST]=3000/ch->generation;
	send_to_char("Your bloodlust is sated.\n\r",ch);}
	if (victim->pcdata->condition[COND_THIRST] < 0)
	  victim->pcdata->condition[COND_THIRST]=0;
	if (victim->pcdata->condition[COND_THIRST] ==0)
	{
	  sprintf(buf,"You have been diablerized!\n\r");
	  send_to_char(buf,victim);
	  raw_kill(victim);
	  sprintf(buf,"%s's body falls lifeless to the ground.\n\r",victim->name);
	  act(buf,ch,NULL,NULL,TO_ROOM);
	  send_to_char(buf,ch);
	  stop_embrace(ch,victim);
     	  if (victim->generation < ch->generation && ch->generation > 3)
  	  {
	    sprintf(buf,"%s has been diablerized by %s.",victim->name,ch->name);
	    ch->generation -= 1;
	    victim->generation += 1;
	    victim->pcdata->condition[COND_THIRST] = 0;
	    do_info(ch,buf);
	  }
	  else
  	  {
	    sprintf(buf,"%s has been diablerized by %s for no generation.", victim->name,ch->name);
	    victim->pcdata->condition[COND_THIRST] = 0;
	    do_info(ch,buf);
	  }
    }
    continue; 
  }
  return;
}

void ww_update( void )
{
    DESCRIPTOR_DATA *d;
    CHAR_DATA       *victim;
    float            dam = 0;

    for ( d = descriptor_list; d != NULL; d = d->next )
      {
      if (!IS_PLAYING(d) || (victim = d->character) == NULL
        || IS_NPC(victim) || IS_IMMORTAL(victim)
	|| victim->in_room == NULL || victim->pcdata->chobj != NULL
        || IS_CLASS(victim,CLASS_WEREWOLF))
	{
            continue;
        }
      if ( !IS_SET( d->character->in_room->room_flags, ROOM_BLADE_BARRIER ) )
        continue;

      act( "The scattered blades on the ground fly up into the air ripping into you.", d->character, NULL, NULL, TO_CHAR );
      act( "The scattered blades on the ground fly up into the air ripping into $n.", d->character, NULL, NULL, TO_ROOM );

      act( "The blades drop to the ground inert.", d->character, NULL, NULL, TO_CHAR );
      act( "The blades drop to the ground inert.", d->character, NULL, NULL, TO_ROOM );

      dam = number_range( 7, 14 );
      dam = dam / 100;
      dam = d->character->hit * dam;
      if ( dam < 100 ) dam = 100;
      d->character->hit = d->character->hit - dam;
      if ( d->character->hit < -10 ) d->character->hit = -10;
      update_pos( victim );
      }

    return;
}


/*
 * Handle all kinds of updates.
 * Called once per pulse from game loop.
 * Random times to defeat tick-timing clients and players.
 */
void update_handler( void )
{
    static  int     pulse_area;
    static  int     pulse_mobile;
    static  int     pulse_violence;
    static  int     pulse_point;
    static  int     pulse_ww;
    static  int     pulse_embrace;
    static int pulse_db_dump;   /* OLC 1.1b */

    /* OLC 1.1b */
    if ( --pulse_db_dump  <= 0 )
    {
        pulse_db_dump   = PULSE_DB_DUMP;
        do_asave( NULL, "" );
    }

    if ( --pulse_ww       <= 0 )
    {
        pulse_ww        = PULSE_WW;
        ww_update       ( );
    }

    if ( --pulse_area     <= 0 )
    {
	pulse_area	= number_range( PULSE_AREA / 2, 3 * PULSE_AREA / 2 );
	area_update	( );
    }

    if ( --pulse_mobile   <= 0 )
    {
	pulse_mobile	= PULSE_MOBILE;
	mobile_update	( );
    }

    if ( --pulse_violence <= 0 )
    {
	pulse_violence	= PULSE_VIOLENCE;
	violence_update	( );
    }
    if ( --pulse_embrace <= 0)
    {
        pulse_embrace = PULSE_EMBRACE;
        embrace_update ( ); 
    }
    if ( --pulse_point    <= 0 )
    {
	pulse_point     = number_range( PULSE_TICK / 2, 3 * PULSE_TICK / 2 );
	weather_update	( );
	char_update	( );
	obj_update	( );
        room_update	( );
    }

    aggr_update( );
    tail_chain( );
    return;
}
