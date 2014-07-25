# Microsoft Developer Studio Project File - Name="Melkor" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Console Application" 0x0103

CFG=Melkor - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "Melkor.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "Melkor.mak" CFG="Melkor - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Melkor - Win32 Release" (based on "Win32 (x86) Console Application")
!MESSAGE "Melkor - Win32 Debug" (based on "Win32 (x86) Console Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "Melkor - Win32 Release"

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
# ADD CPP /nologo /MD /W3 /GR /GX /Zd /O2 /I "./" /I "./Goal" /I "./NeuralNetwork" /I "./Patterns" /I "./SGF" /I "./Util" /I "./Test" /I "./Test/Util" /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /c
# ADD BASE RSC /l 0x809 /d "NDEBUG"
# ADD RSC /l 0x809 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib cppunit.lib /nologo /subsystem:console /map /debug /machine:I386
# SUBTRACT LINK32 /profile
# Begin Special Build Tool
TargetPath=.\Release\Melkor.exe
SOURCE="$(InputPath)"
PostBuild_Desc=Unit Testing
PostBuild_Cmds=$(TargetPath) -mode unit_test
# End Special Build Tool

!ELSEIF  "$(CFG)" == "Melkor - Win32 Debug"

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
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GR /GX /ZI /Od /I "./" /I "./Goal" /I "./NeuralNetwork" /I "./Patterns" /I "./SGF" /I "./Util" /I "./Test" /I "./Test/Util" /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /GZ /c
# ADD BASE RSC /l 0x809 /d "_DEBUG"
# ADD RSC /l 0x809 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib cppunitd.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept
# Begin Special Build Tool
TargetPath=.\Debug\Melkor.exe
SOURCE="$(InputPath)"
PostBuild_Desc=Unit Testing
PostBuild_Cmds=$(TargetPath) -mode unit_test
# End Special Build Tool

!ENDIF 

# Begin Target

# Name "Melkor - Win32 Release"
# Name "Melkor - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Group "Goal"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Goal\CaptureGoal.cpp

!IF  "$(CFG)" == "Melkor - Win32 Release"

# PROP Intermediate_Dir "Goal/Release"

!ELSEIF  "$(CFG)" == "Melkor - Win32 Debug"

# PROP Intermediate_Dir "Goal/Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Goal\InfluenceGoal.cpp

!IF  "$(CFG)" == "Melkor - Win32 Release"

# PROP Intermediate_Dir "Goal/Release"

!ELSEIF  "$(CFG)" == "Melkor - Win32 Debug"

# PROP Intermediate_Dir "Goal/Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Goal\LadderGoal.cpp

!IF  "$(CFG)" == "Melkor - Win32 Release"

# PROP Intermediate_Dir "Goal/Release"

!ELSEIF  "$(CFG)" == "Melkor - Win32 Debug"

# PROP Intermediate_Dir "Goal/Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Goal\LibCountCaptureGoal.cpp

!IF  "$(CFG)" == "Melkor - Win32 Release"

# PROP Intermediate_Dir "Goal/Release"

!ELSEIF  "$(CFG)" == "Melkor - Win32 Debug"

# PROP Intermediate_Dir "Goal/Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Goal\LibertyCountGoal.cpp

!IF  "$(CFG)" == "Melkor - Win32 Release"

# PROP Intermediate_Dir "Goal/Release"

!ELSEIF  "$(CFG)" == "Melkor - Win32 Debug"

# PROP Intermediate_Dir "Goal/Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Goal\LifeDeathGoal.cpp

!IF  "$(CFG)" == "Melkor - Win32 Release"

# PROP Intermediate_Dir "Goal/Release"

!ELSEIF  "$(CFG)" == "Melkor - Win32 Debug"

# PROP Intermediate_Dir "Goal/Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Goal\LooseTacticalGoal.cpp

!IF  "$(CFG)" == "Melkor - Win32 Release"

# PROP Intermediate_Dir "Goal/Release"

!ELSEIF  "$(CFG)" == "Melkor - Win32 Debug"

# PROP Intermediate_Dir "Goal/Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Goal\StoneCountGoal.cpp

!IF  "$(CFG)" == "Melkor - Win32 Release"

# PROP Intermediate_Dir "Goal/Release"

!ELSEIF  "$(CFG)" == "Melkor - Win32 Debug"

# PROP Intermediate_Dir "Goal/Debug"

!ENDIF 

# End Source File
# End Group
# Begin Group "NeuralNetwork"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\NeuralNetwork\BPN.cpp

!IF  "$(CFG)" == "Melkor - Win32 Release"

# PROP Intermediate_Dir "NeuralNetwork/Release"

!ELSEIF  "$(CFG)" == "Melkor - Win32 Debug"

# PROP Intermediate_Dir "NeuralNetwork/Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\NeuralNetwork\BPNExpAct.cpp

!IF  "$(CFG)" == "Melkor - Win32 Release"

# PROP Intermediate_Dir "NeuralNetwork/Release"

!ELSEIF  "$(CFG)" == "Melkor - Win32 Debug"

# PROP Intermediate_Dir "NeuralNetwork/Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\NeuralNetwork\BPNGoAdapter.cpp

!IF  "$(CFG)" == "Melkor - Win32 Release"

# PROP Intermediate_Dir "NeuralNetwork/Release"

!ELSEIF  "$(CFG)" == "Melkor - Win32 Debug"

# PROP Intermediate_Dir "NeuralNetwork/Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\NeuralNetwork\BPNGoAdapterInterface.cpp

!IF  "$(CFG)" == "Melkor - Win32 Release"

# PROP Intermediate_Dir "NeuralNetwork/Release"

!ELSEIF  "$(CFG)" == "Melkor - Win32 Debug"

# PROP Intermediate_Dir "NeuralNetwork/Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\NeuralNetwork\BPNTrainer.cpp

!IF  "$(CFG)" == "Melkor - Win32 Release"

# PROP Intermediate_Dir "NeuralNetwork/Release"

!ELSEIF  "$(CFG)" == "Melkor - Win32 Debug"

# PROP Intermediate_Dir "NeuralNetwork/Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\NeuralNetwork\CN.cpp

!IF  "$(CFG)" == "Melkor - Win32 Release"

# PROP Intermediate_Dir "NeuralNetwork/Release"

!ELSEIF  "$(CFG)" == "Melkor - Win32 Debug"

# PROP Intermediate_Dir "NeuralNetwork/Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\NeuralNetwork\NeuralNetwork.cpp

!IF  "$(CFG)" == "Melkor - Win32 Release"

# PROP Intermediate_Dir "NeuralNetwork/Release"

!ELSEIF  "$(CFG)" == "Melkor - Win32 Debug"

# PROP Intermediate_Dir "NeuralNetwork/Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\NeuralNetwork\newBPN10GoAdapter.cpp

!IF  "$(CFG)" == "Melkor - Win32 Release"

# PROP Intermediate_Dir "NeuralNetwork/Release"

!ELSEIF  "$(CFG)" == "Melkor - Win32 Debug"

# PROP Intermediate_Dir "NeuralNetwork/Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\NeuralNetwork\newBPN11GoAdapter.cpp

!IF  "$(CFG)" == "Melkor - Win32 Release"

# PROP Intermediate_Dir "NeuralNetwork/Release"

!ELSEIF  "$(CFG)" == "Melkor - Win32 Debug"

# PROP Intermediate_Dir "NeuralNetwork/Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\NeuralNetwork\newBPN12GoAdapter.cpp

!IF  "$(CFG)" == "Melkor - Win32 Release"

# PROP Intermediate_Dir "NeuralNetwork/Release"

!ELSEIF  "$(CFG)" == "Melkor - Win32 Debug"

# PROP Intermediate_Dir "NeuralNetwork/Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\NeuralNetwork\newBPN2GoAdapter.cpp

!IF  "$(CFG)" == "Melkor - Win32 Release"

# PROP Intermediate_Dir "NeuralNetwork/Release"

!ELSEIF  "$(CFG)" == "Melkor - Win32 Debug"

# PROP Intermediate_Dir "NeuralNetwork/Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\NeuralNetwork\newBPN3GoAdapter.cpp

!IF  "$(CFG)" == "Melkor - Win32 Release"

# PROP Intermediate_Dir "NeuralNetwork/Release"

!ELSEIF  "$(CFG)" == "Melkor - Win32 Debug"

# PROP Intermediate_Dir "NeuralNetwork/Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\NeuralNetwork\newBPN4GoAdapter.cpp

!IF  "$(CFG)" == "Melkor - Win32 Release"

# PROP Intermediate_Dir "NeuralNetwork/Release"

!ELSEIF  "$(CFG)" == "Melkor - Win32 Debug"

# PROP Intermediate_Dir "NeuralNetwork/Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\NeuralNetwork\newBPN5GoAdapter.cpp

!IF  "$(CFG)" == "Melkor - Win32 Release"

# PROP Intermediate_Dir "NeuralNetwork/Release"

!ELSEIF  "$(CFG)" == "Melkor - Win32 Debug"

# PROP Intermediate_Dir "NeuralNetwork/Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\NeuralNetwork\newBPN6GoAdapter.cpp

!IF  "$(CFG)" == "Melkor - Win32 Release"

# PROP Intermediate_Dir "NeuralNetwork/Release"

!ELSEIF  "$(CFG)" == "Melkor - Win32 Debug"

# PROP Intermediate_Dir "NeuralNetwork/Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\NeuralNetwork\newBPN7GoAdapter.cpp

!IF  "$(CFG)" == "Melkor - Win32 Release"

# PROP Intermediate_Dir "NeuralNetwork/Release"

!ELSEIF  "$(CFG)" == "Melkor - Win32 Debug"

# PROP Intermediate_Dir "NeuralNetwork/Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\NeuralNetwork\newBPN8GoAdapter.cpp

!IF  "$(CFG)" == "Melkor - Win32 Release"

# PROP Intermediate_Dir "NeuralNetwork/Release"

!ELSEIF  "$(CFG)" == "Melkor - Win32 Debug"

# PROP Intermediate_Dir "NeuralNetwork/Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\NeuralNetwork\newBPN9GoAdapter.cpp

!IF  "$(CFG)" == "Melkor - Win32 Release"

# PROP Intermediate_Dir "NeuralNetwork/Release"

!ELSEIF  "$(CFG)" == "Melkor - Win32 Debug"

# PROP Intermediate_Dir "NeuralNetwork/Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\NeuralNetwork\newBPNGoAdapter.cpp

!IF  "$(CFG)" == "Melkor - Win32 Release"

# PROP Intermediate_Dir "NeuralNetwork/Release"

!ELSEIF  "$(CFG)" == "Melkor - Win32 Debug"

# PROP Intermediate_Dir "NeuralNetwork/Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\NeuralNetwork\NNDatabase.cpp

!IF  "$(CFG)" == "Melkor - Win32 Release"

# PROP Intermediate_Dir "NeuralNetwork/Release"

!ELSEIF  "$(CFG)" == "Melkor - Win32 Debug"

# PROP Intermediate_Dir "NeuralNetwork/Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\NeuralNetwork\RBF.cpp

!IF  "$(CFG)" == "Melkor - Win32 Release"

# PROP Intermediate_Dir "NeuralNetwork/Release"

!ELSEIF  "$(CFG)" == "Melkor - Win32 Debug"

# PROP Intermediate_Dir "NeuralNetwork/Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\NeuralNetwork\RPROPBPNTrainer.cpp

!IF  "$(CFG)" == "Melkor - Win32 Release"

# PROP Intermediate_Dir "NeuralNetwork/Release"

!ELSEIF  "$(CFG)" == "Melkor - Win32 Debug"

# PROP Intermediate_Dir "NeuralNetwork/Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\NeuralNetwork\Safety1BPNGoAdapter.cpp

!IF  "$(CFG)" == "Melkor - Win32 Release"

# PROP Intermediate_Dir "NeuralNetwork/Release"

!ELSEIF  "$(CFG)" == "Melkor - Win32 Debug"

# PROP Intermediate_Dir "NeuralNetwork/Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\NeuralNetwork\Urgency1GoAdapter.cpp

!IF  "$(CFG)" == "Melkor - Win32 Release"

# PROP Intermediate_Dir "NeuralNetwork/Release"

!ELSEIF  "$(CFG)" == "Melkor - Win32 Debug"

# PROP Intermediate_Dir "NeuralNetwork/Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\NeuralNetwork\Urgency3BPN.cpp

!IF  "$(CFG)" == "Melkor - Win32 Release"

# PROP Intermediate_Dir "NeuralNetwork/Release"

!ELSEIF  "$(CFG)" == "Melkor - Win32 Debug"

# PROP Intermediate_Dir "NeuralNetwork/Debug"

!ENDIF 

# End Source File
# End Group
# Begin Group "SGF"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\SGF\SGFGameInfo.cpp

!IF  "$(CFG)" == "Melkor - Win32 Release"

# PROP Intermediate_Dir "SGF/Release"

!ELSEIF  "$(CFG)" == "Melkor - Win32 Debug"

# PROP Intermediate_Dir "SGF/Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\SGF\SGFNode.cpp

!IF  "$(CFG)" == "Melkor - Win32 Release"

# PROP Intermediate_Dir "SGF/Release"

!ELSEIF  "$(CFG)" == "Melkor - Win32 Debug"

# PROP Intermediate_Dir "SGF/Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\SGF\SGFProperty.cpp

!IF  "$(CFG)" == "Melkor - Win32 Release"

# PROP Intermediate_Dir "SGF/Release"

!ELSEIF  "$(CFG)" == "Melkor - Win32 Debug"

# PROP Intermediate_Dir "SGF/Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\SGF\SGFReader.cpp

!IF  "$(CFG)" == "Melkor - Win32 Release"

# PROP Intermediate_Dir "SGF/Release"

!ELSEIF  "$(CFG)" == "Melkor - Win32 Debug"

# PROP Intermediate_Dir "SGF/Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\SGF\SGFTree.cpp

!IF  "$(CFG)" == "Melkor - Win32 Release"

# PROP Intermediate_Dir "SGF/Release"

!ELSEIF  "$(CFG)" == "Melkor - Win32 Debug"

# PROP Intermediate_Dir "SGF/Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\SGF\SGFWriter.cpp

!IF  "$(CFG)" == "Melkor - Win32 Release"

# PROP Intermediate_Dir "SGF/Release"

!ELSEIF  "$(CFG)" == "Melkor - Win32 Debug"

# PROP Intermediate_Dir "SGF/Debug"

!ENDIF 

# End Source File
# End Group
# Begin Group "Util"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Util\BoardStruct.cpp

!IF  "$(CFG)" == "Melkor - Win32 Release"

# PROP Intermediate_Dir "Util/Release"

!ELSEIF  "$(CFG)" == "Melkor - Win32 Debug"

# PROP Intermediate_Dir "Util/Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Util\Go.cpp

!IF  "$(CFG)" == "Melkor - Win32 Release"

# PROP Intermediate_Dir "Util/Release"

!ELSEIF  "$(CFG)" == "Melkor - Win32 Debug"

# PROP Intermediate_Dir "Util/Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Util\GoString.cpp

!IF  "$(CFG)" == "Melkor - Win32 Release"

# PROP Intermediate_Dir "Util/Release"

!ELSEIF  "$(CFG)" == "Melkor - Win32 Debug"

# PROP Intermediate_Dir "Util/Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Util\Misc.cpp

!IF  "$(CFG)" == "Melkor - Win32 Release"

# PROP Intermediate_Dir "Util/Release"

!ELSEIF  "$(CFG)" == "Melkor - Win32 Debug"

# PROP Intermediate_Dir "Util/Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Util\Move.cpp

!IF  "$(CFG)" == "Melkor - Win32 Release"

# PROP Intermediate_Dir "Util/Release"

!ELSEIF  "$(CFG)" == "Melkor - Win32 Debug"

# PROP Intermediate_Dir "Util/Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Util\Rect.cpp

!IF  "$(CFG)" == "Melkor - Win32 Release"

# PROP Intermediate_Dir "Util/Release"

!ELSEIF  "$(CFG)" == "Melkor - Win32 Debug"

# PROP Intermediate_Dir "Util/Debug"

!ENDIF 

# End Source File
# End Group
# Begin Group "Test"

# PROP Default_Filter ""
# Begin Group "Util No. 2"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Test\Util\BoardStructTest.cpp
# End Source File
# Begin Source File

SOURCE=.\Test\Util\GoStringTest.cpp
# End Source File
# Begin Source File

SOURCE=.\Test\Util\GoTest.cpp

!IF  "$(CFG)" == "Melkor - Win32 Release"

# PROP Intermediate_Dir "Test/Release"

!ELSEIF  "$(CFG)" == "Melkor - Win32 Debug"

# PROP Intermediate_Dir "Test/Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Test\Util\LinkedListTest.cpp
# End Source File
# Begin Source File

SOURCE=.\Test\Util\TreeNodeTest.cpp
# End Source File
# End Group
# Begin Group "Goal No. 2"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Test\Goal\GoalInterfaceTest.cpp
# End Source File
# End Group
# Begin Group "Patterns No. 1"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Test\Patterns\PatternTest.cpp
# End Source File
# Begin Source File

SOURCE=.\Test\Patterns\SpiralOrdererTest.cpp
# End Source File
# End Group
# Begin Source File

SOURCE=.\Test\AllLegalSuggesterTest.cpp
# End Source File
# Begin Source File

SOURCE=.\Test\LadderExtensionTest.cpp
# End Source File
# End Group
# Begin Group "Patterns No. 3"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Patterns\Pattern.cpp
# End Source File
# End Group
# Begin Source File

SOURCE=.\AllLegalSuggester.cpp
# End Source File
# Begin Source File

SOURCE=.\CaptureSuggester.cpp
# End Source File
# Begin Source File

SOURCE=.\GeneralSuggester.cpp
# End Source File
# Begin Source File

SOURCE=.\GTPFunctions.cpp
# End Source File
# Begin Source File

SOURCE=.\LadderSuggester.cpp
# End Source File
# Begin Source File

SOURCE=.\LogWriter.cpp
# End Source File
# Begin Source File

SOURCE=.\LooseTacticalSuggester.cpp
# End Source File
# Begin Source File

SOURCE=.\Main.cpp
# End Source File
# Begin Source File

SOURCE=.\Melkor.cpp
# End Source File
# Begin Source File

SOURCE=.\MTDFSearch.cpp
# End Source File
# Begin Source File

SOURCE=.\Node.cpp
# End Source File
# Begin Source File

SOURCE=.\SearchSettings.cpp
# End Source File
# Begin Source File

SOURCE=.\Test.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Group "Goal No. 1"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Goal\CaptureGoal.h
# End Source File
# Begin Source File

SOURCE=.\Goal\Goal.h
# End Source File
# Begin Source File

SOURCE=.\Goal\GoalFunctions.h
# End Source File
# Begin Source File

SOURCE=.\Goal\GoalInterface.h
# End Source File
# Begin Source File

SOURCE=.\Goal\InfluenceGoal.h
# End Source File
# Begin Source File

SOURCE=.\Goal\LadderGoal.h
# End Source File
# Begin Source File

SOURCE=.\Goal\LibCountCaptureGoal.h
# End Source File
# Begin Source File

SOURCE=.\Goal\LibertyCountGoal.h
# End Source File
# Begin Source File

SOURCE=.\Goal\LifeDeathGoal.h
# End Source File
# Begin Source File

SOURCE=.\Goal\LooseTacticalGoal.h
# End Source File
# Begin Source File

SOURCE=.\Goal\StoneCountGoal.h
# End Source File
# End Group
# Begin Group "NeuralNetwork No. 1"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\NeuralNetwork\BPN.h
# End Source File
# Begin Source File

SOURCE=.\NeuralNetwork\BPNExpAct.h
# End Source File
# Begin Source File

SOURCE=.\NeuralNetwork\BPNExpActGoAdapter.h
# End Source File
# Begin Source File

SOURCE=.\NeuralNetwork\BPNGoAdapter.h
# End Source File
# Begin Source File

SOURCE=.\NeuralNetwork\BPNGoAdapterInterface.h
# End Source File
# Begin Source File

SOURCE=.\NeuralNetwork\BPNTrainer.h
# End Source File
# Begin Source File

SOURCE=.\NeuralNetwork\CN.h
# End Source File
# Begin Source File

SOURCE=.\NeuralNetwork\CNGoAdapter.h
# End Source File
# Begin Source File

SOURCE=.\NeuralNetwork\NeuralNetwork.h
# End Source File
# Begin Source File

SOURCE=.\NeuralNetwork\newBPN10GoAdapter.h
# End Source File
# Begin Source File

SOURCE=.\NeuralNetwork\newBPN11GoAdapter.h
# End Source File
# Begin Source File

SOURCE=.\NeuralNetwork\newBPN12GoAdapter.h
# End Source File
# Begin Source File

SOURCE=.\NeuralNetwork\newBPN2GoAdapter.h
# End Source File
# Begin Source File

SOURCE=.\NeuralNetwork\newBPN3GoAdapter.h
# End Source File
# Begin Source File

SOURCE=.\NeuralNetwork\newBPN4GoAdapter.h
# End Source File
# Begin Source File

SOURCE=.\NeuralNetwork\newBPN5GoAdapter.h
# End Source File
# Begin Source File

SOURCE=.\NeuralNetwork\newBPN6GoAdapter.h
# End Source File
# Begin Source File

SOURCE=.\NeuralNetwork\newBPN7GoAdapter.h
# End Source File
# Begin Source File

SOURCE=.\NeuralNetwork\newBPN8GoAdapter.h
# End Source File
# Begin Source File

SOURCE=.\NeuralNetwork\newBPN9GoAdapter.h
# End Source File
# Begin Source File

SOURCE=.\NeuralNetwork\newBPNGoAdapter.h
# End Source File
# Begin Source File

SOURCE=.\NeuralNetwork\NNDatabase.h
# End Source File
# Begin Source File

SOURCE=.\NeuralNetwork\RBF.h
# End Source File
# Begin Source File

SOURCE=.\NeuralNetwork\RBFGoAdapter.h
# End Source File
# Begin Source File

SOURCE=.\NeuralNetwork\RPROPBPNTrainer.h
# End Source File
# Begin Source File

SOURCE=.\NeuralNetwork\Safety1BPNGoAdapter.h
# End Source File
# Begin Source File

SOURCE=.\NeuralNetwork\Urgency1GoAdapter.h
# End Source File
# Begin Source File

SOURCE=.\NeuralNetwork\Urgency3BPN.h
# End Source File
# Begin Source File

SOURCE=.\NeuralNetwork\Urgency3BPNGoAdapter.h
# End Source File
# End Group
# Begin Group "SGF No. 1"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\SGF\SGFGameInfo.h
# End Source File
# Begin Source File

SOURCE=.\SGF\SGFNode.h
# End Source File
# Begin Source File

SOURCE=.\SGF\SGFProperty.h
# End Source File
# Begin Source File

SOURCE=.\SGF\SGFReader.h
# End Source File
# Begin Source File

SOURCE=.\SGF\SGFTree.h
# End Source File
# Begin Source File

SOURCE=.\SGF\SGFWriter.h
# End Source File
# End Group
# Begin Group "Util No. 1"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Util\BMP.h
# End Source File
# Begin Source File

SOURCE=.\Util\BoardStruct.h
# End Source File
# Begin Source File

SOURCE=.\Util\globals.h
# End Source File
# Begin Source File

SOURCE=.\Util\Go.h
# End Source File
# Begin Source File

SOURCE=.\Util\GoString.h
# End Source File
# Begin Source File

SOURCE=.\Util\hash_map.h
# End Source File
# Begin Source File

SOURCE=.\Util\hash_set.h
# End Source File
# Begin Source File

SOURCE=.\Util\hash_shared.h
# End Source File
# Begin Source File

SOURCE=.\Util\Link.h
# End Source File
# Begin Source File

SOURCE=.\Util\LinkedList.h
# End Source File
# Begin Source File

SOURCE=.\Util\LogWriter.h
# End Source File
# Begin Source File

SOURCE=.\Util\Matrix.h
# End Source File
# Begin Source File

SOURCE=.\Util\memoryTracking.h
# End Source File
# Begin Source File

SOURCE=.\Util\Misc.h
# End Source File
# Begin Source File

SOURCE=.\Util\Move.h
# End Source File
# Begin Source File

SOURCE=.\Util\my_vector.h
# End Source File
# Begin Source File

SOURCE=.\Util\ObjectMatrix.h
# End Source File
# Begin Source File

SOURCE=.\Util\Point.h
# End Source File
# Begin Source File

SOURCE=.\Util\Rect.h
# End Source File
# Begin Source File

SOURCE=.\Util\SmartPtr.h
# End Source File
# Begin Source File

SOURCE=.\Util\SpecialPoint.h
# End Source File
# Begin Source File

SOURCE=.\Util\Tree.h
# End Source File
# Begin Source File

SOURCE=.\Util\TreeNode.h
# End Source File
# End Group
# Begin Group "Test No. 1"

# PROP Default_Filter ""
# Begin Group "Util No. 3"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Test\Util\BoardStructTest.h
# End Source File
# Begin Source File

SOURCE=.\Test\Util\GoStringTest.h
# End Source File
# Begin Source File

SOURCE=.\Test\Util\GoTest.h
# End Source File
# Begin Source File

SOURCE=.\Test\Util\LinkedListTest.h
# End Source File
# Begin Source File

SOURCE=.\Test\Util\TreeNodeTest.h
# End Source File
# End Group
# Begin Group "Goal No. 3"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Test\Goal\GoalInterfaceTest.h
# End Source File
# End Group
# Begin Group "Patterns No. 2"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Test\Patterns\PatternTest.h
# End Source File
# Begin Source File

SOURCE=.\Test\Patterns\SpiralOrdererTest.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\Test\AllLegalSuggesterTest.h
# End Source File
# Begin Source File

SOURCE=.\Test\LadderExtensionTest.h
# End Source File
# End Group
# Begin Group "Patterns"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Patterns\EyeShapePattern.h
# End Source File
# Begin Source File

SOURCE=.\Patterns\Orderer.h
# End Source File
# Begin Source File

SOURCE=.\Patterns\Pattern.h
# End Source File
# Begin Source File

SOURCE=.\Patterns\PatternMatcher.h
# End Source File
# Begin Source File

SOURCE=.\Patterns\PatternTree.h
# End Source File
# Begin Source File

SOURCE=.\Patterns\SpiralOrderer.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\AllLegalSuggester.h
# End Source File
# Begin Source File

SOURCE=.\CaptureSuggester.h
# End Source File
# Begin Source File

SOURCE=.\GeneralSuggester.h
# End Source File
# Begin Source File

SOURCE=.\GTPFunctions.h
# End Source File
# Begin Source File

SOURCE=.\LadderExtension.h
# End Source File
# Begin Source File

SOURCE=.\LadderSuggester.h
# End Source File
# Begin Source File

SOURCE=.\LooseTacticalSuggester.h
# End Source File
# Begin Source File

SOURCE=.\Main.h
# End Source File
# Begin Source File

SOURCE=.\Melkor.h
# End Source File
# Begin Source File

SOURCE=..\MelkorDLL\MelkorDLL.h
# End Source File
# Begin Source File

SOURCE=.\MoveSuggester.h
# End Source File
# Begin Source File

SOURCE=.\MTDFSearch.h
# End Source File
# Begin Source File

SOURCE=.\Node.h
# End Source File
# Begin Source File

SOURCE=.\SearchAlgorithm.h
# End Source File
# Begin Source File

SOURCE=.\SearchSettings.h
# End Source File
# Begin Source File

SOURCE=.\SelectiveSearchExtension.h
# End Source File
# Begin Source File

SOURCE=.\Test.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# End Group
# End Target
# End Project
