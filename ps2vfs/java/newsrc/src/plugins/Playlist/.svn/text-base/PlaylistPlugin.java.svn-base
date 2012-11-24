
package ps2vfs.pluggable.playlist;

import java.io.File;
import java.io.BufferedReader;
import java.io.FileReader;
import java.util.Vector;

public class PlaylistPlugin implements ps2vfs.plugin.VfsPlugin
{
  public String getName() {
    return new String("PS2 Playlist Converter");
  }

  public String getDescription() {
    return new String("<html><h2>Playlist converter 1.0</h2>" + 
		      "The Playlist plugin will convert playlists into virtual directories and they can thus be played on the PS2Reality player. The main reason for writing this was to be able to play playlists with ShoutCast streams. This version will only support the .pls and .m3u formats." +
		      "</html>");
  }

  public String getProtocol() {
    return null;
  }

  public boolean init(java.util.logging.Logger iLog) {
    log = iLog;
    return true;
  }
  
  public void filterDir(ps2vfs.plugin.VfsDir dir) {

    java.util.List dirContent = dir.list();
    java.util.ListIterator it = dirContent.listIterator();
    
    while(it.hasNext()) {
      ps2vfs.plugin.VfsDirEntry entry = (ps2vfs.plugin.VfsDirEntry) it.next();
      if(!entry.isDirectory()) {
	String name = entry.getVirtualName();
	int ext = name.lastIndexOf('.');
	if(ext > 0 && name.substring(ext).equals(".pls")) {
	  entry.setHandler(this);
	  entry.setVirtualDir(true);
	}
      }
    }
  }
  
  public ps2vfs.plugin.VfsOpenFile open(String path) {
    log.finer("Trying to open: " + path);
    return null;
  }

  public java.util.List readDir(String path) {
    java.util.List dirContent = null;
    try {
      File plsFile = new File(path);
      if(plsFile.canRead()) {
	String plsName = plsFile.getName();
	int ext = plsName.lastIndexOf('.');
	if(ext > 0) {
	  String extName = plsName.substring(ext);
	  if(extName.equals(".pls")) {
	    // Read the content of the playlist.
	    dirContent = readPLS(plsFile);
	  }
	  else if(extName.equals(".m3u")) {
	    // Read the content of the playlist.
	    dirContent = readM3U(plsFile);
	  }
	}
      }
    } catch(Throwable e) {
      log.finest(e.toString());
      e.printStackTrace(System.err);
    }
    return dirContent;
  }

  public void doAbout(Object mainFrame) {
    if(mainFrame != null)
      initGui();
    else
      log.finest("Playlist converter written by Krilon.");
  }
  
  public void doConfigure(Object mainFrame) {
    if(mainFrame != null) 
      initGui();
    else
      log.finest("Configuring playlist converter.");
  }

  private void initGui() {
    return;
  }


  private java.util.List readPLS(File plsFile)
    throws java.io.FileNotFoundException, java.io.IOException
  {
    java.util.List dirContent = null;
    ps2vfs.plugin.VfsDirEntry dirEnt = null;
    BufferedReader in
      = new BufferedReader(new FileReader(plsFile));

    dirContent = new Vector();
    
    String entry = null;
    String numEntry = "";
    String extStr = "";

    while((entry = in.readLine()) != null) {
      if(entry.startsWith("File")) {
	int sep = entry.indexOf("=");
	if(sep > 0) {
	  numEntry = entry.substring(4, sep) + " - ";
	  entry = entry.substring(sep+1);
	} else {
	  entry = null;
	}
	if(entry != null) {
	  dirEnt = new ps2vfs.plugin.VfsDirEntry();
	  File ref = new File(entry);

	  int protoSep = entry.indexOf("://");
	  String protocol = null;
	  if(protoSep != -1) {
	    protocol = entry.substring(0, protoSep);
	  }
	  
	  String openPath = "";
	  if(ref.isAbsolute()) {
	    openPath = ref.getAbsolutePath();
	  } else if(protocol != null) {
	    openPath = entry;
	  } else {
	    openPath = plsFile.getAbsolutePath() + 
	      File.separator + ref.getPath();
	  }
	  dirEnt.setHandler(new ps2vfs.plugin.VfsHandler(openPath, null));
	  String nameStr = ref.getName();
	  nameStr = nameStr.replaceAll(":", "_");
	  if(protocol != null) {
	    extStr = ".smp3";
	  }
	  dirEnt.setVirtualName(numEntry + nameStr + extStr);
	  dirContent.add(dirEnt);
	}
	
      } else if(dirEnt != null && entry.startsWith("Title")) {
	int sep = entry.indexOf("=");
	if(sep > 0) {
	  entry = entry.substring(sep+1);
	  dirEnt.setVirtualName(numEntry + entry + extStr);
	}
      }
    }
    return dirContent;
  }

  private java.util.List readM3U(File plsFile)
    throws java.io.FileNotFoundException, java.io.IOException
  {
    java.util.List dirContent = null;
    ps2vfs.plugin.VfsDirEntry dirEnt = null;
    BufferedReader in
      = new BufferedReader(new FileReader(plsFile));

    dirContent = new Vector();
    
    String entry = null;
    String numEntry = "";
    String extStr = "";

    while((entry = in.readLine()) != null) {
      if(entry.startsWith("File")) {
	int sep = entry.indexOf("=");
	if(sep > 0) {
	  numEntry = entry.substring(4, sep) + " - ";
	  entry = entry.substring(sep+1);
	} else {
	  entry = null;
	}
	if(entry != null) {
	  dirEnt = new ps2vfs.plugin.VfsDirEntry();
	  File ref = new File(entry);
	  String openPath;
	  if(ref.isAbsolute())
	    openPath = ref.getAbsolutePath();
	  else
	    openPath = plsFile.getAbsolutePath() + 
	      File.separator + ref.getPath();
	  dirEnt.setHandler(new ps2vfs.plugin.VfsHandler(openPath, null));

	  String nameStr = ref.getName();
	  int ext = nameStr.lastIndexOf('.');
	  if(ext > 0) {
	    extStr = nameStr.substring(ext);
	    if(nameStr.startsWith("http:"))
	      extStr = ".stream" + extStr;
	    nameStr = nameStr.substring(0, ext - 1);
	  } else {
	    extStr = "";
	  }
	  dirEnt.setVirtualName(numEntry + nameStr + extStr);
	  dirContent.add(dirEnt);
	}
	
      } else if(dirEnt != null && entry.startsWith("Title")) {
	int sep = entry.indexOf("=");
	if(sep > 0) {
	  entry = entry.substring(sep+1);
	  dirEnt.setVirtualName(numEntry + entry + extStr);
	}
      }
    }
    return dirContent;
  }
  
  private java.util.logging.Logger log;

}
