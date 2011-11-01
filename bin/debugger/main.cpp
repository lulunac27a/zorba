/*
 * Copyright 2006-2008 The FLWOR Foundation.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 * 
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifdef WIN32
# include <windows.h>
# include <string.h>
# include <strsafe.h>
#endif

#include <vector>

#include <zorba/config.h>

#include "command_prompt.h"
#include "command_line_handler.h"

using namespace zorba;
using namespace zorba::debugger;

#ifdef WIN32
size_t ExecuteProcess(std::wstring aPathToExe, std::wstring aParameters, size_t SecondsToWait) 
{
  DWORD iReturnVal = 0;
  DWORD dwExitCode = 0;
  std::wstring lTempStr = L"";

  // Add a space to the beginning of the Parameters
  if (aParameters.size() != 0) {
    if (aParameters[0] != L' ') {
      aParameters.insert(0, L" ");
    }
  }

  // The first parameter needs to be the exe itself
  lTempStr = aPathToExe;
  std::wstring::size_type iPos = lTempStr.find_last_of(L"\\");
  lTempStr.erase(0, iPos + 1);
  aParameters = lTempStr.append(aParameters);

  // CreateProcessW can modify Parameters thus we allocate needed memory
  wchar_t * pwszParam = new wchar_t[aParameters.size() + 1];
  if (pwszParam == 0) {
    return 1;
  }
  const wchar_t* pchrTemp = aParameters.c_str();
  wcscpy_s(pwszParam, aParameters.size() + 1, pchrTemp); 

  // CreateProcess API initialization
  STARTUPINFOW siStartupInfo;
  PROCESS_INFORMATION piProcessInfo;
  memset(&siStartupInfo, 0, sizeof(siStartupInfo));
  memset(&piProcessInfo, 0, sizeof(piProcessInfo));
  siStartupInfo.cb = sizeof(siStartupInfo);

  BOOL lResult = CreateProcessW(
    const_cast<LPCWSTR>(aPathToExe.c_str()),
    pwszParam, 0, 0, false,
    CREATE_DEFAULT_ERROR_MODE, 0, 0,
    &siStartupInfo, &piProcessInfo);

  if (lResult) {
    // Watch the process
    dwExitCode = WaitForSingleObject(piProcessInfo.hProcess, (SecondsToWait * 1000));
  }
  else {
    // CreateProcess failed
    iReturnVal = GetLastError();
    LPVOID lpMsgBuf;
    LPVOID lpDisplayBuf;

    FormatMessage(
      FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
      NULL,
      iReturnVal,
      MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
      (LPTSTR) &lpMsgBuf,
      0, NULL);

    // Display the error message and exit the process

    lpDisplayBuf = (LPVOID)LocalAlloc(
      LMEM_ZEROINIT,
      (lstrlen((LPCTSTR)lpMsgBuf) + 40) * sizeof(TCHAR));

    StringCchPrintf(
      (LPTSTR)lpDisplayBuf,
      LocalSize(lpDisplayBuf) / sizeof(TCHAR),
      TEXT("Error (%d) when starting zorba: %s"),
      iReturnVal,
      lpMsgBuf);

    std::wstring lErrorW((wchar_t*)lpDisplayBuf);
    std::string lError;
    lError.assign(lErrorW.begin(), lErrorW.end());
    std::cout << lError << std::endl;

    LocalFree(lpMsgBuf);
    LocalFree(lpDisplayBuf);
  }

  // Free memory
  delete[]pwszParam;
  pwszParam = 0;

  // Release handles
  CloseHandle(piProcessInfo.hProcess);
  CloseHandle(piProcessInfo.hThread);

  return iReturnVal; 
}
#else

// add execute process in Linux

#endif


int startZorba(std::string& aExec, std::vector<std::string>& aArgs)
{
#ifdef WIN32
  std::wstring lExecW, lParamsW;
  lExecW.assign(aExec.begin(), aExec.end());

  for (std::vector<std::string>::size_type j = 0; j < aArgs.size(); j++) {
    std::string lArg(aArgs.at(j));
    std::wstring lArgW;
    lArgW.assign(lArg.begin(), lArg.end());
    lParamsW.append(lArgW);
    lParamsW.append(L" ");
  }

  int lResult = ExecuteProcess(lExecW, lParamsW, 0);
  return lResult;
#else
  pid_t pID = fork();
  if (pID == 0) {
    // Code only executed by child process
    std::stringstream lCommand;
    lCommand << aExec;
    for (std::vector<std::string>::size_type j = 0; j < aArgs.size(); j++) {
      lCommand << " " << aArgs.at(j);
    }

    int lRes = system(lCommand.str().c_str());
    exit(lRes);
  }
  else if (pID < 0) {
    std::cerr << "Failed to fork Zorba" << std::endl;
    exit(1);
  }
  else {
    // Code only executed by parent process
    return 0;
  }
#endif
}

void printUsage(std::string& aProgram)
{
  std::cerr << "Usage:" << std::endl
    << "    " << aProgram << " <zorba_executable> <zorba_arguments>" << std::endl
    << "        the debugger will start the given Zorba executable with the given arguments" << std::endl
    << std::endl
    << "    " << aProgram << " [-p PORT]" << std::endl
    << "        the debugger will start standalone and wait for an incomming connection from Zorba" << std::endl;
}

bool processArguments(int argc, char* argv[], std::string& aProgram, std::string& aZorba, unsigned int& aPort, std::vector<std::string>& aZorbaArgs)
{
  // we will need the program name in usage info
  aPort = 28028;
  aProgram = argv[0];
  std::string::size_type lPos = aProgram.find_last_of(
#ifdef WIN32
    '\\'
#else
    '/'
#endif
  );
  aProgram = aProgram.substr(lPos + 1);

  for (int i = 1; i < argc; i++) {
    std::string lArg = argv[i];
    if (lArg == "-h" || lArg == "--help") {
      return false;
    } else if (lArg == "-p") {
      ++i;
      if (i == argc) {
        std::cerr << "Missing value for -p option." << std::endl;
        return false;
      }
    } else if (lArg.at(0) == '-'){
      std::cerr << "Invalid option \"" << lArg << "\"" << std::endl;
      return false;
    }
  }

  if (argc < 5) {
    std::cout << "Not enough arguments to start Zorba." << std::endl;
    std::cout << "Running the standalone XQuery debugger client on port: " << aPort << std::endl;
    return true;
  }

  // get the zorba executable
  aZorba = argv[1];

  // zorba will need the -d flag
  aZorbaArgs.push_back("-d");

  // gather all arguments (excepting the program name)
  for (int i = 2; i < argc; i++) {
    std::string lArg = argv[i];

    // read the port option
    if (lArg == "-p" || lArg == "--debug-port") {
      // if there is one more argument
      if (i < argc - 1) {
        // get the port value
        int lPort;
        std::stringstream lStream(argv[i + 1]);
        lStream >> lPort;
        if (!lStream.fail()) {
          aPort = lPort;
        }
      }
    }

    // add to Zorba arguments
    aZorbaArgs.push_back(lArg);
  }

  return true;
}

#ifndef _WIN32_WCE
int
main(int argc, char* argv[])
#else
int
_tmain(int argc, _TCHAR* argv[])
#endif
{
  // **************************************************************************
  // processing arguments

  std::string lProgram, lZorbaExec;
  unsigned int lPort = 28028;
  std::vector<std::string> lZorbaArgs;

  if (!processArguments(argc, argv, lProgram, lZorbaExec, lPort, lZorbaArgs)) {
    printUsage(lProgram);
    return 1;
  }

  bool lStandalone = false;

  if (lZorbaExec == "") {
    lStandalone = true;
  }

#ifndef NDEBUG
  // **************************************************************************
  // debug reporting

  if (!lStandalone) {
    std::cout << "Communication port: " << lPort << std::endl;
    std::cout << "Zorba executable:   " << lZorbaExec << std::endl;
    std::cout << "Zorba arguments:   ";
    for (std::vector<std::string>::size_type j = 0; j < lZorbaArgs.size(); j++) {
      std::cout << " " << lZorbaArgs.at(j);
    }
    std::cout << std::endl;
  }
#endif

  // **************************************************************************
  // processing arguments

  try {
    LockFreeQueue<std::size_t> lQueue;
    LockFreeQueue<bool> lContEvent;
    EventHandler lEventHandler(lQueue, lContEvent);
    lEventHandler.init();

    CommandPrompt lCommandPrompt;
    CommandLineHandler lCommandLineHandler(lPort, lQueue, lContEvent, lEventHandler, lCommandPrompt);


    // **************************************************************************
    // start a zorba

    if (!lStandalone) {
      int lResult = startZorba(lZorbaExec, lZorbaArgs);
      if (lResult) {
        return lResult;
      }
    } else {
      std::cout << "Waiting for an incomming Zorba connection..." << std::endl;
    }

    // **************************************************************************
    // start the debugger command line

    lCommandLineHandler.execute();

#ifndef WIN32
    std::cout << "Waiting...";
    wait();
    std::cout << "Done!";
#endif

  } catch (...) {
    return -1;
  }

  return 0;
}
