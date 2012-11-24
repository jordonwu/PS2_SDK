package ps2vfs.pluggable.vfsrelay;

import java.util.Vector;

class Connection
{
  private final static String charset = "ISO-8859-1";

  private String serverHostname;
  private int    serverPortNum;
  private java.io.BufferedReader reader;
  private ps2vfs.server.Ps2VfsClient serverClient;
  private java.net.Socket clientSocket;
  private java.io.DataOutputStream out; 
  private java.io.DataInputStream in; 
  private ps2vfs.plugin.VfsPlugin handler;
  private String                  baseURI;

  Connection(ps2vfs.plugin.VfsPlugin iHandler, String iBaseURI) 
  {
    handler = iHandler;
    baseURI = iBaseURI;
  }

  public void connect(String host, int port) 
    throws java.net.UnknownHostException,
    java.io.IOException
  {
    serverHostname = host;
    serverPortNum = port;
    reconnect();
  }

  private void reconnect()     
    throws java.net.UnknownHostException,
    java.io.IOException
  {
    clientSocket = new java.net.Socket(serverHostname, serverPortNum);
    reader = new java.io.BufferedReader(new java.io.InputStreamReader(System.in));
    serverClient = new ps2vfs.server.Ps2VfsClient(clientSocket, null);
    serverClient.initClientCommand();
    in  = serverClient.getInStream();
    out = serverClient.getOutStream();
  }
  

  int open(String filename)  
    throws Exception
  {
    int fd = -1;
    byte[] pathBytes = filename.getBytes(charset);
    
    System.arraycopy(serverClient.returnInt(pathBytes.length), 0,
		     serverClient.cmdOpen, 0, 4);
    out.write(serverClient.cmdOpen, 0, 9);
    out.write(pathBytes);
    out.flush();
    
    String response = serverClient.readCommandString();
    if(response.equals(serverClient.OPEN)) {
      fd = serverClient.getCode();
    } else {
      throw new Exception("Open returned: " + response + "(" + serverClient.getCode() + ")");
    }
    return fd; 
  }

  public int seek(int fd, int len, int whence)  
    throws Exception
  {
    int pos = -1;
    System.arraycopy(serverClient.returnInt(fd),  0,
		     serverClient.cmdSeek, 0, 4);
    out.write(serverClient.cmdSeek, 0, 9);
    byte[] data = new byte[4*2];
    System.arraycopy(serverClient.returnInt(len), 0,
		     data, 0, 4);
    System.arraycopy(serverClient.returnInt(whence), 0,
		     data, 4, 4);
    out.write(data);
    out.flush();
    
    String response = serverClient.readCommandString();
    if(response.equals(serverClient.SEEK)) {
      pos = serverClient.getCode();
      if(pos < 0) 
	System.out.println("Seek returned: " + pos);
    } else {
      throw new Exception("Seek returned: " + response + "(" + serverClient.getCode() + ")");
    }
    return pos;
  }

  public int read(int fd, byte[] buffer, int offset, int len)  
    throws Exception
  {
    int resultLen = 0;
    System.arraycopy(serverClient.returnInt(fd),  0,
		     serverClient.cmdRead, 0, 4);
    out.write(serverClient.cmdRead, 0, 9);
    byte[] data = new byte[4];
    System.arraycopy(serverClient.returnInt(len), 0,
		     data, 0, 4);
    out.write(data);
    out.flush();
    
    String response = serverClient.readCommandString();
    if(response.equals(serverClient.READ)) {
      resultLen = serverClient.getCode();
      if(resultLen < 0)
	System.out.println("Read returned: " + resultLen);
    } else {
      throw new Exception("Read returned: " + response + "(" + serverClient.getCode() + ")");
    }
    if(resultLen > 0) 
      in.readFully(buffer, offset, resultLen);
    /*
    if(readLen != resultLen)
      System.out.println("Read returned " + resultLen + ", but only " + readLen + 
			 " was actually returned");
    */
    return resultLen;
  }

  public void close(int fd) 
    throws Exception
  {
    int pos = -1;
    System.arraycopy(serverClient.returnInt(fd),  0,
		     serverClient.cmdClose, 0, 4);
    out.write(serverClient.cmdClose, 0, 9);
    out.flush();
    
    String response = serverClient.readCommandString();
    if(response.equals(serverClient.CLOSE)) {
      pos = serverClient.getCode();
      if(pos < 0)
	System.out.println("Close returned: " + pos);
    } else {
      throw new Exception("Close returned: " + response + "(" + serverClient.getCode() + ")");
    }
  }


