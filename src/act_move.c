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



void horn args( ( CHAR_DATA *ch ) );


char *	const	dir_name	[]		=
{
    "north", "east", "south", "west", "up", "down"
};

const	sh_int	rev_dir		[]		=
{
    2, 3, 0, 1, 5, 4
};

const	sh_int	movement_loss	[SECT_MAX]	=
{
    1, 2, 2, 3, 4, 6, 4, 1, 6, 10, 6
};



/*
 * Local functions.
 */
int	find_door	args( ( CHAR_DATA *ch, char *arg ) );
bool	has_key		args( ( CHAR_DATA *ch, int key ) );
int	count_imms	args( ( CHAR_DATA *ch ) );
bool	check_track	args( ( CHAR_DATA *ch, int direction ) );
void	add_tracks	args( ( CHAR_DATA *ch, int direction ) );
void	show_page	args( ( CHAR_DATA *ch, OBJ_DATA *book, int pnum, bool pagefalse ) );
void	show_runes	args( ( CHAR_DATA *ch, OBJ_DATA *page, bool endline ) );
bool	are_runes	args( ( OBJ_DATA *page ) );
void	drow_hate	args( ( CHAR_DATA *ch) );


void move_char( CHAR_DATA *ch, int door )
{
    CHAR_DATA *fch;
    CHAR_DATA *fch_next;
    CHAR_DATA *mount;
    ROOM_INDEX_DATA *in_room;
    ROOM_INDEX_DATA *to_room;
    EXIT_DATA *pexit;
    DESCRIPTOR_DATA *d;
    char buf  [MAX_STRING_LENGTH];
    char poly [MAX_STRING_LENGTH];
    char mount2 [MAX_INPUT_LENGTH];
    char leave [20];
    int revdoor;

    if ( door < 0 || door > 5 )

    {
	bug( "Do_move: bad door %d.", door );
	return;
    }

    in_room = ch->in_room;
    if ( ( pexit   = in_room->exit[door] ) == NULL
    ||   ( to_room = pexit->to_room      ) == NULL )
    {
	send_to_char( "Alas, you cannot go that way.\n\r", ch );
	return;
    }
/*
if ((door= DIR_NORTH && ((obj=get_obj_list(ch,"walln",in_room->contents)
!= NULL) || (obj=get_obj_list(ch,"walls",to_room->contents)) != NULL))

|| if (door= DIR_SOUTH && ((obj=get_obj_list(ch,"walls",in_room->contents)
!= NULL) || (obj=get_obj_list(ch,"walln",to_room->contents)) != NULL))
|| if (door= DIR_EAST && ((obj=get_obj_list(ch,"walle",in_room->contents)
!= NULL) || (obj=get_obj_list(ch,"wallw",to_room->contents)) != NULL))
|| if (door= DIR_WEST && ((obj=get_obj_list(ch,"wallw",in_room->contents)
!= NULL) || (obj=get_obj_list(ch,"walle",to_room->contents)) != NULL))
|| if (door= DIR_UP && ((obj=get_obj_list(ch,"wallu",in_room->contents)
!= NULL) || (obj=get_obj_list(ch,"walld",to_room->contents)) != NULL))
|| if (door= DIR_DOWN && ((obj=get_obj_list(ch,"walld",in_room->contents)
!= NULL) || (obj=get_obj_list(ch,"wallu",to_room->contents)) != NULL)))
{
send_to_char("You are unable to pass the flowing water.\n\r",ch);
return;
}
*/
    if ( IS_SET(pexit->exit_info, EX_CLOSED)
    &&   !IS_AFFECTED(ch, AFF_PASS_DOOR)
    &&   !IS_AFFECTED(ch, AFF_ETHEREAL)
    &&   !IS_AFFECTED(ch, AFF_SHADOWPLANE) )
    {
	if (!IS_NPC(ch) && IS_CLASS(ch, CLASS_WEREWOLF) &&
	    ch->power[DISC_WERE_BOAR] > 0)
	{
	    act( "You smash open the $d.", ch, NULL, pexit->keyword, TO_CHAR );
	    act( "$n smashes open the $d.", ch, NULL, pexit->keyword, TO_ROOM );
	    REMOVE_BIT(pexit->exit_info, EX_CLOSED);
	}
	else
	{
	    act( "The $d is closed.", ch, NULL, pexit->keyword, TO_CHAR );
	    return;
	}
    }

    if ( IS_AFFECTED(ch, AFF_CHARM)
    &&   ch->master != NULL
    &&   in_room == ch->master->in_room )
    {
	send_to_char( "What?  And leave your beloved master?\n\r", ch );
	return;
    }

    if (IS_NPC(ch) && (mount=ch->mount) != NULL && IS_SET(ch->mounted,IS_MOUNT))
    {
	send_to_char( "You better wait for instructions from your rider.\n\r", ch );
	return;
    }

    if ( room_is_private( to_room ))
    {
	if ( IS_NPC( ch ) || ch->trust < MAX_LEVEL)
	{
		send_to_char( "That room is private right now.\n\r", ch );
		return;
	}
	else 
		send_to_char( "That room is private (Access granted).\n\r", ch);
    }

    if ( (IS_LEG_L(ch,BROKEN_LEG) || IS_LEG_L(ch,LOST_LEG)) &&
	 (IS_LEG_R(ch,BROKEN_LEG) || IS_LEG_R(ch,LOST_LEG)) &&
	 (IS_ARM_L(ch,BROKEN_ARM) || IS_ARM_L(ch,LOST_ARM) ||
	    get_eq_char(ch, WEAR_HOLD) != NULL) &&
	 (IS_ARM_R(ch,BROKEN_ARM) || IS_ARM_R(ch,LOST_ARM) ||
	    get_eq_char(ch, WEAR_WIELD) != NULL))
    {
	send_to_char( "You need at least one free arm to drag yourself with.\n\r", ch );
	return;
    }
    else if ( IS_BODY(ch,BROKEN_SPINE) &&
	 (IS_ARM_L(ch,BROKEN_ARM) || IS_ARM_L(ch,LOST_ARM) ||
	    get_eq_char(ch, WEAR_HOLD) != NULL) &&
	 (IS_ARM_R(ch,BROKEN_ARM) || IS_ARM_R(ch,LOST_ARM) ||
	    get_eq_char(ch, WEAR_WIELD) != NULL))
    {
	send_to_char( "You cannot move with a broken spine.\n\r", ch );
	return;
    }

    if ( !IS_NPC(ch) )
    {
	int move;
	if ( in_room->sector_type == SECT_AIR
	||   to_room->sector_type == SECT_AIR )
	{
	    if ( !IS_AFFECTED(ch, AFF_FLYING) &&
	(!IS_NPC(ch) && (!IS_CLASS(ch, CLASS_DROW) ||
!IS_SET(ch->pcdata->powers[1], DPOWER_LEVITATION))) &&
		 (!IS_NPC(ch) && !IS_VAMPAFF(ch, VAM_FLYING) &&
		 (!IS_NPC(ch) && !IS_DEMAFF(ch,DEM_UNFOLDED)))
	    &&   !((mount=ch->mount) != NULL && IS_SET(ch->mounted,IS_RIDING) &&
		 IS_AFFECTED(mount, AFF_FLYING) ) )
	    {
		send_to_char( "You can't fly.\n\r", ch );
		return;
	    }
	}

	if ( in_room->sector_type == SECT_WATER_NOSWIM
	||   to_room->sector_type == SECT_WATER_NOSWIM )
	{
	    OBJ_DATA *obj;
	    bool found;

	    /*
	     * Look for a boat.
	     */
	    found = FALSE;
	    if ( !IS_NPC(ch) && IS_CLASS(ch, CLASS_VAMPIRE))
	    {
		if ( IS_VAMPAFF(ch, VAM_FLYING) )
		    found = TRUE;
		else if ( IS_POLYAFF(ch, POLY_SERPENT) )
		    found = TRUE;
		else if ( IS_AFFECTED(ch, AFF_SHADOWPLANE) )
		    found = TRUE;
		else if ((mount=ch->mount) != NULL && IS_SET(ch->mounted, IS_RIDING) 
		    && IS_AFFECTED(mount, AFF_FLYING))
		    found = TRUE;
		else
		{
		    send_to_char( "You are unable to cross running water.\n\r", ch );
		    return;
		}
	    }
	    if ( IS_AFFECTED(ch, AFF_FLYING) )
		found = TRUE;
	    else if ( !IS_NPC( ch) && IS_DEMAFF(ch, DEM_UNFOLDED) )
		found = TRUE;
	else if (!IS_NPC(ch) && (IS_CLASS(ch, CLASS_DROW) &&
IS_SET(ch->pcdata->powers[1], DPOWER_LEVITATION)))
		found = TRUE;
	    if ( !found )
	    {
	    	for ( obj = ch->carrying; obj != NULL; obj = obj->next_content )
	    	{
		    if ( obj->item_type == ITEM_BOAT )
		    {
		    	found = TRUE;
		    	break;
		    }
	    	}
	    	if ( !found )
	    	{
		    send_to_char( "You need a boat to go there.\n\r", ch );
		    return;
	    	}
	    }
	}
	else if ( !IS_AFFECTED(ch, AFF_FLYING) && IS_POLYAFF(ch, POLY_FISH) )
	{
	    bool from_ok = FALSE;
	    bool to_ok = FALSE;
	    if ( in_room->sector_type == SECT_WATER_NOSWIM ) from_ok = TRUE;
	    if ( in_room->sector_type == SECT_WATER_SWIM   ) from_ok = TRUE;
	    if ( to_room->sector_type == SECT_WATER_NOSWIM ) to_ok   = TRUE;
	    if ( to_room->sector_type == SECT_WATER_SWIM   ) to_ok   = TRUE;
	    if ( !from_ok || !to_ok )
	    {
		send_to_char("You cannot cross land.\n\r",ch);
		return;
	    }
	}

	move = movement_loss[UMIN(SECT_MAX-1, in_room->sector_type)]
	     + movement_loss[UMIN(SECT_MAX-1, to_room->sector_type)]
	     ;

	if (IS_HERO(ch)) move = 0;

	if(ch->move <= 0)
	{
		send_to_char("You are too Exhausted.\n\r",ch);
		return;
	}

	if (IS_SET(ch->mounted, IS_RIDING) && (ch->move < move || ch->move < 1))
	{
	    send_to_char( "You are too exhausted.\n\r", ch );
	    return;
	}

        if (IS_SET(pexit->exit_info, EX_IRON_WALL)
	&& !IS_AFFECTED(ch, AFF_PASS_DOOR) )
    {
	send_to_char("A towering wall of iron blocks your path.\n\r", ch);
	act("$n's path is blocked by the wall of iron.", ch, NULL, NULL, TO_ROOM);
	return;
    }

    if (IS_SET(pexit->exit_info, EX_MUSHROOM_WALL))
    {
	send_to_char("The mushrooms block your path.\n\r", ch);
	return;
    }

    if (IS_SET(pexit->exit_info, EX_ICE_WALL)
	&& ch->power[DISC_DAEM_GELU] < 5 )
    {
	send_to_char("A huge wall of ice blocks your way.\n\r", ch);
	return;
    }

    if (IS_SET(pexit->exit_info, EX_FIRE_WALL)
	&& ch->class == 0)
    {
	act("$n bursts through the wall of fire.", ch, NULL, NULL, TO_ROOM);
	send_to_char("You jump through the flames and are unaffected.\n\r", ch);
    }

    else if (IS_SET(pexit->exit_info, EX_FIRE_WALL)
	&& ch->class != 0 && !IS_NPC(ch) && ch->level > 2)
    {
	act("$n bursts through the wall of fire.", ch, NULL, NULL, TO_ROOM);
	send_to_char("You jump through the flames.\n\r", ch);
	ch->hit -= dice(6, 50);
      stc("The flames sear your flesh.\n\r",ch);
    }

    if (IS_SET(pexit->exit_info, EX_SWORD_WALL)
	&& ch->class == 0)
    {
	act("$n bursts through the wall of swords.", ch, NULL, NULL, TO_ROOM);
	send_to_char("You jump through the swords and are unaffected.\n\r", ch);
    }

    else if (IS_SET(pexit->exit_info, EX_SWORD_WALL)
	&& ch->class != 0 && !IS_NPC(ch) && ch->level > 2)
    {
	act("$n jumps through the wall of swords.", ch, NULL, NULL, TO_ROOM);
	send_to_char("You jump through the swords.\n\r", ch);
	send_to_char("Aaaaaaaaarghhhhhhh! That hurt!\n\r", ch);
	ch->hit -= dice(6, 70);
    }

    if (IS_SET(pexit->exit_info, EX_ASH_WALL) )
    {
	send_to_char("You scream in agony as the wall of ash rips apart your life force.\n\r", ch);
	act("$n screams in agony as the wall of ash rips $s life force apart.", ch, NULL, NULL, TO_ROOM);
	ch->hit /= 2;
	ch->move /= 2;
    }

	WAIT_STATE( ch, 1 );
	if (!IS_SET(ch->mounted, IS_RIDING)) ch->move -= move;
    }

    /* Check for mount message - KaVir */
    if ( (mount = ch->mount) != NULL && ch->mounted == IS_RIDING )
    {
	if (IS_NPC(mount))
	    sprintf(mount2," on %s.",mount->short_descr);
	else
	    sprintf(mount2," on %s.",mount->name);
    }
    else
	sprintf(mount2,".");

    if ( IS_HEAD(ch,LOST_HEAD) || IS_EXTRA(ch,EXTRA_OSWITCH) )
	sprintf(leave,"rolls");
    else if ( IS_AFFECTED(ch,AFF_ETHEREAL) )
	sprintf(leave,"floats");
    else if ( ch->in_room->sector_type == SECT_WATER_SWIM )
	sprintf(leave,"swims");
    else if ( IS_SET(ch->polyaff,POLY_SERPENT) )
	sprintf(leave,"slithers");
    else if ( IS_SET(ch->polyaff,POLY_WOLF) )
	sprintf(leave,"stalks");
    else if ( IS_SET(ch->polyaff,POLY_FROG) )
	sprintf(leave,"hops");
    else if ( IS_SET(ch->polyaff,POLY_FISH) )
	sprintf(leave,"swims");
    else if ( !IS_NPC(ch) && IS_DEMAFF(ch,DEM_UNFOLDED) )
	sprintf(leave,"flies");
    else if ( IS_BODY(ch,BROKEN_SPINE) )
	sprintf(leave,"drags $mself");
    else if ( IS_LEG_L(ch,LOST_LEG) && IS_LEG_R(ch,LOST_LEG) )
	sprintf(leave,"drags $mself");
    else if ( (IS_LEG_L(ch,BROKEN_LEG) || IS_LEG_L(ch,LOST_LEG) || IS_LEG_L(ch,LOST_FOOT)) &&
	 (IS_LEG_R(ch,BROKEN_LEG) || IS_LEG_R(ch,LOST_LEG) || IS_LEG_R(ch,LOST_FOOT)) )
	sprintf(leave,"crawls");
    else if ( ch->hit < (ch->max_hit/4) )
	sprintf(leave,"crawls");
    else if ( (IS_LEG_R(ch,LOST_LEG) || IS_LEG_R(ch,LOST_FOOT)) &&
	 (!IS_LEG_L(ch,BROKEN_LEG) && !IS_LEG_L(ch,LOST_LEG) && 
	  !IS_LEG_L(ch,LOST_FOOT)) )
	sprintf(leave,"hops");
    else if ( (IS_LEG_L(ch,LOST_LEG) || IS_LEG_L(ch,LOST_FOOT)) &&
	 (!IS_LEG_R(ch,BROKEN_LEG) && !IS_LEG_R(ch,LOST_LEG) &&
	  !IS_LEG_R(ch,LOST_FOOT)) )
	sprintf(leave,"hops");
    else if ( (IS_LEG_L(ch,BROKEN_LEG) || IS_LEG_L(ch,LOST_FOOT)) &&
	 (!IS_LEG_R(ch,BROKEN_LEG) && !IS_LEG_R(ch,LOST_LEG) &&
	  !IS_LEG_R(ch,LOST_FOOT)) )
	sprintf(leave,"limps");
    else if ( (IS_LEG_R(ch,BROKEN_LEG) || IS_LEG_R(ch,LOST_FOOT)) &&
	 (!IS_LEG_L(ch,BROKEN_LEG) && !IS_LEG_L(ch,LOST_LEG) &&
	  !IS_LEG_L(ch,LOST_FOOT)) )
	sprintf(leave,"limps");
    else if ( ch->hit < (ch->max_hit/3) )
	sprintf(leave,"limps");
    else if ( ch->hit < (ch->max_hit/2) )
	sprintf(leave,"staggers");
    else if ( !IS_NPC(ch) )
    {
    
	if (ch->pcdata->condition[COND_DRUNK] > 10)
	    sprintf(leave,"staggers");
	else
	    sprintf(leave,"walks");
    }
    else
	sprintf(leave,"walks");

    if (form_data[ch->cur_form].move_word[0] != '\0')
    {
	leave[0] = '\0';
	sprintf(leave, form_data[ch->cur_form].move_word);
    }

    if (!IS_NPC(ch) && ch->stance[0] != -1) do_stance(ch,"");
    for ( d = descriptor_list; d != NULL; d = d->next )
    {
	CHAR_DATA *victim;

	if ( ( victim = d->character ) == NULL )
	    continue;

        if ( ch->in_room == NULL || victim->in_room == NULL ) continue;

        if ( ch == victim || ch->in_room != victim->in_room) continue;

	if ( d->connected != CON_PLAYING || !can_see( ch, victim ) )
	    continue;

	if ( !IS_NPC(ch) && !IS_AFFECTED(ch, AFF_SNEAK) && IS_AFFECTED(ch,AFF_POLYMORPH)
	&& ( IS_NPC(ch) || !IS_SET(ch->act, PLR_WIZINVIS) )
	&& can_see(victim,ch))
	{
	    if (((mount = ch->mount) != NULL && ch->mounted == IS_RIDING &&
	    IS_AFFECTED(mount,AFF_FLYING)) || IS_AFFECTED(ch,AFF_FLYING) ||
	    (!IS_NPC(ch) && IS_VAMPAFF(ch,VAM_FLYING)) )
		sprintf(poly,"%s flies $T%s",ch->morph,mount2);
	    else if ( (mount = ch->mount) != NULL && ch->mounted == IS_RIDING )
		sprintf(poly,"%s rides $T%s",ch->morph,mount2);
	    else
		sprintf(poly,"%s %s $T%s",ch->morph,leave,mount2);
	    act( poly, victim, NULL, dir_name[door], TO_CHAR );
	}
	else if ( !IS_AFFECTED(ch, AFF_SNEAK)
	&& ( IS_NPC(ch) || !IS_SET(ch->act, PLR_WIZINVIS) )
	&& can_see(victim,ch))
	{
	    if (((mount = ch->mount) != NULL && ch->mounted == IS_RIDING &&
	    IS_AFFECTED(mount,AFF_FLYING)) || IS_AFFECTED(ch,AFF_FLYING) ||
	    (!IS_NPC(ch) && IS_VAMPAFF(ch,VAM_FLYING)) )
		sprintf(poly,"$n flies %s%s",dir_name[door],mount2);
	    else if ( (mount = ch->mount) != NULL && ch->mounted == IS_RIDING )
		sprintf(poly,"$n rides %s%s",dir_name[door],mount2);
	    else
		sprintf(poly,"$n %s %s%s",leave,dir_name[door],mount2);
	    act( poly, ch, NULL, victim, TO_VICT );
	}
    }
    char_from_room( ch );
    char_to_room( ch, to_room );
    if      ( door == 0 ) {revdoor = 2;sprintf(buf,"the south");}
    else if ( door == 1 ) {revdoor = 3;sprintf(buf,"the west");}
    else if ( door == 2 ) {revdoor = 0;sprintf(buf,"the north");}
    else if ( door == 3 ) {revdoor = 1;sprintf(buf,"the east");}
    else if ( door == 4 ) {revdoor = 5;sprintf(buf,"below");}
    else                  {revdoor = 4;sprintf(buf,"above");}

    for ( d = descriptor_list; d != NULL; d = d->next )
    {
	CHAR_DATA *victim;

	if ( ( victim = d->character ) == NULL )
	    continue;

        if ( ch->in_room == NULL || victim->in_room == NULL ) continue;

        if ( ch == victim || ch->in_room != victim->in_room) continue;

	if ( d->connected != CON_PLAYING || !can_see( ch, victim ) )
	    continue;

	if ( !IS_NPC(ch) && !IS_AFFECTED(ch, AFF_SNEAK) && IS_AFFECTED(ch,AFF_POLYMORPH)
	&& ( IS_NPC(ch) || !IS_SET(ch->act, PLR_WIZINVIS) ) && can_see(victim,ch))
	{
	    if (((mount = ch->mount) != NULL && ch->mounted == IS_RIDING &&
	    IS_AFFECTED(mount,AFF_FLYING)) || IS_AFFECTED(ch,AFF_FLYING) ||
	    (!IS_NPC(ch) && IS_VAMPAFF(ch,VAM_FLYING)) )
		sprintf(poly,"%s flies in from %s%s",ch->morph,buf,mount2);
	    else if ( (mount = ch->mount) != NULL && ch->mounted == IS_RIDING )
		sprintf(poly,"%s rides in from %s%s",ch->morph,buf,mount2);
	    else
		sprintf(poly,"%s %s in from %s%s",ch->morph,leave,buf,mount2);
	    act( poly, ch, NULL, victim, TO_VICT );
	}
	else if ( !IS_AFFECTED(ch, AFF_SNEAK) && can_see(victim,ch)
	&& ( IS_NPC(ch) || !IS_SET(ch->act, PLR_WIZINVIS) ) )
	{
	    if (((mount = ch->mount) != NULL && ch->mounted == IS_RIDING &&
	    IS_AFFECTED(mount,AFF_FLYING)) || IS_AFFECTED(ch,AFF_FLYING) ||
	    (!IS_NPC(ch) && IS_VAMPAFF(ch,VAM_FLYING)) )
		sprintf( poly, "$n flies in from %s%s",buf,mount2);
	    else if ( (mount = ch->mount) != NULL && ch->mounted == IS_RIDING )
		sprintf(poly,"$n rides in from %s%s",buf,mount2);
	    else
		sprintf( poly, "$n %s in from %s%s",leave,buf,mount2);
	    act( poly, ch, NULL, victim, TO_VICT );
	}
    }

    do_look( ch, "auto" );

    for ( fch = in_room->people; fch != NULL; fch = fch_next )
    {
	fch_next = fch->next_in_room;
	if ( (mount = fch->mount) != NULL && mount == ch && IS_SET(fch->mounted,IS_MOUNT))
	{
	    act( "$N digs $S heels into you.", fch, NULL, ch, TO_CHAR );
	    char_from_room(fch);
	    char_to_room(fch,ch->in_room);
	}

	if ( fch->master == ch && fch->position == POS_STANDING 
		&& fch->in_room != ch->in_room)
	{
	    act( "You follow $N.", fch, NULL, ch, TO_CHAR );
	    move_char( fch, door );
	}
    }
    room_text(ch,">ENTER<");
    return;
}

