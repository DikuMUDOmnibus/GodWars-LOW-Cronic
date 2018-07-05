/*   Daemons    Ala Dunkirk? */

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

void frost_breath args(( CHAR_DATA *ch, CHAR_DATA *victim, bool all));
void fire_effect  args(( void *vo, int level, int dam, int target));
DECLARE_DO_FUN( do_stance );

void do_deathsense(CHAR_DATA *ch, char *argument)
{

    if (ch->power[DISC_DAEM_NETH] < 2)
    {
	stc("You need level 2 Nether to use DeathSense.\n\r",ch);
	return;
    }

    if (IS_SET(ch->act, PLR_HOLYLIGHT))
    {
	TOGGLE_BIT(ch->act, PLR_HOLYLIGHT);
	stc("Your unholy sight fades.\n\r",ch);
	return;
    }
    if (!IS_SET(ch->act, PLR_HOLYLIGHT))
    {
	TOGGLE_BIT(ch->act, PLR_HOLYLIGHT);
	stc("Your eyes glow with unholy sight.\n\r",ch);
	return;
    }
  return;

}

void do_leech(CHAR_DATA *ch, char *argument)
{
    CHAR_DATA *victim = NULL;
    char buf[MAX_STRING_LENGTH];

/*    if (ch->power[DISC_DAEM_NETH] < 4)
    {*/
	if (!IS_DEMPOWER( ch, DEM_LEECH))
    {

	stc("You do not have that power.\n\r",ch);
	return;
    }

    if (argument[0] == '\0')
    {
	stc("Who's life do you wish to leech off of?\n\r", ch);
	return;
    }

    if ((victim = get_char_room(ch, argument)) == NULL)
    {
	    stc("They aren't here.\n\r", ch);
	    return;
    }

      if (is_safe(ch, victim)) return;

	WAIT_STATE(ch, 10);

	act("$n stares intently at $N.", ch, NULL, victim, TO_NOTVICT);
	act("You stare intently at $N.", ch, NULL, victim, TO_CHAR);
	act("$n stares intently at you.", ch, NULL, victim, TO_VICT);

      if (victim->hit >= 1000)
      {
	  sh_int power;
	  sh_int dam;
	  power = 20;
	  dam = dice(power, power * 2);
	  if (dam > 300) dam = 300 + (dam / 10);
	  victim->hit -= dam;
	  ch->hit += dam;
	  if (ch->hit >= ch->max_hit + 500)
		ch->hit = ch->max_hit + 500;
	  sprintf(buf,"#1You absorb %d hitpoints.#n\n\r",dam);
	  stc(buf,ch);
	  sprintf(buf,"#1%s absorbed %d of your hitpoints!#n\n\r",ch->name,dam);
	  stc(buf,victim);
	  }
      else stc("Nothing seemed to happen.\n\r",ch);
	return;

}

void do_entomb (CHAR_DATA *ch, char *argument)
{
    int door;

    if (!IS_DEMPOWER(ch, DEM_ENTOMB))
    {
	stc("You have not been granted that power.\n\r",ch);
	return;
    }

    if (ch->in_room->sector_type == SECT_AIR)
    {
	send_to_char("This ability cannot be used in the air.\n\r", ch);
	return;
    }

    if (!TIME_UP(ch, TIMER_ENTOMB)
	&& ch->level < 12)
    {
	send_to_char("This ability can only be used once every 12 hours.\n\r", ch);
	return;
    }

    for (door = 0 ; door < 4 ; door++)
    {
	if (ch->in_room->exit[door] != NULL)
	    make_wall(ch->in_room, door, EX_ICE_WALL);
    }

    ch->hit += (ch->hit *3/2);
       if (ch->hit > ch->max_hit)
		ch->hit = ch->max_hit;
    SET_TIMER(ch, TIMER_ENTOMB, 12);

    act("$n calls forth sheets of ice, blocking your path in every direction.", ch, NULL, NULL, TO_ROOM);
    send_to_char("You call forth sheets of ice to entrap your prey.\n\r", ch);
    send_to_char("You grin wickedly.\n\r",ch);
    do_say(ch,"Prepare to die! Muahahahaha.");

    WAIT_STATE(ch, 20);
}

void do_gust( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA       *victim;
    ROOM_INDEX_DATA *to_room = NULL;
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

    if (ch->power[DISC_DAEM_GELU] < 7)
    {
	stc("You must achieve a level seven mastery of Geluge to use Gust.\n\r",ch);
	return;

    }

    if (!TIME_UP(ch, TIMER_CAN_GUST) && ch->level < 120)
    {
	send_to_char("You are out of breath.\n\r", ch);
	return;
    }
     
    if ( arg1[0] == '\0' )
    {
	send_to_char("Who do you wish to blow a gust of wind at?\n\r", ch);
	return;
    }
    if ( ( victim = get_char_room( ch, arg1 ) ) == NULL )
    {
	send_to_char("They are not here.\n\r", ch);
	return;
    }
    
    if ( victim == ch )
    {
	send_to_char( "Not on yourself!\n\r", ch );
	return;
    }
     
    if ( is_safe( ch, victim ) )
      return;
     
    if ( victim->size > ch->size)
    {
	send_to_char("They are too large to affect.\n\r",ch);
	return;
    }
    
    WAIT_STATE( ch, 12 );
    if ( !IS_NPC(ch) && number_percent( )/2 > ch->power[DISC_DAEM_GELU] + ch->generation)
    {
	    send_to_char("You huff and puff but they do not budge.\n\r",ch);
	    act("$n blows a gust of cold air into the room.", ch, NULL, NULL, TO_ROOM);
	    return;
    }
    SET_TIMER(ch, TIMER_CAN_GUST, dice(1,3)+1);    
    act("You breathe a gust of cold air at $N.", ch, NULL, victim, TO_CHAR);
    act("$n breathes a gust of cold air at $N.", ch, NULL, victim, TO_NOTVICT);
    act("$n breathes a gust of cold air at you.", ch, NULL, victim, TO_VICT);

    rev_dir = 0;
    if ( arg2[0] == '\0' )
	door = number_range(0,3);
    else
    {
	if      ( !str_prefix( arg2, "north" ) )
	    door = 0;
	else if ( !str_prefix( arg2, "east" ) )
	    door = 1;
	else if ( !str_prefix( arg2, "south" ) )
	    door = 2;
	else if ( !str_prefix( arg2, "west" ) )
	    door = 3;
	else
	{
	    send_to_char("You can only blow people north, south, east or west.\n\r", ch);
	    return;
	}
    }
     
    if (door == 0) {sprintf(direction,"north");rev_dir = 2;}
    if (door == 1) {sprintf(direction,"east");rev_dir = 3;}
    if (door == 2) {sprintf(direction,"south");rev_dir = 0;}
    if (door == 3) {sprintf(direction,"west");rev_dir = 1;}
     
    if (( pexit = ch->in_room->exit[door]) == NULL)
    {
	sprintf(buf,"$N flies into the %s wall.", direction);
	act(buf,ch,NULL,victim,TO_NOTVICT);
	sprintf(buf,"$N flies into the %s wall.", direction);
	act(buf,ch,NULL,victim,TO_CHAR);
	sprintf(buf,"You smash into the %s wall.", direction);
	act(buf,ch,NULL,victim,TO_VICT);
	dam = dice(victim->size*3+1, ch->power[DISC_DAEM_GELU] * 3);
	  damage_old(ch, victim, dam, 0, DAM_OTHER, FALSE);
	return;
    }
    
    pexit = victim->in_room->exit[door];
    if (IS_SET(pexit->exit_info, EX_CLOSED) )
    {
	if (IS_SET(pexit->exit_info, EX_LOCKED))
	    REMOVE_BIT(pexit->exit_info, EX_LOCKED);
	if (IS_SET(pexit->exit_info, EX_CLOSED))
	    REMOVE_BIT(pexit->exit_info, EX_CLOSED);
	sprintf(buf,"$N is blown into the air and flies %s.", direction);
	act(buf,ch,NULL,victim,TO_NOTVICT);
	sprintf(buf,"$N is blown into the air and flies %s.", direction);
	act(buf,ch,NULL,victim,TO_CHAR);
	sprintf(buf,"You are blown into the air and fly %s.", direction);
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
	    dam = dice(victim->size*3+1, ch->power[DISC_DAEM_GELU] * 7);
		damage_old(ch, victim, dam, 0, DAM_OTHER, FALSE);
	}
    }
    else
    {
	sprintf(buf,"$N flies %s.", direction);
	act(buf,ch,NULL,victim,TO_NOTVICT);
	sprintf(buf,"$N flies %s.", direction);   
	act(buf,ch,NULL,victim,TO_CHAR);
	sprintf(buf,"You fly %s.", direction);  
	act(buf,ch,NULL,victim,TO_VICT);
	if (door == 0) sprintf(direction,"south");
	if (door == 1) sprintf(direction,"west");
	if (door == 2) sprintf(direction,"north");
	if (door == 3) sprintf(direction,"east");
	  char_from_room(victim);
	char_to_room(victim,pexit->to_room);
	sprintf(buf,"$n comes flying in from the %s.", direction);
	act(buf,victim,NULL,NULL,TO_ROOM);
	dam = dice(victim->size*3+1, ch->power[DISC_DAEM_GELU] * 5);
	  damage_old(ch, victim, dam, 0, DAM_OTHER, FALSE);
    }
    return;
}

