package org.apache.commons.vfs.ps2reality;
import java.net.*;
import java.io.*;
import java.util.*;
//This import for support jakarta vfs for command dir, pwf, and next features
import org.apache.commons.vfs.FileObject;
import org.apache.commons.vfs.FileSystemException;
import org.apache.commons.vfs.FileSystemManager;
import org.apache.commons.vfs.FileType;
import org.apache.commons.vfs.FileUtil;
import org.apache.commons.vfs.VFS;
import org.apache.commons.vfs.Selectors;
/*java nio*/
import java.nio.*;
import java.nio.channels.*;
import java.nio.charset.*;

class Ps2VfsClientThread extends Thread {

    //Strings for our commands
	public String HELLO="HELLO";
	public String OPEN="OPENA";
	public String CLOSE="CLOSE";
	public String READ="READA";
	public String SEEK="SEEKA";
	public String EXIT="EXITA";
	public String DIR="NEXTA";
	public String PWD="PWDAA";
	public String TEXT="TEXTA";
    public String IOERROR="IOERR";
    public int actualEntry;
	public int totalEntries;
    //ByteBuffer structure for our commands
	public ByteBuffer cmdHello;
	public ByteBuffer cmdOpen;
	public ByteBuffer cmdClose;
	public ByteBuffer cmdRead;
	public ByteBuffer cmdReadCache;
	public ByteBuffer cmdSeek;
	public ByteBuffer cmdExit;
	public ByteBuffer cmdDir;
	public ByteBuffer cmdPwd;
	public ByteBuffer command;
	public ByteBuffer salida;
	public byte commandR[];
	public ByteBuffer readBytes;
	public Charset charset;
    public CharsetDecoder decoder;
    //
	//For pwd and dir commands we need a jakarta VFS FileSystemManager and a FileObject
	private  FileSystemManager mgr;
	private FileObject cwd;
	private FileObject realDirs[];
    //hashtable for our open files
	private Hashtable filesopened;
	//max number of file in java side, but if i am opening a new thread with each open ... need change
	private final static int maxnum=100;
    //index for add to hashtable... need change
	private int numfilesopened=0;
	int logLevel;
	//Our eyes in java side
	PrintWriter screenOut = new PrintWriter(System.out, true);

	public String  pathMedia;
	private SocketChannel socketChannel;
	
	public String clientIP;
	
	
	public String commandString;
	public int numbytes;
	public int mode;
	public int code;
	public boolean done=false;
	
