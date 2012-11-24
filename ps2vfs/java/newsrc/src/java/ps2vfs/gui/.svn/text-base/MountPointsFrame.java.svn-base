/*
 * MountPointsFrame.java
 *
 * Created on 28. januar 2004, 19:54
 */
package ps2vfs.gui;

import javax.swing.UIManager;
import javax.swing.tree.DefaultMutableTreeNode;
/**
 *
 * @author  Stig Petter Olsrød
 */
public class MountPointsFrame extends javax.swing.JFrame
  implements java.awt.event.ActionListener, javax.swing.event.ChangeListener
{
    
  private ps2vfs.vfs.Ps2Vfs vfs;
  private static final boolean useSystemLookAndFeel = true;
  private OpenFilesDialog openFiles = null;
  private DirTreeListener dirListener = null;

  /** Creates new form MountPointsFrame */
  public MountPointsFrame(ps2vfs.vfs.Ps2Vfs vfs, String aboutText) {
    // System.err.println("Init GUI.");
    this.vfs = vfs;
    initLookAndFeel();
    initComponents();
    initDirTree();
    initPluginsTab();
    initMountPointsTable();
    initServerConfig();
    jFilesButton.addActionListener(this);
    jRefreshCache.addActionListener(this);
    addMount.addActionListener(this);
    remMount.addActionListener(this);
    jAboutText.setText(aboutText);    
    // System.err.println("Init GUI.");
  }
  
  public void notifyOpenFilesChanged() {
    if(openFiles != null) {
      openFiles.refreshModel(true);
    }
  }
  
  private void initLookAndFeel() {
    if (useSystemLookAndFeel) {
      try {
	UIManager.setLookAndFeel(UIManager.getSystemLookAndFeelClassName());
      } catch (Exception e) {
	System.err.println("Couldn't use system look and feel.");
      }
    }
    
    //Make sure we have nice window decorations.
    javax.swing.JFrame.setDefaultLookAndFeelDecorated(true);
  }

  private void initPluginsTab() {
    PluginListModel mdl = new PluginListModel(vfs.getPluginList(), 
					      jPluginsDescriptionText,
					      jPluginList, this);
    jPluginList.setModel(mdl);
    jPluginList.addListSelectionListener(mdl);
    jConfBtn.addActionListener(mdl);
    jAboutBtn.addActionListener(mdl);
  }


  private void initDirTree() {
    dirListener = new DirTreeListener(vfs, dirTree);
    dirTree.addTreeWillExpandListener(dirListener);
    dirTree.setToggleClickCount(1);
    jMountViewButton.addActionListener(dirListener);
    jFullViewButton.addActionListener(dirListener);
    dirListener.setMode(true);
  }

  private void initServerConfig() {
    String[] strLevels = new String[9];
    strLevels[0] = "OFF";
    strLevels[1] = "SEVERE";
    strLevels[2] = "WARNING";
    strLevels[3] = "INFO";
    strLevels[4] = "CONFIG";
    strLevels[5] = "FINE";
    strLevels[6] = "FINER";
    strLevels[7] = "FINEST";
    strLevels[8] = "ALL";
    serverConsoleLogLevel.setModel(new javax.swing.DefaultComboBoxModel(strLevels));
    serverFileLogLevel.setModel(new javax.swing.DefaultComboBoxModel(strLevels));
    serverConsoleLogLevel.addActionListener(this);
    serverFileLogLevel.addActionListener(this);

    int idx = 0;
    String level = vfs.getConsoleLogLevel();
    for(idx = 0; idx < 9; idx++) {
      if(strLevels[idx].equals(level))
	break;
    }
    if(idx >= 9)
      idx = -1;
    serverConsoleLogLevel.setSelectedIndex(idx);

    idx = 0;
    level = vfs.getFileLogLevel();
    for(idx = 0; idx < 9; idx++) {
      if(strLevels[idx].equals(level))
	break;
    }
    if(idx >= 9)
      idx = -1;
    serverFileLogLevel.setSelectedIndex(idx);
    
    serverPort.setColumns(6);
    serverPort.setText(""+vfs.getPort());
    serverPort.addActionListener(this);
  }

  private void initMountPointsTable() {
    //jMountPointTable();
    Object [][] strTab = null;
    ps2vfs.vfs.VfsMountTable mntTab = vfs.getMountTable();
    
    ps2vfs.vfs.MountTableIterator it = mntTab.iterator();
    int mntN = 0;
    while(it.hasNext()) {
      it.next();
      mntN++;
    }
    if(mntN > 0) {
      strTab = new Object[mntN][4];
      int n = 0;
      it = mntTab.iterator();
      
      while(it.hasNext()) {
	ps2vfs.vfs.MountTableEntry ent = (ps2vfs.vfs.MountTableEntry) it.next();

	ps2vfs.vfs.MountPoint mnt = ent.getMountPoint();
	String virtualPath = ent.getVirtualPath();
	strTab[n][0] = virtualPath;
	strTab[n][1] = mnt.getOpenPath();
	strTab[n][2] = mnt.isRecursive() ? "yes" : "no";
	strTab[n][3] = mnt.isHidden() ? "yes" : "no";
	n++;
      }
    }
    jMountPointTable.setModel(new javax.swing.table.DefaultTableModel(strTab,
								      new String [] {
									"Virtual Path", "Open Path", "Recursive", "Hidden" }) {
	public boolean isCellEditable(int row, int column) {
	  return false;
	}
      });
    jMountPointTable.setCellSelectionEnabled(false);
    jMountPointTable.setRowSelectionAllowed(true);
    jMountPointTable.setColumnSelectionAllowed(false);
    jMountPointTable.setSelectionMode(javax.swing.ListSelectionModel.SINGLE_SELECTION );
    if(dirListener != null) 
      dirListener.refresh();
  }

  public void stateChanged(javax.swing.event.ChangeEvent e) {
    System.err.println("Unhandled stateChanged: " + e);
  }
  
  public void actionPerformed(java.awt.event.ActionEvent e) {
    String action = e.getActionCommand();
    if(action.equals("files")) {
      if(openFiles == null) {
	openFiles = new OpenFilesDialog(this, false, vfs);
      }
      openFiles.refreshModel(true);
      openFiles.show();
    } else if(action.equals("RefreshContent")) {
      vfs.clearCache();
    } else if(action.equals("AddMount")) {
      new AddMountDialog(this, true, vfs).show();
      initMountPointsTable();
    } else if(action.equals("RemMount")) {
      int selRow = jMountPointTable.getSelectedRow();
      //System.err.println("Should have deleted row: " + selRow);
      if(selRow >= 0) {
	javax.swing.table.TableModel tblModel = jMountPointTable.getModel();
	String str = (String) tblModel.getValueAt(selRow, 1);
	ps2vfs.vfs.VfsMountTable mntTab = vfs.getMountTable();
	String vpath = (String) tblModel.getValueAt(selRow, 0);
	String opath = (String) tblModel.getValueAt(selRow, 1);
	boolean rec = ((String) tblModel.getValueAt(selRow, 2)).equals("yes");;
	boolean hidden = ((String) tblModel.getValueAt(selRow, 3)).equals("yes");;
	mntTab.removeMountPoint(vpath, opath, rec, hidden);
	// System.err.println("Open Path: " + str);
	vfs.persistParams();
      }
      initMountPointsTable(); 
    } else if(e.getSource() == serverPort ) {
      String numEntStr = serverPort.getText();
      int numEnt = 0;
      if(numEntStr != null) {
	try {
	  numEnt = Integer.parseInt(numEntStr);
	  vfs.setPort(numEnt);
	  vfs.persistParams();
	} catch(NumberFormatException ee) {
	  System.out.println("Failed to parse " + numEntStr);
	  serverPort.setText(""+ vfs.getPort());
	}
      }
    } else if(e.getSource() == serverConsoleLogLevel ) {
      String level = (String) serverConsoleLogLevel.getSelectedItem();
      if(level != null) {
	vfs.setConsoleLogLevel(level);
	vfs.persistParams();
      }
    } else if(e.getSource() == serverFileLogLevel ) {
      String level = (String) serverFileLogLevel.getSelectedItem();
      if(level != null) {
	vfs.setFileLogLevel(level);
	vfs.persistParams();
      }
    } else {
      System.err.println("Unhandled actionPerformed: " + e.getActionCommand() + " src:" + e.getSource());
    }
  }
    /** This method is called from within the constructor to
     * initialize the form.
     * WARNING: Do NOT modify this code. The content of this method is
     * always regenerated by the Form Editor.
     */
    private void initComponents() {//GEN-BEGIN:initComponents
        buttonGroup1 = new javax.swing.ButtonGroup();
        buttonGroup2 = new javax.swing.ButtonGroup();
        jTabbedPane1 = new javax.swing.JTabbedPane();
        jStatusPanel = new javax.swing.JPanel();
        jPanel1 = new javax.swing.JPanel();
        jLabel1 = new javax.swing.JLabel();
        jScrollPane1 = new javax.swing.JScrollPane();
        dirTree = new JTree14();
        jPanel2 = new javax.swing.JPanel();
        jStatusText = new javax.swing.JFormattedTextField();
        jPanel3 = new javax.swing.JPanel();
        jMountViewButton = new javax.swing.JRadioButton();
        jFullViewButton = new javax.swing.JRadioButton();
        jFilesButton = new javax.swing.JButton();
        jRefreshCache = new javax.swing.JButton();
        jConfPanel = new javax.swing.JPanel();
        jPanel7 = new javax.swing.JPanel();
        addMount = new javax.swing.JButton();
        remMount = new javax.swing.JButton();
        jScrollPane4 = new javax.swing.JScrollPane();
        jMountPointTable = new javax.swing.JTable();
        jPanel4 = new javax.swing.JPanel();
        serverPort = new javax.swing.JTextField();
        serverConsoleLogLevel = new javax.swing.JComboBox();
        serverFileLogLevel = new javax.swing.JComboBox();
        jPluginPanel = new javax.swing.JPanel();
        jSplitPane1 = new javax.swing.JSplitPane();
        jPluginDescriptionPanel = new javax.swing.JPanel();
        jPanel6 = new javax.swing.JPanel();
        jPluginsDescriptionText = new javax.swing.JLabel();
        jPanel5 = new javax.swing.JPanel();
        jConfBtn = new javax.swing.JButton();
        jAboutBtn = new javax.swing.JButton();
        jScrollPane2 = new javax.swing.JScrollPane();
        jPluginList = new javax.swing.JList();
        AboutPanel = new javax.swing.JPanel();
        jScrollPane3 = new javax.swing.JScrollPane();
        jAboutText = new javax.swing.JLabel();

        setTitle("Ps2Vfs");
        addWindowListener(new java.awt.event.WindowAdapter() {
            public void windowClosing(java.awt.event.WindowEvent evt) {
                exitForm(evt);
            }
        });

        jStatusPanel.setLayout(new java.awt.BorderLayout());

        jPanel1.setLayout(new javax.swing.BoxLayout(jPanel1, javax.swing.BoxLayout.Y_AXIS));

        jLabel1.setText("Directories");
        jPanel1.add(jLabel1);

        jScrollPane1.setViewportView(dirTree);

        jPanel1.add(jScrollPane1);

        jStatusPanel.add(jPanel1, java.awt.BorderLayout.CENTER);

        jPanel2.setLayout(new javax.swing.BoxLayout(jPanel2, javax.swing.BoxLayout.X_AXIS));

        jPanel2.add(jStatusText);

        jStatusPanel.add(jPanel2, java.awt.BorderLayout.SOUTH);

        jPanel3.setLayout(new javax.swing.BoxLayout(jPanel3, javax.swing.BoxLayout.Y_AXIS));

        jMountViewButton.setSelected(true);
        jMountViewButton.setText("Mount View");
        jMountViewButton.setToolTipText("Select mount point view for the tree");
        buttonGroup1.add(jMountViewButton);
        jMountViewButton.setActionCommand("mount");
        jPanel3.add(jMountViewButton);

        jFullViewButton.setText("Full View");
        jFullViewButton.setToolTipText("Turn on full content view for the tree");
        buttonGroup1.add(jFullViewButton);
        jFullViewButton.setActionCommand("full");
        jPanel3.add(jFullViewButton);

        jFilesButton.setText("Open Files");
        jFilesButton.setToolTipText("Show a list of open files with information");
        jFilesButton.setActionCommand("files");
        jPanel3.add(jFilesButton);

        jRefreshCache.setText("Refresh Content");
        jRefreshCache.setToolTipText("Clear the cache to refresh content");
        jRefreshCache.setActionCommand("RefreshContent");
        jPanel3.add(jRefreshCache);

        jStatusPanel.add(jPanel3, java.awt.BorderLayout.EAST);

        jTabbedPane1.addTab("Status", jStatusPanel);

        jConfPanel.setLayout(new java.awt.BorderLayout());

        jPanel7.setMinimumSize(new java.awt.Dimension(100, 100));
        jPanel7.setPreferredSize(new java.awt.Dimension(100, 100));
        addMount.setText("Mount");
        addMount.setToolTipText("Add a mount point to the VFS");
        addMount.setActionCommand("AddMount");
        jPanel7.add(addMount);

        remMount.setText("Unmount");
        remMount.setActionCommand("RemMount");
        jPanel7.add(remMount);

        jConfPanel.add(jPanel7, java.awt.BorderLayout.EAST);

        jMountPointTable.setModel(new javax.swing.table.DefaultTableModel(
            new Object [][] {
                {null, null, null, null},
                {null, null, null, null},
                {null, null, null, null},
                {null, null, null, null}
            },
            new String [] {
                "Title 1", "Title 2", "Title 3", "Title 4"
            }
        ));
        jScrollPane4.setViewportView(jMountPointTable);

        jConfPanel.add(jScrollPane4, java.awt.BorderLayout.CENTER);

        jPanel4.setLayout(new javax.swing.BoxLayout(jPanel4, javax.swing.BoxLayout.Y_AXIS));

        jPanel4.setBorder(new javax.swing.border.TitledBorder("Server Configuration"));
        serverPort.setText("6969");
        serverPort.setToolTipText("Enter the port number for the server");
        serverPort.setBorder(new javax.swing.border.TitledBorder(new javax.swing.border.TitledBorder(""), "Port"));
        jPanel4.add(serverPort);

        serverConsoleLogLevel.setToolTipText("Adjust Console Log Level");
        serverConsoleLogLevel.setBorder(new javax.swing.border.TitledBorder("Console Log Level"));
        jPanel4.add(serverConsoleLogLevel);

        serverFileLogLevel.setToolTipText("Adjust File Log Level");
        serverFileLogLevel.setBorder(new javax.swing.border.TitledBorder("File Log Level"));
        jPanel4.add(serverFileLogLevel);

        jConfPanel.add(jPanel4, java.awt.BorderLayout.NORTH);

        jTabbedPane1.addTab("Configure", jConfPanel);

        jPluginPanel.setLayout(new java.awt.BorderLayout());

        jSplitPane1.setDividerSize(5);
        jSplitPane1.setResizeWeight(0.3);
        jPluginDescriptionPanel.setLayout(new javax.swing.BoxLayout(jPluginDescriptionPanel, javax.swing.BoxLayout.Y_AXIS));

        jPluginDescriptionPanel.setMinimumSize(new java.awt.Dimension(50, 50));
        jPanel6.setLayout(new java.awt.BorderLayout());

        jPanel6.setBorder(new javax.swing.border.TitledBorder("Description"));
        jPluginsDescriptionText.setVerticalAlignment(javax.swing.SwingConstants.TOP);
        jPanel6.add(jPluginsDescriptionText, java.awt.BorderLayout.CENTER);

        jPluginDescriptionPanel.add(jPanel6);

        jPanel5.setLayout(new javax.swing.BoxLayout(jPanel5, javax.swing.BoxLayout.X_AXIS));

        jConfBtn.setText("Configure");
        jConfBtn.setActionCommand("configure");
        jPanel5.add(jConfBtn);

        jAboutBtn.setText("About");
        jAboutBtn.setActionCommand("about");
        jPanel5.add(jAboutBtn);

        jPluginDescriptionPanel.add(jPanel5);

        jSplitPane1.setRightComponent(jPluginDescriptionPanel);

        jPluginList.setSelectionMode(javax.swing.ListSelectionModel.SINGLE_SELECTION);
        jScrollPane2.setViewportView(jPluginList);

        jSplitPane1.setLeftComponent(jScrollPane2);

        jPluginPanel.add(jSplitPane1, java.awt.BorderLayout.CENTER);

        jTabbedPane1.addTab("Plugins", jPluginPanel);

        AboutPanel.setLayout(new java.awt.BorderLayout());

        AboutPanel.setBorder(new javax.swing.border.TitledBorder("Greetings and ChangeLog"));
        jAboutText.setText("jLabel2");
        jAboutText.setVerticalAlignment(javax.swing.SwingConstants.TOP);
        jAboutText.setVerticalTextPosition(javax.swing.SwingConstants.TOP);
        jScrollPane3.setViewportView(jAboutText);

        AboutPanel.add(jScrollPane3, java.awt.BorderLayout.CENTER);

        jTabbedPane1.addTab("About", AboutPanel);

        getContentPane().add(jTabbedPane1, java.awt.BorderLayout.CENTER);

        pack();
    }//GEN-END:initComponents
    
    /** Exit the Application */
    private void exitForm(java.awt.event.WindowEvent evt) {//GEN-FIRST:event_exitForm
      vfs.persistParams();
      System.exit(0);
    }//GEN-LAST:event_exitForm
    
    /**
     * @param args the command line arguments
     */
    public static void main(String args[]) {
    }
    
    
    // Variables declaration - do not modify//GEN-BEGIN:variables
    private javax.swing.JPanel AboutPanel;
    private javax.swing.JButton addMount;
    private javax.swing.ButtonGroup buttonGroup1;
    private javax.swing.ButtonGroup buttonGroup2;
    private javax.swing.JTree dirTree;
    private javax.swing.JButton jAboutBtn;
    private javax.swing.JLabel jAboutText;
    private javax.swing.JButton jConfBtn;
    private javax.swing.JPanel jConfPanel;
    private javax.swing.JButton jFilesButton;
    private javax.swing.JRadioButton jFullViewButton;
    private javax.swing.JLabel jLabel1;
    private javax.swing.JTable jMountPointTable;
    private javax.swing.JRadioButton jMountViewButton;
    private javax.swing.JPanel jPanel1;
    private javax.swing.JPanel jPanel2;
    private javax.swing.JPanel jPanel3;
    private javax.swing.JPanel jPanel4;
    private javax.swing.JPanel jPanel5;
    private javax.swing.JPanel jPanel6;
    private javax.swing.JPanel jPanel7;
    private javax.swing.JPanel jPluginDescriptionPanel;
    private javax.swing.JList jPluginList;
    private javax.swing.JPanel jPluginPanel;
    private javax.swing.JLabel jPluginsDescriptionText;
    private javax.swing.JButton jRefreshCache;
    private javax.swing.JScrollPane jScrollPane1;
    private javax.swing.JScrollPane jScrollPane2;
    private javax.swing.JScrollPane jScrollPane3;
    private javax.swing.JScrollPane jScrollPane4;
    private javax.swing.JSplitPane jSplitPane1;
    private javax.swing.JPanel jStatusPanel;
    private javax.swing.JFormattedTextField jStatusText;
    private javax.swing.JTabbedPane jTabbedPane1;
    private javax.swing.JButton remMount;
    private javax.swing.JComboBox serverConsoleLogLevel;
    private javax.swing.JComboBox serverFileLogLevel;
    private javax.swing.JTextField serverPort;
    // End of variables declaration//GEN-END:variables
    
}