/*

void do_humanform( CHAR_DATA *ch, char *argument )
{
    OBJ_DATA *obj;

    if (IS_NPC(ch)) return;
    if ( ( obj = ch->pcdata->chobj ) == NULL )
    {
	send_to_char("You are already in human form.\n\r",ch);
	return;
    }

    ch->pcdata->obj_vnum = 0;
    obj->chobj = NULL;
    ch->pcdata->chobj = NULL;
    REMOVE_BIT(ch->affected_by, AFF_POLYMORPH);
    REMOVE_BIT(ch->extra, EXTRA_OSWITCH);
    free_string(ch->morph);
    ch->morph = str_dup("");
    act("$p transforms into $n.",ch,obj,NULL,TO_ROOM);
    act("Your reform your human body.",ch,obj,NULL,TO_CHAR);
    extract_obj(obj);
    if (ch->in_room->vnum == ROOM_VNUM_IN_OBJECT)
    {
	char_from_room(ch);
	char_to_room(ch,get_room_index(ROOM_VNUM_HELL));
    }
    return;
}
void do_notravel( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];

    one_argument( argument, arg );

    if (IS_NPC(ch)) return;

    if (IS_IMMUNE(ch, IMM_TRAVEL))
    {
	REMOVE_BIT(ch->immune, IMM_TRAVEL);
	send_to_char("You can now be the target of travel or burrow.\n\r", ch);
	return;
    }
    else if (!IS_IMMUNE(ch, IMM_TRAVEL)) 
    {
	SET_BIT(ch->immune, IMM_TRAVEL);
	send_to_char("You can no longer be the target of travel or burrow.\n\r", ch);
	return;
    }
    else
	send_to_char( "Excuse you?\n\r", ch );
    return;
}
*/
void do_north( CHAR_DATA *ch, char *argument )
{
char wall[MAX_STRING_LENGTH];

OBJ_DATA *obj;
    ROOM_INDEX_DATA *in_room;
   if (IS_AFFECTED(ch,AFF_WEBBED))
    {
	send_to_char("You are unable to move with all this sticky webbing on.\n\r",ch);
	return;
    }
        if (IS_AFFECTED(ch, AFF_TENDRILS))
    {
send_to_char("You are unable to move tangled in these tendrils.\n\r",ch);
	return;
    }
sprintf(wall,"walln");
obj = get_obj_list(ch,wall,ch->in_room->contents);
if (obj != NULL)
{
send_to_char("\n\rYou cant pass the flowing wall of water.\n\r",ch);
return;
}
 

    in_room = ch->in_room;
    move_char( ch, DIR_NORTH );
    if (!IS_NPC(ch) && ch->in_room != in_room)
    {
	ROOM_INDEX_DATA *old_room;
	old_room = ch->in_room;
	char_from_room(ch);
	char_to_room(ch, in_room);
	add_tracks( ch, DIR_NORTH );
	char_from_room(ch);
	char_to_room(ch, old_room);
    }
/*
    if (!IS_NPC(ch) && ch->in_room != in_room) add_tracks( ch, DIR_SOUTH );
*/
	drow_hate(ch);

    return;
}



void do_east( CHAR_DATA *ch, char *argument )
{
    ROOM_INDEX_DATA *in_room;
OBJ_DATA *obj;
char wall[MAX_STRING_LENGTH];

    if (IS_AFFECTED(ch, AFF_WEBBED))
    {
	send_to_char("You are unable to move with all this sticky webbing on.\n\r",ch);
	return;
    }
    if (IS_AFFECTED(ch, AFF_TENDRILS))
    {
send_to_char("You are unable to move tangled in these tendrils.\n\r",ch);
	return;
    }

sprintf(wall,"walle");
obj = get_obj_list(ch,wall,ch->in_room->contents);
if (obj != NULL)
{
send_to_char("\n\rYou cant pass the flowing wall of water.\n\r",ch);
return;
}
 

    in_room = ch->in_room;
    move_char( ch, DIR_EAST );
    if (!IS_NPC(ch) && ch->in_room != in_room)
    {
	ROOM_INDEX_DATA *old_room;
	old_room = ch->in_room;
	char_from_room(ch);
	char_to_room(ch, in_room);
	add_tracks( ch, DIR_EAST );
	char_from_room(ch);
	char_to_room(ch, old_room);
    }
/*
    if (!IS_NPC(ch) && ch->in_room != in_room) add_tracks( ch, DIR_WEST );
*/
	drow_hate(ch);
    return;
}



void do_south( CHAR_DATA *ch, char *argument )
{
char wall[MAX_STRING_LENGTH];

    ROOM_INDEX_DATA *in_room;
OBJ_DATA *obj;

    if (IS_AFFECTED(ch, AFF_WEBBED))
    {
	send_to_char("You are unable to move with all this sticky webbing on.\n\r",ch);
	return;
    }
    if (IS_AFFECTED(ch, AFF_TENDRILS))
    {
send_to_char("You are unable to move tangled in these tendrils.\n\r",ch);
	return;
    }
sprintf(wall,"walls");
obj = get_obj_list(ch,wall,ch->in_room->contents);
if (obj != NULL)
{
send_to_char("\n\rYou cant pass the flowing wall of water.\n\r",ch);
return;
}
 

    in_room = ch->in_room;
    move_char( ch, DIR_SOUTH );
    if (!IS_NPC(ch) && ch->in_room != in_room)
    {
	ROOM_INDEX_DATA *old_room;
	old_room = ch->in_room;
	char_from_room(ch);
	char_to_room(ch, in_room);
	add_tracks( ch, DIR_SOUTH );
	char_from_room(ch);
	char_to_room(ch, old_room);
    }
/*
    if (!IS_NPC(ch) && ch->in_room != in_room) add_tracks( ch, DIR_NORTH );
*/
	drow_hate(ch);
    return;
}



void do_west( CHAR_DATA *ch, char *argument )
{
    ROOM_INDEX_DATA *in_room;
OBJ_DATA *obj;
char wall[MAX_STRING_LENGTH];

    if (IS_AFFECTED(ch, AFF_WEBBED))
    {
	send_to_char("You are unable to move with all this sticky webbing on.\n\r",ch);
	return;
    }
    if (IS_AFFECTED(ch, AFF_TENDRILS))
    {
send_to_char("You are unable to move tangled in these tendrils.\n\r",ch);
	return;
    }
sprintf(wall,"wallw");
obj = get_obj_list(ch,wall,ch->in_room->contents);
if (obj != NULL)
{
send_to_char("\n\rYou cant pass the flowing wall of water.\n\r",ch);
return;
}
 

    in_room = ch->in_room;
    move_char( ch, DIR_WEST );
    if (!IS_NPC(ch) && ch->in_room != in_room)
    {
	ROOM_INDEX_DATA *old_room;
	old_room = ch->in_room;
	char_from_room(ch);
	char_to_room(ch, in_room);
	add_tracks( ch, DIR_WEST );
	char_from_room(ch);
	char_to_room(ch, old_room);
    }
/*
    if (!IS_NPC(ch) && ch->in_room != in_room) add_tracks( ch, DIR_EAST );
*/
	drow_hate(ch);
    return;
}



void do_up( CHAR_DATA *ch, char *argument )
{
    ROOM_INDEX_DATA *in_room;
OBJ_DATA *obj;
char wall[MAX_STRING_LENGTH];

    if (IS_AFFECTED(ch, AFF_WEBBED))
    {
	send_to_char("You are unable to move with all this sticky webbing on.\n\r",ch);
	return;
    }
    if (IS_AFFECTED(ch, AFF_TENDRILS))
    {
send_to_char("You are unable to move tangled in these tendrils.\n\r",ch);
	return;
    }
sprintf(wall,"wallu");
obj = get_obj_list(ch,wall,ch->in_room->contents);
if (obj != NULL)
{
send_to_char("\n\rYou cant pass the flowing wall of water.\n\r",ch);
return;
}
 

    in_room = ch->in_room;
    move_char( ch, DIR_UP );
    if (!IS_NPC(ch) && ch->in_room != in_room)
    {
	ROOM_INDEX_DATA *old_room;
	old_room = ch->in_room;
	char_from_room(ch);
	char_to_room(ch, in_room);
	add_tracks( ch, DIR_UP );
	char_from_room(ch);
	char_to_room(ch, old_room);
    }
/*
    if (!IS_NPC(ch) && ch->in_room != in_room) add_tracks( ch, DIR_DOWN );
*/
	drow_hate(ch);
    return;
}



void do_down( CHAR_DATA *ch, char *argument )
{
    ROOM_INDEX_DATA *in_room;
    OBJ_DATA *obj;
char wall[MAX_STRING_LENGTH];

    if (IS_AFFECTED(ch, AFF_WEBBED))
    {
	send_to_char("You are unable to move with all this sticky webbing on.\n\r",ch);
	return;
    }
    if (IS_AFFECTED(ch, AFF_TENDRILS))
    {
send_to_char("You are unable to move tangled in these tendrils.\n\r",ch);
	return;
    }
sprintf(wall,"walld");
obj = get_obj_list(ch,wall,ch->in_room->contents);
if (obj != NULL)
{
send_to_char("\n\rYou cant pass the flowing wall of water.\n\r",ch);
return;
}
 

    in_room = ch->in_room;
    move_char( ch, DIR_DOWN );
    if (!IS_NPC(ch) && ch->in_room != in_room)
    {
	ROOM_INDEX_DATA *old_room;
	old_room = ch->in_room;
	char_from_room(ch);
	char_to_room(ch, in_room);
	add_tracks( ch, DIR_DOWN );
	char_from_room(ch);
	char_to_room(ch, old_room);
    }
/*
    if (!IS_NPC(ch) && ch->in_room != in_room) add_tracks( ch, DIR_UP );
*/
	drow_hate(ch);
    return;
}



int find_door( CHAR_DATA *ch, char *arg )
{
    EXIT_DATA *pexit;
    int door;

	 if ( !str_cmp( arg, "n" ) || !str_cmp( arg, "north" ) ) door = 0;
    else if ( !str_cmp( arg, "e" ) || !str_cmp( arg, "east"  ) ) door = 1;
    else if ( !str_cmp( arg, "s" ) || !str_cmp( arg, "south" ) ) door = 2;
    else if ( !str_cmp( arg, "w" ) || !str_cmp( arg, "west"  ) ) door = 3;
    else if ( !str_cmp( arg, "u" ) || !str_cmp( arg, "up"    ) ) door = 4;
    else if ( !str_cmp( arg, "d" ) || !str_cmp( arg, "down"  ) ) door = 5;
    else
    {
	for ( door = 0; door <= 5; door++ )
	{
	    if ( ( pexit = ch->in_room->exit[door] ) != NULL
	    &&   IS_SET(pexit->exit_info, EX_ISDOOR)
	    &&   pexit->keyword != NULL
	    &&   is_name( arg, pexit->keyword ) )
		return door;
	}
	act( "I see no $T here.", ch, NULL, arg, TO_CHAR );
	return -1;
    }

    if ( ( pexit = ch->in_room->exit[door] ) == NULL )
    {
	act( "I see no door $T here.", ch, NULL, arg, TO_CHAR );
	return -1;
    }

    if ( !IS_SET(pexit->exit_info, EX_ISDOOR) )
    {
	send_to_char( "You can't do that.\n\r", ch );
	return -1;
    }

    return door;
}

/* random room generation procedure */
ROOM_INDEX_DATA  *get_random_room(CHAR_DATA *ch)
{
    ROOM_INDEX_DATA *room;

    for ( ; ; )
    {
        room = get_room_index( number_range( 0, 32000 ) );
        if ( room != NULL )
        if ( !IS_SET(room->room_flags, ROOM_PRIVATE)
        &&   !IS_SET(room->room_flags, ROOM_SOLITARY) 
	&&   !IS_SET(room->room_flags, ROOM_SAFE) )
            break;
    }

    return room;
}

ROOM_INDEX_DATA  *get_rand_room()
{
    ROOM_INDEX_DATA *room;

    for ( ; ; )
    {
        room = get_room_index( number_range( 0, 32000 ) );
        if ( room != NULL )
        if ( !IS_SET(room->room_flags, ROOM_PRIVATE)
        &&   !IS_SET(room->room_flags, ROOM_SOLITARY) 
	&&   !IS_SET(room->room_flags, ROOM_SAFE) 
	&&  room->vnum > 300)
            break;
    }

    return room;
}

ROOM_INDEX_DATA  *get_treemeld_room()
{
    ROOM_INDEX_DATA *room;

    for ( ; ; )
    {
        room = get_room_index( number_range( 0, 32000 ) );
        if ( room != NULL
	    && room->sector_type == SECT_FOREST ) break;
    }

    return room;
}

ROOM_INDEX_DATA  *get_rand_room_by_sect( int sect )
{
    ROOM_INDEX_DATA *room;

    for ( ; ; )
    {
        room = get_room_index( number_range( 0, 32000 ) );
        if ( room != NULL
	    && room->sector_type == sect ) break;
    }

    return room;
}

/* Designed for the portal spell, but can also have other uses...KaVir
 * V0 = Where the portal will take you.
 * V1 = Number of uses (0 is infinate).
 * V2 = if 2, cannot be entered.
 * V3 = The room the portal is currently in.
 */
void do_enter( CHAR_DATA *ch, char *argument )
{
    ROOM_INDEX_DATA *pRoomIndex;
    ROOM_INDEX_DATA *location;
    char arg[MAX_INPUT_LENGTH];
    char poly [MAX_INPUT_LENGTH];
    OBJ_DATA *obj;
    OBJ_DATA *portal;
    OBJ_DATA *portal_next;
    CHAR_DATA *mount;
    bool found;

    argument = one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
	send_to_char( "Enter what?\n\r", ch );
	return;
    }

    obj = get_obj_list( ch, arg, ch->in_room->contents );
    if ( obj == NULL )
    {
	act( "I see no $T here.", ch, NULL, arg, TO_CHAR );
	return;
    }
    if ( obj->item_type != ITEM_PORTAL )
    {
	act( "You cannot enter that.", ch, NULL, arg, TO_CHAR );
	return;
    }

    if (IS_AFFECTED(ch, AFF_SHADOWPLANE) &&
	!IS_SET(obj->extra_flags, ITEM_SHADOWPLANE) )
    {
	send_to_char( "You are too insubstantual.\n\r", ch );
	return;
    }
    else if (!IS_AFFECTED(ch, AFF_SHADOWPLANE) &&
	IS_SET(obj->extra_flags, ITEM_SHADOWPLANE) )
    {
	send_to_char( "It is too insubstantual.\n\r", ch );
	return;
    }

    if ( obj->value[2] == 2 || obj->value[2] == 3 )
    {
	act( "It seems to be closed.", ch, NULL, arg, TO_CHAR );
	return;
    }

    pRoomIndex = get_room_index(obj->value[0]);
    location = ch->in_room;

    if ( pRoomIndex == NULL )
    {
	act( "You are unable to enter.", ch, NULL, arg, TO_CHAR );
	return;
    }

    act( "You step into $p.",  ch, obj, NULL, TO_CHAR );
    if ( !IS_NPC(ch) && IS_AFFECTED(ch,AFF_POLYMORPH) )
	sprintf(poly,"%s steps into $p.",ch->morph);
    else
	sprintf(poly,"$n steps into $p.");
    act( poly,  ch, obj, NULL, TO_ROOM );
    char_from_room(ch);
    char_to_room(ch,pRoomIndex);
    if ( !IS_NPC(ch) && IS_AFFECTED(ch,AFF_POLYMORPH) )
	sprintf(poly,"%s steps out of $p.",ch->morph);
    else
	sprintf(poly,"$n steps out of $p.");
    act( poly,  ch, obj, NULL, TO_ROOM );
    char_from_room(ch);
    char_to_room(ch,location);
/* Leave this out for now, as it doesn't seem to work properly. KaVir
    if ((obj->value[2] == 1) && (CAN_WEAR(obj,ITEM_TAKE)))
    {
	obj->value[2] = 2;
	REMOVE_BIT(obj->wear_flags, ITEM_TAKE);
    }
*/
    if (obj->value[1] != 0)
    {
	obj->value[1] = obj->value[1] - 1;
	if (obj->value[1] < 1)
	{
	    act( "$p vanishes.",  ch, obj, NULL, TO_CHAR );
	    act( "$p vanishes.",  ch, obj, NULL, TO_ROOM );
	    extract_obj(obj);
	}
    }
    char_from_room(ch);
    char_to_room(ch,pRoomIndex);
    found = FALSE;
    for ( portal = ch->in_room->contents; portal != NULL; portal = portal_next )
    {
	portal_next = portal->next_content;
	if ( ( obj->value[0] == portal->value[3]  )
	    && (obj->value[3] == portal->value[0]) )
	{
	    found = TRUE;
/* Leave this out for now, as it doesn't seem to work properly. KaVir
	    if ((portal->value[2] == 2) && (!CAN_WEAR(obj,ITEM_TAKE)))
	    {
		portal->value[2] = 1;
		SET_BIT(obj->wear_flags, ITEM_TAKE);
	    }
*/
	    if (IS_AFFECTED(ch, AFF_SHADOWPLANE) &&
		!IS_SET(portal->extra_flags, ITEM_SHADOWPLANE) )
	    {
		REMOVE_BIT(ch->affected_by, AFF_SHADOWPLANE);
		break;
	    }
	    else if (!IS_AFFECTED(ch, AFF_SHADOWPLANE) &&
		IS_SET(portal->extra_flags, ITEM_SHADOWPLANE) )
	    {
		SET_BIT(ch->affected_by, AFF_SHADOWPLANE);
		break;
	    }
    	    if (portal->value[1] != 0)
    	    {
		portal->value[1] = portal->value[1] - 1;
		if (portal->value[1] < 1)
		{
		    act( "$p vanishes.",  ch, portal, NULL, TO_CHAR );
		    act( "$p vanishes.",  ch, portal, NULL, TO_ROOM );
		    extract_obj(portal);
		}
	    }
    	}
    }
    do_look(ch,"auto");
    if ( ( mount = ch->mount ) == NULL ) return;
    char_from_room( mount );
    char_to_room( mount, ch->in_room );
    return;
}


void do_open( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    OBJ_DATA *obj;
    int door;

    one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
	send_to_char( "Open what?\n\r", ch );
	return;
    }

    if ( ( obj = get_obj_here( ch, arg ) ) != NULL )
    {
	/* 'open object' */
	if ( obj->item_type != ITEM_CONTAINER && obj->item_type != ITEM_BOOK)
	    { send_to_char( "That's not a container.\n\r", ch ); return; }
	if ( !IS_SET(obj->value[1], CONT_CLOSED) )
	    { send_to_char( "It's already open.\n\r",      ch ); return; }
	if ( !IS_SET(obj->value[1], CONT_CLOSEABLE) && obj->item_type != ITEM_BOOK)
	    { send_to_char( "You can't do that.\n\r",      ch ); return; }
	if ( IS_SET(obj->value[1], CONT_LOCKED) )
	    { send_to_char( "It's locked.\n\r",            ch ); return; }

	REMOVE_BIT(obj->value[1], CONT_CLOSED);
	send_to_char( "Ok.\n\r", ch );
	act( "$n opens $p.", ch, obj, NULL, TO_ROOM );
	return;
    }

    if ( ( door = find_door( ch, arg ) ) >= 0 )
    {
	/* 'open door' */
	ROOM_INDEX_DATA *to_room;
	EXIT_DATA *pexit;
	EXIT_DATA *pexit_rev;

	pexit = ch->in_room->exit[door];
	if ( !IS_SET(pexit->exit_info, EX_CLOSED) )
	    { send_to_char( "It's already open.\n\r",      ch ); return; }
	if (  IS_SET(pexit->exit_info, EX_LOCKED) )
	    { send_to_char( "It's locked.\n\r",            ch ); return; }

	REMOVE_BIT(pexit->exit_info, EX_CLOSED);
	act( "$n opens the $d.", ch, NULL, pexit->keyword, TO_ROOM );
	send_to_char( "Ok.\n\r", ch );

	/* open the other side */
	if ( ( to_room   = pexit->to_room               ) != NULL
	&&   ( pexit_rev = to_room->exit[rev_dir[door]] ) != NULL
	&&   pexit_rev->to_room == ch->in_room )
	{
	    CHAR_DATA *rch;

	    REMOVE_BIT( pexit_rev->exit_info, EX_CLOSED );
	    for ( rch = to_room->people; rch != NULL; rch = rch->next_in_room )
		act( "The $d opens.", rch, NULL, pexit_rev->keyword, TO_CHAR );
	}
    }

    return;
}



