/**
 * This is the interface that all PS2VFS Plugins must implement.
 *
 * @version 0.0.1 
 * @author Stig Petter Olsrød
 */

package ps2vfs.plugin;

public class VfsDirEntry
{

  public static class DirFirstThenNameComparator 
    implements java.util.Comparator
  {
    public int compare(Object e1, Object e2) {
      VfsDirEntry de1 = (VfsDirEntry) e1;
      VfsDirEntry de2 = (VfsDirEntry) e2;
      
      if(de1.isDirectory() == de2.isDirectory()) {
	return de1.getVirtualName().compareTo(de2.getVirtualName());
      } else if(de1.isDirectory()) {
	return -1;
      } else
	return 1;
    }
  }
  
  public VfsDirEntry() {
    handlers = new java.util.Vector(0);
    isVirtual = false;
    isDir = false;
    isHidden = false;
    virtualName = null;
  }

  /**
   * Returns true if the entry is a directory.
   */
  public boolean isDirectory() { return isDir || isVirtual; }

  /**
   * Returns true if the entry is hidden.
   */
  public boolean isHidden() { return isHidden; }

  /**
   * Returns true if the entry is a virtual dir,
   * i.e a file or similar that we just pretent 
   * to be a directory so we can browse it 
   * on the PS2. (Playlists or Media libraries).
   */
  public boolean isVirtualDir() { return isVirtual; }
  
  /** 
   * Returns the name of the entry as it should be
   * displayed on the PS2.
   */
  public String  getVirtualName() {
    if(virtualName == null) {
      String openPath = getOpenPath();
      if(openPath != null) {
	return new java.io.File(openPath).getName();
      }
    } else {
      return virtualName; 
    }
    return "noname";
  }

  /**
   * Returns the path that should be used to open the 
   * file on the host.
   */
  public String  getOpenPath() { 
    if(handlers.size() != 0) {
      VfsHandler hndlr = (VfsHandler) handlers.get(0);
      if(hndlr != null) 
	return hndlr.getOpenPath();
    }
    return null;
  }

  /**
   * Returns the plugin to use for accessing this entry
   */
  public VfsPlugin getHandler() {
    if(handlers.size() != 0) {
      VfsHandler hndlr = (VfsHandler) handlers.get(0);
      if(hndlr != null) 
	return hndlr.getHandler();
    }
    return null;
  }

  /**
   * Returns the plugins to use for accessing this entry
   * Multiples are only allowed for directories.
   */
  public java.util.List /*<VfsHandler>*/ getHandlerList() {
    return handlers;
  }


  /***********************************************/
  public void setOpenPath(String op) {
    if(!isDirectory()) {
      handlers.clear();
    }
    handlers.add(new VfsHandler(op, null));
  }

  public void setHandler(VfsHandler phandler) { 
    if(isDirectory()) {
      handlers.add(phandler);
    } else {
      handlers.clear();
      handlers.add(phandler);
    }
  }

  public void addHandlerList(java.util.List /*<VfsHandler>*/ ihandlers) { 
    if(isDirectory()) {
      handlers.addAll(ihandlers);
    } else {
      System.err.println("Unable to set handlers in non-directory (" + this + ", " + ihandlers + ")");
    }
  }

  public void setHandler(VfsPlugin phandler) { 
    if(isDirectory()) {
      System.err.println("Unable to set handler in directory (" + phandler + ", " + this + ")");
    } else {
      VfsHandler vh = (VfsHandler) handlers.get(0);
      if(vh.getHandler() == null) {
	vh.setHandler(phandler);
      } else {
	System.err.println("Handler is set (" + vh + ", " + this + ")");
      }
    }
  }

  public void setHandler(String op) {
    setOpenPath(op);
  }

  public void setVirtualName(String name) {
    virtualName = name;
  }

  public void setDirectory(boolean s) {
    isDir = s;
  }
  public void setVirtualDir(boolean s) {
    isVirtual = s;
  }
  
  public void setHidden(boolean s) {
    isHidden = s;
  }

  public void setPath(String path) {
    virtualPath = path + getVirtualName();
  }

  public String getVirtualPath() {
    if(virtualPath != null)
      return virtualPath;
    else 
      return getVirtualName();
  }

  public String toString() {
    return "VfsDirEntry: " + getVirtualPath() + " - " + (isDirectory() ? "d" : " ") + " - " +
      handlers.size();
  }

 /*********************************************/

  public static java.util.List toList(java.io.File[] dirContent, boolean includeSubDirs) {
    java.util.Vector dirEntries = new java.util.Vector(dirContent.length);

    for(int n = 0; n < dirContent.length; n++) {

      if(dirContent[n].isDirectory() && !includeSubDirs)
	continue;

      VfsDirEntry dirEnt = new VfsDirEntry();
      dirEnt.setHandler(new VfsHandler(dirContent[n].getAbsolutePath(), null));
      dirEnt.setDirectory(dirContent[n].isDirectory());
      dirEntries.add(dirEnt);
    }

    return dirEntries;
  }

  
 /*********************************************/
  private String virtualName;
  private String virtualPath;
  private java.util.List /*<VfsHandler>*/ handlers;

  private boolean isVirtual;
  private boolean isDir;
  private boolean isHidden;
}
