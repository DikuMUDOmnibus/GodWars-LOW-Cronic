#if defined(macintosh)
#include <types.h>
#else
#include <sys/types.h>
#endif
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "merc.h"


/* Gets a mob suitable for a person to kill, randomly */

MOB_INDEX_DATA *get_random_mob( void )
{
    MOB_INDEX_DATA *mob;
    
    for ( ; ; )
    {
        mob = get_mob_index( number_range( 0, 65535 ) );
	
        if ( mob != NULL
	     && mob->vnum > 500
	     && mob->level < 1500 ) break;
    }
        
    return mob;
}

DECLARE_DO_FUN( do_sremove );

/* Fun fun fun fun fun fun forms */
/* name, hitprob, damprob, acmod, hitpointmod, squishable? , size!!!!*/
/* can_cast, can_fly, can_wear, carry_max, max_items, say_word, 
move_word, xp_bonus, polymorph_level, in_room_desc
*/

const struct form_type form_data [MAX_FORMS] =
{

    {"","",                0,      0,      0,      0,      FALSE, 0,
	FALSE, FALSE, FALSE, 0, 0, "","", 0, 0, "","", TRUE},

    /* The demonic forms - 13 of them, oh goody */

    {"lemure","", 0,      0,      0,      20, FALSE,SIZE_MEDIUM,
	TRUE, FALSE, TRUE, -1, -1, "gurgle","slimes", 0, -1,
	 "A slimy misshapen lump of flesh lies here in a pile.", "the lemure",TRUE},
    {"nupperibo","", 0,      0,      0,      20, FALSE,SIZE_MEDIUM,
	TRUE, FALSE, TRUE, -1, -1, "gurgle","slimes", 0, -1,
	 "A foul bloated humanoid is here, infested with maggots.", "the nupperibo",TRUE},
    {"spinagon","", 0,      0,      0,      20, FALSE,SIZE_MEDIUM,
	TRUE, FALSE, TRUE, -1, -1, "growl","flies", 0, -1,
	 "A small imp like creature covered with spines hovers here.", "the spinagon",TRUE},
    {"barbazu","", 0,      0,      0,      20, FALSE,SIZE_MEDIUM,
	TRUE, FALSE, TRUE, -1, -1, "roar","stalks", 0, -1,
	 "A foul, bearded and tall demonic warrior patrolls here.", "the barbazu",TRUE},
    {"abishai","", 0,      0,      0,      20, FALSE,SIZE_MEDIUM,
	TRUE, FALSE, TRUE, -1, -1, "growl","flies", 0, -1,
	 "A red-skinned, gargoyle-like demon flaps its wings here. ", "the abishai",TRUE},
    {"osyluth","", 0,      0,      0,      20, FALSE,SIZE_MEDIUM,
	TRUE, FALSE, TRUE, -1, -1, "growl","clatters", 0, -1,
	 "A proud demonic warrior grins wickedly here. ", "the osyluth",TRUE},
    {"hamatula","", 0,      0,      0,      20, FALSE,SIZE_MEDIUM,
	TRUE, FALSE, TRUE, -1, -1, "growl","stalks", 0, -1,
	 "A tall spiny demonic knight stands here proudly.", "the hamatula",TRUE},
    {"erinyes","", 0,      0,      0,      20, FALSE,SIZE_MEDIUM,
	TRUE, FALSE, TRUE, -1, -1, "say","walks", 0, -1,
	 "A beautiful woman with white feathery wings stands here.", "the erinyes",TRUE},
    {"amnizu","", 0,      0,      0,      20, FALSE,SIZE_SMALL,
	TRUE, FALSE, TRUE, -1, -1, "cough","floats", 0, -1,
	 "A foul little winged, pug-nosed dwarf hovers here, sneering.", "the amnizu",TRUE},
    {"cornugon","", 0,      0,      0,      20, FALSE,SIZE_LARGE,
	TRUE, FALSE, TRUE, -1, -1, "growl","crashes", 0, -1,
	 "A huge red-skinned demon with a spiny tail stands here.", "the cornugon",TRUE},
    {"gelugon","", 0,      0,      0,      20, FALSE,SIZE_LARGE,
	TRUE, FALSE, TRUE, -1, -1, "howl","slimes", 0, -1,
	 "An insect-like frost demon with a long tail stands here.", "the gelugon",TRUE},
    {"pit fiend","", 0,      0,      0,      20, FALSE,SIZE_HUGE,
	TRUE, FALSE, TRUE, -1, -1, "bellow","crashes", 0, -1,
	 "A hulking twelve-foot demon is here, dripping with molten lava.", "the pit fiend",TRUE},
    {"pit lord","", 0,      0,      0,      20, FALSE,SIZE_HUGE,
	TRUE, FALSE, TRUE, -1, -1, "thunder","crashes", 0, -1,
	 "A huge black-skinned demon lord stands here.", "the pit lord",TRUE},

/*
     tree form 

    {"tree","", -20,      -20,     -200,      0, FALSE,SIZE_LARGE,
	FALSE, FALSE, FALSE, -1, -1, "creak","uproots", 0, -1,
	 "A huge gnarled tree stands here.", "the gnarled tree",FALSE},


     dragon forms 

    {"hatchling dragon", "", 0, 0, 0, 0, FALSE, SIZE_MEDIUM,
	TRUE, TRUE, TRUE, -1, -1, "slurp", "crawls", 0, -1,
	"A small %s dragon hatchling is here.", "the %s hatchling",TRUE},

    {"young dragon", "", 0, 0, 0, 0, FALSE, SIZE_MEDIUM,
	TRUE, TRUE, TRUE, -1, -1, "hisses", "crawls", 0, -1,
	"A young %s dragon is here.", "the %s young dragon",TRUE},

    {"juvenile dragon", "", 0, 0, 0, 0, FALSE, SIZE_MEDIUM,
	TRUE, TRUE, TRUE, -1, -1, "hisses", "crawls", 0, -1,
	"A juvenile %s dragon stands here.", "the %s dragon",TRUE},

    {"young adult dragon", "", 0, 0, 0, 0, FALSE, SIZE_LARGE,
	TRUE, TRUE, TRUE, -1, -1, "rumble", "walks", 0, -1,
	"A medium sized %s dragon stands here.", "the %s dragon",TRUE},

    {"mature dragon", "", 0, 0, 0, 0, FALSE, SIZE_LARGE,
	TRUE, TRUE, TRUE, -1, -1, "rumble", "walks", 0, -1,
	"A mature %s dragon stands here.", "the mature %s dragon",TRUE},

    {"adult dragon", "", 0, 0, 0, 0, FALSE, SIZE_LARGE,
	TRUE, TRUE, TRUE, -1, -1, "growl", "strides", 0, -1,
	"A large, adult %s dragon stands here.", "the adult %s dragon",TRUE},

    {"elder dragon", "", 0, 0, 0, 0, FALSE, SIZE_HUGE,
	TRUE, TRUE, TRUE, -1, -1, "growl", "crashes", 0, -1,
	"A huge elder %s dragon is here.", "the elder %s dragon",TRUE},

    {"venerable dragon", "", 0, 0, 0, 0, FALSE, SIZE_HUGE,
	TRUE, TRUE, TRUE, -1, -1, "bellow", "crashes", 0, -1,
	"A venerable %s dragon is here.", "the venerable %s dragon",TRUE},

    {"ancient dragon", "", 0, 0, 0, 0, FALSE, SIZE_GIANT,
	TRUE, TRUE, TRUE, -1, -1, "roar", "crashes", 0, -1,
	"A giant ancient %s dragon is standing here.", "the ancient %s dragon",TRUE},

    {"lesser wyrm", "", 0, 0, 0, 0, FALSE, SIZE_GIANT,
	TRUE, TRUE, TRUE, -1, -1, "roar", "crashes", 0, -1,
	"A giant lesser %s wyrm is standing here.", "the lesser %s wyrm",TRUE},

    {"wyrm", "", 0, 0, 0, 0, FALSE, SIZE_GIANT,
	TRUE, TRUE, TRUE, -1, -1, "roar", "crashes", 0, -1,
	"A giant %s wyrm is here, towering over you.", "the %s wyrm",TRUE},

    {"great wyrm", "", 0, 0, 0, 0, FALSE, SIZE_GIANT,
	TRUE, TRUE, TRUE, -1, -1, "thunder", "crashes", 0, -1,
	"A Great %s Wyrm is here, towering over you.", "the great %s wyrm",TRUE},

    {"supreme dragon", "", 0, 0, 0, 0, FALSE, SIZE_GIANT,
	TRUE, TRUE, TRUE, -1, -1, "thunder", "crashes", 0, -1,
	"The colossal Supreme %s Dragon towers over you.", "the supreme %s dragon",TRUE},

     The alternative were beasties 

    {"wererat","",  60,     60,     -40,    0,    FALSE, SIZE_MEDIUM,
	FALSE,FALSE,TRUE,500,60,"growl","bounds",140,-1,
	"A filthy half-man, half-rat is standing here.", "a filthy wererat",TRUE},
    {"werebear","",  60,     60,     -40,    0,    FALSE, SIZE_LARGE,
	FALSE,FALSE,TRUE,500,60,"boom","lumbers",140,-1,
	"A hulking great half-man, half-bear is standing here.", "a hulking werebear",TRUE},
    {"weretiger","",  60,     60,     -40,    0,    FALSE, SIZE_MEDIUM,
	FALSE,FALSE,TRUE,500,60,"roar","pounces",140,-1,
	"A ferocious half-man, half-tiger is standing here.", "a ferocious weretiger",TRUE},
    {"wereshark","",  60,     60,     -40,    0,    FALSE, SIZE_MEDIUM,
	FALSE,FALSE,TRUE,500,60,"growl","stalks",140,-1,
	"A ravenous half-man, half-shark is standing here.", "a hungry wereshark",TRUE},
    {"stone golem","",  0,     100,     -300,    0,    FALSE, SIZE_MEDIUM,
	FALSE,FALSE,FALSE,500,60,"scrape","lumbers",140,-1,
	"A roughly carved but lifelike humanoid statue stands here.", "a stone golem",TRUE},
    {"pixie","",  20,     1,     -10,    0,    FALSE, SIZE_SMALL,
	TRUE,TRUE,TRUE,500,60,"squeak","flies",140,-1,
	"A little pixie hovers around the room here, flapping its tiny wings.", "a pixie",FALSE},
    {"manticore","",  50,     50,     -90,    1000,    FALSE, SIZE_LARGE,
	TRUE,TRUE,FALSE,500,60,"roar","flies",140,-1,
	"A manticore patrols the area proudly, growling at you.", "a manticore",TRUE},
*/
    {"giant myrlochar","", 0,      0,  0,      0, FALSE,SIZE_LARGE,
        TRUE, TRUE,TRUE, 200, 30, "hisses","creeps", 0, -1,
         "A giant, hairy myrlochar is creeping along here.", "A giant myrlochar", TRUE}

};

