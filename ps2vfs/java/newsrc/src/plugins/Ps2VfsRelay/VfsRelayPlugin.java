
package ps2vfs.pluggable.vfsrelay;

import java.io.File;
import java.io.BufferedReader;
import java.io.FileReader;
import java.util.Vector;

public class VfsRelayPlugin implements ps2vfs.plugin.VfsPlugin
{
  public String getName() {
    return new String("PS2Vfs Relay Plugin");
  }

  public String getDescription() {
    return new String("<html><h2>PS2VFS-PS Relay Client 0.9</h2>This plugin will allow you to connect a remote PS2VFS-PS to this instance of PS2VFS-PS. Use this if you have several computers serving content.<p><p>Use <b>ps2vfs://&lt;host&gt;:&lt;port&gt;/</b> in the \"Attach URL\" field when mounting a remote PS2VFS-PS.<p></html>");
  }

  public String getProtocol() {
    return "ps2vfs";
  }

  public boolean init(java.util.logging.Logger iLog) {
    log = iLog;
    return true;
  }
  
  public void filterDir(ps2vfs.plugin.VfsDir dir) { 
    // log.finer("Filtering: " + dir);
    return;
  }
  
  public ps2vfs.plugin.VfsOpenFile open(String path) {
    // log.finer("Trying to open: " + path);
    VfsRelayOpenFile of = null;
    log.finest("VfsRelayPlugin open: " + path);
    try {
      java.net.URI uri = new java.net.URI(path);
      // log.finest("URI: " + uri.getScheme() + " at " + uri.getHost() + " port: " + uri.getPort());
      
      String host = uri.getHost();
      int port = uri.getPort();
      
      Connection conn = new Connection(this, uri.getScheme() + "://" + 
				       host + ":" + port);
      conn.connect(host, port);
      of = new VfsRelayOpenFile(uri.getPath(), conn, log);

    } catch(Exception e) {
      log.finer("open failed: " + e);
    }
    return of;
  }

  public java.util.List readDir(String path) {
    log.finest("VfsRelayPlugin readDir: " + path);
    java.util.List dirContent = null;
    ps2vfs.plugin.VfsDirEntry dirEnt = null;
    try {
      java.net.URI uri = new java.net.URI(path);
      // log.finest("URI: " + uri.getScheme() + " at " + uri.getHost() + " port: " + uri.getPort());

      String host = uri.getHost();
      int port = uri.getPort();
      
      Connection conn = new Connection(this, uri.getScheme() + "://" + 
				       host + ":" + port);
      conn.connect(host, port);
      dirContent = conn.readDir(uri.getPath());
      conn.exit();
    } catch(Exception e) {
      log.finer("readDir failed: " + e);
    }
    return dirContent;
  }

  public void doAbout(Object mainFrame) {
    if(mainFrame != null)
      initGui();
    else
      log.finest("PS2Vfs Relay Plugin written by Krilon.");
  }
  
  public void doConfigure(Object mainFrame) {
    if(mainFrame != null) 
      initGui();
    else
      log.finest("Configuring PS2Vfs Relayer.");
  }

  private void initGui() {
    return;
  }

  private java.util.logging.Logger log;

}
