/*
This file is part of Spindle.  For copyright information see the COPYRIGHT 
file in the top level directory, or at 
https://github.com/hpc/Spindle/blob/master/COPYRIGHT

This program is free software; you can redistribute it and/or modify it under
the terms of the GNU General Public License (as published by the Free Software
Foundation) version 2.1 dated February 1999.  This program is distributed in the
hope that it will be useful, but WITHOUT ANY WARRANTY; without even the IMPLIED
WARRANTY OF MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the terms 
and conditions of the GNU General Public License for more details.  You should 
have received a copy of the GNU Lesser General Public License along with this 
program; if not, write to the Free Software Foundation, Inc., 59 Temple
Place, Suite 330, Boston, MA 02111-1307 USA
*/

#include <unistd.h>
#include <stdlib.h>
#include "spindle_debug.h"

static void setupLogging();
static void parseCommandLine(int argc, char *argv[]);
extern int startLaunchmonBE(int argc, char *argv[]);
extern int startSerialBE(int argc, char *argv[]);
enum startup_type_t {
   lmon,
   serial
};
startup_type_t startup_type;

int main(int argc, char *argv[])
{
   int  result;
   setupLogging();

   debug_printf("Spindle Server Cmdline: ");
   for (int i=0; i<argc; i++) {
      bare_printf("%s ", argv[i]);
   }
   bare_printf("\n");

   parseCommandLine(argc, argv);
   
   switch (startup_type) {
      case lmon:
         result = startLaunchmonBE(argc, argv);
         break;
      case serial:
         result = startSerialBE(argc, argv);
         break;
      default:
         err_printf("Unknown startup mode\n");
         result = -1;
         break;
   }

   LOGGING_FINI;
   return result;
}

static void setupLogging()
{
   LOGGING_INIT(const_cast<char *>("Server"));
   if (!spindle_debug_output_f)
      return;

   int fd = fileno(spindle_debug_output_f);
   if (fd == -1)
      return;

   close(1);
   close(2);
   dup2(fd, 1);
   dup2(fd, 2);
}

static void parseCommandLine(int argc, char *argv[])
{
   int i;
   for (i = 0; i < argc; i++) {
      if (strcmp(argv[i], "--spindle_lmon") == 0) {
         startup_type = lmon;
         return;
      }
      else if (strcmp(argv[i], "--spindle_serial") == 0) {
         startup_type = serial;
         return;
      }
   }
   
   err_printf("Could not parse command line\n");
   fprintf(stderr, "Error, spindle_be must be launched via spindle\n");
   exit(-1);      
}