void do_caust(CHAR_DATA *ch, char *argument)
{
    OBJ_DATA *obj;

    if (!IS_DEMPOWER(ch, DEM_CAUST))
    {
	stc("You have not been granted that power.\n\r",ch);
	return;
    }

    if (argument[0] == '\0')
    {
	send_to_char("Which item do you wish to caust?\n\r", ch);
	return;
    }

    if ( (obj = get_obj_carry(ch, argument)) == NULL)
    {
	send_to_char("You are not carrying that item.\n\r", ch);
	return;
    }

    if (obj->item_type != ITEM_WEAPON)
    {
	send_to_char("That is not a weapon!\n\r", ch);
	return;
    }

    if (dice(1, 100) == 1)
    {
	act("$p is disintegrated by $n's poison.", ch,obj, NULL, TO_ROOM);
	act("Your poison eats through $p.", ch,obj, NULL, TO_CHAR);
	extract_obj(obj);
	return;
    }

    if (IS_WEAP(obj, WEAPON_POISON))
    {
	act("$p is already coated with deadly poison.", ch, obj, NULL, TO_CHAR);
	return;
    }

    act("You run your tongue along $p, coating it with a sickly venom.", ch, obj, NULL, TO_CHAR);
    act("$n runs $m tongue along $p, coating it with a sickly venom.", ch, obj, NULL, TO_ROOM);

    WAIT_STATE(ch, 8);
    SET_BIT(obj->weapflags, WEAPON_POISON);
    obj->value[1] += ch->generation;
    obj->value[2] += ch->generation;
}

void do_chaosport( CHAR_DATA *ch, char *argument )
{
char buf[MAX_STRING_LENGTH];
CHAR_DATA *victim;

 /*if (!victim = ch->fighting)
   return; */
 
 
    if (ch->power[DISC_DAEM_DISC] < 4)
    {
	stc("You need at least level 4 in Discord to use Chaos Portal\n\r",ch);
	return;
    }

    if (IS_AFFECTED(ch, AFF_WEBBED))
    {
	send_to_char("You are unable to move with all this sticky webbing on.\n\r",ch);
	return;
    }
  if (ch->fighting && IS_CLASS(ch->fighting, CLASS_MONK) &&

IS_SET(ch->fighting->newbits,NEW_JAWLOCK) && number_percent( ) > 30)
{
sprintf(buf,"Not with god holding you!");
send_to_char(buf,ch);
act("$n cant escape god's mighty hold!",ch,NULL,victim,TO_ROOM);
return;
}

   if (ch->fighting && IS_CLASS(ch->fighting, CLASS_WEREWOLF) && 

IS_SET(ch->fighting->newbits,NEW_JAWLOCK) && number_percent( ) > 10)
{
sprintf(buf,"Not with %s's jaws clamped on your neck!",ch->fighting->name);
send_to_char(buf,ch);
act("$n cant escape $N's clamped jaws!",ch,NULL,ch->fighting,TO_ROOM);
return;
}
    if (IS_AFFECTED(ch,AFF_TENDRILS))
{
send_to_char("You are unable to move while tangled in tendrils.\n\r",ch);
return;
}
    
    if (ch->fighting != NULL)
    {
	if (number_range(1,4) == 3)
	{
	    stc("The powers of Chaos fail you.\n\r",ch);
	    act("$n tries in vain to summon a chaos portal.",ch,NULL,NULL,TO_ROOM);
	    SET_TIMER(ch, TIMER_CHAOSPORT, 3);
		WAIT_STATE(ch, 12);
	    return;
	}
    }

    act("$n disappears through a rift in time.",ch,NULL,NULL,TO_ROOM);
    stc("You are swept away through a rift in time.\n\r",ch);
    if (ch->fighting != NULL)
	stop_fighting( ch, TRUE );
    char_from_room(ch);
    char_to_room(ch, get_random_room(ch));
    stc("You reappear in a swirling storm of chaos.\n\r",ch);
    act("$n appears in a swirling wind of Chaos.",ch,NULL,NULL,TO_ROOM);
    do_look(ch,"auto");
	WAIT_STATE(ch, 12);
    SET_TIMER(ch, TIMER_CHAOSPORT, 10);
    return;
}


void do_bane (CHAR_DATA *ch, char *argument)
{
    OBJ_DATA *obj;

    if (ch->power[DISC_DAEM_CORR] < 3)
    {
	stc("You need level three Corruption to use this power.\n\r",ch);
	return;
    }

    if (argument[0] == '\0')
    {
	send_to_char("Who do you wish to call the Bane against?\n\r", ch);
	return;
    }

    if (!IS_OUTSIDE(ch) && ch->in_room->sector_type != SECT_FOREST)
    { 
	send_to_char("You must be outside to use Bane.\n\r", ch);
	return;
    }
    
    if (!TIME_UP(ch, TIMER_CAN_CALL_ROCKS) )
    {
	send_to_char("You call out but nothing comes.\n\r", ch);
	return;
    }

    act("$n cries out and a menacing sword appears to aid $s.", ch, NULL, NULL,TO_ROOM);
    send_to_char("A wicked longsword joins your battle against good.\n\r",ch);

    obj = create_object(get_obj_index(89), 0);
    obj_to_room(obj, ch->in_room);
    obj->timer = ch->generation / 2 + 2;
    SET_BIT(obj->extra_flags2, ITEM_ATTACK_GOOD);
    SET_TIMER(ch, TIMER_CAN_CALL_ROCKS, 3);

    if (ch->generation > 5)
    {
	obj = create_object(get_obj_index(89), 0);
	obj_to_room(obj, ch->in_room);
	obj->timer = ch->generation / 2 + 1;
	SET_BIT(obj->extra_flags2, ITEM_ATTACK_GOOD);
    }

    if (ch->generation > 10)
    {
	obj = create_object(get_obj_index(89), 0);
	obj_to_room(obj, ch->in_room);
	obj->timer = ch->generation / 2 + 1;
	SET_BIT(obj->extra_flags2, ITEM_ATTACK_GOOD);
    }
}

