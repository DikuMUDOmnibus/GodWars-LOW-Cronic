
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

#define MONK_AUTODROP 11


/*
 * Local functions.
 */
void	autodrop	args( ( CHAR_DATA *ch) );
bool	check_dodge	args( ( CHAR_DATA *ch, CHAR_DATA *victim, int dt ) );
void	check_killer	args( ( CHAR_DATA *ch, CHAR_DATA *victim ) );
bool	check_parry	args( ( CHAR_DATA *ch, CHAR_DATA *victim, int dt ) );
void	dam_message	args( ( CHAR_DATA *ch, CHAR_DATA *victim, int dam, int dt ) );
void	death_cry	args( ( CHAR_DATA *ch ) );
void	group_gain	args( ( CHAR_DATA *ch, CHAR_DATA *victim ) );
int	xp_compute	args( ( CHAR_DATA *gch, CHAR_DATA *victim ) );
void	set_fighting	args( ( CHAR_DATA *ch, CHAR_DATA *victim ) );
bool	can_counter	args( ( CHAR_DATA *ch) );
bool	can_bypass	args( ( CHAR_DATA *ch, CHAR_DATA *victim ) );
int	number_attacks	args( ( CHAR_DATA *ch, CHAR_DATA *victim ) );
int	dambonus	args( ( CHAR_DATA *ch, CHAR_DATA *victim, 
			    int dam, int stance) );
void	update_damcap	args( ( CHAR_DATA *ch, CHAR_DATA *victim ) );
void    check_arena     args( ( CHAR_DATA *ch, CHAR_DATA *victim ) );


/*
 * Control the fights going on.
 * Called periodically by update_handler.
 */
void violence_update( void )
{
    CHAR_DATA *ch;
    CHAR_DATA *ch_next;
    CHAR_DATA *emb;
    CHAR_DATA *victim;
    CHAR_DATA *rch;
    CHAR_DATA *rch_next;
    CHAR_DATA *mount;
    //OBJ_DATA  *obj;
  
    for ( ch = char_list; ch != NULL; ch = ch->next )
    {
        victim = ch->blinkykill; 

 	if (victim == NULL) continue;

	if (IS_SET(ch->flag3, AFF3_BLINK_1ST_RD))
	{
	    REMOVE_BIT(ch->flag3, AFF3_BLINK_1ST_RD);
	    SET_BIT(ch->flag3, AFF3_BLINK_2ND_RD);
	}
	else if (IS_SET(ch->flag3, AFF3_BLINK_2ND_RD))
	{

	    REMOVE_BIT(ch->flag3, AFF3_BLINK_2ND_RD);
	    REMOVE_BIT(ch->flag3, AFF3_BLINK_1ST_RD);

	    /* Has the victim left? */
	    if (victim->in_room != ch->in_room)
	    {
		victim = NULL;
	 	act("$n pops back into existence.", ch, NULL, NULL, TO_ROOM);
		stc("You pop back into existence.\n\r", ch);
		stc("Your victim is no longer in the room.\n\r", ch);
		break;
	    }


	    act("$n pops back into existence next to $N.", ch,
NULL,victim, TO_NOTVICT);
	    act("You pop back into existence next to $N.", ch, NULL,
victim, TO_CHAR);
	    act("$n pops back into existence next to you.", ch, NULL,
victim, TO_VICT);
	    set_fighting(ch, victim);
	    do_say(ch, "Muhahahahahaha");
	    victim = NULL;
            multi_hit(ch, victim, gsn_blinky);
	    multi_hit(ch, victim, gsn_blinky);
	    if (IS_SET(ch->newbits, THIRD_HAND) && get_eq_char(ch, WEAR_THIRD) != NULL )
	    multi_hit(ch, victim, gsn_blinky);
	    if (IS_SET(ch->newbits, FOURTH_HAND) && get_eq_char(ch, WEAR_FOURTH) != NULL)
	    multi_hit(ch, victim, gsn_blinky);
	    update_pos(victim);
	}
    }


    for ( ch = char_list; ch != NULL; ch = ch->next )
    {
	ch_next	= ch->next;

	if (ch->fight_timer > 0)
		ch->fight_timer--;

        if (ch->rage > 0 && IS_CLASS(ch, CLASS_DEMON))
	    ch->rage--;

	if ( ch->embracing != NULL )
	{
	  emb = ch->embracing;
	  if ( emb == NULL ) ch->embracing = NULL;
	  if ( ch->in_room != emb->in_room )
	  {
	    if ( emb->embraced != NULL )
	    {
	      emb->embraced = NULL;
    	      ch->embracing = NULL;
	    }
	    else
	      ch->embracing = NULL;
	  }
	}

	if ( ch->embraced != NULL )
	{
          emb = ch->embraced;
	  if ( emb == NULL ) ch->embraced = NULL;
          if ( ch->in_room != emb->in_room )
          {
            if ( emb->embracing != NULL )
            {
              emb->embracing = NULL;
              ch->embraced   = NULL;
            }
	    else
	      ch->embraced = NULL;
          }
        }  
  
	if ( IS_SET(ch->monkstuff, MONK_DEATH) )
        {
	  if ( ch->hit > (ch->max_hit /2 ) )
	  {
	      ch->hit -=  number_range( 50,200 );
	      stc("Your writhe in agony as magical energies tear you asunder.\n\r",ch);
	      act("$n writhes in agony as magical forces tear apart $s body.",ch,NULL,NULL,TO_ROOM);
	  }               
	  else
	  {
	      if ( number_range( 1,2 ) == 1 )
	      {
	         stc("You feel the magical forces leave your body.\n\r",ch);
	         act("The magical forces leave $n's body.",ch,NULL,NULL,TO_ROOM);
		 REMOVE_BIT(ch->monkstuff, MONK_DEATH);
            }
	      else
	      {
   	          ch->hit -=  number_range( 50,200 );
	          stc("Your writhe in agony as magical energies tear you asunder.\n\r",ch);
	          act("$n writhes in agony as magical forces tear apart $s body.",ch,NULL,NULL,TO_ROOM); 
	      }
	   }
	}

        if ( IS_SET(ch->monkstuff, MONK_HEAL) )
        {
	  if ( ch->hit < (ch->max_hit /2 ) )
	  {
	    if ( ch->hit < ch->max_hit )
	      ch->hit +=  number_range( 200,400 );
	    if ( ch->move < ch->max_move )
	      ch->move += number_range( 175,400 );
	      stc("Your body emits glowing sparks.\n\r",ch);
	      act("$n's body emits glowing sparks and fizzes.",ch,NULL,NULL,TO_ROOM);
	  }               
	  else
	  {
	      if ( number_range( 1,2 ) == 1 )
	      {
	         stc("The sparks fizzle and die.\n\r",ch);
	         act("The sparks around $n's body fizzle and die.",ch,NULL,NULL,TO_ROOM);
		 REMOVE_BIT(ch->monkstuff, MONK_HEAL);
              }
	      else
	      {
	       if ( ch->hit < ch->max_hit )
		 ch->hit +=  number_range( 200,400 );
               if ( ch->move < ch->max_move )
                 ch->move += number_range( 175,400 );
                 stc("Your body emits glowing sparks.\n\r",ch);
                 act("$n's body emits glowing sparks and fizzes.",ch,NULL,NULL,TO_ROOM);
	      }
	   }
	}

/*
	if ( ( victim = ch->fighting ) == NULL || ch->in_room == NULL )
	    continue;

	if (!IS_NPC(ch) && !IS_NPC(victim) && !is_safe(ch, victim) &&
!is_safe(victim,ch))
		ch->fight_timer = 10;
*/

	if (IS_CLASS(ch, CLASS_MONK)
	  && ch->level < 7 && IS_SET(ch->act, PLR_WIZINVIS) )
	REMOVE_BIT(ch->act, PLR_WIZINVIS);

/*        if (!IS_CLASS(ch, CLASS_DEMON) && ch->alignment > 0
	 	&& !IS_NPC(ch) && ch->level > 2)
	{
	    OBJ_DATA *obj_next;

	    for ( obj = ch->in_room->contents; obj != NULL; obj = obj_next )
	    {
		obj_next = obj->next_content;
		if (IS_OBJ_STAT2(obj, ITEM_ATTACK_GOOD) && number_range(1, 3) == 1)
		{
                act("$p jumps into the air and slams into $n.", ch, obj, NULL, TO_ROOM);
                act("$p jumps into the air and slams into you.", ch, obj, NULL, TO_CHAR);
		    ch->hit -= number_range( 40, 60 );
                update_pos( ch );
		}
	    }
	}
*/
	if ( IS_SET(ch->newbits, NEW_POWER) )
 	{
	   if ( ch->move < 75 )
	     do_spiritpower(ch,"");
	   else
	     ch->move -= 15;
        }


      if ( ch->chi[CURRENT] > 0 )
      {
        if ( IS_CLASS(ch, CLASS_MONK) && ch->move > ( 500 - ch->chi[CURRENT] * 20 ))
            ch->move -= ch->chi[CURRENT] * 20;
        else
	{
	  if ( ch->chi[CURRENT] > 0 )
          {
	    stc("You feel exhausted, and lose control of your ch'i.\n\r",ch);
	    ch->chi[CURRENT] -= 1;
	  }
	}
      }

       if ( ( victim = ch->fighting ) == NULL || ch->in_room == NULL )
            continue;

        if (!IS_NPC(ch) && !IS_NPC(victim) && !is_safe(ch, victim) &&
!is_safe(victim,ch))
                ch->fight_timer = 10; 

	if ( IS_AWAKE(ch) && IS_AWAKE(victim) && 
		ch->in_room == victim->in_room ) 
	{
	    multi_hit( ch, victim, TYPE_UNDEFINED );
	if (IS_CLASS(ch, CLASS_DROW) && ch->cur_form == FRM_DROWSPIDER
	&& IS_SET(ch->pcdata->powers[1], DPOWER_ARMS))
	    multi_hit(ch, victim, gsn_spiderform);
	} 
	else
	    stop_fighting( ch, FALSE );

	if ( ( victim = ch->fighting ) == NULL )
	    continue;

	/*
	 * Fun for the whole family!
	 */
	for ( rch = ch->in_room->people; rch != NULL; rch = rch_next )
	{
	    rch_next = rch->next_in_room;

	    if ( IS_AWAKE(rch) && rch->fighting == NULL )
	    {
		/*
		 * Mount's auto-assist their riders and vice versa.
		 */
		if ( (mount = rch->mount) != NULL )
		{
		    if (mount == ch) multi_hit( rch, victim, TYPE_UNDEFINED );
		    continue;
		}
		/*
		 * PC's auto-assist others in their group.
		 */
		if ( !IS_NPC(ch) || IS_AFFECTED(ch, AFF_CHARM) )
		{
		    if ( ( !IS_NPC(rch) || IS_AFFECTED(rch, AFF_CHARM) )
		    &&   is_same_group(ch, rch) )
			multi_hit( rch, victim, TYPE_UNDEFINED );
		    continue;
		}

		/*
		 * NPC's assist NPC's of same type or 12.5% chance regardless.
		 */
		if ( IS_NPC(rch) && !IS_AFFECTED(rch, AFF_CHARM) )
		{
		    if ( rch->pIndexData == ch->pIndexData
		    ||   number_bits( 3 ) == 0 )
		    {
			CHAR_DATA *vch;
			CHAR_DATA *target;
			int number;

			target = NULL;
			number = 0;
			for ( vch = ch->in_room->people; vch; vch = vch->next )
			{
			    if ( can_see( rch, vch )
			    &&   is_same_group( vch, victim )
			    &&   number_range( 0, number ) == 0 )
			    {
				target = vch;
				number++;
			    }
			}

			if ( target != NULL )
			    multi_hit( rch, target, TYPE_UNDEFINED );
		    }
		}
	    }
	}
    }

    

    return;
}



/*
 * Do one group of attacks.
 */
void multi_hit( CHAR_DATA *ch, CHAR_DATA *victim, int dt )
{
    OBJ_DATA *wield;
    OBJ_DATA *wield1;
    OBJ_DATA *wield2;
    OBJ_DATA *wield3;
    OBJ_DATA *wield4;
    char buf[MAX_STRING_LENGTH];
    int	      sn;
    int	      level;
    int       chance;
    int       unarmed;
    int       wieldorig = 0;
    int       wieldtype = 0;
    int       maxcount;
    int       countup;
    int 	x;
    int	      wieldie;

    if ( ch->position < POS_SLEEPING ) return;

    if ( IS_CREATOR(ch) || IS_CLASS(ch, CLASS_MONK) )
    {
	if (!IS_NPC(ch) && wieldorig == 0)
        {
            if      (countup == 0) unarmed = number_range(0,3);
            else if (countup == 1) unarmed = number_range(4,7);
            else                   unarmed = number_range(0,7);
            if ( !IS_NPC(ch) && ch->cmbt[unarmed] != 0 && wieldorig == 0 )
            {
                fightaction(ch,victim,ch->cmbt[unarmed], dt, wieldtype);
            }
        } 
    }

    if ( dt == gsn_headbutt )
    {
	one_hit( ch, victim, dt, 1 );
	return;
    }

	if (dt == gsn_shiroken) 
	{one_hit(ch, victim, dt, 1);
	return;}

    if ( dt == gsn_spiderform ) {
	for (x = 1; x < 4; x++)
	one_hit(ch,victim,dt,0);
    }

    if ( dt == gsn_fangs )
    {
        one_hit( ch, victim, dt, 1 );
        return;
    } 
    if ( dt== gsn_quills)
    {
   	 one_hit(ch,victim,dt,1);
    	 return;
    }
    if ( dt==gsn_venomtong)
    {
  	  one_hit(ch,victim,dt,1);
   	  return;
    }
    if ( dt==gsn_spiketail)
    {
	    one_hit(ch,victim,dt,1);
	    return;
    }  
    if ( dt==gsn_badbreath)
    {
	    one_hit(ch,victim,dt,1);
	    return;
    }  
    if ( dt==gsn_magma)
    {
	    one_hit(ch,victim,dt,1);
	    return;
    }  
    if ( dt==gsn_shards)
    {
	    one_hit(ch,victim,dt,1);
	    return;
    }  
    if ( dt==gsn_cheapshot)
    {
	    send_to_char("You stun them with a shoulder charge!\n\r",ch);
	    send_to_char("You are stunned by a shoulder charge!\n\r",victim);
	  /*victim->position=POS_STUNNED;*/
	    one_hit(ch,victim,dt,1);    
	   /* victim->position=POS_STUNNED;
	    one_hit(ch,victim,dt,1);*/
	    one_hit(ch,victim,dt,1);
	    one_hit(ch,victim,dt,1);
	    return;
    } 
    if ( dt == gsn_buffet )
    {
        one_hit( ch, victim, dt, 1 );
        return;
    } 

    if ( dt == gsn_sweep )
    {
	  one_hit( ch, victim, dt, 1 );
	  return;
    }
    if ( dt == gsn_rfangs )
    {
         one_hit( ch, victim, dt, 1);
         return;
    }

   /* if (!IS_NPC(ch) && IS_AFFECTED(ch, AFF_PEACE))
    {
	send_to_char( "You are unable to attack them.\n\r", ch );
	return;
    }*/

    if (!IS_NPC(victim) && IS_AFFECTED(victim, AFF_PEACE))
    {
	send_to_char( "You can't seem to attack them.\n\r", ch );
	return;
    }


   /* if (!IS_NPC(ch) && IS_ITEMAFF(ch, ITEMA_PEACE))
    {
	send_to_char( "You are unable to attack them.\n\r", ch );
	return;
    }*/

    if (!IS_NPC(victim) && IS_ITEMAFF(victim, ITEMA_PEACE))
    {
	send_to_char( "You can't seem to attack them.\n\r", ch );
	return;
    }

    wield1 = get_eq_char( ch, WEAR_WIELD );
    wield2 = get_eq_char( ch, WEAR_HOLD );
    wield3 = get_eq_char( ch, WEAR_THIRD );
    wield4 = get_eq_char( ch, WEAR_FOURTH);
    if ( wield1 != NULL && wield1->item_type == ITEM_WEAPON ) wieldorig  = 1;
    if ( wield2 != NULL && wield2->item_type == ITEM_WEAPON ) wieldorig += 2;
    if ( wield3 != NULL && wield3->item_type == ITEM_WEAPON ) wieldorig += 4;
    if ( wield4 != NULL && wield4->item_type == ITEM_WEAPON ) wieldorig += 8;
    wieldtype = wieldorig;
    wieldie = number_range(1,4);

    if ( wieldorig == 15 ) 
    { 
        if (wieldie == 2) 
	  wieldtype = 8; 
	else
	  wieldtype = 4;
    }
    if ( wieldorig == 14 )
    {
	if (wieldie == 2)
	  wieldtype = 8;
        else if (wieldie == 1)
	  wieldtype = 4;
	else if (wieldie == 3)
          wieldtype = 2;
	else
	  wieldtype = 1;
    }
    if ( wieldorig == 13 )
    {
	if ( wieldie == 1)
	 wieldtype = 8;
	else if (wieldie == 2)
	 wieldtype = 4;
	else wieldtype = 1;
    }
    if ( wieldorig == 12 )
    {
	if ( wieldie == 1 )
	  wieldtype = 8;
	else
	  wieldtype = 4;
    }
    if ( wieldorig == 11 )
    {
	if ( wieldie == 1 )
	 wieldtype = 8;
	if ( wieldie == 2 )
	 wieldtype = 2;
	else wieldtype = 1;
    }
    if ( wieldorig == 10 )
    {
	if ( wieldie == 1 )
	 wieldtype = 8;
	else wieldtype = 2;
    }
    if ( wieldorig == 9 )
    {
	if ( wieldie == 1 )
	  wieldtype = 8;
	else wieldtype = 1;
    }
    if ( wieldorig == 8 )
	wieldtype = 8;
    if ( wieldorig == 7 )
    {
	if ( wieldie == 1 )
          wieldtype = 4;
	else if ( wieldie == 2 )
	  wieldtype = 2;
	else wieldtype = 1;
    }
    if ( wieldorig == 6 )
    {
       if ( wieldie == 1 || wieldie == 2)
         wieldtype = 2;
       else wieldtype = 4;
    } 
    if ( wieldorig == 5 )
    {
        if ( wieldie == 1)
          wieldtype = 4;
        else wieldtype = 1;
    }
    if ( wieldorig == 4 )
        wieldtype = 4;
    if ( wieldorig == 3 )
    {
        if ( wieldie == 2 || wieldie == 4)
          wieldtype = 2;
        else wieldtype = 1;
    }
    if ( wieldorig == 2 )
        wieldtype = 2;
    if ( wieldorig == 1 )
	wieldtype = 1;
	

         if ( wieldtype == 8 ) wield = wield4;
    else if ( wieldtype == 4 ) wield = wield3;
    else if ( wieldtype == 2 ) wield = wield2;
    else                       wield = wield1;

    if (!IS_NPC(ch) && ch->stance[0] > 0 && number_percent() == 5)
    {
	int stance = ch->stance[0];
	if (ch->stance[stance] >= 200)
	{
	    special_move(ch,victim);
	    return;
	}
    }

    unarmed = 0;
/*
    if (!IS_NPC(ch) && wieldorig == 0)
        {
            if      (countup == 0) unarmed = number_range(0,3);
            else if (countup == 1) unarmed = number_range(4,7);
            else                   unarmed = number_range(0,7);
            if ( !IS_NPC(ch) && !IS_AFFECTED(ch, AFF_POLYMORPH) &&
                ch->cmbt[unarmed] != 0 && wieldorig == 0 )
            {
                fightaction(ch,victim,ch->cmbt[unarmed], dt, wieldtype);
            }
        }
    else
*/

    if ( IS_SET( ch->flag2, VAMP_OBJMASK) )
    {
      act("You return to your normal form.",ch,NULL,NULL,TO_CHAR);
      sprintf(buf,"%s reforms as %s.\n\r",ch->morph, ch->name );
      act(buf,ch,NULL,NULL,TO_ROOM);
      free_string(ch->morph);
      free_string(ch->objdesc);
      ch->long_descr = str_dup("");
      REMOVE_BIT(ch->flag2, VAMP_OBJMASK);
      REMOVE_BIT(ch->affected_by, AFF_POLYMORPH);
    }

	one_hit( ch, victim, dt, wieldtype );

    if ( victim == NULL || victim->position != POS_FIGHTING ) return;

    /* Only want one spell per round from spell weapons...otherwise it's 
     * too powerful, and would unbalance player killing (as this is a PK mud).
     */

    if ( dt == TYPE_UNDEFINED )
    {
	dt = TYPE_HIT;
	if ( wield != NULL && wield->item_type == ITEM_WEAPON )
	{
	    dt += wield->value[3];
	    if (wield->value[0] >= 1)
	    {
		/* Look, just don't ask...   KaVir */
		if (wield->value[0] >= 1000)
		    sn = wield->value[0] - ((wield->value[0] / 1000) * 1000);
		else
		    sn = wield->value[0];
		if (sn != 0 && victim->position == POS_FIGHTING)
		    (*skill_table[sn].spell_fun) (sn,wield->level,ch,victim);
	    }
	}
    }

    if ( victim == NULL || victim->position != POS_FIGHTING ) return;

    if ( ch->fighting != victim || dt == gsn_backstab || dt == gsn_shiroken 
	|| dt == gsn_garotte || dt == gsn_headbutt || dt == gsn_circle )
	return;


    maxcount = number_attacks(ch, victim);
    if (!IS_NPC(ch))
    {
	chance = 0;
	if (wield != NULL && wield->item_type == ITEM_WEAPON)
	{
	    int tempnum = wield->value[3];
	    chance = (ch->wpn[tempnum]) * 0.5;
	}
	else chance = (ch->wpn[0]) * 0.5;
	if (number_percent() <= chance) maxcount += 1;
    }
    if (wieldorig == 3) maxcount += 1;
    for (countup = 0; countup <= maxcount; countup++)
    {
/*
	if (!IS_NPC(ch) && wieldorig == 0)
	{
	    if      (countup == 0) unarmed = number_range(0,3);
	    else if (countup == 1) unarmed = number_range(4,7);
    	    else                   unarmed = number_range(0,7);
    	    if ( !IS_NPC(ch) && !IS_AFFECTED(ch, AFF_POLYMORPH) &&
		ch->cmbt[unarmed] != 0 && wieldorig == 0 )
	    {
		fightaction(ch,victim,ch->cmbt[unarmed], dt, wieldtype);
		continue;
	    }
	}
*/
	wieldtype = wieldorig;
        if ( wieldorig == 15 ) 
    { 
        if (wieldie == 2) 
	  wieldtype = 8; 
	else
	  wieldtype = 4;
    }
    if ( wieldorig == 14 )
    {
	if (wieldie == 2)
	  wieldtype = 8;
        else if (wieldie == 1)
	  wieldtype = 4;
	else if (wieldie == 3)
          wieldtype = 2;
	else
	  wieldtype = 1;
    }
    if ( wieldorig == 13 )
    {
	if ( wieldie == 1)
	 wieldtype = 8;
	else if (wieldie == 2)
	 wieldtype = 4;
	else wieldtype = 1;
    }
    if ( wieldorig == 12 )
    {
	if ( wieldie == 1 )
	  wieldtype = 8;
	else
	  wieldtype = 4;
    }
    if ( wieldorig == 11 )
    {
	if ( wieldie == 1 )
	 wieldtype = 8;
	if ( wieldie == 2 )
	 wieldtype = 2;
	else wieldtype = 1;
    }
    if ( wieldorig == 10 )
    {
	if ( wieldie == 1 )
	 wieldtype = 8;
	else wieldtype = 2;
    }
    if ( wieldorig == 9 )
    {
	if ( wieldie == 1 )
	  wieldtype = 8;
	else wieldtype = 1;
    }
    if ( wieldorig == 7 )
    {
	if ( wieldie == 1 )
          wieldtype = 4;
	else if ( wieldie == 2 )
	  wieldtype = 2;
	else wieldtype = 1;
    }
    if ( wieldorig == 6 )
    {
       if ( wieldie == 1 || wieldie == 2)
         wieldtype = 2;
       else wieldtype = 4;
    } 
    if ( wieldorig == 5 )
    {
        if ( wieldie == 1)
          wieldtype = 4;
        else wieldtype = 1;
    }
    if ( wieldorig == 3 )
    {
        if ( wieldie == 2 || wieldie == 4)
          wieldtype = 2;
        else wieldtype = 1;
    }
	one_hit( ch, victim, -1, wieldtype );
	if ( victim == NULL || victim->position != POS_FIGHTING ) return;
	if ( ch->fighting != victim ) return;
    }

    if (!IS_NPC(ch) && IS_VAMPAFF(ch,VAM_FANGS))
	one_hit( ch, victim, ( TYPE_HIT + 10 ), 0 );
    if ( victim == NULL || victim->position != POS_FIGHTING ) return;
    if (!IS_NPC(ch))
    {
	if (IS_VAMPAFF(ch,VAM_HORNS) 
		&& number_range(1,3) == 1)
	multi_hit( ch, victim, gsn_headbutt );
    }
	if (IS_CLASS(ch, CLASS_DEMON)  && IS_DEMAFF(ch,DEM_HORNS) 
		&& number_range(1,3) == 1)
	multi_hit( ch, victim, gsn_headbutt );

        if (IS_CLASS(ch, CLASS_DEMON)  && IS_DEMAFF(ch, DEM_TAIL)
		&& number_range(1,2) == 2)
	multi_hit( ch, victim, gsn_sweep );

	if (!IS_NPC(ch))
	if (IS_CLASS(ch, CLASS_NINJA) && ch->pcdata->powers[NPOWER_NINGENNO] >=4 && number_range(1,4) != 1)
	multi_hit(ch, victim, gsn_shiroken);


    if (!IS_NPC(ch))
    {
        if (IS_VAMPAFF(ch,VAM_TAIL)
                && number_range(1,2) == 1)
        multi_hit( ch, victim, gsn_sweep );
    }          

    if (!IS_NPC(ch))
    {
        if (IS_VAMPAFF(ch,VAM_HEAD)
                && number_range(1,2) == 1)
        multi_hit( ch, victim, gsn_fangs );
    }        
   if (!IS_NPC(ch))
  {
   if (IS_SET(ch->newbits,NEW_QUILLS))
       multi_hit(ch,victim,gsn_quills);
    }

   if (!IS_NPC(ch)&& IS_CLASS(ch,CLASS_WEREWOLF))
   {
   if (IS_SET(ch->newbits,NEW_SLAM) && number_range(1,
ch->power[DISC_WERE_BEAR]/3)==1)
   multi_hit(ch,victim,gsn_cheapshot);
  }
    if (!IS_NPC(ch))
    {
        if (IS_VAMPAFF(ch,VAM_WINGS)
                && number_range(1,2) == 1)
        multi_hit( ch, victim, gsn_buffet );
    }        
    if (!IS_NPC(ch) && IS_CLASS(ch,CLASS_WEREWOLF)
                    && ch->power[DISC_WERE_RAPT] > 0)
    {
     multi_hit(ch,victim,gsn_rfangs);
    }
                    
    if ( victim == NULL || victim->position != POS_FIGHTING ) return;

    if (!IS_NPC(ch) && IS_CLASS(ch, CLASS_VAMPIRE) && IS_VAMPAFF(ch, VAM_SERPENTIS))
	spell_poison( gsn_poison, (ch->level*number_range(5,10)),ch,victim );
    else if (!IS_NPC(ch) && IS_CLASS(ch, CLASS_WEREWOLF) && ch->power[DISC_WERE_SPID] > 0)
	spell_poison( gsn_poison, (ch->level*number_range(5,10)),ch,victim );
    else if (!IS_NPC(ch) && IS_CLASS(ch, CLASS_DROW) && IS_SET(ch->pcdata->powers[1], DPOWER_DROWPOISON))
	spell_poison(gsn_poison,(ch->level*number_range(10,20)),ch,victim);
	else if (!IS_NPC(ch) && IS_CLASS(ch, CLASS_NINJA) &&
	ch->pcdata->powers[NPOWER_NINGENNO] >=5)
	spell_poison(gsn_poison,(ch->level*number_range(5,10)),ch,victim);

   if (victim->itemaffect < 1) return;
    if (IS_NPC(victim) || victim->spl[1] < 4) level = victim->level;
    else level = (victim->spl[1] * 0.25);
    
    if (IS_SET(victim->newbits, NEW_MONKFLAME) && !IS_AFFECTED(ch, AFF_FLAMING))
    {
	if (number_percent() > 95)
	{
        SET_BIT(ch->affected_by, AFF_FLAMING);
    if (!IS_CLASS(ch, CLASS_WEREWOLF))
    {
    act("Your flaming hands catch $n on fire!", ch, NULL, victim, TO_VICT);
    act("$N's flaming hands catch $n on fire!", ch, NULL, victim, TO_NOTVICT);
    act("$N's flaming hands catch you on fire!.", ch, NULL, victim, TO_CHAR);
    }
    else
    {
    act("Your flaming claws catch $n on fire!", ch, NULL, victim, TO_VICT);
    act("$N's flaming claws catch $n on fire!", ch, NULL, victim, TO_NOTVICT);
    act("$N's flaming claws catch you on fire!.", ch, NULL, victim, TO_CHAR);
    }
   }
    else
	{
	 return;
	}
    }

    if (IS_ITEMAFF(victim, ITEMA_SHOCKSHIELD) && ch->position == POS_FIGHTING)
	if ( (sn = skill_lookup( "lightning bolt" ) ) > 0)
	    (*skill_table[sn].spell_fun) (sn,level,victim,ch);
    if (IS_ITEMAFF(victim, ITEMA_FIRESHIELD) && ch->position == POS_FIGHTING)
	if ( (sn = skill_lookup( "fireball" ) ) > 0)
	    (*skill_table[sn].spell_fun) (sn,level,victim,ch);
    if (IS_ITEMAFF(victim, ITEMA_ICESHIELD) && ch->position == POS_FIGHTING)
	if ( (sn = skill_lookup( "chill touch" ) ) > 0)
	    (*skill_table[sn].spell_fun) (sn,level,victim,ch);
    if (IS_ITEMAFF(victim, ITEMA_ACIDSHIELD) && ch->position == POS_FIGHTING)
	if ( (sn = skill_lookup( "acid blast" ) ) > 0)
	    (*skill_table[sn].spell_fun) (sn,level,victim,ch);
    if (IS_ITEMAFF(victim, ITEMA_CHAOSSHIELD) && ch->position == POS_FIGHTING)
	if ( (sn = skill_lookup( "chaos blast" ) )
> 0)
	    (*skill_table[sn].spell_fun) (sn,level,victim,ch);
    return;
}


