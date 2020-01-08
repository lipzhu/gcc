/* brig-util.h -- gccbrig utility functions
   Copyright (C) 2016-2020 Free Software Foundation, Inc.
   Contributed by Pekka Jaaskelainen <pekka.jaaskelainen@parmance.com>
   for General Processor Tech.

This file is part of GCC.

GCC is free software; you can redistribute it and/or modify it under
the terms of the GNU General Public License as published by the Free
Software Foundation; either version 3, or (at your option) any later
version.

GCC is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or
FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
for more details.

You should have received a copy of the GNU General Public License
along with GCC; see the file COPYING3.  If not see
<http://www.gnu.org/licenses/>.  */

#ifndef GCC_BRIG_UTIL_H
#define GCC_BRIG_UTIL_H

#include <map>
#include <vector>

#include "config.h"
#include "system.h"
#include "ansidecl.h"
#include "coretypes.h"
#include "opts.h"
#include "tree.h"

/* There are 128 c regs and 2048 s/d/q regs each in the HSAIL.  */
#define BRIG_2_TREE_HSAIL_C_REG_COUNT (128)
#define BRIG_2_TREE_HSAIL_S_REG_COUNT (2048)
#define BRIG_2_TREE_HSAIL_D_REG_COUNT (2048)
#define BRIG_2_TREE_HSAIL_Q_REG_COUNT (2048)
#define BRIG_2_TREE_HSAIL_TOTAL_REG_COUNT				\
  (BRIG_2_TREE_HSAIL_C_REG_COUNT + BRIG_2_TREE_HSAIL_S_REG_COUNT	\
   + BRIG_2_TREE_HSAIL_D_REG_COUNT + BRIG_2_TREE_HSAIL_Q_REG_COUNT)

/* Helper class for keeping book of group variable offsets.  */

class group_variable_offset_index
{
public:
  group_variable_offset_index () : m_next_group_offset (0) {}

  typedef std::map<std::string, size_t> varname_offset_table;

  bool has_variable (const std::string &name) const;
  void add (const std::string &name, size_t size, size_t alignment);
  size_t segment_offset (const std::string &name) const;
  size_t size () const { return m_next_group_offset; }

private:
  size_t m_next_group_offset;
  varname_offset_table m_group_offsets;
};

bool gccbrig_hsa_opcode_op_output_p (BrigOpcode16_t opcode, int opnum);

unsigned gccbrig_hsa_type_bit_size (BrigType16_t t);

uint64_t gccbrig_to_uint64_t (const BrigUInt64 &brig_type);

int gccbrig_reg_size (const BrigOperandRegister *brig_reg);

std::string gccbrig_reg_name (const BrigOperandRegister *reg);

std::string gccbrig_type_name (BrigType16_t type);

std::string gccbrig_segment_name (BrigSegment8_t segment);

bool gccbrig_is_float_type (BrigType16_t type);

bool gccbrig_is_bit_operation (BrigOpcode16_t opcode);

BrigType16_t gccbrig_tree_type_to_hsa_type (tree tree_type);
tree gccbrig_tree_type_for_hsa_type (BrigType16_t brig_type);

bool gccbrig_might_be_host_defined_var_p (const BrigDirectiveVariable *brigVar);

/* From hsa.h.  */
bool hsa_type_packed_p (BrigType16_t type);

struct reg_use_info
{
  /* This vector keeps count of the times an HSAIL register is used as
     a tree type in generic expressions.  The count is used to select
     type for 'register' variables to reduce emission of
     VIEW_CONVERT_EXPR nodes.  The data is kept in vector (insertion
     order) for determinism, in a case there is a tie with the
     counts.  */
  std::vector<std::pair<tree, size_t> > m_type_refs;
  /* Tree to index.  Lookup for the above vector.  */
  std::map<tree, size_t> m_type_refs_lookup;
};

/* key = hsa register entry generated by gccbrig_hsa_reg_id ().  */
typedef std::map<size_t, reg_use_info> regs_use_index;

size_t gccbrig_hsa_reg_id (const BrigOperandRegister &reg);
std::string gccbrig_hsa_reg_name_from_id (size_t reg_hash);

void gccbrig_print_reg_use_info (FILE *dump, const regs_use_index &info);

/* Return the number of elements in a VECTOR_TYPE.  BRIG does not support
   variable-length vectors.  */
inline unsigned HOST_WIDE_INT
gccbrig_type_vector_subparts (const_tree type)
{
  return TYPE_VECTOR_SUBPARTS (type).to_constant ();
}

#endif
