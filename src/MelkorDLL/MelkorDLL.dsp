# Microsoft Developer Studio Project File - Name="MelkorDLL" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=MelkorDLL - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "MelkorDLL.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "MelkorDLL.mak" CFG="MelkorDLL - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "MelkorDLL - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "MelkorDLL - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "MelkorDLL - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "MelkorDLL_EXPORTS" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /I "../Melkor" /I "../Melkor/NeuralNetwork" /I "../Melkor/Util" /I "../Melkor/SGF" /I "../Melkor/Goal" /I "../Melkor/Patterns/" /I "../Melkor/Test/" /I "../Melkor/Test/Util" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /FD /c
# SUBTRACT CPP /YX /Yc /Yu
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x809 /d "NDEBUG"
# ADD RSC /l 0x809 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib cppunit.lib /nologo /dll /incremental:yes /machine:I386
# SUBTRACT LINK32 /debug

!ELSEIF  "$(CFG)" == "MelkorDLL - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "../Melkor" /I "../Melkor/NeuralNetwork" /I "../Melkor/Util" /I "../Melkor/SGF" /I "../Melkor/Goal" /I "../Melkor/Patterns/" /I "../Melkor/Test/" /I "../Melkor/Test/Util" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /FD /GZ /c
# SUBTRACT CPP /YX /Yc /Yu
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x809 /d "_DEBUG"
# ADD RSC /l 0x809 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib cppunitd.lib /nologo /dll /debug /machine:I386 /pdbtype:sept

!ENDIF 

# Begin Target

# Name "MelkorDLL - Win32 Release"
# Name "MelkorDLL - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Group "Melkor"

# PROP Default_Filter ""
# Begin Group "Goal"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\Melkor\Goal\CaptureGoal.cpp

!IF  "$(CFG)" == "MelkorDLL - Win32 Release"

# PROP Intermediate_Dir "../Melkor/Goal/Release"

!ELSEIF  "$(CFG)" == "MelkorDLL - Win32 Debug"

# PROP Intermediate_Dir "../Melkor/Goal/Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\Melkor\Goal\InfluenceGoal.cpp

!IF  "$(CFG)" == "MelkorDLL - Win32 Release"

# PROP Intermediate_Dir "../Melkor/Goal/Release"

!ELSEIF  "$(CFG)" == "MelkorDLL - Win32 Debug"

# PROP Intermediate_Dir "../Melkor/Goal/Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\Melkor\Goal\LadderGoal.cpp

!IF  "$(CFG)" == "MelkorDLL - Win32 Release"

# PROP Intermediate_Dir "../Melkor/Goal/Release"

!ELSEIF  "$(CFG)" == "MelkorDLL - Win32 Debug"

# PROP Intermediate_Dir "../Melkor/Goal/Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\Melkor\Goal\LibCountCaptureGoal.cpp

!IF  "$(CFG)" == "MelkorDLL - Win32 Release"

# PROP Intermediate_Dir "../Melkor/Goal/Release"

!ELSEIF  "$(CFG)" == "MelkorDLL - Win32 Debug"

# PROP Intermediate_Dir "../Melkor/Goal/Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\Melkor\Goal\LibertyCountGoal.cpp

!IF  "$(CFG)" == "MelkorDLL - Win32 Release"

# PROP Intermediate_Dir "../Melkor/Goal/Release"

!ELSEIF  "$(CFG)" == "MelkorDLL - Win32 Debug"

# PROP Intermediate_Dir "../Melkor/Goal/Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\Melkor\Goal\LifeDeathGoal.cpp

!IF  "$(CFG)" == "MelkorDLL - Win32 Release"

# PROP Intermediate_Dir "../Melkor/Goal/Release"

!ELSEIF  "$(CFG)" == "MelkorDLL - Win32 Debug"

# PROP Intermediate_Dir "../Melkor/Goal/Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\Melkor\Goal\LooseTacticalGoal.cpp

!IF  "$(CFG)" == "MelkorDLL - Win32 Release"

# PROP Intermediate_Dir "../Melkor/Goal/Release"

!ELSEIF  "$(CFG)" == "MelkorDLL - Win32 Debug"

# PROP Intermediate_Dir "../Melkor/Goal/Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\Melkor\Goal\StoneCountGoal.cpp