/* Fun fun fun fun fun fun forms */
/* name, hitprob, damprob, acmod, hitpointmod, squishable? , size!!!!*/
/* can_cast, can_fly, can_wear, carry_max, max_items, say_word, 
move_word, xp_bonus, polymorph_level, in_room_desc, short_name
*/


int get_normal_form(CHAR_DATA *ch)
{
    if ( IS_CLASS(ch, CLASS_DEMON) )
     return ch->generation;

    return 0;
}

bool is_demonic_form(int form)
{
    if (form >= FRM_LEMURE && form <= FRM_PIT_LORD) return TRUE;
    return FALSE;
}

bool is_vowel(char arg)
{
    if (arg == 'a' || arg == 'e' || arg == 'i'
	|| arg == 'o' || arg == 'u') return TRUE;

    return FALSE;
}

void form_in_room_desc( CHAR_DATA *ch, char *buf )
{
    if (ch->cur_form >= 38 && ch->cur_form <= 50)
    {
	sprintf(buf, form_data[ch->cur_form].in_room_desc, dragon_colour2(ch->dragtype));
	bug(buf, 0);
	bug(dragon_colour2(ch->dragtype), 0);
	return;
    }
    sprintf(buf,form_data[ch->cur_form].in_room_desc);
}

bool is_clan( CHAR_DATA *ch )
{
    return ch->cclan;
}

