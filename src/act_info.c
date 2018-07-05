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
#include<types.h>
#else
#include <sys/types.h>
#endif
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "merc.h"


char *	const	where_name	[] =
{
    "[Light         ] ",
    "[On Finger     ] ",
    "[On Finger     ] ",
    "[Around Neck   ] ",
    "[Around Neck   ] ",
    "[On Body       ] ",
    "[On Head       ] ",
    "[On Legs       ] ",
    "[On Feet       ] ",
    "[On Hands      ] ",
    "[On Arms       ] ",
    "[Off Hand      ] ",
    "[Around Body   ] ",
    "[Around Waist  ] ",
    "[Around Wrist  ] ",
    "[Around Wrist  ] ",
    "[Right Hand    ] ",
    "[Left Hand     ] ",
    "[Third Hand    ] ",
    "[Fourth Hand   ] ",
    "[On Face       ] ",
    "[Left Scabbard ] ",
    "[Right Scabbard] ",
};

const char * exitname [6] =
{
	"north",
	"east",
	"south",
	"west",
	"up",
	"down"
};

const char * exitname2 [6] =
{
	"to the north",
	"to the east",
	"to the south",
	"to the west",
	"above you",
	"below you"
};


/*
 * Local functions.
 */
char *	format_obj_to_char	args( ( OBJ_DATA *obj, CHAR_DATA *ch,
				    bool fShort ) );
void	show_char_to_char_0	args( ( CHAR_DATA *victim, CHAR_DATA *ch ) );
void	show_char_to_char_1	args( ( CHAR_DATA *victim, CHAR_DATA *ch ) );
void	show_char_to_char	args( ( CHAR_DATA *list, CHAR_DATA *ch ) );
bool	check_blind		args( ( CHAR_DATA *ch ) );

void	evil_eye		args( ( CHAR_DATA *victim, CHAR_DATA *ch ) );
void 	check_left_arm		args( ( CHAR_DATA *ch, CHAR_DATA *victim ) );
void 	check_right_arm		args( ( CHAR_DATA *ch, CHAR_DATA *victim ) );
void 	check_left_leg		args( ( CHAR_DATA *ch, CHAR_DATA *victim ) );
void 	check_right_leg		args( ( CHAR_DATA *ch, CHAR_DATA *victim ) );

void	obj_score		args( ( CHAR_DATA *ch, OBJ_DATA *obj ) );


