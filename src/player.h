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



/* Special Bits */



#define SPC_CHAMPION	1 /* PLR_CHAMPION 4 */

#define SPC_DEMON_LORD	2 /* OLD_DEMON Flag */

#define SPC_WOLFMAN	4 /* old PLR_WOLFMAN */

#define SPC_PRINCE      8 /* old EXTRA_PRINCE */

#define SPC_SIRE	16 /* Old EXTRA_SIRE */

#define SPC_ANARCH      32 /* old extra_anarch */

#define SPC_INCONNU     64 /* old extra_inconnu */

#define SPC_DROW_WAR	128

#define SPC_DROW_MAG	256

#define SPC_DROW_CLE	512

#define SPC_ROGUE       1024

/* Class Bits */

#define CLASS_DEMON	 1 /* PLR_DEMON 2 */

#define CLASS_MAGE	 2 

#define CLASS_WEREWOLF	 4 /* PLR_WEREWOLF */

#define CLASS_VAMPIRE	 8 

#define CLASS_HIGHLANDER 16 /* EXTRA_HIGHLANDER */

#define CLASS_DROW	 32

#define CLASS_MONK	 64

#define CLASS_NINJA     128

#define CLASS_ELADRIN   256

#define CLASS_DRAGON    512

#define CLASS_HERO     1024

#define CLASS_ZERG     2048


/* 

 * Bits For Highlanders

 */

#define HPOWER_WPNSKILL 0



/* 

 * Bits For Mages

 */

#define MPOWER_RUNE0	0

#define MPOWER_RUNE1	1

#define MPOWER_RUNE2	2

#define MPOWER_RUNE3	3

#define MPOWER_RUNE4	4

#define MPOWER_RUNES	5
#define MPOWER_SIGILS	6
#define MPOWER_GLYPHS	7




#define NPOWER_SORA 1

#define NPOWER_CHIKYU 2

#define NPOWER_NINGENNO 3

#define BLACK_BELT 4

#define HARA_KIRI 5


/*

 * Bits for Werewolves 

 */

/* Stats */



#define UNI_GEN        	0 /* vampgen */

#define UNI_AFF		1 /* vampaff */

#define UNI_CURRENT	2 /* vamppass */

#define UNI_RAGE	3 /* ch->wolf = how big rage they are in*/

#define UNI_FORM0      	4/* wolfform[0] */

#define UNI_FORM1      	5 /* wolfform[1] */



#define	WOLF_POLYAFF	6


/* 
 * Powers 
 * Totems for werewolves.
 */

#define WPOWER_MANTIS	      0

#define WPOWER_BEAR	      1

#define WPOWER_LYNX	      2

#define WPOWER_BOAR	      3

#define WPOWER_OWL	      4

#define WPOWER_SPIDER	      5

#define WPOWER_WOLF	      6

#define WPOWER_HAWK	      7

#define WPOWER_RAPTOR         8

#define WPOWER_LUNA           9

#define WPOWER_SILVER	     10

#define WPOWER_PAIN          11

#define WPOWER_CONGREGATION  12

#define VPOWER_ANIMAL         1

#define VPOWER_AUSPEX         2

#define VPOWER_CELERITY       3 

#define VPOWER_DOMINATE       4

#define VPOWER_FORTITUDE      5

#define VPOWER_OBFUSCATE      6

#define VPOWER_OBTENEBRATION  7

#define VPOWER_POTENCE        8

#define VPOWER_PRESENCE       9

#define VPOWER_PROTEAN       10

#define VPOWER_QUIETUS       11

#define VPOWER_SERPENTIS     12

#define VPOWER_THAU          13
#define VPOWER_VICISSITUDE   14
#define VPOWER_DAIM          15
#define VPOWER_NECROMANCY    16
#define VPOWER_THANATOSIS    17
#define VPOWER_MELPOMINEE    18
#define VPOWER_OBJ_VNUM      19