	public int ioerrorCounter=0;
	public Ps2VfsClientThread(SocketChannel sc, String pathM,FileSystemManager mgrserver,Hashtable filesOpenedServer,int log){
		super("Ps2VfsClientThread");
		this.socketChannel = sc;
		this.pathMedia=pathM;
		this.mgr=mgrserver;
        this.logLevel=log;
		this.filesopened=filesOpenedServer;
		this.command=ByteBuffer.allocateDirect(9);
		this.charset = Charset.forName("ISO-8859-15");
		this.decoder = charset.newDecoder();

		initCommand(); //init our commands
		
	}

	
		
	

	
	public String readCommandString( )
	{
		
		
		try
		{
			command.clear();
			socketChannel.read(command);
			command.clear();
			code=command.getInt();
			//in.read(command);
			//return command.asCharBuffer().toString();
			command.get(commandR,0,commandR.length);
			return new String(commandR);
		}
		catch(IOException e)
		{
			//ioerrorCounter++;
			return IOERROR;  
         
		}
	}
	
	
	public void Hello()
	{   
	
		try
		{
            
			cmdHello.clear();
			cmdHello.putInt(0);
			cmdHello.clear();
			socketChannel.write(cmdHello);
		}
		catch(IOException e)
		{
			screenOut.println("Error IO processing command HELLO");
		}
	}
	public  void Exit()
	{
		try
		{
            
			cmdExit.clear();
			cmdExit.putInt(0);
			cmdExit.clear();
			socketChannel.write(cmdExit);
		}
		catch(IOException e)
		{
			screenOut.println("Error IO processing command EXIT");
			Ps2VfsServer.deleteClient(this);
		}
       

	}
	public void Text()
	{
			ByteBuffer log=ByteBuffer.allocate(this.code);
		   try
			{
			socketChannel.read(log);
			log.clear();
			screenOut.println("MuereDemonioLog: "+decoder.decode(log));
			}
			catch(IOException e)
			{
			screenOut.println("Error IO processing command TEXT");
			
			}
	}
	public void Open()
	{
		
		int fd;
		try{
			ByteBuffer name=ByteBuffer.allocate(this.code);
			socketChannel.read(name);
			name.clear();
			String filename=new String(name.array());
            System.out.println("filename: "+filename);
			System.out.println("filename: "+pathMedia);
			if(numfilesopened<maxnum)
			{
				RandomAccessFile rfile=new RandomAccessFile(pathMedia+System.getProperty("file.separator")+filename,"r");
				numfilesopened++;
				filesopened.put(new Integer(numfilesopened),rfile.getChannel());
				screenOut.println("Number of files opened in ps2 side "+numfilesopened);
			    fd=numfilesopened;
			}
			else
			{
                screenOut.println("Sorry maxnum files already opened");
				fd=-1;
			}
	
		
			cmdOpen.clear();
			cmdOpen.putInt(fd);
			cmdOpen.clear();
			socketChannel.write(cmdOpen);
			 
		}catch (IOException e ) //catch first exception for file not found
        {

         //  e.printStackTrace();
		    screenOut.println("Sorry file doesn't exist");
		    fd=-1;
			try{
			cmdOpen.clear();
			cmdOpen.putInt(fd);
			cmdOpen.clear();
			socketChannel.write(cmdOpen);
			}catch (IOException e2 )
			{
				e2.printStackTrace();
				System.exit( 1 );
			}
          
        }
		
		

	}
	public void Close()
	{
		Integer fdint=new Integer(this.code);
		int returnCode;
		try{
			if(filesopened.containsKey(fdint))
			{
				FileChannel rfile=(FileChannel)filesopened.get(fdint);
				rfile.close();
                screenOut.println("File with fd "+ this.code+" closed");
				filesopened.remove(fdint);
                numfilesopened--;
				screenOut.println("Number of files opened in ps2 side "+numfilesopened);
                returnCode=0;

			}
			else
			{
				screenOut.println("Sorry fd "+ this.code+"does not exist");
				returnCode=-1;

			}
			cmdClose.clear();
			cmdClose.putInt(returnCode);
			cmdClose.clear();
			socketChannel.write(cmdClose);
			
		}
		catch ( IOException e1 )
        {
            e1.printStackTrace();
            returnCode=-1;
		    try
			{
				cmdClose.clear();
				cmdClose.putInt(returnCode);
				cmdClose.clear();
				socketChannel.write(cmdClose);
			}
			catch (IOException e2 )
			{
				e2.printStackTrace();
				System.exit( 1 );
			}
			
        }



	}
	public void  Read()
	{
				 
		 
		int returnCode;
		Integer fdint=new Integer(this.code);
		int size;
		ByteBuffer numBytes=ByteBuffer.allocate(4);

		try{
			
			socketChannel.read(numBytes);
			numBytes.clear();
			this.numbytes=numBytes.getInt();
			//screenOut.println(" numbytes to read "+ this.numbytes);
			
			if(filesopened.containsKey(fdint))
			{
			    FileChannel rfile=(FileChannel)filesopened.get(fdint);

				long tam=rfile.size()-rfile.position();
				//check if we have bytes
				if (tam>=numbytes)
				{
					size=numbytes;
				}
				else
				{
					size=(int)tam;
				}
				if(size>=0)
				{
					cmdRead.clear();
					cmdRead.putInt(size);
					cmdRead.clear();
					socketChannel.write(cmdRead);
					rfile.position(rfile.position()+rfile.transferTo(rfile.position(),size,socketChannel));
				}
				else
				{	
					cmdRead.clear();
					cmdRead.putInt(-1);
					cmdRead.clear();
					socketChannel.write(cmdRead);
				}
                
				
			}
			else
			{
				screenOut.println("Sorry fd "+ this.code+"does not exist");
				returnCode=-1;
				cmdRead.clear();
				cmdRead.putInt(returnCode);
				cmdRead.clear();
				socketChannel.write(cmdRead);
			}
			
		}
		catch ( IOException e1 )
        {
            e1.printStackTrace();
            returnCode=-1;
			try{
			
				cmdRead.clear();
				cmdRead.putInt(returnCode);
				cmdRead.clear();
				socketChannel.write(cmdRead);

			}
			catch (IOException e2 )
			{
				e2.printStackTrace();
				System.exit( 1 );
			}
        }	
	}
	
