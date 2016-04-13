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
  if (debug)
  {
    cout << endl;
    cout << endl;
    cout << "Debug status = " << debug << endl;
    cout << "Debug insert." << endl;
  }


  stack<BTreeFile::BlockNumber> blockNumberStack;
  BTreeFile::BlockNumber rootNum = _file.getRoot();
  BTreeFile::BlockNumber currentBlockNum;
  BTreeBlock currentBlock;
  BTreeFile::BlockNumber childNumNull = 0;

  if (rootNum == 0)
  {
    if (debug)
    {
      cout << endl;
      cout << "Root is empty" << endl;
      cout << "CurrentBlock" << currentBlock <<endl;
    }

    rootNum = _file.allocateBlock();
    _file.getBlock(rootNum, currentBlock);

    if (debug)
    {
      cout << endl;
      cout << "RootNum:" << rootNum << endl;
      cout << "CurrentBlock|after get and allocate" << currentBlock << endl;
      cout << "Get position of the key: " << currentBlock.getPosition(key) << endl;
      cout << "Header insert child value:" << currentBlock.getChild(currentBlock.getPosition(key)) << endl;

      cout << "###-Values to insert-###" << endl;
      cout << "insert num:" << currentBlock.getPosition(key) << endl;
      cout << "key:" << key << endl;
      cout << "value:" << value << endl;
      cout << "child num:" << childNumNull;
    }

    currentBlock.insert(currentBlock.getPosition(key), key, value, childNumNull);
    _file.setRoot(rootNum);
    _file.putBlock(rootNum, currentBlock);
  }
  else
  {
    if (debug)
    {
      cout << endl;
      cout << "root " << rootNum << endl;
      cout << "currentBlock.isLeaf(): " << currentBlock.isLeaf() << endl;
    }

    currentBlockNum = rootNum;
    _file.getBlock(currentBlockNum, currentBlock);

    while (!currentBlock.isLeaf())
    {
      blockNumberStack.push(currentBlockNum);
      currentBlockNum = currentBlock.getChild(currentBlock.getPosition(key));
      _file.getBlock(currentBlockNum, currentBlock);
    }

    if (debug)
    {
      cout << endl;
      cout << "Current value|at the root: " << currentBlockNum;
      cout << "Current Child value for insert: " << currentBlock.getChild(currentBlock.getPosition(key));
    }

    if (!currentBlock.isLeaf())
    {
      childNumNull = currentBlock.getChild(currentBlock.getPosition(key));
    }

    currentBlock.insert(currentBlock.getPosition(key), key, value, childNumNull);
    _file.putBlock(rootNum, currentBlock);

    while (currentBlock.splitNeeded())
    {
      BTreeBlock newBlock;
      BTreeFile::BlockNumber newBlockNum;

      unsigned midIndex = (currentBlock.getNumberOfKeys() - 1) / 2;
      string midKey = currentBlock.getKey(midIndex);
      string midValue = currentBlock.getValue(midIndex);


      _file.putBlock(currentBlockNum, currentBlock);
      newBlockNum = _file.allocateBlock();
      _file.putBlock(newBlockNum, newBlock);

      currentBlockNum = blockNumberStack.top();

      cout << "Stack top block number " << currentBlockNum << endl;


      _file.getBlock(currentBlockNum, currentBlock);

      currentBlock.insert(currentBlock.getPosition(midKey), midKey, midValue, currentBlock.getChild(currentBlock.getPosition(key)));

      blockNumberStack.pop();
    }
  }

//     currentBlock = BTreeBlock();
//     currentBlockNum = rootNum;
//     _file.getBlock(currentBlockNum, currentBlock);
//
// /*Get to the bottom of the tree*/
//     blockNumberStack.push(rootNum);
//     while (!currentBlock.isLeaf())
//     {
//       currentBlockNum = currentBlock.getChild(currentBlock.getPosition(key));
//       blockNumberStack.push(currentBlockNum);
//       _file.getBlock(currentBlockNum, currentBlock);
//     }
//
//
//     //Intial insert
//     currentBlock.insert(currentBlock.getPosition(key), key, value, currentBlock.getChild(currentBlock.getPosition(key)));
//
//     if (!currentBlock.splitNeeded())
//     {
//
//       if (debug)
//       {
//         cout << endl;
//         cout << "splitNeeded()|FALSE" << endl;
//       }
//
//       _file.putBlock(currentBlockNum, currentBlock);
//     }
//     else
//     {
//       cout << "currentBlock before split check " << currentBlock.splitNeeded() << endl;
//       cout << "currentBlock before split number " << currentBlockNum << endl;
//
//       while (currentBlock.splitNeeded())
//       {
//         BTreeBlock newBlock;
//         BTreeFile::BlockNumber newBlockNum;
//
//         unsigned midIndex = (currentBlock.getNumberOfKeys() - 1) / 2;
//         string midKey = currentBlock.getKey(midIndex);
//         string midValue = currentBlock.getValue(midIndex);
//
//         currentBlock.split(midKey, midValue, newBlock);
//
//         _file.putBlock(currentBlockNum, currentBlock);
//         newBlockNum = _file.allocateBlock();
//         _file.putBlock(newBlockNum, newBlock);
//
//         blockNumberStack.top() = currentBlockNum;
//
//         cout << "Stack top block number " << currentBlockNum << endl;
//
//
//         _file.getBlock(currentBlockNum, currentBlock);
//
//         currentBlock.insert(currentBlock.getPosition(midKey), midKey, midValue, currentBlock.getChild(currentBlock.getPosition(key)));
//
//         blockNumberStack.pop();
//
//         if (currentBlock.splitNeeded() == false)
//         {
//           _file.putBlock(currentBlockNum, currentBlock);
//         }
//       }
//     }
//   }
}


// void BTree::insertR(string key, string value, BTreeBlock currentBlock, BTreeFile::BlockNumber currentBlockNumber)
// {
//   // BTreeFile::BlockNumber root = _file.getRoot();
//   // int currentBlock = editedBlock;
//   //
//   // while (currentBlock != getChild(root))
//   // {
//   //   int currentBlock = getChild(root);
//   // }
//   // BTreeBlock.insert(getPosition(key), key, value, BTreeFile.getChild(currentBlock))
//   // if (splitNeeded(currentBlock) == false)
//   // {
//   //   putBlock(currentBlock);
//   // }
//   // else
//   // {
//   //   BTreeBlock();
//   //   split();
//   //   putBlock();
//   //   allocateBlock();
//   //   putBlock();
//   //   insertR(key, value, currentBlock);
//   // }
// }


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
