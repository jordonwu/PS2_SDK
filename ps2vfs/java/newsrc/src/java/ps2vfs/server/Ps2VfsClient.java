package ps2vfs.server;

import java.net.*;
import java.io.*;
import java.util.*;

/*
//This import for support jakarta vfs for command dir, pwf, and next features
import org.apache.commons.vfs.FileObject;
import org.apache.commons.vfs.FileSystemException;
import org.apache.commons.vfs.FileSystemManager;
import org.apache.commons.vfs.FileType;
import org.apache.commons.vfs.FileUtil;
import org.apache.commons.vfs.VFS;
import org.apache.commons.vfs.Selectors;
*/

public class Ps2VfsClient extends Thread {

  //Strings for our commands
  public String HELLO="HELLO";
  public String OPEN="OPENA";
  public String CLOSE="CLOSE";
  public String READ="READA";
  public String SEEK="SEEKA";
  public String EXIT="EXITA";
  public String DIR="DOPEN";
  public String PWD="PWDAA";
  public String IOERROR="IOERR";

  //Byte structure for our commands
  public byte cmdHello[];
  public byte cmdOpen[];
  public byte cmdClose[];
  public byte cmdRead[];
  public byte cmdSeek[];
  public byte cmdExit[];
  public byte cmdDir[];
  public byte cmdPwd[];

  public final static boolean shortDirMode = false;
  public final static int maxDirEntries = 512;
  public final static int maxDirEntrySize = 128;
  public final static int maxDirEntryNameLength = maxDirEntrySize - 1;
  
  /*
  //For pwd and dir commands we need a jakarta VFS FileSystemManager and a FileObject
  private  FileSystemManager mgr;

  */
  private String charset = "ISO-8859-1";
  private ps2vfs.vfs.Ps2Vfs vfs;
  private ps2vfs.plugin.VfsDir cwd;
  private java.util.logging.Logger log;

  //Our eyes in java side
  PrintWriter screenOut = new PrintWriter(System.out, true);

  private Socket socket = null;
  private String clientIP = null;

  //our data io 
  private DataInputStream in;
  private DataOutputStream out;
	
  private String commandString;
  private long numbytes;
  private int mode;
  private int code;
  private boolean done=false;

  public Ps2VfsClient(Socket socket, java.util.logging.Logger iLog) {
    this.socket = socket;
    this.log = iLog;

    initCommand();

    //open our io
    try {
      this.in=new DataInputStream(socket.getInputStream());
      this.out=new DataOutputStream(socket.getOutputStream());
    } catch(IOException e) {			
      log.warning("Error initalizing IO");
    }
  }

  public Ps2VfsClient(ps2vfs.vfs.Ps2Vfs iVfs, Socket socket, java.util.logging.Logger iLog) {
    super("Ps2VfsClient");
    this.socket = socket;
    this.vfs = iVfs;
    this.log = iLog;

    //initVFS(); //for command pwd and dir
    try {
      cwd = vfs.getDirContent("/", null, false);
    } catch(FileNotFoundException e) {
      log.warning(e.toString());
    }
    initCommand(); //init our commands
    //open our io
    try {
      this.in=new DataInputStream(socket.getInputStream());
      this.out=new DataOutputStream(socket.getOutputStream());
    } catch(IOException e) {			
      log.warning("Error initalizing IO");
    }
  }
  
  public String readCommandString()
  {
    byte command[]=new byte[5];
    try {
      code=in.readInt();
      in.read(command);
      return new String(command);
    } catch(IOException e) {
      log.warning("Client IO error: " + e.toString());
      return IOERROR;  
    }
  }

  public DataOutputStream getOutStream() {
    return out;
  }
  public DataInputStream getInStream() {
    return in;
  }

  public int getCode() 
  {
    return code;
  }

  public String getClientIP() {
    return clientIP;
  }