	public void  Seek()
	{
		
		int numBytes;
		int modeSeek;
		long lnumbytes;
		long currentpos;
	    int returnCode;
		Integer fdint=new Integer(this.code);
	
		try
		{
			readBytes.clear();
			socketChannel.read(readBytes);
			readBytes.clear();
			numBytes=readBytes.getInt();
			modeSeek=readBytes.getInt();
			if(logLevel==1)
            screenOut.println(numBytes+" mode"+modeSeek+" code "+this.code);
			lnumbytes=(long)numBytes;
				if(filesopened.containsKey(fdint))
				{
					FileChannel rfile=(FileChannel)filesopened.get(fdint);
					long size=rfile.size();
                
					switch(modeSeek)
					{   
						case 0://SEEK_SET
							
								if(lnumbytes<=size)
								{
									rfile.position(lnumbytes);
									returnCode=numBytes;
								}
								else
								{	
									rfile.position(size);
									returnCode=(int)size;
								}
								
						
								break;
						case 1://SEEK_CURR
								currentpos=rfile.position();
								if(currentpos+lnumbytes<=size && currentpos+lnumbytes>=0 )
								{
									rfile.position(currentpos+lnumbytes);
									returnCode=(int)(currentpos+lnumbytes);
								}
								else
								{
									if(currentpos+lnumbytes>size)
									{
										rfile.position(size);
										returnCode=(int)size;
									}
									else
									{	
										rfile.position(0);
										returnCode=0;

									}
								}
								returnCode=(int)rfile.position();
								break;
						case 2:  //SEEK_END
								if(lnumbytes==0)
								{	
									rfile.position(size);
									returnCode=(int)size;
								}
								else
								{
									returnCode=-1;
								}
								
								break;
						default:   
								returnCode=-1;
								break;
					}
					
			
				}
				else
				{
					  screenOut.println("Sorry fd "+ this.code+" does not exist in java server");
					returnCode=-1;

				}
				
				cmdSeek.clear();
				cmdSeek.putInt(returnCode);
				cmdSeek.clear();
				socketChannel.write(cmdSeek);
				
		}
		catch ( IOException e1 )
        {
            e1.printStackTrace();
            returnCode=-1;
			try{
				cmdSeek.clear();
				cmdSeek.putInt(returnCode);
				cmdSeek.clear();
				socketChannel.write(cmdSeek);
		    }
			catch (IOException e2 )
			{
				e2.printStackTrace();
				System.exit( 1 );
			}
        }	
	}
	public String getName(FileObject file)
	{
		
		return file.getName().getBaseName();
	}
	public void Pwd()
	{
		int size;
		int i=0;
		
		

		int num=512;
		
		actualEntry=0;
		try{

			

            cwd = mgr.resolveFile( cwd, pathMedia );
       
	        if(this.code!=0)
			{
				ByteBuffer name=ByteBuffer.allocate(this.code);
				socketChannel.read(name);
				name.clear();
				String filename=new String(name.array());
				cwd = mgr.resolveFile( cwd, filename );
				
			}
			
			System.out.println( "Current folder is " + cwd.getName() );
			
			
		
			realDirs = cwd.getChildren();
			cmdPwd.clear();
			cmdPwd.putInt(realDirs.length);
			cmdPwd.clear();

			totalEntries=realDirs.length;
			//System.out.println( "Entries" + totalEntries );
			salida.clear();
			if(totalEntries>0)
			{
				if(totalEntries<512)
				{
					num=totalEntries;
				}
				else
				{
					num=512;
				}
			}
			else
			{	if(totalEntries==0)
				{
					num=0;
					
				}
			}
			
			for(i=0;i<num;i++)
			{
				salida.position(i*128);
				if ( realDirs[i].getType() == FileType.FOLDER )
				{
					
					if(i==(num-1))
					{
						
						salida.put((byte)0x82);
						
					}
					else
					{	
						
						salida.put((byte)0x2);
					}
					
				}
				if ( realDirs[i].getType() == FileType.FILE )
				{
					
					if(i==(num-1))
					{
						salida.put((byte)0x80);
					}
					else
					{	
						salida.put((byte)0x0);
					}
				}
				
				String cadena=getName(realDirs[i]);
				salida.position(128*i+1);
				salida.put(cadena.getBytes());
			}
			
			if(num<512 )
				{
					salida.position(i*128);
					salida.put((byte)0xff);

				}
			else
			{
				actualEntry=512;
			}
			cmdPwd.clear();
			socketChannel.write(cmdPwd);
			salida.clear();
			socketChannel.write(salida);
			//System.out.println( "Salida mandada numero de entradas de pwd "+num);
		 
		}catch ( IOException e )//catch first  FileSystemException 
        {
            e.printStackTrace();
			
			try{
				cmdPwd.clear();
				cmdPwd.putInt(-1);
				cmdPwd.clear();
				socketChannel.write(cmdPwd);

			}catch (IOException e2 )
			{
				e2.printStackTrace();
				System.exit( 1 );
			}
        }
	}
	public void Dir()
	{
		int i;
		int j=actualEntry;
		int num;
   		salida.clear();
		try
		{
			if(totalEntries==512)
				
			{
				salida.clear();
				salida.put((byte)0xff);
			}
			else
			{
				if(actualEntry<totalEntries)
				{
					if((totalEntries-actualEntry)<512)
						
					{ num=totalEntries-actualEntry;
					}
					else
					{num=512;
					
					}
				}
				else
				{	num=1;
				}
				for(i=0;i<num;i++)
				{
					salida.position(i*128);
					if ( realDirs[j].getType() == FileType.FOLDER )
					{
					
						if(j==(totalEntries-1))
						{
							salida.put((byte)0x82);
						
						}
						else
						{	
							salida.put((byte)0x2);
						
						}
					
					}
					if ( realDirs[j].getType() == FileType.FILE )
					{
					
						if(j==(totalEntries-1))
						{
							salida.put((byte)0x80);
						
						}
						else
						{	
							salida.put((byte)0x0);
						
						}
					}	
			

				
					String cadena=getName(realDirs[j]);
				//System.out.println(cadena+":"+cadena.getBytes().length);
					salida.position(128*i+1);
					salida.put(cadena.getBytes());
					j++;
				}
				actualEntry=j;
				cmdDir.clear();
				cmdDir.putInt(num);
				cmdDir.clear();
				socketChannel.write(cmdDir);
				salida.clear();
				socketChannel.write(salida);
				System.out.println( "Salida mandada numero de entradas de next "+num);
		 

			}
		
		}catch ( Exception e )
        {
            e.printStackTrace();
			try{
				cmdDir.clear();
				cmdDir.putInt(-1);
				cmdDir.clear();
				socketChannel.write(cmdDir);
			}catch (IOException e2 )
			{
				e2.printStackTrace();
				System.exit( 1 );
			}
           
        }
		

	
	}

