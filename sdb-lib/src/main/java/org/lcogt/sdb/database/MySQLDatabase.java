package org.lcogt.sdb.database;

import java.io.File;
import java.sql.DriverManager;
import java.sql.ResultSet;
import java.sql.SQLException;
import java.sql.Statement;
import java.util.Formatter;

import org.apache.log4j.Logger;
import org.lcogt.sdb.core.SdbDatum;
import org.lcogt.sdb.core.SdbFile;

import com.mysql.jdbc.Connection;

/**
 * Class for uploading {@link SdbDatum}s read from an {@link SdbFile} into a
 * MySQL database.
 * 
 * @version $Id: MySQLDatabase.java 7674 2008-11-20 23:41:22Z mnorbury $
 * @author mnorbury
 * 
 */
public class MySQLDatabase
{

	/** Class scope logger */
	private static Logger logger = Logger.getLogger(MySQLDatabase.class);

	/** MySQL singleton instance */
	public static MySQLDatabase instance = null;

	/**
	 * MySql hostname Hostname.
	 * 
	 * <p>
	 * Default value is <code>localhost</code>, can be configured from the
	 * command line with -Dhost="<i>hostname</i>".
	 * 
	 */
	private String host;

	/**
	 * MySql port.
	 * 
	 * <p>
	 * Default value is <code>3306</code>, can be configured from the
	 * command line with -Dport=<i>port</i>.
	 * 
	 */
	private String port;

	/**
	 * MySql user.
	 * 
	 * <p>
	 * Default value is <code>ttl</code>, can be configured from the
	 * command line with -Dttl="<i>user</i>".
	 * 
	 */
	private String user;

	/**
	 * MySql password.
	 * 
	 * <p>
	 * Default value is <code>ttl</code>, can be configured from the
	 * command line with -Dpassword="<i>password</i>".
	 * 
	 */
	private String password;

	/**
	 * MySql database.
	 * 
	 * <p>
	 * Default value is <code>telemetry</code>, can be configured from the
	 * command line with -Ddatabase="<i>database</i>".
	 * 
	 */
	private String database;

	/** Singleton contructor */
	private MySQLDatabase()
	{
		/* Connect to database */
		this.host = System.getProperty("host", "localhost");
		this.port = System.getProperty("port", "3306");
		this.database = System.getProperty("database", "telemetry");
		this.user = System.getProperty("user", "ttl");
		this.password = System.getProperty("password", "ttl");

		dbformatter = new Formatter();
	}

	private Formatter dbformatter;

	public static MySQLDatabase getInstance()
	{
		if (instance == null)
			instance = new MySQLDatabase();

		return instance;
	}

	public void loadSdbFile(File tempfile)
	{

		Statement stmt = null;
		ResultSet rs = null;

		Formatter querystring = new Formatter();
		querystring.format("LOAD DATA INFILE \"%s\" INTO TABLE %s", tempfile
				.getAbsolutePath(), "Data");

		logger.debug("Trying query " + querystring.toString());

		try
		{
			Connection connection = getConnection();

			stmt = connection.createStatement();
			rs = stmt.executeQuery(querystring.toString());
		}
		catch (SQLException e)
		{
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
		finally
		{
			if (rs != null)
			{
				try
				{
					rs.close();
				}
				catch (SQLException e)
				{
					/* Ignore */
				}
			}

			if (stmt != null)
			{
				try
				{
					stmt.close();
				}
				catch (SQLException e)
				{
					/* Ignore */
				}
			}
		}

	}

	private Connection getConnection() throws SQLException
	{

		/* Connect to database */
		Connection connection = null;

		dbformatter.format("jdbc:mysql://%s:%s/%s?user=%s&password=%s", host,
				port, database, user, password);

		logger.debug(dbformatter.toString());

		connection = (Connection) DriverManager.getConnection(dbformatter
				.toString());

		return connection;
	}

	public void setHost(String host)
	{
		this.host = host;
	}

	public void setPort(String port)
	{
		this.port = port;
	}

	public void setUser(String user)
	{
		this.user = user;
	}

	public void setPassword(String password)
	{
		this.password = password;
	}

	public void setDatabase(String database)
	{
		this.database = database;
	}

}
