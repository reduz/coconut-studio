//
// C++ Implementation: chibitracker_palm
//
// Description: 
//
//
// Author: Juan Linietsky <reduzio@gmail.com>, (C) 2004
//
// Copyright: See COPYING file that comes with this distribution
//
//

#include <stdio.h>
#include <stdlib.h>
#include <SDL_main.h>
#include <stdio.h>
#include <stdlib.h>
#include "globals/rstring.h"
#include "gui/base/file_system.h"
#include "gui/widgets/scroll_bar.h"
#include "gui/base/window.h"
#include "drivers/file_access.h"
#include "drivers/painter_sdl.h"
#include "drivers/keycodes_sdl.h"
#include "drivers/timer_sdl.h"
#include "drivers/file_system_dirent.h"
#include "drivers/file_system_windows.h"
#include "editor/editor.h"
#include "editor/cskin.h"
#include "error_macros.h"
#include "editor/config_file.h"
#ifdef WINDOWS_ENABLED

#define CONFIG_DIR_BASE String(getenv("APPDATA")?getenv("APPDATA"):".")
#define CONFIG_DIR String(getenv("APPDATA")?"Reshaked":".")
#define CONFIG_FILE String("reshaked.cfg")

#define CONFIG_FILE_PATH ( CONFIG_DIR_BASE + "\\" + CONFIG_DIR + "\\" + CONFIG_FILE )
#define CONFIG_DIR_SKINS ( CONFIG_DIR_BASE + "\\" + CONFIG_DIR + "\\skins" )
#define CONFIG_SYSTEM_SKINS ( String(".\\skins") )
#endif

#ifdef POSIX_ENABLED

#define CONFIG_DIR_BASE String(getenv("HOME"))
#define CONFIG_DIR String(".reshaked")
#define CONFIG_FILE String("reshaked.cfg")

#define CONFIG_FILE_PATH ( CONFIG_DIR_BASE + "/" + CONFIG_DIR + "/" + CONFIG_FILE )
#define CONFIG_DIR_SKINS ( CONFIG_DIR_BASE + "/" + CONFIG_DIR + "/skins" )

#ifdef PREFIX
#define CONFIG_SYSTEM_SKINS ( String("PREFIX")+String("/share/reshaked/skins") )
#else
//not going to work... but hey
#define CONFIG_SYSTEM_SKINS ( String("./skins") ) 

#endif
#endif


namespace GUI {
void __print_error(const char *p_file,int p_line,const char* p_error) {
	
	printf("ERROR: at %s:%i -- %s\n",p_file,p_line,p_error);

	
}

}
     

static void ensure_config_dir_exists() {

	GUI::FileSystem *fs = GUI::FileSystem::instance_func();

	ERR_FAIL_COND(fs->change_dir( CONFIG_DIR_BASE ));
	
	if (fs->change_dir( CONFIG_DIR ) ) {
			
		ERR_FAIL_COND( fs->make_dir( CONFIG_DIR ) );
		fs->change_dir( CONFIG_DIR );
	}
	
	if (fs->change_dir( "skins" ) ) {
			
		ERR_FAIL_COND( fs->make_dir( "skins" ) );
	}
	
	delete fs;
}

//notes:
//pattern - puntero(s) a last read, on write, pointer is invalidated and must search again


#define DEFAULT_H 550
#define DEFAULT_W 750

using namespace GUI;