char *format_obj_to_char( OBJ_DATA *obj, CHAR_DATA *ch, bool fShort )
{
    static char buf[MAX_STRING_LENGTH];

    buf[0] = '\0';
    if ( IS_SET(obj->quest, QUEST_ARTIFACT)) strcat(buf, "#7(Artifact)#n "
);
    else if ( IS_SET(obj->quest, QUEST_RELIC)) strcat(buf,"#3(#7Relic#3)#n " );
    else if ( obj->points < 750 && obj->points != 0) strcat(buf,"#3(Legendary)#n " );
    else if ( obj->points < 1250 && obj->points != 0 ) strcat(buf,
"#7(#2Mythical#7)#n " );
    else if ( obj->points < 3000 && obj->points !=0) strcat(buf,
"#6(#3Priceless#6)#n " );	
    if ( IS_OBJ_STAT(obj, ITEM_INVIS)     )   strcat( buf, "#6(Invis)#n "
);
    if ( IS_AFFECTED(ch, AFF_DETECT_EVIL)
         && !IS_OBJ_STAT(obj, ITEM_ANTI_GOOD)
         && IS_OBJ_STAT(obj, ITEM_ANTI_EVIL)   )   strcat( buf, "#4(Blue Aura)#n "  );
    else if ( IS_AFFECTED(ch, AFF_DETECT_EVIL)
         && IS_OBJ_STAT(obj, ITEM_ANTI_GOOD)
         && !IS_OBJ_STAT(obj, ITEM_ANTI_EVIL)   )   strcat( buf, "#1(Red Aura)#n "  );
    else if ( IS_AFFECTED(ch, AFF_DETECT_EVIL)
         && IS_OBJ_STAT(obj, ITEM_ANTI_GOOD)
         && !IS_OBJ_STAT(obj, ITEM_ANTI_NEUTRAL)
         && IS_OBJ_STAT(obj, ITEM_ANTI_EVIL)   )   strcat( buf, "#3(Yellow Aura)#n "  );
    if ( IS_AFFECTED(ch, AFF_DETECT_MAGIC)
         && IS_OBJ_STAT(obj, ITEM_MAGIC)  )   strcat( buf, "#4(Magical)#n 
" );
        if (IS_SET(obj->spectype, SITEM_COPPER))  strcat(buf,"(Copper) ");
    if (IS_SET(obj->spectype, SITEM_IRON))  strcat(buf,"(Iron) ");
    if (IS_SET(obj->spectype, SITEM_STEEL))  strcat(buf,"(Steel) ");
    if (IS_SET(obj->spectype, SITEM_ADAMANTITE))  strcat(buf,"(Adamantite) ");
    if (IS_SET(obj->spectype, SITEM_HILT))  strcat(buf,"(Hilted) ");
    if (IS_SET(obj->spectype, SITEM_GEMSTONE)) strcat(buf,"(Gemstoned) ");
    if (obj->value[0] == 18000 && obj->item_type == ITEM_WEAPON)
       strcat(buf,"(Sharpened) ");    
    if ( IS_OBJ_STAT(obj, ITEM_SHADOWPLANE) &&
	 obj->in_room != NULL &&
	!IS_AFFECTED(ch,AFF_SHADOWPLANE) )    strcat( buf,
"#0(Shadowplane)#n " );
    if (!IS_OBJ_STAT(obj, ITEM_SHADOWPLANE) &&
	 obj->in_room != NULL &&
	 IS_AFFECTED(ch,AFF_SHADOWPLANE) )    strcat( buf, "#7(Normal plane)#n " );

    if ( fShort )
    {
	if ( obj->short_descr != NULL )
	    strcat( buf, obj->short_descr );
	if ( obj->condition < 100) strcat(buf, " #1(Damaged)#n");
    }
    else
    {
	if ( obj->description != NULL )
	    strcat( buf, obj->description );
    }

    return buf;
}

int char_damroll(CHAR_DATA *ch)
{
    int value = 0;

    value += ch->damroll;

	if (lost_kpower(ch)) value -=50;

    if (!IS_NPC(ch) && ch->generation <= 0)
       ch->generation = 4;

    if (!IS_NPC(ch)) 
    {
        value += str_app[get_curr_str(ch)].todam;
        value += ch->xdamroll;
    }

    /* Vampire Rage */
    if (IS_CLASS(ch, CLASS_VAMPIRE) && ch->rage > 0)
	  value += ch->rage;

	if (IS_CLASS(ch, CLASS_NINJA) && ch->rage > 0)
	value += ch->rage;

    /* Werewolf Rage */
    if (IS_SET(ch->special,SPC_WOLFMAN) && ch->rage > 99)
	  value += ch->rage;


        if (IS_CLASS( ch,CLASS_DEMON) && ch->pcdata->stats[DEMON_POWER] > 0)
	value += (( ch->pcdata->stats[DEMON_POWER]) * ch->pcdata->stats[DEMON_POWER]);
	else if (IS_SET(ch->special,SPC_CHAMPION) && ch->pcdata->stats[DEMON_POWER] > 0)
	value += (( ch->pcdata->stats[DEMON_POWER]) * ch->pcdata->stats[DEMON_POWER]);

	if (!IS_NPC(ch))
	if (IS_CLASS(ch, CLASS_NINJA) && ch->pcdata->powers[NPOWER_CHIKYU] >= 6
	&& ch->pcdata->powers[HARA_KIRI] > 0)
	value += 50;

    if (IS_CLASS(ch, CLASS_HIGHLANDER) && (get_eq_char(ch, WEAR_WIELD) != NULL))
    {
	int wpn = ch->wpn[1];
	  if (wpn >= 1000)
		value += 500;
	  else
		value += wpn/2;
    }
    return value;
}

int char_hitroll(CHAR_DATA *ch)
{
    int value = 0;

    value += ch->hitroll;

	if (lost_kpower(ch)) value -= 50;

    /* Stop boars having 239034 dam and hit for some reason */
    if (!IS_NPC(ch))
        value += str_app[get_curr_str(ch)].tohit;

    /* Vampire Rage */
    if (IS_CLASS(ch, CLASS_VAMPIRE) && ch->rage > 0)
	  value += ch->rage;

	if (IS_CLASS(ch, CLASS_NINJA) && ch->rage > 0)
	value += ch->rage;

	if (!IS_NPC(ch))
	if (IS_CLASS(ch, CLASS_NINJA) && ch->pcdata->powers[NPOWER_CHIKYU] >= 6
	&& ch->pcdata->powers[HARA_KIRI] > 0)
	value += 100;

    /* Werewolf Rage */
    if (IS_SET(ch->special,SPC_WOLFMAN) && ch->rage > 99)
	  value += ch->rage;

    if (IS_CLASS( ch,CLASS_DEMON) && ch->pcdata->stats[DEMON_POWER] > 0)
       value += (( ch->pcdata->stats[DEMON_POWER]) * ch->pcdata->stats[DEMON_POWER]);
        else if (IS_SET(ch->special,SPC_CHAMPION) && ch->pcdata->stats[DEMON_POWER] > 0)
        value += (( ch->pcdata->stats[DEMON_POWER]) * ch->pcdata->stats[DEMON_POWER]);

    /* Highlander */
    if (IS_CLASS(ch, CLASS_HIGHLANDER) && (get_eq_char(ch, WEAR_WIELD) != NULL))
    {
	int wpn = ch->wpn[1];
	  if (wpn >= 1000)
		value += 500;
	  else
		value += wpn/2;
    }

    return value;
}

int char_ac(CHAR_DATA *ch)
{
    int value = 0;

    value = GET_AC(ch);

	if (lost_kpower(ch)) value += 100;

    if (IS_AWAKE(ch))
   	value += dex_app[get_curr_dex(ch)].defensive;

    /* Highlander */
    if (IS_CLASS(ch, CLASS_HIGHLANDER) && (get_eq_char(ch, WEAR_WIELD) != NULL))
    {
	int wpn = ch->wpn[1];
	    value -= wpn;
    }

    return value;
}



/*
 * Show a list to a character.
 * Can coalesce duplicated items.
 */
void show_list_to_char( OBJ_DATA *list, CHAR_DATA *ch, bool fShort, bool fShowNothing )
{
    char buf[MAX_STRING_LENGTH];
    char **prgpstrShow;
    int *prgnShow;
    char *pstrShow;
    OBJ_DATA *obj;
    int nShow;
    int iShow;
    int count;
    bool fCombine;

    if ( ch->desc == NULL )
	return;

    /*
     * Alloc space for output lines.
     */
    count = 0;
    for ( obj = list; obj != NULL; obj = obj->next_content )
	count++;
    prgpstrShow	= alloc_mem( count * sizeof(char *) );
    prgnShow    = alloc_mem( count * sizeof(int)    );
    nShow	= 0;

    /*
     * Format the list of objects.
     */
    for ( obj = list; obj != NULL; obj = obj->next_content )
    { 
	if (!IS_NPC(ch) && ch->pcdata->chobj != NULL && obj->chobj != NULL && obj->chobj == ch)
	    continue;
	if ( obj->wear_loc == WEAR_NONE && can_see_obj( ch, obj ) )
	{
	    pstrShow = format_obj_to_char( obj, ch, fShort );
	    fCombine = FALSE;

	    if ( IS_NPC(ch) || IS_SET(ch->act, PLR_COMBINE) )
	    {
		/*
		 * Look for duplicates, case sensitive.
		 * Matches tend to be near end so run loop backwords.
		 */
		for ( iShow = nShow - 1; iShow >= 0; iShow-- )
		{
		    if ( !strcmp( prgpstrShow[iShow], pstrShow ) )
		    {
			prgnShow[iShow]++;
			fCombine = TRUE;
			break;
		    }
		}
	    }

	    /*
	     * Couldn't combine, or didn't want to.
	     */
	    if ( !fCombine )
	    {
		prgpstrShow [nShow] = str_dup( pstrShow );
		prgnShow    [nShow] = 1;
		nShow++;
	    }
	}
    }

    /*
     * Output the formatted list.
     */
    for ( iShow = 0; iShow < nShow; iShow++ )
    {
	if ( IS_NPC(ch) || IS_SET(ch->act, PLR_COMBINE) )
	{
	    if ( prgnShow[iShow] != 1 )
	    {
		sprintf( buf, "(%2d) ", prgnShow[iShow] );
		send_to_char( buf, ch );
	    }
	    else
	    {
		send_to_char( "     ", ch );
	    }
	}
	send_to_char( prgpstrShow[iShow], ch );
	send_to_char( "\n\r", ch );
	free_string( prgpstrShow[iShow] );
    }

    if ( fShowNothing && nShow == 0 )
    {
	if ( IS_NPC(ch) || IS_SET(ch->act, PLR_COMBINE) )
	    send_to_char( "     ", ch );
	send_to_char( "Nothing.\n\r", ch );
    }

    /*
     * Clean up.
     */
    free_mem( prgpstrShow, count * sizeof(char *) );
    free_mem( prgnShow,    count * sizeof(int)    );

    return;
}



void show_char_to_char_0( CHAR_DATA *victim, CHAR_DATA *ch )
{
    char buf[MAX_STRING_LENGTH];
    char buf2[MAX_STRING_LENGTH];
    char buf3[MAX_STRING_LENGTH];
    char buf4[MAX_STRING_LENGTH];
    char buf6[MAX_STRING_LENGTH];
    char buf7[MAX_STRING_LENGTH];
    char mount2[MAX_STRING_LENGTH];
    CHAR_DATA *mount;

    buf[0] = '\0';
    buf2[0] = '\0';
    buf3[0] = '\0';
    buf6[0] = '\0';


    if (!IS_NPC(victim) && victim->pcdata->chobj != NULL )
	return;

    if ((mount = victim->mount) != NULL && IS_SET(victim->mounted, IS_MOUNT))
	return;

    if ( !IS_NPC(victim) && IS_SET(victim->flag2, VAMP_OBJMASK) )
    {
	sprintf(buf, "     %s\n\r",victim->objdesc);
	stc(buf,ch);
	return;
    }

     if (victim->cur_form != 0)
    {
    buf[0] = '\0';
   
    if ( IS_HEAD(victim, LOST_HEAD) && IS_AFFECTED(victim, AFF_POLYMORPH))
	strcat( buf, "     " );
 else {
	if (!IS_NPC(victim) && victim->desc==NULL ) strcat( buf,"(Link-Dead) ");
    	if ( IS_AFFECTED(victim, AFF_INVISIBLE)   ) strcat( buf, "(Invis) "   );
    	if ( IS_AFFECTED(victim, AFF_HIDE)        ) strcat( buf, "(Hide) " );
    	if ( IS_AFFECTED(victim, AFF_CHARM)       ) strcat( buf, "(Charmed) " );
    	if ( IS_AFFECTED(victim, AFF_PASS_DOOR)  || 
           IS_AFFECTED(victim, AFF_ETHEREAL)    ) strcat( buf, "(Translucent) ");
    	if ( IS_AFFECTED(victim, AFF_FAERIE_FIRE) ) strcat( buf, "(Pink Aura) "  );
    	if ( IS_EVIL(victim)
    	&&   IS_AFFECTED(ch, AFF_DETECT_EVIL)     ) strcat( buf, "(Red Aura) "   );
    	if ( IS_AFFECTED(victim, AFF_SANCTUARY)   ) strcat( buf, "(White Aura) " );
      }
    if ( IS_AFFECTED(ch, AFF_SHADOWPLANE) 
     && !IS_AFFECTED(victim, AFF_SHADOWPLANE))    strcat( buf, "(Normal Plane) ");
    else if ( !IS_AFFECTED(ch, AFF_SHADOWPLANE) 
        && IS_AFFECTED(victim, AFF_SHADOWPLANE))  strcat( buf, "(Shadowplane) "     );
    
    if (ch->class > 0)
    {
       if (IS_CLASS(victim, CLASS_VAMPIRE))
                strcat(buf, "(Vampire) ");
       if (IS_CLASS(victim, CLASS_WEREWOLF))
		strcat(buf, "(Werewolf) ");
       if (IS_CLASS(victim, CLASS_MONK))
	        strcat(buf, "(Monk) ");
       if (IS_CLASS(victim, CLASS_DEMON))
	        strcat(buf, "(Daemon) ");
       if (IS_CLASS(victim, CLASS_DROW))
	        strcat(buf, "(Drow) ");
       if (IS_CLASS(victim, CLASS_ZERG))
		strcat(buf, "(Zerg) ");
       if (IS_CLASS(victim, CLASS_MAGE))
                strcat(buf, "(Warlock) ");
       if (IS_CLASS(victim, CLASS_DEMON) && IS_CLASS(ch, CLASS_DEMON))
       {
                strcat(buf, "(" );
		strcat(buf, victim->pcdata->switchname );
	        strcat(buf, ") " );
       }
       if (IS_VAMPAFF(ch, VAM_DISGUISED) && IS_CLASS(ch, CLASS_VAMPIRE))
       {
                strcat(buf, "(" );
		strcat(buf, victim->pcdata->switchname );
	        strcat(buf, ") " );
       }
       if (ch->class == victim->class && victim->cur_form != get_normal_form(victim) && !IS_CLASS(victim, CLASS_DEMON))
       {
		strcat(buf, "(" );
		strcat(buf, victim->pcdata->switchname );
 	        strcat(buf, ") " );
       }
    }

/*
    if (victim->cur_form == FRM_MIST && victim->position == POS_STANDING)
    	strcat(buf, "A flowing white mist is here, swirling around the room.");
    else if (victim->cur_form == FRM_MIST)
	strcat(buf, "A flowing white mist");
    else if (victim->cur_form == FRM_WOLF && victim->position == POS_STANDING)
    	strcat(buf, "A ferocious dire wolf is here snarling at you.");
    else if (victim->cur_form == FRM_WOLF)
	strcat(buf, "A ferocious dire wolf");
    else if (victim->cur_form == FRM_BAT && victim->position == POS_STANDING)
    	strcat(buf, "A vampire bat is flying around here.");
    else if (victim->cur_form == FRM_BAT)
	strcat(buf, "A vampire bat");
    else if (victim->cur_form == FRM_DOLPHIN && victim->position == POS_STANDING)
    	strcat(buf, "A beautiful dolphin is swimming around here.");
    else if (victim->cur_form == FRM_DOLPHIN)
	strcat(buf, "A beautiful dolphin");
    else if (victim->cur_form == FRM_FLAME && victim->position == POS_STANDING)
    	strcat(buf, "A roaring pillar of flame stands in the centre of the room.");
    else if (victim->cur_form == FRM_FLAME)
	strcat(buf, "A roaring pillar of flame");
    else if (victim->cur_form == FRM_SPIDER && victim->position == POS_STANDING)
    	strcat(buf, "A hairy little spider is creeping along here.");
    else if (victim->cur_form == FRM_SPIDER)
	strcat(buf, "A hairy little spider");
    else if (victim->cur_form == FRM_LIGHT && victim->position == POS_STANDING)
    	strcat(buf, "A ball of glowing light hangs in mid-air here.");
    else if (victim->cur_form == FRM_LIGHT)
	strcat(buf, "A ball of glowing light");
    else if (victim->cur_form == FRM_COCKROACH && victim->position == POS_STANDING)
    	strcat(buf, "A nasty little cockroach is here, waiting to be squished.");
    else if (victim->cur_form == FRM_COCKROACH)
	strcat(buf, "A nasty little cockroach");
    else if (victim->cur_form == FRM_RABBIT && victim->position == POS_STANDING)
    	strcat(buf, "A cute little bunny rabbit is sitting here.");
    else if (victim->cur_form == FRM_RABBIT)
	strcat(buf, "A cute little bunny rabbit");
    else if (victim->cur_form == FRM_BLOB && victim->position == POS_STANDING)
    	strcat(buf, "A slimy green blob is oozing in a corner.");
    else if (victim->cur_form == FRM_BLOB)
	strcat(buf, "A slimy green blob");
    else if (victim->cur_form == FRM_FROG && victim->position == POS_STANDING)
    	strcat(buf, "A slimy little frog is sitting on the floor.");
    else if (victim->cur_form == FRM_FROG)
	strcat(buf, "A slimy little frog");
    else if (victim->cur_form == FRM_WHIRLWIND && victim->position == POS_STANDING)
    	strcat(buf, "A roaring whirlwind is sweeping you around the room.");
    else if (victim->cur_form == FRM_WHIRLWIND)
	strcat(buf, "A roaring whirlwind");
    else if (victim->cur_form == FRM_CRIMSON && victim->position == POS_STANDING)
    	strcat(buf, "An eerie pinkish mist hangs suspended in the air.");
    else if (victim->cur_form == FRM_CRIMSON)
	strcat(buf, "An eerie pinkish mist");
    else if (victim->cur_form == FRM_HAWK && victim->position == POS_STANDING)
    	strcat(buf, "A hawk flies gracefully overhead.");
    else if (victim->cur_form == FRM_HAWK)
	strcat(buf, "A hawk");
*/
    if (victim->cur_form != 0)
    {
	if (victim->position == POS_STANDING)
	{
	    char buf2[MAX_STRING_LENGTH];
	    form_in_room_desc(victim, buf2);
	    strcat(buf, buf2);
	}
	else
	{
	    if (is_vowel(form_data[victim->cur_form].lookname[0]))
	        strcat(buf, "An ");
	    else strcat(buf, "A ");
	    strcat(buf, form_data[victim->cur_form].lookname);
	}
    }
    
    switch ( victim->position )
    {
    case POS_DEAD:     strcat( buf, " is lying dead on the floor." ); break;
    case POS_MORTAL:   strcat( buf, " is lying here, mortally wounded." ); break;
    case POS_INCAP:    strcat( buf, " is lying here incapacitated." ); break;
    case POS_STUNNED:  strcat( buf, " is lying here stunned." ); break;
    case POS_SLEEPING: strcat(buf," is sleeping here."); break;
    case POS_RESTING:  strcat( buf, " is resting here." ); break;
    case POS_SITTING:  strcat(buf, " is sitting here."); break;
    case POS_FIGHTING:
	strcat( buf, " is here, fighting " );
	if ( victim->fighting == NULL )
	    strcat( buf, "thin air??" );
	else if ( victim->fighting == ch )
	    strcat( buf, "YOU!" );
	else if ( victim->in_room == victim->fighting->in_room )
	{
	    strcat( buf, PERS( victim->fighting, ch ) );
	    strcat( buf, "." );
	}
	else
	    strcat( buf, "someone who left??" );
	break;
    }

    strcat( buf, "\n\r" );
    buf[0] = UPPER(buf[0]);
    send_to_char( buf, ch );
    return;
    }	


    if ( IS_HEAD(victim, LOST_HEAD) && IS_AFFECTED(victim, AFF_POLYMORPH))
	strcat( buf, "     " );
    else {
	if (!IS_NPC(victim) && victim->desc==NULL ) strcat( buf,"(Link-Dead) ");
    	if ( IS_AFFECTED(victim, AFF_INVISIBLE)   ) strcat( buf, "(Invis) "   );
    	if ( IS_AFFECTED(victim, AFF_HIDE)        ) strcat( buf, "(Hide) "    );
    	if ( IS_AFFECTED(victim, AFF_CHARM)       ) strcat( buf, "(Charmed) " );
    	if ( IS_AFFECTED(victim, AFF_PASS_DOOR)  ||
         IS_AFFECTED(victim, AFF_ETHEREAL)    )
	 strcat( buf, "(Translucent) ");
    	if ( IS_AFFECTED(victim, AFF_FAERIE_FIRE) ) strcat( buf, "(Pink Aura) "  );
    	if ( IS_EVIL(victim)
    	&&   IS_AFFECTED(ch, AFF_DETECT_EVIL)     ) strcat( buf, "(Red Aura) "   );
    	if ( IS_AFFECTED(victim, AFF_SANCTUARY)   ) strcat( buf, "(White Aura) " );
    }
    if ( IS_AFFECTED(ch, AFF_SHADOWPLANE) && !IS_AFFECTED(victim, AFF_SHADOWPLANE))
	strcat( buf, "(Normal plane) "     );
    else if ( !IS_AFFECTED(ch, AFF_SHADOWPLANE) && IS_AFFECTED(victim, AFF_SHADOWPLANE))
	strcat( buf, "(Shadowplane) "     );
    /* Vampires and werewolves can recognise each other - KaVir */
    if ( !IS_NPC(victim) && !IS_NPC(ch) && IS_CLASS(victim, CLASS_ZERG) && 
	IS_HERO(victim) && IS_HERO(ch) && 
	(IS_CLASS(ch, CLASS_ZERG) || IS_CLASS(ch, CLASS_WEREWOLF)
        || IS_CLASS(ch, CLASS_HIGHLANDER) || IS_CLASS(ch, CLASS_DEMON)
        || IS_SET(ch->special, SPC_CHAMPION) ||IS_CLASS(ch, CLASS_DROW)))
	strcat( buf, "(Zerg) " );
    else if ( !IS_NPC(victim) && !IS_NPC(ch) && IS_CLASS(victim,CLASS_WEREWOLF)
	&& IS_HERO(victim) && IS_HERO(ch) &&
	(IS_CLASS(ch, CLASS_VAMPIRE) || IS_CLASS(ch, CLASS_WEREWOLF)
	|| IS_CLASS(ch, CLASS_HIGHLANDER) || IS_CLASS( ch, CLASS_DEMON)
        || IS_SET(ch->special, SPC_CHAMPION) ||IS_CLASS(ch, CLASS_DROW)))
	strcat( buf, "(Werewolf) " );
    else if ( !IS_NPC(victim) && !IS_NPC(ch) && IS_CLASS( victim,CLASS_DEMON)
	&& IS_HERO(victim) && IS_HERO(ch) && IS_SET(victim->special, SPC_DEMON_LORD) 
	&& (IS_CLASS(ch, CLASS_VAMPIRE) || IS_CLASS(ch, CLASS_WEREWOLF)
	|| IS_CLASS(ch, CLASS_HIGHLANDER) || IS_CLASS(ch, CLASS_DEMON)
        || IS_SET(ch->special, SPC_CHAMPION) ||IS_CLASS(ch, CLASS_DROW)))
	strcat( buf, "(Demon Lord) " );
    else if ( !IS_NPC(victim) && !IS_NPC(ch) && IS_SET(victim->special,SPC_CHAMPION)
	&& IS_HERO(victim) && IS_HERO(ch) &&
	(IS_CLASS(ch, CLASS_VAMPIRE) || IS_CLASS(ch, CLASS_WEREWOLF)
	|| IS_CLASS(ch, CLASS_HIGHLANDER) || IS_CLASS( ch, CLASS_DEMON)
        || IS_SET(ch->special, SPC_CHAMPION) ||IS_CLASS(ch, CLASS_DROW)))
    {
	    strcat( buf, "(Demon) " );
    }
    else if ( !IS_NPC(victim) && !IS_NPC(ch) && IS_CLASS(victim, CLASS_HIGHLANDER)
	&& IS_HERO(victim) && IS_HERO(ch) &&
	(IS_CLASS(ch, CLASS_VAMPIRE) || IS_CLASS(ch, CLASS_WEREWOLF)
	|| IS_CLASS(ch, CLASS_HIGHLANDER) || IS_CLASS(ch, CLASS_DEMON)
        || IS_SET(ch->special, SPC_CHAMPION) ||IS_CLASS(ch, CLASS_DROW)))
	strcat( buf, "(Highlander) " );
    else if ( !IS_NPC(victim) && !IS_NPC(ch) && IS_CLASS(victim, CLASS_MAGE)
	&& IS_HERO(victim) && IS_HERO(ch) &&
	(IS_CLASS(ch, CLASS_VAMPIRE) || IS_CLASS(ch, CLASS_WEREWOLF)
	|| IS_CLASS(ch, CLASS_HIGHLANDER) || IS_CLASS( ch, CLASS_DEMON)
        || IS_SET(ch->special, SPC_CHAMPION) ||IS_CLASS(ch, CLASS_DROW)))
    {
	if (victim->level == LEVEL_APPRENTICE)    strcat( buf,"(Apprentice) " );
	else if (victim->level == LEVEL_MAGE)     strcat( buf,"(Mage) " );
	else if (victim->level == LEVEL_ARCHMAGE) strcat( buf,"(Archmage) " );
    }
    else if ( !IS_NPC(victim) && !IS_NPC(ch) && IS_CLASS(victim, CLASS_DROW) 
	&& IS_HERO(victim) && IS_HERO(ch) &&
	(IS_CLASS(ch, CLASS_VAMPIRE) || IS_CLASS(ch, CLASS_WEREWOLF)
	|| IS_CLASS(ch, CLASS_HIGHLANDER) || IS_CLASS(ch, CLASS_DEMON)
	|| IS_SET(ch->special, SPC_CHAMPION) ||IS_CLASS(ch, CLASS_MONK)))
		strcat(buf, "(Drow) ");
    if ( !IS_NPC(ch) && IS_VAMPAFF(ch,VAM_AUSPEX) && 
	 !IS_NPC(victim) && IS_CLASS(victim, CLASS_VAMPIRE) &&
	  IS_VAMPAFF(victim, VAM_DISGUISED) )
    {
	strcat( buf, "(");
	strcat( buf, victim->name);
	strcat( buf, ") ");
    }
if (victim->embracing!= NULL)
{
sprintf(buf7,"\n\r...%s is holding %s in a stern embrace!",victim->name,victim->embracing->name);
}
else if (victim->embraced!=NULL)
{
sprintf(buf7,"\n\r...%s is being embraced by %s!",victim->name,victim->embraced->name);
}
    if ( IS_EXTRA(victim, EXTRA_ROT))
     {
        if (IS_NPC(victim)) 
          sprintf(buf6,"\n\r...%s is coated with a layer of rotten skin!",victim->short_descr);
        else if (!IS_NPC(victim) && IS_AFFECTED(victim,AFF_POLYMORPH))
           sprintf(buf6,"\n\r...%s is coated with a layer of rotten skin!",victim->morph);
        else 
           sprintf(buf6,"\n\r...%s is coated with a layer of rotten skin!",victim->name);
       }
    if ( IS_AFFECTED(victim, AFF_FLAMING) ) 
    {
	if (IS_NPC(victim))
	    sprintf( buf2, "\n\r...%s is engulfed in blazing flames!", victim->short_descr );
	else if (!IS_NPC(victim) && IS_AFFECTED(victim,AFF_POLYMORPH))
	    sprintf( buf2, "\n\r...%s is engulfed in blazing flames!", victim->morph );
	else
	    sprintf( buf2, "\n\r...%s is engulfed in blazing flames!", victim->name );
    }

    if ( !IS_NPC(victim) && IS_HEAD(victim, LOST_HEAD) && IS_AFFECTED(victim, AFF_POLYMORPH)) 
    {
	if (IS_EXTRA(victim,GAGGED) && IS_EXTRA(victim,BLINDFOLDED))
	    sprintf( buf3, "...%s is gagged and blindfolded, kinky!", victim->morph );
	else if (IS_EXTRA(victim,GAGGED))
	    sprintf( buf3, "...%s is gagged!", victim->morph );
	else if (IS_EXTRA(victim,BLINDFOLDED))
	    sprintf( buf3, "...%s is blindfolded!", victim->morph );
    }

    if ( IS_HEAD(victim, LOST_HEAD) && IS_AFFECTED(victim, AFF_POLYMORPH))
    {
	strcat( buf, victim->morph );
	strcat( buf, " is lying here." );
    	strcat( buf, buf2 );
    	strcat( buf, buf3 );
    	strcat( buf, buf6 );
        strcat(buf,buf7);   
     strcat( buf, "\n\r" );
    	buf[5] = UPPER(buf[5]);
    	send_to_char( buf, ch );
	return;
    }
    
    if ( IS_EXTRA(victim, TIED_UP) ) 
    {
	if (IS_NPC(victim))
	    sprintf( buf3, "\n\r...%s is tied up", victim->short_descr );
	else if (!IS_NPC(victim) && IS_AFFECTED(victim,AFF_POLYMORPH))
	    sprintf( buf3, "\n\r...%s is tied up", victim->morph );
	else
	    sprintf( buf3, "\n\r...%s is tied up", victim->name );
	if (IS_EXTRA(victim,GAGGED) && IS_EXTRA(victim,BLINDFOLDED))
	    strcat( buf3, ", gagged and blindfolded!" );
	else if (IS_EXTRA(victim,GAGGED))
	    strcat( buf3, " and gagged!" );
	else if (IS_EXTRA(victim,BLINDFOLDED))
	    strcat( buf3, " and blindfolded!" );
	else
	    strcat( buf3, "!" );
    }

    if ( IS_AFFECTED(victim, AFF_WEBBED) ) 
    {
	if (IS_NPC(victim))
	    sprintf( buf4, "\n\r...%s is coated in a sticky web.", victim->short_descr );
	else if (!IS_NPC(victim) && IS_AFFECTED(victim,AFF_POLYMORPH))
	    sprintf( buf4, "\n\r...%s is coated in a sticky web.", victim->morph );
	else
	    sprintf( buf4, "\n\r...%s is coated in a sticky web.", victim->name );
	strcat( buf3, buf4 );
    }
        if ( IS_AFFECTED(victim, AFF_TENDRILS) ) 
    {
	if (IS_NPC(victim))
sprintf( buf4, "\n\r...%s is entrapped by tendrils of darkness.",
victim->short_descr );
	else if (!IS_NPC(victim) && IS_AFFECTED(victim,AFF_POLYMORPH))
sprintf( buf4, "\n\r...%s is entrapped by tendrils of darkness.",
victim->morph );
	else
sprintf( buf4, "\n\r...%s is entrapped by tendrils of darkness.",
victim->name );
	strcat( buf3, buf4 );
    }

    if ( !IS_NPC(victim) && IS_AFFECTED(victim, AFF_POLYMORPH) )
	strcat( buf, victim->morph );
    else if ( victim->position == POS_STANDING && victim->long_descr[0] != '\0' && (mount = victim->mount) == NULL )
    {
	strcat( buf, victim->long_descr );
	send_to_char( buf, ch );
	if ( IS_NPC(ch) || !IS_SET(ch->act, PLR_BRIEF) )
	{
	    if ( IS_ITEMAFF(victim, ITEMA_SHOCKSHIELD) ) 
		act( "...$N is surrounded by a crackling shield of lightning.", ch,NULL,victim,TO_CHAR );
	    if ( IS_ITEMAFF(victim, ITEMA_FIRESHIELD) ) 
		act( "...$N is surrounded by a burning shield of fire.", ch,NULL,victim,TO_CHAR );
	    if ( IS_ITEMAFF(victim, ITEMA_ICESHIELD) ) 
		act( "...$N is surrounded by a shimmering shield of ice.", ch,NULL,victim,TO_CHAR );
	    if ( IS_ITEMAFF(victim, ITEMA_ACIDSHIELD) ) 
		act( "...$N is surrounded by a bubbling shield of acid.", ch,NULL,victim,TO_CHAR );
	    if ( IS_ITEMAFF(victim, ITEMA_CHAOSSHIELD) ) 
		act( "...$N is surrounded by a swirling shield of chaos.", ch,NULL,victim,TO_CHAR );
	    if ( IS_ITEMAFF(victim, ITEMA_REFLECT) ) 
		act( "...$N is surrounded by a flickering shield of darkness.", ch,NULL,victim,TO_CHAR );
	}
	return;
    }
    else
    	strcat( buf, PERS( victim, ch ) );

    if ((mount = victim->mount) != NULL && victim->mounted == IS_RIDING)
    {
	if (IS_NPC(mount))
	    sprintf( mount2, " is here riding %s", mount->short_descr );
	else
	    sprintf( mount2, " is here riding %s", mount->name );
	strcat( buf, mount2 );
	if (victim->position == POS_FIGHTING)
	{
	    strcat( buf, ", fighting " );
	    if ( victim->fighting == NULL )
	        strcat( buf, "thin air??" );
	    else if ( victim->fighting == ch )
	    strcat( buf, "YOU!" );
	    else if ( victim->in_room == victim->fighting->in_room )
	    {
	        strcat( buf, PERS( victim->fighting, ch ) );
	        strcat( buf, "." );
	    }
	    else
	        strcat( buf, "somone who left??" );
	}
	else strcat( buf, "." );
    }
    else if ( victim->position == POS_STANDING && IS_AFFECTED(victim, AFF_FLYING) )
	strcat( buf, " is hovering here" );
    else if ( victim->position == POS_STANDING && (!IS_NPC(victim) && (IS_VAMPAFF(victim, VAM_FLYING))) )
	strcat( buf, " is hovering here" );
    else if (victim->position == POS_STANDING && (!IS_NPC(victim) &&
IS_CLASS(victim, CLASS_DROW) && IS_SET(victim->pcdata->powers[1], DPOWER_LEVITATION)))
	strcat(buf, " is hovering here" );
    else
    {
    	switch ( victim->position )
    	{
    	case POS_DEAD:     strcat( buf, " is DEAD!!" );              break;
        case POS_MORTAL:   strcat( buf, " is mortally wounded." );   break; 
    	case POS_INCAP:    strcat( buf, " is incapacitated." );      break;
    	case POS_STUNNED:  strcat( buf, " is lying here stunned." ); break;
    	case POS_SLEEPING: strcat( buf, " is sleeping here." );      break;
    	case POS_RESTING:  strcat( buf, " is resting here." );       break;
    	case POS_MEDITATING: strcat( buf, " is meditating here." );  break;
    	case POS_SITTING:  strcat( buf, " is sitting here." );       break;
    	case POS_STANDING:
	    if      (!IS_NPC(victim) && victim->stance[0] == STANCE_NORMAL)
		strcat( buf, " is here, crouched in a fighting stance." );
	    else if (!IS_NPC(victim) && victim->stance[0] == STANCE_VIPER)
		strcat( buf, " is here, crouched in a viper fighting stance." );
	    else if (!IS_NPC(victim) && victim->stance[0] == STANCE_CRANE)
		strcat( buf, " is here, crouched in a crane fighting stance." );
	    else if (!IS_NPC(victim) && victim->stance[0] == STANCE_CRAB)
		strcat( buf, " is here, crouched in a crab fighting stance." );
	    else if (!IS_NPC(victim) && victim->stance[0] == STANCE_MONGOOSE)
		strcat( buf, " is here, crouched in a mongoose fighting stance." );
	    else if (!IS_NPC(victim) && victim->stance[0] == STANCE_BULL)
		strcat( buf, " is here, crouched in a bull fighting stance." );
	    else if (!IS_NPC(victim) && victim->stance[0] == STANCE_MANTIS)
		strcat( buf, " is here, crouched in a mantis fighting stance." );
	    else if (!IS_NPC(victim) && victim->stance[0] == STANCE_DRAGON)
		strcat( buf, " is here, crouched in a dragon fighting stance." );
	    else if (!IS_NPC(victim) && victim->stance[0] == STANCE_TIGER)
		strcat( buf, " is here, crouched in a tiger fighting stance." );
	    else if (!IS_NPC(victim) && victim->stance[0] == STANCE_MONKEY)
		strcat( buf, " is here, crouched in a monkey fighting stance." );
	    else if (!IS_NPC(victim) && victim->stance[0] == STANCE_SWALLOW)
		strcat( buf, " is here, crouched in a swallow fighting stance." );
            else if (!IS_NPC(victim) && victim->stance[0] == STANCE_XKILLA)
                strcat( buf, " is here, crouched in Xkilla's fighting stance." );
	    else
		strcat( buf, " is here." );
	    break;
    	case POS_FIGHTING:
	    strcat( buf, " is here, fighting " );
	    if ( victim->fighting == NULL )
	        strcat( buf, "thin air??" );
	    else if ( victim->fighting == ch )
	    strcat( buf, "YOU!" );
	    else if ( victim->in_room == victim->fighting->in_room )
	    {
	        strcat( buf, PERS( victim->fighting, ch ) );
	        strcat( buf, "." );
	    }
	    else
	        strcat( buf, "somone who left??" );
	    break;
	}
    }

    strcat( buf, buf2 );
    strcat( buf, buf3 );
    strcat( buf, "\n\r" );
    buf[0] = UPPER(buf[0]);
    send_to_char( buf, ch );

    if ( !IS_NPC(ch) && IS_SET(ch->act, PLR_BRIEF) ) return;

    if ( IS_ITEMAFF(victim, ITEMA_SHOCKSHIELD) ) 
	act( "...$N is surrounded by a crackling shield of lightning.", ch,NULL,victim,TO_CHAR );
    if ( IS_ITEMAFF(victim, ITEMA_FIRESHIELD) ) 
	act( "...$N is surrounded by a burning shield of fire.", ch,NULL,victim,TO_CHAR );
    if ( IS_ITEMAFF(victim, ITEMA_ICESHIELD) ) 
	act( "...$N is surrounded by a shimmering shield of ice.", ch,NULL,victim,TO_CHAR );
    if ( IS_ITEMAFF(victim, ITEMA_ACIDSHIELD) ) 
	act( "...$N is surrounded by a bubbling shield of acid.", ch,NULL,victim,TO_CHAR );
    if ( IS_ITEMAFF(victim, ITEMA_CHAOSSHIELD) ) 
	act( "...$N is surrounded by a swirling shield of chaos.", ch,NULL,victim,TO_CHAR );
    if ( IS_ITEMAFF(victim, ITEMA_REFLECT) ) 
	act( "...$N is surrounded by a flickering shield of darkness.", ch,NULL,victim,TO_CHAR );
    return;
}



void evil_eye( CHAR_DATA *victim, CHAR_DATA *ch )
{
    char buf [MAX_STRING_LENGTH];
    int tabletype;

    if (IS_NPC(victim)) return;
    if (victim == ch) return;
    if (ch->level < 3 || victim->level < 3) return;
    if (victim->power[DISC_DAEM_DISC] < 2) return;
    if (victim->power[DISC_DAEM_DISC] < dice(1, 10)) return;
    if (!str_suffix(victim->poweraction,"gi") ||
	!str_suffix(victim->poweraction,"qui"))
	return;
    if (is_safe(ch, victim) ) return;
    if (victim->powertype != NULL && strlen(victim->powertype) > 1)
    {
	sprintf(buf,"\n\r%s\n\r",victim->powertype);
	send_to_char(buf,ch);
    }
    if (IS_SET(victim->spectype,EYE_SELFACTION) && victim->poweraction != NULL)
	interpret(victim,victim->poweraction);
    if (IS_SET(victim->spectype,EYE_ACTION) && victim->poweraction != NULL)
	interpret(ch,victim->poweraction);
    if (IS_SET(victim->spectype,EYE_SPELL) && victim->specpower > 0)
    {
	tabletype = skill_table[victim->specpower].target;
	(*skill_table[victim->specpower].spell_fun) (victim->specpower,victim->spl[tabletype],victim,ch);
    }
    return;
}


void show_char_to_char_1( CHAR_DATA *victim, CHAR_DATA *ch )
{
    char buf[MAX_STRING_LENGTH];
    OBJ_DATA *obj;
    int iWear;
    int percent;
    bool found;

    if ( can_see( victim, ch ) )
    {
	act( "$n looks at you.", ch, NULL, victim, TO_VICT    );
	act( "$n looks at $N.",  ch, NULL, victim, TO_NOTVICT );
    }

    if (!IS_NPC(ch) && IS_HEAD(victim,LOST_HEAD))
    {
	act( "$N is lying here.", ch, NULL, victim, TO_CHAR );
	return;
    }
    if ( victim->description[0] != '\0' )
    {
	send_to_char( victim->description, ch );
    }
    else
    {
	act( "You see nothing special about $M.", ch, NULL, victim, TO_CHAR );
    }

    if ( victim->max_hit > 0 )
	percent = ( 100 * victim->hit ) / victim->max_hit;
    else
	percent = -1;

    strcpy( buf, PERS(victim, ch) );

         if ( percent >= 100 ) strcat( buf, " is in perfect health.\n\r"  );
    else if ( percent >=  90 ) strcat( buf, " is slightly scratched.\n\r" );
    else if ( percent >=  80 ) strcat( buf, " has a few bruises.\n\r"     );
    else if ( percent >=  70 ) strcat( buf, " has some cuts.\n\r"         );
    else if ( percent >=  60 ) strcat( buf, " has several wounds.\n\r"    );
    else if ( percent >=  50 ) strcat( buf, " has many nasty wounds.\n\r" );
    else if ( percent >=  40 ) strcat( buf, " is bleeding freely.\n\r"    );
    else if ( percent >=  30 ) strcat( buf, " is covered in blood.\n\r"   );
    else if ( percent >=  20 ) strcat( buf, " is leaking guts.\n\r"       );
    else if ( percent >=  10 ) strcat( buf, " is almost dead.\n\r"        );
    else                       strcat( buf, " is DYING.\n\r"              );

    buf[0] = UPPER(buf[0]);
    send_to_char( buf, ch );
 
    if (!IS_NPC( victim ))
    {
    	if (IS_AFFECTED(victim, AFF_INFRARED) || IS_VAMPAFF(victim, VAM_NIGHTSIGHT)) act("$N's eyes are glowing bright red.",ch,NULL,victim,TO_CHAR);
    	if (IS_AFFECTED(victim, AFF_FLYING)) act("$N is hovering in the air.",ch,NULL,victim,TO_CHAR);
	if (!IS_NPC(victim) && IS_CLASS(victim, CLASS_DROW) &&
IS_SET(victim->pcdata->powers[1], DPOWER_LEVITATION))
	act("$N is hovering in the air.", ch,NULL,victim,TO_CHAR);
	if (!IS_NPC(victim) && IS_CLASS(victim, CLASS_DROW) &&
IS_SET(victim->pcdata->powers[1], DPOWER_ARMS))
	act("$N has 6 extra spidery arms protruding from $S body.",ch,NULL,victim,TO_CHAR);
    	if (IS_VAMPAFF(victim, VAM_FANGS)) act("$N has a pair of long, pointed fangs.",ch,NULL,victim,TO_CHAR);
    	if (!IS_NPC(victim) && IS_CLASS(victim, CLASS_VAMPIRE) && IS_VAMPAFF(victim, VAM_CLAWS))
		act("$N has razor sharp claws protruding from under $S finger nails.",ch,NULL,victim,TO_CHAR);
	else if (IS_CLASS(victim, CLASS_NINJA) && IS_VAMPAFF(victim, VAM_CLAWS))
	act("$N has IronClaws protruding from $S wrists.",ch,NULL,victim,TO_CHAR);
    	else if (!IS_NPC(victim) && IS_VAMPAFF(victim, VAM_CLAWS))
		act("$N has razor sharp talons extending from $S fingers.",ch,NULL,victim,TO_CHAR);
    	if (!IS_NPC(victim))
    	{
	if (!IS_CLASS(ch, CLASS_VAMPIRE) && IS_SET(ch->newbits, NEW_SKIN))
act("$N has scales all over $S body.",ch,NULL,victim,TO_CHAR);
		if (IS_VAMPAFF(victim, VAM_HORNS)) act("$N has a pair of pointed horns extending from $S head.",ch,NULL,victim,TO_CHAR);
		if (IS_VAMPAFF(victim, VAM_WINGS))
		{
	    		if (IS_DEMAFF(victim, DEM_UNFOLDED))
				act("$N has a pair of batlike wings spread out from behind $S back.",ch,NULL,victim,TO_CHAR);
    		else
 		act("$N has a pair of batlike wings folded behind $S back.",ch,NULL,victim,TO_CHAR);
		}
	if (IS_VAMPAFF(victim, VAM_HEAD))
	 {
	   act("$N's head resembles that of a fierce lion.",ch,NULL,victim,TO_CHAR);
	 }
   if (IS_VAMPAFF(victim, VAM_TAIL))
	act("$N has a sharp tail extending from $S spine.",ch,NULL,victim,TO_CHAR); 
   if (IS_VAMPAFF(victim, VAM_EXOSKELETON))
	act("$N's skin is covered by a hard exoskeleton.",ch,NULL,victim,TO_CHAR);
 
  	}
    }
    found = FALSE;
    for ( iWear = 0; iWear < MAX_WEAR; iWear++ )
    {
	if ( ( obj = get_eq_char( victim, iWear ) ) != NULL
	&&   can_see_obj( ch, obj ) )
	{
	    if ( !found )
	    {
		send_to_char( "\n\r", ch );
		act( "$N is using:", ch, NULL, victim, TO_CHAR );
		found = TRUE;
	    }
	    send_to_char( where_name[iWear], ch );
	    if (IS_NPC(ch) || ch->pcdata->chobj == NULL || ch->pcdata->chobj != obj)
	    {
	    	send_to_char( format_obj_to_char( obj, ch, TRUE ), ch );
	    	send_to_char( "\n\r", ch );
	    }
	    else
		send_to_char( "you\n\r", ch);
	}
    }

    if ( victim != ch
    &&   !IS_NPC(ch)
    &&   !IS_HEAD(victim, LOST_HEAD)
    &&   number_percent( ) < ch->pcdata->learned[gsn_peek] )
    {
	send_to_char( "\n\rYou peek at the inventory:\n\r", ch );
	show_list_to_char( victim->carrying, ch, TRUE, TRUE );
    }

    return;
}



void show_char_to_char( CHAR_DATA *list, CHAR_DATA *ch )
{
    CHAR_DATA *rch;

    for ( rch = list; rch != NULL; rch = rch->next_in_room )
    {
	if ( rch == ch )
	    continue;

	if ( IS_IMMORTAL(ch) || IS_IMMORTAL(rch) )
	{
	    if ( !IS_NPC(rch)
	    &&   IS_SET(rch->act, PLR_WIZINVIS)
	    &&   get_trust( ch ) < get_trust( rch ) )
		continue;
	}
	else
	{
	    if ( !IS_NPC(rch)
	    && ( IS_SET(rch->act, PLR_WIZINVIS)
	    ||   IS_ITEMAFF(rch, ITEMA_VANISH) )
	    &&   !IS_SET(ch->act, PLR_HOLYLIGHT)
	    &&   !IS_ITEMAFF(ch, ITEMA_VISION) )
		continue;
	}

	if ( !IS_NPC(rch) && IS_HEAD(rch,LOST_HEAD))
	    continue;

	if ( !IS_NPC(rch) && IS_EXTRA(rch,EXTRA_OSWITCH))
	    continue;

	if ( can_see( ch, rch ) )
	    show_char_to_char_0( rch, ch );
	else if ( room_is_dark( ch->in_room )
	&&      ( IS_AFFECTED(rch, AFF_INFRARED)||(!IS_NPC(rch)&&IS_VAMPAFF(rch, VAM_NIGHTSIGHT))))
	{
	    send_to_char( "You see glowing red eyes watching YOU!\n\r", ch );
	}
    }

    return;
} 



bool check_blind( CHAR_DATA *ch )
{

    if ( IS_EXTRA(ch, BLINDFOLDED) )
    {
        send_to_char( "You can't see a thing through the blindfold!\n\r", ch );
        return FALSE;
    }     

    if ( !IS_NPC(ch) && IS_SET(ch->act, PLR_HOLYLIGHT) )
	return TRUE;

    if ( IS_ITEMAFF(ch, ITEMA_VISION) )
	return TRUE;

    if ( IS_HEAD(ch, LOST_EYE_L) && IS_HEAD(ch, LOST_EYE_R) )
    {
	send_to_char( "You have no eyes!\n\r", ch );
	return FALSE;
    }

    if ( IS_AFFECTED(ch, AFF_BLIND) && !IS_AFFECTED(ch, AFF_SHADOWSIGHT))
    {
	send_to_char( "You can't see a thing!\n\r", ch );
	return FALSE;
    }

    return TRUE;
}



void do_look( CHAR_DATA *ch, char *argument )
{
    char buf  [MAX_STRING_LENGTH];
    char arg1 [MAX_INPUT_LENGTH];
    char arg2 [MAX_INPUT_LENGTH];
    EXIT_DATA *pexit;
    CHAR_DATA *victim;
    CHAR_DATA *vch;
    CHAR_DATA *vch_next;
    CHAR_DATA *wizard;
    OBJ_DATA *obj;
    OBJ_DATA *portal;
    OBJ_DATA *portal_next;
    ROOM_INDEX_DATA *pRoomIndex;
    ROOM_INDEX_DATA *location;
    char *pdesc;
    int door;
    bool found;

    if ( ch->desc == NULL && (wizard = ch->wizard) == NULL) return;

    if (ch->in_room == NULL) return;

    if ( ch->position < POS_SLEEPING )
    {
	send_to_char( "You can't see anything but stars!\n\r", ch );
	return;
    }

    if ( ch->position == POS_SLEEPING )
    {
	send_to_char( "You can't see anything, you're sleeping!\n\r", ch );
	return;
    }

    if ( IS_SET(ch->flag2,AFF_TOTALBLIND))
    {
    send_to_char("You can't see anything because you're blinded!\n\r",ch);
    return;
    }

    if ( !check_blind( ch ) )
	return;

    if ( !IS_NPC(ch) && IS_SET(ch->in_room->room_flags, ROOM_TOTAL_DARKNESS) &&
	!IS_ITEMAFF(ch, ITEMA_VISION) && !IS_IMMORTAL(ch) &&
!(IS_CLASS(ch, CLASS_DROW)))
    {
	send_to_char( "It is pitch black ... \n\r", ch );
	return;
    }

    if ( !IS_NPC(ch)
    &&   !IS_SET(ch->act, PLR_HOLYLIGHT)
    &&   !IS_ITEMAFF(ch, ITEMA_VISION)
    &&   !IS_VAMPAFF(ch, VAM_NIGHTSIGHT)
    &&   !IS_AFFECTED(ch, AFF_SHADOWPLANE)
    &&   !(ch->in_room != NULL && ch->in_room->vnum == ROOM_VNUM_IN_OBJECT
    &&   !IS_NPC(ch) && ch->pcdata->chobj != NULL
    &&   ch->pcdata->chobj->in_obj != NULL)
    &&   room_is_dark( ch->in_room ) )
    {
	send_to_char( "It is pitch black ... \n\r", ch );
	show_char_to_char( ch->in_room->people, ch );
	return;
    }

    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );

    if ( arg1[0] == '\0' || !str_cmp( arg1, "auto" ) )
    {
	/* 'look' or 'look auto' */
	if (ch->in_room != NULL && ch->in_room->vnum == ROOM_VNUM_IN_OBJECT
	&& !IS_NPC(ch) && ch->pcdata->chobj != NULL && ch->pcdata->chobj->in_obj != NULL)
	    act( "$p",ch,ch->pcdata->chobj->in_obj,NULL,TO_CHAR);
    	else if ( IS_AFFECTED(ch, AFF_SHADOWPLANE) )
	    send_to_char( "The shadow plane\n\r", ch );
	else
	{
	    sprintf(buf, "%s\n\r", ch->in_room->name);
	    send_to_char( buf, ch );
	}

	if ( !IS_NPC(ch) && IS_SET(ch->act, PLR_AUTOEXIT) )
	    do_exits( ch, "auto" );

	if (ch->in_room != NULL && ch->in_room->vnum == ROOM_VNUM_IN_OBJECT
	&& !IS_NPC(ch) && ch->pcdata->chobj != NULL && ch->pcdata->chobj->in_obj != NULL)
	{
	    act( "You are inside $p.",ch,ch->pcdata->chobj->in_obj,NULL,TO_CHAR);
	    show_list_to_char( ch->pcdata->chobj->in_obj->contains, ch, FALSE, FALSE );
	}
	else if ( (arg1[0] == '\0' || !str_cmp( arg1, "auto" ) ) && IS_AFFECTED(ch, AFF_SHADOWPLANE) )
	    send_to_char( "You are standing in complete darkness.\n\r", ch );
	else if ( ( !IS_NPC(ch) && !IS_SET(ch->act, PLR_BRIEF) ) &&
		( arg1[0] == '\0' || !str_cmp( arg1, "auto" ) ) )
	{
	    send_to_char( ch->in_room->description, ch );
	    if (ch->in_room->blood == 1000)
		sprintf(buf,"You notice that the room is completely drenched in blood.\n\r");
	    else if (ch->in_room->blood > 750)
		sprintf(buf,"You notice that there is a very large amount of blood around the room.\n\r");
	    else if (ch->in_room->blood > 500)
		sprintf(buf,"You notice that there is a large quantity of blood around the room.\n\r");
	    else if (ch->in_room->blood > 250)
		sprintf(buf,"You notice a fair amount of blood on the floor.\n\r");
	    else if (ch->in_room->blood > 100)
		sprintf(buf,"You notice several blood stains on the floor.\n\r");
	    else if (ch->in_room->blood > 50)
		sprintf(buf,"You notice a few blood stains on the floor.\n\r");
	    else if (ch->in_room->blood > 25)
		sprintf(buf,"You notice a couple of blood stains on the floor.\n\r");
	    else if (ch->in_room->blood > 0)
		sprintf(buf,"You notice a few drops of blood on the floor.\n\r");
	    else sprintf(buf,"You notice nothing special in the room.\n\r");
	    ADD_COLOUR(ch, buf, L_RED);
	    if (ch->in_room->blood > 0) send_to_char(buf,ch);
	}
            if (IS_SET(ch->in_room->room_flags, ROOM_FLAMING))
            send_to_char("..This room is engulfed in flames!\n\r",ch);

	show_list_to_char( ch->in_room->contents, ch, FALSE, FALSE );

        for (door=0 ; door < 6 ; door++)
	{	
	    if (ch->in_room == NULL) continue;
	    if (ch->in_room->exit[door] == NULL) continue;

	    if (IS_SET(ch->in_room->exit[door]->exit_info, EX_ICE_WALL))
	    {
		sprintf(buf, "     You see a glacier of ice %s.\n\r", exitname2[door]);
	        send_to_char(buf, ch);
	    }
	    else if (IS_SET(ch->in_room->exit[door]->exit_info,EX_CALTROP_WALL))
	    {
		sprintf(buf, "     You see a wall of caltrops %s.\n\r", exitname2[door]);
	        send_to_char(buf, ch);
	    }
	    else if (IS_SET(ch->in_room->exit[door]->exit_info,EX_FIRE_WALL))
	    {
		sprintf(buf, "     You see a blazing wall of fire %s.\n\r", exitname2[door]);
	        send_to_char(buf, ch);
	    }
	    else if (IS_SET(ch->in_room->exit[door]->exit_info,EX_SWORD_WALL))
	    {
		sprintf(buf, "     You see a spinning wall of swords %s.\n\r", exitname2[door]);
	        send_to_char(buf, ch);
	    }
	    else if (IS_SET(ch->in_room->exit[door]->exit_info,EX_MUSHROOM_WALL))
	    {
		sprintf(buf, "     You see a vibrating mound of mushrooms %s.\n\r", exitname2[door]);
	        send_to_char(buf, ch);
	    }
	    else if (IS_SET(ch->in_room->exit[door]->exit_info,EX_PRISMATIC_WALL)) 	    
	    {
		sprintf(buf, "     You see a shimmering wall of many colours %s.\n\r", exitname2[door]);
	        send_to_char(buf, ch);
	    }
	    else if (IS_SET(ch->in_room->exit[door]->exit_info, EX_IRON_WALL))
	    {
		sprintf(buf, "    You see a solid wall of iron %s.\n\r", exitname2[door]);
	        send_to_char(buf, ch);
	    }
	    else if (IS_SET(ch->in_room->exit[door]->exit_info, EX_ASH_WALL))
	    {
		sprintf(buf, "    You see a deadly wall of ash %s.\n\r", exitname2[door]);
	        send_to_char(buf, ch);
	    }
	}
	show_char_to_char( ch->in_room->people,   ch );
	return;
    }

    if ( !str_cmp( arg1, "i" ) || !str_cmp( arg1, "in" ) )
    {
	/* 'look in' */
	if ( arg2[0] == '\0' )
	{
	    send_to_char( "Look in what?\n\r", ch );
	    return;
	}

	if ( ( obj = get_obj_here( ch, arg2 ) ) == NULL )
	{
	    send_to_char( "You do not see that here.\n\r", ch );
	    return;
	}

	switch ( obj->item_type )
	{
	default:
	    send_to_char( "That is not a container.\n\r", ch );
	    break;

	case ITEM_PORTAL:
	    pRoomIndex = get_room_index(obj->value[0]);
	    location = ch->in_room;
	    if ( pRoomIndex == NULL )
	    {
		send_to_char( "It doesn't seem to lead anywhere.\n\r", ch );
		return;
	    }
	    if (obj->value[2] == 1 || obj->value[2] == 3)
	    {
		send_to_char( "It seems to be closed.\n\r", ch );
		return;
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
		    if (IS_AFFECTED(ch, AFF_SHADOWPLANE) &&
			!IS_SET(portal->extra_flags, ITEM_SHADOWPLANE) )
		    {
			REMOVE_BIT(ch->affected_by, AFF_SHADOWPLANE);
	    		do_look(ch,"auto");
			SET_BIT(ch->affected_by, AFF_SHADOWPLANE);
			break;
		    }
		    else if (!IS_AFFECTED(ch, AFF_SHADOWPLANE) &&
			IS_SET(portal->extra_flags, ITEM_SHADOWPLANE) )
		    {
			SET_BIT(ch->affected_by, AFF_SHADOWPLANE);
	    		do_look(ch,"auto");
			REMOVE_BIT(ch->affected_by, AFF_SHADOWPLANE);
			break;
		    }
		    else
		    {
	    		do_look(ch,"auto");
			break;
		    }
	    	}
	    }
	    char_from_room(ch);
	    char_to_room(ch,location);
	    break;

	case ITEM_DRINK_CON:
	    if ( obj->value[1] <= 0 )
	    {
		send_to_char( "It is empty.\n\r", ch );
		break;
	    }

	    if (obj->value[1] < obj->value[0] / 5)
	    	sprintf( buf, "There is a little %s liquid left in it.\n\r",liq_table[obj->value[2]].liq_color);
	    else if (obj->value[1] < obj->value[0] / 4)
	    	sprintf( buf, "It contains a small about of %s liquid.\n\r",liq_table[obj->value[2]].liq_color);
	    else if (obj->value[1] < obj->value[0] / 3)
	    	sprintf( buf, "It's about a third full of %s liquid.\n\r",liq_table[obj->value[2]].liq_color);
	    else if (obj->value[1] < obj->value[0] / 2)
	    	sprintf( buf, "It's about half full of %s liquid.\n\r",liq_table[obj->value[2]].liq_color);
	    else if (obj->value[1] < obj->value[0])
	    	sprintf( buf, "It is almost full of %s liquid.\n\r",liq_table[obj->value[2]].liq_color);
	    else if (obj->value[1] == obj->value[0])
	    	sprintf( buf, "It's completely full of %s liquid.\n\r",liq_table[obj->value[2]].liq_color);
	    else
	    	sprintf( buf, "Somehow it is MORE than full of %s liquid.\n\r",liq_table[obj->value[2]].liq_color);
	    send_to_char( buf, ch );
	    break;

	case ITEM_CONTAINER:
	case ITEM_CORPSE_NPC:
	case ITEM_CORPSE_PC:
	    if ( IS_SET(obj->value[1], CONT_CLOSED) )
	    {
		send_to_char( "It is closed.\n\r", ch );
		break;
	    }

	    act( "$p contains:", ch, obj, NULL, TO_CHAR );
	    show_list_to_char( obj->contains, ch, TRUE, TRUE );
	    break;
	}
	return;
    }

    if ( ( victim = get_char_room( ch, arg1 ) ) != NULL )
    {
	show_char_to_char_1( victim, ch );
	evil_eye(victim,ch);
	return;
    }

    for ( vch = char_list; vch != NULL; vch = vch_next )
    {
	vch_next	= vch->next;
	if ( vch->in_room == NULL )
	    continue;
	if ( vch->in_room == ch->in_room )
	{
	    if (!IS_NPC(vch) && !str_cmp(arg1,vch->morph))
	    {
		show_char_to_char_1( vch, ch );
		evil_eye(vch,ch);
		return;
	    }
	    continue;
	}
    }

    if (!IS_NPC(ch) && ch->pcdata->chobj != NULL && ch->pcdata->chobj->in_obj != NULL)
    {
	obj = get_obj_in_obj(ch,arg1);
	if (obj != NULL)
	{
	    send_to_char( obj->description, ch );
	    send_to_char( "\n\r", ch );
	    return;
	}
    }

    for ( obj = ch->carrying; obj != NULL; obj = obj->next_content )
    {
	if (!IS_NPC(ch) && ch->pcdata->chobj != NULL && obj->chobj != NULL && obj->chobj == ch)
	    continue;
	if ( can_see_obj( ch, obj ) )
	{
	    pdesc = get_extra_descr( arg1, obj->extra_descr );
	    if ( pdesc != NULL )
	    {
		send_to_char( pdesc, ch );
		return;
	    }

	    pdesc = get_extra_descr( arg1, obj->pIndexData->extra_descr );
	    if ( pdesc != NULL )
	    {
		send_to_char( pdesc, ch );
		return;
	    }
	}

	if ( is_name( arg1, obj->name ) )
	{
	    send_to_char( obj->description, ch );
	    send_to_char( "\n\r", ch );
	    return;
	}
    }

    for ( obj = ch->in_room->contents; obj != NULL; obj = obj->next_content )
    {
	if (!IS_NPC(ch) && ch->pcdata->chobj != NULL && obj->chobj != NULL && obj->chobj == ch)
	    continue;
	if ( can_see_obj( ch, obj ) )
	{
	    pdesc = get_extra_descr( arg1, obj->extra_descr );
	    if ( pdesc != NULL )
	    {
		send_to_char( pdesc, ch );
		return;
	    }

	    pdesc = get_extra_descr( arg1, obj->pIndexData->extra_descr );
	    if ( pdesc != NULL )
	    {
		send_to_char( pdesc, ch );
		return;
	    }
	}

	if ( is_name( arg1, obj->name ) )
	{
	    send_to_char( obj->description, ch );
	    send_to_char( "\n\r", ch );
	    return;
	}
    }

    pdesc = get_extra_descr( arg1, ch->in_room->extra_descr );
    if ( pdesc != NULL )
    {
	send_to_char( pdesc, ch );
	return;
    }

         if ( !str_cmp( arg1, "n" ) || !str_cmp( arg1, "north" ) ) door = 0;
    else if ( !str_cmp( arg1, "e" ) || !str_cmp( arg1, "east"  ) ) door = 1;
    else if ( !str_cmp( arg1, "s" ) || !str_cmp( arg1, "south" ) ) door = 2;
    else if ( !str_cmp( arg1, "w" ) || !str_cmp( arg1, "west"  ) ) door = 3;
    else if ( !str_cmp( arg1, "u" ) || !str_cmp( arg1, "up"    ) ) door = 4;
    else if ( !str_cmp( arg1, "d" ) || !str_cmp( arg1, "down"  ) ) door = 5;
    else
    {
	send_to_char( "You do not see that here.\n\r", ch );
	return;
    }

    /* 'look direction' */
    if ( ( pexit = ch->in_room->exit[door] ) == NULL )
    {
	send_to_char( "Nothing special there.\n\r", ch );
	return;
    }
