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
#define CD CHAR_DATA
void	get_obj		args( ( CHAR_DATA *ch, OBJ_DATA *obj,
			    OBJ_DATA *container ) );
CD *	find_keeper	args( ( CHAR_DATA *ch ) );
int	get_cost	args( ( CHAR_DATA *keeper, OBJ_DATA *obj, bool fBuy ) );
void	sheath		args( ( CHAR_DATA *ch, bool right ) );
void	draw		args( ( CHAR_DATA *ch, bool right ) );
char	*special_item_name args( ( OBJ_DATA *obj ) );
void 	call_all	args( ( CHAR_DATA *ch ) );
#undef	CD




void do_call( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    OBJ_DATA *obj;
    CHAR_DATA *victim = NULL;
    ROOM_INDEX_DATA *chroom;
    ROOM_INDEX_DATA *objroom;

    one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
	send_to_char( "What object do you wish to call?\n\r", ch );
	return;
    }

    if (IS_NPC(ch)) {send_to_char("Not while switched.\n\r",ch); return;}

    if (!IS_HEAD(ch, LOST_HEAD))
    {
    	act("Your eyes flicker with yellow energy.",ch,NULL,NULL,TO_CHAR);
    	act("$n's eyes flicker with yellow energy.",ch,NULL,NULL,TO_ROOM);
    }

    if (!str_cmp(arg,"all"))
    {
	call_all(ch);
	return;
    }

    if ( ( obj = get_obj_world( ch, arg ) ) == NULL )
    {
	send_to_char( "Nothing like that in hell, earth, or heaven.\n\r", ch );
	return;
    }

    if (obj->questowner == NULL || strlen(obj->questowner) < 2 ||
	str_cmp(obj->questowner,ch->pcdata->switchname) || obj->item_type == ITEM_PAGE)
    {
	send_to_char( "Nothing happens.\n\r", ch );
	return;
    }

    if (obj->carried_by != NULL && obj->carried_by != ch)
    {
	victim = obj->carried_by;
	if (!IS_NPC(victim) && victim->desc != NULL && victim->desc->connected != CON_PLAYING) return;
    	act("$p suddenly vanishes from your hands!",victim,obj,NULL,TO_CHAR);
    	act("$p suddenly vanishes from $n's hands!",victim,obj,NULL,TO_ROOM);
	obj_from_char(obj);
    }
    else if (obj->in_room != NULL)
    {
    	chroom = ch->in_room;
    	objroom = obj->in_room;
    	char_from_room(ch);
    	char_to_room(ch,objroom);
    	act("$p vanishes from the ground!",ch,obj,NULL,TO_ROOM);
	if (chroom == objroom) act("$p vanishes from the ground!",ch,obj,NULL,TO_CHAR);
    	char_from_room(ch);
    	char_to_room(ch,chroom);
	obj_from_room(obj);
    }
    else if (obj->in_obj != NULL) obj_from_obj(obj);
    else
    {
	if (!IS_HEAD(ch, LOST_HEAD))
	    send_to_char( "Nothing happens.\n\r", ch );
	return;
    }

    obj_to_char(obj,ch);
    if (IS_SET(obj->extra_flags,ITEM_SHADOWPLANE))
	REMOVE_BIT(obj->extra_flags,ITEM_SHADOWPLANE);
    act("$p materializes in your hands.",ch,obj,NULL,TO_CHAR);
    act("$p materializes in $n's hands.",ch,obj,NULL,TO_ROOM);
    do_autosave(ch,"");
    if (victim != NULL) do_autosave(victim,"");
    return;
}

void call_all( CHAR_DATA *ch )
{
    OBJ_DATA *obj;
    OBJ_DATA *in_obj;
    CHAR_DATA *victim = NULL;
    DESCRIPTOR_DATA *d;
    ROOM_INDEX_DATA *chroom;
    ROOM_INDEX_DATA *objroom;
    bool found = FALSE;

    for ( obj = object_list; obj != NULL; obj = obj->next )
    {
	if ( obj->questowner == NULL || strlen(obj->questowner) < 2 || 
	    str_cmp( ch->pcdata->switchname, obj->questowner ) || obj->item_type == ITEM_PAGE)
	    continue;

	found = TRUE;

	for ( in_obj = obj; in_obj->in_obj != NULL; in_obj = in_obj->in_obj )
	    ;

	if (in_obj->carried_by != NULL)
	{
	    if (in_obj->carried_by == ch) continue;
	}

	if (obj->carried_by != NULL)
	{
	    if (obj->carried_by == ch || obj->carried_by->desc == NULL || 
		obj->carried_by->desc->connected != CON_PLAYING) 
	    {
		if (!IS_NPC(obj->carried_by)) return;
	    }
	    act("$p suddenly vanishes from your hands!",obj->carried_by,obj,NULL,TO_CHAR);
	    act("$p suddenly vanishes from $n's hands!",obj->carried_by,obj,NULL,TO_ROOM);
	    SET_BIT(obj->carried_by->extra, EXTRA_CALL_ALL);
	    obj_from_char(obj);
	}
	else if (obj->in_room != NULL)
	{
	    chroom = ch->in_room;
	    objroom = obj->in_room;
	    char_from_room(ch);
	    char_to_room(ch,objroom);
	    act("$p vanishes from the ground!",ch,obj,NULL,TO_ROOM);
	    if (chroom == objroom) act("$p vanishes from the ground!",ch,obj,NULL,TO_CHAR);
	    char_from_room(ch);
	    char_to_room(ch,chroom);
	    obj_from_room(obj);
	}
	else if (obj->in_obj != NULL) obj_from_obj(obj);
	else continue;
	obj_to_char(obj,ch);
	if (IS_SET(obj->extra_flags,ITEM_SHADOWPLANE))
	    REMOVE_BIT(obj->extra_flags,ITEM_SHADOWPLANE);
	if (!IS_HEAD(ch, LOST_HEAD))
	{
	    act("$p materializes in your hands.",ch,obj,NULL,TO_CHAR);
	    act("$p materializes in $n's hands.",ch,obj,NULL,TO_ROOM);
	}
    }

    if ( !found && !IS_HEAD(ch, LOST_HEAD) )
	send_to_char( "Nothing happens.\n\r", ch );

    for ( d = descriptor_list; d != NULL; d = d->next )
    {
	if ( d->connected != CON_PLAYING ) continue;
	if ( (victim = d->character) == NULL ) continue;
	if ( IS_NPC(victim) ) continue;
	if ( ch != victim && !IS_EXTRA(victim,EXTRA_CALL_ALL) ) continue;
	REMOVE_BIT(victim->extra, EXTRA_CALL_ALL);
	do_autosave(victim,"");
    }
    return;
}

void get_obj( CHAR_DATA *ch, OBJ_DATA *obj, OBJ_DATA *container )
{
    OBJ_DATA *obj2;
    OBJ_DATA *obj_next;
    ROOM_INDEX_DATA *objroom;
    bool move_ch = FALSE;

    /* Objects should only have a shadowplane flag when on the floor */
    if  ( IS_AFFECTED(ch,AFF_SHADOWPLANE) &&
	obj->in_room != NULL &&
	(!IS_SET(obj->extra_flags, ITEM_SHADOWPLANE) ) )
    {
	send_to_char( "Your hand passes right through it!\n\r", ch );
	return;
    }
    if  (!IS_AFFECTED(ch,AFF_SHADOWPLANE) &&
	obj->in_room != NULL &&
	( IS_SET(obj->extra_flags, ITEM_SHADOWPLANE) ) )
    {
	send_to_char( "Your hand passes right through it!\n\r", ch );
	return;
    }
    if ( !CAN_WEAR(obj, ITEM_TAKE) )
    {
	send_to_char( "You can't take that.\n\r", ch );
	return;
    }

    if ( ch->carry_number + 1 > can_carry_n( ch ) )
    {
	act( "$d: you can't carry that many items.",
	    ch, NULL, obj->name, TO_CHAR );
	return;
    }

    if ( ch->carry_weight + get_obj_weight( obj ) > can_carry_w( ch ) )
    {
	act( "$d: you can't carry that much weight.",
	    ch, NULL, obj->name, TO_CHAR );
	return;
    }

    if ( container != NULL )
    {

    	if  ( IS_AFFECTED(ch,AFF_SHADOWPLANE) &&
	     !IS_SET(container->extra_flags, ITEM_SHADOWPLANE) &&
	     (container->carried_by == NULL || container->carried_by != ch) )
    	{
	    send_to_char( "Your hand passes right through it!\n\r", ch );
	    return;
    	}
    	if  (!IS_AFFECTED(ch,AFF_SHADOWPLANE) &&
	      IS_SET(container->extra_flags, ITEM_SHADOWPLANE) &&
	     (container->carried_by == NULL || container->carried_by != ch) )
    	{
	    send_to_char( "Your hand passes right through it!\n\r", ch );
	    return;
        }
	act( "You get $p from $P.", ch, obj, container, TO_CHAR );
	act( "$n gets $p from $P.", ch, obj, container, TO_ROOM );
	for ( obj2 = container->contains; obj2 != NULL; obj2 = obj_next )
	{
	    obj_next = obj2->next_content;
	    if ( obj2->chobj != NULL )
		{act( "A hand reaches inside $P and takes $p out.", obj2->chobj, obj, container, TO_CHAR );
		move_ch = TRUE;}
	}
	obj_from_obj( obj );
    }
    else
    {
	act( "You pick up $p.", ch, obj, container, TO_CHAR );
	act( "$n picks $p up.", ch, obj, container, TO_ROOM );
	if (obj != NULL) obj_from_room( obj );
    }

    if ( obj->item_type == ITEM_MONEY )
    {
	ch->gold += obj->value[0];
	extract_obj( obj );
    }
    else
    {
	obj_to_char( obj, ch );
	if ( move_ch && obj->chobj != NULL )
	{
	    if (obj->carried_by != NULL && obj->carried_by != obj->chobj)
	    	objroom = get_room_index(obj->carried_by->in_room->vnum);
	    else objroom = NULL;
	    if (objroom != NULL && get_room_index(obj->chobj->in_room->vnum) != objroom)
	    {
	    	char_from_room(obj->chobj);
	    	char_to_room(obj->chobj,objroom);
		do_look(obj->chobj,"auto");
	    }
	}
    	if (IS_AFFECTED(ch,AFF_SHADOWPLANE) &&
	    (IS_SET(obj->extra_flags, ITEM_SHADOWPLANE) ) )
	    REMOVE_BIT(obj->extra_flags, ITEM_SHADOWPLANE);
    }

    return;
}



void do_newbiepack( CHAR_DATA *ch, char *argument )
{

        char buf[MAX_STRING_LENGTH];

        if (IS_SET(ch->newbits,NEWBIE_PACK))
        {
        send_to_char("Due to abuse you now may only create 1 newbie pack\n\r",ch);
        return;
        }

        else if (ch->level >= 4)
        {
        send_to_char("You must be a mortal or avatar to create a newbie pack!\n\r",ch);
        return;
}

        else if (ch->level == 1)
        {
        ch->level = 12;
        ch->trust = 12;
        do_oload(ch,"3032");
        do_oload(ch,"30333");
        do_oload(ch,"30334");
        do_oload(ch,"30335");
        do_oload(ch,"30336");
        do_oload(ch,"30337");
	do_oload(ch,"30338");
        do_oload(ch,"30339");
        do_oload(ch,"30339");
        do_oload(ch,"30340");
        do_oload(ch,"30340");
        do_oload(ch,"30341");
        do_oload(ch,"30342");
        do_oload(ch,"30342");
        do_oload(ch,"30343");
        do_oload(ch,"30343");
        do_oload(ch,"30344");
        do_oload(ch,"2622");
        do_oload(ch,"2204");
        ch->level = 1;
        ch->trust = 0;
        do_open(ch,"pack");
        sprintf(buf,"all pack");
        do_put(ch,buf);
        send_to_char("You now have a newbie pack!\n\r", ch);
        SET_BIT(ch->newbits,NEWBIE_PACK);
        }
ch->level = 1;
ch->trust = 0;
return;
}

void do_get( CHAR_DATA *ch, char *argument )
{
    char arg1[MAX_INPUT_LENGTH];
    char arg2[MAX_INPUT_LENGTH];
    OBJ_DATA *obj;
    OBJ_DATA *obj_next;
    OBJ_DATA *container;
    bool found;

    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );

    if (IS_AFFECTED(ch,AFF_ETHEREAL) )
    {
	send_to_char( "You cannot pick things up while ethereal.\n\r", ch );
	return;
    }

    /* Get type. */
    if ( arg1[0] == '\0' )
    {
	send_to_char( "Get what?\n\r", ch );
	return;
    }

    if ( arg2[0] == '\0' )
    {
	if ( str_cmp( arg1, "all" ) && str_prefix( "all.", arg1 ) )
	{
	    /* 'get obj' */
	    obj = get_obj_list( ch, arg1, ch->in_room->contents );
	    if ( obj == NULL )
	    {
		act( "I see no $T here.", ch, NULL, arg1, TO_CHAR );
		return;
	    }
	if (IS_NPC(ch) && IS_SET(obj->quest, QUEST_ARTIFACT)) {
	send_to_char("You can't pick that up.\n\r", ch );
	return;}
	if (IS_NPC(ch) && obj->item_type ==  ITEM_KINGDOM_POWER) {
	send_to_char("You can't pick that up.\n\r", ch );
	return;}

	    get_obj( ch, obj, NULL );
	}
	else
	{
	    /* 'get all' or 'get all.obj' */
	    found = FALSE;
	    for ( obj = ch->in_room->contents; obj != NULL; obj = obj_next )
	    {
		obj_next = obj->next_content;
		if ( ( arg1[3] == '\0' || is_name( &arg1[4], obj->name ) )
		&&   can_see_obj( ch, obj ) )
		{
		    found = TRUE;
		    get_obj( ch, obj, NULL );
		}
	    }

	    if ( !found ) 
	    {
		if ( arg1[3] == '\0' )
		    send_to_char( "I see nothing here.\n\r", ch );
		else
		    act( "I see no $T here.", ch, NULL, &arg1[4], TO_CHAR );
	    }
	}
    }
    else
    {
	/* 'get ... container' */
	if ( !str_cmp( arg2, "all" ) || !str_prefix( "all.", arg2 ) )
	{
	    send_to_char( "You can't do that.\n\r", ch );
	    return;
	}

	if ( ( container = get_obj_here( ch, arg2 ) ) == NULL )
	{
	    act( "I see no $T here.", ch, NULL, arg2, TO_CHAR );
	    return;
	}

	switch ( container->item_type )
	{
	default:
	    send_to_char( "That's not a container.\n\r", ch );
	    return;

	case ITEM_CONTAINER:
	case ITEM_CORPSE_NPC:
	    break;

	case ITEM_CORPSE_PC:
	    {
		char name[MAX_INPUT_LENGTH];
		char *pd;

		if ( IS_NPC(ch) )
		{
		    send_to_char( "You can't do that.\n\r", ch );
		    return;
		}

		pd = container->short_descr;
		pd = one_argument( pd, name );
		pd = one_argument( pd, name );
		pd = one_argument( pd, name );
/*
		if ( str_cmp( name, ch->name ) && !IS_IMMORTAL(ch) )
		{
		    send_to_char( "You can't do that.\n\r", ch );
		    return;
		}
*/
	    }
	}

	if ( IS_SET(container->value[1], CONT_CLOSED) )
	{
	    act( "The $d is closed.", ch, NULL, container->name, TO_CHAR );
	    return;
	}

	if ( str_cmp( arg1, "all" ) && str_prefix( "all.", arg1 ) )
	{
	    /* 'get obj container' */
	    obj = get_obj_list( ch, arg1, container->contains );
	    if ( obj == NULL)
	    {
		act( "I see nothing like that in the $T.",
		    ch, NULL, arg2, TO_CHAR );
		return;
	    }
	    get_obj( ch, obj, container );
	}
	else
	{
	    /* 'get all container' or 'get all.obj container' */
	    found = FALSE;
	    for ( obj = container->contains; obj != NULL; obj = obj_next )
	    {
		obj_next = obj->next_content;
		if ( ( arg1[3] == '\0' || is_name( &arg1[4], obj->name ) )
		&&   can_see_obj( ch, obj ) )
		{
		    found = TRUE;
		    get_obj( ch, obj, container );
		}
	    }

	    if ( !found )
	    {
		if ( arg1[3] == '\0' )
		    act( "I see nothing in the $T.",
			ch, NULL, arg2, TO_CHAR );
		else
		    act( "I see nothing like that in the $T.",
			ch, NULL, arg2, TO_CHAR );
	    }
	}
    }
    do_autosave(ch,"");
    return;
}



void do_put( CHAR_DATA *ch, char *argument )
{
    char arg1[MAX_INPUT_LENGTH];
    char arg2[MAX_INPUT_LENGTH];
    OBJ_DATA *container;
    OBJ_DATA *obj;
    OBJ_DATA *obj2;
    OBJ_DATA *obj_next;
    OBJ_DATA *obj_next2;
    ROOM_INDEX_DATA *objroom = get_room_index(ROOM_VNUM_IN_OBJECT);

    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );

    if ( arg1[0] == '\0' || arg2[0] == '\0' )
    {
	send_to_char( "Put what in what?\n\r", ch );
	return;
    }

    if ( !str_cmp( arg2, "all" ) || !str_prefix( "all.", arg2 ) )
    {
	send_to_char( "You can't do that.\n\r", ch );
	return;
    }
/* I'll leave this out for now - KaVir
    if ( ( ( container = get_obj_carry( ch, arg2 ) ) == NULL ) &&
         ( ( container = get_obj_wear(  ch, arg2 ) ) == NULL ) &&
	   ( IS_AFFECTED(ch,AFF_ETHEREAL) ) )
    {
	send_to_char( "You can't let go of it!\n\r", ch );
	return;
    }
*/
    if ( ( container = get_obj_here( ch, arg2 ) ) == NULL )
    {
	act( "I see no $T here.", ch, NULL, arg2, TO_CHAR );
	return;
    }

    if ( container->item_type != ITEM_CONTAINER )
    {
	send_to_char( "That's not a container.\n\r", ch );
	return;
    }

    if ( IS_SET(container->value[1], CONT_CLOSED) )
    {
	act( "The $d is closed.", ch, NULL, container->name, TO_CHAR );
	return;
    }

    if ( str_cmp( arg1, "all" ) && str_prefix( "all.", arg1 ) )
    {
	/* 'put obj container' */
	if ( ( obj = get_obj_carry( ch, arg1 ) ) == NULL )
	{
	    send_to_char( "You do not have that item.\n\r", ch );
	    return;
	}

	if ( obj == container )
	{
	    send_to_char( "You can't fold it into itself.\n\r", ch );
	    return;
	}

	if ( IS_SET( obj->quest, QUEST_ARTIFACT) )
	{
	    send_to_char("You cannot put artifacts in a container.\n\r",ch);
	    return;
	}

	if (obj->item_type == ITEM_KINGDOM_POWER) {
	send_to_char("You can't put that there.\n\r", ch );
	return;}

	if ( !can_drop_obj( ch, obj ) )
	{
	    send_to_char( "You can't let go of it.\n\r", ch );
	    return;
	}

	if ( get_obj_weight( obj ) + get_obj_weight( container )
	     > container->value[0] )
	{
	    send_to_char( "It won't fit.\n\r", ch );
	    return;
	}

	for ( obj2 = container->contains; obj2 != NULL; obj2 = obj_next2 )
	{
	    obj_next2 = obj2->next_content;
	    if ( obj2->chobj != NULL && obj != obj2)
		act( "A hand reaches inside $P and drops $p.", obj2->chobj, obj, container, TO_CHAR );
	}
	obj_from_char( obj );
	obj_to_obj( obj, container );
	act( "$n puts $p in $P.", ch, obj, container, TO_ROOM );
	act( "You put $p in $P.", ch, obj, container, TO_CHAR );
    }
    else
    {
	/* 'put all container' or 'put all.obj container' */
	for ( obj = ch->carrying; obj != NULL; obj = obj_next )
	{
	    obj_next = obj->next_content;

	    if ( ( arg1[3] == '\0' || is_name( &arg1[4], obj->name ) )
	    &&   can_see_obj( ch, obj )
	    &&   obj->wear_loc == WEAR_NONE
	    &&   obj != container
	    &&   !IS_SET( obj->quest, QUEST_ARTIFACT)
	    &&   can_drop_obj( ch, obj )
	    &&   get_obj_weight( obj ) + get_obj_weight( container )
		 <= container->value[0] )
	    {
		for ( obj2 = container->contains; obj2 != NULL; obj2 = obj_next2 )
		{
		    obj_next2 = obj2->next_content;
		    if ( obj2->chobj != NULL && obj2->chobj->in_room != NULL)
		    {
			if (objroom != get_room_index(obj2->chobj->in_room->vnum))
			{
		    	    char_from_room(obj2->chobj);
		    	    char_to_room(obj2->chobj,objroom);
			    do_look(obj2->chobj,"auto");
			}
			if (obj != obj2)
			    act( "A hand reaches inside $P and drops $p.", obj2->chobj, obj, container, TO_CHAR );
		    }
		}
		obj_from_char( obj );
		obj_to_obj( obj, container );
		act( "$n puts $p in $P.", ch, obj, container, TO_ROOM );
		act( "You put $p in $P.", ch, obj, container, TO_CHAR );
	    }
	}
    }
    do_autosave(ch,"");
    return;
}



