 
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

void do_setgentick(CHAR_DATA *ch,char *argument)
{
char arg[MAX_INPUT_LENGTH];
int value;
one_argument(argument,arg);
value=is_number(arg) ? atoi(arg):-1;
ch->pcdata->gentick=value;
send_to_char("Ok.\n\r",ch);

return;
}
void do_testemb(CHAR_DATA *ch, char *argument  )
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
    if ( arg[0] == '\0' && ch->embracing==NULL)
    {
        send_to_char( "Who do you wish to embrace?\n\r", ch );
	return;
    }
if (ch->embracing != NULL)
{
    if ((victim=ch->embracing) != NULL)
    {
    send_to_char("You retract your fangs.",ch);
    act("$N retracts his fangs.",ch,NULL,NULL,TO_ROOM);
    stop_embrace(ch,victim);
    return;
    }
}
    if ((victim = get_char_room( ch, arg)) == NULL && ch->embracing==NULL)
    {
    send_to_char("They arent here.\n\r",ch);
    return;
    }
    if (IS_NPC(victim) || !IS_CLASS(ch,CLASS_VAMPIRE))
    {
    send_to_char("You can only embrace vampires.\n\r",ch);
    return;
    }
  
    if (victim->position != POS_MORTAL)
    {
    send_to_char("You can only embrace mortally wounded vampires.\n\r",ch);
    return;
    }
    sprintf(buf,
    "%s leaps toward %s baring his fangs.",ch->name,victim->name);
    act(buf,ch,NULL,NULL,TO_ROOM);
    sprintf(buf,
    "You leap toward %s baring your fangs.\n\r",victim->name);
    send_to_char(buf,ch);
    WAIT_STATE(ch, 15);
    
    sprintf(buf,
    "You sink your teeth into their throat.\n\r");
    send_to_char(buf,ch);
    sprintf(buf,
    "%s sinks their teeth into %s's throat."
    ,ch->name,victim->name);
    act(buf,ch,NULL,NULL,TO_ROOM);
    sprintf(buf,
    "%s sinks their teeth into your throat.\n\r",ch->name);
    send_to_char(buf,victim);
    victim->embraced=ch;
    ch->embracing=victim;
 
 
    return;
 
}

void do_conceal(CHAR_DATA *ch, char *argument)
{
char arg[MAX_STRING_LENGTH];
char buf[MAX_STRING_LENGTH];
OBJ_DATA *obj; 
one_argument(argument,arg);
if (IS_NPC(ch)) return;
if (!IS_CLASS(ch,CLASS_VAMPIRE))
{send_to_char("Huh?\n\r",ch);return;}
if (ch->power[DISC_VAMP_OBFU] < 5)
{send_to_char("You need obfuscate 5 to conceal items.\n\r",ch);return;}
if (arg[0]== '\0')
{send_to_char("Syntax: Conceal (item).\n\r",ch);return;}
if ((obj=get_obj_carry(ch,arg)) == NULL)
{send_to_char("You dont have that item.\n\r",ch);return;}
if (IS_SET(obj->extra_flags, ITEM_INVIS))
{
sprintf(buf,"%s fades into existance.",obj->short_descr);
send_to_char(buf,ch);act(buf,ch,NULL,NULL,TO_ROOM);
REMOVE_BIT(obj->extra_flags, ITEM_INVIS);
return;
}
if (!IS_SET(obj->extra_flags, ITEM_INVIS))
{
sprintf(buf,"%s fades out of existance.",obj->short_descr);
send_to_char(buf,ch);act(buf,ch,NULL,NULL,TO_ROOM);
SET_BIT(obj->extra_flags, ITEM_INVIS);
return;
}
 
return;
}

void do_fear( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *victim;
    char arg[MAX_INPUT_LENGTH];
    char buf[MAX_STRING_LENGTH];

    argument = one_argument( argument, arg );

    if ( IS_NPC(ch) ) return;

    if ( !IS_CLASS(ch, CLASS_VAMPIRE) )
    {
	stc("Huh?\n\r",ch);
	return;
    }

    if ( ch->power[DISC_VAMP_DAIM] < 2 )
    {
      stc("You must obtain level 2 Daimoinon to use Fear.\n\r",ch);
      return;
    }

    if ( ( victim = get_char_room( ch, arg) ) == NULL )
    {
      stc("They aren't here.\n\r",ch);
      return;
    }

    arg[0] = UPPER(arg[0]);    

    if ( ch->fighting == NULL )
    {
      stc("You must be fighting to use Fear.\n\r",ch);
      return;
    }

    if ( victim->fighting == NULL )
    {
      sprintf(buf, "%s is not fighting anyone.", arg);
      stc(buf, ch);
      return;
    }  

    WAIT_STATE( ch, 16 );

    if ( IS_NPC(victim) )
    {
      act("You bare your fangs and growl at $N.",ch,NULL,victim,TO_CHAR);
      act("$n bares $s fangs and growls at you.",ch,NULL,victim,TO_VICT);
      act("$n bares $s fangs and growls at $N, and $N flees in terror.",ch,NULL,victim,TO_NOTVICT);
      do_flee( victim, "" );
      return;
    }

    if ( !IS_NPC(victim) )
    {
      if ( ch->power[DISC_VAMP_DAIM] < 5 )
      {
        if ( number_range( 1, 4 ) != 2 )
        {
          act("You bare your fangs and growl at $N, but nothing happens.",ch,NULL,victim,TO_CHAR);
          act("$n bares $s fangs and growls at you.",ch,NULL,victim,TO_VICT);
          act("$n bares $s fangs and growls at $N, but nothing happens.",ch,NULL,victim,TO_NOTVICT);
	  return;
        }
      }

      act("You bare your fangs and growl at $N.",ch,NULL,victim,TO_CHAR);
      act("$n bares $s fangs and growls at you.",ch,NULL,victim,TO_VICT);
      act("$n bares $s fangs and growls at $N, and $N flees in terror.",ch,NULL,victim,TO_NOTVICT);
      do_flee( victim, "" );
      return;
    }

  return;

}

void do_vtwist(CHAR_DATA *ch, char *argument)
{
  char arg1[MAX_STRING_LENGTH];
  char arg2[MAX_STRING_LENGTH];
  OBJ_DATA *obj; 
 
  argument=one_argument(argument,arg1);
  strcpy(arg2,argument);

  if (IS_NPC(ch)) return;

  if (!IS_CLASS(ch,CLASS_VAMPIRE))
  {
    send_to_char("Huh?\n\r",ch);
    return;
  }

  if (ch->power[DISC_VAMP_DAIM] < 5)
  {
    send_to_char("You must obtain level 5 Daimoinon to use Twist.\n\r",ch);
    return;
  }

  if (arg1 == NULL || arg2 == NULL)
  {
    send_to_char("Syntax: Dub (item) (description)?\n\r",ch);
    return;
  }

  if ((obj=get_obj_carry(ch,arg1)) == NULL)
  {
    send_to_char("You dont have that item.\n\r",ch);
    return;
  }

  if (strlen(arg2) > 40 || strlen(arg2) < 3)
  {
    send_to_char("From 3 to 40 characters please.\n\r",ch);
    return;
  }

  free_string(obj->name);
  obj->name = str_dup(arg2);
  free_string(obj->short_descr);
  obj->short_descr=str_dup(arg2);
  send_to_char("Ok.\n\r",ch);
  return;

}

void do_dub(CHAR_DATA *ch, char *argument)
{
char arg1[MAX_STRING_LENGTH];
char arg2[MAX_STRING_LENGTH];
OBJ_DATA *obj; 
 
argument=one_argument(argument,arg1);
strcpy(arg2,argument);
if (IS_NPC(ch)) return;
if (!IS_CLASS(ch,CLASS_VAMPIRE))
{send_to_char("Huh?\n\r",ch);return;}
if (ch->power[DISC_VAMP_OBFU] < 4)
{send_to_char("You need obfuscate 4 to dub items.\n\r",ch);return;}
if (arg1 == NULL || arg2 == NULL)
{send_to_char("Syntax: Dub (item) (description)?\n\r",ch);return;}
if ((obj=get_obj_carry(ch,arg1)) == NULL)
{send_to_char("You dont have that item.\n\r",ch);return;}
if (strlen(arg2) > 40 || strlen(arg2) < 3)
{send_to_char("From 3 to 40 characters please.\n\r",ch);return;}
free_string(obj->name);
obj->name = str_dup(arg2);
free_string(obj->short_descr);
obj->short_descr=str_dup(arg2);
send_to_char("Ok.\n\r",ch);
return;
}

void do_sharpen(CHAR_DATA *ch, char *argument)
{
  char arg[MAX_STRING_LENGTH];
  OBJ_DATA *obj;
  argument=one_argument(argument,arg);
  if (IS_NPC(ch)) return;
 
  if (!IS_CLASS(ch,CLASS_VAMPIRE))
  {
     send_to_char("Huh?\n\r",ch);
     return;
  }

  if (ch->power[DISC_VAMP_QUIE] < 7)
  {
     send_to_char("You need Quetius 7 to sharpen.\n\r",ch);
     return;
  }

  if (arg[0] == '\0')
  {
     send_to_char("What do you wish to sharpen?\n\r",ch);
     return;
  }

  if ( ( obj = get_obj_carry(ch,arg) ) == NULL )
  {
     send_to_char("You dont have that weapon.\n\r",ch);
     return;
  }

  if (IS_SET(obj->quest, QUEST_ARTIFACT) || obj->item_type != ITEM_WEAPON)
  {
     send_to_char("You cant sharpen that item.\n\r",ch);
     return;
  }

  if (obj->value[0] == 18000)
  {
     send_to_char("This item is already Sharp!\n\r",ch);
     return;
  }

  if (obj->value[0] != 0)
  {
     send_to_char(" This weapon already has a power.\n\r",ch);
     return;
  }

  obj->value[0] = 18000;
  obj->value[1] = 20;
  obj->value[2] = 30;

  /*mite as well recycle the arg string*/
  sprintf(arg, "You grind away at %s until it is razor sharp!", obj->short_descr);
  send_to_char(arg,ch);
  sprintf(arg, "%s grinds away at %s until it is razor sharp!", ch->name,obj->short_descr);
  act(arg,ch,NULL,NULL,TO_ROOM);
  return;
}
 
 

/*gourge*/
void do_gourge(CHAR_DATA *ch, char *argument  )
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
   if (ch->power[DISC_VAMP_THAU] < 8)
   {
   send_to_char("You need Thaumaturgy 8 to Gourge.\n\r",ch);
   return;
   }
    if ( arg[0] == '\0' )
    {
        send_to_char( "Who do you wish to gourge?\n\r", ch );
	return;
    }
    if ((victim = get_char_room( ch, arg)) == NULL)
    {
    send_to_char("They arent here.\n\r",ch);
    return;
    }
	if (IS_NPC(victim) && IS_SET(victim->act, ACT_NOAUTOKILL)) {
	send_to_char("You can't do that to them.\n\r",  ch);
	return;}

    if (!IS_NPC(victim))
    {
    send_to_char("You cannot gourge a person.\n\r",ch);
    return;
    }
    if (victim->level > 30)
    {
    send_to_char(
    "Only small creatures are defenceless enough to be gourged on.\n\r",ch);
    return;
    }
    sprintf(buf,
    "%s leaps toward %s baring his fangs.",ch->name,victim->name);
    act(buf,ch,NULL,NULL,TO_ROOM);
    sprintf(buf,
    "You leap toward %s baring your fangs.\n\r",victim->name);
    send_to_char(buf,ch);
    WAIT_STATE(ch, 15);
    
    send_to_char(
    "You rip their throat out and gourge on the blood.\n\r",ch);
    sprintf(buf,
    "%s rips %s's throat out, gourging on all of their blood."
    ,ch->name,victim->name);
    act(buf,ch,NULL,NULL,TO_ROOM);
    ch->pcdata->condition[COND_THIRST] += number_range(100,200);
    if (ch->pcdata->condition[COND_THIRST] >= 1000 /
ch->generation)
    {
    send_to_char("Your bloodlust is sated.\n\r",ch);
    ch->pcdata->condition[COND_THIRST] = 1000/ch->generation;
    }
    sprintf(buf,
    "%s body falls to the ground lifeless.",victim->name);
    send_to_char(buf,ch);
    act(buf,ch,NULL,NULL,TO_ROOM);
    raw_kill( victim );
    return;
}

/*bloodwater*/
 
