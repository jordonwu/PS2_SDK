package ps2vfs.vfs;

// import
import ps2vfs.plugin.*;

class OpenFileDescriptor {
  private VfsOpenFile file;
  private String vPath;
  private ps2vfs.server.Ps2VfsClient client;
  
  OpenFileDescriptor(VfsOpenFile file, String vpath, ps2vfs.server.Ps2VfsClient client) {
    this.file = file;
    this.vPath = vpath;
    this.client = client;
  }

  ps2vfs.server.Ps2VfsClient getClient() {
    return client;
  }

  VfsOpenFile getFile() {
    return file;
  }
  String getVirtualPath() {
    return vPath;
  }
}






