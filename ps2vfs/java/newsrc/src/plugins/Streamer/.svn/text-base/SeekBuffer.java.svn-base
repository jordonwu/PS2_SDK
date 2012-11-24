package ps2vfs.pluggable.streamer;

public class SeekBuffer 
{
  public SeekBuffer(int capacity, int seekFirst) {
    this.capacity = capacity;
    this.seekableSize = seekFirst;
    seekBuffer = new byte[seekableSize];
    buffer = new byte[capacity];
    bufWO = 0;
    bufRO = 0;
    readOffset = 0;
    writeOffset = 0;
    debug = false;
    timer = null;
  }
  
  public synchronized void reset() {
    dry = false;
    bufWO = 0;
    bufRO = 0;
    readOffset = 0;
    writeOffset = 0;
    if(seekBuffer == null) {
      seekBuffer = new byte[seekableSize];
    }
  }

  public synchronized int seek(int offset) {
    if(readOffset > seekableSize) {
      // Can not seek since we have read past the seekable size.
      return -1;
    }
    readOffset = offset;
    return offset;
  }
  
  public synchronized void print() {
    System.err.println("SeekBuffer(" + capacity + ", " + seekableSize + ")  " + 
		       " ro=" + readOffset + " wo=" + writeOffset + 
		       " bro=" + bufRO + " bwo=" + bufWO + 
		       " seekActive=" + (seekBuffer != null));
  }
  
  public synchronized int availableRead() {
    int available = (int) (writeOffset - readOffset);
    int maxCap = capacity;
    if(readOffset < seekableSize) 
      maxCap = capacity + seekableSize;

    if(available > maxCap || available < 0) {
      System.err.println("Illegal availableRead value: available=" + available + 
			 ", wo=" + writeOffset + ", ro=" + readOffset + ", mc=" + maxCap);
    }
    if(available == 0 && dry)
      available = -1; // End of file (buffer).
    return available;
  }

  public synchronized int availableWrite() {
    int available = 0;
    int maxCap;
    if(dry) {
      // Can't write after the buffer
      // has been set to dry mode!
      available = -1;
    } else {
      if(readOffset > seekableSize) {
	maxCap = capacity;
      } else {
	maxCap = capacity + seekableSize;
      }
      
      available = maxCap  - (int)(writeOffset - readOffset);
      
      if(available > maxCap || available < 0) {
	System.err.println("Illegal availableWrite value: available=" + available + 
			   ", wo=" + writeOffset + ", ro=" + readOffset + ", mc=" + 
			   maxCap);
      }
    }
    return available;
  }

  public synchronized double fillPercentage() {
    int available = 0;
    int maxCap;
    if(readOffset > seekableSize) {
      maxCap = capacity;
    } else {
      maxCap = capacity + seekableSize;
    }
    available = maxCap  - (int) (writeOffset - readOffset);

    return (1.0 - (available/((double)maxCap))) * 100.0;
  }

