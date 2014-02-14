#----------------------------------------------------------------------------------------------------
# Author: D. Menasce

# The following script sets up the environment to run qt and Monicelli on local machines.
# Once you have fetched this copy from the cvs repository, make a copy (name it something
# like mySetupQt.csh) and modify the copy to suit your needs. Please do NOT put bac in cvs  
# a modified copy of the original setupQt.csh if you have by mistake accidentally edited it.
#
# Before running Monicelli, source YOUR own, PRIVATE **copy** of this file:

alias do 'make -f MakefileAll '
#setenv BASEDATADIR /home/data/TestBeam/2012_04_April
setenv BASEDATADIR /home/data/TestBeam/2011_10_October

#===============   rulinux01.dhcp.fnal.gov   ==============================================
if(${HOSTNAME} == "rulinux01.dhcp.fnal.gov") then

  #===== Local directories
  setenv MonicelliDir		   `pwd`
  setenv Monicelli_RawData_Dir     ${BASEDATADIR}/Merged
  setenv Monicelli_DataSample_Dir  ${BASEDATADIR}/Merged
  setenv Monicelli_CalSample_Dir   ${BASEDATADIR}/Calibrations
  setenv MonicelliOutputDir	   ${MonicelliDir}/../MonicelliOutput
  setenv Monicelli_XML_Dir         ${MonicelliDir}/xml
  
  #===== Location of the ROOT components
  setenv ROOTSYS		   /opt/local/root

  #===== Location of the Qt components
  setenv QTDIR  		   /opt/local/qt
  setenv QTCREATORDIR		   /opt/local/qt-creator

  #===== Location of the BOOST components
  setenv BOOSTINC		   /usr/include
  setenv BOOSTLIB		   /usr/lib64

  #===== Location of the XERCES components
  setenv XERCESCINC		   /opt/local/xerces/include
  setenv XERCESCLIB		   /opt/local/xerces/lib
  
  setenv LD_LIBRARY_PATH           /usr/lib64
  
endif

#===============   rulinux02.dhcp.fnal.gov   ==============================================
if(${HOSTNAME} == "rulinux02.dhcp.fnal.gov") then

  #===== Local directories
  setenv TognazziDir		   `pwd`
  setenv Monicelli_RawData_Dir     ${BASEDATADIR}/Merged
  setenv Monicelli_DataSample_Dir  ${BASEDATADIR}/Merged
  setenv Monicelli_CalSample_Dir   ${BASEDATADIR}/Calibrations
  setenv MonicelliOutputDir	   ${TognazziDir}/../MonicelliOutput
  setenv Monicelli_XML_Dir         ${TognazziDir}/xml

  setenv TOGNAZZICALIBDIR          ${TognazziDir}/CalibFiles
  setenv TOGNAZZIOUTPUTDIR         ${TognazziDir}/../TognazziOutput
  
  #===== Location of the ROOT components
  setenv ROOTSYS		   /opt/local/root

  #===== Location of the Qt components
  setenv QTDIR  		   /opt/local/qt
  setenv QTCREATORDIR		   /opt/local/qtcreator

  #===== Location of the BOOST components
  setenv BOOSTINC		   /usr/include
  setenv BOOSTLIB		   /usr/lib64

  #===== Location of the XERCES components
  setenv XERCESCINC		   /opt/local/xerces/include
  setenv XERCESCLIB		   /opt/local/xerces/lib
  
  setenv LD_LIBRARY_PATH           /usr/lib64
  
endif

if(${HOSTNAME} == "ftbftracker02.fnal.gov") then

  #===== Local directories
  setenv MonicelliDir		   `pwd`
  setenv Monicelli_RawData_Dir     ${BASEDATADIR}/Merged
  setenv Monicelli_DataSample_Dir  ${BASEDATADIR}/Merged
  setenv Monicelli_CalSample_Dir   ${BASEDATADIR}/Calibrations
  setenv MonicelliOutputDir	   ${BASEDATADIR}/MonicelliOutput
  setenv Monicelli_XML_Dir         ${BASEDATADIR}/Geometries
  setenv TognazziDir		   `pwd`
  setenv TOGNAZZICALIBDIR          ${TognazziDir}/CalibFiles
  setenv TOGNAZZIOUTPUTDIR         ${TognazziDir}/../TognazziOutput
  
  #===== Location of the ROOT components
  setenv ROOTSYS		   /opt/local/root

  #===== Location of the Qt components
  setenv QTDIR  		   /opt/local/qt
  setenv QTCREATORDIR		   /opt/local/qtcreator

  #===== Location of the BOOST components
  setenv BOOSTINC		   /usr/include
  setenv BOOSTLIB		   /usr/lib64

  #===== Location of the XERCES components
  setenv XERCESCINC		   /opt/local/xerces/include
  setenv XERCESCLIB		   /opt/local/xerces/lib
  
  setenv LD_LIBRARY_PATH           /usr/lib64
  
endif

#===== Final PATH definitions
setenv PATH			$ROOTSYS/bin:$QTDIR/bin:$QTCREATORDIR/bin:$PATH
setenv LD_LIBRARY_PATH          $ROOTSYS/lib:$QTDIR/lib:$QTCREATORDIR/lib:$XERCESCLIB/:$TognazziDir/plugins/libs/:$TognazziDir/test/customWidgets/libs:$LD_LIBRARY_PATH