!IF  "$(CFG)" == "MelkorDLL - Win32 Release"

# PROP Intermediate_Dir "../Melkor/Goal/Release"

!ELSEIF  "$(CFG)" == "MelkorDLL - Win32 Debug"

# PROP Intermediate_Dir "../Melkor/Goal/Debug"

!ENDIF 

# End Source File
# End Group
# Begin Group "NeuralNetwork"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\Melkor\NeuralNetwork\BPN.cpp

!IF  "$(CFG)" == "MelkorDLL - Win32 Release"

# PROP Intermediate_Dir "../Melkor/NeuralNetwork/Release"

!ELSEIF  "$(CFG)" == "MelkorDLL - Win32 Debug"

# PROP Intermediate_Dir "../Melkor/NeuralNetwork/Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\Melkor\NeuralNetwork\BPNExpAct.cpp

!IF  "$(CFG)" == "MelkorDLL - Win32 Release"

# PROP Intermediate_Dir "../Melkor/NeuralNetwork/Release"

!ELSEIF  "$(CFG)" == "MelkorDLL - Win32 Debug"

# PROP Intermediate_Dir "../Melkor/NeuralNetwork/Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\Melkor\NeuralNetwork\BPNGoAdapter.cpp

!IF  "$(CFG)" == "MelkorDLL - Win32 Release"

# PROP Intermediate_Dir "../Melkor/NeuralNetwork/Release"

!ELSEIF  "$(CFG)" == "MelkorDLL - Win32 Debug"

# PROP Intermediate_Dir "../Melkor/NeuralNetwork/Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\Melkor\NeuralNetwork\BPNGoAdapterInterface.cpp

!IF  "$(CFG)" == "MelkorDLL - Win32 Release"

# PROP Intermediate_Dir "../Melkor/NeuralNetwork/Release"

!ELSEIF  "$(CFG)" == "MelkorDLL - Win32 Debug"

# PROP Intermediate_Dir "../Melkor/NeuralNetwork/Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\Melkor\NeuralNetwork\BPNTrainer.cpp

!IF  "$(CFG)" == "MelkorDLL - Win32 Release"

# PROP Intermediate_Dir "../Melkor/NeuralNetwork/Release"

!ELSEIF  "$(CFG)" == "MelkorDLL - Win32 Debug"

# PROP Intermediate_Dir "../Melkor/NeuralNetwork/Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\Melkor\NeuralNetwork\CN.cpp

!IF  "$(CFG)" == "MelkorDLL - Win32 Release"

# PROP Intermediate_Dir "../Melkor/NeuralNetwork/Release"

!ELSEIF  "$(CFG)" == "MelkorDLL - Win32 Debug"

# PROP Intermediate_Dir "../Melkor/NeuralNetwork/Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\Melkor\NeuralNetwork\NeuralNetwork.cpp

!IF  "$(CFG)" == "MelkorDLL - Win32 Release"

# PROP Intermediate_Dir "../Melkor/NeuralNetwork/Release"

!ELSEIF  "$(CFG)" == "MelkorDLL - Win32 Debug"

# PROP Intermediate_Dir "../Melkor/NeuralNetwork/Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\Melkor\NeuralNetwork\newBPN10GoAdapter.cpp

!IF  "$(CFG)" == "MelkorDLL - Win32 Release"

# PROP Intermediate_Dir "../Melkor/NeuralNetwork/Release"

!ELSEIF  "$(CFG)" == "MelkorDLL - Win32 Debug"

# PROP Intermediate_Dir "../Melkor/NeuralNetwork/Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\Melkor\NeuralNetwork\newBPN11GoAdapter.cpp

!IF  "$(CFG)" == "MelkorDLL - Win32 Release"

# PROP Intermediate_Dir "../Melkor/NeuralNetwork/Release"

!ELSEIF  "$(CFG)" == "MelkorDLL - Win32 Debug"

# PROP Intermediate_Dir "../Melkor/NeuralNetwork/Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\Melkor\NeuralNetwork\newBPN12GoAdapter.cpp

!IF  "$(CFG)" == "MelkorDLL - Win32 Release"

# PROP Intermediate_Dir "../Melkor/NeuralNetwork/Release"

!ELSEIF  "$(CFG)" == "MelkorDLL - Win32 Debug"

