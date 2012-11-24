package ps2vfs.vfs;

public class Parameters {
  private final String propFileName   = "ps2vfs.props";

  private final String serverPort     = "ps2vfs.server.port";
  private final String serverCollapse = "ps2vfs.server.collapse";
  private final String serverCacheMaxSize = "ps2vfs.server.cache.maxsize";
  private final String serverCacheMaxAge  = "ps2vfs.server.cache.maxage";
  private final String serverLogLevel = "ps2vfs.server.loglevel";
  private final String serverFileLogLevel = "ps2vfs.server.file.loglevel";

  private final String mntPointNum    = "ps2vfs.mountTable.entries";
  private final String mntPointPrefix = "ps2vfs.mountTable.mnt"; 
  private final String mntPointVPath  = ".virtualPath";
  private final String mntPointPPath  = ".physicalPath";
  private final String mntPointAttrs  = ".attributes";

  private final String serverDebugVfs = "ps2vfs.server.debug.vfs";
  private final String serverDebugMnt = "ps2vfs.server.debug.mount";
  
  public Parameters() { props_ = new java.util.Properties(); }

  public void loadParameters() {
    try {
      java.io.FileInputStream in = new java.io.FileInputStream(propFileName);
      props_.load(in);
      in.close();
    } catch(Throwable e) {
      e.printStackTrace(System.out);
    }
  }

  public void saveParameters() {
    try {
      java.io.FileOutputStream out = new java.io.FileOutputStream(propFileName);
      props_.store(out, null);
      out.close();
    } catch(Throwable e) {
      e.printStackTrace(System.out);
    }
  }
  
  public boolean getCollapseDirs() {
    String str = props_.getProperty(serverCollapse);
    boolean collapse = true;
    if(str != null) {
      collapse = Boolean.valueOf(str).booleanValue();
    }
    return collapse;
  }

  public boolean getDebugVfs() {
    String str = props_.getProperty(serverDebugVfs);
    boolean val = false;
    if(str != null) {
      val = Boolean.valueOf(str).booleanValue();
    }
    return val;
  }

  public boolean getDebugMnt() {
    String str = props_.getProperty(serverDebugMnt);
    boolean val = false;
    if(str != null) {
      val = Boolean.valueOf(str).booleanValue();
    }
    return val;
  }

  public int getPort() {
    String numEntStr = props_.getProperty(serverPort);
    int numEnt = 0;
    if(numEntStr != null) {
     try {
       numEnt = Integer.parseInt(numEntStr);
     } catch(NumberFormatException e) {
       System.out.println("Failed to read port number from " + numEntStr);
       numEnt = 0;
     }
    }
    if(numEnt == 0) 
      numEnt = 6969;
    return numEnt;
  }

  public void setPort(int port) {
    props_.setProperty(serverPort, new String(""+port));
  }

  public int getCacheMaxSize() {
    String numEntStr = props_.getProperty(serverCacheMaxSize);
    int numEnt = 0;
    if(numEntStr != null) {
      try {
	numEnt = Integer.parseInt(numEntStr);
      } catch(NumberFormatException e) {
	System.out.println("Failed to read '" + serverCacheMaxSize + "' number from " + numEntStr);
	numEnt = 0;
      }
    }
    if(numEnt <= 0) 
      numEnt = 100;
    return numEnt;
  }

  public long getCacheMaxAge() {
    String numEntStr = props_.getProperty(serverCacheMaxAge);
    long numEnt = 0;
    if(numEntStr != null) {
      try {
	numEnt = Long.parseLong(numEntStr);
      } catch(NumberFormatException e) {
	System.out.println("Failed to read '" + serverCacheMaxAge + "' number from " + numEntStr);
	numEnt = 0;
      }
    }
    if(numEnt <= 0) 
      numEnt = 10*60*1000; // 10 minutes as default maxAge!
    else 
      numEnt *= 60*1000;
    return numEnt;
  }

  public String getServerLogLevel() {
    String retVal =  props_.getProperty(serverLogLevel);
    if(retVal == null)
      return "INFO";
    return retVal;
  }