void do_unnerve(CHAR_DATA *ch, char *argument)
{
    CHAR_DATA *victim;

    if (!IS_DEMPOWER(ch, DEM_UNNERVE))
    {
	stc("You have not been granted that power.\n\r",ch);
	return;
    }

    if (argument[0] == '\0')
    {
	stc("Who do you wish to unnerve?\n\r", ch);
	return;
    }

    if ( ( victim = get_char_room(ch, argument) ) == NULL)
    {
	stc("They aren't here.\n\r", ch);
	return;
    }

    WAIT_STATE(ch, 7);
    do_say(ch,"#1Xeus Dominus Mortai!#n");
    do_stance(victim, "");
}

void do_wfreeze(CHAR_DATA *ch, char *argument)
{
    OBJ_DATA *obj;

    if (!IS_DEMPOWER(ch, DEM_FREEZEWEAPON))
    {
	stc("You have not been granted that power.\n\r",ch);
	return;
    }

    if (argument[0] == '\0')
    {
	send_to_char("Which item do you wish to freeze?\n\r", ch);
	return;
    }

    if ( (obj = get_obj_carry(ch, argument)) == NULL)
    {
	send_to_char("You are not carrying that item.\n\r", ch);
	return;
    }

    if (obj->item_type != ITEM_WEAPON)
    {
	send_to_char("That is not a weapon!\n\r", ch);
	return;
    }

    if (dice(1, 100) == 1)
    {
	act("$p freezes and shatters.", ch,obj, NULL, TO_ROOM);
	act("$p freezes and shatters.", ch,obj, NULL, TO_CHAR);
	extract_obj(obj);
	return;
    }

    if (IS_WEAP(obj, WEAPON_FROST))
    {
	act("$p is already frozen.", ch, obj, NULL, TO_CHAR);
	return;
    }

    act("$p is surrounded by ice crystals.", ch, obj, NULL, TO_CHAR);
    act("$p, carried by $n is surrounded by ice crystals.", ch, obj, NULL, TO_ROOM);

    WAIT_STATE(ch, 8);
    SET_BIT(obj->weapflags, WEAPON_FROST);
}



void do_transform( CHAR_DATA *ch, char *argument )
{
    OBJ_DATA *obj;
    int trans = 0;

    if (IS_NPC(ch)) return;

    if (ch->power[DISC_DAEM_MORP] < 2)
    {
	stc("You must obtain a level two mastery of Morphosis.\n\r",ch);
	return;
    }

    if (argument[0] == '\0')
    {
	send_to_char("You can transform into the following items:\n\r",ch);
	send_to_char("    A Daemonic Helm\n\r",ch);
	if (ch->power[DISC_DAEM_MORP] > 2)
	    send_to_char("    Black Steel Greaves\n\r",ch);
	if (ch->power[DISC_DAEM_MORP] > 3)
	{
	    send_to_char("    A Black Opera Cloak\n\r",ch);
	    send_to_char("    Daemonic Plate Armor\n\r",ch);
	}
	if (ch->power[DISC_DAEM_MORP] > 4)
	{
	    send_to_char("    Ring of Cacophony\n\r",ch);
	    send_to_char("    Blood-Ruby Sword\n\r",ch);
	}
	if (ch->power[DISC_DAEM_MORP] > 5)
	    send_to_char("    Shield of Bone\n\r",ch);
	return;
    }

    if (!str_cmp(argument, "back") )
    {
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
	char_to_room(ch,get_room_index(ROOM_VNUM_ALTAR));
      }
	return;
    }


	 if (!str_prefix(argument, "helm")
	 && ch->power[DISC_DAEM_MORP] > 1)
	trans = OBJ_VNUM_DHELM;
    else if (!str_prefix(argument, "greaves")
	 && ch->power[DISC_DAEM_MORP] > 2)
	trans = OBJ_VNUM_DGREAVES;
    else if (!str_prefix(argument, "cloak")
	&& ch->power[DISC_DAEM_MORP] > 3)
	trans = OBJ_VNUM_DCLOAK;
    else if (!str_prefix(argument, "armor")
	&& ch->power[DISC_DAEM_MORP] > 3)
	trans = OBJ_VNUM_DARMOR;
    else if (!str_prefix(argument, "ring")
	&& ch->power[DISC_DAEM_MORP] > 4)
	trans = OBJ_VNUM_DRING;
    else if (!str_prefix(argument, "sword")
	&& ch->power[DISC_DAEM_MORP] > 4)
	trans = OBJ_VNUM_DSWORD;
    else if (!str_prefix(argument, "shield")
	&& ch->power[DISC_DAEM_MORP] > 5)
	trans = OBJ_VNUM_DSHIELD;
    else
    {
	do_transform(ch, "");
	return;
    }   

    if ((obj = create_object(get_obj_index(trans),60)) == NULL)
    {
	send_to_char( "You are unable to polymorph into that weapon.\n\r", ch);
	return;
    }

    if (IS_AFFECTED(ch,AFF_WEBBED))
    {
	send_to_char( "Not with all this sticky webbing on.\n\r", ch);
	return;
    }

    obj_to_room(obj,ch->in_room);
    act("$n transforms into $p and falls to the ground.",ch,obj,NULL,TO_ROOM);
    act("You transform into $p and fall to the ground.",ch,obj,NULL,TO_CHAR);
    ch->pcdata->obj_vnum = trans;
    obj->chobj = ch;
    ch->pcdata->chobj = obj;
    SET_BIT(ch->affected_by, AFF_POLYMORPH);
    SET_BIT(ch->extra, EXTRA_OSWITCH);
    SET_TIMER(ch, TIMER_DAEMONIC_TRANSFORM , 24);
    free_string(ch->morph);
    ch->morph = str_dup(obj->short_descr);
    if (!str_cmp(argument, "sword"))
    {
	SET_BIT(obj->weapflags, WEAPON_FLAMING);
	SET_BIT(obj->weapflags, WEAPON_VAMPIRIC);
	SET_BIT(obj->weapflags, WEAPON_FROST);
	SET_BIT(obj->weapflags, WEAPON_SUNBLADE);
	SET_BIT(obj->weapflags, WEAPON_ELE_EARTH);
	SET_BIT(obj->weapflags, WEAPON_ELE_AIR);
	SET_BIT(obj->weapflags, WEAPON_SHOCKING);
    }
    return;
}


int get_polymorph_power (CHAR_DATA *ch)
{
    if (ch->class == CLASS_DEMON)
	return ch->power[DISC_DAEM_MORP] - 1;

    if (ch->level == 12)
	return 100;

    return 0;
}

