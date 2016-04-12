/*
 * BTree.cc - implementation of methods of class BTree as declared in BTree.h.
 *
 * Several of the methods in this file must be implemented by students for
 * cs222 project 4
 *
 * If this file is compiled with the symbol PROFESSOR_VERSION defined,
 * it expects a file of that name to exist, and uses the code from
 * that file in place of selected dummy methods left to be written by students.
 *
 * Copyright (c) 2001, 2002, 2003 - Russell C. Bjork
 * Trivial changes 2016 - Russ Tuck
 */

#include <iostream>
using std::cout;

#include "BTree.h"
#include "BTreeFile.h"
#include "BTreeBlock.h"

bool debug = false;

BTree::BTree(string name)
  : _file(*new BTreeFile(name))
{
}


#ifndef PROFESSOR_VERSION

void BTree::insert(string key, string value)
{
  // Student code goes here - remove this line
}


bool BTree::lookup(string key, string& value) const
{
  BTreeFile::BlockNumber root = _file.getRoot();

  if (root == 0)
  {
    return false;
  }
  else
  {
    if (debug)
    {
      cout << endl;
      cout << "value " << value << endl;
    }
    value = "";
    return BTree::rlookup(key, root, value);
  }
}


bool BTree::rlookup(string key, BTreeFile::BlockNumber root, string& value) const
{
  BTreeBlock block;
  BTreeFile::BlockNumber child;

  //Get block
  _file.getBlock(root, block);

  unsigned numberOfKeys = block.getNumberOfKeys();

  //Determine index
  unsigned index = numberOfKeys - 1;

  //Total number of children count start at 0
  unsigned numberOfChildren = index - 1;

  //Current pos
  unsigned currentPos = block.getPosition(key);

  string currentKey = block.getKey(currentPos);

  if (debug)
  {
    cout << endl;

    cout << "Position " << block.getPosition(key) << endl;

    cout << "number of keys" << block.getNumberOfKeys() << endl;

    cout << "Get child 1 " << block.getChild(0) << endl;

    cout << "Get child 2 " << block.getChild(1) << endl;

    cout << "block number" << root << endl;

    cout << "key " << key << endl;
  }


  if (key == currentKey)
  {
    if (debug)
    {
      cout << "IF|key == currentKey" << endl;
    }
    value = block.getValue(currentPos);
    return true;
  }
  else if ((currentPos <= numberOfKeys) && !block.isLeaf())
  {
    if (debug)
    {
      cout << "ELSE IF|currentPos <= numberOfKeys" << endl;
    }
    BTree::rlookup(key, block.getChild(currentPos), value);
  }
  else
  {
    if (debug)
    {
      cout << "ELSE" << endl;
    }
    return false;
  }
}


bool BTree::remove(string key)
{
  return false;       // Student code goes here - remove this line
}


#else

#define QUOTE(Q)           # Q
#define INCLUDE_NAME(X)    QUOTE(X)
#include INCLUDE_NAME(PROFESSOR_VERSION)
#endif

void BTree::print() const
{
  cout << "BTree in file ";
  _file.printHeaderInfo();
  cout << endl;

  BTreeFile::BlockNumber root = _file.getRoot();
  if (root == 0)
  {
    cout << "Empty tree" << endl;
  }
  else
  {
    _file.printBlock(root, true, 1);
  }
}


void BTree::print(BTreeFile::BlockNumber blockNumber) const
{
  _file.printBlock(blockNumber, false, 1);
}


BTree::~BTree()
{
  delete (&_file);
}
