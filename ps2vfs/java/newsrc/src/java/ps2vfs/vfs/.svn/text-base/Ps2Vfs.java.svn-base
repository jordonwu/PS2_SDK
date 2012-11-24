package ps2vfs.vfs;

// import
import java.util.Vector;
import java.util.List;
import java.util.Arrays;
import java.io.*;
import java.net.URI;
import java.net.URL;
import ps2vfs.plugin.*;


public class Ps2Vfs 
{
  private static Ps2Vfs  vfs = null;
  private static final String versionStr = "1.1.1";
  private static boolean debug = false;
  private boolean enableGUI = true;
  private boolean collapseDirs = true;
  private java.util.logging.FileHandler fhand = null; 
  private java.util.logging.Handler chand = null;
  private int port = 6969;
  private Object mntFrame = null;
  private VfsDirCache cache = null;
  
  private String aboutText = "<html>" + 
  "<img src=\"" + getImageURL("images/player.jpg") + "\"></img>" +
  "<H1>PS2VFS-PS V" + versionStr + "</H1>" + 
  "PS2VFS-PS (Plugin-Support) was written by Krilon (C)2004 (Creditware!)." + 
  "<p>" +
  "<p>" +
  "The PS2 interface and server is based on, with additional adaptions for <BR>" + 
  " actual PS2Reality behavior, " + 
  "the Playstation 2 Virtual File System Example, <BR>see copyright details below." + 
  "<p>" +
  "<p>" +
  "Thanks to the PS2Reality team! Great work guys!" +
  "<p>" +
  "<HR width=\"80%\">" + 
  "<PRE>" + 
  "# Playstation 2 Virtual File System Example\n" + 
  "#  _____     ___ ____\n" + 
  "#   ____|   |    ____|      PSX2 OpenSource Project\n" +
  "#  |     ___|   |____       (C)2003,Bigboss, Mavy & Hermes \n" + 
  "#             (bigboss@ps2reality.net,mavy@ps2reality.net,hermes@ps2reality.net)\n"+ 
  "</PRE>" + 
  "<HR width=\"80%\">" + 
  "<BR><BR>" + 
  // Change-log
  "<h3>ChangeLog</h3>" + 
  "<UL>" + 

  "<LI>Krilon 2004-07-07<UL>" + 
  "<LI>Released V1.1.1</LI>" +
  "<LI>Fixed seek problem (guard against seeks beyond start and end of file).<BR>" + 
  "Thanks to VCi15 for reporting this bug." +

  "</UL><LI>Krilon 2004-07-02<UL>" + 
  "<LI>Major rewrite of the mount table. This rewrite should resolve all known limitations of <BR>" + 
  "the previous version." + 
  "<LI>Allow collapsing of duplicate directories instead of enumerating them. This feature can be <BR>" + 
  "turned off with <i>ps2vfs.server.collapse</i> in the property file (not GUI exposed). " +
  "<LI>Added caching of the virtual directories. The size of the cache and maximum age of the entries <BR>" + 
  "can be changed with <i>ps2vfs.server.cache.maxsize</i> and  <i>ps2vfs.server.cache.maxage</i>. <BR> " + 
  "Maxage is the number of minutes a entry should live in the cache before a refresh is forced. " +

  "</UL><LI>Krilon 2004-06-23<UL>" + 
  "<LI>Inital implementation of plugin protocol resolver." + 
  
  "</UL><LI>Krilon 2004-06-22<UL>" + 
  "<LI>Fixed automatic refresh of OpenFilesDialog when opening and closing files." + 
  "<LI>Added content to About-tab." + 

  "</UL><LI>Krilon 2004-06-21<UL>" + 
  "<LI>Fixed bug in Seek with high negative values." +
  "<LI>Finally made the Configure-tab work (not the worlds best GUI design, but it does the job)." + 
  "<LI>Fixed some minor issues in the ShoutCast client." + 
  "</UL>" + 
  "</html>";
  
  /**
   * This method will return the vfs "manager".
   */

