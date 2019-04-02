=====
 ETC
=====

Description:
   Miscellaneous configuration files.

History:

   ETC_1_27
   Yunnan :-
   - specifically disable interface to ACADS during OperOff sequence
   - mirror collimation positions revised
   Faulkes South :-
   - CMT configuration file updated for new Linux autoguider
   - WMS configuration file brought up to date
   Faulkes North and South :-
   - removal of autoguider from Bindings and OperOn sequence
   - autoguider focus direction restored following new autoguider installation
   Simulator (powered) :-
   - creation of WmsSimPower.cfg to get network-served weather from a Boltwood 
   All :-
   - don't start AGC and AGG on the autoguider, not relevant to Linux autoguider

   ETC_1_26
   Yunnan :-
   - wait longer for TCC to boot, addition of process 'NDC' into 'Optional' list
   - increase in WMS humidity limit to 85%, don't duplicate DSWE into CFOR
   - enable altitude ENL corrections, and collect them from /dom
   - mirror collimated positions revised
   Faulkes North, South and Simulator :-
   - place CCT and CMT instance 0 in SYS machine
   Faulkes South :-
   - use rack APS outlets 1.4 and 2.4 for SYS machine
   - addition of Boltwood Cloud Sensor feed into WMS
   Simulator :-
   - use rack APS outlets 1.4 and 2.4 for SYS machine (powered simulator)
   All :-
   - addition of CCT and CMT configuration files for SYS machine
   - DAT configuration files brought up to date
   Also consistent with ETC_0_88 on QNX v4.

   ETC_1_25
   Updated in accordance with ETC_0_87 on QNX v4.
   Liverpool :-
   - OperOn sequence :-
     - AMN takes up to 240 seconds to boot
     - autoguider focus mechanism re-installed
     - homing times for AGD, SFD and SFP reduced
     - wait for azimuth homing back to the norm
   - WMS :-
     - dew-point lower limit reduced
     - digital surface wetness no longer critical
     - analogue surface wetness upper limit increased
   - azimuth :-
     - change of H-infinity controller to haz1LTsiso_11discrete.cfg
     - revert to normal homing duration
     - encoder offsets revised
     - park position reverted back to 90 degrees
   - altitude :-
     - homing time doubled
     - horizon L1 limit now 21 degrees
   - Cassegrain :-
     - L1 limits restricted to +/- 90 degrees
   - auxiliary mechanisms :-
     - run SFD in simulate mode
     - AGD upper limit reduced
     - SMF altitude compensation parameter tweaked
   - mirror support :-
     - collimated positions updated
   Faulkes North :-
   - MCP power control of SYS machine on APS1/2.4
   Faulkes South :-
   - SPT controls power to basebox power switch
   - azimuth encoder assignments re-mapped following node replacement
   - altitude encoder positions and H-infinity controller reflect bracket rework

   ETC_1_24
   Faulkes North :-
   - CMT configuration file updated for new Linux autoguider
   Faulkes South :-
   - IPT put onto 'ins' machine in CIL map
   Simulator :-
   - run SPT and EPT rather than SPI and IET

   ETC_1_23
   Liverpool, Faulkes North, Faulkes South, Yunnan, Simulator :-
   - addition of CCT/CMT configuration files for OCC
   Simulator :-
   - addition of dedicated CMT configuration files

   ETC_1_22
   Liverpool, IUCAA, Faulkes North, Faulkes South :-
   - brought in line with ETC_0_86 (TQB_0_61) on QNX v4

   ETC_1_21
   All :-
   - start AGS with a '-q' switch to avoid logging to 'bootmsg'
   - allow longer for SPT to be ready on startup to read in all descriptions
   - Dat.cfg entries brought up-to-date
   Yunnan :-
   - run MBS with '-swap' switch for site-specific installation
   Simulator :-
   - use CIL ID EPS for Extended Power for Simulator
   - addition of configuration with power control
   - start Operational Hours 60 minutes before sunset
   - lower temperature limit 0 degrees

   ETC_1_20
   Yunnan :-
   - redundant dome-shutter-unavailable datum entries removed
   - disable secondary focus compensations during Oper On/Off
   - add more mirror support lower/raise at end of Oper On
   - altitude ENL compensations added but commented out
   Simulator :-
   - hard-coded IP addresses removed
   - reflect full T2-010 functionality
   - operational hours reflect UTC daytime

   ETC_1_19
   From Yunnan site :-
   - addition of H-infinity controllers (1 x altitude, 2 x Nasmyth)
   - azimuth encoders re-aligned
   - change of altitude H-infinity controller (notch) and AVG
   - autoguider astronomical commissioning
   - SMF altitude coefficient doubled, sign of temperature coefficient fixed
   - primary mirror re-collimated

   ETC_1_18
   Yunnan site updates :-
   - schedule daily reboot at 4:30 UTC
   - Nasmyth commissioned in Operational On
   - WMS network file stale time reduced to 120 seconds
   - Nasmyth commissioned, addition of potential Nasmyth H-infinity controller
   - azimuth and altitude controller work and encoder re-alignment
   - ModBUS commissioned, mirror support lateral pre-load specified

   ETC_1_17
   Yunnan M2 linear encoder offsets and homing positions tweaked on site.
   Log to 'localhost' in development environment.

   ETC_1_16
   Yunnan AuxMech, Mirror and OperOn sequence in Pmm config updated as per end 
   of December 2006 site work.

   ETC_1_15
   Reflects Yunnan site integration progress.

   ETC_1_14
   AmnYnMirror.cfg: Included additional OID's to for inclinometer
   noise rejection filter. Filter has been changed to be the same
   implementation as the load cell noise rejection filter.

   ETC_1_13
   Iet:  Make the default altitude demand 90 degrees.
   Updated axis node config files with changes from site.

   ETC_1_12
   Yunnan autoguider focus motion direction reversed.
   Change Log.cfg so that all nodes log to mcc directly rather than 
   via their own local syslog processes.
   Incorporate changesf rom site to PmmYn.cfg, IetYn.cfg, AcnYnAzimuth.cfg,
   AcnYnAltitude.cfg, AcnYnCassegrain.cfg, AcnYnNasMinus.cfg, AmnYnAuxMech.cfg,
   AmnYnMirror.cfg.
   PmmYn.cfg - shutdown software on on mount nodes during oper off.

   ETC_1_11
   Simulation and Yunnan files reflect further QNX v6.3 development and testing.
   Initial Yunnan system integration updates incorporated.

   ETC_1_10
   Complete overhaul based on TQB_0_54.

   ETC_1_09
   Default logging host in Log.cfg made 'localhost'.

   ETC_1_08
   Set secondary mirror upper position limit.

   ETC_1_07
   Addition of azimuth H-infinity model Hnf_hazyu10_2_0.cfg.
   Yunnan axes commissioning files reflect integration progress.

   ETC_1_06
   Override TST scripts removed. Azimuth H-infinity models added. 
   Operational On sequence modified for ACADS linear encoders.
   H-infinity now default for azimuth/altitude, mirror support not simulated.

   ETC_1_05
   New 'selection' added into PMM config file for Yunnan, to enable an 
   Operational On to select which rotator to home. Node bindings added in to
   PMM configuration file for on-mount recovery. Rotator software runs with
   a Dummy Heartbeat Task to simulate the presence of the other rotator.

   ETC_1_04
   PMM configuration file includes full rack control and oil system start-up.
   Nasmyth configuration file reflects integration progress.
   Run Yunnan mirror support software in simulate mode until functional.

   ETC_1_03
   Runs SPI (not SPT) for T2-408 SPLC. TCC/TCS under start-up/shutdown control.
   Addition of H-infinity files Hnf_hazyu7_83, Hnf_haltyu1_113, Hnf_haltyu3_113.
   Azimuth, altitude and cassegrain commissioned under PID control.
   Auxiliary mechanisms commissioned, mirror support ready to test.

   ETC_1_02
   Addition of Spi.cfg for T2-408 telescope Services PLC interface.
   Reflects further porting of on-mount infrastructure to QNX v6.

   ETC_1_01
   Addition of template configuration files for Yunnan telescope.

   ETC_1_00
   Ported onto QNX v6. Identical to ETC_0_59.

   ETC_0_59
   Reduce wait for axes watchdog on Operational On.
   VEN goes Failed if no heads are homed. Default azimuth homing now 30 seconds. 
   AMN agent number increased. AGI execute attributes updated.
   Liverpool :-
   - start Liverpool WMS with audible alarm from MCC
   - azimuth and cassegrain limits set following site work
   - azimuth homing time increased to 3 minutes
   - azimuth encoder non-linearity parameters added (but turned off)
   - Modbus switched on following integration
   - science fold and ACADS positions updated following on-sky work

   ETC_0_58
   Don't power-cycle dial-in and timeserver on rack restart.
   PMM configuration item (OPERADJUST) to advance Operational Hours start.
   Liverpool weather monitoring limits altered.
   Faulkes 1 network weather monitoring integrated.
   Liverpool altitude H-infinity controller now hm40alt470j3.
   Autoguider configuration files updated in line with updated software.
   Faulkes 1 heartbeat timeout increased from 750 msec to 900 msec (for AMN).
 
   ETC_0_57
   Further work on Faulkes 1 site :-
   - azimuth limits now set correctly
   - science fold stow/deploy position and side port 3 for science camera

   ETC_0_56
   Associations added into PMM configuration file :-
   - bad weather with enclosure closed 
   - SPLC shutdown with Operational Off
   TFP locking allowed 100 seconds rather than 250.
   Faulkes 1 science fold stowed position change.

   ETC_0_55
   Addition of UTC time offsets for sites into PMM configuration file. 
   Telescope-specific configuration files may ug+rw on installation.
   Ice sensor added into Faulkes 1 WMS configuration file.

   ETC_0_54
   Preparation of configuration files for telescope simulation network.

   ETC_0_53
   Updates following integration work on Faulkes 1 site. Files changed are 
   mainly Faulkes 1 files but some changes applied to other telescopes as a 
   result of further development - PMM configs and axis configs (TFP locking).

   ETC_0_52
   Autoguider configuration files updated.

   ETC_0_51
   Minor updates following Liverpool telescope on-site sky work.

   ETC_0_50
   Standardisation of homing velocities/durations, and additions of homing 
   attract positions. Altitude horizon L1 set to 91. Liverpool AMN positions 
   tweaked. Liverpool cassegrain slowed down to 2 deg/sec. Altitude H-infinity 
   file Hnf_hm40alt470j3_125.cfg added. Minor autoguider configuration tweaks. 
   Rack key panel no longer powered from APS. DAT configuration files created.

   ETC_0_49
   Updates following First Light on Liverpool and Faulkes 1, and prior to 
   stripdown of Faulkes 2. Faulkes 1 files are still working files, Liverpool
   files intending to be approaching completion. CMT configuration files added.
   H-infinity files now named afted the model number. WMS configuration files 
   populated. TST scripts added to force the MCP Operational On flag.

   ETC_0_48
   Extensive updates following factory integration of Faulkes 2 :-
   - CIL map caters for RCS & OCS on the OCC
   - PMM configuration file caters for revised system state model
   - on-mount configuration files reflect progess on Faulkes 2
   - H-infinity files are preferred ones according to GAM
   Also incorporates updates from initial Liverpool site System Integration.

   ETC_0_47
   Addition of files to set the correct file attributes upon software 
   installation. This was previously done as part of the build process. As part
   of this, unwanted configuration files for other telescopes are removed. 
   Configuration files for FTP file transfer have also been added.

   ETC_0_46
   CIL map updated to make provision for alternative focal stations. Start/Stop
   Observation system requests renamed to be Operational On/Off. WMS 
   configuration files added, and WMS doesn't run for IUCAA. Cassegrain L1 
   limits revised. Various updates for simulating.

   ETC_0_45
   Updated PMM, Azimuth, Alitutde, H-infinity and Mirror Support configuration 
   files from results of Faulkes 1 factory testing. 'Start/Stop Observation'
   sections in the PMM configuration file revised to reflect Services PLC 
   functionality.

   ETC_0_44
   Faulkes 1 & 2 configuration files filled in with intial defaults. Changes 
   to development network CIL map.

   ETC_0_43
   Updated the CIL map to reflect recent changes to the IP addresses of the
   development network.

   ECT_0_42
   Addition of configuration files for telescope simulation. Also templates
   added into autoguider configuration files.

   ETC_0_41
   Autoguider configuration files now include standard deviation parameters.

   ETC_0_40
   Ensure all CCTs are 'Optional' processes in PMM configuration files, so 
   their absence does not prevent a system OKAY state. NB : this is a 
   temporary measure until the CMT is completed.

   ETC_0_39
   Convert Hinfinity configuration files to 11th order model. Addition of 
   'Optional' section in PMM configuration files, to enable the system state
   to go OKAY without some processes running.

   ETC_0_38
   Added the abs/lin encoder difference tolerance and the bitmask for the
   expected encoders to be used on the axis.

   ETC_0_37
   Change to the starting of software wrt the StartUp and StartObservation 
   sections in the PMM configuration file. SPT & EPT now no longer part of 
   the 'core' software group. IUCAA altitude configuration file changed to 
   reflect the Alpha gearboxes. Addition of template configuration files for
   Faulkes I and Faulkes II telescopes. EPT, IET and SPT configuration files 
   repassed.

   ETC_0_36
   Changed minimum torque for motors in cassegrain on IUCAA in order
   to allow removal of the backlash to get out of zones on the bearing
   where there is otherwise insufficient torque to drive through. This
   means that there may be cases where the two motors are driving
   together, rather than against each other.

   ETC_0_35
   Moving of autoguider system process (AGS) onto the autoguider computer. 
   Autoguider computer and its software added into the telescope PMM 
   configuration files. CHB config changed to send heartbeats to AGS. The
   CCT config file for the Autoguider Control Computer has autoguider 
   software added. Configuration files created for each autoguider on each
   telescope.

   ETC_0_34
   CIL map changes for autoguider processes. IUCAA cassegrain motor torque 
   and positional limits updated. AGD range increased from 105 mm to 107 mm.

   ETC_0_33
   The azimuth and altitude axes now use the H-infinity controller as the 
   default for IUCAA. The Start Observation commands in the PMM 
   configuration files have delays inserted to cater for the hardware 
   watchdog. New TST file to instigate a software-only shutdown as a system
   request.

   ETC_0_32
   Fixed incorrect specification of the file for the IUCAA
   Altitude H-infinity control model.

   ETC_0_31
   Added motor feedback parameters for AMC in the IUCAA Azimuth
   and Altitude ACN configuration files. Also added some
   (commented out) lines for ease of selecting H-infinity
   operation of the AMC.

   ETC_0_30
   Removed the obsolete D_MIF_MOTOR_TORQUE_LIMIT OID from all
   axis configuration files.

   ETC_0_29
   IUCAA Telescope only.
   Modified Azimuth and Altitude H Infinity Configuration Files :
   HnfItAzimuth.cfg  - to implement H Infinity Model : 291C53.
   HnfItAltitude.cfg - to implement H Infinity Model : 466J3.

   ETC_0_28
   For IUCAA telescope use the 'daytime' operational hours file.

   ETC_0_27
   Node sub-system configuration files changed to use new three-letter 
   process names. No other changes made.

   ETC_0_26
   Re-ordered the start-up and shutdown list in the axis node configuration 
   files to eliminate the errors logged to syslog when the node software 
   starts up and shuts down.

   ETC_0_25
   Changed the priority of axis-node processes to be consistent across all 
   axes/telescopes. The start/stop observation commands wrt AMN mechanisms 
   re-ordered to complete in shorter time. TST scripts added to shut down 
   node software and re-read the start/stop observation commands from the 
   configuration file.

   ETC_0_24
   The Liverpool PMM configuration file has the interface to TCC CCT, TCS 
   and cassegrain node software uncommented. ECIs added into the core group 
   in both PMM configuration files. Addition of TST scripts to cancel a 
   current start/stop observation.

   ETC_0_23
   Start and stop observation commands now include the auxiliary mechanisms 
   and primary mirror support. Note that cassegrain is still removed from 
   the Liverpool variant. Maximum moving times added into auxiliary 
   mechanism configuration files. The CCT on the AMN node will start both 
   the auxiliary mechanism and primary mirror support sub-systems.

   ETC_0_22
   Added SHUTDOWN command lines for the NSC to all ACN
   configuration files (IUCAA and Liverpool telescopes). 
   These list the OIDs in end-clients that need to be set
   before the NSC terminates. At this stage it is only the
   SERVER_SHUTDOWN OIDs that are set, however the mechanism
   is generic and may also be used for other OIDs.

   ETC_0_21
   Remove a VEN OID that was not implemented from the cassegrain
   configuration files.

   ETC_0_20
   Azimuth H Infinity controller configuration files modified using 
   Model 291F for improved performance.
 
   ETC_0_19
   Addition of configuration file for altitude axis H Infinity controllers 
    - Model 466C  for both IUCAA & Liverpool telescopes.
   IUCAA PMM configuration file integrated with the TCS and TCC. Addition of 
   agent quantities into node configuration files. CCT startup of cassegrain 
   node software enabled.

   ETC_0_18
   Baseline following cassegrain modifications.

   ETC_0_17
   Addition of configuration files for EPT, IET and SPT.

   ETC_0_16
   Moving of CIL process TST from the MCC onto the SCC. Updated PMM 
   configuration files for telescopes. For IUCAA a full start-up and 
   shutdown with power is implemented, on Liverpool these items are 
   commented out. The AMN configuration file for the CCT starts only the
   auxiliary mechanisms and not the mirror support software.

   ETC_0_15
   Addition of TST scripts to perform all MCP system requests. Updates to 
   auxiliary mechanisms configuration file.

   ETC_0_14
   Added configuration files for the CCT processes.
   Added configuration files for the H-infinity controller
   algorithms (Azimuth only at this stage).

   ETC_0_13
   Changes to PMM configuration files for dummy UITs & AITs for IUCAA, and
   addition of UTC time offsets to get local standard time.

   ETC_0_12
   Modified the acceleration limits for IUCAA and Liverpool
   telescopes to 0.5 arcseconds per second per second.

   ETC_0_11
   Revise configuration file for the ACN altitude axis on the
   Liverpool Telescope (making the "AEI" start-up standard
   like the other telescopes).

   ETC_0_10
   Revised configuration files for ACN altitude.


03-Mar-2008, MJF.

=====