int number_attacks( CHAR_DATA *ch, CHAR_DATA *victim )
{
    int count = 1;

    if ( IS_NPC(ch) )
    {
	if (ch->level >= 50) count += 1;
	if (ch->level >= 100) count += 1;
	return count;
    }

 if (IS_NPC(victim))
 {
    if ( IS_STANCE(ch, STANCE_VIPER) && number_percent() > 
	ch->stance[STANCE_VIPER] * 0.5) count += 1;
    else if ( IS_STANCE(ch, STANCE_MANTIS) && number_percent() > 
	ch->stance[STANCE_MANTIS] * 0.5) count += 1;
    else if ( IS_STANCE(ch, STANCE_TIGER) && number_percent() > 
	ch->stance[STANCE_TIGER] * 0.5) count += 1;
    else if ( IS_STANCE(ch, STANCE_WOLF) && number_percent() >
       ch->stance[STANCE_WOLF] * .5) count += 2;
    else if ( IS_STANCE(ch, STANCE_XKILLA) && number_percent() >
       ch->stance[STANCE_XKILLA] * 1) count += 4;
    if (!IS_NPC(ch))
    {   
        if ( IS_CLASS(ch, CLASS_VAMPIRE) && ch->power[DISC_VAMP_CELE] > 0)
                count += .5;
        else if ( IS_CLASS(ch, CLASS_VAMPIRE) && ch->power[DISC_VAMP_CELE] > 5)
                count += .5;
	else if ( IS_CLASS(ch, CLASS_VAMPIRE) && ch->power[DISC_VAMP_CELE] > 7)
		count += .5;
	else if ( IS_CLASS(ch, CLASS_VAMPIRE) && ch->power[DISC_VAMP_CELE] > 9)
		count += .5;
    	else if (IS_CLASS(ch, CLASS_WEREWOLF) && ch->power[DISC_WERE_LYNX] > 2 )
		count += 2;
	else if ( !IS_NPC(ch) && IS_CLASS(ch, CLASS_DEMON) && IS_DEMPOWER( ch, DEM_SPEED) )
		count += 2;   	
	else if (IS_CLASS(ch, CLASS_DROW) && (IS_SET(ch->special, SPC_DROW_WAR) || ch->generation < 3))
		count += 1;
    }

	if (!IS_NPC(ch) && IS_CLASS(ch, CLASS_DROW) && 
	IS_SET(ch->pcdata->powers[1],DPOWER_SPEED))
	count += 1;

    if ( !IS_NPC(ch) && IS_CLASS(ch,CLASS_WEREWOLF) && ch->power[DISC_WERE_BOAR] > 6)
    {
    count += ch->move/2000;
    }
	if (!IS_NPC(ch) && IS_CLASS(ch, CLASS_NINJA) && ch->pcdata->powers[NPOWER_CHIKYU] >= 3)
		count += 2;

    if ( IS_ITEMAFF(ch, ITEMA_SPEED) ) count += 2;
  }

  else
  {
    if ( IS_STANCE(ch, STANCE_VIPER) && number_percent() > 
	ch->stance[STANCE_VIPER] * 0.5) count += 1;
    else if ( IS_STANCE(ch, STANCE_MANTIS) && number_percent() > 
	ch->stance[STANCE_MANTIS] * 0.5) count += 1;
    else if ( IS_STANCE(ch, STANCE_TIGER) && number_percent() > 
	ch->stance[STANCE_TIGER] * 0.5) count += 1;
    else if ( IS_STANCE(ch, STANCE_XKILLA) && number_percent() >
        ch->stance[STANCE_XKILLA] * 0.5) count += 1;
    

    if (!IS_NPC(ch))
    {   
           if ( IS_CLASS(ch, CLASS_VAMPIRE) && ch->power[DISC_VAMP_CELE] > 0)
            count += 1;
      else if ( IS_CLASS(ch, CLASS_VAMPIRE) && ch->power[DISC_VAMP_CELE] > 5)
            count += 1;
	else if ( IS_CLASS(ch, CLASS_VAMPIRE) && ch->power[DISC_VAMP_CELE] > 7)
		count += 1;
	else if ( IS_CLASS(ch, CLASS_VAMPIRE) && ch->power[DISC_VAMP_CELE] > 9)
		count += 1;
	else if ( IS_CLASS(ch, CLASS_MONK) && ch->monkab[COMBAT] > 0 )
		count += ch->monkab[COMBAT];
    	else if (IS_CLASS(ch, CLASS_WEREWOLF) && ch->power[DISC_WERE_LYNX] > 2 )
		count += 1;
        else if (ch->power[DISC_DAEM_ATTA] > 0)
		count += ch->power[DISC_DAEM_ATTA] / 2;
	else if (IS_CLASS(ch, CLASS_DROW) && (IS_SET(ch->special, SPC_DROW_WAR) || ch->generation < 3))
		count += 1;

    }

    if ( !IS_NPC(ch) && IS_CLASS(ch,CLASS_WEREWOLF) && ch->power[DISC_WERE_BOAR] > 6)
    {
    count += ch->move/4000;
    }

    if ( IS_ITEMAFF(ch, ITEMA_SPEED) ) count += 1;

    if (victim->power[DISC_VAMP_CELE] >= 3)
        count -= victim->power[DISC_VAMP_CELE] / 3;
    if (victim->power[DISC_WERE_MANT] >= 3)
	  count -= victim->power[DISC_WERE_MANT] / 3;
    if (victim->power[DISC_DAEM_ATTA] > 0)
	count -= victim->power[DISC_DAEM_ATTA] /2;
    if (number_range(1,4) == 2)
        count -= 1;
 }
    if ( count < 1)
       count = 1;


    return count;
}


/*
 * Hit one guy once.
 */
void one_hit( CHAR_DATA *ch, CHAR_DATA *victim, int dt, int handtype)
{

    OBJ_DATA *wield;
//  OBJ_DATA *gloves;
    char buf[MAX_STRING_LENGTH];
    int victim_ac;
    int thac0;
    int thac0_00;
    int thac0_32;
    int dam, wdam;
    int diceroll;
    int level;
    int attack_modify;
//  int stance;
    int right_hand;

    /*
     * Can't beat a dead char!
     * Guard against weird room-leavings.
     */
    if ( victim->position == POS_DEAD || ch->in_room != victim->in_room )
	return;

    if ( !IS_NPC(ch) )
    {
	if (IS_AFFECTED( ch, AFF_PEACE) && ch->fighting != NULL)
	REMOVE_BIT(ch->affected_by, AFF_PEACE);
    }
    
    if ( !IS_NPC(victim) )
    {
	if (IS_AFFECTED(victim, AFF_PEACE) && victim->fighting != NULL)
	REMOVE_BIT(victim->affected_by, AFF_PEACE);
    }

    if (!IS_NPC(ch) && IS_AFFECTED(ch, AFF_PEACE))
    {
	send_to_char( "You are unable to attack them.\n\r", ch );
	return;
    }

    if (!IS_NPC(victim) && IS_AFFECTED(victim, AFF_PEACE))
    {
	send_to_char( "You can't seem to attack them.\n\r", ch );
	return;
    }


    if (!IS_NPC(ch) && IS_ITEMAFF(ch, ITEMA_PEACE))
    {
	send_to_char( "You are unable to attack them.\n\r", ch );
	return;
    }

    if (!IS_NPC(victim) && IS_ITEMAFF(victim, ITEMA_PEACE))
    {
	send_to_char( "You can't seem to attack them.\n\r", ch );
	return;
    }

    if (ch->power[DISC_DAEM_HELL] > 3 && number_range(1, 50-ch->generation) == 1
	&& ch->fighting != NULL)
    {
        wdam = dice(ch->generation + 5, 20);
	sprintf(buf,"#1%s breathes out a cloud of fire at you#n.[%d]", ch->name, wdam);
	sprintf(buf,"#1%s breathes out a cloud of fire at %s#n.",ch->name,victim->name );
	sprintf(buf,"#1You breathe out a cloud of fire at %s#n.[%d]",victim->name, wdam );
        victim->hit -= wdam;
	update_pos(victim);
    }

    /*
     * Figure out the type of damage message.
     */
    if (handtype == 8)
    {
	wield = get_eq_char( ch, WEAR_FOURTH );
	right_hand = 8;
    }
    else if (handtype == 4)
    {
	wield = get_eq_char( ch, WEAR_THIRD );
	right_hand = 4;
    }
    else if (handtype == 2)
    {
    	wield = get_eq_char( ch, WEAR_HOLD );
	right_hand = 2;
    }
    else if (handtype == 0) 
    {
	wield = NULL;
	right_hand = 0;
    }
    else
    {
    	wield = get_eq_char( ch, WEAR_WIELD );
	right_hand = 1;
    }

    if ( dt == TYPE_UNDEFINED )
    {
	dt = TYPE_HIT;
	if ( wield != NULL && wield->item_type == ITEM_WEAPON )
	    dt += wield->value[3];
    }

    if (ch->wpn[dt-1000] > 5) level = (ch->wpn[dt-1000] / 5);
    else level = 1;
    if (level > 40) level = 40;

    /*
     * Calculate to-hit-armor-class-0 versus armor.
     */
    if ( IS_NPC(ch) )
    {
	thac0_00 = 20;
	thac0_32 =  0;
    }
    else
    {
	thac0_00 = SKILL_THAC0_00;
	thac0_32 = SKILL_THAC0_32;
    }
    thac0     = interpolate( level, thac0_00, thac0_32 ) - char_hitroll(ch);
    victim_ac = UMAX( -100, char_ac(victim) / 10 );
    if ( !can_see( ch, victim ) )
	victim_ac -= 4;

    /*
     * The moment of excitement!
     */
    while ( ( diceroll = number_bits( 5 ) ) >= 20 )
	;

    if ( diceroll == 0
    || ( diceroll != 19 && diceroll < thac0 - victim_ac ) )
    {
	/* Miss. */
	damage( ch, victim, 0, dt );
	tail_chain( );
	if (!is_safe(ch,victim))
	{
	    improve_wpn(ch,dt,right_hand);
    	    improve_stance(ch);
	}
	return;
    }

    /*
     * Hit.
     * Calc damage.
     */
    if ( IS_NPC(ch) )
    {
	dam = number_range( ch->level / 2, ch->level * 3 / 2 );
	if ( wield != NULL )
	    dam += dam * 0.5;
    }
    else
    {
	if ( IS_VAMPAFF(ch, VAM_CLAWS) && IS_GAR1(ch, WOLF_RAZORCLAWS))
            dam = dice( 25, 35 ); 
	else if ( IS_VAMPAFF(ch,VAM_CLAWS) && wield == NULL)
	    dam = dice( 10, 20 );
	else if ( IS_SET(ch->newbits, NEW_MONKADAM))
	    dam = dice( 10, 25 );
	else if ( wield != NULL && wield->item_type == ITEM_WEAPON )
	    dam = dice( wield->value[1], wield->value[2] );
	else
	    dam = dice( 1, 4 );
    }

    /*
     * Bonuses.
     */
    dam += char_damroll(ch);
    if ( !IS_AWAKE(victim) )
	dam *= 2;


    if ( !IS_NPC(ch) && dt >= TYPE_HIT)
	dam = dam + (dam * ((ch->wpn[dt-1000]+1) / 60));


    /* Other Resistances */
   
    if (IS_ITEMAFF(victim, ITEMA_RESISTANCE))
	    dam /= 2;
       
    /* damage reduction */
    if ( dam > 1000)
	dam = (dam - 1000)/2 + 312;
    if ( dam > 1500)
	dam = (dam - 1500)/2 + 427;


    if ( dam <= 0 )
	dam = 1;

    if ( !IS_NPC(ch) && IS_CLASS(ch, CLASS_DEMON) && 
		IS_DEMPOWER(ch, DEM_MIGHT) )
	dam *= 1.1;

    if (!IS_NPC(ch) && IS_CLASS(ch, CLASS_WEREWOLF)
	&& ch->power[DISC_WERE_BEAR] > 5)
       dam *= 1.2;
     
    if (!IS_NPC(victim) && IS_CLASS(victim, CLASS_WEREWOLF)
	&& ch->power[DISC_WERE_LUNA] > 9)
       {
	int luna;
	luna = number_range(1,50);
	if (luna == 10 || luna == 20 || luna == 30 || luna == 40 || luna == 50)
        {
	stc("#3You absorb your opponents damage#n.\n\r",victim);
	stc("#3Your opponent absorbs your damage#n.\n\r",ch);
	dam = 0;
        }
       }

 if (!IS_NPC(ch) && wield != NULL)
 {

    if (number_range(1,6) == 2)
    {
    	if (ch->fighting == victim && IS_WEAP(wield,WEAPON_VAMPIRIC))
	{
	    wdam = number_range(1,wield->level / 5 + 1);
	    act("$p draws life from $n.",victim,wield,NULL,TO_ROOM);
	    act("You feel $p drawing your life away.",
		victim,wield,NULL,TO_CHAR);
	    victim->hit -= wdam;
	    update_pos( victim );
	    ch->alignment = UMAX(-1000,ch->alignment - 1);
	    ch->hit += wdam/2;
	}

	if (ch->fighting == victim && (IS_WEAP(wield,WEAPON_FLAMING)
		|| IS_WEAP(wield, WEAPON_ELE_FLAME) )  )
	{
	    wdam = number_range(1,wield->level / 4 + 1);
	    act("$n is burned by $p.",victim,wield,NULL,TO_ROOM);
	    act("$p sears your flesh.",victim,wield,NULL,TO_CHAR);
	    fire_effect( (void *) victim,wield->level/2,wdam,TARGET_CHAR);
	    victim->hit -= wdam;
	    update_pos( victim );
	}

	if (ch->fighting == victim && (IS_WEAP(wield,WEAPON_FROST)
		|| IS_WEAP(wield, WEAPON_ELE_WATER) ) )
	{
	    wdam = number_range(1,wield->level / 6 + 2);
	    act("$p freezes $n.",victim,wield,NULL,TO_ROOM);
	    act("The cold touch of $p surrounds you with ice.",
		victim,wield,NULL,TO_CHAR);
	    cold_effect(victim,wield->level/2,wdam,TARGET_CHAR);
	    victim->hit -= wdam;
	    update_pos( victim );
	}

	if (ch->fighting == victim && IS_WEAP(wield, WEAPON_ELE_EARTH) )
	{
	    wdam = number_range(1,wield->level / 5 + 2);
	    act("$p covers $n in acid.",victim,wield,NULL,TO_ROOM);
	    act("The corrosive touch of $p surrounds you with acid.",
		victim,wield,NULL,TO_CHAR);
	    acid_effect(victim,wield->level/2,wdam,TARGET_CHAR);
	    victim->hit -= wdam;
	    update_pos( victim );
	}

	if (ch->fighting == victim && (IS_WEAP(wield,WEAPON_SHOCKING)
		|| IS_WEAP(wield, WEAPON_ELE_AIR) ) )
	{
	    wdam = number_range(1,wield->level/5 + 2);
	    act("$n is struck by lightning from $p.",victim,wield,NULL,TO_ROOM);
	    act("You are shocked by $p.",victim,wield,NULL,TO_CHAR);
	    shock_effect(victim,wield->level/2,wdam,TARGET_CHAR);
	    victim->hit -= wdam;
	    update_pos( victim );
      }

	if (ch->fighting == victim && IS_WEAP(wield, WEAPON_SUNBLADE))
	{
	    wdam = number_range(wield->level/2, wield->level+1);
	    if (victim->alignment > -100)
		wdam = 0;
	    if (victim->alignment > -300)
		wdam = wdam / 2;
	    if (victim->alignment == -1000)
		wdam *= 2;
	    if (victim->class == CLASS_VAMPIRE)
		wdam *= 4;
	    if (wdam > 0)
	    {
	    act("$n's flesh is seared by the radiant aura of $p.",victim,wield,NULL,TO_ROOM);
	    act("Your flesh is seared by the radiant aura of $p.",victim,wield,NULL,TO_CHAR);
	    victim->hit -= wdam;
	    update_pos( victim );
	    ch->alignment = UMAX(-1000,ch->alignment + 1);
	    }
	}

	if (ch->fighting == victim 
		&& IS_WEAP(wield, WEAPON_DRAGONLANCE))
	{
	    wdam = number_range(wield->level/2, wield->level+1)*4;
		wdam = 0;
	    if (wdam > 0)
	    {
	    act("$n's flesh is torn apart by $p.",victim,wield,NULL,TO_ROOM);
	    act("Your flesh is torn apart by $p.",victim,wield,NULL,TO_CHAR);
	    victim->hit -= wdam;
	    update_pos( victim );
	    }
	}

	if ( ch->fighting == victim && IS_CLASS(ch, CLASS_MONK)
	  && !IS_NPC(ch) && ch->monkab[BODY] > 3 )
	{
	    wdam = number_range( (ch->chi[CURRENT]*6),(ch->chi[CURRENT]*10) );
	    if ( victim->alignment < 0 )
             wdam *= 1.5;
            act("$n's hands glow with a heavenly aura.",ch,NULL,victim,TO_VICT);
	    act("$n's aura sears your flesh!",ch,NULL,victim,TO_VICT);
	    act("Your heavenly aura sears $N's flesh.",ch,NULL,victim,TO_CHAR);
	    act("$n's hands' heavenly aura sears $N's flesh.",ch,NULL,victim,TO_NOTVICT);
	    victim->hit -= wdam;
	    update_pos( victim );
	}
     }
  }

    if ( number_range( 1,5 ) == 3 )
    {
	if ( ch->fighting == victim && IS_CLASS(ch, CLASS_MONK)
          && !IS_NPC(ch) && ch->monkab[BODY] > 3 )
        {
            wdam = number_range( (ch->chi[CURRENT]*6),(ch->chi[CURRENT]*10) );
            if ( victim->alignment < 0 )
             wdam *= 1.5;
            act("$n's hands glow with a heavenly aura.",ch,NULL,victim,TO_VICT);
            act("$n's aura sears your flesh!",ch,NULL,victim,TO_VICT);
            act("Your heavenly aura sears $N's flesh.",ch,NULL,victim,TO_CHAR);
            act("$n's hands' heavenly aura sears $N's flesh.",ch,NULL,victim,TO_NOTVICT);
            victim->hit -= wdam;
            update_pos( victim );
        }
    }

  if ( !IS_NPC(ch) && IS_CLASS(ch, CLASS_MONK) )
  {
    if (ch->chi[CURRENT] > 0 && ch->chi[CURRENT] < 3 )
	dam *= 1.3;
    if (ch->chi[CURRENT] > 2 && ch->chi[CURRENT] < 7 )
	dam *= ((ch->chi[CURRENT]) / 2);
  }

    if ( dt == gsn_garotte )
	dam *= (number_range(3,5));
   
    if ( dt == gsn_backstab ) 
	if (IS_CLASS(ch, CLASS_NINJA) && !IS_NPC(ch) && ch->pcdata->powers[NPOWER_NINGENNO] >=2)
	dam *= number_range(4,8);
	else
	dam *= number_range(2,4);

	if (dt == gsn_circle)
	dam *= number_range(2,4);

    dam = cap_dam(ch,victim,dam);	
    attack_modify = dice(1, 100);
    randomize_damage(ch,dam,attack_modify);
    damage( ch, victim, dam, dt );

    if (IS_SET(victim->newbits, NEW_LWRATH) && dt != gsn_garotte)
    {
    act("You absorb the damage and $n is struck by Luna's Wrath!", ch, NULL, victim, TO_VICT);
    act("$N absorbs $n's hit and $n is struck by Luna's Wrath!", ch, NULL, victim, TO_NOTVICT);
       if (!IS_NPC(ch))
    act("$n absorbs your damage and you are struck by Luna's Wrath!", ch, NULL, victim, TO_CHAR);
      ch->hit -= 250;
      victim->hit += 500;
     if (victim->hit > victim->max_hit)
      victim->hit = victim->max_hit;
      REMOVE_BIT(victim->newbits, NEW_LWRATH);
    }


    tail_chain( );
    if (!is_safe(ch,victim))
    {
	improve_wpn(ch,dt,right_hand);
	improve_stance(ch);
    }
 
}

int randomize_damage( CHAR_DATA *ch, int dam, int am )
{
    dam = dam * (am + 50) / 100;
    return dam;
}

int cap_dam(CHAR_DATA *ch, CHAR_DATA *victim, int dam)
{
    
    if (is_safe(ch, victim))
	return 0;

    if (dam > 80)
	dam = (dam - 80) / 2 + 80;

    if (!IS_NPC(victim) && victim->pcdata->condition[COND_DRUNK] > 10)
	dam /= 2;
	
	if (!IS_NPC(victim) && IS_CLASS(victim, CLASS_DROW)) {
	if (IS_SET(victim->pcdata->powers[1], DPOWER_TOUGHSKIN))
	dam /= 2;
	}

    /* Vampire protean 7 (flesh of lava) reduces damage */
    if (victim->power[DISC_VAMP_PROT] >= 10)
    {
  	dam *= 2;
	dam /= 3;
    }

    /* body of stone */
    if (victim->monkab[BODY] >= 2 )
    {
	dam *= 2;
	dam /= 3;
    }

    /* Monk Chi */
    if (victim->chi[CURRENT] > 0 )
    {
	dam *= (100 - (victim->chi[CURRENT] * 8) );
	dam /= 100;
    }

    if (victim->power[DISC_DAEM_IMMU] > 0)
    {
	dam *= (100 - (victim->power[DISC_DAEM_IMMU] * 3.7));
	dam /= 100;
    }

    /* Vampire fortitude reduces damage */
    if (victim->power[DISC_VAMP_FORT] > 0)
    {
        dam *= (100 - (victim->power[DISC_VAMP_FORT] * 8));
        dam /= 100;
    }

	if (!IS_NPC(victim))
	if (victim->pcdata->powers[NPOWER_CHIKYU] >= 1 && IS_CLASS(victim,CLASS_NINJA))
	{
	dam /= 2;
	}

	
        if (!IS_NPC(victim))
        if (victim->pcdata->powers[NPOWER_CHIKYU] >= 1 && IS_CLASS(victim,CLASS_MAGE))
        {
        dam /= 2;
        }	

	else if ( !IS_NPC(victim) && IS_CLASS(victim, CLASS_DEMON) 
		 && IS_DEMPOWER( victim, DEM_TOUGH))
        {
          dam *= 50;
          dam /= 100;
        }

    if (victim->power[DISC_WERE_BOAR] > 2)
    {
	dam *= 50;
        dam /= 100;
    }

    if ( IS_GAR1(victim, WOLF_COCOON) )
        dam /= 1.5;

    /* Vampires should be tougher at night and weaker during the day. */
    if ( !IS_NPC(ch) && IS_CLASS(ch, CLASS_VAMPIRE) )
    {
        if (weather_info.sunlight == SUN_LIGHT && dam > 1)
            dam /= 1.5;
        else if (weather_info.sunlight == SUN_DARK)
            dam *= 1.5;
    }

    if (dam >= 2000)
        dam = dam - (dam / number_range(3,10) + number_range(10,30));

    if (!IS_NPC(victim))
    {
	dam = dam + (dam / number_range(2,5) + number_range(10,50));
	dam *= (number_range(2,4) * number_range(2,3) / number_range(4,6));
    }

    if (dam <= 0)
	dam = 1;
                                                                               
    if (dam > 30000) dam = 30000;                                               

    if ( dam > ch->damcap[DAM_CAP] )
      dam = number_range( (ch->damcap[DAM_CAP] - 200), (ch->damcap[DAM_CAP] + 100) );

    if ( IS_AFFECTED(victim, AFF_SANCTUARY ) ) dam *= 0.5;

    return dam;
}

bool can_counter( CHAR_DATA *ch)
{
    if (!IS_NPC(ch) && IS_STANCE(ch, STANCE_MONKEY)) return TRUE;
    return FALSE;
}

bool can_bypass( CHAR_DATA *ch, CHAR_DATA *victim )
{
    if (IS_NPC(ch) || IS_NPC(victim)) return FALSE;
    else if (IS_STANCE(ch, STANCE_VIPER)) return TRUE;
    else if (IS_STANCE(ch, STANCE_MANTIS)) return TRUE;
    else if (IS_STANCE(ch, STANCE_TIGER)) return TRUE;
    else if (IS_STANCE(ch, STANCE_WOLF)) return TRUE;
    else if (IS_STANCE(ch, STANCE_XKILLA)) return TRUE;
    return FALSE;
}

void update_damcap( CHAR_DATA *ch, CHAR_DATA *victim )
{
int max_dam = 1000;


    if (!IS_NPC(ch))
    {

	if (IS_CLASS(ch, CLASS_DROW))
	{
		if (weather_info.sunlight == SUN_DARK)
		max_dam += 500;
		
		if (IS_SET(ch->newbits, NEW_DROWHATE))
		max_dam += 750;
	}
else if ((IS_CLASS(ch, CLASS_DEMON) || IS_SET(ch->special,
SPC_CHAMPION)) &&
	    ch->in_room != NULL)
	{
	    if (ch->in_room->vnum == 30000) max_dam = 10000; /* was5000Rotain*/
	    max_dam += (ch->pcdata->stats[DEMON_POWER] * 20);
	}	

	if (IS_CLASS(ch, CLASS_MONK))
	    max_dam +=  (ch->monkab[COMBAT] * 100);
	if (IS_CLASS(ch, CLASS_MONK) && ch->chi[CURRENT] > 0 )
	    max_dam += ch->chi[CURRENT] * 200;

	else if (IS_CLASS(ch, CLASS_VAMPIRE))
	{
	     max_dam = 1500;

	    max_dam += (ch->rage * 10);
            if (ch->power[DISC_VAMP_POTE] > 0) 
	         max_dam += (ch->power[DISC_VAMP_POTE] * 125 * 2);
	    if (ch->pcdata->rank == AGE_METHUSELAH) max_dam += 400;
	    else if (ch->pcdata->rank == AGE_ELDER) max_dam += 250;
	    else if (ch->pcdata->rank == AGE_ANCILLA) max_dam += 125;
	
	if (ch->generation == 13)
          max_dam += 75;
         else if (ch->generation ==12)
          max_dam += 100;
         else if (ch->generation ==11)          
         max_dam += 150;
          else if (ch->generation == 10)
          max_dam += 200;
          else if (ch->generation == 9)
          max_dam += 250;
  	else if (ch->generation == 8)
          max_dam += 300;
          else if (ch->generation == 7)
          max_dam += 350;
        else if (ch->generation == 6)
          max_dam += 400;
        else if (ch->generation == 5)
          max_dam += 450;
        else if (ch->generation  == 4)
          max_dam += 500;
	else if (ch->generation  == 3)
          max_dam += 550;
	else if (ch->generation  == 2)
          max_dam += 600;


	}
	else if (IS_CLASS(ch, CLASS_NINJA)) 
	{
	max_dam += (ch->rage * 5);
	if(ch->pcdata->powers[NPOWER_CHIKYU] >=6 &&
	ch->pcdata->powers[HARA_KIRI] > 0) 
	max_dam += 500;
	if (ch->pcdata->powers[NPOWER_CHIKYU] >=2)
	max_dam += 500;
	}
	else if (IS_SET(ch->special, SPC_WOLFMAN))
	{
	    if      (ch->generation < 2) max_dam = 2000;
	    else if (ch->generation < 3) max_dam = 1900;
	    else if (ch->generation < 4) max_dam = 1800;
	    else                      max_dam = 1700;

	    max_dam += ch->rage;

          if (ch->power[DISC_WERE_LUNA] > 6
	    && (weather_info.sunlight == SUN_DARK))
            max_dam += 400;
	}
        
	else if (IS_CLASS(ch, CLASS_DEMON)) max_dam += 500;
	else if (IS_SET(ch->special, SPC_CHAMPION)) max_dam += 250;
	
	if (ch->pcdata->souls >= 10)
	max_dam += 500;

	
	if ( ch->generation > 0 && IS_CLASS(ch, CLASS_DEMON))
	{
  	  max_dam += (ch->generation * 20);
        }

	else if (IS_CLASS(ch, CLASS_HIGHLANDER) && (get_eq_char(ch, WEAR_WIELD) != NULL))
        {
            int wpn = ch->wpn[1];
                    max_dam += wpn;
		max_dam += ch->race*40;
        }
   }
        if (IS_ITEMAFF(ch, ITEMA_ARTIFACT)) max_dam += 500;

	if (IS_ITEMAFF(victim, ITEMA_ARTIFACT)) max_dam += 500;

	if (IS_NPC(victim) || victim->stance[0] != STANCE_MONKEY )
	{
	    if      (ch->stance[0] == STANCE_BULL)   max_dam += 250;
	    else if (ch->stance[0] == STANCE_DRAGON) max_dam += 300;
            else if (ch->stance[0] == STANCE_WOLF)   max_dam += 500;
	    else if (ch->stance[0] == STANCE_TIGER)  max_dam += 350;
	    else if (ch->stance[0] == STANCE_XKILLA) max_dam += 750;
	}

    /* Max Dam Reductions */
    /* Werewolves */

    if (!IS_NPC(victim))
    {
	int silver_tol = (victim->siltol * 2.5);
	if (IS_CLASS(victim, CLASS_WEREWOLF) && 
	    IS_ITEMAFF(ch, ITEMA_RIGHT_SILVER))
	    max_dam += (250 - silver_tol);
	if (IS_CLASS(victim, CLASS_WEREWOLF) && 
	    IS_ITEMAFF(ch, ITEMA_LEFT_SILVER))
	    max_dam += (250 - silver_tol);
    	
        /* Vampies */
	if (!IS_NPC(ch) && (IS_CLASS(victim, CLASS_VAMPIRE)))
 	    max_dam -= (victim->power[DISC_VAMP_FORT] * 100); 

	if (IS_NPC(ch) || ch->stance[0] != STANCE_MONKEY)
	{
	    if      (victim->stance[0] == STANCE_CRAB)    max_dam -= 250;
	    else if (victim->stance[0] == STANCE_DRAGON)  max_dam -= 250;
	    else if (victim->stance[0] == STANCE_SWALLOW) max_dam -= 250;
	}

if ((IS_CLASS(victim,CLASS_DEMON) || IS_SET(victim->special,SPC_CHAMPION)) 
	    && victim->in_room != NULL && victim->in_room->vnum == 30000)
	    max_dam *= 0.2;
}

    if (max_dam < 3000) max_dam = 3000;
    if (ch->level >= LEVEL_BUILDER)
    max_dam = 30000;
    ch->damcap[DAM_CAP] = max_dam;
    ch->damcap[DAM_CHANGE] = 0;
    return;
}

