# Microsoft Developer Studio Project File - Name="Trainnet" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Console Application" 0x0103

CFG=Trainnet - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "Trainnet.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "Trainnet.mak" CFG="Trainnet - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Trainnet - Win32 Release" (based on "Win32 (x86) Console Application")
!MESSAGE "Trainnet - Win32 Debug" (based on "Win32 (x86) Console Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "Trainnet - Win32 Release"

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
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /W3 /GX /O2 /I "./" /I "../../../src/NeuralNetwork" /I "../../../src/Util" /I "../../../src/SGF" /I "../../../src/GTP" /I "../../../src/Goal" /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386
# SUBTRACT LINK32 /profile /map

!ELSEIF  "$(CFG)" == "Trainnet - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 1
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /GZ /c
# ADD CPP /nologo /W3 /Gm /GX /ZI /Od /I "./" /I "../../../src/NeuralNetwork" /I "../../../src/Util" /I "../../../src/SGF" /I "../../../src/GTP" /I "../../../src/Goal" /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /GZ /c
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept
# ADD LINK32 /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept

!ENDIF 

# Begin Target

# Name "Trainnet - Win32 Release"
# Name "Trainnet - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Group "NeuralNetwork"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\..\src\NeuralNetwork\BPN.cpp

!IF  "$(CFG)" == "Trainnet - Win32 Release"

# PROP Intermediate_Dir "../../../src/NeuralNetwork/Release"

!ELSEIF  "$(CFG)" == "Trainnet - Win32 Debug"

# PROP Intermediate_Dir "../../../src/NeuralNetwork/Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\src\NeuralNetwork\BPNExpAct.cpp

!IF  "$(CFG)" == "Trainnet - Win32 Release"

# PROP Intermediate_Dir "../../../src/NeuralNetwork/Release"

!ELSEIF  "$(CFG)" == "Trainnet - Win32 Debug"

# PROP Intermediate_Dir "../../../src/NeuralNetwork/Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\src\NeuralNetwork\BPNGoAdapter.cpp

!IF  "$(CFG)" == "Trainnet - Win32 Release"

# PROP Intermediate_Dir "../../../src/NeuralNetwork/Release"

!ELSEIF  "$(CFG)" == "Trainnet - Win32 Debug"

# PROP Intermediate_Dir "../../../src/NeuralNetwork/Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\src\NeuralNetwork\BPNGoAdapterInterface.cpp

!IF  "$(CFG)" == "Trainnet - Win32 Release"

# PROP Intermediate_Dir "../../../src/NeuralNetwork/Release"

!ELSEIF  "$(CFG)" == "Trainnet - Win32 Debug"

# PROP Intermediate_Dir "../../../src/NeuralNetwork/Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\src\NeuralNetwork\BPNGoTrainer.cpp

!IF  "$(CFG)" == "Trainnet - Win32 Release"

# PROP Intermediate_Dir "../../../src/NeuralNetwork/Release"

!ELSEIF  "$(CFG)" == "Trainnet - Win32 Debug"

# PROP Intermediate_Dir "../../../src/NeuralNetwork/Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\src\NeuralNetwork\BPNTrainer.cpp

!IF  "$(CFG)" == "Trainnet - Win32 Release"

# PROP Intermediate_Dir "../../../src/NeuralNetwork/Release"

!ELSEIF  "$(CFG)" == "Trainnet - Win32 Debug"

# PROP Intermediate_Dir "../../../src/NeuralNetwork/Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\src\NeuralNetwork\CN.cpp

!IF  "$(CFG)" == "Trainnet - Win32 Release"

# PROP Intermediate_Dir "../../../src/NeuralNetwork/Release"

!ELSEIF  "$(CFG)" == "Trainnet - Win32 Debug"

# PROP Intermediate_Dir "../../../src/NeuralNetwork/Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\src\NeuralNetwork\CNBPNTrainer.cpp

!IF  "$(CFG)" == "Trainnet - Win32 Release"

# PROP Intermediate_Dir "../../../src/NeuralNetwork/Release"

!ELSEIF  "$(CFG)" == "Trainnet - Win32 Debug"

# PROP Intermediate_Dir "../../../src/NeuralNetwork/Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\src\NeuralNetwork\NeuralNetwork.cpp

!IF  "$(CFG)" == "Trainnet - Win32 Release"

# PROP Intermediate_Dir "../../../src/NeuralNetwork/Release"

!ELSEIF  "$(CFG)" == "Trainnet - Win32 Debug"

# PROP Intermediate_Dir "../../../src/NeuralNetwork/Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\src\NeuralNetwork\newBPN10GoAdapter.cpp

!IF  "$(CFG)" == "Trainnet - Win32 Release"

# PROP Intermediate_Dir "../../../src/NeuralNetwork/Release"

!ELSEIF  "$(CFG)" == "Trainnet - Win32 Debug"

# PROP Intermediate_Dir "../../../src/NeuralNetwork/Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\src\NeuralNetwork\newBPN11GoAdapter.cpp

!IF  "$(CFG)" == "Trainnet - Win32 Release"

# PROP Intermediate_Dir "../../../src/NeuralNetwork/Release"

!ELSEIF  "$(CFG)" == "Trainnet - Win32 Debug"

# PROP Intermediate_Dir "../../../src/NeuralNetwork/Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\src\NeuralNetwork\newBPN12GoAdapter.cpp

!IF  "$(CFG)" == "Trainnet - Win32 Release"

# PROP Intermediate_Dir "../../../src/NeuralNetwork/Release"

!ELSEIF  "$(CFG)" == "Trainnet - Win32 Debug"

# PROP Intermediate_Dir "../../../src/NeuralNetwork/Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\src\NeuralNetwork\newBPN2GoAdapter.cpp

!IF  "$(CFG)" == "Trainnet - Win32 Release"

# PROP Intermediate_Dir "../../../src/NeuralNetwork/Release"

!ELSEIF  "$(CFG)" == "Trainnet - Win32 Debug"

# PROP Intermediate_Dir "../../../src/NeuralNetwork/Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\src\NeuralNetwork\newBPN3GoAdapter.cpp

!IF  "$(CFG)" == "Trainnet - Win32 Release"

# PROP Intermediate_Dir "../../../src/NeuralNetwork/Release"

!ELSEIF  "$(CFG)" == "Trainnet - Win32 Debug"

# PROP Intermediate_Dir "../../../src/NeuralNetwork/Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\src\NeuralNetwork\newBPN4GoAdapter.cpp

!IF  "$(CFG)" == "Trainnet - Win32 Release"

# PROP Intermediate_Dir "../../../src/NeuralNetwork/Release"

!ELSEIF  "$(CFG)" == "Trainnet - Win32 Debug"

# PROP Intermediate_Dir "../../../src/NeuralNetwork/Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\src\NeuralNetwork\newBPN5GoAdapter.cpp

!IF  "$(CFG)" == "Trainnet - Win32 Release"

# PROP Intermediate_Dir "../../../src/NeuralNetwork/Release"

!ELSEIF  "$(CFG)" == "Trainnet - Win32 Debug"

# PROP Intermediate_Dir "../../../src/NeuralNetwork/Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\src\NeuralNetwork\newBPN6GoAdapter.cpp

!IF  "$(CFG)" == "Trainnet - Win32 Release"

# PROP Intermediate_Dir "../../../src/NeuralNetwork/Release"

!ELSEIF  "$(CFG)" == "Trainnet - Win32 Debug"

# PROP Intermediate_Dir "../../../src/NeuralNetwork/Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\src\NeuralNetwork\newBPN7GoAdapter.cpp

!IF  "$(CFG)" == "Trainnet - Win32 Release"

# PROP Intermediate_Dir "../../../src/NeuralNetwork/Release"

!ELSEIF  "$(CFG)" == "Trainnet - Win32 Debug"

# PROP Intermediate_Dir "../../../src/NeuralNetwork/Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\src\NeuralNetwork\newBPN8GoAdapter.cpp

!IF  "$(CFG)" == "Trainnet - Win32 Release"

# PROP Intermediate_Dir "../../../src/NeuralNetwork/Release"

!ELSEIF  "$(CFG)" == "Trainnet - Win32 Debug"

# PROP Intermediate_Dir "../../../src/NeuralNetwork/Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\src\NeuralNetwork\newBPN9GoAdapter.cpp

!IF  "$(CFG)" == "Trainnet - Win32 Release"

# PROP Intermediate_Dir "../../../src/NeuralNetwork/Release"

!ELSEIF  "$(CFG)" == "Trainnet - Win32 Debug"

# PROP Intermediate_Dir "../../../src/NeuralNetwork/Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\src\NeuralNetwork\newBPNGoAdapter.cpp

!IF  "$(CFG)" == "Trainnet - Win32 Release"

# PROP Intermediate_Dir "../../../src/NeuralNetwork/Release"

!ELSEIF  "$(CFG)" == "Trainnet - Win32 Debug"

# PROP Intermediate_Dir "../../../src/NeuralNetwork/Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\src\NeuralNetwork\NNDatabase.cpp

!IF  "$(CFG)" == "Trainnet - Win32 Release"

# PROP Intermediate_Dir "../../../src/NeuralNetwork/Release"

!ELSEIF  "$(CFG)" == "Trainnet - Win32 Debug"

# PROP Intermediate_Dir "../../../src/NeuralNetwork/Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\src\NeuralNetwork\RBF.cpp

!IF  "$(CFG)" == "Trainnet - Win32 Release"

# PROP Intermediate_Dir "../../../src/NeuralNetwork/Release"

!ELSEIF  "$(CFG)" == "Trainnet - Win32 Debug"

# PROP Intermediate_Dir "../../../src/NeuralNetwork/Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\src\NeuralNetwork\RBFBPNTrainer.cpp

!IF  "$(CFG)" == "Trainnet - Win32 Release"

# PROP Intermediate_Dir "../../../src/NeuralNetwork/Release"

!ELSEIF  "$(CFG)" == "Trainnet - Win32 Debug"

# PROP Intermediate_Dir "../../../src/NeuralNetwork/Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\src\NeuralNetwork\RPROPBPNTrainer.cpp

!IF  "$(CFG)" == "Trainnet - Win32 Release"

# PROP Intermediate_Dir "../../../src/NeuralNetwork/Release"

!ELSEIF  "$(CFG)" == "Trainnet - Win32 Debug"

# PROP Intermediate_Dir "../../../src/NeuralNetwork/Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\src\NeuralNetwork\Safety1BPNGoAdapter.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\NeuralNetwork\Safety1BPNGoTrainer.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\NeuralNetwork\TDBPNTrainer.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\NeuralNetwork\Urgency1BPNGoTrainer.cpp

!IF  "$(CFG)" == "Trainnet - Win32 Release"

# PROP Intermediate_Dir "../../../src/NeuralNetwork/Release"

!ELSEIF  "$(CFG)" == "Trainnet - Win32 Debug"

# PROP Intermediate_Dir "../../../src/NeuralNetwork/Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\src\NeuralNetwork\Urgency1GoAdapter.cpp

!IF  "$(CFG)" == "Trainnet - Win32 Release"

# PROP Intermediate_Dir "../../../src/NeuralNetwork/Release"

!ELSEIF  "$(CFG)" == "Trainnet - Win32 Debug"

# PROP Intermediate_Dir "../../../src/NeuralNetwork/Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\src\NeuralNetwork\Urgency3BPN.cpp

!IF  "$(CFG)" == "Trainnet - Win32 Release"

# PROP Intermediate_Dir "../../../src/NeuralNetwork/Release"

!ELSEIF  "$(CFG)" == "Trainnet - Win32 Debug"

# PROP Intermediate_Dir "../../../src/NeuralNetwork/Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\src\NeuralNetwork\Urgency3BPNGoTrainer.cpp

!IF  "$(CFG)" == "Trainnet - Win32 Release"

# PROP Intermediate_Dir "../../../src/NeuralNetwork/Release"

!ELSEIF  "$(CFG)" == "Trainnet - Win32 Debug"

# PROP Intermediate_Dir "../../../src/NeuralNetwork/Debug"

!ENDIF 

# End Source File
# End Group
# Begin Group "SGF"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\..\src\SGF\SGFGameInfo.cpp

!IF  "$(CFG)" == "Trainnet - Win32 Release"

# PROP Intermediate_Dir "../../../src/SGF/Release"

!ELSEIF  "$(CFG)" == "Trainnet - Win32 Debug"

# PROP Intermediate_Dir "../../../src/SGF/Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\src\SGF\SGFNode.cpp

!IF  "$(CFG)" == "Trainnet - Win32 Release"

# PROP Intermediate_Dir "../../../src/SGF/Release"

!ELSEIF  "$(CFG)" == "Trainnet - Win32 Debug"

# PROP Intermediate_Dir "../../../src/SGF/Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\src\SGF\SGFProperty.cpp

!IF  "$(CFG)" == "Trainnet - Win32 Release"

# PROP Intermediate_Dir "../../../src/SGF/Release"

!ELSEIF  "$(CFG)" == "Trainnet - Win32 Debug"

# PROP Intermediate_Dir "../../../src/SGF/Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\src\SGF\SGFReader.cpp

!IF  "$(CFG)" == "Trainnet - Win32 Release"

# PROP Intermediate_Dir "../../../src/SGF/Release"

!ELSEIF  "$(CFG)" == "Trainnet - Win32 Debug"

# PROP Intermediate_Dir "../../../src/SGF/Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\src\SGF\SGFTree.cpp

!IF  "$(CFG)" == "Trainnet - Win32 Release"

# PROP Intermediate_Dir "../../../src/SGF/Release"

!ELSEIF  "$(CFG)" == "Trainnet - Win32 Debug"

# PROP Intermediate_Dir "../../../src/SGF/Debug"

!ENDIF 

# End Source File
# End Group
# Begin Group "Util No. 1"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\..\src\Util\BoardStruct.cpp

!IF  "$(CFG)" == "Trainnet - Win32 Release"

# PROP Intermediate_Dir "../../../src/Util/Release"

!ELSEIF  "$(CFG)" == "Trainnet - Win32 Debug"

# PROP Intermediate_Dir "../../../src/Util/Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\src\Util\Go.cpp

!IF  "$(CFG)" == "Trainnet - Win32 Release"

# PROP Intermediate_Dir "../../../src/Util/Release"

!ELSEIF  "$(CFG)" == "Trainnet - Win32 Debug"

# PROP Intermediate_Dir "../../../src/Util/Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\src\Util\GoString.cpp

!IF  "$(CFG)" == "Trainnet - Win32 Release"

# PROP Intermediate_Dir "../../../src/Util/Release"

!ELSEIF  "$(CFG)" == "Trainnet - Win32 Debug"

# PROP Intermediate_Dir "../../../src/Util/Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\src\Util\Misc.cpp

!IF  "$(CFG)" == "Trainnet - Win32 Release"

# PROP Intermediate_Dir "../../../src/Util/Release"

!ELSEIF  "$(CFG)" == "Trainnet - Win32 Debug"

# PROP Intermediate_Dir "../../../src/Util/Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\src\Util\Move.cpp

!IF  "$(CFG)" == "Trainnet - Win32 Release"

# PROP Intermediate_Dir "../../../src/Util/Release"

!ELSEIF  "$(CFG)" == "Trainnet - Win32 Debug"

# PROP Intermediate_Dir "../../../src/Util/Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\src\Util\Rect.cpp

!IF  "$(CFG)" == "Trainnet - Win32 Release"

# PROP Intermediate_Dir "../../../src/Util/Release"

!ELSEIF  "$(CFG)" == "Trainnet - Win32 Debug"

# PROP Intermediate_Dir "../../../src/Util/Debug"

!ENDIF 

# End Source File
# End Group
# Begin Group "Goal"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\..\src\Goal\CaptureGoal.cpp

!IF  "$(CFG)" == "Trainnet - Win32 Release"

# PROP Intermediate_Dir "../../../src/Goal/Release"

!ELSEIF  "$(CFG)" == "Trainnet - Win32 Debug"

# PROP Intermediate_Dir "../../../src/Goal/Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\src\Goal\InfluenceGoal.cpp

!IF  "$(CFG)" == "Trainnet - Win32 Release"

# PROP Intermediate_Dir "../../../src/Goal/Release"

!ELSEIF  "$(CFG)" == "Trainnet - Win32 Debug"

# PROP Intermediate_Dir "../../../src/Goal/Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\src\Goal\LadderGoal.cpp

!IF  "$(CFG)" == "Trainnet - Win32 Release"

# PROP Intermediate_Dir "../../../src/Goal/Release"

!ELSEIF  "$(CFG)" == "Trainnet - Win32 Debug"

# PROP Intermediate_Dir "../../../src/Goal/Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\src\Goal\LibCountCaptureGoal.cpp

!IF  "$(CFG)" == "Trainnet - Win32 Release"

# PROP Intermediate_Dir "../../../src/Goal/Release"

!ELSEIF  "$(CFG)" == "Trainnet - Win32 Debug"

# PROP Intermediate_Dir "../../../src/Goal/Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\src\Goal\LibertyCountGoal.cpp

!IF  "$(CFG)" == "Trainnet - Win32 Release"

# PROP Intermediate_Dir "../../../src/Goal/Release"

!ELSEIF  "$(CFG)" == "Trainnet - Win32 Debug"

# PROP Intermediate_Dir "../../../src/Goal/Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\src\Goal\LifeDeathGoal.cpp

!IF  "$(CFG)" == "Trainnet - Win32 Release"

# PROP Intermediate_Dir "../../../src/Goal/Release"

!ELSEIF  "$(CFG)" == "Trainnet - Win32 Debug"

# PROP Intermediate_Dir "../../../src/Goal/Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\src\Goal\StoneCountGoal.cpp

!IF  "$(CFG)" == "Trainnet - Win32 Release"

# PROP Intermediate_Dir "../../../src/Goal/Release"

!ELSEIF  "$(CFG)" == "Trainnet - Win32 Debug"

# PROP Intermediate_Dir "../../../src/Goal/Debug"

!ENDIF 

# End Source File
# End Group
# Begin Source File

SOURCE=.\LogWriter.cpp

!IF  "$(CFG)" == "Trainnet - Win32 Release"

# PROP Intermediate_Dir "Release"

!ELSEIF  "$(CFG)" == "Trainnet - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\main.cpp

!IF  "$(CFG)" == "Trainnet - Win32 Release"

# PROP Intermediate_Dir "Release"

!ELSEIF  "$(CFG)" == "Trainnet - Win32 Debug"

!ENDIF 

# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Group "NeuralNetwork No. 1"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\..\src\NeuralNetwork\BPN.h

!IF  "$(CFG)" == "Trainnet - Win32 Release"

# PROP Intermediate_Dir "../../../src/NeuralNetwork/Release"

!ELSEIF  "$(CFG)" == "Trainnet - Win32 Debug"

# PROP Intermediate_Dir "../../../src/NeuralNetwork/Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\src\NeuralNetwork\BPNExpAct.h

!IF  "$(CFG)" == "Trainnet - Win32 Release"

# PROP Intermediate_Dir "../../../src/NeuralNetwork/Release"

!ELSEIF  "$(CFG)" == "Trainnet - Win32 Debug"

# PROP Intermediate_Dir "../../../src/NeuralNetwork/Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\src\NeuralNetwork\BPNExpActGoAdapter.h

!IF  "$(CFG)" == "Trainnet - Win32 Release"

# PROP Intermediate_Dir "../../../src/NeuralNetwork/Release"

!ELSEIF  "$(CFG)" == "Trainnet - Win32 Debug"

# PROP Intermediate_Dir "../../../src/NeuralNetwork/Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\src\NeuralNetwork\BPNGoAdapter.h

!IF  "$(CFG)" == "Trainnet - Win32 Release"

# PROP Intermediate_Dir "../../../src/NeuralNetwork/Release"

!ELSEIF  "$(CFG)" == "Trainnet - Win32 Debug"

# PROP Intermediate_Dir "../../../src/NeuralNetwork/Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\src\NeuralNetwork\BPNGoAdapterInterface.h

!IF  "$(CFG)" == "Trainnet - Win32 Release"

# PROP Intermediate_Dir "../../../src/NeuralNetwork/Release"

!ELSEIF  "$(CFG)" == "Trainnet - Win32 Debug"

# PROP Intermediate_Dir "../../../src/NeuralNetwork/Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\src\NeuralNetwork\BPNGoTrainer.h

!IF  "$(CFG)" == "Trainnet - Win32 Release"

# PROP Intermediate_Dir "../../../src/NeuralNetwork/Release"

!ELSEIF  "$(CFG)" == "Trainnet - Win32 Debug"

# PROP Intermediate_Dir "../../../src/NeuralNetwork/Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\src\NeuralNetwork\BPNGoTrainerInterface.h

!IF  "$(CFG)" == "Trainnet - Win32 Release"

# PROP Intermediate_Dir "../../../src/NeuralNetwork/Release"

!ELSEIF  "$(CFG)" == "Trainnet - Win32 Debug"

# PROP Intermediate_Dir "../../../src/NeuralNetwork/Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\src\NeuralNetwork\BPNTrainer.h

!IF  "$(CFG)" == "Trainnet - Win32 Release"

# PROP Intermediate_Dir "../../../src/NeuralNetwork/Release"

!ELSEIF  "$(CFG)" == "Trainnet - Win32 Debug"

# PROP Intermediate_Dir "../../../src/NeuralNetwork/Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\src\NeuralNetwork\CN.h

!IF  "$(CFG)" == "Trainnet - Win32 Release"

# PROP Intermediate_Dir "../../../src/NeuralNetwork/Release"

!ELSEIF  "$(CFG)" == "Trainnet - Win32 Debug"

# PROP Intermediate_Dir "../../../src/NeuralNetwork/Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\src\NeuralNetwork\CNBPNTrainer.h

!IF  "$(CFG)" == "Trainnet - Win32 Release"

# PROP Intermediate_Dir "../../../src/NeuralNetwork/Release"

!ELSEIF  "$(CFG)" == "Trainnet - Win32 Debug"

# PROP Intermediate_Dir "../../../src/NeuralNetwork/Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\src\NeuralNetwork\CNGoAdapter.h

!IF  "$(CFG)" == "Trainnet - Win32 Release"

# PROP Intermediate_Dir "../../../src/NeuralNetwork/Release"

!ELSEIF  "$(CFG)" == "Trainnet - Win32 Debug"

# PROP Intermediate_Dir "../../../src/NeuralNetwork/Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\src\NeuralNetwork\NeuralNetwork.h

!IF  "$(CFG)" == "Trainnet - Win32 Release"

# PROP Intermediate_Dir "../../../src/NeuralNetwork/Release"

!ELSEIF  "$(CFG)" == "Trainnet - Win32 Debug"

# PROP Intermediate_Dir "../../../src/NeuralNetwork/Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\src\NeuralNetwork\newBPN10GoAdapter.h

!IF  "$(CFG)" == "Trainnet - Win32 Release"

# PROP Intermediate_Dir "../../../src/NeuralNetwork/Release"

!ELSEIF  "$(CFG)" == "Trainnet - Win32 Debug"

# PROP Intermediate_Dir "../../../src/NeuralNetwork/Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\src\NeuralNetwork\newBPN11GoAdapter.h

!IF  "$(CFG)" == "Trainnet - Win32 Release"

# PROP Intermediate_Dir "../../../src/NeuralNetwork/Release"

!ELSEIF  "$(CFG)" == "Trainnet - Win32 Debug"

# PROP Intermediate_Dir "../../../src/NeuralNetwork/Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\src\NeuralNetwork\newBPN12GoAdapter.h

!IF  "$(CFG)" == "Trainnet - Win32 Release"

# PROP Intermediate_Dir "../../../src/NeuralNetwork/Release"

!ELSEIF  "$(CFG)" == "Trainnet - Win32 Debug"

# PROP Intermediate_Dir "../../../src/NeuralNetwork/Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\src\NeuralNetwork\newBPN2GoAdapter.h

!IF  "$(CFG)" == "Trainnet - Win32 Release"

# PROP Intermediate_Dir "../../../src/NeuralNetwork/Release"

!ELSEIF  "$(CFG)" == "Trainnet - Win32 Debug"

# PROP Intermediate_Dir "../../../src/NeuralNetwork/Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\src\NeuralNetwork\newBPN3GoAdapter.h

!IF  "$(CFG)" == "Trainnet - Win32 Release"

# PROP Intermediate_Dir "../../../src/NeuralNetwork/Release"

!ELSEIF  "$(CFG)" == "Trainnet - Win32 Debug"

# PROP Intermediate_Dir "../../../src/NeuralNetwork/Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\src\NeuralNetwork\newBPN4GoAdapter.h

!IF  "$(CFG)" == "Trainnet - Win32 Release"

# PROP Intermediate_Dir "../../../src/NeuralNetwork/Release"

!ELSEIF  "$(CFG)" == "Trainnet - Win32 Debug"

# PROP Intermediate_Dir "../../../src/NeuralNetwork/Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\src\NeuralNetwork\newBPN5GoAdapter.h

!IF  "$(CFG)" == "Trainnet - Win32 Release"

# PROP Intermediate_Dir "../../../src/NeuralNetwork/Release"

!ELSEIF  "$(CFG)" == "Trainnet - Win32 Debug"

# PROP Intermediate_Dir "../../../src/NeuralNetwork/Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\src\NeuralNetwork\newBPN6GoAdapter.h

!IF  "$(CFG)" == "Trainnet - Win32 Release"

# PROP Intermediate_Dir "../../../src/NeuralNetwork/Release"

!ELSEIF  "$(CFG)" == "Trainnet - Win32 Debug"

# PROP Intermediate_Dir "../../../src/NeuralNetwork/Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\src\NeuralNetwork\newBPN7GoAdapter.h

!IF  "$(CFG)" == "Trainnet - Win32 Release"

# PROP Intermediate_Dir "../../../src/NeuralNetwork/Release"

!ELSEIF  "$(CFG)" == "Trainnet - Win32 Debug"

# PROP Intermediate_Dir "../../../src/NeuralNetwork/Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\src\NeuralNetwork\newBPN8GoAdapter.h

!IF  "$(CFG)" == "Trainnet - Win32 Release"

# PROP Intermediate_Dir "../../../src/NeuralNetwork/Release"

!ELSEIF  "$(CFG)" == "Trainnet - Win32 Debug"

# PROP Intermediate_Dir "../../../src/NeuralNetwork/Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\src\NeuralNetwork\newBPN9GoAdapter.h

!IF  "$(CFG)" == "Trainnet - Win32 Release"

# PROP Intermediate_Dir "../../../src/NeuralNetwork/Release"

!ELSEIF  "$(CFG)" == "Trainnet - Win32 Debug"

# PROP Intermediate_Dir "../../../src/NeuralNetwork/Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\src\NeuralNetwork\newBPNGoAdapter.h

!IF  "$(CFG)" == "Trainnet - Win32 Release"

# PROP Intermediate_Dir "../../../src/NeuralNetwork/Release"

!ELSEIF  "$(CFG)" == "Trainnet - Win32 Debug"

# PROP Intermediate_Dir "../../../src/NeuralNetwork/Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\src\NeuralNetwork\NNDatabase.h

!IF  "$(CFG)" == "Trainnet - Win32 Release"

# PROP Intermediate_Dir "../../../src/NeuralNetwork/Release"

!ELSEIF  "$(CFG)" == "Trainnet - Win32 Debug"

# PROP Intermediate_Dir "../../../src/NeuralNetwork/Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\src\NeuralNetwork\RBF.h

!IF  "$(CFG)" == "Trainnet - Win32 Release"

# PROP Intermediate_Dir "../../../src/NeuralNetwork/Release"

!ELSEIF  "$(CFG)" == "Trainnet - Win32 Debug"

# PROP Intermediate_Dir "../../../src/NeuralNetwork/Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\src\NeuralNetwork\RBFBPNTrainer.h

!IF  "$(CFG)" == "Trainnet - Win32 Release"

# PROP Intermediate_Dir "../../../src/NeuralNetwork/Release"

!ELSEIF  "$(CFG)" == "Trainnet - Win32 Debug"

# PROP Intermediate_Dir "../../../src/NeuralNetwork/Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\src\NeuralNetwork\RBFGoAdapter.h

!IF  "$(CFG)" == "Trainnet - Win32 Release"

# PROP Intermediate_Dir "../../../src/NeuralNetwork/Release"

!ELSEIF  "$(CFG)" == "Trainnet - Win32 Debug"

# PROP Intermediate_Dir "../../../src/NeuralNetwork/Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\src\NeuralNetwork\RPROPBPNTrainer.h

!IF  "$(CFG)" == "Trainnet - Win32 Release"

# PROP Intermediate_Dir "../../../src/NeuralNetwork/Release"

!ELSEIF  "$(CFG)" == "Trainnet - Win32 Debug"

# PROP Intermediate_Dir "../../../src/NeuralNetwork/Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\src\NeuralNetwork\Safety1BPNGoAdapter.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\NeuralNetwork\Safety1BPNGoTrainer.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\NeuralNetwork\TDBPNTrainer.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\NeuralNetwork\Urgency1BPNGoTrainer.h

!IF  "$(CFG)" == "Trainnet - Win32 Release"

# PROP Intermediate_Dir "../../../src/NeuralNetwork/Release"

!ELSEIF  "$(CFG)" == "Trainnet - Win32 Debug"

# PROP Intermediate_Dir "../../../src/NeuralNetwork/Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\src\NeuralNetwork\Urgency1GoAdapter.h

!IF  "$(CFG)" == "Trainnet - Win32 Release"

# PROP Intermediate_Dir "../../../src/NeuralNetwork/Release"

!ELSEIF  "$(CFG)" == "Trainnet - Win32 Debug"

# PROP Intermediate_Dir "../../../src/NeuralNetwork/Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\src\NeuralNetwork\Urgency3BPN.h

!IF  "$(CFG)" == "Trainnet - Win32 Release"

# PROP Intermediate_Dir "../../../src/NeuralNetwork/Release"

!ELSEIF  "$(CFG)" == "Trainnet - Win32 Debug"

# PROP Intermediate_Dir "../../../src/NeuralNetwork/Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\src\NeuralNetwork\Urgency3BPNGoAdapter.h

!IF  "$(CFG)" == "Trainnet - Win32 Release"

# PROP Intermediate_Dir "../../../src/NeuralNetwork/Release"

!ELSEIF  "$(CFG)" == "Trainnet - Win32 Debug"

# PROP Intermediate_Dir "../../../src/NeuralNetwork/Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\src\NeuralNetwork\Urgency3BPNGoTrainer.h

!IF  "$(CFG)" == "Trainnet - Win32 Release"

# PROP Intermediate_Dir "../../../src/NeuralNetwork/Release"

!ELSEIF  "$(CFG)" == "Trainnet - Win32 Debug"

# PROP Intermediate_Dir "../../../src/NeuralNetwork/Debug"

!ENDIF 

# End Source File
# End Group
# Begin Group "Util"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\..\src\Util\BMP.h

!IF  "$(CFG)" == "Trainnet - Win32 Release"

# PROP Intermediate_Dir "../../../src/Util/Release"

!ELSEIF  "$(CFG)" == "Trainnet - Win32 Debug"

# PROP Intermediate_Dir "../../../src/Util/Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\src\Util\BoardStruct.h

!IF  "$(CFG)" == "Trainnet - Win32 Release"

# PROP Intermediate_Dir "../../../src/Util/Release"

!ELSEIF  "$(CFG)" == "Trainnet - Win32 Debug"

# PROP Intermediate_Dir "../../../src/Util/Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\src\Util\globals.h

!IF  "$(CFG)" == "Trainnet - Win32 Release"

# PROP Intermediate_Dir "../../../src/Util/Release"

!ELSEIF  "$(CFG)" == "Trainnet - Win32 Debug"

# PROP Intermediate_Dir "../../../src/Util/Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\src\Util\Go.h

!IF  "$(CFG)" == "Trainnet - Win32 Release"

# PROP Intermediate_Dir "../../../src/Util/Release"

!ELSEIF  "$(CFG)" == "Trainnet - Win32 Debug"

# PROP Intermediate_Dir "../../../src/Util/Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\src\Util\GoString.h

!IF  "$(CFG)" == "Trainnet - Win32 Release"

# PROP Intermediate_Dir "../../../src/Util/Release"

!ELSEIF  "$(CFG)" == "Trainnet - Win32 Debug"

# PROP Intermediate_Dir "../../../src/Util/Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\src\Util\LogWriter.h

!IF  "$(CFG)" == "Trainnet - Win32 Release"

# PROP Intermediate_Dir "../../../src/Util/Release"

!ELSEIF  "$(CFG)" == "Trainnet - Win32 Debug"

# PROP Intermediate_Dir "../../../src/Util/Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\src\Util\Matrix.h

!IF  "$(CFG)" == "Trainnet - Win32 Release"

# PROP Intermediate_Dir "../../../src/Util/Release"

!ELSEIF  "$(CFG)" == "Trainnet - Win32 Debug"

# PROP Intermediate_Dir "../../../src/Util/Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\src\Util\Misc.h

!IF  "$(CFG)" == "Trainnet - Win32 Release"

# PROP Intermediate_Dir "../../../src/Util/Release"

!ELSEIF  "$(CFG)" == "Trainnet - Win32 Debug"

# PROP Intermediate_Dir "../../../src/Util/Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\src\Util\Move.h

!IF  "$(CFG)" == "Trainnet - Win32 Release"

# PROP Intermediate_Dir "../../../src/Util/Release"

!ELSEIF  "$(CFG)" == "Trainnet - Win32 Debug"

# PROP Intermediate_Dir "../../../src/Util/Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\src\Util\ObjectMatrix.h

!IF  "$(CFG)" == "Trainnet - Win32 Release"

# PROP Intermediate_Dir "../../../src/Util/Release"

!ELSEIF  "$(CFG)" == "Trainnet - Win32 Debug"

# PROP Intermediate_Dir "../../../src/Util/Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\src\Util\Point.h

!IF  "$(CFG)" == "Trainnet - Win32 Release"

# PROP Intermediate_Dir "../../../src/Util/Release"

!ELSEIF  "$(CFG)" == "Trainnet - Win32 Debug"

# PROP Intermediate_Dir "../../../src/Util/Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\src\Util\Rect.h

!IF  "$(CFG)" == "Trainnet - Win32 Release"

# PROP Intermediate_Dir "../../../src/Util/Release"

!ELSEIF  "$(CFG)" == "Trainnet - Win32 Debug"

# PROP Intermediate_Dir "../../../src/Util/Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\src\Util\SpecialPoint.h

!IF  "$(CFG)" == "Trainnet - Win32 Release"

# PROP Intermediate_Dir "../../../src/Util/Release"

!ELSEIF  "$(CFG)" == "Trainnet - Win32 Debug"

# PROP Intermediate_Dir "../../../src/Util/Debug"

!ENDIF 

# End Source File
# End Group
# Begin Group "SGF No. 1"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\..\src\SGF\SGFGameInfo.h

!IF  "$(CFG)" == "Trainnet - Win32 Release"

# PROP Intermediate_Dir "../../../src/SGF/Release"

!ELSEIF  "$(CFG)" == "Trainnet - Win32 Debug"

# PROP Intermediate_Dir "../../../src/SGF/Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\src\SGF\SGFNode.h

!IF  "$(CFG)" == "Trainnet - Win32 Release"

# PROP Intermediate_Dir "../../../src/SGF/Release"

!ELSEIF  "$(CFG)" == "Trainnet - Win32 Debug"

# PROP Intermediate_Dir "../../../src/SGF/Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\src\SGF\SGFProperty.h

!IF  "$(CFG)" == "Trainnet - Win32 Release"

# PROP Intermediate_Dir "../../../src/SGF/Release"

!ELSEIF  "$(CFG)" == "Trainnet - Win32 Debug"

# PROP Intermediate_Dir "../../../src/SGF/Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\src\SGF\SGFReader.h

!IF  "$(CFG)" == "Trainnet - Win32 Release"

# PROP Intermediate_Dir "../../../src/SGF/Release"

!ELSEIF  "$(CFG)" == "Trainnet - Win32 Debug"

# PROP Intermediate_Dir "../../../src/SGF/Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\src\SGF\SGFTree.h

!IF  "$(CFG)" == "Trainnet - Win32 Release"

# PROP Intermediate_Dir "../../../src/SGF/Release"

!ELSEIF  "$(CFG)" == "Trainnet - Win32 Debug"

# PROP Intermediate_Dir "../../../src/SGF/Debug"

!ENDIF 

# End Source File
# End Group
# Begin Group "Goal No. 1"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\..\src\Goal\CaptureGoal.h

!IF  "$(CFG)" == "Trainnet - Win32 Release"

# PROP Intermediate_Dir "../../../src/Goal/Release"

!ELSEIF  "$(CFG)" == "Trainnet - Win32 Debug"

# PROP Intermediate_Dir "../../../src/Goal/Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\src\Goal\Goal.h

!IF  "$(CFG)" == "Trainnet - Win32 Release"

# PROP Intermediate_Dir "../../../src/Goal/Release"

!ELSEIF  "$(CFG)" == "Trainnet - Win32 Debug"

# PROP Intermediate_Dir "../../../src/Goal/Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\src\Goal\GoalFunctions.h

!IF  "$(CFG)" == "Trainnet - Win32 Release"

# PROP Intermediate_Dir "../../../src/Goal/Release"

!ELSEIF  "$(CFG)" == "Trainnet - Win32 Debug"

# PROP Intermediate_Dir "../../../src/Goal/Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\src\Goal\GoalInterface.h

!IF  "$(CFG)" == "Trainnet - Win32 Release"

# PROP Intermediate_Dir "../../../src/Goal/Release"

!ELSEIF  "$(CFG)" == "Trainnet - Win32 Debug"

# PROP Intermediate_Dir "../../../src/Goal/Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\src\Goal\InfluenceGoal.h

!IF  "$(CFG)" == "Trainnet - Win32 Release"

# PROP Intermediate_Dir "../../../src/Goal/Release"

!ELSEIF  "$(CFG)" == "Trainnet - Win32 Debug"

# PROP Intermediate_Dir "../../../src/Goal/Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\src\Goal\LadderGoal.h

!IF  "$(CFG)" == "Trainnet - Win32 Release"

# PROP Intermediate_Dir "../../../src/Goal/Release"

!ELSEIF  "$(CFG)" == "Trainnet - Win32 Debug"

# PROP Intermediate_Dir "../../../src/Goal/Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\src\Goal\LibCountCaptureGoal.h

!IF  "$(CFG)" == "Trainnet - Win32 Release"

# PROP Intermediate_Dir "../../../src/Goal/Release"

!ELSEIF  "$(CFG)" == "Trainnet - Win32 Debug"

# PROP Intermediate_Dir "../../../src/Goal/Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\src\Goal\LibertyCountGoal.h

!IF  "$(CFG)" == "Trainnet - Win32 Release"

# PROP Intermediate_Dir "../../../src/Goal/Release"

!ELSEIF  "$(CFG)" == "Trainnet - Win32 Debug"

# PROP Intermediate_Dir "../../../src/Goal/Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\src\Goal\LifeDeathGoal.h

!IF  "$(CFG)" == "Trainnet - Win32 Release"

# PROP Intermediate_Dir "../../../src/Goal/Release"

!ELSEIF  "$(CFG)" == "Trainnet - Win32 Debug"

# PROP Intermediate_Dir "../../../src/Goal/Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\src\Goal\StoneCountGoal.h

!IF  "$(CFG)" == "Trainnet - Win32 Release"

# PROP Intermediate_Dir "../../../src/Goal/Release"

!ELSEIF  "$(CFG)" == "Trainnet - Win32 Debug"

# PROP Intermediate_Dir "../../../src/Goal/Debug"

!ENDIF 

# End Source File
# End Group
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# End Group
# Begin Group "Docs"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Docs\todo.txt
# End Source File
# End Group
# End Target
# End Project