  public void setServerLogLevel(String level) {
    props_.setProperty(serverLogLevel, level);
  }

  public String getServerFileLogLevel() {
    String retVal =  props_.getProperty(serverFileLogLevel);
    if(retVal == null)
      return "OFF";
    return retVal;
  }

  public void setServerFileLogLevel(String level) {
    props_.setProperty(serverFileLogLevel,level);
  }
    
  private void clearMntTable() {
    String numEntStr = props_.getProperty(mntPointNum);
    int numEnt = 0;
    if(numEntStr != null) {
      try {
	numEnt = Integer.parseInt(numEntStr);
      } catch(NumberFormatException e) {
	System.out.println("Failed to read number of entries from " + mntPointNum);
	numEnt = 0;
      }
    }
    for(int n=1; n <= numEnt; n++) {
      String propNamePre = mntPointPrefix + n;
      props_.remove(propNamePre + mntPointVPath);
      props_.remove(propNamePre + mntPointPPath);
      props_.remove(propNamePre + mntPointAttrs);
    }
    props_.setProperty(mntPointNum, "0");
  }

  public void setMntTable(VfsMountTable mntTab) {

    clearMntTable();

    MountTableIterator it = mntTab.iterator();
    int mntN = 0;
    while(it.hasNext()) {
      MountTableEntry ent = (MountTableEntry) it.next();
      mntN++;
      MountPoint mnt = ent.getMountPoint();
      String virtualPath = ent.getVirtualPath();
      String propNamePre = mntPointPrefix + mntN;

      props_.setProperty(propNamePre + mntPointVPath, virtualPath);
      props_.setProperty(propNamePre + mntPointPPath, mnt.getOpenPath());
      props_.setProperty(propNamePre + mntPointAttrs, 
			 new String((mnt.isRecursive()?"r":"") +
				    (mnt.isHidden()?"h":"")));
    }
    props_.setProperty(mntPointNum, new String(""+mntN));
  }

  public VfsMountTable getMntTable() {
    VfsMountTable mountTable = new VfsMountTable();
    String numEntStr = props_.getProperty(mntPointNum);
    int numEnt = 0;
    if(numEntStr != null) {
     try {
       numEnt = Integer.parseInt(numEntStr);
     } catch(NumberFormatException e) {
       System.out.println("Failed to read number of entries from " + mntPointNum);
       numEnt = 0;
     }
    }
    
    for(int n=1; n <= numEnt; n++) {
      String propNamePre = mntPointPrefix + n;
      String vpath, ppath, attrs;
      vpath = props_.getProperty(propNamePre + mntPointVPath);
      ppath = props_.getProperty(propNamePre + mntPointPPath);
      attrs = props_.getProperty(propNamePre + mntPointAttrs);
      if(vpath == null || ppath == null || attrs == null)
	continue; // Not valid

      boolean recursive = false;
      boolean hidden = false;

      recursive = attrs.indexOf('r') >= 0;
      hidden = attrs.indexOf('h') >= 0;
      
      mountTable.addMountPoint(vpath, ppath, false, recursive, hidden);
    }
    /*
    mountTable.addMountPoint("/test1", "testdir", false, false, false);
    mountTable.addMountPoint("/test1r", "testdir", false, true, false);
    mountTable.addMountPoint("/test2", "testdir", true, false, false);
    mountTable.addMountPoint("/test2r", "testdir", true, true, false);
    mountTable.addMountPoint("/mixed", "testdir", true, true, false);
    mountTable.addMountPoint("/mixed", "testdir", false, true, false);
    mountTable.addMountPoint("/mixed/pure", "testdir", false, true, true);
    mountTable.addMountPoint("/test4/a/b/c/d/e/", "testdir", false, true, false);
    mountTable.addMountPoint("/test4/a/b/c/f", "testdir", false, true, false);
    mountTable.addMountPoint("/test4/a/b/c/g", "testdir", false, true, false);
    */
    return mountTable;
  }


  private java.util.Properties props_;
};