void do_close( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    OBJ_DATA *obj;
    int door;

    one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
	send_to_char( "Close what?\n\r", ch );
	return;
    }

    if ( ( obj = get_obj_here( ch, arg ) ) != NULL )
    {
	/* 'close object' */
	if ( obj->item_type != ITEM_CONTAINER && obj->item_type != ITEM_BOOK)
	    { send_to_char( "That's not a container.\n\r", ch ); return; }
	if ( IS_SET(obj->value[1], CONT_CLOSED) )
	    { send_to_char( "It's already closed.\n\r",    ch ); return; }
	if ( !IS_SET(obj->value[1], CONT_CLOSEABLE) && obj->item_type != ITEM_BOOK )
	    { send_to_char( "You can't do that.\n\r",      ch ); return; }

	SET_BIT(obj->value[1], CONT_CLOSED);
	if (obj->item_type == ITEM_BOOK) obj->value[2] = 0;
	send_to_char( "Ok.\n\r", ch );
	act( "$n closes $p.", ch, obj, NULL, TO_ROOM );
	return;
    }

    if ( ( door = find_door( ch, arg ) ) >= 0 )
    {
	/* 'close door' */
	ROOM_INDEX_DATA *to_room;
	EXIT_DATA *pexit;
	EXIT_DATA *pexit_rev;

	pexit	= ch->in_room->exit[door];
	if ( IS_SET(pexit->exit_info, EX_CLOSED) )
	    { send_to_char( "It's already closed.\n\r",    ch ); return; }

	SET_BIT(pexit->exit_info, EX_CLOSED);
	act( "$n closes the $d.", ch, NULL, pexit->keyword, TO_ROOM );
	send_to_char( "Ok.\n\r", ch );

	/* close the other side */
	if ( ( to_room   = pexit->to_room               ) != NULL
	&&   ( pexit_rev = to_room->exit[rev_dir[door]] ) != 0
	&&   pexit_rev->to_room == ch->in_room )
	{
	    CHAR_DATA *rch;

	    SET_BIT( pexit_rev->exit_info, EX_CLOSED );
	    for ( rch = to_room->people; rch != NULL; rch = rch->next_in_room )
		act( "The $d closes.", rch, NULL, pexit_rev->keyword, TO_CHAR );
	}
    }

    return;
}



void do_turn( CHAR_DATA *ch, char *argument )
{
    char arg1[MAX_INPUT_LENGTH];
    char arg2[MAX_INPUT_LENGTH];
    OBJ_DATA *obj;
    int value = 0;

    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );

    if ( arg1[0] == '\0' || arg2[0] == '\0' )
    {
	send_to_char( "Syntax: Turn <book> <forward/back>.\n\r", ch );
	return;
    }

    if (is_number(arg2)) value = atoi(arg2);

    if ( ( obj = get_obj_here( ch, arg1 ) ) == NULL )
    {
	send_to_char("You don't have that book.\n\r",ch);
	return;
    }

    if ( obj->item_type != ITEM_BOOK)
    { send_to_char( "That's not a book.\n\r", ch ); return; }
    if ( IS_SET(obj->value[1], CONT_CLOSED) )
    { send_to_char( "First you should open it.\n\r",    ch ); return; }

    if (!str_cmp(arg2,"f") || !str_cmp(arg2,"forward"))
    {
	if (obj->value[2] >= obj->value[3])
	{
	    send_to_char("But you are already at the end of the book.\n\r",ch);
	    return;
	}
	obj->value[2] += 1;
	act( "You flip forward a page in $p.", ch, obj, NULL, TO_CHAR );
	act( "$n flips forward a page in $p.", ch, obj, NULL, TO_ROOM );
    }
    else if (!str_cmp(arg2,"b") || !str_cmp(arg2,"backward"))
    {
	if (obj->value[2] <= 0)
	{
	    send_to_char("But you are already at the beginning of the book.\n\r",ch);
	    return;
	}
	obj->value[2] -= 1;
	act( "You flip backward a page in $p.", ch, obj, NULL, TO_CHAR );
	act( "$n flips backward a page in $p.", ch, obj, NULL, TO_ROOM );
    }
    else if (is_number(arg2) && value >= 0 && value <= obj->value[3])
    {
	if (value == obj->value[2])
	{
	    act("$p is already open at that page.",ch,obj,NULL,TO_CHAR);
	    return;
	}
	else if (value < obj->value[2])
	{
	    act( "You flip backwards through $p.", ch, obj, NULL, TO_CHAR );
	    act( "$n flips backwards through $p.", ch, obj, NULL, TO_ROOM );
	}
	else
	{
	    act( "You flip forwards through $p.", ch, obj, NULL, TO_CHAR );
	    act( "$n flips forwards through $p.", ch, obj, NULL, TO_ROOM );
	}
	obj->value[2] = value;
    }
    else
	send_to_char("Do you wish to turn forward or backward a page?\n\r",ch);
    return;
}

void do_read( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    char buf[MAX_STRING_LENGTH];
    OBJ_DATA *obj;

    one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
	send_to_char( "What do you wish to read?\n\r", ch );
	return;
    }

    if ( ( obj = get_obj_here( ch, arg ) ) == NULL )
    {
	send_to_char("You don't have that book.\n\r",ch);
	return;
    }

    if ( obj->item_type == ITEM_PAGE)
    {
	if (!str_cmp(obj->victpoweruse,"(null)"))
	    sprintf(buf,"Untitled page.\n\r");
	else
	    sprintf(buf,"%s.\n\r",obj->victpoweruse);
	buf[0] = UPPER(buf[0]);
	send_to_char(buf,ch);
	if (obj->chpoweruse == NULL || obj->chpoweruse == '\0' ||
	    !str_cmp(obj->chpoweruse,"(null)"))
	{
	    if (!are_runes(obj))
		send_to_char("This page is blank.\n\r",ch);
	    else if (IS_AFFECTED(ch, AFF_DETECT_MAGIC) && 
		!IS_SET(obj->quest, QUEST_MASTER_RUNE) &&
		!IS_SET(obj->spectype, ADV_STARTED))
		show_runes(ch, obj, FALSE);
	    else
		send_to_char("This page is blank.\n\r",ch);
	    return;
	}
	send_to_char("--------------------------------------------------------------------------------\n\r",ch);
	send_to_char(obj->chpoweruse,ch);
	send_to_char("\n\r--------------------------------------------------------------------------------\n\r",ch);
	if (IS_AFFECTED(ch, AFF_DETECT_MAGIC) && 
	    !IS_SET(obj->spectype, ADV_STARTED) &&
	    !IS_SET(obj->quest, QUEST_MASTER_RUNE )) show_runes(ch, obj, TRUE);
	return;
    }

    if ( obj->item_type != ITEM_BOOK)
    { send_to_char( "That's not a book.\n\r", ch ); return; }
    if ( IS_SET(obj->value[1], CONT_CLOSED) )
    {
	if (!str_cmp(obj->victpoweruse,"(null)"))
	    sprintf(buf,"The book is untitled.\n\r");
	else
	    sprintf(buf,"The book is titled '%s'.\n\r",obj->victpoweruse);
	buf[0] = UPPER(buf[0]);
	send_to_char(buf,ch);
	return;
    }

    if (obj->value[2] == 0)
    {
	int page;

	send_to_char("Index page.\n\r",ch);
	if (obj->value[3] <= 0)
	{
	    send_to_char("<No pages>\n\r",ch);
	    return;
	}
	for ( page = 1; page <= obj->value[3]; page ++ )
	{
	    sprintf(buf,"Page %d:",page);
	    send_to_char(buf,ch);
	    show_page(ch,obj,page,TRUE);
	}
    }
    else
    {
	sprintf(buf,"Page %d:",obj->value[2]);
	send_to_char(buf,ch);
	show_page(ch,obj,obj->value[2],FALSE);
    }
    return;
}