void do_polymorph (CHAR_DATA *ch, char *argument)
{
    int i, col = 0;
    char buf[MAX_INPUT_LENGTH];
    bool changed = FALSE;


    if (ch->power[DISC_DAEM_MORP] < 2 )
    {
	stc("You require level 2 morphosis.\n\r",ch);
	return;
    }

    if (argument[0] == '\0')
    {
	stc("Syntax : polymorph [<form> list back]\n\r", ch);
	return;
    }

    if (!str_cmp(argument, "list"))
    {
	cent_to_char("Available forms", ch);
	cent_to_char("---===> <===---", ch);

	for (i = 0 ; i < MAX_FORMS ; i++)
	{
	    if (form_data[i].polymorph_level > 0
		&& form_data[i].polymorph_level <= get_polymorph_power(ch)
		&& form_data[i].lookname != NULL )
	    {
		sprintf(buf, "   %-12s", form_data[i].lookname);
		stc(buf, ch);
		if ( ++col % 5 == 0)
		    send_to_char("\n\r", ch);
	    }
	}
	if (col % 5 != 0)
	    stc("\n\r", ch);
	return;
    }

    if (!str_cmp(argument, "back"))
    {
	if (ch->cur_form == get_normal_form(ch))
	{
	    stc("You are already in your normal form.\n\r", ch);
	    return;
	}
	sprintf(buf, "$n polymorphs back into %s.",
	    GET_PROPER_NAME(ch));
	act(buf, ch, NULL, NULL, TO_ROOM);
	stc("You return to your demonic form.\n\r", ch);
	set_form(ch, get_normal_form(ch) );
	WAIT_STATE(ch, 7);
	return;
    }

    if (!TIME_UP(ch, TIMER_CAN_POLYMORPH) && ch->level != 12)
    {
	stc("You have not got enough energy to polymorph so soon.\n\r", ch);
	return;
    }

    for ( i = 0 ; i < MAX_FORMS ; i++ )
    {
	if (form_data[i].lookname != NULL
	    && !str_prefix(argument, form_data[i].lookname)
	    && form_data[i].polymorph_level > 0
	    && form_data[i].polymorph_level <= get_polymorph_power(ch) )
	{
	    if (ch->cur_form == i)
	    {
		stc("You are already in that form.\n\r", ch);
		return;
	    }
	    changed = TRUE;
	    sprintf(buf, "$n transforms into a%s %s.",
		(is_vowel(form_data[i].lookname[0])) ? "n" : "",
		form_data[i].lookname);
	    act(buf, ch, NULL, NULL, TO_ROOM);

	    sprintf(buf, "You transform into a%s %s.\n\r",
		(is_vowel(form_data[i].lookname[0])) ? "n" : "",
		form_data[i].lookname);
	    stc(buf, ch);
	    WAIT_STATE(ch, 8);
	    set_form(ch, i);
	    SET_TIMER(ch, TIMER_CAN_POLYMORPH, dice(2, 3) );
	}
    }    
    if (!changed)    
	stc("No forms of that name found.\n\r", ch);
    return;
}

void do_hellfire (CHAR_DATA *ch, char *argument)
{
    int door;

    if (ch->power[DISC_DAEM_HELL] < 7)
	return;

    if (ch->in_room->sector_type == SECT_AIR)
    {
	send_to_char("This ability cannot be used in the air.\n\r", ch);
	return;
    }

    if ( IS_SET(ch->in_room->room_flags,ROOM_SAFE) )
    {
	stc("You cannot desecrate this Holy Place.\n\r",ch);
	return;
    }

    if (!TIME_UP(ch, TIMER_HELLFIRE_SUMMON)
	&& ch->level < 12)
    {
	send_to_char("This ability can only be used once every 12 hours.\n\r", ch);
	return;
    }

    for (door = 0 ; door < 4 ; door++)
    {
	if (ch->in_room->exit[door] != NULL)
	    make_wall(ch->in_room, door, EX_FIRE_WALL);
    }

    SET_TIMER(ch, TIMER_HELLFIRE_SUMMON, 12);
    stc("You summon the fires of hell!\n\r",ch);
    act("$n summons the Abyssal fires of Hell!",ch,NULL,NULL,TO_ROOM);
}

const sh_int  revdir [6] =
{
    2, 3, 0, 1, 5, 4
};

void put_wall(ROOM_INDEX_DATA *room, int dir, int wall)
{
    int timer;

    timer = dice(2, 2);

    switch (dir)
    {
	case DIR_NORTH :
	    SET_RTIMER(room, RTIMER_WALL_NORTH, timer);
	    SET_RTIMER(room->exit[dir]->to_room, RTIMER_WALL_SOUTH, timer);
	break;
	case DIR_EAST :
	    SET_RTIMER(room, RTIMER_WALL_EAST, timer);
	    SET_RTIMER(room->exit[dir]->to_room, RTIMER_WALL_WEST, timer);
	break;
	case DIR_SOUTH :
	    SET_RTIMER(room, RTIMER_WALL_SOUTH, timer);
	    SET_RTIMER(room->exit[dir]->to_room, RTIMER_WALL_NORTH, timer);
	break;
	case DIR_WEST :
	    SET_RTIMER(room, RTIMER_WALL_WEST, timer);
	    SET_RTIMER(room->exit[dir]->to_room, RTIMER_WALL_EAST, timer);
	break;
	case DIR_UP :
	    SET_RTIMER(room, RTIMER_WALL_UP, timer);
	    SET_RTIMER(room->exit[dir]->to_room, RTIMER_WALL_DOWN, timer);
	break;
	case DIR_DOWN :
	    SET_RTIMER(room, RTIMER_WALL_DOWN, timer);
	    SET_RTIMER(room->exit[dir]->to_room, RTIMER_WALL_UP, timer);
	break;
    }   
    room->exit[dir]->exit_info = wall;

}

void make_wall(ROOM_INDEX_DATA *room, int dir, int wall)
{
    int was;
    bool makewall = TRUE;

    if (dir < 0 || dir > 5)
    {
	bug("Invalid direction (create wall)", 0);
	return;
    }

    if (room->exit[dir] == NULL)
    {
	bug ("Cant make wall!!!!", 0);
	return;
    }

    was = room->exit[dir]->exit_info;

    if (was == EX_ICE_WALL && wall == EX_FIRE_WALL)
    {
	room_message(room, "The wall of ice bursts into flames.");
	room_message(room->exit[dir]->to_room, "The wall of ice bursts into flames.");
    }

    if (was == EX_FIRE_WALL && wall == EX_ICE_WALL)
    {
	room_message(room, "The wall of flames flickers blue for a second.");
	room_message(room->exit[dir]->to_room, "The wall of flames flickers blue for a second.");
	makewall = FALSE;
    }

    if (makewall) put_wall(room, dir, wall);

    if (room->exit[dir]->to_room == NULL)
    {
	bug("Exit doesnt lead anywhere (create wall).", 0);
	return;
    }

    if (room->exit[dir]->to_room->exit[revdir[dir]] == NULL)
    {
	bug("Reverse Exit doesnt lead anywhere (create wall).", 0);
	return;
    }

    if (makewall) put_wall(room->exit[dir]->to_room, revdir[dir], wall);
}

ROOM_INDEX_DATA * locate_obj (OBJ_DATA *obj)
{
    if (obj->in_obj != NULL)
	return locate_obj(obj->in_obj);

    if (obj->carried_by != NULL)
	return obj->carried_by->in_room;

    return obj->in_room;
}

void do_seed(CHAR_DATA *ch, char *argument)
{
    OBJ_DATA *obj;

    if (ch->power[DISC_DAEM_HELL] < 7)
    {
	stc("You must obtain a level seven Mastery of the Hellfire Discipline.\n\r",ch);
	return;
    }

    if ( IS_SET(ch->in_room->room_flags,ROOM_SAFE) )
    {
	stc("You cannot desecrate this Holy Place!\n\r",ch);
	return;
    }

    if (argument[0] == '\0')
    {
	send_to_char("Which item do you wish to plant with Deamonseed?\n\r", ch);
	return;
    }

    if ((obj = get_obj_carry(ch, argument) ) == NULL)
    {
	send_to_char("You do not have that item in your inventory.\n\r",ch);
	return;
    }
  
    if (IS_OBJ_STAT2(obj, ITEM_DAEMONSEED) )
    {
	send_to_char("That item is already set to explode.\n\r", ch);
	return;
    }

    if (IS_OBJ_STAT(obj, ITEM_BLESS)
	|| IS_OBJ_STAT(obj, ITEM_ANTI_EVIL) )
    {
	send_to_char("You cannot affect such a holy item.\n\r", ch);
	return;
    }

    if (IS_SET(obj->quest, QUEST_ARTIFACT)
	|| IS_SET(obj->quest, QUEST_RELIC) )
    {
	send_to_char("You cannot affect such a powerful item.\n\r", ch);
	return;
    }

    act("$p starts to shudder and vibrate in your hands.", ch, obj, NULL, TO_CHAR);
    act("$n lays $s hands on $p.", ch, obj, NULL, TO_ROOM);

    WAIT_STATE(ch, 12);
    SET_BIT(obj->extra_flags2, ITEM_DAEMONSEED);
    obj->timer = ch->generation / 2 + dice(1,3);
}