const char * circlesymbol [7] =
{
	" ",
	"#1*#n",
	"#4~#n",
	"#6=#n",
	"#2###n",
	"#5+#n",
	"#3-#n"
};


int souls_needed( CHAR_DATA *ch )
{
   int souls;
   souls = ch->generation * 5;
   return souls;
}

void do_testform( CHAR_DATA *ch, char *argument )
{
    int form;
    ch->generation = 13;
    ch->class = CLASS_DEMON;
    form = get_normal_form(ch);
    set_form(ch, form);
    return;
}

/* Free hand counter */
bool sremove_obj( CHAR_DATA *ch, int iWear, bool fReplace )
{
    OBJ_DATA *obj;

    if ( ( obj = get_eq_char( ch, iWear ) ) == NULL )
	return TRUE;

    if ( !fReplace )
	return FALSE;

    unequip_char( ch, obj );
    act( "$n stops using $p.", ch, obj, NULL, TO_ROOM );
    act( "You stop using $p.", ch, obj, NULL, TO_CHAR );
    return TRUE;
}


void do_sremove( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    OBJ_DATA *obj;
    

    one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
	send_to_char( "Remove what?\n\r", ch );
	return;
    }

    if (!str_cmp(arg, "all"))
    {
	OBJ_DATA *obj_next;
	for (obj = ch->carrying; obj != NULL; obj = obj_next)
	{
	    obj_next = obj->next_content;
	    if ( obj->wear_loc != WEAR_NONE && can_see_obj(ch, obj))
		sremove_obj(ch,obj->wear_loc, TRUE);
	}
	return;
    }

    if ( ( obj = get_obj_wear( ch, arg ) ) == NULL )
    {
	send_to_char( "You do not have that item.\n\r", ch );
	return;
    }

    sremove_obj( ch, obj->wear_loc, TRUE );
    return;
}