int check_immune(CHAR_DATA *ch, int dam_type)
{
    int immune, def;
    int bit;

    immune = -1;
    def = IS_NORMAL;

    if (dam_type == DAM_NONE)
	return immune;

    /* Firstly, dDo the backbone for each race, and then do the powers,
	which are not at all race specific - so that ie, eladrin can
	learn protean and be immune to fire! */

    /* Eladrin aura deals with itself, as it is switched on and off
	and removes and sets the bits in both */

    if (ch->class == CLASS_VAMPIRE)
    {
	/* They are resistant to cold */
	if (dam_type == DAM_COLD) immune = IS_RESISTANT;
 	/* They are vulnerable to drowning */
	if (dam_type == DAM_DROWNING) immune = IS_VULNERABLE;
	/* They are vulnerable to fire */
	if (dam_type == DAM_FIRE) immune = IS_VULNERABLE;
    }

    /* Firstly, we'll do flesh of lava! */
    if (ch->power[DISC_VAMP_PROT] >= 7
        && dam_type == DAM_FIRE) immune = IS_RESISTANT;

    /* The corruption discipline */
    if (dam_type == DAM_POISON && ch->power[DISC_DAEM_CORR] >= 3)
	immune = IS_DIMMUNE;
    if (dam_type == DAM_POISON && ch->power[DISC_DAEM_CORR] >= 1)
	immune = IS_RESISTANT;

    /* The hellfire discipline */
    if (dam_type == DAM_FIRE && ch->power[DISC_DAEM_HELL] >= 2)
	immune = IS_DIMMUNE;
    if (dam_type == DAM_FIRE && ch->power[DISC_DAEM_HELL] >= 1)
	immune = IS_RESISTANT;

    /* The geluge discipline */
    if (dam_type == DAM_COLD && ch->power[DISC_DAEM_GELU] >= 4)
	immune = IS_DIMMUNE;
    if (dam_type == DAM_COLD && ch->power[DISC_DAEM_GELU] >= 1)
	immune = IS_RESISTANT;

    /* Fortitude level 1 for vampires */
    if (dam_type == DAM_POISON && ch->power[DISC_VAMP_FORT] >= 1)
	immune = IS_RESISTANT;

    /* Returns immune if it has been changed at all ! */
    if (immune != -1) return immune;

    
    /* set bits to check -- VULN etc. must ALL be the same or this will
fail */
    switch (dam_type)
    {
	case(DAM_BASH):         bit = DIMM_BASH;         break;
	case(DAM_PIERCE):       bit = DIMM_PIERCE;       break;
	case(DAM_SLASH):        bit = DIMM_SLASH;        break;
	case(DAM_FIRE):         bit = DIMM_FIRE;         break;
	case(DAM_COLD):         bit = DIMM_COLD;         break;
	case(DAM_LIGHTNING):    bit = DIMM_LIGHTNING;    break;
	case(DAM_ACID):         bit = DIMM_ACID;         break;
	case(DAM_POISON):       bit = DIMM_POISON;       break;
	case(DAM_NEGATIVE):     bit = DIMM_NEGATIVE;     break;
	case(DAM_HOLY):         bit = DIMM_HOLY;         break;
	case(DAM_ENERGY):       bit = DIMM_ENERGY;       break;
	case(DAM_MENTAL):       bit = DIMM_MENTAL;       break;
	case(DAM_DISEASE):      bit = DIMM_DISEASE;      break;
	case(DAM_DROWNING):     bit = DIMM_DROWNING;     break;
	case(DAM_LIGHT):        bit = DIMM_LIGHT;        break;
	case(DAM_CHARM):        bit = DIMM_CHARM;        break;
	case(DAM_SOUND):        bit = DIMM_SOUND;        break;
	default:                return def;
    }


    if (immune == -1)
	return def;
    else
	return immune;
}


/*
 * Inflict damage from a hit.
 */
bool damage_old( CHAR_DATA *ch, CHAR_DATA *victim, int dam, int dt, int dam_type, bool show ) 
{
    OBJ_DATA *corpse;
    bool immune;


    if ( victim->position == POS_DEAD )
	return FALSE;
    
    /* damage reduction */
    if ( dam > 35)
	dam = (dam - 35)/2 + 35; 
    if ( dam > 80)
	dam = (dam - 80)/2 + 80; 
   
    if ( victim != ch )
    {
	/*
	 * Certain attacks are forbidden.
	 * Most other attacks are returned.
	 */
	if ( is_safe( ch, victim ) )
	    return FALSE;

	if ( victim->position > POS_STUNNED )
	{
	    /*
	     * If victim is charmed, ch might attack victim's master.
	     */
	    if ( IS_NPC(ch)         
	    &&   IS_NPC(victim)
	    &&   IS_AFFECTED(victim, AFF_CHARM)
	    &&   victim->master != NULL
	    &&   victim->master->in_room == ch->in_room
	    &&   number_bits( 3 ) == 0 )
	    {
		stop_fighting( ch, FALSE );
		
		set_fighting( ch, victim->master);
		return FALSE;
	    }
	}

	/*
	 * More charm stuff.
	 */
	if ( victim->master == ch )
	    stop_follower( victim );
    }

    /*
     * Inviso attacks ... not.
     */
    if ( IS_AFFECTED(ch, AFF_INVISIBLE) )
    {
	affect_strip( ch, gsn_invis );
	affect_strip( ch, gsn_mass_invis );
	REMOVE_BIT( ch->affected_by, AFF_INVISIBLE );
	act( "$n fades into existence.", ch, NULL, NULL, TO_ROOM );
    }

    /*
     * Damage modifiers.
     */

    if ( dam > 1 && !IS_NPC(victim)
    &&   victim->pcdata->condition[COND_DRUNK]  > 10 )
	dam = 9 * dam / 10;
 
    immune = FALSE;


    switch(check_immune(victim,dam_type))
    {
	case(IS_DIMMUNE):
	    immune = TRUE;
	    dam = 0;
	    break;
	case(IS_RESISTANT):     
	    dam -= dam/3;
	    break;
	case(IS_VULNERABLE):
	    dam += dam/2;
	    break;
    }

    if (show)
	dam_message( ch, victim, dam, dt);

    if (dam == 0)
	return FALSE;

    /*
     * Hurt the victim.
     * Inform the victim of his new state.
     */
    victim->hit -= dam;
    if ( !IS_NPC(victim)
    &&   victim->level >= LEVEL_IMMORTAL
    &&   victim->hit < 1 )
	victim->hit = 1;
    update_pos( victim );

    switch( victim->position )

    {
    case POS_MORTAL:
	act( "$n is mortally wounded, and will die soon, if not aided.",
	    victim, NULL, NULL, TO_ROOM );
	send_to_char( 
	    "You are mortally wounded, and will die soon, if not aided.\n\r",
	    victim );
	break;

    case POS_INCAP:
	act( "$n is incapacitated and will slowly die, if not aided.",
	    victim, NULL, NULL, TO_ROOM );
	send_to_char(
	    "You are incapacitated and will slowly die, if not aided.\n\r",
	    victim );
	break;

    case POS_STUNNED:
	act( "$n is stunned, but will probably recover.",
	    victim, NULL, NULL, TO_ROOM );
	send_to_char("You are stunned, but will probably recover.\n\r",
	    victim );
	break;

    case POS_DEAD:
	act( "$n is DEAD!!", victim, 0, 0, TO_ROOM );
	send_to_char( "You have been KILLED!!\n\r\n\r", victim );
	stop_fighting(ch, TRUE);
	break;

    default:
	if ( dam > victim->max_hit / 4 )
	    send_to_char( "That really did HURT!\n\r", victim );
	if ( victim->hit < victim->max_hit / 4 )
	    send_to_char( "#1You sure are BLEEDING!\n\r", victim );
	break;
    }

    /*
     * Sleep spells and extremely wounded folks.
     */
    if (victim->position == POS_DEAD && !IS_NPC(victim) && victim->class !=0 )
    {
	stop_fighting(victim, TRUE);
	return TRUE;
    }
    else
    {
	update_pos(victim);
    }

    if ( !IS_AWAKE(victim) )
	stop_fighting( victim, FALSE );

    /*
     * Payoff for killing things.
     */

    if ( victim->position == POS_DEAD && victim->class==0)
    {
	
	group_gain( ch, victim );

	if (IS_NPC(victim) && !IS_NPC(ch) )
	    ch->mkill += 1;	


	if ( !IS_NPC(victim) )
	{    
	    if (IS_NPC(ch))
	        victim->mdeath += 1;
	    sprintf( log_buf, "%s killed by %s at %d",
		victim->name,
		(IS_NPC(ch) ? ch->short_descr : ch->name),
		victim->in_room->vnum );
	    log_string( log_buf );
	}
	
	sprintf( log_buf, "%s got crapped upon by %s at %s [room %d]",
	    (IS_NPC(victim) ? victim->short_descr : victim->name),
	    (IS_NPC(ch) ? ch->short_descr : ch->name),
	    ch->in_room->name, ch->in_room->vnum);
		    
	raw_kill(victim);

/*     	if (!IS_NPC(ch) && !IS_NPC(victim) && victim->pcdata->bounty > 0)
	{
    	sprintf(buf,"You recive a %d QP bounty, for killing %s.\n\r",
    	victim->pcdata->bounty, victim->name);
    	send_to_char(buf, ch);
    	ch->pcdata->quest += victim->pcdata->bounty;
    	victim->pcdata->bounty =0;	
	}*/

	if ( !IS_NPC(ch) && IS_NPC(victim) )
	{
	    corpse = get_obj_list( ch, "corpse", ch->in_room->contents );
	 
	    if ( IS_SET(ch->act, PLR_AUTOLOOT) &&
		 corpse && corpse->contains) /* exists and not empty */
		do_get( ch, "all corpse" );
      
	      if ( IS_SET(ch->act,PLR_AUTOLOOT) && corpse && corpse->contains)
		return TRUE;  /* leave if corpse has treasure */
	      else
		do_sacrifice( ch, "corpse" );
	}

	return TRUE;
     }

    if ( victim == ch )
	return TRUE;

    /*
     * Wimp out?
     */

    if ( !IS_NPC(victim)
    &&   victim->hit > 0
    &&   victim->hit <= victim->wimpy
    &&   victim->wait < PULSE_VIOLENCE / 2 )
	do_flee( victim, "" );

    tail_chain( );
    return TRUE;
}

/*
 * Inflict damage from a hit.
 */
void damage( CHAR_DATA *ch, CHAR_DATA *victim, int dam, int dt )
{
    CHAR_DATA *emb;

    if ( victim->position == POS_DEAD )
	return;

    if ( ch->embracing != NULL )
    {
      if ( ( emb = ch->embracing ) != NULL ) stop_embrace(ch,emb);
    else stop_embrace(ch,NULL);
    }

    /*
     * Stop up any residual loopholes.
     */

    if (ch->damcap[DAM_CHANGE] == 1) update_damcap(ch,victim);


    if (IS_NPC(victim) && dam > 2750)
	  dam = number_range( 2600, 2900 );

    if ( victim != ch )
    {
	/*
	 * Certain attacks are forbidden.
	 * Most other attacks are returned.
	 */
	if ( is_safe( ch, victim ) )
	    return;
	check_killer( ch, victim );

	if ( victim->position > POS_STUNNED )
	{
	    if ( victim->fighting == NULL )
		set_fighting( victim, ch );
	    victim->position = POS_FIGHTING;
	}

	if ( victim->position > POS_STUNNED )
	{
	    if ( ch->fighting == NULL )
		set_fighting( ch, victim );

	    /*
	     * If victim is charmed, ch might attack victim's master.
	     */
	    if ( IS_NPC(ch)
	    &&   IS_NPC(victim)
	    &&   IS_AFFECTED(victim, AFF_CHARM)
	    &&   victim->master != NULL
	    &&   victim->master->in_room == ch->in_room
	    &&   number_bits( 3 ) == 0 )
	    {
		stop_fighting( ch, FALSE );
		multi_hit( ch, victim->master, TYPE_UNDEFINED );
		return;
	    }
	}

	/*
	 * More charm stuff.
	 */
	if ( victim->master == ch )
	    stop_follower( victim );

	/*
	 * Damage modifiers.
	 */
	if ( IS_AFFECTED(ch, AFF_HIDE) )
	{
	    if (!can_see(victim,ch)) {dam *= 1.5;
		send_to_char("You use your concealment to get a surprise attack!\n\r",ch);}
	    REMOVE_BIT( ch->affected_by, AFF_HIDE );
	    act( "$n leaps from $s concealment.", ch, NULL, NULL, TO_ROOM );
	}

	if ( IS_AFFECTED(victim, AFF_PROTECT) && IS_EVIL(ch) && dam > 1  )
	    dam -= dam * 0.25;

	if ( dam < 0 )
	    dam = 0;

	/*
	 * Check for disarm, trip, parry, and dodge.
	 */
	if ( dt >= TYPE_HIT )
	{
	    if ( IS_NPC(ch) && number_percent( ) < ch->level * 0.5 )
		disarm( ch, victim );
	    if ( IS_NPC(ch) && number_percent( ) < ch->level * 0.5 )
		trip( ch, victim );
	    if ( check_parry( ch, victim, dt ) )
		return;
	    if ( !IS_NPC(victim) && IS_STANCE(victim, STANCE_CRANE) && 
		victim->stance[STANCE_CRANE] > 100 && !can_counter(ch)
		&& !can_bypass(ch,victim) && check_parry( ch, victim, dt ))
		return;
	    else if ( !IS_NPC(victim) && IS_STANCE(victim, STANCE_MANTIS) && 
		victim->stance[STANCE_MANTIS] > 100 && !can_counter(ch)
		&& !can_bypass(ch,victim) && check_parry( ch, victim, dt ))
		return;
	    if ( check_dodge( ch, victim, dt ) )
		return;
	    if ( !IS_NPC(victim) && IS_STANCE(victim, STANCE_MONGOOSE) && 
		victim->stance[STANCE_MONGOOSE] > 100 && !can_counter(ch)
		&& !can_bypass(ch,victim) && check_dodge( ch, victim, dt ))
		return;
	    else if ( !IS_NPC(victim) && IS_STANCE(victim, STANCE_SWALLOW) && 
		victim->stance[STANCE_SWALLOW] > 100 && !can_counter(ch)
		&& !can_bypass(ch,victim) && check_dodge( ch, victim, dt ))
		return;
	}

	dam_message( ch, victim, dam, dt );
    }
    if ( IS_CLASS( victim, CLASS_MONK ) && victim->monkab[SPIRIT] >= 2 )
    {
	if (IS_SET(victim->newbits, NEW_NATURAL))
        {
	  stc("You absorb the natural imbalances of your opponent.\n\r",victim);
	  act("$n absorbs the natural imbalances of you attack.",victim,NULL,ch,TO_VICT);
	  if ( dam > 2000 ) dam /= 2;
	  victim->hit += dam;
	  REMOVE_BIT(victim->newbits, NEW_NATURAL);
          return;
	}
    }

    hurt_person(ch,victim,dam);
    return;
}

void adv_damage( CHAR_DATA *ch, CHAR_DATA *victim, int dam )
{
    if ( victim->position == POS_DEAD )
	return;

    if ( dam > 1000 ) dam = 1000;

    if ( victim != ch )
    {
	if ( is_safe( ch, victim ) )
	    return;
	check_killer( ch, victim );

	if ( victim->position > POS_STUNNED )
	{
	    if ( victim->fighting == NULL )
		set_fighting( victim, ch );
	    victim->position = POS_FIGHTING;
	}

	if ( victim->position > POS_STUNNED )
	{
	    if ( ch->fighting == NULL )
		set_fighting( ch, victim );

	    /*
	     * If victim is charmed, ch might attack victim's master.
	     */
	    if ( IS_NPC(ch)
	    &&   IS_NPC(victim)
	    &&   IS_AFFECTED(victim, AFF_CHARM)
	    &&   victim->master != NULL
	    &&   victim->master->in_room == ch->in_room
	    &&   number_bits( 3 ) == 0 )
	    {
		stop_fighting( ch, FALSE );
		multi_hit( ch, victim->master, TYPE_UNDEFINED );
		return;
	    }
	}

	if ( victim->master == ch )
	    stop_follower( victim );

	if ( IS_AFFECTED(victim, AFF_PROTECT) && IS_EVIL(ch) && dam > 1  )
	    dam -= dam / 4;
	if (!IS_NPC(victim) && !IS_NPC(ch)) dam = dam *2/3;

	if ( dam < 0 )
	    dam = 0;
    }
    return;
}


void hurt_person( CHAR_DATA *ch, CHAR_DATA *victim, int dam )
{
    char buf[MAX_STRING_LENGTH];
    bool is_npc = FALSE;
    int vnum;

    /*
     * Hurt the victim.
     * Inform the victim of his new state.
     */
    victim->hit -= dam;
    if ( !IS_NPC(victim)
    &&   victim->level >= LEVEL_IMMORTAL
    &&   victim->hit < 1 )
	victim->hit = 1;

    if ( !IS_NPC(victim)
    &&   IS_SET(victim->newbits, NEW_CLOAK)
    &&   victim->hit < 1 )
      {
	  victim->hit += (victim->max_hit * .25);
	  REMOVE_BIT(victim->newbits, NEW_CLOAK);
      }

    if ( !IS_NPC(victim) && IS_CLASS(ch, CLASS_DROW)
    &&    IS_SET(victim->newbits, NEW_DARKNESS)
    &&    victim->hit < 1 )
    {
        if (IS_SET(victim->in_room->room_flags, ROOM_TOTAL_DARKNESS))
        REMOVE_BIT(victim->in_room->room_flags, ROOM_TOTAL_DARKNESS);
    }

    if (victim->hit < 0 && IS_SET(victim->in_room->room_flags, ROOM_ARENA))
      check_arena(ch, victim);
    update_pos( victim );

    switch( victim->position )
    {
    case POS_MORTAL:
	act( "$n is mortally wounded, and spraying blood everywhere.",
	    victim, NULL, NULL, TO_ROOM );
	send_to_char( 
	    "You are mortally wounded, and spraying blood everywhere.\n\r",
	    victim );
	break;

    case POS_INCAP:
	act( "$n is incapacitated, and bleeding badly.",
	    victim, NULL, NULL, TO_ROOM );
	send_to_char(
	    "You are incapacitated, and bleeding badly.\n\r",
	    victim );
	break;

    case POS_STUNNED:
	act( "$n is stunned, but will soon recover.",
	    victim, NULL, NULL, TO_ROOM );
	send_to_char("You are stunned, but will soon recover.\n\r",
	    victim );
	break;

    case POS_DEAD:
	act( "$n is DEAD!!", victim, 0, 0, TO_ROOM );
	send_to_char( "You have been KILLED!!\n\r\n\r", victim );
	break;

    default:
	if ( dam > victim->max_hit / 4 )
	    send_to_char( "That really did HURT!\n\r", victim );
	if ( victim->hit < victim->max_hit / 4 && dam > 0 )
	{
	    if (!IS_NPC(victim) && IS_CLASS(victim, CLASS_VAMPIRE)
		&& number_percent() < victim->beast)
		vamp_rage(victim);
	    else
		send_to_char( "You sure are BLEEDING!\n\r", victim );
	}
	break;
    }

    /*
     * Sleep spells and extremely wounded folks.
     */
    if ( !IS_AWAKE(victim) )
	stop_fighting( victim, FALSE );


    /*
     * Payoff for killing things.
     */
    if ( victim->position == POS_DEAD )
    {
	group_gain( ch, victim );

	if ( !IS_NPC(victim) )
	{
	    sprintf( log_buf, "%s killed by %s at %d",
		victim->name,
		(IS_NPC(ch) ? ch->short_descr : ch->name),
		victim->in_room->vnum );
	    log_string( log_buf );
	

	    /*
	     * Dying penalty:
	     * 1/2 your current exp.
	     */
	    if ( victim->exp > 0 )
		victim->exp = victim->exp / 2;
	}

    	if (IS_NPC(victim) && !IS_NPC(ch))
	{
	    ch->mkill += 1;
	
	    if (IS_CLASS(ch, CLASS_DEMON) || IS_SET(ch->special, SPC_CHAMPION)
		|| IS_CLASS(ch, CLASS_DROW))
	    {
		if ((vnum = victim->pIndexData->vnum) > 29000)
		{
		    switch (vnum)
		    {
		    default:
			if (IS_CLASS(ch, CLASS_DEMON) || IS_SET(ch->special,SPC_CHAMPION)) {
			ch->pcdata->stats[DEMON_CURRENT] +=victim->level*2;
			ch->pcdata->stats[DEMON_TOTAL] +=victim->level*2;}
			else
			ch->pcdata->stats[DROW_POWER] += victim->level*2;
			break;
		    case 29600:
		    case 30001:
		    case 30006:
		    case 30007:
		    case 30008:
		    case 30009:
                case 30000:
			break;
		    }
		}

		else if (IS_NPC(victim) && !IS_SET(victim->act, ACT_NOEXP))
		{
		if (IS_CLASS(ch, CLASS_DEMON) || IS_SET(ch->special,SPC_CHAMPION)) {
		    ch->pcdata->stats[DEMON_CURRENT] += victim->level*2;
		    ch->pcdata->stats[DEMON_TOTAL] += victim->level*2;}
		else
		    ch->pcdata->stats[DROW_POWER] += victim->level*2;
		}
	    }
	    if (ch->level == 1 && ch->mkill > 4)
	    {
		ch->level = 2;
		do_save(ch,"");
	    }
	}
    	if (!IS_NPC(victim) && IS_NPC(ch)) 
        {
           victim->mdeath = victim->mdeath + 1;
        }

	raw_kill( victim );

/*     	if (!IS_NPC(ch) && !IS_NPC(victim) && victim->pcdata->bounty > 0)
	{
    	sprintf(buf,"You recive a %d QP bounty, for killing %s.\n\r",
    	victim->pcdata->bounty, victim->name);
    	send_to_char(buf, ch);
    	ch->pcdata->quest += victim->pcdata->bounty;
    	victim->pcdata->bounty =0;	
	}*/

	if ( IS_SET(ch->act, PLR_AUTOLOOT) )
	    do_get( ch, "all corpse" );
	else
	    do_look( ch, "in corpse" );

	if ( !IS_NPC(ch) && is_npc )
	{
	    if ( IS_SET(ch->act, PLR_AUTOSAC) )
		do_sacrifice( ch, "corpse" );
	}

	return;
    }

    if ( victim == ch )
	return;


    if ( !IS_NPC(victim) && victim->desc == NULL )
    {
	if ( number_range( 0, victim->wait ) == 0 )
	{
	    do_recall( victim, "" );
	    return;
	}
    }

/*
    if ( IS_NPC(victim) && dam > 0 )
    {
	if ( ( IS_SET(victim->act, ACT_WIMPY) && number_bits( 1 ) == 0
	&&   victim->hit < victim->max_hit / 2 )
	||   ( IS_AFFECTED(victim, AFF_CHARM) && victim->master != NULL
	&&     victim->master->in_room != victim->in_room ) )
	    do_flee( victim, "" );
    }
*/

    if ( !IS_NPC(victim)
    &&   victim->hit > 0
    &&   victim->hit <= victim->wimpy
    &&   victim->wait == 0 )
	do_flee( victim, "" );

    tail_chain( );
    return;
}



bool is_safe( CHAR_DATA *ch, CHAR_DATA *victim )
{

  if (!IS_NPC(victim) && !IS_NPC(ch))
  {
    if ( !CAN_PK(ch) || !CAN_PK(victim) )
    {
        send_to_char( "Both players must be avatars to fight.\n\r", ch );
        return TRUE;
    }  
  }
	if (victim->fight_timer > 0) return FALSE;

     if ( !IS_NPC(ch ) )
     {
	if (IS_AFFECTED(ch, AFF_PEACE) && ch->fighting != NULL)
	REMOVE_BIT(ch->affected_by, AFF_PEACE);
     }
     if ( !IS_NPC(victim) )
     {
	if (IS_AFFECTED(victim, AFF_PEACE) && victim->fighting != NULL)
	REMOVE_BIT(victim->affected_by, AFF_PEACE);
     }

    /* Ethereal people can only attack other ethereal people */
    if ( IS_AFFECTED(ch, AFF_ETHEREAL) && !IS_AFFECTED(victim, AFF_ETHEREAL) )
    {
	send_to_char( "You cannot while ethereal.\n\r", ch );
	return TRUE;
    }
    if ( !IS_AFFECTED(ch, AFF_ETHEREAL) && IS_AFFECTED(victim, AFF_ETHEREAL) )
    {
	send_to_char( "You cannot fight an ethereal person.\n\r", ch );
	return TRUE;
    }

    /* You cannot attack across planes */
    if ( IS_AFFECTED(ch, AFF_SHADOWPLANE) && !IS_AFFECTED(victim, AFF_SHADOWPLANE) )
    {
	act( "You are too insubstantial!", ch, NULL, victim, TO_CHAR );
	return TRUE;
    }
    if ( !IS_AFFECTED(ch, AFF_SHADOWPLANE) && IS_AFFECTED(victim, AFF_SHADOWPLANE) )
    {
	act( "$E is too insubstantial!", ch, NULL, victim, TO_CHAR );
	return TRUE;
    }

    if ( IS_SET(ch->in_room->room_flags,ROOM_SAFE) )
    {
	send_to_char( "You cannot fight in a safe room.\n\r", ch );
	return TRUE;
    }

    /* ARENA room checking */
    if (  IS_SET(ch->in_room->room_flags,ROOM_ARENA)
       && IS_SET(victim->in_room->room_flags,ROOM_ARENA) )
        return FALSE;


    if ( IS_HEAD(ch,LOST_HEAD) || IS_EXTRA(ch,EXTRA_OSWITCH) )
    {
	send_to_char( "Objects cannot fight!\n\r", ch );
	return TRUE;
    }
    else if ( IS_HEAD(victim,LOST_HEAD) || IS_EXTRA(victim,EXTRA_OSWITCH))
    {
	send_to_char( "You cannot attack an object.\n\r", ch );
	return TRUE;
    }

    if ( IS_NPC(ch) || IS_NPC(victim) )
	return FALSE;

    /* Thx Josh! */
    if ( victim->fighting == ch )
	return FALSE;

    if (IS_AFFECTED(ch, AFF_PEACE))
    {
	send_to_char( "You are unable to attack them.\n\r", ch );
        return TRUE;
    }

    if (IS_AFFECTED(victim, AFF_PEACE))
    {
	send_to_char( "You can't seem to attack them.\n\r", ch );
        return TRUE;
    }

    if (IS_ITEMAFF(ch, ITEMA_PEACE))
    {
	send_to_char( "You are unable to attack them.\n\r", ch );
	return TRUE;
    }

    if (IS_ITEMAFF(victim, ITEMA_PEACE))
    {
	send_to_char( "You can't seem to attack them.\n\r", ch );
	return TRUE;
    }
	return FALSE;
}

bool no_attack( CHAR_DATA *ch, CHAR_DATA *victim )
{


  if (!IS_NPC(ch) && !IS_NPC(victim))
  {
    if ( !CAN_PK(ch) || !CAN_PK(victim) )
    {
        send_to_char( "Both players must be avatars to fight.\n\r", ch );
        return TRUE;
    }     
  }
    if (victim->fight_timer > 0)
   	return FALSE;

    /* Ethereal people can only attack other ethereal people */
    if ( IS_AFFECTED(ch, AFF_ETHEREAL) && !IS_AFFECTED(victim, AFF_ETHEREAL) )
    {
	send_to_char( "You cannot while ethereal.\n\r", ch );
	return TRUE;
    }
    if ( !IS_AFFECTED(ch, AFF_ETHEREAL) && IS_AFFECTED(victim, AFF_ETHEREAL) )
    {
	send_to_char( "You cannot fight an ethereal person.\n\r", ch );
	return TRUE;
    }

    /* You cannot attack across planes */
    if ( IS_AFFECTED(ch, AFF_SHADOWPLANE) && !IS_AFFECTED(victim, AFF_SHADOWPLANE) )
    {
	act( "You are too insubstantial!", ch, NULL, victim, TO_CHAR );
	return TRUE;
    }
    if ( !IS_AFFECTED(ch, AFF_SHADOWPLANE) && IS_AFFECTED(victim, AFF_SHADOWPLANE) )
    {
	act( "$E is too insubstantial!", ch, NULL, victim, TO_CHAR );
	return TRUE;
    }

    if ( IS_SET(ch->in_room->room_flags,ROOM_SAFE) )
    {
	send_to_char( "You cannot fight in a safe room.\n\r", ch );
	return TRUE;
    }

    if ( IS_HEAD(ch,LOST_HEAD) || IS_EXTRA(ch,EXTRA_OSWITCH) )
    {
	send_to_char( "Objects cannot fight!\n\r", ch );
	return TRUE;
    }
    else if ( IS_HEAD(victim,LOST_HEAD) || IS_EXTRA(victim,EXTRA_OSWITCH) )
    {
	send_to_char( "You cannot attack objects.\n\r", ch );
	return TRUE;
    }

    return FALSE;
}



/*
 * See if an attack justifies a KILLER flag.
 */
void check_killer( CHAR_DATA *ch, CHAR_DATA *victim )
{
    /*
     * Follow charm thread to responsible character.
     * Attacking someone's charmed char is hostile!
     */
    while ( IS_AFFECTED(victim, AFF_CHARM) && victim->master != NULL )
	victim = victim->master;

    /*
     * NPC's are fair game.
     * So are killers and thieves.
     */
    if ( IS_NPC(victim) )
	return;

    /*
     * Charm-o-rama.
     */
    if ( IS_SET(ch->affected_by, AFF_CHARM) )
    {
	if ( ch->master == NULL )
	{
	    affect_strip( ch, gsn_charm_person );
	    REMOVE_BIT( ch->affected_by, AFF_CHARM );
	    return;
	}
	stop_follower( ch );
	return;
    }
    return;
}



/*
 * Check for parry.
 */