void do_write( CHAR_DATA *ch, char *argument )
{
    char arg1[MAX_INPUT_LENGTH];
    char arg2[MAX_INPUT_LENGTH];
    char arg3[MAX_INPUT_LENGTH];
    char buf[MAX_STRING_LENGTH];
    OBJ_DATA *obj;
    OBJ_DATA *object;
    CHAR_DATA *mobile;

    smash_tilde( argument );
    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );
    strcpy( arg3, argument );

    if (IS_NPC(ch)) return;
    if ( arg1[0] == '\0' || arg2[0] == '\0' || arg3[0] == '\0' )
    {
	send_to_char("Syntax: Write <page> <title/line> <text>.\n\r",ch);
	return;
    }

    if ( ( ( ( obj = get_eq_char(ch, WEAR_HOLD) ) != NULL) && 
	( obj->item_type != ITEM_TOOL || !IS_SET(obj->value[0],TOOL_PEN) ) )
	|| obj == NULL)
    {
	if ( ( ( ( obj = get_eq_char(ch, WEAR_WIELD) ) != NULL) && 
	    ( obj->item_type != ITEM_TOOL || !IS_SET(obj->value[0],TOOL_PEN) ) )
	    || obj == NULL)
	{
	    send_to_char("You are not holding a pen.\n\r",ch);
	    return;
	}
    }

    if ( ( obj = get_obj_carry( ch, arg1) ) == NULL )
    {
	send_to_char("You are not carrying that item.\n\r",ch);
	return;
    }
    if ( obj->item_type != ITEM_PAGE && obj->item_type != ITEM_BOOK )
    {
	send_to_char("You cannot write on that.\n\r",ch);
	return;
    }
    if (!str_cmp( arg2,"title"))
    {
	free_string( obj->victpoweruse );
	obj->victpoweruse = str_dup( arg3 );
	send_to_char("Ok.\n\r",ch);
	act("$n writes something on $p.",ch,obj,NULL,TO_ROOM);
	return;
    }
    else if (str_cmp( arg2,"line"))
    {
	send_to_char("You can write a TITLE or a LINE.\n\r",ch);
	return;
    }
    else if ( obj->item_type == ITEM_BOOK )
    {
	send_to_char("You can only write a title on the book.\n\r",ch);
	return;
    }
	else if (IS_SET(obj->spectype, ADV_FINISHED)) {
		send_to_char("It is already finished!\n\r", ch );
		return;}

    if (obj->chpoweruse != NULL) strcpy(buf,obj->chpoweruse);
	else return;
    if (!str_cmp(buf,"(null)"))
    {
	arg3[0] = UPPER(arg3[0]);
	free_string( obj->chpoweruse );
	obj->chpoweruse = str_dup( arg3 );
	send_to_char("Ok.\n\r",ch);
	act("$n writes something on $p.",ch,obj,NULL,TO_ROOM);
	if (!IS_CLASS(ch, CLASS_MAGE) && !IS_IMMORTAL(ch) 
	  && !IS_SET(obj->spectype, ADV_FINISHED))
	    SET_BIT(obj->spectype, ADV_FAILED);
	else if (!str_cmp(arg3,"start.damage.spell") && obj->spectype == 0)
	    {SET_BIT(obj->spectype, ADV_STARTED);
	    SET_BIT(obj->spectype, ADV_DAMAGE);}
	else if (!str_cmp(arg3,"start.affect.spell") && obj->spectype == 0)
	    {SET_BIT(obj->spectype, ADV_STARTED);
	    SET_BIT(obj->spectype, ADV_AFFECT);}
	else if (!str_cmp(arg3,"start.action.spell") && obj->spectype == 0)
	    {SET_BIT(obj->spectype, ADV_STARTED);
	    SET_BIT(obj->spectype, ADV_ACTION);}
	else if (!str_cmp(arg3,"start.spell") && obj->spectype == 0)
	    SET_BIT(obj->spectype, ADV_STARTED);
	else if (!IS_SET(obj->spectype, ADV_FINISHED))
	    SET_BIT(obj->spectype, ADV_FAILED);
	return;
    }
    if (obj->chpoweruse != NULL && buf[0] != '\0')
    {
	if (strlen(buf)+strlen(arg3) >= MAX_STRING_LENGTH-4)
	{
	    send_to_char("Line too long.\n\r",ch);
	    return;
	}
	else
	{
	    free_string( obj->chpoweruse );
	    strcat( buf, "\n\r" );
	    strcat( buf, arg3 );
	    obj->chpoweruse = str_dup( buf );

	    argument = one_argument( argument, arg1 );
	    strcpy( arg2, argument );

	    if (!IS_CLASS(ch, CLASS_MAGE) && !IS_IMMORTAL(ch) 
		&& !IS_SET(obj->spectype, ADV_FINISHED))
		SET_BIT(obj->spectype, ADV_FAILED);
	    else if (!str_cmp(arg1,"start.damage.spell") && obj->spectype == 0)
		{SET_BIT(obj->spectype, ADV_STARTED);
		SET_BIT(obj->spectype, ADV_DAMAGE);}
	    else if (!str_cmp(arg1,"start.affect.spell") && obj->spectype == 0)
		{SET_BIT(obj->spectype, ADV_STARTED);
		SET_BIT(obj->spectype, ADV_AFFECT);}
	    else if (!str_cmp(arg1,"start.action.spell") && obj->spectype == 0)
		{SET_BIT(obj->spectype, ADV_STARTED);
		SET_BIT(obj->spectype, ADV_ACTION);}
	    else if (!str_cmp(arg1,"start.spell") && obj->spectype == 0)
		SET_BIT(obj->spectype, ADV_STARTED);
	    else if (!str_cmp(arg1,"end.spell") 
		&& IS_SET(obj->spectype, ADV_STARTED) 
		&& !IS_SET(obj->spectype, ADV_FINISHED))
	    {
		SET_BIT(obj->spectype, ADV_FINISHED);
		obj->toughness = ch->pcdata->powers[MPOWER_RUNE0];
		obj->points += 1;
	    }
	    else if (!str_cmp(arg1,"damage.spell") 
		&& IS_SET(obj->spectype, ADV_STARTED) 
		&& !IS_SET(obj->spectype, ADV_DAMAGE)
		&& !IS_SET(obj->spectype, ADV_AFFECT)
		&& !IS_SET(obj->spectype, ADV_ACTION)
		&& !IS_SET(obj->spectype, ADV_FINISHED))
		SET_BIT(obj->spectype, ADV_DAMAGE);
	    else if (!str_cmp(arg1,"affect.spell") 
		&& IS_SET(obj->spectype, ADV_STARTED) 
		&& !IS_SET(obj->spectype, ADV_DAMAGE)
		&& !IS_SET(obj->spectype, ADV_AFFECT)
		&& !IS_SET(obj->spectype, ADV_ACTION)
		&& !IS_SET(obj->spectype, ADV_FINISHED))
		SET_BIT(obj->spectype, ADV_AFFECT);
	    else if (!str_cmp(arg1,"action.spell") 
		&& IS_SET(obj->spectype, ADV_STARTED) 
		&& !IS_SET(obj->spectype, ADV_DAMAGE)
		&& !IS_SET(obj->spectype, ADV_AFFECT)
		&& !IS_SET(obj->spectype, ADV_ACTION)
		&& !IS_SET(obj->spectype, ADV_FINISHED))
		SET_BIT(obj->spectype, ADV_AFFECT);
	    else if (!str_cmp(arg1,"area.affect") 
		&& IS_SET(obj->spectype, ADV_STARTED) 
		&& !IS_SET(obj->spectype, ADV_AREA_AFFECT)
		&& !IS_SET(obj->spectype, ADV_FINISHED))
		{SET_BIT(obj->spectype, ADV_AREA_AFFECT);obj->points += 100;}
	    else if (!str_cmp(arg1,"victim.target") 
		&& IS_SET(obj->spectype, ADV_STARTED) 
		&& !IS_SET(obj->spectype, ADV_VICTIM_TARGET)
		&& !IS_SET(obj->spectype, ADV_FINISHED))
		{SET_BIT(obj->spectype, ADV_VICTIM_TARGET);obj->points += 5;}
	    else if (!str_cmp(arg1,"object.target") 
		&& IS_SET(obj->spectype, ADV_STARTED) 
		&& !IS_SET(obj->spectype, ADV_OBJECT_TARGET)
		&& !IS_SET(obj->spectype, ADV_FINISHED))
		{SET_BIT(obj->spectype, ADV_OBJECT_TARGET);obj->points += 5;}
	    else if (!str_cmp(arg1,"global.target") 
		&& IS_SET(obj->spectype, ADV_STARTED) 
		&& !IS_SET(obj->spectype, ADV_GLOBAL_TARGET)
		&& !IS_SET(obj->spectype, ADV_FINISHED))
		{SET_BIT(obj->spectype, ADV_GLOBAL_TARGET);obj->points += 50;}
	    else if (!str_cmp(arg1,"next.page") 
		&& IS_SET(obj->spectype, ADV_STARTED) 
		&& !IS_SET(obj->spectype, ADV_NEXT_PAGE)
		&& !IS_SET(obj->spectype, ADV_FINISHED))
		{SET_BIT(obj->spectype, ADV_NEXT_PAGE);obj->points += 5;}
	    else if (!str_cmp(arg1,"parameter:") 
		&& IS_SET(obj->spectype, ADV_STARTED) 
		&& !IS_SET(obj->spectype, ADV_PARAMETER)
		&& !IS_SET(obj->spectype, ADV_FINISHED))
	    {
		if ( arg2[0] == '\0' )
		    SET_BIT(obj->spectype, ADV_FAILED);
		else
		{
		    SET_BIT(obj->spectype, ADV_PARAMETER);
		    free_string(obj->chpoweron);
		    obj->chpoweron = str_dup(arg2);
		}
	    }
	    else if (!str_cmp(arg1,"spell.first") 
		&& IS_SET(obj->spectype, ADV_STARTED) 
		&& !IS_SET(obj->spectype, ADV_SPELL_FIRST)
		&& !IS_SET(obj->spectype, ADV_FINISHED))
		SET_BIT(obj->spectype, ADV_SPELL_FIRST);
	    else if (!str_cmp(arg1,"not.caster") 
		&& IS_SET(obj->spectype, ADV_STARTED) 
		&& !IS_SET(obj->spectype, ADV_NOT_CASTER)
		&& !IS_SET(obj->spectype, ADV_FINISHED))
		SET_BIT(obj->spectype, ADV_NOT_CASTER);
	    else if (!str_cmp(arg1,"no.players") 
		&& IS_SET(obj->spectype, ADV_STARTED) 
		&& !IS_SET(obj->spectype, ADV_NO_PLAYERS)
		&& !IS_SET(obj->spectype, ADV_FINISHED))
		SET_BIT(obj->spectype, ADV_NO_PLAYERS);
	    else if (!str_cmp(arg1,"second.victim") 
		&& IS_SET(obj->spectype, ADV_STARTED) 
		&& !IS_SET(obj->spectype, ADV_SECOND_VICTIM)
		&& !IS_SET(obj->spectype, ADV_FINISHED))
		{SET_BIT(obj->spectype, ADV_SECOND_VICTIM);obj->points += 5;}
	    else if (!str_cmp(arg1,"second.object") 
		&& IS_SET(obj->spectype, ADV_STARTED) 
		&& !IS_SET(obj->spectype, ADV_SECOND_OBJECT)
		&& !IS_SET(obj->spectype, ADV_FINISHED))
		{SET_BIT(obj->spectype, ADV_SECOND_OBJECT);obj->points += 5;}
	    else if (!str_cmp(arg1,"reversed")
		&& IS_SET(obj->spectype, ADV_STARTED) 
		&& !IS_SET(obj->spectype, ADV_REVERSED)
		&& !IS_SET(obj->spectype, ADV_FINISHED))
		SET_BIT(obj->spectype, ADV_REVERSED);
	    else if (!str_cmp(arg1,"min.damage:")
		&& IS_SET(obj->spectype, ADV_STARTED) 
		&& IS_SET(obj->spectype, ADV_DAMAGE)
		&& !IS_SET(obj->spectype, ADV_FINISHED))
	    {
		if ( arg2[0] == '\0' || !is_number(arg2) || 
		    atoi(arg2) < 0 || atoi(arg2) > 500)
		    SET_BIT(obj->spectype, ADV_FAILED);
		else {obj->value[1] = atoi(arg2);obj->points += (atoi(arg2)*0.5);}
	    }
	    else if (!str_cmp(arg1,"max.damage:")
		&& IS_SET(obj->spectype, ADV_STARTED) 
		&& IS_SET(obj->spectype, ADV_DAMAGE)
		&& !IS_SET(obj->spectype, ADV_FINISHED))
	    {
		if ( arg2[0] == '\0' || !is_number(arg2) || 
		    atoi(arg2) < 0 || atoi(arg2) > 1000)
		    SET_BIT(obj->spectype, ADV_FAILED);
		else {obj->value[2] = atoi(arg2);obj->points += (atoi(arg2)*0.5);}
	    }
	    else if (!str_cmp(arg1,"move")
		&& IS_SET(obj->spectype, ADV_STARTED) 
		&& IS_SET(obj->spectype, ADV_ACTION)
		&& obj->value[1] == ACTION_NONE
		&& !IS_SET(obj->spectype, ADV_FINISHED))
		{obj->value[1] = ACTION_MOVE;obj->points += 500;}
	    else if (!str_cmp(arg1,"mob:")
		&& IS_SET(obj->spectype, ADV_STARTED) 
		&& IS_SET(obj->spectype, ADV_ACTION)
		&& obj->value[1] == ACTION_NONE
		&& !IS_SET(obj->spectype, ADV_FINISHED))
	    {
		obj->value[1] = ACTION_MOB;
		if ( arg2[0] == '\0' ||
		    ( mobile = get_char_world2( ch, arg2 ) ) == NULL)
		    SET_BIT(obj->spectype, ADV_FAILED);
		else {obj->value[2] = mobile->pIndexData->vnum;obj->points += 500;}
	    }
	    else if (!str_cmp(arg1,"object:")
		&& IS_SET(obj->spectype, ADV_STARTED) 
		&& IS_SET(obj->spectype, ADV_ACTION)
		&& obj->value[1] == ACTION_NONE
		&& !IS_SET(obj->spectype, ADV_FINISHED))
	    {
		obj->value[1] = ACTION_OBJECT;
		if ( arg2[0] == '\0' ||
		    ( object = get_obj_world2( ch, arg2 ) ) == NULL)
		    SET_BIT(obj->spectype, ADV_FAILED);
		else if (IS_SET(object->quest, QUEST_ARTIFACT))
		    SET_BIT(object->spectype, ADV_FAILED);
		else if (strlen(object->questowner) > 2)
		    SET_BIT(object->spectype, ADV_FAILED);
		else {obj->value[2] = object->pIndexData->vnum;obj->points += 500;}
	    }
	    else if (!str_cmp(arg1,"apply:")
		&& IS_SET(obj->spectype, ADV_STARTED) 
		&& IS_SET(obj->spectype, ADV_AFFECT)
		&& !IS_SET(obj->spectype, ADV_FINISHED))
	    {
		if ( arg2[0] == '\0' )
		    SET_BIT(obj->spectype, ADV_FAILED);
		else if ((!str_cmp(arg2,"strength") || !str_cmp(arg2,"str")) && 
		    !IS_SET(obj->value[1], ADV_STR))
		    SET_BIT(obj->value[1], ADV_STR);
		else if ((!str_cmp(arg2,"dexterity") || !str_cmp(arg2,"dex")) && 
		    !IS_SET(obj->value[1], ADV_DEX))
		    SET_BIT(obj->value[1], ADV_DEX);
		else if ((!str_cmp(arg2,"intelligence") || !str_cmp(arg2,"int")) && 
		    !IS_SET(obj->value[1], ADV_INT))
		    SET_BIT(obj->value[1], ADV_INT);
		else if ((!str_cmp(arg2,"wisdom") || !str_cmp(arg2,"wis")) && 
		    !IS_SET(obj->value[1], ADV_WIS))
		    SET_BIT(obj->value[1], ADV_WIS);
		else if ((!str_cmp(arg2,"constitution") || !str_cmp(arg2,"con")) && 
		    !IS_SET(obj->value[1], ADV_CON))
		    SET_BIT(obj->value[1], ADV_CON);
		else if (!str_cmp(arg2,"mana") && 
		    !IS_SET(obj->value[1], ADV_MANA))
		    SET_BIT(obj->value[1], ADV_MANA);
		else if ((!str_cmp(arg2,"hp") || !str_cmp(arg2,"hits") || 
		    !str_cmp(arg2,"hitpoints")) && 
		    !IS_SET(obj->value[1], ADV_HIT))
		    SET_BIT(obj->value[1], ADV_HIT);
		else if ((!str_cmp(arg2,"move") || !str_cmp(arg2,"movement")) && 
		    !IS_SET(obj->value[1], ADV_MOVE))
		    SET_BIT(obj->value[1], ADV_MOVE);
		else if ((!str_cmp(arg2,"ac") || !str_cmp(arg2,"armour") ||
		    !str_cmp(arg2,"armor")) && 
		    !IS_SET(obj->value[1], ADV_AC))
		    SET_BIT(obj->value[1], ADV_AC);
		else if ((!str_cmp(arg2,"hr") || !str_cmp(arg2,"hit") ||
		    !str_cmp(arg2,"hitroll")) && 
		    !IS_SET(obj->value[1], ADV_HITROLL))
		    SET_BIT(obj->value[1], ADV_HITROLL);
		else if ((!str_cmp(arg2,"dr") || !str_cmp(arg2,"dam") ||
		    !str_cmp(arg2,"damroll")) && 
		    !IS_SET(obj->value[1], ADV_DAMROLL))
		    SET_BIT(obj->value[1], ADV_DAMROLL);
		else if ((!str_cmp(arg2,"save") || !str_cmp(arg2,"save.spell") ||
		    !str_cmp(arg2,"save_spell")) && 
		    !IS_SET(obj->value[1], ADV_SAVING_SPELL))
		    SET_BIT(obj->value[1], ADV_SAVING_SPELL);
		else {SET_BIT(obj->spectype, ADV_FAILED);return;}
		obj->points += 25;
	    }
	    else if (!str_cmp(arg1,"affect:")
		&& IS_SET(obj->spectype, ADV_STARTED) 
		&& IS_SET(obj->spectype, ADV_AFFECT)
		&& !IS_SET(obj->spectype, ADV_FINISHED))
	    {
		if ( arg2[0] == '\0' )
		    SET_BIT(obj->spectype, ADV_FAILED);
		else if ((!str_cmp(arg2,"blind") || !str_cmp(arg2,"blindness")) && 
		    !IS_SET(obj->value[3], AFF_BLIND))
		    SET_BIT(obj->value[3], AFF_BLIND);
		else if ((!str_cmp(arg2,"invis") || !str_cmp(arg2,"invisible")
		    || !str_cmp(arg2,"invisibility")) && 
		    !IS_SET(obj->value[3], AFF_BLIND))
		    SET_BIT(obj->value[3], AFF_BLIND);
		else if (!str_cmp(arg2,"detect.evil") && 
		    !IS_SET(obj->value[3], AFF_DETECT_EVIL))
		    SET_BIT(obj->value[3], AFF_DETECT_EVIL);
		else if ((!str_cmp(arg2,"detect.invis") || !str_cmp(arg2,"detect.invisible")
		    || !str_cmp(arg2,"detect.invisibility")) && 
		    !IS_SET(obj->value[3], AFF_DETECT_INVIS))
		    SET_BIT(obj->value[3], AFF_DETECT_INVIS);
		else if (!str_cmp(arg2,"detect.magic") && 
		    !IS_SET(obj->value[3], AFF_DETECT_MAGIC))
		    SET_BIT(obj->value[3], AFF_DETECT_MAGIC);
		else if (!str_cmp(arg2,"detect.hidden") && 
		    !IS_SET(obj->value[3], AFF_DETECT_HIDDEN))
		    SET_BIT(obj->value[3], AFF_DETECT_HIDDEN);
		else if ((!str_cmp(arg2,"shadowplane") || 
		    !str_cmp(arg2,"shadow.plane")) && 
		    !IS_SET(obj->value[3], AFF_SHADOWPLANE))
		    SET_BIT(obj->value[3], AFF_SHADOWPLANE);
		else if ((!str_cmp(arg2,"sanct") || 
		    !str_cmp(arg2,"sanctuary")) && 
		    !IS_SET(obj->value[3], AFF_SANCTUARY))
		    SET_BIT(obj->value[3], AFF_SANCTUARY);
		else if (!str_cmp(arg2,"faerie.fire") && 
		    !IS_SET(obj->value[3], AFF_FAERIE_FIRE))
		    SET_BIT(obj->value[3], AFF_FAERIE_FIRE);
		else if ((!str_cmp(arg2,"infravision") || 
		    !str_cmp(arg2,"infrared") || !str_cmp(arg2,"infra")) && 
		    !IS_SET(obj->value[3], AFF_SANCTUARY))
		    SET_BIT(obj->value[3], AFF_SANCTUARY);
		else if (!str_cmp(arg2,"curse") && 
		    !IS_SET(obj->value[3], AFF_CURSE))
		    SET_BIT(obj->value[3], AFF_CURSE);
		else if ((!str_cmp(arg2,"flaming") || !str_cmp(arg2,"burning"))
		    && !IS_SET(obj->value[3], AFF_FLAMING))
		    SET_BIT(obj->value[3], AFF_FLAMING);
		else if (!str_cmp(arg2,"poison") && 
		    !IS_SET(obj->value[3], AFF_POISON))
		    SET_BIT(obj->value[3], AFF_POISON);
		else if ((!str_cmp(arg2,"protect") || !str_cmp(arg2,"protection"))
		    && !IS_SET(obj->value[3], AFF_PROTECT))
		    SET_BIT(obj->value[3], AFF_PROTECT);
		else if (!str_cmp(arg2,"ethereal") && 
		    !IS_SET(obj->value[3], AFF_ETHEREAL))
		    SET_BIT(obj->value[3], AFF_ETHEREAL);
		else if (!str_cmp(arg2,"sneak") && 
		    !IS_SET(obj->value[3], AFF_SNEAK))
		    SET_BIT(obj->value[3], AFF_SNEAK);
		else if (!str_cmp(arg2,"hide") && 
		    !IS_SET(obj->value[3], AFF_HIDE))
		    SET_BIT(obj->value[3], AFF_HIDE);
		else if (!str_cmp(arg2,"sleep") && 
		    !IS_SET(obj->value[3], AFF_SLEEP))
		    SET_BIT(obj->value[3], AFF_SLEEP);/*
		else if (!str_cmp(arg2,"charm") && 
		    !IS_SET(obj->value[3], AFF_CHARM))
		    SET_BIT(obj->value[3], AFF_CHARM);*/
		else if ((!str_cmp(arg2,"fly") || !str_cmp(arg2,"flying"))
		    && !IS_SET(obj->value[3], AFF_FLYING))
		    SET_BIT(obj->value[3], AFF_FLYING);
		else if ((!str_cmp(arg2,"passdoor") || !str_cmp(arg2,"pass.door"))
		    && !IS_SET(obj->value[3], AFF_PASS_DOOR))
		    SET_BIT(obj->value[3], AFF_PASS_DOOR);
		else if ((!str_cmp(arg2,"shadowsight") || 
		    !str_cmp(arg2,"shadow.sight")) && 
		    !IS_SET(obj->value[3], AFF_SHADOWSIGHT))
		    SET_BIT(obj->value[3], AFF_SHADOWSIGHT);
		else if ((!str_cmp(arg2,"web") || !str_cmp(arg2,"webbed"))
		    && !IS_SET(obj->value[3], AFF_WEBBED))
		    SET_BIT(obj->value[3], AFF_WEBBED);
/*
		else if (!str_cmp(arg2,"contraception") && 
		    !IS_SET(obj->value[3], AFF_CONTRACEPTION))
		    SET_BIT(obj->value[3], AFF_CONTRACEPTION);
*/
		else {SET_BIT(obj->spectype, ADV_FAILED);return;}
		obj->points += 25;
	    }
	    else if (!str_cmp(arg1,"bonus:")
		&& IS_SET(obj->spectype, ADV_STARTED) 
		&& IS_SET(obj->spectype, ADV_AFFECT)
		&& !IS_SET(obj->spectype, ADV_FINISHED))
	    {
		if ( arg2[0] == '\0' || !is_number(arg2) || 
		    atoi(arg2) < 0 || atoi(arg2) > 100)
		    SET_BIT(obj->spectype, ADV_FAILED);
		else {obj->value[2] = atoi(arg2);obj->points += (atoi(arg2)*15);}
	    }
	    else if (!str_cmp(arg1,"duration:")
		&& IS_SET(obj->spectype, ADV_STARTED) 
		&& IS_SET(obj->spectype, ADV_AFFECT)
		&& obj->level == 0
		&& !IS_SET(obj->spectype, ADV_FINISHED))
	    {
		if ( arg2[0] == '\0' || !is_number(arg2) || 
		    atoi(arg2) < 1 || atoi(arg2) > 60)
		    SET_BIT(obj->spectype, ADV_FAILED);
		else {obj->level = atoi(arg2);obj->points += (atoi(arg2)*10);}
	    }
	    else if ((!str_cmp(arg1,"message.one:") || !str_cmp(arg1,"message.1:")) 
		&& IS_SET(obj->spectype, ADV_STARTED) 
		&& !IS_SET(obj->spectype, ADV_MESSAGE_1)
		&& !IS_SET(obj->spectype, ADV_FINISHED))
	    {
		if ( arg2[0] == '\0' )
		    SET_BIT(obj->spectype, ADV_FAILED);
		else
		{
		    SET_BIT(obj->spectype, ADV_MESSAGE_1);
		    free_string(obj->chpoweroff);
		    obj->chpoweroff = str_dup(arg2);
		}
	    }
	    else if ((!str_cmp(arg1,"message.two:") || !str_cmp(arg1,"message.2:")) 
		&& IS_SET(obj->spectype, ADV_STARTED) 
		&& !IS_SET(obj->spectype, ADV_MESSAGE_2)
		&& !IS_SET(obj->spectype, ADV_FINISHED))
	    {
		if ( arg2[0] == '\0' )
		    SET_BIT(obj->spectype, ADV_FAILED);
		else
		{
		    SET_BIT(obj->spectype, ADV_MESSAGE_2);
		    free_string(obj->victpoweron);
		    obj->victpoweron = str_dup(arg2);
		}
	    }
	    else if ((!str_cmp(arg1,"message.three:") || !str_cmp(arg1,"message.3:")) 
		&& IS_SET(obj->spectype, ADV_STARTED) 
		&& !IS_SET(obj->spectype, ADV_MESSAGE_3)
		&& !IS_SET(obj->spectype, ADV_FINISHED))
	    {
		if ( arg2[0] == '\0' )
		    SET_BIT(obj->spectype, ADV_FAILED);
		else
		{
		    SET_BIT(obj->spectype, ADV_MESSAGE_3);
		    free_string(obj->victpoweroff);
		    obj->victpoweroff = str_dup(arg2);
		}
	    }
	    else if (!IS_SET(obj->spectype, ADV_FINISHED))
		SET_BIT(obj->spectype, ADV_FAILED);
	}
    }
    else return;
    act("$n writes something on $p.",ch,obj,NULL,TO_ROOM);
    send_to_char("Ok.\n\r",ch);
    return;
}

void show_page( CHAR_DATA *ch, OBJ_DATA *book, int pnum, bool pagefalse )
{
    OBJ_DATA *page;
    OBJ_DATA *page_next;
    char buf[MAX_STRING_LENGTH];
    bool found = FALSE;

    for ( page = book->contains; page != NULL; page = page_next )
    {
	page_next = page->next_content;
	if (page->value[0] == pnum)
	{
	    found = TRUE;
	    if (!str_cmp(page->victpoweruse,"(null)"))
		sprintf(buf,"Untitled page.\n\r");
	    else
		sprintf(buf,"%s.\n\r",page->victpoweruse);
	    buf[0] = UPPER(buf[0]);
	    send_to_char(buf,ch);
	    if (!pagefalse)
	    {
		if (page->chpoweruse == NULL || page->chpoweruse == '\0' ||
			!str_cmp(page->chpoweruse,"(null)"))
		{
		    if (!are_runes(page))
			send_to_char("This page is blank.\n\r",ch);
		    else if (IS_AFFECTED(ch, AFF_DETECT_MAGIC) && 
			!IS_SET(page->quest, QUEST_MASTER_RUNE) &&
			!IS_SET(page->spectype, ADV_STARTED))
			show_runes(ch, page, FALSE);
		    else
			send_to_char("This page is blank.\n\r",ch);
		    return;
		}
		send_to_char("--------------------------------------------------------------------------------\n\r",ch);
		send_to_char(page->chpoweruse,ch);
		send_to_char("\n\r--------------------------------------------------------------------------------\n\r",ch);
		if (IS_AFFECTED(ch, AFF_DETECT_MAGIC) && 
		    !IS_SET(page->quest, QUEST_MASTER_RUNE) && 
		    !IS_SET(page->spectype, ADV_STARTED))
		    show_runes(ch, page, TRUE);
	    }
	}
    }
    if (!found)
	send_to_char("This page has been torn out.\n\r",ch);
    return;
}

void show_runes( CHAR_DATA *ch, OBJ_DATA *page, bool endline )
{
    if (page->value[1] + page->value[2] + page->value[3] < 1) return;
    send_to_char("This page contains the following symbols:\n\r",ch);
    send_to_char("Runes:",ch);
    if (page->value[1] > 0)
    {
	if (IS_SET(page->value[1], RUNE_FIRE  )) send_to_char(" Fire",  ch);
	if (IS_SET(page->value[1], RUNE_AIR   )) send_to_char(" Air",   ch);
	if (IS_SET(page->value[1], RUNE_EARTH )) send_to_char(" Earth", ch);
	if (IS_SET(page->value[1], RUNE_WATER )) send_to_char(" Water", ch);
	if (IS_SET(page->value[1], RUNE_DARK  )) send_to_char(" Dark",  ch);
	if (IS_SET(page->value[1], RUNE_LIGHT )) send_to_char(" Light", ch);
	if (IS_SET(page->value[1], RUNE_LIFE  )) send_to_char(" Life",  ch);
	if (IS_SET(page->value[1], RUNE_DEATH )) send_to_char(" Death", ch);
	if (IS_SET(page->value[1], RUNE_MIND  )) send_to_char(" Mind",  ch);
	if (IS_SET(page->value[1], RUNE_SPIRIT)) send_to_char(" Spirit",ch);
    }
    else send_to_char(" None",ch);
    send_to_char(".\n\r",ch);
    send_to_char("Glyphs:",ch);
    if (page->value[2] > 0)
    {
	if (IS_SET(page->value[2], GLYPH_CREATION)) send_to_char(" Creation", ch);
	if (IS_SET(page->value[2], GLYPH_DESTRUCTION)) send_to_char(" Destruction",  ch);
	if (IS_SET(page->value[2], GLYPH_SUMMONING)) send_to_char(" Summoning",ch);
	if (IS_SET(page->value[2], GLYPH_TRANSFORMATION)) send_to_char(" Transformation",ch);
	if (IS_SET(page->value[2], GLYPH_TRANSPORTATION)) send_to_char(" Transportation", ch);
	if (IS_SET(page->value[2], GLYPH_ENHANCEMENT)) send_to_char(" Enhancement",ch);
	if (IS_SET(page->value[2], GLYPH_REDUCTION )) send_to_char(" Reduction", ch);
	if (IS_SET(page->value[2], GLYPH_CONTROL)) send_to_char(" Control",ch);
	if (IS_SET(page->value[2], GLYPH_PROTECTION)) send_to_char(" Protection",ch);
	if (IS_SET(page->value[2], GLYPH_INFORMATION)) send_to_char(" Information",ch);
    }
    else send_to_char(" None",ch);
    send_to_char(".\n\r",ch);
    send_to_char("Sigils:",ch);
    if (page->value[3] > 0)
    {
	if (IS_SET(page->value[3], SIGIL_SELF      )) send_to_char(" Self", ch);
	if (IS_SET(page->value[3], SIGIL_TARGETING )) send_to_char(" Targeting", ch);
	if (IS_SET(page->value[3], SIGIL_AREA      )) send_to_char(" Area", ch);
	if (IS_SET(page->value[3], SIGIL_OBJECT    )) send_to_char(" Object", ch);
    }
    else send_to_char(" None",ch);
    send_to_char(".\n\r",ch);
    if (endline) send_to_char("--------------------------------------------------------------------------------\n\r",ch);
    return;
}

bool are_runes( OBJ_DATA *page )
{
    if (page->value[1] + page->value[2] + page->value[3] < 1) return FALSE;
    return TRUE;
}

bool has_key( CHAR_DATA *ch, int key )
{
    OBJ_DATA *obj;

    for ( obj = ch->carrying; obj != NULL; obj = obj->next_content )
    {
	if ( obj->pIndexData->vnum == key )
	    return TRUE;
    }

    return FALSE;
}



