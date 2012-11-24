/**
 * This is the interface that all PS2VFS Plugins must implement.
 *
 * @version 0.0.1 
 * @author Stig Petter Olsrød
 */

package ps2vfs.plugin;

public interface VfsOpenFile 
{
  public final static int SEEK_BEGIN = 0;
  public final static int SEEK_CUR = 1;
  public final static int SEEK_END = 2;

  public String getOpenPath();
  public String getInfo();

  public int seek(int len, int whence) throws java.io.IOException;
  public int read(byte[] buf, int offset, int len) throws java.io.IOException;
  public boolean close();
};
