package ps2vfs.pluggable.vfsrelay;

class VfsRelayOpenFile implements ps2vfs.plugin.VfsOpenFile
{
  private java.util.logging.Logger log;
  private Connection               conn;
  private int                      fid;
  private String                   path;
  private long                     totalReadBytes = 0;

  public VfsRelayOpenFile(String iPath, Connection iConn, java.util.logging.Logger ilog)
    throws java.io.FileNotFoundException
  {
    log = ilog;
    conn = iConn;
    path = iPath;
    
    log.finer("Opening file: " + path);
    
    try {
      fid = conn.open(path);
    } catch (Exception e) {
      fid = -1;
    }
    if(fid < 0) {
      throw new java.io.FileNotFoundException(path);
    }
  }
  
  public int seek(int len, int whence) 
    throws java.io.IOException  
  {
    try {
      return conn.seek(fid, len, whence);
    } catch(Exception e) {
      throw new java.io.IOException("seek failed");
    }
  }

  public int read(byte[] buf, int offset, int len) 
    throws java.io.IOException
  {
    try {
      int readBytes = conn.read(fid, buf, offset, len);
      if(readBytes >= 0)
	totalReadBytes += readBytes;
      return readBytes;
    } catch(Exception e) {
      throw new java.io.IOException("read failed");
    }
  }

  public boolean close() {
    boolean ok = false;
    try {
      conn.close(fid);
      ok = true;
    } catch(Exception e) {}
    fid = -1;
    try {
      conn.exit();
    } catch(Exception e) {}
    return ok;
  }

  public String getOpenPath() {
    return path;
  }

  public String getInfo() {
    return "Remote vfs file (bytes read: " + nicePrintByteSize(totalReadBytes) + ")";
}
  
  private String nicePrintByteSize(long size) {
    java.text.DecimalFormat df = new java.text.DecimalFormat();
    df.setMaximumFractionDigits(2);
    return "" + 
      ((size > 1024*1024) ? (df.format(size/(1024*1024.0)) + "MB") 
       : ((size > 1024) ? (df.format(size/1024.0) + "KB") 
	  : (size + "B")));
    
  }
}
