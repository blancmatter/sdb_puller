package org.lcogt.sdb;

import java.io.BufferedOutputStream;
import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;

import org.apache.log4j.Logger;
import org.lcogt.sdb.core.SdbDatum;
import org.lcogt.sdb.core.SdbFile;
import org.lcogt.sdb.database.MySQLDatabase;
import org.lcogt.sdb.exception.SdbException;

/**
 * Application to parse Sdb files - specified by -Ddatafile="<i>filename</i>"
 * and upload into a MySQL database.
 * 
 * This application:-
 * 
 * <UL>
 * <LI>Creates a temporary file.
 * <LI>Opens Sdb File
 * <LI>Iterates through datums in Sdb file - outputing to temporary file in
 * MySQL <code>LOAD INFILE</code> format.
 * <LI>Closes Sdb file.
 * </UL>
 * 
 * @version $Id: SdbParserApp.java 7674 2008-11-20 23:41:22Z mnorbury $
 * @author mnorbury
 * @see MySQLDatabase
 * 
 */
public class SdbParserApp
{

	/** Class scope logger */
	private static Logger logger = Logger.getLogger(SdbParserApp.class);

	public static void main(String[] args)
	{
		String datafile = System.getProperty("datafile",
				"src/test/resources/08092021.sdb");

		logger.info("data path set to " + datafile);

		try
		{
			/* Create a temporary file */
			File tempfile = File.createTempFile("SdbParser", ".dat");
			tempfile.createNewFile();
			tempfile.deleteOnExit();
			FileOutputStream fos = new FileOutputStream(tempfile);
			BufferedOutputStream bos = new BufferedOutputStream(fos);

			logger.info("Created temporary file " + tempfile.getAbsolutePath());

			/* Open Sdb file */
			SdbFile sdbfile = new SdbFile(datafile);

			/* Parse the sdb */
			SdbDatum datum;
			int count = 0;
			while ((datum = sdbfile.readNextDatum()) != null)
			{
				count++;
				bos.write(datum.toString().getBytes());
			}

			/* Close input stream */
			sdbfile.closeFile();

			logger.info("Read " + count + " records");

			/* Insert into database */
			MySQLDatabase dbinstance = MySQLDatabase.getInstance();
			dbinstance.loadSdbFile(tempfile);
		}
		catch (SdbException e)
		{
			e.printStackTrace();
		}
		catch (IOException e1)
		{
			e1.printStackTrace();
		}

	}
}