# PROP Intermediate_Dir "../Melkor/NeuralNetwork/Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\Melkor\NeuralNetwork\newBPN2GoAdapter.cpp

!IF  "$(CFG)" == "MelkorDLL - Win32 Release"

# PROP Intermediate_Dir "../Melkor/NeuralNetwork/Release"

!ELSEIF  "$(CFG)" == "MelkorDLL - Win32 Debug"

# PROP Intermediate_Dir "../Melkor/NeuralNetwork/Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\Melkor\NeuralNetwork\newBPN3GoAdapter.cpp

!IF  "$(CFG)" == "MelkorDLL - Win32 Release"

# PROP Intermediate_Dir "../Melkor/NeuralNetwork/Release"

!ELSEIF  "$(CFG)" == "MelkorDLL - Win32 Debug"

# PROP Intermediate_Dir "../Melkor/NeuralNetwork/Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\Melkor\NeuralNetwork\newBPN4GoAdapter.cpp

!IF  "$(CFG)" == "MelkorDLL - Win32 Release"

# PROP Intermediate_Dir "../Melkor/NeuralNetwork/Release"

!ELSEIF  "$(CFG)" == "MelkorDLL - Win32 Debug"

# PROP Intermediate_Dir "../Melkor/NeuralNetwork/Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\Melkor\NeuralNetwork\newBPN5GoAdapter.cpp

!IF  "$(CFG)" == "MelkorDLL - Win32 Release"

# PROP Intermediate_Dir "../Melkor/NeuralNetwork/Release"

!ELSEIF  "$(CFG)" == "MelkorDLL - Win32 Debug"

# PROP Intermediate_Dir "../Melkor/NeuralNetwork/Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\Melkor\NeuralNetwork\newBPN6GoAdapter.cpp

!IF  "$(CFG)" == "MelkorDLL - Win32 Release"

# PROP Intermediate_Dir "../Melkor/NeuralNetwork/Release"

!ELSEIF  "$(CFG)" == "MelkorDLL - Win32 Debug"

# PROP Intermediate_Dir "../Melkor/NeuralNetwork/Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\Melkor\NeuralNetwork\newBPN7GoAdapter.cpp

!IF  "$(CFG)" == "MelkorDLL - Win32 Release"

# PROP Intermediate_Dir "../Melkor/NeuralNetwork/Release"

!ELSEIF  "$(CFG)" == "MelkorDLL - Win32 Debug"

# PROP Intermediate_Dir "../Melkor/NeuralNetwork/Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\Melkor\NeuralNetwork\newBPN8GoAdapter.cpp

!IF  "$(CFG)" == "MelkorDLL - Win32 Release"

# PROP Intermediate_Dir "../Melkor/NeuralNetwork/Release"

!ELSEIF  "$(CFG)" == "MelkorDLL - Win32 Debug"

# PROP Intermediate_Dir "../Melkor/NeuralNetwork/Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\Melkor\NeuralNetwork\newBPN9GoAdapter.cpp

!IF  "$(CFG)" == "MelkorDLL - Win32 Release"

# PROP Intermediate_Dir "../Melkor/NeuralNetwork/Release"

!ELSEIF  "$(CFG)" == "MelkorDLL - Win32 Debug"

# PROP Intermediate_Dir "../Melkor/NeuralNetwork/Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\Melkor\NeuralNetwork\newBPNGoAdapter.cpp

!IF  "$(CFG)" == "MelkorDLL - Win32 Release"

# PROP Intermediate_Dir "../Melkor/NeuralNetwork/Release"

!ELSEIF  "$(CFG)" == "MelkorDLL - Win32 Debug"

# PROP Intermediate_Dir "../Melkor/NeuralNetwork/Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\Melkor\NeuralNetwork\NNDatabase.cpp

!IF  "$(CFG)" == "MelkorDLL - Win32 Release"

# PROP Intermediate_Dir "../Melkor/NeuralNetwork/Release"

!ELSEIF  "$(CFG)" == "MelkorDLL - Win32 Debug"

# PROP Intermediate_Dir "../Melkor/NeuralNetwork/Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\Melkor\NeuralNetwork\RBF.cpp

!IF  "$(CFG)" == "MelkorDLL - Win32 Release"

