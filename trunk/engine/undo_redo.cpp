//
// C++ Implementation: undo_redo
//
// Description: 
//
//
// Author: Juan Linietsky <reduzio@gmail.com>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "undo_redo.h"

void UndoRedo::begin_group(String p_name) {


}
void UndoRedo::end_group() {


}

void UndoRedo::add_action(String p_description,CommandBase *p_do_method, CommandBase *p_undo_method) {

	p_do_method->call();

}

void UndoRedo::clean() {

}

UndoRedo::UndoRedo()
{
}


UndoRedo::~UndoRedo()
{
}