#define DROW_POWER	8

#define DROW_TOTAL	9

#define DROW_MAGIC	11



/*

 * Bits for Demonic Champions.

 */



/*Stats*/



#define DEMON_CURRENT		      8	/* POWER_CURRENT 0*/

#define DEMON_TOTAL		      9	/* POWER_TOTAL   1*/

#define DEMON_POWER		     10 /* TOTAL ARMOUR BOUNS */

#define DEMON_PPOWER		     11 /* ARMOUR BONUS FROM PLAYERS */

/*Powers*/

#define DPOWER_FLAGS		      0 /* C_POWERS  1 */

#define DPOWER_CURRENT		      1 /* C_CURRENT 2 */

#define DPOWER_HEAD		      2 /* C_HEAD    3 */

#define DPOWER_TAIL		      3 /* C_TAIL    4 */

#define DPOWER_OBJ_VNUM		      4 /* DISC[10]  5 */

#define VPOWER_CURRENT		     18



#define DEM_UNFOLDED		      1



#define DEM_FANGS		      1

#define DEM_CLAWS		      2

#define DEM_DAMN		      3

#define DEM_HORNS		      4

#define DEM_HOOVES		      8

#define DEM_EYES		     16

#define DEM_WINGS		     32

#define DEM_MIGHT		     64

#define DEM_TOUGH		    128

#define DEM_SPEED		    256

#define DEM_TRAVEL		    512

#define DEM_SCRY		   1024

#define DEM_SHADOWSIGHT		   2048

/* Object Powers */

#define DEM_MOVE		   4096   /* 1 Can roll as an object */

#define DEM_LEAP		   8192   /* 2 Can leap out of someone's hands */

#define DEM_MAGIC		  16384   /* 4 Can cast spells as an object */

#define DEM_LIFESPAN		  32768   /* 8 Can change lifespan */
#define DEM_HEAD		 131072

#define DEM_TAIL		 262144

#define DEM_SHIELD		 524288

#define DEM_TRUESIGHT		1048576

#define DEM_GRAFT		2097152
#define DEM_IMMOLATE		4194304
#define DEM_INFERNO		8388608
#define DEM_CAUST	       16777216
#define DEM_ENTOMB	       33554432
#define DEM_FREEZEWEAPON       67108864
#define DEM_UNNERVE	      121601760
#define DEM_LEECH	      243203520
#define DEM_BLINK	      486407040


#define HEAD_NORMAL		      0

#define HEAD_WOLF		      1

#define HEAD_EAGLE		      2

#define HEAD_LION		      4

#define HEAD_SNAKE		      8



#define TAIL_NONE		      0

#define TAIL_POISONOUS		      1

#define TAIL_SCORPION		      2



/* Drow Powers */

#define DPOWER_DROWFIRE		1
#define DPOWER_DARKNESS		2
#define DPOWER_LIGHTS		4
#define DPOWER_DROWSIGHT	8
#define DPOWER_LEVITATION	16
#define DPOWER_DROWSHIELD	32
#define DPOWER_DROWPOISON	64
#define DPOWER_SHADOWWALK	128
#define DPOWER_GAROTTE		256
#define DPOWER_ARMS		512
#define DPOWER_DROWHATE		1024
#define DPOWER_SPIDERFORM	2048
#define DPOWER_WEB		4096
#define DPOWER_DGAROTTE		8192
#define DPOWER_CONFUSE		16384
#define DPOWER_GLAMOUR		32768
#define DPOWER_EARTHSHATTER	65536
#define DPOWER_SPEED		131072
#define DPOWER_TOUGHSKIN	262144

/*new demon powers*/

#define DEM_CONE		1

#define DEM_STRENGTH		2

#define DEM_FORM		3

#define DEM_STAKE		4

#define DEM_SPEEDY		5

#define DEM_TRUE		6

/* Monk Power Bit */

#define PMONK			0

/* Zerg Powers */
#define PZERG			0
