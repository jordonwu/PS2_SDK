package ps2vfs.pluggable.streamer;

public class InterrupterTask extends java.util.TimerTask 
{
  private Thread target;

  public InterrupterTask(Thread t) {
    target = t;
  }

  public void run() {
    target.interrupt();
  }
}


