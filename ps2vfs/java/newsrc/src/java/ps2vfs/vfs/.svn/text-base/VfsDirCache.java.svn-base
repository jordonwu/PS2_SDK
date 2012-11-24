package ps2vfs.vfs;

import ps2vfs.plugin.*;

class VfsDirCacheEntry {
  private java.util.Date createdTime;
  private VfsDir dir;
  
  VfsDirCacheEntry(VfsDir vd) {
    dir = vd;
    createdTime = new java.util.Date();
  }

  VfsDir getVfsDir() {
    return dir;
  }
  
  long getAge() {
    long now = new java.util.Date().getTime();
    long age = now - createdTime.getTime();
    return age;
  }
}

public class VfsDirCache
{
  private boolean                 debug = false;
  private java.util.LinkedHashMap map;
  private long                    maxAge;
  private int                     maxSize;
  private java.util.logging.Logger log = java.util.logging.Logger.getLogger("ps2vfs");


  public VfsDirCache(int cacheMaxSize, long maxAgeMS) {
    maxSize = cacheMaxSize;

    map = new java.util.LinkedHashMap(maxSize, 1.0f, true) {
	protected boolean removeEldestEntry(java.util.Map.Entry eldest) {
	  return size() > maxSize;
	}
      };
    maxAge = maxAgeMS;
  }
    
  public synchronized VfsDir get(String path) {
    // Normalize paths. They should always end with "/".
    if(!path.endsWith("/")) {
      path = path + "/";
    }
    VfsDirCacheEntry ce = (VfsDirCacheEntry) map.get(path);
    VfsDir dir = null;
    if(ce != null) {
      long age = ce.getAge();
      if(age > maxAge) {
	// Too old to return. Discard it.
	if(debug) {
	  log.finest("VfsDirCache::get(" + path + ") dropping " + 
		     ce.getVfsDir() + " due to age " + age );
	}
	dir = null;
      } else {
	dir = ce.getVfsDir();
      }
    }
    if(debug) {
      log.finest("VfsDirCache::get(" + path + ") returned " + dir);
    }
    return dir;
  }
  
  public synchronized void put(VfsDir dir) {
    if(dir != null) {
      if(debug) 
	log.finest("VfsDirCache::put(" + dir + ")");
      map.put(dir.getPath(), new VfsDirCacheEntry(dir));
    }
  }
  
  public synchronized void clear() {
    if(debug) 
      log.finest("VfsDirCache::clear()");
    map.clear();
  }
}
