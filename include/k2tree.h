/*
 * ----------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE" (Revision 42):
 * <nlehmann@dcc.uchile.cl> wrote this file. As long as you retain this notice you
 * can do whatever you want with this stuff. If we meet some day, and you think
 * this stuff is worth it, you can buy me a beer in return Nicolás Lehmann
 * ----------------------------------------------------------------------------
 */

#ifndef INCLUDE_K2TREE_H_
#define INCLUDE_K2TREE_H_

#include <bits/basic_k2treebuilder.h>
#include <bits/basic_k2tree.h>
#include <k2treepartition_builder.h>
#include <k2tree_partition.h>
namespace libk2tree {

typedef basic_k2tree<unsigned int> K2Tree;
typedef basic_k2tree<size_t> LongK2Tree;
typedef basic_k2treebuilder<unsigned int> K2TreeBuilder;
typedef basic_k2treebuilder<size_t> LongK2TreeBuilder;

}  // namespace libk2tree
#endif  // INCLUDE_K2TREE_H_