# PROP Intermediate_Dir "../Melkor/NeuralNetwork/Release"

!ELSEIF  "$(CFG)" == "MelkorDLL - Win32 Debug"

# PROP Intermediate_Dir "../Melkor/NeuralNetwork/Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\Melkor\NeuralNetwork\RPROPBPNTrainer.cpp

!IF  "$(CFG)" == "MelkorDLL - Win32 Release"

# PROP Intermediate_Dir "../Melkor/NeuralNetwork/Release"

!ELSEIF  "$(CFG)" == "MelkorDLL - Win32 Debug"

# PROP Intermediate_Dir "../Melkor/NeuralNetwork/Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\Melkor\NeuralNetwork\Safety1BPNGoAdapter.cpp

!IF  "$(CFG)" == "MelkorDLL - Win32 Release"

# PROP Intermediate_Dir "../Melkor/NeuralNetwork/Release"

!ELSEIF  "$(CFG)" == "MelkorDLL - Win32 Debug"

# PROP Intermediate_Dir "../Melkor/NeuralNetwork/Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\Melkor\NeuralNetwork\Urgency1GoAdapter.cpp

!IF  "$(CFG)" == "MelkorDLL - Win32 Release"

# PROP Intermediate_Dir "../Melkor/NeuralNetwork/Release"

!ELSEIF  "$(CFG)" == "MelkorDLL - Win32 Debug"

# PROP Intermediate_Dir "../Melkor/NeuralNetwork/Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\Melkor\NeuralNetwork\Urgency3BPN.cpp

!IF  "$(CFG)" == "MelkorDLL - Win32 Release"

# PROP Intermediate_Dir "../Melkor/NeuralNetwork/Release"

!ELSEIF  "$(CFG)" == "MelkorDLL - Win32 Debug"

# PROP Intermediate_Dir "../Melkor/NeuralNetwork/Debug"

!ENDIF 

# End Source File
# End Group
# Begin Group "SGF"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\Melkor\SGF\SGFGameInfo.cpp

!IF  "$(CFG)" == "MelkorDLL - Win32 Release"

# PROP Intermediate_Dir "../Melkor/SGF/Release"

!ELSEIF  "$(CFG)" == "MelkorDLL - Win32 Debug"

# PROP Intermediate_Dir "../Melkor/SGF/Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\Melkor\SGF\SGFNode.cpp

!IF  "$(CFG)" == "MelkorDLL - Win32 Release"

# PROP Intermediate_Dir "../Melkor/SGF/Release"

!ELSEIF  "$(CFG)" == "MelkorDLL - Win32 Debug"

# PROP Intermediate_Dir "../Melkor/SGF/Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\Melkor\SGF\SGFProperty.cpp

!IF  "$(CFG)" == "MelkorDLL - Win32 Release"

# PROP Intermediate_Dir "../Melkor/SGF/Release"

!ELSEIF  "$(CFG)" == "MelkorDLL - Win32 Debug"

# PROP Intermediate_Dir "../Melkor/SGF/Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\Melkor\SGF\SGFReader.cpp

!IF  "$(CFG)" == "MelkorDLL - Win32 Release"

# PROP Intermediate_Dir "../Melkor/SGF/Release"

!ELSEIF  "$(CFG)" == "MelkorDLL - Win32 Debug"

# PROP Intermediate_Dir "../Melkor/SGF/Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\Melkor\SGF\SGFTree.cpp

!IF  "$(CFG)" == "MelkorDLL - Win32 Release"

# PROP Intermediate_Dir "../Melkor/SGF/Release"

!ELSEIF  "$(CFG)" == "MelkorDLL - Win32 Debug"

# PROP Intermediate_Dir "../Melkor/SGF/Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\Melkor\SGF\SGFWriter.cpp

!IF  "$(CFG)" == "MelkorDLL - Win32 Release"

# PROP Intermediate_Dir "../Melkor/SGF/Release"

!ELSEIF  "$(CFG)" == "MelkorDLL - Win32 Debug"

# PROP Intermediate_Dir "../Melkor/SGF/Debug"

!ENDIF 

# End Source File
# End Group
# Begin Group "Util"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\Melkor\Util\BoardStruct.cpp

!IF  "$(CFG)" == "MelkorDLL - Win32 Release"