bool check_parry( CHAR_DATA *ch, CHAR_DATA *victim, int dt )
{
    OBJ_DATA *obj = NULL;
    int chance = 0;
    bool claws = FALSE;

    if ( !IS_AWAKE(victim) )
	return FALSE;

    if ( IS_NPC(victim) )
	obj = NULL;
    else if ( IS_CLASS(victim, CLASS_WEREWOLF)	&& 
	victim->power[DISC_WERE_BEAR] > 2	&& 
	IS_VAMPAFF(victim, VAM_CLAWS)		&&
	get_eq_char(victim, WEAR_WIELD) == NULL	&&
	get_eq_char(victim, WEAR_HOLD) == NULL)
    {
	obj = NULL;
	claws = TRUE;
    }
    else if ( IS_CLASS(victim, CLASS_MONK) &&
	      IS_SET(victim->newbits, NEW_MONKADAM) &&
	      get_eq_char(victim, WEAR_WIELD) == NULL &&
	      get_eq_char(victim, WEAR_HOLD) == NULL)
    {
	obj = NULL;
	claws = TRUE;
    }

    else
    {
	if ( ( obj = get_eq_char( victim, WEAR_WIELD ) ) == NULL 
		|| obj->item_type != ITEM_WEAPON )
	{
	    if ( ( obj = get_eq_char( victim, WEAR_HOLD ) ) == NULL 
		|| obj->item_type != ITEM_WEAPON ) return FALSE;
	}
    }

    if (dt < 1000 || dt > 1012) return FALSE;
    if (!IS_NPC(ch))   chance -= (ch->wpn[dt-1000] * 0.1);
	else             chance -= (ch->level * 0.2);
    if (!IS_NPC(victim)) chance += (victim->wpn[dt-1000] * 0.5);
	else               chance += victim->level;

    if (!IS_NPC(victim) && IS_STANCE(victim, STANCE_CRANE) &&
	victim->stance[STANCE_CRANE] > 0 && !can_counter(ch) &&
	!can_bypass(ch,victim))
	chance += (victim->stance[STANCE_CRANE] * 0.25);

    else if (!IS_NPC(victim) && IS_STANCE(victim, STANCE_MANTIS) &&
	victim->stance[STANCE_MANTIS] > 0 && !can_counter(ch) &&
	!can_bypass(ch,victim))
	chance += (victim->stance[STANCE_MANTIS] * 0.25);

      chance -= (char_hitroll(ch) * 0.1);

    if (claws) 
    {
	    if (victim->power[DISC_WERE_LYNX] > 3)
	       chance += (char_hitroll(victim) * 0.1);
	    else
	       chance += (char_hitroll(victim) * 0.075);
    }


    if (!IS_NPC(ch))
    {
           if (IS_CLASS(ch, CLASS_VAMPIRE))
           {
	         if (ch->power[DISC_VAMP_CELE] > 0)
               chance -= (ch->power[DISC_VAMP_CELE]*4);
           }
	if (IS_CLASS(ch, CLASS_NINJA)  && ch->pcdata->powers[NPOWER_CHIKYU] >= 6
	&& ch->pcdata->powers[HARA_KIRI] > 0) 
	chance -= 25;
	if (ch->pcdata->powers[NPOWER_CHIKYU] >= 5 && IS_CLASS(ch, CLASS_NINJA))
	chance -= 25;
	else if (IS_CLASS(ch, CLASS_DEMON) && IS_DEMPOWER(ch, DEM_SPEED) )
	    chance -= 25;
        if ( IS_CLASS(ch, CLASS_MONK) && ch->chi[CURRENT] > 0 )
	    chance -= (ch->chi[CURRENT] * 7);
	if (IS_CLASS(ch, CLASS_DROW) && IS_SET(ch->pcdata->powers[1], DPOWER_ARMS))
		chance -= 30;
	if (IS_SET(ch->newbits, NEW_MONKFLAME))
	    chance -= 3;
        if (IS_CLASS(ch, CLASS_WEREWOLF) &&
	    ch->power[DISC_WERE_MANT] < 11 )
	    chance -= (ch->power[DISC_WERE_MANT] * 4);
    }

    if (!IS_NPC(victim))
    {
	if (victim->pcdata->powers[NPOWER_CHIKYU] >= 5 && IS_CLASS(victim, CLASS_NINJA))
	chance += 25;
        if (victim->power[DISC_VAMP_CELE] > 0 && 
	    IS_CLASS(victim, CLASS_VAMPIRE)) 
            chance += (victim->power[DISC_VAMP_CELE]*4);
	if (IS_CLASS(victim, CLASS_DROW) && IS_SET(victim->pcdata->powers[1], DPOWER_ARMS))
		chance += 30;
	if (IS_CLASS(victim, CLASS_MONK) && ch->chi[CURRENT] > 0 )
		chance += (ch->chi[CURRENT] * 8);
	else if (IS_CLASS(ch, CLASS_DEMON) && IS_DEMPOWER(ch, DEM_SPEED) )
	    chance -= 25;
	if (IS_SET(victim->newbits, NEW_MONKFLAME))
	   chance += 4;
    }

    if      (chance > 80) chance = 80;
    else if (chance < 20) chance = 20;

    if (!IS_NPC(ch) && ch->power[DISC_VAMP_CELE] > 0 && IS_CLASS(ch, CLASS_VAMPIRE))
    {
	if (ch->pcdata->rank == AGE_METHUSELAH) chance -= 15;
	else if (ch->pcdata->rank == AGE_ELDER) chance -= 10;
	else if (ch->pcdata->rank == AGE_ANCILLA) chance -= 5;
    }

    if (!IS_NPC(victim) && victim->power[DISC_VAMP_CELE] > 0 && 
	IS_CLASS(victim, CLASS_VAMPIRE))
    {
	if (victim->pcdata->rank == AGE_METHUSELAH) chance += 15;
	else if (victim->pcdata->rank == AGE_ELDER) chance += 10;
	else if (victim->pcdata->rank == AGE_ANCILLA) chance += 5;
    }

    if ( !IS_NPC(victim) && IS_CLASS(victim, CLASS_MONK) )
    {
     if ( victim->monkblock / 4 > 0 )
	chance += victim->monkblock / 4;
    }

    if ( number_percent( ) < 100 && number_percent( ) >= chance )
	return FALSE;

    if (claws)
    {
	if (IS_NPC(victim) || !IS_SET(victim->act, PLR_BRIEF))
	{
	  if (IS_CLASS(victim, CLASS_WEREWOLF))
	    act( "You parry $n's blow with your claws.",  ch, NULL, victim, TO_VICT );
	else
	    act( "You parry $n's blow with your Adamantium hands.", ch,NULL,victim, TO_VICT);
	}
	    if (!IS_NPC(victim) && IS_CLASS(victim, CLASS_MONK)
       		 && victim->monkblock < 150 )
	      victim->monkblock++;
        
	if (IS_NPC(ch) || !IS_SET(ch->act, PLR_BRIEF))
	{
	  if (IS_CLASS(victim, CLASS_WEREWOLF))
	    act( "$N parries your blow with $S claws.", ch, NULL, victim, TO_CHAR );
	else
	    act( "$N parries your blow with $S Adamantium claws.", ch,NULL,victim, TO_CHAR);
	}
	return TRUE;
    }
    if (!IS_NPC(victim) && obj != NULL && obj->item_type == ITEM_WEAPON &&
	obj->value[3] >= 0 && obj->value[3] <= 12)
    {
	if (IS_NPC(victim) || !IS_SET(victim->act, PLR_BRIEF))
	    act( "You parry $n's blow with $p.",  ch, obj, victim, TO_VICT );
	if (IS_NPC(ch) || !IS_SET(ch->act, PLR_BRIEF))
	    act( "$N parries your blow with $p.", ch, obj, victim, TO_CHAR );
	return TRUE;
    }
    if (IS_NPC(victim) || !IS_SET(victim->act, PLR_BRIEF))
	act( "You parry $n's attack.",  ch, NULL, victim, TO_VICT    );
    if (IS_NPC(ch) || !IS_SET(ch->act, PLR_BRIEF))
	act( "$N parries your attack.", ch, NULL, victim, TO_CHAR    );
    return TRUE;
}



/*
 * Check for dodge.
 */
bool check_dodge( CHAR_DATA *ch, CHAR_DATA *victim, int dt )
{
    int chance = 0;

    if ( !IS_AWAKE(victim) )
	return FALSE;

    if (!IS_NPC(ch))     chance -= (ch->wpn[dt-1000] * 0.1);
	else               chance -= (ch->level * 0.2);
    if (!IS_NPC(victim)) chance += (victim->wpn[0] * 0.5);
	else               chance += victim->level;

    if (!IS_NPC(victim) && IS_STANCE(victim, STANCE_MONGOOSE) &&
	victim->stance[STANCE_MONGOOSE] > 0 && !can_counter(ch) &&
	!can_bypass(ch,victim))
	(chance += victim->stance[STANCE_MONGOOSE] * 0.25);

    if (!IS_NPC(victim) && IS_STANCE(victim, STANCE_SWALLOW) &&
	victim->stance[STANCE_SWALLOW] > 0 && !can_counter(ch) &&
	!can_bypass(ch,victim))
	(chance += victim->stance[STANCE_SWALLOW] * 0.25);

    if (!IS_NPC(ch))
    {
	if (IS_CLASS(ch, CLASS_NINJA)  && ch->pcdata->powers[NPOWER_CHIKYU] >= 6
	&& ch->pcdata->powers[HARA_KIRI] > 0)
	chance -= 25;
	if (ch->pcdata->powers[NPOWER_CHIKYU] >= 5 && IS_CLASS(ch, CLASS_NINJA))
	chance -= 25;
        if (ch->power[DISC_VAMP_CELE] > 0 && IS_CLASS(ch, CLASS_VAMPIRE)) 
            chance -= (ch->power[DISC_VAMP_CELE]*4);
	else if (IS_CLASS(ch, CLASS_DEMON) && IS_DEMPOWER( ch, DEM_SPEED) )
	    chance -= 25;
	else if (IS_CLASS(ch, CLASS_WEREWOLF) &&
	    ch->power[DISC_WERE_MANT] < 11 )
	    chance -= (ch->power[DISC_WERE_MANT] * 4);
	else if (IS_CLASS(ch, CLASS_DROW) &&
	IS_SET(ch->pcdata->powers[1], DPOWER_SPEED))
	chance -= 25;
    }
    if (!IS_NPC(victim))
    {
	if (victim->pcdata->powers[NPOWER_CHIKYU] >= 5 && IS_CLASS(victim, CLASS_NINJA))
	chance += 30;
        if (victim->power[DISC_VAMP_CELE] > 0 && IS_CLASS(victim, CLASS_VAMPIRE)) 
            chance += (victim->power[DISC_VAMP_CELE]*4);
	else if (IS_CLASS(victim, CLASS_DEMON) && IS_DEMPOWER( victim, DEM_SPEED) )
	    chance += 25;
      else if (IS_CLASS(victim, CLASS_WEREWOLF) && victim->power[DISC_WERE_MANT] < 11)
            chance += (victim->power[DISC_WERE_MANT] * 5);
	else if (IS_CLASS(victim, CLASS_DROW) && 
	IS_SET(victim->pcdata->powers[1], DPOWER_SPEED))
	chance += 25;
    }

    if      (chance > 80) chance = 80;
    else if (chance < 20) chance = 20;

    if (!IS_NPC(ch) && IS_CLASS(ch, CLASS_VAMPIRE))
    {
	if (ch->pcdata->rank == AGE_METHUSELAH) chance -= 10;
	else if (ch->pcdata->rank == AGE_ELDER) chance -= 7;
	else if (ch->pcdata->rank == AGE_ANCILLA) chance -= 3;
    }
    if (!IS_NPC(victim) && IS_CLASS(victim, CLASS_VAMPIRE))
    {
	if (victim->pcdata->rank == AGE_METHUSELAH) chance += 10;
	else if (victim->pcdata->rank == AGE_ELDER) chance += 7;
	else if (victim->pcdata->rank == AGE_ANCILLA) chance += 3;
    }

    if (!IS_NPC(victim) && IS_CLASS(victim, CLASS_MONK) 
	&& ( victim->monkblock / 4 > 0 ) )
    chance += (victim->monkblock / 4);

    if ( number_percent( ) < 100 && number_percent( ) >= chance )
	return FALSE;

    if (IS_NPC(victim) || !IS_SET(victim->act, PLR_BRIEF))
	act( "You dodge $n's attack.", ch, NULL, victim, TO_VICT );
    if (IS_NPC(ch) || !IS_SET(ch->act, PLR_BRIEF))
	act( "$N dodges your attack.", ch, NULL, victim, TO_CHAR );
    return TRUE;
}



/*
 * Set position of a victim.
 */
void update_pos( CHAR_DATA *victim )
{
    CHAR_DATA *mount;
    if ( victim->hit > 0 )
    {
    	if ( victim->position <= POS_STUNNED )
	{
	    bool gm_stance = FALSE;
	    victim->position = POS_STANDING;
	    if (!IS_NPC(victim) && victim->stance[0] > 0)
	    {
		int stance = victim->stance[0];
		if (victim->stance[stance] >= 200) gm_stance = TRUE;
	    }
	    if (IS_NPC(victim) || victim->max_hit * 0.25 > victim->hit || 
		!gm_stance)
	    {
		act( "$n clambers back to $s feet.", victim, NULL, NULL, TO_ROOM);
		act( "You clamber back to your feet.", victim, NULL, NULL, TO_CHAR);
	    }
	    else
	    {
		act( "$n flips back up to $s feet.", victim, NULL, NULL, TO_ROOM);
		act( "You flip back up to your feet.", victim, NULL, NULL, TO_CHAR);
	    }
	}
	return;
    }
    else if ( (mount = victim->mount) != NULL)
    {
	if (victim->mounted == IS_MOUNT)
	{
	    act("$n rolls off $N.",mount,NULL,victim,TO_ROOM);
	    act("You roll off $N.",mount,NULL,victim,TO_CHAR);
	}
	else if (victim->mounted == IS_RIDING)
	{
	    act("$n falls off $N.",victim,NULL,mount,TO_ROOM);
	    act("You fall off $N.",victim,NULL,mount,TO_CHAR);
	}
	mount->mount    = NULL;
	victim->mount   = NULL;
	mount->mounted  = IS_ON_FOOT;
	victim->mounted = IS_ON_FOOT;
    }

    if (!IS_NPC(victim) && victim->hit <=-11 && IS_HERO(victim))
    {
        victim->hit = -10;
        if (victim->position == POS_FIGHTING) 
           stop_fighting(victim,TRUE);
        return;
    }

    if ( IS_NPC(victim) || victim->hit <= -11 )
    {
	victim->position = POS_DEAD;
	return;
    }

         if ( victim->hit <= -6 ) victim->position = POS_MORTAL;
    else if ( victim->hit <= -3 ) victim->position = POS_INCAP;
    else                          victim->position = POS_STUNNED;

    return;
}



/*
 * Start fights.
 */
void set_fighting( CHAR_DATA *ch, CHAR_DATA *victim )
{
    if ( ch->fighting != NULL )
    {
//	bug( "Set_fighting: already fighting", 0 );
	return;
    }

    if ( IS_AFFECTED(ch, AFF_SLEEP) )
	affect_strip( ch, gsn_sleep );

    ch->fighting = victim;
    ch->position = POS_FIGHTING;
    ch->damcap[DAM_CHANGE] = 1;

	autodrop(ch);
	autodrop(victim);

    return;
}



/*
 * Stop fights.
 */
void stop_embrace( CHAR_DATA *ch, CHAR_DATA *victim )
{
  if ( victim == NULL)
  {
    ch->embracing=NULL;
    return;
  }

  victim->embraced=NULL;
  ch->embracing=NULL;
  return;
}

void stop_fighting( CHAR_DATA *ch, bool fBoth )
{
    CHAR_DATA *fch;

    for ( fch = char_list; fch != NULL; fch = fch->next )
    {
	if ( fch == ch || ( fBoth && fch->fighting == ch ) )
	{
	    fch->fighting	= NULL;
	    fch->position	= POS_STANDING;
	    update_pos( fch );
	}
    }

    return;
}



/*
 * Make a corpse out of a character.
 */
void make_corpse( CHAR_DATA *ch )
{
    char buf[MAX_STRING_LENGTH];
    OBJ_DATA *corpse;
    OBJ_DATA *obj;
    OBJ_DATA *obj_next;
    char *name;

    
   if ( IS_NPC(ch) )
    {
	name		= ch->short_descr;
	corpse		= create_object(get_obj_index(OBJ_VNUM_CORPSE_NPC), 0);
	corpse->timer	= number_range( 4, 8 );
	corpse->value[2]= ch->pIndexData->vnum;
	if ( ch->gold > 0 )
	{
	    obj_to_obj( create_money( ch->gold ), corpse );
	    ch->gold = 0;
	}
    }
    else
    {
	name		= ch->name;
	corpse		= create_object(get_obj_index(OBJ_VNUM_CORPSE_PC), 0);
	corpse->timer	= number_range( 25, 40 );
	/* Why should players keep their gold? */
	if ( ch->gold > 0 )
	{
	    obj = create_money( ch->gold );
	    if (IS_AFFECTED(ch,AFF_SHADOWPLANE) &&
	        (!IS_SET(obj->extra_flags, ITEM_SHADOWPLANE) ) )
	        SET_BIT(obj->extra_flags, ITEM_SHADOWPLANE);
	    obj_to_obj( obj, corpse );
	    ch->gold = 0;
	}
    }
if (IS_SET(ch->extra, EXTRA_ZOMBIE))
{
SET_BIT(corpse->quest, QUEST_ZOMBIE);
}

    sprintf( buf, corpse->short_descr, name );
    free_string( corpse->short_descr );
    corpse->short_descr = str_dup( buf );

    sprintf( buf, corpse->description, name );
    free_string( corpse->description );
    corpse->description = str_dup( buf );
   
    for ( obj = ch->carrying; obj != NULL; obj = obj_next )
    {
	obj_next = obj->next_content;

	obj_from_char( obj );
	if ( IS_SET( obj->extra_flags, ITEM_VANISH ) )
	    extract_obj( obj );
	else
	{
	    if (IS_AFFECTED(ch,AFF_SHADOWPLANE) &&
	        (!IS_SET(obj->extra_flags, ITEM_SHADOWPLANE) ) )
	        SET_BIT(obj->extra_flags, ITEM_SHADOWPLANE);
	    obj_to_obj( obj, corpse );
	}
    }

    /* If you die in the shadowplane, your corpse should stay there */
    if (IS_AFFECTED(ch, AFF_SHADOWPLANE))
	SET_BIT(corpse->extra_flags, ITEM_SHADOWPLANE);

    obj_to_room( corpse, ch->in_room );
    return;
}



void death_cry( CHAR_DATA *ch )
{
    ROOM_INDEX_DATA *was_in_room;
    char *msg;
    int door;

    if ( IS_NPC(ch) )
	msg = "You hear something's death cry.";
    else
	msg = "You hear someone's death cry.";

    was_in_room = ch->in_room;
    for ( door = 0; door <= 5; door++ )
    {
	EXIT_DATA *pexit;

	if ( ( pexit = was_in_room->exit[door] ) != NULL
	&&   pexit->to_room != NULL
	&&   pexit->to_room != was_in_room )
	{
	    ch->in_room = pexit->to_room;
	    act( msg, ch, NULL, NULL, TO_ROOM );
	}
    }
    ch->in_room = was_in_room;
    return;
}

void make_part( CHAR_DATA *ch, char *argument )
{
    char  arg [MAX_INPUT_LENGTH];
    int vnum;

    argument = one_argument(argument,arg);
    vnum = 0;

    if (arg[0] == '\0') return;
    if (!str_cmp(arg,"head")) vnum = OBJ_VNUM_SEVERED_HEAD;
    else if (!str_cmp(arg,"arm")) vnum = OBJ_VNUM_SLICED_ARM;
    else if (!str_cmp(arg,"leg")) vnum = OBJ_VNUM_SLICED_LEG;
    else if (!str_cmp(arg,"heart")) vnum = OBJ_VNUM_TORN_HEART;
    else if (!str_cmp(arg,"turd")) vnum = OBJ_VNUM_TORN_HEART;
    else if (!str_cmp(arg,"entrails")) vnum = OBJ_VNUM_SPILLED_ENTRAILS;
    else if (!str_cmp(arg,"brain")) vnum = OBJ_VNUM_QUIVERING_BRAIN;
    else if (!str_cmp(arg,"eyeball")) vnum = OBJ_VNUM_SQUIDGY_EYEBALL;
    else if (!str_cmp(arg,"blood")) vnum = OBJ_VNUM_SPILT_BLOOD;
    else if (!str_cmp(arg,"face")) vnum = OBJ_VNUM_RIPPED_FACE;
    else if (!str_cmp(arg,"windpipe")) vnum = OBJ_VNUM_TORN_WINDPIPE;
    else if (!str_cmp(arg,"cracked_head")) vnum = OBJ_VNUM_CRACKED_HEAD;
    else if (!str_cmp(arg,"ear")) vnum = OBJ_VNUM_SLICED_EAR;
    else if (!str_cmp(arg,"nose")) vnum = OBJ_VNUM_SLICED_NOSE;
    else if (!str_cmp(arg,"tooth")) vnum = OBJ_VNUM_KNOCKED_TOOTH;
    else if (!str_cmp(arg,"tongue")) vnum = OBJ_VNUM_TORN_TONGUE;
    else if (!str_cmp(arg,"hand")) vnum = OBJ_VNUM_SEVERED_HAND;
    else if (!str_cmp(arg,"foot")) vnum = OBJ_VNUM_SEVERED_FOOT;
    else if (!str_cmp(arg,"thumb")) vnum = OBJ_VNUM_SEVERED_THUMB;
    else if (!str_cmp(arg,"index")) vnum = OBJ_VNUM_SEVERED_INDEX;
    else if (!str_cmp(arg,"middle")) vnum = OBJ_VNUM_SEVERED_MIDDLE;
    else if (!str_cmp(arg,"ring")) vnum = OBJ_VNUM_SEVERED_RING;
    else if (!str_cmp(arg,"little")) vnum = OBJ_VNUM_SEVERED_LITTLE;
    else if (!str_cmp(arg,"toe")) vnum = OBJ_VNUM_SEVERED_TOE;

    if ( vnum != 0 )
    {
	char buf[MAX_STRING_LENGTH];
	OBJ_DATA *obj;
	char *name;

	name		= IS_NPC(ch) ? ch->short_descr : ch->name;
	obj		= create_object( get_obj_index( vnum ), 0 );
	if (IS_NPC(ch))   obj->timer = number_range(2,5);
	else              obj->timer = -1;
	if (!str_cmp(arg,"head") && IS_NPC(ch))
	    obj->value[1] = ch->pIndexData->vnum;
	else if (!str_cmp(arg,"head") && !IS_NPC(ch))
	    {ch->pcdata->chobj = obj; obj->chobj = ch;obj->timer = number_range(2,3); obj->item_type = ITEM_HEAD;}
	else if (!str_cmp(arg,"arm"))
	    SET_BIT(obj->extra_flags2, ITEM_ARM);
	else if (!str_cmp(arg,"brain") && !IS_NPC(ch) && 
	IS_AFFECTED(ch,AFF_POLYMORPH) && IS_HEAD(ch, LOST_HEAD))
	{
	    if (ch->pcdata->chobj != NULL) ch->pcdata->chobj->chobj = NULL;
	    ch->pcdata->chobj = obj;
	    obj->chobj = ch;
	    obj->timer = number_range(2,3);
	    obj->item_type = ITEM_HEAD;
	}

	/* For blood :) KaVir */
	if (vnum == OBJ_VNUM_SPILT_BLOOD) obj->timer = 2;

	/* For voodoo dolls - KaVir */
	if (!IS_NPC(ch))
	{
	    sprintf( buf, obj->name, name );
	    free_string( obj->name );
	    obj->name = str_dup( buf );
	}
	else
	{
	    sprintf( buf, obj->name, "mob" );
	    free_string( obj->name );
	    obj->name = str_dup( buf );
	}

	sprintf( buf, obj->short_descr, name );
	free_string( obj->short_descr );
	obj->short_descr = str_dup( buf );

	sprintf( buf, obj->description, name );
	free_string( obj->description );
	obj->description = str_dup( buf );

    	if (IS_AFFECTED(ch, AFF_SHADOWPLANE))
	    SET_BIT(obj->extra_flags, ITEM_SHADOWPLANE);

	obj_to_room( obj, ch->in_room );
    }
    return;
}



void raw_kill( CHAR_DATA *victim )
{
    CHAR_DATA *mount;
    stop_fighting( victim, TRUE );
    death_cry( victim );
    make_corpse( victim );

    if ( (mount = victim->mount) != NULL)
    {
	if (victim->mounted == IS_MOUNT)
	{
	    act("$n rolls off the corpse of $N.",mount,NULL,victim,TO_ROOM);
	    act("You roll off the corpse of $N.",mount,NULL,victim,TO_CHAR);
	}
	else if (victim->mounted == IS_RIDING)
	{
	    act("$n falls off $N.",victim,NULL,mount,TO_ROOM);
	    act("You fall off $N.",victim,NULL,mount,TO_CHAR);
	}
	mount->mount    = NULL;
	victim->mount   = NULL;
	mount->mounted  = IS_ON_FOOT;
	victim->mounted = IS_ON_FOOT;
    }
 
    if ( IS_NPC(victim) )
    {
	victim->pIndexData->killed++;
	kill_table[URANGE(0, victim->level, MAX_LEVEL-1)].killed++;
	extract_char( victim, TRUE );
	return;
    }

    extract_char( victim, FALSE );
    while ( victim->affected )
	affect_remove( victim, victim->affected );
    if (IS_AFFECTED(victim,AFF_POLYMORPH) && IS_AFFECTED(victim,AFF_ETHEREAL))
    {
    	victim->affected_by	= AFF_POLYMORPH + AFF_ETHEREAL;
    }
    else if (IS_AFFECTED(victim,AFF_POLYMORPH))
    	victim->affected_by	= AFF_POLYMORPH;
    else if (IS_AFFECTED(victim,AFF_ETHEREAL))
    	victim->affected_by	= AFF_ETHEREAL;
    else
    	victim->affected_by	= 0;

    REMOVE_BIT(victim->immune, IMM_STAKE);
    REMOVE_BIT(victim->extra, TIED_UP);
    REMOVE_BIT(victim->extra, GAGGED);
    REMOVE_BIT(victim->extra, BLINDFOLDED);
    REMOVE_BIT(victim->extra, EXTRA_PREGNANT);
    REMOVE_BIT(victim->extra, EXTRA_LABOUR);
    victim->pcdata->stats[DEMON_POWER] = 0;

    victim->itemaffect	 = 0;
    victim->loc_hp[0]    = 0;
    victim->loc_hp[1]    = 0;
    victim->loc_hp[2]    = 0;
    victim->loc_hp[3]    = 0;
    victim->loc_hp[4]    = 0;
    victim->loc_hp[5]    = 0;
    victim->loc_hp[6]    = 0;
    victim->armor	 = 100;
    victim->position	 = POS_RESTING;
    victim->hit		 = UMAX( 1, victim->hit  );
    victim->mana	 = UMAX( 1, victim->mana );
    victim->move	 = UMAX( 1, victim->move );
    victim->hitroll	 = 0;
    victim->damroll	 = 0;
    victim->saving_throw = 0;
    victim->carry_weight = 0;
    victim->carry_number = 0;
    do_call(victim,"all");
    save_char_obj( victim );
    return;
}


void behead( CHAR_DATA *victim )
{
    char buf [MAX_STRING_LENGTH];
    ROOM_INDEX_DATA *location;

    if (IS_NPC(victim)) return;

    location = victim->in_room;

    stop_fighting( victim, TRUE );

    make_corpse( victim );

    extract_char( victim, FALSE );

    char_from_room(victim);
    char_to_room(victim,location);

    if (victim == NULL)
    {bug( "Behead: Victim no longer exists.", 0 );return;}

    make_part( victim, "head" );

    while ( victim->affected )
	affect_remove( victim, victim->affected );
    if (IS_AFFECTED(victim,AFF_POLYMORPH) && IS_AFFECTED(victim,AFF_ETHEREAL))
    {
    	victim->affected_by	= AFF_POLYMORPH + AFF_ETHEREAL;
    }
    else if (IS_AFFECTED(victim,AFF_POLYMORPH))
    	victim->affected_by	= AFF_POLYMORPH;
    else if (IS_AFFECTED(victim,AFF_ETHEREAL))
    	victim->affected_by	= AFF_ETHEREAL;
    else
    	victim->affected_by	= 0;

    REMOVE_BIT(victim->immune, IMM_STAKE);
    REMOVE_BIT(victim->extra, TIED_UP);
    REMOVE_BIT(victim->extra, GAGGED);
    REMOVE_BIT(victim->extra, BLINDFOLDED);
    REMOVE_BIT(victim->extra, EXTRA_PREGNANT);
    REMOVE_BIT(victim->extra, EXTRA_LABOUR);
    victim->pcdata->stats[DEMON_POWER] = 0;

    victim->itemaffect	 = 0;
    victim->loc_hp[0]    = 0;
    victim->loc_hp[1]    = 0;
    victim->loc_hp[2]    = 0;
    victim->loc_hp[3]    = 0;
    victim->loc_hp[4]    = 0;
    victim->loc_hp[5]    = 0;
    victim->loc_hp[6]    = 0;
    victim->affected_by	 = 0;
    victim->armor	 = 100;
    victim->position	 = POS_STANDING;
    victim->hit		 = 1;
    victim->mana	 = UMAX( 1, victim->mana );
    victim->move	 = UMAX( 1, victim->move );
    victim->hitroll	 = 0;
    victim->damroll	 = 0;
    victim->saving_throw = 0;
    victim->carry_weight = 0;
    victim->carry_number = 0;

    if (victim->cur_form != get_normal_form(victim))
    {
	set_form(victim, get_normal_form(victim));
	stc("Your body reverts to it's normal form.\n\r",victim);
    }

    SET_BIT(victim->loc_hp[0],LOST_HEAD);
    SET_BIT(victim->affected_by,AFF_POLYMORPH);
    sprintf(buf,"the severed head of %s",victim->name);
    free_string(victim->morph);
    victim->morph = str_dup(buf);
    do_call(victim,"all");
    save_char_obj( victim );
    return;
}



void group_gain( CHAR_DATA *ch, CHAR_DATA *victim )
{
    char buf[MAX_STRING_LENGTH];
    CHAR_DATA *gch;
    CHAR_DATA *lch;
    CHAR_DATA *mount;
    int xp;
    int members;

    /*
     * Monsters don't get kill xp's or alignment changes.
     * P-killing doesn't help either.
     * Dying of mortal wounds or poison doesn't give xp to anyone!
     */
    if ( (IS_NPC(ch) && (mount = ch->mount) == NULL) || victim == ch )
	return;
    
    members = 0;
    for ( gch = ch->in_room->people; gch != NULL; gch = gch->next_in_room )
    {
	if ( is_same_group( gch, ch ) )
	    members++;
    }

    if ( members == 0 )
    {
	bug( "Group_gain: members.", members );
	members = 1;
    }

    lch = (ch->leader != NULL) ? ch->leader : ch;

    for ( gch = ch->in_room->people; gch != NULL; gch = gch->next_in_room )
    {
	OBJ_DATA *obj;
	OBJ_DATA *obj_next;

	if ( !is_same_group( gch, ch ) )
	    continue;

	xp = xp_compute( gch, victim )*1.2 / members;
	sprintf( buf, "You receive %d experience points.\n\r", xp );
	send_to_char( buf, gch );
	if ((mount = gch->mount) != NULL) send_to_char( buf, mount );
	gain_exp( gch, xp );

	for ( obj = ch->carrying; obj != NULL; obj = obj_next )
	{
	    obj_next = obj->next_content;
	    if ( obj->wear_loc == WEAR_NONE )
		continue;

	    if ( ( IS_OBJ_STAT(obj, ITEM_ANTI_EVIL)    && IS_EVIL(ch)    )
	    ||   ( IS_OBJ_STAT(obj, ITEM_ANTI_GOOD)    && IS_GOOD(ch)    )
	    ||   ( IS_OBJ_STAT(obj, ITEM_ANTI_NEUTRAL) && IS_NEUTRAL(ch) ) )
	    {
		act( "You are zapped by $p.", ch, obj, NULL, TO_CHAR );
		act( "$n is zapped by $p.",   ch, obj, NULL, TO_ROOM );
		obj_from_char( obj );
		obj_to_room( obj, ch->in_room );
	    }
	}
    }

    return;
}



/*
 * Compute xp for a kill.
 * Also adjust alignment of killer.
 * Edit this function to change xp computations.
 */
