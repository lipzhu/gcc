/****************************************************************************
 *                                                                          *
 *                         GNAT COMPILER COMPONENTS                         *
 *                                                                          *
 *                          V X _ C R T B E G I N                           *
 *                                                                          *
 *                          C Implementation File                           *
 *                                                                          *
 *              Copyright (C) 2016, Free Software Foundation, Inc.          *
 *                                                                          *
 * GNAT is free software;  you can  redistribute it  and/or modify it under *
 * terms of the  GNU General Public License as published  by the Free Soft- *
 * ware  Foundation;  either version 3,  or (at your option) any later ver- *
 * sion.  GNAT is distributed in the hope that it will be useful, but WITH- *
 * OUT ANY WARRANTY;  without even the  implied warranty of MERCHANTABILITY *
 * or FITNESS FOR A PARTICULAR PURPOSE.                                     *
 *                                                                          *
 * As a special exception under Section 7 of GPL version 3, you are granted *
 * additional permissions described in the GCC Runtime Library Exception,   *
 * version 3.1, as published by the Free Software Foundation.               *
 *                                                                          *
 * You should have received a copy of the GNU General Public License and    *
 * a copy of the GCC Runtime Library Exception along with this program;     *
 * see the files COPYING3 and COPYING.RUNTIME respectively.  If not, see    *
 * <http://www.gnu.org/licenses/>.                                          *
 *                                                                          *
 * GNAT was originally developed  by the GNAT team at  New York University. *
 * Extensive contributions were provided by Ada Core Technologies Inc.      *
 *                                                                          *
 ****************************************************************************/

/* crtbegin kind of file for ehframe registration/deregistration
   purposes on VxWorks.  This variant provides _ctors and _dtors
   arrays that the kernel module loader knows to process when it has
   been configured for this purpose (c++ constructor strategy set to
   automatic).  The ctor/dtor functions need not be visible in this
   case.  */

#define CDTOR_VISIBILITY static
#include "vx_crtbegin.inc"

typedef void (*func_ptr) (void);
func_ptr _dtors [] = {DTOR_NAME, 0};
func_ptr _ctors [] = {CTOR_NAME, 0};
