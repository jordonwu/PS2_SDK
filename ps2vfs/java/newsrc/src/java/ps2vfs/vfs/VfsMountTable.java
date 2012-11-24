package ps2vfs.vfs;

import ps2vfs.plugin.*;



public class VfsMountTable 
{
  private static boolean debug = false;
  private java.util.logging.Logger log;
  private java.util.HashMap mountTable;
  private MountPoint rootMP = null;
  private Ps2Vfs     vfs = null;

  public static void setDebug(boolean val) {
    debug = val;
  }

  VfsMountTable() {
    mountTable = new java.util.HashMap(); 
    rootMP = new MountPoint("/");
    log = java.util.logging.Logger.getLogger("ps2vfs");
    vfs = Ps2Vfs.getVfs();
  }

  public MountTableIterator iterator() {
    return new MountTableIterator(mountTable.entrySet().iterator());
  }

  String NormalizePathSep(String path) {
    return path.replaceAll("\\\\", "/");
  }
  
  public boolean addMountPoint(String virtualPath, String actualPath, 
			       boolean addPathNameToVirtual, boolean recursive, 
			       boolean hidden) 
  {
    virtualPath = NormalizePathSep(virtualPath);
    actualPath = NormalizePathSep(actualPath);

    if(addPathNameToVirtual) {
      // Get name of dir and add it as a virtul MP dir element.
      java.io.File filePath = new java.io.File(actualPath);
      String fileName = filePath.getName();
      virtualPath += virtualPath.endsWith("/") ? "" : "/"  + fileName;
    }

    if(debug) {
      System.err.println("addMountPoint: " + virtualPath);
    }

    int schemeIdx = actualPath.indexOf("://");
    if(schemeIdx > 0 ) {
      String scheme = actualPath.substring(0, schemeIdx);
      int pathIdx = actualPath.indexOf("/", schemeIdx + 3);
      String host = null;
      String path = null;

      if(pathIdx > 0) {
	host = actualPath.substring(schemeIdx + 3, pathIdx);
	path = actualPath.substring(pathIdx);
      }
      // System.out.println("Adding URI: " + scheme + "://" + host + path);
      try {
	java.net.URI uri = new java.net.URI(actualPath);
      } catch(Exception e) {
	// Malformed URI. Try to fix.
	try {
	  java.net.URI uri = new java.net.URI(scheme, host, path, null, null);
	  actualPath = uri.toString();
	} catch(Exception ee) {
	  log.warning("Failed to convert to URI: " + actualPath + "(" + ee + ")");
	}
      }
    }

    String[] pathElements = virtualPath.split("/+");
    //    System.out.println("pe (" + pathElements.length + "): ");
    
    MountPoint curMP = rootMP;
    for(int n = 0; n < pathElements.length; n++) {
      if(debug)
	System.out.println(pathElements[n]);
      if(pathElements[n] == null || pathElements[n].length() == 0)
        continue;
      curMP = curMP.addChild(new MountPoint(pathElements[n]));
    }
    mountTable.put(virtualPath, curMP);
    
    curMP.addChild(new MountPoint(actualPath, recursive, hidden));
    if(vfs == null) 
      vfs = Ps2Vfs.getVfs();
    if(vfs != null)
      vfs.clearCache();
    return true;
  }

  public boolean removeMountPoint(String virtualPath, String actualPath, 
				  boolean recursive, boolean hidden) {

    boolean removed = false;

    if(debug) {
      System.err.println("removeMountPoint("+virtualPath+")");
    }
    
    MountPoint curMP = (MountPoint) mountTable.get(virtualPath);
    if(curMP != null) {
      curMP.removeChild(new MountPoint(actualPath, 
				       recursive, hidden));
      if(vfs == null) 
	vfs = Ps2Vfs.getVfs();
      if(vfs != null)
	vfs.clearCache();
      removed = true;
    }

    if(!removed)
      System.err.println("Failed to remove: " + virtualPath);
    return removed;
  }
  
  public ps2vfs.plugin.VfsDir getRootContent() {
    java.util.List dirContent = rootMP.resolveDir();
    return new ps2vfs.plugin.VfsDir("/", dirContent);
  }

  public void print() {
    java.util.Set keys = mountTable.keySet();
    for(java.util.Iterator it = keys.iterator(); it.hasNext();) {
      System.out.println("Key: " + it.next());
    }
  }

  public String[] getChildren(String path) 
  {
    java.util.List mounted = null;
    String[] pathElements = path.split("/+");
    MountPoint curMnt = rootMP;

    boolean pathNotFound = false;
    if(debug) 
      System.err.println("getChildren1: " + path);
    
    for(int n = 0; n < pathElements.length; n++) {
      if(pathElements[n] == null || pathElements[n].length() == 0)
        continue;
      if(debug)
	System.out.println("getChildren2: " + pathElements[n]);
      curMnt.getChild(new MountPoint(pathElements[n]));
      MountPoint child = curMnt.getChild(new MountPoint(pathElements[n]));
      if(child == null) {
	if(debug)
	  System.err.println("getChildren: element not found " + pathElements[n]);
	pathNotFound = true;
	break;
      } 
      curMnt = child;
    }
    if(pathNotFound) 
      return null;

    if(curMnt != null) 
      mounted = (java.util.List) curMnt.getChildren();
    
    String[] strArray = null;
    if(mounted != null) {
      java.util.ArrayList sortedMpList = new java.util.ArrayList(mounted);
      java.util.Collections.sort(sortedMpList, new MountPoint.DirComp());
      
      int size = sortedMpList.size();
      
      strArray = new String[size];
      int n = 0;
      for(java.util.ListIterator li = sortedMpList.listIterator(); 
	  li.hasNext();) {
	MountPoint mp = (MountPoint) li.next();
	if(mp.isVirtual()) {
	  strArray[n++] = mp.getOpenPath();
	} else {
	  strArray[n++] = mp.toString();
	}
      }
    }
    if(debug)
      System.err.println("getChildrenR: " + strArray);
    return strArray;
  }
};
