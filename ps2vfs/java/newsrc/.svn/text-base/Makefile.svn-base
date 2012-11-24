all: Ps2Vfs.class install

Ps2Vfs.class: VfsPlugin.class VfsMountTable.class

VfsPlugin.class: VfsOpenFile.class VfsDirEntry.class

ImagePlugin.class: ImagePlugin.java VfsPlugin.class

PlaylistPlugin.class: VfsPlugin.class

VfsMountTable.class: VfsResolvedDir.class VfsDirEntry.class

PLUGINS := ImagePlugin.class \
	PlaylistPlugin.class 

install: $(PLUGINS)
	cp $(PLUGINS) plugins

.SUFFIXES: .java .class
.java.class: 
	javac $<