package org.lcogt.sdb.exception;

/**
 * Exception raised if specified Sdb file cannot be found.
 * 
 * @version $Id: SdbFileNotFoundException.java 7674 2008-11-20 23:41:22Z mnorbury $
 * @author mnorbury
 *
 */
@SuppressWarnings("serial")
public class SdbFileNotFoundException extends SdbException
{

	public SdbFileNotFoundException(String msg)
	{
		super(msg);
	}

}
