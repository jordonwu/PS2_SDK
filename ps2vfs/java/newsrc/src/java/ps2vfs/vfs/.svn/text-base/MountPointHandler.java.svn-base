package ps2vfs.vfs;

import ps2vfs.plugin.*;

class MountPointHandler implements VfsPlugin
{
  private MountPoint mp;
  
  MountPointHandler(MountPoint iMP) {
    mp = iMP;
  }
  
  public java.util.List readDir(String path) {
    // System.out.println("MountPointHandler::readDir (" + path + ") in " + mp);
    return mp.resolveDir(); 
  }

  public String getName() {
    return null;
  }

  public String getDescription() {
    return null;
  }

  public String getProtocol() {
    return null;
  }

  public boolean init(java.util.logging.Logger iLog) {
    return true;
  }
  
  public void filterDir(ps2vfs.plugin.VfsDir dir) {
    
  }
  public ps2vfs.plugin.VfsOpenFile open(String path) {
    return null;
  }

  public void doAbout(Object mainFrame) {
  }
  
  public void doConfigure(Object mainFrame) {
  }


}
