/*
 * BTree.h - declarations for the class BTree.	CS212 project 4
 *
 * Copyright (c) 2001, 2002, 2003 - Russell C. Bjork
 * Trivial changes 2016 - Russ Tuck
 */

#ifndef BTREE_H
#define BTREE_H

#include <string>
#include "BTreeFile.h"
#include "BTreeBlock.h"

class BTree
{
  public:

    // Constructor
    //
    // Parameter: name of file (which may be a relative or absolute
    // path).  If a file of this name exists, it is opened and the
    // tree stored in it is accessed; else a new file containing an
    // empty tree is created
    BTree(string name);

    // Insert a new key, value pair into this structure
    //
    // Parameters: key and associated value.  The key may not contain any
    // embedded spaces or control characters.
    void insert(string key, string value);

    void insertR(string key, string value, BTreeBlock currentBlock, BTreeFile::BlockNumber currentBlockNumber, BTreeFile::BlockNumber leftChildBlockNumber, BTreeFile::BlockNumber rightChildBlockNumber);

    BTreeFile::BlockNumber getParent(BTreeFile::BlockNumber child, string key) const;

    bool hasChildren(BTreeFile::BlockNumber currentBlockNumber, BTreeBlock currentBlock) const;

    // Lookup a key.  If found, set value to associated value and
    // return true, else leave value unchanged and return false
    bool lookup(string key, string & value) const;

    bool rlookup(string key,BTreeFile::BlockNumber blockNumber,string value) const;

    // Remove a key and its associated value.  Return true if
    // found and removed, false if not.
    bool remove(string key);

    bool removeR(string key);

    // Print the contents of the entire tree to cout for testing/debugging
    // purposes.
    void print() const;

    // Print the contents of a specific block to cout for testing/debugging
    // purposes
    void print(BTreeFile::BlockNumber blockNumber) const;

    // Destructor
    ~BTree();

  private:

    BTreeFile & _file;
};

#endif