void set_switchname( CHAR_DATA *ch, char *title )
{
    char buf[MAX_STRING_LENGTH];

    if ( IS_NPC(ch) )
    {
	bug( "Set_switchname: NPC.", 0 );
	return;
    }
    strcpy( buf, title );
    free_string( ch->pcdata->switchname );
    ch->pcdata->switchname = str_dup( buf );
    return;
}

void set_pc_name( CHAR_DATA *ch, char *title )
{
    char buf[MAX_STRING_LENGTH];

    if ( IS_NPC(ch) )
    {
	bug( "Set_pc_name: NPC.", 0 );
	return;
    }
    strcpy( buf, title );
    free_string( ch->name);
    ch->name = str_dup( buf );
    return;
}

void set_enter_room_text( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_STRING_LENGTH]; 

    if ( IS_NPC(ch) )
	return;

    if ( strlen(argument) > 45 )
	argument[45] = '\0';

    smash_tilde( argument );

    strcpy( buf, argument );

    free_string( ch->pcdata->enter_room );
    ch->pcdata->enter_room = str_dup( buf );
    return;

}

const struct clanrank_type clanrank [9][14] =
{
    { /* None */
	{       "",              0   ,"" ,""   },
	{       "",              0   ,"" ,""   },
	{       "",              0   ,"" ,""   },
	{       "",              0   ,"" ,""   },
	{       "",              0   ,"" ,""   },
	{       "",              0   ,"" ,""   },
	{       "",              0   ,"" ,""   },
	{       "",              0   ,"" ,""   },
	{       "",              0   ,"" ,""   },
	{       "",              0   ,"" ,""   },
	{       "",              0   ,"" ,""   },
	{       "",              0   ,"" ,""   },
	{       "",              0   ,"" ,""   },
	{       "",              0   ,"" ,""   }
    },

    { /* Dragon */
	{       " bug - chris you git",  0 ,"",""     },
	{       "Hatchling",       30 ,"Dragon Hatchling","Dragon Hatchling"},
	{       "Young",           34 ,"Young Dragon","Young Dragon"},
	{       "Juvenile",        40 ,"Juvenile Dragon","Juvenile Dragon"},
	{       "Young Adult",     46 ,"Dragon","Dragon"     },
	{       "Mature",          52 ,"Mature Dragon","Mature Dragon"},
	{       "Adult",           58 ,"Adult Dragon","Adult Dragon"},
	{       "Elder",           65 ,"Elder Dragon","Elder Dragon"},
	{       "Venerable",       72 ,"Venerable Dragon","Venerable Dragon"},
	{       "Ancient",         79 ,"Ancient Dragon","Ancient Dragon"},
	{       "Lesser Wyrm",     86  ,"Lesser Wyrm","Lesser Wyrm"    },
	{       "Wyrm",            93  ,"Wyrm","Wyrm"    },
	{       "Great Wyrm",      100 ,"Great Wyrm","Great Wyrm"    },
	{       "Supreme Dragon",  112 ,"Supreme Dragon","Supreme Dragon"}
    },
    { /* Demon */
	{       "  *** BUG ***",         0  ,"bug",""    },
	{       "Lemure",             20  ," the lemure","lemure"    },
	{       "Nupperibo",          25  ," the nupperibo","nupperibo"},
	{       "Spinagon",           31  ," the spinagon","spinagon"},
	{       "Barbazu",            38  ," the barbazu","barbazu"},
	{       "Abishai",            45  ," the abishai","abishai"},
	{       "Osyluth",            52  ," the osyluth","osyluth"},
	{       "Hamatula",           59  ," the hamatula","hamatula"},
	{       "Erinyes",            67  ," the erinyes","erinyes"},
	{       "Amnizu",             75  ," the amnizu","amnizu"    },
	{       "Cornugon",           84  ," the cornugon","cornugon"},
	{       "Gelugon",            93  ," the gelugon","gelugon"},
	{       "Pit Fiend",          102  ," the pit fiend","pit fiend"},
	{       "Pit Lord",           112  ," the pit lord","pit lady"}
    },
    { /* Vampires */
	{       " AARGH bug bzzz",       0   ,"",""   },
	{       " Childe}",             20   ,"Childe Vampire","Childe Vampiress"   },
	{       " Childe}",             25   ,"Childe Vampire","Childe Vampiress"   },
	{       " Neonate}",            31   ,"Neonate Vampire","Neonate Vampiress"   },
	{       " Primogen}",           38   ,"Vampire","Vampiress"   },
	{       " Elder}",              45   ,"Vampire","Vampiress"   },
	{       " Archon}",             52   ,"Vampire","Vampiress"   },
	{       " Justicar}",           59   ,"Vampire","Vampiress"   },
	{       " Prince}",             67   ,"Vampire Prince","Vampire Princess"   },
	{       " of the Inner Circle}",75   ,"Vampire Prince","Vampire Princess"   },
	{       " Methuselah}",         84   ,"Methuselah","Methuselah"},
	{       " Antediluvian}",       93   ,"Antediluvian","Antediluvian"   },
	{       " Inconnu}",       102  ,"Vampire Lord","Vampire Lady"},
	{       " Supreme Vampire}",       112  ,"Son of Caine","Daughter of Caine"   }
    },
    { /* Lupine */
	{       "None ",                 0    ,"",""   },
	{       " Pup)",                 20   ,"Were-pup","Were-pup"   },
	{       " Kid)",                 25   ,"Werewolf","Werewolf"   },
	{       " Adult)",               31   ,"Werewolf","Werewolf"   },
	{       " Mature Adult)",        38   ,"Werewolf","Werewolf"   },
	{       " Tracker)",             45   ,"Werewolf","Werewolf"   }, 
	{       " Hunter)",              52   ,"Werewolf","Werewolf"   },
	{       " Warrior)",             59   ,"Werewolf","Werewolf"   },
	{       " Warlord)",             67   ,"Werewolf Lord","Werewolf Lady"   },
	{       " Shaman)",              75   ,"Werewolf Shaman","Werewolf Shaman"   },
	{       " Tribal Chief)",        84   ,"Werewolf Chief","Werewolf Chief"   },
	{       " War Chief)",           93   ,"Werewolf Prince","Werewolf Princess"   },
	{       " Overlord)",            102  ,"Werewolf Overlord","Werewolf Overlord"   },	
      {       " Deity)",               112  ,"Werewolf Diety","Werewolf Diety"   }
    },
    { /* Eladrin */
	{       "None ",                 0      ,"",""},
	{       " Coure]",              20      ,"Coure","Coure"},
	{       " Coure]",              25      ,"Coure","Coure"},
	{       " Noviere]",            31      ,"Noviere","Noviere"},
	{       " Noviere]",            38      ,"Noviere","Noviere"},
	{       " Bralani]",            45      ,"Bralani","Bralani"},
	{       " Bralani]",            52      ,"Bralani","Bralani"},
	{       " Shiere]",             59      ,"Shiere","Shiere"},
	{       " Firre]",              67      ,"Firre","Firre"},
	{       " Firre]",              75      ,"Firre","Firre"},
	{       " Ghaele]",             84      ,"Ghaele","Ghaele"},
	{       " Ghaele]",             93      ,"Ghaele","Ghaele"},
	{       " Tulani]",             102     ,"Tulani","Tulani" },
	{       " Faerie Lord]",        112     ,"Faerie Prince","Faerie Princess" }
    },
    { /* Warlocks */
	{       "None ",                  0   ,"",""   },
	{       " Student>-",             15  ,"Student Wizard","Student Wizard"    },
	{       " Adept>-",               19  ,"Adept Wizard","Adept Wizard"    },
	{       " Artisan>-",           24   ,"Artisan Wizard","Artisan Wizard"   },
	{       " Master>-",            30   ,"Master Wizard","Lady Wizard"   },
	{       " Conjurer>-",          36   ,"Conjurer","Conjuress"   },
	{       " Enchanter>-",         42   ,"Enchanter","Enchantress" },
	{       " Abjurer>-",           49   ,"Abjurer","Abjuress"   },
	{       " Sorcerer>-",          57   ,"Sorcerer","Sorceress"   },
	{       " Mage>-",              66   ,"Mage","Magi"   },
	{       " Wizard>-",            78   ,"High Wizard","High Wizard" },
	{       " Grand Wizard>-",      93   ,"Grand Wizard","Grand Magi" },
	{       " Archmage>-",          99   ,"Archmage","Archmagi"   },
	{       " Supreme Wizard>-",    112  ,"Supreme Wizard","Supreme Wizard"   }
    },
    { /* Hero */
	{       "None ",                 0   ,"",""   },
	{       " Traveller<>",          15  ,"Adventurer","Adventurer" },
	{       " Journeyman<>",         19  ,"Adventurer","Adventurer" },
	{       " Artisan<>",            24  ,"Adventurer","Adventurer" },
	{       " Master<>",             30  ,"Adventurer","Adventurer" },
	{       " Defender<>",           37  ,"Adventurer","Adventurer" },
	{       " Protector<>",          43  ,"Adventurer","Adventurer" },
	{       " Crusader<>",           50  ,"Crusader","Crusader"     },
	{       " Hero<>",               58  ,"Hero","Heroin"    },
	{       " Legend<>",             68  ,"Hero","Heroin"    },
	{       " Legend<>",             80  ,"Hero","Heroin"    },
	{       " Avatar<>",             93  ,"Avatar","Avatar"    },
	{       " Saviour<>",            102   ,"Avatar","Avatar"  },
	{       " Immortal<>",           112   ,"Immortal","Immortal"  }
    },
    { /* Death Knight */
	{       "None ",                 0   ,"",""   },
	{       " Squire>",             15  ,"death knight","death knight" },
	{       " Squire>",             19  ,"Adventurer","Adventurer" },
	{       " Trainee>",            24  ,"Adventurer","Adventurer" },
	{       " Master>",             30  ,"Adventurer","Adventurer" },
	{       " Lord>",               37  ,"Adventurer","Adventurer" },
	{       " Anti-Hero>",          43  ,"Adventurer","Adventurer" },
	{       " Anti-Hero>",          50  ,"Crusader","Crusader"    },
	{       " Knight>",             58  ,"Hero","Heroin"    },
	{       " Grey Knight>",        68  ,"Hero","Heroin"    },
	{       " Dark Knight>",        80  ,"Dark Knight","Dark Knight" },
	{       " Black Knight>",       93  ,"Black Knight","Black Knight" },
	{       " Death Knight>",       102   ,"Death Knight","Death Knight"  },
	{       " Death Knight>",       112   ,"Death Knight","Death Knight"  }
    }
};