/*
int xp_compute( CHAR_DATA *gch, CHAR_DATA *victim )
{
    int align;
    int xp;

    int vnum;
    int victim_level; 

    xp    = 300 - URANGE( -3, 3 - (victim->level*5/3), 6 ) * 50;
    align = gch->alignment - victim->alignment;

    if (victim->level > 1000 )
	victim_level = 1000;
    else
	victim_level = victim->level;

    if (IS_HERO(gch))
    {
	
	gch->alignment  = gch->alignment;
    }


     
    if (IS_NPC(victim) && (vnum = victim->pIndexData->vnum) > 29000)
    {
	switch (vnum)
	{
	default:
	    break;
	case 29600:
	case 30001:
	case 30006:
	case 30007:
	case 30008:
	case 30009:
        case 30000:
	    return 0;
	}
    }

    if (IS_NPC(victim) && (IS_SET(victim->act, ACT_NOEXP) || IS_IMMORTAL(gch))) 
	return 0;

    xp    -= xp * number_range( -2, 2 ) / 8;

    xp     = number_range( xp * 3 / 4, xp * 5 / 4 );
    xp     = UMAX( 0, xp );
    xp     = (xp * (victim_level+1) * 0.65);
//  xp     = xp / 2; 


    if (!IS_NPC(gch))
    {
	gch->pcdata->score[SCORE_TOTAL_LEVEL] += victim->level;
	if (victim->level > gch->pcdata->score[SCORE_HIGH_LEVEL])
	    gch->pcdata->score[SCORE_HIGH_LEVEL] += victim->level;
	if (victim->level > 950) return 0;
	gch->pcdata->score[SCORE_TOTAL_XP] += xp;
	if (xp > gch->pcdata->score[SCORE_HIGH_XP])
	    gch->pcdata->score[SCORE_HIGH_XP] += xp;
    }
    
   
    if (!IS_NPC(gch) && xp > 499
	&& gch->pcdata->disc_points != 666
	&& gch->pcdata->disc_research != -1)
    {
	stc("You gained a discipline point.\n\r", gch);
	gain_disc_points(gch, 1);
    }
    
    xp *= 5/3;
    return xp;
}

*/


/*
 * Compute xp for a kill.
 * Also adjust alignment of killer.
 * Edit this function to change xp computations.
 */
int xp_compute( CHAR_DATA *gch, CHAR_DATA *victim )
{
    int align;
    int xp;
    int extra;
    int level;
    int number;
    int vnum;
    int victim_level; /* put to stop mass exp give outs Rotain */

    xp    = 300 - URANGE( -3, 3 - (victim->level*5/3), 6 ) * 50;
    align = gch->alignment - victim->alignment;

    if (victim->level > 1000 )
	victim_level = 1000;
    else
	victim_level = victim->level;

    if (IS_HERO(gch))
    {
	/* Avatars shouldn't be able to change their alignment */
	gch->alignment  = gch->alignment;
    }
    else if ( align >  500 )
    {
	gch->alignment  = UMIN( gch->alignment + (align-500)/4,  1000 );
	xp = 5 * xp / 4;
    }
    else if ( align < -500 )
    {
	gch->alignment  = UMAX( gch->alignment + (align+500)/4, -1000 );
    }
    else
    {
	gch->alignment -= gch->alignment / 4;
	xp = 3 * xp / 4;
    }

     /* 
      * Put in mob vnum that you don't want players to gain exp for
      * Rotain
      */
    if (IS_NPC(victim) && (vnum = victim->pIndexData->vnum) > 29000)
    {
	switch (vnum)
	{
	default:
	    break;
	case 29600:
	case 30001:
	case 30006:
	case 30007:
	case 30008:
	case 30009:
        case 30000:
	    return 0;
	}
    }

    if (IS_NPC(victim) && (IS_SET(victim->act, ACT_NOEXP) || IS_IMMORTAL(gch))) 
	return .40;

    /*
     * Adjust for popularity of target:
     *   -1/8 for each target over  'par' (down to -100%)
     *   +1/8 for each target under 'par' (  up to + 25%)
     */
    level  = URANGE( 0, victim_level, MAX_LEVEL - 1 );
    number = UMAX( 1, kill_table[level].number );
    extra  = victim->pIndexData->killed - kill_table[level].killed / number;
/*  xp    -= xp * URANGE( -2, extra, 8 ) / 8;
    xp    -= xp * URANGE( -2, extra, 6 ) / 8; */

    xp    -= xp * number_range( -2, 2 ) / 8;

    xp     = number_range( xp * 3 / 4, xp * 5 / 4 );
    xp     = UMAX( 0, xp );
    xp     = (xp * (victim_level+1) * 0.60);
    xp     = xp / 2; /* Put in cause players compaling to much exp :P */

    if (!IS_NPC(gch))
    {
	gch->pcdata->score[SCORE_TOTAL_LEVEL] += victim->level;
	if (victim->level > gch->pcdata->score[SCORE_HIGH_LEVEL])
	    gch->pcdata->score[SCORE_HIGH_LEVEL] += victim->level;
	if (victim->level > 950) return 0;
	gch->pcdata->score[SCORE_TOTAL_XP] += xp;
	if (xp > gch->pcdata->score[SCORE_HIGH_XP])
	    gch->pcdata->score[SCORE_HIGH_XP] += xp;
    }
	xp = xp *4/3;
    return xp;
}



void dam_message( CHAR_DATA *ch, CHAR_DATA *victim, int dam, int dt )
{
    static char * const attack_table[] =
    {
	"hit",
	"slice",  "stab",  "slash", "whip", "claw",
	"blast",  "pound", "crush", "grep", "bite",
	"pierce", "suck"
    };
    static char * const attack_table2[] =
    {
	"hits",
	"slices",  "stabs",  "slashes", "whips", "claws",
	"blasts",  "pounds", "crushes", "greps", "bites",
	"pierces", "sucks"
    };

    char buf1[512], buf2[512], buf3[512];
    const char *vs;
    const char *vp;
    const char *attack;
    const char *attack2;
    int damp;
    int bodyloc;
    int recover;
    bool critical = FALSE;
    char punct;

if (!IS_NPC(ch) && IS_SET(ch->newbits,NEW_REND)&& dam>0
	&& number_range(1,7) == 5) critical=TRUE;
         if ( dam ==   0 ) { vs = " miss";	vp = " misses";		}
    else if ( dam <=  50 ) { vs = " lightly";	vp = " lightly";	}
    else if ( dam <= 100 ) { vs = "";		vp = "";		}
    else if ( dam <= 250 ) { vs = " hard";	vp = " hard";		}
    else if ( dam <= 500 ) { vs = " very hard";	vp = " very hard";	}
    else if ( dam <= 750 ) { vs = " extremely hard"; vp = " extremely hard";}
    else if ( dam <= 1250 ) { vs = " incredibly hard";vp = " incredibly hard";}	
    else if ( dam <= 2000 ) { vs = " insanely hard";vp= " insanely hard";}
    else                   { vs = " unbelievably hard";vp = " unbelievably hard";}

    /* If victim's hp are less/equal to 'damp', attacker gets a death blow */
    if (IS_NPC(victim)) damp = 0;
    else                damp = -10;
    if ( (victim->hit - dam > damp ) || (dt >= 0 && dt < MAX_SKILL) ||
	(IS_NPC(victim) && IS_SET(victim->act, ACT_NOPARTS)) )
    {
    	punct   = (dam <= 250) ? '.' : '!';

    	if ( dt == TYPE_HIT && !IS_NPC(ch) && !IS_VAMPAFF(ch,VAM_CLAWS) )
    	{
	    if (dam == 0)
	    {
	    	sprintf( buf1, "$n%s $N%c[%d]",  vp, punct, dam );
	    	sprintf( buf2, "You%s $N%c[%d]", vs, punct, dam );
	    	sprintf( buf3, "$n%s you%c", vp, punct );
	    }
	    else
	    {
	    	sprintf( buf1, "$n hits $N%s%c[%d]",  vp, punct, dam );
	    	sprintf( buf2, "You hit $N%s%c[%d]", vs, punct, dam );
	    	sprintf( buf3, "$n hits you%s%c[%d]", vp, punct, dam );
		critical = TRUE;
	    }
    	}

	else if ( dt == TYPE_HIT && !IS_NPC(ch) && IS_SET(ch->newbits, NEW_MONKFLAME))
    	{
	    if (dam >= 1)
	    {
              if (!IS_CLASS(ch, CLASS_WEREWOLF))
              {
	    	sprintf( buf1, "$n's flaming hands hit $N%s%c[%d]", vp, punct, dam );
	    	sprintf( buf2, "Your flaming hands hit $N%s%c[%d]", vs, punct, dam );
	    	sprintf( buf3, "$n's flaming hands hit you%s%c[%d]", vp, punct, dam );
	      }
	      else
	      {
		sprintf( buf1, "$n's flaming claws hit $N%s%c[%d]", vp, punct, dam );
                sprintf( buf2, "Your flaming claws hit $N%s%c[%d]", vs, punct, dam );
                sprintf( buf3, "$n's flaming claws hit you%s%c[%d]", vp, punct, dam );
	      }
	    }
	    else 
	    {
	    	sprintf( buf1, "$n's hit%s $N%c[%d]",  vp, punct, dam );
	    	sprintf( buf2, "Your hit%s $N%c[%d]", vs, punct, dam );
	    	sprintf( buf3, "$n hit%s you%c[%d]", vp, punct, dam );
		critical = TRUE;
	    }
    	}
    	else
    	{
	    if ( dt == TYPE_HIT && !IS_NPC(ch) && IS_VAMPAFF(ch,VAM_CLAWS))
	    {
	    	attack  = attack_table[dt - TYPE_HIT + 5];
	    	attack2 = attack_table2[dt - TYPE_HIT + 5];
	    }
	    else if ( dt >= 0 && dt < MAX_SKILL )
	    {
	    	attack = skill_table[dt].noun_damage;
	    	attack2 = skill_table[dt].noun_damage;
	    }
	    else if ( dt >= TYPE_HIT
	    && dt < TYPE_HIT + sizeof(attack_table)/sizeof(attack_table[0]) )
	    {
	    	attack  = attack_table[dt - TYPE_HIT];
	    	attack2 = attack_table2[dt - TYPE_HIT];
	    }
	    else
	    {/*
	        bug( "Dam_message: bad dt %d.", dt );
	       */ dt  = TYPE_HIT;
	        attack  = attack_table[0];
	        attack2 = attack_table2[0];
	    }

	    if (dam == 0)
	    {
	    	sprintf( buf1, "$n's %s%s $N%c[%d]",  attack, vp, punct, dam );
	    	sprintf( buf2, "Your %s%s $N%c[%d]",  attack, vp, punct, dam );
	    	sprintf( buf3, "$n's %s%s you%c[%d]", attack, vp, punct, dam );
	    }

	    else if (IS_SET(ch->newbits, NEW_MONKFLAME) && (dam == 0))
	    {
	    	sprintf( buf1, "$n's flaming %s%s $N%c[%d]", attack2, vp, punct, dam );
	    	sprintf( buf2, "Your flaming %s%s $N%c[%d]",  attack, vp, punct, dam );
	    	sprintf( buf3, "$n's flaming %s%s you%c[%d]", attack2, vp, punct, dam );
	    }
       

	    else
	    {
		if ( dt >= 0 && dt < MAX_SKILL )
		{
	    	    sprintf( buf1, "$n's %s strikes $N%s%c[%d]",  attack2, vp, punct, dam );
	    	    sprintf( buf2, "Your %s strikes $N%s%c[%d]",  attack, vp, punct, dam );
	    	    sprintf( buf3, "$n's %s strikes you%s%c[%d]", attack2, vp, punct, dam );
		}

		else if (IS_SET(ch->newbits, NEW_MONKFLAME))
            {
                sprintf( buf1, "$n's flaming %s hits $N%s%c[%d]", attack2, vp, punct, dam);
                sprintf( buf2, "Your flaming %s hits $N%s%c[%d]", attack, vp, punct, dam);
                sprintf( buf3, "$n's flaming %s hits you%s%c[%d]", attack2, vp, punct, dam);
            }
       

		else
		{
	    	    sprintf( buf1, "$n %s $N%s%c[%d]",  attack2, vp, punct, dam );
	    	    sprintf( buf2, "You %s $N%s%c[%d]",  attack, vp, punct, dam );
	    	    sprintf( buf3, "$n %s you%s%c[%d]", attack2, vp, punct, dam );
		    critical = TRUE;
		}
	    }

	    recover = 0;

            if (!IS_NPC(victim) && IS_SET(victim->newbits,NEW_SKIN) &&
                 (attack=="slash" || attack == "slice" || attack ==
                  "pierce" || attack == "stab"))
           recover=number_range(1,dam);           
	    /* Check for weapon resistance - KaVir */
	    if ( !IS_NPC(victim) && IS_IMMUNE(victim, IMM_SLASH) &&
		(attack == "slash" || attack == "slice") && dam > 0 )
		recover = number_range(1,dam);
	    if ( !IS_NPC(victim) && IS_IMMUNE(victim, IMM_STAB) &&
		(attack == "stab" || attack == "pierce") && dam > 0 )
		recover = number_range(1,dam);
	    if ( !IS_NPC(victim) && IS_IMMUNE(victim, IMM_SMASH) &&
		(attack == "blast" || attack == "pound" || attack == "crush") && dam > 0 )
		recover = number_range(1,dam);
	    if ( !IS_NPC(victim) && IS_IMMUNE(victim, IMM_ANIMAL) &&
		(attack == "bite" || attack == "claw") && dam > 0 )
		recover = number_range(1,dam);
	    if ( !IS_NPC(victim) && IS_IMMUNE(victim, IMM_MISC) &&
		(attack == "grep" || attack == "suck" || attack == "whip") && dam > 0 )
		recover = number_range(1,dam);
	    
        }
        act( buf1, ch, NULL, victim, TO_NOTVICT );
        act( buf2, ch, NULL, victim, TO_CHAR );
        act( buf3, ch, NULL, victim, TO_VICT );
	if (critical) critical_hit(ch,victim,dt,dam);
	return;
    }

    if ( dt == TYPE_HIT && !IS_NPC( ch ) && !IS_VAMPAFF(ch,VAM_CLAWS) 
	&&!IS_VAMPAFF(ch,VAM_FANGS))
    {
	damp=number_range(1,5);
	if ( damp == 1 )
	{
	    act("You ram your fingers into $N's eye sockets and rip $S face off.", ch, NULL, victim, TO_CHAR);
	    act("$n rams $s fingers into $N's eye sockets and rips $S face off.", ch, NULL, victim, TO_NOTVICT);
	    act("$n rams $s fingers into your eye sockets and rips your face off.", ch, NULL, victim, TO_VICT);
	    make_part(victim,"face");
	}
	else if ( damp == 2 )
	{
	    act("You grab $N by the throat and tear $S windpipe out.", ch, NULL, victim, TO_CHAR);
	    act("$n grabs $N by the throat and tears $S windpipe out.", ch, NULL, victim, TO_NOTVICT);
	    act("$n grabs you by the throat and tears your windpipe out.", ch, NULL, victim, TO_VICT);
	    make_part(victim,"windpipe");
	}
	else if ( damp == 3 )
	{
	    act("You punch your fist through $N's stomach and rip out $S entrails.", ch, NULL, victim, TO_CHAR);
	    act("$n punches $s fist through $N's stomach and rips out $S entrails.", ch, NULL, victim, TO_NOTVICT);
	    act("$n punches $s fist through your stomach and rips out your entrails.", ch, NULL, victim, TO_VICT);
	    make_part(victim,"entrails");
	}
	else if ( damp == 4 )
	{
	    if (!IS_BODY(victim,BROKEN_SPINE))
		SET_BIT(victim->loc_hp[1],BROKEN_SPINE);
	    act("You hoist $N above your head and slam $M down upon your knee.\n\rThere is a loud cracking sound as $N's spine snaps.", ch, NULL, victim, TO_CHAR);
	    act("$n hoists $N above $s head and slams $M down upon $s knee.\n\rThere is a loud cracking sound as $N's spine snaps.", ch, NULL, victim, TO_NOTVICT);
	    act("$n hoists you above $s head and slams you down upon $s knee.\n\rThere is a loud cracking sound as your spine snaps.", ch, NULL, victim, TO_VICT);
	}
	else if ( damp == 5 )
	{
	    act("You lock your arm around $N's head, and give it a vicious twist.", ch, NULL, victim, TO_CHAR);
	    act("$n locks $s arm around $N's head, and gives it a vicious twist.", ch, NULL, victim, TO_NOTVICT);
	    act("$n locks $s arm around your head, and gives it a vicious twist.", ch, NULL, victim, TO_VICT);
	    if (!IS_BODY(victim,BROKEN_NECK))
	    {
		act("There is a loud snapping noise as your neck breaks.", victim, NULL, NULL, TO_CHAR);
		act("There is a loud snapping noise as $n's neck breaks.", victim, NULL, NULL, TO_ROOM);
		SET_BIT(victim->loc_hp[1],BROKEN_NECK);
	    }
	}
	return;
    }
    if ( dt >= 0 && dt < MAX_SKILL )
	attack = skill_table[dt].noun_damage;
    else if ( dt >= TYPE_HIT
    && dt < TYPE_HIT + sizeof(attack_table)/sizeof(attack_table[0]) )
	attack = attack_table[dt - TYPE_HIT];
    else
    {
	dt  = TYPE_HIT;
	attack  = attack_table[0];
    }

    if ( attack == "slash" || attack == "slice" )
    {
	damp=number_range(1,8);
	if ( damp == 1 )
	{
	    act("You swing your blade in a low arc, rupturing $N's abdominal cavity.\n\r$S entrails spray out over a wide area.", ch, NULL, victim, TO_CHAR);
	    act("$n swings $s blade in a low arc, rupturing $N's abdominal cavity.\n\r$S entrails spray out over a wide area.", ch, NULL, victim, TO_NOTVICT);
	    act("$n swings $s blade in a low arc, rupturing your abdominal cavity.\n\rYour entrails spray out over a wide area.", ch, NULL, victim, TO_VICT);
	    make_part(victim,"entrails");
	}
	else if ( damp == 2 )
	{
	    act("You thrust your blade into $N's mouth and twist it viciously.\n\rThe end of your blade bursts through the back of $S head.", ch, NULL, victim, TO_CHAR);
	    act("$n thrusts $s blade into $N's mouth and twists it viciously.\n\rThe end of the blade bursts through the back of $N's head.", ch, NULL, victim, TO_NOTVICT);
	    act("$n thrusts $s blade into your mouth and twists it viciously.\n\rYou feel the end of the blade burst through the back of your head.", ch, NULL, victim, TO_VICT);
	}
	else if ( damp == 3 )
	{
	    if (!IS_BODY(victim,CUT_THROAT))
		SET_BIT(victim->loc_hp[1],CUT_THROAT);
	    if (!IS_BLEEDING(victim,BLEEDING_THROAT))
	    	SET_BIT(victim->loc_hp[6],BLEEDING_THROAT);
	    act("Your blow slices open $N's carotid artery, spraying blood everywhere.", ch, NULL, victim, TO_CHAR);
	    act("$n's blow slices open $N's carotid artery, spraying blood everywhere.", ch, NULL, victim, TO_NOTVICT);
	    act("$n's blow slices open your carotid artery, spraying blood everywhere.", ch, NULL, victim, TO_VICT);
	    make_part(victim,"blood");
	}
	else if ( damp == 4 )
	{
	    if (!IS_BODY(victim,CUT_THROAT))
		SET_BIT(victim->loc_hp[1],CUT_THROAT);
	    if (!IS_BLEEDING(victim,BLEEDING_THROAT))
	    	SET_BIT(victim->loc_hp[6],BLEEDING_THROAT);
	    act("You swing your blade across $N's throat, showering the area with blood.", ch, NULL, victim, TO_CHAR);
	    act("$n swings $s blade across $N's throat, showering the area with blood.", ch, NULL, victim, TO_NOTVICT);
	    act("$n swings $s blade across your throat, showering the area with blood.", ch, NULL, victim, TO_VICT);
	    make_part(victim,"blood");
	}
	else if ( damp == 5 )
	{
	    if (!IS_HEAD(victim,BROKEN_SKULL))
	    {
	    	act("You swing your blade down upon $N's head, splitting it open.\n\r$N's brains pour out of $S forehead.", ch, NULL, victim, TO_CHAR);
	    	act("$n swings $s blade down upon $N's head, splitting it open.\n\r$N's brains pour out of $S forehead.", ch, NULL, victim, TO_NOTVICT);
	    	act("$n swings $s blade down upon your head, splitting it open.\n\rYour brains pour out of your forehead.", ch, NULL, victim, TO_VICT);
	    	make_part(victim,"brain");
		SET_BIT(victim->loc_hp[0],BROKEN_SKULL);
	    }
	    else
	    {
		act("You plunge your blade deep into $N's chest.", ch, NULL, victim, TO_CHAR);
		act("$n plunges $s blade deep into $N's chest.", ch, NULL, victim, TO_NOTVICT);
		act("$n plunges $s blade deep into your chest.", ch, NULL, victim, TO_VICT);
	    }
	}
	else if ( damp == 6 )
	{
	    act("You swing your blade between $N's legs, nearly splitting $M in half.", ch, NULL, victim, TO_CHAR);
	    act("$n swings $s blade between $N's legs, nearly splitting $M in half.", ch, NULL, victim, TO_NOTVICT);
	    act("$n swings $s blade between your legs, nearly splitting you in half.", ch, NULL, victim, TO_VICT);
	}
	else if ( damp == 7 )
	{
	    if (!IS_ARM_L(victim,LOST_ARM))
	    {
	    	act("You swing your blade in a wide arc, slicing off $N's arm.", ch, NULL, victim, TO_CHAR);
	    	act("$n swings $s blade in a wide arc, slicing off $N's arm.", ch, NULL, victim, TO_NOTVICT);
	    	act("$n swings $s blade in a wide arc, slicing off your arm.", ch, NULL, victim, TO_VICT);
	    	make_part(victim,"arm");
		SET_BIT(victim->loc_hp[2],LOST_ARM);
		if (!IS_BLEEDING(victim,BLEEDING_ARM_L))
		    SET_BIT(victim->loc_hp[6],BLEEDING_ARM_L);
		if (IS_BLEEDING(victim,BLEEDING_HAND_L))
		    REMOVE_BIT(victim->loc_hp[6],BLEEDING_HAND_L);
	    }
	    else if (!IS_ARM_R(victim,LOST_ARM))
	    {
	    	act("You swing your blade in a wide arc, slicing off $N's arm.", ch, NULL, victim, TO_CHAR);
	    	act("$n swings $s blade in a wide arc, slicing off $N's arm.", ch, NULL, victim, TO_NOTVICT);
	    	act("$n swings $s blade in a wide arc, slicing off your arm.", ch, NULL, victim, TO_VICT);
	    	make_part(victim,"arm");
		SET_BIT(victim->loc_hp[3],LOST_ARM);
		if (!IS_BLEEDING(victim,BLEEDING_ARM_R))
		    SET_BIT(victim->loc_hp[6],BLEEDING_ARM_R);
		if (IS_BLEEDING(victim,BLEEDING_HAND_R))
		    REMOVE_BIT(victim->loc_hp[6],BLEEDING_HAND_R);
	    }
	    else
	    {
		act("You plunge your blade deep into $N's chest.", ch, NULL, victim, TO_CHAR);
		act("$n plunges $s blade deep into $N's chest.", ch, NULL, victim, TO_NOTVICT);
		act("$n plunges $s blade deep into your chest.", ch, NULL, victim, TO_VICT);
	    }
	}
	else if ( damp == 8 )
	{
	    if (!IS_LEG_L(victim,LOST_LEG))
	    {
	    	act("You swing your blade in a low arc, slicing off $N's leg at the hip.", ch, NULL, victim, TO_CHAR);
	    	act("$n swings $s blade in a low arc, slicing off $N's leg at the hip.", ch, NULL, victim, TO_NOTVICT);
	    	act("$n swings $s blade in a wide arc, slicing off your leg at the hip.", ch, NULL, victim, TO_VICT);
	    	make_part(victim,"leg");
		SET_BIT(victim->loc_hp[4],LOST_LEG);
		if (!IS_BLEEDING(victim,BLEEDING_LEG_L))
		    SET_BIT(victim->loc_hp[6],BLEEDING_LEG_L);
		if (IS_BLEEDING(victim,BLEEDING_FOOT_L))
		    REMOVE_BIT(victim->loc_hp[6],BLEEDING_FOOT_L);
	    }
	    else if (!IS_LEG_R(victim,LOST_LEG))
	    {
	    	act("You swing your blade in a low arc, slicing off $N's leg at the hip.", ch, NULL, victim, TO_CHAR);
	    	act("$n swings $s blade in a low arc, slicing off $N's leg at the hip.", ch, NULL, victim, TO_NOTVICT);
	    	act("$n swings $s blade in a wide arc, slicing off your leg at the hip.", ch, NULL, victim, TO_VICT);
	    	make_part(victim,"leg");
		SET_BIT(victim->loc_hp[5],LOST_LEG);
		if (!IS_BLEEDING(victim,BLEEDING_LEG_R))
		    SET_BIT(victim->loc_hp[6],BLEEDING_LEG_R);
		if (IS_BLEEDING(victim,BLEEDING_FOOT_R))
		    REMOVE_BIT(victim->loc_hp[6],BLEEDING_FOOT_R);
	    }
	    else
	    {
		act("You plunge your blade deep into $N's chest.", ch, NULL, victim, TO_CHAR);
		act("$n plunges $s blade deep into $N's chest.", ch, NULL, victim, TO_NOTVICT);
		act("$n plunges $s blade deep into your chest.", ch, NULL, victim, TO_VICT);
	    }
	}
    }
    else if ( attack == "stab" || attack == "pierce" )
    {
	damp=number_range(1,5);
	if ( damp == 1 )
	{
	    act("You defty invert your weapon and plunge it point first into $N's chest.\n\rA shower of blood sprays from the wound, showering the area.", ch, NULL, victim, TO_CHAR);
	    act("$n defty inverts $s weapon and plunge it point first into $N's chest.\n\rA shower of blood sprays from the wound, showering the area.", ch, NULL, victim, TO_NOTVICT);
	    act("$n defty inverts $s weapon and plunge it point first into your chest.\n\rA shower of blood sprays from the wound, showering the area.", ch, NULL, victim, TO_VICT);
	    make_part(victim,"blood");
	}
	else if ( damp == 2 )
	{
	    act("You thrust your blade into $N's mouth and twist it viciously.\n\rThe end of your blade bursts through the back of $S head.", ch, NULL, victim, TO_CHAR);
	    act("$n thrusts $s blade into $N's mouth and twists it viciously.\n\rThe end of the blade bursts through the back of $N's head.", ch, NULL, victim, TO_NOTVICT);
	    act("$n thrusts $s blade into your mouth and twists it viciously.\n\rYou feel the end of the blade burst through the back of your head.", ch, NULL, victim, TO_VICT);
	}
	else if ( damp == 3 )
	{
	    act("You thrust your weapon up under $N's jaw and through $S head.", ch, NULL, victim, TO_CHAR);
	    act("$n thrusts $s weapon up under $N's jaw and through $S head.", ch, NULL, victim, TO_NOTVICT);
	    act("$n thrusts $s weapon up under your jaw and through your head.", ch, NULL, victim, TO_VICT);
	}
	else if ( damp == 4 )
	{
	    act("You ram your weapon through $N's body, pinning $M to the ground.", ch, NULL, victim, TO_CHAR);
	    act("$n rams $s weapon through $N's body, pinning $M to the ground.", ch, NULL, victim, TO_NOTVICT);
	    act("$n rams $s weapon through your body, pinning you to the ground.", ch, NULL, victim, TO_VICT);
	}
	else if ( damp == 5 )
	{
	    act("You stab your weapon into $N's eye and out the back of $S head.", ch, NULL, victim, TO_CHAR);
	    act("$n stabs $s weapon into $N's eye and out the back of $S head.", ch, NULL, victim, TO_NOTVICT);
	    act("$n stabs $s weapon into your eye and out the back of your head.", ch, NULL, victim, TO_VICT);
	    if (!IS_HEAD(victim,LOST_EYE_L) && number_percent() < 50)
		SET_BIT(victim->loc_hp[0],LOST_EYE_L);
	    else if (!IS_HEAD(victim,LOST_EYE_R))
		SET_BIT(victim->loc_hp[0],LOST_EYE_R);
	    else if (!IS_HEAD(victim,LOST_EYE_L))
		SET_BIT(victim->loc_hp[0],LOST_EYE_L);
	}
    }
    else if ( attack == "blast" || attack == "pound" || attack == "crush" )
    {
	damp=number_range(1,3);
	bodyloc = 0;
	if ( damp == 1)
	{
	    act("Your blow smashes through $N's chest, caving in half $S ribcage.", ch, NULL, victim, TO_CHAR);
	    act("$n's blow smashes through $N's chest, caving in half $S ribcage.", ch, NULL, victim, TO_NOTVICT);
	    act("$n's blow smashes through your chest, caving in half your ribcage.", ch, NULL, victim, TO_VICT);
	    if (IS_BODY(victim,BROKEN_RIBS_1 )) {bodyloc += 1;
		REMOVE_BIT(victim->loc_hp[1],BROKEN_RIBS_1);}
	    if (IS_BODY(victim,BROKEN_RIBS_2 )) {bodyloc += 2;
		REMOVE_BIT(victim->loc_hp[1],BROKEN_RIBS_2);}
	    if (IS_BODY(victim,BROKEN_RIBS_4 )) {bodyloc += 4;
		REMOVE_BIT(victim->loc_hp[1],BROKEN_RIBS_4);}
	    if (IS_BODY(victim,BROKEN_RIBS_8 )) {bodyloc += 8;
		REMOVE_BIT(victim->loc_hp[1],BROKEN_RIBS_8);}
	    if (IS_BODY(victim,BROKEN_RIBS_16)) {bodyloc += 16;
		REMOVE_BIT(victim->loc_hp[1],BROKEN_RIBS_16);}
	    bodyloc += number_range(1,3);
	    if (bodyloc > 24) bodyloc = 24;
	    if (bodyloc >= 16) {bodyloc -= 16;
		SET_BIT(victim->loc_hp[1],BROKEN_RIBS_16);}
	    if (bodyloc >= 8) {bodyloc -= 8;
		SET_BIT(victim->loc_hp[1],BROKEN_RIBS_8);}
	    if (bodyloc >= 4) {bodyloc -= 4;
		SET_BIT(victim->loc_hp[1],BROKEN_RIBS_4);}
	    if (bodyloc >= 2) {bodyloc -= 2;
		SET_BIT(victim->loc_hp[1],BROKEN_RIBS_2);}
	    if (bodyloc >= 1) {bodyloc -= 1;
		SET_BIT(victim->loc_hp[1],BROKEN_RIBS_1);}
	}
	else if ( damp == 2)
	{
	    act("Your blow smashes $N's spine, shattering it in several places.", ch, NULL, victim, TO_CHAR);
	    act("$n's blow smashes $N's spine, shattering it in several places.", ch, NULL, victim, TO_NOTVICT);
	    act("$n's blow smashes your spine, shattering it in several places.", ch, NULL, victim, TO_VICT);
	    if (!IS_BODY(victim,BROKEN_SPINE))
		SET_BIT(victim->loc_hp[1],BROKEN_SPINE);
	}
	else if ( damp == 3)
	{
	    if (!IS_HEAD(victim,BROKEN_SKULL))
	    {
	    	act("You swing your weapon down upon $N's head.\n\r$N's head cracks open like an overripe melon, leaking out brains.", ch, NULL, victim, TO_CHAR);
	    	act("$n swings $s weapon down upon $N's head.\n\r$N's head cracks open like an overripe melon, covering you with brains.", ch, NULL, victim, TO_NOTVICT);
	    	act("$n swings $s weapon down upon your head.\n\rYour head cracks open like an overripe melon, spilling your brains everywhere.", ch, NULL, victim, TO_VICT);
	    	make_part(victim,"brain");
		SET_BIT(victim->loc_hp[0],BROKEN_SKULL);
	    }
	    else
	    {
	    	act("You hammer your weapon into $N's side, crushing bone.", ch, NULL, victim, TO_CHAR);
	    	act("$n hammers $s weapon into $N's side, crushing bone.", ch, NULL, victim, TO_NOTVICT);
	    	act("$n hammers $s weapon into your side, crushing bone.", ch, NULL, victim, TO_VICT);
	    }
	}
    }
    else if ( !IS_NPC( ch ) && (attack == "bite" ||IS_VAMPAFF(ch,VAM_FANGS)))
    {
	act("You sink your teeth into $N's throat and tear out $S jugular vein.\n\rYou wipe the blood from your chin with one hand.", ch, NULL, victim, TO_CHAR);
	act("$n sink $s teeth into $N's throat and tears out $S jugular vein.\n\r$n wipes the blood from $s chin with one hand.", ch, NULL, victim, TO_NOTVICT);
	act("$n sink $s teeth into your throat and tears out your jugular vein.\n\r$n wipes the blood from $s chin with one hand.", ch, NULL, victim, TO_VICT);
	make_part(victim,"blood");
	if (!IS_BODY(victim,CUT_THROAT))
	    SET_BIT(victim->loc_hp[1],CUT_THROAT);
	if (!IS_BLEEDING(victim,BLEEDING_THROAT))
	    SET_BIT(victim->loc_hp[6],BLEEDING_THROAT);
    }
    else if ( !IS_NPC(ch) && (attack == "claw" || IS_VAMPAFF(ch,VAM_CLAWS)))
    {
	damp=number_range(1,2);
	if ( damp == 1 )
	{
	    act("You tear out $N's throat, showering the area with blood.", ch, NULL, victim, TO_CHAR);
	    act("$n tears out $N's throat, showering the area with blood.", ch, NULL, victim, TO_NOTVICT);
	    act("$n tears out your throat, showering the area with blood.", ch, NULL, victim, TO_VICT);
	    make_part(victim,"blood");
	    if (!IS_BODY(victim,CUT_THROAT))
		SET_BIT(victim->loc_hp[1],CUT_THROAT);
	    if (!IS_BLEEDING(victim,BLEEDING_THROAT))
	    	SET_BIT(victim->loc_hp[6],BLEEDING_THROAT);
	}
	if ( damp == 2 )
	{
	    if (!IS_HEAD(victim,LOST_EYE_L) && number_percent() < 50)
	    {
	    	act("You rip an eyeball from $N's face.", ch, NULL, victim, TO_CHAR);
	    	act("$n rips an eyeball from $N's face.", ch, NULL, victim, TO_NOTVICT);
	    	act("$n rips an eyeball from your face.", ch, NULL, victim, TO_VICT);
	    	make_part(victim,"eyeball");
		SET_BIT(victim->loc_hp[0],LOST_EYE_L);
	    }
	    else if (!IS_HEAD(victim,LOST_EYE_R))
	    {
	    	act("You rip an eyeball from $N's face.", ch, NULL, victim, TO_CHAR);
	    	act("$n rips an eyeball from $N's face.", ch, NULL, victim, TO_NOTVICT);
	    	act("$n rips an eyeball from your face.", ch, NULL, victim, TO_VICT);
	    	make_part(victim,"eyeball");
		SET_BIT(victim->loc_hp[0],LOST_EYE_R);
	    }
	    else if (!IS_HEAD(victim,LOST_EYE_L))
	    {
	    	act("You rip an eyeball from $N's face.", ch, NULL, victim, TO_CHAR);
	    	act("$n rips an eyeball from $N's face.", ch, NULL, victim, TO_NOTVICT);
	    	act("$n rips an eyeball from your face.", ch, NULL, victim, TO_VICT);
	    	make_part(victim,"eyeball");
		SET_BIT(victim->loc_hp[0],LOST_EYE_L);
	    }
	    else
	    {
	    	act("You claw open $N's chest.", ch, NULL, victim, TO_CHAR);
	    	act("$n claws open $N's chest.", ch, NULL, victim, TO_NOTVICT);
	    	act("$n claws open $N's chest.", ch, NULL, victim, TO_VICT);
	    }
	}
    }
    else if ( attack == "whip" )
    {
	act("You entangle $N around the neck, and squeeze out $S life.", ch, NULL, victim, TO_CHAR);
	act("$n entangles $N around the neck, and squeezes out $S life.", ch, NULL, victim, TO_NOTVICT);
	act("$n entangles you around the neck, and squeezes the life out of you.", ch, NULL, victim, TO_VICT);
	if (!IS_BODY(victim,BROKEN_NECK))
	    SET_BIT(victim->loc_hp[1],BROKEN_NECK);
    }
    else if ( attack == "suck" || attack == "grep" )
    {
	act("You place your weapon on $N's head and suck out $S brains.", ch, NULL, victim, TO_CHAR);
	act("$n places $s weapon on $N's head and suck out $S brains.", ch, NULL, victim, TO_NOTVICT);
	act("$n places $s weapon on your head and suck out your brains.", ch, NULL, victim, TO_VICT);
    }
    else
    {
	/*bug( "Dam_message: bad dt %d.", dt );*/
    }
    return;
}



