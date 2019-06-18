package org.lcogt.sdb.core;

import java.io.DataInputStream;
import java.io.EOFException;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.IOException;
import java.util.Calendar;

import org.apache.log4j.Logger;
import org.lcogt.sdb.exception.SdbException;
import org.lcogt.sdb.exception.SdbFileNotFoundException;

/**
 * Class to load and parse an Sdb file.
 * 
 * @version $Id: SdbFile.java 7674 2008-11-20 23:41:22Z mnorbury $
 * @author mnorbury
 * 
 */
public class SdbFile
{

	/** Class scope logger */
	private static Logger logger = Logger.getLogger(SdbFile.class);

	/** Sdb file timeoffset unixtime (seconds) */
	private int timeoffset;

	/** Data input stream */
	private DataInputStream dis;

	private static final long INT2UNSIGNEDMASK = 0xFFFFFFFFL;

	private static final double MICROSECONDS2SECONDS = 1e-6;

	private File fp;

	/**
	 * Open an Sdb file.
	 * 
	 * @param fileuri
	 * @throws SdbFileNotFoundException
	 */
	public SdbFile(String sdbfile) throws SdbException
	{
		fp = new File(sdbfile);

		try
		{
			FileInputStream fis = new FileInputStream(fp);
			dis = new DataInputStream(fis);

			/* Read in but ignore header */
			dis.readInt();

			/* Read in Sdb time offset */
			timeoffset = swapEndian(dis.readInt());

			logger.debug("Opened sdb file " + sdbfile);
			logger.debug("Timeoffset = " + timeoffset);
			Calendar calinstance = Calendar.getInstance();
			calinstance.setTimeInMillis(((long) timeoffset) * 1000);
			logger.debug("Got timestamp " + calinstance.getTime().toString());
		}
		catch (FileNotFoundException e)
		{
			throw new SdbFileNotFoundException("File not found " + sdbfile);
		}
		catch (IOException e)
		{
			throw new SdbException("Error reading header information");
		}

	}

	private int swapEndian(int v)
	{
		return (v >>> 24) | (v << 24) | ((v << 8) & 0x00FF0000)
				| ((v >> 8) & 0x0000FF00);
	}

	public SdbDatum readNextDatum() throws IOException
	{

		int SourceId;
		int DatumId;
		int value;
		double time;
		SdbDatum returndatum = null;

		try
		{
			/* Read in datum code */
			int datumcode = swapEndian(dis.readInt());

			SourceId = datumcode >> 24;
			SourceId &= 0xFF;
			DatumId = datumcode & 0xFFFFFF;

			long datumtime = swapEndian(dis.readInt());
			datumtime &= INT2UNSIGNEDMASK;
			value = swapEndian(dis.readInt());

			time = (double) timeoffset + (double) datumtime
					* MICROSECONDS2SECONDS;

			returndatum = new SdbDatum(SourceId, DatumId, time, value);
		}
		catch (EOFException e)
		{
			returndatum = null;
		}

		return returndatum;
	}

	public void closeFile() throws SdbException
	{
		try
		{
			/* Close the input stream */
			dis.close();
		}
		catch (IOException e)
		{
			e.printStackTrace();
			throw new SdbException("Error closing file");
		}
	}
}
