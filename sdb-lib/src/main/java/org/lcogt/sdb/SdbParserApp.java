package org.lcogt.sdb;

import java.io.BufferedOutputStream;
import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;


import org.lcogt.sdb.core.SdbDatum;
import org.lcogt.sdb.core.SdbFile;
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


	public static void main(String[] args)
	{
		String datafile = System.getProperty("datafile",
				"src/test/resources/19121002.sdb");

		try
		{
			/* Create a temporary file */
		   File tempfile = new File("SdbParser.dat");
         if(tempfile.createNewFile()){
            System.out.println("file.txt File Created in Project root directory");
         }else System.out.println("File file.txt already exists in the project root directory");
			System.out.print("Created Temp File? ");
			FileOutputStream fos = new FileOutputStream(tempfile);
			BufferedOutputStream bos = new BufferedOutputStream(fos);


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
