package org.lcogt.sdb.exception;

/**
 * Main Sdb Exception. All other exception sub-class this.
 * 
 * @version $Id: SdbException.java 7674 2008-11-20 23:41:22Z mnorbury $
 * @author mnorbury
 * 
 */
@SuppressWarnings("serial")
public class SdbException extends Exception
{
	public SdbException(String msg)
	{
		super(msg);
	}
}