  public synchronized int read(byte[] buf, int offset, int len) {
    int available = availableRead();
    if(available <= 0)
      return available;

    int readLen = 0;
    long seekLen = 0;
    int remainLen = len;

    if(remainLen > available) 
      remainLen = available;

    seekLen = seekableSize - readOffset;
    
    if(seekLen > 0) {
      if(seekLen > remainLen)
	seekLen = remainLen;

      int iSeekLen = (int) seekLen;
      if(debug) {
	System.err.println("Reading from seekBuffer ro: " + readOffset + " l: " + iSeekLen + " o: " + offset);
      }
      System.arraycopy(seekBuffer, (int)readOffset, buf, offset, iSeekLen);
      readLen += iSeekLen;
      remainLen -= iSeekLen;
    }
    
    if(remainLen > 0) {
      int endLen = 0;
      int startLen = 0;
      
      if(bufRO >= bufWO) {
	endLen = capacity - bufRO;
	startLen = bufWO;
      } else {
	endLen = bufWO - bufRO;
	startLen = 0;
      }
      if(debug) {
	System.err.println("Reading from ringBuffer ro: " + readOffset + 
			   " bro: " + bufRO + " bwo: " + bufWO + 
			   " el: " + endLen + " sl: " + startLen);
      }

      if(endLen > remainLen) {
	endLen = remainLen;
	startLen = 0;
      }
      if(endLen > 0) {
	if(debug) {
	  System.err.println("Reading from ringBuffer end: " + bufRO +  
			     " l: " + endLen + " offs: " + (offset + readLen));
	}
	System.arraycopy(buffer, bufRO, buf, offset + readLen, endLen);
	readLen += endLen;
	remainLen -= endLen;
	bufRO += endLen;
      }
      
      if(startLen > remainLen) 
	startLen = remainLen;

      if(startLen > 0) {
	if(debug) {
	  System.err.println("Reading from ringBuffer end: " + 0 +  
			     " l: " + startLen + " offs: " + (offset + readLen));
	}
	System.arraycopy(buffer, 0, buf, offset + readLen, startLen);
	readLen += startLen;
	remainLen -= startLen;
	bufRO = startLen;
      }
    }
    
    if(readLen > 0) {
      readOffset += readLen;
      // We read something, so notify any waiting writers, so they can 
      // fill in more if they want to.
      notifyAll();
    }

    if(seekBuffer != null && readOffset > seekableSize) { 
      // remove reference since this buffer is no longer needed.
      seekBuffer = null; 
    }
    if(debug) {
      System.err.println("Read o: " + offset + " l: " + len + " rl: " + 
			 readLen + " reml: " + remainLen + 
			 " ro: " + readOffset + " wo: " + writeOffset + 
			 " bro: " + bufRO + " bwo: " + bufWO);
    }

    return readLen;
  }

  public synchronized int write(byte[] buf, int offset, int len) {
    int available = availableWrite();

    if(available <= 0) 
      return available;

    int writtenLen = 0;
    int remainLen = len;

    if(remainLen > available) 
      remainLen = available;
    
    long lSeekLen = seekableSize - writeOffset;
    
    if(lSeekLen > 0) {
      if(lSeekLen > remainLen)
	lSeekLen = remainLen;

      int seekLen = (int) lSeekLen;
      
      if(debug) {
	System.err.println("Writing to seekBuffer ro: " + writeOffset + 
			   " l: " + seekLen + " o: " + offset);
      }
      System.arraycopy(buf, offset, seekBuffer, (int) writeOffset, seekLen);
      writtenLen += seekLen;
      remainLen -= seekLen;
    }
    
    if(remainLen > 0) {
      int endLen = 0;
      int startLen = 0;
      
      if(bufRO > bufWO) {
	endLen = bufRO - bufWO;
	startLen = 0;
      } else {
	endLen = capacity - bufWO;
	startLen = bufRO;
      }
      
      if(endLen > remainLen) {
	endLen = remainLen;
	startLen = 0;
      }
      if(endLen > 0) {
	if(debug) {
	  System.err.println("Writing to ringBuffer end: " + bufWO +  " l: " + endLen + 
			     " offs: " + (offset + writtenLen));
	}
	System.arraycopy(buf, offset + writtenLen, buffer, bufWO, endLen);
	writtenLen += endLen;
	remainLen -= endLen;
	bufWO += endLen;
      }
      
      if(startLen > remainLen) {
	startLen = remainLen;
      }

      if(startLen > 0) {
	if(debug) {
	  System.err.println("Writing to ringBuffer start: " + bufWO +  " l: " + startLen + 
			     " offs: " + (offset + writtenLen));
	}
	System.arraycopy(buf, offset + writtenLen, buffer, 0, startLen);
	writtenLen += startLen;
	remainLen -= startLen;
	bufWO = startLen;
      }
    }

    if(writtenLen > 0) {
      writeOffset += writtenLen;
      // We wrote something, so notify any waiting readers
      // so they can fetch the new data if they want to.
      notifyAll();
    }

    if(debug) {
      System.err.println("Write o: " + offset + " l: " + len + " wl: " + 
			 writtenLen + " reml: " + remainLen + 
			 " ro: " + readOffset + " wo: " + writeOffset + 
			 " bro: " + bufRO + " bwo: " + bufWO);
    }
    return writtenLen;
  }

