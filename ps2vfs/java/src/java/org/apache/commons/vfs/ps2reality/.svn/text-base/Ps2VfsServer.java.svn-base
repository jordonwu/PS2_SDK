package org.apache.commons.vfs.ps2reality;
import java.net.*;
import java.io.*;
import java.util.*;
import org.apache.commons.vfs.FileObject;
import org.apache.commons.vfs.FileSystemException;
import org.apache.commons.vfs.FileSystemManager;
import org.apache.commons.vfs.FileType;
import org.apache.commons.vfs.FileUtil;
import org.apache.commons.vfs.VFS;
import org.apache.commons.vfs.Selectors;
/*añadido nio*/
import java.nio.*;
import java.nio.channels.*;

public class Ps2VfsServer extends Thread
{   
	//todo admin delete client 
	static Vector clientList= new Vector();
	static Vector clientListcomandos= new Vector();
	static int counter=0;
	
	
	static PrintWriter screenOut;

	private static  FileSystemManager mgr;
	private static FileObject cwd;
    	private static int numfilesopened=0;
   private static Hashtable filesopened;
   private static String serverIP;
   private static boolean first=true;
      private static boolean consoleMode;

   private static void loadProperties(Properties prop)
	{
		try
		{
			InputStream inProp = new FileInputStream("ps2vfs.properties");
			prop.load(inProp);              
			inProp.close();
		}
		catch ( Exception e )
		{
			            

          screenOut.println("ps2vfs properties file is not present  getting default values");  

		}

	}
	private static int getPort(Properties props)
	{
		int port;
		try{
			port=Integer.parseInt(props.getProperty("port"));
		}
		catch(NumberFormatException e)
		{
			port=6969;

		}
		return port;
	}
	private static boolean getConsoleMode(Properties props)
	{
		boolean console;
		int num;
		try{
			num=Integer.parseInt(props.getProperty("ConsoleMode"));
			if(num==1)
			{
				console=true;
			}
			else
			{
				console=false;
			}

		}
		catch(NumberFormatException e)
		{
			console=false;

		}
		return console;
	}
	private static String getDir(Properties props)
	{
		String root=null;
		int check;
		 root=props.getProperty("root");
		
		 try
		 {
			 check=root.length();

		 }
		 catch(NullPointerException e)
		 {
			root=System.getProperty("user.dir");

		 }

		
		 
		return root;

	}
	private static String getIP(Properties props)
	{
		String ip=null;
		int check;
		 ip=props.getProperty("ip");
		
		 try
		 {
			 check=ip.length();

		 }
		 catch(NullPointerException e)
		 {
			 screenOut.println("Get Default ip ");
			ip=null;
		 }

		
		 
		return ip;

	}
	private static void initVFS(String directorio)
	{

		try
		{
			mgr = VFS.getManager();
			//cwd = mgr.resolveFile( System.getProperty( "user.dir" ) );
			cwd = mgr.resolveFile(directorio);

			filesopened=new Hashtable();
			numfilesopened=0;
			screenOut.println("New Vfs manager");
			screenOut.println("Hashtable fileopened created");
            screenOut.println("Number of files opened in ps2 side "+numfilesopened);
		}
		catch ( Exception e )
        {
            e.printStackTrace();
            System.exit( 1 );
        }

	}
	private static ServerSocketChannel setup(int port) throws IOException
	{
		ServerSocketChannel ssc = ServerSocketChannel.open();
		InetSocketAddress isa;
		if(serverIP!=null)
		{
			 isa = new InetSocketAddress(serverIP, port);
		}
		else
		{
			 isa = new InetSocketAddress(InetAddress.getLocalHost(), port);
		}
		//screenOut.println("ip que coge"+InetAddress.getLocalHost());
		ssc.socket().bind(isa);
		return ssc;
    }
	public static void main(String[] args) throws IOException {
			Thread tempThread;
	        Ps2RealityControlCenter myGui;
			Ps2VfsClientThread ps2Client;
	        ServerSocket serverSocket = null;
			boolean listening = true;
			int port=6969;
			String directorio;
			screenOut = new PrintWriter(System.out, true);	
			screenOut.println("PlayStation 2 Virtual File System with Java NIO");
				
				screenOut.println("#  _____     ___ ____");
				screenOut.println("#   ____|   |    ____|      PSX2 OpenSource Project");
				screenOut.println("#  |     ___|   |____       (C)2003,2004,Bigboss, Mavy & Hermes ");
				screenOut.println("#            (bigboss@ps2reality.net,mavy@ps2reality.net,hermes@ps2reality.net)");
				screenOut.println("#  ----------------------------------------------------------------------------");
				screenOut.println("Checking ps2vfs properties");
            Properties props = new Properties();
				
			loadProperties(props);
				
			port=getPort(props);
			consoleMode=getConsoleMode(props);
			directorio=getDir(props);
			serverIP=getIP(props);
			myGui = new Ps2RealityControlCenter(directorio);
			
			
			try{
						
				
				
				
				
				ServerSocketChannel ssc = setup(port);

			
				screenOut.println("Server listening at port "+port);  /*mInterface.getPath()*/
                screenOut.println("Path to search media files is: "+directorio);
				screenOut.println("Log Level 1 set");

				initVFS(directorio);
				
				while (listening){
				if(consoleMode)
				{
					ps2Client=new Ps2VfsClientThread(ssc.accept(),directorio,mgr,filesopened,0);
				}
				else
				{
					if(first)
					{
						 
						 if (myGui.initControlCenter("PS2Reality Mediaplayer Control Center")) 
						 {
  							myGui.setVisible(true);
       
						 } 
						 else 
						 {
      						javax.swing.JOptionPane.showMessageDialog(null,"Failed to initialize interface: " );
						 }
						 first=false;
					}
					ps2Client = new Ps2VfsClientThread(ssc.accept(),myGui.getPath(),mgr,filesopened,myGui.getLog());
				}
				//counter++;
				screenOut.println("New client connected total= "+counter);
				counter++;
				clientList.add(ps2Client);
				tempThread = new Thread(ps2Client);
				tempThread.start();
				
				}


			}catch(IOException e){
				System.err.println("Error opening server socket in  port: "+port);
				System.exit(-1);
			}
						
			
	}
	static void deleteClient(Ps2VfsClientThread deadClient){
                int num;
                System.out.println("Client disconnected");
               
                num=clientList.indexOf(deadClient);
                clientList.remove(num);
                
                //System.out.println("Numero de usuarios en el sistema: "+contador);

        }

	

}