  public static Ps2Vfs getVfs() {
    return vfs;
  }

  public Ps2Vfs(String args[]) 
    throws Exception
  {
    String pluginPath = null;
    boolean printHelp = false;
    int n;
    for(n = 0; n < args.length; n++) {
      if(args[n].charAt(0) == '-') {
	if(args[n].equals("-nw")) 
	  enableGUI = false;
	else if(args[n].equals("-?") || args[n].equals("-help")) {
	  printHelp = true;
	} else if(args[n].equals("-version")) {
	  System.out.println("PS2VFS-PS version \"" + versionStr + "\"");
	  throw new Exception(versionStr);
	}
      } else {
	pluginPath = args[n];
	break;
      }
    }

    if(pluginPath == null) 
      pluginPath = "plugins";

    java.io.File pluginsFile = new java.io.File(pluginPath);
    if(!pluginsFile.isDirectory()) {
      printHelp = true;
    }

    if(printHelp) {
      printUsage();
      if(!pluginsFile.isDirectory()) {
	System.out.println("");
	System.out.println("Plugins directory is not readable: " + pluginsFile.getAbsolutePath());
	System.out.println("");
      }
      throw new Exception();
    }
    
    Parameters params = getParameters();
    cache = new VfsDirCache(params.getCacheMaxSize(), params.getCacheMaxAge());
    numOpenFiles = 0;
    openFiles = new java.util.Hashtable();

    initLogger();
    try {
      initMountTable();
      initPlugins(pluginPath);
    } catch(Throwable ee) {
      ee.printStackTrace(System.err);
      log.warning(ee.toString());
    }
  }

  private void printUsage() {
    System.out.println("Usage: [-options] [PluginPath]");
    System.out.println("");
    System.out.println("where options include:");
    System.out.println("    -nw        run without GUI");
    System.out.println("    -? -help   print this help message");
    System.out.println("    -version   print version and exit");
    System.out.println("");
    System.out.println("    PluginPath path to plugin repository (default is plugins)");
  }

  
  /**
   * This method will return the content of the deepest directory
   * qualified by the virtualPath and the cwd.
   *
   * @param virtualPath     The path to resolve.
   * @param cwd             The cwd to use if the virtualPath is not absolute,
   *                        if this is null the root will be the cwd.
   * @param getParentOfFile Set to true if a file should be allowed as the 
   *                        last element in the path. The parent dir content is returned.
   *
   * @return The content of the directory qualified by the input parameters.
   */
 