/*
    if ( pexit->description != NULL && pexit->description[0] != '\0' )
	send_to_char( pexit->description, ch );
    else
	send_to_char( "Nothing special there.\n\r", ch );
*/

    if ( pexit->keyword    != NULL
    &&   pexit->keyword[0] != '\0'
    &&   pexit->keyword[0] != ' ' )
    {
	if ( IS_SET(pexit->exit_info, EX_CLOSED) )
	{
	    act( "The $d is closed.", ch, NULL, pexit->keyword, TO_CHAR );
	}
	else if ( IS_SET(pexit->exit_info, EX_ISDOOR) )
	{
	    act( "The $d is open.",   ch, NULL, pexit->keyword, TO_CHAR );
	    if ((pexit = ch->in_room->exit[door]) == NULL) return;
	    if ((pRoomIndex = pexit->to_room) == NULL) return;
	    location = ch->in_room;
	    char_from_room(ch);
	    char_to_room(ch,pRoomIndex);
	    do_look(ch,"auto");
	    char_from_room(ch);
	    char_to_room(ch,location);
	}
	else
	{
	    if ((pexit = ch->in_room->exit[door]) == NULL) return;
	    if ((pRoomIndex = pexit->to_room) == NULL) return;
	    location = ch->in_room;
	    char_from_room(ch);
	    char_to_room(ch,pRoomIndex);
	    do_look(ch,"auto");
	    char_from_room(ch);
	    char_to_room(ch,location);
	}
    }
    else
    {
	if ((pexit = ch->in_room->exit[door]) == NULL) return;
	if ((pRoomIndex = pexit->to_room) == NULL) return;
	location = ch->in_room;
	char_from_room(ch);
	char_to_room(ch,pRoomIndex);
	do_look(ch,"auto");
	char_from_room(ch);
	char_to_room(ch,location);
    }

    return;
}



void do_examine( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_STRING_LENGTH];
    char arg[MAX_INPUT_LENGTH];
    OBJ_DATA *obj;

    one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
	send_to_char( "Examine what?\n\r", ch );
	return;
    }

    do_look( ch, arg );

    if ( ( obj = get_obj_here( ch, arg ) ) != NULL )
    {
	     if (obj->condition >= 100) 
	    sprintf( buf, "You notice that %s is in perfect condition.\n\r",obj->short_descr );
	else if (obj->condition >= 75 )
	    sprintf( buf, "You notice that %s is in good condition.\n\r",obj->short_descr );
	else if (obj->condition >= 50 )
	    sprintf( buf, "You notice that %s is in average condition.\n\r",obj->short_descr );
	else if (obj->condition >= 25 )
	    sprintf( buf, "You notice that %s is in poor condition.\n\r",obj->short_descr );
	else
	    sprintf( buf, "You notice that %s is in awful condition.\n\r",obj->short_descr );
	send_to_char(buf,ch);
	switch ( obj->item_type )
	{
	default:
	    break;

	case ITEM_DRINK_CON:
	case ITEM_CONTAINER:
	case ITEM_CORPSE_NPC:
	case ITEM_CORPSE_PC:
	    send_to_char( "When you look inside, you see:\n\r", ch );
	    sprintf( buf, "in %s", arg );
	    do_look( ch, buf );
	}
    }

    return;
}



/*
 * Thanks to Zrin for auto-exit part.
 */
void do_exits( CHAR_DATA *ch, char *argument )
{
    extern char * const dir_name[];
    char buf[MAX_STRING_LENGTH];
    EXIT_DATA *pexit;
    bool found;
    bool fAuto;
    int door;

    buf[0] = '\0';
    fAuto  = !str_cmp( argument, "auto" );

    if ( !check_blind( ch ) )
	return;

    strcpy( buf, fAuto ? "[Exits:" : "Obvious exits:\n\r" );

    found = FALSE;
    for ( door = 0; door <= 5; door++ )
    {
	if ( ( pexit = ch->in_room->exit[door] ) != NULL
	&&   pexit->to_room != NULL
	&&   !IS_SET(pexit->exit_info, EX_CLOSED) )
	{
	    found = TRUE;
	    if ( fAuto )
	    {
		strcat( buf, " " );
		strcat( buf, dir_name[door] );
	    }
	    else
	    {
		sprintf( buf + strlen(buf), "%-5s - %s\n\r",
		    capitalize( dir_name[door] ),
		    room_is_dark( pexit->to_room )
			?  "Too dark to tell"
			: pexit->to_room->name
		    );
	    }
	}
    }

    if ( !found )
	strcat( buf, fAuto ? " none" : "None.\n\r" );

    if ( fAuto )
	strcat( buf, "]\n\r" );

    send_to_char( buf, ch );
    return;
}



const char * transformation_message[2][12] =
{
{
"\n\rYour body shudders and convulses as your form transforms. Arms and legs grow out of
your slimy torso, your skin turns a sickly veined green.\n\r" ,
"\n\rYour demonic form is reduced to a pool of pulsating jelly, the fires of hell
heat and char your semi-liquid form until you stiffen.  Limbs grow and
spiny talons extend from your arms and back.\n\r" ,
"\n\rYou start the transformation.  Your gargoylish apperance is changed as you grow
another two feet in height.  Scales, dripping with acid, begin to cover your skin.\n\r" ,
"\n\rThe fires of hell surround you once more.  Sharp claws start to grow from
your hands, and you grow a viscious barbed tail.  Small flames play over your
burnt and blackened skin.\n\r" ,
"\n\rYour next form is that of a sickly, gaunt skeleton.  Your leathery flesh
held tight over a demonic frame.  A huge tail, with a poisonous barb on the end
protrudes out of the base of your spine.\n\r" ,
"\n\rAs you transform, you grow another four feet in height. This time, wicked
barbs cover your entire body.  You find you have the ability to wield weapons, and use armor.\n\r" ,
"\n\rWhat?  What is happening, with the transformation into an Erinyes, your
form is no longer repulsive and demonic, but you are now a beautiful
mortal, with long auburn hair and huge white wings growing out of your
back.  You can see the world again with your mortal eyes, but the 
passions of a demon will always stay.\n\r",
"\n\rYour skin melts and you transform once more, but this time you become a gruesome
pug nosed, vile dwarf-like creature with large leathery wings and a mouth
filled of razor sharp teeth, an Amnizu.\n\r" ,
"\n\rThe intense heat of the fires of Baator surround you. Your skin is
wrenched apart and you transform into a grotesque 9-foot tall monstrosity
with huge wings, a snaking, prehensile tail, and a long barbed
whip.  A true demon, a Cornugon.\n\r",
"\n\rThe power of Baator now fills your very soul.  You grow again
as the chill winds of Caina tear the flesh from your bones.  You look to see what
you have become, you look alien, with a twelve foot long icy insect-like
body, your head bulging with multi-faceted eyes and you have
a long tail covered with razor-sharp spikes.\n\r",
"\n\rAaargh!  You are cast into the pit of flame. The fires, more intense
than ever before sear and scar your flesh as it bubbles and boils.
You have become the most terrible demon, the Pit Fiend, a huge bat winged 
humanoid, twelve feet tall, with large fangs that drip with a vile green liquid
and a hulking red scaly body that bursts into flame when you are angered or
excited.\n\r",
"\n\rYour fiery skin blackens as you are infused with the embodiment of
ultimate evil.  You are the most powerful demon, the lord of all.  You
are everything, nothing can oppose you.  Twenty foot tall, your torso
drips with the vile acid of the Abyss.  A ravenous hunger is gnawing at your
bones.  Welcome to Immortality..."
	},
	{
"Your first transformation is quite remarkable, you grow to over seven feet tall,
and your skin starts to dull and lose all signs of vitality.  Your teeth begin to
extend.  The world starts to look different, you have left mortality behind.\n\r",
"You start to grow, the base of your spine slowly extends into a short stump of
a tail.  The metamorphosis is tearing you apart.  Your skin begin to harden, and
your hands twist into claws.\n\r",
"Once again the painful metamorphosis strikes.  Your bones rend through your
scaled flesh as jagged spines shoot through your back.  You feel as if you are
being wrenched apart.  You are now over ten feet tall, and your skin is covered
with thick green scales.\n\r",
"You transform once again.  This time a small pair of wings burst through your
back.  Your hands and feet have become viscious claws, and your nose has extended
into a snout.  Long jagged teeth begin to grow in your mouth, and your tail now
reaches the floor.\n\r",
"You fall to the floor in agony.  Clutching your chest as the transformation
strikes.  You grow to over fifteen feet in length, towering above all the
mortals that you have left behind.  Impenetrable scales now cover your body, 
and smoke issues from your nostrils when you breathe.\n\r",
"You grow to over twenty feet in length, your tail and claws grow at an alarming
rate.  Your wings are now so powerful that you can fly.  Nobody can oppose you
now.  Suddenly it hits you, a hunger so intense that you cannot ignore it.
You must feed.\n\r",
"",
"",
"",
"",
"You have reached the pinnacle of power for a dragon.  As your body undergoes
the by now familiar transformation you grow to over fifty feet tall, with
incredibly sharp claws and fangs, a powerful tail and a fiery breath weapon.
Nobody can oppose you now.  You are the lord of the realm.\n\r",
""
	}
};


void do_promote (CHAR_DATA *ch, char *argument)
{
    char arg[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;
    bool override = FALSE;

    if (IS_NPC(ch)) return;

    argument = one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
	send_to_char( "Who do you wish to promote?\n\r", ch );
	return;
    }

    if (argument[0] != 0 && !str_prefix(argument, "override") && IS_CREATOR(ch))
    {
	stc("Manual override granted.\n\r", ch);
	override = TRUE;
    }

    if (!IS_CLASS(ch, CLASS_DEMON) && !override) {
	send_to_char("Only demons can promote.\n\r", ch );
	return;}

    if ( ( victim = get_char_room( ch, arg ) ) == NULL )
    {
	send_to_char( "They aren't here.\n\r", ch );
	return;
    }

    if (IS_NPC(victim))
    {
	send_to_char("You cannot promote a NPC.\n\r", ch);
	return;
    }

/*
    if ((IS_CLASS(ch, CLASS_DEMON)
	&& ch->generation == 10
	&& IS_CLASS(victim, CLASS_DEMON)
	&& victim->generation > 4))
    {
	send_to_char("Cornugons may only promote demons of Barbazu rank and lower.\n\r", ch);
	return;
    }

    if (IS_CLASS(victim, CLASS_DEMON)
	&& victim->generation == 10
	&& victim->power[DISC_DAEM_GELU] < 10)
    {
	send_to_char("Only a demon with level 10 Geluge may become a Gelugon.\n\r", ch);
	return;
    }
*/

    if ((IS_CLASS(victim, CLASS_DEMON)
	&& victim->pcdata->souls < souls_needed(victim) ) && !override)
    {
	send_to_char("They have not collected enough souls to be promoted.\n\r", ch);
	return;
    }

    if (!IS_CLASS(victim, CLASS_DEMON))
    {
	send_to_char("Only demons can be promoted in this fashion.\n\r", ch);
	return;
    }

/*
    if ( victim == ch )
    {
	send_to_char("You must be promoted by another.\n\r",ch);
	return;
    }
*/

    if ( ch->class != victim->class && ch->level < 12)
    {
	send_to_char("You can't promote someone of a different race.\n\r",ch);
	return;
    }

/*
    if ( victim->generation >= ch->generation)
    {
	send_to_char("You can't promote someone of an equal or higher rank.\n\r",ch);
	return;
    }
*/

    send_to_char("You have been chosen for promotion.\n\r",victim);

    if ( victim->generation == 13)
    {
	send_to_char("But you cannot transform any further.\n\r",victim);
	send_to_char("They are already maximum generation.\n\r",ch);
	return;
    }

    set_form(victim, get_normal_form(victim) + 1);
    victim->generation+=1;
    look_name(victim);

/*
    sprintf(buf, "%s has been promoted by %s.", victim->pcdata->switchname,
ch->pcdata->switchname);
    do_info(ch,buf);
*/

    /* DEMON GENERATIONS....  HAVE FUN */
    
    if (IS_CLASS(ch, CLASS_DEMON)
	&& victim->generation == 11)
    {
	  victim->size = SIZE_HUGE;
	  victim->proper_size = SIZE_HUGE;
    }

    if (IS_CLASS(victim, CLASS_DEMON)
	&& victim->generation > 7 )
    {
	ch->pcdata->perm_str += 1;
    }

    victim->hit += 100;
    victim->max_hit += 100;
    victim->xhitroll += 10;
    victim->xdamroll += 10;

    send_to_char(transformation_message[0][victim->generation-2],victim);
    act("$N has been successfully promoted to the next rank.", ch, NULL, victim, TO_CHAR);
}



void do_weaplist( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_STRING_LENGTH];
    if (IS_NPC(ch)) return;
 
 
send_to_char(

"------------------------------------------------------------------------------\n\r",ch);
send_to_char(
 "                    Weapon, Stance, and Spell Skill           \n\r",ch);
send_to_char(

"------------------------------------------------------------------------------\n\r",ch);
send_to_char(
 "                            Weapon\n\r",ch);
sprintf( buf,
"Unarm:%-4d Slice:%-4d Stab:%-4d Slash:%-4d Whip:%-4d Claw:%-4d Blast:%-4d\n\r",
ch->wpn[0],ch->wpn[1],ch->wpn[2],ch->wpn[3],ch->wpn[4],ch->wpn[5],ch->wpn[6]);
send_to_char(buf,ch);
sprintf( buf,
"Pound  :%-4d Crush  :%-4d Grep   :%-4d Bite    :%-4d Pierce:%-4d Suck   :%-4d\n\r",
ch->wpn[7],ch->wpn[8],ch->wpn[9],ch->wpn[10],ch->wpn[11],ch->wpn[12]);
send_to_char(buf,ch);
send_to_char(

"------------------------------------------------------------------------------\n\r",ch);
send_to_char(
 "                            Stances\n\r",ch);
sprintf( buf,
"Viper : %-4d Crane: %-4d Crab  : %-4d Mongoose: %-4d Bull: %-4d Mantis: %-4d\n\r",
ch->stance[1],ch->stance[2],ch->stance[3],ch->stance[4],ch->stance[5],ch->stance[6]);
send_to_char(buf,ch);
sprintf( buf,
"Dragon: %-4d Tiger: %-4d Monkey: %-4d Swallow : %-4d XKilla: %-4d\n\r",
ch->stance[7],ch->stance[8],ch->stance[9],ch->stance[10],ch->stance[11]);
send_to_char(buf,ch);
send_to_char(

"------------------------------------------------------------------------------\n\r",ch);
send_to_char(
 "                            Magic\n\r",ch);
sprintf( buf,
"Purple: %-4d  Red: %-4d  Blue: %-4d  Green: %-4d  Yellow: %-4d\n\r",ch->spl[0],ch->spl[1],ch->spl[2],ch->spl[3],ch->spl[4]);
send_to_char(buf,ch);
send_to_char(

"------------------------------------------------------------------------------\n\r",ch);
 
 
return;
}

 
void do_far( CHAR_DATA *ch, char *argument )
{
   CHAR_DATA *victim;
   ROOM_INDEX_DATA *chroom;
   ROOM_INDEX_DATA *victimroom;
   char arg1[MAX_STRING_LENGTH];

   argument = one_argument (argument, arg1);

    if ( IS_NPC(ch) )
	return;

	if (arg1[0] == '\0')
	{
	   send_to_char("Who do you wish to use farcommand on?\n\r",ch);
	   return;
	}
/*  
        if (arg2[0] == '\0')
	{
	   send_to_char("What do you wish to do to them?\n\r",ch);
	   return;
	}

	if (arg3[0] == '\0')
	{
	   send_to_char("Possess or command who?\n\r",ch);
	   return;
	}
*/
  	if (!IS_CLASS(ch, CLASS_VAMPIRE))
  	{
  	send_to_char("Huh?\n\r", ch);
  	return;
  	}
  	
  	if ( IS_CLASS(ch, CLASS_VAMPIRE) && ch->power[DISC_VAMP_PRES] < 9 )
  	{
  	send_to_char("You need level 9 Presence to use this power.\n\r", ch);
	return;
	}

      if ( ( victim = get_char_world( ch, arg1 ) ) == NULL )
    {
	send_to_char( "They aren't here.\n\r", ch );
	return;
    }
 
     if ( !IS_NPC(victim) )
	{
	   send_to_char("Their mind is too strong!\n\r",ch);
	   return;
	}
    if ( victim->level > 250)
    {
    send_to_char("They are too big!\n\r",ch);
    return;
    } 
  
    chroom = ch->in_room;
    victimroom = victim->in_room;
    char_from_room(ch);
    char_to_room(ch,victimroom);
    ch->pcdata->familiar = victim;
    victim->wizard = ch;
    send_to_char("You possess their body.\n\r",ch);
    WAIT_STATE(ch, 50);
    char_from_room(ch);
    char_to_room(ch,chroom);  	
    
    return;
}

void do_stat( CHAR_DATA *ch, char *argument )
{
    char buf   [MAX_STRING_LENGTH];
    char lin   [MAX_STRING_LENGTH];
    char age   [MAX_STRING_LENGTH];
    int blood;
    int bloodpool;

    if (ch->generation <= 0)
	ch->generation = 4;

    {
    if (IS_SET(ch->newbits, NEW_TIDE))
    bloodpool = (3000 / ch->generation);
    else bloodpool = (2000 / ch->generation);
    }

    blood = ch->pcdata->condition[COND_THIRST];
    if (IS_NPC(ch)) return;
    if (!IS_CLASS(ch, CLASS_VAMPIRE)) return;
    if (ch->pcdata->rank == AGE_ANCILLA)         sprintf( age, "Ancilla");
    else if (ch->pcdata->rank == AGE_CHILDE)     sprintf( age, "Childe");
    else if (ch->pcdata->rank == AGE_NEONATE)    sprintf( age, "Neonate");
    else if (ch->pcdata->rank == AGE_ELDER)      sprintf( age, "Elder");
    else if (ch->pcdata->rank == AGE_METHUSELAH) sprintf( age, "Methuselah");
    sprintf( lin,
"---------------------------------------------------------------------------\n\r");
    send_to_char( lin,ch);
    send_to_char(
"                              Vampire Status\n\r",ch);
send_to_char(lin,ch);
sprintf(buf,
"Generation:%d  Bloodpool:%d  Age:%s  Blood:%d\n\r",
ch->generation,bloodpool,age,blood);
send_to_char(buf,ch);
send_to_char(lin,ch);
send_to_char(
"                              Disciplines\n\r",ch);
send_to_char(lin,ch);
sprintf(buf,
"Animalism:    [%d]             Celerity:   [%d]             Fortitude: [%d]\n\r",
ch->power[DISC_VAMP_ANIM],ch->power[DISC_VAMP_CELE],
ch->power[DISC_VAMP_FORT]);
send_to_char(buf,ch);
sprintf(buf,
"Obtenebration:[%d]             Presence:   [%d]             Quietus:   [%d]\n\r",
ch->power[DISC_VAMP_OBTE],ch->power[DISC_VAMP_PRES],
ch->power[DISC_VAMP_QUIE]);
send_to_char(buf,ch);
sprintf(buf,
"Thaumaturgy:  [%d]             Auspex:     [%d]             Dominate:  [%d]\n\r",
ch->power[DISC_VAMP_THAU],ch->power[DISC_VAMP_AUSP],
ch->power[DISC_VAMP_DOMI]);
send_to_char(buf,ch);
sprintf(buf,
"Obfuscate:    [%d]             Potence:    [%d]             Protean:   [%d]\n\r",
ch->power[DISC_VAMP_OBFU],ch->power[DISC_VAMP_POTE],
ch->power[DISC_VAMP_PROT]);
send_to_char(buf,ch);
sprintf(buf,
"Serpentis:    [%d]             Vicissitude:[%d]             Daimoinon: [%d]\n\r",
ch->power[DISC_VAMP_SERP],ch->power[DISC_VAMP_VICI],
ch->power[DISC_VAMP_DAIM]);
send_to_char(buf,ch);
sprintf(buf,			 
"Chimerstry:   [%d]             Obeah:      [%d]             Melpominee:[%d]\n\r",
ch->power[DISC_VAMP_CHIM], ch->power[DISC_VAMP_OBEA], ch->power[DISC_VAMP_MELP]);
send_to_char(buf,ch);
sprintf(buf,
"Necromancy:   [%d]                                         Thanatosis:[%d]\n\r",
ch->power[DISC_VAMP_NECR], ch->power[DISC_VAMP_THAN] );
stc(buf,ch);
send_to_char(lin,ch);
 
return;
}

