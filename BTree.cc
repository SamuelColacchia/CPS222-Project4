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

BTree::BTree(string name)
  : _file(*new BTreeFile(name))
{
}


#ifndef PROFESSOR_VERSION

void BTree::insert(string key, string value)
{
  if (value == 0)
  {
    cout << "/* search fails */" << endl;
  }


  else
  {
    BTreeFile::BlockNumber root = _file.getRoot();

 
    BTreeBlock currentBlock;

    _file.getBlock(root, currentBlock);

    while (!isLeaf(currentBlock))
    {
      _file.getBlock(getChild(currentBlock.getPosition(key)), currentBlock);
    }

    BTreeBlock::insert(currentBlock.getPosition(key), key, value, currentBlock)

    if (splitNeeded(currentBlock) == false)
    {
      putBlock(currentBlock);
    }
    else
    {
      BTreeBlock.BTreeBlock();
      BTreeBlock.split(string & promotedKey, string & promotedValue,
                       BTreeBlock & rightHalf);
      BTreeFile.putBlock();
      BTreeFile.allocateBlock();
      BTreeFile.putBlock();
      insertR(key, value, currentBlock);
    }
  }
}


void BTree::insertR(string key, string value, editedBlock)
{
  int parent = getRoot();
  int currentBlock = editedBlock;

  while (currentBlock != getChild(root))
  {
    int currentBlock = getChild(root);
  }
  BTreeBlock.insert(getPosition(key), key, value, BTreeFile.getChild(currentBlock))
  if (splitNeeded(currentBlock) == false)
  {
    putBlock(currentBlock);
  }
  else
  {
    BTreeBlock();
    split();
    putBlock();
    allocateBlock();
    putBlock();
    insertR(key, value, currentBlock);
  }
}


bool BTree::lookup(string key, string& value) const
{
  return false;       // Student code goes here - remove this line
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