  java.util.List readDir(String path) 
    throws Exception
  {
    // System.out.println("PWD of " + path);
    java.util.List dirContent = null;
    byte[] pathBytes = null;
    int numEntries = 0;

    if(path.equals("/")) {
      pathBytes = null;
    } else {
      String sendPath = path;
      if(path.charAt(0) == '/') {
	sendPath = path.substring(1);
      }
      //System.err.println("dir of path: " + sendPath);
      pathBytes = sendPath.getBytes(charset);
    }

    int numPathBytes = 0;
    if(pathBytes != null) {
      numPathBytes = pathBytes.length;
    }
    System.arraycopy(serverClient.returnInt(numPathBytes), 0,
		     serverClient.cmdPwd, 0, 4);
    out.write(serverClient.cmdPwd, 0, 9);
    if(numPathBytes != 0)
      out.write(pathBytes);
    out.flush();
    
    String response = serverClient.readCommandString();
    if(response.equals(serverClient.PWD)) {
      numEntries = serverClient.getCode();
      /*      if(numEntries < 0)
	      System.out.println("PWD returned: " + numEntries);
      */
    } else {
      throw new Exception("PWD returned: " + response + 
			  "(" + serverClient.getCode() + ")");
    }
    
    // Read entry data;
    if(numEntries >= 0) {
      byte[] entries = new byte[ps2vfs.server.Ps2VfsClient.maxDirEntrySize*
				ps2vfs.server.Ps2VfsClient.maxDirEntries];
      
      // Trying short mode first, i.e. read just what we need.
      int entriesToRead = numEntries + 1;
      if(entriesToRead > ps2vfs.server.Ps2VfsClient.maxDirEntries) {
	entriesToRead = ps2vfs.server.Ps2VfsClient.maxDirEntries;
      }
      in.readFully(entries, 0, entriesToRead*ps2vfs.server.Ps2VfsClient.maxDirEntrySize);

      dirContent = new Vector();
      for(int n = 0; n < numEntries; n++) {
	int entryOffset = n*ps2vfs.server.Ps2VfsClient.maxDirEntrySize;
	if(entries[entryOffset] == 0xff) {
	  // This is the end marker, so we just end the printing here.
	  break;
	}
	boolean isDir = ((entries[entryOffset] & 0x02) != 0);
	int len = ps2vfs.server.Ps2VfsClient.maxDirEntryNameLength;
	while(entries[entryOffset + len] == 0)
	  len--;
	
	String entryName = new String(entries, entryOffset + 1, len);
	// System.out.println("Flags: " + (long)entries[entryOffset] + " name: " + entryName);
	ps2vfs.plugin.VfsDirEntry dirEnt = new ps2vfs.plugin.VfsDirEntry();
	if(isDir) {
	  dirEnt.setVirtualDir(true);
	}

	if(!path.endsWith("/"))
	  path = path + "/";
	
	String uriStr = new java.net.URI("ps2vfs", null, serverHostname, serverPortNum,
					 path + entryName, null, null).toString();
	// System.out.println("Setting open path: " + uriStr);
	dirEnt.setHandler(new ps2vfs.plugin.VfsHandler(uriStr, handler));
	dirEnt.setVirtualName(entryName);
	dirContent.add(dirEnt);
      }

      if(in.available()>0 || !ps2vfs.server.Ps2VfsClient.shortDirMode) {
	int remainingBytes = (ps2vfs.server.Ps2VfsClient.maxDirEntries - 
			      entriesToRead)*ps2vfs.server.Ps2VfsClient.maxDirEntrySize;
	
	if(ps2vfs.server.Ps2VfsClient.shortDirMode) {
	  System.err.println("More data available, assuming long mode.... ");
	}
	if(remainingBytes > 0)
	  in.readFully(entries, 0, remainingBytes);
	
	if(ps2vfs.server.Ps2VfsClient.shortDirMode) {
	  System.err.println("Full PWD mode ended.");
	}
      }
    }
    return dirContent;
  }

  /** Does a 'exit' command. */
  public void exit() throws Exception
  {
    System.arraycopy(serverClient.returnInt(0), 0,
		     serverClient.cmdExit, 0, 4);
    out.write(serverClient.cmdExit, 0, 9);
    out.flush();
  }
}
