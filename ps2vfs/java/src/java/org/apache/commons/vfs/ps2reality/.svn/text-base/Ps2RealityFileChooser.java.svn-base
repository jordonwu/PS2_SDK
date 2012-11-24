package org.apache.commons.vfs.ps2reality;
import java.io.*;
import java.awt.*;
import java.awt.event.*;
import javax.swing.*;
import javax.swing.border.TitledBorder;
import javax.swing.filechooser.*;
import javax.swing.event.*;
import java.util.*;


public class Ps2RealityFileChooser extends JPanel implements ActionListener {
    JButton openButton;
	JRadioButton level1Button;
	JRadioButton level2Button;
	int log;
    ButtonGroup group;
	JTextField field;
	JPanel buttonPanel;
	JPanel logPanel;
    JFileChooser fc;
	public String ps2VfsRootPath=System.getProperty("users.dir");
	
	public String getMediaPath()
	{

		return this.ps2VfsRootPath;
	}
	public int getLog()
	{

		return this.log;
	}
    public Ps2RealityFileChooser(String directorio) {
        super(new BorderLayout());

       
            
            String dir=directorio;
			

        //Create a file chooser
        fc = new JFileChooser();

        //only directories
        fc.setFileSelectionMode(JFileChooser.DIRECTORIES_ONLY);

      
        openButton = new JButton("Media Path...");
                                 
		field = new JTextField(40);
		field.setText(dir);
		field.setEditable(false);
        openButton.addActionListener(this);

        
		level1Button = new JRadioButton("Level 1 Only Connect/Disconnect Events");
	   	level2Button = new JRadioButton("Level 2 Command Event and Connect/Disconnect Events");
        level1Button.setSelected(true);
		
		level1Button.setActionCommand("Level 1 Only Connect/Disconnect Events");
		level2Button.setActionCommand("Level 2 Command Event and Connect/Disconnect Events");
        level1Button.addActionListener(this);
		level2Button.addActionListener(this);
        group = new ButtonGroup();
		group.add(level1Button);
		group.add(level2Button);
        //For layout purposes, put the buttons in a separate panel
        buttonPanel = new JPanel(); //use FlowLayout
        logPanel = new JPanel(new GridLayout(0, 1));
		TitledBorder  mediaBorder=BorderFactory.createTitledBorder("Choose your Media Path");
		mediaBorder.setTitleColor(Color.blue);
		buttonPanel.setBorder(mediaBorder);
        TitledBorder  logBorder=BorderFactory.createTitledBorder("Console Log Level");
		logBorder.setTitleColor(Color.blue);
		logPanel.setBorder(logBorder);

		buttonPanel.add(openButton);
        buttonPanel.add(field);

		logPanel.add(level1Button);
		logPanel.add(level2Button);


        //Add the buttons and the log to this panel.
        add(buttonPanel, BorderLayout.NORTH);
         add(logPanel, BorderLayout.CENTER);
    }

    public void actionPerformed(ActionEvent e) {

        //Handle open button action and level Button.
        if (e.getSource() == openButton) {
            int returnVal = fc.showOpenDialog(Ps2RealityFileChooser.this);

            if (returnVal == JFileChooser.APPROVE_OPTION) {
                  File file = fc.getSelectedFile();
				  ps2VfsRootPath=file.getAbsolutePath();
				  field.setText(ps2VfsRootPath);
				  System.out.println("Path to search media files is: "+ ps2VfsRootPath);
			
			}
    
		

        
        }
		else
		{
			if(e.getSource() == level1Button)
			{
				log=0;
				System.out.println("Log level 1 set");

			}
			else
			{
				System.out.println("Log level 2 set");

				log=1;
			}


		}
    }

  

	
	
}