  public ps2vfs.plugin.VfsDir getDirContent(String virtualPath, ps2vfs.plugin.VfsDir cwd,
					    boolean getParentOfFile) 
    throws java.io.FileNotFoundException 
  {
    
    if(virtualPath == null)
      return null;

    if(!virtualPath.startsWith("/")) {
      // vp is relative to cwd.
      cwd.getPath();
      virtualPath = cwd.getPath() + "/" + virtualPath;
    }

    String originalPath = virtualPath;

    if(debug)
      log.finer("VP: " + virtualPath + " cwd("  + cwd + ")");

    ps2vfs.plugin.VfsDir cached = cache.get(virtualPath);
    if(cached == null && getParentOfFile) {
      String parentVirtualPath = null;
      int ls = virtualPath.lastIndexOf('/', virtualPath.length()-1);
      if(ls > 0) {
	parentVirtualPath = virtualPath.substring(0,ls);
      }
      if(parentVirtualPath != null)
	cached = cache.get(parentVirtualPath);

      if(cached != null) {
	// Check if a file with the last name is present.
	String name = virtualPath.substring(ls+1);
	if(debug)
	  System.out.println("Looking up (" + name + ") in " + cached);
	VfsDirEntry dirEnt = cached.getEntry(name);
	if(dirEnt == null) {
	  throw new java.io.FileNotFoundException(virtualPath + " at '" + name + "'");
	} else if(dirEnt.isDirectory()) {
	  cached = null;
	}
      }
    }
    
    if(cached != null) {
      return cached;
    }
    cwd = cache.get("/");
    if(cwd == null) {
      cwd = mountTable.getRootContent();
      if(cwd != null) {
	filterDirContent(cwd);
	cache.put(cwd);
      }
    }
    
    String[] pathElements = virtualPath.split("/+");
    List dirContent = null;
    for(int n = 0; n < pathElements.length; n++) {
      if(pathElements[n] == null || pathElements[n].length() == 0)
	continue;
      cached = null;
      VfsDirEntry dirEnt = cwd.getEntry(pathElements[n]);
      boolean lastElement = (n == pathElements.length-1);

      if(debug) {
	log.finer("Looking up path element[" + n + "] = " + pathElements[n] + 
		  ", last=" + lastElement);
	if(dirEnt != null) 
	  printDirEntry(dirEnt);
	else
	  System.out.println("No entry for element");
      }
      
      if(dirEnt == null)
	throw new java.io.FileNotFoundException(originalPath + " at '" + pathElements[n] + "'");
      
      if(dirEnt.isDirectory()) {
	// Select ok.
	cached = cache.get(cwd.getPath() + pathElements[n]);
	if(cached == null) {
	  cwd = resolveDir(cwd.getPath(), dirEnt);
	} else {
	  cwd = cached;
	}
	if(cwd == null) {
	  throw new java.io.FileNotFoundException(originalPath + " at '" + 
						  pathElements[n] + "'");
	}
      } else if(getParentOfFile && lastElement) {
	// file as last element is ok.
      } else {
	// Trying to select a file, invalid path.
	throw new java.io.FileNotFoundException(originalPath + " at '" + 
						pathElements[n] + "'");
      }
      
      if(cached == null) 
	cache.put(cwd);
    }

    return cwd;
  }
  

  private java.util.List localResolve(String openPath) 
  {
    // Try fallback method for opening path.
    java.util.List dirContent = null;
    if(debug)
      log.finest("LRD fallback: " + openPath);
    File dir = new File(openPath);
    if(dir.isDirectory()) {
      if(debug) {
	log.finer("LRD: Reading content of dir: " + dir.getAbsolutePath());
      }
      File[] files = dir.listFiles();
      dirContent = ps2vfs.plugin.VfsDirEntry.toList(files, true);
    } else {
      if(debug)
	log.finer("LRD: Not supported " + openPath);
    }
    return dirContent;
  }

  public ps2vfs.plugin.VfsDir resolveDir(String path, VfsDirEntry dirEnt) 
  {
    if(debug) {
      System.out.print("Resolving: " + path + " in ");
      printDirEntry(dirEnt);
    }

    java.util.List dirContent = null;
    ps2vfs.plugin.VfsDir resolvedDir = null;
    boolean needFallback = true;
    
    if(dirEnt.isDirectory()) {
      java.util.List handlerList = dirEnt.getHandlerList();
      if(debug)
	log.finest("RD: " + handlerList);
      if(handlerList != null) {
	java.util.Iterator it = handlerList.iterator();
	while(it.hasNext()) {
	  VfsHandler handler = (VfsHandler) it.next();
	  if(handler != null) {
	    VfsPlugin plg = handler.getHandler();
	    String op = handler.getOpenPath();
	    // log.finest("RD (handler): " + op + " " + plg);
	    if(plg != null) {
	      java.util.List dc = plg.readDir(op);
	      if(dc != null) {
		if(dirContent == null) 
		  dirContent = new java.util.Vector(0);
		dirContent.addAll(dc);
	      }
	    } else {
	      java.util.List dc = localResolve(op);
	      if(dc != null) {
		if(dirContent == null) 
		  dirContent = new java.util.Vector(0);
		dirContent.addAll(dc);
	      }
	    }
	  }
	}
      }
    }

    if(dirContent != null) {
      resolvedDir = new ps2vfs.plugin.VfsDir(path + dirEnt.getVirtualName() + "/", dirContent);
      if(debug) 
	log.finest("RD returning: " + resolvedDir);
      filterDirContent(resolvedDir);
    }
    return resolvedDir;
  }

