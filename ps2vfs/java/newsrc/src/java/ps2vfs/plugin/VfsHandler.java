package ps2vfs.plugin;

public class VfsHandler
{
  private String path;
  private VfsPlugin handler;

  public VfsHandler(String openPath, VfsPlugin ihandler) 
  {
    path = openPath;
    handler = ihandler;
  }
  
  public VfsPlugin getHandler() {
    return handler;
  }

  public String getOpenPath() {
    return path;
  }
  
  public void setHandler(VfsPlugin pl) {
    handler = pl;
  }

  public String toString() {
    return "VH: " + path + ", " + handler;
  }
  
}
