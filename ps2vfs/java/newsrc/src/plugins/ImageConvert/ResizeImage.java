
package ps2vfs.pluggable.imageconverter;

// import
import javax.imageio.*;
import javax.imageio.stream.*;
import java.awt.Image;
import javax.swing.ImageIcon;
import java.awt.Toolkit;
import java.awt.image.*;
import java.awt.Graphics2D;
import java.io.*;
import java.util.logging.*;

public class ResizeImage 
{
  private final int maxW;
  private final int maxH;
  private final float quality;

  private static final boolean oldWay = false;
  private ImageWriter jpgWriter = null;
  private ImageWriteParam iwp = null;


  public ResizeImage(int maxW, int maxH, float quality) {
    this.maxW = maxW;
    this.maxH = maxH;
    this.quality = quality;
    ImageIO.setUseCache(false);
    initJPGWriter();
  }
  
  private void initJPGWriter() {
    java.util.Iterator it = ImageIO.getImageWritersByFormatName("jpeg");
    while(it.hasNext()) {
      ImageWriter wrt = (ImageWriter) it.next();
      ImageWriteParam param = wrt.getDefaultWriteParam();
      if(jpgWriter == null) {
	jpgWriter = wrt;
	iwp = param;
	iwp.setCompressionMode(ImageWriteParam.MODE_EXPLICIT);
	iwp.setCompressionQuality(quality);
      }
      double compQual = 0.0;
      int mode = 0;
      try {
	mode = param.getCompressionMode();
	compQual = param.getCompressionQuality();
      } catch(Exception e) {;}
      
      /* System.err.println("Writer: " + wrt + " with compression: " + 
	 mode + "/" + compQual);      */
    }
  }
  
  public String doResize(String filename) 
    throws FileNotFoundException,
    IOException
  {
    boolean convertImage = true;
    File inF = new File(filename);

    int ext = filename.lastIndexOf('.');
    if(ext > 0) {
      String extStr = filename.substring(ext);
      if(extStr.equals(".jpeg") || extStr.equals(".jpg")) {
	// Find some way to check the readers or BufferedImage so we
	// don't have to depend on a well formed file name.
	convertImage = false;
      }
    }
    /*
    System.out.println("Reading file '" + filename + "'" + 
		       (convertImage ? " and converting to JPEG" : ""));
    */
    BufferedImage img = ImageIO.read(inF);
    if(img == null) {
      System.err.println("Image format is not supported");
      return null;
    }
    img.flush();
    int width = img.getWidth();
    int height = img.getHeight();
    double scale = 1.0;
    double hscale = 1.0;
    if(width > maxW) {
      scale = ((double)maxW)/width;
    } 
    
    if(height > maxH) {
      hscale = ((double)maxH)/height;
      if(hscale < scale)
	scale = hscale;
    }
    
    int newH = (int) Math.floor(height * scale);
    int newW = (int) Math.floor(width * scale);
    
    /*
      System.out.println("" + new java.util.Date() + " - Image is " + width + "x" + height + " need to be scaled by min(" + scale + "," + hscale + ") to " + newW+ "x" + newH);
    */
    if(convertImage || scale != 1.0 || true) {
      BufferedImage rimg; 
      if(scale == 1.0)
	rimg = img;
      else
	rimg = convertToBufferedImage(scaleImage(img, newW, newH));
      //System.err.println("" + new java.util.Date() + " - Scaled");

      return writeToFile(rimg, inF.getName());
    }
    return null; // No need to scale this image.
  }

  private BufferedImage scaleImage(Image img, int width, int height) {
    if(oldWay) {
      return convertToBufferedImage(img.getScaledInstance(width, 
							  height, 
							  BufferedImage.SCALE_SMOOTH));
    } else {
      ImageFilter filter = new ReplicateScaleFilter(width, height);
      ImageProducer producer = new FilteredImageSource(img.getSource(), filter);
      Image resizedImage = Toolkit.getDefaultToolkit().createImage(producer);
      return convertToBufferedImage(resizedImage);
    }
  }
  private BufferedImage convertToBufferedImage(Image img) {
    int width = img.getWidth(null);
    int height = img.getHeight(null);
    BufferedImage rimg = new BufferedImage(width, 
					   height,
					   BufferedImage.TYPE_INT_RGB);
    Graphics2D gp = rimg.createGraphics();
    gp.setColor(java.awt.Color.white);
    gp.fillRect(0 , 0, width, height);
    gp.drawImage(img, 0, 0, null);
    gp.dispose();
    return rimg;
  }

  private String writeToFile(BufferedImage img, String name) 
    throws java.io.IOException
  {
    File outf;
    String ofn = name;
    int ext = ofn.lastIndexOf('.');
    if(ext > 0) {
      ofn = ofn.substring(0,ext) + "_";
    }
    outf = File.createTempFile(ofn, ".jpg");
    //System.out.println("Writing results to outf: '" + outf.getName() + "' (" + outf.getPath() + ")");

    FileImageOutputStream out = new FileImageOutputStream(outf);
    IIOImage image = new IIOImage(img, null, null);
    if(jpgWriter != null) {
      jpgWriter.setOutput(out);
      jpgWriter.write(null, image, iwp);
      out.close();
    }
    return outf.getAbsolutePath();
  } 
}