  public java.util.List /* <VfsDirEntry> */ resolveURI(String uriStr)
  {
    java.util.List dirContent = null;
    try {
      java.net.URI uri = java.net.URI.create(uriStr);

      /*
	log.finer("URI: " + uri.getScheme() + " at " + uri.getHost() + " port: " + uri.getPort() + 
	" uriStr: " + uriStr);
      */
      VfsPlugin handler = selectProtocolHandler(uri.getScheme());

      if(handler != null) {
	log.finest("Handler found " + handler + " for " + uri.getScheme());
	dirContent = handler.readDir(uriStr);
      } else {
	log.warning("No handler found for " + uri.getScheme());
      }
    } catch(java.lang.Exception e) {
      log.warning("resolveURI failed: " + e);
    }
    return dirContent;
  }
  


  public int openFile(VfsDirEntry fileEnt, ps2vfs.server.Ps2VfsClient client) 
    throws TooManyOpenFilesException, java.io.FileNotFoundException 
  {
    int fd = -1;

    if(fileEnt != null) {
      VfsPlugin handler = fileEnt.getHandler();
      VfsOpenFile ofile = null;
      if(handler != null) {
	ofile = handler.open(fileEnt.getOpenPath());
      } else {
	// Dummy handler (use Jakarta VFS??)
	log.finer("Opening file: " + fileEnt.getVirtualName() + " => " + fileEnt.getOpenPath());
	ofile = new VfsDefaultOpenFile(fileEnt.getOpenPath());
	log.finer("Opened file: " + ofile);
      }
      
      if(ofile != null) {
	fd = getFileDescriptor(new OpenFileDescriptor(ofile, fileEnt.getVirtualPath(), client));
	log.finer("Opened file: " + ofile + " => fd: " + fd);
      } else {
	throw new java.io.FileNotFoundException(fileEnt.getVirtualName());
      }
    }
    if(fd >= 0 && enableGUI) {
      ps2vfs.gui.MountPointsFrame frame = (ps2vfs.gui.MountPointsFrame) mntFrame;
      frame.notifyOpenFilesChanged();
      
    }
    return fd;
  }

  public void closeFile(int fd) {
    VfsOpenFile file = freeFileDescriptor(fd);
    file.close();
    if(enableGUI) {
      ps2vfs.gui.MountPointsFrame frame = (ps2vfs.gui.MountPointsFrame) mntFrame;
      frame.notifyOpenFilesChanged();
    }
  }

  public synchronized VfsOpenFile getFileFromDescriptor(int fd) {
    OpenFileDescriptor ofd = (OpenFileDescriptor) openFiles.get(new Integer(fd));
    if(ofd != null) 
      return ofd.getFile();
    else
      return null;
  }
  
  public synchronized int getFileDescriptor(OpenFileDescriptor ofile) 
    throws TooManyOpenFilesException 
  {
    if(numOpenFiles >= maxOpenFiles) {
      throw new TooManyOpenFilesException();
    }
    numOpenFiles++;

    int fd = numOpenFiles;
    while(fd > 0 && openFiles.containsKey(new Integer(fd)) )
      fd--;

    while(openFiles.containsKey(new Integer(fd))) {
      fd++;
    }
    
    openFiles.put(new Integer(fd), ofile);
    log.finer("Allocating file descriptor " + fd);
    return fd;
  }
  
  public synchronized VfsOpenFile freeFileDescriptor(int fd) {
    Integer fdInt = new Integer(fd);
    OpenFileDescriptor ofd = (OpenFileDescriptor) openFiles.get(fdInt);
    openFiles.remove(fdInt);
    if(numOpenFiles > 0)
      numOpenFiles--;
    log.finer("Freeing file descriptor " + fdInt + " number of open files " + numOpenFiles);
    if(ofd != null) 
      return ofd.getFile();
    else
      return null;
  }

