#include <stdio.h>

#include "mmlib/mms_queue.h"

int main(int argc, char *argv[])
{
  char *pszFileName, *pszDir;
  MmsEnvelope *psoEnv;
  MmsMsg *psoMsg = NULL;
  Octstr *psoMIMEOStr = NULL;
  MIMEEntity *psoMIME, *psoMIMETmp;
  int iMIMENum;
  int iInd;

  if(argc == 3) {
    pszDir = argv[1];
    pszFileName = argv[2];
  } else {
    return 1;
  }

  gwlib_init();
  mms_strings_init();

  psoEnv = default_qfuncs.mms_queue_readenvelope(pszFileName, pszDir, 0);
  if(psoEnv) {
    psoMsg = default_qfuncs.mms_queue_getdata(psoEnv);
    default_qfuncs.mms_queue_free_env(psoEnv);
  } else {
    return -1;
  }

  if(!psoMsg) {
    return -1;
  }

  fprintf(stdout, "message type: %d\n", mms_messagetype(psoMsg));
  fprintf(stdout, "message encoding: ");
  switch(mms_message_enc(psoMsg)) {
    case MS_1_1:
      fprintf(stdout, "MS_1_1");
      break;
    case MS_1_2:
      fprintf(stdout, "MS_1_2");
      break;
    case MM7_5:
      fprintf(stdout, "MM7_5");
      break;
  }
  fprintf(stdout, "\n");
  psoMIME = mms_tomime(psoMsg, 1);
  if(psoMIME) {
    iMIMENum = mime_entity_num_parts(psoMIME);
    fprintf(stdout, "MIME parts number: %d\n", iMIMENum);
    for(iInd = 0; iInd < iMIMENum; ++iInd) {
      psoMIMETmp = mime_entity_get_part(psoMIME, iInd);
      if(psoMIMEOStr) {
        octstr_destroy(psoMIMEOStr);
      }
      psoMIMEOStr = mime_entity_to_octstr(psoMIMETmp);
      fprintf(stdout, "MIME #%d: %s\n", iInd, octstr_get_cstr(psoMIMEOStr));
    }
  }
  done:

  mms_strings_shutdown();
  gwlib_shutdown();

  return 0;
}