void do_racestats( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_STRING_LENGTH];
    
    banner_to_char("S T A T I S T I C S",ch);
    
    if (ch->class == 0)
       sprintf(buf,"You have yet to be transformed.");
    else if (ch->class == CLASS_DEMON)
       sprintf(buf,"You have been transformed into a Demon.");
    else if (ch->class == CLASS_VAMPIRE)
       sprintf(buf,"You have been transformed into a Vampire.");
    else if (ch->class == CLASS_WEREWOLF)
       sprintf(buf,"You have been transformed into a Werewolf.");
    else if (ch->class == CLASS_MAGE)
       sprintf(buf,"You have been trained in the ways of Magic.");
    else if (ch->class == CLASS_DROW)
       sprintf(buf,"You have become a legion of Lloth's Army.");
    else if (ch->class == CLASS_MONK)
       sprintf(buf,"You have been guided by the ways of the Almighty.");
    else
       sprintf(buf,"You have been transformed.");

    cent_to_char( buf, ch );

    if (ch->class == CLASS_VAMPIRE)
    {
	sprintf(buf, "You are of generation %d.", ch->generation);
	cent_to_char(buf, ch);
    }

    if (ch->class == 0)
    {
	sprintf(buf, "You are %d%% of the way to being powerful enough for transformation,",
		ch->explevel * 100 / 20);
	cent_to_char(buf, ch);
	if (ch->max_hit < 1500 && ch->max_move >= 1500)
	    sprintf(buf, "and you need %d more hit points.", 1500 - ch->max_hit);
	else if (ch->max_hit < 1500 && ch->max_move < 1500)
	    sprintf(buf, "and you need %d more hit points and %d more vitality.",
		1500 - ch->max_hit, 1500 - ch->max_move);
	else if (ch->max_move < 1500)
	    sprintf(buf, "and you need %d more vitality.", 1500 - ch->max_move);
	else sprintf(buf, "and you have the required hit points and vitality.");
	cent_to_char(buf, ch);
    }

    if (ch->cur_form != 0)
    {
	sprintf(buf, "You are currently in the form of a %s.",
	    form_data[ch->cur_form].lookname);
	cent_to_char(buf, ch);
    }

    ch->size = get_size(ch);

    if (ch->size == SIZE_TINY)
	cent_to_char("You are of tiny size.", ch);
    else if (ch->size == SIZE_SMALL)
	cent_to_char("You are of small size.", ch);
    else if (ch->size == SIZE_MEDIUM)
	cent_to_char("You are of medium size.", ch);
    else if (ch->size == SIZE_LARGE)
	cent_to_char("You are of large size.", ch);
    else if (ch->size == SIZE_HUGE)
	cent_to_char("You are of huge size.", ch);
    else if (ch->size == SIZE_GIANT)
	cent_to_char("You are absolutely gigantic.", ch);
    else
	cent_to_char("You are of medium size.", ch);

    if (IS_SET(ch->act, PLR_WIZINVIS) && IS_IMMORTAL(ch))
    {
	sprintf( buf, "You are invisible.");
	cent_to_char(buf,ch);
    }

    divide5_to_char(ch);

    if (IS_SET(ch->newbits, THIRD_HAND) && !IS_SET(ch->newbits, FOURTH_HAND))
	  cent_to_char("You have grown an extra arm.",ch);
    if (!IS_SET(ch->newbits, THIRD_HAND) && IS_SET(ch->newbits, FOURTH_HAND))
        cent_to_char("You have grown an extra arm.",ch);
    if (IS_SET(ch->newbits, THIRD_HAND) && IS_SET(ch->newbits, FOURTH_HAND))
        cent_to_char("You have grown two extra arms.",ch);

    if (ch->class == CLASS_VAMPIRE
	&& ch->pcdata->stats[COND_THIRST] < 20)
    	cent_to_char("You are dangerously low on blood.", ch);

    if (!IS_NPC(ch) && str_cmp(ch->name, ch->pcdata->switchname)
	&& !IS_BASE_FORM(ch))
    {
	sprintf(buf,"You are currently masked as %s.", ch->name);
	cent_to_char(buf, ch);
    }

    if (ch->class == CLASS_DEMON
	&& !IS_NPC(ch)
	&& ch->generation != 13)
    {
	sprintf(buf,"You have collected %d souls, and need %d more for promotion."
		, ch->pcdata->souls, souls_needed(ch) - ch->pcdata->souls);
	cent_to_char(buf, ch);
    }
   
    if (ch->class == 0)
    {
	if (IS_NEWFLAG(ch,AFF_ALLOW_VAMP))
		cent_to_char("You may be bitten by vampires.",ch);
	if (IS_NEWFLAG(ch,AFF_ALLOW_WERE))
		cent_to_char("You may be cursed with lycanthropy.",ch);
	if (IS_NEWFLAG(ch,AFF_ALLOW_ELAD))
		cent_to_char("You may join the eladrin.",ch);
	if (IS_NEWFLAG(ch,AFF_ALLOW_DEMON))
		cent_to_char("You are willing to sell your soul to Baator.",ch);
	if (IS_NEWFLAG(ch,AFF_ALLOW_WARLOCK))
		cent_to_char("You are ready to study to become a Warlock.",ch);
    }

   divide5_to_char(ch);

    if (!IS_NPC(ch) )
    {
	  sprintf(buf,"Your maximum damage is currently %d.",ch->damcap[0]);
        cent_to_char(buf, ch);
	sprintf(buf, "You have killed %d enemy mobs, and been killed by %d.", ch->mkill, ch->mdeath);
	cent_to_char(buf, ch);
    }

    if (ch->power[DISC_DAEM_GELU] > 1 && ch->played/18000 > 0)
    {
	sprintf(buf,"Your icy strength is improving your damage by %d.", ch->played/18000);
	cent_to_char(buf, ch);
    }

   if (IS_VAMPAFF(ch, VAM_FANGS)
	|| IS_VAMPAFF(ch, VAM_CLAWS)
	|| IS_VAMPAFF(ch, VAM_WINGS)
	|| IS_VAMPAFF(ch, VAM_HORNS)
	|| IS_VAMPAFF(ch, VAM_TAIL))
   {
	bool comma = FALSE;
	strcpy(buf, "You have extra ");
	if (IS_VAMPAFF(ch, VAM_FANGS))
	{
	    comma = TRUE;
	    strcat(buf, "bite");
        }
	if (IS_VAMPAFF(ch, VAM_CLAWS))
	{
	    if (comma)
	    {
		if (!IS_VAMPAFF(ch, VAM_WINGS) && IS_VAMPAFF(ch,VAM_TAIL))
		    strcat(buf, " and ");
		else strcat(buf, ", ");
	    }
	    comma = TRUE;
	    strcat(buf, "claw");
        }
	if (IS_VAMPAFF(ch, VAM_WINGS))
	{
	    if (comma)
	    {
		if (!IS_VAMPAFF(ch, VAM_TAIL))
		    strcat(buf, " and ");
		else strcat(buf,", ");
	    }
	    comma = TRUE;
	    strcat(buf,"wing");
        }
	if (IS_VAMPAFF(ch, VAM_HORNS))
	{
	    if (comma)
	    {
		if (!IS_VAMPAFF(ch, VAM_TAIL))
		    strcat(buf, " and ");
		else strcat(buf,", ");
	    }
	    comma = TRUE;
	    strcat(buf,"horn");
        }
	if (IS_VAMPAFF(ch, VAM_TAIL))
	{
	    if (comma) strcat(buf," and ");
	    strcat(buf,"tail");
        }

	strcat(buf," attacks.");
	cent_to_char(buf,ch);
    }

    divide4_to_char(ch);
}

/**** ALLOW COMMANDS ****/

void do_vampallow(CHAR_DATA *ch, char *argument)
{
    if (IS_NPC(ch)) return;

    if (IS_NEWFLAG(ch,AFF_ALLOW_VAMP))
    {
	send_to_char("You refuse your body to the kindred.\n\r",ch);
	REMOVE_BIT(ch->flag2,AFF_ALLOW_VAMP);
	return;
    }
    else
    {
	send_to_char("You give your body to the kindred.\n\r",ch);
	SET_BIT(ch->flag2,AFF_ALLOW_VAMP);
	return;
    }
}

void do_wereallow(CHAR_DATA *ch, char *argument)
{
    if (IS_NPC(ch)) return;

    if (IS_NEWFLAG(ch,AFF_ALLOW_WERE))
    {
	send_to_char("You are now immune to lycanthropy.\n\r",ch);
	REMOVE_BIT(ch->flag2,AFF_ALLOW_WERE);
	return;
    }
    else
    {
	send_to_char("You gift your body to the lupines.\n\r",ch);
	SET_BIT(ch->flag2,AFF_ALLOW_WERE);
	return;
    }
}

void do_eladallow(CHAR_DATA *ch, char *argument)
{
    if (IS_NPC(ch)) return;

    if (IS_NEWFLAG(ch,AFF_ALLOW_ELAD))
    {
	send_to_char("You reject the ways of the faerie folk.\n\r",ch);
	REMOVE_BIT(ch->flag2,AFF_ALLOW_ELAD);
	return;
    }
    else
    {
	send_to_char("You pledge your life to nature.\n\r",ch);
	SET_BIT(ch->flag2,AFF_ALLOW_ELAD);
	return;
    }
}

void do_demonallow(CHAR_DATA *ch, char *argument)
{
    if (IS_NPC(ch)) return;

    if (IS_NEWFLAG(ch,AFF_ALLOW_DEMON))
    {
	send_to_char("You snatch your soul back from eternal damnation.\n\r",ch);
	REMOVE_BIT(ch->flag2,AFF_ALLOW_DEMON);
	return;
    }
    else
    {
	send_to_char("You sell your soul to darkness.\n\r",ch);
	SET_BIT(ch->flag2,AFF_ALLOW_DEMON);
	return;
    }
}

void do_warlockallow(CHAR_DATA *ch, char *argument)
{
    if (IS_NPC(ch)) return;

    if (IS_NEWFLAG(ch,AFF_ALLOW_WARLOCK))
    {
	send_to_char("You reject the paths of magic.\n\r",ch);
	REMOVE_BIT(ch->flag2,AFF_ALLOW_WARLOCK);
	return;
    }
    else
    {
	send_to_char("You prepare yourself for a life of studying.\n\r",ch);
	SET_BIT(ch->flag2,AFF_ALLOW_WARLOCK);
	return;
    }
}

void do_score( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_STRING_LENGTH];
    char ss1[MAX_STRING_LENGTH];
    char ss2[MAX_STRING_LENGTH];
    AFFECT_DATA *paf;
    int a_c = char_ac(ch);

	if (IS_NPC(ch)) return;

    if (!IS_NPC(ch) && (IS_EXTRA(ch,EXTRA_OSWITCH) || IS_HEAD(ch,LOST_HEAD)))
	{obj_score(ch,ch->pcdata->chobj);return;}
    sprintf( buf,
	"You are %s%s.  You have been playing for %d hours.\n\r",
	ch->pcdata->switchname,
	IS_NPC(ch) ? "" : ch->pcdata->title,
	(get_age(ch) - 17) * 2 );
    send_to_char( buf, ch );
    if (!IS_NPC(ch)) birth_date(ch, TRUE);
    if (!IS_NPC(ch) && IS_EXTRA(ch, EXTRA_PREGNANT)) birth_date(ch, FALSE);

    if ( get_trust( ch ) != ch->level )
    {
	sprintf( buf, "You are trusted at level %d.\n\r",
	    get_trust( ch ) );
	send_to_char( buf, ch );
    }

    sprintf( buf,
	"You have %d/%d hit, %d/%d mana, %d/%d movement, %d primal energy.\n\r",
	ch->hit,  ch->max_hit,
	ch->mana, ch->max_mana,
	ch->move, ch->max_move,
	ch->practice );
    send_to_char( buf, ch );

    sprintf( buf,
	"You are carrying %d/%d items with weight %d/%d kg.\n\r",
	ch->carry_number, can_carry_n(ch),
	ch->carry_weight, can_carry_w(ch) );
    send_to_char( buf, ch );

    sprintf( buf,
	"Str: %d  Int: %d  Wis: %d  Dex: %d  Con: %d.\n\r",
	get_curr_str(ch),
	get_curr_int(ch),
	get_curr_wis(ch),
	get_curr_dex(ch),
	get_curr_con(ch) );
    send_to_char( buf, ch );

    sprintf( buf,
	"You have scored %d exp, and have %d gold coins.\n\r",
	ch->exp,  ch->gold );
    send_to_char( buf, ch );

    if (!IS_NPC(ch) && (IS_CLASS( ch, CLASS_DEMON) || IS_SET(ch->special,SPC_CHAMPION)))
    {
	sprintf( buf,
	"You have %d out of %d points of demonic power stored.\n\r",
	ch->pcdata->stats[DEMON_CURRENT],  ch->pcdata->stats[DEMON_TOTAL] );
	send_to_char( buf, ch );
    }
    if (!IS_NPC(ch) && IS_CLASS(ch, CLASS_DEMON) && ch->generation < 13)
    {
	sprintf( buf,
	"You have obtained %d souls and require %d more for a promotion.\n\r",
	ch->pcdata->souls, souls_needed(ch));
	stc(buf, ch);
    }

    if (!IS_NPC(ch) && IS_CLASS(ch, CLASS_DROW)) {
	sprintf( buf, "You have %d drow power points!.\n\r",
	ch->pcdata->stats[DROW_POWER] );
	send_to_char( buf, ch );
	sprintf( buf, "You have %d points of magic resistance.\n\r",
	ch->pcdata->stats[DROW_MAGIC] );
	send_to_char( buf, ch ); 
	if (weather_info.sunlight == SUN_DARK) 
	send_to_char("You feel strong in the night.\n\r", ch );}

    sprintf( buf,
	"Autoexit: %s.  Autoloot: %s.  Autosac: %s.\n\r",
	(!IS_NPC(ch) && IS_SET(ch->act, PLR_AUTOEXIT)) ? "yes" : "no",
	(!IS_NPC(ch) && IS_SET(ch->act, PLR_AUTOLOOT)) ? "yes" : "no",
	(!IS_NPC(ch) && IS_SET(ch->act, PLR_AUTOSAC) ) ? "yes" : "no" );
    send_to_char( buf, ch );
    
    sprintf( buf, "Wimpy set to %d hit points.", ch->wimpy );
    if ( !IS_NPC(ch) && IS_EXTRA(ch, EXTRA_DONE) )
	strcat(buf,"  You've done the dead.\n\r");
    else strcat(buf,"\n\r");
    send_to_char( buf, ch );


    if(!IS_NPC(ch))
    {
       sprintf(buf,"[Arena Stats] Wins: %d  losses: %d\n\r",
       ch->pcdata->awins, ch->pcdata->alosses );
       send_to_char( buf, ch);
       if(ch->challenger != NULL)
       {
          sprintf(buf,"[Arena] You have been challenged by %s.\n\r",
          ch->challenger->name);
          send_to_char( buf, ch);
       }
       if(ch->challenged != NULL)
       {
          sprintf(buf,"[Arena] You have challenged %s.\n\r",
          ch->challenged->name);
          send_to_char( buf, ch);
       }
       if(ch->gladiator != NULL)
       {
          sprintf(buf,"[Arena bet] You have a %d gold bet on %s\n\r",
          ch->pcdata->plr_wager, ch->gladiator->name);
          send_to_char( buf, ch);
       }
    }

    if ( !IS_NPC(ch) && IS_CLASS(ch, CLASS_MONK) )
    {
      sprintf(buf,"Your block counter is currently: %d\n\r",ch->monkblock);
      stc(buf,ch);
      sprintf(buf,"Your current level of chi:       %d\n\r",ch->chi[CURRENT]);
      stc(buf,ch);
      sprintf(buf,"Your maximum level of chi:       %d\n\r",ch->chi[MAXIMUM]);
      stc(buf,ch);
      sprintf(buf,"Your current focus points:       %d\n\r",ch->focus[CURRENT]);
      stc(buf,ch);
      sprintf(buf,"Your maximum focus points:       %d\n\r",ch->focus[MAXIMUM]);
      stc(buf,ch);
    }

    if ( !IS_NPC(ch) && ch->pcdata->condition[COND_DRUNK]   > 10 )
	send_to_char( "You are drunk.\n\r",   ch );
    if ( !IS_NPC(ch) && ch->pcdata->condition[COND_THIRST] ==  0 )
	send_to_char( "You are thirsty.\n\r", ch );
    if ( !IS_NPC(ch) && ch->pcdata->condition[COND_FULL]   ==  0 )
	send_to_char( "You are hungry.\n\r",  ch );
    if ( !IS_NPC(ch) && ch->pcdata->stage[0] >= 100 )
	send_to_char( "You are feeling extremely horny.\n\r",  ch );
    else if ( !IS_NPC(ch) && ch->pcdata->stage[0] >= 50 )
	send_to_char( "You are feeling pretty randy.\n\r",  ch );
    else if ( !IS_NPC(ch) && ch->pcdata->stage[0] >= 1 )
	send_to_char( "You are feeling rather kinky.\n\r",  ch );

    if ( !IS_NPC(ch) && ch->pcdata->stage[1] > 0 && ch->position == POS_STANDING )
    {
	send_to_char( "You are having sexual intercourse.\n\r",  ch );
	if (!IS_NPC(ch) && (ch->pcdata->stage[2] + 25) >= ch->pcdata->stage[1])
	    send_to_char( "You are on the verge of having an orgasm.\n\r",  ch );
    }
    else switch ( ch->position )
    {
    case POS_DEAD:     
	send_to_char( "You are DEAD!!\n\r",		ch );
	break;
    case POS_MORTAL:
	send_to_char( "You are mortally wounded.\n\r",	ch );
	break;
    case POS_INCAP:
	send_to_char( "You are incapacitated.\n\r",	ch );
	break;
    case POS_STUNNED:
	send_to_char( "You are stunned.\n\r",		ch );
	break;
    case POS_SLEEPING:
	send_to_char( "You are sleeping.\n\r",		ch );
	break;
    case POS_RESTING:
	send_to_char( "You are resting.\n\r",		ch );
	break;
    case POS_MEDITATING:
	send_to_char( "You are meditating.\n\r",	ch );
	break;
    case POS_SITTING:
	send_to_char( "You are sitting.\n\r",		ch );
	break;
    case POS_STANDING:
	send_to_char( "You are standing.\n\r",		ch );
	break;
    case POS_FIGHTING:
	send_to_char( "You are fighting.\n\r",		ch );
	break;
    }

    if ( ch->level >= 0 )
    {
	sprintf( buf, "AC: %d.  ", a_c );
	send_to_char( buf, ch );
    }

    send_to_char( "You are ", ch );
         if ( a_c >=  101 ) send_to_char( "naked!\n\r", ch );
    else if ( a_c >=   80 ) send_to_char( "barely clothed.\n\r",   ch );
    else if ( a_c >=   60 ) send_to_char( "wearing clothes.\n\r",  ch );
    else if ( a_c >=   40 ) send_to_char( "slightly armored.\n\r", ch );
    else if ( a_c >=   20 ) send_to_char( "somewhat armored.\n\r", ch );
    else if ( a_c >=    0 ) send_to_char( "armored.\n\r",          ch );
    else if ( a_c >=  -50 ) send_to_char( "well armored.\n\r",     ch );
    else if ( a_c >= -100 ) send_to_char( "strongly armored.\n\r", ch );
    else if ( a_c >= -250 ) send_to_char( "heavily armored.\n\r",  ch );
    else if ( a_c >= -500 ) send_to_char( "superbly armored.\n\r", ch );
    else if ( a_c >= -749 ) send_to_char( "divinely armored.\n\r", ch );
    else                    send_to_char( "ultimately armored!\n\r",ch );

    sprintf( buf, "Hitroll: %d.  Damroll: %d.  Damcap: %d. ",
char_hitroll(ch), char_damroll(ch), ch->damcap[0] );
    send_to_char( buf, ch );
    

    if ( !IS_NPC(ch) && IS_CLASS(ch,CLASS_VAMPIRE) )
    {
	sprintf( buf, "Blood: %d.\n\r", ch->pcdata->condition[COND_THIRST] );
	send_to_char( buf, ch );

/*	sprintf( buf, "Beast: %d.  ", ch->beast );
	send_to_char( buf, ch );
	if      (ch->beast <  0 ) send_to_char("You are a cheat!\n\r",ch);
	else if (ch->beast == 0 ) send_to_char("You have attained Golconda!\n\r",ch);
	else if (ch->beast <= 5 ) send_to_char("You have almost reached Golconda!\n\r",ch);
	else if (ch->beast <= 10) send_to_char("You are nearing Golconda!\n\r",ch);
	else if (ch->beast <= 15) send_to_char("You have great control over your beast.\n\r",ch);
	else if (ch->beast <= 20) send_to_char("Your beast has little influence over your actions.\n\r",ch);
	else if (ch->beast <= 30) send_to_char("You are in control of your beast.\n\r",ch);
	else if (ch->beast <= 40) send_to_char("You are able to hold back the beast.\n\r",ch);
	else if (ch->beast <= 60) send_to_char("You are constantly struggling for control of your beast.\n\r",ch);
	else if (ch->beast <= 75) send_to_char("Your beast has great control over your actions.\n\r",ch);
	else if (ch->beast <= 90) send_to_char("The power of the beast overwhelms you.\n\r",ch);
	else if (ch->beast <= 99) send_to_char("You have almost lost your battle with the beast!\n\r",ch);
	else                      send_to_char("The beast has taken over!\n\r",ch);
*/
    }
    else if (ch->level >= 0)
	send_to_char( "\n\r", ch );

    if ( ch->level >= 0 )
    {
	sprintf( buf, "Alignment: %d.  ", ch->alignment );
	send_to_char( buf, ch );
    }

    send_to_char( "You are ", ch );
         if ( ch->alignment >  900 ) send_to_char( "angelic.\n\r", ch );
    else if ( ch->alignment >  700 ) send_to_char( "saintly.\n\r", ch );
    else if ( ch->alignment >  350 ) send_to_char( "good.\n\r",    ch );
    else if ( ch->alignment >  100 ) send_to_char( "kind.\n\r",    ch );
    else if ( ch->alignment > -100 ) send_to_char( "neutral.\n\r", ch );
    else if ( ch->alignment > -350 ) send_to_char( "mean.\n\r",    ch );
    else if ( ch->alignment > -700 ) send_to_char( "evil.\n\r",    ch );
    else if ( ch->alignment > -900 ) send_to_char( "demonic.\n\r", ch );
    else                             send_to_char( "satanic.\n\r", ch );
    
    if ( !IS_NPC(ch) && ch->level >= 0 )
    {
	sprintf( buf, "Status: %d.  ", ch->race );
	send_to_char( buf, ch );
    }


    if (!IS_NPC(ch)) send_to_char( "You are ", ch );

         if (!IS_NPC(ch) && ch->level == 1 ) send_to_char( "a Mortal.\n\r", ch);
    else if (!IS_NPC(ch) && ch->level == 2 ) send_to_char( "a Mortal.\n\r", ch);
    else if (!IS_NPC(ch) && ch->level == 7 ) send_to_char( "a Builder.\n\r", ch);
    else if (!IS_NPC(ch) && ch->level == 8 ) send_to_char( "a Quest Maker.\n\r", ch);
    else if (!IS_NPC(ch) && ch->level == 9 ) send_to_char( "an Enforcer.\n\r", ch);
    else if (!IS_NPC(ch) && ch->level == 10) send_to_char( "a Judge.\n\r", ch);
    else if (!IS_NPC(ch) && ch->level == 11) send_to_char( "a High Judge.\n\r", ch);
    else if (!IS_NPC(ch) && ch->level == 12) send_to_char( "an Implementor.\n\r", ch);
    else if (!IS_NPC(ch) && ch->race <= 0 ) send_to_char( "an Avatar.\n\r",ch);
    else if (!IS_NPC(ch) && ch->race <= 4 ) send_to_char( "Immortal.\n\r", ch);
    else if (!IS_NPC(ch) && ch->race <= 9 ) send_to_char( "Godling.\n\r", ch);
    else if (!IS_NPC(ch) && ch->race <= 14) send_to_char( "Demigod.\n\r",ch);
    else if (!IS_NPC(ch) && ch->race <= 19) send_to_char( "Lesser God.\n\r", ch);
    else if (!IS_NPC(ch) && ch->race <= 24) send_to_char( "Greater God.\n\r", ch);
    else if (!IS_NPC(ch) && ch->race >= 25) send_to_char( "Supreme God.\n\r", ch);
    else if (!IS_NPC(ch)) send_to_char( "a Bugged Character!\n\r", ch);

    if ( !IS_NPC(ch) )
    {
	if      (ch->pkill  == 0) sprintf(ss1,"no players");
	else if (ch->pkill  == 1) sprintf(ss1,"%d player",ch->pkill);
	else                      sprintf(ss1,"%d players",ch->pkill);
	if      (ch->pdeath == 0) sprintf(ss2,"no players");
	else if (ch->pdeath == 1) sprintf(ss2,"%d player",ch->pdeath);
	else                      sprintf(ss2,"%d players",ch->pdeath);
	sprintf( buf, "You have killed %s and have been killed by %s.\n\r", ss1, ss2 );
	send_to_char( buf, ch );
	if      (ch->mkill  == 0) sprintf(ss1,"no mobs");
	else if (ch->mkill  == 1) sprintf(ss1,"%d mob",ch->mkill);
	else                      sprintf(ss1,"%d mobs",ch->mkill);
	if      (ch->mdeath == 0) sprintf(ss2,"no mobs");
	else if (ch->mdeath == 1) sprintf(ss2,"%d mob",ch->mdeath);
	else                      sprintf(ss2,"%d mobs",ch->mdeath);
	sprintf( buf, "You have killed %s and have been killed by %s.\n\r", ss1, ss2 );
	send_to_char( buf, ch );
    }

    if ( !IS_NPC(ch) && ch->pcdata->quest > 0)
    {
	if (ch->pcdata->quest == 1)
	    sprintf( buf, "You have a single quest point.\n\r" );
	else
	    sprintf( buf, "You have %d quest points.\n\r", ch->pcdata->quest );
	send_to_char( buf, ch );
    }

    if (IS_AFFECTED(ch,AFF_HIDE)) send_to_char( "You are keeping yourself hidden from those around you.\n\r", ch );

    if ( !IS_NPC(ch) )
    {
	if ( IS_CLASS(ch,CLASS_WEREWOLF) && ch->siltol > 0)
	{
	    sprintf(buf,"You have attained %d points of silver tolerance.\n\r",ch->siltol);
	    send_to_char( buf, ch );
	}
	if ( IS_CLASS(ch,CLASS_VAMPIRE) && ch->rage > 0)
	{
	    sprintf(buf,"The beast is in control of your actions:  Affects Hitroll and Damroll by +%d.\n\r",ch->rage);
	    send_to_char( buf, ch );
	}
	if ( IS_CLASS(ch, CLASS_WEREWOLF) && IS_SET(ch->special,SPC_WOLFMAN) && ch->rage > 0)
	{
	    sprintf(buf,"You are raging:  Affects Hitroll and Damroll by +%d.\n\r",ch->rage);
	    send_to_char( buf, ch );
	}
	if (IS_CLASS(ch, CLASS_NINJA) && ch->rage > 0) {
	sprintf(buf,"Your michi gives you an extra %d hitroll and damroll.\n\r", ch->rage);
	send_to_char(buf,ch);
	}
	if (IS_CLASS(ch, CLASS_NINJA) && ch->pcdata->powers[NPOWER_CHIKYU] >= 6
	&& ch->pcdata->powers[HARA_KIRI] > 0) {
	sprintf(buf,"You receive the power of HaraKiri for %d more ticks.\n\r",ch->pcdata->powers[HARA_KIRI]);
	send_to_char(buf,ch);
	}

        if ( !IS_CLASS(ch, CLASS_WEREWOLF) && !IS_CLASS(ch, CLASS_VAMPIRE) &&
ch->rage > 0 && !IS_CLASS(ch, CLASS_NINJA))
	{
	   sprintf(buf,"You are in a mad frenzy, adding %d Hitroll and Damroll.\n\r",ch->rage);
	   stc(buf, ch);
	}

	else if ( IS_CLASS( ch, CLASS_DEMON) && ch->pcdata->stats[DEMON_POWER] > 0)
	{
	    sprintf(buf,"You are wearing demonic armour:  Affects Hitroll and Damroll by +%d.\n\r",
		((ch->pcdata->stats[DEMON_POWER]) *ch->pcdata->stats[DEMON_POWER]));
	    send_to_char( buf, ch );
	}
/*
	if ( IS_CLASS( ch, CLASS_DEMON) && ch->pcdata->stats[DEMON_POWER] > 0)
	{
	sprintf(buf,"You are wearing demonic armour:  Affects Hitroll and Damroll by +%d.\n\r", (ch->pcdata->stats[DEMON_POWER] * ch->pcdata->stats[DEMON_POWER]));
	stc(buf, ch);
	}

	else if ( IS_SET(ch->special,SPC_CHAMPION) && ch->pcdata->stats[DEMON_POWER] > 0)
	{
	sprintf(buf,"You are wearing demonic armour:  Affects Hitroll and Damroll by +%d.\n\r", (ch->pcdata->stats[DEMON_POWER] * ch->pcdata->stats[DEMON_POWER]));
	stc(buf, ch);
	}
*/
    }	
	if (ch->fight_timer > 0) {
	sprintf(buf,"You have %d rounds left on your fight timer.\n\r", ch->fight_timer);
	send_to_char(buf,ch);
	}
	
	
    if ( ch->affected != NULL )
    {
	send_to_char( "You are affected by:\n\r", ch );
	for ( paf = ch->affected; paf != NULL; paf = paf->next )
	{
	    sprintf( buf, "Spell: '%s'", skill_table[paf->type].name );
	    send_to_char( buf, ch );

	    if ( ch->level >= 0 )
	    {
		sprintf( buf,
		    " modifies %s by %d for %d hours with bits %s.\n\r",
		    affect_loc_name( paf->location ),
		    paf->modifier,
		    paf->duration,
		    affect_bit_name( paf->bitvector ) );
		send_to_char( buf, ch );
	    }
	}
    }

    return;
}