/*
 * Disarm a creature.
 * Caller must check for successful attack.
 */
void disarm( CHAR_DATA *ch, CHAR_DATA *victim )
{
    OBJ_DATA *obj;
    char buf [MAX_STRING_LENGTH];

    /* I'm fed up of being disarmed every 10 seconds - KaVir */
    if (IS_NPC(ch) && victim->level > 2 && number_percent() > 5)
	return;

    if (!IS_IMMUNE(ch, IMM_DISARM))
	return;

/*
    if ( ( (obj = get_eq_char( ch, WEAR_WIELD ) == NULL) || obj->item_type != ITEM_WEAPON )
    &&   ( (obj = get_eq_char( ch, WEAR_HOLD  ) == NULL) || obj->item_type != ITEM_WEAPON ) )
	return;
*/
    if ( ( (obj = get_eq_char( victim, WEAR_WIELD ) ) == NULL) || obj->item_type != ITEM_WEAPON )
    {
	if ( ( (obj = get_eq_char( victim, WEAR_HOLD ) ) == NULL) || obj->item_type != ITEM_WEAPON )
	return;
    }

    sprintf(buf,"$n disarms you!");
    ADD_COLOUR(ch,buf,WHITE);
    act( buf, ch, NULL, victim, TO_VICT    );
    sprintf(buf,"You disarm $N!");
    ADD_COLOUR(ch,buf,WHITE);
    act( buf,  ch, NULL, victim, TO_CHAR    );
    sprintf(buf,"$n disarms $N!");
    ADD_COLOUR(ch,buf,WHITE);
    act( buf,  ch, NULL, victim, TO_NOTVICT );

    obj_from_char( obj );

    /* Loyal weapons come back ;)  KaVir */
    if (IS_SET(obj->extra_flags, ITEM_LOYAL) && ( !IS_NPC(victim) ) )
    {
        act( "$p leaps back into your hand!", victim, obj, NULL, TO_CHAR );
        act( "$p leaps back into $n's hand!",  victim, obj, NULL, TO_ROOM );
	obj_to_char( obj, victim );
	do_wear( victim, obj->name);
    }
    else if ( IS_NPC(victim) )
	obj_to_char( obj, victim );
    else
	obj_to_room( obj, victim->in_room );

    return;
}



/*
 * Trip a creature.
 * Caller must check for successful attack.
 */
void trip( CHAR_DATA *ch, CHAR_DATA *victim )
{
    char buf [MAX_STRING_LENGTH];

    if (IS_AFFECTED(victim,AFF_FLYING))
	return;

    if (IS_NPC(ch) && victim->level > 2 && number_percent() > 5)
	return;

    if (!IS_NPC(victim))
    {
	if (IS_CLASS(victim, CLASS_VAMPIRE) && IS_VAMPAFF(victim,VAM_FLYING))
	    return;
	if (IS_CLASS(victim, CLASS_DEMON) && IS_DEMAFF(victim, DEM_UNFOLDED))
	    return;
	if (IS_SET(victim->special, SPC_CHAMPION) && IS_DEMAFF(victim,DEM_UNFOLDED))
	    return;
	if (IS_CLASS(victim, CLASS_DROW) && IS_SET(victim->pcdata->powers[1], DPOWER_LEVITATION)) return;
    }
    if ( victim->wait == 0)
    {
    	sprintf(buf,"$n trips you and you go down!");
    	ADD_COLOUR(ch,buf,WHITE);
    	act( buf, ch, NULL, victim, TO_VICT    );
    	sprintf(buf,"You trip $N and $E goes down!");
    	ADD_COLOUR(ch,buf,WHITE);
    	act( buf,  ch, NULL, victim, TO_CHAR    );
    	sprintf(buf,"$n trips $N and $E goes down!");
    	ADD_COLOUR(ch,buf,WHITE);
    	act( buf,  ch, NULL, victim, TO_NOTVICT );

	WAIT_STATE( ch,     2 * PULSE_VIOLENCE );
	WAIT_STATE( victim, 2 * PULSE_VIOLENCE );
	victim->position = POS_RESTING;
    }

    return;
}



void do_kill( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;

    one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
	send_to_char( "Kill whom?\n\r", ch );
	return;
    }

    if ( ( victim = get_char_room( ch, arg ) ) == NULL )
    {
	send_to_char( "They aren't here.\n\r", ch );
	return;
    }

    if ( victim == ch )
    {
	send_to_char( "You cannot kill yourself!\n\r", ch );
	return;
    }

	  if ( is_safe( ch, victim ) )
	return;

    if ( IS_AFFECTED(ch, AFF_CHARM) && ch->master == victim )
    {
	act( "$N is your beloved master.", ch, NULL, victim, TO_CHAR );
	return;
    }

	
    if ( ch->position == POS_FIGHTING )
    {
	send_to_char( "You do the best you can!\n\r", ch );
	return;
    }

	/* Autostancing - Loki *
    if (!IS_NPC(victim) && IS_SET(victim->extra, EXTRA_STANCE) )
    {
      if ( victim->stance[11] == STANCE_VIPER )
        do_stance(victim,"viper");
      else if ( victim->stance[11] == STANCE_CRANE )
        do_stance(victim,"crane");
      else if ( victim->stance[11] == STANCE_CRAB )
        do_stance(victim,"crab");
      else if ( victim->stance[11] == STANCE_MONGOOSE )
        do_stance(victim,"mongoose");
      else if ( victim->stance[11] == STANCE_BULL )
        do_stance(victim,"bull");
      else if ( victim->stance[11] == STANCE_MANTIS )
        do_stance(victim,"mantis");
      else if ( victim->stance[11] == STANCE_DRAGON )
        do_stance(victim,"dragon");
      else if ( victim->stance[11] == STANCE_TIGER )
        do_stance(victim,"tiger");
      else if ( victim->stance[11] == STANCE_MONKEY )
        do_stance(victim,"monkey");
      else if ( victim->stance[11] == STANCE_XKILLA )
        do_stance(victim,"xkilla");
      else if ( victim->stance[11] == STANCE_SWALLOW )
        do_stance(victim,"swallow");
    }

    if (!IS_NPC(ch) && IS_SET(ch->extra, EXTRA_STANCE) )
    {
      if ( ch->stance[11] == STANCE_VIPER )
        do_stance(ch,"viper");
      else if ( ch->stance[11] == STANCE_CRANE )
        do_stance(ch,"crane");
      else if ( ch->stance[11] == STANCE_CRAB )
        do_stance(ch,"crab");
      else if ( ch->stance[11] == STANCE_MONGOOSE )
        do_stance(ch,"mongoose");
      else if ( ch->stance[11] == STANCE_BULL )
        do_stance(ch,"bull");
      else if ( ch->stance[11] == STANCE_MANTIS )
        do_stance(ch,"mantis");
      else if ( ch->stance[11] == STANCE_DRAGON )
        do_stance(ch,"dragon");
      else if ( ch->stance[11] == STANCE_TIGER )
        do_stance(ch,"tiger");
      else if ( ch->stance[11] == STANCE_MONKEY )
        do_stance(ch,"monkey");
      else if ( victim->stance[11] == STANCE_XKILLA )
        do_stance(victim,"xkilla");
      else if ( ch->stance[11] == STANCE_SWALLOW )
        do_stance(ch,"swallow");
    }
*/

    WAIT_STATE( ch, 1 * PULSE_VIOLENCE );
    check_killer( ch, victim );
    if (!IS_NPC(ch) && IS_CLASS(ch, CLASS_WEREWOLF) && number_range(1,3) == 1
	&& ch->power[DISC_WERE_BOAR] > 1 && victim->position == POS_STANDING)
    {

	act("You charge into $N, knocking $M from $S feet.",ch,NULL,victim,TO_CHAR);
	act("$n charge into $N, knocking $M from $S feet.",ch,NULL,victim,TO_NOTVICT);
	act("$n charge into you, knocking you from your feet.",ch,NULL,victim,TO_VICT);
	victim->position = POS_STUNNED;
	multi_hit( ch, victim, TYPE_UNDEFINED );
	multi_hit( ch, victim, TYPE_UNDEFINED );
	return;
    }
    multi_hit( ch, victim, TYPE_UNDEFINED );
    return;
}


void do_backstab( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;
    OBJ_DATA *obj;

    one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
	send_to_char( "Backstab whom?\n\r", ch );
	return;
    }

    if ( ( victim = get_char_room( ch, arg ) ) == NULL )
    {
	send_to_char( "They aren't here.\n\r", ch );
	return;
    }

    if ( victim == ch )
    {
	send_to_char( "How can you sneak up on yourself?\n\r", ch );
	return;
    }

    if ( is_safe( ch, victim ) )
      return;

    if ( ( ( obj = get_eq_char( ch, WEAR_WIELD ) ) == NULL
    ||   obj->value[3] != 11 )
    &&   ( ( obj = get_eq_char( ch, WEAR_HOLD ) ) == NULL
    ||   obj->value[3] != 11 ) )
    {
	send_to_char( "You need to wield a piercing weapon.\n\r", ch );
	return;
    }

    if ( victim->fighting != NULL )
    {
	send_to_char( "You can't backstab a fighting person.\n\r", ch );
	return;
    }

    if ( victim->hit < victim->max_hit )
    {
	act( "$N is hurt and suspicious ... you can't sneak up.",
	    ch, NULL, victim, TO_CHAR );
	return;
    }

    check_killer( ch, victim );
    WAIT_STATE( ch, skill_table[gsn_backstab].beats );
    if ( !IS_NPC(victim) 
	  && IS_IMMUNE(victim, IMM_BACKSTAB)
	  && ch->power[DISC_VAMP_QUIE] < 10 &&
	(!IS_CLASS(ch, CLASS_NINJA) || ch->pcdata->powers[NPOWER_NINGENNO] < 2))
	damage( ch, victim, 0, gsn_backstab );
    else if ( !IS_AWAKE(victim)
    ||   IS_NPC(ch)
    ||   number_percent( ) < ch->pcdata->learned[gsn_backstab] )
	multi_hit( ch, victim, gsn_backstab );
    else
	damage( ch, victim, 0, gsn_backstab );

    return;
}



void do_flee( CHAR_DATA *ch, char *argument )
{
    ROOM_INDEX_DATA *was_in;
    ROOM_INDEX_DATA *now_in;
    CHAR_DATA *victim;
    char buf[MAX_STRING_LENGTH];    
    int attempt;

    if ( ( victim = ch->fighting ) == NULL )
    {
	if ( ch->position == POS_FIGHTING )
	    ch->position = POS_STANDING;
	send_to_char( "You aren't fighting anyone.\n\r", ch );
	return;
    }
	if(ch->move <= 0)
	{
	   send_to_char("You can't find the energy to leave.\n\r",ch);
	   return;
	}

    if (IS_AFFECTED(ch, AFF_WEBBED))
    {
	send_to_char("You are unable to move with all this sticky webbing on.\n\r",ch);
	return;
    }

    if ( IS_SET(ch->flag2, AFF_TOTALBLIND) )
    {
	stc("You are unable to see anything, let alone flee!\n\r",victim);
	return;
    }

   if (IS_CLASS(victim, CLASS_VAMPIRE) &&

IS_SET(victim->newbits,NEW_COIL) && number_percent( ) > 15)
{
sprintf(buf,"Not with %s coiled around you!",victim->name);
send_to_char(buf,ch);
act("$n cant escape with $N coiled around them.",ch,NULL,victim,TO_ROOM);
return;
}

 if (IS_CLASS(victim, CLASS_MONK) &&

IS_SET(victim->newbits,NEW_JAWLOCK) && number_percent( ) > 30)
{
sprintf(buf,"Not with god holding you!\n\r");
send_to_char(buf,ch);
act("$n cant escape god's mighty hold!",ch,NULL,victim,TO_ROOM);
return;
}

   if (IS_CLASS(victim, CLASS_WEREWOLF) && 

IS_SET(victim->newbits,NEW_JAWLOCK) && number_percent( ) > 10)
{
sprintf(buf,"Not with %s's jaws clamped on your neck!",victim->name);
send_to_char(buf,ch);
act("$n cant escape $N's clamped jaws!",ch,NULL,victim,TO_ROOM);
return;
}
    if (IS_AFFECTED(ch,AFF_TENDRILS))
{
send_to_char("You are unable to move while tangled in tendrils.\n\r",ch);
return;
}

    if (!IS_NPC(ch) && ch->rage >= 0)
    {
	if (IS_CLASS(ch,CLASS_VAMPIRE) && number_percent() <= ch->rage)
	{
	    send_to_char("Your inner beast refuses to let you run!\n\r",ch);
	    WAIT_STATE(ch,12);
	    return;
	}
	else if (IS_CLASS(ch, CLASS_WEREWOLF) && number_percent() <= ch->rage * 0.3)
	{
	    send_to_char("Your rage is too great!\n\r",ch);
	    WAIT_STATE(ch,12);
	    return;
	}
    }
    was_in = ch->in_room;
    for ( attempt = 0; attempt < 6; attempt++ )
    {
	EXIT_DATA *pexit;
	int door;

	door = number_door( );
	if ( ( pexit = was_in->exit[door] ) == 0
	||   pexit->to_room == NULL
	||   IS_SET(pexit->exit_info, EX_CLOSED)
	|| ( IS_NPC(ch)
	&&   IS_SET(pexit->to_room->room_flags, ROOM_NO_MOB) ) )
	    continue;

	move_char( ch, door );
	if ( ( now_in = ch->in_room ) == was_in )
	    continue;
        
	ch->in_room = was_in;
	act( "$n has fled!", ch, NULL, NULL, TO_ROOM );
	ch->in_room = now_in;

	if ( !IS_NPC(ch) )
	    send_to_char( "You flee from combat!  Coward!\n\r", ch );

	stop_fighting( ch, TRUE );
	return;
    }

    send_to_char( "You were unable to escape!\n\r", ch );
    return;
}



void do_rescue( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;
    CHAR_DATA *fch;

    one_argument( argument, arg );
    if ( arg[0] == '\0' )
    {
	send_to_char( "Rescue whom?\n\r", ch );
	return;
    }

    if ( ( victim = get_char_room( ch, arg ) ) == NULL )
    {
	send_to_char( "They aren't here.\n\r", ch );
	return;
    }

    if ( victim == ch )
    {
	send_to_char( "What about fleeing instead?\n\r", ch );
	return;
    }

    if ( !IS_NPC(ch) && IS_NPC(victim) )
    {
	send_to_char( "Doesn't need your help!\n\r", ch );
	return;
    }

    if ( ch->fighting == victim )
    {
	send_to_char( "Too late.\n\r", ch );
	return;
    }

    if ( ( fch = victim->fighting ) == NULL )
    {
	send_to_char( "That person is not fighting right now.\n\r", ch );
	return;
    }

    if ( is_safe(ch, fch) || is_safe(ch, victim) ) return;

    WAIT_STATE( ch, skill_table[gsn_rescue].beats );
    if ( !IS_NPC(ch) && number_percent( ) > ch->pcdata->learned[gsn_rescue] )
    {
	send_to_char( "You fail the rescue.\n\r", ch );
	return;
    }

    act( "You rescue $N!",  ch, NULL, victim, TO_CHAR    );
    act( "$n rescues you!", ch, NULL, victim, TO_VICT    );
    act( "$n rescues $N!",  ch, NULL, victim, TO_NOTVICT );

    stop_fighting( fch, FALSE );
    stop_fighting( victim, FALSE );

    check_killer( ch, fch );
    set_fighting( ch, fch );
    set_fighting( fch, ch );

    do_humanity(ch,"");
    return;
}



void do_kick( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *victim;
    OBJ_DATA *boots;
    int dam;
    int stance;
    int hitpoints;

    if ( !IS_NPC(ch)
    &&   ch->level < skill_table[gsn_kick].skill_level)
    {
	send_to_char(
	    "First you should learn to kick.\n\r", ch );
	return;
    }

    if ( ( victim = ch->fighting ) == NULL )
    {
	send_to_char( "You aren't fighting anyone.\n\r", ch );
	return;
    }

    WAIT_STATE( ch, skill_table[gsn_kick].beats );
    if ( IS_NPC(ch) || number_percent( ) < ch->pcdata->learned[gsn_kick] )
    	dam = number_range(1,4);
    else
    {
    	dam = 0;
    	damage( ch, victim, dam, gsn_kick );
	return;
    }

    dam += char_damroll(ch);
    if (dam == 0) dam = 1;
    if ( !IS_AWAKE(victim) )
	dam *= 2;


    if ( !IS_NPC(victim) && IS_CLASS(victim, CLASS_WEREWOLF) )
    {
	if (IS_SET(victim->special, SPC_WOLFMAN)) dam *= 0.8;
	if (victim->power[DISC_WERE_BOAR] > 2 ) dam *= 0.5;
	if ( ( boots = get_eq_char( ch, WEAR_FEET ) ) != NULL 
	    && IS_SET(boots->spectype, SITEM_SILVER)) dam *= 2;
    }

       

    if ( !IS_NPC(ch) ) dam = dam + (dam * ((ch->wpn[0]+1) / 100));

    if ( !IS_NPC(ch) )
    {
	stance = ch->stance[0];
	if ( IS_STANCE(ch, STANCE_NORMAL) ) dam *= 1.25;
	else dam = dambonus(ch,victim,dam,stance);
    }

    if ( !IS_NPC(victim) && dam > 750 )
	dam = 750;
    if ( dam <= 0 )
	dam = 2;

    dam = cap_dam(ch, victim, dam);

    hitpoints = victim->hit;

	if ( !IS_NPC(victim) && IS_IMMUNE(victim, IMM_KICK) &&
!(IS_CLASS(ch, CLASS_DEMON) && IS_DEMAFF(ch, DEM_HOOVES)))
    {
	if (IS_CLASS(ch, CLASS_DEMON) && IS_DEMAFF(ch, DEM_HOOVES))
	dam = dam /2 ;
	victim->hit += dam;
	damage( ch, victim, dam, gsn_kick );
	victim->hit = hitpoints;
    }
    else if IS_IMMUNE(victim, IMM_KICK)
    {
    dam=dam/2;
    }
else damage( ch, victim, dam, gsn_kick );
    return;
}


int dambonus( CHAR_DATA *ch, CHAR_DATA *victim, int dam, int stance)
{
    if (dam < 1) return 0;
    if (stance < 1) return dam;
    if (!IS_NPC(ch) && !can_counter(victim))
    {
	if ( IS_STANCE(ch, STANCE_MONKEY) )
	{
	    int mindam = dam * 0.25;
	    dam *= (ch->stance[STANCE_MONKEY]+1) / 200;
	    if (dam < mindam) dam = mindam;
	}
	else if ( IS_STANCE(ch, STANCE_BULL) && ch->stance[STANCE_BULL] > 100 )
	    dam += dam * (ch->stance[STANCE_BULL] / 100);
	else if ( IS_STANCE(ch, STANCE_DRAGON) && ch->stance[STANCE_DRAGON] > 100 )
	    dam += dam * (ch->stance[STANCE_DRAGON] / 100);
         else if ( IS_STANCE(ch, STANCE_WOLF) && ch->stance[STANCE_WOLF] > 100)
           dam += dam*(ch->stance[STANCE_DRAGON] /100);
	else if ( IS_STANCE(ch, STANCE_TIGER) && ch->stance[STANCE_TIGER] > 100 )
	    dam += dam * (ch->stance[STANCE_TIGER] / 100);
        else if ( IS_STANCE(ch, STANCE_XKILLA) && ch->stance[STANCE_XKILLA] > 100 )
            dam += dam * (ch->stance[STANCE_XKILLA] / 100);
	else if ( ch->stance[0] > 0 && ch->stance[stance] < 100 )
	    dam *= 0.5;
    }
    if (!IS_NPC(victim) && !can_counter(ch))
    {
	if ( IS_STANCE(victim, STANCE_CRAB) && victim->stance[STANCE_CRAB] > 100 )
	    dam /= victim->stance[STANCE_CRAB]/100;
        else if ( IS_STANCE(victim, STANCE_DRAGON) && victim->stance[STANCE_DRAGON] > 100)
            dam /= victim->stance[STANCE_DRAGON]/100;
	else if ( IS_STANCE(victim, STANCE_DRAGON) && victim->stance[STANCE_DRAGON] > 100 )
	    dam /= victim->stance[STANCE_DRAGON]/100;
	else if ( IS_STANCE(victim, STANCE_SWALLOW) && victim->stance[STANCE_SWALLOW] > 100 )
	    dam /= victim->stance[STANCE_SWALLOW]/100;
        else if ( IS_STANCE(victim, STANCE_XKILLA) && victim->stance[STANCE_XKILLA] > 100 )
            dam /= victim->stance[STANCE_XKILLA]/100;
    }
    return dam;
}



void do_punch( CHAR_DATA *ch, char *argument )
{
    char arg [MAX_INPUT_LENGTH];
    CHAR_DATA *victim;
    int dam;
    int store;
    bool broke = FALSE;

    one_argument(argument,arg);

    if ( IS_NPC(ch) ) return;

    if ( ch->level < skill_table[gsn_punch].skill_level)
    {
	send_to_char( "First you should learn to punch.\n\r", ch );
	return;
    }

    if ( ( victim = get_char_room( ch, arg ) ) == NULL )
    {
	send_to_char( "You aren't fighting anyone.\n\r", ch );
	return;
    }

    if ( ch == victim )
    {
	send_to_char( "You cannot punch yourself!\n\r", ch );
	return;
    }

    if (is_safe(ch,victim)) return;

    if ( victim->hit < victim->max_hit )
    {
	send_to_char( "They are hurt and suspicious.\n\r", ch );
	return;
    }

    if ( victim->position < POS_FIGHTING )
    {
	send_to_char( "You can only punch someone who is standing.\n\r", ch );
	return;
    }

    act("You draw your fist back and aim a punch at $N.",ch,NULL,victim,TO_CHAR);
    act("$n draws $s fist back and aims a punch at you.",ch,NULL,victim,TO_VICT);
    act("$n draws $s fist back and aims a punch at $N.",ch,NULL,victim,TO_NOTVICT);
    WAIT_STATE( ch, skill_table[gsn_punch].beats );
    if ( IS_NPC(ch) || number_percent( ) < ch->pcdata->learned[gsn_punch] )
    	dam = number_range(1,4);
    else
    {
    	dam = 0;
    	damage( ch, victim, dam, gsn_punch );
	return;
    }

    dam += char_damroll(ch);
    if (dam == 0) dam = 1;
    if ( !IS_AWAKE(victim) )
	dam *= 2;
    if ( !IS_NPC(ch) ) dam = dam + (dam * (ch->wpn[0] / 100));

    if ( dam <= 0 )
	dam = 1;

    if (!IS_NPC(victim) && IS_CLASS(victim, CLASS_WEREWOLF) && victim->power[DISC_WERE_BOAR] > 3)
    {
	store = victim->hit;
	victim->hit += dam;
	damage( ch, victim, dam, gsn_punch );
	victim->hit = store;
	if (number_percent() <= 25 && !IS_ARM_L(ch,LOST_ARM) && 
	!IS_ARM_L(ch,LOST_HAND))
	{
	    if (!IS_ARM_L(ch, LOST_FINGER_I) && !IS_ARM_L(ch, BROKEN_FINGER_I))
		{SET_BIT(ch->loc_hp[LOC_ARM_L],BROKEN_FINGER_I);broke = TRUE;}
	    if (!IS_ARM_L(ch, LOST_FINGER_M) && !IS_ARM_L(ch, BROKEN_FINGER_M))
		{SET_BIT(ch->loc_hp[LOC_ARM_L],BROKEN_FINGER_M);broke = TRUE;}
	    if (!IS_ARM_L(ch, LOST_FINGER_R) && !IS_ARM_L(ch, BROKEN_FINGER_R))
		{SET_BIT(ch->loc_hp[LOC_ARM_L],BROKEN_FINGER_R);broke = TRUE;}
	    if (!IS_ARM_L(ch, LOST_FINGER_L) && !IS_ARM_L(ch, BROKEN_FINGER_L))
		{SET_BIT(ch->loc_hp[LOC_ARM_L],BROKEN_FINGER_L);broke = TRUE;}
	    if (broke)
	    {
		act("The fingers on your left hand shatter under the impact of the blow!",ch,NULL,NULL,TO_CHAR);
		act("The fingers on $n's left hand shatter under the impact of the blow! ",ch,NULL,NULL,TO_ROOM);
	    }
	}
	else if (number_percent() <= 25 && !IS_ARM_R(ch,LOST_ARM) && 
	!IS_ARM_R(ch,LOST_HAND))
	{
	    if (!IS_ARM_R(ch, LOST_FINGER_I) && !IS_ARM_R(ch, BROKEN_FINGER_I))
		{SET_BIT(ch->loc_hp[LOC_ARM_R],BROKEN_FINGER_I);broke = TRUE;}
	    if (!IS_ARM_R(ch, LOST_FINGER_M) && !IS_ARM_R(ch, BROKEN_FINGER_M))
		{SET_BIT(ch->loc_hp[LOC_ARM_R],BROKEN_FINGER_M);broke = TRUE;}
	    if (!IS_ARM_R(ch, LOST_FINGER_R) && !IS_ARM_R(ch, BROKEN_FINGER_R))
		{SET_BIT(ch->loc_hp[LOC_ARM_R],BROKEN_FINGER_R);broke = TRUE;}
	    if (!IS_ARM_R(ch, LOST_FINGER_L) && !IS_ARM_R(ch, BROKEN_FINGER_L))
		{SET_BIT(ch->loc_hp[LOC_ARM_R],BROKEN_FINGER_L);broke = TRUE;}
	    if (broke)
	    {
		act("The fingers on your right hand shatter under the impact of the blow!",ch,NULL,NULL,TO_CHAR);
		act("The fingers on $n's right hand shatter under the impact of the blow! ",ch,NULL,NULL,TO_ROOM);
	    }
	}
	stop_fighting(victim,TRUE);
	return;
    }

    damage( ch, victim, dam, gsn_punch );
    if (victim == NULL || victim->position == POS_DEAD || dam < 1) return;
    if (victim->position == POS_FIGHTING) stop_fighting(victim,TRUE);
    if (number_percent() <= 25 && !IS_HEAD(victim,BROKEN_NOSE)
	 && !IS_HEAD(victim,LOST_NOSE))
    {
    	act("Your nose shatters under the impact of the blow!",victim,NULL,NULL,TO_CHAR);
    	act("$n's nose shatters under the impact of the blow!",victim,NULL,NULL,TO_ROOM);
	SET_BIT(victim->loc_hp[LOC_HEAD],BROKEN_NOSE);
    }
    else if (number_percent() <= 25 && !IS_HEAD(victim,BROKEN_JAW))
    {
    	act("Your jaw shatters under the impact of the blow!",victim,NULL,NULL,TO_CHAR);
    	act("$n's jaw shatters under the impact of the blow!",victim,NULL,NULL,TO_ROOM);
	SET_BIT(victim->loc_hp[LOC_HEAD],BROKEN_JAW);
    }
    act("You fall to the ground stunned!",victim,NULL,NULL,TO_CHAR);
    act("$n falls to the ground stunned!",victim,NULL,NULL,TO_ROOM);
    victim->position = POS_STUNNED;
    return;
}