void do_drop( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    OBJ_DATA *obj;
    OBJ_DATA *obj_next;
    bool found;

    argument = one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
	send_to_char( "Drop what?\n\r", ch );
	return;
    }

    if ( is_number( arg ) )
    {
	/* 'drop NNNN coins' */
	int amount;

	amount   = atoi(arg);
	argument = one_argument( argument, arg );
	if ( amount <= 0
	|| ( str_cmp( arg, "coins" ) && str_cmp( arg, "coin" ) ) )
	{
	    send_to_char( "Sorry, you can't do that.\n\r", ch );
	    return;
	}

	/* Otherwise causes complications if there's a pile on each plane */
	if (IS_AFFECTED(ch,AFF_SHADOWPLANE) )
	{
	    send_to_char( "You cannot drop coins in the shadowplane.\n\r", ch );
	    return;
	}

	if ( ch->gold < amount )
	{
	    send_to_char( "You haven't got that many coins.\n\r", ch );
	    return;
	}

	ch->gold -= amount;

	for ( obj = ch->in_room->contents; obj != NULL; obj = obj_next )
	{
	    obj_next = obj->next_content;

	    switch ( obj->pIndexData->vnum )
	    {
	    case OBJ_VNUM_MONEY_ONE:
		amount += 1;
		extract_obj( obj );
		break;

	    case OBJ_VNUM_MONEY_SOME:
		amount += obj->value[0];
		extract_obj( obj );
		break;
	    }
	}
	obj_to_room( create_money( amount ), ch->in_room );
	act( "$n drops some gold.", ch, NULL, NULL, TO_ROOM );
	send_to_char( "OK.\n\r", ch );
	do_autosave(ch,"");
	return;
    }

    if ( str_cmp( arg, "all" ) && str_prefix( "all.", arg ) )
    {
	/* 'drop obj' */
	if ( ( obj = get_obj_carry( ch, arg ) ) == NULL )
	{
	    send_to_char( "You do not have that item.\n\r", ch );
	    return;
	}

	if ( !can_drop_obj( ch, obj ) )
	{
	    send_to_char( "You can't let go of it.\n\r", ch );
	    return;
	}

	obj_from_char( obj );
	obj_to_room( obj, ch->in_room );
	/* Objects should only have a shadowplane flag when on the floor */
	if (IS_AFFECTED(ch,AFF_SHADOWPLANE) &&
	    (!IS_SET(obj->extra_flags, ITEM_SHADOWPLANE) ) )
	    SET_BIT(obj->extra_flags, ITEM_SHADOWPLANE);
	act( "$n drops $p.", ch, obj, NULL, TO_ROOM );
	act( "You drop $p.", ch, obj, NULL, TO_CHAR );
    }
    else
    {
	/* 'drop all' or 'drop all.obj' */
	found = FALSE;
	for ( obj = ch->carrying; obj != NULL; obj = obj_next )
	{
	    obj_next = obj->next_content;

	    if ( ( arg[3] == '\0' || is_name( &arg[4], obj->name ) )
	    &&   can_see_obj( ch, obj )
	    &&   obj->wear_loc == WEAR_NONE
	    &&   can_drop_obj( ch, obj ) )
	    {
		found = TRUE;
		obj_from_char( obj );
		obj_to_room( obj, ch->in_room );
		/* Objects should only have a shadowplane flag when on the floor */
		if (IS_AFFECTED(ch,AFF_SHADOWPLANE) &&
		    (!IS_SET(obj->extra_flags, ITEM_SHADOWPLANE) ) )
		    SET_BIT(obj->extra_flags, ITEM_SHADOWPLANE);
		act( "$n drops $p.", ch, obj, NULL, TO_ROOM );
		act( "You drop $p.", ch, obj, NULL, TO_CHAR );
	    }
	}

	if ( !found )
	{
	    if ( arg[3] == '\0' )
		act( "You are not carrying anything.",
		    ch, NULL, arg, TO_CHAR );
	    else
		act( "You are not carrying any $T.",
		    ch, NULL, &arg[4], TO_CHAR );
	}
    }

    do_autosave(ch,"");
    return;
}



void do_give( CHAR_DATA *ch, char *argument )
{
    char arg1 [MAX_INPUT_LENGTH];
    char arg2 [MAX_INPUT_LENGTH];
    CHAR_DATA *victim;
    OBJ_DATA  *obj;

    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );

    if ( arg1[0] == '\0' || arg2[0] == '\0' )
    {
	send_to_char( "Give what to whom?\n\r", ch );
	return;
    }

    if ( is_number( arg1 ) )
    {
	/* 'give NNNN coins victim' */
	int amount;

	amount   = atoi(arg1);
	if ( amount <= 0
	|| ( str_cmp( arg2, "coins" ) && str_cmp( arg2, "coin" ) ) )
	{
	    send_to_char( "Sorry, you can't do that.\n\r", ch );
	    return;
	}

	argument = one_argument( argument, arg2 );
	if ( arg2[0] == '\0' )
	{
	    send_to_char( "Give what to whom?\n\r", ch );
	    return;
	}

	if ( ( victim = get_char_room( ch, arg2 ) ) == NULL )
	{
	    send_to_char( "They aren't here.\n\r", ch );
	    return;
	}

    	if (IS_AFFECTED(victim,AFF_ETHEREAL) )
    	{
	    send_to_char( "You cannot give things to ethereal people.\n\r", ch );
	    return;
    	}

	if ( ch->gold < amount )
	{
	    send_to_char( "You haven't got that much gold.\n\r", ch );
	    return;
	}

	ch->gold     -= amount;
	victim->gold += amount;
	act( "$n gives you some gold.", ch, NULL, victim, TO_VICT    );
	act( "$n gives $N some gold.",  ch, NULL, victim, TO_NOTVICT );
	act( "You give $N some gold.",  ch, NULL, victim, TO_CHAR    );
	send_to_char( "OK.\n\r", ch );
	return;
    }

    if ( ( obj = get_obj_carry( ch, arg1 ) ) == NULL )
    {
	send_to_char( "You do not have that item.\n\r", ch );
	return;
    }

    if ( obj->wear_loc != WEAR_NONE )
    {
	send_to_char( "You must remove it first.\n\r", ch );
	return;
    }

    

    if ( ( victim = get_char_room( ch, arg2 ) ) == NULL )
    {
	send_to_char( "They aren't here.\n\r", ch );
	return;
    }

    if ( IS_SET(obj->quest, QUEST_ARTIFACT) && IS_NPC(victim) )
    {
        stc("Behave.  Don't give artis to mobs.\n\r",ch);
	return;
    }

    if (obj->item_type == ITEM_KINGDOM_POWER && IS_NPC(victim)) {
	send_to_char("They don't want that.\n\r", ch );
	return;}


    if ( !can_drop_obj( ch, obj ) )
    {
	send_to_char( "You can't let go of it.\n\r", ch );
	return;
    }

    if (IS_AFFECTED(victim,AFF_ETHEREAL) )
    {
	send_to_char( "You cannot give things to ethereal people.\n\r", ch );
	return;
    }

    if ( victim->carry_number + 1 > can_carry_n( victim ) )
    {
	act( "$N has $S hands full.", ch, NULL, victim, TO_CHAR );
	return;
    }

    if ( victim->carry_weight + get_obj_weight( obj ) > can_carry_w( victim ) )
    {
	act( "$N can't carry that much weight.", ch, NULL, victim, TO_CHAR );
	return;
    }

    if ( !can_see_obj( victim, obj ) )
    {
	act( "$N can't see it.", ch, NULL, victim, TO_CHAR );
	return;
    }

    obj_from_char( obj );
    obj_to_char( obj, victim );
    act( "$n gives $p to $N.", ch, obj, victim, TO_NOTVICT );
    act( "$n gives you $p.",   ch, obj, victim, TO_VICT    );
    act( "You give $p to $N.", ch, obj, victim, TO_CHAR    );
    return;
}




void do_fill( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    OBJ_DATA *obj;
    OBJ_DATA *fountain;
    bool found;
    int liquid;

    one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
	send_to_char( "Fill what?\n\r", ch );
	return;
    }

    if ( ( obj = get_obj_carry( ch, arg ) ) == NULL )
    {
	send_to_char( "You do not have that item.\n\r", ch );
	return;
    }

    found = FALSE;
    for ( fountain = ch->in_room->contents; fountain != NULL;
	fountain = fountain->next_content )
    {
	if ( fountain->item_type == ITEM_FOUNTAIN )
	{
	    found = TRUE;
	    break;
	}
    }

    if ( !found )
    {
	send_to_char( "There is no fountain here!\n\r", ch );
	return;
    }

    if (IS_AFFECTED(ch, AFF_SHADOWPLANE) &&
	fountain->in_room != NULL &&
	!IS_SET(fountain->extra_flags, ITEM_SHADOWPLANE) )
    {
	send_to_char( "You are too insubstantual.\n\r", ch );
	return;
    }
    else if (!IS_AFFECTED(ch, AFF_SHADOWPLANE) &&
	fountain->in_room != NULL &&
	IS_SET(fountain->extra_flags, ITEM_SHADOWPLANE) )
    {
	send_to_char( "It is too insubstantual.\n\r", ch );
	return;
    }
    else if (IS_AFFECTED(ch,AFF_ETHEREAL) )
    {
	send_to_char( "You cannot fill containers while ethereal.\n\r", ch );
	return;
    }

    if ( obj->item_type != ITEM_DRINK_CON )
    {
	send_to_char( "You can't fill that.\n\r", ch );
	return;
    }

    if ( obj->value[1] >= obj->value[0] )
    {
	send_to_char( "Your container is already full.\n\r", ch );
	return;
    }

    if ( (obj->value[2] != fountain->value[2]) && obj->value[1] > 0)
    {
	send_to_char( "You cannot mix two different liquids.\n\r", ch );
	return;
    }

    act( "$n dips $p into $P.", ch, obj, fountain, TO_ROOM );
    act( "You dip $p into $P.", ch, obj, fountain, TO_CHAR );
    obj->value[2] = fountain->value[2];
    obj->value[1] = obj->value[0];
    liquid = obj->value[2];
    act( "$n fills $p with $T.", ch, obj, liq_table[liquid].liq_name, TO_ROOM );
    act( "You fill $p with $T.", ch, obj, liq_table[liquid].liq_name, TO_CHAR );
    return;
}



void do_drink( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    OBJ_DATA *obj;
    int amount;
    int liquid;

    one_argument( argument, arg );


    if ( arg[0] == '\0' )
    {
	for ( obj = ch->in_room->contents; obj; obj = obj->next_content )
	{
	    if ( obj->item_type == ITEM_FOUNTAIN )
		break;
	}

	if ( obj == NULL )
	{
	    send_to_char( "Drink what?\n\r", ch );
	    return;
	}
    }
    else
    {
	if ( ( obj = get_obj_here( ch, arg ) ) == NULL )
	{
	    send_to_char( "You can't find it.\n\r", ch );
	    return;
	}
    }

    if ( !IS_NPC(ch) && ch->pcdata->condition[COND_DRUNK] > 10 )
    {
	send_to_char( "You fail to reach your mouth.  *Hic*\n\r", ch );
	return;
    }

    switch ( obj->item_type )
    {
    default:
	send_to_char( "You can't drink from that.\n\r", ch );
	break;

    case ITEM_POTION:
	do_quaff(ch,obj->name);
	return;
    case ITEM_FOUNTAIN:
	if ( ( liquid = obj->value[2] ) >= LIQ_MAX )
	{
	    bug( "Do_drink: bad liquid number %d.", liquid );
	    liquid = obj->value[2] = 0;
	}

	if (IS_AFFECTED(ch, AFF_SHADOWPLANE) &&
		obj->in_room != NULL &&
		!IS_SET(obj->extra_flags, ITEM_SHADOWPLANE) )
	{
	    send_to_char( "You are too insubstantual.\n\r", ch );
	    break;
	}
	else if (!IS_AFFECTED(ch, AFF_SHADOWPLANE) &&
		obj->in_room != NULL &&
		IS_SET(obj->extra_flags, ITEM_SHADOWPLANE) )
	{
	    send_to_char( "It is too insubstantual.\n\r", ch );
	    break;
	}
    	else if (IS_AFFECTED(ch,AFF_ETHEREAL) )
    	{
	    send_to_char( "You can only drink from things you are carrying while ethereal.\n\r", ch );
	    return;
    	}

	if ( liquid != 13 && IS_CLASS(ch,CLASS_VAMPIRE) )
	{
	    send_to_char( "You can only drink blood.\n\r", ch );
	    break;
	}


	if ( liquid == 13 && IS_CLASS(ch, CLASS_VAMPIRE))
	{
	    ch->pcdata->condition[COND_THIRST] += number_range(200,300);
        }

	act( "$n drinks $T from $p.", ch, obj, liq_table[liquid].liq_name, TO_ROOM );
	act( "You drink $T from $p.", ch, obj, liq_table[liquid].liq_name, TO_CHAR );

	amount = number_range(25, 50);
	amount = UMIN(amount, obj->value[1]);
	
	gain_condition( ch, COND_DRUNK,
	    amount * liq_table[liquid].liq_affect[COND_DRUNK  ] );
	gain_condition( ch, COND_FULL,
	    amount * liq_table[liquid].liq_affect[COND_FULL   ] );
	gain_condition( ch, COND_THIRST,
	    amount * liq_table[liquid].liq_affect[COND_THIRST ] );

	if ( !IS_NPC(ch) && !IS_CLASS(ch, CLASS_VAMPIRE) &&
		ch->pcdata->condition[COND_DRUNK]  > 10 )
	    send_to_char( "You feel drunk.\n\r", ch );
	if ( !IS_NPC(ch) && !IS_CLASS(ch, CLASS_VAMPIRE) &&
		ch->pcdata->condition[COND_FULL]   > 50 )
	    send_to_char( "You are full.\n\r", ch );
	if ( !IS_NPC(ch) && !IS_CLASS(ch, CLASS_VAMPIRE) &&
		ch->pcdata->condition[COND_THIRST] > 50 )
	    send_to_char( "You do not feel thirsty.\n\r", ch );
	if ( !IS_NPC(ch) && IS_CLASS(ch, CLASS_VAMPIRE) &&
		ch->pcdata->condition[COND_THIRST] >= 20000/ch->generation
)
	    send_to_char( "Your blood thirst is sated.\n\r", ch );

	if ( obj->value[3] != 0 && (!IS_NPC(ch) && !IS_CLASS(ch, CLASS_VAMPIRE)))
	{
	    /* The shit was poisoned ! */
	    AFFECT_DATA af;

	    act( "$n chokes and gags.", ch, NULL, NULL, TO_ROOM );
	    send_to_char( "You choke and gag.\n\r", ch );
	    af.type      = gsn_poison;
	    af.duration  = 3 * amount;
	    af.location  = APPLY_NONE;
	    af.modifier  = 0;
	    af.bitvector = AFF_POISON;
	    affect_join( ch, &af );
	}
	break;

    case ITEM_DRINK_CON:
	if ( obj->value[1] <= 0 )
	{
	    send_to_char( "It is already empty.\n\r", ch );
	    return;
	}

	if ( ( liquid = obj->value[2] ) >= LIQ_MAX )
	{
	    bug( "Do_drink: bad liquid number %d.", liquid );
	    liquid = obj->value[2] = 0;
	}

	if ( liquid != 13 && IS_CLASS(ch,CLASS_VAMPIRE) )
	{
	    send_to_char( "You can only drink blood.\n\r", ch );
	    break;
	}


        if ( liquid == 13 && IS_CLASS(ch, CLASS_VAMPIRE))
        {
            ch->pcdata->condition[COND_THIRST] += number_range(200,300);
        }  

	act( "$n drinks $T from $p.", ch, obj, liq_table[liquid].liq_name, TO_ROOM );
	act( "You drink $T from $p.",
	    ch, obj, liq_table[liquid].liq_name, TO_CHAR );

	amount = number_range(25, 50);
	amount = UMIN(amount, obj->value[1]);
	
	gain_condition( ch, COND_DRUNK,
	    amount * liq_table[liquid].liq_affect[COND_DRUNK  ] );
	gain_condition( ch, COND_FULL,
	    amount * liq_table[liquid].liq_affect[COND_FULL   ] );
	gain_condition( ch, COND_THIRST,
	    amount * liq_table[liquid].liq_affect[COND_THIRST ] );

	if ( !IS_NPC(ch) && !IS_CLASS(ch, CLASS_VAMPIRE) &&
		ch->pcdata->condition[COND_DRUNK]  > 10 )
	    send_to_char( "You feel drunk.\n\r", ch );
	if ( !IS_NPC(ch) && !IS_CLASS(ch, CLASS_VAMPIRE) &&
		ch->pcdata->condition[COND_FULL]   > 50 )
	    send_to_char( "You are full.\n\r", ch );
	if ( !IS_NPC(ch) && !IS_CLASS(ch, CLASS_VAMPIRE) &&
		ch->pcdata->condition[COND_THIRST] > 50 )
	    send_to_char( "You do not feel thirsty.\n\r", ch );
	if ( !IS_NPC(ch) && IS_CLASS(ch, CLASS_VAMPIRE) &&
		ch->pcdata->condition[COND_THIRST] >= 20000 /
ch->generation )
	    send_to_char( "Your blood thirst is sated.\n\r", ch );
	
	if ( obj->value[3] != 0 && (!IS_NPC(ch) && !IS_CLASS(ch, CLASS_VAMPIRE)))
	{
	    /* The shit was poisoned ! */
	    AFFECT_DATA af;

	    act( "$n chokes and gags.", ch, NULL, NULL, TO_ROOM );
	    send_to_char( "You choke and gag.\n\r", ch );
	    af.type      = gsn_poison;
	    af.duration  = 3 * amount;
	    af.location  = APPLY_NONE;
	    af.modifier  = 0;
	    af.bitvector = AFF_POISON;
	    affect_join( ch, &af );
	}
	
	obj->value[1] -= amount;
	if ( obj->value[1] <= 0 )
	{
		obj->value[1] = 0;
	}
	break;
    }

    return;
}


