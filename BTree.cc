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

/**
 * @param string key | the key for the inserted item.
 * @param string value | the value for the inserted item.
 *
 * @description | This functions works by checking for a number of cases.
 *
 * @case-1 | Tree is empty
 * @case-1-description | Create a block, insert the item into this block
 * set this block as a root then store it to disk
 *
 * @transition-1 | Get to the leaf
 * @transition-1-description | Get to the leaf of the tree, by transitioning down
 * using the key to find the appropriate value
 *
 * @case-2 | root block and split needed
 * @case-2-description | Start by splitting the currentblock then creating two slots
 * on the disk, we then set the root to the block that we will use as the new parent,
 * then save the rightchildblock and currentblock (leftchildblock) to disk. We call the recursive
 * function, passing the new parent block number and block as the current block.
 *
 * @case-3 | not root block and split needed
 * @case-3-description | Start by splitting the currentblock then create one slot on the disk,
 * then save the rightchildblock and currentblock (leftchildblock) to disk, we then get the parent
 * of the current block, we call the recursive function, passing the parent we found
 * and the making sure to set leftchildnum to 0
 *
 * @case-4 | no split needed insert
 * @case-4-description | At this point we attempt to insert but before we can we must check for a number
 * of things
 *
 * @case-4-subcase-1 | case when we need a new root
 * @case-4-subcase-1-description | This case happens when leftchildblocknumber is not 0, this only occures
 * when we have set the currentblocknum as the new parent (a new root was made). This value is the left child
 * recorded here so we dont lose it when we create a new root. After we insert we set the left child
 * (insert takes care of the right) and then check to ensure that the block does not need to be split, (though
 * this case would be almost if not completely impossible, since to get here we had to create a new root, but
 * just to be safe).
 *
 * @case-4-subcase-2 | simple insert
 * @case-4-subcase-2-description | This is the base case, if everything is fine then we insert the desired
 * key and value inthe current block. Then we check to ensure that no split is needed, if a split is needed
 * we call the recursive function again with the propervalues and the process repeats. If a split is not needed then
 * we save the block back to disk.
 *
 */
void BTree::insert(string key, string value)
{
  //Create initial variables
  stack<BTreeFile::BlockNumber> blockNumberStack;

  BTreeFile::BlockNumber rootNum = _file.getRoot();

  BTreeFile::BlockNumber currentBlockNum;
  BTreeBlock currentBlock;

  //@case-1
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


    //@transition-1
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

/**
* @param string key | the key for the inserted item.
* @param string value | the value for the inserted item.
* @param BTreeBlock currentBlock | the current block in memory
* @param BTreeFile::BlockNumber currentBlockNumber | the current block number
* @param BTreeFile::BlockNumber leftChildBlockNumber | the left child number
* @param BTreeFile::BlockNumber rightChildBlockNumber | the right child number
*
* @description | Helper function called by insert
*
*/

void BTree::insertR(string key, string value, BTreeBlock currentBlock, BTreeFile::BlockNumber currentBlockNumber, BTreeFile::BlockNumber leftChildBlockNumber, BTreeFile::BlockNumber rightChildBlockNumber)
{
  BTreeFile::BlockNumber rootNum = _file.getRoot();

  BTreeFile::BlockNumber newRightBlockNum;
  BTreeBlock newRightBlock;

  BTreeFile::BlockNumber newParentNum;
  BTreeBlock newParent;

  // @case-2
  if ((currentBlockNumber == rootNum) && currentBlock.splitNeeded())
  {
    //Get the middle value and key and set them to variables
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

  // @case-3
  else if ((currentBlockNumber != rootNum) && currentBlock.splitNeeded())
  {
    //Get the middle value and key and set them to variables
    unsigned midIndex = (currentBlock.getNumberOfKeys() - 1) / 2;
    string midKey = currentBlock.getKey(midIndex);
    string midValue = currentBlock.getValue(midIndex);

    currentBlock.split(midKey, midValue, newRightBlock);

    newRightBlockNum = _file.allocateBlock();

    _file.putBlock(currentBlockNumber, currentBlock);
    _file.putBlock(newRightBlockNum, newRightBlock);

    newParentNum = BTree::getParent(currentBlockNumber, key);

    _file.getBlock(newParentNum, newParent);

    BTree::insertR(key, value, newParent, newParentNum, 0, newRightBlockNum);
  }

  // @case-4
  else
  {
    // @case-4-subcase-1
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
    // @case-4-subcase-2
    else
    {
      //just insert
      currentBlock.insert(currentBlock.getPosition(key), key, value, rightChildBlockNumber);

      if (currentBlock.splitNeeded())
      {
        cout << "insert" << endl;
        BTree::insertR(key, value, currentBlock, currentBlockNumber, 0, rightChildBlockNumber);
      }
      else
      {
        _file.putBlock(currentBlockNumber, currentBlock);
      }
    }
  }
}


/**
* @param BTreeFile::BlockNumber child | the child block we want the parent for
* @param string key | the key of the child block
*
* @return BTreeFile::BlockNumber | the parent of the child specefied or 0 if unable to find parent
*
* @description | Function to get the parent of a given block. Works by creating to pointers one at the
* current block then one head, we then compare the current pointer and then the peack pointer
* if the equal we return that block if not then we repeat the process till we reach the block that
* called us, if we dont find it then we turn 0.
*
*/

BTreeFile::BlockNumber BTree::getParent(BTreeFile::BlockNumber child, string key) const
{
  BTreeFile::BlockNumber rootNum = _file.getRoot();

  BTreeFile::BlockNumber currentBlockNum = rootNum;
  BTreeBlock currentBlock;

  BTreeFile::BlockNumber peackBlockNum = rootNum;
  BTreeBlock peackBlock;

  _file.getBlock(currentBlockNum, currentBlock);

  peackBlockNum = currentBlock.getChild(currentBlock.getPosition(key));
  _file.getBlock(peackBlockNum, peackBlock);

  if (peackBlockNum == child)
  {
    return currentBlockNum;
  }

  while (peackBlockNum != child)
  {
    cout << "child:" << child << endl;
    cout << "Peackblock:" << peackBlockNum << endl;

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

    BTreeFile::BlockNumber something;



    something = BTree::getParent(root, key);

    cout << "Parent" << something << endl;
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
