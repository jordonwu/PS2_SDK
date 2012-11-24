package ps2vfs.pluggable.streamer;

import java.io.*;

public class StreamReader extends Thread {
  private IceConnection conn;
  private SeekBuffer    buf;
  private final int     maxLen; 
  private final int     bufferSize = 4096;
  private boolean       readData = true;

  private java.util.logging.Logger log = null;

  public StreamReader(IceConnection conn, SeekBuffer sbuf, int maxLen) {
    this.conn = conn;
    buf = sbuf;
    this.maxLen = maxLen;
  }

  public void setLogger(java.util.logging.Logger ilog) {
    log = ilog;
  }

  public void endReader() {
    readData = false;
  }

  public void run() {
    int totalSize = 0;
    try {
      int len;
      InputStream inp = conn.getDataStream();
      byte[] bbuf = new byte[bufferSize];
      while(totalSize < maxLen && readData) {
	len = inp.read(bbuf, 0, bufferSize);
	if(len < 0) {
	  // End of file (stream).
	  if(log != null)
	    log.warning("Stream reader: " + len);
	  buf.setDry();
	  break;
	} else {
	  totalSize += len;
	  //if(log != null)
	  //  log.finest("Stream reader got: " + len);
	  buf.writeFully(bbuf, 0, len, 0);
	}
      }
    } catch(IOException e) {
      if(log != null) {
	log.warning("Stream reader: " + e);
      }
      e.printStackTrace(System.err);
      buf.setDry();
    }
    if(log != null) {
      log.finer("Stream reader finished: " + totalSize);
    }
  }
}