void do_immolate(CHAR_DATA *ch, char *argument)
{
    OBJ_DATA *obj;

    if (!IS_DEMPOWER(ch, DEM_IMMOLATE))
    {
	stc("You have not been granted that power.\n\r",ch);
	return;
    }

    if (argument[0] == '\0')
    {
	send_to_char("Which item do you wish to immolate?\n\r", ch);
	return;
    }

    if ( (obj = get_obj_carry(ch, argument)) == NULL)
    {
	send_to_char("You are not carrying that item.\n\r", ch);
	return;
    }

    if (obj->item_type != ITEM_WEAPON)
    {
	send_to_char("That is not a weapon!\n\r", ch);
	return;
    }

    if (dice(1, 100) == 1)
    {
	act("$p explodes in a burst of flames.", ch,obj, NULL, TO_ROOM);
	act("$p explodes in a burst of flames.", ch,obj, NULL, TO_CHAR);
	damage(ch,ch,500,gsn_inferno);
	extract_obj(obj);
	return;
    }

    if (IS_WEAP(obj, WEAPON_FLAMING))
    {
	act("$p is already flaming.", ch, obj, NULL, TO_CHAR);
	return;
    }

    act("$p bursts into flames.", ch, obj, NULL, TO_CHAR);
    act("$p, carried by $n bursts into flames.", ch, obj, NULL, TO_ROOM);

    WAIT_STATE(ch, 8);
    SET_BIT(obj->weapflags, WEAPON_FLAMING);
}

