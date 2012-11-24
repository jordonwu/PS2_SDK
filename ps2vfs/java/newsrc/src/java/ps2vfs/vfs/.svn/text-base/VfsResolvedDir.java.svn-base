package ps2vfs.vfs;

public class VfsResolvedDir {
  
  VfsResolvedDir() {
    dirContent = null;
    virtualPath = "";
    isRoot = false;
  }
  
  VfsResolvedDir(java.util.List iDirContent, String vPath, boolean iIsRoot) {
    dirContent = iDirContent;
    virtualPath = vPath;
    isRoot = iIsRoot;
  }

  public void add(VfsResolvedDir rootDir) {
    if(isRoot && rootDir.isRoot) {
      // Do we need to check the virtualPath???
      dirContent.addAll(rootDir.getDirContent());
    } else {
      // Throw something here???
      // this is a usage error.
    }
  }
  
  public java.util.List getDirContent() { return dirContent; }
  public String getVirtualPath() { return virtualPath; }
  public boolean isRootDir() { return isRoot; }

  private java.util.List dirContent;
  private String virtualPath;
  private boolean isRoot;
};
