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


void do_bloodwall(CHAR_DATA *ch, char *argument)
{
OBJ_DATA *obj = NULL;
OBJ_DATA *objc;
char arg[MAX_STRING_LENGTH];
char arg2[MAX_STRING_LENGTH];
char wall[MAX_STRING_LENGTH];
char buf[MAX_STRING_LENGTH];
int value;

argument=one_argument(argument,arg);
argument=one_argument(argument,arg2);

if (IS_NPC(ch)) return;

if (!IS_CLASS(ch,CLASS_VAMPIRE))
{
send_to_char("Huh?\n\r",ch);
return;
}
if (ch->power[DISC_VAMP_DAIM] < 2)
{
send_to_char("You need Daiminion 2 to call Blood Walls.\n\r",ch);
return;
}

if (arg[0] == '\0' || (str_cmp(arg,"n")
                   && str_cmp(arg,"s")
                   && str_cmp(arg,"e")
                   && str_cmp(arg,"w")
                   && str_cmp(arg,"u")
                   && str_cmp(arg,"d")))
{
send_to_char("You may wall n, w, s, e, d or u.\n\r",ch);
return;
}

if (arg2[0] == '\0')
{
send_to_char("Please supply the amount of blood to expend.\n\r",ch);
return;
}

value = is_number( arg2 ) ? atoi( arg2 ) : -1;

if (value > 200 || value < 1)
{
send_to_char("Please select a value between 1 and 200.\n\r",ch);
return;
}

ch->pcdata->condition[COND_THIRST] -= value;
sprintf(buf,"A look of concentration passes over %s's face.",ch->name);
act(buf,ch,NULL,NULL,TO_ROOM);
send_to_char("A look of concentration passes over your face.\n\r",ch);
if (!str_cmp(arg,"n")) sprintf(wall,"walln");
if (!str_cmp(arg,"w")) sprintf(wall,"wallw");
if (!str_cmp(arg,"s")) sprintf(wall,"walls");
if (!str_cmp(arg,"e")) sprintf(wall,"walle");
if (!str_cmp(arg,"d")) sprintf(wall,"walld");
if (!str_cmp(arg,"u")) sprintf(wall,"wallu");
objc = get_obj_list(ch,wall,ch->in_room->contents);
if ( objc != NULL)
{
send_to_char("There is already a wall blocking that direction.\n\r",ch);
return;
}
WAIT_STATE(ch,25);
sprintf(buf,"A wall of blood pours out of the ground.");
act(buf,ch,NULL,NULL,TO_ROOM);
send_to_char(buf,ch);
if (!str_cmp(arg,"n")) 
{
obj=create_object(get_obj_index(30043),0);
sprintf(buf,"A wall of blood is here, blocking your exit north.");
obj->short_descr = str_dup(buf);
}
if (!str_cmp(arg,"s")) 
{
obj=create_object(get_obj_index(30044),0);
sprintf(buf,"A wall of blood is here, blocking your exit south.");
obj->short_descr = str_dup(buf);
}
if (!str_cmp(arg,"e")) 
{
obj=create_object(get_obj_index(30045),0);
sprintf(buf,"A wall of blood is here, blocking your exit east");
obj->short_descr = str_dup(buf);
}
if (!str_cmp(arg,"w")) 
{
obj=create_object(get_obj_index(30046),0);
sprintf(buf,"A wall of blood is here, blocking your exit west");
obj->short_descr = str_dup(buf);
}
if (!str_cmp(arg,"d")) 
{
obj=create_object(get_obj_index(30047),0);
sprintf(buf,"A wall of blood is here, blocking your exit down");
obj->short_descr = str_dup(buf);
}
if (!str_cmp(arg,"u")) 
{
obj=create_object(get_obj_index(30048),0);
sprintf(buf,"A wall of blood is here, blocking your exit up.");
obj->short_descr = str_dup(buf);
}
obj_to_room(obj,ch->in_room);
obj->timer=(value/4);
obj->item_type = ITEM_WALL; 
return;
}

void do_bloodrod( CHAR_DATA *ch, char *argument )
{
    OBJ_DATA *obj;
 
    if (IS_NPC(ch)) return;
 
    if (!IS_CLASS(ch, CLASS_VAMPIRE))
    {
	send_to_char("Huh?\n\r",ch);
	return;
    }
 
    if (ch->power[DISC_VAMP_THAU] < 2)
    {
	send_to_char("You need to obtain level 2 Thaumaturgy to create a Blood Rod.\n\r",ch);
	return;
    }

    if ( 10 > ch->practice)
    {
	send_to_char("It costs 10 points of primal to create a Blood Rod.\n\r",ch);
	return;
    }
	ch->practice   -= 10;
	obj = create_object(get_obj_index(OBJ_VNUM_BROD)  ,0 );
    	obj_to_char(obj, ch);
	act("A Rod of Blood appears in your hands.",ch,NULL,NULL,TO_CHAR);
	act("A Rod of blood appears in $n's hands.",ch,NULL,NULL,TO_ROOM);
	return;
}

