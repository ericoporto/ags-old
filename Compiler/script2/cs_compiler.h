/*
// 'C'-style script compiler
// Copyright (C) 1999-2011 Chris Jones and 2011-20xx others
// The full list of copyright holders can be found in the Copyright.txt
// file, which is part of this source code distribution.
//
// The AGS source code is provided under the Artistic License 2.0.
// A copy of this license can be found in the file License.txt and at
// http://www.opensource.org/licenses/artistic-license-2.0.php
*/

#ifndef __CS_COMPILER2_H
#define __CS_COMPILER2_H

#include "script/cc_script.h"
#include "cs_message_handler.h"

// compile the script supplied, returns nullptr on failure
// cc_error() gets called.
extern ccScript *ccCompileText2(std::string const &script, std::string const &scriptName, long options, MessageHandler &mh);

#endif // __CS_COMPILER2_H