void do_bloodwater( CHAR_DATA *ch, char *argument )
{
CHAR_DATA *victim;
char arg1 [MAX_INPUT_LENGTH];
int dam;
 
argument = one_argument( argument, arg1 );
 
 
if ( IS_NPC(ch) )
    return;
 
 
if (!IS_CLASS(ch, CLASS_VAMPIRE))
     {
          send_to_char("Huh?\n\r",ch);
      return;
 }
 
if (ch->power[DISC_VAMP_NECR] < 5)
 {
send_to_char(
"You need at least level 5 Necromancy to use Blood Water.\n\r",ch);
    return;
 }
 
 
if ( arg1[0] == '\0' )
{
send_to_char( "Whose blood do you wish to turn to water?\n\r", ch );
   return;
}
 if ( ( victim = get_char_room( ch, arg1 ) ) == NULL )
{
    send_to_char( "They aren't here.\n\r", ch );
   return;
}
 
 
if (is_safe(ch,victim) == TRUE) return;
if (IS_NPC(victim)) dam=2000;
if (!IS_NPC(victim)) dam=1000;
if (!IS_CLASS(victim,CLASS_VAMPIRE)) victim->hit -= dam;
    act(
    "$N screams in agony as you turn his blood to water.",
   ch, NULL, victim,TO_CHAR);
act("$N screams in agony as $n turns his blood to water.",
 ch, NULL, victim,TO_NOTVICT);
act("You scream in agony as $n turns your blood to water.",
ch, NULL, victim,TO_VICT);
if (!IS_CLASS(victim,CLASS_VAMPIRE))    WAIT_STATE(ch,12);
if (IS_CLASS(victim,CLASS_VAMPIRE))
{
WAIT_STATE(ch,6);
victim->pcdata->condition[COND_THIRST] -= 150;
}
return;
}
/*spew*/
void do_spew( CHAR_DATA *ch, char *argument )
{
char  buf[MAX_INPUT_LENGTH];
int   sn;  
int   level;
int   spelltype;
 
 
    if ( IS_NPC(ch) )return;
 
if (!IS_CLASS(ch, CLASS_VAMPIRE))
        {
        send_to_char("Huh?\n\r", ch);
        return;
    }
    if ( IS_CLASS(ch, CLASS_VAMPIRE) &&
    ch->power[DISC_VAMP_THAU] < 6)
    {
    send_to_char(
    "You need level 6 Thaumaturgy to use this power.\n\r",ch);
    return; 
    }
if (ch->pcdata->condition[COND_THIRST] < 20)
{send_to_char("You need 20 blood to spew.\n\r",ch);return;}
ch->pcdata->condition[COND_THIRST] -= number_range(18,22);
 
if (IS_SET(ch->in_room->room_flags, ROOM_SAFE))
{
send_to_char("You cannot do that here.\n\r",ch);
return;
}
if ( ( sn = skill_lookup( "spew" ) ) < 0 )
{
sprintf(buf,"Yep, sn is bieng set to %d.",sn);
send_to_char(buf,ch);
return;
 
}
 
spelltype = skill_table[sn].target;
level = ch->power[DISC_VAMP_THAU];
(*skill_table[sn].spell_fun) ( sn, level, ch, NULL);
WAIT_STATE( ch, 12 );
return;
}

/*vampdarkness*/
void do_vampdarkness( CHAR_DATA *ch, char *argument )
{
char buf[MAX_STRING_LENGTH];
ROOM_INDEX_DATA *inroom;   
inroom= ch->in_room;
 
if (IS_NPC(ch)) return;
if (!IS_CLASS(ch, CLASS_VAMPIRE))
{
  send_to_char("Huh?\n\r", ch);
  return;
}
if (ch->power[DISC_VAMP_QUIE] < 6)
{
      send_to_char("You require level 6 Quietus to use Darkness of Death.\n\r",ch);
return;
    }
 
if (ch->pcdata->condition[COND_THIRST] < 300)
{
send_to_char("You have insufficient blood.\n\r", ch);
 return;
}
ch->pcdata->condition[COND_THIRST] -= 300;
SET_BIT(inroom->room_flags, ROOM_DARK);
sprintf(buf,"A look of concentration passes over %s's face.",
ch->name);
act(buf,ch,NULL,NULL,TO_ROOM);
send_to_char("A look of concentration passes over your face.\n\r",ch);
sprintf(buf,"A complete darkness fills the room.\n\r");
act(buf,ch,NULL,NULL,TO_ROOM);
send_to_char(buf,ch);
return;
}
 

/*dragon*/
void do_dragonform( CHAR_DATA *ch, char *argument )
{
char arg [MAX_INPUT_LENGTH];
char buf [MAX_STRING_LENGTH];
argument = one_argument( argument, arg );
 
if (IS_NPC(ch)) return;
 
if (!IS_CLASS(ch, CLASS_VAMPIRE))
    {
     send_to_char("Huh?\n\r",ch);
        return;
}
if (ch->power[DISC_VAMP_VICI] < 4)
{
send_to_char("You must obtain at least level 4 in Vicissitude to use Dragonform\n\r",ch);
return;
}
 
 
if (!IS_POLYAFF(ch, POLY_ZULOFORM))
{
send_to_char("You can only Dragonform while in Zuloform.\n\r",ch);
return;
}
 
if (IS_EXTRA(ch, EXTRA_DRAGON))
{
REMOVE_BIT(ch->extra,EXTRA_DRAGON);
act( "You transform back into zuloform.", ch, NULL, NULL, TO_CHAR );
    act( "$n shrinks back into a big black monster.", ch, NULL, NULL,
TO_ROOM );
    ch->max_hit = ch->max_hit - 500;
ch->hit = ch->hit - 500;
 
if (ch->hit < 1) ch->hit = 1;
    ch->damroll = ch->damroll / 1.2;
ch->hitroll = ch->hitroll / 1.2;
    free_string( ch->morph );
    ch->morph = str_dup( "A big black monster" );
return;
}
 
 
if ( ch->pcdata->condition[COND_THIRST] < 100 )
{
    send_to_char("You have insufficient blood.\n\r",ch);
    return;
}
 
if (ch->mounted == IS_RIDING) do_dismount(ch,"");
ch->pcdata->condition[COND_THIRST] -= number_range(200,400);
    act( "You transform into a large dragon.", ch, NULL, NULL, TO_CHAR );
    act( "$n's body grows and distorts into a large dragon.", ch, NULL,
NULL,TO_ROOM);
SET_BIT(ch->extra,EXTRA_DRAGON);
sprintf(buf, "%s, the huge rabid dragon", ch->name);
free_string( ch->morph );
ch->morph = str_dup( buf );
ch->max_hit = ch->max_hit + 500;
ch->hit = ch->hit + 500;
ch->damroll = ch->damroll * 1.2;
ch->hitroll = ch->hitroll * 1.2;
return;
}

/*obj*/
void do_obj( CHAR_DATA *ch, char *argument )
{
    OBJ_DATA *obj;                              
    OBJ_DATA *obji;
    char buf[MAX_STRING_LENGTH];
    char arg[MAX_STRING_LENGTH];
    argument=one_argument(argument,arg);
    if (IS_NPC(ch)) return;
    if (!IS_CLASS(ch, CLASS_VAMPIRE))
    {
	send_to_char( "Huh?\n\r", ch);
	return;
    }
    if (ch->power[DISC_VAMP_VICI] < 10)
    {
    send_to_char("You require Vicissitude 10 to Object.\n\r",ch);
    return;
    }
    if (IS_AFFECTED(ch,AFF_POLYMORPH) && ch->pcdata->chobj == NULL)
    {
	send_to_char( "You cannot do this while polymorphed.\n\r", ch);
	return;
    }
 
   if (ch->pcdata->obj_vnum != 0)
   {
   do_humanform(ch,"");
   return;
   }
 
   if (arg[0] == '\0')
   {
    send_to_char("Which object in the game do you want to become?\n\r",ch); 
    return;
   }
   if ((obji = get_obj_world(ch,arg)) == NULL)
    {
        send_to_char( "Nothing like that is in the game.\n\r", ch);
	return;
    }
    if (IS_AFFECTED(ch,AFF_WEBBED))
    {
	send_to_char( "Not with all this sticky webbing on.\n\r", ch);
	return;
    }
if (obji->item_type == ITEM_GEMSTONE ||
    obji->item_type == ITEM_HILT     ||
    obji->item_type == ITEM_COPPER   ||
    obji->item_type == ITEM_IRON     ||
    obji->item_type == ITEM_STEEL    ||
    obji->item_type == ITEM_ADAMANTITE||
    obji->item_type == ITEM_QUEST ||
    IS_SET(obji->quest,QUEST_ARTIFACT)
    || IS_SET(obji->quest, QUEST_RELIC))

{send_to_char("You are unable to take the form of that.\n\r",ch);return;}
 
    
   

    obj=create_object(get_obj_index(obji->pIndexData->vnum),60);
    obj_to_room(obj,ch->in_room);
    send_to_char("You concentrate deeply and transform into the basics of the item.\n\r",ch);
    act("$n's form shrinks and distorts into $p.",ch,obj,NULL,TO_ROOM);
    act("Your form shrinks and distorts into $p.",ch,obj,NULL,TO_CHAR);
    ch->pcdata->obj_vnum = obj->pIndexData->vnum;
    sprintf(buf,"%d",ch->pcdata->obj_vnum);
    send_to_char(buf,ch);
    obj->chobj = ch;
    ch->pcdata->chobj = obj;
    SET_BIT(ch->affected_by, AFF_POLYMORPH);
    SET_BIT(ch->extra, EXTRA_OSWITCH);
    free_string(ch->morph);
    ch->morph = str_dup(obj->short_descr);
    return;
}
 
 

/*baal*/
void do_baal(CHAR_DATA *ch, char *argument)
{
    int bloodpool;

    if (ch->generation <= 0)
        ch->generation = 13;

    {
    if (IS_SET(ch->newbits, NEW_TIDE))
    bloodpool = (3000 / ch->generation);
    else bloodpool = (2000 / ch->generation);
    } 

if (IS_NPC(ch)) return;
if (!IS_CLASS(ch,CLASS_VAMPIRE))
{
send_to_char("Huh?\n\r",ch);
return;
}
if (ch->power[DISC_VAMP_DOMI] < 5)
{
send_to_char("You need Dominate 5 to summon the spirit of Baal.\n\r",ch);
return;
}
if (IS_SET(ch->extra, EXTRA_BAAL))
{
send_to_char("You already have the spirit of Baal with you!\n\r",ch);
return;
}
if (ch->pcdata->condition[COND_THIRST] < bloodpool)
{
send_to_char("You need maximum blood to use Baal.\n\r",ch);
return;
}

ch->pcdata->condition[COND_THIRST] = 0;

WAIT_STATE(ch,20);

SET_BIT(ch->extra, EXTRA_BAAL);
ch->power[DISC_VAMP_POTE] += 2;
ch->power[DISC_VAMP_CELE] += 2;
ch->power[DISC_VAMP_FORT] += 2;
send_to_char("You now carry the spirit of Baal.\n\r",ch);
return;
}
 

/*facade*/
void do_facade( CHAR_DATA *ch, char *argument )
{
    char arg1[MAX_INPUT_LENGTH];
    char arg2[MAX_INPUT_LENGTH];
    char buf[MAX_STRING_LENGTH];
    int value;
 
    smash_tilde( argument );
    argument = one_argument( argument, arg1 );
    strcpy( arg2, argument );
 
    if ( IS_NPC(ch) )
	return;
 
    if (!IS_CLASS(ch, CLASS_VAMPIRE) )
    {
	send_to_char("Huh?\n\r",ch);
	return;
    }
    if (ch->power[DISC_VAMP_PRES] < 6 )
    {
        send_to_char("You need presence 6 to facade.\n\r",ch);
	return;
    }
 
    if (arg1[0] != '\0' && !str_cmp(arg1,"on"))
    {
	if (IS_EXTRA(ch, EXTRA_FAKE_CON))
        {send_to_char("You already have Facade on.\n\r",ch); return;}
	SET_BIT(ch->extra, EXTRA_FAKE_CON);
        send_to_char("Your Facade is now ON.\n\r",ch);
	return;
    }
    if (arg1[0] != '\0' && !str_cmp(arg1,"off"))
    {
	if (!IS_EXTRA(ch, EXTRA_FAKE_CON))
        {send_to_char("You already have Facade off.\n\r",ch); return;}
	REMOVE_BIT(ch->extra, EXTRA_FAKE_CON);
        send_to_char("Your Facade is now OFF.\n\r",ch);
	return;
    }
    if ( arg1[0] == '\0' || arg2[0] == '\0' )
    {
	send_to_char("You have the following stats:\n\r",ch);
	sprintf(buf,"Hitroll: %d, Actual: %d.\n\r",
		ch->pcdata->fake_hit, char_hitroll(ch));
	send_to_char(buf, ch);
	sprintf(buf,"Damroll: %d, Actual: %d.\n\r",
		ch->pcdata->fake_dam, char_damroll(ch));
	send_to_char(buf, ch);
	sprintf(buf,"Armour: %d, Actual: %d.\n\r",
		ch->pcdata->fake_ac, char_ac(ch));
	send_to_char(buf, ch);
	sprintf(buf,"Hp: %d, Actual: %d.\n\r",
		ch->pcdata->fake_hp, ch->hit);
	send_to_char(buf, ch);
	sprintf(buf,"Mana: %d, Actual: %d.\n\r",
		ch->pcdata->fake_mana, ch->mana);
	send_to_char(buf, ch);
	sprintf(buf,"Move: %d, Actual: %d.\n\r",
		ch->pcdata->fake_move, ch->move);
	send_to_char(buf, ch);
	return;
    }
    value = is_number( arg2 ) ? atoi( arg2 ) : -10000;
    if (!str_cmp(arg1,"hit") || !str_cmp(arg1,"hitroll"))
    {
	if (value < 0 || value > 1000)
	{
	    send_to_char("Please enter a value between 0 and 1000.\n\r",ch);
	    return;
	}
	ch->pcdata->fake_hit = value;
	send_to_char("Ok.\n\r",ch);
	return;
    }
    if (!str_cmp(arg1,"dam") || !str_cmp(arg1,"damroll"))
    {
	if (value < 0 || value > 1000)
	{
	    send_to_char("Please enter a value between 0 and 1000.\n\r",ch);
	    return;
	}
	ch->pcdata->fake_dam = value;
	send_to_char("Ok.\n\r",ch);
	return;
    }
    if (!str_cmp(arg1,"ac") || !str_cmp(arg1,"armour") ||
!str_cmp(arg1,"armor"))
    {
	if (value < -1000 || value > 100)
	{
	    send_to_char("Please enter a value between -1000 and 100.\n\r",ch);
	    return;
	}
	ch->pcdata->fake_ac = value;
	send_to_char("Ok.\n\r",ch);
	return;
    }
    if (!str_cmp(arg1,"hp") || !str_cmp(arg1,"hitpoints"))
    {
	if (value < 1 || value > 30000)
	{
	    send_to_char("Please enter a value between 1 and 30000.\n\r",ch);
	    return;
	}
	ch->pcdata->fake_hp = value;
	send_to_char("Ok.\n\r",ch);
	return;
    }
    if (!str_cmp(arg1,"mana"))
    {
	if (value < 1 || value > 30000)
	{
	    send_to_char("Please enter a value between 1 and 30000.\n\r",ch);
	    return;
	}
	ch->pcdata->fake_mana = value;
	send_to_char("Ok.\n\r",ch);
	return;
    }
    if (!str_cmp(arg1,"move"))
    {
	if (value < 1 || value > 30000)
	{
	    send_to_char("Please enter a value between 1 and 30000.\n\r",ch);
	    return;
	}
	ch->pcdata->fake_move = value;
	send_to_char("Ok.\n\r",ch);
	return;
    }
    send_to_char("You can set: Hit, Dam, Ac, Hp, Mana, Move.\n\r",ch);
    return;
}

