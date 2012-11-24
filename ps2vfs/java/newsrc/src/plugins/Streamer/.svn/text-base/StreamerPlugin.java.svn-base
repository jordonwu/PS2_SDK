package ps2vfs.pluggable.streamer;

public class StreamerPlugin implements ps2vfs.plugin.VfsPlugin
{


  public String getName() {
    return new String("PS2 Streamer Client");
  }

  public String getDescription() {
    return new String("<html><H2>ShoutCast relayer 1.1</h2>" + 
		      "This plugin will relay shoutcast streams to the PS2Reality player and allow you to listen to you favorite internet radio station on the PS2. You would typically download a playlist from the radio station (or from a radio station directory service).<P><BR><P>" +
		      "Known limitations: It can take a really long time to get the stream started due to a PS2 side issue. A 96k stream might take 60 seconds to start (PS2Reality wants 640KB before it starts to play).<p></hmtl>");
  }

  public String getProtocol() {
    return null;
  }

  public boolean init(java.util.logging.Logger iLog) {
    log = iLog;
    log.config("Initializing streamer.");
    return true;
  }
  
  public void filterDir(ps2vfs.plugin.VfsDir dir) {
    java.util.List dirContent = dir.list();
    java.util.ListIterator it = dirContent.listIterator();
    
    while(it.hasNext()) {
      ps2vfs.plugin.VfsDirEntry entry = (ps2vfs.plugin.VfsDirEntry) it.next();
      if(!entry.isDirectory()) {
	String name = entry.getVirtualName();
	if(name.endsWith(".smp3")) {
	  int ext = name.lastIndexOf('.');
	  if(ext >= 0) {
	    entry.setVirtualName(name.substring(0,ext) + ".mp3");
	  }
	  entry.setHandler(this);
	}
      }
    }
  }
  
  public ps2vfs.plugin.VfsOpenFile open(String path) {
    log.finer("Trying to open: " + path);
    try {
      return new StreamOpenFile(path, log);
    } catch(Exception e) {
      String msg = e.getMessage();
      if(msg == null)
	msg = e.toString();
      log.warning("Failed to open stream: " + msg);
    }
    return null;
  }

  public java.util.List readDir(String path) {
    return null;
  }

  public void doAbout(Object mainFrame) {
    System.out.println("Streamer adapter written by Krilon.");
  }
  
  public void doConfigure(Object mainFrame) {
    System.out.println("Configure the streamer adapter.");
  }

  private java.util.logging.Logger log;
}