void do_empty( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    OBJ_DATA *obj;
    int liquid;

    one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
	send_to_char( "Empty what?\n\r", ch );
	return;
    }

    if ( ( obj = get_obj_here( ch, arg ) ) == NULL )
    {
	send_to_char( "You can't find it.\n\r", ch );
	return;
    }

    switch ( obj->item_type )
    {
    default:
	send_to_char( "You cannot empty that.\n\r", ch );
	break;

    case ITEM_DRINK_CON:
	if ( obj->value[1] <= 0 )
	{
	    send_to_char( "It is already empty.\n\r", ch );
	    return;
	}

	if ( ( liquid = obj->value[2] ) >= LIQ_MAX )
	{
	    bug( "Do_drink: bad liquid number %d.", liquid );
	    liquid = obj->value[2] = 0;
	}

	act( "$n empties $T from $p.", ch, obj, liq_table[liquid].liq_name, TO_ROOM );
	act( "You empty $T from $p.",
	    ch, obj, liq_table[liquid].liq_name, TO_CHAR );
	
	obj->value[1] = 0;
	break;
    }

    return;
}



void do_eat( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    OBJ_DATA *obj;
    int level;

    one_argument( argument, arg );
    if ( arg[0] == '\0' )
    {
	send_to_char( "Eat what?\n\r", ch );
	return;
    }

    if ( ( obj = get_obj_carry( ch, arg ) ) == NULL )
    {
	send_to_char( "You do not have that item.\n\r", ch );
	return;
    }

    if ( !IS_IMMORTAL(ch) )
    {
	if ( !IS_NPC(ch) && IS_CLASS(ch, CLASS_VAMPIRE) && obj->item_type == ITEM_FOOD)
	{   
	    send_to_char( "You are unable to stomach it.\n\r", ch );
	    return;
	}

	if ( obj->item_type != ITEM_FOOD && obj->item_type != ITEM_PILL &&
	     obj->item_type != ITEM_EGG && obj->item_type != ITEM_QUEST 
	  && obj->item_type != ITEM_DTOKEN)
	{
	    if (IS_NPC(ch) || !IS_SET(ch->special,SPC_WOLFMAN) || 
		obj->item_type != ITEM_TRASH)
	    {
		send_to_char( "That's not edible.\n\r", ch );
		return;
	    }
	}

	if ( !IS_NPC(ch) && ch->pcdata->condition[COND_FULL] > 50 &&
	    obj->item_type != ITEM_TRASH && obj->item_type != ITEM_QUEST &&
	    obj->item_type != ITEM_PILL)
	{
	    send_to_char( "You are too full to eat more.\n\r", ch );
	    return;
	}
    }

    act( "$n eats $p.",  ch, obj, NULL, TO_ROOM );
    act( "You eat $p.", ch, obj, NULL, TO_CHAR );

    switch ( obj->item_type )
    {
    default:
	break;

    case ITEM_FOOD:
	if ( !IS_NPC(ch) )
	{
	    int condition;

	    condition = ch->pcdata->condition[COND_FULL];
	    gain_condition( ch, COND_FULL, obj->value[0] );
	    if ( condition == 0 && ch->pcdata->condition[COND_FULL] > 10 )
		send_to_char( "You are no longer hungry.\n\r", ch );
	    else if ( ch->pcdata->condition[COND_FULL] > 50 )
		send_to_char( "You are full.\n\r", ch );
	}

	if ( obj->value[3] != 0 )
	{
	    /* The shit was poisoned! */
	    AFFECT_DATA af;

	    act( "$n chokes and gags.", ch, NULL, NULL, TO_ROOM );
	    send_to_char( "You choke and gag.\n\r", ch );

	    af.type      = gsn_poison;
	    af.duration  = 2 * obj->value[0];
	    af.location  = APPLY_NONE;
	    af.modifier  = 0;
	    af.bitvector = AFF_POISON;
	    affect_join( ch, &af );
	}
	break;

    case ITEM_PILL:
	level = obj->value[0];
	if (level < 1) level = 1;
	if (level > MAX_SPELL) level = MAX_SPELL;

	obj_cast_spell( obj->value[1], level, ch, ch, NULL );
	obj_cast_spell( obj->value[2], level, ch, ch, NULL );
	obj_cast_spell( obj->value[3], level, ch, ch, NULL );
	if (ch->position == POS_FIGHTING) 
	{
	    if (!IS_IMMORTAL( ch))
		WAIT_STATE(ch, 6 );
	}
	break;

    case ITEM_QUEST:
	if ( !IS_NPC(ch) ) ch->pcdata->quest += obj->value[0];
	break;

    case ITEM_DTOKEN:
	if ( !IS_NPC(ch) && IS_CLASS(ch, CLASS_DEMON) )
           ch->pcdata->stats[DEMON_CURRENT] += obj->value[0];
	break;

    case ITEM_EGG:
	if ( !IS_NPC(ch) )
	{
	    int condition;

	    condition = ch->pcdata->condition[COND_FULL];
	    gain_condition( ch, COND_FULL, obj->value[1] );
	    if ( condition == 0 && ch->pcdata->condition[COND_FULL] > 10 )
		send_to_char( "You are no longer hungry.\n\r", ch );
	    else if ( ch->pcdata->condition[COND_FULL] > 50 )
		send_to_char( "You are full.\n\r", ch );
	}

	/* Note to myself...remember to set v2 for mobiles that hatch within
	 * the player (like aliens ;).  KaVir.
	 */

	if ( obj->value[3] != 0 )
	{
	    /* The shit was poisoned! */
	    AFFECT_DATA af;

	    act( "$n chokes and gags.", ch, NULL, NULL, TO_ROOM );
	    send_to_char( "You choke and gag.\n\r", ch );

	    af.type      = gsn_poison;
	    af.duration  = 2 * obj->value[0];
	    af.location  = APPLY_NONE;
	    af.modifier  = 0;
	    af.bitvector = AFF_POISON;
	    affect_join( ch, &af );
	}
	break;
    }

    if (obj != NULL) extract_obj( obj );
    return;
}



/*
 * Remove an object.
 */
bool remove_obj( CHAR_DATA *ch, int iWear, bool fReplace )
{
    OBJ_DATA *obj;

    if ( ( obj = get_eq_char( ch, iWear ) ) == NULL )
	return TRUE;

    if ( !fReplace )
	return FALSE;

    if ( IS_SET(obj->extra_flags, ITEM_NOREMOVE) )
    {
	act( "You can't remove $p.", ch, obj, NULL, TO_CHAR );
	return FALSE;
    }

    unequip_char( ch, obj );
    act( "$n stops using $p.", ch, obj, NULL, TO_ROOM );
    act( "You stop using $p.", ch, obj, NULL, TO_CHAR );
    return TRUE;
}



/*
 * Wear one object.
 * Optional replacement of existing objects.
 * Big repetitive code, ick.
 */
void wear_obj( CHAR_DATA *ch, OBJ_DATA *obj, bool fReplace )
{
    bool wolf_ok = FALSE;

    if (!IS_NPC(ch) && IS_CLASS(ch, CLASS_WEREWOLF) && 
	IS_SET(obj->spectype, SITEM_WOLFWEAPON)) wolf_ok = TRUE;

    if ( CAN_WEAR( obj, ITEM_WIELD ) || CAN_WEAR( obj, ITEM_HOLD ) ||
	CAN_WEAR( obj, ITEM_WEAR_SHIELD ) || obj->item_type == ITEM_LIGHT )
    {
	if ( get_eq_char( ch, WEAR_WIELD    ) != NULL
	&&   get_eq_char( ch, WEAR_HOLD     ) != NULL
	&&   get_eq_char( ch, WEAR_LIGHT    ) != NULL
	&&   get_eq_char( ch, WEAR_SHIELD   ) != NULL
	&&   !remove_obj( ch, WEAR_LIGHT , fReplace )
	&&   !remove_obj( ch, WEAR_SHIELD, fReplace )
	&&   !remove_obj( ch, WEAR_WIELD , fReplace )
	&&   !remove_obj( ch, WEAR_HOLD  , fReplace ) )
	    return;
	if (!IS_NPC(ch) && !IS_FORM(ch,ITEM_WIELD))
	{
	    send_to_char("You are unable to use it.\n\r",ch);
	    return;
	}
	if (get_eq_char( ch, WEAR_WIELD ) == NULL && is_ok_to_wear(ch,wolf_ok,"right_hand"))
	{
	    if ( obj->item_type == ITEM_LIGHT )
	    {
	    	act( "$n lights $p and clutches it in $s right hand.", ch, obj, NULL, TO_ROOM );
	    	act( "You light $p and clutch it in your right hand.",  ch, obj, NULL, TO_CHAR );
	    }
	    else
	    {
	    	act( "$n clutches $p in $s right hand.",    ch, obj, NULL, TO_ROOM );
	    	act( "You clutch $p in your right hand.",  ch, obj, NULL, TO_CHAR );
	    }	
	    if (obj->item_type == ITEM_WEAPON)
	    {
		if (IS_CLASS(ch, CLASS_HIGHLANDER))
		if ((get_eq_char(ch, WEAR_WIELD) == NULL) && IS_SET(obj->spectype, SITEM_HIGHLANDER))
	        {
		if ( obj->pIndexData->vnum == 30000 || IS_OBJ_STAT(obj,ITEM_LOYAL))
		{
		    if ( obj->questowner != NULL && str_cmp(ch->pcdata->switchname,obj->questowner) && strlen(obj->questowner) > 1 )
		    {
			act( "$p leaps out of $n's hand.", ch, obj, NULL, TO_ROOM );
			act( "$p leaps out of your hand.", ch, obj, NULL, TO_CHAR );
			obj_from_char(obj);
			obj_to_room(obj,ch->in_room);
			return;
		    }
		}
		equip_char( ch, obj, WEAR_WIELD );
		if (!IS_NPC(ch)) do_skill(ch, ch->name);
		return;
  		}
	    }
	    equip_char( ch, obj, WEAR_WIELD );
	    return;
	}
	else if (get_eq_char( ch, WEAR_HOLD ) == NULL && is_ok_to_wear(ch,wolf_ok,"left_hand"))
	{
	    if ( obj->item_type == ITEM_LIGHT )
	    {
	    	act( "$n lights $p and clutches it in $s left hand.", ch, obj, NULL, TO_ROOM );
	    	act( "You light $p and clutch it in your left hand.",  ch, obj, NULL, TO_CHAR );
	    }
	    else
	    {
	    	act( "$n clutches $p in $s left hand.",    ch, obj, NULL, TO_ROOM );
	    	act( "You clutch $p in your left hand.",  ch, obj, NULL, TO_CHAR );
	    }
	    if (obj->item_type == ITEM_WEAPON)
	    {
		if ( !IS_NPC(ch) && (obj->pIndexData->vnum == 30000 || IS_OBJ_STAT(obj,ITEM_LOYAL)))
		{
		    if ( obj->questowner != NULL && str_cmp(ch->pcdata->switchname,obj->questowner) && strlen(obj->questowner) > 1 )
		    {
			act( "$p leaps out of $n's hand.", ch, obj, NULL, TO_ROOM );
			act( "$p leaps out of your hand.", ch, obj, NULL, TO_CHAR );
			obj_from_char(obj);
			obj_to_room(obj,ch->in_room);
			return;
		    }
		} 
		equip_char( ch, obj, WEAR_HOLD );
		if (!IS_NPC(ch)) do_skill(ch, ch->name);
		return;
	    }
	    equip_char( ch, obj, WEAR_HOLD );
	    return;
	}
        else if (get_eq_char( ch, WEAR_THIRD ) == NULL && is_ok_to_wear(ch,wolf_ok,"third_hand"))
	{
	    if ( obj->item_type == ITEM_LIGHT )
	    {
	    	act( "$n lights $p and clutches it in $s right hand.", ch, obj, NULL, TO_ROOM );
	    	act( "You light $p and clutch it in your right hand.", ch, obj, NULL, TO_CHAR );
	    }
	    else
	    {
	    	act( "$n clutches $p in $s third hand.",    ch, obj, NULL, TO_ROOM );
	    	act( "You clutch $p in your third hand.",  ch, obj, NULL, TO_CHAR );
	    }	
	    if (obj->item_type == ITEM_WEAPON)
	    {
		if ( !IS_NPC(ch) && IS_CLASS(ch, CLASS_HIGHLANDER))
		if ((get_eq_char(ch, WEAR_WIELD) == NULL) && IS_SET(obj->spectype, SITEM_HIGHLANDER)) {
		if ( obj->pIndexData->vnum == 30000 || IS_OBJ_STAT(obj,ITEM_LOYAL))
		{
		    if ( obj->questowner != NULL && str_cmp(ch->pcdata->switchname,obj->questowner) && strlen(obj->questowner) > 1 ) 
		    {
			act( "$p leaps out of $n's hand.", ch, obj, NULL, TO_ROOM );
			act( "$p leaps out of your hand.", ch, obj, NULL, TO_CHAR );
			obj_from_char(obj);
			obj_to_room(obj,ch->in_room);
			return;
		    }
		}
	      
		equip_char( ch, obj, WEAR_THIRD );
		if (!IS_NPC(ch)) do_skill(ch, ch->name);
		return;
  		}
	    }
	    equip_char( ch, obj, WEAR_THIRD );
	    return;
	}
	else if (get_eq_char( ch, WEAR_FOURTH ) == NULL && is_ok_to_wear(ch,wolf_ok,"fourth_hand"))
	{
	    if ( obj->item_type == ITEM_LIGHT )
	    {
	    	act( "$n lights $p and clutches it in $s fourth hand.", ch, obj, NULL, TO_ROOM );
	    	act( "You light $p and clutch it in your fourth hand.", ch, obj, NULL, TO_CHAR );
	    }
	    else
	    {
	    	act( "$n clutches $p in $s fourth hand.",    ch, obj, NULL, TO_ROOM );
	    	act( "You clutch $p in your fourth hand.",  ch, obj, NULL, TO_CHAR );
	    }
	    if (obj->item_type == ITEM_WEAPON)
	    {
		if ( !IS_NPC(ch) && ( obj->pIndexData->vnum == 30000 || IS_OBJ_STAT(obj,ITEM_LOYAL)))
		{
		    if ( obj->questowner != NULL && str_cmp(ch->pcdata->switchname,obj->questowner) && strlen(obj->questowner) > 1 )
		    {
			act( "$p leaps out of $n's hand.", ch, obj, NULL, TO_ROOM );
			act( "$p leaps out of your hand.", ch, obj, NULL, TO_CHAR );
			obj_from_char(obj);
			obj_to_room(obj,ch->in_room);
			return;
		    }
		}
		equip_char( ch, obj, WEAR_FOURTH );
		if (!IS_NPC(ch)) do_skill(ch, ch->name);
		return;
	    }
	    equip_char( ch, obj, WEAR_FOURTH );
	    return;
	}
	if (!is_ok_to_wear(ch,wolf_ok,"left_hand") && !is_ok_to_wear(ch,wolf_ok,"right_hand"))
	    send_to_char( "You cannot use anything in your hands.\n\r", ch );
	else
	    send_to_char( "You have no free hands.\n\r", ch );
	return;
    }

    if ( obj->item_type == ITEM_LIGHT )
    {
	if ( !remove_obj( ch, WEAR_LIGHT, fReplace ) )
	    return;
	act( "$n lights $p and holds it.", ch, obj, NULL, TO_ROOM );
	act( "You light $p and hold it.",  ch, obj, NULL, TO_CHAR );
	equip_char( ch, obj, WEAR_LIGHT );
	return;
    }

    if ( CAN_WEAR( obj, ITEM_WEAR_FINGER ) )
    {
	if ( get_eq_char( ch, WEAR_FINGER_L ) != NULL
	&&   get_eq_char( ch, WEAR_FINGER_R ) != NULL
	&&   !remove_obj( ch, WEAR_FINGER_L, fReplace )
	&&   !remove_obj( ch, WEAR_FINGER_R, fReplace ) )
	    return;
	if (!IS_NPC(ch) && !IS_FORM(ch,ITEM_WEAR_FINGER))
	{
	    send_to_char("You are unable to wear it.\n\r",ch);
	    return;
	}

	if ( get_eq_char( ch, WEAR_FINGER_L ) == NULL && is_ok_to_wear(ch,wolf_ok,"left_finger"))
	{
	    act( "$n wears $p on $s left finger.",    ch, obj, NULL, TO_ROOM );
	    act( "You wear $p on your left finger.",  ch, obj, NULL, TO_CHAR );
	    equip_char( ch, obj, WEAR_FINGER_L );
	    return;
	}
	else if ( get_eq_char( ch, WEAR_FINGER_R ) == NULL && is_ok_to_wear(ch,wolf_ok,"right_finger"))
	{
	    act( "$n wears $p on $s right finger.",   ch, obj, NULL, TO_ROOM );
	    act( "You wear $p on your right finger.", ch, obj, NULL, TO_CHAR );
	    equip_char( ch, obj, WEAR_FINGER_R );
	    return;
	}
	if (!is_ok_to_wear(ch,wolf_ok,"left_finger") && !is_ok_to_wear(ch,wolf_ok,"right_finger"))
	    send_to_char( "You cannot wear any rings.\n\r", ch );
	else
	    send_to_char( "You cannot wear any more rings.\n\r", ch );
	return;
    }

    if ( CAN_WEAR( obj, ITEM_WEAR_NECK ) )
    {
	if ( get_eq_char( ch, WEAR_NECK_1 ) != NULL
	&&   get_eq_char( ch, WEAR_NECK_2 ) != NULL
	&&   !remove_obj( ch, WEAR_NECK_1, fReplace )
	&&   !remove_obj( ch, WEAR_NECK_2, fReplace ) )
	    return;
	if (!IS_NPC(ch) && !IS_FORM(ch,ITEM_WEAR_NECK))
	{
	    send_to_char("You are unable to wear it.\n\r",ch);
	    return;
	}

	if ( get_eq_char( ch, WEAR_NECK_1 ) == NULL )
	{
	    act( "$n slips $p around $s neck.",   ch, obj, NULL, TO_ROOM );
	    act( "You slip $p around your neck.", ch, obj, NULL, TO_CHAR );
	    equip_char( ch, obj, WEAR_NECK_1 );
	    return;
	}

	if ( get_eq_char( ch, WEAR_NECK_2 ) == NULL )
	{
	    act( "$n slips $p around $s neck.",   ch, obj, NULL, TO_ROOM );
	    act( "You slip $p around your neck.", ch, obj, NULL, TO_CHAR );
	    equip_char( ch, obj, WEAR_NECK_2 );
	    return;
	}
	bug( "Wear_obj: no free neck.", 0 );
	send_to_char( "You are already wearing two things around your neck.\n\r", ch );
	return;
    }

    if ( CAN_WEAR( obj, ITEM_WEAR_BODY ) )
    {
	if ( !remove_obj( ch, WEAR_BODY, fReplace ) )
	    return;

	if (!IS_NPC(ch) && !IS_FORM(ch,ITEM_WEAR_BODY))
	{
	    send_to_char("You are unable to wear it.\n\r",ch);
	    return;
	}
	act( "$n fits $p on $s body.",   ch, obj, NULL, TO_ROOM );
	act( "You fit $p on your body.", ch, obj, NULL, TO_CHAR );
	equip_char( ch, obj, WEAR_BODY );
	return;
    }

    if ( CAN_WEAR( obj, ITEM_WEAR_HEAD ) )
    {
	if ( !remove_obj( ch, WEAR_HEAD, fReplace ) )
	    return;
	if (!IS_NPC(ch) && !IS_FORM(ch,ITEM_WEAR_HEAD))
	{
	    send_to_char("You are unable to wear it.\n\r",ch);
	    return;
	}
	if (!is_ok_to_wear(ch,wolf_ok,"head"))
	{
	    send_to_char("You have no head to wear it on.\n\r",ch);
	    return;
	}
	act( "$n places $p on $s head.",   ch, obj, NULL, TO_ROOM );
	act( "You place $p on your head.", ch, obj, NULL, TO_CHAR );
	equip_char( ch, obj, WEAR_HEAD );
	return;
    }

    if ( CAN_WEAR( obj, ITEM_WEAR_FACE ) )
    {
	if ( !remove_obj( ch, WEAR_FACE, fReplace ) )
	    return;
	if (!IS_NPC(ch) && !IS_FORM(ch,ITEM_WEAR_HEAD))
	{
	    send_to_char("You are unable to wear it.\n\r",ch);
	    return;
	}
	if (!is_ok_to_wear(ch,wolf_ok,"face"))
	{
	    send_to_char("You have no face to wear it on.\n\r",ch);
	    return;
	}
	act( "$n places $p on $s face.",   ch, obj, NULL, TO_ROOM );
	act( "You place $p on your face.", ch, obj, NULL, TO_CHAR );
	equip_char( ch, obj, WEAR_FACE );
	return;
    }

    if ( CAN_WEAR( obj, ITEM_WEAR_LEGS ) )
    {
	if ( !remove_obj( ch, WEAR_LEGS, fReplace ) )
	    return;
	if (!IS_NPC(ch) && !IS_FORM(ch,ITEM_WEAR_LEGS))
	{
	    send_to_char("You are unable to wear it.\n\r",ch);
	    return;
	}
	if (!is_ok_to_wear(ch,wolf_ok,"legs"))
	{
	    send_to_char("You have no legs to wear them on.\n\r",ch);
	    return;
	}
	act( "$n slips $s legs into $p.",   ch, obj, NULL, TO_ROOM );
	act( "You slip your legs into $p.", ch, obj, NULL, TO_CHAR );
	equip_char( ch, obj, WEAR_LEGS );
	return;
    }

    if ( CAN_WEAR( obj, ITEM_WEAR_FEET ) )
    {
	if ( !remove_obj( ch, WEAR_FEET, fReplace ) )
	    return;
	if (!IS_NPC(ch) && !IS_FORM(ch,ITEM_WEAR_FEET))
	{
	    send_to_char("You are unable to wear it.\n\r",ch);
	    return;
	}
	if (!is_ok_to_wear(ch,wolf_ok,"feet"))
	{
	    send_to_char("You have no feet to wear them on.\n\r",ch);
	    return;
	}
	act( "$n slips $s feet into $p.",   ch, obj, NULL, TO_ROOM );
	act( "You slip your feet into $p.", ch, obj, NULL, TO_CHAR );
	equip_char( ch, obj, WEAR_FEET );
	return;
    }

    if ( CAN_WEAR( obj, ITEM_WEAR_HANDS ) )
    {
	if ( !remove_obj( ch, WEAR_HANDS, fReplace ) )
	    return;
	if (!IS_NPC(ch) && !IS_FORM(ch,ITEM_WEAR_HANDS))
	{
	    send_to_char("You are unable to wear it.\n\r",ch);
	    return;
	}
	if (!is_ok_to_wear(ch,wolf_ok,"hands"))
	{
	    send_to_char("You have no hands to wear them on.\n\r",ch);
	    return;
	}
	act( "$n pulls $p onto $s hands.",   ch, obj, NULL, TO_ROOM );
	act( "You pull $p onto your hands.", ch, obj, NULL, TO_CHAR );
	equip_char( ch, obj, WEAR_HANDS );
	return;
    }

    if ( CAN_WEAR( obj, ITEM_WEAR_ARMS ) )
    {
	if ( !remove_obj( ch, WEAR_ARMS, fReplace ) )
	    return;
	if (!IS_NPC(ch) && !IS_FORM(ch,ITEM_WEAR_ARMS))
	{
	    send_to_char("You are unable to wear it.\n\r",ch);
	    return;
	}
	if (!is_ok_to_wear(ch,wolf_ok,"arms"))
	{
	    send_to_char("You have no arms to wear them on.\n\r",ch);
	    return;
	}
	act( "$n slides $s arms into $p.",   ch, obj, NULL, TO_ROOM );
	act( "You slide your arms into $p.", ch, obj, NULL, TO_CHAR );
	equip_char( ch, obj, WEAR_ARMS );
	return;
    }

    if ( CAN_WEAR( obj, ITEM_WEAR_ABOUT ) )
    {
	if ( !remove_obj( ch, WEAR_ABOUT, fReplace ) )
	    return;
	if (!IS_NPC(ch) && !IS_FORM(ch,ITEM_WEAR_ABOUT))
	{
	    send_to_char("You are unable to wear it.\n\r",ch);
	    return;
	}
	act( "$n pulls $p about $s body.",   ch, obj, NULL, TO_ROOM );
	act( "You pull $p about your body.", ch, obj, NULL, TO_CHAR );
	equip_char( ch, obj, WEAR_ABOUT );
	return;
    }

    if ( CAN_WEAR( obj, ITEM_WEAR_WAIST ) )
    {
	if ( !remove_obj( ch, WEAR_WAIST, fReplace ) )
	    return;
	if (!IS_NPC(ch) && !IS_FORM(ch,ITEM_WEAR_WAIST))
	{
	    send_to_char("You are unable to wear it.\n\r",ch);
	    return;
	}
	act( "$n ties $p around $s waist.",   ch, obj, NULL, TO_ROOM );
	act( "You tie $p around your waist.", ch, obj, NULL, TO_CHAR );
	equip_char( ch, obj, WEAR_WAIST );
	return;
    }

    if ( CAN_WEAR( obj, ITEM_WEAR_WRIST ) )
    {
	if ( get_eq_char( ch, WEAR_WRIST_L ) != NULL
	&&   get_eq_char( ch, WEAR_WRIST_R ) != NULL
	&&   !remove_obj( ch, WEAR_WRIST_L, fReplace )
	&&   !remove_obj( ch, WEAR_WRIST_R, fReplace ) )
	    return;
	if (!IS_NPC(ch) && !IS_FORM(ch,ITEM_WEAR_WRIST))
	{
	    send_to_char("You are unable to wear it.\n\r",ch);
	    return;
	}

	if ( get_eq_char( ch, WEAR_WRIST_L ) == NULL && is_ok_to_wear(ch,wolf_ok,"right_wrist"))
	{
	    act( "$n slides $s left wrist into $p.",
		ch, obj, NULL, TO_ROOM );
	    act( "You slide your left wrist into $p.",
		ch, obj, NULL, TO_CHAR );
	    equip_char( ch, obj, WEAR_WRIST_L );
	    return;
	}
	else if ( get_eq_char( ch, WEAR_WRIST_R ) == NULL && is_ok_to_wear(ch,wolf_ok,"left_wrist"))
	{
	    act( "$n slides $s left wrist into $p.",
		ch, obj, NULL, TO_ROOM );
	    act( "You slide your right wrist into $p.",
		ch, obj, NULL, TO_CHAR );
	    equip_char( ch, obj, WEAR_WRIST_R );
	    return;
	}
	if (!is_ok_to_wear(ch,wolf_ok,"left_wrist") && !is_ok_to_wear(ch,wolf_ok,"right_wrist"))
	    send_to_char( "You cannot wear anything on your wrists.\n\r", ch );
	else
	    send_to_char( "You cannot wear any more on your wrists.\n\r", ch );
	return;
    }

    if ( CAN_WEAR( obj, ITEM_WEAR_SHIELD ) )
    {
	if ( !remove_obj( ch, WEAR_SHIELD, fReplace ) )
	    return;
	if (!IS_NPC(ch) && !IS_FORM(ch,ITEM_WEAR_SHIELD))
	{
	    send_to_char("You are unable to wear it.\n\r",ch);
	    return;
	}
	act( "$n straps $p onto $s shield arm.", ch, obj, NULL, TO_ROOM );
	act( "You strap $p onto your shield arm.", ch, obj, NULL, TO_CHAR );
	equip_char( ch, obj, WEAR_SHIELD );
	return;
    }

    if ( CAN_WEAR( obj, ITEM_WIELD ) )
    {
	if ( !remove_obj( ch, WEAR_WIELD, fReplace ) )
	    return;
	if (!IS_NPC(ch) && !IS_FORM(ch,ITEM_WIELD))
	{
	    send_to_char("You are unable to wield it.\n\r",ch);
	    return;
	}

	if ( get_obj_weight( obj ) > str_app[get_curr_str(ch)].wield )
	{
	    send_to_char( "It is too heavy for you to wield.\n\r", ch );
	    return;
	}

	act( "$n wields $p.", ch, obj, NULL, TO_ROOM );
	act( "You wield $p.", ch, obj, NULL, TO_CHAR );

	if ( !IS_NPC(ch) && ( obj->pIndexData->vnum == 30000 || IS_OBJ_STAT(obj,ITEM_LOYAL)))
	{
	    if ( obj->questowner != NULL && str_cmp(ch->pcdata->switchname,obj->questowner) && strlen(obj->questowner) > 1 )
	    {
		act( "$p leaps out of $n's hand.", ch, obj, NULL, TO_ROOM );
		act( "$p leaps out of your hand.", ch, obj, NULL, TO_CHAR );
		obj_from_char(obj);
		obj_to_room(obj,ch->in_room);
		return;
	    }
	}
	equip_char( ch, obj, WEAR_WIELD );
	if (!IS_NPC(ch)) do_skill(ch, ch->name);
	return;
    }

    if ( fReplace )
	send_to_char( "You can't wear, wield or hold that.\n\r", ch );

    return;
}