# PROP Intermediate_Dir "../Melkor/Util/Release"

!ELSEIF  "$(CFG)" == "MelkorDLL - Win32 Debug"

# PROP Intermediate_Dir "../Melkor/Util/Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\Melkor\Util\Go.cpp

!IF  "$(CFG)" == "MelkorDLL - Win32 Release"

# PROP Intermediate_Dir "../Melkor/Util/Release"

!ELSEIF  "$(CFG)" == "MelkorDLL - Win32 Debug"

# PROP Intermediate_Dir "../Melkor/Util/Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\Melkor\Util\GoString.cpp

!IF  "$(CFG)" == "MelkorDLL - Win32 Release"

# PROP Intermediate_Dir "../Melkor/Util/Release"

!ELSEIF  "$(CFG)" == "MelkorDLL - Win32 Debug"

# PROP Intermediate_Dir "../Melkor/Util/Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\Melkor\Util\Misc.cpp

!IF  "$(CFG)" == "MelkorDLL - Win32 Release"

# PROP Intermediate_Dir "../Melkor/Util/Release"

!ELSEIF  "$(CFG)" == "MelkorDLL - Win32 Debug"

# PROP Intermediate_Dir "../Melkor/Util/Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\Melkor\Util\Move.cpp

!IF  "$(CFG)" == "MelkorDLL - Win32 Release"

# PROP Intermediate_Dir "../Melkor/Util/Release"

!ELSEIF  "$(CFG)" == "MelkorDLL - Win32 Debug"

# PROP Intermediate_Dir "../Melkor/Util/Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\Melkor\Util\Rect.cpp

!IF  "$(CFG)" == "MelkorDLL - Win32 Release"

# PROP Intermediate_Dir "../Melkor/Util/Release"

!ELSEIF  "$(CFG)" == "MelkorDLL - Win32 Debug"

# PROP Intermediate_Dir "../Melkor/Util/Debug"

!ENDIF 

# End Source File
# End Group
# Begin Group "Test"

# PROP Default_Filter ""
# Begin Group "Util No. 2"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\Melkor\Test\Util\BoardStructTest.cpp
# End Source File
# Begin Source File

SOURCE=..\Melkor\Test\Util\GoStringTest.cpp
# End Source File
# Begin Source File

SOURCE=..\Melkor\Test\Util\GoTest.cpp

!IF  "$(CFG)" == "MelkorDLL - Win32 Release"

# PROP Intermediate_Dir "../Melkor/Test/Release"

!ELSEIF  "$(CFG)" == "MelkorDLL - Win32 Debug"

# PROP Intermediate_Dir "../Melkor/Test/Debug"

!ENDIF 

# End Source File
# End Group
# End Group
# Begin Source File

SOURCE=..\Melkor\AllLegalSuggester.cpp

!IF  "$(CFG)" == "MelkorDLL - Win32 Release"

# PROP Intermediate_Dir "../Melkor/Release"

!ELSEIF  "$(CFG)" == "MelkorDLL - Win32 Debug"

# PROP Intermediate_Dir "../Melkor/Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\Melkor\CaptureSuggester.cpp

!IF  "$(CFG)" == "MelkorDLL - Win32 Release"

# PROP Intermediate_Dir "../Melkor/Release"

!ELSEIF  "$(CFG)" == "MelkorDLL - Win32 Debug"

# PROP Intermediate_Dir "../Melkor/Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\Melkor\GeneralSuggester.cpp

!IF  "$(CFG)" == "MelkorDLL - Win32 Release"

# PROP Intermediate_Dir "../Melkor/Release"

!ELSEIF  "$(CFG)" == "MelkorDLL - Win32 Debug"

# PROP Intermediate_Dir "../Melkor/Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\Melkor\GTPFunctions.cpp

!IF  "$(CFG)" == "MelkorDLL - Win32 Release"

# PROP Intermediate_Dir "../Melkor/Release"

!ELSEIF  "$(CFG)" == "MelkorDLL - Win32 Debug"

# PROP Intermediate_Dir "../Melkor/Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\Melkor\LadderSuggester.cpp

!IF  "$(CFG)" == "MelkorDLL - Win32 Release"

# PROP Intermediate_Dir "../Melkor/Release"

!ELSEIF  "$(CFG)" == "MelkorDLL - Win32 Debug"