		public void run(){
		
		clientIP = socketChannel.socket().getInetAddress().getHostAddress();
		screenOut.println("PS2 Client conected from: "+clientIP);
		
			while(!done)
			{   
	
				commandString=readCommandString();
				//screenOut.println("Receive cmd:"+commandString+" code= "+code);
				switch(commandString.charAt(0))
				{

					case 'h': 
						if(logLevel==1)
						screenOut.println("command Hello called ");
						Hello();
						break;
					case 'o': 
						if(logLevel==1)
						screenOut.println("command Open called ");
						Open();
						break;
					case 'c':
						if(logLevel==1)
						screenOut.println("Ps2Vfs Close called ");
						Close();
						break;
					case 'r':       		
						if(logLevel==1)
						screenOut.println("Ps2Vfs Read called");
						Read();
						break;	
					case 's':
						if(logLevel==1)
                        screenOut.println("Ps2Vfs Seek called ");
						Seek();
						break;
					case 'n':
						if(logLevel==1)
                        screenOut.println("Ps2Vfs Dir called ");
						Dir();
						break;
					case 'p':
						if(logLevel==1)
                        screenOut.println("Ps2Vfs Pwd called ");
						Pwd();
						break;
					case 't':
						if(logLevel==1)
                        screenOut.println("Ps2Vfs Text called ");
						Text();
						break;
					case 'I':
					//	if(ioerrorCounter==20)
					//	{
					//		done=true;
					//	}
						break;
					default:
						if(logLevel==1)
						screenOut.println("Command not recognized:"+commandString);
						break;
					case 'e': 
						if(logLevel==1)
						screenOut.println("Ps2Vfs Exit called ");
						Exit();
						done=true;
						break;
				}
				
	
			}
			
		try
		{
			socketChannel.close();
		}
		catch(IOException e)
		{
			screenOut.println("Error closing IO");
		}
		
		
	}
	public byte[] returnInt(int a)
	{
		byte array[]=new byte[4];

		array[0]=(byte)(a >> 24 & 0xff);
        array[1]=(byte)(a >> 16 & 0xff);
		array[2]=(byte)(a >> 8 & 0xff);
		array[3]=(byte)(a >> 0 & 0xff);
		return array;
	}