char *	const	day_name	[] =
{
    "the Moon", "the Bull", "Deception", "Thunder", "Freedom",
    "the Great Gods", "the Sun"
};

char *	const	month_name	[] =
{
    "Winter", "the Winter Wolf", "the Frost Giant", "the Old Forces",
    "the Grand Struggle", "the Spring", "Nature", "Futility", "the Dragon",
    "the Sun", "the Heat", "the Battle", "the Dark Shades", "the Shadows",
    "the Long Shadows", "the Ancient Darkness", "the Great Evil"
};

void do_time( CHAR_DATA *ch, char *argument )
{
    extern char str_boot_time[];
    char buf[MAX_STRING_LENGTH];
    char *suf;
    int day;

    day     = time_info.day + 1;

         if ( day > 4 && day <  20 ) suf = "th";
    else if ( day % 10 ==  1       ) suf = "st";
    else if ( day % 10 ==  2       ) suf = "nd";
    else if ( day % 10 ==  3       ) suf = "rd";
    else                             suf = "th";

    sprintf( buf,
	"It is %d o'clock %s, Day of %s, %d%s the Month of %s.\n\rThe Cronic started up at %s\rThe system time is %s\r",

	(time_info.hour % 12 == 0) ? 12 : time_info.hour % 12,
	time_info.hour >= 12 ? "pm" : "am",
	day_name[day % 7],
	day, suf,
	month_name[time_info.month],
	str_boot_time,
	(char *) ctime( &current_time )
	);

    send_to_char( buf, ch );
    return;
}


void do_weather( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_STRING_LENGTH];

    static char * const sky_look[4] =
    {
	"cloudless",
	"cloudy",
	"rainy",
	"lit by flashes of lightning"
    };

    if ( !IS_OUTSIDE(ch) )
    {
	send_to_char( "You can't see the weather indoors.\n\r", ch );
	return;
    }

    sprintf( buf, "The sky is %s and %s.\n\r",
	sky_look[weather_info.sky],
	weather_info.change >= 0
	? "a warm southerly breeze blows"
	: "a cold northern gust blows"
	);
    send_to_char( buf, ch );
    return;
}




void do_wyrm( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_STRING_LENGTH];

        if ( IS_NPC(ch) )
        return;

        if (!IS_SET(ch->extra,EXTRA_BSD))
        {
        send_to_char( "You are now a Black Spiral Dancer.\n\r", ch );
        sprintf(buf,"%s is now a Black Spiral Dancer!",ch->name);
        ADD_COLOUR(ch,buf,L_GREEN); 
	do_info(ch,buf);
        REMOVE_BIT(ch->extra,EXTRA_BSD);
        return;
        }

        else if (!IS_SET(ch->extra,EXTRA_BSD))
        {
        send_to_char( "You are now a Black Spiral Dancer.\n\r", ch );
        sprintf(buf,"%s is now a Black Spiral Dancer!",ch->name);
        ADD_COLOUR(ch,buf,L_GREEN);
        do_info(ch,buf);
        SET_BIT(ch->extra,EXTRA_BSD);
        return;
        }

else
return;

}





/*
 * New 'who' command originally by Alander of Rivers of Mud.
 */
void do_who( CHAR_DATA *ch, char *argument )
{
    char cls[MAX_STRING_LENGTH];
    char buf[MAX_STRING_LENGTH];
    char buf2[MAX_STRING_LENGTH];
    char buf3[MAX_STRING_LENGTH];
    char buf4[MAX_STRING_LENGTH];
    char buf5[MAX_STRING_LENGTH];
    char buf6[MAX_STRING_LENGTH];
    char buf7[MAX_STRING_LENGTH];
    char buf8[MAX_STRING_LENGTH];
    char buf9[MAX_STRING_LENGTH];
    char buf10[MAX_STRING_LENGTH];
    char buf11[MAX_STRING_LENGTH];
    char buf12[MAX_STRING_LENGTH];
    char buf13[MAX_STRING_LENGTH];
    char buf14[MAX_STRING_LENGTH];
    char buf15[MAX_STRING_LENGTH];
    char buf16[MAX_STRING_LENGTH];
    char buf17[MAX_STRING_LENGTH];
    char buf18[MAX_STRING_LENGTH];
    char buf19[MAX_STRING_LENGTH];
    char buf20[MAX_STRING_LENGTH];
    char kav[MAX_STRING_LENGTH];
    char king[MAX_STRING_LENGTH];
    char openb[5];
    char closeb[5];
    char *mage_col;
    DESCRIPTOR_DATA *d;
    CHAR_DATA *wch;
    int iLevelLower;
    int iLevelUpper;
    int nNumber;
    int nMatch;
    bool fClassRestrict;
    bool fImmortalOnly;
    bool immortal = FALSE,avatar = FALSE,mortal = FALSE, main = FALSE; 
    bool imm = FALSE,ava = FALSE, mor = FALSE;
    bool a1,a2,a3,a4,a5,a6,a7,a8,a9,a10,a11,a12,a13,a14,a15,a16;

    if (IS_NPC(ch)) return;

    /*
     * Set default arguments.
     */
    iLevelLower    = 0;
    iLevelUpper    = MAX_LEVEL;
    fClassRestrict = FALSE;
    fImmortalOnly  = FALSE;

    /*
     * Parse arguments.
     */
    nNumber = 0;
    for ( ;; )
    {
	char arg[MAX_STRING_LENGTH];

	argument = one_argument( argument, arg );
	if ( arg[0] == '\0' )
	    break;

	if ( is_number( arg ) )
	{
	    send_to_char("Enter 'Avatar' for level 3's, or 'God' for level 4's and 5's.\n\r",ch);
	    return;
	}
	else
	{
	    /*
	     * Look for classes to turn on.
	     */
	    arg[3]    = '\0';
	    if (   !str_cmp( arg, "imm" ) || !str_cmp( arg, "immortal" )
		|| !str_cmp( arg, "ava" ) || !str_cmp( arg, "avatar"   ) )
	    {
		fClassRestrict = TRUE;
	    }
	    else if ( !str_cmp( arg, "god" ) || !str_cmp( arg, "imp" ) )
	    {
		fImmortalOnly = TRUE;
	    }
	    else
	    {
		send_to_char("Enter 'Avatar' for level 3's, or 'God' for level 4's and 5's.\n\r",ch);
		return;
	    }
	}
    }

    /*
     * Now show matching chars.
     */
    nMatch = 0;
    buf[0] = '\0';
    buf3[0] = '\0';
    buf4[0] = '\0';
    buf5[0] = '\0';
    buf6[0] = '\0';
    buf7[0] = '\0';
    buf8[0] = '\0';
    buf9[0] = '\0';
    buf10[0] = '\0';
    buf11[0] = '\0';
    buf12[0] = '\0';
    buf13[0] = '\0';
    buf14[0] = '\0';
    buf15[0] = '\0';
    buf16[0] = '\0';
    buf17[0] = '\0';
    buf18[0] = '\0';
    buf19[0] = '\0';
    buf20[0] = '\0';
 
    for ( d = descriptor_list; d != NULL; d = d->next )
    {
//	CHAR_DATA *wch;
	char const *class;

	/*
	 * Check for match against restrictions.
	 * Don't use trust as that exposes trusted mortals.
	 */

	if ( (d->connected != CON_PLAYING && d->connected != CON_EDITING) 
	   || (!can_see( ch, d->character ) && (!IS_SET(ch->act, PLR_WATCHER)) )  )
	    continue;

	wch   = ( d->original != NULL ) ? d->original : d->character;
	if ( wch->level < iLevelLower
	||   wch->level > iLevelUpper
	|| ( fImmortalOnly  && wch->level <  LEVEL_IMMORTAL )
	|| ( fClassRestrict && wch->level != LEVEL_HERO     ) )
	    continue;

	nMatch++;

	/*
	 * Figure out what to print for class.
	 */
	class = " ";

        if ((IS_HEAD(wch,LOST_HEAD) || IS_EXTRA(wch,EXTRA_OSWITCH)) && wch->pcdata->chobj != NULL)
	{
	    if (wch->pcdata->chobj->pIndexData->vnum == 12)
	    	class = "#7A Head#n           ";
	    else if (wch->pcdata->chobj->pIndexData->vnum == 30005)
	    	class = "#0A Brain#n         ";
	    else
	    	class = "#1An Object#n       ";
	} else
	switch ( wch->level )
	{
	default: break;
	case MAX_LEVEL -  0:
		     if ( IS_EXTRA(wch, EXTRA_AFK) )
			class = "#1       AFK#n      ";
	        else if (wch->prefix != NULL)
			class = wch->prefix;
else if (!str_cmp(wch->pcdata->switchname,"Trace")) class = "#2The Cronic Lord#n ";
else if (!str_cmp(wch->pcdata->switchname,"Xkilla")) class = "#1Cronic Coder#n    ";
else if (!str_cmp(wch->pcdata->switchname,"Kip"))    class = "#4HellBringer#n     ";
	        else class = "Implementor     "; break;  
	case MAX_LEVEL -  1: 
if (!str_cmp(wch->pcdata->switchname,"Dracos")) class =      "#7Help File Lord#n  ";
                else class = "High Judge      "; break;
	case MAX_LEVEL -  2: class = "Priest          "; break;
	case MAX_LEVEL -  3: class = "Sherriff        "; break;
	case MAX_LEVEL -  4: class = "Alterboy        "; break;
	case MAX_LEVEL -  5: class = "Builder         "; break;
	case MAX_LEVEL -  6:
	case MAX_LEVEL -  7:
	case MAX_LEVEL -  8:
	case MAX_LEVEL -  9:
	             if (wch->prefix != NULL) class = wch->prefix;
		else if (wch->race <= 0 ) class = "#1Piss Ant#n        ";
		else if (wch->race <= 3 ) class = "#2Squire#n          ";
		else if (wch->race <= 6 ) class = "#3Apprentice#n      ";
		else if (wch->race <= 9 ) class = "#4Scout#n           ";
		else if (wch->race <= 12) class = "#5Soldier#n         ";
		else if (wch->race <= 15) class = "#6Lieutenant#n      ";
	        else if (wch->race <= 18) class = "#7Captain#n         ";
		else if (wch->race <= 21) class = "#0Guard#n           ";
		else if (wch->race <= 24) class = "#1Kni#2ght#n          ";
		else if (wch->race <= 27) class = "#3Ran#4ger#n          ";
		else if (wch->race <= 30) class = "#5Hun#6ter#n          ";
		else if (wch->race <= 33) class = "#7Ass#0assin#n        ";
		else if (wch->race <= 36) class = "#4L#1o#5rd#n            ";
		else if (wch->race <= 39) class = "#1B#6ar#7on#n           ";
		else if (wch->race <= 42) class = "#5D#7uk#1e#n            ";
		else if (wch->race <= 45) class = "#0Pr#2in#5ce#n          ";
		else if (wch->race <= 50) class = "#6K#0i#7n#2g#n            ";
		break;

	case MAX_LEVEL - 10: class = "Mortal          "; break;
	case MAX_LEVEL - 11: class = "Newbie          "; break;
	case MAX_LEVEL - 12: class = "#7Undefined#n       "; break;
	}
	/*
	 * Format it up.
	 */
	if ( IS_CLASS(wch, CLASS_VAMPIRE))
	{ strcpy( openb, "<<" ); strcpy( closeb, ">>" ); }
	else if ( IS_CLASS(wch, CLASS_WEREWOLF))
	{ strcpy( openb, "(" ); strcpy( closeb, ")" ); }
	else if ( IS_CLASS(wch,CLASS_MAGE))
	{ strcpy( openb, "{" ); strcpy( closeb, "}" ); }
	else if ( IS_CLASS(wch, CLASS_HIGHLANDER))
	{ strcpy( openb, "-=" ); strcpy( closeb, "=-" ); }
	else if ( IS_CLASS(wch, CLASS_DROW))
	{ strcpy( openb, ".o0"); strcpy(closeb, "0o." ); }
        else if ( IS_CLASS(wch, CLASS_ZERG))
        { strcpy( openb, ".)|("); strcpy(closeb, ")|(." ); }
        else if ( IS_CLASS(wch, CLASS_NINJA))
        { strcpy( openb, "***"); strcpy(closeb, "***" ); }
	else if (IS_CLASS(wch, CLASS_MONK))
	{ strcpy(openb, ".x["); strcpy(closeb, "]x."); }
	else
	{ strcpy( openb, "[" ); strcpy( closeb, "]" ); }
	if ( ch->class > 0 || IS_IMMORTAL( ch ))
	  {
           
	    if ( IS_CLASS(wch, CLASS_DEMON))
	        sprintf( kav, " %sDemon%s",openb,closeb);
	    else if ( IS_CLASS(wch, CLASS_MONK) && wch->generation <= 1)
	        sprintf( kav, " %sMaster Monk%s", openb, closeb);
	    else if ( IS_CLASS(wch, CLASS_VAMPIRE) && wch->generation == 1 )
	    	sprintf( kav, " %sMaster Vampire%s", openb, closeb );
	    else if (IS_CLASS(wch, CLASS_WEREWOLF) && wch->generation == 1 )
	    	sprintf( kav, " %sMaster Werewolf%s", openb, closeb );
	    else if (IS_CLASS(wch, CLASS_DROW) && wch->generation == 1 )
		sprintf( kav, " %sLloth's Avatar%s", openb, closeb );
            else if (IS_CLASS(wch, CLASS_ZERG) && wch->generation == 1 )
                sprintf( kav, " %sXkilla's Zerg%s", openb, closeb );
	    else if ( IS_CLASS(wch, CLASS_VAMPIRE))
	    {
		bool get_age = FALSE;

		get_age = TRUE;

		if (get_age)
		{
		    if (wch->generation == 2)
			sprintf( kav, " %sVampire Antediluvian%s", openb, closeb );
			else if (IS_EXTRA(wch, SPC_INCONNU))
	/*	if ( IS_EXTRA(wch, EXTRA_AFK) )*/
		sprintf( kav, " %sInconnu Vampire%s", openb, closeb );
			
		else switch ( wch->pcdata->rank )
		    {
		    default:
			sprintf( kav, " %sVampire Childe%s", openb,
closeb );
			break;
		    case AGE_NEONATE:
			sprintf( kav, " %sVampire Neonate%s", openb,
closeb );
			break;
		    case AGE_ANCILLA:
			sprintf( kav, " %sVampire Ancilla%s", openb,
closeb );
			break;
		    case AGE_ELDER:
			sprintf( kav, " %sVampire Elder%s", openb,
closeb );
			break;
		    case AGE_METHUSELAH:
			sprintf( kav, " %sVampire Methuselah%s",
openb, closeb );
			break;
		    }
		}
	    }

	    else if ( IS_CLASS(wch, CLASS_WEREWOLF))
	    {
		if (!IS_SET(ch->extra,EXTRA_BSD))
                   sprintf( kav, " %sBlack Spiral Dancer%s", openb, closeb );
	    	if (wch->generation == 2)
	    	    sprintf( kav, " %sChieftain%s", openb, closeb );
	    	else
	    	    sprintf( kav, " %sWerewolf%s", openb, closeb );
	    }
	    
	else if ( IS_CLASS(wch, CLASS_MONK))
	    {
		if (wch->generation == 2)
		        sprintf(kav," %sAbbot%s", openb, closeb);
		else if (wch->generation > 2)
			sprintf(kav," %sMonk%s", openb, closeb);
	    }
	    else if ( IS_CLASS(wch, CLASS_DROW)) {
		if (wch->generation == 2)
			sprintf(kav," %sDrow Matron%s", openb,closeb );
		else if (IS_SET(wch->special, SPC_DROW_WAR))
			sprintf(kav, " %sDrow Warrior%s", openb, closeb );
		else if (IS_SET(wch->special, SPC_DROW_CLE))
			sprintf(kav, " %sDrow Cleric%s", openb, closeb );
		else if (IS_SET(wch->special, SPC_DROW_MAG))
			sprintf(kav, " %sDrow Mage%s", openb, closeb );
		else sprintf(kav, " %sDrow%s", openb, closeb);
		}
	    else if ( IS_CLASS(wch, CLASS_MAGE) )
	    {
            sprintf( kav, "%sMage%s", openb,closeb);
/*		mage_col = str_dup(wch->clan);
		     if (wch->level == LEVEL_APPRENTICE)
		    sprintf( kav, " %s%s Apprentice%s", openb, mage_col, closeb );
		else if (wch->level == LEVEL_MAGE)
		    sprintf( kav, " %s%s Mage%s", openb, mage_col, closeb );
		else if (wch->level == LEVEL_ARCHMAGE)
		    sprintf( kav, " %s%s Archmage%s", openb, mage_col,
closeb);*/
	    }
	    else if ( IS_CLASS(wch, CLASS_ZERG))
	    {
		if (wch->pcdata->stats[UNI_GEN] == 2)
		        sprintf(kav," %sZerg%s", openb, closeb);
		else if (wch->pcdata->stats[UNI_GEN] > 2)
			sprintf(kav," %sZerg%s", openb, closeb);
	    }
	    else if ( IS_CLASS(wch, CLASS_HIGHLANDER) )
	    {
		if (wch->generation == 1)
		sprintf(kav, " %sHighlander%s",openb,closeb);
		else if (wch->generation == 2)
		sprintf(kav," %sHighlander%s",openb,closeb);
		else if (wch->generation == 3)
		sprintf(kav," %sHighlander%s",openb,closeb);
		else
		sprintf( kav, " %sHighlander%s", openb, closeb);
	    }
            else if ( IS_CLASS(wch, CLASS_NINJA) )
                sprintf( kav, " %sNinja%s", openb, closeb);
	    else
	      sprintf( kav, " None" );
	}

	else
	    sprintf( kav, " %sNone%s" ,openb,closeb);


            if (IS_CLASS(wch,CLASS_VAMPIRE))
sprintf(cls,"Vampire");
      else  if (IS_CLASS(wch,CLASS_WEREWOLF))
sprintf(cls,"Werewolf");
      else  if (IS_CLASS(wch,CLASS_DEMON)) 	sprintf(cls,"Demon");
      else  if (IS_CLASS(wch,CLASS_DROW)) 	sprintf(cls,"Drow");
      else  if (IS_CLASS(wch,CLASS_ZERG))	sprintf(cls,"Zerg");
      else  if (IS_CLASS(wch,CLASS_MAGE)) 	sprintf(cls,"Mage");
      else 					sprintf(cls,"None");

      sprintf( king, "%s", king_table[wch->pcdata->kingdom].who_name);
      if ( IS_SET(wch->special, SPC_PRINCE) )
        sprintf( king, "Prince of %s", king_table[wch->pcdata->kingdom].who_name);
      else if ( !strcmp(wch->pcdata->switchname,
king_table[wch->pcdata->kingdom].leader_name) )
        sprintf( king, "Leader of %s", king_table[wch->pcdata->kingdom].who_name );
      else if ( !strcmp(wch->pcdata->switchname, "Zelucifur"))
        sprintf( king, "Jester of %s",king_table[wch->pcdata->kingdom].who_name);
	if ( wch->level > 6 )
        {
	sprintf( buf3 + strlen(buf3), "%-16s %-28s%-12s %s\n\r",
	    class,
	    kav,
	    wch->pcdata->switchname,
	    king);
	  imm = TRUE;
	}    
 
	else if ( wch->level >= 3 && wch->level <= 6 )
        {
          if ( wch->race >= 43 )
          {
            sprintf( buf4 + strlen(buf4), "%-16s %-28s%-12s %s\n\r",
              class,
              kav,
              wch->pcdata->switchname,
              king);
		a16 = TRUE;
          }
          else if ( wch->race >= 40 && wch->race <= 42 )
          {
            sprintf( buf6 + strlen(buf6), "%-16s %-28s%-12s %s\n\r",
              class,
              kav,
              wch->pcdata->switchname,
              king);
		a15 = TRUE;
          }
          else if ( wch->race >= 37 && wch->race <= 39 )
          {
            sprintf( buf7 + strlen(buf7), "%-16s %-28s%-12s %s\n\r",
              class,
              kav,
              wch->pcdata->switchname,
              king);
		a14 = TRUE;
          }
          else if ( wch->race >= 34 && wch->race <= 36 )
          {
            sprintf( buf8 + strlen(buf8), "%-16s %-28s%-12s %s\n\r",
              class,
              kav,
              wch->pcdata->switchname,
              king);
		a13 = TRUE;
          }
          else if ( wch->race >= 31 && wch->race <= 33 )
          {
            sprintf( buf9 + strlen(buf9), "%-16s %-28s%-12s %s\n\r",
              class,
              kav,
              wch->pcdata->switchname,
              king);
		a12 = TRUE;
          }
          else if ( wch->race >= 28 && wch->race <= 30 )
          {
            sprintf( buf10 + strlen(buf10), "%-16s %-28s%-12s %s\n\r",
              class,
              kav,
              wch->pcdata->switchname,
              king);
		a11 = TRUE;
          }
          else if ( wch->race >= 25 && wch->race <= 27 )
          {
            sprintf( buf11 + strlen(buf11), "%-16s %-28s%-12s %s\n\r",
              class,
              kav,
              wch->pcdata->switchname,
              king);
		a10 = TRUE;
          }
          else if ( wch->race >= 22 && wch->race <= 24 )
          {
            sprintf( buf12 + strlen(buf12), "%-16s %-28s%-12s %s\n\r",
              class,
              kav,
              wch->pcdata->switchname,
              king);
		a9 = TRUE;
          }
          else if ( wch->race >= 19 && wch->race <= 21 )
          {
            sprintf( buf13 + strlen(buf13), "%-16s %-28s%-12s %s\n\r",
              class,
              kav,
              wch->pcdata->switchname,
              king);
		a8 = TRUE;
          }
          else if ( wch->race >= 16 && wch->race <= 18 )
          {
            sprintf( buf14 + strlen(buf14), "%-16s %-28s%-12s %s\n\r",
              class,
              kav,
              wch->pcdata->switchname,
              king);
		a7 = TRUE;
          }
          else if ( wch->race >= 13 && wch->race <= 15 )
          {
            sprintf( buf15 + strlen(buf15), "%-16s %-28s%-12s %s\n\r",
              class,
              kav,
              wch->pcdata->switchname,
              king);
		a6 = TRUE;
          }
          else if ( wch->race >= 10 && wch->race <= 12 )
          {
            sprintf( buf16 + strlen(buf16), "%-16s %-28s%-12s %s\n\r",
              class,
              kav,
              wch->pcdata->switchname,
              king);
		a5 = TRUE;
          }
          else if ( wch->race >= 7 && wch->race <= 9 )
          {
            sprintf( buf17 + strlen(buf17), "%-16s %-28s%-12s %s\n\r",
              class,
              kav,
              wch->pcdata->switchname,
              king);
	    a4 = TRUE;
          }
          else if ( wch->race >= 4 && wch->race <= 6 )
          {
            sprintf( buf18 + strlen(buf18), "%-16s %-28s%-12s %s\n\r",
              class,
              kav,
              wch->pcdata->switchname,
              king);
	    a3 = TRUE;
          }
          else if ( wch->race >= 1 && wch->race <= 3 )
          {
            sprintf( buf19 + strlen(buf19), "%-16s %-28s%-12s %s\n\r",
              class,
              kav,
              wch->pcdata->switchname,
              king);
	    a2 = TRUE;
          }
          else if ( wch->race == 0 )
          {
            sprintf( buf20 + strlen(buf20), "%-16s %-28s%-12s %s\n\r",
              class,
              kav,
              wch->pcdata->switchname,
              king);
            a1 = TRUE;
          }
	  ava = TRUE;
        }

        else if ( wch->level < 3 )
        {
        sprintf( buf5 + strlen(buf5), "%-16s %-28s%-12s %s\n\r",
            class,
            kav,
            wch->pcdata->switchname,
            king);
          mor = TRUE;
        }

   if ( !main )
   {
      sprintf(buf2,"#2The Cronic#n"); 
      banner_to_char(buf2,ch);      
      main = TRUE;
   }
     
  }  

    if ( imm )
    {
      if ( !immortal )
      { stc("\n\r",ch);banner2_to_char("#1Admins#n",ch); immortal = TRUE; }
      stc(buf3,ch);
    }

    if ( ava )
    {
      if ( immortal ) stc("\n\r",ch);
      if ( !avatar )
      { banner2_to_char("#3Cronicles#n",ch); avatar = TRUE; }
      if ( a16 ) stc(buf4, ch);
      if ( a15 ) stc(buf6, ch);
      if ( a14 ) stc(buf7, ch);
      if ( a13 ) stc(buf8, ch);
      if ( a12 ) stc(buf9, ch);
      if ( a11 ) stc(buf10,ch);
      if ( a10 ) stc(buf11,ch);
      if ( a9  ) stc(buf12,ch);
      if ( a8  ) stc(buf13,ch);
      if ( a7  ) stc(buf14,ch);
      if ( a6  ) stc(buf15,ch);
      if ( a5  ) stc(buf16,ch);
      if ( a4  ) stc(buf17,ch);
      if ( a3  ) stc(buf18,ch);
      if ( a2  ) stc(buf19,ch);
      if ( a1  ) stc(buf20,ch);
    }
 
    if ( mor )
    {
      if ( avatar ) stc("\n\r",ch);
      if ( immortal && !avatar ) stc("\n\r",ch);
      if ( !mortal )
      { banner2_to_char("#4Newbies#n",ch); mortal = TRUE; }
      stc(buf5,ch);
    }

     
    stc("\n\r",ch);
    sprintf(buf2," Total Players: %d ",nMatch);
    banner_to_char(buf2,ch);
    return;
 
}