int main(int argc,char *argv[]) {

	FileAccess::set_as_default();

#ifdef WINDOWS_ENABLED	
	FileSystemWindows::set_default_filesystem();
#else
	FileSystemDirent::set_default_filesystem();

#endif

	/* CONFIG DIR */

	ensure_config_dir_exists();

	FileAccessWrapper * config_file_access = FileAccessWrapper::create();
	

	ConfigFile config_file(config_file_access);
	config_file.set_path( CONFIG_FILE_PATH );
/*
	SoundDriverSDL *sound_driver_sdl = new SoundDriverSDL;	
	SoundDriverManager::register_driver( sound_driver_sdl );
	
*/
	
	if (SDL_Init(SDL_INIT_VIDEO|SDL_INIT_TIMER) != 0) {
		printf("Unable to initialize SDL: %s\n", SDL_GetError());
		return 1;
	}
 
	SDL_EnableUNICODE(1);

	const SDL_VideoInfo *vinfo=SDL_GetVideoInfo();
	Uint32 flags =SDL_SWSURFACE | SDL_RESIZABLE;
	SDL_Surface *screen = SDL_SetVideoMode  (DEFAULT_W, DEFAULT_H,vinfo->vfmt->BitsPerPixel , flags);
	
	PainterSDL *painter = new PainterSDL(screen);
	
	TimerSDL *timer = new TimerSDL;
	
	ScrollBar::set_can_focus_by_default( false );
		
	CSkin skin;
	Editor * window = new Editor( painter, timer, &skin,&config_file );
	window->set_size( GUI::Size( DEFAULT_W,DEFAULT_H ) );
/*
	interface->get_mixer_dialog()->set_mixer( &mixer_software );
	interface->get_ui_settings_dialog()->set_theme_path( CONFIG_DIR_SKINS );
	interface->get_ui_settings_dialog()->set_system_theme_path( CONFIG_SYSTEM_SKINS );
*/
	
	/*
	HBoxContainer * hbc = new HBoxContainer;
	
	window->set_root_frame( hbc );


	PatternScreen *ps=hbc->add( new PatternScreen(tracker.editor),1 );
	
	*/	
	window->set_size( GUI::Size( screen->w , screen->h ) );
	
	window->redraw_all();
	window->update();
	
	/* LOAD THING */

	
/*
	if (argc>1) {

		interface->open_song(argv[1]);
	}
*/
	
/*
	if (config_file.load()) { //coundl't load settings
		
		SoundDriverManager::init_driver(); //init default
		//use as default theme
		interface->get_ui_settings_dialog()->load_theme("Chibiness"); 
		interface->show_splash();
	}
*/
	GUI::KeycodesSDL keycode_sdl_translator;
	
	bool done=false;
	
	SDL_Event event;
	Uint32 last_click_tick=0;
	bool can_dblclick=false;
	while(!done) {
	
	
		if (SDL_WaitEvent(&event)==0)
			continue;
			
		do {
		
			switch(event.type) {
				//case SDL_USEREVENT:
			//		HandleUserEvents(&event);
		//			break;
				case SDL_VIDEORESIZE:
					printf("??\n");	
		
					screen = SDL_SetVideoMode  (event.resize.w,event.resize.h, 32, flags);
					window->set_size( GUI::Size(event.resize.w,event.resize.h ) );
					window->redraw_all();
					window->update();
					
					break;
				case SDL_KEYUP:
				case SDL_KEYDOWN: {
			// Handle any key presses here.
			
					unsigned int mod=0;
					
					if (  event.key.keysym.mod & (KMOD_LSHIFT|KMOD_RSHIFT))
						mod|=GUI::KEY_MASK_SHIFT;
					
					if (  event.key.keysym.mod & (KMOD_LALT|KMOD_RALT))
						mod|=GUI::KEY_MASK_ALT;
					
					if (  event.key.keysym.mod & (KMOD_LCTRL|KMOD_RCTRL))
						mod|=GUI::KEY_MASK_CTRL;
									
	
					if (  event.key.keysym.mod & (KMOD_LMETA|KMOD_RMETA)) {
	#ifdef META_AS_ALT					
						mod|=GUI::KEY_MASK_ALT;
	#else
						
						mod|=GUI::KEY_MASK_META;
	#endif
					}
					
					window->key( event.key.keysym.unicode, keycode_sdl_translator.get_code(event.key.keysym.sym), event.key.state==SDL_PRESSED,false, mod );
					
					
					/*if (event.key.keysym.sym==SDLK_F1) {
	
						painter->draw_fill_rect( GUI::Point() , GUI::Size( screen->w , screen->h ), GUI::Color(100,20,33) );
						SDL_UpdateRect(screen, 0,0,0,0);
					}*/
	
					//if (event.key.keysym.sym==SDLK_F2) {
	
					//painter->update_screen();
					//}
	
					/*if ((event.key.keysym.mod&KMOD_LSHIFT) && event.key.keysym.sym==SDLK_F12) {
						
						printf("DeadLocking Audio Thread on Purpose! (shift-f12)!!\n");
						SDL_LockAudio();
					}*/
					
	//				printf("Pressed Key %s, unicode %i\n",GUI::Keyboard::get_code_name( keycode_sdl_translator.get_code(event.key.keysym.sym)).ascii().get_data(),event.key.keysym.unicode);
				} break;
	
				case SDL_MOUSEBUTTONUP:
				case SDL_MOUSEBUTTONDOWN: {
					
					
					Uint32 last_click_delta=0;
					
					
					if (event.button.type==SDL_MOUSEBUTTONDOWN && event.button.button==BUTTON_LEFT) {
						
						last_click_delta=SDL_GetTicks()-last_click_tick;
						last_click_tick=SDL_GetTicks();
					}
					
																
					SDLMod	mod_state=SDL_GetModState();
	
					unsigned int mod=0;
					
					if (  mod_state & (KMOD_LSHIFT|KMOD_RSHIFT))
						mod|=GUI::KEY_MASK_SHIFT;
					
					if (  mod_state & (KMOD_LALT|KMOD_RALT))
						mod|=GUI::KEY_MASK_ALT;
					
					if (  mod_state & (KMOD_LCTRL|KMOD_RCTRL))
						mod|=GUI::KEY_MASK_CTRL;
									
					if (  mod_state & (KMOD_LMETA|KMOD_RMETA))
						mod|=GUI::KEY_MASK_META;
					
					
					window->mouse_button( GUI::Point( event.button.x, event.button.y ), event.button.button, event.button.type==SDL_MOUSEBUTTONDOWN, mod );
					
					if (can_dblclick && last_click_delta<250 && event.button.type==SDL_MOUSEBUTTONDOWN && event.button.button==BUTTON_LEFT) {
						window->mouse_doubleclick( GUI::Point( event.button.x, event.button.y ), mod );
						can_dblclick=false;
	
					} else
						can_dblclick=true;
											
				} break;
					
				case SDL_MOUSEMOTION:
	
					can_dblclick=false; //can't doubleclick! wah wah wah
					/* Motion compensation, in case there are MANY motion events pending */					
					window->mouse_motion( GUI::Point( event.motion.x, event.motion.y ), GUI::Point( event.motion.xrel, event.motion.yrel ), event.motion.state );
					break;
					
				case SDL_USEREVENT: {
					
					if (event.user.code!=TimerSDL::SDL_TIMER_EVENT_CODE)
						break;
					
					TimerID *timer_id=(TimerID*)event.user.data1;
					timer->call(*timer_id);
				} break; 
				case SDL_QUIT:
					window->quit_request();
					break;
	
				default: {}
					break;
	
			}   // End switch
            	} while ( SDL_PollEvent(&event) );
  

//		printf("check updates! - %i\n",i++);
		timer->loop_iterate();
		window->check_for_updates();
		//SDL_UpdateRect   (screen, 0, 0, 0, 0);
		
		done=window->must_quit();
		
#ifdef FB_PAINTER
		
		SDL_UpdateRect(screen,0,0,0,0);
#endif
	}   // End while
        
//	config_file.save();
	
//	interface->get_tracker()->player->play_stop();
	/* stop stuff that may have to do with interface */
//	SoundDriverManager::finish_driver(); //finish any active driver, make sure no callbacks
//	timer->stop_all();
	
	delete window;
	
	//delete SDL stuff
	delete timer;
	delete painter;
	// delete config
/*	delete config_file_access;*/
	//quit!
	SDL_Quit();
	return 0;
}