# PROP Intermediate_Dir "../Melkor/Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\Melkor\LogWriter.cpp

!IF  "$(CFG)" == "MelkorDLL - Win32 Release"

# PROP Intermediate_Dir "../Melkor/Release"

!ELSEIF  "$(CFG)" == "MelkorDLL - Win32 Debug"

# PROP Intermediate_Dir "../Melkor/Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\Melkor\LooseTacticalSuggester.cpp

!IF  "$(CFG)" == "MelkorDLL - Win32 Release"

# PROP Intermediate_Dir "../Melkor/Release"

!ELSEIF  "$(CFG)" == "MelkorDLL - Win32 Debug"

# PROP Intermediate_Dir "../Melkor/Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\Melkor\Main.cpp

!IF  "$(CFG)" == "MelkorDLL - Win32 Release"

# PROP Intermediate_Dir "../Melkor/Release"

!ELSEIF  "$(CFG)" == "MelkorDLL - Win32 Debug"

# PROP Intermediate_Dir "../Melkor/Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\Melkor\Melkor.cpp

!IF  "$(CFG)" == "MelkorDLL - Win32 Release"

# PROP Intermediate_Dir "../Melkor/Release"

!ELSEIF  "$(CFG)" == "MelkorDLL - Win32 Debug"

# PROP Intermediate_Dir "../Melkor/Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\Melkor\MTDFSearch.cpp

!IF  "$(CFG)" == "MelkorDLL - Win32 Release"

# PROP Intermediate_Dir "../Melkor/Release"

!ELSEIF  "$(CFG)" == "MelkorDLL - Win32 Debug"

# PROP Intermediate_Dir "../Melkor/Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\Melkor\Node.cpp

!IF  "$(CFG)" == "MelkorDLL - Win32 Release"

# PROP Intermediate_Dir "../Melkor/Release"

!ELSEIF  "$(CFG)" == "MelkorDLL - Win32 Debug"

# PROP Intermediate_Dir "../Melkor/Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\Melkor\SearchSettings.cpp

!IF  "$(CFG)" == "MelkorDLL - Win32 Release"

# PROP Intermediate_Dir "../Melkor/Release"

!ELSEIF  "$(CFG)" == "MelkorDLL - Win32 Debug"

# PROP Intermediate_Dir "../Melkor/Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\Melkor\Test.cpp

!IF  "$(CFG)" == "MelkorDLL - Win32 Release"

# PROP Intermediate_Dir "../Melkor/Release"

!ELSEIF  "$(CFG)" == "MelkorDLL - Win32 Debug"

# PROP Intermediate_Dir "../Melkor/Debug"

!ENDIF 

# End Source File
# End Group
# Begin Source File

SOURCE=.\MelkorDLL.cpp
# End Source File
# Begin Source File

SOURCE=.\MelkorDLL.def
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Group "Melkor No. 1"

# PROP Default_Filter ""
# Begin Group "Goal No. 1"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\Melkor\Goal\CaptureGoal.h
# End Source File
# Begin Source File

SOURCE=..\Melkor\Goal\Goal.h
# End Source File
# Begin Source File

SOURCE=..\Melkor\Goal\GoalFunctions.h
# End Source File
# Begin Source File

SOURCE=..\Melkor\Goal\GoalInterface.h
# End Source File
# Begin Source File

SOURCE=..\Melkor\Goal\InfluenceGoal.h
# End Source File
# Begin Source File

SOURCE=..\Melkor\Goal\LadderGoal.h
# End Source File
# Begin Source File

SOURCE=..\Melkor\Goal\LibCountCaptureGoal.h
# End Source File
# Begin Source File

SOURCE=..\Melkor\Goal\LibertyCountGoal.h
# End Source File
# Begin Source File

SOURCE=..\Melkor\Goal\LifeDeathGoal.h
# End Source File
# Begin Source File

SOURCE=..\Melkor\Goal\LooseTacticalGoal.h
# End Source File
# Begin Source File

SOURCE=..\Melkor\Goal\StoneCountGoal.h
# End Source File
# End Group
# Begin Group "NeuralNetwork No. 1"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\Melkor\NeuralNetwork\BPN.h
# End Source File
# Begin Source File

