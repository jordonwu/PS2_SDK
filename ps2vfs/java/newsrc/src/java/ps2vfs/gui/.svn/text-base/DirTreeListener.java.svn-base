package ps2vfs.gui;
import javax.swing.event.*;
import javax.swing.tree.*;

class DirTreeListener implements TreeWillExpandListener, java.awt.event.ActionListener
{
  private ps2vfs.vfs.Ps2Vfs vfs;
  private javax.swing.JTree tree;
  private boolean           useMountTable;
  private DefaultTreeModel  treeModel;

  public void refresh() {
    setMode(useMountTable);
  }

  public void setMode(boolean useMountTable) {
    this.useMountTable = useMountTable;
    DefaultMutableTreeNode top = new DefaultMutableTreeNode("/");
    if(useMountTable) {
      addTreeNodes("/", top);
    } else {
      top.setAllowsChildren(true);
      //addVfsDirToNode("/", top);
    }
    treeModel = new javax.swing.tree.DefaultTreeModel(top, !useMountTable);
    tree.setModel(treeModel);
    tree.collapsePath(new javax.swing.tree.TreePath(top));
  }

  public DirTreeListener(ps2vfs.vfs.Ps2Vfs vfs, 
			 javax.swing.JTree tree) {
    this.vfs = vfs;
    this.tree = tree;
  }
  
  public void treeWillExpand(TreeExpansionEvent e)
    throws ExpandVetoException 
  {
    StringBuffer strBuf = new StringBuffer();
    Object[] path = e.getPath().getPath();
    if(path != null && path.length > 0) {
      strBuf.append(path[0].toString());
      for(int n = 1; n < path.length; n++) {
	strBuf.append(path[n].toString() + "/");
      }
      DefaultMutableTreeNode node = (DefaultMutableTreeNode) e.getPath().getLastPathComponent();
      if(useMountTable) {
      } else {
	addVfsDirToNode(strBuf.toString(), node);
      }
    }
  }
  
  public void treeWillCollapse(TreeExpansionEvent event)
    throws ExpandVetoException {
  }

  public void actionPerformed(java.awt.event.ActionEvent e) {
    if("mount".equals(e.getActionCommand())) {
      setMode(true);
    } else if("full".equals(e.getActionCommand())) {
      setMode(false);
    }
  }
  
  private boolean addTreeNodes(String path, DefaultMutableTreeNode top) {
    ps2vfs.vfs.VfsMountTable mntTbl = vfs.getMountTable();
    String [] strArray = mntTbl.getChildren(path);
    if(strArray != null) {
      for(int n = 0; n < strArray.length; n++) {
	DefaultMutableTreeNode node = new DefaultMutableTreeNode(strArray[n]);
	boolean hasChildren = addTreeNodes(path + strArray[n] + "/", node);
	node.setAllowsChildren(hasChildren);
	top.add(node);
      }
    } else {
      return false;
    }
    return true;
  }

  private void addVfsDirToNode(String path, DefaultMutableTreeNode node) {
    ps2vfs.plugin.VfsDir dir = null;
    node.removeAllChildren();
    try {
      dir = vfs.getDirContent(path, null, false);
    } catch (java.io.FileNotFoundException e) {
      return;
    }
    ps2vfs.plugin.VfsDirEntry[] entries = dir.listEntries();
    int numEntries = entries.length;
    for(int i = 0; i < numEntries; i++) {	
      node.add(new DefaultMutableTreeNode(entries[i].getVirtualName(), 
					 entries[i].isDirectory()));
    }
    treeModel.reload(node);
  }
}