void do_berserk( CHAR_DATA *ch, char *argument )
{
    char       arg [MAX_INPUT_LENGTH];
    CHAR_DATA *vch;
    CHAR_DATA *vch_next;
    CHAR_DATA *mount;
    int number_hit = 0;

    argument = one_argument( argument, arg );

    if (IS_NPC(ch)) return;

    if ( ch->level < skill_table[gsn_berserk].skill_level)
    {
	send_to_char( "You are not wild enough to go berserk.\n\r", ch );
	return;
    }

    WAIT_STATE( ch, skill_table[gsn_berserk].beats );
    if ( number_percent( ) > ch->pcdata->learned[gsn_berserk] )
    {
    	act("You rant and rave, but nothing much happens.",ch,NULL,NULL,TO_CHAR);
    	act("$n gets a wild look in $s eyes, but nothing much happens.",ch,NULL,NULL,TO_ROOM);
	return;
    }

    act("You go BERSERK!",ch,NULL,NULL,TO_CHAR);
    act("$n goes BERSERK!",ch,NULL,NULL,TO_ROOM);
	
	/* Autostancing - Loki *
    if (!IS_NPC(ch) && IS_SET(ch->extra, EXTRA_STANCE) )
    {
      if ( ch->stance[11] == STANCE_VIPER )
        do_stance(ch,"viper");
      else if ( ch->stance[11] == STANCE_CRANE )
        do_stance(ch,"crane");
      else if ( ch->stance[11] == STANCE_CRAB )
        do_stance(ch,"crab");
      else if ( ch->stance[11] == STANCE_MONGOOSE )
        do_stance(ch,"mongoose");
      else if ( ch->stance[11] == STANCE_BULL )
        do_stance(ch,"bull");
      else if ( ch->stance[11] == STANCE_MANTIS )
        do_stance(ch,"mantis");
      else if ( ch->stance[11] == STANCE_DRAGON )
        do_stance(ch,"dragon");
      else if ( ch->stance[11] == STANCE_TIGER )
        do_stance(ch,"tiger");
      else if ( ch->stance[11] == STANCE_MONKEY )
        do_stance(ch,"monkey");
      else if ( ch->stance[11] == STANCE_SWALLOW )
        do_stance(ch,"swallow");
      else if ( ch->stance[11] == STANCE_XKILLA )
        do_stance(ch,"xkilla");
    }
*/



    for ( vch = char_list; vch != NULL; vch = vch_next )
    {
	vch_next	= vch->next;
	if (number_hit > 4) continue;
	if ( vch->in_room == NULL )
	    continue;
	if ( !IS_NPC(vch) && vch->pcdata->chobj != NULL )
	    continue;
	if ( ch == vch )
	    continue;
	if ( vch->in_room == ch->in_room )
	{
	    if ( ( mount = ch->mount ) != NULL ) {if ( mount == vch ) continue;}
	    if (can_see(ch,vch))
	    {
/*
    if (!IS_NPC(vch) && IS_SET(vch->extra, EXTRA_STANCE) )
    {
      if ( vch->stance[11] == STANCE_VIPER )
        do_stance(vch,"viper");
      else if ( vch->stance[11] == STANCE_CRANE )
        do_stance(vch,"crane");
      else if ( vch->stance[11] == STANCE_CRAB )
        do_stance(vch,"crab");
      else if ( vch->stance[11] == STANCE_MONGOOSE )
        do_stance(vch,"mongoose");
      else if ( vch->stance[11] == STANCE_BULL )
        do_stance(vch,"bull");
      else if ( vch->stance[11] == STANCE_MANTIS )
        do_stance(vch,"mantis");
      else if ( vch->stance[11] == STANCE_DRAGON )
        do_stance(vch,"dragon");
      else if ( vch->stance[11] == STANCE_TIGER )
        do_stance(vch,"tiger");
      else if ( vch->stance[11] == STANCE_MONKEY )
        do_stance(vch,"monkey");
      else if ( vch->stance[11] == STANCE_SWALLOW )
        do_stance(vch,"swallow");
      else if ( vch->stance[11] == STANCE_XKILLA )
        do_stance(vch,"xkilla");
    }
*/
		multi_hit( ch, vch, TYPE_UNDEFINED );
                          number_hit++;
	        
  if (IS_CLASS(ch,CLASS_WEREWOLF) && ch->power[DISC_WERE_RAPT] >0)
                {
                send_to_char("You go NUTS!\n\r",ch);
                multi_hit(ch,vch,TYPE_UNDEFINED);
                          number_hit++;
                }
	    }
	}
    }
    do_beastlike(ch,"");
    return;
}
       


/* Hurl skill by KaVir */
void do_hurl( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA       *victim;
    CHAR_DATA       *mount;
    ROOM_INDEX_DATA *to_room;
    EXIT_DATA       *pexit;
    EXIT_DATA       *pexit_rev;
    char            buf       [MAX_INPUT_LENGTH];
    char            direction [MAX_INPUT_LENGTH];
    char            arg1      [MAX_INPUT_LENGTH];
    char            arg2      [MAX_INPUT_LENGTH];
    int             door;
    int             rev_dir;
    int             dam;

    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );

    if ( !IS_NPC(ch) && ch->pcdata->learned[gsn_hurl] < 1 )
    {
	send_to_char("Maybe you should learn the skill first?\n\r",ch);
	return;
    }

    if ( arg1[0] == '\0' )
    {
	send_to_char("Who do you wish to hurl?\n\r", ch);
	return;
    }

    if ( ( victim = get_char_room( ch, arg1 ) ) == NULL )
    {
	send_to_char("They are not here.\n\r", ch);
	return;
    }

    if ( victim == ch )
    {
	send_to_char( "How can you hurl yourself?\n\r", ch );
	return;
    }

    if ( is_safe( ch, victim ) )
      return;

    if ( (mount = victim->mount) != NULL && victim->mounted == IS_MOUNT)
    {
	send_to_char("But they have someone on their back!\n\r",ch);
	return;
    }
    else if ( (mount = victim->mount) != NULL && victim->mounted == IS_RIDING)
    {
	send_to_char("But they are riding!\n\r",ch);
	return;
    }

    if ( !IS_NPC(victim) && IS_IMMUNE(victim, IMM_HURL) )
    {
	send_to_char("You are unable to get their feet of the ground.\n\r",ch);
	return;
    }

    if ( IS_NPC(victim) && victim->level > 900 )
    {
	send_to_char("You are unable to get their feet of the ground.\n\r",ch);
	return;
    }

    if ( (victim->hit < victim->max_hit)
	 || ( victim->position == POS_FIGHTING && victim->fighting != ch) )
    {
	act( "$N is hurt and suspicious, and you are unable to approach $M.",
	    ch, NULL, victim, TO_CHAR );
	return;
    }

    WAIT_STATE( ch, skill_table[gsn_hurl].beats );
    if ( !IS_NPC(ch) && number_percent( ) > ch->pcdata->learned[gsn_hurl] )
    {
	    send_to_char("You are unable to get their feet of the ground.\n\r",ch);
	    multi_hit( victim, ch, TYPE_UNDEFINED );
	    return;
    }

    rev_dir = 0;

    if ( arg2[0] == '\0' )
	door = number_range(0,3);
    else
    {
    	if      ( !str_cmp( arg2, "n" ) || !str_cmp( arg2, "north" ) )
	    door = 0;
    	else if ( !str_cmp( arg2, "e" ) || !str_cmp( arg2, "east" ) )
	    door = 1;
    	else if ( !str_cmp( arg2, "s" ) || !str_cmp( arg2, "south" ) )
	    door = 2;
    	else if ( !str_cmp( arg2, "w" ) || !str_cmp( arg2, "west" ) )
	    door = 3;
    	else
    	{
	    send_to_char("You can only hurl people north, south, east or west.\n\r", ch);
	    return;
    	}
    }

    if (door == 0) {sprintf(direction,"north");rev_dir = 2;}
    if (door == 1) {sprintf(direction,"east");rev_dir = 3;}
    if (door == 2) {sprintf(direction,"south");rev_dir = 0;}
    if (door == 3) {sprintf(direction,"west");rev_dir = 1;}

    if (( pexit = ch->in_room->exit[door]) == NULL || (to_room = pexit->to_room) == NULL)
    {
	sprintf(buf,"$n hurls $N into the %s wall.", direction);
	act(buf,ch,NULL,victim,TO_NOTVICT);
	sprintf(buf,"You hurl $N into the %s wall.", direction);
	act(buf,ch,NULL,victim,TO_CHAR);
	sprintf(buf,"$n hurls you into the %s wall.", direction);
	act(buf,ch,NULL,victim,TO_VICT);
	dam = number_range(ch->level, (ch->level * 4));
	victim->hit = victim->hit - dam;
	update_pos(victim);
    	if (IS_NPC(victim) && !IS_NPC(ch)) ch->mkill = ch->mkill + 1;
    	if (!IS_NPC(victim) && IS_NPC(ch)) victim->mdeath = victim->mdeath + 1;
	if (victim->position == POS_DEAD) {raw_kill(victim);return;}
	return;
    }

    pexit = victim->in_room->exit[door];
    if (IS_SET(pexit->exit_info, EX_CLOSED) && 
	!IS_AFFECTED(victim,AFF_PASS_DOOR ) &&
	!IS_AFFECTED(victim,AFF_ETHEREAL  ) )
    {
	if (IS_SET(pexit->exit_info, EX_LOCKED))
	    REMOVE_BIT(pexit->exit_info, EX_LOCKED);
	if (IS_SET(pexit->exit_info, EX_CLOSED))
	    REMOVE_BIT(pexit->exit_info, EX_CLOSED);
	sprintf(buf,"$n hoists $N in the air and hurls $M %s.", direction);
	act(buf,ch,NULL,victim,TO_NOTVICT);
	sprintf(buf,"You hoist $N in the air and hurl $M %s.", direction);
	act(buf,ch,NULL,victim,TO_CHAR);
	sprintf(buf,"$n hurls you %s, smashing you through the %s.", direction,pexit->keyword);
	act(buf,ch,NULL,victim,TO_VICT);
	sprintf(buf,"There is a loud crash as $n smashes through the $d.");
	act(buf,victim,NULL,pexit->keyword,TO_ROOM);

	if ( ( to_room   = pexit->to_room               ) != NULL
	&&   ( pexit_rev = to_room->exit[rev_dir] ) != NULL
	&&   pexit_rev->to_room == ch->in_room
	&&   pexit_rev->keyword != NULL )
	{
	    if (IS_SET(pexit_rev->exit_info, EX_LOCKED))
		REMOVE_BIT( pexit_rev->exit_info, EX_LOCKED );
	    if (IS_SET(pexit_rev->exit_info, EX_CLOSED))
		REMOVE_BIT( pexit_rev->exit_info, EX_CLOSED );
	    if (door == 0) sprintf(direction,"south");
	    if (door == 1) sprintf(direction,"west");
	    if (door == 2) sprintf(direction,"north");
	    if (door == 3) sprintf(direction,"east");
	    char_from_room(victim);
	    char_to_room(victim,to_room);
	    sprintf(buf,"$n comes smashing in through the %s $d.", direction);
	    act(buf,victim,NULL,pexit->keyword,TO_ROOM);
	    dam = number_range(ch->level, (ch->level * 6));
	    victim->hit = victim->hit - dam;
	    update_pos(victim);
    	    if (IS_NPC(victim) && !IS_NPC(ch)) ch->mkill = ch->mkill + 1;
    	    if (!IS_NPC(victim) && IS_NPC(ch)) victim->mdeath = victim->mdeath + 1;
	    if (victim->position == POS_DEAD) {raw_kill(victim);return;}
	}
    }
    else
    {
	sprintf(buf,"$n hurls $N %s.", direction);
	act(buf,ch,NULL,victim,TO_NOTVICT);
	sprintf(buf,"You hurl $N %s.", direction);
	act(buf,ch,NULL,victim,TO_CHAR);
	sprintf(buf,"$n hurls you %s.", direction);
	act(buf,ch,NULL,victim,TO_VICT);
	if (door == 0) sprintf(direction,"south");
	if (door == 1) sprintf(direction,"west");
	if (door == 2) sprintf(direction,"north");
	if (door == 3) sprintf(direction,"east");
	char_from_room(victim);
	char_to_room(victim,to_room);
	sprintf(buf,"$n comes flying in from the %s.", direction);
	act(buf,victim,NULL,NULL,TO_ROOM);
	dam = number_range(ch->level, (ch->level * 2));
	victim->hit = victim->hit - dam;
	update_pos(victim);
    	if (IS_NPC(victim) && !IS_NPC(ch)) ch->mkill = ch->mkill + 1;
    	if (!IS_NPC(victim) && IS_NPC(ch)) victim->mdeath = victim->mdeath + 1;
	if (victim->position == POS_DEAD) {raw_kill(victim);return;}
    }
    return;
}



void do_disarm( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *victim;
    OBJ_DATA *obj;
    int percent;

    if ( !IS_NPC(ch)
    &&   ch->level < skill_table[gsn_disarm].skill_level)
    {
	send_to_char( "You don't know how to disarm opponents.\n\r", ch );
	return;
    }

    if ( ( get_eq_char( ch, WEAR_WIELD ) == NULL )
    &&   ( get_eq_char( ch, WEAR_HOLD  ) == NULL ) )
    {
	send_to_char( "You must wield a weapon to disarm.\n\r", ch );
	return;
    }

    if ( ( victim = ch->fighting ) == NULL )
    {
	send_to_char( "You aren't fighting anyone.\n\r", ch );
	return;
    }

    if ( ( ( obj = get_eq_char( victim, WEAR_WIELD ) ) == NULL )
    &&   ( ( obj = get_eq_char( victim, WEAR_HOLD  ) ) == NULL ) )
    {
	send_to_char( "Your opponent is not wielding a weapon.\n\r", ch );
	return;
    }

    WAIT_STATE( ch, skill_table[gsn_disarm].beats );
    percent = number_percent( ) + victim->level - ch->level;
    if (!IS_NPC(victim) && IS_IMMUNE(victim, IMM_DISARM))
	send_to_char( "You failed.\n\r", ch );
    else if ( IS_NPC(ch) || percent < ch->pcdata->learned[gsn_disarm] * 2 / 3 )
	disarm( ch, victim );
    else
	send_to_char( "You failed.\n\r", ch );
    return;
}



void do_sla( CHAR_DATA *ch, char *argument )
{
    send_to_char( "If you want to SLAY, spell it out.\n\r", ch );
    return;
}



void do_slay( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *victim;
    char arg[MAX_INPUT_LENGTH];

    one_argument( argument, arg );
    if ( arg[0] == '\0' )
    {
	send_to_char( "Slay whom?\n\r", ch );
	return;
    }

    if ( ( victim = get_char_room( ch, arg ) ) == NULL )
    {
	send_to_char( "They aren't here.\n\r", ch );
	return;
    }

    if ( ch == victim )
    {
	send_to_char( "Suicide is a mortal sin.\n\r", ch );
	return;
    }

    if (IS_SET(victim->act, PLR_GODLESS))
    {
	send_to_char( "You failed.\n\r", ch );
	return;
    }

    if ( !IS_NPC(victim) && victim->level >= ch->level )
    {
	send_to_char( "You failed.\n\r", ch );
	return;
    }

    act( "You slay $M in cold blood!",  ch, NULL, victim, TO_CHAR    );
    act( "$n slays you in cold blood!", ch, NULL, victim, TO_VICT    );
    act( "$n slays $N in cold blood!",  ch, NULL, victim, TO_NOTVICT );
    raw_kill( victim );
    return;
}

/* Had problems with people not dying when POS_DEAD...KaVir */
void do_killperson( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *victim;
    char arg[MAX_INPUT_LENGTH];

    one_argument( argument, arg );
    if ( arg[0] == '\0' )
	return;
    if ( ( victim = get_char_room( ch, arg ) ) == NULL )
	return;
    send_to_char( "You have been KILLED!!\n\r\n\r", victim );
    if (IS_NPC(victim) && !IS_NPC(ch)) ch->mkill = ch->mkill + 1;
    if (!IS_NPC(victim) && IS_NPC(ch)) victim->mdeath = victim->mdeath + 1;
    raw_kill( victim );
    return;
}

/* Demons Soul Stealing Shit. */
void do_stealsoul( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *victim;
    char arg[MAX_INPUT_LENGTH];
    char buf[MAX_STRING_LENGTH];

    one_argument( argument, arg );

    if ( IS_NPC(ch) ) return;

    if (!IS_CLASS(ch, CLASS_DEMON))
    {
	stc("Huh?\n\r",ch);
      return;
    }

    if ( arg[0] == '\0' )
    {
	send_to_char( "Who's soul are you going to steal?\n\r", ch );
	return;
    }

    if ( ( victim = get_char_room( ch, arg ) ) == NULL )
    {
	send_to_char( "They aren't here.\n\r", ch );
	return;
    }

    if ( IS_CLASS(victim, CLASS_DEMON) )
    {
        stc("Demons don't have a soul!\n\r",ch);
        return;
    }


    if ( ch == victim )
    {
	send_to_char( "That might be a bit tricky...\n\r", ch );
	return;
    }

    if ( IS_NPC(victim) )
    {
	send_to_char( "You can only steal the soul of other players.\n\r", ch );
	return;
    }

    if ( !CAN_PK(ch) )
    {
	send_to_char( "You must be an avatar to steal someone's soul.\n\r", ch );
	return;
    }

    if ( !CAN_PK(victim) )
    {
	send_to_char( "You can only steal the soul of other Avatars.\n\r", ch );
	return;
    }

    if ( victim->position > 1 )
    {
	send_to_char( "You can only do this to mortally wounded players.\n\r", ch );
	return;
    }

    if (is_safe(ch,victim)) return;

    act( "You tear $N's soul out of their body!",  ch, NULL, victim, TO_CHAR    );
    send_to_char( "Your soul is torn from your body!\n\r", victim);
    act( "$n tears $N's soul out of $M lifeless body!",  ch, NULL, victim, TO_NOTVICT );
    act( "$n's soul is torn from $m body!", victim, NULL, NULL, TO_ROOM);
    
    if (( ch->race - victim->race > 10 && ch->race > 0)
	|| (victim->race == 0 && ch->race > 0)  )
    {
	ch->paradox[0] += ch->race;
	ch->paradox[1] += ch->race;
	ch->paradox[2] += PARADOX_TICK;
    	sprintf(buf,"%s's has been stolen by %s.",victim->pcdata->switchname,ch->pcdata->switchname);
    	do_info(ch,buf);
	sprintf( log_buf, "%s soulstealed by %s at %d for no status.",
	    victim->pcdata->switchname, ch->pcdata->switchname, victim->in_room->vnum );
	log_string( log_buf );
        do_paradox(ch,"self");
	if (IS_CLASS(victim, CLASS_VAMPIRE)) do_mortalvamp(victim,"");
	else if (IS_SET(victim->special, SPC_WOLFMAN)) do_unwerewolf(victim,"");
	if (IS_CLASS(victim, CLASS_MAGE) && IS_AFFECTED(victim, AFF_POLYMORPH)) 
	    do_unpolymorph(victim,"");
	behead( victim );
	do_beastlike(ch,"");
	ch->pkill = ch->pkill + 1;
	victim->pdeath = victim->pdeath + 1;
	if (ch->pcdata->kingdom != 0)
	king_table[ch->pcdata->kingdom].pk += 1;
	if (victim->pcdata->kingdom != 0)
	king_table[victim->pcdata->kingdom].pd += 1;
	save_kingdoms();
	victim->level = 2;
	return;
    }
    
    if (ch->race == 0) ch->race++;
        ch->pcdata->souls++;
        if (ch->pcdata->souls >= souls_needed(ch))
            stc("You have achieved the required souls for a promotion!\n\r",ch);
    if (IS_CLASS(victim, CLASS_VAMPIRE))      do_mortalvamp(victim,"");
    else if (IS_SET(victim->special, SPC_WOLFMAN)) do_unwerewolf(victim,"");
    if (IS_CLASS(victim, CLASS_MAGE) && IS_AFFECTED(victim, AFF_POLYMORPH)) 
	do_unpolymorph(victim,"");
    behead( victim );
    do_beastlike(ch,"");
	if (ch->pcdata->kingdom != 0)
	king_table[ch->pcdata->kingdom].pk += 1;
	if (victim->pcdata->kingdom != 0)
	king_table[victim->pcdata->kingdom].pd += 1;
	save_kingdoms();

    ch->pkill = ch->pkill + 1;
    victim->pdeath = victim->pdeath + 1;
    victim->pcdata->stats[UNI_RAGE] = 0;
    victim->level = 2;
    sprintf(buf,"%s's soul has been stolen by %s.",victim->pcdata->switchname,ch->pcdata->switchname);
    do_info(ch,buf);
    sprintf( log_buf, "%s soulstolen by %s at %d.",
	victim->pcdata->switchname, ch->pcdata->switchname,
victim->in_room->vnum );
    log_string( log_buf );
    return;
}


/* For decapitating players - KaVir */
void do_decapitate( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *victim;
    OBJ_DATA  *obj;
    OBJ_DATA  *obj2;
    char arg[MAX_INPUT_LENGTH];
    char buf[MAX_STRING_LENGTH];

    one_argument( argument, arg );

    if ( IS_NPC(ch) ) return;

    obj2 = get_eq_char( ch, WEAR_HOLD );
    if ( ( obj = get_eq_char( ch, WEAR_WIELD ) ) == NULL )
    {
    	if ( ( obj2 = get_eq_char( ch, WEAR_HOLD ) ) == NULL )
    	{
	    send_to_char( "First you better get a weapon out!\n\r", ch );
	    return;
        }
    }

    if ( ( obj != NULL && obj->item_type != ITEM_WEAPON ) )
    {
    	if ( ( obj2 != NULL && obj2->item_type != ITEM_WEAPON ) )
    	{
	    send_to_char( "But you are not wielding any weapons!\n\r", ch );
	    return;
        }
    }

    if ( ( obj  != NULL && obj->value[3]  != 1 && obj->value[3]  != 3 )
    &&   ( obj2 != NULL && obj2->value[3] != 1 && obj2->value[3] != 3 ) )
    {
	if (IS_CLASS(ch, CLASS_DROW)) {
	if (( obj != NULL && obj->value[3] != 4) && (obj2 != NULL &&
	obj2->value[3] != 4)) {
	send_to_char("You need to wield a slashing, slicing, or whipping weapon to decapitate.\n\r", ch );
	return;}}
	else {
	send_to_char( "You need to wield a slashing or slicing weapon to decapitate.\n\r", ch );
	return;}
    }

    if ( arg[0] == '\0' )
    {
	send_to_char( "Decapitate whom?\n\r", ch );
	return;
    }

    if ( ( victim = get_char_room( ch, arg ) ) == NULL )
    {
	send_to_char( "They aren't here.\n\r", ch );
	return;
    }

    if ( ch == victim )
    {
	send_to_char( "That might be a bit tricky...\n\r", ch );
	return;
    }

    if ( IS_NPC(victim) )
    {
	send_to_char( "You can only decapitate other players.\n\r", ch );
	return;
    }


    if ( !CAN_PK(ch) )
    {
	send_to_char( "You must be an avatar to decapitate someone.\n\r", ch );
	return;
    }

    if ( !CAN_PK(victim) )
    {
	send_to_char( "You can only decapitate other avatars.\n\r", ch );
	return;
    }

    if ( victim->position > 1 )
    {
	send_to_char( "You can only do this to mortally wounded players.\n\r", ch );
	return;
    }

    if (is_safe(ch,victim)) return;

    if ( IS_CLASS(ch, CLASS_VAMPIRE) && IS_SET(ch->special, SPC_INCONNU) && 
	(IS_CLASS(victim, CLASS_VAMPIRE) || IS_VAMPAFF(victim, VAM_MORTAL)))
    {
	send_to_char( "You cannot decapitate another vampire.\n\r", ch );
	return;
    }

    if ( IS_CLASS(victim, CLASS_VAMPIRE) && IS_SET(victim->special, SPC_INCONNU) && 
	(IS_CLASS(ch, CLASS_VAMPIRE) || IS_VAMPAFF(ch, VAM_MORTAL)))
    {
	send_to_char( "You cannot decapitate an Inconnu vampire.\n\r", ch );
	return;
    }

    act( "You bring your weapon down upon $N's neck!",  ch, NULL, victim, TO_CHAR    );
    send_to_char( "Your head is sliced from your shoulders!\n\r", victim);
    act( "$n swings $s weapon down towards $N's neck!",  ch, NULL, victim, TO_NOTVICT );
    act( "$n's head is sliced from $s shoulders!", victim, NULL, NULL, TO_ROOM);

 
	if (IS_CLASS(ch, CLASS_DEMON) || IS_SET(ch->special, SPC_CHAMPION)) 

    if ((IS_CLASS(ch, CLASS_DEMON) || IS_SET(ch->special, SPC_CHAMPION)) && 
	!IS_CLASS(victim,CLASS_DEMON) &&
	!IS_SET(victim->special,SPC_CHAMPION) && !IS_CLASS(ch, CLASS_DROW))
    {
	if (ch->race == 0 && victim->race == 0)
	{
	    ch->pcdata->stats[DEMON_CURRENT] += 1000;
	    ch->pcdata->stats[DEMON_TOTAL] += 1000;
	}
	else
	{
	    ch->pcdata->stats[DEMON_CURRENT] += victim->race * 1000;
	    ch->pcdata->stats[DEMON_TOTAL] += victim->race * 1000;
	}
    }

   if (IS_CLASS(ch, CLASS_DROW) && !IS_CLASS(victim, CLASS_DEMON) && !IS_CLASS(victim, CLASS_DROW)) {
	if (victim->race == 0 && ch->race == 0 )
		ch->pcdata->stats[DROW_POWER] += 1000;
	else ch->pcdata->stats[DROW_POWER] += 1000* victim->race;
}
    if (( ch->race - victim->race > 10 && ch->race > 0)
	|| (victim->race == 0 && ch->race > 0)  )
    {
	ch->paradox[0] += ch->race;
	ch->paradox[1] += ch->race;
	ch->paradox[2] += PARADOX_TICK;
/*
	if (!str_cmp(ch->name,"amoeba"))
	{
	    ch->paradox[0] += (25 - ch->race);
	    ch->paradox[1] += (25 - ch->race);
 	}
*/


    	sprintf(buf,"%s has been decapitated by %s for no status.",victim->pcdata->switchname,ch->pcdata->switchname);
    	do_info(ch,buf);
	sprintf( log_buf, "%s decapitated by %s at %d for no status.",
	    victim->pcdata->switchname, ch->pcdata->switchname, victim->in_room->vnum );
	log_string( log_buf );
                   do_paradox(ch,"self");
	if (IS_CLASS(victim, CLASS_VAMPIRE))      do_mortalvamp(victim,"");
	else if (IS_SET(victim->special, SPC_WOLFMAN)) do_unwerewolf(victim,"");
	if (IS_CLASS(victim, CLASS_MAGE) && IS_AFFECTED(victim, AFF_POLYMORPH)) 
	    do_unpolymorph(victim,"");
	ch->fight_timer += 10;
	behead( victim );
	do_beastlike(ch,"");
	if (ch->pcdata->kingdom != 0)
	king_table[ch->pcdata->kingdom].pk += 1;
	if (victim->pcdata->kingdom != 0)
	king_table[victim->pcdata->kingdom].pd += 1;
	save_kingdoms();

	ch->pkill = ch->pkill + 1;
	victim->pdeath = victim->pdeath + 1;
	victim->level = 2;
	return;
    }
    ch->exp = ch->exp +1000;
    if (ch->race - ((ch->race/100)*100) == 0)
    	ch->race = ch->race + 1;
    else if (ch->race - ((ch->race/100)*100) < 45)
    	ch->race = ch->race + 1;
    if (ch->race - ((ch->race/100)*100) == 0)
    	victim->race = victim->race;
    else if (victim->race - ((victim->race/100)*100) > 0)
    	victim->race = victim->race - 1;
    act( "A misty white vapour pours from $N's corpse into your body.",  ch, NULL, victim, TO_CHAR    );
    act( "A misty white vapour pours from $N's corpse into $n's body.",  ch, NULL, victim, TO_NOTVICT );
    act( "You double over in agony as raw energy pulses through your veins.",  ch, NULL, NULL, TO_CHAR    );
    act( "$n doubles over in agony as sparks of energy crackle around $m.",  ch, NULL, NULL, TO_NOTVICT );
    if (IS_CLASS(victim, CLASS_VAMPIRE))      do_mortalvamp(victim,"");
    else if (IS_SET(victim->special, SPC_WOLFMAN)) do_unwerewolf(victim,"");
    if (IS_CLASS(victim, CLASS_MAGE) && IS_AFFECTED(victim, AFF_POLYMORPH)) 
	do_unpolymorph(victim,"");
    ch->fight_timer += 10;
    if (IS_CLASS(ch, CLASS_DEMON) && !IS_NPC(ch) && !IS_NPC(victim) && victim->race + 15 >= ch->race)
    {
      send_to_char("You rip away a part of their soul.\n\r", ch);
      send_to_char("You feel a part of your soul has been torn away.\n\r", victim);
      ch->pcdata->souls += 1;
      if (ch->pcdata->souls > souls_needed(ch))
          ch->pcdata->souls = souls_needed(ch);
    }
    behead( victim );
    do_beastlike(ch,"");
	if (ch->pcdata->kingdom != 0)
	king_table[ch->pcdata->kingdom].pk += 1;
	if (victim->pcdata->kingdom != 0)
	king_table[victim->pcdata->kingdom].pd += 1;
	save_kingdoms();

    ch->pkill = ch->pkill + 1;
    victim->pdeath = victim->pdeath + 1;
    victim->rage = 0;
    victim->level = 2;
    sprintf(buf,"%s got his head chopped off by %s.",victim->pcdata->switchname,ch->pcdata->switchname);
    ch->exp += 10000;
 
    do_info(ch,buf);
    sprintf( log_buf, "%s decapitated by %s at %d.",
	victim->pcdata->switchname, ch->pcdata->switchname, victim->in_room->vnum );
    log_string( log_buf );
    return;
}

