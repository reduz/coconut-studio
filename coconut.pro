# -------------------------------------------------
# Project created by QtCreator 2010-02-10T00:03:53
# -------------------------------------------------
TARGET = coconut
TEMPLATE = app
INCLUDEPATH += "globals"
INCLUDEPATH += "dsp"
CONFIG += link_pkgconfig
PKGCONFIG += jack
SOURCES += bin/coconut.cpp \
    gui_main/main_window.cpp \
    gui_main/song_window.cpp \
    gui_main/pattern_editor.cpp \
    gui_main/order_editor.cpp \
    globals/tree_saver.cpp \
    globals/tree_loader.cpp \
    globals/rstring.cpp \
    engine/track.cpp \
    engine/audio_effect.cpp \
    engine/song.cpp \
    engine/audio_lock.cpp \
    engine/pattern_track.cpp \
    engine/edit_commands.cpp \
    engine/undo_redo.cpp
HEADERS += gui_main/main_window.h \
    gui_main/song_window.h \
    gui_main/pattern_editor.h \
    gui_main/order_editor.h \
    globals/typedefs.h \
    globals/tree_saver.h \
    globals/tree_loader.h \
    globals/rstring.h \
    globals/error_macros.h \
    globals/error_list.h \
    dsp/frame.h \
    dsp/event.h \
    engine/track.h \
    engine/audio_effect.h \
    engine/song.h \
    globals/value_stream.h \
    globals/vector.h \
    globals/map.h \
    engine/audio_lock.h \
    engine/pattern_track.h \
    engine/edit_commands.h \
    engine/undo_redo.h \
    globals/list.h \
    globals/simple_type.h
FORMS += gui_main/main_window.ui \
    gui_main/song_window.ui
RESOURCES += gui_main/resources/icons.qrc
