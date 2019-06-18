package org.lcogt.sdb.core;

import static org.junit.Assert.assertNotNull;
import static org.junit.Assert.fail;

import org.junit.Before;
import org.junit.Test;
import org.lcogt.sdb.exception.SdbException;

/**
 * Basic test class to try loading and accessing and Sdb file.
 * 
 * @version $Id: SdbFileTest.java 7674 2008-11-20 23:41:22Z mnorbury $
 * @author mnorbury
 *
 */
public class SdbFileTest
{

	/** File to open */
	private String sdbfile;

	@Before
	public void setUp() throws Exception
	{
		sdbfile = "src/test/resources/08092021.sdb";
	}

	@Test
	public void testSdbFile()
	{

		/* Create SdbFile */
		try
		{
			new SdbFile(sdbfile);
		}
		catch (SdbException e)
		{
			fail("SdbException thrown");
		}
	}

	@Test
	public void testReadingNonExistentSdbFile()
	{
		try
		{
			new SdbFile("Rubbish");
			fail("An expected exception wasn't thrown");
		}
		catch (SdbException e)
		{
			/* Do nothing as this exception was expected */
		}
	}
	
	@Test
	public void testReadNextDatum() throws Exception
	{
		SdbFile sdb = new SdbFile(sdbfile);

		SdbDatum nextdatum = null;

		nextdatum = sdb.readNextDatum();

		assertNotNull("nextdatum is null", nextdatum);
	}

}