void do_lock( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    OBJ_DATA *obj;
    int door;

    one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
	send_to_char( "Lock what?\n\r", ch );
	return;
    }

    if ( ( obj = get_obj_here( ch, arg ) ) != NULL )
    {
	/* 'lock object' */
	if ( obj->item_type != ITEM_CONTAINER )
	    { send_to_char( "That's not a container.\n\r", ch ); return; }
	if ( !IS_SET(obj->value[1], CONT_CLOSED) )
	    { send_to_char( "It's not closed.\n\r",        ch ); return; }
	if ( obj->value[2] < 0 )
	    { send_to_char( "It can't be locked.\n\r",     ch ); return; }
	if ( !has_key( ch, obj->value[2] ) )
	    { send_to_char( "You lack the key.\n\r",       ch ); return; }
	if ( IS_SET(obj->value[1], CONT_LOCKED) )
	    { send_to_char( "It's already locked.\n\r",    ch ); return; }

	SET_BIT(obj->value[1], CONT_LOCKED);
	send_to_char( "*Click*\n\r", ch );
	act( "$n locks $p.", ch, obj, NULL, TO_ROOM );
	return;
    }

    if ( ( door = find_door( ch, arg ) ) >= 0 )
    {
	/* 'lock door' */
	ROOM_INDEX_DATA *to_room;
	EXIT_DATA *pexit;
	EXIT_DATA *pexit_rev;

	pexit	= ch->in_room->exit[door];
	if ( !IS_SET(pexit->exit_info, EX_CLOSED) )
	    { send_to_char( "It's not closed.\n\r",        ch ); return; }
	if ( pexit->key < 0 )
	    { send_to_char( "It can't be locked.\n\r",     ch ); return; }
	if ( !has_key( ch, pexit->key) )
	    { send_to_char( "You lack the key.\n\r",       ch ); return; }
	if ( IS_SET(pexit->exit_info, EX_LOCKED) )
	    { send_to_char( "It's already locked.\n\r",    ch ); return; }

	SET_BIT(pexit->exit_info, EX_LOCKED);
	send_to_char( "*Click*\n\r", ch );
	act( "$n locks the $d.", ch, NULL, pexit->keyword, TO_ROOM );

	/* lock the other side */
	if ( ( to_room   = pexit->to_room               ) != NULL
	&&   ( pexit_rev = to_room->exit[rev_dir[door]] ) != 0
	&&   pexit_rev->to_room == ch->in_room )
	{
	    SET_BIT( pexit_rev->exit_info, EX_LOCKED );
	}
    }

    return;
}



void do_unlock( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    OBJ_DATA *obj;
    int door;

    one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
	send_to_char( "Unlock what?\n\r", ch );
	return;
    }

    if ( ( obj = get_obj_here( ch, arg ) ) != NULL )
    {
	/* 'unlock object' */
	if ( obj->item_type != ITEM_CONTAINER )
	    { send_to_char( "That's not a container.\n\r", ch ); return; }
	if ( !IS_SET(obj->value[1], CONT_CLOSED) )
	    { send_to_char( "It's not closed.\n\r",        ch ); return; }
	if ( obj->value[2] < 0 )
	    { send_to_char( "It can't be unlocked.\n\r",   ch ); return; }
	if ( !has_key( ch, obj->value[2] ) )
	    { send_to_char( "You lack the key.\n\r",       ch ); return; }
	if ( !IS_SET(obj->value[1], CONT_LOCKED) )
	    { send_to_char( "It's already unlocked.\n\r",  ch ); return; }

	REMOVE_BIT(obj->value[1], CONT_LOCKED);
	send_to_char( "*Click*\n\r", ch );
	act( "$n unlocks $p.", ch, obj, NULL, TO_ROOM );
	return;
    }

    if ( ( door = find_door( ch, arg ) ) >= 0 )
    {
	/* 'unlock door' */
	ROOM_INDEX_DATA *to_room;
	EXIT_DATA *pexit;
	EXIT_DATA *pexit_rev;

	pexit = ch->in_room->exit[door];
	if ( !IS_SET(pexit->exit_info, EX_CLOSED) )
	    { send_to_char( "It's not closed.\n\r",        ch ); return; }
	if ( pexit->key < 0 )
	    { send_to_char( "It can't be unlocked.\n\r",   ch ); return; }
	if ( !has_key( ch, pexit->key) )
	    { send_to_char( "You lack the key.\n\r",       ch ); return; }
	if ( !IS_SET(pexit->exit_info, EX_LOCKED) )
	    { send_to_char( "It's already unlocked.\n\r",  ch ); return; }

	REMOVE_BIT(pexit->exit_info, EX_LOCKED);
	send_to_char( "*Click*\n\r", ch );
	act( "$n unlocks the $d.", ch, NULL, pexit->keyword, TO_ROOM );

	/* unlock the other side */
	if ( ( to_room   = pexit->to_room               ) != NULL
	&&   ( pexit_rev = to_room->exit[rev_dir[door]] ) != NULL
	&&   pexit_rev->to_room == ch->in_room )
	{
	    REMOVE_BIT( pexit_rev->exit_info, EX_LOCKED );
	}
    }

    return;
}



void do_pick( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    CHAR_DATA *gch;
    OBJ_DATA *obj;
    int door;

    one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
	send_to_char( "Pick what?\n\r", ch );
	return;
    }

    WAIT_STATE( ch, skill_table[gsn_pick_lock].beats );

    /* look for guards */
    for ( gch = ch->in_room->people; gch; gch = gch->next_in_room )
    {
	if ( IS_NPC(gch) && IS_AWAKE(gch) && ch->level + 5 < gch->level )
	{
	    act( "$N is standing too close to the lock.",
		ch, NULL, gch, TO_CHAR );
	    return;
	}
    }

    if ( !IS_NPC(ch) && number_percent( ) > ch->pcdata->learned[gsn_pick_lock] )
    {
	send_to_char( "You failed.\n\r", ch);
	return;
    }

    if ( ( obj = get_obj_here( ch, arg ) ) != NULL )
    {
	/* 'pick object' */
	if ( obj->item_type != ITEM_CONTAINER )
	    { send_to_char( "That's not a container.\n\r", ch ); return; }
	if ( !IS_SET(obj->value[1], CONT_CLOSED) )
	    { send_to_char( "It's not closed.\n\r",        ch ); return; }
	if ( obj->value[2] < 0 )
	    { send_to_char( "It can't be unlocked.\n\r",   ch ); return; }
	if ( !IS_SET(obj->value[1], CONT_LOCKED) )
	    { send_to_char( "It's already unlocked.\n\r",  ch ); return; }
	if ( IS_SET(obj->value[1], CONT_PICKPROOF) )
	    { send_to_char( "You failed.\n\r",             ch ); return; }

	REMOVE_BIT(obj->value[1], CONT_LOCKED);
	send_to_char( "*Click*\n\r", ch );
	act( "$n picks $p.", ch, obj, NULL, TO_ROOM );
	return;
    }

    if ( ( door = find_door( ch, arg ) ) >= 0 )
    {
	/* 'pick door' */
	ROOM_INDEX_DATA *to_room;
	EXIT_DATA *pexit;
	EXIT_DATA *pexit_rev;

	pexit = ch->in_room->exit[door];
	if ( !IS_SET(pexit->exit_info, EX_CLOSED) )
	    { send_to_char( "It's not closed.\n\r",        ch ); return; }
	if ( pexit->key < 0 )
	    { send_to_char( "It can't be picked.\n\r",     ch ); return; }
	if ( !IS_SET(pexit->exit_info, EX_LOCKED) )
	    { send_to_char( "It's already unlocked.\n\r",  ch ); return; }
	if ( IS_SET(pexit->exit_info, EX_PICKPROOF) )
	    { send_to_char( "You failed.\n\r",             ch ); return; }

	REMOVE_BIT(pexit->exit_info, EX_LOCKED);
	send_to_char( "*Click*\n\r", ch );
	act( "$n picks the $d.", ch, NULL, pexit->keyword, TO_ROOM );

	/* pick the other side */
	if ( ( to_room   = pexit->to_room               ) != NULL
	&&   ( pexit_rev = to_room->exit[rev_dir[door]] ) != NULL
	&&   pexit_rev->to_room == ch->in_room )
	{
	    REMOVE_BIT( pexit_rev->exit_info, EX_LOCKED );
	}
    }

    return;
}




void do_stand( CHAR_DATA *ch, char *argument )
{
    switch ( ch->position )
    {
    case POS_SLEEPING:
	if ( IS_AFFECTED(ch, AFF_SLEEP) )
	    { send_to_char( "You can't wake up!\n\r", ch ); return; }

	send_to_char( "You wake and stand up.\n\r", ch );
	act( "$n wakes and stands up.", ch, NULL, NULL, TO_ROOM );
	ch->position = POS_STANDING;
	break;

    case POS_RESTING:
    case POS_SITTING:
	send_to_char( "You stand up.\n\r", ch );
	act( "$n stands up.", ch, NULL, NULL, TO_ROOM );
	ch->position = POS_STANDING;
	break;

    case POS_MEDITATING:
	send_to_char( "You uncross your legs and stand up.\n\r", ch );
	act( "$n uncrosses $s legs and stands up.", ch, NULL, NULL, TO_ROOM );
	ch->position = POS_STANDING;
	break;

    case POS_STANDING:
	send_to_char( "You are already standing.\n\r", ch );
	break;

    case POS_FIGHTING:
	send_to_char( "You are already fighting!\n\r", ch );
	break;
    }

    return;
}



void do_rest( CHAR_DATA *ch, char *argument )
{
    switch ( ch->position )
    {
    case POS_SLEEPING:
	send_to_char( "You are already sleeping.\n\r", ch );
	break;

    case POS_RESTING:
	send_to_char( "You are already resting.\n\r", ch );
	break;

    case POS_MEDITATING:
    case POS_SITTING:
    case POS_STANDING:
	send_to_char( "You rest.\n\r", ch );
	act( "$n rests.", ch, NULL, NULL, TO_ROOM );
	ch->position = POS_RESTING;
	break;

    case POS_FIGHTING:
	send_to_char( "You are already fighting!\n\r", ch );
	break;
    }

    return;
}



void do_sit( CHAR_DATA *ch, char *argument )
{
    switch ( ch->position )
    {
    case POS_SLEEPING:
	send_to_char( "You are already sleeping.\n\r", ch );
	break;

    case POS_RESTING:
	send_to_char( "You are already resting.\n\r", ch );
	break;

    case POS_MEDITATING:
	send_to_char( "You are already meditating.\n\r", ch );
	break;

    case POS_SITTING:
	send_to_char( "You are already sitting.\n\r", ch );
	break;

    case POS_STANDING:
	send_to_char( "You sit down.\n\r", ch );
	act( "$n sits down.", ch, NULL, NULL, TO_ROOM );
	ch->position = POS_SITTING;
	break;

    case POS_FIGHTING:
	send_to_char( "You are already fighting!\n\r", ch );
	break;
    }

    return;
}



void do_meditate( CHAR_DATA *ch, char *argument )
{
    if (IS_NPC(ch) || (!IS_CLASS(ch, CLASS_MAGE) && !IS_CLASS(ch, CLASS_MONK) 
&& !IS_CLASS(ch, CLASS_NINJA) && !IS_CLASS(ch, CLASS_DROW)))
    {
	send_to_char("You are unable to attain the correct state of mind.\n\r",ch);
	return;
    }

    switch ( ch->position )
    {
    case POS_SLEEPING:
	send_to_char( "You are already sleeping.\n\r", ch );
	break;

    case POS_RESTING:
	send_to_char( "You are already resting.\n\r", ch );
	break;

    case POS_MEDITATING:
	send_to_char( "You are already meditating.\n\r", ch );
	break;

    case POS_SITTING:
	send_to_char( "You cross your legs and start meditating.\n\r", ch );
	act( "$n crosses $s legs and starts meditating.", ch, NULL, NULL, TO_ROOM );
	ch->position = POS_MEDITATING;
	break;

    case POS_STANDING:
	send_to_char( "You sit down, cross your legs and start meditating.\n\r", ch );
	act( "$n sits down, crosses $s legs and starts meditating.", ch, NULL, NULL, TO_ROOM );
	ch->position = POS_MEDITATING;
	break;

    case POS_FIGHTING:
	send_to_char( "You are already fighting!\n\r", ch );
	break;
    }

    return;
}

void do_healing( CHAR_DATA *ch, char *argument )
{
    if (IS_NPC(ch) || (!IS_CLASS(ch, CLASS_VAMPIRE) && ch->power[DISC_VAMP_PROT] < 8))
    {
	send_to_char("You are unable to assume a state of healing.\n\r",ch);
	return;
    }

    switch ( ch->position )
    {
    case POS_SLEEPING:
	send_to_char( "You are already sleeping.\n\r", ch );
	break;

    case POS_RESTING:
	send_to_char( "You are already resting.\n\r", ch );
	break;

    case POS_MEDITATING:
	send_to_char( "You are already in a healing position.\n\r", ch );
	break;

    case POS_SITTING:
	send_to_char( "You close your eyes and become completely relaxed.\n\r", ch );
	act( "$n closes $m eyes and looks completely relaxed.", ch, NULL, NULL, TO_ROOM );
	ch->position = POS_MEDITATING;
	break;

    case POS_STANDING:
	send_to_char( "You close your eyes and become completely relaxed.\n\r", ch );
	act( "$n closes $m eyes and looks completely relaxed.", ch, NULL, NULL, TO_ROOM );
	ch->position = POS_MEDITATING;
	break;

    case POS_FIGHTING:
	send_to_char( "You are already fighting!\n\r", ch );
	break;
    }

    return;
}

void do_sleep( CHAR_DATA *ch, char *argument )
{
    switch ( ch->position )
    {
    case POS_SLEEPING:
	send_to_char( "You are already sleeping.\n\r", ch );
	break;

    case POS_SITTING:
    case POS_MEDITATING:
    case POS_RESTING:
    case POS_STANDING: 
	send_to_char( "You sleep.\n\r", ch );
	act( "$n sleeps.", ch, NULL, NULL, TO_ROOM );
	ch->position = POS_SLEEPING;
	break;

    case POS_FIGHTING:
	send_to_char( "You are already fighting!\n\r", ch );
	break;
    }

    return;
}



void do_wake( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;

    one_argument( argument, arg );
    if ( arg[0] == '\0' )
	{ do_stand( ch, argument ); return; }

    if ( !IS_AWAKE(ch) )
	{ send_to_char( "You are asleep yourself!\n\r",       ch ); return; }

    if ( ( victim = get_char_room( ch, arg ) ) == NULL )
	{ send_to_char( "They aren't here.\n\r",              ch ); return; }

    if ( IS_AWAKE(victim) )
	{ act( "$N is already awake.", ch, NULL, victim, TO_CHAR ); return; }

    if ( IS_AFFECTED(victim, AFF_SLEEP) )
	{ act( "You can't wake $M!",   ch, NULL, victim, TO_CHAR );  return; }

    if (victim->position < POS_SLEEPING)
	{ act( "$E doesn't respond!",   ch, NULL, victim, TO_CHAR );  return; }

    act( "You wake $M.", ch, NULL, victim, TO_CHAR );
    act( "$n wakes you.", ch, NULL, victim, TO_VICT );
    victim->position = POS_STANDING;
    return;
}



void do_sneak( CHAR_DATA *ch, char *argument )
{
    AFFECT_DATA af;

    send_to_char( "You attempt to move silently.\n\r", ch );
    affect_strip( ch, gsn_sneak );

    if ( IS_NPC(ch) || number_percent( ) < ch->pcdata->learned[gsn_sneak] )
    {
	af.type      = gsn_sneak;
	af.duration  = ch->level;
	af.location  = APPLY_NONE;
	af.modifier  = 0;
	af.bitvector = AFF_SNEAK;
	affect_to_char( ch, &af );
    }

    return;
}



void do_hide( CHAR_DATA *ch, char *argument )
{

	if (!IS_NPC(ch) && (IS_CLASS(ch, CLASS_MONK) || IS_CLASS(ch,
		CLASS_NINJA))) {
	send_to_char("You cannot use this power.\n\r", ch );
	return;}

    send_to_char( "You attempt to hide.\n\r", ch );

    if ( IS_AFFECTED(ch, AFF_HIDE) )
	REMOVE_BIT(ch->affected_by, AFF_HIDE);

    if ( IS_NPC(ch) || number_percent( ) < ch->pcdata->learned[gsn_hide] )
	SET_BIT(ch->affected_by, AFF_HIDE);

    return;
}



/*
 * Contributed by Alander.
 */
void do_visible( CHAR_DATA *ch, char *argument )
{
    affect_strip ( ch, gsn_invis			);
    affect_strip ( ch, gsn_mass_invis			);
    affect_strip ( ch, gsn_sneak			);
    REMOVE_BIT   ( ch->affected_by, AFF_HIDE		);
    REMOVE_BIT   ( ch->affected_by, AFF_INVISIBLE	);
    REMOVE_BIT   ( ch->affected_by, AFF_SNEAK		);
    send_to_char( "Ok.\n\r", ch );
    return;
}



void do_unpolymorph( CHAR_DATA *ch, char *argument )
{
    if (!is_affected(ch, gsn_polymorph) )
    {
	send_to_char("But you are not polymorphed!\n\r",ch);
	return;
    }
    act("$n's body begins to distort.",ch,NULL,NULL,TO_ROOM);
    affect_strip ( ch, gsn_polymorph );
    act("$n resumes $s normal form.",ch,NULL,NULL,TO_ROOM);
    send_to_char( "You resume your normal form.\n\r", ch );
    return;
}



void do_recall( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_STRING_LENGTH];
    CHAR_DATA *victim;
    CHAR_DATA *mount;
    ROOM_INDEX_DATA *location;
  
    act( "$n's body flickers with green energy.", ch, NULL, NULL, TO_ROOM );
    act( "Your body flickers with green energy.", ch, NULL, NULL, TO_CHAR );

    if ( ch->position == POS_FIGHTING)
    {
     send_to_char( "You can't recall while fighting.\n\r",ch);
     return;
    }

    if ( ( location = get_room_index( ch->home ) ) == NULL )
    {
	send_to_char( "You are completely lost.\n\r", ch );
	return;
    }
/*
    if ( IS_AFFECTED(ch, AFF_DARKNESS))
	{
	REMOVE_BIT(ch->affected_by, AFF_DARKNESS);
	return;
	}
*/
    if ( ch->in_room == location )
	return;

    if ( IS_SET(ch->in_room->room_flags, ROOM_NO_RECALL)
    ||   IS_AFFECTED(ch, AFF_CURSE) )
    {
	send_to_char( "You are unable to recall.\n\r", ch );
	return;
    }
    
    if ( IS_SET(ch->flag2, AFF_TOTALBLIND) )
    {
	stc("You are unable to recall.\n\r",ch);
	return;
    }
/*
    if ( ( victim = ch->fighting ) != NULL )
    {
	if ( number_bits( 1 ) == 0 )
	{
	    WAIT_STATE( ch, 4 );
	    sprintf( buf, "You failed!\n\r" );
	    send_to_char( buf, ch );
	    return;
	}
	sprintf( buf, "You recall from combat!\n\r");
	send_to_char( buf, ch );
	stop_fighting( ch, TRUE );
    }*/

    act( "$n disappears.", ch, NULL, NULL, TO_ROOM );
    char_from_room( ch );
    char_to_room( ch, location );
    act( "$n appears in the room.", ch, NULL, NULL, TO_ROOM );
    do_look( ch, "auto" );
    if ( ( mount = ch->mount ) == NULL ) return;
    char_from_room( mount );
    char_to_room( mount, ch->in_room );
    return;
}

void do_home( CHAR_DATA *ch, char *argument )
{
    char arg [MAX_INPUT_LENGTH];

    argument = one_argument( argument, arg );

    if ( IS_NPC(ch) ) return;

    if ( arg[0] == '\0' || str_cmp(arg,"here") )
    {
	send_to_char( "If you wish this to be your room, you must type 'home here'.\n\r", ch );
	return;
    }

    if ( ch->in_room->vnum == ch->home )
    {
	send_to_char( "But this is already your home!\n\r", ch );
	return;
    }

    if ( IS_SET(ch->in_room->room_flags, ROOM_NO_RECALL) ||
	 IS_SET(ch->in_room->room_flags, ROOM_SAFE) )
    {
	send_to_char( "You are unable to make this room your home.\n\r", ch );
	return;
    }

    ch->home = ch->in_room->vnum;
    send_to_char( "This room is now your home.\n\r", ch );

    return;
}


void do_escape( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_STRING_LENGTH];
    ROOM_INDEX_DATA *location;

    if (IS_NPC(ch) || !IS_HERO(ch))
	return;
  
    if ( ch->position >= POS_SLEEPING )
    {
	send_to_char( "You can only do this if you are dying.\n\r", ch );
	return;
    }

    if ( ( location = get_room_index( ROOM_VNUM_TEMPLE ) ) == NULL )
    {
	send_to_char( "You are completely lost.\n\r", ch );
	return;
    }

    if ( ch->in_room == location )
	return;
    ch->move = 0;
    ch->mana = 0;
    act( "$n fades out of existance.", ch, NULL, NULL, TO_ROOM );
    char_from_room( ch );
    char_to_room( ch, location );
    act( "$n fades into existance.", ch, NULL, NULL, TO_ROOM );
    do_look( ch, "auto" );
    sprintf(buf,"%s has escaped defenceless from a fight.",ch->pcdata->switchname);
    do_info(ch,buf);
    if (IS_SET(ch->newbits, NEW_DARKNESS)) {
        REMOVE_BIT(ch->newbits, NEW_DARKNESS);
        if (ch->in_room != NULL)
        if (IS_SET(ch->in_room->room_flags, ROOM_TOTAL_DARKNESS))
        REMOVE_BIT(ch->in_room->room_flags, ROOM_TOTAL_DARKNESS);
        return;
        }
    return;
}


