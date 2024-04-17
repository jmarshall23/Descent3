/*
 THE COMPUTER CODE CONTAINED HEREIN IS THE SOLE PROPERTY OF OUTRAGE
 ENTERTAINMENT, INC. ("OUTRAGE").  OUTRAGE, IN DISTRIBUTING THE CODE TO
 END-USERS, AND SUBJECT TO ALL OF THE TERMS AND CONDITIONS HEREIN, GRANTS A
 ROYALTY-FREE, PERPETUAL LICENSE TO SUCH END-USERS FOR USE BY SUCH END-USERS
 IN USING, DISPLAYING,  AND CREATING DERIVATIVE WORKS THEREOF, SO LONG AS
 SUCH USE, DISPLAY OR CREATION IS FOR NON-COMMERCIAL, ROYALTY OR REVENUE
 FREE PURPOSES.  IN NO EVENT SHALL THE END-USER USE THE COMPUTER CODE
 CONTAINED HEREIN FOR REVENUE-BEARING PURPOSES.  THE END-USER UNDERSTANDS
 AND AGREES TO THE TERMS HEREIN AND ACCEPTS THE SAME BY USE OF THIS FILE.
 COPYRIGHT 1996-2000 OUTRAGE ENTERTAINMENT, INC.  ALL RIGHTS RESERVED.
 */
#ifndef LEVELGOALEXTERNAL_H_
#define LEVELGOALEXTERNAL_H_

#define LF_AUTO_END_LEVEL 0x01
#define LF_ALL_PRIMARIES_DONE 0x02

// Level Item Types
#define LIT_TERRAIN_CELL 0
#define LIT_INTERNAL_ROOM 1
#define LIT_OBJECT 2
#define LIT_TRIGGER 3
#define LIT_ANY_MINE 4

// Level Item Operations
#define LO_SET_SPECIFIED 0
#define LO_GET_SPECIFIED 1
#define LO_CLEAR_SPECIFIED 2

// Level Goal Flags
#define LGF_BLANK1 0x00000001
#define LGF_SECONDARY_GOAL 0x00000002
#define LGF_ENABLED 0x00000004
#define LGF_COMPLETED 0x00000008
#define LGF_TELCOM_LISTS 0x00000010
#define LGF_GB_DOESNT_KNOW_LOC 0x00000020
#define LGF_NOT_LOC_BASED 0x00000040
#define LGF_FAILED 0x00000080
#define LGF_COMP_ACTIVATE 0x00000100
#define LGF_COMP_ENTER 0x00000200
#define LGF_COMP_DESTROY 0x00000400
#define LGF_COMP_PLAYER_WEAPON 0x00000800
#define LGF_COMP_PLAYER 0x00001000
#define LGF_COMP_DALLAS 0x00002000

#define LGF_COMP_MASK                                                                                                  \
  (LGF_COMP_ACTIVATE | LGF_COMP_ENTER | LGF_COMP_DESTROY | LGF_COMP_PLAYER_WEAPON | LGF_COMP_PLAYER | LGF_COMP_DALLAS)

#define LGF_COMP_TERRAIN_MASK (LGF_COMP_ENTER | LGF_COMP_DALLAS)
#define LGF_COMP_ROOM_MASK (LGF_COMP_ENTER | LGF_COMP_DALLAS)
#define LGF_COMP_OBJECT_MASK (LGF_COMP_DESTROY | LGF_COMP_PLAYER_WEAPON | LGF_COMP_PLAYER | LGF_COMP_DALLAS)
#define LGF_COMP_TRIGGER_MASK (LGF_COMP_ACTIVATE | LGF_COMP_DALLAS)
#define LGF_COMP_MINE_MASK (LGF_COMP_ENTER | LGF_COMP_DALLAS)

#define MAX_GOAL_ITEMS 12
#define MAX_LEVEL_GOALS 32

#define MAX_GOAL_LISTS 4

#endif