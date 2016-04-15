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
bool debug = false;

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
      if (currentBlock.getKey(currentBlock.getPosition(key)) == key)
      {
        cout << "Duplicate key:" << key << endl;
        cout << "Insert canceled" << endl;
      }
      else
      {
        BTree::insertR(key, value, currentBlock, currentBlockNum, 0, 0);
      }
    }

    while (!currentBlock.isLeaf())
    {
      currentBlockNum = currentBlock.getChild(currentBlock.getPosition(key));
      _file.getBlock(currentBlockNum, currentBlock);
      if (currentBlock.isLeaf())
      {
        if (currentBlock.getKey(currentBlock.getPosition(key)) == key)
        {
          cout << "Duplicate key:" << key << endl;
          cout << "Insert canceled" << endl;
        }
        else
        {
          BTree::insertR(key, value, currentBlock, currentBlockNum, 0, 0);
        }
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

    BTree::insertR(midKey, midValue, newParent, newParentNum, currentBlockNumber, newRightBlockNum);
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

    BTree::insertR(midKey, midValue, newParent, newParentNum, 0, newRightBlockNum);
  }

  // @case-4
  else
  {
    // @case-4-subcase-1
    if (leftChildBlockNumber != 0)
    {
      currentBlock.insert(currentBlock.getPosition(key), key, value, rightChildBlockNumber);
      currentBlock.setChild(currentBlock.getPosition(key), leftChildBlockNumber);

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
      currentBlock.insert(currentBlock.getPosition(key), key, value, rightChildBlockNumber);

      if (currentBlock.splitNeeded())
      {
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


unsigned BTree::countDirectChildren(BTreeFile::BlockNumber currentBlockNumber, BTreeBlock currentBlock) const
{
  unsigned numberOfKeys = currentBlock.getNumberOfKeys();
  unsigned numberOfPossibleChildren = numberOfKeys + 1;
  unsigned numberOfVerifiedChildren;
  BTreeFile::BlockNumber childnum;

  for (int i = 0; i < numberOfPossibleChildren; i++)
  {
    childnum = currentBlock.getChild(i);
    if (childnum != 0)
    {
      numberOfVerifiedChildren++;
    }
  }
  return numberOfVerifiedChildren;
}


bool BTree::hasChildren(BTreeFile::BlockNumber currentBlockNumber, BTreeBlock currentBlock) const
{
  unsigned currentKeys = currentBlock.getNumberOfKeys();

  cout << "currentkeys:" << currentKeys << endl;
  for (int i = 0; i < currentKeys; i++)
  {
    cout << "currentblock child" << currentBlock.getChild(i) << endl;
    if (currentBlock.getChild(i) != 0)
    {
      return true;
    }
  }
  return false;
}


//TODO redo comment based on code changes

/**
 * @param string key | the key of the item to find
 * @param string& value | the value we set if we find the key
 *
 * @description | Given a key look thourgh the tree and find the item.
 * If root = 0 then we return false because the tree is empty.
 * Then call the recursive lookup function, passing it the key,
 * root and value. If the key == the current key then we return true and
 * set value = to the blocks current vlaue at the currentPos. If not and
 * the currentblock is not a leaf then we call Rlookup with the root being changed.
 *
 */
bool BTree::lookup(string key, string& value) const
{
  BTreeFile::BlockNumber root = _file.getRoot();

  BTreeBlock currentBlock;
  BTreeFile::BlockNumber currentBlockNumber = root;

  string currentKey;

  _file.getBlock(currentBlockNumber, currentBlock);

  if (root == 0)
  {
    return false;
  }
  else
  {
    while (!currentBlock.isLeaf())
    {
      currentKey = currentBlock.getKey(currentBlock.getPosition(key));
      if (key == currentKey)
      {
        value = currentBlock.getValue(currentBlock.getPosition(key));
        return true;
      }
      currentBlockNumber = currentBlock.getChild(currentBlock.getPosition(key));
      _file.getBlock(currentBlockNumber, currentBlock);
    }
    currentKey = currentBlock.getKey(currentBlock.getPosition(key));
    if (key == currentKey)
    {
      value = currentBlock.getValue(currentBlock.getPosition(key));
      return true;
    }
    else if (currentBlock.isLeaf())
    {
      return false;
    }
  }
}


/**
 * @param string key | the key of the item to find
 * @param BTreeFile::BlockNumber root | the current root
 * @param string& value | the value we set if we find the key
 *
 * @description | Helper function called by lookup
 */
bool BTree::rlookup(string key, BTreeFile::BlockNumber currentBlockNumber, string value) const
{
  BTreeBlock currentBlock;

  _file.getBlock(currentBlockNumber, currentBlock);

  string currentKey = currentBlock.getKey(currentBlock.getPosition(key));

  cout << "currentKey:" << currentKey << endl;
  cout << "currentPos:" << currentBlock.getPosition(key) << endl;
  if (key == currentKey)
  {
    value = currentBlock.getValue(currentBlock.getPosition(key));
    cout << "return true" << endl;
    cout << "value" << value << endl;
    return true;
  }
  else if (currentBlock.isLeaf())
  {
    cout << "return false" << endl;
    return false;
  }
  else
  {
    BTree::rlookup(key, currentBlock.getChild(currentBlock.getPosition(key)), value);
  }
}


bool BTree::remove(string key)
{
  string value;
  BTreeFile::BlockNumber rootNum = _file.getRoot();

  BTreeFile::BlockNumber currentBlockNum = rootNum;
  BTreeBlock currentBlock;

  BTreeFile::BlockNumber parentBlockNum;
  BTreeBlock parentBlock;

  BTreeFile::BlockNumber neighborBlockNum;
  BTreeBlock neighborBlock;


  _file.getBlock(currentBlockNum, currentBlock);

  if (!BTree::lookup(key, value))
  {
    return false;
  }

  while (!currentBlock.isLeaf() && currentBlock.getKey(currentBlock.getPosition(key)) != key)
  {
    currentBlockNum = currentBlock.getChild(currentBlock.getPosition(key));
    _file.getBlock(currentBlockNum, currentBlock);
  }

  return BTree::removeR(key, currentBlockNum, currentBlock, parentBlockNum, parentBlock, neighborBlockNum, neighborBlock);
}


bool BTree::removeR(string key, BTreeFile::BlockNumber currentBlockNum, BTreeBlock currentBlock, BTreeFile::BlockNumber parentBlockNum, BTreeBlock parentBlock, BTreeFile::BlockNumber neighborBlockNum, BTreeBlock neighborBlock)
{
  unsigned numberOfKeys = currentBlock.getNumberOfKeys();
  unsigned numberOfChildren = BTree::countDirectChildren(currentBlockNum, currentBlock);

  cout << "numberofchildren: " << numberOfChildren << endl;
  if ((numberOfKeys > 1) && (numberOfKeys + 1 >= numberOfChildren))
  {
    currentBlock.remove(currentBlock.getPosition(key));
    _file.putBlock(currentBlockNum, currentBlock);
    return true;
  }
  //delete block cases
  else
  {
    parentBlockNum = BTree::getParent(currentBlockNum, key);
    _file.getBlock(parentBlockNum, parentBlock);

    int currentBlockParentPos = parentBlock.getPosition(key);

    neighborBlockNum = parentBlock.getChild(currentBlockParentPos - 1);

    _file.getBlock(neighborBlockNum, neighborBlock);

    cout << "numberof keys" << neighborBlock.getNumberOfKeys() << endl;

    if (neighborBlock.getNumberOfKeys() > 1)
    {
      currentBlock.remove(currentBlock.getPosition(key));

      currentBlock.insert(currentBlock.getPosition(parentBlock.getKey(parentBlock.getPosition(key))), parentBlock.getKey(parentBlock.getPosition(key)), parentBlock.getValue(parentBlock.getPosition(key)),0);

      _file.putBlock(currentBlockNum,currentBlock);

      parentBlock.remove(parentBlock.getPosition(key));

      parentBlock.insert(parentBlock.getPosition(neighborBlock.getKey(neighborBlock.getPosition(key)-1)),neighborBlock.getKey(neighborBlock.getPosition(key)-1), neighborBlock.getValue(neighborBlock.getPosition(key)-1), currentBlockNum);

      _file.putBlock(parentBlockNum, parentBlock);

      neighborBlock.remove(neighborBlock.getPosition(key)-1);

      _file.putBlock(neighborBlockNum,neighborBlock);
    }


    // if (neighborBlockParentPos > 0)
    // {
    //   neighborBlockParentPos--;
    // }
    // else
    // {
    //   neighborBlockParentPos++;
    // }
    //
    // if (currentBlockParentPos > neighborBlockParentPos)
  }



  // currentBlock.remove(currentBlock.getPosition(key));
  // _file.putBlock(currentBlockNum, currentBlock);
  // if (currentBlock.getNumberOfKeys() == 1)
  // {
  //   cout << "block removed" << endl;
  //
  //   parentBlockNum = BTree::getParent(currentBlockNum, key);
  //   _file.getBlock(parentBlockNum, parentBlock);
  //
  //   int neighborPosition = parentBlock.getPosition(key);
  //
  //
  //   if (neighborPosition > 0)
  //   {
  //     neighborPosition--;
  //   }
  //   else
  //   {
  //     neighborPosition++;
  //   }
  //
  //   neighborBlockNum = parentBlock.getChild(neighborPosition);
  //   _file.getBlock(neighborBlockNum, neighborBlock);
  //
  //   unsigned currentBlockNumChildren = BTree::countDirectChildren(currentBlockNum, currentBlock);
  //   unsigned neighborBlockNumChildren = BTree::countDirectChildren(neighborBlockNum, neighborBlock);
  //
  //   cout << "currentblock:" << currentBlockNum << endl;
  //   cout << "neighblock:" << neighborBlockNum << endl;
  //
  //   if(neighborBlock.getNumberOfKeys() + 1 >= neighborBlockNumChildren)
  //   {
  //
  //   }

  // if (currentBlock.getNumberOfKeys() + 1 >= currentBlockNumChildren && neighborBlock.getNumberOfKeys() + 1 >= neighborBlockNumChildren)
  // {
  //   cout << "combine" << endl;
  //   for (int i = 0; i < neighborBlock.getNumberOfKeys() - 1;i++)
  //   {
  //     currentBlock.setKey(currentBlock.getPosition(neighborBlock.getKey(i)),neighborBlock.getKey(i));
  //     neighborBlock.setKey(i,0);
  //
  //     currentBlock.setValue(currentBlock.getPosition(neighborBlock.getKey(i)),neighborBlock.getValue(i));
  //     neighborBlock.setValue(i,0);
  //   }
  //   neighborBlock.setNumberOfKeys(0);
  //
  //   currentBlock.setKey(parentBlock.getPosition(key), parentBlock.getKey(parentBlock.getPosition(key)));
  //   currentBlock.setValue(parentBlock.getPosition(key), parentBlock.getValue(parentBlock.getPosition(key)));
  //   _file.putBlock(neighborBlockNum,neighborBlock);
  //   _file.putBlock(currentBlockNum,currentBlock);
  // }
  // else
  // {
  //
  // }
  //
  // parentBlock.setChild(currentBlock.getPosition(key), 0);
  // _file.putBlock(parentBlockNum, parentBlock);
  //
  // _file.deallocateBlock(currentBlockNum);
  //}
  //   return true;
  // }
  // else if (numberOfChildren == 1)
  // {
  // }
  // else if (numberOfChildren >= 2)
  // {
  // }
  // else
  // {
  //   return false;
  // }
  return false;

  //base case delete node with no child

  //case delete node with one child

  //case delete node with two children
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
