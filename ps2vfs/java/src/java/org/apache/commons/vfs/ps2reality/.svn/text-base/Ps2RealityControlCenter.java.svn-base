package org.apache.commons.vfs.ps2reality;
import java.io.*;
import java.awt.*;
import java.awt.event.*;
import javax.swing.*;
import javax.swing.event.*;
import javax.swing.text.*;
import javax.swing.border.TitledBorder;

public class  Ps2RealityControlCenter extends JFrame {


private final JTabbedPane jtpMain = new JTabbedPane();
	private final JPanel panelAbout = new JPanel();
	public final JPanel panelChooser;
	private final static int iDEFAULT_FontSize = 12;
	
	public String getPath()
	{
        String cadena=((Ps2RealityFileChooser)panelChooser).getMediaPath();
		if(cadena==null)
		{
			cadena=System.getProperty("user.dir");
		}
		return cadena;
	}
	public int getLog()
	{
        int level=((Ps2RealityFileChooser)panelChooser).getLog();
		
		return level;
	}
 Ps2RealityControlCenter(String dir) throws IOException {
 
		try 
		{
			byte[] abIcon;
 			InputStream inputstreamIcon =this.getClass().getResourceAsStream("icon.gif");
     		int iIconSize = inputstreamIcon.available();
			abIcon = new byte[iIconSize];
 			inputstreamIcon.read(abIcon);
   			this.setIconImage(new ImageIcon(abIcon).getImage());

		} catch(Exception ex) {
     	// the default icon will be used
		}
		panelChooser = new Ps2RealityFileChooser(dir);
 
 }
	
	
	
	boolean initControlCenter(String title)
	{

		

		this.setTitle(title);
		 // set up content pane
		Container content = this.getContentPane();
		content.setLayout(new BorderLayout());
		panelAbout.setLayout(new BorderLayout());
		
		JTextArea textArea=new JTextArea("PlayStation 2 Virtual File System release 1.0 \n\nSpecials thanks to our betatester\nPS2Linux Betatester: Mrbrown and Sarah\nPS2 betatester: Oobles, Caveman, Gamebytes, Ping^Spike, Josekenshin, Padawan, pakor, SandraThx and Rolando\n\nAdded little gui in java swing\nAdded feature to choose directory for media files\nAdded support for properties files\nCheck for updates at ps2dev.org\n\nRelease 1.2\n\nRewrite io with java NIO\nadded console mode support\n");
				textArea.setEditable(false);

		JScrollPane areaScrollPane = new JScrollPane(textArea);
		areaScrollPane.setVerticalScrollBarPolicy(
		JScrollPane.VERTICAL_SCROLLBAR_ALWAYS);
		areaScrollPane.setPreferredSize(new Dimension(250, 250));
		TitledBorder  aboutBorder=BorderFactory.createTitledBorder("Change log and Greets");
		aboutBorder.setTitleColor(Color.blue);
		panelAbout.setBorder(aboutBorder);


        panelAbout.add(areaScrollPane);
		//set up tabbed pane
		content.add(jtpMain);
		
		jtpMain.addTab("Configure",panelChooser);
        jtpMain.addTab("About",panelAbout);
		 //  set up display area
		//jtaDisplay.setEditable(false);
		//jtaDisplay.setLineWrap(true);
		//jtaDisplay.setMargin(new Insets(5, 5, 5, 5));
		//jtaDisplay.setFont(
		// new Font("Monospaced", Font.PLAIN, iDEFAULT_FontSize));
		//jspDisplay.setViewportView(jtaDisplay);
		//jspDisplay.setVerticalScrollBarPolicy(JScrollPane.VERTICAL_SCROLLBAR_ALWAYS );
		//panelConsole.add(jspDisplay, BorderLayout.CENTER);
		//panelConsole.add(jtfCommand, BorderLayout.SOUTH);
				//panelConsole.add(jtaDisplay, BorderLayout.CENTER);

		// listener: window closer
		this.addWindowListener(
    	new WindowAdapter(){
     	  public void windowClosing(WindowEvent e){
       				System.exit(0);}
		    });

		this.vResize();
		return true;


	}
	
	void vResize(){
  		Dimension dimScreenSize =new Dimension(640,200);
		//Toolkit.getDefaultToolkit().getScreenSize();
		this.setSize(dimScreenSize);
	}
	
	
}
