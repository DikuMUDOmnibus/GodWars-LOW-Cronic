#include <sys/time.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "merc.h"


void load_kingdoms(void) {

	FILE *fp;
	ROOM_INDEX_DATA *room;
	OBJ_INDEX_DATA *obj;
	int i;
	int vnum;

	king_table[0].name = "";
	king_table[0].who_name = "";
	king_table[0].leader_name = "";
	king_table[0].pk = 0;
	king_table[0].pd = 0;
	king_table[0].num = 0;
	king_table[0].recall = 0;
	king_table[0].object = NULL;
	king_table[0].altar = 0;

	if ((fp = fopen("../area/kingdom.dat", "r")) == NULL) {
	log_string("Error: kingdom.dat not found!");
	exit(1);}

	for (i = 1; i <= MAX_KINGDOM; i++) {

	vnum = 90000 + i*100;

	king_table[i].name = fread_string(fp);
	king_table[i].who_name = fread_string(fp);
	king_table[i].leader_name = fread_string(fp);
	king_table[i].pk = fread_number(fp);
	king_table[i].pd = fread_number(fp);
	king_table[i].recall = fread_number(fp);
	king_table[i].altar = fread_number(fp);
	king_table[i].num = i;
/*	obj = get_obj_index(vnum);
	if (obj != NULL)
	king_table[i].object =  create_object(obj,0);
	if (king_table[i].object != NULL && (room = get_room_index(king_table[i].altar)) != NULL)
		obj_to_room(king_table[i].object,room);
*/	}	

	fclose(fp);
	return;
}


void save_kingdoms(void) {

	FILE *fp;
	int i;

	if ((fp = fopen("../area/kingdom.dat","w")) == NULL) {
	log_string("Error writing to kingdom.dat");
	}

	for (i = 1; i <= MAX_KINGDOM; i++) {
	fprintf(fp,"%s~\n", king_table[i].name);
	fprintf(fp,"%s~\n", king_table[i].who_name);
	fprintf(fp,"%s~\n", king_table[i].leader_name);
	fprintf(fp, "%d\n%d\n",king_table[i].pk, king_table[i].pd);
	fprintf(fp, "%d\n%d\n",king_table[i].recall,king_table[i].altar);

	}

	fclose(fp);
	return;
}

void do_clanlist(CHAR_DATA *ch, char *argument) {

	char buf[MAX_STRING_LENGTH];
	int i;

	if (IS_NPC(ch)) return;

	sprintf(buf, "Clan               Leader       PKs     PDs\n\r" );
	send_to_char(buf,ch);
	sprintf(buf, "----               ------       ---     ---\n\r" );
	send_to_char(buf,ch);
	for (i = 1; i <= MAX_KINGDOM; i++) {
	sprintf(buf, "%-18s %-12s %-7d %-7d\n\r", 
	king_table[i].name,
	king_table[i].leader_name,
	king_table[i].pk,
	king_table[i].pd);
	send_to_char(buf,ch);
	}
	send_to_char("\n\r", ch );

	return;
}

void do_induct(CHAR_DATA *ch, char *argument) {

	CHAR_DATA *victim;
	char arg[MAX_STRING_LENGTH];

	argument = one_argument(argument,arg);

	if (IS_NPC(ch)) return;

	if (ch->pcdata->kingdom == 0) {
	send_to_char("You are not in a clan.\n\r", ch );
	return;}

	if (str_cmp(ch->pcdata->switchname,king_table[ch->pcdata->kingdom].leader_name)
	&& !IS_SET(ch->special, SPC_PRINCE)) {
	send_to_char("You cannot induct people.\n\r", ch );
	return;}

	if ((victim = get_char_room(ch, arg)) == NULL) {
	send_to_char("They are not here.\n\r", ch );
	return;}

	if (IS_NPC(victim)) {
	send_to_char("Not on NPCs.\n\r", ch );
	return;}
/*
	if (!IS_SET(victim->newbits, NEW_CLANNABLE)) {
	send_to_char("That person is not CLANNABLE.\n\r", ch );
	return;}
*/
	if (victim->pcdata->kingdom != 0) {
	send_to_char("That person is already clanned.\n\r", ch );
	return;}

	victim->pcdata->kingdom = ch->pcdata->kingdom;
	if (IS_SET(victim->special, SPC_PRINCE)) 
	REMOVE_BIT(victim->special, SPC_PRINCE);
	send_to_char("They are now in your clan.\n\r", ch);
	send_to_char("You are now in a clan.\n\r", victim);
	save_char_obj(victim);
	return;
}