  public synchronized void closeAllClientFiles(ps2vfs.server.Ps2VfsClient client) {
    final int maxRemoveChunk = 10;
    int[] removeFds = new int[maxRemoveChunk];
    int removeN;
    
    do {
      removeN = 0;
      java.util.Iterator it = openFiles.keySet().iterator();
      while(it.hasNext() && removeN < maxRemoveChunk) {
	Integer fdInt = (Integer) it.next();
	OpenFileDescriptor ofd = (OpenFileDescriptor) openFiles.get(fdInt);
	if(ofd.getClient() == client) {
	  removeFds[removeN] = fdInt.intValue();
	  removeN++;
	}
      }
      for(int n = 0; n < removeN; n++) {
	freeFileDescriptor(removeFds[n]);
      }
    } while(removeN > 0);
  }

  public synchronized VfsOpenFileInfo[] getOpenFilesInfo() {
    VfsOpenFileInfo[] info = new VfsOpenFileInfo[numOpenFiles];
    java.util.Iterator it = openFiles.keySet().iterator();
    int n=0;
    while(it.hasNext()) {
      Integer fdInt = (Integer) it.next();
      OpenFileDescriptor ofd = (OpenFileDescriptor) openFiles.get(fdInt);
      info[n] = new VfsOpenFileInfo(fdInt.intValue(), ofd.getClient().getClientIP());
      VfsOpenFile of = ofd.getFile();
      info[n].setInfo(of.getInfo());
      info[n].setVPath(ofd.getVirtualPath());
      info[n].setOPath(of.getOpenPath());
      n++;
    }
    return info;
  }


  private VfsPlugin selectProtocolHandler(String protocol) 
  {
    if(protocol == null) 
      return null;

    java.util.Iterator it = plugins.iterator();
    while(it.hasNext()) {
      VfsPlugin plugin = (VfsPlugin) it.next();
      String pluginProto = plugin.getProtocol();
      if(pluginProto != null && protocol.equals(pluginProto))
	return plugin;
    }
    return null;
  }

  public void filterDirContent(ps2vfs.plugin.VfsDir dir) {
    java.util.Iterator it = plugins.iterator();
    while(it.hasNext()) {
      VfsPlugin plugin = (VfsPlugin) it.next();
      plugin.filterDir(dir);
    }
    dir.sort();
    dir.uniqify(collapseDirs);
    printDirContent(dir);
  }
  
  public void printDirEntry(VfsDirEntry entry) {
      System.out.print(entry.getVirtualName());
      System.out.print(" | ");
      if(entry.isDirectory())
	System.out.print("d");
      else
	System.out.print("-");
      if(entry.isVirtualDir())
	System.out.print("v");
      else
	System.out.print("-");
      
      System.out.print(" | " + entry.getOpenPath() + " | ");
      System.out.println("" + entry.getHandler());
  }

  public void printDirContent(ps2vfs.plugin.VfsDir dir) {
    
    if(!debug) {
      return;
    }
    System.out.println("Content of " + dir.getPath());

    VfsDirEntry[] entries = dir.listEntries();
    for(int n = 0; n < entries.length; n++) {
      System.out.print(entries[n].getVirtualName());
      System.out.print(" | ");
      if(entries[n].isDirectory())
	System.out.print("d");
      else
	System.out.print("-");
      if(entries[n].isVirtualDir())
	System.out.print("v");
      else
	System.out.print("-");
      
      System.out.print(" | " + entries[n].getOpenPath() + " | ");
      System.out.println("" + entries[n].getHandler());
    }

    if(entries == null || entries.length == 0){
      System.out.println("Empty dir");
    }
  }

  public void setConsoleLogLevel(String clevel) {
    if(chand != null)  
      chand.setLevel(java.util.logging.Level.parse(clevel));
  } 

  public void setFileLogLevel(String flevel) {
    if(fhand != null) {
      fhand.setLevel(java.util.logging.Level.parse(flevel));
    }
  }

  public String getConsoleLogLevel() {
    if(chand != null) {
      return chand.getLevel().getName();
    }
    return "OFF";
  }

  public String getFileLogLevel() {
    if(fhand != null) {
      return fhand.getLevel().getName();
    }
    return "OFF";
  }

