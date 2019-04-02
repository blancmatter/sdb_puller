=====
 AGS
=====

This package is the Autoguider Service task (AGS).


History:

   AGS_1_07
   Non-QNX-friendly (e.g. Linux) paths added.

   AGS_1_06
   Allow AGS to enter OKAY state from INIT to cope with new AGG simulation mode.
   (AGS_0_20)

   AGS_1_05
   Prevent Ags temporarily flicking into OKAY state following an 
   ACTIVATE request. See bug 1923.

   AGS_1_04
   Build on QNX6.3SP2.

   AGS_1_03 
   Bring in line with AGS_0_19.

   AGS_1_02
   Handle request to go SAFE without creating an error.

   AGS_1_01
   Brought QNX6 code in line with QNX4 baseline AGS_0_17.

   AGS_1_00
   First QNX v6 baseline.

   AGS_0_13
   Added Cil flush before entering main receive loop to avoid flood
   of messages on start up.

   AGS_0_12
   Updated for new baseline.

   AGS_0_11
   Included two Oid's to record the CCD case temperature
   and CCD cooler power level.

   AGS_0_10
   Corrected command to Agg on request to go safe.

   AGS_0_09 
   Added check for data to be submitted to SDB always or only 
   when changed.

   AGS_0_08
   Corrected SDB units in AgsPrivate.h.

   AGS_0_07
   Minor changes to responses to CENTROID and AUTOGUIDE ON commands.

   AGS_0_06
   Fixed memory leak bug.

   AGS_0_05
   Increase frequency of logging of centroids so that all centroids are 
   reported to the SDB.

   AGS_0_04
   Added reporting of chip temperature and star magnitude to SDB.

   AGS_0_03
   Various bug fixes flushed out in testing.

   AGS_0_02
   Minor changes to build procedure to integrate into the TTL QNX software
   build process. No functional changes.

   AGS_0_01
   First baseline.


Files:

   Ags.h
   Ags.lis
   Ags.mak
   AgsInit.c
   AgsMain.c
   AgsPrivate.h
   AgsProcess.c
   AgsReply.c
   AgsReport.c
   AgsUtil.c
   AgsReadme.txt


Documentation:

   ???


Notes:

   -


30-Oct-2007, MJF.

=====



