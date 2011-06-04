/*
 * Copyright (c) 2008 James Molloy, Jörg Pfähler, Matthew Iselin
 *
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

/* This file was originally written by the copyright holder James Molloy
   as part of the Pedigree project (http://www.pedigree-project.org).

It was originally written as part of the in-kernel debugger, so its design
is centred around requiring *no dynamic memory* whatsoever (hence the "StaticString") */

#ifndef DEMANGLE_H
#define DEMANGLE_H

#include <StaticString.h>

typedef struct symbol
{
  symbol() :
    name(),
    nParams(0)
  {}
    LargeStaticString name; // Function name.
    LargeStaticString params[32]; // Parameters.
  size_t nParams;
  bool is_ctor;
} symbol_t;

void demangle(LargeStaticString src, symbol_t *sym);
void demangle_full(LargeStaticString src, LargeStaticString &dest);

#endif
