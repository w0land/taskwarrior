////////////////////////////////////////////////////////////////////////////////
// task - a command line task list manager.
//
// Copyright 2006 - 2009, Paul Beckingham.
// All rights reserved.
//
// This program is free software; you can redistribute it and/or modify it under
// the terms of the GNU General Public License as published by the Free Software
// Foundation; either version 2 of the License, or (at your option) any later
// version.
//
// This program is distributed in the hope that it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
// FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
// details.
//
// You should have received a copy of the GNU General Public License along with
// this program; if not, write to the
//
//     Free Software Foundation, Inc.,
//     51 Franklin Street, Fifth Floor,
//     Boston, MA
//     02110-1301
//     USA
//
////////////////////////////////////////////////////////////////////////////////

#include "Mod.h"

////////////////////////////////////////////////////////////////////////////////
Mod::Mod ()
{
}

////////////////////////////////////////////////////////////////////////////////
Mod::~Mod ()
{
}

////////////////////////////////////////////////////////////////////////////////
// before after
// not
// none any
// over under
// synth
// first last
// this
// next
// is isnt
// has hasnt
// startswith endswith
bool Mod::isValid ()
{
  if (*this == "before"     || *this == "after"    ||
      *this == "not"        ||
      *this == "none"       || *this == "any"      ||
      *this == "synth"      ||
      *this == "under"      || *this == "over"     ||
      *this == "first"      || *this == "last"     ||
      *this == "this"       ||
      *this == "next"       ||
      *this == "is"         || *this == "isnt"     ||
      *this == "has"        || *this == "hasnt"    ||
      *this == "startswith" || *this == "endswith")
    return true;

  return false;
}

////////////////////////////////////////////////////////////////////////////////
bool Mod::eval (const Mod& other)
{
  // before
  // after
  // non
  // none
  // any
  // synth
  // under
  // over
  // first
  // last
  // this
  // next

  if (*this == ".is")
    return *this == other ? true : false;

  if (*this == ".isnt")
    return *this != other ? true : false;

  // has
  // hasnt
  // startswith
  // endswith

  return false;
}

////////////////////////////////////////////////////////////////////////////////
