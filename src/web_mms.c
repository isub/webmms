#include "web_mms.h"

int main(int argc, char *argv[])
{
  int iRetVal = 0;
  char *pszFileName, *pszMMSDir, *pszWebDir;

  if(argc == 4) {
    pszMMSDir = argv[1];
    pszFileName = argv[2];
    pszWebDir = argv[3];
  } else {
    return 1;
  }

  iRetVal = create_web_data(pszFileName, pszMMSDir, pszWebDir);

  return iRetVal;
}