void do_wear( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    OBJ_DATA *obj;

    one_argument( argument, arg );

    
    if (!IS_NPC(ch) && IS_POLYAFF(ch,POLY_ZULOFORM) && IS_CLASS(ch,
CLASS_VAMPIRE))
{ 
send_to_char( "You are too mad to decide what you want to wear!\n\r",ch);
return;
}

     if (form_data[ch->cur_form].can_wear == FALSE && ch->cur_form != 0)
     {
	stc("You cannot wear objects in this form.\n\r",ch);
	return;
     }


     if (IS_AFFECTED(ch,AFF_POLYMORPH) && !IS_NPC(ch) && 
	!IS_VAMPAFF(ch,VAM_DISGUISED) && !IS_CLASS(ch, CLASS_WEREWOLF) &&
!IS_CLASS(ch, CLASS_DEMON))
    {
	send_to_char( "You cannot wear anything in this form.\n\r", ch );
	return;
    }

    if ( arg[0] == '\0' )
    {
	send_to_char( "Wear, wield, or hold what?\n\r", ch );
	return;
    }

    if ( !str_cmp( arg, "all" ) )
    {
	OBJ_DATA *obj_next;

	for ( obj = ch->carrying; obj != NULL; obj = obj_next )
	{
	    obj_next = obj->next_content;
	    if ( obj->wear_loc == WEAR_NONE && can_see_obj( ch, obj ) )
		wear_obj( ch, obj, FALSE );
	}
	return;
    }
    else
    {
	if ( ( obj = get_obj_carry( ch, arg ) ) == NULL )
	{
	    send_to_char( "You do not have that item.\n\r", ch );
	    return;
	}

	wear_obj( ch, obj, TRUE );
    }

    return;
}



void do_remove( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    OBJ_DATA *obj;

    one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
	send_to_char( "Remove what?\n\r", ch );
	return;
    }

    
   if (!IS_NPC(ch) && IS_CLASS(ch, CLASS_HIGHLANDER) && IS_POLYAFF(ch,
POLY_ZULOFORM))
{
send_to_char("Your too mad to remove anything!\n\r",ch);
return;
}

        if (!form_data[ch->cur_form].can_wear && ch->cur_form != 0)
	{
	    stc("You cannot remove anything in this form.\n\r",ch);
	    return;
	}

    if ( !str_cmp( arg, "all" ) )
    {
	OBJ_DATA *obj_next;

	for ( obj = ch->carrying; obj != NULL; obj = obj_next )
	{
	    obj_next = obj->next_content;
	    if ( obj->wear_loc != WEAR_NONE && can_see_obj( ch, obj ) )
		remove_obj( ch, obj->wear_loc, TRUE );
	}
	return;
    }
    if ( ( obj = get_obj_wear( ch, arg ) ) == NULL )
    {
	send_to_char( "You do not have that item.\n\r", ch );
	return;
    }
    remove_obj( ch, obj->wear_loc, TRUE );
    return;
}



void do_sacrifice( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    OBJ_DATA *obj;
    CHAR_DATA *victim;
    char buf[MAX_INPUT_LENGTH];
    int expgain;

    one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
	send_to_char("Sacrifice what?\n\r",ch);
	return;
    }

    if ( ( victim = get_char_room(ch, arg) ) != NULL )
    {
       do_offer(ch, arg);
       return;
    }

    obj = get_obj_list( ch, arg, ch->in_room->contents );
    if ( obj == NULL )
    {
	send_to_char( "You can't find it.\n\r", ch );
	return;
    }

    if ( !CAN_WEAR(obj, ITEM_TAKE) || obj->item_type == ITEM_QUEST ||
	 obj->item_type == ITEM_MONEY || obj->item_type == ITEM_TREASURE ||
	 obj->item_type == ITEM_QUESTCARD || IS_SET(obj->quest, QUEST_ARTIFACT) ||
	obj->item_type == ITEM_KINGDOM_POWER ||
	( obj->questowner != NULL && strlen(obj->questowner) > 1 && str_cmp(ch->name,obj->questowner)))
    {
	act( "You are unable to drain any energy from $p.", ch, obj, 0, TO_CHAR );
	return;
    }
    else if (obj->chobj != NULL && !IS_NPC(obj->chobj) &&
	obj->chobj->pcdata->obj_vnum != 0)
    {
	act( "You are unable to drain any energy from $p.", ch, obj, 0, TO_CHAR );
	return;
    }
    expgain = obj->cost / 100;
    if (expgain < 1)
	expgain = 1;
    if (expgain > 50)
	expgain = 50;
    ch->exp += expgain;
    sprintf( buf,"You drain %d exp of energy from $p.", expgain );
    act( buf, ch, obj, NULL, TO_CHAR );
    act( "$p disintegrates into a fine powder.", ch, obj, NULL, TO_CHAR );
    act( "$n drains the energy from $p.", ch, obj, NULL, TO_ROOM );
    act( "$p disintegrates into a fine powder.", ch, obj, NULL, TO_ROOM );
    if (obj->points > 0 && !IS_NPC(ch) && obj->item_type != ITEM_PAGE)
    {
	sprintf( buf, "You receive a refund of %d quest points from $p.", obj->points);
	act( buf, ch, obj, NULL, TO_CHAR );
	ch->pcdata->quest += obj->points;
    }
    extract_obj( obj );
    return;
}



void do_quaff( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    OBJ_DATA *obj;
    int level;

    one_argument( argument, arg );

	if (IS_NPC(ch)) return;

    if ( arg[0] == '\0' )
    {
	send_to_char( "Quaff what?\n\r", ch );
	return;
    }

    if ( ( obj = get_obj_carry( ch, arg ) ) == NULL )
    {
	send_to_char( "You do not have that potion.\n\r", ch );
	return;
    }

    if ( obj->item_type != ITEM_POTION )
    {
	send_to_char( "You can quaff only potions.\n\r", ch );
	return;
    }
	if (IS_NPC(ch)) return;

    act( "$n quaffs $p.", ch, obj, NULL, TO_ROOM );
    act( "You quaff $p.", ch, obj, NULL ,TO_CHAR );

    level = obj->value[0];
    if (level < 1) level = 1;
    if (level > MAX_SPELL) level = MAX_SPELL;

    obj_cast_spell( obj->value[1], level, ch, ch, NULL );
    obj_cast_spell( obj->value[2], level, ch, ch, NULL );
    obj_cast_spell( obj->value[3], level, ch, ch, NULL );

    extract_obj( obj );
    if (ch->position == POS_FIGHTING) 
    {
	if (!IS_IMMORTAL( ch))
		WAIT_STATE(ch, 6 );
    }
    return;
}



void do_recite( CHAR_DATA *ch, char *argument )
{
    char arg1[MAX_INPUT_LENGTH];
    char arg2[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;
    OBJ_DATA *scroll;
    OBJ_DATA *obj;
    int level;

    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );

	if (IS_NPC(ch)) return;

    if ( ( scroll = get_obj_carry( ch, arg1 ) ) == NULL )
    {
	send_to_char( "You do not have that scroll.\n\r", ch );
	return;
    }

    if ( scroll->item_type != ITEM_SCROLL )
    {
	send_to_char( "You can recite only scrolls.\n\r", ch );
	return;
    }

    obj = NULL;
    if ( arg2[0] == '\0' )
    {
	victim = ch;
    }
    else
    {
	if ( ( victim = get_char_room ( ch, arg2 ) ) == NULL
	&&   ( obj    = get_obj_here  ( ch, arg2 ) ) == NULL )
	{
	    send_to_char( "You can't find it.\n\r", ch );
	    return;
	}
    }
	if (IS_NPC(ch)) return;

    act( "$n recites $p.", ch, scroll, NULL, TO_ROOM );
    act( "You recite $p.", ch, scroll, NULL, TO_CHAR );

    level = scroll->value[0];
    if (level < 1) level = 1;
    if (level > MAX_SPELL) level = MAX_SPELL;

    obj_cast_spell( scroll->value[1], level, ch, victim, obj );
    obj_cast_spell( scroll->value[2], level, ch, victim, obj );
    obj_cast_spell( scroll->value[3], level, ch, victim, obj );

    extract_obj( scroll );
    if (ch->position == POS_FIGHTING) 
    {
    	if (!IS_IMMORTAL( ch))
		WAIT_STATE(ch, 6 );
    }
    return;
}



void do_brandish( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *vch;
    CHAR_DATA *vch_next;
    OBJ_DATA *temp;
    OBJ_DATA *staff;
    int sn;
    int level;

    staff = get_eq_char( ch, WEAR_WIELD );
    temp = get_eq_char( ch, WEAR_HOLD );

	if (IS_NPC(ch)) return;

    if ( staff == NULL && temp == NULL )
    {
	send_to_char( "You hold nothing in your hand.\n\r", ch );
	return;
    }

    if ( staff == NULL ) staff = temp;
    if ( temp == NULL  ) temp = staff;

    if ( staff->item_type != ITEM_STAFF ) staff=temp;

    if ( staff->item_type != ITEM_STAFF )
    {
	send_to_char( "You can brandish only with a staff.\n\r", ch );
	return;
    }

    if ( ( sn = staff->value[3] ) < 0
    ||   sn >= MAX_SKILL
    ||   skill_table[sn].spell_fun == 0 )
    {
	bug( "Do_brandish: bad sn %d.", sn );
	return;
    }
	if (IS_NPC(ch)) return;

    if (!IS_IMMORTAL( ch))
	WAIT_STATE( ch, 2 * PULSE_VIOLENCE );

    if ( staff->value[2] > 0 )
    {
	act( "$n brandishes $p.", ch, staff, NULL, TO_ROOM );
	act( "You brandish $p.",  ch, staff, NULL, TO_CHAR );
	for ( vch = ch->in_room->people; vch; vch = vch_next )
	{
	    vch_next	= vch->next_in_room;

	    switch ( skill_table[sn].target )
	    {
	    default:
		bug( "Do_brandish: bad target for sn %d.", sn );
		return;

	    case TAR_IGNORE:
		if ( vch != ch )
		    continue;
		break;

	    case TAR_CHAR_OFFENSIVE:
		if ( IS_NPC(ch) ? IS_NPC(vch) : !IS_NPC(vch) )
		    continue;
		break;
		
	    case TAR_CHAR_DEFENSIVE:
		if ( IS_NPC(ch) ? !IS_NPC(vch) : IS_NPC(vch) )
		    continue;
		break;

	    case TAR_CHAR_SELF:
		if ( vch != ch )
		    continue;
		break;
	    }

	    level = staff->value[0];
	    if (level < 1) level = 1;
	    if (level > MAX_SPELL) level = MAX_SPELL;

	    obj_cast_spell( staff->value[3], level, ch, vch, NULL );
	}
    }

    if ( --staff->value[2] <= 0 )
    {
	act( "$n's $p blazes bright and is gone.", ch, staff, NULL, TO_ROOM );
	act( "Your $p blazes bright and is gone.", ch, staff, NULL, TO_CHAR );
	extract_obj( staff );
    }

    return;
}



