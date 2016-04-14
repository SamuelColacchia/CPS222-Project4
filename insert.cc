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
      //cout << "CurrentBlock" << currentBlock <<endl;
    }

    rootNum = _file.allocateBlock();
    cout << "RootNum:" << rootNum << endl;
    // cout << "Currentblockvalue:" << currentBlock.getValue(currentBlock.getNumberOfKeys()-1) << endl;

    _file.setRoot(rootNum);


    if (debug)
    {
      cout << endl;
      cout << "RootNum:" << rootNum << endl;
      cout << "Get position of the key: " << currentBlock.getPosition(key) << endl;
      cout << "Header insert child value:" << currentBlock.getChild(currentBlock.getPosition(key)) << endl;

      cout << "###-Values to insert-###" << endl;
      cout << "insert num:" << currentBlock.getPosition(key) << endl;
      cout << "key:" << key << endl;
      cout << "value:" << value << endl;
      cout << "child num:" << childNumNull;
    }

    currentBlock.insert(0, key, value, childNumNull);
    currentBlock.setChild(0, 0);
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
      cout << "Current Child value for insert: " << currentBlock.getChild(currentBlock.getPosition(key)) << endl;
    }

    // if (!currentBlock.isLeaf())
    // {
    //   childNumNull = currentBlock.getChild(currentBlock.getPosition(key));
    // }

    currentBlock.insert(currentBlock.getPosition(key), key, value, 0);

    if (!currentBlock.splitNeeded())
    {
      _file.putBlock(currentBlockNum, currentBlock);
    }

    while (currentBlock.splitNeeded())
    {
      cout << "split loop" << endl;
      BTreeBlock newBlock;
      BTreeBlock newParent;
      BTreeFile::BlockNumber newBlockNum;
      BTreeFile::BlockNumber newParentNum;

      unsigned midIndex = (currentBlock.getNumberOfKeys() - 1) / 2;
      string midKey = currentBlock.getKey(midIndex);
      string midValue = currentBlock.getValue(midIndex);

      newBlockNum = _file.allocateBlock();


      currentBlock.split(midKey, midValue, newBlock);


      _file.putBlock(currentBlockNum, currentBlock);
      _file.putBlock(newBlockNum, newBlock);


      if (blockNumberStack.empty())
      {
        if (currentBlockNum == rootNum)
        {
          newParentNum = _file.allocateBlock();
          newParent.insert(newParent.getPosition(midKey), midKey, midValue, newBlockNum);
          newParent.setChild(0, currentBlockNum);
          _file.setRoot(newParentNum);
          _file.putBlock(newParentNum, newParent);
        }
      }
      else
      {
        currentBlockNum = blockNumberStack.top();
        _file.getBlock(currentBlockNum,currentBlock);
        currentBlock.insert(currentBlock.getPosition(midKey), midKey, midValue, newBlockNum);
        _file.putBlock(currentBlockNum, currentBlock);
        blockNumberStack.pop();
      }
      cout << "split before 3" << endl;

      cout << "Stack top block number " << currentBlockNum << endl;

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
