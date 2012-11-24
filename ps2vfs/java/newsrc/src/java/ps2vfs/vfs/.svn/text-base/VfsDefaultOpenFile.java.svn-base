package ps2vfs.vfs;

public class VfsDefaultOpenFile implements ps2vfs.plugin.VfsOpenFile
{
  private java.io.FileInputStream openFile;
  private String openFilePath;
  private java.nio.channels.FileChannel channel;
  private static final boolean debug = false;

  public VfsDefaultOpenFile(String filename) 
    throws java.io.FileNotFoundException
  {
    openFilePath = filename;
    openFile = new java.io.FileInputStream(filename);
    channel = openFile.getChannel();
  }

  public int seek(int len, int whence) 
    throws java.io.IOException
  {
    if(openFile == null || channel == null)
      return -1;

    long size = channel.size();
    long newPos = 0;
    
    if(whence == SEEK_BEGIN) {
      newPos = len;
    } else if(whence == SEEK_CUR) {
      newPos = len + channel.position();
    } else if(whence == SEEK_END) { 
      newPos = len + size;
    }
    if(newPos < 0)
      newPos = 0;
    else if(newPos > size)
      newPos = size;
    
    if(newPos > Integer.MAX_VALUE) {
      // The PS2VFS protocol is currently limited to 2GB files.
      newPos = Integer.MAX_VALUE;
    }
    channel.position(newPos);
    return (int) channel.position();
  }
  
  public int read(byte[] buf, int offset, int len) 
    throws java.io.IOException
  {
    int ret = -1;
    if(openFile != null && channel != null)
      ret = channel.read(java.nio.ByteBuffer.wrap(buf, offset, len));
    if(false) {
      System.err.println("Reading from file " + openFile + " offset " + offset +
			 " len " + len  + " ret " + ret);
    }
    return ret;
  }
  
  public boolean close() {
    if(openFile != null) {
      try {
	openFile.close();
      } catch(java.io.IOException e) {
      }
      openFile = null;
      return true;
    }
    return false;
  }

  public String getOpenPath() {
    return openFilePath;
  }

  public String getInfo() {
    try {
      return "Local file: " + byteSizeToStr(channel.position()) + "/" + 
	byteSizeToStr(channel.size());
    } catch(java.io.IOException e) {
      return "Local file: size unavailable.";
    }
  }
  
  public static String byteSizeToStr(long size) {
    java.text.DecimalFormat df = new java.text.DecimalFormat();
    df.setMaximumFractionDigits(2);
    return "" + 
      ((size > 1024*1024) ? (df.format(size/(1024*1024.0)) + "MB") 
       : ((size > 1024) ? (df.format(size/1024.0) + "KB") 
	  : (size + "B")));
    
  }

}