void do_zap( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;
    OBJ_DATA *temp;
    OBJ_DATA *wand;
    OBJ_DATA *obj;
    int level;

    one_argument( argument, arg );

	if (IS_NPC(ch)) return;

    if ( arg[0] == '\0' && ch->fighting == NULL )
    {
	send_to_char( "Zap whom or what?\n\r", ch );
	return;
    }

    wand = get_eq_char( ch, WEAR_WIELD );
    temp = get_eq_char( ch, WEAR_HOLD );

    if ( wand == NULL && temp == NULL )
    {
	send_to_char( "You hold nothing in your hand.\n\r", ch );
	return;
    }

    if ( wand == NULL ) wand = temp;
    if ( temp == NULL ) temp = wand;

    if ( wand->item_type != ITEM_WAND ) wand=temp;

    if ( wand->item_type != ITEM_WAND )
    {
	send_to_char( "You can zap only with a wand.\n\r", ch );
	return;
    }
	if (IS_NPC(ch)) return;

    obj = NULL;
    if ( arg[0] == '\0' )
    {
	if ( ch->fighting != NULL )
	{
	    victim = ch->fighting;
	}
	else
	{
	    send_to_char( "Zap whom or what?\n\r", ch );
	    return;
	}
    }
    else
    {
	if ( ( victim = get_char_room ( ch, arg ) ) == NULL
	&&   ( obj    = get_obj_here  ( ch, arg ) ) == NULL )
	{
	    send_to_char( "You can't find it.\n\r", ch );
	    return;
	}
    }

    if (!IS_IMMORTAL( ch))
	WAIT_STATE( ch, 2 * PULSE_VIOLENCE );

    if ( wand->value[2] > 0 )
    {
	if ( victim != NULL )
	{
	    act( "$n zaps $N with $p.", ch, wand, victim, TO_ROOM );
	    act( "You zap $N with $p.", ch, wand, victim, TO_CHAR );
	}
	else
	{
	    act( "$n zaps $P with $p.", ch, wand, obj, TO_ROOM );
	    act( "You zap $P with $p.", ch, wand, obj, TO_CHAR );
	}

	level = wand->value[0];
	if (level < 1) level = 1;
	if (level > MAX_SPELL) level = MAX_SPELL;

	obj_cast_spell( wand->value[3], level, ch, victim, obj );
    }

    if ( --wand->value[2] <= 0 )
    {
	act( "$n's $p explodes into fragments.", ch, wand, NULL, TO_ROOM );
	act( "Your $p explodes into fragments.", ch, wand, NULL, TO_CHAR );
	extract_obj( wand );
    }

    return;
}



void do_steal( CHAR_DATA *ch, char *argument )
{
    char buf  [MAX_STRING_LENGTH];
    char arg1 [MAX_INPUT_LENGTH];
    char arg2 [MAX_INPUT_LENGTH];
    CHAR_DATA *victim;
    OBJ_DATA *obj;
    int percent;

    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );

    if ( arg1[0] == '\0' || arg2[0] == '\0' )
    {
	send_to_char( "Steal what from whom?\n\r", ch );
	return;
    }

    if ( ( victim = get_char_room( ch, arg2 ) ) == NULL )
    {
	send_to_char( "They aren't here.\n\r", ch );
	return;
    }

    if ( victim == ch )
    {
	send_to_char( "That's pointless.\n\r", ch );
	return;
    }

    if ( IS_IMMORTAL( victim ))
    {
	send_to_char("Steal from an immortal are you crasy!\n\r", ch);
	return;
    }

    if (!IS_IMMORTAL( ch))
    	WAIT_STATE( ch, skill_table[gsn_steal].beats );

    percent  = number_percent( ) + ( IS_AWAKE(victim) ? 10 : -50 );

    if ( ( ch->level + number_range(1,20) < victim->level          )
    ||   ( !IS_NPC(ch) && !IS_NPC(victim) && ch->level < 3         )
    ||   ( !IS_NPC(ch) && !IS_NPC(victim) && victim->level < 3     )
    ||   ( victim->position == POS_FIGHTING                        )
    ||   ( !IS_NPC(victim) && IS_IMMUNE(victim,IMM_STEAL)          )
    ||   ( !IS_NPC(victim) && IS_IMMORTAL(victim)                  )
    ||   ( !IS_NPC(ch) && percent > ch->pcdata->learned[gsn_steal] ) )
    {
	/*
	 * Failure.
	 */
	send_to_char( "Oops.\n\r", ch );
	act( "$n tried to steal from you.\n\r", ch, NULL, victim, TO_VICT    );
	act( "$n tried to steal from $N.\n\r",  ch, NULL, victim, TO_NOTVICT );
	sprintf( buf, "%s is a bloody thief!", ch->name );
	do_shout( victim, buf );
	if ( !IS_NPC(ch) )
	{
	    if ( IS_NPC(victim) )
	    {
		multi_hit( victim, ch, TYPE_UNDEFINED );
	    }
	    else
	    {
		log_string( buf );
		save_char_obj( ch );
	    }
	}

	return;
    }

    if ( !str_cmp( arg1, "coin"  )
    ||   !str_cmp( arg1, "coins" )
    ||   !str_cmp( arg1, "gold"  ) )
    {
	int amount;

	amount = victim->gold * number_range(1, 10) / 100;
	if ( amount <= 0 )
	{
	    send_to_char( "You couldn't get any gold.\n\r", ch );
	    return;
	}

	ch->gold     += amount;
	victim->gold -= amount;
	sprintf( buf, "Bingo!  You got %d gold coins.\n\r", amount );
	send_to_char( buf, ch );
	do_autosave(ch,"");
	do_autosave(victim,"");
	return;
    }

    if ( ( obj = get_obj_carry( victim, arg1 ) ) == NULL )
    {
	send_to_char( "You can't find it.\n\r", ch );
	return;
    }
	
    if ( !can_drop_obj( ch, obj )
    ||   IS_SET(obj->extra_flags, ITEM_LOYAL)
    ||   IS_SET(obj->extra_flags, ITEM_INVENTORY) )
    {
	send_to_char( "You can't pry it away.\n\r", ch );
	return;
    }

    if ( ch->carry_number + 1 > can_carry_n( ch ) )
    {
	send_to_char( "You have your hands full.\n\r", ch );
	return;
    }

    if ( ch->carry_weight + get_obj_weight( obj ) > can_carry_w( ch ) )
    {
	send_to_char( "You can't carry that much weight.\n\r", ch );
	return;
    }

    obj_from_char( obj );
    obj_to_char( obj, ch );
    send_to_char( "You got it!\n\r", ch );
    do_autosave(ch,"");
    do_autosave(victim,"");
    return;
}



/*
 * Shopping commands.
 */
CHAR_DATA *find_keeper( CHAR_DATA *ch )
{
    CHAR_DATA *keeper;
    SHOP_DATA *pShop;
    char buf [MAX_STRING_LENGTH];

    pShop = NULL;
    for ( keeper = ch->in_room->people; keeper; keeper = keeper->next_in_room )
    {
	if ( IS_NPC(keeper) && (pShop = keeper->pIndexData->pShop) != NULL )
	    break;
    }

    if ( pShop == NULL )
    {
	send_to_char( "You can't do that here.\n\r", ch );
	return NULL;
    }

    /*
     * Shop hours.
     */
    if ( time_info.hour < pShop->open_hour )
    {
	strcpy( buf, "Sorry, come back later." );
	do_say( keeper, buf );
	return NULL;
    }
    
    if ( time_info.hour > pShop->close_hour )
    {
	strcpy( buf, "Sorry, come back tomorrow." );
	do_say( keeper, buf );
	return NULL;
    }

    /*
     * Invisible or hidden people.
     */
    if ( !can_see( keeper, ch ) )
    {
	strcpy( buf, "I don't trade with folks I can't see." );
	do_say( keeper, buf );
	return NULL;
    }

    return keeper;
}



int get_cost( CHAR_DATA *keeper, OBJ_DATA *obj, bool fBuy )
{
    SHOP_DATA *pShop;
    int cost;

    if ( obj == NULL || ( pShop = keeper->pIndexData->pShop ) == NULL )
	return 0;

    if ( fBuy )
    {
	cost = obj->cost * pShop->profit_buy  / 100;
    }
    else
    {
	OBJ_DATA *obj2;
	int itype;

	cost = 0;
	for ( itype = 0; itype < MAX_TRADE; itype++ )
	{
	    if ( obj->item_type == pShop->buy_type[itype] )
	    {
		cost = obj->cost * pShop->profit_sell / 100;
		break;
	    }
	}

	for ( obj2 = keeper->carrying; obj2; obj2 = obj2->next_content )
	{
	    if ( obj->pIndexData == obj2->pIndexData )
	    {
		cost = 0;
		break;
	    }
	}
    }

    if ( obj->item_type == ITEM_STAFF || obj->item_type == ITEM_WAND )
	cost = cost * obj->value[2] / obj->value[1];

    return cost;
}



void do_buy( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];

    argument = one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
	send_to_char( "Buy what?\n\r", ch );
	return;
    }

    if ( IS_SET(ch->in_room->room_flags, ROOM_PET_SHOP) )
    {
	char buf[MAX_STRING_LENGTH];
	CHAR_DATA *pet;
	ROOM_INDEX_DATA *pRoomIndexNext;
	ROOM_INDEX_DATA *in_room;

	if ( IS_NPC(ch) )
	    return;

	pRoomIndexNext = get_room_index( ch->in_room->vnum + 1 );
	if ( pRoomIndexNext == NULL )
	{
	    bug( "Do_buy: bad pet shop at vnum %d.", ch->in_room->vnum );
	    send_to_char( "Sorry, you can't buy that here.\n\r", ch );
	    return;
	}

	in_room     = ch->in_room;
	ch->in_room = pRoomIndexNext;
	pet         = get_char_room( ch, arg );
	ch->in_room = in_room;

	if ( pet == NULL || !IS_SET(pet->act, ACT_PET) )
	{
	    send_to_char( "Sorry, you can't buy that here.\n\r", ch );
	    return;
	}

	if ( ch->gold < 10 * pet->level * pet->level )
	{
	    send_to_char( "You can't afford it.\n\r", ch );
	    return;
	}

	if ( ch->level < pet->level )
	{
	    send_to_char( "You're not ready for this pet.\n\r", ch );
	    return;
	}

	ch->gold		-= 10 * pet->level * pet->level;
	pet			= create_mobile( pet->pIndexData );
	SET_BIT(pet->act, ACT_PET);
	SET_BIT(pet->affected_by, AFF_CHARM);

	argument = one_argument( argument, arg );
	if ( arg[0] != '\0' )
	{
	    sprintf( buf, "%s %s", pet->name, arg );
	    free_string( pet->name );
	    pet->name = str_dup( buf );
	}

	sprintf( buf, "%sA neck tag says 'I belong to %s'.\n\r",
	    pet->description, ch->name );
	free_string( pet->description );
	pet->description = str_dup( buf );

	char_to_room( pet, ch->in_room );
	add_follower( pet, ch );
	send_to_char( "Enjoy your pet.\n\r", ch );
	act( "$n bought $N as a pet.", ch, NULL, pet, TO_ROOM );
	return;
    }
    else
    {
	CHAR_DATA *keeper;
	OBJ_DATA *obj;
	int cost;

	if ( ( keeper = find_keeper( ch ) ) == NULL )
	    return;

	obj  = get_obj_carry( keeper, arg );
	cost = get_cost( keeper, obj, TRUE );

	if ( cost <= 0 || !can_see_obj( ch, obj ) )
	{
	    act( "$n tells you 'I don't sell that -- try 'list''.",
		keeper, NULL, ch, TO_VICT );
	    ch->reply = keeper;
	    return;
	}

	if ( ch->gold < cost )
	{
	    act( "$n tells you 'You can't afford to buy $p'.",
		keeper, obj, ch, TO_VICT );
	    ch->reply = keeper;
	    return;
	}
	
	if ( (obj->level > ch->level) && ch->level < 3 )
	{
	    act( "$n tells you 'You can't use $p yet'.",
		keeper, obj, ch, TO_VICT );
	    ch->reply = keeper;
	    return;
	}

	if ( ch->carry_number + 1 > can_carry_n( ch ) )
	{
	    send_to_char( "You can't carry that many items.\n\r", ch );
	    return;
	}

	if ( ch->carry_weight + get_obj_weight( obj ) > can_carry_w( ch ) )
	{
	    send_to_char( "You can't carry that much weight.\n\r", ch );
	    return;
	}

	act( "$n buys $p.", ch, obj, NULL, TO_ROOM );
	act( "You buy $p.", ch, obj, NULL, TO_CHAR );
	ch->gold     -= cost;
	keeper->gold += cost;

	if ( IS_SET( obj->extra_flags, ITEM_INVENTORY ) )
	    obj = create_object( obj->pIndexData, obj->level );
	else
	    obj_from_char( obj );

	obj_to_char( obj, ch );
	return;
    }
}



void do_list( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_STRING_LENGTH];

    if ( IS_SET(ch->in_room->room_flags, ROOM_PET_SHOP) )
    {
	ROOM_INDEX_DATA *pRoomIndexNext;
	CHAR_DATA *pet;
	bool found;

	pRoomIndexNext = get_room_index( ch->in_room->vnum + 1 );
	if ( pRoomIndexNext == NULL )
	{
	    bug( "Do_list: bad pet shop at vnum %d.", ch->in_room->vnum );
	    send_to_char( "You can't do that here.\n\r", ch );
	    return;
	}

	found = FALSE;
	for ( pet = pRoomIndexNext->people; pet; pet = pet->next_in_room )
	{
	    if ( IS_SET(pet->act, ACT_PET) )
	    {
		if ( !found )
		{
		    found = TRUE;
		    send_to_char( "Pets for sale:\n\r", ch );
		}
		sprintf( buf, "[%2d] %8d - %s\n\r",
		    pet->level,
		    10 * pet->level * pet->level,
		    pet->short_descr );
		send_to_char( buf, ch );
	    }
	}
	if ( !found )
	    send_to_char( "Sorry, we're out of pets right now.\n\r", ch );
	return;
    }
    else
    {
	char arg[MAX_INPUT_LENGTH];
	CHAR_DATA *keeper;
	OBJ_DATA *obj;
	int cost;
	bool found;

	one_argument( argument, arg );

	if ( ( keeper = find_keeper( ch ) ) == NULL )
	    return;

	found = FALSE;
	for ( obj = keeper->carrying; obj; obj = obj->next_content )
	{
	    if ( obj->wear_loc == WEAR_NONE
	    &&   can_see_obj( ch, obj )
	    && ( cost = get_cost( keeper, obj, TRUE ) ) > 0
	    && ( arg[0] == '\0' || is_name( arg, obj->name ) ) )
	    {
		if ( !found )
		{
		    found = TRUE;
		    send_to_char( "[Lv Price] Item\n\r", ch );
		}

		sprintf( buf, "[%2d %5d] %s.\n\r",
		    obj->level, cost, capitalize( obj->short_descr ) );
		send_to_char( buf, ch );
	    }
	}

	if ( !found )
	{
	    if ( arg[0] == '\0' )
		send_to_char( "You can't buy anything here.\n\r", ch );
	    else
		send_to_char( "You can't buy that here.\n\r", ch );
	}
	return;
    }
}



void do_sell( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_STRING_LENGTH];
    char arg[MAX_INPUT_LENGTH];
    CHAR_DATA *keeper;
    OBJ_DATA *obj;
    int cost;

    one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
	send_to_char( "Sell what?\n\r", ch );
	return;
    }

    if ( ( keeper = find_keeper( ch ) ) == NULL )
	return;

    if ( ( obj = get_obj_carry( ch, arg ) ) == NULL )
    {
	act( "$n tells you 'You don't have that item'.",
	    keeper, NULL, ch, TO_VICT );
	ch->reply = keeper;
	return;
    }

    if ( !can_drop_obj( ch, obj ) )
    {
	send_to_char( "You can't let go of it.\n\r", ch );
	return;
    }

    if ( ( cost = get_cost( keeper, obj, FALSE ) ) <= 0 )
    {
	act( "$n looks uninterested in $p.", keeper, obj, ch, TO_VICT );
	return;
    }

    act( "$n sells $p.", ch, obj, NULL, TO_ROOM );
    sprintf( buf, "You sell $p for %d gold piece%s.",
	cost, cost == 1 ? "" : "s" );
    act( buf, ch, obj, NULL, TO_CHAR );
    ch->gold     += cost;
    keeper->gold -= cost;
    if ( keeper->gold < 0 )
	keeper->gold = 0;

    if ( obj->item_type == ITEM_TRASH )
    {
	extract_obj( obj );
    }
    else
    {
	obj_from_char( obj );
	obj_to_char( obj, keeper );
    }

    return;
}



void do_value( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_STRING_LENGTH];
    char arg[MAX_INPUT_LENGTH];
    CHAR_DATA *keeper;
    OBJ_DATA *obj;
    int cost;

    one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
	send_to_char( "Value what?\n\r", ch );
	return;
    }

    if ( ( keeper = find_keeper( ch ) ) == NULL )
	return;

    if ( ( obj = get_obj_carry( ch, arg ) ) == NULL )
    {
	act( "$n tells you 'You don't have that item'.",
	    keeper, NULL, ch, TO_VICT );
	ch->reply = keeper;
	return;
    }

    if ( !can_drop_obj( ch, obj ) )
    {
	send_to_char( "You can't let go of it.\n\r", ch );
	return;
    }

    if ( ( cost = get_cost( keeper, obj, FALSE ) ) <= 0 )
    {
	act( "$n looks uninterested in $p.", keeper, obj, ch, TO_VICT );
	return;
    }

    sprintf( buf, "$n tells you 'I'll give you %d gold coins for $p'.", cost );
    act( buf, keeper, obj, ch, TO_VICT );
    ch->reply = keeper;

    return;
}

