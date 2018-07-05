 
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

void do_sclaws( CHAR_DATA *ch, char *argument )
{

   if (IS_NPC(ch))
	return;

   if (!IS_CLASS(ch, CLASS_WEREWOLF))
   {
        send_to_char("What?\n\r",ch);
        return;
   }

   if (ch->power[DISC_WERE_LUNA] < 5)
   {
	send_to_char("You need level 5 in Luna to use Silver Claws.\n\r",ch);
	return;
   }

   if (!IS_SET(ch->newbits, NEW_SCLAWS))
   {
	send_to_char("Your claws turn into pure silver!\n\r",ch);
        act("$n's claws turn to pure silver!",ch,NULL,NULL,TO_ROOM);
        SET_BIT(ch->newbits, NEW_SCLAWS);
	return;
   }

   if (IS_SET(ch->newbits, NEW_SCLAWS))
   {
	send_to_char("Your claws return to their normal state.\n\r",ch);
	act("$n's claws are no longer composed of pure silver.\n\r",ch,NULL,NULL,TO_ROOM);
	REMOVE_BIT(ch->newbits, NEW_SCLAWS);
	return;
   }

}

void do_moonbeam( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *victim;
    char arg1 [MAX_INPUT_LENGTH];

 
    argument = one_argument( argument, arg1 );
  
 
    if ( IS_NPC(ch) )
    	return;
 
 
    if (!IS_CLASS(ch, CLASS_WEREWOLF))
    {
	send_to_char("Huh?\n\r",ch);
      return;
    }
 
    if (ch->power[DISC_WERE_LUNA] < 8 )
    {
	send_to_char("You need to obtain level 8 in Luna to use Moonbeam.\n\r",ch);
    	return;
    }
 

    if (ch->mana < 500)
    {
	send_to_char("You do not have enough energy to summon a Moonbeam.\n\r",ch);
	return;
    }
 
    if ( arg1[0] == '\0' )
    {
	send_to_char( "Who do you wish to strike?\n\r", ch );
      return;
    }

    if ( ( victim = get_char_room( ch, arg1 ) ) == NULL )
    {
   	 send_to_char( "They aren't here.\n\r", ch );
	 return;
    }

    if (is_safe(ch,victim) == TRUE) return;
  
    	act("$N is struck by a huge moonbeam.", ch, NULL, victim,TO_CHAR);
	act("$N is struck by a deadly beam of moonlight from $n.", ch, NULL, victim,TO_NOTVICT);
	act("You are struck by a deadly beam of moonlight!", ch, NULL, victim,TO_VICT);
    if (IS_GOOD(victim))
	victim->hit -= 300;
    if (IS_EVIL(victim))
	victim->hit -= 400;
    if (IS_NEUTRAL(victim))
	victim->hit -= 350;
	ch->mana -= 500;
    if (ch->fighting == NULL)
 	set_fighting(ch,victim);
	update_pos(victim);
	WAIT_STATE(ch,36);
	return;
    
}


