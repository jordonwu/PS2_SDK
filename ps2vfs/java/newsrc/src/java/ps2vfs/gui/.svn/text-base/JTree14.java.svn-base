package ps2vfs.gui;

import com.sun.java.swing.plaf.windows.WindowsTreeUI;
import java.awt.*;
import java.util.*;
import javax.swing.*;
import javax.swing.plaf.basic.*;
import javax.swing.plaf.*;
import javax.swing.tree.*; 

/** 
 * A subclass of JTree that fixes bug 4887931: Tree lines 
 * missing on Windows XP 
 */

public class JTree14 extends JTree {    
  public JTree14() {    }    
  public JTree14(Object[] value) {	super(value);    }    
  public JTree14(Vector value) {	super(value);    }    
  public JTree14(Hashtable value) {	super(value);    }    
  public JTree14(TreeNode root) {	super(root);    }    
  public JTree14(TreeNode root, boolean asksAllowsChildren) {	super(root, asksAllowsChildren);    }    
  public JTree14(TreeModel newModel) {	super(newModel);    }     
  public void setUI(TreeUI ui) {	
    if (ui instanceof WindowsTreeUI &&	    
	System.getProperty("java.version").equals("1.4.2")) {
      ui = new FixedWindowsTreeUI();	
    }	
    super.setUI(ui);
  }     
  
  private static class FixedWindowsTreeUI extends BasicTreeUI {
    protected void ensureRowsAreVisible(int beginRow, int endRow) {
      if(tree != null && beginRow >= 0 && endRow < getRowCount(tree)) {	
	Rectangle visRect = tree.getVisibleRect();	
	if(beginRow == endRow) {
	  Rectangle     scrollBounds = getPathBounds(tree, getPathForRow	
						     (tree, beginRow));
	  if(scrollBounds != null) {	
	    scrollBounds.x = visRect.x;	
	    scrollBounds.width = visRect.width;
	    tree.scrollRectToVisible(scrollBounds);
	  }
	}
	else {		
	  Rectangle   beginRect = getPathBounds(tree, getPathForRow	
						(tree, beginRow));	
	  Rectangle   testRect = beginRect;		
	  int         beginY = beginRect.y;
	  int         maxY = beginY + visRect.height;
	  for(int counter = beginRow + 1; counter <= endRow; counter++) {
	    testRect = getPathBounds(tree,	
				     getPathForRow(tree, counter));
	    if((testRect.y + testRect.height) > maxY)
	      counter = endRow;		
	  }	
	  tree.scrollRectToVisible(new Rectangle(visRect.x, beginY, 1,	
						 testRect.y + testRect.height-beginY));
	}	    
      }	
    }
    protected TreeCellRenderer createDefaultCellRenderer() {
      return new WindowsTreeUI().new WindowsTreeCellRenderer();	
    }
  }
}

