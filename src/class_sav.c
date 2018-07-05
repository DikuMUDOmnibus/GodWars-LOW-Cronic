/******************************************************************
 * Class save, etc.  Used for.. stuff.				  *
 ******************************************************************/

#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "merc.h"

void save_config args(( void ));
void load_config args(( void ));
void check_class args(( CHAR_DATA *ch ));

void check_class( CHAR_DATA *ch )
{
    if ( IS_CLASS(ch, CLASS_VAMPIRE) )
    {
      if ( (config_info->drow + config_info->demons + config_info->ninjas
+ config_info->monks) / 3 < config_info->vampires )
      {
	stc("Sorry. There are too many Vampires at the moment.\n",ch);
	stc("Try another class, or select None for now.\n\r",ch);
	ch->class = 0;
	ch->generation = 1;
      }
    }

    else if ( IS_CLASS(ch, CLASS_DROW) )
    {
      if ( (config_info->vampires + config_info->demons +
config_info->ninjas + config_info->monks) / 3
         < config_info->drow )
      {
        stc("Sorry. There are too many Drow at the moment.\n\r",ch);
        stc("Try another class, or select None for now.\n\r",ch);
        ch->class = 0;
        ch->generation = 1;
      }
    }

    else if ( IS_CLASS(ch, CLASS_DEMON) )
    {
      if ( (config_info->vampires + config_info->drow +
config_info->ninjas + config_info->monks) / 3
         < config_info->demons )
      {
	stc("Sorry. There are too many Demons at the moment.\n\r",ch);
	stc("Try another class, or select None for now.\n\r",ch);
	ch->class = 0;
	ch->generation = 1;
      }
    }

     else if ( IS_CLASS(ch, CLASS_NINJA) )
    {
      if ( (config_info->vampires + config_info->drow +
config_info->demons + config_info->monks) / 3
         < config_info->ninjas )
      {
        stc("Sorry. There are too many Ninjas at the moment.\n\r",ch);
        stc("Try another class, or select None for now.\n\r",ch);
        ch->class = 0;
        ch->generation = 1;
      }
    }

    else if ( IS_CLASS(ch, CLASS_MONK) )
    {
      if ( (config_info->vampires + config_info->drow + 
config_info->ninjas + config_info->demons) / 3
         < config_info->monks )
      {
	stc("Sorry. There are too many Monks at the moment.\n\r",ch);
	stc("Try another class, or select None for now.\n\r",ch);
	ch->class = 0;
	ch->generation = 1;
      }
    }
  
    else return;

  return;

}
/*
void do_configure(CHAR_DATA *ch, char *argument)
{
    char buf[MAX_STRING_LENGTH];
    char arg1[MAX_INPUT_LENGTH];
    char arg2[MAX_INPUT_LENGTH];
    char arg3[MAX_INPUT_LENGTH];
    char *wholearg;
    CHAR_DATA *vch;
    int ox, oy;

    argument = one_argument( argument, arg1 );
    wholearg = argument;
    argument = one_argument( argument, arg2 );
    argument = one_argument( argument, arg3 );

    if (arg1[0] == '\0')
    {
	send_to_char("Current Configuration: \n\r",ch);

	sprintf(buf, "hitpoints = Mobile Hp     *= %d / %d\n\r",
	    config_info->mob_hp_mult, config_info->mob_hp_div);
	send_to_char(buf, ch);
	sprintf(buf, "level     = Mobile Level  *= %d / %d\n\r",
	    config_info->mob_level_mult, config_info->mob_level_div);
	send_to_char(buf, ch);
    	sprintf(buf, "damage    = Mobile Damage *= %d / %d\n\r",
	    config_info->mob_dam_mult, config_info->mob_dam_div);
    	send_to_char(buf, ch);
    	sprintf(buf, "Exp       = Exp Given     *= %d / %d\n\r",
	    config_info->research_exp_mult, config_info->research_exp_div);
    	send_to_char(buf, ch);
        sprintf(buf, "Forge     = Number-Range     %d , %d\n\r",
	    config_info->forge1, config_info->forge2);
	sprintf(buf, "autohelp = Automatic Help Prompt = %s\n\r",config_info->autohelp);
	send_to_char(buf, ch);
	return;
    }

    if (!str_prefix(arg1, "autohelp"))
    {
	if (wholearg[0] == '\0')
	{
	    send_to_char("Syntax:  configure autohelp <help page>",ch);
	    return;
	}

	free_string( config_info->autohelp );
	config_info->autohelp = str_dup( wholearg );
	send_to_char("Successfully updated Auto Help.\n\r",ch);
	save_config();
	send_to_char("Parameters successfully saved in config.sys.\n\r",ch);
	return;
    }	

    if (!is_number(arg2) || !is_number(arg3))
    {
	send_to_char("Syntax:  configure <variable> <x> <y>\n\r",ch);
	send_to_char("where x:y is the ratio you wish to set the variable to.\n\r",ch);
	send_to_char("eg. configure hitpoints 2 1\n\r",ch);
	send_to_char("Doubles the number of hp every mob has.\n\r",ch);
	send_to_char("Configure on it's own lists all variables.\n\r",ch);
	return;
    }

    if (atoi(arg3) == 0 || atoi(arg2)==0)
    {
	send_to_char("Values of zero are not allowed.\n\r",ch);
	return;
    }

    if (atoi(arg2) / atoi(arg3) > 6)
    {
	send_to_char("Ratio too large.\n\r",ch);
	return;
    }

    if (atoi(arg3) / atoi(arg2) > 6)
    {
	send_to_char("Ratio too small.\n\r",ch);
	return;
    }

    if (!str_prefix(arg1, "forge"))
    {
	ox = config_info->forge1;
	oy = config_info->forge2;

	config_info->forge1 = atoi(arg2);
	config_info->forge2 = atoi(arg3);

	save_config();
	stc("Forge updated.\n\r",ch);
	return;
   }

    if (!str_prefix(arg1, "hitpoints"))
    {
	ox = config_info->mob_hp_mult;
	oy = config_info->mob_hp_div;

	config_info->mob_hp_mult = atoi(arg2);
	config_info->mob_hp_div  = atoi(arg3);

	stc("Successful update of config.sys.\n\r",ch);

	for (vch = char_list; vch != NULL; vch = vch->next)
	{
	    if (IS_NPC(vch))
	    {
		vch->hit = vch->hit * oy / ox;
		vch->hit = vch->hit * config_info->mob_hp_mult / config_info->mob_hp_div;
		vch->max_hit = vch->max_hit * oy / ox;
		vch->max_hit = vch->max_hit * config_info->mob_hp_mult / config_info->mob_hp_div;
	    }
	}

	send_to_char("Successful update of mobile hitpoints.\n\r",ch);
	save_config();
	send_to_char("Parameters successfully saved in config.sys.\n\r",ch);
	return;
    }

    if (!str_prefix(arg1, "damage"))
    {

	ox = config_info->mob_dam_mult;
	oy = config_info->mob_dam_div;

	config_info->mob_dam_mult = atoi(arg2);
	config_info->mob_dam_div  = atoi(arg3);

	for (vch = char_list; vch != NULL; vch = vch->next)
	{
	    if (IS_NPC(vch))
	    {
		vch->damroll = vch->damroll * oy / ox;
		vch->damroll = vch->damroll * config_info->mob_hp_mult / config_info->mob_hp_div; 
	    }
	}

	send_to_char("Successful update of mobile damage.\n\r",ch);
	save_config();
	send_to_char("Parameters successfully saved in config.sys.\n\r",ch);
	return;
    }

    if (!str_prefix(arg1, "level"))
    {
	ox = config_info->mob_level_mult;
	oy = config_info->mob_level_div;

	config_info->mob_level_mult = atoi(arg2);
	config_info->mob_level_div  = atoi(arg3);

	for (vch = char_list; vch != NULL; vch = vch->next)
	{
	    if (IS_NPC(vch))
	    {
		int lvl;
		lvl = vch->level;
		vch->level = vch->level * oy / ox;
		vch->level = vch->level * config_info->mob_level_mult / config_info->mob_level_div;
		if (vch->level < lvl)
		    vch->level = lvl;
	    }
	}

	send_to_char("Successful update of mobile level.\n\r",ch);
	save_config();
	send_to_char("Parameters successfully saved in config.sys.\n\r",ch);
	return;
    }

    if (!str_prefix(arg1, "exp"))
    {
	config_info->research_exp_mult = atoi(arg2);
	config_info->research_exp_div  = atoi(arg3);

	send_to_char("Successful update of exp.\n\r",ch);
	save_config();
	send_to_char("Parameters successfully saved in config.sys.\n\r",ch);
	return;
    }

    if (!str_prefix(arg1, "points"))
    {
	config_info->prac_exp_mult = atoi(arg2);
	config_info->prac_exp_div  = atoi(arg3);

	fread_word(fp);
	send_to_char("Successful update of Disc Points.\n\r",ch);
	send_to_char("Parameters successfully saved in config.sys.\n\r",ch);
	return;
    }


    send_to_char("Cannot update a non-existant variable.\n\r",ch);
    send_to_char("config with no arguments to list.\n\r",ch);

}
*/