void do_outcast(CHAR_DATA *ch, char *argument) {

	CHAR_DATA *victim;
	char arg[MAX_STRING_LENGTH];
	argument = one_argument(argument, arg);

	if (IS_NPC(ch)) return;

	if (ch->pcdata->kingdom == 0) {
	send_to_char("You are not in a clan.\n\r", ch );
	return;}

	if (str_cmp(ch->pcdata->switchname,king_table[ch->pcdata->kingdom].leader_name)) {
	send_to_char("You are not a clan leader.\n\r", ch );
	return;}

	if ((victim = get_char_world(ch, arg)) == NULL ) {
	send_to_char("They are not here.\n\r", ch );
	return;}

	if (IS_NPC(victim)) {
	send_to_char("Not on NPCs.\n\r", ch );
	return;}

	if (victim->pcdata->kingdom != ch->pcdata->kingdom) {
	send_to_char("They are not in your clan.\n\r", ch );
	return;}

	if (ch == victim) {
	send_to_char("You can't outcast yourself.\n\r", ch );
	return;}

	victim->pcdata->kingdom = 0;
	if (IS_SET(victim->special, SPC_PRINCE))
	REMOVE_BIT(victim->special, SPC_PRINCE);
	send_to_char("You remove them from your clan.\n\r", ch );
	send_to_char("You have been outcasted from your clan!\n\r",victim);	

	return;
}

void do_prince(CHAR_DATA *ch, char *argument) {

	CHAR_DATA *victim;
	char arg[MAX_STRING_LENGTH];

	argument = one_argument(argument, arg);

	if (IS_NPC(ch)) return;

	if (ch->pcdata->kingdom == 0) {
	send_to_char("You are not in a clan.\n\r", ch );
	return;}

	if (str_cmp(ch->pcdata->switchname,king_table[ch->pcdata->kingdom].leader_name)) {
	send_to_char("You are not the leader of your clan.\n\r", ch );
	return;}

	if ((victim = get_char_room(ch, arg)) == NULL) {
	send_to_char("They are not here.\n\r", ch );
	return;}

	if (IS_NPC(victim)) {
	send_to_char("Not on NPCs.\n\r", ch );
	return;}

	if (victim->pcdata->kingdom != ch->pcdata->kingdom) {
	send_to_char("They are not in your clan.\n\r", ch );
	return;}

	if (ch == victim) {
	send_to_char("You can't do that.\n\r", ch );
	return;}

	if (IS_SET(victim->special, SPC_PRINCE)) {
	REMOVE_BIT(victim->special, SPC_PRINCE);
	send_to_char("You are no longer a prince.\n\r", victim);
	send_to_char("They are no longer a prince.\n\r", ch );
	save_char_obj(victim);
	return;}

	SET_BIT(victim->special, SPC_PRINCE);
	send_to_char("You are now a prince!\n\r", victim);
	send_to_char("You make them a prince.\n\r", ch );
	save_char_obj(victim);
	return;
}

void do_clannable(CHAR_DATA *ch, char *argument) {

	if (IS_NPC(ch)) return;

	if (ch->pcdata->kingdom != 0) {
	send_to_char("You already are in a clan.\n\r", ch );
	return;}

	if (IS_SET(ch->newbits, NEW_CLANNABLE)) {
	REMOVE_BIT(ch->newbits, NEW_CLANNABLE);
	send_to_char("You can no longer be clanned.\n\r", ch );
	return;}

	SET_BIT(ch->newbits, NEW_CLANNABLE);
	send_to_char("You can now be clanned.\n\r", ch );

	return;
}