  public int getPort() {
    return port;
  }

  public void setPort(int port) {
    this.port = port;
  }

  public void persistParams() {
    Parameters params = getParameters();
    params.setMntTable(mountTable);
    if(chand != null) {
      params.setServerLogLevel(chand.getLevel().getName());
    }
    if(fhand != null) {
      params.setServerFileLogLevel(fhand.getLevel().getName());
    }
    params.setPort(port);
    params.saveParameters();
  }

  public void clearCache() {
    cache.clear();
  }

  public void startServer() {
    printBanner();

    while(true) {
      try {
	Parameters params = getParameters();
	
	port = params.getPort();
	collapseDirs = params.getCollapseDirs();
	debug = params.getDebugVfs();
	
	ps2vfs.server.Ps2VfsServer server = new ps2vfs.server.Ps2VfsServer(port,
									   this, log);
	server.start();

	int sleepInterval = 60000;
	if(enableGUI) {
	  ps2vfs.gui.MountPointsFrame frame = new ps2vfs.gui.MountPointsFrame(this, aboutText);
	  frame.show();
	  mntFrame = frame;
	  sleepInterval = 5000;
	}

	while(true) {
	  try {
	    Thread.sleep(sleepInterval);
	    if(enableGUI) {
	      ps2vfs.gui.MountPointsFrame frame = (ps2vfs.gui.MountPointsFrame) mntFrame;
	      // Update open files log.
	      // frame.notifyOpenFilesChanged();

	    }
	  } catch(InterruptedException e) {
	  }
	}
      } catch (Throwable e) {
	e.printStackTrace(System.err);
      }
    }
  }

  protected static String getImageURL(String path) {
    java.net.URL imgURL = Ps2Vfs.class.getResource(path);
    if (imgURL != null) {
      return "" + imgURL;
    } else {
      return path;
    }
  }

  private void printBanner() {
    log.info("# PS2VFS-PS V" + versionStr + ".");
    log.info("# The PS2 interface and server is based on, with additional ");
    log.info("# adaptions for actual PS2Reality behavior, the Playstation 2");
    log.info("# Virtual File System Example, see copyright details below.");
    log.info("# ");
    log.info("# (C)2004, Krilon ");
    log.info("# ");
    log.info("# Thanks to the PS2Reality team!");
    log.info("# ------------------------------------------------------------------------------");
  }

  public static void main(String args[]) {
    
    try {
      vfs = new Ps2Vfs(args);
    } catch(Exception e) {
      // e.printStackTrace(System.err);
      return;
    }
    vfs.startServer();
    vfs.persistParams();
  }



  private void initPlugins(String pluginPath) {
    try {
      log.info("Loading plugins..");	
      File dir = new File(pluginPath);
      plugins = new java.util.Vector();
      protoMap = new java.util.HashMap();
      
      java.io.File dirContent[] = dir.listFiles();
      if(dirContent == null) {
	return;
      }
      
      URL urls[] = {dir.toURI().toURL()};
      java.net.URLClassLoader loader = new java.net.URLClassLoader(urls);
      
      for(int n = 0; n < dirContent.length; n++) {
	if(!dirContent[n].isDirectory()) {
	  String name = dirContent[n].getName();
	  String myClassName = null;
	  int extpos = name.lastIndexOf('.');
	  if(extpos > 0) {
	    String ext = name.substring(extpos);
	    if(ext.equals(".jar")) {
	      java.util.jar.JarFile jf = new java.util.jar.JarFile(dirContent[n].getAbsolutePath());
	      java.util.jar.Manifest manf = jf.getManifest();
	      
	      URL jarUrls[] = {dirContent[n].toURI().toURL()};
	      java.net.URLClassLoader jarLoader = new java.net.URLClassLoader(jarUrls);
	      
	      
	      if(manf != null) {
		// Extract plugins from MANIFEST.
		java.util.jar.Attributes attrs = manf.getMainAttributes();
		
		myClassName = attrs.getValue(java.util.jar.Attributes.Name.MAIN_CLASS);
		java.util.Map emap = manf.getEntries();
		if(emap != null) {
		  java.util.Collection ecol = emap.values();
		  for(java.util.Iterator it = ecol.iterator(); it.hasNext();) {
		    attrs = (java.util.jar.Attributes) it.next();
		    myClassName = attrs.getValue(java.util.jar.Attributes.Name.MAIN_CLASS);
		    if(myClassName != null)
		      loadPluginClass(myClassName, jarLoader);
		  }
		}
	      } else {
		myClassName = name.substring(0, extpos);
		loadPluginClass(myClassName, jarLoader);
	      }
	    } else if (ext.equals(".class") || ext.equals(".jar")) {
	      myClassName = name.substring(0, extpos);
	      loadPluginClass(myClassName, loader);
	    }
	  }
	}
      }
    } catch(Throwable ee) {
      if(debug) {
	ee.printStackTrace(System.err);
      }
      log.warning(ee.toString());
    }
  }