void do_moongate(CHAR_DATA *ch, char *argument)
{
    CHAR_DATA *victim;
    OBJ_DATA  *obj;
    char       arg [MAX_INPUT_LENGTH];
 
    one_argument( argument, arg );
 
    if (IS_NPC(ch)) return;
    if (!IS_CLASS(ch, CLASS_WEREWOLF))
    {
      send_to_char("Huh?\n\r",ch);
      return;
    }

    if (ch->power[DISC_WERE_LUNA] < 6)
    {
      send_to_char("You need to obtain level 6 Luna to use moongate.\n\r",ch);
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
 
 
    obj = create_object( get_obj_index( OBJ_VNUM_GATE2 ), 0 );
    obj->value[0] = victim->in_room->vnum;
    obj->value[3] = ch->in_room->vnum;
    obj->timer = 5;
    if (IS_AFFECTED(ch,AFF_SHADOWPLANE))
	obj->extra_flags = ITEM_SHADOWPLANE;
    obj_to_room( obj, ch->in_room );
 
    obj = create_object( get_obj_index( OBJ_VNUM_GATE2 ), 0 );
    obj->value[0] = ch->in_room->vnum;
    obj->value[3] = victim->in_room->vnum;
    obj->timer = 5;
    if (IS_AFFECTED(victim,AFF_SHADOWPLANE))
	obj->extra_flags = ITEM_SHADOWPLANE;
    obj_to_room( obj, victim->in_room );
    act( "A look of concentration passes over $n's face.", ch, NULL,NULL, TO_ROOM );
    send_to_char("A look of concentration passes over your face.\n\r",ch);
    act( "$p appears in front of $n in a beam of moonlight.", ch, obj, NULL, TO_ROOM );
    act( "$p appears in front of you in a beam of moonlight.", ch, obj, NULL, TO_CHAR );
    act( "$p appears in front of $n in a beam of moonlight.", victim, obj, NULL, TO_ROOM );
    act( "$p appears in front of you in a beam of moonlight.", ch, obj, victim, TO_VICT );
    return;
}

void do_gmotherstouch( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *victim;
    char arg1 [MAX_INPUT_LENGTH];
 
    argument = one_argument( argument, arg1 );
  
 
    if ( IS_NPC(ch) )
    	return;
 
 
    if (!IS_CLASS(ch, CLASS_WEREWOLF))
    {
	send_to_char("Huh?\n\r",ch);
      	return;
    }
 
    if (ch->power[DISC_WERE_LUNA] < 4 )
    {
	send_to_char(
	"You need to obtain level 4 in Luna to use Grandmother's Touch.\n\r",ch);
    	return;
    }
 

    if (ch->mana < 400)
    {
	send_to_char("You do not have enough energy to summon the Grandmother's Touch.\n\r",ch);
	return;
    }
 
    if ( arg1[0] == '\0' )
    {
	send_to_char( "Who is in need of your touch?\n\r", ch );
      return;
    }

    if ( ( victim = get_char_room( ch, arg1 ) ) == NULL )
    {
   	send_to_char( "They aren't here.\n\r", ch );
	return;
    }
  
        
    if (ch->fighting != NULL)
    {
    	act("$N is filled with a feeling of warmth.", ch, NULL, victim,TO_CHAR);
	act("$N is surrounded by a beam of moonlight from $n's touch.", ch, NULL, victim,TO_NOTVICT);
	act("You are surrounded by a beam of moonlight, and feel restored.", ch, NULL, victim,TO_VICT);
	victim->hit += 200;
	ch->mana -= 400;
	WAIT_STATE(ch,16);
	return;
    }

    if (ch->fighting == NULL)
    {
        act("$N is filled with a feeling of warmth.", ch, NULL, victim,TO_CHAR);
	act("$N is surrounded by a beam of moonlight from $n's touch.", ch, NULL, victim,TO_NOTVICT);
	act("You are surrounded by a beam of moonlight, and feel restored.", ch, NULL, victim,TO_VICT);
	victim->hit += 600;
	ch->mana -= 400;
	WAIT_STATE(ch,6);
	return;
    }
}


void do_motherstouch( CHAR_DATA *ch, char *argument )
{
    if (IS_NPC(ch))
	return;

    if (ch->power[DISC_WERE_LUNA] < 3 )
    {
	send_to_char("You need to obtain level 3 in Luna to use Mother's Touch.\n\r",ch);
	return;
    }

    if (ch->mana < 400)
    {
	send_to_char("You don't have enough energy to use Mother's Touch.\n\r",ch);
	return;
    }

    if (ch->fighting != NULL)
    {
	send_to_char("You are touched by a beam of light and feel healed.\n\r",ch);
	act("$n is touched by a beam of moonlight, and looks refreshed.\n\r",ch,NULL,NULL,TO_ROOM);
        ch->hit += 100;
        ch->mana -= 400;
	WAIT_STATE(ch,16);
	return;
    }

    if (ch->fighting == NULL)
    {
	send_to_char("You are surrounded by a beam of light, and feel healed.\n\r",ch);
	act("$n is surrounded by a beam of moonlight, and looks much more refreshed.\n\r",ch,NULL,NULL,TO_ROOM);
	ch->hit += 500;
	ch->mana -= 400;
	WAIT_STATE(ch,8);
	return;
    }

}

void do_flameclaws( CHAR_DATA *ch, char *argument )
{

    if (IS_NPC(ch))
	return;
    
   if (!IS_CLASS(ch, CLASS_WEREWOLF))
   {
        send_to_char("What?\n\r",ch);
        return;
   }
    if (ch->power[DISC_WERE_LUNA] < 1)
    {
	send_to_char("You need to obtain level 1 in luna to use flame claws.\n\r",ch);
	return;
    }

    if (IS_SET(ch->newbits, NEW_MONKFLAME))
    {
	send_to_char("The flames surrounding your claws fade.\n\r",ch);
	act("The flames surrounding $n's claws fade.\n\r",ch,NULL,NULL,TO_ROOM);
	REMOVE_BIT(ch->newbits, NEW_MONKFLAME);
	return;
    }

    if (!IS_SET(ch->newbits, NEW_MONKFLAME))
    {
	send_to_char("Your claws are surrounded by blazing flames.\n\r",ch);
        act("$n's claws are surrounded by blazing flames.\n\r",ch,NULL,NULL,TO_ROOM);
	SET_BIT(ch->newbits, NEW_MONKFLAME);
	return;
    }

}

void do_moonarmour( CHAR_DATA *ch, char *argument )
{
    OBJ_INDEX_DATA *pObjIndex;
    OBJ_DATA *obj;
    char arg[MAX_INPUT_LENGTH];
    int vnum = 0;

    argument = one_argument( argument, arg );

    if (IS_NPC(ch)) return;

    if (!IS_CLASS(ch, CLASS_WEREWOLF))
    {
	    send_to_char("Huh?\n\r",ch);
	    return;
    }
    
    if (ch->power[DISC_WERE_LUNA] < 2)
    {
	send_to_char("You must attain level 2 in Luna before you can create moon armour.\n\r",ch);
	return;
    }

    if (arg[0] == '\0')
    {
    send_to_char("What piece of moon armour do you wish to create?.\n\r", ch);
    send_to_char("A Ring, Plate, or Belt? \n\r",ch);
    return;
    }


    if      (!str_cmp(arg,"ring"     )) vnum = 29711;
    else if (!str_cmp(arg,"belt"   )) vnum = 29710;
    else if (!str_cmp(arg,"plate"    )) vnum = 29709;
    else
    {
	send_to_char("What piece of moon armour do you wish to create?.\n\r", ch);
    send_to_char("A Ring, Plate, or Belt? \n\r",ch);
    return;
    }
    if (ch->practice < 65)
    {
	send_to_char("It costs 65 primal to create a piece of moon armour\n\r",ch);
	return;
    }
   
    if ( vnum == 0 || (pObjIndex = get_obj_index( vnum )) == NULL)
    {
	send_to_char("Missing object, please inform Infidel.\n\r",ch);
	return;
    }
    ch->practice -= 65;
    obj = create_object(pObjIndex, 50);
    obj_to_char(obj, ch);
    act("$p appears in a beam of moonlight.",ch,obj,NULL,TO_CHAR);
    act("$p appears in a beam of moonlight.",ch,obj,NULL,TO_ROOM);
    return;
}



void do_rend(CHAR_DATA *ch,char *argument)
{
  if (IS_NPC(ch)) return;
  if (!IS_CLASS(ch,CLASS_WEREWOLF))
  {
  send_to_char("Huh?\n\r",ch);
  return;
  }
  if (ch->power[DISC_WERE_BOAR] < 7)
  {
  send_to_char("You need Boar 7 to Rend equipment.\n\r",ch);
  return;
  }
  if (!IS_SET(ch->newbits, NEW_REND))
  {
  send_to_char("You will now ruthlessly rend your opponents eq.\n\r",ch);
  SET_BIT(ch->newbits, NEW_REND);
  return;
  }
  else if (IS_SET(ch->newbits, NEW_REND))
  { 
  send_to_char("Your no longer mean to your enemies eq.\n\r",ch);
  REMOVE_BIT(ch->newbits,NEW_REND);
  return;
  }
return;
}

void do_skin(CHAR_DATA *ch,char *argument)
{
  if (IS_NPC(ch)) return;
  if (!IS_CLASS(ch,CLASS_WEREWOLF))
  {
  send_to_char("Huh?\n\r",ch);
  return;
  }
  if (ch->power[DISC_WERE_BEAR] < 7)
  {
  send_to_char("You need Bear 7 to toughen your skin.\n\r",ch);
  return;
  }
  if (!IS_SET(ch->newbits, NEW_SKIN))
  {
  send_to_char("Your skin hardens.\n\r",ch);
  ch->armor -= 100 ;
  SET_BIT(ch->newbits, NEW_SKIN);
  return;
  }
  else if (IS_SET(ch->newbits, NEW_SKIN))
  { 
  send_to_char("Your skin softens.\n\r",ch);
  ch->armor += 100;
  REMOVE_BIT(ch->newbits,NEW_SKIN);
  return;
  }
return;
}

void do_jawlock(CHAR_DATA *ch,char *argument)
{
  if (IS_NPC(ch)) return;
  if (!IS_CLASS(ch,CLASS_WEREWOLF))
  {
  send_to_char("Huh?\n\r",ch);
  return;
  }
  if (ch->power[DISC_WERE_RAPT] < 8)
  {
  send_to_char("You need Raptor 8 to use locked jaw.\n\r",ch);
  return;
  }
  if (!IS_SET(ch->newbits, NEW_JAWLOCK))
  {
  send_to_char("Your jaw grows stronger and more ready to clamp on your enemy.\n\r",ch);
  SET_BIT(ch->newbits, NEW_JAWLOCK);
  return;
  }
  else if (IS_SET(ch->newbits, NEW_JAWLOCK))
  { 
  send_to_char("Your jaw weakens.\n\r",ch);
  REMOVE_BIT(ch->newbits,NEW_JAWLOCK);
  return;
  }
return;
}

void do_perception(CHAR_DATA *ch,char *argument)
{
  if (IS_NPC(ch)) return;
  if (!IS_CLASS(ch,CLASS_WEREWOLF))
  {
  send_to_char("Huh?\n\r",ch);
  return;
  }
  if (ch->power[DISC_WERE_RAPT] < 3)
  {
  send_to_char("You need Raptor 3 to heighten your Perception.\n\r",ch);
  return;
  }
  if (!IS_SET(ch->newbits, NEW_PERCEPTION))
  {
  send_to_char("You become more aware of stealthy enemies.\n\r",ch);
  SET_BIT(ch->newbits, NEW_PERCEPTION);
  return;
  }
  else if (IS_SET(ch->newbits, NEW_PERCEPTION))
  { 
  send_to_char("Your perception dulls.\n\r",ch);
  REMOVE_BIT(ch->newbits,NEW_PERCEPTION);
  return;
  }
return;
}

void do_roar(CHAR_DATA *ch,char *argument)
{
CHAR_DATA *victim;
char buf[MAX_STRING_LENGTH];
if (IS_NPC(ch)) return;
if (!IS_CLASS(ch,CLASS_WEREWOLF))
{send_to_char("You let out an insane BeeF-powered ROAR!!!!!!!!\n\r",ch);return;}
if (ch->power[DISC_WERE_BEAR] < 6)
{
send_to_char("You need bear 6 to roar.\n\r",ch);
return;
}
if ((victim=ch->fighting) == NULL)
{
send_to_char("You arent fighting anyone.\n\r",ch);
return;
}

if (number_range(1,6)==2)
{
sprintf(buf,"%s roars and screams, intimidating the hell out of %s.",
ch->name,victim->name);
act(buf,ch,NULL,NULL,TO_ROOM);
sprintf(buf,"You roar and scream, intimidating the hell out of %s.\n\r",
victim->name);
send_to_char(buf,ch);
do_flee(victim,"");
WAIT_STATE(ch,20);
return;
}
else
{
   stc("You roar loudly.\n\r",ch);
   stc("Nothing happens.\n\r",ch);
   act("$n roars loudly.",ch,NULL,NULL,TO_ROOM);
   WAIT_STATE(ch, 10);
   return;
}
}
 
/*
void do_quills(CHAR_DATA *ch,char *argument)
{
  if (IS_NPC(ch)) return;
  if (!IS_CLASS(ch,CLASS_WEREWOLF))
  {
  send_to_char("Huh?\n\r",ch);
  return;
  }
  if (ch->power[DISC_WERE_BEAR] < 5)
  {
  send_to_char("You need Bear 5 to extend your quills.\n\r",ch);
  return;
  }
  if (!IS_SET(ch->newbits, NEW_QUILLS))
  {
  send_to_char("Sharp quills extend from your back.\n\r",ch);
  SET_BIT(ch->newbits, NEW_QUILLS);
  return;
  }
  else if (IS_SET(ch->newbits, NEW_QUILLS))
  { 
  send_to_char("Your quills sink into your back.\n\r",ch);
  REMOVE_BIT(ch->newbits,NEW_QUILLS);
  return;
  }
return;
}
*/

void do_slam(CHAR_DATA *ch,char *argument)
{
  if (IS_NPC(ch)) return;
  if (!IS_CLASS(ch,CLASS_WEREWOLF))
  {
  send_to_char("Huh?\n\r",ch);
  return;
  }
  if (ch->power[DISC_WERE_BEAR] < 8)
  {
  send_to_char("You need Bear 8 to attempt to shoulder slam.\n\r",ch);
  return;
  }
  if (!IS_SET(ch->newbits, NEW_SLAM))
  {
  send_to_char("You will now shoulder slam when you get a chance.\n\r",ch);
  SET_BIT(ch->newbits, NEW_SLAM);
  return;
  }
  else if (IS_SET(ch->newbits, NEW_SLAM))
  { 
  send_to_char("Your now dont shoulderslam when given a chance.\n\r",ch);
  REMOVE_BIT(ch->newbits,NEW_SLAM);
  return;
  }
return;
}
 
void do_congregation(CHAR_DATA *ch, char *argument )
{
 
    char buf [MAX_INPUT_LENGTH];
    char arg[MAX_STRING_LENGTH];
    CHAR_DATA *victim;
    int blah;
    AFFECT_DATA af;
    argument=one_argument(argument,arg);
    if (IS_NPC(ch)) return;
    if (!IS_CLASS(ch, CLASS_WEREWOLF))
     {
     send_to_char("Huh?\n\r",ch);
     return;
     }
if (arg[0]=='\0')
{
send_to_char("Summon what?\n\r",ch);
return;
}
 
blah=ch->power[DISC_WERE_CONG];
 
    if (ch->pcdata->followers > 0)
    {
	send_to_char("Nothing happens.\n\r",ch);
	return;
    }
    ch->pcdata->followers++;
    victim=create_mobile( get_mob_index( MOB_VNUM_DOG ) );
 if (!str_cmp(arg,"scorpion")&& blah>0)
{
    free_string(victim->short_descr);
    free_string(victim->name);
    free_string(victim->long_descr);
    victim->name=str_dup("scorpion");
    victim->short_descr=str_dup("a Huge Scorpion");
    victim->long_descr=str_dup("a Huge Scorpion stands here.");
    victim->level = 40;
    victim->hit = 300;
    victim->max_hit = 300;
    victim->hitroll = 40;
    victim->damroll = 40;
}
else if (!str_cmp(arg,"cobra")&& blah>1)
{
    free_string(victim->short_descr);
    free_string(victim->name);
    free_string(victim->long_descr);
    victim->name=str_dup("huge cobra");
    victim->short_descr=str_dup("a Huge Cobra");
    victim->long_descr=str_dup("a Huge Cobra stands here.");
    victim->level = 50;
    victim->hit = 400;
    victim->max_hit = 400;
    victim->hitroll = 50;
    victim->damroll = 50;
}
else if (!str_cmp(arg,"lynx")&&blah>2)
{
    free_string(victim->short_descr);
    free_string(victim->name);
    free_string(victim->long_descr);
    victim->name=str_dup("sneaky lynx");
    victim->short_descr=str_dup("a Sneaky Lynx");
    victim->long_descr=str_dup("a stealthy lynx stands here.");
    victim->level = 60;
    victim->hit = 500;
    victim->max_hit = 500;
    victim->hitroll = 60;
    victim->damroll = 60;
}
else if ((!str_cmp(arg,"timber")||!str_cmp(arg,"wolf"))&& blah>3)
{
    free_string(victim->short_descr);
    free_string(victim->name);
    free_string(victim->long_descr);
    victim->name=str_dup("timber wolf");
    victim->short_descr=str_dup("a Huge Timber Wolf");
    victim->long_descr=str_dup("a Huge Timber Wolf stands here.");
    victim->level = 70;
    victim->hit = 2000;
    victim->max_hit = 2000;
    victim->hitroll = 70;
    victim->damroll = 70;
}
else if (!str_cmp(arg,"boar")&&blah>4)
{
    free_string(victim->short_descr);
    free_string(victim->name);
    free_string(victim->long_descr);
    victim->name=str_dup("boar");
    victim->short_descr=str_dup("a Huge Boar");
    victim->long_descr=str_dup("a Huge Boar stands here.");
    victim->level = 100;
    victim->hit = 3000;
    victim->max_hit = 3000;
    victim->hitroll = 100;
    victim->damroll = 100;
}
else if ((!str_cmp(arg,"rhino")||!str_cmp(arg,"white"))&&blah>5)
{
    free_string(victim->short_descr);
    free_string(victim->name);
    free_string(victim->long_descr);
    victim->name=str_dup("white rhino");
    victim->short_descr=str_dup("a Huge White Rhino");
    victim->long_descr=str_dup("a Huge Rhino stands here.");
    victim->level = 150;
    victim->hit = 4000;
    victim->max_hit = 4000;
    victim->hitroll = 150;
    victim->damroll = 150;
}
else if (!str_cmp(arg,"black")&& blah>6)
{
    free_string(victim->short_descr);
    free_string(victim->name);
    free_string(victim->long_descr);
    victim->name=str_dup("black rhino");
    victim->short_descr=str_dup("a Huge Black Rhino");
    victim->long_descr=str_dup("a Huge Rhino stands here.");
    victim->level = 200;
    victim->hit = 5000;
    victim->max_hit = 5000;
    victim->hitroll = 200;
    victim->damroll = 200;
}
else if (!str_cmp(arg,"whitewolf")&& blah>7)
{
    free_string(victim->short_descr);
    free_string(victim->name);
    free_string(victim->long_descr);
    victim->name=str_dup("white wolf");
    victim->short_descr=str_dup("a Huge White Wolf");
    victim->long_descr=str_dup("a Huge Wolf stands here.");
    victim->level = 250;
    victim->hit = 6000;
    victim->max_hit = 6000;
    victim->hitroll = 250;
    victim->damroll = 250;
}
else if (!str_cmp(arg,"clone")&&blah>8)
{
    free_string(victim->short_descr);
    free_string(victim->name);
    free_string(victim->long_descr);
    victim->name=str_dup(ch->name);
    victim->short_descr=str_dup(ch->name);
    victim->long_descr=str_dup(ch->name);
    victim->level = 500;
    victim->hit = 10000;
    victim->max_hit = 10000;
    victim->hitroll =500;
    victim->damroll =500;
} 
else if (!str_cmp(arg,"fenris")&&blah>9)
{
    free_string(victim->short_descr);
    free_string(victim->name);
    free_string(victim->long_descr);
    victim->name=str_dup("fenris");
    victim->short_descr=str_dup("Fenris Wolf");
    victim->long_descr=str_dup("Fenris Wolf stands here.");
    victim->level =950;
    victim->hit = 30000;
    victim->max_hit = 30000;
    victim->hitroll = 999;
    victim->damroll = 999;
}
else
{
send_to_char("Your incorrect call attracts a weak partner\n\r",ch);
}
 
    sprintf(buf,"%s runs out of nowhere.\n\r",victim->short_descr);
    send_to_char(buf,ch);
    
    act(buf,ch,NULL,NULL,TO_ROOM);
   sprintf(buf,"%s\n\r",victim->long_descr);
     free_string(victim->long_descr);
     victim->long_descr=str_dup(buf);
    char_to_room( victim, ch->in_room );
    add_follower( victim, ch );
    af.duration  = 666;
    af.location  = APPLY_NONE;
    af.modifier  = 0;
    af.bitvector = AFF_CHARM;
    affect_to_char( victim, &af );
    SET_BIT(victim->act,ACT_NOEXP);
    SET_BIT(victim->act,PLR_HOLYLIGHT);
    do_group(ch,victim->name);
    return;
}

void do_shred( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;
    int dam;
    argument = one_argument( argument, arg );
 
 
    if(!IS_CLASS(ch,CLASS_WEREWOLF))
    {
    send_to_char("Huh?\n\r",ch);
    return;
    }
    if (ch->power[DISC_WERE_RAPT] < 7)
    {
    send_to_char("You need Raptor 7 to Shred.\n\r",ch);
    return;
    }
 
    if ( arg[0] == '\0' )
    {
        send_to_char( "Shred whom?\n\r", ch );
	return;
    }
 
 
    if ( ( victim = get_char_room( ch, arg ) ) == NULL )
    {
	send_to_char( "They aren't here.\n\r", ch );
	return;
    }
 
    if ( victim == ch )
    {
        send_to_char( "How can you shred yourself?\n\r", ch );
	return;
    }
 
    if ( victim->fighting != NULL )
    {
        send_to_char( "You can't shred a fighting person.\n\r", ch );
	return;
    }
 
    if ( victim->hit < victim->max_hit )
    {
	act( "$N is hurt and suspicious ... you can't sneak up.",
	    ch, NULL, victim, TO_CHAR );
	return;
    }
    if (!IS_AFFECTED(ch, AFF_SHADOWPLANE))
    {
    send_to_char("You must be ShadowPlaned to Shred!\n\r",ch);
    return;
    }
do_shadowplane(ch,"");
if (is_safe(ch,victim)==TRUE) return;

    if (!IS_NPC(victim)) dam = 400;
    if (IS_NPC(victim)) dam = 1200;
dam += number_range(1,20);
WAIT_STATE(ch,15);
if (ch->power[DISC_WERE_RAPT] >6)
damage(ch,victim,dam,gsn_shred);
damage(ch,victim,dam,gsn_shred);
damage(ch,victim,dam,gsn_shred);
if (ch->power[DISC_WERE_RAPT] > 7)
damage(ch,victim,dam,gsn_shred);
if (ch->power[DISC_WERE_RAPT] == 10)
damage(ch,victim,dam,gsn_shred);
    return;
}

void do_run(CHAR_DATA *ch, char *argument)
{
EXIT_DATA *pexit;
char direct[MAX_STRING_LENGTH];
char arg [MAX_INPUT_LENGTH];
char buf[MAX_STRING_LENGTH];
ROOM_INDEX_DATA *in_room;
ROOM_INDEX_DATA *to_room;
int dir = 0;
argument=one_argument(argument,arg);
in_room=ch->in_room;


if (IS_NPC(ch)) return;
if (!IS_CLASS(ch,CLASS_WEREWOLF))
{
send_to_char("Huh?\n\r",ch);
return;
}
if (ch->power[DISC_WERE_RAPT] < 2)
{
send_to_char("You need Raptor 2 to use Run.\n\r",ch);
return;
}
if ( arg[0] == '\0')
{
send_to_char("Run which direction?\n\r",ch);
return;
}
if (str_cmp(arg, "w") && str_cmp(arg,"e") && str_cmp(arg,"n") &&
   str_cmp(arg, "s"))
{
send_to_char("You can only run n, s, e or w.\n\r",ch);
return;
}
if (!str_cmp(arg,"w")) sprintf(direct,"west");
if (!str_cmp(arg,"e")) sprintf(direct,"east");
if (!str_cmp(arg,"s")) sprintf(direct,"south");
if (!str_cmp(arg,"n")) sprintf(direct,"north");
if (!str_cmp(arg,"w")) 
{
dir=3;
}
if (!str_cmp(arg,"e")) 
{
dir=1;
}
 
if (!str_cmp(arg,"n")) 
{
dir=0;
}
if (!str_cmp(arg,"s")) 
{
dir=2;
}
if ( (pexit =  in_room->exit[dir]) == NULL ||
     (to_room= pexit->to_room) == NULL)
{
send_to_char("You stop running.\n\r",ch);
sprintf(buf,"%s comes to a screeching halt.\n\r",ch->name);
act(buf,ch,NULL,NULL,TO_ROOM);
WAIT_STATE(ch,10);
return;
}
sprintf(buf,"You run %s.\n\r",direct);
send_to_char(buf,ch);
sprintf(buf,"%s RUNS %s.\n\r",ch->name,direct);
act(buf,ch,NULL,NULL,TO_ROOM);
move_char(ch,dir);
do_run(ch,arg);
return;
}
 
 

void do_talons( CHAR_DATA *ch, char *argument )
{
CHAR_DATA *victim;
 
char buf[MAX_STRING_LENGTH];
 
int dam;
if (IS_NPC(ch)) return;
 
if (!IS_CLASS(ch,CLASS_WEREWOLF))
{
send_to_char("Huh?\n\r",ch);
return;
}
if (ch->power[DISC_WERE_RAPT] < 10)
{
send_to_char("You need level 10 Raptor to use talons.\n\r",ch);
    return;
}
 
 
if ( ( victim = ch->fighting ) == NULL )
{
send_to_char( "You aren't fighting anyone.\n\r", ch );
      return;
 
}
WAIT_STATE( ch, 12 );
if (!IS_NPC(victim))
{
dam = 350;
}
else if (IS_NPC(victim))
{
dam = 1000;
}
if ( !IS_NPC(victim) && IS_CLASS(victim, CLASS_WEREWOLF) )
{
    if (victim->power[DISC_WERE_BOAR] > 2 ) dam *= 0.5;
}
 
dam += number_range(1,30);
if ( dam <= 0 )
   dam = 1;
sprintf(buf,"Your talons strike $N incredibly hard! [%d]\n\r",dam);
act(buf,ch,NULL,victim,TO_CHAR);
sprintf(buf,"$n's talons strike you incredibly hard! [%d]\n\r",dam);
act(buf,ch,NULL,victim,TO_VICT);
sprintf(buf,"$n's talons strike $N incredibly hard! [%d]\n\r",dam);
act(buf,ch,NULL,victim,TO_NOTVICT);
 
 
send_to_char("\n\r",ch);
victim->hit -= dam;
return;
}
 
 
 
void do_devour( CHAR_DATA *ch, char *argument )
{
char arg[MAX_INPUT_LENGTH];
OBJ_DATA *obj;
char buf[MAX_INPUT_LENGTH];
int hpgain;
 
one_argument( argument, arg );
if (!IS_CLASS(ch,CLASS_WEREWOLF))
{
send_to_char("Huh?\n\r",ch);
return;
}
if (ch->power[DISC_WERE_RAPT] < 5)
{
send_to_char("You require Raptor 5 to devour.\n\r",ch);
return;
}
 
 
if ( arg[0] == '\0' )
{
    send_to_char("Devour what?\n\r",ch);
    return;
    }
obj = get_obj_list( ch, arg, ch->in_room->contents );
if ( obj == NULL )
    {
        send_to_char( "You can't find it.\n\r", ch );
    return;
}
if (obj->item_type != ITEM_CORPSE_NPC)
{
    act( "You are unable to devour $p.", ch, obj, 0, TO_CHAR);
return;
}
else if (obj->chobj != NULL && !IS_NPC(obj->chobj) &&
obj->chobj->pcdata->obj_vnum != 0)
{
    act( "You are unable to devour $p.", ch, obj, 0, TO_CHAR);
    return;
}
hpgain = number_range(100,250);
ch->hit += hpgain;        
if (ch->hit > ch->max_hit)
{
ch->hit = ch->max_hit;
} 
sprintf( buf,"You devour %d hp of life from $p.", hpgain );
act( buf, ch, obj, NULL, TO_CHAR );
act( "You devour $p.", ch, obj, NULL, TO_CHAR );
act( "$p is devoured by $n.", ch, obj, NULL, TO_ROOM );
extract_obj( obj );
return;
}
 
 

