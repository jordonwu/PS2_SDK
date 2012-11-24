package ps2vfs.pluggable.streamer;

class StreamOpenFile implements ps2vfs.plugin.VfsOpenFile
{
  private final static int chunkSize = 0x20000;
  private final int readLimit = Integer.MAX_VALUE;
  private final long timeout = 160*1000; // Allow for 8kbps
  
  private SeekBuffer sbuf;
  private IceConnection iceconn;
  private StreamReader reader;
  private String origURL;

  private java.util.logging.Logger log;

  public StreamOpenFile(String urlStr, java.util.logging.Logger ilog)
    throws java.io.FileNotFoundException
  {
    log = ilog;
    log.finer("Connecting to: " + urlStr);
    origURL = urlStr;

    java.net.URL url;
    try {
      url = new java.net.URL(urlStr);
    } catch(java.net.MalformedURLException e) {
      throw new java.io.FileNotFoundException("Malformed url: " + urlStr);
    }

    try {
      iceconn = new IceConnection(url);
    } catch(java.io.IOException e) {
      throw new java.io.FileNotFoundException("Opening stream \"" + urlStr + "\" failed: " + e);
    }    
    
    log.finer("Connection returned: " + iceconn.getHeaders());
    if(!iceconn.isValid() ) {
      throw new java.io.FileNotFoundException("Opening stream \"" + urlStr + "\" failed: " +
					      iceconn.getHeaders());
    }
    
    try {
      sbuf = new SeekBuffer(chunkSize*10, chunkSize*6);
      reader = new StreamReader(iceconn, sbuf, readLimit);
      reader.setLogger(log);
      log.finer("Kicking off stream reader and buffering data");
      reader.start();
      
      sbuf.waitAvailableRead(chunkSize*1, timeout*1);
      log.finer("Data buffered. Ready for action.");
    } catch(java.io.IOException e) {
      throw new java.io.FileNotFoundException("Opening stream \"" + urlStr + "\" failed: " +
					      e.toString());
    }
  }
  
  public int seek(int len, int whence) 
    throws java.io.IOException  
  {
    if(whence == ps2vfs.plugin.VfsOpenFile.SEEK_END) {
      // Fake seek to end. It will just give the
      // maximum length of this stream.
      // Typically set to MAX_VALUE, but can be
      // configured to less.
      if(len >= 0)
	return readLimit;
      else
	return readLimit + len;
    } 
    else if(whence == ps2vfs.plugin.VfsOpenFile.SEEK_BEGIN) {
      // Seek from start.
      return sbuf.seek(len);
    }

    // Unsupported seek mode
    return -1;
  }

  public int read(byte[] buf, int offset, int len) 
    throws java.io.IOException
  {
    int readBytes = 0;
    try {
      readBytes = sbuf.readFully(buf, offset, len, timeout);
    } catch(java.io.InterruptedIOException e) {
      readBytes = e.bytesTransferred;
      if(readBytes == 0) {
	sbuf.setDry();
	iceconn.close();
      }
    }
    return readBytes;
  }

  public boolean close() {
    log.finer("Closing stream");
    reader.endReader();
    reader.interrupt();
    try {
      reader.join();
    } catch(InterruptedException e) {;}

    try {
      iceconn.close();
    } catch(java.io.IOException e) {
      log.warning("Error closing streamer file: " + e);
      return false;
    }
    return true;
  }

  public String getOpenPath() {
    return origURL;
  }

  public String getInfo() {
    if(iceconn.isValid()){
      return "Name: " + iceconn.getHeader("icy-name") + 
	" Bit rate: " + iceconn.getHeader("icy-br") + "kbps" +  
	" Buffer: " + sbuf.fillPercentage() + " (" + 
	nicePrintByteSize(sbuf.availableRead()) + ")" +
	"                   (Headers: " + iceconn.getHeaders() + ")";
    } else {
      java.util.Map headers = iceconn.getHeaders();
      if(headers != null) {
	return "Error: " + headers.toString();
      }
    }
    return "Error: Not connected.";
  }
  
  private String nicePrintByteSize(int size) {
    java.text.DecimalFormat df = new java.text.DecimalFormat();
    df.setMaximumFractionDigits(2);
    return "" + 
      ((size > 1024*1024) ? (df.format(size/(1024*1024.0)) + "MB") 
       : ((size > 1024) ? (df.format(size/1024.0) + "KB") 
	  : (size + "B")));
    
  }
}
