package client;

public class Ps2VfsTestClient 
{
  private final static String charset = "ISO-8859-1";

  private final static int CHUNK_SIZE = 0x20000;
  private final static int MAX_FILE_SIZE = 20*1024*1024;
  
  private String serverHostname;
  private int    serverPortNum;
  private java.io.BufferedReader reader;
  private String cwd = "/";
  private ps2vfs.server.Ps2VfsClient serverClient;
  private java.net.Socket clientSocket;
  private java.io.DataOutputStream out; 
  private java.io.DataInputStream in; 
  private int sleepTime = 0;

  private void setSleepTime(int st) {
    sleepTime = st;
  }

  public static void main(final String[] args) {
    try {
      Ps2VfsTestClient client = new Ps2VfsTestClient(args[0], args[1]);
      if(args.length == 3) {
	client.setSleepTime(Integer.parseInt(args[2]));
      }
      client.go();
    }
    catch(Exception e ) {
      e.printStackTrace();
      System.exit( 1 );
    }
    System.exit( 0 );
  }

  private Ps2VfsTestClient(String hostname, String portno) 
    throws java.net.UnknownHostException,
    java.io.IOException
  {
    serverPortNum = Integer.parseInt(portno);
    serverHostname = hostname;
    
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
  
  private void go() throws Exception
  {
    System.out.println("Ps2Vfs Test Client");
    while(true) {
      final String[] cmd = nextCommand();
      if(cmd == null) {
	return;
      }
      if(cmd.length == 0) {
	continue;
      }
      final String cmdName = cmd[0];
      if (cmdName.equalsIgnoreCase("exit") || cmdName.equalsIgnoreCase("quit")) {
	exit();
	return;
      }
      try {
	handleCommand(cmd);
      } catch (final Exception e) {
	System.err.println( "Command failed:" );
	e.printStackTrace( System.err );
      }
    }
  }
  
  /** Handles a command. */
  private void handleCommand(final String[] cmd) 
    throws Exception
  {
    final String cmdName = cmd[0];
    if(cmdName.equalsIgnoreCase("get")) {
      get(cmd);
    }
    else if(cmdName.equalsIgnoreCase("cd")) {
      lsAndCd(cmd);
    }
    else if (cmdName.equalsIgnoreCase("help")) {
      help();
    }
    else if (cmdName.equalsIgnoreCase("ls" )) {
      lsAndCd(cmd);
    }
    else if (cmdName.equalsIgnoreCase("pwd")) {
      pwd();
    }
    else if (cmdName.equalsIgnoreCase("reconnect")) {
      reconnect();
    }
    else {
      System.err.println( "Unknown command \"" + cmdName + "\"." );
    }
  }
  
  /** Does a 'help' command. */
  private void help()
  {
    System.out.println( "Commands:" );
    System.out.println( "get <file>         Download the contents of a file." );
    System.out.println( "cd [folder]        Changes current folder." );
    System.out.println( "help               Shows this message." );
    System.out.println( "ls [path]          Lists contents of a file or folder." );
    System.out.println( "pwd                Displays current folder." );
    System.out.println( "exit       Exits this program." );
    System.out.println( "quit       Exits this program." );
  }
  
  /** Does a 'get' command. */
  private void get(final String[] cmd) throws Exception
  {
    int fd = -1;
    if (cmd.length < 2) {
      throw new Exception( "USAGE: get <path> [local-path]" );
    }
    String filename = cmd[1];
    if(filename.charAt(0) != '/' && filename.charAt(0) != '\\') {
      filename = cwd + filename;
    }
    String localFilename = null;
    if(cmd.length > 2) {
      localFilename = cmd[2];
    } else {
      localFilename = (new java.io.File(filename)).getName();
    }
    
    byte[] pathBytes = filename.getBytes(charset);
    
    System.arraycopy(serverClient.returnInt(pathBytes.length), 0,
		     serverClient.cmdOpen, 0, 4);
    out.write(serverClient.cmdOpen, 0, 9);
    out.write(pathBytes);
    out.flush();
    
    String response = serverClient.readCommandString();
    if(response.equals(serverClient.OPEN)) {
      fd = serverClient.getCode();
      if(fd < 0)
	System.out.println("Open returned: " + fd);
    } else {
      throw new Exception("Open returned: " + response + "(" + serverClient.getCode() + ")");
    }
    
    if(fd >= 0) {
      download(fd, localFilename);
    }
  }

  private void download(int fd, String localFile) 
    throws Exception 
  {
    int totalSize = 0;
    int size = 0;
    byte[] buffer = new byte[CHUNK_SIZE];
    int readSize = 0;

    size = seek(fd, 0, 2);
    seek(fd, 0, 0);
    int size2 = seek(fd, 0, 2);
    seek(fd, 0, 0);
    if(size != size2) {
      // Inconsistent results.
      System.out.println("Seek end returned inconsistent results: " + size + "/" + size2);
      size = size2;
    }
    java.text.DecimalFormat df = new java.text.DecimalFormat();
    df.setMaximumFractionDigits(2);
    /*
    System.out.println("Size of file is: " + 
		       ((size > 1024*1024) ? (df.format(size/(1024*1024.0)) + "MB") 
			: ((size > 1024) ? (df.format(size/1024.0) + "KB") 
			   : (size + "B"))));
    */

    // Simulate the way PS2Reality reads files.
    for(totalSize = 0; totalSize < size && totalSize < CHUNK_SIZE*4; totalSize += readSize) {
      readSize = read(fd, buffer, 0, CHUNK_SIZE);
      if(readSize <= 0) 
	break; // End of file
    }
    
    seek(fd, 0, 0);
    System.out.println("Starting download to: " + localFile);
    java.io.FileOutputStream ostrm = new java.io.FileOutputStream(localFile, false);

    if(size > MAX_FILE_SIZE) {
      size = MAX_FILE_SIZE;
      System.out.println("Truncating length of file to " + MAX_FILE_SIZE);
    }
      
    for(totalSize = 0; totalSize < size; totalSize += readSize) {
      if(sleepTime != 0) 
	Thread.sleep(sleepTime);

      readSize = read(fd, buffer, 0, CHUNK_SIZE);
      //System.out.println((new java.util.Date()).toString() + " Read " + readSize + " bytes");
      if(readSize <= 0)
	break; // End of file
      ostrm.write(buffer, 0, readSize);
    }
    //readSize = read(fd, buffer, 0, CHUNK_SIZE);
    ostrm.close();

    System.out.println("Got: " + 
		       ((totalSize > 1024*1024) ? (df.format(totalSize/(1024*1024.0)) + "MB") 
			: ((totalSize > 1024) ? (df.format(totalSize/1024.0) + "KB") 
			   : (totalSize + "B"))));
    close(fd);
  }
  
  private int seek(int fd, int len, int whence)  
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

  private int read(int fd, byte[] buffer, int offset, int len)  
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

  private void close(int fd) 
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


  /** Does a 'exit' command. */
  private void exit() throws Exception
  {
    System.arraycopy(serverClient.returnInt(0), 0,
		     serverClient.cmdExit, 0, 4);
    out.write(serverClient.cmdExit, 0, 9);
    out.flush();
  }

  /** Does a 'ls' or a 'cd' command. */
  private void lsAndCd(final String[] cmd) throws Exception
  {
    if (cmd.length < 1) {
      throw new Exception( "USAGE: <cmd>" );
    }
    boolean cd = false;
    if(cmd[0].equalsIgnoreCase("cd")) {
      cd = true;
      if(cmd.length < 2) {
	throw new Exception( "USAGE: cd path" );
      }
    }
    
    String path = cwd;
    byte[] pathBytes = null;
    int numEntries = 0;

    if(cmd.length >= 2) {
      path = cmd[1];
      if(path.equals("..")) {
	// Looking for parent
	int parentSep = cwd.lastIndexOf('/');
	// System.err.println("cwd:" + cwd + " " + parentSep);
	if(parentSep > 0)
	  parentSep = cwd.lastIndexOf('/', parentSep - 1);

	if(parentSep > 0)
	  path = cwd.substring(0, parentSep);
	else 
	  path = "/";
	// System.err.println("cwd:" + cwd + " parent: " + path);
      }
      if(path.charAt(0) != '/')
	path = cwd + path;
    }
    
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
      if(numEntries < 0)
	System.out.println("PWD returned: " + numEntries);
      if(cd) {
	if(numEntries >= 0) {
	  cwd = path;
	  if(!cwd.endsWith("/")) {
	    cwd = cwd + "/";
	  }
	}
      } 
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

      if(!cd) {
	// Print content.
	for(int n = 0; n < numEntries; n++) {
	  int entryOffset = n*ps2vfs.server.Ps2VfsClient.maxDirEntrySize;
	  if(entries[entryOffset] == 0xff) {
	    // This is the end marker, so we just end the printing here.
	    break;
	  }
	  boolean isDir = (entries[entryOffset] & 0x02) != 0;
	  int len = ps2vfs.server.Ps2VfsClient.maxDirEntryNameLength;
	  while(entries[entryOffset + len] == 0)
	    len--;
	  
	  String entryName = new String(entries, entryOffset + 1, len);
	  if(isDir)
	    entryName = entryName + "/";
	  System.out.println(entryName);
	}
      }
      if(in.available()>0 || !ps2vfs.server.Ps2VfsClient.shortDirMode) {
	int remainingBytes = (ps2vfs.server.Ps2VfsClient.maxDirEntries - 
			      entriesToRead)*ps2vfs.server.Ps2VfsClient.maxDirEntrySize;

	if(ps2vfs.server.Ps2VfsClient.shortDirMode) {
	  System.err.println("More data available, assuming long mode.... ");
	}
	/*
	System.err.println("Reading remaining overhead " + remainingBytes + "(" + 
			   (entriesToRead*ps2vfs.server.Ps2VfsClient.maxDirEntrySize) + 
			   "/" + 
			   (remainingBytes + 
			    entriesToRead*ps2vfs.server.Ps2VfsClient.maxDirEntrySize) +
			   "/" + (ps2vfs.server.Ps2VfsClient.maxDirEntrySize*
				  ps2vfs.server.Ps2VfsClient.maxDirEntries) + ")" + 
			   " bytes.");
	*/
	if(remainingBytes > 0)
	  in.readFully(entries, 0, remainingBytes);
	
	if(ps2vfs.server.Ps2VfsClient.shortDirMode) {
	  System.err.println("Full PWD mode ended.");
	}
      }
    }
  }
  
  /** Does a 'pwd' command. */
  private void pwd()
  {
    System.out.println( "Current folder is " + cwd);
  }
  

  /** Returns the next command, split into tokens. */
  private String[] nextCommand() 
    throws java.io.IOException 
  {
    System.out.print( "> " );
    
    final String line = reader.readLine();
    if (line == null) {
      return null;
    }
    final EscapedStringTokenizer escTokens = new EscapedStringTokenizer(line);
    return escTokens.toArray();
  }
}