  /*
    
    private void initVFS()
    {
    
    try
    {
	mgr = VFS.getManager();
	cwd = mgr.resolveFile( System.getProperty( "user.dir" ) );
	filesopened=new Hashtable();
	numfilesopened=0;
	screenOut.println("New Vfs manager");
	screenOut.println("Hashtable fileopened created");
	screenOut.println("Number of files opened in ps2 side "+numfilesopened);
      }
    catch ( Exception e )
      {
	e.printStackTrace();
	System.exit( 1 );
      }

  }
  */

  public void Hello()
  {   
    try {
      //hello command only called with initial conection response with 0+HELLO
      log.finer("Sending HELLO to " + clientIP);
      System.arraycopy(returnInt(0),0,cmdHello,0,4);
      this.out.write(cmdHello,0,9);
      this.out.flush();
    }
    catch(IOException e) {
      log.finer("Error IO processing command HELLO");
    }
  }

  public  void Exit()
  {
    try {   //exit command only called to end conection response with 0+EXITA
      log.finer("Sending EXIT to " + clientIP);
      System.arraycopy(returnInt(0),0,cmdExit,0,4);
      this.out.write(cmdExit,0,9);
      this.out.flush();
    }
    catch(IOException e) {
      log.finer("Error IO processing command EXIT");
    }

  }

  public void Open()
  {
    //open command we read filename and try to open in read mode(default mode now)
    //if succes response fileid+OPENA 
    //error reponse -1+OPENA
    int fd = -1;
    try{
      byte name []=new byte[this.code];
      this.in.read(name);
      String filename=new String(name, charset);
      filename = NormalizePathSep(filename);
      
      log.fine("Receive OPEN(" + filename + ") from " + clientIP + " cwd: " + cwd);
      
      ps2vfs.plugin.VfsDir parent = cwd;
      ps2vfs.plugin.VfsDirEntry fileEntry = null;
      if(parent != null) {
	fileEntry = parent.getEntry(filename);
      }
      if(fileEntry == null) {
	log.finer("Not found in pwd: " + 
		  (parent != null ? parent.getPath() : "not set"));
	if(filename.charAt(0) == '/') {
	  try {
	    parent = vfs.getDirContent(filename, null, true);
	  } catch(FileNotFoundException e) {
	    log.warning(e.toString());
	    parent = null;
	  }
	}
	if(parent != null) {
	  fileEntry = parent.getEntry(filename);
	}
      } else {
	log.finer("Found in pwd: " + parent.getPath());
      }
      if(fileEntry != null) {
	fd = vfs.openFile(fileEntry, this);
      }

      // Remember files opened by this connection so we can do
      // some house cleaning when/if this connection is 
      // closed/broken.

      System.arraycopy(returnInt(fd),0,cmdOpen,0,4);
      this.out.write(cmdOpen,0,9);
      this.out.flush();
      log.fine("Sending OPEN(" + fd + ") to " + clientIP);
      return;
    } catch (IOException e) {
      log.warning("IOException caught while opening file: " + e);
    } catch (ps2vfs.vfs.TooManyOpenFilesException e ) { //catch first exception for file not found
      log.warning("TooManyOpenFilesException: " + e);
    } catch (Throwable e) {
      log.warning("Open general error: " + e);
      e.printStackTrace();
    }
    
    // Something went wrong since we are here.
    fd=-1;
    try{
      System.arraycopy(returnInt(fd),0,cmdOpen,0,4);
      this.out.write(cmdOpen,0,9);
      this.out.flush();
    } catch (IOException e2 ) {
      log.severe("Error IO processing command OPEN");
    }
  }

  public void Close()
  {
    //command for closing file
    Integer fdint=new Integer(this.code);
    log.fine("Receive CLOSE(" + fdint + ") from " + clientIP);
    int returnCode = -1;
    try {
      vfs.closeFile(fdint.intValue());
      returnCode=0;
    } catch(Throwable e) {
      // Should be IOError or something similar.
      log.warning("Close non-open fd " + fdint + " from " + clientIP + ": " + e);
      e.printStackTrace(System.err);
    }
    try {
      System.arraycopy(returnInt(returnCode),0,cmdClose,0,4);
      this.out.write(cmdClose,0,9);
      this.out.flush();
    } catch ( IOException e1 ) {
      e1.printStackTrace();
      returnCode=-1;
      try{
	System.arraycopy(returnInt(returnCode),0,cmdClose,0,4);
	this.out.write(cmdClose,0,9);
	this.out.flush();
      }catch (IOException e2 ) {
	e2.printStackTrace();
	log.finer("Error IO processing command CLOSE");
      }
    }
  }

