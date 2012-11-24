package ps2vfs.gui;

class PluginListModel extends javax.swing.AbstractListModel 
  implements javax.swing.event.ListSelectionListener,
  java.awt.event.ActionListener
{
  private java.util.Vector list;
  private javax.swing.JLabel description;
  private javax.swing.JList jList;
  private java.awt.Frame frame;

  public PluginListModel(java.util.Vector plist, javax.swing.JLabel descr, 
			 javax.swing.JList jlist, java.awt.Frame frame) {
    this.list = plist;
    this.description = descr;
    this.jList = jlist;
    this.frame = frame;
  }
  
  public Object getElementAt(int index) {
    ps2vfs.plugin.VfsPlugin plugin = (ps2vfs.plugin.VfsPlugin) list.get(index);
    return plugin.getName();
  }
  
  public void valueChanged(javax.swing.event.ListSelectionEvent e) {
    if(!e.getValueIsAdjusting()) {
      javax.swing.JList jlist = (javax.swing.JList) e.getSource();
      int index = jlist.getSelectedIndex();
      ps2vfs.plugin.VfsPlugin plugin = (ps2vfs.plugin.VfsPlugin) list.get(index);
      String descText = "";
      if(plugin != null) 
	descText = plugin.getDescription();
      description.setText(descText);
    }
  }

  public void actionPerformed(java.awt.event.ActionEvent e) {
    int index = jList.getSelectedIndex();
    ps2vfs.plugin.VfsPlugin plugin = null;
    if(index >= 0) 
      plugin = (ps2vfs.plugin.VfsPlugin) list.get(index);
    if(plugin != null) {
      if("about".equals(e.getActionCommand())) {
	plugin.doAbout(frame);
      } else if("configure".equals(e.getActionCommand())) {
	plugin.doConfigure(frame);
      }
    }
  }
  
  public int getSize() {
    return list.size();
  }
  
}
