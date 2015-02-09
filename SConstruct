
EnsureSConsVersion(0,14);


import os;
import string;
import sys

win_enabled=False

if (os.name=="nt"):
	wind32=True
	if (os.getenv("MINGW_PREFIX")!=None):
		env = Environment(CPPPATH=['#/globals','#gui','#.'],CPPFLAGS=['-DANSIC_LIBS_ENABLED','-DWINDOWS_ENABLED','-DSDL_ENABLED'],ENV=os.environ,tools=['mingw']);
		env.Append(CPPFLAGS=['-I'+os.getenv("MINGW_PREFIX")+'\\include\\SDL']);
		env.Append(LIBS=['mingw32','SDLmain','SDL']);
		env.icon_obj="cticon.o"
		env["DEBUG_CXXFLAGS"]=['-g3','-Wall']
		env["DEBUG_LINKFLAGS"]=[]
		env["RELEASE_CXXFLAGS"]=['-O2','-ffast-math']
		env["RELEASE_LINKFLAGS"]=['-Wl,--subsystem,windows']
		env["PROFILE_CXXFLAGS"]=['-pg','-g3','-Wall']
		env["PROFILE_LINKFLAGS"]=['-pg']
		
	else:
		env = Environment(CPPPATH=['#/globals','#gui','#.'],CPPFLAGS=['/DANSIC_LIBS_ENABLED','/DWINDOWS_ENABLED'],ENV=os.environ);
#		env.Append(CPPFLAGS=['/IC:\Program Files\Microsoft Visual Studio 8\VC\include\SDL','/D_REENTRANT','/DWINDOWS_ENABLED','/Zi','/Yd','/MT','/Gd']);
#	        env.Append(LIBS=['SDLmain','SDL']);
#		env.Append(LINKFLAGS=['/FORCE:MULTIPLE','/DEBUG']);
#		env["DEBUG_CXXFLAGS"]=['-g3','-Wall']
#		env["DEBUG_LINKFLAGS"]=[]
#		env["RELEASE_CXXFLAGS"]=['-O2','-ffast-math']
#		env["RELEASE_LINKFLAGS"]=[]
#		env["PROFILE_CXXFLAGS"]=['-pg','-g3','-Wall']
#		env["PROFILE_LINKFLAGS"]=['-pg']
		
		env.icon_obj=""
	
	win_enabled=True		
else:
	#unix

	env = Environment(CPPPATH=['#/globals','#gui','#.'],CPPFLAGS=['-DANSIC_LIBS_ENABLED'],ENV=os.environ);
	errorval=os.system("sdl-config --version");

        if (errorval):
                print "Error: cant execute sdl-config, make sure SDL is installed";
                exit(255);
	else:
		print("libSDL Detected");

	env.ParseConfig('sdl-config --cflags --libs')

	env.Append(CPPFLAGS=['-DPOSIX_ENABLED','-DSDL_ENABLED','-fno-exceptions']);
	
	errorval=os.system("pkg-config alsa --modversion");

        if (True or errorval):
                print "ALSA not detected.";
	else:
	 	print "ALSA detected.";
		env.ParseConfig('pkg-config alsa --cflags --libs')
		env.Append(CPPFLAGS=['-DALSA_ENABLED']);	

	env["DEBUG_CXXFLAGS"]=['-g3','-Wall']
	env["DEBUG_LINKFLAGS"]=[]
	env["RELEASE_CXXFLAGS"]=['-O2','-ffast-math']
	env["RELEASE_LINKFLAGS"]=[]
	env["PROFILE_CXXFLAGS"]=['-pg','-g3','-Wall']
	env["PROFILE_LINKFLAGS"]=['-pg']
	env.icon_obj=""


def add_sources(self, sources, filetype, lib_env = None, shared = False):
	import glob;
	import string;
	#if not lib_objects:
	if not lib_env:
		lib_env = self
	if type(filetype) == type(""):

		dir = self.Dir('.').abspath
		list = glob.glob(dir + "/"+filetype)
		for f in list:
			sources.append( self.Object(f) )
	else:
		for f in filetype:
			sources.append(self.Object(f))
			
		
env.__class__.add_sources=add_sources

opts=Options()
opts.Add('target', 'Target: (debug/profile/release).', "debug")
opts.Add('prefix', 'The installation prefix', '/usr/local/')
opts.Add('meta_as_alt', 'META Key works as ALT (MAC keyboards)', 0)

env.Append(CXXFLAGS=['-DGUI_EXTERNAL_STRING="\\"globals/rstring.h\\""'])
opts.Update(env)
Help(opts.GenerateHelpText(env))
	


if (env['target']=='debug'):
        env.Append(CXXFLAGS=env["DEBUG_CXXFLAGS"]);
        env.Append(LINKFLAGS=env["DEBUG_LINKFLAGS"]);
	
if (env['target']=='profile'):
        env.Append(CXXFLAGS=env["PROFILE_CXXFLAGS"]);
        env.Append(LINKFLAGS=env["PROFILE_LINKFLAGS"]);

if (env['target']=='release'):
        env.Append(CXXFLAGS=env["RELEASE_CXXFLAGS"]);
        env.Append(LINKFLAGS=env["RELEASE_LINKFLAGS"]);

if ((env['meta_as_alt'])):
	env.Append(CXXFLAGS=['-DMETA_AS_ALT']);

env.bin_targets=[]
Export('env');


SConscript('drivers/SCsub');
SConscript('editor/SCsub');
SConscript('engine/SCsub');
SConscript('gui/SCsub');
SConscript('globals/SCsub');
#SConscript('song/SCsub');
#SConscript('mixer/SCsub');
#SConscript('tracker/SCsub');
#SConscript('interface/SCsub');

SConscript('bin/SCsub');

if env['prefix']:
        dst_target = env.Install(env['prefix']+'/bin', env.bin_targets)
        env.Alias('install', dst_target )

