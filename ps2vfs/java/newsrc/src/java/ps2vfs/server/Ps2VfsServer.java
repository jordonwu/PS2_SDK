package ps2vfs.server;

import java.net.*;
import java.io.*;
import java.util.*;

public class Ps2VfsServer extends Thread
{   
  //todo admin delete client 
  Vector clientList= new Vector();
  Vector clientListcomandos= new Vector();
  int counter=0;
  
  public Ps2VfsServer(int iPort, ps2vfs.vfs.Ps2Vfs iVfs, java.util.logging.Logger iLog) {
    log = iLog;
    port = iPort;
    vfs = iVfs;
    
    log.info("# Playstation 2 Virtual File System Example");
    log.info("#  _____     ___ ____");
    log.info("#   ____|   |    ____|      PSX2 OpenSource Project");
    log.info("#  |     ___|   |____       (C)2003,Bigboss, Mavy & Hermes ");
    log.info("#             (bigboss@ps2reality.net,mavy@ps2reality.net,hermes@ps2reality.net)");
    log.info("# ------------------------------------------------------------------------------");
  }
  
  public void run() 
  {
    Thread tempThread;
    Ps2VfsClient ps2Client;
    ServerSocket serverSocket = null;
    boolean listening = true;
    try{
      serverSocket = new ServerSocket(port);
    } catch(IOException e){
      log.severe("Error opening server socket on port: " + port);
      e.printStackTrace(System.out);
      return;
    }

    log.info("Server listening on port " + port);
    while (listening){ 
      try{
	ps2Client = new Ps2VfsClient(vfs, serverSocket.accept(), log);
      } catch(IOException e){
	log.severe("Error opening server socket on port: " + port);
	return;
      }
      
      //counter++;
      clientList.add(ps2Client);
      tempThread = new Thread(ps2Client);
      tempThread.start();
    }
  } 

  public static void main(String[] args) throws IOException {
    Ps2VfsServer server = new Ps2VfsServer(6969, null, null);
    server.run();
  }
  
  private ps2vfs.vfs.Ps2Vfs        vfs;
  private int                      port;
  private java.util.logging.Logger log;
}