void do_activate( CHAR_DATA *ch, char *argument )
{
    char arg1[MAX_INPUT_LENGTH];
    char arg2[MAX_INPUT_LENGTH];
    OBJ_DATA *obj;
    OBJ_DATA *obj2;
    CHAR_DATA *victim;
    CHAR_DATA *victim_next;
    CHAR_DATA *mount;
    CHAR_DATA *mob;
    ROOM_INDEX_DATA *pRoomIndex;

    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );

    if ( arg1[0] == '\0' )
    {
	send_to_char( "Which item do you wish to activate?\n\r", ch );
	return;
    }
    if ( (obj = get_obj_wear(ch,arg1)) == NULL ) 
    {
	if ( ( obj = get_obj_here( ch, arg1 ) ) == NULL )
	{
	    send_to_char( "You can't find that item.\n\r", ch );
	    return;
	}
	/* You should only be able to use nontake items on floor */
	if (CAN_WEAR(obj, ITEM_TAKE))
	{
	    send_to_char( "But you are not wearing it!\n\r", ch );
	    return;
	}
    }
    if ( obj == NULL || !IS_SET(obj->spectype, SITEM_ACTIVATE) )
    {
	send_to_char( "This item cannot be activated.\n\r", ch );
	return;
    }
    if ( IS_SET(obj->spectype, SITEM_TARGET) && arg2[0] == '\0' )
    {
	send_to_char( "Who do you wish to activate it on?\n\r", ch );
	return;
    }
    if ( IS_SET(obj->spectype, SITEM_TARGET) )
    {
	if ( ( victim = get_char_room( ch, arg2 ) ) == NULL )
	{
	    send_to_char( "Nobody here by that name.\n\r", ch );
	    return;
	}
    }
    else victim = ch;
    if (obj->chpoweruse != NULL && obj->chpoweruse != '\0'
	&& str_cmp(obj->chpoweruse,"(null)") )
	kavitem(str_dup(obj->chpoweruse),ch,obj,NULL,TO_CHAR);
    if (obj->victpoweruse != NULL && obj->victpoweruse != '\0'
	&& str_cmp(obj->victpoweruse,"(null)") )
	kavitem(str_dup(obj->victpoweruse),ch,obj,NULL,TO_ROOM);
    if ( IS_SET(obj->spectype, SITEM_SPELL))
    {
	int castlevel = obj->level;
	if (castlevel < 1) castlevel = 1;
	else if (castlevel > 60) castlevel = 60;
	obj_cast_spell( obj->specpower, castlevel, ch, victim, NULL );
    	if (!IS_IMMORTAL( ch))
		WAIT_STATE(ch,6);
	if ( IS_SET(obj->spectype, SITEM_DELAY1)) WAIT_STATE(ch,6);
	if ( IS_SET(obj->spectype, SITEM_DELAY2)) WAIT_STATE(ch,12);
	return;
    }
    else if ( IS_SET(obj->spectype, SITEM_TRANSPORTER))
    {
    	if (obj->chpoweron != NULL && obj->chpoweron != '\0'
	    && str_cmp(obj->chpoweron,"(null)") )
	    kavitem(str_dup(obj->chpoweron),ch,obj,NULL,TO_CHAR);
    	if (obj->victpoweron != NULL && obj->victpoweron != '\0'
	    && str_cmp(obj->victpoweron,"(null)") )
	    kavitem(str_dup(obj->victpoweron),ch,obj,NULL,TO_ROOM);
	pRoomIndex     = get_room_index(obj->specpower);
	obj->specpower = ch->in_room->vnum;
	if ( pRoomIndex == NULL ) return;
	char_from_room(ch);
	char_to_room(ch,pRoomIndex);
	do_look(ch,"auto");
    	if (obj->chpoweroff != NULL && obj->chpoweroff != '\0'
	    && str_cmp(obj->chpoweroff,"(null)") )
	    kavitem(str_dup(obj->chpoweroff),ch,obj,NULL,TO_CHAR);
    	if (obj->victpoweroff != NULL && obj->victpoweroff != '\0'
	    && str_cmp(obj->victpoweroff,"(null)") )
	    kavitem(str_dup(obj->victpoweroff),ch,obj,NULL,TO_ROOM);
       if (!IS_SET(obj->quest, QUEST_ARTIFACT) &&
           ( IS_SET(ch->in_room->room_flags, ROOM_NO_TELEPORT) ||
ch->in_room->vnum == 30000) &&
            CAN_WEAR(obj,ITEM_TAKE))
        {
            send_to_char("A powerful force hurls you from the room.\n\r",ch);
            act("$n is hurled from the room by a powerful force.",ch,NULL,NULL,TO_ROOM);
            ch->position = POS_STUNNED;
            char_from_room(ch);
            char_to_room(ch,get_room_index(ROOM_VNUM_LIMBO));
            act("$n appears in the room, and falls to the ground stunned.",ch,NULL,NULL,TO_ROOM);
        }
    	if ( (mount = ch->mount) == NULL) return;
    	char_from_room( mount );
    	char_to_room( mount, ch->in_room );
    	do_look( mount, "auto" );
	return;
    }
    else if ( IS_SET(obj->spectype, SITEM_TELEPORTER))
    {
    	if (obj->chpoweron != NULL && obj->chpoweron != '\0'
	    && str_cmp(obj->chpoweron,"(null)") )
	    kavitem(str_dup(obj->chpoweron),ch,obj,NULL,TO_CHAR);
    	if (obj->victpoweron != NULL && obj->victpoweron != '\0'
	    && str_cmp(obj->victpoweron,"(null)") )
	    kavitem(str_dup(obj->victpoweron),ch,obj,NULL,TO_ROOM);
	pRoomIndex     = get_room_index(obj->specpower);
	if ( pRoomIndex == NULL ) return;
	char_from_room(ch);
	char_to_room(ch,pRoomIndex);
	do_look(ch,"auto");
    	if (obj->chpoweroff != NULL && obj->chpoweroff != '\0'
	    && str_cmp(obj->chpoweroff,"(null)") )
	    kavitem(str_dup(obj->chpoweroff),ch,obj,NULL,TO_CHAR);
    	if (obj->victpoweroff != NULL && obj->victpoweroff != '\0'
	    && str_cmp(obj->victpoweroff,"(null)") )
	    kavitem(str_dup(obj->victpoweroff),ch,obj,NULL,TO_ROOM);
        if (!IS_SET(obj->quest, QUEST_ARTIFACT) &&
            IS_SET(ch->in_room->room_flags, ROOM_NO_TELEPORT) &&
            CAN_WEAR(obj,ITEM_TAKE))
        {
            send_to_char("A powerful force hurls you from the room.\n\r",ch);
            act("$n is hurled from the room by a powerful force.",ch,NULL,NULL,TO_ROOM);
            ch->position = POS_STUNNED;
            char_from_room(ch);
            char_to_room(ch,get_room_index(ROOM_VNUM_LIMBO));
            act("$n appears in the room, and falls to the ground stunned.",ch,NULL,NULL,TO_ROOM);
        }
    	if ( (mount = ch->mount) == NULL) return;
    	char_from_room( mount );
    	char_to_room( mount, ch->in_room );
    	do_look( mount, "auto" );
	return;
    }
    else if ( IS_SET(obj->spectype, SITEM_OBJECT))
    {
	if ( get_obj_index(obj->specpower) == NULL ) return;
	obj2 = create_object(get_obj_index(obj->specpower), ch->level);
	if ( CAN_WEAR(obj2,ITEM_TAKE) ) obj_to_char(obj2,ch);
	else                           obj_to_room(obj2,ch->in_room);
    }
    else if ( IS_SET(obj->spectype, SITEM_MOBILE))
    {
	if ( get_mob_index(obj->specpower) == NULL ) return;
	mob = create_mobile(get_mob_index(obj->specpower));
	char_to_room(mob,ch->in_room);
    }
    else if ( IS_SET(obj->spectype, SITEM_ACTION))
    {
	interpret(ch,obj->victpoweron);
	if (obj->victpoweroff != NULL && str_cmp(obj->victpoweroff,"(null)") &&
	    obj->victpoweroff != '\0')
	{
	    for ( victim = char_list; victim != NULL; victim = victim_next )
	    {
		victim_next	= victim->next;
		if ( victim->in_room == NULL ) continue;
		if ( victim == ch ) continue;
		if ( victim->in_room == ch->in_room )
		{
		    interpret(victim,obj->victpoweroff);
		    continue;
		}
	    }
	}
    }
    return;
}

void do_press( CHAR_DATA *ch, char *argument )
{
    char arg1[MAX_INPUT_LENGTH];
    char arg2[MAX_INPUT_LENGTH];
    OBJ_DATA *obj;
    OBJ_DATA *obj2;
    CHAR_DATA *victim;
    CHAR_DATA *victim_next;
    CHAR_DATA *mount;
    CHAR_DATA *mob;
    ROOM_INDEX_DATA *pRoomIndex;

    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );

    if ( arg1[0] == '\0' )
    {
	send_to_char( "Which item do you wish to press?\n\r", ch );
	return;
    }
    if ( (obj = get_obj_wear(ch,arg1)) == NULL ) 
    {
	if ( ( obj = get_obj_here( ch, arg1 ) ) == NULL )
	{
	    send_to_char( "You can't find that item.\n\r", ch );
	    return;
	}
	/* You should only be able to use nontake items on floor */
	if (CAN_WEAR(obj, ITEM_TAKE))
	{
	    send_to_char( "But you are not wearing it!\n\r", ch );
	    return;
	}
    }
    if ( obj == NULL || !IS_SET(obj->spectype, SITEM_PRESS) )
    {
	send_to_char( "There is nothing on this item to press.\n\r", ch );
	return;
    }
    if ( IS_SET(obj->spectype, SITEM_TARGET) && arg2[0] == '\0' )
    {
	send_to_char( "Who do you wish to use it on?\n\r", ch );
	return;
    }
    if ( IS_SET(obj->spectype, SITEM_TARGET) )
    {
	if ( ( victim = get_char_room( ch, arg2 ) ) == NULL )
	{
	    send_to_char( "Nobody here by that name.\n\r", ch );
	    return;
	}
    }
    else victim = ch;
    if (obj->chpoweruse != NULL && obj->chpoweruse != '\0'
	&& str_cmp(obj->chpoweruse,"(null)") )
	kavitem(str_dup(obj->chpoweruse),ch,obj,NULL,TO_CHAR);
    if (obj->victpoweruse != NULL && obj->victpoweruse != '\0'
	&& str_cmp(obj->victpoweruse,"(null)") )
	kavitem(str_dup(obj->victpoweruse),ch,obj,NULL,TO_ROOM);
    if ( IS_SET(obj->spectype, SITEM_SPELL))
    {
	int castlevel = obj->level;
	if (castlevel < 1) castlevel = 1;
	else if (castlevel > 60) castlevel = 60;
	obj_cast_spell( obj->specpower, castlevel, ch, victim, NULL );
 	if (!IS_IMMORTAL( ch))
		WAIT_STATE(ch,6);
	if ( IS_SET(obj->spectype, SITEM_DELAY1)) 
        {
	    if (!IS_IMMORTAL( ch))
		WAIT_STATE(ch,6);
	}
	if ( IS_SET(obj->spectype, SITEM_DELAY2)) 
	{
	    if (!IS_IMMORTAL( ch))
		WAIT_STATE(ch,12);
	}
	return;
    }
    else if ( IS_SET(obj->spectype, SITEM_TRANSPORTER))
    {
    	if (obj->chpoweron != NULL && obj->chpoweron != '\0'
	    && str_cmp(obj->chpoweron,"(null)") )
	    kavitem(str_dup(obj->chpoweron),ch,obj,NULL,TO_CHAR);
    	if (obj->victpoweron != NULL && obj->victpoweron != '\0'
	    && str_cmp(obj->victpoweron,"(null)") )
	    kavitem(str_dup(obj->victpoweron),ch,obj,NULL,TO_ROOM);
	pRoomIndex     = get_room_index(obj->specpower);
	obj->specpower = ch->in_room->vnum;
	if ( pRoomIndex == NULL ) return;
	char_from_room(ch);
	char_to_room(ch,pRoomIndex);
	do_look(ch,"auto");
    	if (obj->chpoweroff != NULL && obj->chpoweroff != '\0'
	    && str_cmp(obj->chpoweroff,"(null)") )
	    kavitem(str_dup(obj->chpoweroff),ch,obj,NULL,TO_CHAR);
    	if (obj->victpoweroff != NULL && obj->victpoweroff != '\0'
	    && str_cmp(obj->victpoweroff,"(null)") )
	    kavitem(str_dup(obj->victpoweroff),ch,obj,NULL,TO_ROOM);
       if (!IS_SET(obj->quest, QUEST_ARTIFACT) &&
            IS_SET(ch->in_room->room_flags, ROOM_NO_TELEPORT) &&
            CAN_WEAR(obj,ITEM_TAKE))
        {
            send_to_char("A powerful force hurls you from the room.\n\r",ch);
            act("$n is hurled from the room by a powerful force.",ch,NULL,NULL,TO_ROOM);
            ch->position = POS_STUNNED;
            char_from_room(ch);
            char_to_room(ch,get_room_index(ROOM_VNUM_LIMBO));
            act("$n appears in the room, and falls to the ground stunned.",ch,NULL,NULL,TO_ROOM);
        }
    	if ( (mount = ch->mount) == NULL) return;
    	char_from_room( mount );
    	char_to_room( mount, ch->in_room );
    	do_look( mount, "auto" );
	return;
    }
    else if ( IS_SET(obj->spectype, SITEM_TELEPORTER))
    {
    	if (obj->chpoweron != NULL && obj->chpoweron != '\0'
	    && str_cmp(obj->chpoweron,"(null)") )
	    kavitem(str_dup(obj->chpoweron),ch,obj,NULL,TO_CHAR);
    	if (obj->victpoweron != NULL && obj->victpoweron != '\0'
	    && str_cmp(obj->victpoweron,"(null)") )
	    kavitem(str_dup(obj->victpoweron),ch,obj,NULL,TO_ROOM);
	pRoomIndex     = get_room_index(obj->specpower);
	if ( pRoomIndex == NULL ) return;
	char_from_room(ch);
	char_to_room(ch,pRoomIndex);
	do_look(ch,"auto");
    	if (obj->chpoweroff != NULL && obj->chpoweroff != '\0'
	    && str_cmp(obj->chpoweroff,"(null)") )
	    kavitem(str_dup(obj->chpoweroff),ch,obj,NULL,TO_CHAR);
    	if (obj->victpoweroff != NULL && obj->victpoweroff != '\0'
	    && str_cmp(obj->victpoweroff,"(null)") )
	    kavitem(str_dup(obj->victpoweroff),ch,obj,NULL,TO_ROOM);
        if (!IS_SET(obj->quest, QUEST_ARTIFACT) &&
            IS_SET(ch->in_room->room_flags, ROOM_NO_TELEPORT) &&
            CAN_WEAR(obj,ITEM_TAKE))
        {
            send_to_char("A powerful force hurls you from the room.\n\r",ch);
            act("$n is hurled from the room by a powerful force.",ch,NULL,NULL,TO_ROOM);
            ch->position = POS_STUNNED;
            char_from_room(ch);
            char_to_room(ch,get_room_index(ROOM_VNUM_LIMBO));
            act("$n appears in the room, and falls to the ground stunned.",ch,NULL,NULL,TO_ROOM);
        }
    	if ( (mount = ch->mount) == NULL) return;
    	char_from_room( mount );
    	char_to_room( mount, ch->in_room );
    	do_look( mount, "auto" );
	return;
    }
    else if ( IS_SET(obj->spectype, SITEM_OBJECT))
    {
	if ( get_obj_index(obj->specpower) == NULL ) return;
	obj2 = create_object(get_obj_index(obj->specpower), ch->level);
	if ( CAN_WEAR(obj2,ITEM_TAKE) ) obj_to_char(obj2,ch);
	else                           obj_to_room(obj2,ch->in_room);
    }
    else if ( IS_SET(obj->spectype, SITEM_MOBILE))
    {
	if ( get_mob_index(obj->specpower) == NULL ) return;
	mob = create_mobile(get_mob_index(obj->specpower));
	char_to_room(mob,ch->in_room);
    }
    else if ( IS_SET(obj->spectype, SITEM_ACTION))
    {
	interpret(ch,obj->victpoweron);
	if (obj->victpoweroff != NULL && str_cmp(obj->victpoweroff,"(null)") &&
	    obj->victpoweroff != '\0')
	{
	    for ( victim = char_list; victim != NULL; victim = victim_next )
	    {
		victim_next	= victim->next;
		if ( victim->in_room == NULL ) continue;
		if ( victim == ch ) continue;
		if ( victim->in_room == ch->in_room )
		{
		    interpret(victim,obj->victpoweroff);
		    continue;
		}
	    }
	}
    }
    return;
}

void do_twist( CHAR_DATA *ch, char *argument )
{
    char arg1[MAX_INPUT_LENGTH];
    char arg2[MAX_INPUT_LENGTH];
    OBJ_DATA *obj;
    OBJ_DATA *obj2;
    CHAR_DATA *victim;
    CHAR_DATA *victim_next;
    CHAR_DATA *mount;
    CHAR_DATA *mob;
    ROOM_INDEX_DATA *pRoomIndex;

    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );

    if ( arg1[0] == '\0' )
    {
	send_to_char( "Which item do you wish to twist?\n\r", ch );
	return;
    }
    if ( (obj = get_obj_wear(ch,arg1)) == NULL ) 
    {
	if ( ( obj = get_obj_here( ch, arg1 ) ) == NULL )
	{
	    send_to_char( "You can't find that item.\n\r", ch );
	    return;
	}
	/* You should only be able to use nontake items on floor */
	if (CAN_WEAR(obj, ITEM_TAKE))
	{
	    send_to_char( "But you are not wearing it!\n\r", ch );
	    return;
	}
    }
    if ( obj == NULL || !IS_SET(obj->spectype, SITEM_TWIST) )
    {
	send_to_char( "This item cannot be twisted.\n\r", ch );
	return;
    }
    if ( IS_SET(obj->spectype, SITEM_TARGET) && arg2[0] == '\0' )
    {
	send_to_char( "Who do you wish to use it on?\n\r", ch );
	return;
    }
    if ( IS_SET(obj->spectype, SITEM_TARGET) )
    {
	if ( ( victim = get_char_room( ch, arg2 ) ) == NULL )
	{
	    send_to_char( "Nobody here by that name.\n\r", ch );
	    return;
	}
    }
    else victim = ch;
    if (obj->chpoweruse != NULL && obj->chpoweruse != '\0'
	&& str_cmp(obj->chpoweruse,"(null)") )
	kavitem(str_dup(obj->chpoweruse),ch,obj,NULL,TO_CHAR);
    if (obj->victpoweruse != NULL && obj->victpoweruse != '\0'
	&& str_cmp(obj->victpoweruse,"(null)") )
	kavitem(str_dup(obj->victpoweruse),ch,obj,NULL,TO_ROOM);
    if ( IS_SET(obj->spectype, SITEM_SPELL))
    {
	int castlevel = obj->level;
	if (castlevel < 1) castlevel = 1;
	else if (castlevel > 60) castlevel = 60;
	obj_cast_spell( obj->specpower, castlevel, ch, victim, NULL );
    	if (!IS_IMMORTAL( ch))
		WAIT_STATE(ch,6);
	if ( IS_SET(obj->spectype, SITEM_DELAY1)) 
	{
	    if (!IS_IMMORTAL( ch))
		WAIT_STATE(ch,6);
	}
	if ( IS_SET(obj->spectype, SITEM_DELAY2))
	{
     		if (!IS_IMMORTAL( ch))
			WAIT_STATE(ch,12);
	}
	return;
    }
    else if ( IS_SET(obj->spectype, SITEM_TRANSPORTER))
    {
    	if (obj->chpoweron != NULL && obj->chpoweron != '\0'
	    && str_cmp(obj->chpoweron,"(null)") )
	    kavitem(str_dup(obj->chpoweron),ch,obj,NULL,TO_CHAR);
    	if (obj->victpoweron != NULL && obj->victpoweron != '\0'
	    && str_cmp(obj->victpoweron,"(null)") )
	    kavitem(str_dup(obj->victpoweron),ch,obj,NULL,TO_ROOM);
	pRoomIndex     = get_room_index(obj->specpower);
	obj->specpower = ch->in_room->vnum;
	if ( pRoomIndex == NULL ) return;
	char_from_room(ch);
	char_to_room(ch,pRoomIndex);
	do_look(ch,"auto");
    	if (obj->chpoweroff != NULL && obj->chpoweroff != '\0'
	    && str_cmp(obj->chpoweroff,"(null)") )
	    kavitem(str_dup(obj->chpoweroff),ch,obj,NULL,TO_CHAR);
    	if (obj->victpoweroff != NULL && obj->victpoweroff != '\0'
	    && str_cmp(obj->victpoweroff,"(null)") )
	    kavitem(str_dup(obj->victpoweroff),ch,obj,NULL,TO_ROOM);
       if (!IS_SET(obj->quest, QUEST_ARTIFACT) &&
            IS_SET(ch->in_room->room_flags, ROOM_NO_TELEPORT) &&
            CAN_WEAR(obj,ITEM_TAKE))
        {
            send_to_char("A powerful force hurls you from the room.\n\r",ch);
            act("$n is hurled from the room by a powerful force.",ch,NULL,NULL,TO_ROOM);
            ch->position = POS_STUNNED;
            char_from_room(ch);
            char_to_room(ch,get_room_index(ROOM_VNUM_LIMBO));
            act("$n appears in the room, and falls to the ground stunned.",ch,NULL,NULL,TO_ROOM);
        }
    	if ( (mount = ch->mount) == NULL) return;
    	char_from_room( mount );
    	char_to_room( mount, ch->in_room );
    	do_look( mount, "auto" );
	return;
    }
    else if ( IS_SET(obj->spectype, SITEM_TELEPORTER))
    {
    	if (obj->chpoweron != NULL && obj->chpoweron != '\0'
	    && str_cmp(obj->chpoweron,"(null)") )
	    kavitem(str_dup(obj->chpoweron),ch,obj,NULL,TO_CHAR);
    	if (obj->victpoweron != NULL && obj->victpoweron != '\0'
	    && str_cmp(obj->victpoweron,"(null)") )
	    kavitem(str_dup(obj->victpoweron),ch,obj,NULL,TO_ROOM);
	pRoomIndex     = get_room_index(obj->specpower);
	if ( pRoomIndex == NULL ) return;
	char_from_room(ch);
	char_to_room(ch,pRoomIndex);
	do_look(ch,"auto");
    	if (obj->chpoweroff != NULL && obj->chpoweroff != '\0'
	    && str_cmp(obj->chpoweroff,"(null)") )
	    kavitem(str_dup(obj->chpoweroff),ch,obj,NULL,TO_CHAR);
    	if (obj->victpoweroff != NULL && obj->victpoweroff != '\0'
	    && str_cmp(obj->victpoweroff,"(null)") )
	    kavitem(str_dup(obj->victpoweroff),ch,obj,NULL,TO_ROOM);
       if (!IS_SET(obj->quest, QUEST_ARTIFACT) &&
            IS_SET(ch->in_room->room_flags, ROOM_NO_TELEPORT) &&
            CAN_WEAR(obj,ITEM_TAKE))
        {
            send_to_char("A powerful force hurls you from the room.\n\r",ch);
            act("$n is hurled from the room by a powerful force.",ch,NULL,NULL,TO_ROOM);
            ch->position = POS_STUNNED;
            char_from_room(ch);
            char_to_room(ch,get_room_index(ROOM_VNUM_LIMBO));
            act("$n appears in the room, and falls to the ground stunned.",ch,NULL,NULL,TO_ROOM);
        }
    	if ( (mount = ch->mount) == NULL) return;
    	char_from_room( mount );
    	char_to_room( mount, ch->in_room );
    	do_look( mount, "auto" );
	return;
    }
    else if ( IS_SET(obj->spectype, SITEM_OBJECT))
    {
	if ( get_obj_index(obj->specpower) == NULL ) return;
	obj2 = create_object(get_obj_index(obj->specpower), ch->level);
	if ( CAN_WEAR(obj2,ITEM_TAKE) ) obj_to_char(obj2,ch);
	else                           obj_to_room(obj2,ch->in_room);
    }
    else if ( IS_SET(obj->spectype, SITEM_MOBILE))
    {
	if ( get_mob_index(obj->specpower) == NULL ) return;
	mob = create_mobile(get_mob_index(obj->specpower));
	char_to_room(mob,ch->in_room);
    }
    else if ( IS_SET(obj->spectype, SITEM_ACTION))
    {
	interpret(ch,obj->victpoweron);
	if (obj->victpoweroff != NULL && str_cmp(obj->victpoweroff,"(null)") &&
	    obj->victpoweroff != '\0')
	{
	    for ( victim = char_list; victim != NULL; victim = victim_next )
	    {
		victim_next	= victim->next;
		if ( victim->in_room == NULL ) continue;
		if ( victim == ch ) continue;
		if ( victim->in_room == ch->in_room )
		{
		    interpret(victim,obj->victpoweroff);
		    continue;
		}
	    }
	}
    }
    return;
}

