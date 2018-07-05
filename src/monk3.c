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
#include "player.h"
#include "monk.h"

void do_blinding( CHAR_DATA *ch, char *argument )
{
  CHAR_DATA *victim;
  char arg[MAX_INPUT_LENGTH];

  argument = one_argument( argument, arg );
 
  if ( IS_NPC(ch)) return;

  if ( !IS_CLASS(ch, CLASS_MONK))
  {
    stc("Huh?\n\r",ch);
    return;
  }

  if ( ch->monkab[SPIRIT] < 1 )
  {
    stc("Your spirit is far too weak.\n\r",ch);
    stc("You need a level one mastery of the Spirit to use Blinding Agony.\n\r",ch);
    return;
  }

  if ( arg[0] == '\0' )
  {
    stc("Who do you wish to use Blinding Agony on?\n\r",ch);
    return;
  }

  if ( ch->move < 200 )
  {
    stc("You are far too exhausted to use Blinding Agony.\n\r",ch);
    stc("Try getting 200 movement points.\n\r",ch);
    return;
  }

  if ( ( victim = get_char_room( ch, arg ) ) == NULL )
  {
    stc("They are not here.\n\r",ch);
    return;
  }

  if ( is_safe(ch,victim) ) return;

  if ( IS_SET(victim->flag2, AFF_TOTALBLIND) )
  {
    act("You hurl a ball of blinding light at $N.",ch,NULL,victim,TO_CHAR);
    act("$n hurls a ball of blinding light at you.",ch,NULL,victim,TO_VICT); 
    act("$n hurls a ball of blinding light at $N.",ch,NULL,victim,TO_ROOM);
    stc("Nothing happens.\n\r",ch);
    ch->move -= 100;
    return;
  }

  else
  {
    act("You hurl a ball of blinding light at $N.",ch,NULL,victim,TO_CHAR);
    act("$n hurls a ball of blinding light at you.",ch,NULL,victim,TO_VICT);
    act("$n hurls a ball of blinding light at $N.",ch,NULL,victim,TO_ROOM);
    SET_BIT(victim->flag2, AFF_TOTALBLIND);
    ch->move -= 200;
    stc("The searing heat of the ball blinds you.\n\r",victim);
    return;
  }

 return;

}
