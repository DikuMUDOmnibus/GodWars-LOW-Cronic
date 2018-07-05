#include <stdio.h>
#include <sys/types.h>
#include <stdlib.h>
#include <string.h>
#include "merc.h"

// Necromancy, Melpominee, Obeah

void do_preserve( CHAR_DATA *ch, char *argument )
{
    OBJ_DATA *obj;
    char arg [MAX_INPUT_LENGTH];
    
    argument = one_argument( argument, arg );

    if ( IS_NPC(ch) ) return;

    if ( !IS_CLASS(ch, CLASS_VAMPIRE) )
    {
      stc("Huh?\n\r",ch);
      return;
    }

    if ( ch->power[DISC_VAMP_NECR] < 2 )
    {
      stc("You must obtain level 2 Necromancy to use Preserve.\n\r",ch);
      return;
    }

    if ( ( obj = get_obj_carry(ch, arg) ) == NULL )
    {
      stc("You do not have that item.\n\r",ch);
      return;
    }

    if ( obj->item_type == ITEM_HEAD )
    {
      stc("You cannot preserve that item.\n\r",ch);
      return;
    }

    if ( obj->timer <= 0 )
    {
      stc("That object has no timer.\n\r",ch);
      return;
    }

    obj->timer = -1;
    act("You place your hands on $p and concentrate on it.",ch,obj,NULL,TO_CHAR);
    act("$n places $s hands on $p and it glows brightly.",ch,obj,NULL,TO_ROOM);
    return;
}

void do_spiritguard( CHAR_DATA *ch, char *argument )
{
   
    if (IS_NPC(ch) ) return;

    if (!IS_CLASS(ch, CLASS_VAMPIRE) )
    {
      stc("Huh?\n\r",ch);
      return;
    }

    if ( ch->power[DISC_VAMP_NECR] < 4 )
    {
      stc("You must obtain level 4 Necromancy to use Spirit Guardian.\n\r",ch);
      return;
    }

    if ( !IS_SET(ch->flag2, AFF_SPIRITGUARD) )
    {
      stc("You awaken your spirit guardian.\n\r",ch);
      SET_BIT(ch->flag2, AFF_SPIRITGUARD);
      return;
    }

    else
    {
      stc("You dismiss your spirit guardian.\n\r",ch);
      REMOVE_BIT(ch->flag2, AFF_SPIRITGUARD);
      return;
    }
  return;
}

void do_spiritgate( CHAR_DATA *ch, char *argument )
{
    OBJ_DATA *obj;
    char arg[MAX_INPUT_LENGTH];
   
    argument = one_argument( argument, arg );
   
    if ( IS_NPC(ch) ) return;
  
    if ( !IS_CLASS(ch, CLASS_VAMPIRE) )
    {
      stc("Huh?\n\r",ch);
      return;
    }

    if ( ch->power[DISC_VAMP_NECR] < 3 )
    {
      stc("You must obtain level 3 Necromancy to use SpiritGate.\n\r",ch);
      return;
    }

    if ( ( obj = get_obj_world( ch, arg ) ) == NULL )
    {
      stc("You cannot find that object.\n\r",ch);
      return;
    }

    if ( ch->pcdata->condition[COND_THIRST] < 66 )
    {
      stc("You do not have enough blood.\n\r",ch);
      return;
    }

    if ( obj->in_room == NULL )
    {
      stc("You cannot locate that object.\n\r",ch);
      return;
    }

    if ( obj->carried_by != NULL )
    {
      stc("That corpse is being carried by someone!\n\r",ch);
      return;
    }

    if ( obj->item_type != ITEM_CORPSE_PC && obj->item_type != ITEM_CORPSE_NPC )
    {
      stc("That's not even a corpse!\n\r",ch);
      return;
    }

    if ( obj->in_room->vnum == ch->in_room->vnum )
    {
      stc("You're already there!\n\r",ch);
      return;
    }

    act("$n steps into a spirit gate and vanishes.",ch,NULL,NULL,TO_ROOM);
    char_from_room(ch);
    char_to_room(ch, get_room_index( obj->in_room->vnum ));
    act("You step through a spirit gate and appear before $p.",ch,obj,NULL,TO_CHAR);
    act("$n steps out of a spirit gate in front of $p.",ch,obj,NULL,TO_ROOM);
    ch->pcdata->condition[COND_THIRST] -= 65;
    return;
}

void do_scream( CHAR_DATA *ch, char *argument )
{
   CHAR_DATA *vch;
   CHAR_DATA *vch_next;
   CHAR_DATA *mount;
   char arg[MAX_INPUT_LENGTH];

   argument = one_argument( argument, arg );

   if ( IS_NPC(ch) ) return;
 
   if ( !IS_CLASS(ch, CLASS_VAMPIRE) )
   {
     stc("Huh?\n\r",ch);
     return;
   }

   if ( ch->power[DISC_VAMP_MELP] < 1 )
   {
     stc("You must obtain level 1 Melpominee to use Scream.\n\r",ch);
     return;
   }

   if ( IS_SET(ch->in_room->room_flags, ROOM_SAFE) )
   {
     stc("Your screams will have no effect here.\n\r",ch);
     return;
   }

   if ( ch->pcdata->condition[COND_THIRST] < 50 )
   {
     stc("You have insufficient blood.\n\r",ch);
     return;
   }

    for ( vch = char_list; vch != NULL; vch = vch_next )
    {
        vch_next        = vch->next;

        if ( vch->in_room == NULL )
            continue;
        if ( !IS_NPC(vch) && vch->pcdata->chobj != NULL )
            continue;
        if ( ch == vch )
              continue;
        if ( vch->in_room == ch->in_room )
        {
            if ( ( mount = ch->mount ) != NULL )
            {
                if ( mount == vch )
                continue;
            }
            if (can_see(ch,vch))
            {
              if ( number_range( 1, (7 - ch->power[DISC_VAMP_MELP])) == 2 )
              {
                act("$n lets out an ear-popping scream!",ch,NULL,vch,TO_ROOM);
                stc("You fall to the ground, clutching your ears.\n\r",vch);
                vch->position = POS_STUNNED;
                act("$n falls to the ground, stunned.",vch,NULL,vch,TO_NOTVICT);
                return;
              }
	      else
	      {
                act("$n lets out an ear-popping scream!",ch,NULL,vch,TO_ROOM);
	        return;
	      }
	    }
	    else continue;

	    return;
        }
    }

  return;
}
