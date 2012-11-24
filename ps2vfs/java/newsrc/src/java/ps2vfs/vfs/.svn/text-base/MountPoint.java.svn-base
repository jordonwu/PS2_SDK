package ps2vfs.vfs;

import ps2vfs.plugin.*;

public class MountPoint 
{
  private String  openPath;
  private boolean recursive;
  private boolean virtual;
  private boolean hidden;
  private static boolean   debug = false;
  private java.util.Vector children = null;
  private MountPoint       parent = null;

  public static class DirComp
    implements java.util.Comparator
  {
    public int compare(Object e1, Object e2) {
      MountPoint de1 = (MountPoint) e1;
      MountPoint de2 = (MountPoint) e2;
      
      if(de1.isVirtual() == de2.isVirtual()) {
	return de1.getOpenPath().compareTo(de2.getOpenPath());
      } else if(de1.isVirtual()) {
	return -1;
      } else
	return 1;
    }
  }

  MountPoint(String iname) {
    openPath =  iname;
    virtual = true;
    recursive = true;
    children = new java.util.Vector(0);
  }
  
  MountPoint(String  iPath, 
	     boolean iRecursive,
	     boolean iHidden) {
    openPath = iPath;
    recursive = iRecursive;
    hidden = iHidden;
    virtual = false;
  }

  public MountPoint addChild(MountPoint child) {
    int idx = children.indexOf(child);
    if(idx < 0) {
      children.add(child);
      child.setParent(this);
    } else {
      child = (MountPoint) children.get(idx);
    }
    if(debug)
      System.out.println("MP addChild: " + child);
    return child;
  }
  
  public void removeChild(MountPoint child) {
    children.remove(child);
    prune();
  }
  
  public java.util.List getChildren() {
    return children;
  }
  
  public MountPoint getChild(MountPoint child) {
    int idx = children.indexOf(child);
    if(idx >= 0) {
      return (MountPoint) children.get(idx);
    }
    return null;
  }
  
  public MountPoint getParent() {
    return parent;
  }

  public MountPoint setParent(MountPoint iparent) {
    parent = iparent;
    return parent;
  }
  
  void prune() {
    if(children.size() == 0 && parent != null) {
      parent.removeChild(this);
    }
  }

  public String toString() {
    if(virtual) {
      return "MPv: " + openPath;
    } else {
      return "MP : " + (recursive ? "r" : "-") + (hidden?"h":"-") + " : " + openPath;
    }
  }

  public boolean equals(Object o) throws ClassCastException {
    boolean eq = false;
    MountPoint mp = (MountPoint) o;
    eq = mp.openPath.equals(openPath);
    if(eq) {
      if(virtual) {
	eq = mp.virtual;
      } else {
	eq = (mp.recursive == recursive);
      }
    }
    return eq;
  }


  public java.util.List /*<java.plugin.VfsDirEntry>*/ resolveDir() {
    java.util.List fileVec = null;

    if(debug) {
      System.out.println("Resolving mp " + this);
    }
    
    if(children.size() != 0) {
      fileVec = new java.util.Vector(1);
      java.util.Iterator it = children.iterator();
      while(it.hasNext()) {
        MountPoint mp = (MountPoint) it.next();
	if(mp.isVirtual()) {
	  if(debug)
	    System.out.println("Adding virtual dir: '" + mp + "'");
	  
	  VfsDirEntry vDirEnt = new VfsDirEntry();
	  vDirEnt.setVirtualName(mp.getOpenPath());
	  vDirEnt.setDirectory(true);
	  vDirEnt.setHandler(new ps2vfs.plugin.VfsHandler(mp.getOpenPath(),
							  new MountPointHandler(mp)));  
	  fileVec.add(vDirEnt);
	} else {
	  java.io.File dir = new java.io.File(mp.getOpenPath());
	  if(dir.isDirectory()) {
	    if(debug) {
	      System.out.println("Reading content of dir: " +  
				 dir.getAbsolutePath() + " mp: " + mp);
	    }
	    java.io.File[] files = dir.listFiles();
	    java.util.List content = VfsDirEntry.toList(files, mp.isRecursive());
	    if(content != null) 
	      fileVec.addAll(content);
	  } else {
	    Ps2Vfs vfs = Ps2Vfs.getVfs();
	    java.util.List content = vfs.resolveURI(mp.getOpenPath());
	    if(content != null) 
	      fileVec.addAll(content);
	    /*
	    java.util.logging.Logger.getLogger("ps2vfs").warning("Not a supported path: " + 
								 dir.getAbsolutePath() + " mp: " + mp);
	    */
	  }
	}
      }
    }
    return fileVec;
  }

  public String  getOpenPath() { return openPath; }
  public boolean isRecursive() { return recursive; }
  public boolean isVirtual() { return virtual; }
  public boolean isHidden() { return hidden; }
};