void look_name(CHAR_DATA *ch)
{
	char const * class;
	char buf[MAX_STRING_LENGTH];

	if (IS_NPC(ch))
	return;
	
	class = race_table[ch->race].name;
	
	if (ch->class > 0)
	{
		if (ch->sex != SEX_FEMALE)
		  class = clanrank[ch->class][ch->generation].male_name;
		else
		  class = clanrank[ch->class][ch->generation].female_name;
	}

	    sprintf(buf, " the %s", class);


	set_enter_room_text(ch, buf);
}

/* race table */
const   struct  race_type       race_table      []              =
{
/*
    {
	name,           ishname,      pc_race?,
	act bits,       aff_by bits,    off bits,
	imm,            res,            vuln,
	form,           parts 
    },
*/
    { "unique",   "unique",      FALSE, 0, 0, 0, 0, 0, 0, 0, 0 },

    { 
	"human",  "human",              TRUE, 
	0,              0,              0,
	0,              0,              0,
	0,              0
    },

    {
	"elf",    "elvish",              TRUE,
	0,              0,      0,
	0,              0,      0,
	0,              0
    },

    {
	"half-elf",   "half-elvish",          TRUE,
	0,              0,      0,
	0,              0,      0,
	0,              0
    },

    {
	"gnome",     "gnomish",           TRUE,
	0,              0,           0,
	0,              0,           0,
	0,              0
    },

    {
	NULL, NULL, 0, 0, 0, 0, 0, 0
    }
};

