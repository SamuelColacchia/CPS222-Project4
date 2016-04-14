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
