/* -*- coding: utf-8 -*- */
/* -*- mode: c -*- */
/*
 * Dislocker -- enables to read/write on BitLocker encrypted partitions under
 * Linux
 * Copyright (C) 2012-2013  Romain Coltel, Hervé Schauer Consultants
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301,
 * USA.
 */

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <termios.h>
#include <unistd.h>


#include <string.h>
#include <time.h>

#include "dislocker/xstd/xstdio.h"
#include "dislocker/xstd/xstdlib.h"

#include <Library/UefiBootServicesTableLib.h>


/* Keep track of the verbosity level */
int verbosity = L_QUIET;


/* Levels transcription into strings */
static char* msg_tab[DIS_LOGS_NB] = {
	"CRITICAL",
	"ERROR",
	"WARNING",
	"INFO",
	"DEBUG"
};


/**
 * Initialize outputs for display messages
 *
 * @param v Application verbosity
 * @param file File where putting logs (stdout if NULL)
 */
void dis_stdio_init(DIS_LOGS, const char*) {}


/**
 * Create and return an unbuffered stdin
 *
 * @return The file descriptor of the unbuffered input tty
 */
int get_input_fd()
{
	return -1;
}


/**
 * Close the unbuffered stdin if opened
 */
void close_input_fd() {}


/**
 * Endify in/outputs
 */
void dis_stdio_end() {}


/**
 * Remove the '\n', '\r' or '\r\n' before the first '\0' if present
 *
 * @param string String where the '\n', '\r' or '\r\n' is removed
 */
void chomp(char* string)
{
	size_t len = strlen(string);
	if(len == 0)
		return;

	if(string[len - 1] == '\n' || string[len - 1] == '\r')
		string[len - 1] = '\0';

	if(len == 1)
		return;

	if(string[len - 2] == '\r')
		string[len - 2] = '\0';
}

void wait_for_key();

/**
 * Do as printf(3) but displaying nothing if verbosity is not high enough
 * Messages are redirected to the log file if specified into xstdio_init()
 *
 * @param level Level of the message to print
 * @param format String to display (cf printf(3))
 * @param ... Cf printf(3)
 * @return The number of characters printed
 */
int EFIAPI dis_printf(DIS_LOGS level, const char* format, ...)
{
	int ret = -1;

	if(verbosity < level || verbosity <= L_QUIET)
		return 0;

	if(level >= DIS_LOGS_NB)
		level = L_DEBUG;


	VA_LIST arg;
	VA_START(arg, format);

	ret = dis_vprintf(level, format, arg);

	VA_END(arg);

	wait_for_key();

	return ret;
}

UINTN
AsciiInternalPrint (
  IN  CONST CHAR8                      *Format,
  IN  EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL  *Console,
  IN  VA_LIST                          Marker
  );


/**
 * Do as vprintf(3) but displaying nothing if verbosity is not high enough
 * Messages are redirected to the log file if specified into xstdio_init()
 *
 * @param level Level of the message to print
 * @param format String to display (cf vprintf(3))
 * @param ap Cf vprintf(3)
 */
int EFIAPI dis_vprintf(DIS_LOGS level, const char* format, VA_LIST ap)
{
	if(verbosity < level || verbosity <= L_QUIET)
		return 0;

	if(level >= DIS_LOGS_NB)
		level = L_DEBUG;


	AsciiPrint(msg_tab[level]);
	AsciiPrint(": ");
	AsciiInternalPrint(format, gST->ConOut, ap);
	return 0;
}