  public void  Read()
  {
    //command for read we have already fileid and now is time to read size 
    int returnCode = -1;
    int fdint = this.code;
    int size;
    byte buffer[]=null;
    try {
      int numbytes = in.readInt();
      log.finer("Receive READ(" + fdint + ", " + numbytes + ") from " + clientIP);
      ps2vfs.plugin.VfsOpenFile of = vfs.getFileFromDescriptor(fdint);

      buffer = new byte[numbytes];
      returnCode = of.read(buffer, 0, numbytes);

      if(returnCode == 0) {
	log.warning("Read returned 0. This should never happen as it markes the end of the file in the PS2Reality world.");
      }

      if(returnCode < 0) {
	// PS2Reality does not recognize -1 as end of file.
	// Need to translate it to 0, which is the end of file in
	// the ps2reality world.
	returnCode = 0; 
      }
    } catch(Throwable e) {
      log.warning("READ failed: " + e);
      returnCode=0;
    }
    
    try {
      log.finer("Sending READ(" + returnCode + ") to " + clientIP);
      System.arraycopy(returnInt(returnCode),0,cmdRead,0,4);
      if(returnCode>0) {
	// we send our command Number of bytes read+ READA and our bytes... 
	this.out.write(cmdRead,0,9);
	this.out.write(buffer,0,returnCode);
	this.out.flush();
      } else {
	this.out.write(cmdRead,0,9);
	this.out.flush();
      }
    } catch ( IOException e1 ) {
      returnCode=0; // End of file!
      try{
	System.arraycopy(returnInt(returnCode),0,cmdRead,0,4);
	this.out.write(cmdRead,0,9);
	this.out.flush();
      } catch (IOException e2 ) {
	log.warning("Error IO processing command READ");
      }	
    }
  }

  public void  Seek()
  {
    //command seek now we read offset and whence
    int numBytes;
    int modeSeek;
    long lnumbytes;
    long currentpos;
    int returnCode = -1;
    int fdint = this.code;
    
    try{
      numBytes= in.readInt();
      modeSeek= in.readInt();
      log.fine("Receive SEEK(" + fdint + ", " + numBytes + ", SEEK_" + 
	       (modeSeek == 0 ? "SET" : (modeSeek == 1 ? "OFF" : (modeSeek == 2 ? "END" : "NA"))) + 
	       ") from " + clientIP);
      
      lnumbytes=(long)numBytes;
      
      ps2vfs.plugin.VfsOpenFile of = vfs.getFileFromDescriptor(fdint);
      returnCode = of.seek(numBytes, modeSeek);
    } catch(Throwable e) {
      log.warning("SEEK failed");
      returnCode=-1;
    }
    System.arraycopy(returnInt(returnCode),0,cmdSeek,0,4);
    try {
      log.fine("Sending SEEK(" + returnCode + ") to " + clientIP);
      this.out.write(cmdSeek,0,9);
      this.out.flush();
    } catch ( IOException e1 ) {
      returnCode=-1;
      try{
	System.arraycopy(returnInt(returnCode),0,cmdSeek,0,4);
	this.out.write(cmdSeek,0,9);
	//this.out.write(returnInt(returnCode),0,4);
	//this.out.write(SEEK.getBytes(),0,5);
	this.out.flush();
      }catch (IOException e2 ) {
	log.warning("Error IO processing command SEEK");
      }
    }
  }
  
