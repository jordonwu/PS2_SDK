/**
 * This is the interface that all PS2VFS Plugins must implement.
 *
 * @version 0.0.1 
 * @author Stig Petter Olsrød
 */
package ps2vfs.plugin;

public interface VfsPlugin 
{
  /** 
   * Initializes the plugin.
   * It will be disabled if false is returned.
   *
   * param Logger should be used to add log messages.
   */
  public boolean init(java.util.logging.Logger log);
  
  /**
   * Show about information for the plugin.
   * If guiEnabled is set the plugin can 
   * use GUI classes.
   */
  public void doAbout(Object mainFrame);
  public void doConfigure(Object mainFrame);

  /**
   * The plugin should return a short name.
   */ 
  public String getName();

  /**
   * The plugin should return a short description
   * of itself
   */
  public String getDescription();


  /**
   * The plugin should return the name of the protocol
   * it supports. 
   * If it is a pure filter plugin, null should be returned.
   */
  public String getProtocol();

  /**
   * This is method is called so the plugin can change or add
   * to the directory listing.
   */
  // public void    filterDir(java.util.List<ps2vfs.plugin.VfsDirEntry> dirContent);
  public void    filterDir(ps2vfs.plugin.VfsDir dir);

  /**
   * The plugin can overload the reading of a directory.
   * Objects in the list should implement the VfsDirEntry interface!
   *
   */
  // public java.util.List<ps2vfs.plugin.VfsDirEntry>  readDir(String path);
  public java.util.List readDir(String path);

  /**
   * This method is called so the plugin can overload the open
   * method.
   */
  public VfsOpenFile open(String path);

}