void do_tear( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *victim;
    char arg[MAX_INPUT_LENGTH];
    char buf[MAX_STRING_LENGTH];

    one_argument( argument, arg );

    if ( IS_NPC(ch) ) return;

    if ( !IS_CLASS(ch, CLASS_WEREWOLF))
    {
	send_to_char( "Huh?\n\r", ch );
	return;
    }

    if ( !IS_SET(ch->special, SPC_WOLFMAN))
    {
	send_to_char( "You can only tear heads off while in Crinos form.\n\r", ch );
	return;
    }

    if ( !IS_VAMPAFF(ch, VAM_CLAWS))
    {
	send_to_char( "You better get your claws out first.\n\r", ch );
	return;
    }

    if ( arg[0] == '\0' )
    {
	send_to_char( "Who's head do you wish to tear off?\n\r", ch );
	return;
    }

    if ( ( victim = get_char_room( ch, arg ) ) == NULL )
    {
	send_to_char( "They aren't here.\n\r", ch );
	return;
    }


    if ( ch == victim )
    {
	send_to_char( "That might be a bit tricky...\n\r", ch );
	return;
    }

    if ( IS_NPC(victim) )
    {
	send_to_char( "You can only tear the heads off other players.\n\r", ch );
	return;
    }
    if (!str_cmp(victim->pcdata->switchname,"SirMog")) return;

    if ( !CAN_PK(ch) )
    {
	send_to_char( "You must be an avatar to tear someone's head off.\n\r", ch );
	return;
    }

    if ( !CAN_PK(victim) )
    {
	send_to_char( "You can only tear the head off another avatar.\n\r", ch );
	return;
    }

    if ( victim->position > 1 )
    {
	send_to_char( "You can only do this to mortally wounded players.\n\r", ch );
	return;
    }

    if (is_safe(ch,victim)) return;

    act( "You tear $N's head from $S shoulders!",  ch, NULL, victim, TO_CHAR    );
    send_to_char( "Your head is torn from your shoulders!\n\r", victim);
    act( "$n tears $N's head from $S shoulders!",  ch, NULL, victim, TO_NOTVICT );
    if ((IS_CLASS(ch, CLASS_DEMON) || IS_SET(ch->special, SPC_CHAMPION))
	&& !IS_CLASS(victim, CLASS_DEMON) && !IS_CLASS(victim, CLASS_DROW)
	&& !IS_SET(victim->special, SPC_CHAMPION)) 
    {
	if (ch->race == 0 && victim->race == 0)
	{
	    ch->pcdata->stats[DEMON_CURRENT] += 1000;
	    ch->pcdata->stats[DEMON_TOTAL] += 1000;
	}
	else
	{
	    ch->pcdata->stats[DEMON_CURRENT] += victim->race * 1000;
	    ch->pcdata->stats[DEMON_TOTAL] += victim->race * 1000;
	}
    }
	if (IS_CLASS(ch, CLASS_DROW) && !IS_CLASS(victim, CLASS_DROW) &&
	!IS_CLASS(victim, CLASS_DEMON) && !IS_SET(victim->special, SPC_CHAMPION))
	{
		if (ch->race == 0 && victim->race == 0)
		ch->pcdata->stats[DROW_POWER] += 1000;
		else
		ch->pcdata->stats[DROW_POWER] += 1000 * victim->race;
	}
 
    if (( ch->race - victim->race > 10 && ch->race > 0)
	|| (victim->race == 0 && ch->race > 0)  )
    {
	ch->paradox[0] += ch->race;
	ch->paradox[1] += ch->race;
	ch->paradox[2] += PARADOX_TICK;

    	sprintf(buf,"%s has been decapitated by %s.",victim->pcdata->switchname,ch->pcdata->switchname);
    	do_info(ch,buf);
	sprintf( log_buf, "%s decapitated by %s at %d for no status.",
	    victim->pcdata->switchname, ch->pcdata->switchname, victim->in_room->vnum );
	log_string( log_buf );
	if (IS_CLASS(victim, CLASS_VAMPIRE))      do_mortalvamp(victim,"");
	else if (IS_SET(victim->special, SPC_WOLFMAN)) do_unwerewolf(victim,"");
	if (IS_CLASS(victim, CLASS_MAGE) && IS_AFFECTED(victim, AFF_POLYMORPH)) 
	    do_unpolymorph(victim,"");
	ch->fight_timer += 10;
	behead( victim );
	do_beastlike(ch,"");
	if (ch->pcdata->kingdom != 0)
	king_table[ch->pcdata->kingdom].pk += 1;
	if (victim->pcdata->kingdom != 0)
	king_table[victim->pcdata->kingdom].pd += 1;
	save_kingdoms();

	ch->pkill = ch->pkill + 1;
	victim->pdeath = victim->pdeath + 1;
	return;
    }
    ch->exp = ch->exp +1000;
    if (ch->race - ((ch->race/100)*100) == 0)
    	ch->race = ch->race + 1;
    else if (ch->race - ((ch->race/100)*100) < 45)
    	ch->race = ch->race + 1;
    if (ch->race - ((ch->race/100)*100) == 0)
    	victim->race = victim->race;
    else if (victim->race - ((victim->race/100)*100) > 0)
    	victim->race = victim->race - 1;
    act( "A misty white vapour pours from $N's corpse into your body.",  ch, NULL, victim, TO_CHAR    );
    act( "A misty white vapour pours from $N's corpse into $n's body.",  ch, NULL, victim, TO_NOTVICT );
    act( "You double over in agony as raw energy pulses through your veins.",  ch, NULL, NULL, TO_CHAR    );
    act( "$n doubles over in agony as sparks of energy crackle around $m.",  ch, NULL, NULL, TO_NOTVICT );
    if (IS_CLASS(victim, CLASS_VAMPIRE))      do_mortalvamp(victim,"");
    else if (IS_SET(victim->special, SPC_WOLFMAN)) do_unwerewolf(victim,"");
    if (IS_CLASS(victim, CLASS_MAGE) && IS_AFFECTED(victim, AFF_POLYMORPH)) 
	do_unpolymorph(victim,"");
    ch->fight_timer += 10;
    behead( victim );
    do_beastlike(ch,"");
	if (ch->pcdata->kingdom != 0)
	king_table[ch->pcdata->kingdom].pk += 1;
	if (victim->pcdata->kingdom != 0)
	king_table[victim->pcdata->kingdom].pd += 1;
	save_kingdoms();

    ch->pkill = ch->pkill + 1;
    victim->pdeath = victim->pdeath + 1;
    victim->rage = 0;
    victim->level = 2;
    sprintf(buf,"%s has been decapitated by %s.",victim->pcdata->switchname,ch->pcdata->switchname);
    do_info(ch,buf);
    sprintf( log_buf, "%s decapitated by %s at %d.",
	victim->pcdata->switchname, ch->pcdata->switchname, victim->in_room->vnum );
    log_string( log_buf );
    return;
}

void do_crack( CHAR_DATA *ch, char *argument )
{
    OBJ_DATA  *obj;
    OBJ_DATA  *right;
    OBJ_DATA  *left;

    right = get_eq_char( ch, WEAR_WIELD );
    left = get_eq_char( ch, WEAR_HOLD );
    if (right != NULL && right->pIndexData->vnum == 12) obj = right;
    else if (left != NULL && left->pIndexData->vnum == 12) obj = left;
    else
    {
	send_to_char("You are not holding any heads.\n\r",ch);
	return;
    }
    act("You hurl $p at the floor.", ch, obj, NULL, TO_CHAR);
    act("$n hurls $p at the floor.", ch, obj, NULL, TO_ROOM);
    act("$p cracks open, leaking brains out across the floor.", ch, obj, NULL, TO_CHAR);
    if (obj->chobj != NULL)
    	{act("$p cracks open, leaking brains out across the floor.", ch, obj, obj->chobj, TO_NOTVICT);
    	act("$p crack open, leaking brains out across the floor.", ch, obj, obj->chobj, TO_VICT);}
    else
    	{act("$p cracks open, leaking brains out across the floor.", ch, obj, NULL, TO_ROOM);}
    crack_head(ch,obj,obj->name);
    obj_from_char(obj);
    extract_obj(obj);
}

void crack_head( CHAR_DATA *ch, OBJ_DATA *obj, char *argument )
{
    CHAR_DATA *victim;
    MOB_INDEX_DATA *pMobIndex; 

    char      arg1 [MAX_INPUT_LENGTH];
    char      arg2 [MAX_INPUT_LENGTH];
    char       buf [MAX_INPUT_LENGTH];

    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );
    if (str_cmp(arg2,"mob") && obj->chobj != NULL && !IS_NPC(obj->chobj) &&
	IS_AFFECTED(obj->chobj,AFF_POLYMORPH))
    {
	victim = obj->chobj;
	make_part(victim,"cracked_head");
	make_part(victim,"brain");
	sprintf(buf,"the quivering brain of %s",victim->name);
	free_string(victim->morph);
	victim->morph = str_dup(buf);
	return;
    }
    else if (!str_cmp(arg2,"mob"))
    {
	if ( ( pMobIndex = get_mob_index( obj->value[1] ) ) == NULL ) return;
	victim = create_mobile( pMobIndex );
	char_to_room(victim,ch->in_room);
	make_part(victim,"cracked_head");
	make_part(victim,"brain");
	extract_char(victim,TRUE);
	return;
    }
    else
    {
	if ( ( pMobIndex = get_mob_index( 30002 ) ) == NULL ) return;
	victim = create_mobile( pMobIndex );

	sprintf( buf, capitalize(arg2) );
	free_string( victim->short_descr );
	victim->short_descr = str_dup( buf );

	char_to_room(victim,ch->in_room);
	make_part(victim,"cracked_head");
	make_part(victim,"brain");
	extract_char(victim,TRUE);
	return;
    }
    return;
}

/* Voodoo skill by KaVir */

void do_voodoo( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *victim;
    OBJ_DATA  *obj;
    char      arg1 [MAX_INPUT_LENGTH];
    char      arg2 [MAX_INPUT_LENGTH];
    char       buf [MAX_INPUT_LENGTH];
    char     part1 [MAX_INPUT_LENGTH];
    char     part2 [MAX_INPUT_LENGTH];

    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );

    if ( arg1[0] == '\0')
    {
	send_to_char( "Who do you wish to use voodoo magic on?\n\r", ch );
	return;
    }

    if ( ( obj = get_eq_char( ch, WEAR_HOLD ) ) == NULL )
    {
	send_to_char( "You are not holding a voodoo doll.\n\r", ch );
	return;
    }

    if ( ( victim = get_char_world( ch, arg1 ) ) == NULL )
    {
	send_to_char( "They are not here.\n\r", ch );
	return;
    }

    if ( IS_NPC(victim) )
    {
	send_to_char( "Not on NPC's.\n\r", ch );
	return;
    }

    sprintf(part2,obj->name);
    sprintf(part1,"%s voodoo doll",victim->name);

    if ( str_cmp(part1,part2) )
    {
	sprintf(buf,"But you are holding %s, not %s!\n\r",obj->short_descr,victim->name);
	send_to_char( buf, ch );
	return;
    }

    if ( arg2[0] == '\0')
    {
	send_to_char( "You can 'stab', 'burn' or 'throw' the doll.\n\r", ch );
    }
    else if ( !str_cmp(arg2, "stab") )
    {
	WAIT_STATE(ch,12);
	act("You stab a pin through $p.", ch, obj, NULL, TO_CHAR); 
 	act("$n stabs a pin through $p.", ch, obj, NULL, TO_ROOM);
	if (!IS_NPC(victim) && IS_IMMUNE(victim, IMM_VOODOO)) return;
	act("You feel an agonising pain in your chest!", victim, NULL, NULL, TO_CHAR);
 	act("$n clutches $s chest in agony!", victim, NULL, NULL, TO_ROOM);
    }
    else if ( !str_cmp(arg2, "burn") )
    {
	WAIT_STATE(ch,12);
	act("You set fire to $p.", ch, obj, NULL, TO_CHAR);
 	act("$n sets fire to $p.", ch, obj, NULL, TO_ROOM);
	act("$p burns to ashes.", ch, obj, NULL, TO_CHAR);
 	act("$p burns to ashes.", ch, obj, NULL, TO_ROOM);
	obj_from_char(obj);
	extract_obj(obj);
	if (!IS_NPC(victim) && IS_IMMUNE(victim, IMM_VOODOO)) return;
	if (IS_AFFECTED(victim,AFF_FLAMING) )
	    return;
	SET_BIT(victim->affected_by, AFF_FLAMING);
	act("You suddenly burst into flames!", victim, NULL, NULL, TO_CHAR);
 	act("$n suddenly bursts into flames!", victim, NULL, NULL, TO_ROOM);
    }
    else if ( !str_cmp(arg2, "throw") )
    {
	WAIT_STATE(ch,12);
	act("You throw $p to the ground.", ch, obj, NULL, TO_CHAR);
 	act("$n throws $p to the ground.", ch, obj, NULL, TO_ROOM);
	obj_from_char(obj);
	obj_to_room(obj, ch->in_room);
	if (!IS_NPC(victim) && IS_IMMUNE(victim, IMM_VOODOO)) return;
	if (victim->position < POS_STANDING)
	    return;
	if (victim->position == POS_FIGHTING)
	    stop_fighting(victim, TRUE);
	act("A strange force picks you up and hurls you to the ground!", victim, NULL, NULL, TO_CHAR);
 	act("$n is hurled to the ground by a strange force.", victim, NULL, NULL, TO_ROOM);
	victim->position = POS_RESTING;
	victim->hit = victim->hit - number_range(ch->level,(5*ch->level));
	update_pos(victim);
	if (victim->position == POS_DEAD && !IS_NPC(victim))
	{
	    do_killperson(ch,victim->name);
	    return;
	}
    }
    else
    {
	send_to_char( "You can 'stab', 'burn' or 'throw' the doll.\n\r", ch );
    }
    return;
}

void check_arena( CHAR_DATA *ch, CHAR_DATA *victim )
{
    DESCRIPTOR_DATA *d; /* needed for Arena bet checking */
    char buf[MAX_STRING_LENGTH];
    float odds;
    float lvl1, lvl2;
    int payoff;

    sprintf(buf,"[Arena] %s has defeated %s!\n\r", 
                 ch->name,victim->name);
    for ( d = descriptor_list; d; d = d->next )
    {
       if( d->connected == CON_PLAYING &&
         ( d->character != victim && d->character != ch ))
       {
	  ADD_COLOUR(ch,buf,L_RED);
          send_to_char( buf, d->character );
       }
    }

    ch->pcdata->awins += 1;
    victim->pcdata->alosses += 1;
    lvl1 = ch->hit;
    lvl2 = victim->hit;
    odds = ( lvl2 / lvl1 );
    for( d = descriptor_list; d; d = d->next )
    {
       if (d->connected == CON_PLAYING )
       {
          if (d->character->gladiator == ch)
          {
             payoff = d->character->pcdata->plr_wager * ( odds + 1 );
             sprintf(buf, "You won! Your wager: %d, payoff: %d\n\r",
                           d->character->pcdata->plr_wager, payoff );
             send_to_char(buf,d->character);
             d->character->exp += payoff;
             /* reset the betting info */
             d->character->gladiator = NULL;
             d->character->pcdata->plr_wager = 0;
             payoff = 0;
          }
          if (d->character->gladiator != ch
           && d->character->pcdata->plr_wager >= 1)
          {
             int tmp = 0;
             sprintf(buf, "You lost! Your wager: %d\n\r",
                           d->character->pcdata->plr_wager);
             send_to_char(buf,d->character);
             if (d->character->pcdata->plr_wager >d->character->exp )
             {
                tmp = d->character->pcdata->plr_wager; 
                d->character->pcdata->plr_wager -= tmp;
             }
             if ( tmp > 0 )
                d->character->pcdata->quest -= tmp;
             d->character->exp -= d->character->pcdata->plr_wager;
             /* reset the betting info */
             d->character->gladiator = NULL;
             d->character->pcdata->plr_wager = 0;
          }
       }
    }

    /* now move both fighters out of arena and back
       to the regular "world" be sure to define 
       ROOM_VNUM_AWINNER and ROOM_VNUM_ALOSER */
    stop_fighting(victim,TRUE);
    char_from_room(victim);
    char_to_room(victim,get_room_index(ROOM_VNUM_ALOSER));
    victim->hit = victim->max_hit;
    victim->mana = victim->max_mana;
    update_pos( victim );
    do_look(victim, "auto");
    stop_fighting(ch,TRUE);
    char_from_room(ch);
    char_to_room(ch,get_room_index(ROOM_VNUM_AWINNER));
    ch->hit = ch->max_hit;
    ch->mana = ch->max_mana;
    update_pos( ch );
    do_look(ch, "auto");

    if (IS_SET(ch->act,PLR_CHALLENGER))
        REMOVE_BIT(ch->act,PLR_CHALLENGER);
    if (IS_SET(victim->act,PLR_CHALLENGER))
        REMOVE_BIT(victim->act,PLR_CHALLENGER);
    if (IS_SET(victim->act,PLR_CHALLENGED))
        REMOVE_BIT(victim->act,PLR_CHALLENGED);
    if (IS_SET(ch->act,PLR_CHALLENGED))
        REMOVE_BIT(ch->act,PLR_CHALLENGED);

    ch->challenger = NULL;
    ch->challenged = NULL;
    victim->challenger = NULL;
    victim->challenged = NULL;
    REMOVE_BIT(ch->act, PLR_SILENCE);
    REMOVE_BIT(ch->act,PLR_NO_TELL);
    REMOVE_BIT(victim->act,PLR_SILENCE);
    REMOVE_BIT(victim->act,PLR_NO_TELL);

    arena = FIGHT_OPEN; /* clear the arena */

return;
}


void do_garotte(CHAR_DATA *ch, char *argument)
{
	char buf[MAX_STRING_LENGTH];
	char arg[MAX_INPUT_LENGTH];
	CHAR_DATA *victim;
	OBJ_DATA *obj;
	int chance;

	one_argument(argument, arg);
	chance = number_percent();

	if (IS_NPC(ch)) return;

	if (!IS_CLASS(ch, CLASS_DROW) || !IS_SET(ch->pcdata->powers[1], DPOWER_GAROTTE)) {
	send_to_char("Huh?\n\r", ch );
	return;}

	if ((victim = get_char_room(ch, arg)) == NULL) {
	send_to_char("Garotte whom?\n\r", ch );
	return;}

	if (victim == ch) {
	send_to_char("That would be a bad idea.\n\r", ch );
	return;}

	if (!IS_NPC(victim) && !str_cmp(victim->pcdata->switchname,"SirMog"))
	return;

	if (is_safe(ch, victim)) return;

    if ( ( ( obj = get_eq_char( ch, WEAR_WIELD ) ) == NULL
    ||   obj->value[3] != 4 )
    &&   ( ( obj = get_eq_char( ch, WEAR_HOLD ) ) == NULL
    ||   obj->value[3] != 4 ) )
    {
        send_to_char( "You need to wield a whip.\n\r", ch );
        return;
    }
     
    if ( victim->fighting != NULL )
    {
        send_to_char( "You can't garotte a fighting person.\n\r", ch );
        return;
    }

	if (victim->hit < victim->max_hit) {
	act("$N is too hurt for you to sneak up.", ch, NULL,victim,TO_CHAR);
	return;}

	check_killer(ch, victim);
	WAIT_STATE(ch, skill_table[gsn_garotte].beats);
	
	if (chance <= 5) {
        ch->fight_timer += 5;
	damage(ch, victim, 0, gsn_garotte);
	return;}

	if ( chance == 13 || chance == 31 ) { /* bad luck :) */
	act("$n wraps a whip around $N's neck! $N is choking to death!",ch,NULL,victim,TO_NOTVICT);
	act("You wrap your whip around $N's neck and refuse to let go!",ch,NULL,victim,TO_CHAR);
	act("$n wraps his whip around your neck, and you can't break free!",ch,NULL,victim,TO_VICT);
	
	if (!IS_NPC(victim)) {
        ch->fight_timer += 5;
	behead(victim);
        ch->pkill = ch->pkill + 1;
        victim->pdeath = victim->pdeath + 1;
        if (ch->pcdata->kingdom != 0)
        king_table[ch->pcdata->kingdom].pk += 1;
        if (victim->pcdata->kingdom != 0)
        king_table[victim->pcdata->kingdom].pd += 1;
        save_kingdoms();
        victim->level = 2;
	do_call(victim,"all");
	sprintf(buf, "%s has been assassinated by %s!",victim->name,ch->name);
	do_info(ch,buf);
	} else
	{
	victim->hit = 1;
	damage(ch, victim, 1000, gsn_garotte);
	}

	return;} else
	{
	multi_hit(ch, victim, gsn_garotte);
	return;
	}
	return;
}

void do_dark_garotte(CHAR_DATA *ch, char *argument)
{
	char buf[MAX_STRING_LENGTH];
	char arg[MAX_INPUT_LENGTH];
	CHAR_DATA *victim;
	OBJ_DATA *obj;
	int chance;
	int i;

	one_argument(argument, arg);
	chance = number_percent();

	if (IS_NPC(ch)) return;

	if (!IS_CLASS(ch, CLASS_DROW) || !IS_SET(ch->pcdata->powers[1], DPOWER_GAROTTE)
	|| !IS_SET(ch->pcdata->powers[1], DPOWER_DGAROTTE)) {
	send_to_char("Huh?\n\r", ch );
	return;}

	if (!IS_SET(ch->newbits, NEW_DARKNESS)) {
	send_to_char("You have to be surrounded in darkness.\n\r", ch );
	return;}

	if ((victim = get_char_room(ch, arg)) == NULL) {
	send_to_char("Dark Garotte whom?\n\r", ch );
	return;}

	if (victim == ch) {
	send_to_char("That would be a bad idea.\n\r", ch );
	return;}

	if (!IS_NPC(victim) && !str_cmp(victim->pcdata->switchname,"SirMog"))
	return;

	if (is_safe(ch, victim)) return;

    if ( ( ( obj = get_eq_char( ch, WEAR_WIELD ) ) == NULL
    ||   obj->value[3] != 4 )
    &&   ( ( obj = get_eq_char( ch, WEAR_HOLD ) ) == NULL
    ||   obj->value[3] != 4 ) )
    {
        send_to_char( "You need to wield a whip.\n\r", ch );
        return;
    }
     
    if ( victim->fighting != NULL )
    {
        send_to_char( "You can't garotte a fighting person.\n\r", ch );
        return;
    }

	if (victim->hit < victim->max_hit) {
	act("$N is too hurt for you to sneak up.", ch, NULL,victim,TO_CHAR);
	return;}

	check_killer(ch, victim);
	WAIT_STATE(ch, skill_table[gsn_garotte].beats);
	REMOVE_BIT(ch->newbits, NEW_DARKNESS);	
	REMOVE_BIT(ch->in_room->room_flags, ROOM_TOTAL_DARKNESS);

	if ( chance > 95 ) { /* bad luck :) */
	act("$n wraps a whip around $N's neck! $N is choking to death!",ch,NULL,victim,TO_NOTVICT);
	act("You wrap your whip around $N's neck and refuse to let go!",ch,NULL,victim,TO_CHAR);
	act("$n wraps his whip around your neck, and you can't break free!",ch,NULL,victim,TO_VICT);
	
	if (!IS_NPC(victim)) {
        ch->fight_timer += 5;
	behead(victim);
        ch->pkill = ch->pkill + 1;
        victim->pdeath = victim->pdeath + 1;
        if (ch->pcdata->kingdom != 0)
        king_table[ch->pcdata->kingdom].pk += 1;
        if (victim->pcdata->kingdom != 0)
        king_table[victim->pcdata->kingdom].pd += 1;
        save_kingdoms();
        victim->level = 2;
	do_call(victim,"all");
	sprintf(buf, "%s has been assassinated by %s!",victim->name,ch->name);
	do_info(ch,buf);
	} else
	{
	victim->hit = 1;
	damage(ch, victim, 1000, gsn_garotte);
	}

	return;} else
	if (chance <= 5) {
        ch->fight_timer += 5;
	damage(ch, victim, 0, gsn_garotte);
	return;}
	else 
	{
	for (i = 1; i < 4; i++)
	multi_hit(ch, victim, gsn_garotte);
	return;
	}
	return;
}


bool has_timer(CHAR_DATA *ch) 
{
	if (ch->fight_timer > 0 && !IS_NPC(ch)) {
	send_to_char("Not until your fight timer runs out!\n\r", ch );
	return TRUE;}

	return FALSE;
}



void do_circle( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;
    OBJ_DATA *obj;

    one_argument( argument, arg );

	if (IS_NPC(ch)) return;

	if (!IS_CLASS(ch, CLASS_NINJA) || ch->pcdata->powers[NPOWER_NINGENNO] < 6) {
	send_to_char("Huh?\n\r", ch );
	return;}

    if ( ( victim = get_char_room( ch, arg ) ) == NULL )
    {
	if (ch->fighting == NULL) {
	send_to_char( "They aren't here.\n\r", ch );
	return;}
	else victim = ch->fighting;
    }

    if ( victim == ch )
    {
	send_to_char( "How can you circle yourself?\n\r", ch );
	return;
    }

    if ( is_safe( ch, victim ) )
      return;

    if ( ( ( obj = get_eq_char( ch, WEAR_WIELD ) ) == NULL
    ||   obj->value[3] != 11 )
    &&   ( ( obj = get_eq_char( ch, WEAR_HOLD ) ) == NULL
    ||   obj->value[3] != 11 ) )
    {
	send_to_char( "You need to wield a piercing weapon.\n\r", ch );
	return;
    }

    check_killer( ch, victim );
	if (number_percent() > 10)
	multi_hit( ch, victim, gsn_circle );
    else
	damage( ch, victim, 0, gsn_circle );

    WAIT_STATE( ch, 8 );
    return;
}


void do_autostance(CHAR_DATA *ch, char *argument ) {

        char    arg     [MAX_INPUT_LENGTH];

        argument = one_argument(argument,arg);

        if (IS_NPC(ch)) return;

	if (!str_cmp(arg,"none")) {
	send_to_char("You no longer autostance.\n\r", ch );
	ch->stance[MONK_AUTODROP] = STANCE_NONE;}
        else if (!str_cmp(arg, "crane")) {
        send_to_char("You now autostance into the crane stance.\n\r",ch );
        ch->stance[MONK_AUTODROP] = STANCE_CRANE;}
        else if (!str_cmp(arg, "crab")) {
        send_to_char("You now autostance into the crab stance.\n\r", ch );
        ch->stance[MONK_AUTODROP] = STANCE_CRAB;}
        else if (!str_cmp(arg, "bull")) {
        send_to_char("You now autostance into the bull stance.\n\r", ch );
        ch->stance[MONK_AUTODROP] = STANCE_BULL;}
        else if (!str_cmp(arg, "viper")) {
        send_to_char("You now autostance into the viper stance.\n\r", ch );
        ch->stance[MONK_AUTODROP] = STANCE_VIPER;}
        else if (!str_cmp(arg, "mongoose")) {
        send_to_char("You now autostance into the mongoose stance.\n\r", ch);
        ch->stance[MONK_AUTODROP] = STANCE_MONGOOSE;}
        else if (!str_cmp(arg, "mantis")
        && ch->stance[STANCE_CRANE] >= 200
        && ch->stance[STANCE_VIPER] >= 200) {
        send_to_char("You now autostance into the mantis stance.\n\r", ch);
        ch->stance[MONK_AUTODROP] = STANCE_MANTIS;}
        else if (!str_cmp(arg, "monkey")
        && ch->stance[STANCE_CRANE] >= 200
        && ch->stance[STANCE_MONGOOSE] >= 200) {
        send_to_char("You now autostance into the monkey stance.\n\r", ch);
        ch->stance[MONK_AUTODROP] = STANCE_MONKEY;}
        else if (!str_cmp(arg, "swallow")  
        && ch->stance[STANCE_CRAB] >= 200
        && ch->stance[STANCE_MONGOOSE] >= 200) {
        send_to_char("You now autostance into the swallow stance.\n\r", ch);
        ch->stance[MONK_AUTODROP] = STANCE_SWALLOW;}
        else if (!str_cmp(arg, "tiger")  
        && ch->stance[STANCE_BULL] >= 200
        && ch->stance[STANCE_VIPER] >= 200) {
        send_to_char("You now autostance into the tiger stance.\n\r", ch);
        ch->stance[MONK_AUTODROP] = STANCE_TIGER;}
        else if (!str_cmp(arg, "dragon")  
        && ch->stance[STANCE_CRAB] >= 200
        && ch->stance[STANCE_BULL] >= 200) {
        send_to_char("You now autostance into the dragon stance.\n\r", ch);
        ch->stance[MONK_AUTODROP] = STANCE_DRAGON;}
        else send_to_char("You can't set your autostance to that!\n\r",ch);
        /*else if (!str_cmp(arg, "xkilla")
        && ch->stance[STANCE_TIGER] >= 200
        && ch->stance[STANCE_SWALLOW] >= 200) {
        send_to_char("You now autostance into the xkilla stance.\n\r", ch);
        ch->stance[MONK_AUTODROP] = STANCE_XKILLA;}
        else send_to_char("You can't set your autostance to that!\n\r",ch);*/
}

void autodrop(CHAR_DATA *ch) {
        
        int st;
        char buf [MAX_INPUT_LENGTH];
        char buf2 [MAX_INPUT_LENGTH];
        char stancename [10];

        if (IS_NPC(ch)) return;

        st = ch->stance[11];
        if (st == STANCE_VIPER) sprintf(stancename,"viper");
        else if (st == STANCE_CRANE) sprintf(stancename,"crane");
        else if (st == STANCE_CRAB) sprintf(stancename,"crab");
        else if (st == STANCE_MONGOOSE) sprintf(stancename,"mongoose");
        else if (st == STANCE_BULL) sprintf(stancename,"bull");
        else if (st == STANCE_MANTIS) sprintf(stancename,"mantis");
        else if (st == STANCE_DRAGON) sprintf(stancename,"dragon");
        else if (st == STANCE_TIGER) sprintf(stancename,"tiger");
        else if (st == STANCE_MONKEY) sprintf(stancename,"monkey");
        else if (st == STANCE_SWALLOW) sprintf(stancename,"swallow");
	else if (st == STANCE_XKILLA) sprintf(stancename,"xkilla");
        else return;
     
        if (ch->stance[0] < 1) {
        ch->stance[0] = st;
    
        sprintf(buf, "#7You autodrop into the #3%s#7 stance.",
stancename);
        act(buf, ch, NULL, NULL, TO_CHAR);
        sprintf(buf2, "#7$n autodrops into the #3%s#7 stance.",
stancename);
        act(buf2, ch, NULL, NULL, TO_ROOM);
}       
}