  public void Pwd() throws IOException
  {
    int size;
    try {
      String dir = "/";
      if(this.code > 0) {
	byte name[] = new byte[this.code];
	this.in.read(name);
	dir = dir + new String(name, charset);
	dir = NormalizePathSep(dir);
      }
      boolean found = true;

      log.fine("Receive PWD(" + dir + ") from " + clientIP);
      try {
	cwd = vfs.getDirContent(dir, null, false);
      } catch(FileNotFoundException e) {
	log.fine(e.toString());
	found = false;
      }
      /* PS2Reality seems to want 128 bytes per entry 
       * the first byte is a control byte:
       *   0x00 = Entry is a file
       *   0x02 = Entry is a directory
       *   0xff = End of entries
       * The rest of the block is the name.
       */
      if(found) {
	ps2vfs.plugin.VfsDirEntry[] entries = cwd.listEntries();
	System.arraycopy(returnInt(entries.length),0,cmdPwd,0,4);
	this.out.write(cmdPwd,0,9);
	log.fine("Sending PWD(" + entries.length + ") to " + clientIP);
	
	
	int numEntries = entries.length;
	if(numEntries > maxDirEntries) {
	  numEntries = maxDirEntries;
	}
	int i = 0; 
	byte[] outBuf = new byte[maxDirEntrySize*maxDirEntries];
	int lastIndex = numEntries-1;
	for(i = 0; i < numEntries; i++) {	
	  String name=entries[i].getVirtualName();
	  outBuf[i*maxDirEntrySize] = (byte) ((entries[i].isDirectory() ? 0x02 : 0x00) | 
					      (i == lastIndex ? 0x80 : 0x00));
	  byte[] nameBytes = name.getBytes(charset);
	  int nameLength = nameBytes.length;
	  if(nameLength > maxDirEntryNameLength) {
	    nameLength = maxDirEntryNameLength;
	    log.warning("Name to long for PWD entry, truncating without regard to extensions '" + 
			name + "'." );
	  }
	  System.arraycopy(nameBytes, 0, outBuf, i*maxDirEntrySize + 1, nameLength);
	}
	if(i < maxDirEntries) {
	  outBuf[i*maxDirEntrySize] = (byte)0xff; // End of list marker
	}
	if(shortDirMode)
	  this.out.write(outBuf, 0, (i+1)*maxDirEntrySize);
	else
	  this.out.write(outBuf);
      } else {
	System.arraycopy(returnInt(-1),0,cmdPwd,0,4);
	this.out.write(cmdPwd,0,9);
	log.fine("Sending PWD(-1) to " + clientIP);
      }
      this.out.flush();
      return;
    } catch ( IOException e ) {
      log.warning("PWD failed: " + e);
      e.printStackTrace();
    } catch ( Throwable e) {
      log.warning("PWD failed: " + e);
      e.printStackTrace();
    }
    
    try {
      System.arraycopy(returnInt(-1),0,cmdPwd,0,4);
      this.out.write(cmdPwd,0,9);
    } catch (IOException e2 ) {
      log.warning("Error IO processing command PWD");
    }
  }

  public void Dir()
  {
    int size;
    try{
      //we can add here name directory to read, now only list default directory
      String dir=cwd.getPath();
      int path=dir.length();
      
      //FileObject realDirs[] = cwd.getChildren();
      ps2vfs.plugin.VfsDirEntry[] dirs = cwd.listEntries();

      System.arraycopy(returnInt(dirs.length),0,cmdDir,0,4);
      this.out.write(cmdDir,0,9);
      for(int i=0;i<dirs.length;i++) {	
	String name=dirs[i].getVirtualName();
	this.out.write(returnInt(name.length()),0,4);
	this.out.write(name.getBytes(),0,name.length());
      }
    } catch ( Exception e ) {
      e.printStackTrace();
      try{
	System.arraycopy(returnInt(-1),0,cmdDir,0,4);
	this.out.write(cmdDir,0,9);
      }catch (IOException e2 ) {
	log.warning("Error IO processing command DIR");
      }
    }
  }

