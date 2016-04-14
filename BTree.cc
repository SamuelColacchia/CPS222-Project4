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
#include <stack>
#include <iostream>
using std::cout;
using std::stack;

#include "BTree.h"
#include "BTreeFile.h"
#include "BTreeBlock.h"


//TODO add comments
bool debug = true;

BTree::BTree(string name)
  : _file(*new BTreeFile(name))
{
}


#ifndef PROFESSOR_VERSION

void BTree::insert(string key, string value)
{
  //Create initial variables
  stack<BTreeFile::BlockNumber> blockNumberStack;

  BTreeFile::BlockNumber rootNum = _file.getRoot();

  BTreeFile::BlockNumber currentBlockNum;
  BTreeBlock currentBlock;

  if (rootNum == 0)
  {
    rootNum = _file.allocateBlock();
    _file.setRoot(rootNum);
    currentBlock.insert(currentBlock.getPosition(key), key, value, 0);
    currentBlock.setChild(currentBlock.getPosition(key), 0);
    _file.putBlock(rootNum, currentBlock);
  }
  else
  {
    currentBlockNum = rootNum;
    _file.getBlock(currentBlockNum, currentBlock);

    if (currentBlock.isLeaf())
    {
      BTree::insertR(key, value, currentBlock, currentBlockNum, 0, 0);
    }

    while (!currentBlock.isLeaf())
    {
      currentBlockNum = currentBlock.getChild(currentBlock.getPosition(key));
      _file.getBlock(currentBlockNum, currentBlock);
      if (currentBlock.isLeaf())
      {
        BTree::insertR(key, value, currentBlock, currentBlockNum, 0, 0);
      }
    }
  }
}


void BTree::insertR(string key, string value, BTreeBlock currentBlock, BTreeFile::BlockNumber currentBlockNumber, BTreeFile::BlockNumber leftChildBlockNumber, BTreeFile::BlockNumber rightChildBlockNumber)
{
  BTreeFile::BlockNumber rootNum = _file.getRoot();



  BTreeFile::BlockNumber newRightBlockNum;
  BTreeBlock newRightBlock;

  BTreeFile::BlockNumber newParentNum;
  BTreeBlock newParent;




  //Case | root only block in tree
  if ((currentBlockNumber == rootNum) && currentBlock.splitNeeded())
  {

    unsigned midIndex = (currentBlock.getNumberOfKeys() - 1) / 2;
    string midKey = currentBlock.getKey(midIndex);
    string midValue = currentBlock.getValue(midIndex);

    currentBlock.split(midKey, midValue, newRightBlock);

    newRightBlockNum = _file.allocateBlock();
    newParentNum = _file.allocateBlock();

    _file.setRoot(newParentNum);

    _file.putBlock(currentBlockNumber, currentBlock);
    _file.putBlock(newRightBlockNum, newRightBlock);

    BTree::insertR(key, value, newParent, newParentNum, currentBlockNumber, newRightBlockNum);
  }

  //Case | Not root
  else if ((currentBlockNumber != rootNum) && currentBlock.splitNeeded())
  {
    cout << endl;
    //perform a regular split
  }

  //Case | No split needed
  else
  {
    if (leftChildBlockNumber != 0)
    {
      cout << "rightChildBlockNumber:" << rightChildBlockNumber << endl;
      cout << "rightChildBlockNumber:" << leftChildBlockNumber << endl;
      currentBlock.insert(currentBlock.getPosition(key), key, value, rightChildBlockNumber);
      currentBlock.setChild(currentBlock.getPosition(key), leftChildBlockNumber);

      cout << "currentBlock.splitNeeded():" << currentBlock.splitNeeded() << endl;
      if (currentBlock.splitNeeded())
      {
        BTree::insertR(key, value, currentBlock, currentBlockNumber, 0, 0);
      }
      else
      {
        _file.putBlock(currentBlockNumber, currentBlock);
      }
    }
    else
    {
      //just insert
      currentBlock.insert(currentBlock.getPosition(key), key, value, 0);
      currentBlock.setChild(currentBlock.getPosition(key), 0);

      if (currentBlock.splitNeeded())
      {
        cout << "insert" << endl;
        BTree::insertR(key, value, currentBlock, currentBlockNumber, 0, 0);
      }
      else
      {
        _file.putBlock(currentBlockNumber, currentBlock);
      }
    }
  }
}


BTreeFile::BlockNumber BTree::getParent(BTreeFile::BlockNumber child, string key)
{
  BTreeFile::BlockNumber rootNum = _file.getRoot();

  BTreeFile::BlockNumber currentBlockNum = rootNum;
  BTreeBlock currentBlock;

  BTreeFile::BlockNumber peackBlockNum = rootNum;
  BTreeBlock peackBlock;

  _file.getBlock(currentBlockNum, currentBlock);

  peackBlockNum = currentBlock.getChild(currentBlock.getPosition(key));
  _file.getBlock(peackBlockNum, peackBlock);

  while (peackBlockNum != child)
  {
    currentBlockNum = currentBlock.getChild(currentBlock.getPosition(key));
    _file.getBlock(currentBlockNum, currentBlock);

    peackBlockNum = currentBlock.getChild(currentBlock.getPosition(key));
    _file.getBlock(peackBlockNum, peackBlock);

    if (peackBlockNum == child)
    {
      return currentBlockNum;
    }
    else if (child == rootNum)
    {
      return 0;
    }
  }
  return 0;
}


//TODO Add comments
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


//TODO add comments
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
    cout << "index " << index << endl;

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