int disc_points_needed(CHAR_DATA *ch)
{
    
    if (IS_NPC(ch)) return 1;

    return (ch->power[ch->pcdata->disc_research] + 1) * 10;
}

void gain_disc_points(CHAR_DATA *ch, int points)
{   
    if (IS_NPC(ch)) return;

    if (ch->pcdata->disc_points == 666) return;
    if (ch->pcdata->disc_research == -1) return;

    ch->pcdata->disc_points += points;

    if (ch->pcdata->disc_points < 0)
    {
	ch->pcdata->disc_points = 0;
	return;
    }

    if (ch->pcdata->disc_points >= 
	    disc_points_needed(ch) )
    {
	ch->pcdata->disc_points = 666;
	WAIT_STATE(ch, 6);
	stc("#4You have finished researching your discipline.\n\r", ch);
	stc("You may now use the 'train' command.#n\n\r", ch);
	return;
    }
}


void do_research(CHAR_DATA *ch, char *argument)
{
    int i;
    int needed;
    int maxlevel;
    char buf[MAX_STRING_LENGTH];

    maxlevel = 0;

/*
     if (IS_CLASS(ch, CLASS_VAMPIRE))
     {
        if (ch->pcdata->rank == AGE_CHILDE || ch->pcdata->rank == AGE_NEONATE)
          maxlevel = 5;
        else if (ch->pcdata->rank == AGE_ANCILLA)
          maxlevel = 7;
        else if (ch->pcdata->rank == AGE_ELDER)
          maxlevel = 9;
        else
          maxlevel = 10;
      }
*/
      if (IS_CLASS(ch, CLASS_DEMON))
      {
	if (ch->generation <= 5)
	  maxlevel = 5;
	else if (ch->generation == 6)
	  maxlevel = 6;
	else if (ch->generation == 7)
	  maxlevel = 7;
	else if (ch->generation == 8)
	  maxlevel = 8;
	else if (ch->generation == 9)
	  maxlevel = 9;
	else if (ch->generation == 10)
	  maxlevel = 10;
	else
	  maxlevel = 10;    
    }

     else maxlevel = 10;

    if (IS_NPC(ch)) return;

    if (!IS_CLASS(ch, CLASS_DEMON))
    {
	stc("Only demons can research disciplines.\n\r", ch);
	return;
    }

    if (!str_cmp(argument, "cancel"))
    {
	if (ch->pcdata->disc_research == -1)
	{
	    stc("You are not currently researching a discipline.\n\r",ch);
	    return;
	}
	stc("You stop your research.\n\r", ch);
	ch->pcdata->disc_research = -1;
	ch->pcdata->disc_points = 0;
 	return;
    }

    if (argument[0] == '\0')
    {
	stc("What do you wish to research.\n\r", ch);
	return;
    }

    if (ch->pcdata->disc_research != -1)
    {
	stc("You are already researching a discipline.\n\r", ch);
	stc("Type 'research cancel' to stop.\n\r",ch);
	return;
    }
 
    for ( i = 0 ; i < MAX_DISCIPLINES ; i++)
    {
	if (discipline[i][0] != '\0'
	    && !str_prefix(argument, discipline[i]) )
	{
		needed = ((ch->power[i] - 5) * 5);
	    if (ch->power[i] < 0)	    
	    {
    		stc("You don't know any disciplines of that name.\n\r", ch);
		return;
	    }

/*
          if ( IS_CLASS(ch, CLASS_VAMPIRE)
	     && ch->power[i] >= maxlevel)
	  {
		stc("You need to age more before progressing in this discipline.\n\r",ch);
	        return;
	  }
*/
	  if ( IS_CLASS(ch, CLASS_DEMON)
	    && ch->power[i] >= maxlevel)
	  {
		stc("You must obtain a higher rank before progressing.\n\r",ch);
		return;
	  }


	  if ( ch->power[i] >= 10 )
	  {
		stc("You have reached full mastery of this discipline.\n\r",ch);
	        return;
	  }

	    sprintf(buf, "You begin your research into %s.\n\r",
		discipline[i]);
	    stc(buf, ch);
	    ch->pcdata->disc_points = 0;
	    ch->pcdata->disc_research = i;
	    return;
	}
    }
    stc("You don't know any disciplines of that name.\n\r", ch);
}

const  char * wwgift [MAX_GIFTS] =
{
	"",
	"Homid",		"Metis",
	"Lupus",		"Ragabash",
	"Theurge",		"Philodox",
	"Galliard",		"Ahroun",
	"Black Furies",		"Bone Gnawers",
	"Children of Gaia",	"Fianna",
	"Get of Fenris",	"Glass Walkers",
	"Red Talons",		"Shadow Lords",
	"Silent Striders",	"Silver Fangs",
	"Stargazers",		"Uktena",
	"Wendigo"
};

const  char * discipline [MAX_DISCIPLINES] =
{
	  "",			"",
        "celerity",		"fortitude",
        "obtenebration",	"presence",
        "quietus",		"thaumaturgy",
        "auspex",		"dominate",
	"obfuscate",	        "potence",
	"protean",	        "serpentis",
	"vicissitude",          "daimoinon",	    
        "animalism",		"",

/* Werewolf */

	"bear",			"lynx",
        "boar",			"owl",
        "spider",		"wolf",
	"hawk",			"mantis",
	"raptor",	        "luna",
	"pain",			"congregation",

/* Daemon */
	"hellfire",		"attack",
	"temptation",		"morphosis",
	"corruption",		"geluge",
	"discord",		"nether",
	"immunae",		"chimerstry",
        "thanatosis",           "obeah",
	"necromancy",    	"melpominee"

};

void do_disciplines(CHAR_DATA *ch, char *argument)    
{
    char buf[MAX_INPUT_LENGTH];
    char buf2[MAX_INPUT_LENGTH];
    int loop;
    int percent = 0, i;
    int indent = 0;

    if (IS_NPC(ch)) return;

    if (!IS_CLASS(ch, CLASS_DEMON))
    {
	stc("Only Demons may improve their disciplines.\n\r",ch);
	return;
    }

//    if (IS_CLASS(ch, CLASS_WEREWOLF))
//        sprintf(buf, "---===[ Guardians ]===---");
//    else
        sprintf(buf, "---===[ Disciplines ]===---");


    cent_to_char(buf, ch);
    send_to_char("\n\r", ch);

    for (loop = 0; loop < MAX_DISCIPLINES; loop++)
    {
	if (discipline[loop][0] != '\0'
	&& strlen(discipline[loop]) != 0
	&& ch->power[loop] >= 0 )
	{
	    sprintf(buf2, discipline[loop]);
	    buf2[0] = UPPER(buf2[0]);

	    sprintf(buf,"     %-14s: %-2d",
		buf2,
		ch->power[loop]);
	    send_to_char(buf,ch);
	    indent++;
	    if (indent == 3)
	    {
		stc("\n\r", ch);
		indent = 0;
	    }
	}
    }
    if (indent != 0) stc("\n\r", ch);

    /* This tells them if they are currently researching */
    if (ch->pcdata->disc_research < 0)
    {
	send_to_char("\n\r", ch);
	divide6_to_char(ch);
	return;
    }

    if (ch->power[ch->pcdata->disc_research] < 0)
    {
	send_to_char("\n\r", ch);
	divide6_to_char(ch);
	return;
    }

    if (ch->pcdata->disc_points == 666)
    {
	stc("\n\r", ch);
	sprintf(buf, "You have finished researching %s.",
	    discipline[ch->pcdata->disc_research]);
	cent_to_char(buf, ch);
	stc("\n\r", ch);
	divide6_to_char(ch);
	return;
    }

    if (ch->pcdata->disc_research < MAX_DISCIPLINES)
    {
	send_to_char("\n\r", ch);
	sprintf(buf, "You are currently researching %s.",
	    discipline[ch->pcdata->disc_research]);
    	cent_to_char(buf, ch);
    }

    percent = ch->pcdata->disc_points * 40 / disc_points_needed(ch);

    sprintf(buf, "-[#1");

    for( i = 0 ; i < percent ; i++)
	strcat(buf, "*");

    for( i = percent ; i < 40 ; i++)
	strcat(buf, " ");

    strcat(buf, "#n]-");

    cent_to_char(buf, ch);
}

void do_train( CHAR_DATA *ch, char *argument )
{
    char arg1[MAX_STRING_LENGTH];
    char arg2[MAX_STRING_LENGTH];
    char buf[MAX_STRING_LENGTH];
    sh_int *pAbility;
    char *pOutput;
    int cost;
    int magic;
    int immcost;
    int primal;
    int silver;
    int gnosis;
    int focus;
    int ancilla;
    int methuselah;
    int elder;
    int beast;
    int max_stat = 18;
    bool last = TRUE;
    bool is_ok = FALSE;

    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );

    if ( IS_NPC(ch) )
	return;

    if ( arg1[0] == '\0' )
    {
	sprintf( buf, "You have %d experience points.\n\r", ch->exp );
	send_to_char( buf, ch );
	strcpy(arg1,"foo");
    }

         if (!str_cmp(arg1,"str")   ) is_ok = TRUE;
    else if (!str_cmp(arg1,"int")   ) is_ok = TRUE;
    else if (!str_cmp(arg1,"wis")   ) is_ok = TRUE;
    else if (!str_cmp(arg1,"dex")   ) is_ok = TRUE;
    else if (!str_cmp(arg1,"con")   ) is_ok = TRUE;
    else if (!str_cmp(arg1,"hp")    ) is_ok = TRUE;
    else if (!str_cmp(arg1,"mana")  ) is_ok = TRUE;
    else if (!str_cmp(arg1,"move")  ) is_ok = TRUE;
    else if (!str_cmp(arg1,"slash") || !str_cmp(arg1,"smash") ||
!str_cmp(arg1,"beast") || !str_cmp(arg1,"grab") || !str_cmp(arg1,"charm") ||
!str_cmp(arg1,"heat") || !str_cmp(arg1,"cold") || !str_cmp(arg1,"lightning") ||
!str_cmp(arg1,"acid") || !str_cmp(arg1,"drain") || !str_cmp(arg1,"hurl") ||
!str_cmp(arg1,"backstab") || !str_cmp(arg1,"kick") || !str_cmp(arg1,"disarm") ||
!str_cmp(arg1,"steal") || !str_cmp(arg1,"stab")) 	is_ok = TRUE;
    else if (!str_cmp(arg1,"primal")) is_ok = TRUE;
	else if (!str_cmp(arg1,"sunlight") && IS_CLASS(ch, CLASS_VAMPIRE))
	is_ok = TRUE;
    else if (!str_cmp(arg1,"ancilla") && IS_CLASS(ch,CLASS_VAMPIRE) && 
    (ch->pcdata->rank == AGE_NEONATE))
        is_ok = TRUE;
    else if (!str_cmp(arg1,"ancilla") && IS_CLASS(ch,CLASS_VAMPIRE) &&
    (ch->pcdata->rank == AGE_CHILDE))
        is_ok = TRUE;
    else if (!str_cmp(arg1,"silver") && IS_CLASS(ch,CLASS_WEREWOLF))
        is_ok = TRUE;
    else if (!str_cmp(arg1,"gnosis") && IS_CLASS(ch, CLASS_WEREWOLF) )
	is_ok = TRUE;
    else if (!str_cmp(arg1,"control") && IS_CLASS(ch, CLASS_VAMPIRE)
	&& ch->beast > 0 )
	is_ok = TRUE;
    else if (!str_cmp(arg1,"focus") && IS_CLASS(ch, CLASS_MONK) )
	is_ok = TRUE;
    else if (!str_cmp(arg1,"elder") && IS_CLASS(ch,CLASS_VAMPIRE) &&
(ch->pcdata->rank == AGE_ANCILLA))
	is_ok = TRUE;
    else if (!str_cmp(arg1,"methuselah") && IS_CLASS(ch,CLASS_VAMPIRE) &&
(ch->pcdata->rank == AGE_ELDER))    
        is_ok = TRUE;
 else if (!str_cmp(arg1,"magic") && IS_CLASS(ch,CLASS_DROW))
	is_ok = TRUE;


/*Vorador put this back in, with the cap. */
	if (!str_cmp(arg1,"mv") || !str_cmp(arg1,"move"))
        {
 	arg2[0] = '\0';
           if (ch->exp < ch->max_move * 50) {
 	     send_to_char("You don't have the experience required to improve movement by 50\r\n", ch);
	     return;
	     } else if (ch->max_move + 50 > 15000) {
		send_to_char("You cannot train movement points over 15000, sorry.\r\n", ch); 
  	        return;
             } 
       else 
       {
            ch->exp-=ch->max_move*50;
		ch->max_move+= 50;
		send_to_char("Ok.\r\n", ch);
 		return;
 	 }
}

	if (!str_cmp(arg1,"mn") || !str_cmp(arg1,"mana"))
        {
arg2[0] = '\0';
   	  if (ch->exp < ch->max_mana * 50) {
            send_to_char("You don't have the experience required to improve mana by 50.\r\n", ch);
            return;
            } else if (ch->max_mana + 50 >15000) {
               send_to_char("You cannot train mana points over 15000, sorry.\r\n", ch);
                return;
		} else {
		  ch->exp-=ch->max_mana*50;
		ch->max_mana+= 50;
                send_to_char("Ok.\r\n", ch);
                return;
	  }
     }
	if (!str_cmp(arg1,"hp") || !str_cmp(arg1,"hit"))
	{
arg2[0] = '\0';
	  if (ch->exp < ch->max_hit * 50)
	  {
	    send_to_char("You dont have the experience required to improve hp by 50.\n\r",ch);
	    return;
	  } else if (ch->max_hit+50 > 15000) {
 		  send_to_char("You cannot train hit points over 15000, sorry.\r\n", ch);
                return;
		 } else {              
		ch->exp-=ch->max_hit*50;
		ch->max_hit+= 50;
 		send_to_char("Ok.\r\n", ch);
		return;
}
	}