void do_shadowgaze(CHAR_DATA *ch, char *argument  )
{
    char arg[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;
    char buf[MAX_STRING_LENGTH];
   one_argument( argument,arg);
 
    if (IS_NPC(ch)) return;
    if (!IS_CLASS(ch,CLASS_VAMPIRE))
    {
    send_to_char("Huh?\n\r",ch);
    return;
    }
   if (ch->power[DISC_VAMP_OBTE] < 10)
   {
   send_to_char("You need level 10 Obtenebration to use Shadow Gaze.\n\r",ch);
   return;
   }

   if (!IS_AFFECTED(ch, AFF_SHADOWPLANE))
   {
	send_to_char("You need to be in the Shadowplane, first!\n\r",ch);
	return;
   }


    if ( arg[0] == '\0' )
    {
        send_to_char( "Who do you wish to use Shadow Gaze on?\n\r", ch );
	return;
    }
    if ((victim = get_char_room( ch, arg)) == NULL)
    {
    send_to_char("They arent here.\n\r",ch);
    return;
    }
  
    if (IS_AFFECTED(victim, AFF_SHADOWPLANE))
   {
        send_to_char("They are already in the Shadowplane!\n\r",ch);
        return;
   } 

    if (!IS_NPC(victim))
    {
    send_to_char("You cannot Shadow Gaze a person.\n\r",ch);
    return;
    }
    sprintf(buf,
    "%s gazes intently at %s.",ch->name,victim->short_descr);
    act(buf,ch,NULL,NULL,TO_ROOM);
    sprintf(buf,
    "You gaze deeply into %s's eyes.\n\r",victim->short_descr);
    send_to_char(buf,ch);
    WAIT_STATE(ch, 8);   
    sprintf(buf,
    "You pull %s into the Shadowplane!",victim->short_descr);
    act(buf,ch,NULL,NULL,TO_CHAR);
    sprintf(buf,
    "%s rips %s into the Shadowplane!"
    ,ch->name,victim->short_descr);
    act(buf,ch,NULL,NULL,TO_ROOM);
    SET_BIT(victim->affected_by, AFF_SHADOWPLANE);
    return;
}


void do_grab(CHAR_DATA *ch, char *argument  )
{
    char arg[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;
    char buf[MAX_STRING_LENGTH];
   one_argument( argument,arg);
 
    if (IS_NPC(ch)) return;
    if (!IS_CLASS(ch,CLASS_VAMPIRE))
    {
    send_to_char("Huh?\n\r",ch);
    return;
    }
   if (ch->power[DISC_VAMP_OBTE] < 8)
   {
   send_to_char("You need level 8 Obtenebration to use Grab.\n\r",ch);
   return;
   }

   if (!IS_AFFECTED(ch, AFF_SHADOWPLANE))
   {
	send_to_char("You need to be in the Shadowplane, first!\n\r",ch);
	return;
   }


    if ( arg[0] == '\0' )
    {
        send_to_char( "Who do you wish to Grab?\n\r", ch );
	return;
    }
    if ((victim = get_char_room( ch, arg)) == NULL)
    {
    send_to_char("They arent here.\n\r",ch);
    return;
    }

     if (IS_AFFECTED(victim, AFF_SHADOWPLANE))
   {
        send_to_char("They are already in the Shadowplane!\n\r",ch);
        return;
   }

    if (IS_NPC(victim))
    {
    send_to_char("You cannot Grab a mobile!\n\r",ch);
    return;
    }
    sprintf(buf,
    "%s pulls %s into the Shadowplane.",ch->name,victim->name);
    act(buf,ch,NULL,NULL,TO_ROOM);
    sprintf(buf,
    "You pull %s into the Shadowplane!\n\r",victim->name);
    send_to_char(buf,ch);
    sprintf(buf,
    "%s pulls you into the Shadowplane!\n\r",ch->name);
    act(buf,ch,NULL,victim,TO_VICT);
    WAIT_STATE(ch, 8);     
    SET_BIT(victim->affected_by, AFF_SHADOWPLANE);
    return;
}


void do_share( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *victim;
    CHAR_DATA *familiar;
    char arg[MAX_INPUT_LENGTH];
    char buf[MAX_STRING_LENGTH];

    one_argument( argument, arg );

    if ( IS_NPC(ch) )
	return;

    if (!IS_CLASS(ch, CLASS_VAMPIRE) )
    {
	send_to_char("Huh?\n\r",ch);
	return;
    }

    if (ch->power[DISC_VAMP_ANIM] < 4)
    {
        send_to_char("You must obtain at least level 4 in Animalism to use Share Spirits.\n\r",ch);
	return;
    }

    if ( ( familiar = ch->pcdata->familiar ) != NULL)
    {
	sprintf(buf,"You release %s.\n\r",familiar->short_descr);
	send_to_char( buf, ch );
	familiar->wizard = NULL;
	ch->pcdata->familiar = NULL;
	return;
    }

    if ( arg[0] == '\0' )
    {
	send_to_char( "What do you wish to Share spirits with?\n\r", ch );
	return;
    }

    if ( ( victim = get_char_room( ch, arg ) ) == NULL )
    {
	send_to_char( "They aren't here.\n\r", ch );
	return;
    }

    if ( ch == victim )
    {
	send_to_char( "What an intelligent idea!\n\r", ch );
	return;
    }

    if ( !IS_NPC(victim) )
    {
	send_to_char( "Not on players.\n\r", ch );
	return;
    }

    if (victim->wizard != NULL)
    {
	send_to_char( "You are unable to Share Spirits them.\n\r", ch );
	return;
    }

    if (victim->level > (ch->spl[RED_MAGIC] * 0.25))
    {
	send_to_char( "They are too powerful.\n\r", ch );
	return;
    }

    if ( ch->pcdata->condition[COND_THIRST] < 25 )
    {
	send_to_char("You have insufficient blood.\n\r",ch);
	return;
    }

    ch->pcdata->condition[COND_THIRST] -= 25;
    ch->pcdata->familiar = victim;
    victim->wizard = ch;
    act("You share your being with $N.",ch,NULL,victim,TO_CHAR);
    act("$n is staring at you!",ch,NULL,victim,TO_VICT);
    act("$n starts staring at $N",ch,NULL,victim,TO_NOTVICT);
    return;
}

void do_frenzy( CHAR_DATA *ch, char *argument )
{
/*
    char arg[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;

    argument = one_argument (argument, arg);
*/

    if ( IS_NPC(ch) )
	return;


    if ( ch->power[DISC_VAMP_ANIM] < 5 )
    {
	send_to_char("You need level 5 Animalism to use Frenzy.\n\r",ch);
	return;
    }
/*
    if ( ( victim = get_char_room( ch, arg ) ) == NULL )
    {
        send_to_char( "They aren't here.\n\r", ch );
        return;
    }    

    if ( IS_NPC(victim) )
	{
	  send_to_char("Not on NPC's!\n\r",ch);
	  return;
	}



    if (IS_CLASS(victim, CLASS_VAMPIRE) && arg[0] != '\0')
    {
	if ( victim->rage >= 25 )
	{
	    send_to_char("Their beast is already controlling them!\n\r",ch);
	    return;
	}

      if (number_percent ( )  <= 50)
	{
	    send_to_char("They shake off your attempt.\n\r",ch);
	    act("You shake off $n's attempt to frenzy you.\n\r",ch,NULL,victim,TO_VICT);
	    act("$N shakes off $n's attempt to frenzy $m.\n\r",ch,NULL,victim,TO_ROOM);
          return;
      }

      if (victim->beast == 0 )
	{
	  send_to_char("They are too calm to frenzy!\n\r",ch);
	  return;
	}

	if (!IS_VAMPAFF(victim, VAM_NIGHTSIGHT)) do_nightsight(victim,"");
	if (!IS_VAMPAFF(victim, VAM_FANGS)) do_fangs(victim,"");
	if (!IS_VAMPAFF(victim, VAM_CLAWS)) do_claws(victim,"");
      act("$n sends you into a frenzied rage!",ch,NULL,victim,TO_VICT);
	act("You excite the beast within $n!",victim,NULL,NULL,TO_CHAR);
	act("$n bares $s fangs and growls as $s inner beast consumes $m.",ch,NULL,NULL,TO_ROOM);
	ch->rage += number_range(10,20);
	if (victim->beast > 0) do_beastlike(victim,"");
	WAIT_STATE(ch,12);
	return;
    }
   
    send_to_char("But they are already in a rage!\n\r",ch);
    return;
*/

    if (IS_CLASS(ch, CLASS_VAMPIRE))
    {
      if (ch->beast >= 20)
      {
	if ( ch->rage >= ch->beast )
	{
	    send_to_char("Your beast is already controlling you.\n\r",ch);
	    return;
	}
      }
      if (ch->beast <= 19)
      {
        if ( ch->rage >= 20 )
        {
            send_to_char("Your beast is already controlling you.\n\r",ch);
            return;
        }
      } 

/*
        if (ch->beast == 0)
	{
	   send_to_char("You are too calm to use frenzy.\n\r",ch);
	   return;
	}

*/

	if (!IS_VAMPAFF(ch, VAM_NIGHTSIGHT)) do_nightsight(ch,"");
	if (!IS_VAMPAFF(ch, VAM_FANGS)) do_fangs(ch,"");
	if (!IS_VAMPAFF(ch, VAM_CLAWS)) do_claws(ch,"");
	send_to_char("You bare your fangs and growl as your inner beast consumes you.\n\r",ch);
	act("$n bares $s fangs and growls as $s inner beast consumes $m.",ch,NULL,NULL,TO_ROOM);
	ch->rage += number_range(20,30);
	if (ch->beast > 0) do_beastlike(ch,"");
	WAIT_STATE(ch,12);
	return;
    }
 
    send_to_char("But you are already in a rage!\n\r",ch);
    return;
}

void do_shroud( CHAR_DATA *ch, char *argument )
{
    if ( IS_NPC(ch) )
	return;

    if (!IS_CLASS(ch, CLASS_VAMPIRE) || (ch->power[DISC_VAMP_OBTE] < 1))
     {
	  send_to_char("You need level 1 Obtenebration to use this power.\n\r",ch);
	  return;
     }

    if ( IS_SET(ch->act, PLR_WIZINVIS) )
    {
	REMOVE_BIT(ch->act, PLR_WIZINVIS);
	send_to_char( "You step out from the shadows.\n\r", ch );
	act("$n emerges from the shadows.",ch,NULL,NULL,TO_ROOM);
    }
    else
    {
	send_to_char( "You slowly blend with the shadows.\n\r", ch );
	act("$n blends in with the shadows and disappears.",ch,NULL,NULL,TO_ROOM);
	SET_BIT(ch->act, PLR_WIZINVIS);
    }
    return;
}

void do_summon(CHAR_DATA *ch, char *argument)
{
    CHAR_DATA *victim;
    CHAR_DATA *mount;
    char arg [MAX_INPUT_LENGTH];

    argument = one_argument (argument,arg);


    if ( !IS_CLASS(ch, CLASS_VAMPIRE) )
    {
      do_nosummon(ch,"");
      return;
    }

    if (arg[0] == '\0')
    {
	send_to_char("Summon whom?\n\r",ch);
	return;
    }  

    if ( ( victim = get_char_world( ch, arg ) ) == NULL )
    {
	send_to_char( "They aren't here.\n\r", ch );
	return;
    }


    if   (victim == ch
    ||   victim->in_room == NULL
    ||   IS_SET(victim->in_room->room_flags, ROOM_SAFE)
    ||   IS_SET(victim->in_room->room_flags, ROOM_PRIVATE)
    ||   IS_SET(victim->in_room->room_flags, ROOM_SOLITARY)
    ||   IS_SET(victim->in_room->room_flags, ROOM_NO_RECALL)
    ||   victim->level >= ch->level + 5
    ||   victim->fighting != NULL
    ||   (!IS_NPC(victim) && !IS_IMMUNE(victim, IMM_SUMMON) ))
    
    {
	send_to_char( "You cannot locate them.\n\r", ch );
	return;
    }

    if (ch->power[DISC_VAMP_PRES] < 4)
    {
	send_to_char("You need level 4 in Presence to use this.\n\r",ch);
	return;
    }
    WAIT_STATE(ch,10);
    act( "$n disappears suddenly.", victim, NULL, NULL, TO_ROOM );
    char_from_room( victim );
    char_to_room( victim, ch->in_room );
    act( "$n arrives suddenly.", victim, NULL, NULL, TO_ROOM );
    act( "$N has summoned you!", victim, NULL, ch,   TO_CHAR );
    do_look( victim, "auto" );
    if ( (mount = victim->mount) == NULL ) return;
    char_from_room( mount );
    char_to_room( mount, get_room_index(victim->in_room->vnum) );
    do_look( mount, "auto" );
    return;
}


void do_drain( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *victim;
    char arg [MAX_INPUT_LENGTH];
    int dam;

    argument = one_argument( argument, arg );

    if ( IS_NPC(ch) )
	return;

    if (!IS_CLASS(ch, CLASS_VAMPIRE))
     {
	  send_to_char("Huh?\n\r",ch);
	  return;
     }

    if (ch->power[DISC_VAMP_THAN] < 5 )
     {
	send_to_char("You must obtain level 5 Thanatosis to use Drain Life.\n\r",ch);
	return;
     }
     
  
   if ( ( victim = get_char_room( ch, arg ) ) == NULL && (ch->fighting) == NULL)
    {
	send_to_char( "They aren't here.\n\r", ch );
	return;
    }
   if ((ch->fighting) != NULL)
   {
   victim = ch->fighting;
   }
    
   if ( arg[0] == '\0' && (ch->fighting) == NULL)
    {
	send_to_char( "Whose life do you wish to drain?\n\r", ch );
	return;
    }

    if (victim->hit < 500)
	{
	   send_to_char("They are far to hurt to do that!\n\r",ch);
	   return;
	}

	 if (is_safe(ch, victim))
	 {
       send_to_char("Not while in a safe room!\n\r", ch);
       return;
    }

    if (IS_NPC(victim))
	{
	  dam = ch->power[DISC_VAMP_THAN] * 10;
        victim->hit = (victim->hit - dam);
        if (dam > 500) dam = number_range( 450, 550 );
        ch->hit = (ch->hit + dam);
	if (ch->hit > ch->max_hit + 1000)
	ch->hit = ch->max_hit + 1000;
        act("You drain the lifeforce out of $N.", ch, NULL, victim, TO_CHAR);
        act("$n drains the lifeforce out of $N.", ch, NULL, victim, TO_NOTVICT);
        act("You feel $n draining your lifeforce.", ch, NULL, victim, TO_VICT);
        WAIT_STATE(ch,12);
	  return;
      }

    if (!IS_NPC(victim))
	{
	  dam = ch->power[DISC_VAMP_THAN] * 50;
        victim->hit = (victim->hit - dam);
        if (dam > 400) dam = number_range( 350, 450 );
        ch->hit = (ch->hit + dam);
        if (ch->hit > ch->max_hit + 1000)
        ch->hit = ch->max_hit + 1000;      
        act("You drain the lifeforce out of $N.", ch, NULL, victim, TO_CHAR);
        act("$n drains the lifeforce out of $N.", ch, NULL, victim, TO_NOTVICT);
        act("You feel $n draining your lifeforce.", ch, NULL, victim, TO_VICT);
        WAIT_STATE(ch,12);
	  return;
      }


}

void do_flamehands( CHAR_DATA *ch, char *argument)
{
    if ( IS_NPC(ch) )
     return;

    if (!IS_CLASS(ch, CLASS_VAMPIRE))
    {
	  send_to_char("Huh?\n\r",ch);
	  return;
    }

    if (ch->power[DISC_VAMP_PROT] < 5)
    {
	  send_to_char("You need level 5 Protean to use flaming hands.\n\r",ch);
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

void do_cauldron( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *victim;
    char arg1 [MAX_INPUT_LENGTH];
    char arg2 [MAX_INPUT_LENGTH];
    int value;
    int dam;

    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );

    value = is_number( arg2 ) ? atoi( arg2 ) : -1;

    if ( IS_NPC(ch) )
	return;

    if (!IS_CLASS(ch, CLASS_VAMPIRE))
     {
	  send_to_char("Huh?\n\r",ch);
	  return;
     }

    if (ch->power[DISC_VAMP_THAU] < 2)
     {
	send_to_char("You need at least level 2 Thaumaturgy to use this power.\n\r",ch);
	return;
     }


    if ( arg1[0] == '\0' )
    {
	send_to_char( "Whose blood do you wish to boil?\n\r", ch );
	return;
    }
     if ( ( victim = get_char_room( ch, arg1 ) ) == NULL )
    {
	send_to_char( "They aren't here.\n\r", ch );
	return;
    }

    if (arg2[0] == '\0' )
    {
	send_to_char( "How much blood do you want to use?\n\r", ch );
	return;
    }

    if (value > 200)
    {
	stc("You can only use 200 blood on this power.\n\r",ch);
	return;
    }


    if (ch->pcdata->condition[COND_THIRST] < value )
	{
	   send_to_char("You don't have that much blood!\n\r",ch);
	   return;
	}

if (is_safe(ch,victim) == TRUE) return;

    if (IS_NPC(victim))
	{
	dam = (value * 4);
        victim->hit = (victim->hit - dam);
        ch->pcdata->condition[COND_THIRST] = ch->pcdata->condition[COND_THIRST] - value;
        act("$N screams in agony as $m blood begins to boil.", ch, NULL, victim, TO_CHAR);
        act("$N screams in agony as $n causes his blood to boil.", ch, NULL, victim, TO_NOTVICT);
        act("You scream in agony as $n causes your blood to boil.", ch, NULL, victim, TO_VICT);
        WAIT_STATE(ch,12);
	  return;
      }

    if (!IS_NPC(victim))
	{
	  dam = value * 1.5;
        victim->hit = (victim->hit - dam);
	if (victim->pcdata->condition[COND_THIRST] < value)
	victim->pcdata->condition[COND_THIRST] = 1;
        victim->pcdata->condition[COND_THIRST] = victim->pcdata->condition[COND_THIRST] - value;
	ch->pcdata->condition[COND_THIRST] = ch->pcdata->condition[COND_THIRST] - value;
        act("$N screams in agony as $m blood begins to boil.", ch, NULL, victim, TO_CHAR);
        act("$N screams in agony as $n causes his blood to boil.", ch, NULL, victim, TO_NOTVICT);
        act("You scream in agony as $n causes your blood to boil.", ch, NULL, victim, TO_VICT);
        WAIT_STATE(ch,12);
	  return;
      }


}
void do_vset( CHAR_DATA *ch, char *argument )
{
    char arg1 [MAX_INPUT_LENGTH];
    char arg2 [MAX_INPUT_LENGTH];
    char arg3 [MAX_INPUT_LENGTH];
    char buf[MAX_STRING_LENGTH];
    CHAR_DATA *victim;
    int value;
    
    sprintf(buf,"%s: Vset %s",ch->name,argument);
    if (ch->level < NO_WATCH) do_watching(ch,buf);
/*
    smash_tilde( argument );
  */
  argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );
argument=one_argument(argument,arg3);
  /*
   strcpy( arg3, argument );
  */
    if ( arg1[0] == '\0' || arg2[0] == '\0' || arg3[0] == '\0' )
    {
	send_to_char( "\n\r",ch);
	send_to_char( "Syntax:vset <victim> <field>  <value>\n\r", ch );
	send_to_char( "\n\r", ch );
	send_to_char( "Field being one of:\n\r", ch );
	send_to_char( "    Animal Obtenebration Thau    \n\r",   ch );
	send_to_char( "    Serpentis Celerity Daim      \n\r",       ch ); 
        send_to_char( "    Potence Viciss Fortitude     \n\r",       ch ); 
        send_to_char( "    Dominate Protean Quietus     \n\r",	     ch );
	send_to_char( "    Auspex Presence Obfuscate    \n\r",       ch );
	send_to_char( " --------------------------------\n\r",       ch );
        send_to_char( "    Clan  Lord  Generation  Age  \n\r",       ch );
	send_to_char( " --------------------------------\n\r",       ch );
	return;
    }
    if ((victim = get_char_room(ch, arg1)) == NULL)
     {
       
      send_to_char("They arent here.\n\r",ch);
    return;
}

    
    value = is_number( arg3 ) ? atoi( arg3 ) : -1;
    /*
     * Set something.
     */
    if ( !str_cmp( arg2, "animal" ) )
    {
	if ( IS_NPC(victim) )
	{
	    send_to_char( "Not on NPC's.\n\r", ch );
	    return;
	}

	if ( value < 0 || value > 10 )
	{
	    send_to_char( "Range is from 0 to 10.\n\r", ch );
	    return;
	}


	if (IS_CLASS(victim, CLASS_VAMPIRE))
	victim->power[DISC_VAMP_ANIM] = value;
	send_to_char("Ok. \n\r",ch);
	return;
    }

    if ( !str_cmp( arg2, "thau" ) )
    {
	if ( IS_NPC(victim) )
	{
	    send_to_char( "Not on NPC's.\n\r", ch );
	    return;
	}

	if ( value < 0 || value > 10 )
	{
	    send_to_char( "Range is from 0 to 10.\n\r", ch );
	    return;
	}

	if (IS_CLASS(victim, CLASS_VAMPIRE))
	victim->power[DISC_VAMP_THAU] = value;
	send_to_char("Ok. \n\r",ch);
	return;
    }

    if ( !str_cmp( arg2, "Obtenebration" ) )
    {
	if ( IS_NPC(victim) )
	{
	    send_to_char( "Not on NPC's.\n\r", ch );
	    return;
	}

	if ( value < 0 || value > 10 )
	{
	    send_to_char( "Range is from 0 to 10.\n\r", ch );
	    return;
	}

	if (IS_CLASS(victim, CLASS_VAMPIRE))
	victim->power[DISC_VAMP_OBTE] = value;
	send_to_char("Ok. \n\r",ch);
	return;
    }

    if ( !str_cmp( arg2, "serpentis" ) )
    {
	if ( IS_NPC(victim) )
	{
	    send_to_char( "Not on NPC's.\n\r", ch );
	    return;
	}

	if ( value < 0 || value > 10 )
	{
	    send_to_char( "Range is from 0 to 10.\n\r", ch );
	    return;
	}

	if (IS_CLASS(victim, CLASS_VAMPIRE))
	victim->power[DISC_VAMP_SERP] = value;
	send_to_char("Ok. \n\r",ch);
	return;
    }

  if ( !str_cmp( arg2, "celerity" ) )
{

    if ( IS_NPC(victim) )
	{
	    send_to_char( "Not on NPC's.\n\r", ch );
	    return;
	}

	if ( value < 0 || value > 10 )
	{
	    send_to_char( "Range is from 0 to 10.\n\r", ch );
	    return;
	}

	if (IS_CLASS(victim, CLASS_VAMPIRE))
	victim->power[DISC_VAMP_CELE] = value;
	send_to_char("Ok. \n\r",ch);
	return;
    }

    if ( !str_cmp( arg2, "presence" ) )
    {
	if ( IS_NPC(victim) )
	{
	    send_to_char( "Not on NPC's.\n\r", ch );
	    return;
	}

	if ( value < 0 || value > 10 )
	{
	    send_to_char( "Range is from 0 to 10.\n\r", ch );
	    return;
	}

	if (IS_CLASS(victim, CLASS_VAMPIRE))
	victim->power[DISC_VAMP_PRES] = value;
	send_to_char("Ok. \n\r",ch);
	return;
    }

    if ( !str_cmp( arg2, "auspex" ) )
    {
	if ( IS_NPC(victim) )
	{
	    send_to_char( "Not on NPC's.\n\r", ch );
	    return;
	}

	if ( value < 0 || value > 10 )
	{
	    send_to_char( "Range is from 0 to 10.\n\r", ch );
	    return;
	}

	if (IS_CLASS(victim, CLASS_VAMPIRE))
	victim->power[DISC_VAMP_AUSP] = value;
	send_to_char("Ok. \n\r",ch);
	return;
    }

  if ( !str_cmp( arg2, "potence" ) )
    {

    if ( IS_NPC(victim) )
	{
	    send_to_char( "Not on NPC's.\n\r", ch );
	    return;
	}

	if ( value < 0 || value > 10 )
	{
	    send_to_char( "Range is from 0 to 10.\n\r", ch );
	    return;
	}

	if (IS_CLASS(victim, CLASS_VAMPIRE))
	victim->power[DISC_VAMP_POTE] = value;
	send_to_char("Ok. \n\r",ch);
	return;
    }

    if ( !str_cmp( arg2, "viciss" ) )
    {
	if ( IS_NPC(victim) )
	{
	    send_to_char( "Not on NPC's.\n\r", ch );
	    return;
	}

	if ( value < 0 || value > 10 )
	{
	    send_to_char( "Range is from 0 to 10.\n\r", ch );
	    return;
	}

	if (IS_CLASS(victim, CLASS_VAMPIRE))
	victim->power[DISC_VAMP_VICI] = value;
	send_to_char("Ok. \n\r",ch);
	return;
    }

    if ( !str_cmp( arg2, "fortitude" ) )
    {
	if ( IS_NPC(victim) )
	{
	    send_to_char( "Not on NPC's.\n\r", ch );
	    return;
	}

	if ( value < 0 || value > 10 )
	{
	    send_to_char( "Range is from 0 to 10.\n\r", ch );
	    return;
	}

	if (IS_CLASS(victim, CLASS_VAMPIRE))
	victim->power[DISC_VAMP_FORT] = value;
	send_to_char("Ok. \n\r",ch);
	return;
    }

        if ( !str_cmp( arg2, "quietus" ) )
    {
	if ( IS_NPC(victim) )
	{
	    send_to_char( "Not on NPC's.\n\r", ch );
	    return;
	}

	if ( value < 0 || value > 10 )
	{
	    send_to_char( "Range is from 0 to 10.\n\r", ch );
	    return;
	}

	if (IS_CLASS(victim, CLASS_VAMPIRE))
	victim->power[DISC_VAMP_QUIE] = value;
	send_to_char("Ok. \n\r",ch);
	return;
    }

  if ( !str_cmp( arg2, "dominate" ) )
   { 

    if ( IS_NPC(victim) )
	{
	    send_to_char( "Not on NPC's.\n\r", ch );
	    return;
	}

	if ( value < 0 || value > 10 )
	{
	    send_to_char( "Range is from 0 to 10.\n\r", ch );
	    return;
	}

	if (IS_CLASS(victim, CLASS_VAMPIRE))
	victim->power[DISC_VAMP_DOMI] = value;
	send_to_char("Ok. \n\r",ch);
	return;
    }

    if ( !str_cmp( arg2, "protean" ) )
    {
	if ( IS_NPC(victim) )
	{
	    send_to_char( "Not on NPC's.\n\r", ch );
	    return;
	}

	if ( value < 0 || value > 10 )
	{
	    send_to_char( "Range is from 0 to 10.\n\r", ch );
	    return;
	}

	if (IS_CLASS(victim, CLASS_VAMPIRE))
	victim->power[DISC_VAMP_PROT] = value;
	send_to_char("Ok. \n\r",ch);
	return;
    }

    if ( !str_cmp( arg2, "daim" ) )
    {
	if ( IS_NPC(victim) )
	{
	    send_to_char( "Not on NPC's.\n\r", ch );
	    return;
	}

	if ( value < 0 || value > 10 )
	{
	    send_to_char( "Range is from 0 to 10.\n\r", ch );
	    return;
	}

	if (IS_CLASS(victim, CLASS_VAMPIRE))
	victim->power[DISC_VAMP_DAIM] = value;
	send_to_char("Ok. \n\r",ch);
	return;
    }

    if ( !str_cmp( arg2, "age" ) )
    {
	if ( IS_NPC(victim)) 
	{
	   send_to_char("Not on Mobiles.\n\r",ch);
	   return;
	}

	if (!IS_CLASS(ch,CLASS_VAMPIRE))
	{
	   send_to_char("Only on Vampires!\n\r",ch);
	   return;
	}

	     if ( !str_cmp( arg3, "childe"     ) )
{
 victim->pcdata->rank = AGE_CHILDE;
send_to_char("Ok.\n\r",ch);
return;
}
	else if ( !str_cmp( arg3, "ancilla"    ) )
{
 victim->pcdata->rank = AGE_ANCILLA;
send_to_char("Ok.\n\r",ch);
return;
}
	else if ( !str_cmp( arg3, "elder"      ) )
{
 victim->pcdata->rank = AGE_ELDER;
send_to_char("Ok.\n\r",ch);
return;
}
	else if ( !str_cmp( arg3, "methuselah" ) ) 
{
victim->pcdata->rank = AGE_METHUSELAH;
send_to_char("Ok.\n\r",ch);
return;
}
	else if (!IS_NPC(victim))
	{
	   
           send_to_char( "Age should be one of the following:\n\r Childe,
Ancilla, Elder, or Methuselah.\n\r", ch );
	    return;
	}

    }


    if ( !str_cmp( arg2, "blood" ) )
    {
	if ( IS_NPC(victim) )
	{
	    send_to_char( "Not on NPC's.\n\r", ch );
	    return;
	}

	if ( value < 0 || value > 1000 )
	{
	    send_to_char( "Blood range is 0 to 1000.\n\r", ch );
	    return;
	}

	victim->pcdata->condition[COND_THIRST] = value;
	send_to_char("Ok.\n\r",ch);
	return;
    }

    


    if ( !str_cmp( arg2, "lord" ) )
    {
	if ( IS_NPC(victim) )
	{
	    send_to_char( "Not on NPC's.\n\r", ch );
	    return;
	}

        smash_tilde(argument);
        capitalize(arg3);
	free_string( victim->lord );
	victim->lord = str_dup( arg3 );
	send_to_char("Ok.\n\r",ch);
	return;
    }

    if ( !str_cmp( arg2, "generation" ) )
    {
	if ( IS_NPC(victim) )
	{
	    send_to_char( "Not on NPC's.\n\r",ch );
	    return;
	}
	if (!IS_CLASS(ch, CLASS_VAMPIRE))
	{
	    send_to_char( "Only on Vampires!\n\r",ch);
	    return;
	}

	if ( value < 1 || value > 15 )
	{
	   send_to_char("Valid range is 1 - 15 \n\r",ch);
	   return;
	}

	victim->generation = value;
	send_to_char("Ok.\n\r",ch);
	return;
     }

   
    do_vset( ch, "" );
    return;
}


void do_bonemod( CHAR_DATA *ch, char *argument )
{
    char arg1 [MAX_INPUT_LENGTH];

    smash_tilde( argument );
    argument = one_argument( argument, arg1 );


	if (IS_NPC(ch)) return;

	if (!IS_CLASS(ch, CLASS_VAMPIRE))
	{
	   send_to_char("Huh?\n\r",ch);
	   return;
	}
	if (ch->power[DISC_VAMP_VICI] < 3)
	{
	   send_to_char("You need level 3 Vicissitude to use this power.\n\r",ch);
	   return;
	}

    if ( arg1[0] == '\0')
    {
	send_to_char( "Syntax: bonemod <modification>\n\r",	ch );
	send_to_char( "Horns, Wings, Head, Exoskeleton, Tail\n\r",ch);
        return;
    }


    /*
     * Set something.
     */
    if ( !str_cmp( arg1, "horns" ) )
    {
	
 	if (!IS_VAMPAFF(ch, VAM_HORNS))
	 {
	  SET_BIT(ch->pcdata->stats[UNI_AFF], VAM_HORNS);
	  send_to_char("You pull a set of horns out of your head.\n\r",ch);
	  act("$n pulls a set of horns from his head!\n\r",ch,NULL,NULL,TO_ROOM);
	  return;
       }
	
	if (IS_VAMPAFF(ch, VAM_HORNS))
	 {
	  REMOVE_BIT(ch->pcdata->stats[UNI_AFF], VAM_HORNS);
	  send_to_char("You push your horns back into your head.\n\r",ch);
	  act("$n pushes $n's horns back into $n head.\n\r",ch,NULL,NULL,TO_ROOM); 
	  return;

	 }
    }

    if ( !str_cmp( arg1, "wings" ) )
    {
	
 	if (!IS_VAMPAFF(ch, VAM_WINGS))
	 {
	  SET_BIT(ch->pcdata->stats[UNI_AFF], VAM_WINGS);
	  send_to_char("You pull a pair of leathery wings from your back.\n\r",ch);
	  act("$n pulls a pair of leathery wings from $s back!\n\r",ch,NULL,NULL,TO_ROOM);
	  return;
       }
	
	if (IS_VAMPAFF(ch, VAM_WINGS))
	 {
	  REMOVE_BIT(ch->pcdata->stats[UNI_AFF], VAM_WINGS);
	  send_to_char("You push your wings into your back.\n\r",ch);
	  act("$n pushes $s wings into $s back.\n\r",ch,NULL,NULL,TO_ROOM);
	  return;

	 }
    }    

     if ( !str_cmp( arg1, "exoskeleton" ) )
    {
	
 	if (!IS_VAMPAFF(ch, VAM_EXOSKELETON))
	 {
	  SET_BIT(ch->pcdata->stats[UNI_AFF], VAM_EXOSKELETON);
	  send_to_char("Your skin is covered by a hard exoskeleton.\n\r",ch);
	  act("$n's skin is covered by a hard exoskeleton!\n\r",ch,NULL,NULL,TO_ROOM);
	  return;
       }
	
	if (IS_VAMPAFF(ch, VAM_EXOSKELETON))
	 {
	  REMOVE_BIT(ch->pcdata->stats[UNI_AFF], VAM_EXOSKELETON);
	  send_to_char("Your exoskeleton slowly disappears under your skin.\n\r",ch);
	  act("$n's hard exoskeleton disappears under $s skin.\n\r",ch,NULL,NULL,TO_ROOM);
	  return;

	 }
    }    

     if ( !str_cmp( arg1, "tail" ) )
    {
	
 	if (!IS_VAMPAFF(ch, VAM_TAIL))
	 {
	  SET_BIT(ch->pcdata->stats[UNI_AFF], VAM_TAIL);
	  send_to_char("Your spine extends out into a long, pointed tail.\n\r",ch);
	  act("$n's spine extends to form a long pointed tail!\n\r",ch,NULL,NULL,TO_ROOM);
	  return;
       }
	
	if (IS_VAMPAFF(ch, VAM_TAIL))
	 {
	  REMOVE_BIT(ch->pcdata->stats[UNI_AFF], VAM_TAIL);
	  send_to_char("Your tail slowly retracts into your spine.\n\r",ch);
	  act("$n's tail shrinks and vanishes into $s spine.\n\r",ch,NULL,NULL,TO_ROOM);
	  return;

	 }
    }    

     if ( !str_cmp( arg1, "head" ) )
    {
	
 	if (!IS_VAMPAFF(ch, VAM_HEAD))
	 {
	  SET_BIT(ch->pcdata->stats[UNI_AFF], VAM_HEAD);
	  send_to_char("Your head transforms into that of a fierce lion.\n\r",ch);	  
	  act("$n's head transforms into that of a fierce lion!\n\r",ch,NULL,NULL,TO_ROOM);
	  return;
       }
	
	if (IS_VAMPAFF(ch, VAM_HEAD))
	 {
	  REMOVE_BIT(ch->pcdata->stats[UNI_AFF], VAM_HEAD);
	  send_to_char("Your head slowly resumes it's normal form.\n\r",ch);
	  act("$n's head resumes its normal form.\n\r",ch,NULL,NULL,TO_ROOM);
	  return;

	 }
    }    




    /*
     * Generate usage message.
     */
    do_bonemod( ch, "" );
    return;
}

