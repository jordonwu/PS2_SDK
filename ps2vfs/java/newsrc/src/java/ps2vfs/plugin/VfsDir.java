/**
 * This is the interface that all PS2VFS Plugins must implement.
 *
 * @version 0.0.1 
 * @author Stig Petter Olsrød
 */

package ps2vfs.plugin;



public class VfsDir 
{
  private String vpath;
  private java.util.List dirContent;

  public VfsDir(String vpath, java.util.List content) {
    // Make sure the path is terminated with '/'.
    if(!vpath.endsWith("/")) {
      this.vpath = vpath + "/";
    } else {
      this.vpath = vpath;
    }
    dirContent = content;
  }

  public String getName() {
    int len = vpath.length();
    if(len == 1)
      return vpath;
    int li1 = vpath.lastIndexOf('/');
    if(li1 <= 0)
      return null;
    if(li1 != len-1) {
      return vpath.substring(li1);
    }
    li1--;
    int li2 = vpath.lastIndexOf('/', li1);
    if(li2 >= 0) {
      li2++;
      return vpath.substring(li2, li2-li1);
    }
    return null;
    
  }

  public String getPath() {return vpath;}

  public String getParentPath() {
    return getParentPath(vpath);
  }

  public static String getParentPath(String inPath) {
    if(inPath == null)
      return null;
    
    int len = inPath.length();
    if(len == 1)
      return null;
    int li = inPath.lastIndexOf('/');
    if(li <= 0)
      return null;
    if(li != len-1) {
      return inPath.substring(0, li+1);
    }
    li--;
    li = inPath.lastIndexOf('/', li);
    if(li >= 0) {
      return inPath.substring(0, li+1);
    }
    return null;
  }    

  public VfsDirEntry getEntry(String elementName) {
    java.io.File elFile = new java.io.File(elementName);
    String fileName = elFile.getName();
    String path = getParentPath(elementName);

    if(path == null || path.equals(vpath)) {
      java.util.Iterator it = dirContent.iterator();
      while(it.hasNext()) {
	VfsDirEntry dirEnt = (VfsDirEntry) it.next();
	String vn = dirEnt.getVirtualName();
	// System.out.println("Checking entry " + vn + " " + fileName);
	if(vn != null && vn.equals(fileName)) {
	  dirEnt.setPath(vpath);
	  return dirEnt;
	}
      }
    }
    return null;
  }


  public VfsDirEntry[] listEntries() {
    VfsDirEntry[] a = new VfsDirEntry[0];
    return (VfsDirEntry[])dirContent.toArray(a);
  }

  public java.util.List list() {
    return dirContent;
  }

  public void sort() {
    java.util.Collections.sort(dirContent, new VfsDirEntry.DirFirstThenNameComparator());
  }

  public void uniqify(boolean collapse) {
    java.util.ListIterator it = dirContent.listIterator();
    VfsDirEntry prevDirEntry = null;
    int sequenceNum = 0;
    VfsDirEntry prevEntry = null;
    String origPrevName = null;

    while(it.hasNext()) {
      VfsDirEntry entry = (VfsDirEntry) it.next();
      boolean dup = false;
      if(origPrevName != null) {
	if(origPrevName.equals(entry.getVirtualName()))
	  dup = true;
      } else if(prevEntry != null && 
		prevEntry.getVirtualName().equals(entry.getVirtualName())) {
	dup = true;
      }
      if(dup) {
	if(origPrevName == null)
	  origPrevName = entry.getVirtualName();

	if(collapse) {
	  // Collapse directory entries into one, keeping the name of it
	  // and changing the name of any files.
	  if(entry.isDirectory()) {
	    // System.out.println("Uniqify: entry is dir " + entry + " prevDir " + prevDirEntry);
	    if(prevDirEntry == null) {
	      if(prevEntry.isDirectory()) {
		prevDirEntry = prevEntry;
	      } else {
		prevDirEntry = entry;
		if(sequenceNum == 0) {
		  sequenceNum++;
		  int ext = origPrevName.lastIndexOf('.');
		  if(ext > 0) 
		    prevEntry.setVirtualName(origPrevName.substring(0, ext) 
					     + "~" + sequenceNum + 
					     origPrevName.substring(ext));
		  else 
		    prevEntry.setVirtualName(origPrevName + "~" + sequenceNum);

		  sequenceNum++;
		}
		continue;
	      }
	    }
	    prevDirEntry.addHandlerList(entry.getHandlerList());
	    it.remove();
	    continue;
	  }
	}

	if(sequenceNum == 0) {
	  sequenceNum++;
	  int ext = origPrevName.lastIndexOf('.');
	  if(ext > 0) 
	    prevEntry.setVirtualName(origPrevName.substring(0, ext) 
				     + "~" + sequenceNum + 
				     origPrevName.substring(ext));
	  else 
	    prevEntry.setVirtualName(origPrevName + "~" + sequenceNum);
	}
	sequenceNum++;
	int ext = origPrevName.lastIndexOf('.');
	if(ext > 0) 
	  entry.setVirtualName(origPrevName.substring(0, ext) 
			       + "~" + sequenceNum + 
			       origPrevName.substring(ext));
	else 
	  entry.setVirtualName(origPrevName + "~" + sequenceNum);
      } else {
	origPrevName = null;
	sequenceNum = 0;
	prevDirEntry = null;
      }
      prevEntry = entry;
    }
  }

  public String toString() {
    return new String("VfsDir: " + vpath + " numEntries: " + dirContent.size());
  }
}