void do_pull( CHAR_DATA *ch, char *argument )
{
    char arg1[MAX_INPUT_LENGTH];
    char arg2[MAX_INPUT_LENGTH];
    OBJ_DATA *obj;
    OBJ_DATA *obj2;
    CHAR_DATA *victim;
    CHAR_DATA *victim_next;
    CHAR_DATA *mount;
    CHAR_DATA *mob;
    ROOM_INDEX_DATA *pRoomIndex;

    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );

    if ( arg1[0] == '\0' )
    {
	send_to_char( "What do you wish to pull?\n\r", ch );
	return;
    }
    if ( (obj = get_obj_wear(ch,arg1)) == NULL ) 
    {
	if ( ( obj = get_obj_here( ch, arg1 ) ) == NULL )
	{
	    send_to_char( "You can't find that item.\n\r", ch );
	    return;
	}
	/* You should only be able to use nontake items on floor */
	if (CAN_WEAR(obj, ITEM_TAKE))
	{
	    send_to_char( "But you are not wearing it!\n\r", ch );
	    return;
	}
    }
    if ( obj == NULL || !IS_SET(obj->spectype, SITEM_PULL) )
    {
	send_to_char( "This item cannot be pulled.\n\r", ch );
	return;
    }
    if ( IS_SET(obj->spectype, SITEM_TARGET) && arg2[0] == '\0' )
    {
	send_to_char( "Who do you wish to use it on?\n\r", ch );
	return;
    }
    if ( IS_SET(obj->spectype, SITEM_TARGET) )
    {
	if ( ( victim = get_char_room( ch, arg2 ) ) == NULL )
	{
	    send_to_char( "Nobody here by that name.\n\r", ch );
	    return;
	}
    }
    else victim = ch;
    if (obj->chpoweruse != NULL && obj->chpoweruse != '\0'
	&& str_cmp(obj->chpoweruse,"(null)") )
	kavitem(str_dup(obj->chpoweruse),ch,obj,NULL,TO_CHAR);
    if (obj->victpoweruse != NULL && obj->victpoweruse != '\0'
	&& str_cmp(obj->victpoweruse,"(null)") )
	kavitem(str_dup(obj->victpoweruse),ch,obj,NULL,TO_ROOM);
    if ( IS_SET(obj->spectype, SITEM_SPELL))
    {
	int castlevel = obj->level;
	if (castlevel < 1) castlevel = 1;
	else if (castlevel > 60) castlevel = 60;
	obj_cast_spell( obj->specpower, castlevel, ch, victim, NULL );
    	if (!IS_IMMORTAL( ch))
		WAIT_STATE(ch,6);
	if ( IS_SET(obj->spectype, SITEM_DELAY1))
	{
		if (!IS_IMMORTAL( ch))
			WAIT_STATE(ch,6);
	}
	if ( IS_SET(obj->spectype, SITEM_DELAY2)) 
	{
	    if (!IS_IMMORTAL( ch))
		WAIT_STATE(ch,12);
	}
	return;
    }
    else if ( IS_SET(obj->spectype, SITEM_TRANSPORTER))
    {
    	if (obj->chpoweron != NULL && obj->chpoweron != '\0'
	    && str_cmp(obj->chpoweron,"(null)") )
	    kavitem(str_dup(obj->chpoweron),ch,obj,NULL,TO_CHAR);
    	if (obj->victpoweron != NULL && obj->victpoweron != '\0'
	    && str_cmp(obj->victpoweron,"(null)") )
	    kavitem(str_dup(obj->victpoweron),ch,obj,NULL,TO_ROOM);
	pRoomIndex     = get_room_index(obj->specpower);
	obj->specpower = ch->in_room->vnum;
	if ( pRoomIndex == NULL ) return;
	char_from_room(ch);
	char_to_room(ch,pRoomIndex);
	do_look(ch,"auto");
    	if (obj->chpoweroff != NULL && obj->chpoweroff != '\0'
	    && str_cmp(obj->chpoweroff,"(null)") )
	    kavitem(str_dup(obj->chpoweroff),ch,obj,NULL,TO_CHAR);
    	if (obj->victpoweroff != NULL && obj->victpoweroff != '\0'
	    && str_cmp(obj->victpoweroff,"(null)") )
	    kavitem(str_dup(obj->victpoweroff),ch,obj,NULL,TO_ROOM);
       if (!IS_SET(obj->quest, QUEST_ARTIFACT) &&
            IS_SET(ch->in_room->room_flags, ROOM_NO_TELEPORT) &&
            CAN_WEAR(obj,ITEM_TAKE))
        {
            send_to_char("A powerful force hurls you from the room.\n\r",ch);
            act("$n is hurled from the room by a powerful force.",ch,NULL,NULL,TO_ROOM);
            ch->position = POS_STUNNED;
            char_from_room(ch);
            char_to_room(ch,get_room_index(ROOM_VNUM_LIMBO));
            act("$n appears in the room, and falls to the ground stunned.",ch,NULL,NULL,TO_ROOM);
        }
    	if ( (mount = ch->mount) == NULL) return;
    	char_from_room( mount );
    	char_to_room( mount, ch->in_room );
    	do_look( mount, "auto" );
	return;
    }
    else if ( IS_SET(obj->spectype, SITEM_TELEPORTER))
    {
    	if (obj->chpoweron != NULL && obj->chpoweron != '\0'
	    && str_cmp(obj->chpoweron,"(null)") )
	    kavitem(str_dup(obj->chpoweron),ch,obj,NULL,TO_CHAR);
    	if (obj->victpoweron != NULL && obj->victpoweron != '\0'
	    && str_cmp(obj->victpoweron,"(null)") )
	    kavitem(str_dup(obj->victpoweron),ch,obj,NULL,TO_ROOM);
	pRoomIndex     = get_room_index(obj->specpower);
	if ( pRoomIndex == NULL ) return;
	char_from_room(ch);
	char_to_room(ch,pRoomIndex);
	do_look(ch,"auto");
    	if (obj->chpoweroff != NULL && obj->chpoweroff != '\0'
	    && str_cmp(obj->chpoweroff,"(null)") )
	    kavitem(str_dup(obj->chpoweroff),ch,obj,NULL,TO_CHAR);
    	if (obj->victpoweroff != NULL && obj->victpoweroff != '\0'
	    && str_cmp(obj->victpoweroff,"(null)") )
	    kavitem(str_dup(obj->victpoweroff),ch,obj,NULL,TO_ROOM);
       if (!IS_SET(obj->quest, QUEST_ARTIFACT) &&
            IS_SET(ch->in_room->room_flags, ROOM_NO_TELEPORT) &&
            CAN_WEAR(obj,ITEM_TAKE))
        {
            send_to_char("A powerful force hurls you from the room.\n\r",ch);
            act("$n is hurled from the room by a powerful force.",ch,NULL,NULL,TO_ROOM);
            ch->position = POS_STUNNED;
            char_from_room(ch);
            char_to_room(ch,get_room_index(ROOM_VNUM_LIMBO));
            act("$n appears in the room, and falls to the ground stunned.",ch,NULL,NULL,TO_ROOM);
        }
    	if ( (mount = ch->mount) == NULL) return;
    	char_from_room( mount );
    	char_to_room( mount, ch->in_room );
    	do_look( mount, "auto" );
	return;
    }
    else if ( IS_SET(obj->spectype, SITEM_OBJECT))
    {
	if ( get_obj_index(obj->specpower) == NULL ) return;
	obj2 = create_object(get_obj_index(obj->specpower), ch->level);
	if ( CAN_WEAR(obj2,ITEM_TAKE) ) obj_to_char(obj2,ch);
	else                           obj_to_room(obj2,ch->in_room);
    }
    else if ( IS_SET(obj->spectype, SITEM_MOBILE))
    {
	if ( get_mob_index(obj->specpower) == NULL ) return;
	mob = create_mobile(get_mob_index(obj->specpower));
	char_to_room(mob,ch->in_room);
    }
    else if ( IS_SET(obj->spectype, SITEM_ACTION))
    {
	interpret(ch,obj->victpoweron);
	if (obj->victpoweroff != NULL && str_cmp(obj->victpoweroff,"(null)") &&
	    obj->victpoweroff != '\0')
	{
	    for ( victim = char_list; victim != NULL; victim = victim_next )
	    {
		victim_next	= victim->next;
		if ( victim->in_room == NULL ) continue;
		if ( victim == ch ) continue;
		if ( victim->in_room == ch->in_room )
		{
		    interpret(victim,obj->victpoweroff);
		    continue;
		}
	    }
	}
    }
    return;
}

bool is_ok_to_wear( CHAR_DATA *ch, bool wolf_ok, char *argument )
{
    char arg [MAX_INPUT_LENGTH];
    int count;
    argument = one_argument( argument, arg );

    if (!str_cmp(arg,"head"))
    {
	if (IS_HEAD(ch,LOST_HEAD)) return FALSE;
    }
    else if (!str_cmp(arg,"face"))
    {
	if (IS_HEAD(ch,LOST_HEAD)) return FALSE;
    }
    else if (!str_cmp(arg,"left_hand"))
    {
	if (!IS_NPC(ch) && IS_SET(ch->special, SPC_WOLFMAN) && !wolf_ok) return FALSE;
	if (IS_ARM_L(ch,LOST_ARM)) return FALSE;
	else if (IS_ARM_L(ch,BROKEN_ARM)) return FALSE;
	else if (IS_ARM_L(ch,LOST_HAND)) return FALSE;
	else if (IS_ARM_L(ch,BROKEN_THUMB)) return FALSE;
	else if (IS_ARM_L(ch,LOST_THUMB)) return FALSE;
	count = 0;
	if (IS_ARM_L(ch,LOST_FINGER_I)||IS_ARM_L(ch,BROKEN_FINGER_I)) count+= 1;
	if (IS_ARM_L(ch,LOST_FINGER_M)||IS_ARM_L(ch,BROKEN_FINGER_M)) count+= 1;
	if (IS_ARM_L(ch,LOST_FINGER_R)||IS_ARM_L(ch,BROKEN_FINGER_R)) count+= 1;
	if (IS_ARM_L(ch,LOST_FINGER_L)||IS_ARM_L(ch,BROKEN_FINGER_L)) count+= 1;
	if (count > 2) return FALSE;
    }
    else if (!str_cmp(arg,"right_hand"))
    {
	if (!IS_NPC(ch) && IS_SET(ch->special, SPC_WOLFMAN) && !wolf_ok) return FALSE;
	if (IS_ARM_R(ch,LOST_ARM)) return FALSE;
	else if (IS_ARM_R(ch,BROKEN_ARM)) return FALSE;
	else if (IS_ARM_R(ch,LOST_HAND)) return FALSE;
	else if (IS_ARM_R(ch,BROKEN_THUMB)) return FALSE;
	else if (IS_ARM_R(ch,LOST_THUMB)) return FALSE;
	count = 0;
	if (IS_ARM_R(ch,LOST_FINGER_I)||IS_ARM_R(ch,BROKEN_FINGER_I)) count+= 1;
	if (IS_ARM_R(ch,LOST_FINGER_M)||IS_ARM_R(ch,BROKEN_FINGER_M)) count+= 1;
	if (IS_ARM_R(ch,LOST_FINGER_R)||IS_ARM_R(ch,BROKEN_FINGER_R)) count+= 1;
	if (IS_ARM_R(ch,LOST_FINGER_L)||IS_ARM_R(ch,BROKEN_FINGER_L)) count+= 1;
	if (count > 2) return FALSE;
    }
    else if (!str_cmp(arg,"third_hand"))
    {
      if (!IS_SET(ch->newbits, THIRD_HAND)) return FALSE;
	if (!IS_NPC(ch) && IS_SET(ch->special, SPC_WOLFMAN) && !wolf_ok) return FALSE;
/*
	if (IS_ARM_T(ch,LOST_ARM)) return FALSE;
	else if (IS_ARM_T(ch,BROKEN_ARM)) return FALSE;
	else if (IS_ARM_T(ch,LOST_HAND)) return FALSE;
	else if (IS_ARM_T(ch,BROKEN_THUMB)) return FALSE;
	else if (IS_ARM_T(ch,LOST_THUMB)) return FALSE;
	count = 0;
	if (IS_ARM_T(ch,LOST_FINGER_I)||IS_ARM_T(ch,BROKEN_FINGER_I)) count+= 1;
	if (IS_ARM_T(ch,LOST_FINGER_M)||IS_ARM_T(ch,BROKEN_FINGER_M)) count+= 1;
	if (IS_ARM_T(ch,LOST_FINGER_R)||IS_ARM_T(ch,BROKEN_FINGER_R)) count+= 1;
	if (IS_ARM_T(ch,LOST_FINGER_L)||IS_ARM_T(ch,BROKEN_FINGER_L)) count+= 1;
	if (count > 2) return FALSE;
*/
    }
    else if (!str_cmp(arg,"fourth_hand"))
    {
      if (!IS_SET(ch->newbits, FOURTH_HAND)) return FALSE;
	if (!IS_NPC(ch) && IS_SET(ch->special, SPC_WOLFMAN) && !wolf_ok) return FALSE;
/*
	if (IS_ARM_F(ch,LOST_ARM)) return FALSE;
	else if (IS_ARM_F(ch,BROKEN_ARM)) return FALSE;
	else if (IS_ARM_F(ch,LOST_HAND)) return FALSE;
	else if (IS_ARM_F(ch,BROKEN_THUMB)) return FALSE;
	else if (IS_ARM_F(ch,LOST_THUMB)) return FALSE;
	count = 0;
	if (IS_ARM_F(ch,LOST_FINGER_I)||IS_ARM_F(ch,BROKEN_FINGER_I)) count+= 1;
	if (IS_ARM_F(ch,LOST_FINGER_M)||IS_ARM_F(ch,BROKEN_FINGER_M)) count+= 1;
	if (IS_ARM_F(ch,LOST_FINGER_R)||IS_ARM_F(ch,BROKEN_FINGER_R)) count+= 1;
	if (IS_ARM_F(ch,LOST_FINGER_L)||IS_ARM_F(ch,BROKEN_FINGER_L)) count+= 1;
	if (count > 2) return FALSE;
*/
    }
    else if (!str_cmp(arg,"left_wrist"))
    {
	if (IS_ARM_L(ch,LOST_ARM)) return FALSE;
	else if (IS_ARM_L(ch,LOST_HAND)) return FALSE;
    }
    else if (!str_cmp(arg,"right_wrist"))
    {
	if (IS_ARM_R(ch,LOST_ARM)) return FALSE;
	else if (IS_ARM_R(ch,LOST_HAND)) return FALSE;
    }
    else if (!str_cmp(arg,"left_finger"))
    {
	if (IS_ARM_L(ch,LOST_ARM)) return FALSE;
	else if (IS_ARM_L(ch,LOST_HAND)) return FALSE;
	else if (IS_ARM_L(ch,LOST_FINGER_R)) return FALSE;
    }
    else if (!str_cmp(arg,"right_finger"))
    {
	if (IS_ARM_R(ch,LOST_ARM)) return FALSE;
	else if (IS_ARM_R(ch,LOST_HAND)) return FALSE;
	else if (IS_ARM_R(ch,LOST_FINGER_R)) return FALSE;
    }
    else if (!str_cmp(arg,"arms"))
    {
	if (IS_ARM_L(ch,LOST_ARM) && IS_ARM_R(ch,LOST_ARM)) return FALSE;
    }
    else if (!str_cmp(arg,"hands"))
    {
	if (IS_ARM_L(ch,LOST_ARM) && IS_ARM_R(ch,LOST_ARM)) return FALSE;
	if (IS_ARM_L(ch,LOST_HAND) || IS_ARM_R(ch,LOST_HAND)) return FALSE;
    }
    else if (!str_cmp(arg,"legs"))
    {
	if (IS_LEG_L(ch,LOST_LEG) && IS_LEG_R(ch,LOST_LEG)) return FALSE;
    }
    else if (!str_cmp(arg,"feet"))
    {
	if (IS_LEG_L(ch,LOST_LEG) && IS_LEG_R(ch,LOST_LEG)) return FALSE;
	if (IS_LEG_L(ch,LOST_FOOT) || IS_LEG_R(ch,LOST_FOOT)) return FALSE;
    }
    return TRUE;
}