  public synchronized int readFully(byte[] buf, int offset, int len, long timeoutMS) 
    throws java.io.InterruptedIOException
  {
    int readOffset = offset;
    int remainLen = len;
    java.util.TimerTask tTask = null;

    do {
      int readLen = read(buf, readOffset, remainLen);
      
      if(readLen > 0) {
	// Update the remainLen
	remainLen -= readLen;
	readOffset += readLen;
      }
      
      if(readLen < 0 || availableRead() < 0) {
	// End of file (stream) detected, no use 
	// waiting for more.
	if(len != remainLen)
	  return len - remainLen; // return what we have read so far.
	else
	  return -1; // Mark end of file (stream).
      }

      if(remainLen > 0) {
	try {
	  if(timeoutMS > 0) {
	    if(tTask == null) {
	      if(timer == null) {
		timer = new java.util.Timer();
	      }
	      tTask = new InterrupterTask(Thread.currentThread());
	      timer.schedule(tTask, timeoutMS);
	    }
	  }
	  // wait for Producer to write some data.
	  wait(0);
	} catch (InterruptedException e) {
	  java.io.InterruptedIOException e1 = new java.io.InterruptedIOException("Read timed out");
	  e1.bytesTransferred = len - remainLen;
	  if(tTask != null)
	    tTask.cancel();
	  throw e1;
	}
      }
    } while(remainLen > 0);

    if(tTask != null)
      tTask.cancel();
    
    return len;
  }


  public synchronized int writeFully(byte [] buf, int offset, 
				     int len, long timeoutMS) 
    throws java.io.InterruptedIOException
  {
    int writtenOffset = offset;
    int remainLen = len;
    java.util.TimerTask tTask = null;
    
    do {
      int writtenLen = write(buf, writtenOffset, remainLen);
      if(writtenLen < 0) {
	if(len != remainLen)
	  return len - remainLen;
	else
	  return -1;
      }

      remainLen -= writtenLen;
      writtenOffset += writtenLen;

      if(remainLen > 0) {
	try {
	  if(timeoutMS > 0) {
	    if(tTask == null) {
	      if(timer == null) {
		timer = new java.util.Timer();
	      }
	      tTask = new InterrupterTask(Thread.currentThread());
	      timer.schedule(tTask, timeoutMS);
	    }
	  }
	  // wait for Consumer to read some data.
	  wait(0);
	} catch (InterruptedException e) {
	  // Interrupted by timer Task.
	  java.io.InterruptedIOException e1 = new java.io.InterruptedIOException("Write timed out");
	  e1.bytesTransferred = len - remainLen;
	  if(tTask != null)
	    tTask.cancel();
	  throw e1;
	}
      }
    } while(remainLen > 0);

    if(tTask != null) 
      tTask.cancel();

    return len;
  }
  
  public synchronized int waitAvailableRead(int limit, long timeoutMS) 
    throws java.io.InterruptedIOException
  {
    int available = availableRead();
    if(available < 0)
      return -1;
    else if(dry) {
      return available;
    }
    
    java.util.TimerTask tTask = null;
    
    while(available < limit) {
      try {
	// wait for Producer to write some data.
	if(timeoutMS > 0) {
	  if(tTask == null) {
	    if(timer == null) {
	      timer = new java.util.Timer();
	    }
	    tTask = new InterrupterTask(Thread.currentThread());
	    timer.schedule(tTask, timeoutMS);
	  }
	}
	wait(0);
      } catch (InterruptedException e) {
	// Interrupted by timer Task.
	java.io.InterruptedIOException e1 = new java.io.InterruptedIOException("WaitAvailableRead timed out");
	e1.bytesTransferred = availableRead();
	if(tTask != null)
	  tTask.cancel();
	throw e1;
      }
      available = availableRead();
      if(available < 0) {
	if(tTask != null)
	  tTask.cancel();
	return -1;
      } else if(dry) {
	break;
      }
    }

    if(tTask != null) 
      tTask.cancel();
    
    return available;
  }
  
  public synchronized void setDry() {
    // Need to wake any sleeping writers or readers since
    // the state of the buffer has changed.
    dry = true;
    notifyAll();
  }

  private java.util.Timer timer;

  private boolean dry = false;
  private boolean debug = false;
  // 
  private long readOffset = 0;   // Total read offset
  private long writeOffset = 0;  // Total write offset

  // The seekable buffer.
  private final int seekableSize;
  private byte[]    seekBuffer;

  // Ring buffer (seperate out!?! and extend instead)
  private final int capacity;
  private       int bufWO;        // Ringbuffer write position.
  private       int bufRO;        // Ringbuffer read position.
  private byte[]    buffer;
}
