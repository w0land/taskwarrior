////////////////////////////////////////////////////////////////////////////////
// taskwarrior - a command line task list manager.
//
// Copyright 2006 - 2011, Paul Beckingham, Federico Hernandez.
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

#define L10N                                           // Localization complete.

#include <sstream>
#include <Context.h>
#include <Permission.h>
#include <main.h>
#include <text.h>
#include <i18n.h>
#include <CmdDone.h>

extern Context context;

////////////////////////////////////////////////////////////////////////////////
CmdDone::CmdDone ()
{
  _keyword     = "done";
  _usage       = "task done ID [tags] [attrs] [desc...]";
  _description = STRING_CMD_DONE_USAGE;
  _read_only   = false;
  _displays_id = false;
}

////////////////////////////////////////////////////////////////////////////////
int CmdDone::execute (std::string& output)
{
  int rc = 0;
  int count = 0;
  std::stringstream out;

  std::vector <Task> tasks;
  context.tdb.lock (context.config.getBoolean ("locking"));
  context.tdb.loadPending (tasks);

  // Apply filter.
  std::vector <Task> filtered;
  filter (tasks, filtered);

  if (filtered.size () == 0)
  {
    context.footnote (STRING_FEEDBACK_NO_TASKS_SP);
    return 1;
  }

  Permission permission;
  if (filtered.size () > (size_t) context.config.getInteger ("bulk"))
    permission.bigSequence ();

  bool nagged = false;
  std::vector <Task>::iterator task;
  for (task = filtered.begin (); task != filtered.end (); ++task)
  {
    if (task->getStatus () == Task::pending ||
        task->getStatus () == Task::waiting)
    {
      Task before (*task);

      // Apply the command line modifications to the new task.
      Arguments modifications = context.args.extract_modifications ();
      modify_task (*task, modifications);
      apply_defaults (*task);

      // Add an end date.
      char entryTime[16];
      sprintf (entryTime, "%u", (unsigned int) time (NULL));
      task->set ("end", entryTime);

      // Change status.
      task->setStatus (Task::completed);

      // Stop the task, if started.
      if (task->has ("start") &&
          context.config.getBoolean ("journal.time"))
        task->addAnnotation (context.config.get ("journal.time.stop.annotation"));

      // Only allow valid tasks.
      task->validate ();

      if (taskDiff (before, *task))
      {
        if (permission.confirmed (before, taskDifferences (before, *task) + STRING_CMD_DONE_PROCEED))
        {
          context.tdb.update (*task);

          if (context.config.getBoolean ("echo.command"))
            out << format (STRING_CMD_DONE_COMPLETED, task->id, task->get ("description"))
                << "\n";

          dependencyChainOnComplete (*task);
          context.footnote (onProjectChange (*task, false));

          ++count;
        }
      }

      updateRecurrenceMask (filtered, *task);
      if (!nagged)
        nagged = nag (*task);
    }
    else
    {
      out << format (STRING_CMD_DONE_NOT_PENDING, task->id, task->get ("description"))
          << "\n";
      rc = 1;
    }
  }

  if (count)
    context.tdb.commit ();

  context.tdb.unlock ();

  if (context.config.getBoolean ("echo.command"))
    if (count == 1)
      out << format (STRING_CMD_DONE_MARKED, count)
          << "\n";
    else
      out << format (STRING_CMD_DONE_MARKED_N, count)
          << "\n";

  output = out.str ();
  return rc;
}

////////////////////////////////////////////////////////////////////////////////
