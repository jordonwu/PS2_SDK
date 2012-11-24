package ps2vfs.vfs;

 
public class MountTableIterator implements java.util.Iterator {

  private java.util.Iterator it;
  private java.util.Iterator childIt = null;
  private java.util.Map.Entry curEnt = null;
  private boolean             iterateOnNext = true;
  private MountPoint curChild = null;
  private boolean debug = false;

  MountTableIterator(java.util.Iterator iit) {
    it = iit;
  }

  private void nextCurChild()
  {
    if(debug) {
      System.err.println("nextCurChild(): [" + it + " " + (it != null ? ("" + it.hasNext()) : "" ) + "] " + curEnt + 
			 " [" + childIt + " " + (childIt != null ? ("" + childIt.hasNext()) : "" ) + "] " + curChild);
    }
    curChild = null;

    if(curEnt == null) {
      if(!it.hasNext()) {
	curChild = null;
	return;
      }
      curEnt = (java.util.Map.Entry) it.next(); 
    }

    if(childIt == null) {
      MountPoint mp = (MountPoint) curEnt.getValue();
      if(mp != null) {
	childIt = mp.getChildren().iterator();
      } else {
	curChild = null;
	return;
      }
    }
    
    while(it.hasNext() || childIt.hasNext()) {
      curChild = null;
      if(debug) {
	System.err.println("nextCurChild(): [" + it + " " + (it != null ? ("" + it.hasNext()) : "" ) + "] " + curEnt + 
			   " [" + childIt + " " + (childIt != null ? ("" + childIt.hasNext()) : "" ) + "] " + curChild);
      }
      if(!childIt.hasNext()) {
	// Select next mount entry.
	if(debug) {
	  System.err.println("nextCurChild(): Ran out of children"); 
	}
	curEnt = (java.util.Map.Entry) it.next();
	MountPoint mp = (MountPoint) curEnt.getValue();
	if(mp != null) {
	  java.util.List children = mp.getChildren();
	  if(children != null) 
	    childIt = children.iterator();
	  else
	    continue;
	} else {
	  continue;
	}
      }
      while(childIt.hasNext()) {
	curChild = (MountPoint) childIt.next();
	if(debug) {
	  System.err.println("nextCurChild(): lookin at child: " + curChild); 
	}
	if(curChild != null && !curChild.isVirtual()) {
	  break;
	}
      }
      
      if(curChild != null && !curChild.isVirtual()) {
	break;
      }
    }
  }
  
  public boolean hasNext() {
    if(iterateOnNext) {
      nextCurChild();
      iterateOnNext = false;
    }
    if(debug) {
      System.err.println("hasNext returning: " + (curChild != null) + " [" + (curChild != null ? curChild.toString() : "") + "]"); 
    }
    return (curChild != null);
  }

  public java.lang.Object next() 
    throws java.util.NoSuchElementException
  {
    if(iterateOnNext) {
      nextCurChild();
    }
    iterateOnNext = true; // Allways iterate on next.

    if(curChild == null) {
      throw(new java.util.NoSuchElementException());
    }
    String virtualPath = (String) curEnt.getKey();
    if(debug) {
      System.err.println("next returning: " + curChild); 
    }
    return new MountTableEntry(virtualPath, curChild);
  }

  public void remove() 
    throws java.lang.UnsupportedOperationException
  {
    throw(new java.lang.UnsupportedOperationException());
  }
};