if (str_cmp(arg1,"magic")) 
    if ( arg2[0] != '\0' && is_ok )
    {
	int amount = 0;
         
	if (!is_number(arg2))
	{
	    send_to_char("Please enter a numeric value.\n\r",ch);
	    return;
	}
	amount = atoi( arg2 );
	if (amount < 1 || amount > 50)
	{
	    send_to_char("Please enter a value between 1 and 50.\n\r",ch);
	    return;
	}
	if (amount > 1)
	{
	    sprintf(buf,"%s %d",arg1,amount-1);
	    do_train(ch,buf);
	    last = FALSE;
	}
    }

    cost = 200;
    immcost = count_imms(ch);
    primal = (1+ch->practice)*500;
    magic = (1+ch->pcdata->stats[DROW_MAGIC])*100;
    silver = (1+ch->siltol)*2500;
    gnosis = ( 1 + ch->gnosis[GMAXIMUM] ) * 250000;
    if ( ch->beast > 25 )
     beast = 5;
    else if ( ch->beast > 15 )
     beast = 10;
    else if ( ch->beast > 10 )
     beast = 15;
    else if ( ch->beast > 2 )
     beast = 20;
    else
     beast = 25;
    focus = 50000;
    ancilla =1500000;
    elder = 7500000;
    methuselah = 15000000;
    if (IS_CLASS(ch, CLASS_HIGHLANDER)) max_stat = 25;

    if ( !str_cmp( arg1, "str" ) )
    {
	pAbility    = &ch->pcdata->perm_str;
	pOutput     = "strength";
    }

    else if ( !str_cmp( arg1, "int" ) )
    {
	pAbility    = &ch->pcdata->perm_int;
	pOutput     = "intelligence";
    }

    else if ( !str_cmp( arg1, "wis" ) )
    {
	pAbility    = &ch->pcdata->perm_wis;
	pOutput     = "wisdom";
    }

    else if ( !str_cmp( arg1, "dex" ) )
    {
	pAbility    = &ch->pcdata->perm_dex;
	pOutput     = "dexterity";
    }

    else if ( !str_cmp( arg1, "con" ) )
    {
	pAbility    = &ch->pcdata->perm_con;
	pOutput     = "constitution";
    }

    else if ( !str_cmp( arg1, "avatar") && (ch->level == 2))
    {
        cost        = 1000;
	pAbility    = &ch->level;
	pOutput     = "level";
    }

    else if ( !str_cmp( arg1, "primal") )
    {
        cost        = primal;
	pAbility    = &ch->practice;
	pOutput     = "primal";
    }
    else if (!str_cmp(arg1, "magic") && IS_CLASS(ch, CLASS_DROW))
	{
	  sh_int quiet_pointer=ch->pcdata->stats[DROW_MAGIC];
	  cost = magic;
	  pAbility = &quiet_pointer;
	  pOutput = "drow magic resistance";
      }
	
    else if ( !str_cmp( arg1, "silver") && IS_CLASS(ch, CLASS_WEREWOLF) )
    {
	sh_int quiet_pointer=ch->siltol; /* dirrent type of int*/
        cost        = silver;
	pAbility    = &quiet_pointer;
	pOutput     = "tolerance to silver";
    }
    else if ( !str_cmp( arg1, "focus") && IS_CLASS(ch, CLASS_MONK))
    {
	sh_int quiet_pointer = ch->focus[MAXIMUM];
	cost 	    = focus;
	pAbility    = &quiet_pointer;
	pOutput     = "monk focus";
    }
    else if ( !str_cmp( arg1, "gnosis") && IS_CLASS(ch, CLASS_WEREWOLF))
    {
	sh_int quiet_pointer = ch->gnosis[GMAXIMUM];
	cost 	    = gnosis;
        pAbility    = &quiet_pointer;
        pOutput     = "gnosis points";
    }
    else if ( !str_cmp( arg1, "control") && IS_CLASS(ch, CLASS_VAMPIRE))
    {
	sh_int quiet_pointer = ch->beast;
	cost 	    = beast;
	pAbility    = &quiet_pointer;
	pOutput     = "control over your beast";
    }

    else if ( !str_cmp( arg1, "ancilla") && IS_CLASS(ch, CLASS_VAMPIRE) &&
 
((ch->pcdata->rank ==AGE_NEONATE) || (ch->pcdata->rank == AGE_CHILDE))) 
{
    
    if (ch->exp < ancilla)
    {
       send_to_char( "You need 1500000 exp to become ancilla!\n\r",ch);
       return;
    }
      ch->exp -= 1500000;
      ch->pcdata->rank = AGE_ANCILLA;
      sprintf(buf,"%s is now an Ancilla!", ch->name);
      do_info(ch,buf);
      send_to_char( "You are now an Ancilla.\n\r",ch);
      return;
 }
   else if ( !str_cmp( arg1, "elder") && IS_CLASS(ch,CLASS_VAMPIRE) &&
   (ch->pcdata->rank == AGE_ANCILLA))
      {
        if (ch->exp < elder)
        {
         send_to_char("You need 7500000 exp to become an elder.\n\r",ch);
         return; 
        }
        ch->exp -= 7500000;   
        ch->pcdata->rank = AGE_ELDER;
        sprintf(buf,"%s is now an Elder!", ch->name);
        do_info(ch,buf);
        send_to_char( "You are now an Elder!\n\r",ch);
        return;
}
   else if ( !str_cmp( arg1, "methuselah") && IS_CLASS(ch, CLASS_VAMPIRE)
&& (ch->pcdata->rank == AGE_ELDER))
  {
     if (ch->exp < methuselah)
      {
      send_to_char( "You need 15000000 exp to become Methuselah!\n\r",ch);
      return;
      }
    ch->exp -= 15000000;
    ch->pcdata->rank = AGE_METHUSELAH;
    sprintf(buf,"%s is now a Methuselah!", ch->name);
    do_info(ch, buf);
    send_to_char( "You are now Methuselah!\n\r",ch);
    return;
}

    else if ( !str_cmp( arg1, "sunlight") && !IS_IMMUNE(ch,IMM_SUNLIGHT)
&& IS_CLASS(ch,CLASS_VAMPIRE))
   {
      if (ch->exp < 1000000)
      {
      send_to_char("You dont have enough experience.\n\r",ch);
	return;
      }

ch->exp = ch->exp - 1000000;
SET_BIT(ch->immune, IMM_SUNLIGHT);
send_to_char("You are now immune to sunlight.\n\r",ch);
return;
}


    
else if ( !str_cmp( arg1, "slash") && !IS_IMMUNE(ch,IMM_SLASH) )
    
    {
	if (ch->exp < immcost)
	{
	    send_to_char( "You don't have enough exp.\n\r", ch );
	    return;
	}
	ch->exp = ch->exp - immcost;
	SET_BIT(ch->immune, IMM_SLASH);
	send_to_char( "You are now more resistant to slashing and slicing weapons.\n\r", ch );
	return;
    }

    else if ( !str_cmp( arg1, "stab") && !IS_IMMUNE(ch,IMM_STAB) )
    {
	if (ch->exp < immcost)
	{
	    send_to_char( "You don't have enough exp.\n\r", ch );
	    return;
	}
	ch->exp = ch->exp - immcost;
	SET_BIT(ch->immune, IMM_STAB);
	send_to_char( "You are now more resistant to stabbing and piercing weapons.\n\r", ch );
	return;
    }

    else if ( !str_cmp( arg1, "smash") && !IS_IMMUNE(ch,IMM_SMASH) )
    {
	if (ch->exp < immcost)
	{
	    send_to_char( "You don't have enough exp.\n\r", ch );
	    return;
	}
	ch->exp = ch->exp - immcost;
	SET_BIT(ch->immune, IMM_SMASH);
	send_to_char( "You are now more resistant to blasting, pounding and crushing weapons.\n\r", ch );
	return;
    }

    else if ( !str_cmp( arg1, "beast") && !IS_IMMUNE(ch,IMM_ANIMAL) )
    {
	if (ch->exp < immcost)
	{
	    send_to_char( "You don't have enough exp.\n\r", ch );
	    return;
	}
	ch->exp = ch->exp - immcost;
	SET_BIT(ch->immune, IMM_ANIMAL);
	send_to_char( "You are now more resistant to claw and bite attacks.\n\r", ch );
	return;
    }

    else if ( !str_cmp( arg1, "grab") && !IS_IMMUNE(ch,IMM_MISC) )
    {
	if (ch->exp < immcost)
	{
	    send_to_char( "You don't have enough exp.\n\r", ch );
	    return;
	}
	ch->exp = ch->exp - immcost;
	SET_BIT(ch->immune, IMM_MISC);
	send_to_char( "You are now more resistant to grepping, sucking and whipping weapons.\n\r", ch );
	return;
    }

    else if ( !str_cmp( arg1, "charm") && !IS_IMMUNE(ch,IMM_CHARM) )
    {
	if (ch->exp < immcost)
	{
	    send_to_char( "You don't have enough exp.\n\r", ch );
	    return;
	}
	ch->exp = ch->exp - immcost;
	SET_BIT(ch->immune, IMM_CHARM);
	send_to_char( "You are now immune to charm spells.\n\r", ch );
	return;
    }

    else if ( !str_cmp( arg1, "heat") && !IS_IMMUNE(ch,IMM_HEAT) )
    {
	if (ch->exp < immcost)
	{
	    send_to_char( "You don't have enough exp.\n\r", ch );
	    return;
	}
	ch->exp = ch->exp - immcost;
	SET_BIT(ch->immune, IMM_HEAT);
	send_to_char( "You are now immune to heat and fire spells.\n\r", ch );
	return;
    }

    else if ( !str_cmp( arg1, "cold") && !IS_IMMUNE(ch,IMM_COLD) )
    {
	if (ch->exp < immcost)
	{
	    send_to_char( "You don't have enough exp.\n\r", ch );
	    return;
	}
	ch->exp = ch->exp - immcost;
	SET_BIT(ch->immune, IMM_COLD);
	send_to_char( "You are now immune to cold spells.\n\r", ch );
	return;
    }

    else if ( !str_cmp( arg1, "lightning") && !IS_IMMUNE(ch,IMM_LIGHTNING) )
    {
	if (ch->exp < immcost)
	{
	    send_to_char( "You don't have enough exp.\n\r", ch );
	    return;
	}
	ch->exp = ch->exp - immcost;
	SET_BIT(ch->immune, IMM_LIGHTNING);
	send_to_char( "You are now immune to lightning and electrical spells.\n\r", ch );
	return;
    }

    else if ( !str_cmp( arg1, "acid") && !IS_IMMUNE(ch,IMM_ACID) )
    {
	if (ch->exp < immcost)
	{
	    send_to_char( "You don't have enough exp.\n\r", ch );
	    return;
	}
	ch->exp = ch->exp - immcost;
	SET_BIT(ch->immune, IMM_ACID);
	send_to_char( "You are now immune to acid spells.\n\r", ch );
	return;
    }

    else if ( !str_cmp( arg1, "sleep") && !IS_IMMUNE(ch,IMM_SLEEP) )
    {
	if (ch->exp < immcost)
	{
	    send_to_char( "You don't have enough exp.\n\r", ch );
	    return;
	}
	ch->exp = ch->exp - immcost;
	SET_BIT(ch->immune, IMM_SLEEP);
	send_to_char( "You are now immune to the sleep spell.\n\r", ch );
	return;
    }

    else if ( !str_cmp( arg1, "drain") && !IS_IMMUNE(ch,IMM_DRAIN) )
    {
	if (ch->exp < immcost)
	{
	    send_to_char( "You don't have enough exp.\n\r", ch );
	    return;
	}
	ch->exp = ch->exp - immcost;
	SET_BIT(ch->immune, IMM_DRAIN);
	send_to_char( "You are now immune to the energy drain spell.\n\r", ch );
	return;
    }


    else if ( !str_cmp( arg1, "voodoo") && !IS_IMMUNE(ch,IMM_VOODOO) )
    {
	if (ch->exp < immcost)
	{
	    send_to_char( "You don't have enough exp.\n\r", ch );
	    return;
	}
	ch->exp = ch->exp - immcost;
	SET_BIT(ch->immune, IMM_VOODOO);
	send_to_char( "You are now immune to voodoo magic.\n\r", ch );
	return;
    }

    else if ( !str_cmp( arg1, "hurl") && !IS_IMMUNE(ch,IMM_HURL) )
    {
	if (ch->exp < immcost)
	{
	    send_to_char( "You don't have enough exp.\n\r", ch );
	    return;
	}
	ch->exp = ch->exp - immcost;
	SET_BIT(ch->immune, IMM_HURL);
	send_to_char( "You are now immune to being hurled.\n\r", ch );
	return;
    }

    else if ( !str_cmp( arg1, "backstab") && !IS_IMMUNE(ch,IMM_BACKSTAB) )
    {
	if (ch->exp < immcost)
	{
	    send_to_char( "You don't have enough exp.\n\r", ch );
	    return;
	}
	ch->exp = ch->exp - immcost;
	SET_BIT(ch->immune, IMM_BACKSTAB);
	send_to_char( "You are now immune to being backstabbed.\n\r", ch );
	return;
    }

    else if ( !str_cmp( arg1, "kick") && !IS_IMMUNE(ch,IMM_KICK) )
    {
	if (ch->exp < immcost)
	{
	    send_to_char( "You don't have enough exp.\n\r", ch );
	    return;
	}
	ch->exp = ch->exp - immcost;
	SET_BIT(ch->immune, IMM_KICK);
	send_to_char( "You are now immune to being kicked.\n\r", ch );
	return;
    }

    else if ( !str_cmp( arg1, "disarm") && !IS_IMMUNE(ch,IMM_DISARM) )
    {
	if (ch->exp < immcost)
	{
	    send_to_char( "You don't have enough exp.\n\r", ch );
	    return;
	}
	ch->exp = ch->exp - immcost;
	SET_BIT(ch->immune, IMM_DISARM);
	send_to_char( "You are now immune to being disarmed.\n\r", ch );
	return;
    }

    else if ( !str_cmp( arg1, "steal") && !IS_IMMUNE(ch,IMM_STEAL) )
    {
	if (ch->exp < immcost)
	{
	    send_to_char( "You don't have enough exp.\n\r", ch );
	    return;
	}
	ch->exp = ch->exp - immcost;
	SET_BIT(ch->immune, IMM_STEAL);
	send_to_char( "You are now immune to being stolen from.\n\r", ch );
	return;
    }

    else
    {
	sprintf( buf, "You can train the following:\n\r" );
	send_to_char( buf, ch );

	send_to_char( "Stats:", ch );
	if ( ch->pcdata->perm_str < max_stat ) send_to_char( " Str", ch );
	if ( ch->pcdata->perm_int < max_stat ) send_to_char( " Int", ch );
	if ( ch->pcdata->perm_wis < max_stat ) send_to_char( " Wis", ch );
	if ( ch->pcdata->perm_dex < max_stat ) send_to_char( " Dex", ch );
	if ( ch->pcdata->perm_con < max_stat ) send_to_char( " Con", ch );
	if ( ( ch->pcdata->perm_str >= max_stat )
		&& ( ch->pcdata->perm_wis >= max_stat )
		&& ( ch->pcdata->perm_int >= max_stat )
		&& ( ch->pcdata->perm_dex >= max_stat )
		&& ( ch->pcdata->perm_con >= max_stat ) )
	    send_to_char( " None left to train.\n\r", ch );
	else
	    send_to_char( ".\n\r", ch );

	if ( ch->level == 2 )
	{
	   sprintf( buf, "Become an avatar - 1000 exp.\n\r" );
	   send_to_char( buf, ch );
	}
       	
        if ( ch->max_hit      < 15000 )
        {
           sprintf( buf, "Hit              - %d exp per 50 points.\n\r",(ch->max_hit*50) ); 
           send_to_char( buf, ch );
        }

	if ( ch->max_mana      < 15000 )
	{
	   sprintf( buf, "Mana             - %d exp per 50 points.\n\r",(ch->max_mana *50) );
	   send_to_char( buf, ch );
	}
	if ( ch->max_move      < 15000 )
	{
	   sprintf( buf, "Move             - %d exp per 50 points.\n\r",(ch->max_move*50) );
	   send_to_char( buf, ch );
	}
	if ( ch->practice        < 999 )
	{
	   sprintf( buf, "Primal           - %d exp per point of primal energy.\n\r", (1+ch->practice)*500 );
	   send_to_char( buf, ch );
	}
       if (IS_CLASS(ch,CLASS_VAMPIRE) )
        {
       if  ( ch->pcdata->rank == AGE_CHILDE)
         send_to_char("Age: Ancilla\n\r",ch); 	
       else if (ch->pcdata->rank == AGE_NEONATE)
         send_to_char("Age: Ancilla\n\r",ch);
       else if (ch->pcdata->rank == AGE_ANCILLA)
        send_to_char("Age: Elder\n\r",ch);
        else if (ch->pcdata->rank == AGE_ELDER)
       send_to_char("Age: Methuselah\n\r",ch);
        }
       if ( IS_CLASS(ch, CLASS_VAMPIRE) && !IS_IMMUNE(ch, IMM_SUNLIGHT))
       {
	   stc("Immunity to Sunlight - 1,000,000 experience.\n\r",ch);
       }
       if ( ch->siltol < 100 && IS_CLASS(ch, CLASS_WEREWOLF))
	{
	   sprintf( buf, "Silver tolerance - %d exp per point of tolerance.\n\r", silver );
	   send_to_char( buf, ch );
	}
       if ( ch->gnosis[GMAXIMUM] < 20 && IS_CLASS(ch, CLASS_WEREWOLF))
       {
	   sprintf( buf, "Gnosis Points - %d exp per point of Gnosis.\n\r", gnosis );
	   stc( buf, ch );
       }
       if ( ch->beast > 0 && IS_CLASS(ch, CLASS_VAMPIRE) )
       {
	   sprintf( buf, "Beast Control- %d primal per point of Beast.\n\r", beast );
	   stc( buf, ch );
       }
       if ( ch->focus[MAXIMUM] < 100 && IS_CLASS(ch, CLASS_MONK))
       {
	   sprintf( buf, "Focus Points - %d exp per point of Focus.\n\r", focus );
	   stc( buf, ch );
       }
	if (ch->pcdata->stats[DROW_MAGIC] < 100 && IS_CLASS(ch, CLASS_DROW))
	{ sprintf( buf, "Magic resistance - %d drow points per point of magic.\n\r", magic );
	  send_to_char( buf, ch);}
	
	sprintf( buf, "Natural resistances and immunities - %d exp each.\n\r", immcost );
	send_to_char( buf, ch );

	/* Weapon resistance affects */
	send_to_char( "Weapon resistances:", ch );
	if ( !IS_IMMUNE(ch, IMM_SLASH)  ) send_to_char( " Slash", ch );
	if ( !IS_IMMUNE(ch, IMM_STAB)   ) send_to_char( " Stab", ch );
	if ( !IS_IMMUNE(ch, IMM_SMASH)  ) send_to_char( " Smash", ch );
	if ( !IS_IMMUNE(ch, IMM_ANIMAL) ) send_to_char( " Beast", ch );
	if ( !IS_IMMUNE(ch, IMM_MISC)   ) send_to_char( " Grab", ch );
	if ( IS_IMMUNE(ch, IMM_SLASH) && IS_IMMUNE(ch, IMM_STAB) && 
	IS_IMMUNE(ch, IMM_SMASH) && IS_IMMUNE(ch, IMM_ANIMAL) && 
	IS_IMMUNE(ch, IMM_MISC) )
	   send_to_char( " None left to learn.\n\r", ch );
	else
	   send_to_char( ".\n\r", ch );

	/* Spell immunity affects */
	send_to_char( "Magical immunities:", ch );
	if ( !IS_IMMUNE(ch, IMM_CHARM)     ) send_to_char( " Charm", ch );
	if ( !IS_IMMUNE(ch, IMM_HEAT)      ) send_to_char( " Heat", ch );
	if ( !IS_IMMUNE(ch, IMM_COLD)      ) send_to_char( " Cold", ch );
	if ( !IS_IMMUNE(ch, IMM_LIGHTNING) ) send_to_char( " Lightning", ch );
	if ( !IS_IMMUNE(ch, IMM_ACID)      ) send_to_char( " Acid", ch );
	if ( !IS_IMMUNE(ch, IMM_SLEEP)     ) send_to_char( " Sleep", ch );
	if ( !IS_IMMUNE(ch, IMM_DRAIN)     ) send_to_char( " Drain", ch );
	if ( !IS_IMMUNE(ch, IMM_VOODOO)    ) send_to_char( " Voodoo", ch );
	if ( IS_IMMUNE(ch, IMM_CHARM) && IS_IMMUNE(ch, IMM_HEAT) && 
	IS_IMMUNE(ch, IMM_COLD) && IS_IMMUNE(ch, IMM_LIGHTNING) && 
	IS_IMMUNE(ch, IMM_ACID) && IS_IMMUNE(ch, IMM_SLEEP) &&
	IS_IMMUNE(ch, IMM_DRAIN) && IS_IMMUNE(ch, IMM_VOODOO) )
	   send_to_char( " None left to learn.\n\r", ch );
	else
	   send_to_char( ".\n\r", ch );

	/* Skill immunity affects */
	send_to_char( "Skill immunities:", ch );
	if ( !IS_IMMUNE(ch, IMM_HURL)     ) send_to_char( " Hurl", ch );
	if ( !IS_IMMUNE(ch, IMM_BACKSTAB) ) send_to_char( " Backstab", ch );
	if ( !IS_IMMUNE(ch, IMM_KICK)     ) send_to_char( " Kick", ch );
	if ( !IS_IMMUNE(ch, IMM_DISARM)   ) send_to_char( " Disarm", ch );
	if ( !IS_IMMUNE(ch, IMM_STEAL)    ) send_to_char( " Steal", ch );
	if ( IS_IMMUNE(ch, IMM_HURL) && IS_IMMUNE(ch, IMM_BACKSTAB) && 
	IS_IMMUNE(ch, IMM_KICK)      && IS_IMMUNE(ch, IMM_DISARM)   && 
	IS_IMMUNE(ch, IMM_STEAL) )
	   send_to_char( " None left to learn.\n\r", ch );
	else
	   send_to_char( ".\n\r", ch );

	return;
    }

    if ( (*pAbility >= max_stat) && (!str_cmp( arg1, "str")))
    {
	if (last) act( "Your $T is already at maximum.", ch, NULL, pOutput, TO_CHAR );
	return;
    }
    if ( (*pAbility >= max_stat) && (!str_cmp( arg1, "int")))
    {
	if (last) act( "Your $T is already at maximum.", ch, NULL, pOutput, TO_CHAR );
	return;
    }
    if ( (*pAbility >= max_stat) && (!str_cmp( arg1, "wis")))
    {
	if (last) act( "Your $T is already at maximum.", ch, NULL, pOutput, TO_CHAR );
	return;
    }
    if ( (*pAbility >= max_stat) && (!str_cmp( arg1, "dex")))
    {
	if (last) act( "Your $T is already at maximum.", ch, NULL, pOutput, TO_CHAR );
	return;
    }
    if ( (*pAbility >= max_stat) && (!str_cmp( arg1, "con")))
    {
	if (last) act( "Your $T is already at maximum.", ch, NULL, pOutput, TO_CHAR );
	return;
    }
    if ( (*pAbility >= 15000) && (!str_cmp( arg1, "hp")))
    {
        if (last) act( "Your $T is already at maximum.", ch, NULL, pOutput, TO_CHAR );
        return;
    }
    if ( (*pAbility >= 15000) && (!str_cmp( arg1, "mana")))
    {
	if (last) act( "Your $T is already at maximum.", ch, NULL, pOutput, TO_CHAR );
	return;
    }
    if ( (*pAbility >= 15000) && (!str_cmp( arg1, "move")))
    {
	if (last) act( "Your $T is already at maximum.", ch, NULL, pOutput, TO_CHAR );
	return;
    }
    if ( (*pAbility >= 999) && (!str_cmp( arg1, "primal")))
    {
	if (last) act( "Your $T is already at maximum.", ch, NULL, pOutput, TO_CHAR );
	return;
    }
    if ((*pAbility >= 100) && (!str_cmp(arg1, "magic"))) {
		if (last) act("Your $T is already at a maximum.", ch, NULL, pOutput, TO_CHAR);
		return;}

    if ( (*pAbility >= 20 ) && (!str_cmp( arg1, "gnosis") ))
    {
	if (last) act( "Your $T is already at maximum.",ch,NULL,pOutput,TO_CHAR);
	return;
    }

    if ( (*pAbility == 0 ) && (!str_cmp( arg2, "control") ) )
    {
	if (last) act( "You have already mastered your beast.",ch,NULL,pOutput,TO_CHAR);
	return;
    }

    if ( ( *pAbility >= 100 ) && (!str_cmp( arg1, "focus") ) )
    {
	if (last) act( "Your $T is already at maximum.",ch,NULL,pOutput,TO_CHAR);
	return;
    }

    if ( (*pAbility >= 100) && (!str_cmp( arg1, "silver")))
    {
	if (last) act( "Your $T is already at maximum.", ch, NULL, pOutput, TO_CHAR );
	return;
    }

    if ( cost < 1 ) cost = 1;
    if ( cost > ch->pcdata->stats[DROW_POWER] && !str_cmp(arg1, "magic"))
	{ if (last) send_to_char( "You don't have enough drow power points.\n\r", ch);
	return;}
else     if ( cost > ch->exp )
    {
	if (last) send_to_char( "You don't have enough exp.\n\r", ch );
	return;
    }
if (!str_cmp(arg1, "magic"))
	ch->pcdata->stats[DROW_POWER] -= cost;
else
    ch->exp		-= cost;
	if (!str_cmp( arg1, "silver"))
		ch->siltol += 1;
	else if (!str_cmp(arg1, "gnosis"))
	{
	   ch->gnosis[GCURRENT]++;
	   ch->gnosis[GMAXIMUM]++;
	}
	
	else if (!str_cmp(arg1, "focus"))
        {
	   ch->focus[CURRENT]++;
	   ch->focus[MAXIMUM]++;
	}
        else if (!str_cmp(arg1, "control"))
	  ch->beast--;
	else if (!str_cmp(arg1, "magic"))
		ch->pcdata->stats[DROW_MAGIC] += 1;
	else
    *pAbility		+= 1;
    if ( !str_cmp( arg1, "avatar") )
    {
	act( "You become an avatar!", ch, NULL, NULL, TO_CHAR );
	sprintf( buf, "%s has become an avatar!",ch->pcdata->switchname );
	do_info(ch,buf);
	if (IS_CLASS(ch, CLASS_DEMON)) ch->alignment = -1000;
	if (ch->level < ch->trust) ch->level = ch->trust;
	if (!IS_NPC(ch) && IS_VAMPAFF(ch, VAM_MORTAL))
	    do_mortalvamp(ch,"");
    }
    else if (last)
	act( "Your $T increases!", ch, NULL, pOutput, TO_CHAR );
    return;
}

void do_mount( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;

    argument = one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
	send_to_char( "Mount what?\n\r", ch );
	return;
    }

    if (IS_AFFECTED(ch, AFF_POLYMORPH) && !IS_VAMPAFF(ch, VAM_DISGUISED))
    {
	send_to_char( "You cannot ride in this form.\n\r",ch );
	return;
    }

    if ( ( victim = get_char_room( ch, arg ) ) == NULL )
    {
	send_to_char( "They aren't here.\n\r",ch );
	return;
    }

    if ( ch == victim )
    {
	send_to_char( "You cannot ride on your own back!\n\r",ch );
	return;
    }

    if ( ch->mounted > 0 )
    {
	send_to_char( "You are already riding.\n\r",ch );
	return;
    }

    if ( !IS_NPC(victim) || victim->mounted > 0 || (IS_NPC(victim) && !IS_SET(victim->act, ACT_MOUNT) ) )
    {
	send_to_char( "You cannot mount them.\n\r",ch );
	return;
    }

    if ( victim->position < POS_STANDING )
    {
	if ( victim->position < POS_SLEEPING )
	    act( "$N is too badly hurt for that.", ch, NULL, victim, TO_CHAR );
	else if ( victim->position == POS_SLEEPING )
	    act( "First you better wake $m up.", ch, NULL, victim, TO_CHAR );
	else if ( victim->position == POS_RESTING )
	    act( "First $e better stand up.", ch, NULL, victim, TO_CHAR );
	else if ( victim->position == POS_MEDITATING )
	    act( "First $e better stand up.", ch, NULL, victim, TO_CHAR );
	else if ( victim->position == POS_SITTING )
	    act( "First $e better stand up.", ch, NULL, victim, TO_CHAR );
	else if ( victim->position == POS_SLEEPING )
	    act( "First you better wake $m up.", ch, NULL, victim, TO_CHAR );
	else if ( victim->position == POS_FIGHTING )
	    act( "Not while $e's fighting.", ch, NULL, victim, TO_CHAR );
	return;
    }

    if (!IS_NPC(ch) && ch->stance[0] != -1) do_stance(ch,"");

    ch->mounted     = IS_RIDING;
    victim->mounted = IS_MOUNT;
    ch->mount = victim;
    victim->mount = ch;

    act( "You clamber onto $N's back.", ch, NULL, victim, TO_CHAR );
    act( "$n clambers onto $N's back.", ch, NULL, victim, TO_ROOM );
    return;
}