char * dragon_colour (sh_int dragtype)
{
    if (dragtype == DRAGON_RED) return "#1Red#n";
    else if (dragtype == DRAGON_WHITE) return "#7White#n";
    else if (dragtype == DRAGON_BLACK) return "Black";
    else if (dragtype == DRAGON_GREEN) return "#2Green#n";
    else if (dragtype == DRAGON_BLUE) return "#4Blue#n";
    else if (dragtype == DRAGON_SILVER) return "#6Silver#n";
    else if (dragtype == DRAGON_GOLD) return "#3Gold#n";
    else if (dragtype == DRAGON_SHADOW) return "Shadow";
    return "Bug, please inform Jander";
}

char * dragon_colour2 (sh_int dragtype)
{
    if (dragtype == DRAGON_RED) return "red";
    else if (dragtype == DRAGON_WHITE) return "white";
    else if (dragtype == DRAGON_BLACK) return "black";
    else if (dragtype == DRAGON_GREEN) return "green";
    else if (dragtype == DRAGON_BLUE) return "blue";
    else if (dragtype == DRAGON_SILVER) return "silver";
    else if (dragtype == DRAGON_GOLD) return "gold";
    else if (dragtype == DRAGON_SHADOW) return "shadow";
    return "Bug, please inform Jander";
}