void do_destroy (CHAR_DATA *ch, char *argument)
{
    char arg[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;

    argument = one_argument( argument, arg );
    
    if (ch->power[DISC_DAEM_HELL] < 5)
    {
	send_to_char("Only true masters of the hellfire discipline may use this ability.\n\r", ch);
	return;
    }


    if ( arg[0] == '\0' )
    {
	send_to_char( "Which daemon do you wish to destroy?\n\r", ch );
	return;
    }

    if ( ( victim = get_char_room( ch, arg ) ) == NULL )
    {
	send_to_char( "They aren't here.\n\r", ch );
	return;
    }

    if ( victim == ch )
    {
	send_to_char("You cannot destroy yourself.\n\r",ch);
	return;
    }

    if ( victim->class != ch->class && ch->level != 120)
    {
	send_to_char("You may only destroy daemons.\n\r",ch);
	return;
    }

    if ( victim->generation >= ch->generation)
    {
	send_to_char("They are too powerful to destroy.\n\r",ch);
	return;
    }
    
    if (ch->generation < 12)
    {
	send_to_char("Only Pit Fiends or above may use the destroy ability.\n\r", ch);
	return;
    }

    ch->alignment -= 30;
    if (ch->alignment < -1000) ch->alignment = -1000;

    act("You breathe out a huge gout of abyssal flame and incinerate $N.", ch, NULL, victim, TO_CHAR);
    act("$n breathes out a huge gout of abyssal flame and incinerates you.", ch, NULL, victim, TO_VICT);
    act("$n breathes out a huge gout of flame and incinerates $N.", ch, NULL, victim, TO_NOTVICT);
    send_to_char("You have been KILLED!", victim);
    act("$n drops dead.", victim, NULL, NULL, TO_ROOM);
    victim->hit = -10;
    victim->move /= 3;
    update_pos(victim);
}

void do_dinferno(CHAR_DATA *ch, char *argument)
{
    CHAR_DATA *vch, *vch_next;
    char buf[MAX_STRING_LENGTH];    

    if (!IS_DEMPOWER(ch, DEM_INFERNO))
    {
	send_to_char("You have not been granted that power.\n\r", ch);
	return;
    }

    if ( !TIME_UP(ch, TIMER_INFERNO) )
    {
	sprintf(buf,"You cannot use Inferno for another %d hours.\n\r",ch->tick_timer[TIMER_INFERNO] );
	stc(buf,ch);
	return;
    }

    if ( ch->hit > 0 )
    {
	send_to_char("This is only used as a last resort.\n\r",ch);
	return;
    }

    act("$n explodes, sending forth a massive fireballs in all directions.",ch,NULL,NULL,TO_ROOM);
    send_to_char("You explode in a huge blast of abyssal flame.\n\r",ch);
    for (vch = ch->in_room->people; vch != NULL; vch = vch_next)
    {
	    vch_next = vch->next_in_room;
	    if (vch != ch)
	    {
		damage(vch, ch, number_range(1500,2000), gsn_inferno);
		damage(ch, ch, number_range(1500,2000), gsn_inferno);
		fire_effect(vch, ch->level, 2000, TARGET_CHAR);
	    }
    }
    WAIT_STATE(ch, 56);
    ch->hit = 10;
    update_pos(ch);
    SET_TIMER(ch, TIMER_INFERNO, 80);
}   

void acid_effect(void *vo, int level, int dam, int target)
{
    if (target == TARGET_ROOM) /* nail objects on the floor */
     {
	ROOM_INDEX_DATA *room = (ROOM_INDEX_DATA *) vo;
	OBJ_DATA *obj, *obj_next;

	for (obj = room->contents; obj != NULL; obj = obj_next)
	{
	    obj_next = obj->next_content;
	    acid_effect(obj,level,dam,TARGET_OBJ);
	}
	return;
    }

    if (target == TARGET_CHAR)  /* do the effect on a victim */
    {
	CHAR_DATA *victim = (CHAR_DATA *) vo;
	OBJ_DATA *obj, *obj_next;

	/* let's toast some gear */
	for (obj = victim->carrying; obj != NULL; obj = obj_next)
	{
	    obj_next = obj->next_content;
	    acid_effect(obj,level,dam,TARGET_OBJ);
	}
	return;
    }

    if (target == TARGET_OBJ) /* toast an object */
    {
	OBJ_DATA *obj = (OBJ_DATA *) vo;
	OBJ_DATA *t_obj,*n_obj;
	int chance;
	char *msg;

	
	chance = level / 4 + dam / 10;

	if (chance > 25)
	    chance = (chance - 25) / 2 + 25;
	 if (chance > 50)
	    chance = (chance - 50) / 2 + 50;

	if (IS_OBJ_STAT(obj,ITEM_BLESS))
	    chance -= 5;

	chance -= obj->level * 2;

	switch (obj->item_type)
	{
	    default:
		return;
	    case ITEM_CONTAINER:
	    case ITEM_CORPSE_PC:
	    case ITEM_CORPSE_NPC:
		msg = "$p fumes and dissolves.";
		break;
	    case ITEM_STAFF:
	    case ITEM_WAND:
		chance -= 10;
		msg = "$p corrodes and breaks.";
		break;
	    case ITEM_SCROLL:
		chance += 10;
		msg = "$p is burned into waste.";
		break; 
	}

	chance = URANGE(5,chance,95);

	if (number_percent() > chance)
	    return;

	if (IS_SET(obj->quest, QUEST_SPELLPROOF))
	    return;

	if (obj->carried_by != NULL)
	    act(msg,obj->carried_by,obj,NULL,TO_ALL);
	else if (obj->in_room != NULL && obj->in_room->people != NULL)
	    act(msg,obj->in_room->people,obj,NULL,TO_ALL);

	/* get rid of the object */
	if (obj->contains)  /* dump contents */
	{
	    for (t_obj = obj->contains; t_obj != NULL; t_obj = n_obj)
	    {
		n_obj = t_obj->next_content;
		obj_from_obj(t_obj);
		if (obj->in_room != NULL)
		    obj_to_room(t_obj,obj->in_room);
		else if (obj->carried_by != NULL)
		    obj_to_room(t_obj,obj->carried_by->in_room);
		else
		{
		    extract_obj(t_obj);
		    continue;
		}

		acid_effect(t_obj,level/2,dam/2,TARGET_OBJ);
	    }
	}

	extract_obj(obj);
	return;
    }
}


void shock_effect(void *vo,int level, int dam, int target)
{
    if (target == TARGET_ROOM)
    {
	ROOM_INDEX_DATA *room = (ROOM_INDEX_DATA *) vo;
	OBJ_DATA *obj, *obj_next;

	for (obj = room->contents; obj != NULL; obj = obj_next)
	{
	    obj_next = obj->next_content;
	    shock_effect(obj,level,dam,TARGET_OBJ);
	}
	return;
    }

    if (target == TARGET_CHAR)
    {
	CHAR_DATA *victim = (CHAR_DATA *) vo;
	OBJ_DATA *obj, *obj_next;

	/* toast some gear */
	for (obj = victim->carrying; obj != NULL; obj = obj_next)
	{
	    obj_next = obj->next_content;
	    shock_effect(obj,level,dam,TARGET_OBJ);
	}
	return;
    }

    if (target == TARGET_OBJ)
    {
	OBJ_DATA *obj = (OBJ_DATA *) vo;
	int chance;
	char *msg;

	chance = level / 4 + dam / 10;

	if (chance > 25)
	    chance = (chance - 25) / 2 + 25;
	if (chance > 50)
	    chance = (chance - 50) /2 + 50;

	if (IS_OBJ_STAT(obj,ITEM_BLESS))
	    chance -= 5;

	chance -= obj->level * 2;

	switch(obj->item_type)
	{
	    default:
		return;
	   case ITEM_WAND:
	   case ITEM_STAFF:
		chance += 10;
		msg = "$p overloads and explodes!";
		break;
	}

	chance = URANGE(5,chance,95);

	if (number_percent() > chance)
	    return;

	if (IS_SET(obj->quest, QUEST_SPELLPROOF))
	    return;

	if (obj->carried_by != NULL)
	    act(msg,obj->carried_by,obj,NULL,TO_ALL);
	else if (obj->in_room != NULL && obj->in_room->people != NULL)
	    act(msg,obj->in_room->people,obj,NULL,TO_ALL);

	extract_obj(obj);
	return;
    }
}       

void fire_effect(void *vo, int level, int dam, int target)
{
    if (target == TARGET_ROOM)  /* nail objects on the floor */
    {
	ROOM_INDEX_DATA *room = (ROOM_INDEX_DATA *) vo;
	OBJ_DATA *obj, *obj_next;

	for (obj = room->contents; obj != NULL; obj = obj_next)
	{
	    obj_next = obj->next_content;
	    fire_effect(obj,level,dam,TARGET_OBJ);
	}
	return;
    }

    if (target == TARGET_CHAR)   /* do the effect on a victim */
    {
	CHAR_DATA *victim = (CHAR_DATA *) vo;
	OBJ_DATA *obj, *obj_next;

	/* chance of blindness */
	if (!IS_AFFECTED(victim,AFF_BLIND))
	{
	    AFFECT_DATA af;
	    act("$n is blinded by smoke!",victim,NULL,NULL,TO_ROOM);
	    act("Your eyes tear up from smoke...you can't see a thing!",
		victim,NULL,NULL,TO_CHAR);

	    af.type         = skill_lookup("fire breath");
	    af.duration     = number_range(0,level/10);
	    af.location     = APPLY_HITROLL;
	    af.modifier     = -20;
	    af.bitvector    = AFF_BLIND;

	    affect_to_char(victim,&af);
	}

	/* let's toast some gear! */
	for (obj = victim->carrying; obj != NULL; obj = obj_next)
	{
	    obj_next = obj->next_content;
	    fire_effect(obj,level,dam,TARGET_OBJ);
	}
	return;
    }

    if (target == TARGET_OBJ)  /* toast an object */
    {
	OBJ_DATA *obj = (OBJ_DATA *) vo;
	OBJ_DATA *t_obj,*n_obj;
	int chance;
	char *msg;

	chance = level / 4 + dam / 10;
 
	if (chance > 25)
	    chance = (chance - 25) / 2 + 25;
	if (chance > 50)
	    chance = (chance - 50) / 2 + 50;

	if (IS_OBJ_STAT(obj,ITEM_BLESS))
	    chance -= 5;
	chance -= obj->level * 2;

	switch ( obj->item_type )
	{
	default:         
	    return;
	case ITEM_CONTAINER:
	    msg = "$p ignites and burns!";
	    break;
	case ITEM_POTION:
	    chance += 25;
	    msg = "$p bubbles and boils!";
	    break;
	case ITEM_SCROLL:
	    chance += 50;
	    msg = "$p crackles and burns!";
	    break;
	case ITEM_STAFF:
	    chance += 10;
	    msg = "$p smokes and chars!";
	    break;
	case ITEM_WAND:
	    msg = "$p sparks and sputters!";
	    break;
	case ITEM_FOOD:
	    msg = "$p blackens and crisps!";
	    break;
	case ITEM_PILL:
	    msg = "$p melts and drips!";
	    break;
	}

	chance = URANGE(5,chance,95);

	if (number_percent() > chance)
	    return;
 
	if (IS_SET(obj->quest, QUEST_SPELLPROOF))
	    return;      

	if (obj->carried_by != NULL)
	    act( msg, obj->carried_by, obj, NULL, TO_ALL );
	else if (obj->in_room != NULL && obj->in_room->people != NULL)
	    act(msg,obj->in_room->people,obj,NULL,TO_ALL);

	if (obj->contains)
	{
	    /* dump the contents */
 
	    for (t_obj = obj->contains; t_obj != NULL; t_obj = n_obj)
	    {
		n_obj = t_obj->next_content;
		obj_from_obj(t_obj);
		if (obj->in_room != NULL)
		    obj_to_room(t_obj,obj->in_room);
		else if (obj->carried_by != NULL)
		    obj_to_room(t_obj,obj->carried_by->in_room);
		else
		{
		    extract_obj(t_obj);
		    continue;
		}
		fire_effect(t_obj,level/2,dam/2,TARGET_OBJ);
	    }
	}

	extract_obj( obj );
	return;
    }
}

void do_blink(CHAR_DATA *ch, char *argument)
{
    CHAR_DATA *victim;
/*
    if (ch->power[DISC_DAEM_ATTA] < 6)
    {
	stc("You need level 6 Attack to use blink.\n\r",ch);
	return;
    }
*/

	if (IS_NPC(ch)) return;

    if (!IS_CLASS(ch,CLASS_DEMON))
    {
      send_to_char("Huh?\n\r",ch);
      return;
    }


    if (!IS_DEMPOWER( ch, DEM_BLINK) && IS_CLASS(ch,CLASS_DEMON))
    {
        stc("You need to get blink first.\n\r",ch);
        return;
    }


    if (ch->fighting == NULL && argument[0] == '\0')
    {
	stc("Who do you wish to blink into combat with?\n\r", ch);
	return;
    }

    if (ch->fighting == NULL)
    {
	if ( (victim = get_char_room(ch, argument) ) == NULL)
	{
	    stc("They aren't here.\n\r", ch);
	    return;
	}

	WAIT_STATE(ch, 24);
	SET_BIT(ch->flag3, AFF3_BLINK_1ST_RD);
	stop_fighting(ch, TRUE);
	stc("You pop out of existance.\n\r", ch);
	act("$n pops out of existance.", ch, NULL, NULL, TO_ROOM);
	ch->blinkykill = victim;
	return;
    }
    WAIT_STATE(ch, 24);
    victim = ch->fighting;
    SET_BIT(ch->flag3, AFF3_BLINK_2ND_RD);
    stop_fighting(ch, TRUE);
    stc("You pop out of existance.\n\r", ch);
    act("$n pops out of existance.", ch, NULL, NULL, TO_ROOM);
    ch->blinkykill = victim;
}

void do_graft(CHAR_DATA *ch, char *argument)
{
    OBJ_DATA *obj;
    char arg[MAX_INPUT_LENGTH];
    argument = one_argument(argument, arg);

    if (IS_NPC(ch)) return; 

	if (!IS_CLASS(ch,CLASS_DEMON))
    {
    send_to_char("Huh?\n\r",ch);
    return;
    }


/*
    if (ch->power[DISC_DAEM_ATTA] < 5)
    {
*/
	if (!IS_DEMPOWER( ch, DEM_GRAFT) && IS_CLASS(ch,CLASS_DEMON))
    {
	stc("You need to get graft first.\n\r",ch);
	return;
    }

    if ( arg[0] == '\0')
    {
	stc("Which limb do you wish to graft on to yourself?\n\r",ch);
	return;
    }

    if ( ( obj = get_obj_carry(ch,arg) ) == NULL )
    {
	stc("You do not have that limb.\n\r",ch);
	return;
    }

    if (!IS_SET(obj->extra_flags2, ITEM_ARM))
    {
	stc("That's not even an arm!\n\r",ch);
	return;
    }

    if (IS_SET(ch->newbits, THIRD_HAND) && IS_SET(ch->newbits, FOURTH_HAND))
    {
	stc("You already have four arms!\n\r",ch);
	return;
    }

    if (!IS_SET(ch->newbits, FOURTH_HAND))
    {
	act("You graft an arm onto your body.",ch,NULL,obj,TO_CHAR);
	act("$n grafts an arm onto $m body.",ch,NULL,obj,TO_ROOM);
	SET_BIT(ch->newbits, FOURTH_HAND);
	SET_TIMER(ch, TIMER_FOURTH_ARM_GOING, dice(ch->generation, 6) );
	WAIT_STATE(ch, 18);
	extract_obj(obj);
	return;    
    }

    if (!IS_SET(ch->newbits, THIRD_HAND))
    {
	act("You graft an arm onto your body.",ch,NULL,obj,TO_CHAR);
	act("$n grafts an arm onto $m body.",ch,NULL,obj,TO_ROOM);
	SET_BIT(ch->newbits, THIRD_HAND);
	SET_TIMER(ch, TIMER_THIRD_ARM_GOING, dice(ch->generation, 6) );
	WAIT_STATE(ch, 18);
	extract_obj(obj);
	return;    
    }    

    return;
}


void do_shadowdemon(CHAR_DATA *ch, char *argument)
{
      char buf[MAX_STRING_LENGTH];
      char arg[MAX_INPUT_LENGTH];
      char *fname;
      int form;

      argument = one_argument(argument, arg);

    if (!IS_CLASS(ch, CLASS_VAMPIRE))
    {
	stc("Huh?\n\r",ch);
	return;
    }

     if (arg[0] == '\0')
      {
	  stc("What demonic form do you wish to mimic?\n\r",ch);
	  return;
      }

      if (!str_cmp(arg, "return") && ch->cur_form == get_normal_form(ch))
      {
	  stc("You are already in your normal demonic form!\n\r",ch);
	  return;
      }

	     if (!str_cmp(arg, "lemure"   )) form = FRM_LEMURE;
      else if (!str_cmp(arg, "nupperibo")) form = FRM_NUPPERIBO;
      else if (!str_cmp(arg, "spinagon" )) form = FRM_SPINAGON;
      else if (!str_cmp(arg, "barbazu"  )) form = FRM_BARBAZU;
      else if (!str_cmp(arg, "abishai"  )) form = FRM_ABISHAI;
      else if (!str_cmp(arg, "osyluth"  )) form = FRM_OSYLUTH;
      else if (!str_cmp(arg, "hamatula" )) form = FRM_HAMATULA;
      else if (!str_cmp(arg, "erinyes"  )) form = FRM_ERINYES;
      else if (!str_cmp(arg, "amnizu"   )) form = FRM_AMNIZU;
      else if (!str_cmp(arg, "cornugon" )) form = FRM_CORNUGON;
      else if (!str_cmp(arg, "gelugon"  )) form = FRM_GELUGON;
      else if (!str_cmp(arg, "return"   )) form = get_normal_form(ch);
      else
      {
	stc("That is an invalid demonic form.\n\r",ch);
	return;
      }

     
     if ((form - 23) <= ch->generation)
      {
	  fname = str_dup(ch->name);
	  stc("Your skin slowly melts away and you form a puddle on the floor.\n\r",ch);
	  act("$n's body slowly melts into a puddle on the ground.",ch,NULL,NULL,TO_ROOM);
	  set_form(ch, form);
	  sprintf(buf,"Your body slowly reforms as %s.",ch->name);
	  stc(buf,ch);
	  sprintf(buf,"%s's body slowly reforms as %s.",fname,ch->name);
	  act(buf,ch,NULL,NULL,TO_ROOM);
	  return;
      }
	  
	
}


void do_dgate( CHAR_DATA *ch, char *argument )
{
    char arg1[MAX_INPUT_LENGTH];
    DESCRIPTOR_DATA *d;
    CHAR_DATA *victim;
	
    argument = one_argument( argument, arg1 );
    
    if (!IS_CLASS(ch, CLASS_DEMON))
    {
	stc("Huh?\n\r",ch);
	return;
    }

    if ( arg1[0] == '\0' )
    {
	send_to_char( "Gate whom?\n\r", ch );
	return;
    }

    if ( !str_cmp( arg1, "all" ) )
    {
	if ( ch->generation < 12 )
	{
	    send_to_char("You do not yet have the power to gate all demons.\n\r",ch);
	}
	else
	{
		for ( d = descriptor_list; d != NULL; d = d->next )
		{
		    if ( d->connected == CON_PLAYING
		    &&   d->character != ch
		    &&   d->character->in_room != NULL
		    &&   can_see( ch, d->character ) 
		    &&   d->character->generation < ch->generation
		    &&   d->character->class == CLASS_DEMON)
		    {
			if ( d->character->fighting != NULL ) stop_fighting( d->character, TRUE );
			    act( "A scaly hand reaches down and drags $n away.", d->character, NULL, NULL, TO_ROOM );
			    char_from_room( d->character );
			    char_to_room( d->character, ch->in_room );
			    act( "You have successfully gated $N.", ch, NULL, d->character, TO_CHAR );
			    act( "$n has successfully gated $N.", ch, NULL, d->character, TO_NOTVICT );
			    if ( ch != d->character )
				act( "$n has pulled you through the demonic void.", ch, NULL, d->character, TO_VICT );
			    do_look( d->character, "auto" );
			    send_to_char( "Ok.\n\r", ch );                      
		    }
		}
	}
	return;
    }

    if ( ( victim = get_char_world( ch, arg1 ) ) == NULL )
    {
	send_to_char( "They aren't here.\n\r", ch );
	return;
    }

    if ( victim->class != CLASS_DEMON )
    {
	send_to_char( "You may only gate in demons.\n\r", ch );
	return;
    }

    if ( victim->in_room == NULL )
    {
	send_to_char( "They are in limbo.\n\r", ch );
	return;
    }

    if ( victim->generation >= ch->generation )
    {
	send_to_char("You may only gate one of lesser power than yourself.\n\r",ch);
	return;
    }    
    if  (!IS_NPC(victim) && !IS_IMMUNE(victim, IMM_SUMMON) )
{
	 send_to_char("Your powers seem to have failed you.\n\r",ch);
return;
}
    if ( number_range(1, ch->generation) == 1 )
    {
	send_to_char("Your powers seem to have failed you.\n\r",ch);
	send_to_char("You hear demonic chanting in the distance.\n\r",victim);
	return;
    }
    
    if ( victim->fighting != NULL ) stop_fighting( victim, TRUE );
    act( "A scaly hand reaches down and drags $n away.", victim, NULL, NULL, TO_ROOM );
    char_from_room( victim );
    char_to_room( victim, ch->in_room);
    act( "You have successfully gated $N.", ch, NULL, victim, TO_CHAR );
    act( "$n has successfully gated $N.", ch, NULL, victim, TO_NOTVICT );
    if ( ch != victim )
	act( "$n has pulled you through the demonic void.", ch, NULL, victim, TO_VICT );
    do_look( victim, "auto" );
    send_to_char( "Ok.\n\r", ch );
}

void do_frostbreath (CHAR_DATA *ch, char *argument)
{
    CHAR_DATA *victim;
    char arg[MAX_STRING_LENGTH];

    argument = one_argument(argument, arg);

    if (ch->power[DISC_DAEM_GELU] < 2)
    {
	stc("You must obtain level two in Geluge to use Frostbreath.\n\r",ch);
	return;
    }

    if (!TIME_UP(ch, TIMER_CAN_BREATHE_FROST) && ch->level < 12)
    {
	send_to_char("You are out of breath.\n\r", ch);
	return;
    }

    if (arg[0] == '\0')
    {
	send_to_char("Who do you wish to breathe on?\n\r", ch);
	return;
    }

    if (!str_cmp(arg, "all"))
    {
      CHAR_DATA *vch, *vch_next;
      for (vch = ch->in_room->people; vch != NULL; vch = vch_next)
      {
	  vch_next = vch->next_in_room;
	  if (vch == ch) continue;

	  if (is_safe(ch,vch) 
	  &&   (ch->fighting != vch || vch->fighting != ch))
	     continue;

	WAIT_STATE(ch, 24);
	SET_TIMER(ch, TIMER_CAN_BREATHE_FROST, 11 - ch->generation / 2);
	frost_breath(ch, vch, TRUE);
      }
    }

    if ( (victim = get_char_room(ch, arg)) == NULL)
    {
	send_to_char("They aren't here.\n\r", ch);
	return;
    }

    WAIT_STATE(ch, 12);
    SET_TIMER(ch, TIMER_CAN_BREATHE_FROST, 9 - ch->generation / 2);
    frost_breath(ch, victim, FALSE);
}


void cold_effect(void *vo, int level, int dam, int target)
{
    if (target == TARGET_ROOM) /* nail objects on the floor */
    {
	ROOM_INDEX_DATA *room = (ROOM_INDEX_DATA *) vo;
	OBJ_DATA *obj, *obj_next;
 
	for (obj = room->contents; obj != NULL; obj = obj_next)
	{
	    obj_next = obj->next_content;
	    cold_effect(obj,level,dam,TARGET_OBJ);
	}
	return;
    }

    else if (target == TARGET_CHAR) /* whack a character */
    {
	CHAR_DATA *victim = (CHAR_DATA *) vo;
	OBJ_DATA *obj, *obj_next;

	/* chill touch effect */
	    AFFECT_DATA af;

	    af.type      = skill_lookup("chill touch");
	    af.duration  = 6;
	    af.location  = APPLY_STR;
	    af.modifier  = -3;
	    af.bitvector = 0;
	    affect_to_char( victim, &af );

	/* let's toast some gear */
	for (obj = victim->carrying; obj != NULL; obj = obj_next)
	{
	    obj_next = obj->next_content;
	    cold_effect(obj,level,dam,TARGET_OBJ);
	}
	return;
   }

   else if (target == TARGET_OBJ) /* toast an object */
   {
	OBJ_DATA *obj = (OBJ_DATA *) vo;
	int chance;
	char *msg;

	chance = level / 4 + dam / 10;

	if (chance > 25)
	    chance = (chance - 25) / 2 + 25;
	if (chance > 50)
	    chance = (chance - 50) / 2 + 50;

	if (IS_OBJ_STAT(obj,ITEM_BLESS))
	    chance -= 5;

	chance -= obj->level * 2;

	switch(obj->item_type)
	{
	    default:
		return;
	    case ITEM_POTION:
		msg = "$p freezes and shatters!";
		chance += 25;
		break;
	    case ITEM_DRINK_CON:
		msg = "$p freezes and shatters!";
		chance += 5;
		break;
	}

	chance = URANGE(5,chance,95);

	if (number_percent() > chance)
	    return;

	if (IS_SET(obj->quest, QUEST_SPELLPROOF))
	    return;

	if (obj->carried_by != NULL)
	    act(msg,obj->carried_by,obj,NULL,TO_ALL);
	else if (obj->in_room != NULL && obj->in_room->people != NULL)
	    act(msg,obj->in_room->people,obj,NULL,TO_ALL);

	extract_obj(obj);
	return;
    }
}


void frost_breath(CHAR_DATA *ch, CHAR_DATA *victim, bool all)
{
    CHAR_DATA *vch, *vch_next;
    int dam;
    int power;

    if (IS_CLASS(ch, CLASS_DEMON)) power = ch->power[DISC_DAEM_GELU];
    else power = 0;

    if (all)
    {
	dam = (ch->generation + 5 * power+5) * 8;
	
	act("$n breathes forth a stream of frost over you.",ch,NULL,NULL,TO_ROOM);
	act("You breath forth a stream of frost.",ch,NULL,NULL,TO_CHAR);

	for (vch = ch->in_room->people; vch != NULL; vch = vch_next)
	{
	    vch_next = vch->next_in_room;
	    if (vch == ch) continue;

	    if (is_safe(ch,vch) 
	    &&   (ch->fighting != vch || vch->fighting != ch))
		continue;

		cold_effect(vch,ch->explevel,dam,TARGET_CHAR);
		damage(ch,vch,dam,skill_lookup("frost breath"));
	}
	return;
    }

    if (is_safe(ch, victim)) return;
    dam = (ch->generation +3 * power+3) * 20;

    act("$n breathes forth a stream of frost over $N.",ch,NULL,victim,TO_NOTVICT);
    act("$n breathes a stream of frost over you!",ch,NULL,victim,TO_VICT);
    act("You breath forth a stream of frost over $N.",ch,NULL,victim,TO_CHAR);
    
    damage(ch,victim,dam,skill_lookup("frost breath"));
    cold_effect(victim,ch->explevel,dam,TARGET_CHAR);
}
