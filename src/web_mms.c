#include <stdio.h>

#include "mmlib/mms_queue.h"

int main(int argc, char *argv[])
{
  const char *pszFileName = NULL;
  MmsMsg *psoMsg = NULL;
  Octstr *psoBinMsg, *psoMIMEOStr;
  MIMEEntity *psoMIME, *psoMIMETmp;
  int iMIMENum;
  int iInd;

  gwlib_init();

  if(argc == 2) {
    pszFileName = argv[1];
  } else {
    return 1;
  }

  psoBinMsg = octstr_read_file(pszFileName);
  if(!psoBinMsg) {
    fprintf(stdout, "can not to read file\n");
    return -1;
  }
  psoMsg = mms_frombinary(psoBinMsg, octstr_imm(""));
  if(psoMsg) {
    fprintf(stdout, "mms was converted successfully\n");
  } else {
    goto done;
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
      psoMIMEOStr = mime_entity_to_octstr(psoMIMETmp);
      fprintf(stdout, "MIME #%d: %s\n", iInd, octstr_get_cstr(psoMIMEOStr));
    }
  }
  done:
  octstr_destroy(psoBinMsg);

  gwlib_shutdown();

  return 0;
}
