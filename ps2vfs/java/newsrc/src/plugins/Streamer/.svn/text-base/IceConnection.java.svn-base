package ps2vfs.pluggable.streamer;

public class IceConnection 
{
  private static final boolean debug = false;
  private static final int maxLineBufferSize = 2048;
  private boolean validConnection = false;
  private java.util.Map headerMap;
  private java.net.HttpURLConnection conn;
  private java.io.InputStream inputStream = null;
  private java.net.URL url = null;

  public IceConnection(java.net.URL iceUrl) 
    throws java.io.IOException 
  {
    url = iceUrl;
    reconnect();
  }
  
  java.io.InputStream getDataStream() 
    throws java.io.IOException 
  {
    if(inputStream != null)
      return inputStream;
    return conn.getInputStream();
  }

  public boolean isValid() { return validConnection; }

  public java.util.Map getHeaders() {
    return headerMap;
  }

  public String getHeader(String name) {
    java.util.List val = (java.util.List) headerMap.get(name);
    if(val != null)
      return val.toString();
    else 
      return null;
  }
  
  public void reconnect() 
    throws java.io.IOException 
  {
    validConnection = false;
    headerMap = null;;
    inputStream = null;

    try {
      if(conn != null) 
	conn.disconnect();
    } catch(Throwable e) {
    }

    conn = (java.net.HttpURLConnection) url.openConnection();
    conn.setRequestProperty("User-Agent", "PS2VFS-PS Stream Relayer/1.0");
    conn.setRequestProperty("Connection", "close");
    conn.setInstanceFollowRedirects(true);
    try {
      conn.connect();
    } catch (java.io.IOException e) {
      headerMap = new java.util.HashMap();
      String message = e.getMessage();
      if(message == null)
	message = e.toString();
      headerMap.put("ERROR: ", message);
      validConnection = false;
      return;
    }
    if(debug) {
      System.out.println("Http Headers: " + conn.getHeaderFields());
    }
    parseHeaders();
  }

  public void close() 
    throws java.io.IOException 
  {
    conn.disconnect();
  }

  private void parseHeaders() {

    try {
      if(conn.getResponseCode() != -1) {
	System.err.println("HTTP returned: " + conn.getResponseCode() + " msg: " + conn.getResponseMessage());
	headerMap = conn.getHeaderFields();
	return; // Nothing more to do. The HTTP class has taken care of the headers.
      }
 
      headerMap = new java.util.HashMap();

      validConnection = false;
      java.io.InputStream inp = conn.getInputStream();
      byte[] bbuf = new byte[maxLineBufferSize];
      
      String line = null;
      boolean headerComplete = false;
      boolean firstHeader = true;
      int offset = 0;
      int startPos = 0;
      int lineEndPos = 0;
      int len = 0;
      boolean useMark = inp.markSupported();
      
      if(useMark)
	inp.mark(maxLineBufferSize);
      len = inp.read(bbuf, 0, maxLineBufferSize);
      
      // System.err.println("Read " + len + " bytes at " + offset);
      
      while(!headerComplete) {
	offset = 0;
	if(len < 0) {
	  break;
	}
	if(len > 0) {
	  startPos = lineEndPos;
	  
	  for(;lineEndPos < len;lineEndPos++) {
	    if(bbuf[lineEndPos] == '\n') {
	      // Found end of line.
	      break;
	    }
	  }
	  /*
	  System.err.println("Line found from " + startPos + 
			     " to " + lineEndPos);
	  */
	  int lineLen = 0;
	  if(lineEndPos < len && bbuf[lineEndPos] == '\n') {
	    lineEndPos++;
	    lineLen = lineEndPos-startPos;
	  }
	  
	  if(lineLen == 0) {
	    // Not enough data.
	    if(debug) {
	      System.err.println("Did not complete the header in the first " + 
				 len + " " + lineEndPos + "/" + startPos);
	    }
	    
	    if(len >= maxLineBufferSize) {
	      headerComplete = true;
	      // System.err.println("Giving up.");
	    } else {
	      offset = lineEndPos - startPos;
	      if(startPos > 0 ) {
		System.arraycopy(bbuf, startPos, bbuf, 0, offset);
	      }
	      startPos = 0;
	      lineEndPos = 0;
	      if(useMark)
		inp.mark(maxLineBufferSize);

	      len = inp.read(bbuf, offset, maxLineBufferSize-offset);
	      if(len < 0) {
		headerComplete = true;
	      }
	      len += offset;
	      offset = 0;
	      continue;
	    }
	  } else {
	    line = new String(bbuf, startPos, lineLen);
	  }
	  
	  if(line != null) {
	    if(firstHeader) {
	      if(line.startsWith("ICY 200 OK")) {
		firstHeader = false;
		validConnection = true;
	      } else {
		if(debug) {
		  System.err.println("Did not find expected header: '" + 
				     line.trim() + "'");
		}
		headerComplete = true;
	      }
	    } else { 
	      if (line.equals("\r\n") || line.equals("\n")) {
		headerComplete = true;
		offset = lineEndPos;
		// System.err.println("End of header at " + offset);
	      } else {
		int fieldNameEnd = line.indexOf(':');
		
		if(fieldNameEnd > 0) {
		  String fieldName = line.substring(0, fieldNameEnd).trim();
		  String fieldValue = line.substring(fieldNameEnd+1).trim();
		  
		  java.util.List valueList = (java.util.List)headerMap.get(fieldName);
		  if(valueList == null)
		    valueList = new java.util.ArrayList(1);
		  
		  valueList.add(fieldValue);
		  headerMap.put(fieldName, valueList);

		} else {
		  if(debug) {
		    System.err.println("Ignoring malformed header: " + line);
		  }
		}
	      }
	    }
	  }
	}
      }
    
      len -= offset;
      
      if(len > 0) {
	if(useMark) {
	  inp.reset();
	  inp.skip(offset);
	} else {
	  // Setup a byte input stream and the true input stream inside a
	  // SequenceInputStream.
	  inputStream = new java.io.SequenceInputStream(new java.io.ByteArrayInputStream(bbuf, offset, len),
							inp);
	}
      }
    } catch(Throwable e) {
      headerMap = new java.util.HashMap();
      String message = e.getMessage();
      if(message == null)
	message = e.toString();
      headerMap.put("ERROR: ", message);
      validConnection = false;
    }
  }

}