void initialise_config()
{
    if (config_info == NULL)
	config_info = alloc_perm( sizeof(*config_info) );

    config_info->demons		= 0;
    config_info->mages		= 0;
    config_info->vampires	= 0;
    config_info->werewolves	= 0;
    config_info->drow		= 0;
    config_info->monks		= 0;
    config_info->ninjas		= 0;
    load_config();
}

void load_config()
{
    FILE *fp;

    if ((fp = fopen("config.sys","r")) == NULL)
    {
	bug("Creating config.sys",0);
	save_config();
	return;
    }

    fread_to_eol( fp );
	fread_word(fp);
    config_info->demons		= fread_number( fp ); 
	fread_word(fp);
    config_info->mages		= fread_number( fp ); 
	fread_word(fp);
    config_info->vampires	= fread_number( fp ); 
	fread_word(fp);
    config_info->werewolves	= fread_number( fp ); 
	fread_word(fp);
    config_info->drow		= fread_number( fp ); 
	fread_word(fp);
    config_info->monks		= fread_number( fp ); 
        fread_word(fp);
    config_info->ninjas         = fread_number( fp );
    fclose( fp );
}


void save_config()
{
    FILE *fp;

    if ((fp = fopen("config.sys","w")) == NULL)
    {
	bug("Error saving to config.sys",0);
	return;
    }

    fprintf( fp, "Config.sys - Stuff for Classes, etc.\n");
    fprintf( fp, "Demons	%d\n",config_info->demons    );
    fprintf( fp, "Mages		%d\n",config_info->mages     );
    fprintf( fp, "Vampires      %d\n",config_info->vampires  );
    fprintf( fp, "Werewolves	%d\n",config_info->werewolves);
    fprintf( fp, "Drow		%d\n",config_info->drow	     );
    fprintf( fp, "Monks		%d\n",config_info->monks     );
    fprintf( fp, "Ninjas        %d\n",config_info->ninjas    );
    fclose( fp );
}