	//initializing byte structure for our commands 
	public void initCommand()
	{
		commandR=new byte[5];
		readBytes=ByteBuffer.allocateDirect(8);
		cmdHello=ByteBuffer.allocateDirect(9);
		cmdOpen=ByteBuffer.allocateDirect(9);
		cmdClose=ByteBuffer.allocateDirect(9);
		cmdRead=ByteBuffer.allocateDirect(9);
		cmdSeek=ByteBuffer.allocateDirect(9);
		cmdExit=ByteBuffer.allocateDirect(9);
		cmdPwd=ByteBuffer.allocateDirect(9);
		cmdDir=ByteBuffer.allocateDirect(9);
		salida=ByteBuffer.allocateDirect(65536);
		
		cmdHello.position(4);
		cmdHello.put(HELLO.getBytes());
		cmdOpen.position(4);
		cmdOpen.put(OPEN.getBytes());
		cmdClose.position(4);
		cmdClose.put(CLOSE.getBytes());
		cmdRead.position(4);
		cmdRead.put(READ.getBytes());
		cmdSeek.position(4);
		cmdSeek.put(SEEK.getBytes());
		cmdExit.position(4);
		cmdExit.put(EXIT.getBytes());
		cmdPwd.position(4);
		cmdPwd.put(PWD.getBytes());
		cmdDir.position(4);
		cmdDir.put(DIR.getBytes());


	}
	
}