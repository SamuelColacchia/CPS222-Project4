bool BTree::remove(string key)
{
        //Create initial variables

        BTreeFile::BlockNumber rootNum = _file.getRoot();

        BTreeFile::BlockNumber currentBlockNum;
        BTreeBlock currentBlock;
        BTreeBlock parentBlock;
        BTreeBlock Siblings;

        _file.getBlock(currentBlockNum, currentBlock);

        //intrsting problem, getting to the value if not a leaf
        while (currentBlock.getKey(currentBlock.getPosition(key) == currentBlock.getChild()))
        {
                currentBlockNum = currentBlock.getChild(currentBlock.getPosition(key));
                _file.getBlock(currentBlockNum, currentBlock);
        }

        if(currentBlock.isLeaf()) {

                if(currentBlock.getNumberOfKeys() != 1) {
                        currentBlock.setValue(currentBlock.getPosition(key), NULL);
                        currentBlock.setKey(currentBlock.getPosition(key), NULL);
                        _file.putBlock(currentBlockNumber, currentBlock);
                }

                else{

                  newParentNum = BTree::getParent(currentBlockNumber, key);
                  _file.getBlock(newParentNum, parentBlock);

                  if(currentBlock.getKey(currentBlock.getPosition(key) >
                      parentBLock.getKey(currentBlock.getPosition(parentBLock.getNumberOfKeys()) )
                  {
                    _file.getBlock(newParentNum -1, Siblings);
                  }

                  else{
                    _file.getBlock(newParentNum +1, Siblings);
                  }

                  while(currentBlock.getNumberOfKeys() > Siblings.getNumberOfKeys()){

                  }

                  }
        }
}

As we have seen in other kinds of trees, deleting a key from a
         leaf will be much simpler than deleting a key from an interior node.
         As before, then, we use the trick of converting a deletion from an
         interior node into a deletion from a leaf by promoting a key from
         a leaf - typically the first key in the leftmost subtree of the
         child just after the key.

if(isLeaf){
  if(if not the only key){
    delete
  }
  else{
    find a sibling node;
    rearrange siblings nodes so they both have minimun
      (dont understnand this onechild);
      this will mean changing the divider key between the them in the parent;
      If(if either of the nodes are to small){
        combine them instead;
        (it says remove pointer in the online doc but this may be automatic)
      }
  }
else{
  delete key and pull key from child and sibling
  depending on the arangement of keys
  in extreme cases we have to merge siblings and even reduce a row;
  (working on this case and what it will look like)
}
}
