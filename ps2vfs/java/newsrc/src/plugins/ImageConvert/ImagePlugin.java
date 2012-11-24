package ps2vfs.pluggable.imageconverter;

import javax.imageio.ImageIO;


class ImageOpenFile extends ps2vfs.vfs.VfsDefaultOpenFile 
{

  private final boolean temp;
  public ImageOpenFile(String filename, boolean temp)
    throws java.io.FileNotFoundException
  {
    super(filename);
    fileRep = new java.io.File(filename);
    this.temp = temp;
  }
  
  public boolean close() {
    boolean ret = super.close();
    
    if(temp && ret) {
      fileRep.delete();
      return true;
    }
    return false;
  }

  public String getOpenPath() {
    return fileRep.getPath();
  }
  
  public String getInfo() {
    return "Image converted";
  }


  private java.io.File fileRep; 
}

public class ImagePlugin implements ps2vfs.plugin.VfsPlugin
{
  private java.util.logging.Logger log;
  private java.util.HashSet supportedTypes;
  private ResizeImage resizer; 
  private final String propFileName   = "imageplugin.props";
  private final String maxWidthName   = "ps2vfs.plugins.imageplugin.maxWidth";
  private final String maxHeightName  = "ps2vfs.plugins.imageplugin.maxHeight";
  private final String qualityName    = "ps2vfs.plugins.imageplugin.quality";
  private java.util.Properties props;
  private int maxW;
  private int maxH;
  private float quality;

  public String getName() {
    return new String("PS2 Image Converter");
  }

  public String getDescription() {
    return new String("<html><h2>Image converter and resizer 1.0</h2>" + 
		      "This plugin will allow you to convert images to jpeg and resize them to fit the PS2Reality maximum resolution.<p>" + 
		      "Currently the plugin will convert all known formats to jpeg. This will be made configurable when I get the time.<html>");
  }
  
  public String getProtocol() {
    return null;
  }

  public boolean init(java.util.logging.Logger iLog) {
    log = iLog;
    props = new java.util.Properties();
    readConfig();
    saveConfig();
    resizer = new ResizeImage(maxW, maxH, quality);

    supportedTypes = new java.util.HashSet();
    String[] readerFormats = ImageIO.getReaderFormatNames();
    boolean jpegReaderPresent = false;
    if(readerFormats != null) {
      for(int n = 0; n < readerFormats.length; n++) {
	if(readerFormats[n].equalsIgnoreCase("jpeg") || readerFormats[n].equalsIgnoreCase("jpg"))
	  jpegReaderPresent = true;
	String lstr = readerFormats[n].toLowerCase();
	log.finer("Adding format extension: " + lstr);
	supportedTypes.add(lstr);
      }
    }

    String[] writerFormats = ImageIO.getWriterFormatNames();
    boolean jpegWriterPresent = false;
    if(writerFormats != null) {
      for(int n = 0; n < writerFormats.length; n++) {
	if(writerFormats[n].equalsIgnoreCase("jpeg") || writerFormats[n].equalsIgnoreCase("jpg"))
	  jpegWriterPresent = true;
      }
    }

    // Check to see if we have a purpose in life.
    // We could require only the writer to be present, but
    // for now we like to have both reader and writer.
    return jpegWriterPresent && jpegReaderPresent;
  }
  
  public void filterDir(ps2vfs.plugin.VfsDir dir) {

    java.util.List dirContent = dir.list();
    java.util.ListIterator it = dirContent.listIterator();
    
    while(it.hasNext()) {
      ps2vfs.plugin.VfsDirEntry entry = (ps2vfs.plugin.VfsDirEntry) it.next();
      if(!entry.isDirectory()) {
	String name = entry.getVirtualName();
	int ext = name.lastIndexOf('.');
	if(ext > 0 && ext < (name.length()-1)) {
	  String extStr = name.substring(ext+1).toLowerCase();
	  // System.out.println("Found ext: " + extStr);
	  if(supportedTypes.contains(extStr)) {
	    if(!(extStr.equals("jpeg") || extStr.equals("jpg"))) 
	      entry.setVirtualName(name + ".auto.jpg");
	    entry.setHandler(this);
	  }
	}
      }
    }
  }
  
  public ps2vfs.plugin.VfsOpenFile open(String path) 
  {
    log.finer("Trying to open: " + path);
    try {
      String convertedFilename = resizer.doResize(path);
      if(convertedFilename != null)
	path = convertedFilename;
      log.finer("Image open: " + path);
      return new ImageOpenFile(path, convertedFilename != null);
    } catch(Exception e) {
      System.err.println("Exception: " + e);
      e.printStackTrace(System.err);
      return null;
    }
  }
  
  public java.util.List readDir(String path) {
    return null;
  }

  public void doAbout(Object mainFrame) {
    if(mainFrame != null) {
      java.awt.Frame topFrame = (java.awt.Frame) mainFrame;
      new About(topFrame, true).show();
    } else {
      System.out.println("Image converter written by Krilon.");
    }
  }
  
  public void doConfigure(Object mainFrame) {
    if(mainFrame != null) {
      java.awt.Frame topFrame = (java.awt.Frame) mainFrame;
      new ConfigurationDialog(topFrame, true).show();
    } else {
      System.out.println("Configuring image converter.");
      System.out.println("Current config: maxW=" + maxW + ", maxH=" + maxH + 
			 ", quality=" + quality);
    }
  }

  private void readConfig() {
    
    try {
      java.io.FileInputStream in = new java.io.FileInputStream(propFileName);
      props.load(in);
      in.close();
    } catch(Throwable e) {
      e.printStackTrace(System.err);
    }
    maxW = getPropInt(maxWidthName, 1280);
    maxH = getPropInt(maxHeightName, 1024);
    quality = getPropFloat(qualityName, 0.932f);
  }

  public void saveConfig() {
    try {
      java.io.FileOutputStream out = new java.io.FileOutputStream(propFileName);
      props.store(out, null);
      out.close();
    } catch(Throwable e) {
      e.printStackTrace(System.out);
    }
  }

  private int getPropInt(String name, int defVal) {
    String numEntStr = props != null ? props.getProperty(name) : null;
    int numEnt = defVal;
    boolean valSet = false;
    if(numEntStr != null) {
      try {
	numEnt = Integer.parseInt(numEntStr);
	valSet = true;
      } catch(NumberFormatException e) {
	e.printStackTrace(System.err);
      }
    }
    if(!valSet) {
      props.setProperty(name, new Integer(defVal).toString());
    }
    return numEnt;
  }

  private float getPropFloat(String name, float defVal) {
    String numEntStr = props != null ? props.getProperty(name) : null;
    float numEnt = defVal;
    boolean valSet = false;
    if(numEntStr != null) {
      try {
	numEnt = Float.parseFloat(numEntStr);
	valSet = true;
      } catch(NumberFormatException e) {
	e.printStackTrace(System.err);
      }
    }
    if(!valSet) {
      props.setProperty(name, new Float(defVal).toString());
    }

    return numEnt;
  }

  private static void printArray(String prefix, String[] strArr) {
    System.out.print(prefix);
    for(int n = 0; n < strArr.length; n++) {
      System.out.print(" " + strArr[n]);
    }
    System.out.println();
  }
}