void do_qmake( CHAR_DATA *ch, char *argument )
{
    OBJ_INDEX_DATA *pObjIndex;
    OBJ_DATA *obj;
    char arg[MAX_INPUT_LENGTH];

    argument = one_argument( argument, arg );

    if (arg[0] == '\0')
    {
	send_to_char("Do you wish to qmake a MACHINE or a CARD?\n\r",ch);
	return;
    }
    if (!str_cmp(arg,"card"))
    {
    	if ( (pObjIndex = get_obj_index( OBJ_VNUM_QUESTCARD )) == NULL)
    	{
	    send_to_char("Missing object, please inform KaVir.\n\r",ch);
	    return;
    	}
        if (ch->in_room == NULL) return;
        obj = create_object(pObjIndex, 0);
        obj_to_char(obj, ch);
	quest_object(ch,obj);
    }
    else if (!str_cmp(arg,"machine"))
    {
    	if ( (pObjIndex = get_obj_index( OBJ_VNUM_QUESTMACHINE )) == NULL)
    	{
	    send_to_char("Missing object, please inform KaVir.\n\r",ch);
	    return;
    	}
        if (ch->in_room == NULL) return;
        obj = create_object(pObjIndex, 0);
        obj_to_room(obj, ch->in_room);
    }
    else
    {
	send_to_char("You can only qmake a MACHINE or a CARD.\n\r",ch);
	return;
    }
    send_to_char("Ok.\n\r",ch);
    return;
}

void do_recharge( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_INPUT_LENGTH];
    char arg1[MAX_INPUT_LENGTH];
    char arg2[MAX_INPUT_LENGTH];
    OBJ_DATA *obj;
    OBJ_DATA *qobj;
    int count = 0;
    int value = 1;

    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );

    if ( arg1[0] == '\0' || arg2[0] == '\0' )
    {
	send_to_char( "Syntax: recharge <quest card> <quest machine>\n\r", ch );
	return;
    }
    if ( (obj = get_obj_carry(ch,arg1)) == NULL )
    {
	send_to_char( "You are not carrying that object.\n\r", ch );
	return;
    }
    if (obj->item_type != ITEM_QUESTCARD)
    {
	send_to_char( "That is not a quest card.\n\r", ch );
	return;
    }
    if ( (qobj = get_obj_here(ch,arg2)) == NULL )
    {
	send_to_char( "There is nothing for you to recharge it with.\n\r", ch );
	return;
    }
    if (qobj->item_type != ITEM_QUESTMACHINE)
    {
	send_to_char( "That is not a quest machine.\n\r", ch );
	return;
    }
    if (obj->value[0] == -1) count += 1;
    if (obj->value[1] == -1) count += 1;
    if (obj->value[2] == -1) count += 1;
    if (obj->value[3] == -1) count += 1;
    if (count == 4) quest_object( ch, obj );
    else
    {
	send_to_char( "You have not yet completed the current quest.\n\r", ch );
	return;
    }
    act("You place $p into a small slot in $P.",ch,obj,qobj,TO_CHAR);
    act("$n places $p into a small slot in $P.",ch,obj,qobj,TO_ROOM);
    act("$P makes a few clicks and returns $p.",ch,obj,qobj,TO_CHAR);
    act("$P makes a few clicks and returns $p.",ch,obj,qobj,TO_ROOM);
    value = obj->level;
    if (value < 1) value = 1; else if (value > 50) value = 50;

    obj = create_object(get_obj_index( OBJ_VNUM_PROTOPLASM ), 0);
    obj->value[0] = value;
    obj->level = value;
    obj->cost = value*1000;
    obj->item_type = ITEM_QUEST;
    obj_to_char(obj,ch);
    if (obj->questmaker != NULL) free_string(obj->questmaker);
    obj->questmaker = str_dup(ch->name);
    free_string( obj->name );
    obj->name = str_dup( "quest token" );
    free_string( obj->short_descr );
    sprintf(buf,"a %d point quest token",value);
    obj->short_descr = str_dup( buf );
    free_string( obj->description );
    sprintf(buf,"A %d point quest token lies on the floor.",value);
    obj->description = str_dup( buf );
    act("You take $p from $P.",ch,obj,qobj,TO_CHAR);
    act("$n takes $p from $P.",ch,obj,qobj,TO_ROOM);
    if (!IS_NPC(ch))
    {
	ch->pcdata->score[SCORE_NUM_QUEST]++;
	ch->pcdata->score[SCORE_QUEST] += value;
	sprintf(buf,"%s has completed a quest!.",ch->name);
    }
    else sprintf(buf,"%s has completed a quest!.",ch->pcdata->switchname);
    buf[0] = UPPER(buf[0]);
    do_info(ch,buf);
    do_autosave(ch,"");
    return;
}

void quest_object( CHAR_DATA *ch, OBJ_DATA *obj )
{
    static const sh_int quest_selection[] = 
    {
	 102,
	 9201, 9225,  605, 1329, 2276, 5112, 6513, 6517, 6519, 5001,
	 5005, 5011, 5012, 5013, 2902, 1352, 2348, 2361, 3005, 5011,
	 5012, 5013, 2902, 1352, 2348, 2361, 3005,  300,  303,  307,
	 7216, 1100,  100,30315, 5110, 6001, 3050,  301, 5230,30302,
	  663, 7303, 2915, 2275, 8600, 8601, 8602, 8603, 5030, 9321,
	 6010, 1304, 1307, 1332, 1333, 1342, 1356, 1361, 2304, 2322,
	 2331, 2382, 8003, 8005, 5300, 5302, 5309, 5310, 5311, 4000,
	  601,  664,  900,  906,  923,  311, 7203, 7206, 1101, 5214,
	 5223, 5228, 2804, 1612, 5207, 9302, 5301, 5224, 7801, 9313,
	 6304, 2003, 3425, 3423, 608,  1109,30319, 8903, 9317, 9307,
	 4050,  911, 2204, 4100, 3428,  310, 5113, 3402, 5319, 6512,
	 5114,  913,30316, 2106, 8007, 6601, 2333, 3610, 2015, 5022,
	 1394, 2202, 1401, 6005, 1614,  647, 1388, 9311, 3604, 4701,
	30325, 6106, 2003, 7190, 9322, 1384, 3412, 2342, 1374, 2210,
	 2332, 2901, 7200, 7824, 3410, 2013, 1510, 8306, 3414, 2005
    };
    int object;
    if (obj == NULL || obj->item_type != ITEM_QUESTCARD) return;

    object = number_range(obj->level, obj->level + 100);
    if (object < 1 || object > 150) object = 0;
    obj->value[0] = quest_selection[object];

    object = number_range(obj->level, obj->level + 100);
    if (object < 1 || object > 150) object = 0;
    obj->value[1] = quest_selection[object];

    object = number_range(obj->level, obj->level + 100);
    if (object < 1 || object > 150) object = 0;
    obj->value[2] = quest_selection[object];

    object = number_range(obj->level, obj->level + 100);
    if (object < 1 || object > 150) object = 0;
    obj->value[3] = quest_selection[object];
    return;
}

void do_complete( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_STRING_LENGTH];
    char arg1[MAX_INPUT_LENGTH];
    char arg2[MAX_INPUT_LENGTH];
    OBJ_DATA *qobj;
    OBJ_DATA *obj;
    OBJ_INDEX_DATA *pObjIndex;
    int count = 0;
    int count2 = 0;

    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );

    if ( arg1[0] == '\0' )
    {
	send_to_char( "Syntax: complete <quest card> <object>\n\r", ch );
	return;
    }

    if ( (qobj = get_obj_carry(ch,arg1)) == NULL )
    {
	send_to_char( "You are not carrying that object.\n\r", ch );
	return;
    }
    else if (qobj->item_type != ITEM_QUESTCARD)
    {
	send_to_char( "That is not a quest card.\n\r", ch );
	return;
    }
    if (qobj->value[0] == -1) count += 1;
    if (qobj->value[1] == -1) count += 1;
    if (qobj->value[2] == -1) count += 1;
    if (qobj->value[3] == -1) count += 1;

    if ( arg2[0] == '\0' )
    {
	if (count == 4) {send_to_char("This quest card has been completed.\n\r",ch);return;}
	send_to_char("You still need to find the following:\n\r",ch);
	if (qobj->value[0] != -1)
	    {pObjIndex = get_obj_index( qobj->value[0] );
	    if ( pObjIndex != NULL )
		sprintf(buf,"     %s.\n\r",pObjIndex->short_descr);
		buf[5] = UPPER(buf[5]);send_to_char(buf,ch);}
	if (qobj->value[1] != -1)
	    {pObjIndex = get_obj_index( qobj->value[1] );
	    if ( pObjIndex != NULL )
		sprintf(buf,"     %s.\n\r",pObjIndex->short_descr);
		buf[5] = UPPER(buf[5]);send_to_char(buf,ch);}
	if (qobj->value[2] != -1)
	    {pObjIndex = get_obj_index( qobj->value[2] );
	    if ( pObjIndex != NULL )
		sprintf(buf,"     %s.\n\r",pObjIndex->short_descr);
		buf[5] = UPPER(buf[5]);send_to_char(buf,ch);}
	if (qobj->value[3] != -1)
	    {pObjIndex = get_obj_index( qobj->value[3] );
	    if ( pObjIndex != NULL )
		sprintf(buf,"     %s.\n\r",pObjIndex->short_descr);
		buf[5] = UPPER(buf[5]);send_to_char(buf,ch);}
	return;
    }

    if (count == 4)
    {
	act( "But $p has already been completed!",ch,qobj,NULL,TO_CHAR );
	return;
    }

    if ( (obj = get_obj_carry(ch,arg2)) == NULL )
    {
	send_to_char( "You are not carrying that object.\n\r", ch );
	return;
    }
    if (obj->questmaker != NULL && strlen(obj->questmaker) > 1)
    {
	send_to_char( "You cannot use that item.\n\r", ch );
	return;
    }
    if (obj->pIndexData->vnum == 30037 || obj->pIndexData->vnum == 30041)
    {
	send_to_char( "That item has lost its quest value, you must collect a new one.\n\r", ch );
	return;
    }
    if (qobj->value[0] != -1)
	{pObjIndex = get_obj_index( qobj->value[0] );
	if ( pObjIndex != NULL &&
	!str_cmp(obj->short_descr, pObjIndex->short_descr))
	qobj->value[0] = -1;}
    if (qobj->value[1] != -1)
	{pObjIndex = get_obj_index( qobj->value[1] );
	if ( pObjIndex != NULL &&
	!str_cmp(obj->short_descr, pObjIndex->short_descr))
	qobj->value[1] = -1;}
    if (qobj->value[2] != -1)
	{pObjIndex = get_obj_index( qobj->value[2] );
	if ( pObjIndex != NULL &&
	!str_cmp(obj->short_descr, pObjIndex->short_descr))
	qobj->value[2] = -1;}
    if (qobj->value[3] != -1)
	{pObjIndex = get_obj_index( qobj->value[3] );
	if ( pObjIndex != NULL &&
	!str_cmp(obj->short_descr, pObjIndex->short_descr))
	qobj->value[3] = -1;}
    if (qobj->value[0] == -1) count2 += 1;
    if (qobj->value[1] == -1) count2 += 1;
    if (qobj->value[2] == -1) count2 += 1;
    if (qobj->value[3] == -1) count2 += 1;
    if (count == count2)
    {
	send_to_char( "That item is not required.\n\r", ch );
	return;
    }

    act("You touch $p to $P, and $p vanishes!",ch,obj,qobj,TO_CHAR);
    act("$n touches $p to $P, and $p vanishes!",ch,obj,qobj,TO_ROOM);
    obj_from_char(obj);
    extract_obj(obj);
    if (count >= 3) {act("$p has been completed!",ch,qobj,NULL,TO_CHAR);}
    else if (count == 2) {act("$p now requires one more object!",ch,qobj,NULL,TO_CHAR);}
    else if (count == 1) {act("$p now requires two more objects!",ch,qobj,NULL,TO_CHAR);}
    else if (count == 0) {act("$p now requires three more objects!",ch,qobj,NULL,TO_CHAR);}
    return;
}

void do_sheath( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];

    one_argument(argument,arg);

    if (arg[0] == '\0') send_to_char("Which hand, left or right?\n\r",ch);
    else if (!str_cmp(arg,"all") || !str_cmp(arg,"both"))
    {
	sheath(ch,TRUE );
	sheath(ch,FALSE);
    }
    else if (!str_cmp(arg,"l") || !str_cmp(arg,"left" )) sheath(ch,FALSE);
    else if (!str_cmp(arg,"r") || !str_cmp(arg,"right")) sheath(ch,TRUE );
    else send_to_char("Which hand, left or right?\n\r",ch);
    return;
}

void do_draw( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];

    one_argument(argument,arg);

    if (!IS_NPC(ch) && IS_SET(ch->special, SPC_WOLFMAN))
    {
	send_to_char("Not in this form.\n\r",ch);
	return;
    }
    if (arg[0] == '\0') send_to_char("Which hand, left or right?\n\r",ch);
    else if (!str_cmp(arg,"all") || !str_cmp(arg,"both"))
    {
	draw(ch,TRUE );
	draw(ch,FALSE);
    }
    else if (!str_cmp(arg,"l") || !str_cmp(arg,"left" )) draw(ch,FALSE);
    else if (!str_cmp(arg,"r") || !str_cmp(arg,"right")) draw(ch,TRUE );
    else send_to_char("Which hand, left or right?\n\r",ch);
    return;
}

void sheath( CHAR_DATA *ch, bool right )
{
    OBJ_DATA *obj;
    OBJ_DATA *obj2;
    int scabbard;

    if (right)
    {
	scabbard = WEAR_SCABBARD_R;
	if ( (obj = get_eq_char(ch,WEAR_WIELD)) == NULL )
	{
	    send_to_char("You are not holding anything in your right hand.\n\r",ch);
	    return;
	}
	else if ( (obj2 = get_eq_char(ch,scabbard)) != NULL)
	{
	    act("You already have $p in your right scabbard.",ch,obj2,NULL,TO_CHAR);
	    return;
	}
    	act("You slide $p into your right scabbard.",ch,obj,NULL,TO_CHAR);
    	act("$n slides $p into $s right scabbard.",ch,obj,NULL,TO_ROOM);
    }
    else
    {
	scabbard = WEAR_SCABBARD_L;
	if ( (obj = get_eq_char(ch,WEAR_HOLD)) == NULL )
	{
	    send_to_char("You are not holding anything in your left hand.\n\r",ch);
	    return;
	}
	else if ( (obj2 = get_eq_char(ch,scabbard)) != NULL)
	{
	    act("You already have $p in your left scabbard.",ch,obj2,NULL,TO_CHAR);
	    return;
	}
    	act("You slide $p into your left scabbard.",ch,obj,NULL,TO_CHAR);
    	act("$n slides $p into $s left scabbard.",ch,obj,NULL,TO_ROOM);
    }
    if (obj->item_type != ITEM_WEAPON)
    {
	act("$p is not a weapon.",ch,obj,NULL,TO_CHAR);
	return;
    }
    unequip_char(ch,obj);
    obj->wear_loc = scabbard;
    return;
}

void draw( CHAR_DATA *ch, bool right )
{
    OBJ_DATA *obj;
    OBJ_DATA *obj2;
    int scabbard;
    int worn;
    if (right)
    {
	scabbard = WEAR_SCABBARD_R;
	worn = WEAR_WIELD;
	if ( (obj = get_eq_char(ch,scabbard)) == NULL )
	{
	    send_to_char("Your right scabbard is empty.\n\r",ch);
	    return;
	}
	else if ( (obj2 = get_eq_char(ch,WEAR_WIELD)) != NULL)
	{
	    act("You already have $p in your right hand.",ch,obj2,NULL,TO_CHAR);
	    return;
	}
    	act("You draw $p from your right scabbard.",ch,obj,NULL,TO_CHAR);
    	act("$n draws $p from $s right scabbard.",ch,obj,NULL,TO_ROOM);
    }
    else
    {
	scabbard = WEAR_SCABBARD_L;
	worn = WEAR_HOLD;
	if ( (obj = get_eq_char(ch,scabbard)) == NULL )
	{
	    send_to_char("Your left scabbard is empty.\n\r",ch);
	    return;
	}
	else if ( (obj2 = get_eq_char(ch,WEAR_HOLD)) != NULL)
	{
	    act("You already have $p in your left hand.",ch,obj2,NULL,TO_CHAR);
	    return;
	}
    	act("You draw $p from your left scabbard.",ch,obj,NULL,TO_CHAR);
    	act("$n draws $p from $s left scabbard.",ch,obj,NULL,TO_ROOM);
    }
    obj->wear_loc = -1;
    equip_char(ch,obj,worn);
    return;
}

void do_special( CHAR_DATA *ch, char *argument )
{
    char bname[MAX_INPUT_LENGTH];
    char bshort[MAX_INPUT_LENGTH];
    char blong[MAX_INPUT_LENGTH];
    char *kav;
    int dice = number_range(1,3);
    OBJ_DATA *obj;

    obj = create_object(get_obj_index( OBJ_VNUM_PROTOPLASM ), 0);

    kav = special_item_name( obj );

    switch ( dice )
    {
    default:
	sprintf(bname,"%s ring", kav);
	sprintf(bshort,"a %s ring", kav);
	sprintf(blong,"A %s ring lies here.", kav);
	obj->wear_flags = ITEM_WEAR_FINGER + ITEM_TAKE;
	break;
    case 1:
	sprintf(bname,"%s ring", kav);
	sprintf(bshort,"a %s ring", kav);
	sprintf(blong,"A %s ring lies here.", kav);
	obj->wear_flags = ITEM_WEAR_FINGER + ITEM_TAKE;
	break;
    case 2:
	sprintf(bname,"%s necklace", kav);
	sprintf(bshort,"a %s necklace", kav);
	sprintf(blong,"A %s necklace lies here.", kav);
	obj->wear_flags = ITEM_WEAR_NECK + ITEM_TAKE;
	break;
    case 3:
	sprintf(bname,"%s plate", kav);
	sprintf(bshort,"a suit of %s platemail", kav);
	sprintf(blong,"A suit of %s platemail lies here.", kav);
	obj->wear_flags = ITEM_WEAR_BODY + ITEM_TAKE;
	break;
    }

    if (obj->wear_flags == 513 || obj->wear_flags == 8193 || 
	obj->wear_flags == 16385)
    {
	obj->item_type = ITEM_WEAPON;
    	obj->value[1] = 10;
    	obj->value[2] = 20;
    	obj->value[3] = number_range(1,12);
    }
    else
    {
	obj->item_type = ITEM_ARMOR;
    	obj->value[0] = 15;
    }

    obj->level = 50;
    obj->cost = 100000;

    if (obj->questmaker != NULL) free_string(obj->questmaker);
    obj->questmaker = str_dup(ch->name);

    free_string( obj->name );
    obj->name = str_dup( bname );

    free_string( obj->short_descr );
    obj->short_descr = str_dup( bshort );

    free_string( obj->description );
    obj->description = str_dup( blong );

    obj_to_char(obj,ch);
    return;
}

char *special_item_name( OBJ_DATA *obj )
{
    static char buf[MAX_INPUT_LENGTH];
    int dice = number_range(1,4);
    switch ( dice )
    {
    default:
	strcpy(buf,"golden");
	break;
    case 1:
	strcpy(buf,"golden");
	break;
    case 2:
	strcpy(buf,"silver");
	break;
    case 3:
	strcpy(buf,"brass");
	break;
    case 4:
	strcpy(buf,"copper");
	break;
    }
    return buf;
}

