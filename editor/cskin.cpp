
#include "cskin.h"
#include "base/style_box.h"

#define SET_STYLEBOX( m_which , m_style )\
set_stylebox_name(m_which,#m_which);\
set_stylebox(m_which,m_style);

#define SET_CONSTANT( m_which , m_constant )\
set_constant_name(m_which,#m_which);\
set_constant(m_which,m_constant);

#define SET_FONT( m_which , m_font )\
set_font_name(m_which,#m_which);\
set_font(m_which,m_font);

#define SET_BITMAP( m_which , m_bitmap )\
set_bitmap_name(m_which,#m_which);\
set_bitmap(m_which,m_bitmap);

#define SET_COLOR( m_which , m_color )\
set_color_name(m_which,#m_which);\
set_color(m_which,m_color);


void CSkin::set_default_extra() {

	/* PAttern Editor */
	
	StyleBox sbaux=StyleBox(1,Color(0),Color(255,50,50),Color(255,50,50),false);
	sbaux.draw_center=false;
	
	SET_STYLEBOX(SB_PATTERN_EDITOR_CURSOR,sbaux);

	
	SET_COLOR(COLOR_PATTERN_EDITOR_BG,Color(255));
	SET_COLOR(COLOR_PATTERN_EDITOR_ROW_BAR,Color(0));
	SET_COLOR(COLOR_PATTERN_EDITOR_ROW_BEAT,Color(20));
	SET_COLOR(COLOR_PATTERN_EDITOR_ROW_SUB_BEAT,Color(80));

	SET_COLOR(COLOR_PATTERN_EDITOR_NOTE,Color(110,110,255));
	SET_COLOR(COLOR_PATTERN_EDITOR_NOTE_NOFIT,Color(200,200,255));
	SET_COLOR(COLOR_PATTERN_EDITOR_NOTE_SELECTED,Color(255,255,255));
	SET_COLOR(COLOR_PATTERN_EDITOR_HL_BAR,Color(200,200,255));
	SET_COLOR(COLOR_PATTERN_EDITOR_HL_BEAT,Color(230,230,255));

	SET_COLOR(COLOR_PATTERN_EDITOR_TRACK_SEPARATOR,Color(150,150,150));
	SET_COLOR(COLOR_PATTERN_EDITOR_ROW_SEPARATOR,Color(0,0,0));

	SET_COLOR(COLOR_PATTERN_EDITOR_TRACK_NAME,Color(180,180,255));
	SET_COLOR(COLOR_PATTERN_EDITOR_AUTOMATION_NAME,Color(220,220,140));


	SET_COLOR(COLOR_PATTERN_EDITOR_AUTOMATION_VALUE,Color(155,155,32));
	SET_COLOR(COLOR_PATTERN_EDITOR_AUTOMATION_VALUE_NOFIT,Color(190,190,60));
	SET_COLOR(COLOR_PATTERN_EDITOR_AUTOMATION_VALUE_SELECTED,Color(255,255,255));
	SET_COLOR(COLOR_PATTERN_EDITOR_AUTOMATION_HL_BEAT,Color(255,255,170));
	SET_COLOR(COLOR_PATTERN_EDITOR_AUTOMATION_HL_BAR,Color(231,231,110));
	SET_COLOR(COLOR_PATTERN_EDITOR_AUTOMATION_POINT,Color(255,50,0));

	SET_COLOR(COLOR_PATTERN_EDITOR_CURSOR,Color(255,40,40));

	SET_CONSTANT(C_PATTERN_EDITOR_COLUMN_SEPARATION,5);
	SET_CONSTANT(C_PATTERN_EDITOR_ROW_SEPARATION,1);
	SET_CONSTANT(C_PATTERN_EDITOR_TRACK_SEPARATION,8);

	SET_FONT(FONT_PATTERN_EDITOR,0);
	SET_FONT(FONT_PATTERN_EDITOR_TOP,0);
	SET_FONT(FONT_PATTERN_EDITOR_ROWS,0);
	SET_FONT(FONT_PATTERN_EDITOR_COLUMN_NAME,0);


	set_bitmap(BITMAP_PATTERN_EDITOR_TRACK_POPUP,-1);
	set_bitmap(BITMAP_PATTERN_EDITOR_TRACK_POPUP_PRESSED,-1);
	
	/* Orderlist */
	
	SET_FONT(FONT_ORDERLIST,0);
	sbaux=StyleBox(1,Color(0),Color(255,50,50),Color(255,50,50),false);
	sbaux.draw_center=false;	
	SET_STYLEBOX(SB_ORDERLIST_CURSOR,sbaux);
	
	/* Sample Editor */
	
	StyleBox msf;
	for (int i=0;i<4;i++)
		msf.margins[i]=5;
	SET_STYLEBOX( SB_MAIN_STACK_FRAME, msf );
	SET_STYLEBOX( SB_TOP_STACK_FRAME, msf );
	
	/* Sample Table */
	
	SET_FONT(FONT_SAMPLETABLE,0);
	
	/* Envelope */
	SET_FONT(FONT_ENVELOPE,0);
	
	/* PlayBack Button */

	SET_STYLEBOX( SB_PLAYBACK_BUTTON_NORMAL, StyleBox( 2, Color( 150,150,150 ), Color( 250,250,250 ), Color( 50,50,50 ) ) );

	SET_STYLEBOX( SB_PLAYBACK_BUTTON_PRESSED, StyleBox( 2, Color( 110,110,110 ), Color( 50,50,50 ), Color( 200,200,200 ) ) );
	SET_STYLEBOX( SB_PLAYBACK_BUTTON_HOVER, StyleBox( 2, Color( 190,190,190 ), Color( 250,250,250 ), Color( 50,50,50 ) ) );
	SET_STYLEBOX( SB_PLAYBACK_BUTTON_FOCUS, StyleBox( 1, Color( 0,0,0 ), Color( 255,50,50 ), Color( 255,50,50 ) ) );

	for (int i=0;i<4;i++) {

		StyleBox sb=get_stylebox( SB_PLAYBACK_BUTTON_NORMAL );
		sb.margins[i]=6;
		SET_STYLEBOX( SB_PLAYBACK_BUTTON_NORMAL, sb );

	}
}


CSkin::CSkin() : Skin(C_SB_MAX,C_C_MAX,C_BITMAP_MAX,C_FONT_MAX,C_COLOR_MAX) {


	set_default_extra();

}

CSkin::~CSkin() {


}
