package ps2vfs.vfs;


class LogFormatter extends java.util.logging.Formatter
{
  public LogFormatter(boolean iDebug) {
    debug = iDebug;
  }

  public void setDebug(boolean iDebug) {
    debug = iDebug;
  }

  public boolean getDebug() {
    return debug;
  }
  
  public String format(java.util.logging.LogRecord rec) {
    StringBuffer logStrBuf = new StringBuffer();
    
    logStrBuf.append("[" + new java.util.Date(rec.getMillis()) + "]");
    logStrBuf.append(" " + rec.getLevel() + " -");
    if(debug) {
      logStrBuf.append(" " + rec.getSourceClassName() + " " + rec.getSourceMethodName());
    }
    logStrBuf.append(" - " + rec.getMessage()); 
    if(debug) {
      Throwable e = rec.getThrown();
      if(e != null) {
	logStrBuf.append(" T: " + e);
      }
    }
    logStrBuf.append("\n");
    return logStrBuf.toString();
  }

  private boolean debug;
}