SOURCE=..\Melkor\NeuralNetwork\BPNExpAct.h
# End Source File
# Begin Source File

SOURCE=..\Melkor\NeuralNetwork\BPNExpActGoAdapter.h
# End Source File
# Begin Source File

SOURCE=..\Melkor\NeuralNetwork\BPNGoAdapter.h
# End Source File
# Begin Source File

SOURCE=..\Melkor\NeuralNetwork\BPNGoAdapterInterface.h
# End Source File
# Begin Source File

SOURCE=..\Melkor\NeuralNetwork\BPNTrainer.h
# End Source File
# Begin Source File

SOURCE=..\Melkor\NeuralNetwork\CN.h
# End Source File
# Begin Source File

SOURCE=..\Melkor\NeuralNetwork\CNGoAdapter.h
# End Source File
# Begin Source File

SOURCE=..\Melkor\NeuralNetwork\NeuralNetwork.h
# End Source File
# Begin Source File

SOURCE=..\Melkor\NeuralNetwork\newBPN10GoAdapter.h
# End Source File
# Begin Source File

SOURCE=..\Melkor\NeuralNetwork\newBPN11GoAdapter.h
# End Source File
# Begin Source File

SOURCE=..\Melkor\NeuralNetwork\newBPN12GoAdapter.h
# End Source File
# Begin Source File

SOURCE=..\Melkor\NeuralNetwork\newBPN2GoAdapter.h
# End Source File
# Begin Source File

SOURCE=..\Melkor\NeuralNetwork\newBPN3GoAdapter.h
# End Source File
# Begin Source File

SOURCE=..\Melkor\NeuralNetwork\newBPN4GoAdapter.h
# End Source File
# Begin Source File

SOURCE=..\Melkor\NeuralNetwork\newBPN5GoAdapter.h
# End Source File
# Begin Source File

SOURCE=..\Melkor\NeuralNetwork\newBPN6GoAdapter.h
# End Source File
# Begin Source File

SOURCE=..\Melkor\NeuralNetwork\newBPN7GoAdapter.h
# End Source File
# Begin Source File

SOURCE=..\Melkor\NeuralNetwork\newBPN8GoAdapter.h
# End Source File
# Begin Source File

SOURCE=..\Melkor\NeuralNetwork\newBPN9GoAdapter.h
# End Source File
# Begin Source File

SOURCE=..\Melkor\NeuralNetwork\newBPNGoAdapter.h
# End Source File
# Begin Source File

SOURCE=..\Melkor\NeuralNetwork\NNDatabase.h
# End Source File
# Begin Source File

SOURCE=..\Melkor\NeuralNetwork\RBF.h
# End Source File
# Begin Source File

SOURCE=..\Melkor\NeuralNetwork\RBFGoAdapter.h
# End Source File
# Begin Source File

SOURCE=..\Melkor\NeuralNetwork\RPROPBPNTrainer.h
# End Source File
# Begin Source File

SOURCE=..\Melkor\NeuralNetwork\Safety1BPNGoAdapter.h
# End Source File
# Begin Source File

SOURCE=..\Melkor\NeuralNetwork\Urgency1GoAdapter.h
# End Source File
# Begin Source File

SOURCE=..\Melkor\NeuralNetwork\Urgency3BPN.h
# End Source File
# Begin Source File

SOURCE=..\Melkor\NeuralNetwork\Urgency3BPNGoAdapter.h
# End Source File
# End Group
# Begin Group "SGF No. 1"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\Melkor\SGF\SGFGameInfo.h
# End Source File
# Begin Source File

SOURCE=..\Melkor\SGF\SGFNode.h
# End Source File
# Begin Source File

SOURCE=..\Melkor\SGF\SGFProperty.h
# End Source File
# Begin Source File

SOURCE=..\Melkor\SGF\SGFReader.h
# End Source File
# Begin Source File

SOURCE=..\Melkor\SGF\SGFTree.h
# End Source File
# Begin Source File

SOURCE=..\Melkor\SGF\SGFWriter.h
# End Source File
# End Group
# Begin Group "Util No. 1"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\Melkor\Util\BMP.h
# End Source File
# Begin Source File

SOURCE=..\Melkor\Util\BoardStruct.h
# End Source File
# Begin Source File