void do_dismount( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *victim;

    if ( ch->mounted == 0 )
    {
	send_to_char( "But you are not riding!\n\r",ch );
	return;
    }

    if ( (victim = ch->mount) == NULL )
    {
	send_to_char( "But you are not riding!\n\r",ch );
	return;
    }

    act( "You clamber off $N's back.", ch, NULL, victim, TO_CHAR );
    act( "$n clambers off $N's back.", ch, NULL, victim, TO_ROOM );

    ch->mounted     = IS_ON_FOOT;
    victim->mounted = IS_ON_FOOT;

    ch->mount = NULL;
    victim->mount = NULL;

    return;
}

void do_tie( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *victim;
    char arg [MAX_INPUT_LENGTH];
    char buf [MAX_INPUT_LENGTH];

    argument = one_argument( argument, arg );
    if ( ( victim = get_char_room( ch, arg ) ) == NULL )
    {
	send_to_char( "They aren't here.\n\r", ch );
	return;
    }
    if (victim == ch)
    {
	send_to_char( "You cannot tie yourself up!\n\r", ch );
	return;
    }
    if (IS_EXTRA(victim,TIED_UP))
    {
	send_to_char( "But they are already tied up!\n\r", ch );
	return;
    }
    if (victim->position > POS_STUNNED || victim->hit > 0)
    {
	send_to_char( "You can only tie up a defenceless person.\n\r", ch );
	return;
    }
    act("You quickly tie up $N.",ch,NULL,victim,TO_CHAR);
    act("$n quickly ties up $N.",ch,NULL,victim,TO_ROOM);
    send_to_char("You have been tied up!\n\r",victim);
    SET_BIT(victim->extra,TIED_UP);
    sprintf(buf,"%s has been tied up by %s.",victim->pcdata->switchname,ch->pcdata->switchname);
    do_info(ch,buf);
    return;
}

void do_untie( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *victim;
    char arg [MAX_INPUT_LENGTH];

    argument = one_argument( argument, arg );
    if ( ( victim = get_char_room( ch, arg ) ) == NULL )
    {
	send_to_char( "They aren't here.\n\r", ch );
	return;
    }
    if (!IS_EXTRA(victim,TIED_UP))
    {
	send_to_char( "But they are not tied up!\n\r", ch );
	return;
    }
    if (victim == ch)
    {
	send_to_char( "You cannot untie yourself!\n\r", ch );
	return;
    }
    act("You quickly untie $N.",ch,NULL,victim,TO_CHAR);
    act("$n quickly unties $N.",ch,NULL,victim,TO_NOTVICT);
    act("$n quickly unties you.",ch,NULL,victim,TO_VICT);
    REMOVE_BIT(victim->extra,TIED_UP);
    return;
}

void do_gag( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *victim;
    char arg [MAX_INPUT_LENGTH];

    argument = one_argument( argument, arg );
    if ( ( victim = get_char_room( ch, arg ) ) == NULL )
    {
	send_to_char( "They aren't here.\n\r", ch );
	return;
    }
    if (victim == ch && !IS_EXTRA(victim,GAGGED) && IS_EXTRA(victim,TIED_UP))
    {
	send_to_char( "You cannot gag yourself!\n\r", ch );
	return;
    }
    if (!IS_EXTRA(victim,TIED_UP) && !IS_EXTRA(victim,GAGGED))
    {
	send_to_char( "You can only gag someone who is tied up!\n\r", ch );
	return;
    }
    if (!IS_EXTRA(victim,GAGGED))
    {
    	act("You place a gag over $N's mouth.",ch,NULL,victim,TO_CHAR);
    	act("$n places a gag over $N's mouth.",ch,NULL,victim,TO_NOTVICT);
    	act("$n places a gag over your mouth.",ch,NULL,victim,TO_VICT);
    	SET_BIT(victim->extra,GAGGED);
	return;
    }
    if (ch == victim)
    {
    	act("You remove the gag from your mouth.",ch,NULL,victim,TO_CHAR);
    	act("$n removes the gag from $s mouth.",ch,NULL,victim,TO_ROOM);
    	REMOVE_BIT(victim->extra,GAGGED);
	return;
    }
    act("You remove the gag from $N's mouth.",ch,NULL,victim,TO_CHAR);
    act("$n removes the gag from $N's mouth.",ch,NULL,victim,TO_NOTVICT);
    act("$n removes the gag from your mouth.",ch,NULL,victim,TO_VICT);
    REMOVE_BIT(victim->extra,GAGGED);
    return;
}

void do_blindfold( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *victim;
    char arg [MAX_INPUT_LENGTH];

    argument = one_argument( argument, arg );
    if ( ( victim = get_char_room( ch, arg ) ) == NULL )
    {
	send_to_char( "They aren't here.\n\r", ch );
	return;
    }
    if (victim == ch && !IS_EXTRA(victim,BLINDFOLDED) && IS_EXTRA(victim,TIED_UP))
    {
	send_to_char( "You cannot blindfold yourself!\n\r", ch );
	return;
    }
    if (!IS_EXTRA(victim,TIED_UP) && !IS_EXTRA(victim,BLINDFOLDED))
    {
	send_to_char( "You can only blindfold someone who is tied up!\n\r", ch );
	return;
    }
    if (!IS_EXTRA(victim,BLINDFOLDED))
    {
    	act("You place a blindfold over $N's eyes.",ch,NULL,victim,TO_CHAR);
    	act("$n places a blindfold over $N's eyes.",ch,NULL,victim,TO_NOTVICT);
    	act("$n places a blindfold over your eyes.",ch,NULL,victim,TO_VICT);
    	SET_BIT(victim->extra,BLINDFOLDED);
	return;
    }
    if (ch == victim)
    {
    	act("You remove the blindfold from your eyes.",ch,NULL,victim,TO_CHAR);
    	act("$n removes the blindfold from $s eyes.",ch,NULL,victim,TO_ROOM);
    	REMOVE_BIT(victim->extra,BLINDFOLDED);
	return;
    }
    act("You remove the blindfold from $N's eyes.",ch,NULL,victim,TO_CHAR);
    act("$n removes the blindfold from $N's eyes.",ch,NULL,victim,TO_NOTVICT);
    act("$n removes the blindfold from your eyes.",ch,NULL,victim,TO_VICT);
    REMOVE_BIT(victim->extra,BLINDFOLDED);
    return;
}


int count_imms( CHAR_DATA *ch )
{
    int count = 0;

    if ( IS_IMMUNE(ch, IMM_SLASH)     ) count += 1;
    if ( IS_IMMUNE(ch, IMM_STAB)      ) count += 1;
    if ( IS_IMMUNE(ch, IMM_SMASH)     ) count += 1;
    if ( IS_IMMUNE(ch, IMM_ANIMAL)    ) count += 1;
    if ( IS_IMMUNE(ch, IMM_MISC)      ) count += 1;
    if ( IS_IMMUNE(ch, IMM_CHARM)     ) count += 1;
    if ( IS_IMMUNE(ch, IMM_HEAT)      ) count += 1;
    if ( IS_IMMUNE(ch, IMM_COLD)      ) count += 1;
    if ( IS_IMMUNE(ch, IMM_LIGHTNING) ) count += 1;
    if ( IS_IMMUNE(ch, IMM_ACID)      ) count += 1;
    if ( IS_IMMUNE(ch, IMM_SLEEP)     ) count += 1;
    if ( IS_IMMUNE(ch, IMM_DRAIN)     ) count += 1;
    if ( IS_IMMUNE(ch, IMM_VOODOO)    ) count += 1;
    if ( IS_IMMUNE(ch, IMM_HURL)      ) count += 1;
    if ( IS_IMMUNE(ch, IMM_BACKSTAB)  ) count += 1;
    if ( IS_IMMUNE(ch, IMM_KICK)      ) count += 1;
    if ( IS_IMMUNE(ch, IMM_DISARM)    ) count += 1;
    if ( IS_IMMUNE(ch, IMM_STEAL)     ) count += 1;
    return ( ( count * 10000 ) + 10000 );
}

void do_track( CHAR_DATA *ch, char *argument )
{
    bool found = FALSE;
    if (!IS_NPC(ch) && number_percent() > ch->pcdata->learned[gsn_track])
    {
	send_to_char("You cannot sense any trails from this room.\n\r",ch);
	return;
    }
    if (check_track(ch,0)) found = TRUE;
    if (check_track(ch,1)) found = TRUE;
    if (check_track(ch,2)) found = TRUE;
    if (check_track(ch,3)) found = TRUE;
    if (check_track(ch,4)) found = TRUE;
    if (found == FALSE)
    {
	send_to_char("You cannot sense any trails from this room.\n\r",ch);
	return;
    }
    act("$n carefully examines the ground for tracks.",ch,NULL,NULL,TO_ROOM);
    return;
}

void do_hunt( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];

    argument = one_argument( argument, arg );
/*
    send_to_char("This command is temporarily disabled.\n\r",ch);
    return;
*/
    if ( arg[0] == '\0' )
    {
	if (strlen(ch->hunting) > 1)
	{
	    free_string(ch->hunting);
	    ch->hunting = str_dup( "" );
	    send_to_char( "You stop hunting your prey.\n\r", ch );
	}
	else send_to_char( "Who do you wish to hunt?\n\r", ch );
	return;
    }
    if (!str_cmp(arg,ch->name))
    {
	send_to_char("How can you hunt yourself?\n\r",ch);
	return;
    }
    ch->hunting = str_dup(arg);
    send_to_char( "Ok.\n\r", ch );
    return;
}

void check_hunt( CHAR_DATA *ch )
{
    CHAR_DATA *victim;
    bool found = FALSE;
    int direction = 0;
    ROOM_INDEX_DATA *in_room;
    in_room = ch->in_room;
    if (!IS_NPC(ch) && number_percent() > ch->pcdata->learned[gsn_track])
    {
	send_to_char("You cannot sense any trails from this room.\n\r",ch);
	free_string(ch->hunting);
	ch->hunting = str_dup( "" );
	return;
    }
    if (check_track(ch,0)) {found = TRUE;direction = ch->in_room->track_dir[0];}
    else if (check_track(ch,1)) {found = TRUE;direction = ch->in_room->track_dir[1];}
    else if (check_track(ch,2)) {found = TRUE;direction = ch->in_room->track_dir[2];}
    else if (check_track(ch,3)) {found = TRUE;direction = ch->in_room->track_dir[3];}
    else if (check_track(ch,4)) {found = TRUE;direction = ch->in_room->track_dir[4];}
    else if ( ( victim = get_char_room( ch, ch->hunting ) ) == NULL )
    {
	send_to_char("You cannot sense any trails from this room.\n\r",ch);
	free_string(ch->hunting);
	ch->hunting = str_dup( "" );
	return;
    }
    if (strlen(ch->hunting) < 2) return;
    if ( ( victim = get_char_room( ch, ch->hunting ) ) != NULL ) return;
    act("$n carefully examines the ground for tracks.",ch,NULL,NULL,TO_ROOM);
    move_char(ch,direction);
    if (in_room == ch->in_room || victim != NULL)
    {
	free_string(ch->hunting);
	ch->hunting = str_dup( "" );
    }
    return;
}

void add_tracks( CHAR_DATA *ch, int direction )
{
    int loop;

    if (IS_NPC(ch)) return;
    if (IS_CLASS(ch, CLASS_WEREWOLF) && ch->power[DISC_WERE_LYNX] > 0)
	return;
	if (IS_CLASS(ch, CLASS_NINJA) && ch->pcdata->powers[NPOWER_SORA] >= 4)
	return;
    if (IS_ITEMAFF(ch, ITEMA_STALKER)) return;
    for ( loop = 0; loop <= 4; loop++ )
    {
    	if (ch->in_room->track[loop] != NULL && !str_cmp(ch->in_room->track[loop],ch->name))
    	{
	    free_string(ch->in_room->track[loop]);
	    ch->in_room->track[loop] = str_dup("");
    	}
    }
    if (strlen(ch->in_room->track[0]) < 2)
    {
	free_string(ch->in_room->track[0]);
	ch->in_room->track[0]     = str_dup(ch->pcdata->switchname);
	ch->in_room->track_dir[0] = direction;
    }
    else if (strlen(ch->in_room->track[1]) < 2)
    {
	free_string(ch->in_room->track[1]);
	ch->in_room->track[1]     = str_dup(ch->pcdata->switchname);
	ch->in_room->track_dir[1] = direction;
    }
    else if (strlen(ch->in_room->track[2]) < 2)
    {
	free_string(ch->in_room->track[2]);
	ch->in_room->track[2]     = str_dup(ch->pcdata->switchname);
	ch->in_room->track_dir[2] = direction;
    }
    else if (strlen(ch->in_room->track[3]) < 2)
    {
	free_string(ch->in_room->track[3]);
	ch->in_room->track[3]     = str_dup(ch->pcdata->switchname);
	ch->in_room->track_dir[3] = direction;
    }
    else if (strlen(ch->in_room->track[4]) < 2)
    {
	free_string(ch->in_room->track[4]);
	ch->in_room->track[4]     = str_dup(ch->pcdata->switchname);
	ch->in_room->track_dir[4] = direction;
    }
    else
    {
	free_string(ch->in_room->track[0]);
	ch->in_room->track[0]     = str_dup(ch->in_room->track[1]);
	ch->in_room->track_dir[0] = ch->in_room->track_dir[1];
	free_string(ch->in_room->track[1]);
	ch->in_room->track[1]     = str_dup(ch->in_room->track[2]);
	ch->in_room->track_dir[1] = ch->in_room->track_dir[2];
	free_string(ch->in_room->track[2]);
	ch->in_room->track[2]     = str_dup(ch->in_room->track[3]);
	ch->in_room->track_dir[2] = ch->in_room->track_dir[3];
	free_string(ch->in_room->track[3]);
	ch->in_room->track[3]     = str_dup(ch->in_room->track[4]);
	ch->in_room->track_dir[3] = ch->in_room->track_dir[4];
	free_string(ch->in_room->track[4]);
	ch->in_room->track[4]     = str_dup(ch->name);
	ch->in_room->track_dir[4] = direction;
    }
    return;
}

bool check_track( CHAR_DATA *ch, int direction )
{
    CHAR_DATA *victim;
    char buf [MAX_INPUT_LENGTH];
    char vict [MAX_INPUT_LENGTH];
    int door;

    strcpy(vict,ch->hunting);
    if (!str_cmp(ch->hunting,vict))
    {
	if ( ( victim = get_char_room( ch, vict ) ) != NULL )
	{
	    act("You have found $N!",ch,NULL,victim,TO_CHAR);
	    free_string(ch->hunting);
	    ch->hunting = str_dup( "" );
	    return TRUE;
	}
    }
    if (strlen(ch->in_room->track[direction]) < 2) return FALSE;
    if (!str_cmp(ch->in_room->track[direction],ch->name)) return FALSE;
    if (strlen(ch->hunting) > 1 && str_cmp(ch->in_room->track[direction],ch->hunting)) return FALSE;
    door = ch->in_room->track_dir[direction];
    sprintf(buf,"You sense the trail of %s leading $T from here.",ch->in_room->track[direction]);
    act( buf, ch, NULL, dir_name[door], TO_CHAR );
    return TRUE;
}

void do_roll( CHAR_DATA *ch, char *argument )
{
    OBJ_DATA *obj;
    char arg[MAX_INPUT_LENGTH];

    argument = one_argument( argument, arg );

    if (IS_NPC(ch)) return;

    if (!IS_CLASS(ch, CLASS_DEMON))
    {
	stc("Huh?\n\r",ch);
	return;
    }

    if ( ( obj = ch->pcdata->chobj ) == NULL )
    {
	send_to_char("Huh?\n\r",ch);
	return;
    }

    if ( obj->chobj == NULL || obj->chobj != ch )
    {
	send_to_char("Huh?\n\r",ch);
	return;
    }

    if ( obj->in_room == NULL )
    {
	send_to_char("You are unable to move.\n\r",ch);
	return;
    }

    if      (!str_cmp(arg,"n") || !str_cmp(arg,"north")) do_north(ch,"");
    else if (!str_cmp(arg,"s") || !str_cmp(arg,"south")) do_south(ch,"");
    else if (!str_cmp(arg,"e") || !str_cmp(arg,"east" )) do_east(ch,"");
    else if (!str_cmp(arg,"w") || !str_cmp(arg,"west" )) do_west(ch,"");
    else if (!str_cmp(arg,"u") || !str_cmp(arg,"up"   )) do_up(ch,"");
    else if (!str_cmp(arg,"d") || !str_cmp(arg,"down" )) do_down(ch,"");
    else
    {
	send_to_char("Do you wish to roll north, south, east, west, up or down?\n\r",ch);
	return;
    }
    obj_from_room(obj);
    obj_to_room(obj,ch->in_room);
    return;
}

void do_leap( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *victim;
    OBJ_DATA *container;
    OBJ_DATA *obj;
    char arg[MAX_INPUT_LENGTH];

    argument = one_argument( argument, arg );

    if (IS_NPC(ch)) return;

    if (!IS_CLASS(ch,CLASS_DEMON) && !IS_SET(ch->special,SPC_CHAMPION))
    {
	send_to_char("Huh?\n\r",ch);
	return;
    }

    if ( ( obj = ch->pcdata->chobj ) == NULL )
    {
	send_to_char("Huh?\n\r",ch);
	return;
    }

    if ( obj->chobj == NULL || obj->chobj != ch )
    {
	send_to_char("Huh?\n\r",ch);
	return;
    }

    if ( obj->in_room != NULL && arg[0] == '\0' )
    {
	send_to_char("Where do you want to leap?\n\r",ch);
	return;
    }

    if ( obj->in_room != NULL )
    {
	if ( ( victim = get_char_room( ch, arg ) ) != NULL )
	{
	    act("$p leaps into your hands.",victim,obj,NULL,TO_CHAR);
	    act("$p leaps into $n's hands.",victim,obj,NULL,TO_ROOM);
	    obj_from_room(obj);
	    obj_to_char(obj,victim);
	    return;
	}
	else if ( ( container = get_obj_room( ch, arg ) ) != NULL )
	{
	    if (container->item_type != ITEM_CONTAINER &&
		container->item_type != ITEM_CORPSE_NPC &&
		container->item_type != ITEM_CORPSE_PC)
	    {
		send_to_char("You cannot leap into that sort of object.\n\r",ch);
		return;
	    }
	    act("$p leap into $P.",ch,obj,container,TO_CHAR);
	    act("$p leaps into $P.",ch,obj,container,TO_ROOM);
	    obj_from_room(obj);
	    obj_to_obj(obj,container);
	    return;
	}
	else send_to_char( "Nothing here by that name.\n\r",ch );
	return;
    }
    if ( obj->carried_by != NULL )
    {
	act("$p leaps from your hands.",obj->carried_by,obj,NULL,TO_CHAR);
	act("$p leaps from $n's hands.",obj->carried_by,obj,NULL,TO_ROOM);
	obj_from_char(obj);
	obj_to_room(obj,ch->in_room);
	return;
    }
    else if ( (container = obj->in_obj) != NULL && container->in_room != NULL )
    {
	obj_from_obj(obj);
	obj_to_room(obj,container->in_room);
	char_from_room(ch);
	char_to_room(ch,container->in_room);
	act("$p leap from $P.",ch,obj,container,TO_CHAR);
	act("$p leaps from $P.",ch,obj,container,TO_ROOM);
	return;
    }
    if (obj->in_room != NULL)
	send_to_char("You seem unable to leap anywhere.\n\r",ch);
    else
	send_to_char("You seem to be stuck!\n\r",ch);
    return;
}


void drow_hate( CHAR_DATA *ch)
{
	CHAR_DATA *vch;
	CHAR_DATA *vch_next;

	if (IS_NPC(ch)) return;
	
	if (!IS_CLASS(ch, CLASS_DROW) || !IS_SET(ch->newbits, NEW_DROWHATE)) return;
	
	if (number_percent() < 25) {

	for (vch = char_list; vch != NULL; vch = vch_next)
	{
	vch_next = vch->next;
	if ( ch == vch) continue;
	if ( vch->in_room == NULL) continue;
	if ( vch->in_room == ch->in_room) {
		send_to_char("You scream out in hatred and attack!\n\r", ch );
		act("$n screams out in hatred and attacks!",ch,NULL,vch,TO_ROOM);
		do_kill(ch,vch->name);
		return;}
	}
	}

	return;
}
