/******************************************************************************
 *                                                                            *
 *                 This is the file that has the shit for Garou               *
 *                                                                            *
 ******************************************************************************/

/* 
The Breed, Auspice and Tribe defines are for ch->gifts[ ], replacing Totems.
*/

#define MAX_GIFTS	22

/* Breed */
#define HOMID     	1
#define METIS     	2
#define LUPUS		3

/* Auspice */
#define RAGABASH	4
#define THEURGE		5
#define PHILODOX	6
#define GALLIARD	7
#define AHROUN		8

/* Tribe */
#define BLACKFURIES	9
#define BONEGNAWERS	10
#define CHILDREN	11
#define FIANNA		12
#define GETOFFENRIS	13
#define GLASSWALKERS	14
#define REDTALONS	15
#define SHADOWLORDS	16
#define SILENTSTRIDERS	17
#define SILVERFANGS	18
#define STARGAZERS	19
#define UKTENA		20
#define WENDIGO		21

/* Garou Macros */
#define IS_GAR1(ch, gift)	(IS_SET((ch)->garou1, (gift)))

/* Gnosis */
#define GCURRENT		0
#define GMAXIMUM		1

/* Garou 1 Flags */
#define WOLF_RAZORCLAWS			1
#define WOLF_COCOON			2