SOURCE=..\Melkor\Util\DAG.h
# End Source File
# Begin Source File

SOURCE=..\Melkor\Util\DAGNode.h
# End Source File
# Begin Source File

SOURCE=..\Melkor\Util\globals.h
# End Source File
# Begin Source File

SOURCE=..\Melkor\Util\Go.h
# End Source File
# Begin Source File

SOURCE=..\Melkor\Util\GoString.h
# End Source File
# Begin Source File

SOURCE=..\Melkor\Util\hash_map.h
# End Source File
# Begin Source File

SOURCE=..\Melkor\Util\hash_set.h
# End Source File
# Begin Source File

SOURCE=..\Melkor\Util\hash_shared.h
# End Source File
# Begin Source File

SOURCE=..\Melkor\Util\LogWriter.h
# End Source File
# Begin Source File

SOURCE=..\Melkor\Util\Matrix.h
# End Source File
# Begin Source File

SOURCE=..\Melkor\Util\memoryTracking.h
# End Source File
# Begin Source File

SOURCE=..\Melkor\Util\Misc.h
# End Source File
# Begin Source File

SOURCE=..\Melkor\Util\Move.h
# End Source File
# Begin Source File

SOURCE=..\Melkor\Util\my_vector.h
# End Source File
# Begin Source File

SOURCE=..\Melkor\Util\ObjectMatrix.h
# End Source File
# Begin Source File

SOURCE=..\Melkor\Util\Point.h
# End Source File
# Begin Source File

SOURCE=..\Melkor\Util\Rect.h
# End Source File
# Begin Source File

SOURCE=..\Melkor\Util\SmartPtr.h
# End Source File
# Begin Source File

SOURCE=..\Melkor\Util\SpecialPoint.h
# End Source File
# End Group
# Begin Group "Patterns"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\Melkor\Patterns\EyeShapePattern.h
# End Source File
# Begin Source File

SOURCE=..\Melkor\Patterns\Orderer.h
# End Source File
# Begin Source File

SOURCE=..\Melkor\Patterns\Pattern.h
# End Source File
# Begin Source File

SOURCE=..\Melkor\Patterns\PatternMatcher.h
# End Source File
# Begin Source File

SOURCE=..\Melkor\Patterns\PatternTree.h
# End Source File
# Begin Source File

SOURCE=..\Melkor\Patterns\SpiralOrderer.h
# End Source File
# End Group
# Begin Group "Test No. 1"

# PROP Default_Filter ""
# Begin Group "Util No. 3"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\Melkor\Test\Util\BoardStructTest.h
# End Source File
# Begin Source File

SOURCE=..\Melkor\Test\Util\GoStringTest.h
# End Source File
# Begin Source File

SOURCE=..\Melkor\Test\Util\GoTest.h
# End Source File
# End Group
# End Group
# Begin Source File

SOURCE=..\Melkor\AllLegalSuggester.h
# End Source File
# Begin Source File

SOURCE=..\Melkor\CaptureSuggester.h
# End Source File
# Begin Source File

SOURCE=..\Melkor\GeneralSuggester.h
# End Source File
# Begin Source File

SOURCE=..\Melkor\GTPFunctions.h
# End Source File
# Begin Source File

SOURCE=..\Melkor\LadderSuggester.h
# End Source File
# Begin Source File

SOURCE=..\Melkor\LooseTacticalSuggester.h
# End Source File
# Begin Source File

SOURCE=..\Melkor\Main.h
# End Source File
# Begin Source File

SOURCE=..\Melkor\Melkor.h
# End Source File
# Begin Source File

SOURCE=.\MelkorDLL.h
# End Source File
# Begin Source File

SOURCE=..\Melkor\MoveSuggester.h
# End Source File
# Begin Source File

SOURCE=..\Melkor\MTDFSearch.h
# End Source File
# Begin Source File

SOURCE=..\Melkor\Node.h
# End Source File
# Begin Source File

SOURCE=..\Melkor\SearchAlgorithm.h
# End Source File
# Begin Source File

SOURCE=..\Melkor\SearchSettings.h
# End Source File
# Begin Source File

SOURCE=..\Melkor\Test.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\DLLutil.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# End Group
# Begin Source File

SOURCE=.\ReadMe.txt
# End Source File
# End Target
# End Project