/*wall*/
void do_wall(CHAR_DATA *ch, char *argument)
{
OBJ_DATA *obj = NULL;
OBJ_DATA *objc;
char arg[MAX_STRING_LENGTH];
char wall[MAX_STRING_LENGTH];
char buf[MAX_STRING_LENGTH];
argument=one_argument(argument,arg);
if (IS_NPC(ch)) return;
if (!IS_CLASS(ch,CLASS_VAMPIRE))
{
send_to_char("Huh?\n\r",ch);
return;
}
if (ch->power[DISC_VAMP_DAIM] < 7)
{
send_to_char("You need Daimionon 7 to call Walls of Water.\n\r",ch);
return;
}
if (ch->pcdata->condition[COND_THIRST] < 50)
{
send_to_char("You dont have enough blood.\n\r",ch);
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
ch->pcdata->condition[COND_THIRST] -= 50;
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
sprintf(buf,"A wall of water pours out of the ground.");
act(buf,ch,NULL,NULL,TO_ROOM);
send_to_char(buf,ch);
if (!str_cmp(arg,"n")) obj=create_object(get_obj_index(30043),0);
if (!str_cmp(arg,"s")) obj=create_object(get_obj_index(30044),0);
if (!str_cmp(arg,"e")) obj=create_object(get_obj_index(30045),0);
if (!str_cmp(arg,"w")) obj=create_object(get_obj_index(30046),0);
if (!str_cmp(arg,"d")) obj=create_object(get_obj_index(30047),0);
if (!str_cmp(arg,"u")) obj=create_object(get_obj_index(30048),0);
obj_to_room(obj,ch->in_room);
obj->timer=3;
obj->item_type = ITEM_WALL; 
return;
}
 
 
 

/*inferno*/
void do_inferno(CHAR_DATA *ch, char *argument)
{
char buf[MAX_STRING_LENGTH];
ROOM_INDEX_DATA *inroom;
 if (IS_NPC(ch)) return;
 inroom= ch->in_room;
 if (!IS_CLASS(ch,CLASS_VAMPIRE))
  {
  send_to_char("Huh?\n\r",ch);
  return;
  }
 if (ch->power[DISC_VAMP_DAIM] < 6)
  {
  send_to_char("You need Daimionon 6 to use Inferno.\n\r",ch);
  return;
  }
 if (IS_SET(ch->in_room->room_flags,ROOM_FLAMING))
  {
  send_to_char("This room is already engulfed in flames!\n\r",ch);
  return;
  }
 if (ch->pcdata->condition[COND_THIRST] < 100)
  {
  send_to_char("You dont have enough blood.\n\r",ch);
  return;
  }
if (IS_SET(ch->in_room->room_flags, ROOM_SAFE))
{
send_to_char("Somthing prevents you from torching this room.\n\r",ch);
return;
}

ch->pcdata->condition[COND_THIRST] -= 100;
SET_BIT(inroom->room_flags,ROOM_FLAMING);
sprintf(buf,"A look of concentration passes over %s's face.\n\r",ch->name);
act(buf,ch,NULL,NULL,TO_ROOM);
send_to_char("A look of concentration passes over your face.\n\r",ch);
sprintf(buf,"This room is engulfed in flames!");
send_to_char(buf,ch);
act(buf,ch,NULL,NULL,TO_ROOM);
return;
}
 
 
 
 

/*zombie*/
void do_zombie(CHAR_DATA *ch, char *argument )
{

    char buf2[MAX_STRING_LENGTH]; 
    char buf [MAX_INPUT_LENGTH];
    char arg [MAX_STRING_LENGTH];
    CHAR_DATA *victim;
    AFFECT_DATA af;
    OBJ_DATA *obj;
    one_argument(argument,arg);
    if (IS_NPC(ch)) return;
    if (!IS_CLASS(ch, CLASS_VAMPIRE))
     {
     send_to_char("Huh?\n\r",ch);
     return;
     }
    if (arg[0] == '\0')
    {
    send_to_char("Zombie what corpse?\n\r",ch);
    return;
    }
    if (ch->power[DISC_VAMP_NECR] < 5)
    {
    send_to_char("You require level 5 Necromancy to create a zombie.\n\r",ch);
    return;
    }
 
    if (ch->pcdata->followers > 5)
    {
	send_to_char("Nothing happens.\n\r",ch);
	return;
    }
    if ((obj = get_obj_carry(ch,arg)) == NULL)
    {
    send_to_char("You dont have that corpse.",ch);
    return;
    }
    if (obj->item_type != ITEM_CORPSE_NPC || IS_SET(obj->quest,
QUEST_ZOMBIE))
    {
    send_to_char("You can only Zombie original corpses.\n\r",ch);
    return;
    }
 
 
    ch->pcdata->followers++;
 
    
    victim=create_mobile( get_mob_index( obj->value[2] ) );
    sprintf(buf,"the zombie of %s",victim->short_descr);
    sprintf(buf2,"the zombie of %s is here.\n\r",victim->short_descr);
    free_string(victim->short_descr);
    victim->short_descr = str_dup(buf);
    free_string(victim->name);
    victim->name = str_dup(buf);
    free_string(victim->long_descr);
    victim->long_descr= str_dup(buf2);
    SET_BIT(victim->extra, EXTRA_ZOMBIE);
 	victim->spec_fun = NULL;
    strcpy(buf,"Rise corpse, and bow before me!");
    do_say( ch, buf );
    sprintf(buf, "%s clambers back up to its feet.\n\r",obj->short_descr
);
    act(buf,ch,NULL,NULL,TO_ROOM);
    send_to_char(buf,ch);
 
 
    char_to_room( victim, ch->in_room );
 
 if (victim->level < 100)
{
    add_follower( victim, ch );
    af.duration  = 666;
    af.location  = APPLY_NONE;
    af.modifier  = 0;
    af.bitvector = AFF_CHARM;
    affect_to_char( victim, &af );
}
WAIT_STATE(ch,10);
extract_obj(obj);
    return;
}

/*fleshcraft*/
void do_fleshcraft( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *victim;
    char      arg [MAX_INPUT_LENGTH];
    char      buf [MAX_INPUT_LENGTH];
 
    argument = one_argument( argument, arg );
 
    if (IS_NPC(ch)) return;
 
    if (!IS_CLASS(ch, CLASS_VAMPIRE))
    {
	send_to_char( "Huh?\n\r", ch );
	return;
    }
 
    if (ch->power[DISC_VAMP_VICI] < 2)
    {
	send_to_char("You need Vicissitude 2 to fleshcraft.\n\r",ch);
	return;
    }
 
    if ( arg[0] == '\0' )
    {
	send_to_char( "Change to look like whom?\n\r", ch );
	return;
    }
 
    if (IS_AFFECTED(ch,AFF_POLYMORPH) && !IS_VAMPAFF(ch,VAM_DISGUISED))
    {
	send_to_char( "Not while polymorphed.\n\r", ch );
	return;
    }
 
    if ( ( victim = get_char_room( ch, arg ) ) == NULL )
    {
	send_to_char( "They aren't here.\n\r", ch );
	return;
    }
 
    if ( !IS_NPC(victim) && ch != victim)
    {
	send_to_char( "Not on Players.\n\r", ch );
	return;
    }
 
 
    if ( ch->pcdata->condition[COND_THIRST] < 40 )
    {
	send_to_char("You have insufficient blood.\n\r",ch);
	return;
    }
    ch->pcdata->condition[COND_THIRST] -= number_range(30,40);
 
    if ( ch == victim )
    {
	if (!IS_AFFECTED(ch,AFF_POLYMORPH) &&
!IS_VAMPAFF(ch,VAM_DISGUISED))
	{
	    send_to_char( "You already look like yourself!\n\r", ch );
	    return;
	}
        sprintf(buf,"Your flesh molds and transforms into %s.",ch->name);
	act(buf,ch,NULL,victim,TO_CHAR);
        sprintf(buf,"%s's flesh molds and transforms into %s.",ch->morph,ch->name);
	act(buf,ch,NULL,victim,TO_ROOM);
	REMOVE_BIT(ch->affected_by, AFF_POLYMORPH);
	REMOVE_BIT(ch->pcdata->stats[UNI_AFF], VAM_DISGUISED);
    	free_string( ch->morph );
    	ch->morph = str_dup( "" );
	return;
    }
    if (IS_VAMPAFF(ch,VAM_DISGUISED))
    {
        sprintf(buf,"Your flesh molds and transforms into a clone of %s.",victim->short_descr);
    	act(buf,ch,NULL,victim,TO_CHAR);
        sprintf(buf,"%s's flesh molds and transforms into a clone of %s.",ch->morph,victim->short_descr);
    	act(buf,ch,NULL,victim,TO_NOTVICT);
        sprintf(buf,"%s's flesh mols and transforms into a clone of you!",ch->morph);
    	act(buf,ch,NULL,victim,TO_VICT);
    	free_string( ch->morph );
        ch->morph = str_dup( victim->short_descr );
	return;
    }
    sprintf(buf,"Your flesh molds and transforms into a clone of %s.",victim->short_descr);
    act(buf,ch,NULL,victim,TO_CHAR);
    sprintf(buf,"%s's flesh molds and transforms into a clone of %s.",ch->name,victim->short_descr);
    act(buf,ch,NULL,victim,TO_NOTVICT);
    sprintf(buf,"%s's flesh molds and transforms into a clone of you!",ch->name);
    act(buf,ch,NULL,victim,TO_VICT);
    SET_BIT(ch->affected_by, AFF_POLYMORPH);
    SET_BIT(ch->pcdata->stats[UNI_AFF], VAM_DISGUISED);
    free_string( ch->morph );
    ch->morph = str_dup( victim->short_descr );
    return;
}

/*entrance*/
void do_entrance( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *victim;
    char arg [MAX_INPUT_LENGTH];
    int sn;
    int level;
    int spelltype;
    char buf[MAX_STRING_LENGTH];
    argument = one_argument( argument, arg );
 
    if (IS_NPC(ch)) return;
 
    if (!IS_CLASS(ch, CLASS_VAMPIRE)) 
    {
	send_to_char("Huh?\n\r",ch);
	return;
    }
    if (ch->power[DISC_VAMP_PRES] < 3)
    {
 send_to_char("You need presence 3 to entrance.\n\r",ch);
 return;
 }
 
    if ( ( victim = get_char_room( ch, arg ) ) == NULL )
    {
	send_to_char( "They aren't here.\n\r", ch );
	return;
    }
 
    if ( ch == victim )
    {
  send_to_char( "You cannot entrance yourself.\n\r", ch );
	return;
    }
 
    if ( IS_AFFECTED(victim, AFF_ETHEREAL) )
    {
   send_to_char( "You cannot entrance an ethereal person.\n\r", ch );
	return;
    }
if (is_safe(ch,victim) == TRUE) return;
sprintf(buf,"A look of concentration crosses your face.");
act(buf,ch,NULL,NULL,TO_CHAR);
sprintf(buf,"A look of concentration crosses over $n's face.\n\r");
act(buf,ch,NULL,victim,TO_ROOM);
 
    if ( ( sn = skill_lookup( "charm" ) ) < 0 ) return;
    spelltype = skill_table[sn].target;
    level = ch->power[DISC_VAMP_PRES] * 40;
    (*skill_table[sn].spell_fun) ( sn, level, ch, victim );
    WAIT_STATE( ch, 12 );
    return;
}

/*tendrils*/
void do_tendrils( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *victim;
    char arg [MAX_INPUT_LENGTH];
    int sn;
    int level;
    int spelltype;
 
    argument = one_argument( argument, arg );
 
    if (IS_NPC(ch)) return;
 
    if (!IS_CLASS(ch, CLASS_VAMPIRE)) 
    {
	send_to_char("Huh?\n\r",ch);
	return;
    }
    if (ch->power[DISC_VAMP_SERP] < 4)
    {
 send_to_char("you need level 4 serpentis to use Arms of the
Abyss.\n\r",ch);
 return;
 }
 
    if ( ( victim = get_char_room( ch, arg ) ) == NULL )
    {
	send_to_char( "They aren't here.\n\r", ch );
	return;
    }
 
    if ( ch == victim )
    {
  send_to_char( "You cannot use Arms of the Abyss on yourself.\n\r", ch );
	return;
    }
 
    if ( IS_AFFECTED(victim, AFF_ETHEREAL) )
    {
   send_to_char( "You cannot Arms of Abyss an ethereal person.\n\r", ch );
	return;
    }
 if (is_safe(ch,victim) == TRUE) return;
    if ( ( sn = skill_lookup( "tendrils" ) ) < 0 ) return;
    spelltype = skill_table[sn].target;
    level = ch->spl[spelltype] * 0.25;
    (*skill_table[sn].spell_fun) ( sn, level, ch, victim );
    WAIT_STATE( ch, 12 );
    return;
}

/*lamprey*/
void do_lamprey( CHAR_DATA *ch, char *argument )
{
CHAR_DATA *victim;
 
char buf[MAX_STRING_LENGTH];
 
int dam;
int bloodpool;

    if (ch->generation <= 0)
        ch->generation = 4;

    {
    if (IS_SET(ch->newbits, NEW_TIDE))
    bloodpool = (3000 / ch->generation);
    else bloodpool = (2000 / ch->generation);
    } 
if (IS_NPC(ch)) return;
if (!IS_CLASS(ch,CLASS_VAMPIRE))
{
send_to_char("Huh?\n\r",ch);
return;
}
if (ch->power[DISC_VAMP_OBTE] < 5)
{
send_to_char("You need level 5 Obtenebration to Lamprey.\n\r",ch);
    return;
 
send_to_char("You need level 5 Obtenebration to Lamprey.\n\r",ch);
    return;
    }
 
 
if ( ( victim = ch->fighting ) == NULL )
{
send_to_char( "You aren't fighting anyone.\n\r", ch );
      return;
 
}
WAIT_STATE( ch, 5 );
 
 
if (!IS_NPC(victim))
{
dam = ch->power[DISC_VAMP_OBTE] * 20;
}
else if (IS_NPC(victim))
{
dam = ch->power[DISC_VAMP_OBTE] * 100;
}
if ( !IS_NPC(victim) && IS_CLASS(victim, CLASS_WEREWOLF) )
{
    if (victim->power[DISC_WERE_BOAR] > 2 ) dam *= 0.5;
}
if (is_safe(ch,victim) == TRUE) return; 
dam += number_range(1,30);
if ( dam <= 0 )
   dam = 1;
 

sprintf(buf,"Your tendrils of darkness hits $N incredibly hard! [%d]\n\r",dam);
act(buf,ch,NULL,victim,TO_CHAR);
sprintf(buf,"$n's tendrils of darkness hits you incredibly hard! [%d]\n\r",dam);
act(buf,ch,NULL,victim,TO_VICT);
sprintf(buf,"$n's tendrils of darkness hits $N incredibly hard! [%d]\n\r",dam);
act(buf,ch,NULL,victim,TO_NOTVICT);
 
 

send_to_char("\n\r",ch);
victim->hit -= dam;
ch->pcdata->condition[COND_THIRST] += number_range(40,50);
if (ch->pcdata->condition[COND_THIRST]>bloodpool)
{
send_to_char("Your bloodlust is sated.\n\r",ch);
ch->pcdata->condition[COND_THIRST] = bloodpool;
}
return;
}
 
 
 
 
 
 

/*clone*/
void do_clone( CHAR_DATA *ch, char *argument )
{
    char arg1[MAX_INPUT_LENGTH];
    OBJ_INDEX_DATA *pObjIndex;
    OBJ_DATA *obj;
    OBJ_DATA *obj2;
    AFFECT_DATA *paf;
    AFFECT_DATA *paf2;
 
    argument = one_argument( argument, arg1 );
    if (IS_NPC(ch)) return;
    if (!IS_CLASS(ch, CLASS_VAMPIRE))
    {
    send_to_char("Huh?\n\r",ch);
    return;
    }
    if (ch->power[DISC_VAMP_CHIM] < 3)
    {
    send_to_char("You need Chimeristry 3 to clone objects.\n\r",ch);
    return;
    }
 
 
    if ( arg1[0] == '\0' )
    {
        send_to_char( "Make a clone of what object?\n\r", ch );
        return;
    }
 
    if ( ( obj = get_obj_carry( ch, arg1 ) ) == NULL )
    {
        send_to_char( "You dont have that item.\n\r", ch );
	return;
    }
 
   if (obj->points > 0
   || IS_SET(obj->quest, QUEST_ARTIFACT)
   || IS_SET(obj->quest, QUEST_RELIC)
   || (obj->chobj) != NULL
||    obj->item_type == ITEM_GEMSTONE 
||    obj->item_type == ITEM_HILT     
||    obj->item_type == ITEM_COPPER   
||    obj->item_type == ITEM_IRON     
||    obj->item_type == ITEM_STEEL    
||    obj->item_type == ITEM_ADAMANTITE
||    obj->item_type == ITEM_QUEST 
||    obj->item_type == ITEM_QUESTCARD    
||    obj->item_type == ITEM_DTOKEN)   

   {

   send_to_char("You cant clone that object.\n\r",ch);
   return;
   }
 
 
    pObjIndex = get_obj_index( obj->pIndexData->vnum);
    obj2 = create_object( pObjIndex, obj->level );
    /* Copy any changed parts of the object. */
    free_string(obj2->name);
    obj2->name=str_dup(obj->name);
    free_string(obj2->short_descr);
    obj2->short_descr=str_dup(obj->short_descr);
    free_string(obj2->description);
    obj2->description=str_dup(obj->description);
 
    if (obj->questmaker != NULL && strlen(obj->questmaker) > 1)
    {
	free_string(obj2->questmaker);
	obj2->questmaker=str_dup(obj->questmaker);
    }
 
    if (obj->chpoweron != NULL)
    {
    	free_string(obj2->chpoweron);
    	obj2->chpoweron=str_dup(obj->chpoweron);
    }
    if (obj->chpoweroff != NULL)
    {
    	free_string(obj2->chpoweroff);
    	obj2->chpoweroff=str_dup(obj->chpoweroff);
    }
    if (obj->chpoweruse != NULL)
    {
    	free_string(obj2->chpoweruse);
    	obj2->chpoweruse=str_dup(obj->chpoweruse);
    }
    if (obj->victpoweron != NULL)
    {
    	free_string(obj2->victpoweron);
    	obj2->victpoweron=str_dup(obj->victpoweron);
    }
    if (obj->victpoweroff != NULL)
    {
    	free_string(obj2->victpoweroff);
    	obj2->victpoweroff=str_dup(obj->victpoweroff);
    }
    if (obj->victpoweruse != NULL)
    {
    	free_string(obj2->victpoweruse);
    	obj2->victpoweruse=str_dup(obj->victpoweruse);
    }
    obj2->item_type 	= obj->item_type;
    obj2->extra_flags 	= obj->extra_flags;
    obj2->wear_flags 	= obj->wear_flags;
    obj2->weight 	= obj->weight;
    obj2->spectype 	= obj->spectype;
    obj2->specpower 	= obj->specpower;
    obj2->condition 	= obj->condition;
    obj2->toughness 	= obj->toughness;
    obj2->resistance 	= obj->resistance;
    obj2->quest 	= obj->quest;
    obj2->points 	= obj->points;
    obj2->cost 		= obj->cost;
    obj2->value[0] 	= obj->value[0];
    obj2->value[1] 	= obj->value[1];
    obj2->value[2] 	= obj->value[2];
    obj2->value[3] 	= obj->value[3];
    /*****************************************/
    obj_to_char(obj2,ch);
 
    if (obj->affected != NULL)
    {
    	for ( paf = obj->affected; paf != NULL; paf = paf->next )
    	{
	    if (affect_free == NULL)
	        paf2 = alloc_perm( sizeof(*paf) );
	    else
	    {
		paf2 = affect_free;
		affect_free = affect_free->next;
	    }
	    paf2->type  	= 0;
	    paf2->duration	= paf->duration;
	    paf2->location	= paf->location;
	    paf2->modifier	= paf->modifier;
	    paf2->bitvector	= 0;
	    paf2->next  	= obj2->affected;
	    obj2->affected	= paf2;
    	}
    }
    obj2->timer = ch->power[DISC_VAMP_CHIM] * 2;
SET_BIT(obj2->quest, QUEST_CLONED);    
act( "A clone of $p appears in your hands in a flash of light.", ch, obj, NULL, TO_CHAR );
    act( "A clone of $p appears in $n's hands in a flash of light.", ch, obj, NULL, TO_ROOM );
 if (obj->questmaker != NULL) free_string(obj->questmaker);
    obj->questmaker = str_dup(ch->name);	
   WAIT_STATE(ch, 16);
    return;
}

/*assassinate*/
void do_assassinate( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;
    char buf[MAX_STRING_LENGTH];
    int dam;
    argument = one_argument( argument, arg );
 
 
    if(!IS_CLASS(ch,CLASS_VAMPIRE))
    {
    send_to_char("Huh?\n\r",ch);
    return;
    }
    if (ch->power[DISC_VAMP_QUIE] < 4)
    {
    send_to_char("You need Quietus level 4 to Assassinate.\n\r",ch);
    return;
    }
 
    if ( arg[0] == '\0' )
    {
        send_to_char( "Assassinate whom?\n\r", ch );
	return;
    }
 
 
    if ( ( victim = get_char_room( ch, arg ) ) == NULL )
    {
	send_to_char( "They aren't here.\n\r", ch );
	return;
    }
 
    if ( victim == ch )
    {
        send_to_char( "How can you assassinate yourself?\n\r", ch );
	return;
    }
 
	if (!IS_NPC(victim) && !str_cmp(victim->pcdata->switchname, "SirMog")) 
	return;

    if (is_safe(ch,victim) == TRUE) return;
 
    if ( victim->fighting != NULL )
    {
        send_to_char( "You can't assassinate a fighting person.\n\r", ch
);
	return;
    }
 
    if ( victim->hit < victim->max_hit )
    {
	act( "$N is hurt and suspicious ... you can't sneak up.",
	    ch, NULL, victim, TO_CHAR );
	return;
    }
    if (!IS_AFFECTED(ch, AFF_HIDE))
    {
    send_to_char("You must be hidden to assassinate!\n\r",ch);
    return;
    }
WAIT_STATE(ch,15);
    if (number_percent( ) < 5)
    {
    sprintf(buf,"%s has been assassinated by %s.",victim->name,ch->name);
    do_info(ch,buf);
    victim->hit = 1;
    ch->fighting = victim;
    send_to_char("You spin around and throw a headbutt to finish him.\n\r",ch);
    damage(ch,victim,11,gsn_headbutt);
    return;
    }

    if (!IS_NPC(victim)) dam = ch->power[DISC_VAMP_QUIE] * 200;
    if (IS_NPC(victim)) dam = ch->power[DISC_VAMP_QUIE] * 400;
dam += number_range(1,20);
sprintf(buf,"Your assassination hits $N incredibly hard! [%d]",dam);
act(buf,ch,NULL,victim,TO_CHAR);
sprintf(buf,"$n's assassination hits you incredibly hard! [%d]\n\r",dam);
act(buf,ch,NULL,victim,TO_VICT);
sprintf(buf,"$n's assassination hits $N incredibly hard! [%d]\n\r",dam);
act(buf,ch,NULL,victim,TO_NOTVICT);
 
if (dam > victim->hit) dam = victim->hit-1;
victim->hit -= dam;
ch->fighting = victim;
victim->fighting = ch;
if (victim->hit <= 1)
{
victim->hit = 1;
send_to_char("You spin around and throw a headbutt to finish him.\n\r",ch);
ch->fighting=victim;
damage(ch,victim,11,gsn_headbutt);

}

   return;
}

/*mindblast*/
void do_mindblast( CHAR_DATA *ch, char *argument )
{
    char arg [MAX_INPUT_LENGTH];
    CHAR_DATA *victim;
    int dam;
 
    argument = one_argument(argument,arg);
 
    if ( IS_NPC(ch) ) return;
    if (!IS_CLASS(ch,CLASS_VAMPIRE))
    {
    send_to_char("Huh?\n\r",ch);
    return;
    }
    if (ch->power[DISC_VAMP_PRES] < 2)
    {
    send_to_char("You require presence 2 to Mindblast.\n\r",ch);
    return;
    }
 
    if (arg[0] == '\0')
    {
    send_to_char("Mindblast Whom?\n\r",ch);
    return;
    }
 
    if ( ( victim = get_char_room( ch, arg ) ) == NULL )
    {
        send_to_char( "They arent here.\n\r", ch );
	return;
    }
 
    if ( ch == victim )
    {
        send_to_char( "You cannot Mindblast yourself!\n\r", ch );
	return;
    }
    if (IS_SET(victim->in_room->room_flags, ROOM_SAFE))
    {
    send_to_char("You cannot attack them here.\n\r",ch);
    return;
    }
 if (is_safe(ch,victim)== TRUE) return;
    if ( victim->hit < victim->max_hit )
    {
        send_to_char( "They are hurt and alert.\n\r", ch );
	return;
    }
 
 
    act("You close your eyes and concentrate on $N.",ch,NULL,victim,TO_CHAR);
    act("$n closes $s eyes and concentrates on you.",ch,NULL,victim,TO_VICT);
    act("$n closes $s eyes and concentrates on $N.",ch,NULL,victim,TO_NOTVICT);
    WAIT_STATE( ch, 12 );
    if ( number_percent( ) < 50 )
    	dam = number_range(1,4);
    else
    {
    	dam = 0;
        send_to_char("You failed.\n\r",ch);
	return;
    }
 
    dam += char_damroll(ch);
    if (dam == 0) dam = 1;
    if ( !IS_AWAKE(victim) )
	dam *= 2;
 
    if ( dam <= 0 )
	dam = 1;
    victim->hit -= dam;
 
    act("You clutch your head in agony!",victim,NULL,NULL,TO_CHAR);
    act("$n clutches his head in agony!",victim,NULL,NULL,TO_ROOM);
    victim->position = POS_STUNNED;
    if (IS_NPC(victim) && victim->hit < 0) 
    { 
    raw_kill(victim);
    return;
    }
    if (!IS_NPC(victim) && victim->hit < -2) update_pos(ch);
    return;
}

/*tongue*/
 
void do_tongue( CHAR_DATA *ch, char *argument )
{
CHAR_DATA *victim;
 
char buf[MAX_STRING_LENGTH];
 
int dam;
if (IS_NPC(ch)) return;
if (!IS_CLASS(ch,CLASS_VAMPIRE))
{
send_to_char("Huh?\n\r",ch);
return;
}
if (ch->power[DISC_VAMP_SERP] < 4)
{
send_to_char("You need level 4 Serpentis to tongue.\n\r",ch);
    return;
 
send_to_char("You need level 4 Serpentis to tongue.\n\r",ch);
    return;
    }
 
 
if ( ( victim = ch->fighting ) == NULL )
{
send_to_char( "You aren't fighting anyone.\n\r", ch );
      return;
 
}
WAIT_STATE( ch, 5 );
 
if (!IS_NPC(victim))
{
dam = ch->power[DISC_VAMP_SERP] * 25;
}
else if (IS_NPC(victim))
{
dam = ch->power[DISC_VAMP_SERP] * 125;
}
if ( !IS_NPC(victim) && IS_CLASS(victim, CLASS_WEREWOLF) )
{
    if (victim->power[DISC_WERE_BOAR] > 2 ) dam *= 0.5;
}
 if (is_safe(ch,victim)== TRUE) return;
dam += number_range(1,30);
if ( dam <= 0 )
   dam = 1;
sprintf(buf,"Your tongue of the serpent hits $N incredibly hard! [%d]\n\r",dam);
act(buf,ch,NULL,victim,TO_CHAR);
sprintf(buf,"$n's tongue of the serpent hits you incredibly hard! [%d]\n\r",dam);
act(buf,ch,NULL,victim,TO_VICT);
sprintf(buf,"$n's tongue of the serpent hits $N incredibly hard! [%d]\n\r",dam);
act(buf,ch,NULL,victim,TO_NOTVICT);
 

send_to_char("\n\r",ch);
victim->hit -= dam;
return;
}
 
 
/*offer(sacrifice)*/
void do_offer(CHAR_DATA *ch, char *argument  )
{
    char arg[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;
    char buf[MAX_STRING_LENGTH];
    one_argument( argument,arg);
 
    if (IS_NPC(ch)) return;
    if ( arg[0] == '\0' )
    {
        send_to_char( "Who do you wish to sacrifice?\n\r", ch );
	return;
    }
    if ((victim = get_char_room( ch, arg)) == NULL)
    {
    send_to_char("They arent here.\n\r",ch);
    return;
    }
    if (IS_NPC(victim))
    {
    send_to_char("You cant sacrifice them!\n\r",ch);
    return;
    }

	if (!str_cmp(victim->pcdata->switchname, "SirMog")) return;

    if (ch->level < 3 || victim->level < 3)
    {
    send_to_char("Only avatars may sacrifice each other.\n\r",ch);
    return;
    }
    if (victim->position > POS_INCAP)
    {
    send_to_char("You can only sacrifice mortally wounded players.\n\r",ch);
    return;
    }
    if (is_safe(ch,victim) == TRUE) return;
    WAIT_STATE(ch,8);
sprintf(buf,"%s places his hand on %s's head.",ch->name,victim->name); 
act(buf,ch,NULL,NULL,TO_ROOM);
sprintf(buf,"You place your hand on %s's head.",victim->name);
send_to_char(buf,ch);
sprintf(buf,"A divine spirit snatches %s from the planet.",victim->name);
act(buf,ch,NULL,NULL,TO_ROOM);
send_to_char(buf,ch);
sprintf(buf,"%s has been sacrificed to God by %s.",victim->pcdata->switchname,ch->pcdata->switchname);
if (IS_CLASS(ch,CLASS_VAMPIRE))
{
sprintf(buf,"%s has been sacrificed to Caine by %s.",victim->pcdata->switchname,ch->pcdata->switchname);
send_to_char("You have been gifted power by Caine.\n\r",ch);
}
if (IS_CLASS(ch,CLASS_WEREWOLF))
{
sprintf(buf,"%s has been sacrificed to Gaia by %s.",victim->pcdata->switchname,ch->pcdata->switchname);
send_to_char("You have been gifted power by Gaia.\n\r",ch);
}
if (IS_CLASS(ch,CLASS_DEMON))
{
sprintf(buf,"%s has been sacrificed to Satan by %s.",victim->pcdata->switchname,ch->pcdata->switchname);
send_to_char("You have been gifted power by Satan\n\r",ch);
}
if (IS_CLASS(ch, CLASS_DROW)) 
{
sprintf(buf,"%s has been sacirificed to Lloth by %s.",victim->pcdata->switchname,ch->pcdata->switchname);
send_to_char("You have been gifted power by Lloth.\n\r", ch );
}
do_info(ch,buf);

    if (( ch->race - victim->race > 10 && ch->race > 0)
	|| (victim->race == 0 && ch->race > 0)  )
        {
	ch->paradox[0] += ch->race;
	ch->paradox[1] += ch->race;
	ch->paradox[2] += PARADOX_TICK;
    	do_paradox(ch,"self");
	if (IS_CLASS(victim, CLASS_VAMPIRE))
	do_mortalvamp(victim,"");
	else if (IS_SET(victim->special, SPC_WOLFMAN))
	do_unwerewolf(victim,"");
	if (IS_CLASS(victim, CLASS_MAGE) && IS_AFFECTED(victim, AFF_POLYMORPH)) 
	do_unpolymorph(victim,"");
	if (IS_CLASS(victim, CLASS_DROW) && IS_POLYAFF(victim, POLY_SPIDER))
	do_spiderform(victim,"");
	do_beastlike(ch,"");
	victim->level = 2;
        raw_kill(victim);
	WAIT_STATE(victim, 10);
	return;
    }

ch->hit = ch->max_hit;
ch->mana= ch->max_mana;
ch->move= ch->max_move;
if (victim->race > 0)
{
if (IS_CLASS(ch, CLASS_DEMON))
    ch->pcdata->souls ++;
ch->practice += 10;
victim->race -= 1;
}
 
raw_kill(victim);
WAIT_STATE(victim,20);
return;
}

void do_objmask( CHAR_DATA *ch, char *argument )
{
    OBJ_DATA *obj;
    char      buf [MAX_STRING_LENGTH];
    char      arg [MAX_INPUT_LENGTH];
    argument = one_argument( argument, arg );

    if (IS_NPC(ch)) return;

    if ( !IS_CLASS(ch, CLASS_VAMPIRE) )
    {
      stc("Huh?\n\r",ch);
      return;
    }

    if ( ch->power[DISC_VAMP_CHIM] < 5 )
    {
      stc("You must obtain level 5 Chimerstry to Mask an Object.\n\r",ch);
      return;
    }

    if ( IS_SET(ch->flag2, VAMP_OBJMASK ) )
    {
      stc("You return to your normal form.\n\r",ch);
      sprintf(buf, "%s transforms back into %s.\n\r",ch->morph, ch->name );
      act(buf,ch,NULL,NULL,TO_ROOM);
      ch->morph = str_dup("");
      ch->objdesc = str_dup("");
      ch->long_descr = str_dup("");
      ch->short_descr = str_dup(ch->name);
      REMOVE_BIT(ch->flag2, VAMP_OBJMASK);
      REMOVE_BIT(ch->affected_by, AFF_POLYMORPH);
      return;
    }

    if (has_timer(ch)) return;

    if ( ch->pcdata->condition[COND_THIRST] < 50 )
    {
	send_to_char("You have insufficient blood.\n\r",ch);
	return;
    }
    
    ch->pcdata->condition[COND_THIRST] -= number_range(40, 50);

    if ( arg[0] == '\0' )
    {
      stc("What object do you wish to mask yourself as?\n\r",ch);
      return;
    }

    if ( ( obj = get_obj_here( ch, arg ) ) == NULL )
    {
	send_to_char( "That object is not here.\n\r", ch );
	return;
    }

    SET_BIT(ch->flag2, VAMP_OBJMASK );
    SET_BIT(ch->affected_by, AFF_POLYMORPH);
    act("You mask yourself as $p.",ch,obj,NULL,TO_CHAR);
    act("$n masks $mself as $p.",ch,obj,NULL,TO_ROOM);
    free_string(ch->morph);
    ch->morph = str_dup(obj->short_descr);
    free_string(ch->objdesc);
    ch->objdesc = str_dup(obj->description);
    ch->long_descr = str_dup("");
    return;
}

void do_mirror( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *victim;
    char buf[MAX_INPUT_LENGTH];

    if (IS_NPC(ch)) return;

    if ( !IS_CLASS(ch, CLASS_VAMPIRE) )
    {
      stc("Huh?\n\r",ch);
      return;
    }

    if ( ch->power[DISC_VAMP_CHIM] < 1 )
    {
      stc("You must obtain level 1 Chimersty to use Mirror Image.\n\r",ch);
      return;
    }

    if ( ch->pcdata->condition[COND_THIRST] < 20 )
    {
      stc("You do not have enough blood to create a Mirror Image of yourself.\n\r",ch);
      return;
    }

    if (ch->pcdata->followers > 4)
    {
	send_to_char("Nothing happens.\n\r",ch);
	return;
    }

    ch->pcdata->followers++;


      victim = create_mobile( get_mob_index( 30000 ) );
      victim->short_descr = str_dup(ch->name);
      sprintf(buf, "%s is hovering here.\n\r",ch->name);
      victim->long_descr = str_dup(buf);
      victim->name = str_dup(ch->name);
      victim->level = 20;
      victim->max_hit = 2000;
      victim->hit = 2000;
      victim->mana = 2000;
      victim->max_mana = 2000;
      victim->max_move = 2000;
      victim->move = 2000;
      SET_BIT(victim->flag2, VAMP_CLONE);
	SET_BIT(victim->act, ACT_NOEXP);
      ch->pcdata->condition[COND_THIRST] -= 20;
	char_to_room( victim, ch->in_room );
	act( "You concentrate your powers and form a mirror image of yourself.", ch, NULL, victim, TO_CHAR );
	act( "$n waves $s hands and a mirror image of $mself appears.", ch, NULL, victim, TO_ROOM );
	return;

}

void do_control( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *victim;
    CHAR_DATA *familiar;
    char arg[MAX_INPUT_LENGTH];

    one_argument( argument, arg );

    if ( !IS_CLASS(ch, CLASS_VAMPIRE) )
    {
      stc("Huh?\n\r",ch);
      return;
    }

    if ( ch->power[DISC_VAMP_CHIM] < 4 )
    {
      stc("You must obtain level 4 Chimerstry to use Control the Clone.\n\r",ch);
      return;
    }

    if ( arg[0] == '\0' )
    {
	send_to_char( "Which clone do you wish to control?\n\r", ch );
	return;
    }

    if ( ( victim = get_char_room( ch, arg ) ) == NULL )
    {
	send_to_char( "They aren't here.\n\r", ch );
	return;
    }

    if ( ch == victim )
    {
	send_to_char( "Become your own familiar?\n\r", ch );
	return;
    }

    if ( !IS_NPC(victim) )
    {
      stc("Not on players.\n\r",ch);
      return;
    }

    if ( !IS_SET(victim->flag2, VAMP_CLONE) )
    {
      stc("That is not a clone.\n\r",ch);
      return;
    }

    if ( ( familiar = ch->pcdata->familiar ) != NULL ) 
	familiar->wizard = NULL;
    ch->pcdata->familiar = victim;
    victim->wizard = ch;
    act("You take control of $N's mind.",ch,NULL,victim,TO_CHAR);
    act("$n takes control of $N's mind.",ch,NULL,victim,TO_ROOM);

    return;
}


void do_formillusion( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *victim;
    char buf[MAX_INPUT_LENGTH];

    if (IS_NPC(ch)) return;

    if ( !IS_CLASS(ch, CLASS_VAMPIRE) )
    {
      stc("Huh?\n\r",ch);
      return;
    }

    if ( ch->power[DISC_VAMP_CHIM] < 2 )
    {
      stc("You must obtain level 2 Chimersty to Form an Illusion of yourself.\n\r",ch);
      return;
    }

    if ( ch->pcdata->condition[COND_THIRST] < 30 )
    {
      stc("You do not have enough blood to Form an Illusion of yourself.\n\r",ch);
      return;
    }

    if (ch->pcdata->followers > 4)
    {
	send_to_char("Nothing happens.\n\r",ch);
	return;
    }

    ch->pcdata->followers++;


	victim = create_mobile( get_mob_index( 30000 ) );
      victim->short_descr = str_dup(ch->name);
      sprintf(buf, "%s is hovering here.\n\r",ch->name);
      victim->long_descr = str_dup(buf);
      victim->name = str_dup(ch->name);
      victim->level = 200;
      victim->max_hit  = ch->max_hit;
      victim->hit      = victim->max_hit;
      victim->max_mana = ch->max_mana;
      victim->mana     = victim->max_mana;
      victim->max_move = ch->max_move;
      victim->move     = victim->max_move;
      victim->hitroll  = ch->hitroll;
      victim->damroll  = ch->damroll;
      SET_BIT(victim->flag2, VAMP_CLONE);
	SET_BIT(victim->act, ACT_NOEXP);
	char_to_room( victim, ch->in_room );
      ch->pcdata->condition[COND_THIRST] -= 30;
	act( "You concentrate your powers and form an illusion of yourself.", ch, NULL, victim, TO_CHAR );
	act( "$n waves $s hands and splits in two.", ch, NULL, victim, TO_ROOM );
	return;

}

void do_ashes( CHAR_DATA *ch, char *argument )
{
    OBJ_DATA *obj;
    
     if ( IS_NPC(ch) )
	return;
 
    if (!IS_CLASS(ch, CLASS_VAMPIRE)) 
    {
	send_to_char("Huh?\n\r", ch);
	return;
    }

    if (ch->power[DISC_VAMP_THAN] < 3)
    {
       send_to_char("You must obtain level 3 in Thanatosis to use Body of Ashes.\n\r", ch);
       return;
    }
 
    save_char_obj(ch);

    if ((obj = create_object(get_obj_index( 70000 ),60)) == NULL)
    {
        send_to_char( "Error - Please inform Dunkirk.\n\r", ch);
	  return;
    }

    if ( IS_SET(ch->flag2, VAMP_ASHES) )
    {
      ch->pcdata->obj_vnum = 0;
      REMOVE_BIT(ch->affected_by, AFF_POLYMORPH);
      REMOVE_BIT(ch->extra, EXTRA_OSWITCH);
      REMOVE_BIT(ch->flag2, VAMP_ASHES);
      ch->pcdata->chobj = NULL;
      obj->chobj = NULL;
      free_string(ch->morph);
      ch->morph = str_dup("");
      act("$p transforms into $n.",ch,obj,NULL,TO_ROOM);
      act("Your reform your human body.",ch,obj,NULL,TO_CHAR);
      extract_obj( obj );
      return;
                
    }
    
    if ( IS_AFFECTED(ch, AFF_POLYMORPH) )
    {
	send_to_char( "Not while polymorphed.\n\r", ch );
	return;
    }

    act("$n transforms into $p and falls to the ground.",ch,obj,NULL,TO_ROOM);
    act("You transform into $p and falls to the ground.",ch,obj,NULL,TO_CHAR);
    ch->pcdata->obj_vnum = ch->pcdata->powers[VPOWER_OBJ_VNUM];
    obj->chobj = ch;
    ch->pcdata->chobj = obj;
    SET_BIT(ch->affected_by, AFF_POLYMORPH);
    SET_BIT(ch->extra, EXTRA_OSWITCH);
    SET_BIT(ch->flag2, VAMP_ASHES);
    free_string(ch->morph);
    ch->morph = str_dup("a pile of ashes");
    obj_to_room(obj,ch->in_room);
    return;
}

void do_unveil( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;

    one_argument( argument, arg );

    if ( !IS_CLASS(ch, CLASS_VAMPIRE) )
    {
      stc("Huh?\n\r",ch);
      return;
    }

    if ( ch->power[DISC_VAMP_AUSP] < 3 )
    {
      stc("You must obtain level 3 Auspex to use Unveil the Shrouded Mind.\n\r",ch);
      return;
    }

    if ( arg[0] == '\0' )
    {
	send_to_char( "Whose mind do you wish to Unveil?\n\r", ch );
	return;
    }

    if ( ( victim = get_char_room( ch, arg ) ) == NULL )
    {
	send_to_char( "They aren't here.\n\r", ch );
	return;
    }

    if ( victim == ch )
    {
	stc("Why would you want to unveil your own mind?\n\r",ch);
	return;
    }

    if ( victim->level > ch->level )
    {
      stc("Their mind is far too powerful for you to Unveil.\n\r",ch);
      return;
    }

    if ( victim->spl[BLUE_MAGIC] > ch->spl[BLUE_MAGIC] / 2 )
    {
      stc("Their mind is far too powerful for you to Unveil.\n\r",ch);
      return;
    }

    victim->unveil = ch;
    act("You gaze deeply into $N's eyes.\n\rYou have unveiled $S mind.\n\r", ch,NULL,victim,TO_CHAR );
    return;
}

void do_astralwalk( CHAR_DATA *ch, char *argument)
{
    CHAR_DATA *victim;
    char arg[MAX_INPUT_LENGTH];

    if ( IS_NPC(ch) )
	return;

    argument = one_argument (argument, arg);

    if (!IS_CLASS(ch, CLASS_VAMPIRE))
    {
       send_to_char("Huh?\n\r",ch);
       return;
    }

    if ( ch->power[DISC_VAMP_AUSP] < 4 )
    {
        send_to_char("You must obtain level 4 Auspex to use Astral Walk.\n\r",ch);
	return;
    }

    if ( arg[0] == '\0' )
    {
	send_to_char( "Astral Walk to who?\n\r", ch );
	return;
    }

    if ( ( victim = get_char_world( ch, arg ) ) == NULL )
    {
	send_to_char( "They aren't here.\n\r", ch );
	return;
    }
 
    if (!IS_IMMUNE(victim, IMM_SUMMON)) 
    {
      send_to_char("You are unable to locate them.\n\r",ch);
      return;
    }


    if (ch == victim)
    {
        send_to_char("But you're already at yourself!\n\r",ch);
	return;
    }

    if (victim->in_room == ch->in_room)
    {
	send_to_char("But you're already there!\n\r",ch);
      return;
    }

    char_from_room(ch);
    char_to_room(ch,victim->in_room);
    act("You step through a rift in the Astral plane.\n\rYou leap out of the Astral Plane before $N.", ch, NULL, victim, TO_CHAR);
    act("$n leaps out of a rift in the Astral Plane.", ch, NULL, victim, TO_NOTVICT);
    act("$n leaps out of a rift in the Astral Plane.", ch, NULL, victim, TO_VICT);
    do_look(ch,"auto");
    return;
}

void do_hagswrinkles( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *victim;
    char      arg [MAX_INPUT_LENGTH];
    char      buf [MAX_INPUT_LENGTH];

    argument = one_argument( argument, arg );

    if (IS_NPC(ch)) return;

    if (!IS_CLASS(ch, CLASS_VAMPIRE))
    {
	send_to_char( "Huh?\n\r", ch );
	return;
    }
    if (ch->power[DISC_VAMP_THAN] < 1  )
    {
      send_to_char("You must obtain at least level 1 in Thanatosis to use Hagswrinkles.\n\r",ch);
	return;
    }

    if ( arg[0] == '\0' )
    {
	send_to_char( "Change to look like whom?\n\r", ch );
	return;
    }

    if (IS_AFFECTED(ch,AFF_POLYMORPH) && !IS_VAMPAFF(ch,VAM_DISGUISED))
    {
	send_to_char( "Not while polymorphed.\n\r", ch );
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

    if ( IS_IMMORTAL(victim) && victim != ch )
    {
	send_to_char( "You can only use hagswrinkles on Avatars or lower.\n\r", ch );
	return;
    }

    if ( ch->pcdata->condition[COND_THIRST] < 40 )
    {
	send_to_char("You have insufficient blood.\n\r",ch);
	return;
    }
    ch->pcdata->condition[COND_THIRST] -= number_range(30,40);

    if ( ch == victim )
    {
	if (!IS_AFFECTED(ch,AFF_POLYMORPH) && !IS_VAMPAFF(ch,VAM_DISGUISED))
	{
	    send_to_char( "You already look like yourself!\n\r", ch );
	    return;
	}
	sprintf(buf,"Your body wrinkles and reshapes as %s.",ch->name);
	act(buf,ch,NULL,victim,TO_CHAR);
	sprintf(buf,"%s's body wrinkles and reshapes as %s.",ch->morph,ch->name);
	act(buf,ch,NULL,victim,TO_ROOM);
	REMOVE_BIT(ch->affected_by, AFF_POLYMORPH);
	REMOVE_BIT(ch->pcdata->stats[UNI_AFF], VAM_DISGUISED);
    	free_string( ch->morph );
    	ch->morph = str_dup( "" );
	return;
    }
    if (IS_VAMPAFF(ch,VAM_DISGUISED))
    {
    	sprintf(buf,"Your body wrinkles and reshapes as %s.",victim->name);
    	act(buf,ch,NULL,victim,TO_CHAR);
    	sprintf(buf,"%s's body wrinkles and reshapes as %s.",ch->morph,victim->name);
    	act(buf,ch,NULL,victim,TO_NOTVICT);
    	sprintf(buf,"%s's body wrinkles and reshapes as you!",ch->morph);
    	act(buf,ch,NULL,victim,TO_VICT);
    	free_string( ch->morph );
    	ch->morph = str_dup( victim->name );
	return;
    }
    sprintf(buf,"Your body wrinkles and reshapes as %s.",victim->name);
    act(buf,ch,NULL,victim,TO_CHAR);
    sprintf(buf,"%s's body wrinkles and reforms as %s.",ch->name,victim->name);
    act(buf,ch,NULL,victim,TO_NOTVICT);
    sprintf(buf,"%s's body wrinkles and reforms as you!",ch->name);
    act(buf,ch,NULL,victim,TO_VICT);
    SET_BIT(ch->affected_by, AFF_POLYMORPH);
    SET_BIT(ch->pcdata->stats[UNI_AFF], VAM_DISGUISED);
    free_string( ch->morph );
    ch->morph = str_dup( victim->name );
    return;
}

/*gate*/

void do_gate(CHAR_DATA *ch, char *argument)
{
    CHAR_DATA *victim;
    OBJ_DATA  *obj;
    char       arg [MAX_INPUT_LENGTH];
 
    one_argument( argument, arg );
 
    if (IS_NPC(ch)) return;
    if (!IS_CLASS(ch, CLASS_VAMPIRE))
    {
    send_to_char("Huh?\n\r",ch);
    return;
    }
    if (ch->power[DISC_VAMP_DAIM] < 3)
    {
    send_to_char("You require Daimoinon level 3 to gate.\n\r",ch);
    return;
    }
    if ( arg[0] == '\0')
    {
        send_to_char( "Who do you wish to gate to?\n\r", ch );
	return;
    }
 
    if ( (victim = get_char_world( ch, arg ) ) == NULL )
    {
    send_to_char("They arent here.\n\r",ch);
    return;
    }
 
 
    if ( (victim == ch) 
    ||   victim->in_room == NULL
    ||   IS_NPC(victim)
    ||   (!IS_NPC(victim) && !IS_IMMUNE(victim, IMM_SUMMON) )
    ||   IS_SET(ch->in_room->room_flags, ROOM_PRIVATE)
    ||   IS_SET(ch->in_room->room_flags, ROOM_SOLITARY)
    ||   IS_SET(ch->in_room->room_flags, ROOM_NO_RECALL)
    ||   IS_SET(victim->in_room->room_flags, ROOM_PRIVATE)
    ||   IS_SET(victim->in_room->room_flags, ROOM_SOLITARY)
    ||   IS_SET(victim->in_room->room_flags, ROOM_NO_RECALL)
    ||   victim->in_room->vnum == ch->in_room->vnum)
    {
	send_to_char( "You failed.\n\r", ch );
	return;
    }
 
 
    obj = create_object( get_obj_index( OBJ_VNUM_GATE ), 0 );
    obj->value[0] = victim->in_room->vnum;
    obj->value[3] = ch->in_room->vnum;
    obj->timer = 5;
    if (IS_AFFECTED(ch,AFF_SHADOWPLANE))
	obj->extra_flags = ITEM_SHADOWPLANE;
    obj_to_room( obj, ch->in_room );
 
    obj = create_object( get_obj_index( OBJ_VNUM_GATE ), 0 );
    obj->value[0] = ch->in_room->vnum;
    obj->value[3] = victim->in_room->vnum;
    obj->timer = 5;
    if (IS_AFFECTED(victim,AFF_SHADOWPLANE))
	obj->extra_flags = ITEM_SHADOWPLANE;
    obj_to_room( obj, victim->in_room );
    act( "A look of concentration passes over $n's face.", ch, NULL,NULL,
TO_ROOM );
    send_to_char("A look of concentration passes over your face.\n\r",ch);
    act( "$p appears in front of $n in a blast of flames.", ch, obj, NULL,
TO_ROOM );
    act( "$p appears in front of you in a blast of flames.", ch, obj,
NULL, TO_CHAR );
    act( "$p appears in front of $n in a blast of flames.", victim, obj,
NULL, TO_ROOM );
    act( "$p appears in front of you in a blast of flames.", ch, obj,
victim, TO_VICT );
    return;
}

/*pigeon*/
void do_pigeon( CHAR_DATA *ch, char *argument )
{
    char arg1[MAX_INPUT_LENGTH];
    char arg2[MAX_INPUT_LENGTH];
    OBJ_DATA *obj;
    CHAR_DATA *victim;
 
    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );
 
    if (IS_NPC(ch)) return;
    if (!IS_CLASS(ch, CLASS_VAMPIRE))
    {
    send_to_char("Huh?\n\r",ch);
    return;
    }
    if (ch->power[DISC_VAMP_ANIM] < 3)
    {
    send_to_char("You require Aimalism level 3 to pigeon.\n\r",ch);
    return;
    }
    if ( arg1[0] == '\0' )
    {
        send_to_char( "Pigeon which object?\n\r", ch );
        return;
    }
 
    if ( arg2[0] == '\0' )
    {
        send_to_char( "Pigeon what to whom?\n\r", ch );
        return;
    }
    if ( ( obj = get_obj_carry( ch, arg1 ) ) == NULL )
    {
	send_to_char( "You are not carrying that item.\n\r", ch );
	return;
    }
    victim = get_char_world(ch,arg2);
    if ( (victim = get_char_world( ch, arg2 ) ) == NULL )
    {
	send_to_char( "They arent here.\n\r", ch );
	return;
    }
 
    if (!IS_NPC(victim) && !IS_IMMUNE(victim, IMM_TRANSPORT))
    {
        send_to_char( "They dont want anything transported to them.\n\r",
ch );
	return;
    }
 
    act("You place $p in a pigeon's beak and it flies away.",
ch,obj,NULL,TO_CHAR);
    act("$n places $p in a pigeon's beak and it flies away."
,ch,obj,NULL,TO_ROOM);
    obj_from_char(obj);
    obj_to_char(obj,victim);
 act("A pigeon lands on your shoulders and flies away after handing you $p."
   ,victim,obj,NULL,TO_CHAR);
 act("A pigeon lands on $n's shoulders and flies away after handing him $p."
   ,victim,obj,NULL,TO_ROOM);
    do_autosave(ch,"");
    do_autosave(victim,"");
    return;
}

/*bloodagony*/
void do_bloodagony(CHAR_DATA *ch, char *argument)
{
    char arg[MAX_STRING_LENGTH];
    int value;
    int pcost;
    OBJ_DATA *obj; 
    one_argument( argument,arg);


    value = ch->power[DISC_VAMP_QUIE];
    pcost = ch->power[DISC_VAMP_QUIE] * 60;
 
    if ( arg[0] == '\0')
    {
    send_to_char("Bloodagony what?\n\r",ch);
    return;
    }
 
    if (IS_NPC(ch)) return;
    if ((obj= get_obj_carry(ch,arg)) == NULL)
    {
    send_to_char("You dont have that weapon.\n\r",ch);
    return;
    }
 
    if (!IS_CLASS(ch, CLASS_VAMPIRE))
    {
         send_to_char("Huh?\n\r",ch);
         return;
    }
    if (ch->power[DISC_VAMP_QUIE] < 3 )
    {
         send_to_char("You need level 3 Quietus to use bloodagony.\n\r",ch);
         return;
    }
 
    if ( obj->item_type != ITEM_WEAPON
    ||   IS_SET(obj->quest, QUEST_ARTIFACT)
    ||   obj->chobj != NULL 
    ||  IS_SET(obj->quest, QUEST_BLOODA))
    {
        send_to_char("You are unable to bloodagony this weapon.\n\r",ch);
	return;
    }
    ch->pcdata->quest += pcost;
    oset_affect(ch,obj,value,APPLY_DAMROLL,TRUE);
    oset_affect(ch,obj,value,APPLY_HITROLL,TRUE);
    obj->points -= pcost;
    SET_BIT(obj->quest, QUEST_BLOODA);
 
        act( "You cut your wrist and smear your blood on $p.", ch, obj,
NULL, TO_CHAR );
        act( "$n cuts his wrist and smears blood on $p.", ch, obj, NULL,
TO_ROOM );
    return;
}

void do_diablerise(CHAR_DATA *ch, char *argument  )
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

    if ( arg[0] == '\0' && ch->embracing==NULL)
    {
        send_to_char( "Who do you wish to embrace?\n\r", ch );
	  return;
    }

    if (ch->embracing != NULL)
    {
      if ( ( victim = ch->embracing) != NULL)
      {
        send_to_char("You retract your fangs.",ch);
        act("$N retracts his fangs.",ch,NULL,NULL,TO_ROOM);
        stop_embrace(ch,victim);
        return;
      }
    }

    if ((victim = get_char_room( ch, arg)) == NULL && ch->embracing==NULL)
    {
      send_to_char("They arent here.\n\r",ch);
      return;
    }


    if (IS_NPC(victim) || !IS_CLASS(victim,CLASS_VAMPIRE))
    {
      send_to_char("You can only embrace vampires.\n\r",ch);
      return;
    }
  
    if (victim->position != POS_MORTAL)
    {
      send_to_char("You can only embrace mortally wounded vampires.\n\r",ch);
      return;
    }

    sprintf(buf,"%s leaps toward %s baring his fangs.",ch->name,victim->name);
    act(buf,ch,NULL,NULL,TO_ROOM);
    sprintf(buf,"You leap toward %s baring your fangs.\n\r",victim->name);
    send_to_char(buf,ch);
    WAIT_STATE(ch, 15);
    
    sprintf(buf,"You sink your teeth into their throat.\n\r");
    send_to_char(buf,ch);
    sprintf(buf,"%s sinks their teeth into %s's throat.",ch->name,victim->name);
    act(buf,ch,NULL,NULL,TO_ROOM);
    sprintf(buf,"%s sinks their teeth into your throat.\n\r",ch->name);
    send_to_char(buf,victim);
    victim->embraced = ch;
    ch->embracing = victim;
 
    return;
 }

/*embrace*/
void do_embrace(CHAR_DATA *ch, char *argument  )
{
    char arg[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;
    char buf[MAX_STRING_LENGTH];
    int bloodpool;
    one_argument( argument, arg );

    if (ch->generation <= 0)
        ch->generation = 4;

    
    if (IS_SET(ch->newbits, NEW_TIDE))
    bloodpool = (3000 / ch->generation);
    else bloodpool = (2000 / ch->generation);
    
 
    if (IS_NPC(ch)) return;

    if (!IS_CLASS(ch,CLASS_VAMPIRE))
    {
      send_to_char("Huh?\n\r",ch);
      return;
    }


    if ( arg[0] == '\0' )
    {
        send_to_char( "Who do you wish to embrace?\n\r", ch );
	return;
    }

    if ((victim = get_char_room( ch, arg)) == NULL)
    {
      send_to_char("They arent here.\n\r",ch);
      return;
    }

    if (IS_NPC(victim) && IS_SET(victim->act, ACT_NOAUTOKILL)) 
    {
	send_to_char("You can't do that to them.\n\r", ch);
	return;
    }

	/* Shaktis check for level */

	if (victim->level > 75)
	{
		send_to_char("They are too powerful to embrace!\n\r",ch);
		return;
	}

	
    if (!IS_NPC(victim))
    {
      send_to_char("You cannot embrace a person.\n\r",ch);
      return;
    }

    if (IS_SET(victim->in_room->room_flags, ROOM_SAFE))
    {
	send_to_char("You cannot embrace them here.\n\r",ch);
	return;
    }

    sprintf(buf,"%s leaps toward %s baring his fangs.\n\r",ch->name,victim->short_descr);
    act(buf,ch,NULL,NULL,TO_ROOM);
    sprintf(buf,"You leap toward %s baring your fangs.\n\r",victim->short_descr);
    send_to_char(buf,ch);
    WAIT_STATE(ch, 15);
    
    if (victim->position != POS_STUNNED && victim->position != POS_SLEEPING && victim->position != POS_DEAD)
    {
      send_to_char("They lunge away from you.\n\r",ch);
      sprintf(buf,"%s lunges away from %s.",victim->name,ch->short_descr);
      act(buf,ch,NULL,NULL,TO_ROOM);
      return;
    }

    send_to_char("You bury your fangs into their neck, and begins an orgy of blood-sucking!\n\r",ch);
    sprintf(buf,"%s buries his fangs into %s's neck, and begins an orgy of blood-sucking!\n\r",ch->name,victim->short_descr);
    act(buf,ch,NULL,NULL,TO_ROOM);
	
	/* New update send routine, allows for anything to get embraced.         */
	/* and sets a mobs blood to its level *8.Shakti	     					 */

	victim->embraced = ch;
    ch->embracing = victim;
	if (IS_NPC(victim))
	{
		(victim->practice = victim->level  );
		(victim->practice *= 8);
	}	

	return ;


}


/*decay*/
void do_withering (CHAR_DATA *ch,char *argument)
{
    CHAR_DATA *victim;
    OBJ_DATA *obj;
    char arg [MAX_INPUT_LENGTH];
    char buf [MAX_STRING_LENGTH];

    argument = one_argument (argument, arg);
        
    
    if (IS_NPC(ch)) return;
    if (!IS_CLASS(ch, CLASS_VAMPIRE))
    {
    send_to_char("Huh?\n\r",ch);
    return;
    }

    if (ch->power[DISC_VAMP_THAN] < 4 )
    {
    send_to_char("You must obtain level 4 Thanatosis to use Withering.\n\r",ch);
    return;
    }
    if ( arg[0] == '\0' )
    {
    send_to_char("Who do you wish to wither?\n\r",ch);
    return;
    }
    victim = get_char_room(ch, arg);
    if ( ( victim = get_char_room( ch, arg) ) == NULL )
    {
     send_to_char("They arent here.\n\r",ch);
     return;
    }

   if (IS_SET(victim->in_room->room_flags, ROOM_SAFE))
   {
     send_to_char("You cannot attack them here.\n\r",ch);
     return;
   }

   if (is_safe(ch,victim) == TRUE) 
     return;
 
   WAIT_STATE(ch,35);
    sprintf(buf,"A look of concentration crosses over %s's face.",ch->name);
    act(buf,ch,NULL,NULL,TO_ROOM);
    send_to_char("A look of concentration crosses over your face.\n\r",ch);
    if (IS_ARM_R(victim,LOST_ARM) && IS_ARM_L(victim,LOST_ARM) &&
        IS_LEG_R(victim,LOST_LEG) && IS_LEG_L(victim,LOST_LEG))
        {
        send_to_char("All of their limbs are severed already!\n\r",ch);
        return;
        }
       if (number_percent ( ) < 15 )
       {
        if (!IS_ARM_R(victim,LOST_ARM))
        {
        SET_BIT(victim->loc_hp[3],LOST_ARM);
        if ((obj = get_eq_char( victim, WEAR_HOLD )) != NULL )
           take_item(victim,obj);
        if ((obj = get_eq_char( victim, WEAR_HANDS )) != NULL)
           take_item(victim,obj);
        if ((obj = get_eq_char( victim, WEAR_WRIST_R )) != NULL)
           take_item(victim,obj);
        if ((obj = get_eq_char( victim, WEAR_FINGER_R )) != NULL)
           take_item(victim,obj);
        make_part(victim,"arm");
        sprintf(buf,
        "A supernatural force rips %s's arm off!",victim->name);
        act(buf,ch,NULL,NULL,TO_ROOM);
        send_to_char(buf,ch);
        return;
        }
        if (!IS_ARM_L(victim,LOST_ARM))
        {
        SET_BIT(victim->loc_hp[2],LOST_ARM);
        if ((obj = get_eq_char( victim, WEAR_HOLD )) != NULL )
           take_item(victim,obj);
        if ((obj = get_eq_char( victim, WEAR_HANDS )) != NULL)
           take_item(victim,obj);
        if ((obj = get_eq_char( victim, WEAR_WRIST_L )) != NULL)
           take_item(victim,obj);
        if ((obj = get_eq_char( victim, WEAR_FINGER_L )) != NULL)
           take_item(victim,obj);
        make_part(victim,"arm");
        sprintf(buf,
        "A supernatural force rips %s's arm off!",victim->name);
        act(buf,ch,NULL,NULL,TO_ROOM);
        send_to_char(buf,ch);
        return;
        }
        if (!IS_LEG_R(victim,LOST_LEG))
        {
        SET_BIT(victim->loc_hp[5],LOST_LEG);
        if ((obj = get_eq_char( victim, WEAR_FEET )) != NULL )
           take_item(victim,obj);
        if ((obj = get_eq_char( victim, WEAR_LEGS )) != NULL)
           take_item(victim,obj);
        make_part(victim,"leg");
        sprintf(buf,
        "A supernatural force rips %s's leg off!",victim->name);
        act(buf,ch,NULL,NULL,TO_ROOM);
        send_to_char(buf,ch);
        return;
        }
        if (!IS_LEG_L(victim,LOST_LEG))
        {
        SET_BIT(victim->loc_hp[4],LOST_LEG);
        if ((obj = get_eq_char( victim, WEAR_FEET )) != NULL )
           take_item(victim,obj);
        if ((obj = get_eq_char( victim, WEAR_LEGS )) != NULL)
           take_item(victim,obj);
        make_part(victim,"leg");
        sprintf(buf,
        "A supernatural force rips %s's leg off!",victim->name);
        act(buf,ch,NULL,NULL,TO_ROOM);
        send_to_char(buf,ch);
        return;
        }
        }
send_to_char("You failed.\n\r",ch);
 
return;
}
 
 
 

/* Infirmity Thinger */
void do_infirmity( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *victim;
    char  arg[MAX_INPUT_LENGTH];
    char  buf[MAX_INPUT_LENGTH];
    int   sn;
    int   level;
    int   spelltype;
    
    argument = one_argument (argument, arg);

    if ( IS_NPC(ch) )
	return;

	
  	if (!IS_CLASS(ch, CLASS_VAMPIRE))
        {
  	send_to_char("Huh?\n\r", ch);
        return;
        }
     	if ( IS_CLASS(ch, CLASS_VAMPIRE) && ch->power[DISC_VAMP_QUIE] < 2 )
        {
        send_to_char("You need level 2 Quietus to use this power.\n\r",ch);
        return; 
        }
       if (arg[0] == '\0')
	{
	   send_to_char("Who do you wish to use Infirmity on?\n\r",ch);
	   return;
	}
  
  	
      if ( ( victim = get_char_room( ch, arg ) ) == NULL )
    {
	send_to_char( "They aren't here.\n\r", ch );
	return;
    }
if (is_safe(ch,victim) == TRUE) return;  	
    if ( ( sn = skill_lookup( "infirmity" ) ) < 0 ) 
{
sprintf(buf,"Yep, sn is bieng set to %d.",sn);
send_to_char(buf,ch);
return;

}  

    spelltype = skill_table[sn].target;
    level = ch->power[DISC_VAMP_QUIE];
    (*skill_table[sn].spell_fun) ( sn, level, ch, victim );
    WAIT_STATE( ch, 12 );
    return; 
}


/* Guardian DEmon for Vamps */
void do_guardian(CHAR_DATA *ch, char *argument )
{
 
    char buf [MAX_INPUT_LENGTH];
    CHAR_DATA *victim;
    AFFECT_DATA af;
    if (IS_NPC(ch)) return;
    if (!IS_CLASS(ch, CLASS_VAMPIRE))
     {
     send_to_char("Huh?\n\r",ch);
     return;
     }
    if (ch->power[DISC_VAMP_DAIM] < 1)
    {
    send_to_char("You require level 1 Daimoinon to create a guardian.\n\r",ch);
    return;
    }
 
    if (ch->pcdata->followers > 5)
    {
	send_to_char("Nothing happens.\n\r",ch);
	return;
    }
    ch->pcdata->followers++;
 
    victim=create_mobile( get_mob_index( MOB_VNUM_GUARDIAN ) );
    victim->level = 200;
    victim->hit = 5000;
    victim->max_hit = 5000;
    victim->hitroll = 50;
    victim->damroll = 50;
    victim->armor = 300;
    SET_BIT(victim->act, ACT_NOEXP); 

    strcpy(buf,"Come forth, creature of darkness, and do my bidding!");
    do_say( ch, buf );
 
    send_to_char( "A demon bursts from the ground and bows before you.\n\r",ch );
    act( "$N bursts from the ground and bows before $n.", ch, NULL, victim, TO_ROOM );
 
    char_to_room( victim, ch->in_room );
 
 
    add_follower( victim, ch );
    af.duration  = 666;
    af.location  = APPLY_NONE;
    af.modifier  = 0;
    af.bitvector = AFF_CHARM;
    affect_to_char( victim, &af );
    return;

}

/*demonic servants*/
void do_servant(CHAR_DATA *ch, char *argument )
{
 
    char buf [MAX_INPUT_LENGTH];
    CHAR_DATA *victim;
    AFFECT_DATA af;
    if (IS_NPC(ch)) return;
    if (!IS_CLASS(ch, CLASS_VAMPIRE))
     {
     send_to_char("Huh?\n\r",ch);
     return;
     }
    if (ch->power[DISC_VAMP_DAIM] < 8)
    {
    send_to_char("You require level 8 Daimoinon to create a servant.\n\r",ch);
    return;
    }
 
    if (ch->pcdata->followers > 5)
    {
	send_to_char("Nothing happens.\n\r",ch);
	return;
    }
    ch->pcdata->followers++;
 
    victim=create_mobile( get_mob_index( MOB_VNUM_SERVANT ) );
    victim->level = 300;
    victim->hit = 30000;
    victim->max_hit = 30000;
    victim->hitroll = 100;
    victim->damroll = 100;
    victim->armor = 700;
 
    strcpy(buf,"Come forth, creature of darkness, and do my bidding!");
    do_say( ch, buf );
 
    send_to_char( "A demonic servant bursts from the ground and bows before you.\n\r",ch );
    act( "$N bursts from the ground and bows before $n.", ch, NULL, victim, TO_ROOM );
 
    char_to_room( victim, ch->in_room );
 
 
    add_follower( victim, ch );
    af.duration  = 666;
    af.location  = APPLY_NONE;
    af.modifier  = 0;
    af.bitvector = AFF_CHARM;
    affect_to_char( victim, &af );
    SET_BIT(victim->act, ACT_NOEXP); 
    return;
}
/* Beckon */
void do_beckon(CHAR_DATA *ch, char *argument  )
{
    char arg[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;
    one_argument( argument,arg);
 
    if (IS_NPC(ch)) return;
    if (!IS_CLASS(ch,CLASS_VAMPIRE))
    {
    send_to_char("Huh?\n\r",ch);
    return;
    }
    if (ch->power[DISC_VAMP_ANIM] < 1)
    {
    send_to_char("You require level 1 animalism to beckon the wild.\n\r",ch);
    return;
    }
    if ( arg[0] == '\0' )
    {
        send_to_char( "You can beckon a frog, a raven, a cat or a dog.\n\r", ch );
	return;
    }
 
    if (ch->pcdata->followers > 5 )
    {
    send_to_char("Nothing happened.\n\r",ch);
    return;
    }
    
    if ( !str_cmp( argument, "frog" ) )
    {
	victim = create_mobile( get_mob_index( MOB_VNUM_FROG ) );
	if (victim == NULL)
        {send_to_char("Error - please inform Infidel.\n\r",ch); return;}
    }
    else if ( !str_cmp( argument, "raven" ) )
    {
	victim = create_mobile( get_mob_index( MOB_VNUM_RAVEN ) );
	if (victim == NULL)
        {send_to_char("Error - please inform Infidel.\n\r",ch); return;}
    }
    else if ( !str_cmp( argument, "cat" ) )
    {
	victim = create_mobile( get_mob_index( MOB_VNUM_CAT ) );
	if (victim == NULL)
        {send_to_char("Error - please inform Infidel.\n\r",ch); return;}
    }
    else if ( !str_cmp( argument, "dog" ) )
    {
	victim = create_mobile( get_mob_index( MOB_VNUM_DOG ) );
	if (victim == NULL)
        {send_to_char("Error - please inform Infidel.\n\r",ch); return;}
    }
    else 
    {
        send_to_char( "Your can beckon a frog, a raven, a cat or a dog.\n\r", ch );
	return;
    }
 
    act( "You whistle loudly and $N walks in.", ch, NULL, victim, TO_CHAR);
    act( "$n whistles loudly and $N walks in.", ch, NULL, victim, TO_ROOM);
    char_to_room( victim, ch->in_room );
    ch->pcdata->followers += 1;
    SET_BIT(victim->act, ACT_NOEXP);
    return;
}
void do_spit( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *victim;

    char buf[MAX_STRING_LENGTH];
  
  int dam;
    if (IS_NPC(ch)) return;
    if (!IS_CLASS(ch,CLASS_VAMPIRE)) 
    {
    send_to_char("Huh?\n\r",ch);
    return;
    }
    if (ch->power[DISC_VAMP_QUIE] < 1)
    {
    send_to_char("You need level 1 Quietus to spit.\n\r",ch);
    return;
    }
 
 
    if ( ( victim = ch->fighting ) == NULL )
    {
send_to_char( "You aren't fighting anyone.\n\r", ch );
	return;

    }
    if (ch->pcdata->condition[COND_THIRST] < 5)
    {
    send_to_char("You dont have enough blood.\n\r",ch); 
    return; 
    }
    WAIT_STATE( ch, 12 );
    if (!IS_NPC(victim))
    {
    dam = ch->power[DISC_VAMP_QUIE] * 50;
    } 
    else if (IS_NPC(victim))
    {
    dam = ch->power[DISC_VAMP_QUIE] * 250;
    }
    ch->pcdata->condition[COND_THIRST] -= 5;
    if ( !IS_NPC(victim) && IS_CLASS(victim, CLASS_WEREWOLF) )
    {
	if (victim->power[DISC_WERE_BOAR] > 2 ) dam *= 0.5;
    }
 
    dam += number_range(1,30);
    if ( dam <= 0 )
       dam = 1;
sprintf(buf,"Your spit of acid hits $N incredibly hard! [%d]",dam);
act(buf,ch,NULL,victim,TO_CHAR);
sprintf(buf,"$n's spit of acid hits you incredibly hard! [%d]",dam);
act(buf,ch,NULL,victim,TO_VICT);
sprintf(buf,"$n's spit of acid hits $N incredibly hard! [%d]",dam);
act(buf,ch,NULL,victim,TO_NOTVICT);
 

    send_to_char("\n\r",ch);
    victim->hit -= dam;
    if (victim->hit <= 10) one_hit(ch,victim,20,gsn_headbutt);
    return;
}
void do_reimburse( CHAR_DATA *ch, char *argument)
{
    char buf[MAX_STRING_LENGTH];
    char strsave[MAX_STRING_LENGTH];
    char *word;
    FILE *fp;
    bool fMatch;
 
sprintf(strsave,"%s%s",BACKUP_DIR,ch->name);
if ( ( fp= fopen(strsave,"r")) == NULL)
{
send_to_char("You dont have a reimbursement file.\n\r",ch);
return;
}


if ( ( fp = fopen( strsave,"r")) != NULL)
{
 
 
    for ( ; ; )
    {
	word   = feof( fp ) ? "End" : fread_word( fp );
	fMatch = FALSE;
 
	switch ( UPPER(word[0]) )
	{
	case '*':
	    fMatch = TRUE;
	    fread_to_eol( fp );
	    break;
 
        case 'H':
	    if ( !str_cmp( word, "HpManaMove" ) )
	    {
		ch->hit		= fread_number( fp );
		ch->max_hit	= fread_number( fp );
		ch->mana	= fread_number( fp );
		ch->max_mana	= fread_number( fp );
		ch->move	= fread_number( fp );
		ch->max_move	= fread_number( fp );
                send_to_char("***Hp/Mana/Move Reimbursed.\n\r",ch);
		fMatch = TRUE;
		break;
	    }
	    break;
 
 
          
        case 'P':
           if ( !str_cmp(word, "Primal" ) )
           {
           ch->practice =  fread_number( fp );
           fMatch = TRUE;
            send_to_char("***Primal reimbursed.\n\r",ch);
           break;
           }
            if (!str_cmp(word,"PkPdMkMd"))
            {
            ch->pkill= fread_number(fp);
            ch->pdeath= fread_number(fp);
            ch->mkill= fread_number(fp);
            ch->mdeath= fread_number(fp);
            send_to_char("***Player/Mob Deaths/Kills restored.\n\r",ch);
            fMatch=TRUE;
             break;
              }           
  break; 
 
	case 'S':
 
 
 
	    if ( !str_cmp( word, "Spells" ) )
	    {
		ch->spl[0]	= fread_number( fp );
		ch->spl[1]	= fread_number( fp );
		ch->spl[2]	= fread_number( fp );
		ch->spl[3]	= fread_number( fp );
		ch->spl[4]	= fread_number( fp );
                send_to_char("***Spell level reimbursed.\n\r",ch);
		fMatch = TRUE;
		break;
	    }
 
 
	    if ( !str_cmp( word, "Stance" ) )
	    {
		ch->stance[0]	= fread_number( fp );
		ch->stance[1]	= fread_number( fp );
		ch->stance[2]	= fread_number( fp );
		ch->stance[3]	= fread_number( fp );
		ch->stance[4]	= fread_number( fp );
		ch->stance[5]	= fread_number( fp );
		ch->stance[6]	= fread_number( fp );
		ch->stance[7]	= fread_number( fp );
		ch->stance[8]	= fread_number( fp );
		ch->stance[9]	= fread_number( fp );
		ch->stance[10]	= fread_number( fp );
		fMatch = TRUE;
                send_to_char("***Stances reimbursed.\n\r",ch);
		break;
	    }
           
 
	    break;

        case 'E':
         
         if (!str_cmp(word,"End"))
         {
          send_to_char("***Reimbursed.. Player saved.\n\r",ch);
          send_to_char("***If you havnt noticed, There was a pwipe.\n\r",ch);
          send_to_char("***We have reimbursed what we could but you will have to earn the rest.\n\r",ch);
          send_to_char("***Classes will be passed out again. -Trace\n\r",ch);
          ch->level = 3;
          ch->trust = 3;
          ch->exp += 250000;
	  do_autosave(ch,"");
          return;
          fMatch= TRUE;
           break;
         }
         break; 

	case 'W':
	    if ( !str_cmp( word, "Weapons" ) )
	    {
		ch->wpn[0]	= fread_number( fp );
		ch->wpn[1]	= fread_number( fp );
		ch->wpn[2]	= fread_number( fp );
		ch->wpn[3]	= fread_number( fp );
		ch->wpn[4]	= fread_number( fp );
		ch->wpn[5]	= fread_number( fp );
		ch->wpn[6]	= fread_number( fp );
		ch->wpn[7]	= fread_number( fp );
		ch->wpn[8]	= fread_number( fp );
		ch->wpn[9]	= fread_number( fp );
		ch->wpn[10]	= fread_number( fp );
		ch->wpn[11]	= fread_number( fp );
		ch->wpn[12]	= fread_number( fp );
                send_to_char("Weapons reimbursed.\n\r",ch);
		fMatch = TRUE;
		break;
	    }
            break;
	}
 
	if ( !fMatch )
	{
	    sprintf(buf, "Fread_char: no match. WORD: %s", word);
	    fread_to_eol( fp );
	}
       
       }
      }
return;
}
void do_scales(CHAR_DATA *ch,char *argument)
{
  if (IS_NPC(ch)) return;
  if (!IS_CLASS(ch,CLASS_VAMPIRE))
  {
  send_to_char("Huh?\n\r",ch);
  return;
  }
  if (ch->power[DISC_VAMP_SERP] < 5)
  {
  send_to_char("You need Serpentis 6 to get scales.\n\r",ch);
  return;
  }
  if (!IS_SET(ch->newbits, NEW_SKIN))
  {
  send_to_char("Scales slide out of your body and surround you.\n\r",ch);
  ch->armor -= 100 ;
  SET_BIT(ch->newbits, NEW_SKIN);
  return;
  }
  else if (IS_SET(ch->newbits, NEW_SKIN))
  { 
  send_to_char("Your scales slide back into your body.\n\r",ch);
  ch->armor += 100;
  REMOVE_BIT(ch->newbits,NEW_SKIN);
  return;
  }
return;
}

void do_cserpent(CHAR_DATA *ch, char *argument  )
{
    char arg[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;
    one_argument( argument,arg);
 
    if (IS_NPC(ch)) return;
    if (!IS_CLASS(ch,CLASS_VAMPIRE))
    {
    send_to_char("Huh?\n\r",ch);
    return;
    }
    if (ch->power[DISC_VAMP_SERP] < 4)
    {
    send_to_char("You require level 5 serpentis to call a serpent.\n\r",ch);
    return;
    }
/*
    if ( arg[0] == '\0' )
    {
        send_to_char( "You can beckon a serpent at anytime.\n\r", ch );
	return;
    }
*/ 

    if (ch->pcdata->followers > 5 )
    {
    send_to_char("Nothing happened.\n\r",ch);
    return;
    }
    
	victim = create_mobile( get_mob_index( MOB_VNUM_SERPENT ) );
	if (victim == NULL)
        {send_to_char("Error - please inform Trace.\n\r",ch); return;}
 
    act( "You close your eyes and concentrate a huge serpent walks in.", ch, NULL, victim, TO_CHAR);
    act( "$n closes his eyes and concentrates a huge $N walks in.", ch, NULL, victim, TO_ROOM);
    char_to_room( victim, ch->in_room );
    ch->pcdata->followers += 1;
    SET_BIT(victim->act, ACT_NOEXP);
    SET_BIT(victim->act, ACT_MOUNT);
    return;
}

void do_illusion(CHAR_DATA *ch, char *argument  )
{
    char arg[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;
    one_argument( argument,arg);
 
    if (IS_NPC(ch)) return;
    if (!IS_CLASS(ch,CLASS_VAMPIRE))
    {
    send_to_char("Huh?\n\r",ch);
    return;
    }
    if (ch->power[DISC_VAMP_CHIM] < 1)
    {
    send_to_char("You require level 1 chim to make an illusion.\n\r",ch);
    return;
    } 

    if (ch->pcdata->followers > 5 )
    {
    send_to_char("Nothing happened.\n\r",ch);
    return;
    }
    
	victim = create_mobile( get_mob_index( MOB_VNUM_ILLUSION ) );
	if (victim == NULL)
        {send_to_char("Error - please inform Trace.\n\r",ch); return;}
 
    act( "You close your eyes and concentrate an illusion appears in the
room.", ch, NULL, victim, TO_CHAR);
    act( "$n closes his eyes and concentrates an $N appears in the room.",
ch, NULL, victim, TO_ROOM);
    char_to_room( victim, ch->in_room );
    ch->pcdata->followers += 1;
    SET_BIT(victim->act, ACT_NOEXP);
    do_form(victim,"justtesting");
    return;
}

