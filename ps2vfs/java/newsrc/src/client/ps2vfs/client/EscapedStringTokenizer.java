package client;

public class EscapedStringTokenizer 
{
  private final boolean debug = false;
  private String inp;
  private boolean escaped;
  private String delim;
  private String[] strArray; 

  public EscapedStringTokenizer(String inp) {
    this.inp = inp;
    escaped = true;
    delim = "\\s+";
    tokenize();
  } 
 
  public EscapedStringTokenizer(String inp, boolean escaped) {
    this.inp = inp;
    this.escaped = escaped;
    delim = "\\s+";
    tokenize();
  }

  public EscapedStringTokenizer(String inp, String delim, boolean escaped) {
    this.inp = inp;
    this.escaped = escaped;
    this.delim = delim;
    tokenize();
  }

  public void tokenize() {
    if(debug)
      System.err.println("Tokenizing: " + inp);

    if(escaped) {
      boolean inPhrase = false;
      boolean escaped = false;
      java.util.regex.Pattern p = java.util.regex.Pattern.compile(delim);
      java.util.regex.Matcher m = p.matcher(inp);

      java.util.ArrayList array = new java.util.ArrayList();
      int len = inp.length();
      int firstMatchStart = -1;
      int firstMatchEnd = -1;
      int startToken = 0;
      int startPhrase = 0;
      for(int n = 0; n < len; n++) {
	char c = inp.charAt(n);

	if(escaped) {
	  escaped = false;
	} else {
	  if( c == '\\') {
	    escaped = !escaped;
	  } else if( c == '"' ) {
	    if(debug) {
	      System.err.println("quote at (" + n + "): " + 
				 startToken + " " + startPhrase + " " + inPhrase);
	    }
	    if(inPhrase) {
	      array.add(removeEscapes(inp.substring(startPhrase, n)));
	      startToken = n + 1;
	      inPhrase = false;
	    } else {
	      if(startToken < n)
		array.add(removeEscapes(inp.substring(startToken, n)));
	      inPhrase = true;
	      startPhrase = n + 1;
	    }
	  } else {
	    if(!inPhrase) {
	      if(n > firstMatchStart && m.find(n)) {
		if(debug) {
		  System.err.println("delim pattern match(" + n + "): " + 
				     m.start() + " " + m.end());
		}
		firstMatchStart = m.start();
		firstMatchEnd = m.end();
	      } 
	      if(firstMatchStart == n) {
		if(startToken < n)
		  array.add(removeEscapes(inp.substring(startToken, n)));
		n = firstMatchEnd - 1;
		startToken = firstMatchEnd;
	      }
	      if(debug) {
		System.err.println("startToken: " + startToken + " " + n);
	      }
	    }
	  }
	}
      }
      if(startToken < len) 
	array.add(removeEscapes(inp.substring(startToken, len)));
      strArray = (String[]) array.toArray(new String[array.size()]);
    } else {
      strArray = inp.split(delim);
    }
  }

  private String removeEscapes(String inp) {
    return inp.replaceAll("\\\\(.)", "$1");
  }

  public String[] toArray() {
    return strArray;
  }
  
}