  public void run(){
		
		
    clientIP = socket.getInetAddress().getHostAddress();
    log.finer("PS2 Client connected from: " + clientIP);
    try {
      while(!done) {   
	commandString=readCommandString();
	log.finer("Receive cmd: " + commandString + " code= " + code);
	switch(commandString.charAt(0)) {
	case 'H': 
	case 'h': 
	  log.finer("command Hello called ");
	  Hello();
	  break;
	case 'O': 
	case 'o': 
	  log.finer("command Open called ");
	  Open();
	  break;
	case 'C':
	case 'c':
	  log.finer("Ps2Vfs Close called ");
	  Close();
	  break;
	case 'R': 
	case 'r': 
	  log.finer("Ps2Vfs Read called ");
	  Read();
	  break;
	case 'S':
	case 's':
	  log.finer("Ps2Vfs Seek called ");
	  Seek();
	  break;
	case 'D':
	case 'd':
	  log.finer("Ps2Vfs Dir called ");
	  Dir();
	  break;
	case 'P':
	case 'p':
	  log.finer("Ps2Vfs Pwd called ");
	  Pwd();
	  break;
	default:
	  if(commandString.equals(IOERROR)) {
	    done = true;
	  } else {
	    log.finer("Command not recognized:"+commandString);
	  }
	  break;
	case 'E': 
	case 'e': 
	  log.finer("Ps2Vfs Exit called ");
	  Exit();
	  done=true;
	  break;
	}
      }
    } catch (IOException e) {
      log.warning("Client IO error: " + e.toString());
    }

    try {
      this.in.close();
      this.out.close();
      this.socket.close();
    } catch(IOException e) {
      log.warning("Error closing IO");
    }
    vfs.closeAllClientFiles(this);
  }

  String NormalizePathSep(String path) {
    return path.replaceAll("\\\\", "/");
  }

  public byte[] returnInt(int a)
  {
    byte array[]=new byte[4];

    array[0]=(byte)(a >> 24 & 0xff);
    array[1]=(byte)(a >> 16 & 0xff);
    array[2]=(byte)(a >> 8 & 0xff);
    array[3]=(byte)(a >> 0 & 0xff);
    return array;
  }

  //initializing byte structure for our commands 
  public void initCommand()
  {
    cmdHello=new byte[9];
    cmdOpen=new byte[9];
    cmdClose=new byte[9];
    cmdRead=new byte[9];
    cmdSeek=new byte[9];
    cmdExit=new byte[9];
    cmdPwd=new byte[9];
    cmdDir=new byte[9];
    System.arraycopy(HELLO.getBytes(),0,cmdHello,4,5);
    System.arraycopy(OPEN.getBytes(),0,cmdOpen,4,5);
    System.arraycopy(CLOSE.getBytes(),0,cmdClose,4,5);
    System.arraycopy(READ.getBytes(),0,cmdRead,4,5);
    System.arraycopy(SEEK.getBytes(),0,cmdSeek,4,5);
    System.arraycopy(EXIT.getBytes(),0,cmdExit,4,5);
    System.arraycopy(PWD.getBytes(),0,cmdPwd,4,5);
    System.arraycopy(DIR.getBytes(),0,cmdDir,4,5);
  }

  //initializing byte structure for our commands 
  public void initClientCommand()
  {
    cmdHello=new byte[9];
    cmdOpen=new byte[9];
    cmdClose=new byte[9];
    cmdRead=new byte[9];
    cmdSeek=new byte[9];
    cmdExit=new byte[9];
    cmdPwd=new byte[9];
    cmdDir=new byte[9];
    System.arraycopy(HELLO.toLowerCase().getBytes(),0,cmdHello,4,5);
    System.arraycopy(OPEN.toLowerCase().getBytes(),0,cmdOpen,4,5);
    System.arraycopy(CLOSE.toLowerCase().getBytes(),0,cmdClose,4,5);
    System.arraycopy(READ.toLowerCase().getBytes(),0,cmdRead,4,5);
    System.arraycopy(SEEK.toLowerCase().getBytes(),0,cmdSeek,4,5);
    System.arraycopy(EXIT.toLowerCase().getBytes(),0,cmdExit,4,5);
    System.arraycopy(PWD.toLowerCase().getBytes(),0,cmdPwd,4,5);
    System.arraycopy(DIR.toLowerCase().getBytes(),0,cmdDir,4,5);
  }
			
}