void set_form(CHAR_DATA *ch, sh_int form)
{
    char shortdesc[MAX_STRING_LENGTH];
    char name[MAX_STRING_LENGTH];
    int oldform;

/*
    if (form == 0 && ch->secondaryrace == RACE_DRAGON)
    {
        form = 37 + ch->generation;
	if (ch->generation < 1 || ch->generation > 13) form = 0;
    }
*/
/*    oldform = form;
    ch->max_hit  -= form_data[ch->cur_form].hit_mod;
    ch->xhitroll -= form_data[ch->cur_form].hit;
    ch->xdamroll -= form_data[ch->cur_form].dam;
    ch->armor    -= form_data[ch->cur_form].ac;
    ch->cur_form = form;
    ch->max_hit  += form_data[ch->cur_form].hit_mod;
    ch->xhitroll += form_data[ch->cur_form].hit;
    ch->xdamroll += form_data[ch->cur_form].dam;
    ch->armor    += form_data[ch->cur_form].ac;
    ch->size = get_size( ch );
*/
/*
//  if (ch->secondaryrace == RACE_ELADRIN) ch->body_form = 0;
    if (form >= 38 && form <=50)
    {
	if (form_data[form].short_desc != NULL
		&& form_data[form].short_desc[0] != '\0')
	sprintf(shortdesc, form_data[ch->cur_form].short_desc, dragon_colour2(ch->dragtype));
	else shortdesc[0] = 0;
    }
    else
    {
	if (form_data[form].short_desc != NULL
		&& form_data[form].short_desc[0] != '\0')
	sprintf(shortdesc, form_data[ch->cur_form].short_desc);
	else shortdesc[0] = 0;
    }
*/
    if (!IS_CLASS(ch,CLASS_DROW))
    {
    sprintf(name, form_data[ch->cur_form].name);
    set_enter_room_text(ch, name);
    if (ch->cur_form == 0)
    {
	look_name(ch);
	set_pc_name(ch, GET_PROPER_NAME(ch));
    }
    else
	if (shortdesc != NULL
		&& shortdesc[0] != '\0')
	set_pc_name(ch, shortdesc);
    }
    if (IS_CLASS(ch, CLASS_DROW))
    {
    sprintf(name, form_data[ch->cur_form].name);
    set_enter_room_text(ch, name);
    if (ch->cur_form == 0)
    {
        set_pc_name(ch, GET_PROPER_NAME(ch));
    }
    else
        if (shortdesc != NULL
                && shortdesc[0] != '\0')
        set_pc_name(ch, shortdesc);
    }  
	
    if (ch->cur_form != 0 && !form_data[ch->cur_form].can_wear)
    {
	OBJ_DATA *obj;

	do_sremove(ch, "all");
	for (obj = ch->carrying; obj != NULL; obj = obj->next_content)
	{
	    SET_BIT(obj->extra_flags2, ITEM_FORM_MELDED);
	}
    }
    else
    {
	OBJ_DATA *obj;

	for (obj = ch->carrying; obj != NULL; obj = obj->next_content)
	{
	    REMOVE_BIT(obj->extra_flags2, ITEM_FORM_MELDED);
	}
    }

/*
    if (ch->cur_form == FRM_WOLF)
    {
	SET_BIT(ch->flag2, AFF_BITE);
	SET_BIT(ch->flag2, AFF_CLAW);
    }

    else if (!( oldform == 0
	  && ch->cur_form == 0))
    {
	REMOVE_BIT(ch->flag2, AFF_BITE);
	REMOVE_BIT(ch->flag2, AFF_CLAW);
    }
*/

    if (ch->cur_form == 0) ch->size = get_size(ch);

    save_char_obj(ch);
}

int get_size(CHAR_DATA *ch)
{
    if (ch->cur_form != 0) return form_data[ch->cur_form].size;
//    if (ch->secondaryrace == RACE_ELADRIN)
//  {
//	if (ch->generation < 3) return SIZE_SMALL;
//	return SIZE_MEDIUM;
//  }

    else if (ch->class == 0)
    {
	return SIZE_MEDIUM;
    }
/*
    else if (ch->secondaryrace == RACE_DRAGON)
    {
	if (ch->generation > 9) return SIZE_GIANT;
	if (ch->generation > 7) return SIZE_HUGE;
	if (ch->generation > 3) return SIZE_LARGE;
    }
*/
    else if (IS_CLASS(ch, CLASS_DEMON))
    {
	if (ch->generation == 10) return SIZE_LARGE;
	if (ch->generation > 10) return SIZE_HUGE;
    }

    return SIZE_MEDIUM;
}