void do_inventory( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_INPUT_LENGTH];
    OBJ_DATA *obj;
    OBJ_DATA *portal;
    OBJ_DATA *portal_next;
    ROOM_INDEX_DATA *pRoomIndex;
    ROOM_INDEX_DATA *location;
    bool found;

    if (!IS_NPC(ch) && IS_HEAD(ch,LOST_HEAD))
        {send_to_char( "You are not a container.\n\r", ch ); return;}
    else if (!IS_NPC(ch) && IS_EXTRA(ch,EXTRA_OSWITCH))
    {
        if ( !IS_NPC(ch) && (obj = ch->pcdata->chobj) == NULL)
	    {send_to_char( "You are not a container.\n\r", ch ); return;}
	switch ( obj->item_type )
	{
	default:
	    send_to_char( "You are not a container.\n\r", ch );
	    break;

	case ITEM_PORTAL:
	    pRoomIndex = get_room_index(obj->value[0]);
	    location = ch->in_room;
	    if ( pRoomIndex == NULL )
	    {
		send_to_char( "You don't seem to lead anywhere.\n\r", ch );
		return;
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
		    if (IS_AFFECTED(ch, AFF_SHADOWPLANE) &&
			!IS_SET(portal->extra_flags, ITEM_SHADOWPLANE) )
		    {
			REMOVE_BIT(ch->affected_by, AFF_SHADOWPLANE);
	    		do_look(ch,"auto");
			SET_BIT(ch->affected_by, AFF_SHADOWPLANE);
			break;
		    }
		    else if (!IS_AFFECTED(ch, AFF_SHADOWPLANE) &&
			IS_SET(portal->extra_flags, ITEM_SHADOWPLANE) )
		    {
			SET_BIT(ch->affected_by, AFF_SHADOWPLANE);
	    		do_look(ch,"auto");
			REMOVE_BIT(ch->affected_by, AFF_SHADOWPLANE);
			break;
		    }
		    else
		    {
	    		do_look(ch,"auto");
			break;
		    }
	    	}
	    }
	    char_from_room(ch);
	    char_to_room(ch,location);
	    break;

	case ITEM_DRINK_CON:
	    if ( obj->value[1] <= 0 )
	    {
		send_to_char( "You are empty.\n\r", ch );
		break;
	    }
	    if (obj->value[1] < obj->value[0] / 5)
	    	sprintf( buf, "There is a little %s liquid left in you.\n\r",liq_table[obj->value[2]].liq_color);
	    else if (obj->value[1] < obj->value[0] / 4)
	    	sprintf( buf, "You contain a small about of %s liquid.\n\r",liq_table[obj->value[2]].liq_color);
	    else if (obj->value[1] < obj->value[0] / 3)
	    	sprintf( buf, "You're about a third full of %s liquid.\n\r",liq_table[obj->value[2]].liq_color);
	    else if (obj->value[1] < obj->value[0] / 2)
	    	sprintf( buf, "You're about half full of %s liquid.\n\r",liq_table[obj->value[2]].liq_color);
	    else if (obj->value[1] < obj->value[0])
	    	sprintf( buf, "You are almost full of %s liquid.\n\r",liq_table[obj->value[2]].liq_color);
	    else if (obj->value[1] == obj->value[0])
	    	sprintf( buf, "You're completely full of %s liquid.\n\r",liq_table[obj->value[2]].liq_color);
	    else
	    	sprintf( buf, "Somehow you are MORE than full of %s liquid.\n\r",liq_table[obj->value[2]].liq_color);
	    send_to_char( buf, ch );
	    break;

	case ITEM_CONTAINER:
	case ITEM_CORPSE_NPC:
	case ITEM_CORPSE_PC:
	    act( "$p contain:", ch, obj, NULL, TO_CHAR );
	    show_list_to_char( obj->contains, ch, TRUE, TRUE );
	    break;
	}
	return;
    }
    send_to_char( "You are carrying:\n\r", ch );
    show_list_to_char( ch->carrying, ch, TRUE, TRUE );
    return;
}



void do_equipment( CHAR_DATA *ch, char *argument )
{
    OBJ_DATA *obj;
    int iWear;
    bool found;

    send_to_char( "You are using:\n\r", ch );
    found = FALSE;
    for ( iWear = 0; iWear < MAX_WEAR; iWear++ )
    {
	if ( ( obj = get_eq_char( ch, iWear ) ) == NULL )
	    continue;

	send_to_char( where_name[iWear], ch );
	if ( can_see_obj( ch, obj ) )
	{
	    send_to_char( format_obj_to_char( obj, ch, TRUE ), ch );
	    send_to_char( "\n\r", ch );
	}
	else
	{
	    send_to_char( "something.\n\r", ch );
	}
	found = TRUE;
    }

    if ( !found )
	send_to_char( "Nothing.\n\r", ch );

    return;
}



void do_compare( CHAR_DATA *ch, char *argument )
{
    char arg1[MAX_INPUT_LENGTH];
    char arg2[MAX_INPUT_LENGTH];
    OBJ_DATA *obj1;
    OBJ_DATA *obj2;
    int value1;
    int value2;
    char *msg;

    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );
    if ( arg1[0] == '\0' )
    {
	send_to_char( "Compare what to what?\n\r", ch );
	return;
    }

    if ( ( obj1 = get_obj_carry( ch, arg1 ) ) == NULL )
    {
	send_to_char( "You do not have that item.\n\r", ch );
	return;
    }

    if ( arg2[0] == '\0' )
    {
	for ( obj2 = ch->carrying; obj2 != NULL; obj2 = obj2->next_content )
	{
	    if ( obj2->wear_loc != WEAR_NONE
	    &&   can_see_obj( ch, obj2 )
	    &&   obj1->item_type == obj2->item_type
	    && ( obj1->wear_flags & obj2->wear_flags & ~ITEM_TAKE) != 0 )
		break;
	}

	if ( obj2 == NULL )
	{
	    send_to_char( "You aren't wearing anything comparable.\n\r", ch );
	    return;
	}
    }
    else
    {
	if ( ( obj2 = get_obj_carry( ch, arg2 ) ) == NULL )
	{
	    send_to_char( "You do not have that item.\n\r", ch );
	    return;
	}
    }
	    
    msg		= NULL;
    value1	= 0;
    value2	= 0;

    if ( obj1 == obj2 )
    {
	msg = "You compare $p to itself.  It looks about the same.";
    }
    else if ( obj1->item_type != obj2->item_type )
    {
	msg = "You can't compare $p and $P.";
    }
    else
    {
	switch ( obj1->item_type )
	{
	default:
	    msg = "You can't compare $p and $P.";
	    break;

	case ITEM_ARMOR:
	    value1 = obj1->value[0];
	    value2 = obj2->value[0];
	    break;

	case ITEM_WEAPON:
	    value1 = obj1->value[1] + obj1->value[2];
	    value2 = obj2->value[1] + obj2->value[2];
	    break;
	}
    }

    if ( msg == NULL )
    {
	     if ( value1 == value2 ) msg = "$p and $P look about the same.";
	else if ( value1  > value2 ) msg = "$p looks better than $P.";
	else                         msg = "$p looks worse than $P.";
    }

    act( msg, ch, obj1, obj2, TO_CHAR );
    return;
}



void do_credits( CHAR_DATA *ch, char *argument )
{
    do_help( ch, "diku" );
    return;
}



void do_wizlist( CHAR_DATA *ch, char *argument )
{
    do_help( ch, "wizlist" );
    return;
}



void do_where( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_STRING_LENGTH];
    char arg[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;
    DESCRIPTOR_DATA *d;
    bool found;

    one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
	send_to_char( "Players near you:\n\r", ch );
	found = FALSE;
	for ( d = descriptor_list; d != NULL; d = d->next )
	{
	    if ( ( d->connected == CON_PLAYING
	    ||   d->connected == CON_EDITING)
	    && ( victim = d->character ) != NULL
	    &&   !IS_NPC(victim)
	    &&   victim->in_room != NULL
	    &&   victim->in_room->area == ch->in_room->area
	    &&   victim->pcdata->chobj == NULL
	    &&   can_see( ch, victim ) )
	    {
		found = TRUE;
		sprintf( buf, "%-28s %s\n\r",
		    victim->name, victim->in_room->name );
		send_to_char( buf, ch );
	    }
	}
	if ( !found )
	    send_to_char( "None\n\r", ch );
    }
    else
    {
	found = FALSE;
	for ( victim = char_list; victim != NULL; victim = victim->next )
	{
	    if ( victim->in_room != NULL
	    &&   victim->in_room->area == ch->in_room->area
	    &&   !IS_AFFECTED(victim, AFF_HIDE)
	    &&   !IS_AFFECTED(victim, AFF_SNEAK)
	    &&   can_see( ch, victim )
	    &&   is_name( arg, victim->name ) )
	    {
		found = TRUE;
		sprintf( buf, "%-28s %s\n\r",
		    PERS(victim, ch), victim->in_room->name );
		send_to_char( buf, ch );
		break;
	    }
	}
	if ( !found )
	    act( "You didn't find any $T.", ch, NULL, arg, TO_CHAR );
    }

    return;
}




void do_consider( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;
    char *msg;
    int diff;
    int overall;
    int con_hit;
    int con_dam;
    int con_ac;
    int con_hp;

    one_argument( argument, arg );
    overall = 0;

    if ( arg[0] == '\0' )
    {
	send_to_char( "Consider killing whom?\n\r", ch );
	return;
    }

    if ( ( victim = get_char_room( ch, arg ) ) == NULL )
    {
	send_to_char( "They're not here.\n\r", ch );
	return;
    }

    act( "You examine $N closely, looking for $S weaknesses.", ch, NULL, victim, TO_CHAR );
    act( "$n examine $N closely, looking for $S weaknesses.", ch, NULL, victim, TO_NOTVICT );
    act( "$n examines you closely, looking for your weaknesses.", ch, NULL, victim, TO_VICT );

    if (!IS_NPC(victim)) do_skill(ch,victim->name);

    if (!IS_NPC(victim) && IS_CLASS(victim, CLASS_VAMPIRE) && 
	IS_EXTRA(victim, EXTRA_FAKE_CON))
    {
	con_hit = victim->pcdata->fake_hit;
	con_dam = victim->pcdata->fake_dam;
	con_ac = victim->pcdata->fake_ac;
	con_hp = victim->pcdata->fake_hp;
    }
    else
    {
	con_hit = char_hitroll(victim);
	con_dam = char_damroll(victim);
	con_ac = char_ac(victim);
	con_hp = victim->hit;
    }
    if (con_hp < 1) con_hp = 1;

    diff = victim->level - ch->level + con_hit - char_hitroll(ch);
         if ( diff <= -35 ) {msg = "You are FAR more skilled than $M."; overall = overall + 3;}
    else if ( diff <= -15 ) {msg = "$E is not as skilled as you are."; overall = overall + 2;}
    else if ( diff <=  -5 ) {msg = "$E doesn't seem quite as skilled as you."; overall = overall + 1;}
    else if ( diff <=   5 ) {msg = "You are about as skilled as $M.";}
    else if ( diff <=  15 ) {msg = "$E is slightly more skilled than you are."; overall = overall - 1;}
    else if ( diff <=  35 ) {msg = "$E seems more skilled than you are."; overall = overall -2;}
    else                    {msg = "$E is FAR more skilled than you."; overall = overall - 3;}
    act( msg, ch, NULL, victim, TO_CHAR );

    diff = victim->level - ch->level + con_dam - char_damroll(ch);
         if ( diff <= -35 ) {msg = "You are FAR more powerful than $M."; overall = overall + 3;}
    else if ( diff <= -15 ) {msg = "$E is not as powerful as you are."; overall = overall + 2;}
    else if ( diff <=  -5 ) {msg = "$E doesn't seem quite as powerful as you."; overall = overall + 1;}
    else if ( diff <=   5 ) {msg = "You are about as powerful as $M.";}
    else if ( diff <=  15 ) {msg = "$E is slightly more powerful than you are."; overall = overall - 1;}
    else if ( diff <=  35 ) {msg = "$E seems more powerful than you are."; overall = overall -2;}
    else                    {msg = "$E is FAR more powerful than you."; overall = overall - 3;}
    act( msg, ch, NULL, victim, TO_CHAR );

    diff = ch->hit * 100 / con_hp;
         if ( diff <=  10 ) {msg = "$E is currently FAR healthier than you are."; overall = overall - 3;}
    else if ( diff <=  50 ) {msg = "$E is currently much healthier than you are."; overall = overall - 2;}
    else if ( diff <=  75 ) {msg = "$E is currently slightly healthier than you are."; overall = overall - 1;}
    else if ( diff <= 125 ) {msg = "$E is currently about as healthy as you are.";}
    else if ( diff <= 200 ) {msg = "You are currently slightly healthier than $M."; overall = overall + 1;}
    else if ( diff <= 500 ) {msg = "You are currently much healthier than $M."; overall = overall + 2;}
    else                    {msg = "You are currently FAR healthier than $M."; overall = overall + 3;}
    act( msg, ch, NULL, victim, TO_CHAR );

    diff = con_ac - char_ac(ch);
         if ( diff <= -100) {msg = "$E is FAR better armoured than you."; overall = overall - 3;}
    else if ( diff <= -50 ) {msg = "$E looks much better armoured than you."; overall = overall - 2;}
    else if ( diff <= -25 ) {msg = "$E looks better armoured than you."; overall = overall - 1;}
    else if ( diff <=  25 ) {msg = "$E seems about as well armoured as you.";}
    else if ( diff <=  50 ) {msg = "You are better armoured than $M."; overall = overall + 1;}
    else if ( diff <=  100) {msg = "You are much better armoured than $M."; overall = overall + 2;}
    else                    {msg = "You are FAR better armoured than $M."; overall = overall + 3;}
    act( msg, ch, NULL, victim, TO_CHAR );

    diff = overall;
         if ( diff <= -11 ) msg = "Conclusion: $E would kill you in seconds.";
    else if ( diff <=  -7 ) msg = "Conclusion: You would need a lot of luck to beat $M.";
    else if ( diff <=  -3 ) msg = "Conclusion: You would need some luck to beat $N.";
    else if ( diff <=   2 ) msg = "Conclusion: It would be a very close fight.";
    else if ( diff <=   6 ) msg = "Conclusion: You shouldn't have a lot of trouble defeating $M.";
    else if ( diff <=  10 ) msg = "Conclusion: $N is no match for you.  You can easily beat $M.";
    else                    msg = "Conclusion: $E wouldn't last more than a few seconds against you.";
    act( msg, ch, NULL, victim, TO_CHAR );

    return;
}


void set_prefix( CHAR_DATA *ch, char *title )
{
    char buf[MAX_STRING_LENGTH];

    if ( IS_NPC(ch) )
    {
	bug( "Set_title: NPC.", 0 );
	return;
    }

    if ( isalpha(title[0]) || isdigit(title[0]) )
    {
	buf[0] = ' ';
	strcpy( buf+1, title );
    }
    else
    {
	strcpy( buf, title );
    }

    free_string( ch->prefix );
    ch->prefix = str_dup( buf );
    return;
}



void do_prefix( CHAR_DATA *ch, char *argument )
{
 
    char buf[MAX_STRING_LENGTH];

    if ( IS_NPC(ch) )
	return;

    if ( argument[0] == '\0' )
    {
	send_to_char( "Change your prefix to what?\n\r", ch );
	return;
    }

    if ( strlen(argument) > 13 )
	argument[13] = '\0';

    smash_tilde( argument );
    sprintf(buf,"#n");
    strcpy(buf,argument);
    set_prefix( ch, argument );
    send_to_char( "Prefix Set.\n\r", ch );
}

void set_title( CHAR_DATA *ch, char *title )
{
    char buf[MAX_STRING_LENGTH];

    if ( IS_NPC(ch) )
    {
	bug( "Set_title: NPC.", 0 );
	return;
    }

    if ( isalpha(title[0]) || isdigit(title[0]) )
    {
	buf[0] = ' ';
	strcpy( buf+1, title );
    }
    else
    {
	strcpy( buf, title );
    }

    free_string( ch->pcdata->title );
    ch->pcdata->title = str_dup( buf );
    return;
}



void do_title( CHAR_DATA *ch, char *argument )
{
 
    char buf[MAX_STRING_LENGTH];

    if ( IS_NPC(ch) )
	return;

    if ( argument[0] == '\0' )
    {
	send_to_char( "Change your title to what?\n\r", ch );
	return;
    }

    if ( strlen(argument) > 17 )
	argument[17] = '\0';

    smash_tilde( argument );
    sprintf(buf,"#n");
    strcpy(buf,argument);
    set_title( ch, argument );
    send_to_char( "Ok.\n\r", ch );
}

void do_afk( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_STRING_LENGTH];

	if ( IS_NPC(ch) )
	return;

        if (IS_SET(ch->extra, EXTRA_AFK))
	{
	free_string( ch->pcdata->title);
	ch->pcdata->title = str_dup( ch->short_descr );
	free_string( ch->short_descr );
	ch->short_descr = NULL;
	send_to_char( "You are no longer AFK.\n\r", ch );
	sprintf(buf,"%s is no longer AFK!",ch->pcdata->switchname);
        ADD_COLOUR(ch,buf,L_RED);
	do_info(ch,buf);
	REMOVE_BIT(ch->extra,EXTRA_AFK);
        return;
	}
	
	else if (!IS_SET(ch->extra,EXTRA_AFK))
	{
	free_string( ch->short_descr );
	ch->short_descr = str_dup( ch->pcdata->title );
	free_string( ch->pcdata->title );
	ch->pcdata->title = str_dup("#n(#1AFK#n)");
	send_to_char( "You are now AFK.\n\r", ch );
	sprintf(buf,"%s is now AFK!",ch->pcdata->switchname);
	ADD_COLOUR(ch,buf,L_RED);
	do_info(ch,buf);
	SET_BIT(ch->extra,EXTRA_AFK);
	WAIT_STATE(ch, 25);
        return;
	}

else
return;

}

void do_email( CHAR_DATA *ch, char *argument )
{
    if ( IS_NPC(ch) )
	return;

    if ( argument[0] == '\0' )
    {
	send_to_char( "What do you wish to set your email address to?\n\r", ch );
	return;
    }

    if ( strlen(argument) > 50 )
	argument[50] = '\0';

    smash_tilde( argument );
    free_string( ch->pcdata->email );
    ch->pcdata->email = str_dup( argument );
    send_to_char( "Ok.\n\r", ch );
}



void do_description( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_STRING_LENGTH];

    if ( argument[0] != '\0' )
    {
	buf[0] = '\0';
	smash_tilde( argument );
	if ( argument[0] == '+' )
	{
	    if ( ch->description != NULL )
		strcat( buf, ch->description );
	    argument++;
	    while ( isspace(*argument) )
		argument++;
	}

	if ( strlen(buf) + strlen(argument) >= MAX_STRING_LENGTH - 2 )
	{
	    send_to_char( "Description too long.\n\r", ch );
	    return;
	}

	strcat( buf, argument );
	strcat( buf, "\n\r" );
	free_string( ch->description );
	ch->description = str_dup( buf );
    }

    send_to_char( "Your description is:\n\r", ch );
    send_to_char( ch->description ? ch->description : "(None).\n\r", ch );
    return;
}



void do_report( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *vch;
    CHAR_DATA *vch_next;
    char buf[MAX_STRING_LENGTH];
    char hit_str[MAX_INPUT_LENGTH];
    char mana_str[MAX_INPUT_LENGTH];
    char move_str[MAX_INPUT_LENGTH];
    char mhit_str[MAX_INPUT_LENGTH];
    char mmana_str[MAX_INPUT_LENGTH];
    char mmove_str[MAX_INPUT_LENGTH];
    char exp_str[MAX_INPUT_LENGTH];
    sprintf(hit_str, "%d", ch->hit);
    COL_SCALE(hit_str, ch, ch->hit, ch->max_hit);
    sprintf(mana_str, "%d", ch->mana);
    COL_SCALE(mana_str, ch, ch->mana, ch->max_mana);
    sprintf(move_str, "%d", ch->move);
    COL_SCALE(move_str, ch, ch->move, ch->max_move);
    sprintf(exp_str, "%d", ch->exp);
    COL_SCALE(exp_str, ch, ch->exp, 1000);
    sprintf(mhit_str, "%d", ch->max_hit);
    ADD_COLOUR(ch, mhit_str, L_CYAN);
    sprintf(mmana_str, "%d", ch->max_mana);
    ADD_COLOUR(ch, mmana_str, L_CYAN);
    sprintf(mmove_str, "%d", ch->max_move);
    ADD_COLOUR(ch, mmove_str, L_CYAN);
    sprintf( buf,
	"You report: %s/%s hp %s/%s mana %s/%s mv %s xp.\n\r",
	hit_str,  mhit_str,
	mana_str, mmana_str,
	move_str, mmove_str,
	exp_str   );

    send_to_char( buf, ch );

    for ( vch = char_list; vch != NULL; vch = vch_next )
    {
	vch_next	= vch->next;
	if ( vch == NULL ) continue;
	if ( vch == ch ) continue;
	if ( vch->in_room == NULL ) continue;
	if ( vch->in_room != ch->in_room ) continue;
    	sprintf(hit_str, "%d", ch->hit);
    	COL_SCALE(hit_str, vch, ch->hit, ch->max_hit);
    	sprintf(mana_str, "%d", ch->mana);
    	COL_SCALE(mana_str, vch, ch->mana, ch->max_mana);
    	sprintf(move_str, "%d", ch->move);
    	COL_SCALE(move_str, vch, ch->move, ch->max_move);
    	sprintf(exp_str, "%d", ch->exp);
    	COL_SCALE(exp_str, vch, ch->exp, 1000);
    	sprintf(mhit_str, "%d", ch->max_hit);
    	ADD_COLOUR(vch, mhit_str, L_CYAN);
    	sprintf(mmana_str, "%d", ch->max_mana);
    	ADD_COLOUR(vch, mmana_str, L_CYAN);
    	sprintf(mmove_str, "%d", ch->max_move);
    	ADD_COLOUR(vch, mmove_str, L_CYAN);
    	if (!IS_NPC(ch) && IS_AFFECTED(ch,AFF_POLYMORPH))
    	    sprintf( buf, "%s reports: %s/%s hp %s/%s mana %s/%s mv %s xp.\n\r",
	    ch->morph,
	    hit_str,  mhit_str,
	    mana_str, mmana_str,
	    move_str, mmove_str,
	    exp_str   );
        else
    	    sprintf( buf, "%s reports: %s/%s hp %s/%s mana %s/%s mv %s xp.\n\r",
	    IS_NPC(ch) ? capitalize(ch->short_descr) : ch->name,
	    hit_str,  mhit_str,
	    mana_str, mmana_str,
	    move_str, mmove_str,
	    exp_str   );
	buf[0] = UPPER(buf[0]);
    	send_to_char( buf, vch );
    }
    return;
}



void do_practice( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_STRING_LENGTH];
    int sn;

    if ( IS_NPC(ch) )
	return;

    if ( argument[0] == '\0' )
    {
	int col;

	col    = 0;
	for ( sn = 0; sn < MAX_SKILL; sn++ )
	{
	    if ( skill_table[sn].name == NULL )
		break;
	    if ( ch->level < skill_table[sn].skill_level )
		continue;
	    sprintf( buf, "%18s %3d%%  ",
		skill_table[sn].name, ch->pcdata->learned[sn] );
	    send_to_char( buf, ch );
	    if ( ++col % 3 == 0 )
		send_to_char( "\n\r", ch );
	}

	if ( col % 3 != 0 )
	    send_to_char( "\n\r", ch );

	sprintf( buf, "You have %d exp left.\n\r", ch->exp );
	send_to_char( buf, ch );
    }
    else
    {
	if ( !IS_AWAKE(ch) )
	{
	    send_to_char( "In your dreams, or what?\n\r", ch );
	    return;
	}

	if ( ch->exp <= 0 )
	{
	    send_to_char( "You have no exp left.\n\r", ch );
	    return;
	}

	if ( ( sn = skill_lookup( argument ) ) < 0
	|| ( !IS_NPC(ch)
	&&   ch->level < skill_table[sn].skill_level ) )
	{
	    send_to_char( "You can't practice that.\n\r", ch );
	    return;
	}

	
	if ( ch->pcdata->learned[sn] >= SKILL_ADEPT )
	{
	    sprintf( buf, "You are already an adept of %s.\n\r",
	         skill_table[sn].name );
	    send_to_char( buf, ch );
	}
	else if ( ch->pcdata->learned[sn] > 0 &&
		ch->exp < 5000 )
	{
	    sprintf( buf, "You need 5000 exp to increase %s any more.\n\r",
		(skill_table[sn].name));
	    send_to_char( buf, ch );
	}
	else if ( ch->pcdata->learned[sn] == 0 && ch->exp < 5000 )
	{
	    sprintf( buf, "You need 5000 exp to increase %s.\n\r",
		skill_table[sn].name );
	    send_to_char( buf, ch );
	}
	else
	{
	    if (ch->pcdata->learned[sn] == 0)
	    {
	    	ch->exp -= 5000;
	    	ch->pcdata->learned[sn] +=100;
	    }
	    else
	    {
	    	ch->exp -= (ch->pcdata->learned[sn]/2);
	    	ch->pcdata->learned[sn] += int_app[get_curr_int(ch)].learn;
	    }
	    if ( ch->pcdata->learned[sn] < SKILL_ADEPT )
	    {
		act( "You practice $T.",
		    ch, NULL, skill_table[sn].name, TO_CHAR );
	    }
	    else
	    {
		ch->pcdata->learned[sn] = SKILL_ADEPT;
		act( "You are now an adept of $T.",
		    ch, NULL, skill_table[sn].name, TO_CHAR );
	    }
	}
    }
    return;
}



/*
 * 'Wimpy' originally by Dionysos.
 */
void do_wimpy( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_STRING_LENGTH];
    char arg[MAX_INPUT_LENGTH];
    int wimpy;

    one_argument( argument, arg );

    if ( arg[0] == '\0' )
	wimpy = ch->max_hit / 5;
    else
	wimpy = atoi( arg );

    if ( wimpy < 0 )
    {
	send_to_char( "Your courage exceeds your wisdom.\n\r", ch );
	return;
    }

    if ( wimpy > ch->max_hit )
    {
	send_to_char( "Such cowardice ill becomes you.\n\r", ch );
	return;
    }

    ch->wimpy	= wimpy;
    sprintf( buf, "Wimpy set to %d hit points.\n\r", wimpy );
    send_to_char( buf, ch );
    return;
}



void do_password( CHAR_DATA *ch, char *argument )
{
    char arg1[MAX_INPUT_LENGTH];
    char arg2[MAX_INPUT_LENGTH];
    char *pArg;
    char *pwdnew;
    char *p;
    char cEnd;

    if ( IS_NPC(ch) )
	return;

    /*
     * Can't use one_argument here because it smashes case.
     * So we just steal all its code.  Bleagh.
     */
    pArg = arg1;
    while ( isspace(*argument) )
	argument++;

    cEnd = ' ';
    if ( *argument == '\'' || *argument == '"' )
	cEnd = *argument++;

    while ( *argument != '\0' )
    {
	if ( *argument == cEnd )
	{
	    argument++;
	    break;
	}
	*pArg++ = *argument++;
    }
    *pArg = '\0';

    pArg = arg2;
    while ( isspace(*argument) )
	argument++;

    cEnd = ' ';
    if ( *argument == '\'' || *argument == '"' )
	cEnd = *argument++;

    while ( *argument != '\0' )
    {
	if ( *argument == cEnd )
	{
	    argument++;
	    break;
	}
	*pArg++ = *argument++;
    }
    *pArg = '\0';

    if ( arg1[0] == '\0' || arg2[0] == '\0' )
    {
	send_to_char( "Syntax: password <old> <new>.\n\r", ch );
	return;
    }

    if ( strcmp( arg1, ch->pcdata->pwd ) &&
         strcmp( crypt( arg1, ch->pcdata->pwd ), ch->pcdata->pwd ) )
    {
	WAIT_STATE( ch, 40 );
	send_to_char( "Wrong password.  Wait 10 seconds.\n\r", ch );
	return;
    }

    if ( strlen(arg2) < 5 )
    {
	send_to_char(
	    "New password must be at least five characters long.\n\r", ch );
	return;
    }

    /*
     * No tilde allowed because of player file format.
     */
    pwdnew = crypt( arg2, ch->pcdata->switchname );
    for ( p = pwdnew; *p != '\0'; p++ )
    {
	if ( *p == '~' )
	{
	    send_to_char(
		"New password not acceptable, try again.\n\r", ch );
	    return;
	}
    }

    free_string( ch->pcdata->pwd );
    ch->pcdata->pwd = str_dup( pwdnew );
    if (!IS_EXTRA(ch,EXTRA_NEWPASS)) SET_BIT(ch->extra,EXTRA_NEWPASS);
    save_char_obj( ch );
    if (ch->desc != NULL && ch->desc->connected == CON_PLAYING )
	send_to_char( "Ok.\n\r", ch );
    return;
}