  private void loadPluginClass(String pluginClassName, ClassLoader loader) {
    Class newPlugin = null;
    VfsPlugin newPluginInst = null;

    try {
      newPlugin = loader.loadClass(pluginClassName);
    } catch(Throwable e) {
      e.printStackTrace(System.err);
    }
    if(newPlugin != null) {
      try {
	newPluginInst = (VfsPlugin) newPlugin.newInstance();
	log.info("Loaded plugin '" + newPluginInst.getName() + "'");
      } catch (Throwable e) {
	e.printStackTrace(System.err);
	log.warning("Remove non-plugin class from plugin dir " + newPlugin);
      }
    }
    
    if(newPluginInst != null) {
      if(newPluginInst.init(log)) 
	plugins.add(newPluginInst);
    }
  }

  private void initMountTable() {
    Parameters params = getParameters();
    VfsMountTable.setDebug(params.getDebugMnt());
    mountTable = params.getMntTable();
  }

  public VfsMountTable getMountTable() {
    return mountTable;
  }


  private void initLogger() {
    LogFormatter lf = new LogFormatter(debug);
    
    log = java.util.logging.Logger.getLogger("ps2vfs");
    log.setUseParentHandlers(false);
    log.setLevel(java.util.logging.Level.ALL);
    
    Parameters params = getParameters();

    chand = new java.util.logging.ConsoleHandler();
    java.util.logging.Level logLevel = java.util.logging.Level.OFF;
    try {
      logLevel = java.util.logging.Level.parse(params.getServerLogLevel());
    } catch(Throwable e) {
    }
    chand.setLevel(logLevel);
    chand.setFormatter(lf);
    log.addHandler(chand);
    /*
      java.util.logging.MemoryHandler mhand = new java.util.logging.MemoryHandler(chand2, 1000, 
      java.util.logging.Level.OFF);
      mhand.setLevel(java.util.logging.Level.CONFIG);
      log.addHandler(mhand);
    */
    try {
      fhand = new java.util.logging.FileHandler("ps2vfs-%u.log.%g");
      logLevel = java.util.logging.Level.OFF;
      try {
	logLevel = java.util.logging.Level.parse(params.getServerFileLogLevel());
      } catch(Throwable e) {
      }
      fhand.setLevel(logLevel);
      fhand.setFormatter(lf);
      log.addHandler(fhand);
    } catch(java.io.IOException e) {
      log.warning("Failed to open file log: " + e);
    }
    
    log.config("Log initialized.");
  }
  

  public synchronized static Parameters getParameters() {
    parameters_ = new Parameters();
    parameters_.loadParameters();
    return parameters_;
  }

  public java.util.Vector getPluginList() {
    return plugins;
  }

  private static Parameters parameters_;

  private java.util.logging.Logger log;
  private java.util.Vector plugins;
  private java.util.HashMap protoMap;



  private VfsMountTable mountTable;  

  // Open file descriptor bookkeeping.
  private java.util.Hashtable openFiles;
  private int numOpenFiles;
  private int maxOpenFiles = 20;
};