void do_kingdom(CHAR_DATA *ch, char *argument)
{

    char buf[MAX_STRING_LENGTH];
    CHAR_DATA *gch;
   char rankname[9];

    if (IS_NPC(ch)) return;

	if (ch->pcdata->kingdom == 0) {
	send_to_char("You are not in a kingdom.\n\r", ch );
	return;}

	sprintf(buf, "%s's Kingdom of %s:\n\r",
	king_table[ch->pcdata->kingdom].leader_name,
	king_table[ch->pcdata->kingdom].name);

    send_to_char( buf, ch );
    send_to_char("#2[    Name    ] [ Hits ] [ HPmax ] [ Mana ] [ Move ] [ Status ] [  Rank  ]#n\n\r", ch );
    for ( gch = char_list; gch != NULL; gch = gch->next )
    {
	if ( IS_NPC(gch) ) continue;
	if ( gch->pcdata->kingdom != ch->pcdata->kingdom) 
	    continue;
	if (!str_cmp(king_table[ch->pcdata->kingdom].leader_name,gch->pcdata->switchname)) 
	sprintf(rankname,"King");
	else if (IS_SET(gch->special, SPC_PRINCE)) 
	sprintf(rankname, "Prince");
	else
	 sprintf(rankname,"Member");
	    sprintf( buf,
	    "#2[#n%-12s#2] [#n%6d#2] [#n%7d#2] [#n%6d#2] [#n%6d#2] [#n   %-2d   #2] [ #n%-6s #2]#n\n\r",
		gch->pcdata->switchname ,
		gch->hit,gch->max_hit,gch->mana,gch->move,
		gch->race, rankname);
		send_to_char( buf, ch );
    }
    return;
}

void do_krecall(CHAR_DATA *ch, char *argument) {

	CHAR_DATA *victim;
	CHAR_DATA *mount;
	ROOM_INDEX_DATA *location;

	if (IS_NPC(ch)) return;

	if (ch->pcdata->kingdom == 0) {
	send_to_char("Huh?\n\r", ch );
	return;}

	act("$n's body flickers with green energy.",ch,NULL,NULL,TO_ROOM);
	act("Your body flickers with green energy.",ch,NULL,NULL,TO_CHAR);

	if ((location = get_room_index(king_table[ch->pcdata->kingdom].recall)) == NULL) {
	send_to_char("You are completely lost.\n\r", ch );
	return;}

	if (ch->in_room == location) return;

	if (IS_SET(ch->in_room->room_flags, ROOM_NO_RECALL) || IS_AFFECTED(ch, AFF_CURSE)) {
	send_to_char("You are unable to recall.\n\r", ch );
	return;}

	if ((victim = ch->fighting) != NULL) {
	if (number_bits(1) == 0) {
		WAIT_STATE(ch,4);
		send_to_char("You failed!\n\r", ch );
		return;}
	send_to_char("You recall from combat!\n\r", ch );
	stop_fighting(ch,TRUE);
	}

	act("$n dissapears.",ch,NULL,NULL,TO_ROOM);
	char_from_room(ch);
	char_to_room(ch,location);
	act("$n appears in the room.",ch,NULL,NULL,TO_ROOM);
	do_look(ch,"auto");
	if ((mount = ch->mount) == NULL) return;
	char_from_room(mount);
	char_to_room(mount,ch->in_room);
	
	return;
}


bool lost_kpower(CHAR_DATA *ch) {

	ROOM_INDEX_DATA *room;

	if (IS_NPC(ch)) return FALSE;
	
	if (ch->pcdata->kingdom <= 0 || ch->pcdata->kingdom > MAX_KINGDOM) 
		return FALSE;

	if (king_table[ch->pcdata->kingdom].object == NULL)
		return FALSE;

	if ((room = get_room_index(king_table[ch->pcdata->kingdom].altar)) == NULL) 
		return FALSE;

/*	if (king_table[ch->pcdata->kingdom].object->in_room->vnum ==
king_table[ch->pcdata->kingdom].altar) 
		return FALSE;
*/
	return FALSE;
	return TRUE;
}