void do_socials( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_STRING_LENGTH];
    int iSocial;
    int col;

    col = 0;

    for ( iSocial = 0; social_table[iSocial].name[0] != '\0'; iSocial++ )
    {
	sprintf( buf, "%-12s", social_table[iSocial].name );
	send_to_char( buf, ch );
	if ( ++col % 6 == 0 )
	    send_to_char( "\n\r", ch );
    }
 
    if ( col % 6 != 0 )
	send_to_char( "\n\r", ch );

    return;
}



void do_xsocials( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_STRING_LENGTH];
    int iSocial;
    int col;

    col = 0;

    for ( iSocial = 0; xsocial_table[iSocial].name[0] != '\0'; iSocial++ )
    {
	sprintf( buf, "%-12s", xsocial_table[iSocial].name );
	send_to_char( buf, ch );
	if ( ++col % 6 == 0 )
	    send_to_char( "\n\r", ch );
    }
 
    if ( col % 6 != 0 )
	send_to_char( "\n\r", ch );

    return;
}



void do_spells( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_STRING_LENGTH];
    int sn;
    int col;

    col = 0;
    for ( sn = 0; sn < MAX_SKILL && skill_table[sn].name != NULL; sn++ )
    {
	sprintf( buf, "%-12s", skill_table[sn].name );
	send_to_char( buf, ch );
	if ( ++col % 6 == 0 )
	    send_to_char( "\n\r", ch );
    }

    if ( col % 6 != 0 )
	send_to_char( "\n\r", ch );
    return;
}



/*
 * Contributed by Alander.
 */
void do_commands( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_STRING_LENGTH];
    int cmd;
    int col;
 
    col = 0;
    for ( cmd = 0; cmd_table[cmd].name[0] != '\0'; cmd++ )
    {
        if ( cmd_table[cmd].level == 0
        &&   cmd_table[cmd].level <= get_trust( ch ) )
	{
	    sprintf( buf, "%-12s", cmd_table[cmd].name );
	    send_to_char( buf, ch );
	    if ( ++col % 6 == 0 )
		send_to_char( "\n\r", ch );
	}
    }
 
    if ( col % 6 != 0 )
	send_to_char( "\n\r", ch );
    return;
}



void do_channels( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];

    one_argument( argument, arg );

    if (IS_NPC(ch)) return;
    if ( arg[0] == '\0' )
    {
	if ( !IS_NPC(ch) && IS_SET(ch->act, PLR_SILENCE) )
	{
	    send_to_char( "You are silenced.\n\r", ch );
	    return;
	}

	send_to_char( "Channels:", ch );

/*	send_to_char( !IS_SET(ch->deaf, CHANNEL_AUCTION)
	    ? " +AUCTION"
	    : " -auction",
	    ch );
*/
	send_to_char( !IS_SET(ch->deaf, CHANNEL_CHAT)
	    ? " +CHAT"
	    : " -chat",
	    ch );

#if 0
	send_to_char( !IS_SET(ch->deaf, CHANNEL_HACKER)
	    ? " +HACKER"
	    : " -hacker",
	    ch );
#endif

	if ( IS_IMMORTAL(ch) )
	{
	    send_to_char( !IS_SET(ch->deaf, CHANNEL_IMMTALK)
		? " +IMMTALK"
		: " -immtalk",
		ch );
	}

	send_to_char( !IS_SET(ch->deaf, CHANNEL_MUSIC)
	    ? " +MUSIC"
	    : " -music",
	    ch );

	send_to_char( !IS_SET(ch->deaf, CHANNEL_QUESTION)
	    ? " +QUESTION"
	    : " -question",
	    ch );

	send_to_char( !IS_SET(ch->deaf, CHANNEL_SHOUT)
	    ? " +SHOUT"
	    : " -shout",
	    ch );

	send_to_char( !IS_SET(ch->deaf, CHANNEL_HOWL)
	    ? " +HOWL"
	    : " -howl",
	    ch );

	if (IS_IMMORTAL(ch))
	{
	    send_to_char( !IS_SET(ch->deaf, CHANNEL_LOG)
		? " +LOG"
		: " -log",
		ch );
	}

        if (IS_CLASS( ch, CLASS_MAGE) || IS_IMMORTAL(ch))
        {
            send_to_char( !IS_SET(ch->deaf, CHANNEL_MAGETALK)
                ? " +MAGE"
                : " -mage",
                ch );
        }

        if (IS_CLASS( ch, CLASS_MONK) || IS_IMMORTAL(ch))
        {
            send_to_char( !IS_SET(ch->deaf, CHANNEL_MONK)
                ? " +MONK"
                : " -monk",
                ch );
        }

	if (IS_CLASS(ch, CLASS_DEMON) || IS_IMMORTAL(ch))
	{
	    send_to_char( !IS_SET(ch->deaf, CHANNEL_PRAY)
		? " +PRAY"
		: " -pray",
		ch );
	}

	send_to_char( !IS_SET(ch->deaf, CHANNEL_INFO)
	    ? " +INFO"
	    : " -info",
	    ch );

	if (IS_CLASS(ch, CLASS_VAMPIRE) || IS_IMMORTAL(ch))
	{
	    send_to_char( !IS_SET(ch->deaf, CHANNEL_VAMPTALK)
		? " +VAMP"
		: " -vamp",
		ch );
	}

	if (IS_CLASS(ch, CLASS_HIGHLANDER) || IS_IMMORTAL(ch)) {
		send_to_char( !IS_SET(ch->deaf, CHANNEL_HIGHTALK)
			? " +HIGH"
			: " -high",
			ch ); }

	send_to_char( !IS_SET(ch->deaf, CHANNEL_TELL)
	    ? " +TELL"
	    : " -tell",
	    ch );

	send_to_char( ".\n\r", ch );
    }
    else
    {
	bool fClear;
	int bit;

	     if ( arg[0] == '+' ) fClear = TRUE;
	else if ( arg[0] == '-' ) fClear = FALSE;
	else
	{
	    send_to_char( "Channels -channel or +channel?\n\r", ch );
	    return;
	}

/*	     if ( !str_cmp( arg+1, "auction"  ) ) bit = CHANNEL_AUCTION;*/
        if ( !str_cmp( arg+1, "chat"     ) ) bit = CHANNEL_CHAT;
#if 0
	else if ( !str_cmp( arg+1, "hacker"   ) ) bit = CHANNEL_HACKER;
#endif
	else if ( !str_cmp( arg+1, "immtalk"  ) ) bit = CHANNEL_IMMTALK;
	else if ( !str_cmp( arg+1, "music"    ) ) bit = CHANNEL_MUSIC;
	else if ( !str_cmp( arg+1, "question" ) ) bit = CHANNEL_QUESTION;
	else if ( !str_cmp( arg+1, "shout"    ) ) bit = CHANNEL_SHOUT;
	else if ( !str_cmp( arg+1, "yell"     ) ) bit = CHANNEL_YELL;
	else if ( !str_cmp( arg+1, "howl"     ) ) bit = CHANNEL_HOWL;
	else if (IS_IMMORTAL(ch) && !str_cmp( arg+1, "log") ) bit = CHANNEL_LOG;
        else if ( !str_cmp( arg+1, "monk"     ) ) bit = CHANNEL_MONK;          
        else if ( !str_cmp( arg+1, "mage"     ) ) bit = CHANNEL_MAGETALK;
	else if ((IS_SET(ch->special, SPC_CHAMPION) || (IS_IMMORTAL(ch))) && !str_cmp( arg+1, "pray") ) 
	    bit = CHANNEL_PRAY;
	else if ( !str_cmp( arg+1, "info"     ) ) bit = CHANNEL_INFO;
	else if ( !str_cmp( arg+1, "vamp"     ) ) bit = CHANNEL_VAMPTALK;
	else if ( !str_cmp( arg+1, "tell"     ) ) bit = CHANNEL_TELL;
	else
	{
	    send_to_char( "Set or clear which channel?\n\r", ch );
	    return;
	}

	if ( fClear )
	    REMOVE_BIT (ch->deaf, bit);
	else
	    SET_BIT    (ch->deaf, bit);

	send_to_char( "Ok.\n\r", ch );
    }

    return;
}



/*
 * Contributed by Grodyn.
 */
void do_config( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];

    if ( IS_NPC(ch) )
	return;

    one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
        send_to_char( "[ Keyword  ] Option\n\r", ch );

	send_to_char(  IS_SET(ch->act, PLR_ANSI)
            ? "[+ANSI     ] You have ansi colour on.\n\r"
	    : "[-ansi     ] You have ansi colour off.\n\r"
	    , ch );

	send_to_char(  IS_SET(ch->act, PLR_AUTOEXIT)
            ? "[+AUTOEXIT ] You automatically see exits.\n\r"
	    : "[-autoexit ] You don't automatically see exits.\n\r"
	    , ch );

	send_to_char(  IS_SET(ch->act, PLR_AUTOLOOT)
	    ? "[+AUTOLOOT ] You automatically loot corpses.\n\r"
	    : "[-autoloot ] You don't automatically loot corpses.\n\r"
	    , ch );

	send_to_char(  IS_SET(ch->act, PLR_AUTOSAC)
	    ? "[+AUTOSAC  ] You automatically sacrifice corpses.\n\r"
	    : "[-autosac  ] You don't automatically sacrifice corpses.\n\r"
	    , ch );

	send_to_char(  IS_SET(ch->act, PLR_BLANK)
	    ? "[+BLANK    ] You have a blank line before your prompt.\n\r"
	    : "[-blank    ] You have no blank line before your prompt.\n\r"
	    , ch );

	send_to_char(  IS_SET(ch->act, PLR_BRIEF)
	    ? "[+BRIEF    ] You see brief descriptions.\n\r"
	    : "[-brief    ] You see long descriptions.\n\r"
	    , ch );
         
	send_to_char(  IS_SET(ch->act, PLR_COMBINE)
	    ? "[+COMBINE  ] You see object lists in combined format.\n\r"
	    : "[-combine  ] You see object lists in single format.\n\r"
	    , ch );

	send_to_char(  IS_SET(ch->act, PLR_PROMPT)
	    ? "[+PROMPT   ] You have a prompt.\n\r"
	    : "[-prompt   ] You don't have a prompt.\n\r"
	    , ch );

	send_to_char(  IS_SET(ch->act, PLR_TELNET_GA)
	    ? "[+TELNETGA ] You receive a telnet GA sequence.\n\r"
	    : "[-telnetga ] You don't receive a telnet GA sequence.\n\r"
	    , ch );

	send_to_char(  IS_SET(ch->act, PLR_SILENCE)
	    ? "[+SILENCE  ] You are silenced.\n\r"
	    : ""
	    , ch );

	send_to_char( !IS_SET(ch->act, PLR_NO_TELL)
	    ? ""
	    : "[-tell     ] You can't use 'tell'.\n\r"
	    , ch );
    }
    else
    {
	bool fSet;
	int bit;

	     if ( arg[0] == '+' ) fSet = TRUE;
	else if ( arg[0] == '-' ) fSet = FALSE;
	else
	{
	    send_to_char( "Config -option or +option?\n\r", ch );
	    return;
	}

             if ( !str_cmp( arg+1, "ansi"     ) ) bit = PLR_ANSI;
        else if ( !str_cmp( arg+1, "autoexit" ) ) bit = PLR_AUTOEXIT;
	else if ( !str_cmp( arg+1, "autoloot" ) ) bit = PLR_AUTOLOOT;
	else if ( !str_cmp( arg+1, "autosac"  ) ) bit = PLR_AUTOSAC;
	else if ( !str_cmp( arg+1, "blank"    ) ) bit = PLR_BLANK;
	else if ( !str_cmp( arg+1, "brief"    ) ) bit = PLR_BRIEF;
	else if ( !str_cmp( arg+1, "combine"  ) ) bit = PLR_COMBINE;
        else if ( !str_cmp( arg+1, "prompt"   ) ) bit = PLR_PROMPT;
	else if ( !str_cmp( arg+1, "telnetga" ) ) bit = PLR_TELNET_GA;
	else
	{
	    send_to_char( "Config which option?\n\r", ch );
	    return;
	}

	if ( fSet )
	    SET_BIT    (ch->act, bit);
	else
	    REMOVE_BIT (ch->act, bit);

	send_to_char( "Ok.\n\r", ch );
    }

    return;
}

void do_ansi( CHAR_DATA *ch, char *argument )
{
    if ( IS_NPC(ch) ) return;
    if (IS_SET(ch->act, PLR_ANSI)) do_config(ch,"-ansi");
    else do_config(ch,"+ansi");
    return;
}

void do_autoexit( CHAR_DATA *ch, char *argument )
{
    if ( IS_NPC(ch) ) return;
    if (IS_SET(ch->act, PLR_AUTOEXIT)) do_config(ch,"-autoexit");
    else do_config(ch,"+autoexit");
    return;
}

void do_autoloot( CHAR_DATA *ch, char *argument )
{
    if ( IS_NPC(ch) ) return;
    if (IS_SET(ch->act, PLR_AUTOLOOT)) do_config(ch,"-autoloot");
    else do_config(ch,"+autoloot");
    return;
}

void do_autosac( CHAR_DATA *ch, char *argument )
{
    if ( IS_NPC(ch) ) return;
    if (IS_SET(ch->act, PLR_AUTOSAC)) do_config(ch,"-autosac");
    else do_config(ch,"+autosac");
    return;
}

void do_blank( CHAR_DATA *ch, char *argument )
{
    if ( IS_NPC(ch) ) return;
    if (IS_SET(ch->act, PLR_BLANK)) do_config(ch,"-blank");
    else do_config(ch,"+blank");
    return;
}

void do_brief( CHAR_DATA *ch, char *argument )
{
    if ( IS_NPC(ch) ) return;
    if (IS_SET(ch->act, PLR_BRIEF)) do_config(ch,"-brief");
    else do_config(ch,"+brief");
    return;
}

void do_diagnose( CHAR_DATA *ch, char *argument )
{
    char buf  [MAX_STRING_LENGTH];
    char arg  [MAX_INPUT_LENGTH];
    int teeth = 0;
    int ribs = 0;
    CHAR_DATA *victim;

    argument = one_argument( argument, arg );

    if (arg == '\0')
    {
	send_to_char("Who do you wish to diagnose?\n\r",ch);
	return;
    }

    if ( ( victim = get_char_room( ch, arg ) ) == NULL )
    {
	send_to_char("Nobody here by that name.\n\r",ch);
	return;
    }
    act("$n examines $N carefully, diagnosing $S injuries.",ch,NULL,victim,TO_NOTVICT);
    act("$n examines you carefully, diagnosing your injuries.",ch,NULL,victim,TO_VICT);
    act("Your diagnoses of $N reveals the following...",ch,NULL,victim,TO_CHAR);
    send_to_char("----------------------------------------------------------------------------\n\r",ch);
    if ( ( victim->loc_hp[0] + victim->loc_hp[1] + victim->loc_hp[2] + 
	   victim->loc_hp[3] + victim->loc_hp[4] + victim->loc_hp[5] +
	   victim->loc_hp[6] ) == 0 )
    {
	act("$N has no apparent injuries.",ch,NULL,victim,TO_CHAR);
	send_to_char("----------------------------------------------------------------------------\n\r",ch);
	return;
    }
    /* Check head */
    if (IS_HEAD(victim,LOST_EYE_L) && IS_HEAD(victim,LOST_EYE_R))
	act("$N has lost both of $S eyes.",ch,NULL,victim,TO_CHAR);
    else if (IS_HEAD(victim,LOST_EYE_L))
	act("$N has lost $S left eye.",ch,NULL,victim,TO_CHAR);
    else if (IS_HEAD(victim,LOST_EYE_R))
	act("$N has lost $S right eye.",ch,NULL,victim,TO_CHAR);
    if (IS_HEAD(victim,LOST_EAR_L) && IS_HEAD(victim,LOST_EAR_R))
	act("$N has lost both of $S ears.",ch,NULL,victim,TO_CHAR);
    else if (IS_HEAD(victim,LOST_EAR_L))
	act("$N has lost $S left ear.",ch,NULL,victim,TO_CHAR);
    else if (IS_HEAD(victim,LOST_EAR_R))
	act("$N has lost $S right ear.",ch,NULL,victim,TO_CHAR);
    if (IS_HEAD(victim,LOST_NOSE))
	act("$N has lost $S nose.",ch,NULL,victim,TO_CHAR);
    else if (IS_HEAD(victim,BROKEN_NOSE))
	act("$N has got a broken nose.",ch,NULL,victim,TO_CHAR);
    if (IS_HEAD(victim,BROKEN_JAW))
	act("$N has got a broken jaw.",ch,NULL,victim,TO_CHAR);
    if (IS_HEAD(victim,LOST_HEAD))
    {
	act("$N has had $S head cut off.",ch,NULL,victim,TO_CHAR);
	if (IS_BLEEDING(victim,BLEEDING_HEAD))
	    act("...Blood is spurting from the stump of $S neck.",ch,NULL,victim,TO_CHAR);
    }
    else
    {
	if (IS_BODY(victim,BROKEN_NECK))
	    act("$N has got a broken neck.",ch,NULL,victim,TO_CHAR);
	if (IS_BODY(victim,CUT_THROAT))
	{
	    act("$N has had $S throat cut open.",ch,NULL,victim,TO_CHAR);
	    if (IS_BLEEDING(victim,BLEEDING_THROAT))
		act("...Blood is pouring from the wound.",ch,NULL,victim,TO_CHAR);
	}
    }
    if (IS_HEAD(victim,BROKEN_SKULL))
	act("$N has got a broken skull.",ch,NULL,victim,TO_CHAR);
    if (IS_HEAD(victim,LOST_TOOTH_1 )) teeth += 1;
    if (IS_HEAD(victim,LOST_TOOTH_2 )) teeth += 2;
    if (IS_HEAD(victim,LOST_TOOTH_4 )) teeth += 4;
    if (IS_HEAD(victim,LOST_TOOTH_8 )) teeth += 8;
    if (IS_HEAD(victim,LOST_TOOTH_16)) teeth += 16;
    if (teeth > 0)
    {
	sprintf(buf,"$N has had %d teeth knocked out.",teeth);
	act(buf,ch,NULL,victim,TO_CHAR);
    }
    if (IS_HEAD(victim,LOST_TONGUE))
	act("$N has had $S tongue ripped out.",ch,NULL,victim,TO_CHAR);
    if (IS_HEAD(victim,LOST_HEAD))
    {
    	send_to_char("----------------------------------------------------------------------------\n\r",ch);
    	return;
    }
    /* Check body */
    if (IS_BODY(victim,BROKEN_RIBS_1 )) ribs += 1;
    if (IS_BODY(victim,BROKEN_RIBS_2 )) ribs += 2;
    if (IS_BODY(victim,BROKEN_RIBS_4 )) ribs += 4;
    if (IS_BODY(victim,BROKEN_RIBS_8 )) ribs += 8;
    if (IS_BODY(victim,BROKEN_RIBS_16)) ribs += 16;
    if (ribs > 0)
    {
	sprintf(buf,"$N has got %d broken ribs.",ribs);
	act(buf,ch,NULL,victim,TO_CHAR);
    }
    if (IS_BODY(victim,BROKEN_SPINE))
	act("$N has got a broken spine.",ch,NULL,victim,TO_CHAR);
    /* Check arms */
    check_left_arm(ch,victim);
    check_right_arm(ch,victim);
    check_left_leg(ch,victim);
    check_right_leg(ch,victim);
    send_to_char("----------------------------------------------------------------------------\n\r",ch);
    return;
}

void check_left_arm( CHAR_DATA *ch, CHAR_DATA *victim )
{
    char buf    [MAX_STRING_LENGTH];
    char finger [10];
    int fingers = 0;

    if (IS_ARM_L(victim,LOST_ARM) && IS_ARM_R(victim,LOST_ARM))
    {
	act("$N has lost both of $S arms.",ch,NULL,victim,TO_CHAR);
	if (IS_BLEEDING(victim,BLEEDING_ARM_L) && IS_BLEEDING(victim,BLEEDING_ARM_R))
	    act("...Blood is spurting from both stumps.",ch,NULL,victim,TO_CHAR);
	else if (IS_BLEEDING(victim,BLEEDING_ARM_L))
	    act("...Blood is spurting from the left stump.",ch,NULL,victim,TO_CHAR);
	else if (IS_BLEEDING(victim,BLEEDING_ARM_R))
	    act("...Blood is spurting from the right stump.",ch,NULL,victim,TO_CHAR);
	return;
    }
    if (IS_ARM_L(victim,LOST_ARM))
    {
	act("$N has lost $S left arm.",ch,NULL,victim,TO_CHAR);
	if (IS_BLEEDING(victim,BLEEDING_ARM_L))
	    act("...Blood is spurting from the stump.",ch,NULL,victim,TO_CHAR);
	return;
    }
    if (IS_ARM_L(victim,BROKEN_ARM) && IS_ARM_R(victim,BROKEN_ARM))
	act("$N arms are both broken.",ch,NULL,victim,TO_CHAR);
    else if (IS_ARM_L(victim,BROKEN_ARM))
	act("$N's left arm is broken.",ch,NULL,victim,TO_CHAR);
    if (IS_ARM_L(victim,LOST_HAND) && IS_ARM_R(victim,LOST_HAND) &&
	!IS_ARM_R(victim,LOST_ARM))
    {
	act("$N has lost both of $S hands.",ch,NULL,victim,TO_CHAR);
	if (IS_BLEEDING(victim,BLEEDING_HAND_L) && IS_BLEEDING(victim,BLEEDING_HAND_R))
	    act("...Blood is spurting from both stumps.",ch,NULL,victim,TO_CHAR);
	else if (IS_BLEEDING(victim,BLEEDING_HAND_L))
	    act("...Blood is spurting from the left stump.",ch,NULL,victim,TO_CHAR);
	else if (IS_BLEEDING(victim,BLEEDING_HAND_R))
	    act("...Blood is spurting from the right stump.",ch,NULL,victim,TO_CHAR);
	return;
    }
    if (IS_ARM_L(victim,LOST_HAND))
    {
	act("$N has lost $S left hand.",ch,NULL,victim,TO_CHAR);
	if (IS_BLEEDING(victim,BLEEDING_HAND_L))
	    act("...Blood is spurting from the stump.",ch,NULL,victim,TO_CHAR);
	return;
    }
    if (IS_ARM_L(victim,LOST_FINGER_I)) fingers += 1;
    if (IS_ARM_L(victim,LOST_FINGER_M)) fingers += 1;
    if (IS_ARM_L(victim,LOST_FINGER_R)) fingers += 1;
    if (IS_ARM_L(victim,LOST_FINGER_L)) fingers += 1;
    if (fingers == 1) sprintf(finger,"finger");
    else sprintf(finger,"fingers");
    if (fingers > 0 && IS_ARM_L(victim,LOST_THUMB))
    {
	sprintf(buf,"$N has lost %d %s and $S thumb from $S left hand.",fingers,finger);
	act(buf,ch,NULL,victim,TO_CHAR);
    }
    else if (fingers > 0)
    {
	sprintf(buf,"$N has lost %d %s from $S left hand.",fingers,finger);
	act(buf,ch,NULL,victim,TO_CHAR);
    }
    else if (IS_ARM_L(victim,LOST_THUMB))
    {
	sprintf(buf,"$N has lost the thumb from $S left hand.");
	act(buf,ch,NULL,victim,TO_CHAR);
    }
    fingers = 0;
    if (IS_ARM_L(victim,BROKEN_FINGER_I) && !IS_ARM_L(victim,LOST_FINGER_I)) fingers += 1;
    if (IS_ARM_L(victim,BROKEN_FINGER_M) && !IS_ARM_L(victim,LOST_FINGER_M)) fingers += 1;
    if (IS_ARM_L(victim,BROKEN_FINGER_R) && !IS_ARM_L(victim,LOST_FINGER_R)) fingers += 1;
    if (IS_ARM_L(victim,BROKEN_FINGER_L) && !IS_ARM_L(victim,LOST_FINGER_L)) fingers += 1;
    if (fingers == 1) sprintf(finger,"finger");
    else sprintf(finger,"fingers");
    if (fingers > 0 && IS_ARM_L(victim,BROKEN_THUMB) && !IS_ARM_L(victim,LOST_THUMB))
    {
	sprintf(buf,"$N has broken %d %s and $S thumb on $S left hand.",fingers,finger);
	act(buf,ch,NULL,victim,TO_CHAR);
    }
    else if (fingers > 0)
    {
	sprintf(buf,"$N has broken %d %s on $S left hand.",fingers,finger);
	act(buf,ch,NULL,victim,TO_CHAR);
    }
    else if (IS_ARM_L(victim,BROKEN_THUMB) && !IS_ARM_L(victim,LOST_THUMB))
    {
	sprintf(buf,"$N has broken the thumb on $S left hand.");
	act(buf,ch,NULL,victim,TO_CHAR);
    }
    return;
}

void check_right_arm( CHAR_DATA *ch, CHAR_DATA *victim )
{
    char buf    [MAX_STRING_LENGTH];
    char finger [10];
    int fingers = 0;

    if (IS_ARM_L(victim,LOST_ARM) && IS_ARM_R(victim,LOST_ARM))
	return;
    if (IS_ARM_R(victim,LOST_ARM))
    {
	act("$N has lost $S right arm.",ch,NULL,victim,TO_CHAR);
	if (IS_BLEEDING(victim,BLEEDING_ARM_R))
	    act("...Blood is spurting from the stump.",ch,NULL,victim,TO_CHAR);
	return;
    }
    if (!IS_ARM_L(victim,BROKEN_ARM) && IS_ARM_R(victim,BROKEN_ARM))
	act("$N's right arm is broken.",ch,NULL,victim,TO_CHAR);
    else if (IS_ARM_L(victim,LOST_ARM) && IS_ARM_R(victim,BROKEN_ARM))
	act("$N's right arm is broken.",ch,NULL,victim,TO_CHAR);
    if (IS_ARM_L(victim,LOST_HAND) && IS_ARM_R(victim,LOST_HAND))
	return;
    if (IS_ARM_R(victim,LOST_HAND))
    {
	act("$N has lost $S right hand.",ch,NULL,victim,TO_CHAR);
	if (IS_BLEEDING(victim,BLEEDING_HAND_R))
	    act("...Blood is spurting from the stump.",ch,NULL,victim,TO_CHAR);
	return;
    }
    if (IS_ARM_R(victim,LOST_FINGER_I)) fingers += 1;
    if (IS_ARM_R(victim,LOST_FINGER_M)) fingers += 1;
    if (IS_ARM_R(victim,LOST_FINGER_R)) fingers += 1;
    if (IS_ARM_R(victim,LOST_FINGER_L)) fingers += 1;
    if (fingers == 1) sprintf(finger,"finger");
    else sprintf(finger,"fingers");
    if (fingers > 0 && IS_ARM_R(victim,LOST_THUMB))
    {
	sprintf(buf,"$N has lost %d %s and $S thumb from $S right hand.",fingers,finger);
	act(buf,ch,NULL,victim,TO_CHAR);
    }
    else if (fingers > 0)
    {
	sprintf(buf,"$N has lost %d %s from $S right hand.",fingers,finger);
	act(buf,ch,NULL,victim,TO_CHAR);
    }
    else if (IS_ARM_R(victim,LOST_THUMB))
    {
	sprintf(buf,"$N has lost the thumb from $S right hand.");
	act(buf,ch,NULL,victim,TO_CHAR);
    }
    fingers = 0;
    if (IS_ARM_R(victim,BROKEN_FINGER_I) && !IS_ARM_R(victim,LOST_FINGER_I)) fingers += 1;
    if (IS_ARM_R(victim,BROKEN_FINGER_M) && !IS_ARM_R(victim,LOST_FINGER_M)) fingers += 1;
    if (IS_ARM_R(victim,BROKEN_FINGER_R) && !IS_ARM_R(victim,LOST_FINGER_R)) fingers += 1;
    if (IS_ARM_R(victim,BROKEN_FINGER_L) && !IS_ARM_R(victim,LOST_FINGER_L)) fingers += 1;
    if (fingers == 1) sprintf(finger,"finger");
    else sprintf(finger,"fingers");
    if (fingers > 0 && IS_ARM_R(victim,BROKEN_THUMB) && !IS_ARM_R(victim,LOST_THUMB))
    {
	sprintf(buf,"$N has broken %d %s and $S thumb on $S right hand.",fingers,finger);
	act(buf,ch,NULL,victim,TO_CHAR);
    }
    else if (fingers > 0)
    {
	sprintf(buf,"$N has broken %d %s on $S right hand.",fingers,finger);
	act(buf,ch,NULL,victim,TO_CHAR);
    }
    else if (IS_ARM_R(victim,BROKEN_THUMB) && !IS_ARM_R(victim,LOST_THUMB))
    {
	sprintf(buf,"$N has broken the thumb on $S right hand.");
	act(buf,ch,NULL,victim,TO_CHAR);
    }
    return;
}

