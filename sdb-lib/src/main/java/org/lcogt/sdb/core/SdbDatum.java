package org.lcogt.sdb.core;

import java.util.Formatter;

/**
 * A class to represent an Sdb datum.
 * 
 * @version $Id: SdbDatum.java 7674 2008-11-20 23:41:22Z mnorbury $
 * @author mnorbury
 * 
 */
public class SdbDatum
{

	/** Sdb datum id */
	private final int datumId;

	/** Sdb source id */
	private final int sourceId;

	/** Raw sdb timestamp */
	private final double time;

	/** Datum value */
	private final int value;

	/**
	 * Default <code>SdbDatum</code> constructor.
	 * 
	 * @param sourceId
	 *            SourceId
	 * @param datumId
	 *            DatumId
	 * @param time
	 *            Time (unixtime in seconds)
	 * @param value
	 *            Datum value.
	 */
	public SdbDatum(int sourceId, int datumId, double time, int value)
	{
		this.sourceId = sourceId;
		this.datumId = datumId;
		this.time = time;
		this.value = value;
	}

	/**
	 * Gets the datum id for the current datum.
	 * 
	 * @return Returns the datum id.
	 */
	public int getDatumId()
	{
		return datumId;
	}

	/**
	 * Get the source id for the current datum.
	 * 
	 * @return Returns the source id.
	 */
	public int getSourceId()
	{
		return sourceId;
	}

	/**
	 * Get the datum time (nano-seconds)
	 * 
	 * @return Datum time in nano-seconds.
	 */
	public long getTimeNsec()
	{
		long timeSec = getTimeSec();
		double fracseconds = this.time - (double) timeSec;
		return (long) (fracseconds * 1e9);
	}

	/**
	 * Get the datum time (seconds)
	 * 
	 * @return Datum time in seconds.
	 */
	public long getTimeSec()
	{
		return (long) Math.floor(time);
	}

	/**
	 * Get the datum value.
	 * 
	 * @return Datum value.
	 */
	public int getValue()
	{
		return value;
	}

	/**
	 * Function to print the datum as a formatted string suitable for loading
	 * into a MySQL datafile.
	 */
	@Override
	public String toString()
	{
		Formatter format = new Formatter();
		return format.format("%d\t%d\t%d\t%d\t%d\n", getSourceId(),
				getDatumId(), getValue(), getTimeSec(), getTimeNsec())
				.toString();
	}

}