void check_left_leg( CHAR_DATA *ch, CHAR_DATA *victim )
{
    char buf    [MAX_STRING_LENGTH];
    char toe [10];
    int toes = 0;

    if (IS_LEG_L(victim,LOST_LEG) && IS_LEG_R(victim,LOST_LEG))
    {
	act("$N has lost both of $S legs.",ch,NULL,victim,TO_CHAR);
	if (IS_BLEEDING(victim,BLEEDING_LEG_L) && IS_BLEEDING(victim,BLEEDING_LEG_R))
	    act("...Blood is spurting from both stumps.",ch,NULL,victim,TO_CHAR);
	else if (IS_BLEEDING(victim,BLEEDING_LEG_L))
	    act("...Blood is spurting from the left stump.",ch,NULL,victim,TO_CHAR);
	else if (IS_BLEEDING(victim,BLEEDING_LEG_R))
	    act("...Blood is spurting from the right stump.",ch,NULL,victim,TO_CHAR);
	return;
    }
    if (IS_LEG_L(victim,LOST_LEG))
    {
	act("$N has lost $S left leg.",ch,NULL,victim,TO_CHAR);
	if (IS_BLEEDING(victim,BLEEDING_LEG_L))
	    act("...Blood is spurting from the stump.",ch,NULL,victim,TO_CHAR);
	return;
    }
    if (IS_LEG_L(victim,BROKEN_LEG) && IS_LEG_R(victim,BROKEN_LEG))
	act("$N legs are both broken.",ch,NULL,victim,TO_CHAR);
    else if (IS_LEG_L(victim,BROKEN_LEG))
	act("$N's left leg is broken.",ch,NULL,victim,TO_CHAR);
    if (IS_LEG_L(victim,LOST_FOOT) && IS_LEG_R(victim,LOST_FOOT))
    {
	act("$N has lost both of $S feet.",ch,NULL,victim,TO_CHAR);
	if (IS_BLEEDING(victim,BLEEDING_FOOT_L) && IS_BLEEDING(victim,BLEEDING_FOOT_R))
	    act("...Blood is spurting from both stumps.",ch,NULL,victim,TO_CHAR);
	else if (IS_BLEEDING(victim,BLEEDING_FOOT_L))
	    act("...Blood is spurting from the left stump.",ch,NULL,victim,TO_CHAR);
	else if (IS_BLEEDING(victim,BLEEDING_FOOT_R))
	    act("...Blood is spurting from the right stump.",ch,NULL,victim,TO_CHAR);
	return;
    }
    if (IS_LEG_L(victim,LOST_FOOT))
    {
	act("$N has lost $S left foot.",ch,NULL,victim,TO_CHAR);
	if (IS_BLEEDING(victim,BLEEDING_FOOT_L))
	    act("...Blood is spurting from the stump.",ch,NULL,victim,TO_CHAR);
	return;
    }
    if (IS_LEG_L(victim,LOST_TOE_A)) toes += 1;
    if (IS_LEG_L(victim,LOST_TOE_B)) toes += 1;
    if (IS_LEG_L(victim,LOST_TOE_C)) toes += 1;
    if (IS_LEG_L(victim,LOST_TOE_D)) toes += 1;
    if (toes == 1) sprintf(toe,"toe");
    else sprintf(toe,"toes");
    if (toes > 0 && IS_LEG_L(victim,LOST_TOE_BIG))
    {
	sprintf(buf,"$N has lost %d %s and $S big toe from $S left foot.",toes,toe);
	act(buf,ch,NULL,victim,TO_CHAR);
    }
    else if (toes > 0)
    {
	sprintf(buf,"$N has lost %d %s from $S left foot.",toes,toe);
	act(buf,ch,NULL,victim,TO_CHAR);
    }
    else if (IS_LEG_L(victim,LOST_TOE_BIG))
    {
	sprintf(buf,"$N has lost the big toe from $S left foot.");
	act(buf,ch,NULL,victim,TO_CHAR);
    }
    toes = 0;
    if (IS_LEG_L(victim,BROKEN_TOE_A) && !IS_LEG_L(victim,LOST_TOE_A)) toes += 1;
    if (IS_LEG_L(victim,BROKEN_TOE_B) && !IS_LEG_L(victim,LOST_TOE_B)) toes += 1;
    if (IS_LEG_L(victim,BROKEN_TOE_C) && !IS_LEG_L(victim,LOST_TOE_C)) toes += 1;
    if (IS_LEG_L(victim,BROKEN_TOE_D) && !IS_LEG_L(victim,LOST_TOE_D)) toes += 1;
    if (toes == 1) sprintf(toe,"toe");
    else sprintf(toe,"toes");
    if (toes > 0 && IS_LEG_L(victim,BROKEN_TOE_BIG) && !IS_LEG_L(victim,LOST_TOE_BIG))
    {
	sprintf(buf,"$N has broken %d %s and $S big toe from $S left foot.",toes,toe);
	act(buf,ch,NULL,victim,TO_CHAR);
    }
    else if (toes > 0)
    {
	sprintf(buf,"$N has broken %d %s on $S left foot.",toes,toe);
	act(buf,ch,NULL,victim,TO_CHAR);
    }
    else if (IS_LEG_L(victim,BROKEN_TOE_BIG) && !IS_LEG_L(victim,LOST_TOE_BIG))
    {
	sprintf(buf,"$N has broken the big toe on $S left foot.");
	act(buf,ch,NULL,victim,TO_CHAR);
    }
    return;
}

void check_right_leg( CHAR_DATA *ch, CHAR_DATA *victim )
{
    char buf    [MAX_STRING_LENGTH];
    char toe [10];
    int toes = 0;

    if (IS_LEG_L(victim,LOST_LEG) && IS_LEG_R(victim,LOST_LEG))
	return;
    if (IS_LEG_R(victim,LOST_LEG))
    {
	act("$N has lost $S right leg.",ch,NULL,victim,TO_CHAR);
	if (IS_BLEEDING(victim,BLEEDING_LEG_R))
	    act("...Blood is spurting from the stump.",ch,NULL,victim,TO_CHAR);
	return;
    }
    if (!IS_LEG_L(victim,BROKEN_LEG) && IS_LEG_R(victim,BROKEN_LEG))
	act("$N's right leg is broken.",ch,NULL,victim,TO_CHAR);
    if (IS_LEG_L(victim,LOST_FOOT) && IS_LEG_R(victim,LOST_FOOT))
	return;
    if (IS_LEG_R(victim,LOST_FOOT))
    {
	act("$N has lost $S right foot.",ch,NULL,victim,TO_CHAR);
	if (IS_BLEEDING(victim,BLEEDING_FOOT_R))
	    act("...Blood is spurting from the stump.",ch,NULL,victim,TO_CHAR);
	return;
    }
    if (IS_LEG_R(victim,LOST_TOE_A)) toes += 1;
    if (IS_LEG_R(victim,LOST_TOE_B)) toes += 1;
    if (IS_LEG_R(victim,LOST_TOE_C)) toes += 1;
    if (IS_LEG_R(victim,LOST_TOE_D)) toes += 1;
    if (toes == 1) sprintf(toe,"toe");
    else sprintf(toe,"toes");
    if (toes > 0 && IS_LEG_R(victim,LOST_TOE_BIG))
    {
	sprintf(buf,"$N has lost %d %s and $S big toe from $S right foot.",toes,toe);
	act(buf,ch,NULL,victim,TO_CHAR);
    }
    else if (toes > 0)
    {
	sprintf(buf,"$N has lost %d %s from $S right foot.",toes,toe);
	act(buf,ch,NULL,victim,TO_CHAR);
    }
    else if (IS_LEG_R(victim,LOST_TOE_BIG))
    {
	sprintf(buf,"$N has lost the big toe from $S right foot.");
	act(buf,ch,NULL,victim,TO_CHAR);
    }
    toes = 0;
    if (IS_LEG_R(victim,BROKEN_TOE_A) && !IS_LEG_R(victim,LOST_TOE_A)) toes += 1;
    if (IS_LEG_R(victim,BROKEN_TOE_B) && !IS_LEG_R(victim,LOST_TOE_B)) toes += 1;
    if (IS_LEG_R(victim,BROKEN_TOE_C) && !IS_LEG_R(victim,LOST_TOE_C)) toes += 1;
    if (IS_LEG_R(victim,BROKEN_TOE_D) && !IS_LEG_R(victim,LOST_TOE_D)) toes += 1;
    if (toes == 1) sprintf(toe,"toe");
    else sprintf(toe,"toes");
    if (toes > 0 && IS_LEG_R(victim,BROKEN_TOE_BIG) && !IS_LEG_R(victim,LOST_TOE_BIG))
    {
	sprintf(buf,"$N has broken %d %s and $S big toe on $S right foot.",toes,toe);
	act(buf,ch,NULL,victim,TO_CHAR);
    }
    else if (toes > 0)
    {
	sprintf(buf,"$N has broken %d %s on $S right foot.",toes,toe);
	act(buf,ch,NULL,victim,TO_CHAR);
    }
    else if (IS_LEG_R(victim,BROKEN_TOE_BIG) && !IS_LEG_R(victim,LOST_TOE_BIG))
    {
	sprintf(buf,"$N has broken the big toe on $S right foot.");
	act(buf,ch,NULL,victim,TO_CHAR);
    }
    return;
}

void obj_score( CHAR_DATA *ch, OBJ_DATA *obj )
{
    char buf[MAX_STRING_LENGTH];
    AFFECT_DATA *paf;
    int itemtype;

    sprintf( buf,"You are %s.\n\r",obj->short_descr);
    send_to_char( buf, ch );

    sprintf( buf,"Type %s, Extra flags %s.\n\r",item_type_name(obj),
	extra_bit_name(obj->extra_flags));
    send_to_char( buf, ch );

    sprintf( buf,"You weigh %d pounds and are worth %d gold coins.\n\r",obj->weight,obj->cost);
    send_to_char( buf, ch );

    if (obj->questmaker != NULL && strlen(obj->questmaker) > 1 &&
	obj->questowner != NULL && strlen(obj->questowner) > 1)
    {
	sprintf( buf, "You were created by %s, and are owned by %s.\n\r", obj->questmaker,obj->questowner );
	send_to_char( buf, ch );
    }
    else if (obj->questmaker != NULL && strlen(obj->questmaker) > 1)
    {
	sprintf( buf, "You were created by %s.\n\r", obj->questmaker );
	send_to_char( buf, ch );
    }
    else if (obj->questowner != NULL && strlen(obj->questowner) > 1)
    {
	sprintf( buf, "You are owned by %s.\n\r", obj->questowner );
	send_to_char( buf, ch );
    }

    switch ( obj->item_type )
    {
    case ITEM_SCROLL: 
    case ITEM_POTION:
	sprintf( buf, "You contain level %d spells of:", obj->value[0] );
	send_to_char( buf, ch );

	if ( obj->value[1] >= 0 && obj->value[1] < MAX_SKILL )
	{
	    send_to_char( " '", ch );
	    send_to_char( skill_table[obj->value[1]].name, ch );
	    send_to_char( "'", ch );
	}

	if ( obj->value[2] >= 0 && obj->value[2] < MAX_SKILL )
	{
	    send_to_char( " '", ch );
	    send_to_char( skill_table[obj->value[2]].name, ch );
	    send_to_char( "'", ch );
	}

	if ( obj->value[3] >= 0 && obj->value[3] < MAX_SKILL )
	{
	    send_to_char( " '", ch );
	    send_to_char( skill_table[obj->value[3]].name, ch );
	    send_to_char( "'", ch );
	}

	send_to_char( ".\n\r", ch );
	break;

    case ITEM_QUEST:
	sprintf( buf, "Your quest point value is %d.\n\r", obj->value[0] );
	send_to_char( buf, ch );
	break;

    case ITEM_WAND: 
    case ITEM_STAFF: 
	sprintf( buf, "You have %d(%d) charges of level %d",
	    obj->value[1], obj->value[2], obj->value[0] );
	send_to_char( buf, ch );

	if ( obj->value[3] >= 0 && obj->value[3] < MAX_SKILL )
	{
	    send_to_char( " '", ch );
	    send_to_char( skill_table[obj->value[3]].name, ch );
	    send_to_char( "'", ch );
	}

	send_to_char( ".\n\r", ch );
	break;
      
    case ITEM_WEAPON:
	sprintf( buf, "You inflict %d to %d damage in combat (average %d).\n\r",
	    obj->value[1], obj->value[2],
	    ( obj->value[1] + obj->value[2] ) / 2 );
	send_to_char( buf, ch );

	if (obj->value[0] >= 1000)
	    itemtype = obj->value[0] - ((obj->value[0] / 1000) * 1000);
	else
	    itemtype = obj->value[0];

	if (itemtype > 0)
	{
	    if (obj->level < 10)
		sprintf(buf,"You are a minor spell weapon.\n\r");
	    else if (obj->level < 20)
		sprintf(buf,"You are a lesser spell weapon.\n\r");
	    else if (obj->level < 30)
		sprintf(buf,"You are an average spell weapon.\n\r");
	    else if (obj->level < 40)
		sprintf(buf,"You are a greater spell weapon.\n\r");
	    else if (obj->level < 50)
		sprintf(buf,"You are a major spell weapon.\n\r");
	    else
		sprintf(buf,"You are a supreme spell weapon.\n\r");
	    send_to_char(buf,ch);
	}

	if (itemtype == 1)
	    sprintf (buf, "You are dripping with corrosive acid.\n\r");
	else if (itemtype == 4)
	    sprintf (buf, "You radiate an aura of darkness.\n\r");
	else if (itemtype == 30)
	    sprintf (buf, "You are the bane of all evil.\n\r");
	else if (itemtype == 34)
	    sprintf (buf, "You drink the souls of your victims.\n\r");
	else if (itemtype == 37)
	    sprintf (buf, "You have been tempered in hellfire.\n\r");
	else if (itemtype == 48)
	    sprintf (buf, "You crackle with sparks of lightning.\n\r");
	else if (itemtype == 53)
	    sprintf (buf, "You are dripping with a dark poison.\n\r");
	else if (itemtype > 0)
	    sprintf (buf, "You have been imbued with the power of %s.\n\r",skill_table[itemtype].name);
	if (itemtype > 0)
	    send_to_char( buf, ch );

	if (obj->value[0] >= 1000)
	    itemtype = obj->value[0] / 1000;
	else
	    break;

	if (itemtype == 4 || itemtype == 1)
	    sprintf (buf, "You radiate an aura of darkness.\n\r");
	else if (itemtype == 27 || itemtype == 2)
	    sprintf (buf, "You allow your wielder to see invisible things.\n\r");
	else if (itemtype == 39 || itemtype == 3)
	    sprintf (buf, "You grant your wielder the power of flight.\n\r");
	else if (itemtype == 45 || itemtype == 4)
	    sprintf (buf, "You allow your wielder to see in the dark.\n\r");
	else if (itemtype == 46 || itemtype == 5)
	    sprintf (buf, "You render your wielder invisible to the human eye.\n\r");
	else if (itemtype == 52 || itemtype == 6)
	    sprintf (buf, "You allow your wielder to walk through solid doors.\n\r");
	else if (itemtype == 54 || itemtype == 7)
	    sprintf (buf, "You protect your wielder from evil.\n\r");
	else if (itemtype == 57 || itemtype == 8)
	    sprintf (buf, "You protect your wielder in combat.\n\r");
	else if (itemtype == 9)
	    sprintf (buf, "You allow your wielder to walk in complete silence.\n\r");
	else if (itemtype == 10)
	    sprintf (buf, "You surround your wielder with a shield of lightning.\n\r");
	else if (itemtype == 11)
	    sprintf (buf, "You surround your wielder with a shield of fire.\n\r");
	else if (itemtype == 12)
	    sprintf (buf, "You surround your wielder with a shield of ice.\n\r");
	else if (itemtype == 13)
	    sprintf (buf, "You surround your wielder with a shield of acid.\n\r");
	else if (itemtype == 14)
	    sprintf (buf, "You protect your wielder from attacks from DarkBlade clan guardians.\n\r");
	else if (itemtype == 15)
	    sprintf (buf, "You surround your wielder with a shield of chaos.\n\r");
	else if (itemtype == 16)
	    sprintf (buf, "You regenerate the wounds of your wielder.\n\r");
	else if (itemtype == 17)
	    sprintf (buf, "You enable your wielder to move at supernatural speed.\n\r");
	else if (itemtype == 18)
	    sprintf (buf, "You can slice through armour without difficulty.\n\r");
	else if (itemtype == 19)
	    sprintf (buf, "You protect your wielder from player attacks.\n\r");
	else if (itemtype == 20)
	    sprintf (buf, "You surround your wielder with a shield of darkness.\n\r");
	else if (itemtype == 21)
	    sprintf (buf, "You grant your wielder superior protection.\n\r");
	else if (itemtype == 22)
	    sprintf (buf, "You grant your wielder supernatural vision.\n\r");
	else if (itemtype == 23)
	    sprintf (buf, "You make your wielder fleet-footed.\n\r");
	else if (itemtype == 24)
	    sprintf (buf, "You conceal your wielder from sight.\n\r");
	else if (itemtype == 25)
	    sprintf (buf, "You invoke the power of your wielders beast.\n\r");
	else
	    sprintf (buf, "You are bugged...please report it.\n\r");
	if (itemtype > 0)
	    send_to_char( buf, ch );
	break;

    case ITEM_ARMOR:
	sprintf( buf, "Your armor class is %d.\n\r", obj->value[0] );
	send_to_char( buf, ch );
	if (obj->value[3] < 1)
	    break;
	if (obj->value[3] == 4 || obj->value[3] == 1)
	    sprintf (buf, "You radiate an aura of darkness.\n\r");
	else if (obj->value[3] == 27 || obj->value[3] == 2)
	    sprintf (buf, "You allow your wearer to see invisible things.\n\r");
	else if (obj->value[3] == 39 || obj->value[3] == 3)
	    sprintf (buf, "You grant your wearer the power of flight.\n\r");
	else if (obj->value[3] == 45 || obj->value[3] == 4)
	    sprintf (buf, "You allow your wearer to see in the dark.\n\r");
	else if (obj->value[3] == 46 || obj->value[3] == 5)
	    sprintf (buf, "You render your wearer invisible to the human eye.\n\r");
	else if (obj->value[3] == 52 || obj->value[3] == 6)
	    sprintf (buf, "You allow your wearer to walk through solid doors.\n\r");
	else if (obj->value[3] == 54 || obj->value[3] == 7)
	    sprintf (buf, "You protect your wearer from evil.\n\r");
	else if (obj->value[3] == 57 || obj->value[3] == 8)
	    sprintf (buf, "You protect your wearer in combat.\n\r");
	else if (obj->value[3] == 9)
	    sprintf (buf, "You allow your wearer to walk in complete silence.\n\r");
	else if (obj->value[3] == 10)
	    sprintf (buf, "You surround your wearer with a shield of lightning.\n\r");
	else if (obj->value[3] == 11)
	    sprintf (buf, "You surround your wearer with a shield of fire.\n\r");
	else if (obj->value[3] == 12)
	    sprintf (buf, "You surround your wearer with a shield of ice.\n\r");
	else if (obj->value[3] == 13)
	    sprintf (buf, "You surround your wearer with a shield of acid.\n\r");
	else if (obj->value[3] == 14)
	    sprintf (buf, "You protect your wearer from attacks from DarkBlade clan guardians.\n\r");
	else if (obj->value[3] == 15)
	    sprintf (buf, "You surround your wielder with a shield of chaos.\n\r");
	else if (obj->value[3] == 16)
	    sprintf (buf, "You regenerate the wounds of your wielder.\n\r");
	else if (obj->value[3] == 17)
	    sprintf (buf, "You enable your wearer to move at supernatural speed.\n\r");
	else if (obj->value[3] == 18)
	    sprintf (buf, "You can slice through armour without difficulty.\n\r");
	else if (obj->value[3] == 19)
	    sprintf (buf, "You protect your wearer from player attacks.\n\r");
	else if (obj->value[3] == 20)
	    sprintf (buf, "You surround your wearer with a shield of darkness.\n\r");
	else if (obj->value[3] == 21)
	    sprintf (buf, "You grant your wearer superior protection.\n\r");
	else if (obj->value[3] == 22)
	    sprintf (buf, "You grant your wearer supernatural vision.\n\r");
	else if (obj->value[3] == 23)
	    sprintf (buf, "You make your wearer fleet-footed.\n\r");
	else if (obj->value[3] == 24)
	    sprintf (buf, "You conceal your wearer from sight.\n\r");
	else if (obj->value[3] == 25)
	    sprintf (buf, "You invoke the power of your wearers beast.\n\r");
	else
	    sprintf (buf, "You are bugged...please report it.\n\r");
	if (obj->value[3] > 0)
	    send_to_char( buf, ch );
	break;
    }

    for ( paf = obj->pIndexData->affected; paf != NULL; paf = paf->next )
    {
	if ( paf->location != APPLY_NONE && paf->modifier != 0 )
	{
	    sprintf( buf, "You affect %s by %d.\n\r",
		affect_loc_name( paf->location ), paf->modifier );
	    send_to_char( buf, ch );
	}
    }

    for ( paf = obj->affected; paf != NULL; paf = paf->next )
    {
	if ( paf->location != APPLY_NONE && paf->modifier != 0 )
	{
	    sprintf( buf, "You affect %s by %d.\n\r",
		affect_loc_name( paf->location ), paf->modifier );
	    send_to_char( buf, ch );
	}
    }
    return;
}

/* Do_prompt from Morgenes from Aldara Mud */
void do_prompt( CHAR_DATA *ch, char *argument )
{
   char buf [ MAX_STRING_LENGTH ];

   buf[0] = '\0';
   if ( IS_NPC(ch) ) return;

   if ( argument[0] == '\0' )
   {
       do_help( ch, "prompt" );
       return;
   }

   if( !strcmp( argument, "on" ) )
   {
      if (IS_EXTRA(ch, EXTRA_PROMPT))
         send_to_char("But you already have customised prompt on!\n\r",ch);
      else
      {
         send_to_char("Ok.\n\r",ch);
         SET_BIT(ch->extra, EXTRA_PROMPT);
      }
      return;
   }
   else if( !strcmp( argument, "off" ) )
   {
      if (!IS_EXTRA(ch, EXTRA_PROMPT))
         send_to_char("But you already have customised prompt off!\n\r",ch);
      else
      {
         send_to_char("Ok.\n\r",ch);
         REMOVE_BIT(ch->extra, EXTRA_PROMPT);
      }
      return;
   }
   else if( !strcmp( argument, "clear" ) )
   {
      free_string(ch->prompt);
      ch->prompt = str_dup( "" );
      return;
   }
   else
   {
      if ( strlen( argument ) > 50 )
	  argument[50] = '\0';
      smash_tilde( argument );
      strcat( buf, argument );
   }

   free_string( ch->prompt );
   ch->prompt = str_dup( buf );
   send_to_char( "Ok.\n\r", ch );
   return;
} 

/* Do_prompt from Morgenes from Aldara Mud */
void do_cprompt( CHAR_DATA *ch, char *argument )
{
   char buf [ MAX_STRING_LENGTH ];

   buf[0] = '\0';
   if ( IS_NPC(ch) ) return;

   if ( argument[0] == '\0' )
   {
       do_help( ch, "cprompt" );
       return;
   }

   if( !strcmp( argument, "clear" ) )
   {
      free_string(ch->cprompt);
      ch->cprompt = str_dup( "" );
      return;
   }
   else
   {
      if ( strlen( argument ) > 50 )
	  argument[50] = '\0';
      smash_tilde( argument );
      strcat( buf, argument );
   }

   free_string( ch->cprompt );
   ch->cprompt = str_dup( buf );
   send_to_char( "Ok.\n\r", ch );
   return;
} 

void 	do_finger( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_STRING_LENGTH];
    char strsave[MAX_INPUT_LENGTH];
    char *buf;
    char buf2[MAX_INPUT_LENGTH];
    FILE *fp;
    int  num;
    int  num2;
    int  extra;

    if ( IS_NPC(ch) ) return;
    one_argument(argument,arg);
    if (!check_parse_name( argument ))
    {
	send_to_char( "Thats an illegal name.\n\r", ch );
	return;
    }

    if ( !char_exists(TRUE,argument) )
    {
	send_to_char( "That player doesn't exist.\n\r", ch );
	return;
    }

    fclose( fpReserve );
    sprintf( strsave, "%sbackup/%s", PLAYER_DIR, capitalize(argument) );
    if ( ( fp = fopen( strsave, "r" ) ) != NULL )
    {
	send_to_char("----------------------------------------------------------------------------\n\r",ch);
	buf=fread_string( fp );
	send_to_char(buf,ch);
	send_to_char(" ",ch);
	buf=fread_string( fp );
	send_to_char(buf,ch);
	send_to_char(".\n\r",ch);
	send_to_char("----------------------------------------------------------------------------\n\r",ch);
	send_to_char("Last connected ",ch);
	buf=fread_string( fp );
	send_to_char("at ",ch);
	buf=fread_string( fp );
	send_to_char(buf,ch);
	send_to_char(".\n\r",ch);
	send_to_char("----------------------------------------------------------------------------\n\r",ch);
	extra=fread_number( fp );
	num=fread_number( fp );
	send_to_char("Sex: ",ch);
	buf=fread_string( fp );
	if (num == SEX_MALE) send_to_char("Male. ",ch);
	else if (num == SEX_FEMALE) 
	{
	    send_to_char("Female. ",ch);
	    if (IS_SET(extra, EXTRA_PREGNANT)) other_age(ch,extra,TRUE,buf);
	}
	else send_to_char("None. ",ch);
	buf=fread_string( fp );
	other_age(ch,extra,FALSE,buf);
	num=fread_number( fp );
	num2=fread_number( fp );
	switch ( num )
	{
	    default:
		send_to_char("They are mortal, ",ch);
		break;
	    case LEVEL_AVATAR:
	    case LEVEL_APPRENTICE:
	    case LEVEL_MAGE:
	    case LEVEL_ARCHMAGE:
	    {
		if (num2 < 1)
		    send_to_char("They are a Avatar, ",ch);
		else if (num2 < 5)
		    send_to_char("They are a Immortal, ",ch);
		else if (num2 < 10)
		    send_to_char("They are a Godling, ",ch);
		else if (num2 < 15)
		    send_to_char("They are a Demigod, ",ch);
		else if (num2 < 20)
		    send_to_char("They are a Lesser God, ",ch);
		else if (num2 < 25)
		    send_to_char("They are a Greater God, ",ch);
		else
		    send_to_char("They are a Supreme God, ",ch);
		break;
	    }
	    case LEVEL_BUILDER:
		send_to_char("They are a Builder, ",ch);
		break;
	    case LEVEL_QUESTMAKER:
		send_to_char("They are a Quest Maker, ",ch);
		break;
	    case LEVEL_ENFORCER:
		send_to_char("They are an Enforcer, ",ch);
		break;
	    case LEVEL_JUDGE:
		send_to_char("They are a Judge, ",ch);
		break;
	    case LEVEL_HIGHJUDGE:
		send_to_char("They are a High Judge, ",ch);
		break;
	    case LEVEL_IMPLEMENTOR:
		send_to_char("They are an Implementor, ",ch);
		break;
	}
	num=fread_number( fp );
	if (num > 0) num2 = (2*(num / 7200)); else num2 = 0;
	sprintf(buf2,"and have been playing for %d hours.\n\r",num2);
	send_to_char(buf2,ch);
	buf=fread_string( fp );
	if (strlen(buf) > 2)
	{
	    if (IS_SET(extra, EXTRA_MARRIED))
		sprintf(buf2,"They are married to %s.\n\r",buf);
	    else
		sprintf(buf2,"They are engaged to %s.\n\r",buf);
	    send_to_char(buf2,ch);
	}
	num=fread_number( fp );
	num2=fread_number( fp );
	sprintf(buf2,"Player kills: %d, Player Deaths: %d.\n\r",num,num2);
	send_to_char(buf2,ch);
	num=fread_number( fp );
	num2=fread_number( fp );
	sprintf(buf2,"Arena kills: %d, Arena Deaths: %d.\n\r",num,num2);
        send_to_char(buf2,ch);
        num=fread_number(fp);
        num2=fread_number(fp);
        sprintf(buf2,"Mob Kills: %d, Mob Deaths: %d.\n\r",num,num2);
	send_to_char(buf2,ch);
	send_to_char("----------------------------------------------------------------------------\n\r",ch);
    }
    else
    {
	bug( "Do_finger: fopen", 0 );
    }
    fclose( fp );
    fpReserve = fopen( NULL_FILE, "r" );
    return;
}
