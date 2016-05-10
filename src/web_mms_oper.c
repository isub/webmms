#include "web_mms.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>

int operate_single_mime(Octstr *p_psoHdrs, MIMEEntity *p_psoMIME, const char *p_pszWebDir);

int create_web_data(char *p_pszFileName, char *p_pszMMSDir, const char *p_pszWebDir)
{
  int iRetVal = 0;
  MmsEnvelope *psoEnv;
  MmsMsg *psoMsg = NULL;
  List *psoHdrList = NULL;
  Octstr *psoHdrs = NULL, *psoHdrName = NULL, *psoHdrValue = NULL, *psoWebDir = NULL;
  MIMEEntity *psoMIME, *psoMIMETmp;
  int iMIMENum, iMIMEInd;
  int iFile = -1;

  gwlib_init();
  mms_strings_init();

  /* формируем имя директрии для вывода результатов */
  psoWebDir = octstr_create(p_pszWebDir);
  if('/' != octstr_get_char(psoWebDir, octstr_len(psoWebDir) - 1)) {
    octstr_append_char(psoWebDir, '/');
  }
  octstr_append_cstr(psoWebDir, p_pszFileName);

  /* создаем директорию */
  if(mkdir(octstr_get_cstr(psoWebDir), 0777)) {
    if(errno != EEXIST) {
      iRetVal = 2;
    }
    goto done;
  }

  psoEnv = default_qfuncs.mms_queue_readenvelope(p_pszFileName, p_pszMMSDir, 0);
  if(psoEnv) {
    psoMsg = default_qfuncs.mms_queue_getdata(psoEnv);
    default_qfuncs.mms_queue_free_env(psoEnv);
  } else {
    iRetVal = 3;
    goto done;
  }

  if(!psoMsg) {
    iRetVal = 4;
    goto done;
  }

  psoHdrs = octstr_create("");
  /* выбираем необходимые заголовки */
  psoHdrList = mms_message_headers(psoMsg);
  /* From */
  psoHdrName = octstr_create("From");
  psoHdrValue = http_header_value(psoHdrList, psoHdrName);
  octstr_format_append(psoHdrs, "%S/%S\r\n", psoHdrName, psoHdrValue);
  octstr_truncate(psoHdrName, 0);
  /* To */
  octstr_append_cstr(psoHdrName, "To");
  psoHdrValue = http_header_value(psoHdrList, psoHdrName);
  octstr_format_append(psoHdrs, "%S/%S\r\n", psoHdrName, psoHdrValue);
  octstr_truncate(psoHdrName, 0);
  /* Date */
  octstr_append_cstr(psoHdrName, "Date");
  psoHdrValue = http_header_value(psoHdrList, psoHdrName);
  octstr_format_append(psoHdrs, "%S/%S", psoHdrName, psoHdrValue);
  octstr_truncate(psoHdrName, 0);
  /**/

  psoMIME = mms_tomime(psoMsg, 1);
  iMIMENum = mime_entity_num_parts(psoMIME);
  if(psoMIME) {
    for(iMIMEInd = 0; iMIMEInd < iMIMENum; ++iMIMEInd) {
      psoMIMETmp = mime_entity_get_part(psoMIME, iMIMEInd);
      operate_single_mime(psoHdrs, psoMIMETmp, octstr_get_cstr(psoWebDir));
    }
  }

  /* формируем имя файла для вывода результатов, используем ту же переменную, что и для директории */
  if('/' != octstr_get_char(psoWebDir, octstr_len(psoWebDir) - 1)) {
    octstr_append_char(psoWebDir, '/');
  }
  octstr_append_cstr(psoWebDir, "common");
  /* создаем файл */
  iFile = open(octstr_get_cstr(psoWebDir), O_CREAT | O_EXCL | O_WRONLY, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
  if(0 < iFile) {
    if(octstr_len(psoHdrs) != write(iFile, octstr_get_cstr(psoHdrs), octstr_len(psoHdrs))) {
      iRetVal = 5;
    }
    close(iFile);
  } else {
    if(errno != EEXIST) {
      iRetVal = 6;
    }
  }

  done:
  if(psoHdrs) {
    octstr_destroy(psoHdrs);
  }
  if(psoHdrName) {
    octstr_destroy(psoHdrName);
  }
  if(psoWebDir) {
    octstr_destroy(psoWebDir);
  }

  mms_strings_shutdown();
  gwlib_shutdown();

  return iRetVal;
}

int operate_single_mime(Octstr *p_psoHdrs, MIMEEntity *p_psoMIME, const char *p_pszWebDir)
{
  int iRetVal = 0;
  Octstr *psoHdrName = NULL, *psoHdrValue = NULL;
  Octstr *psoMIMEOStr = NULL, *psoFileName = NULL;
  List *psoHdrList = NULL;
  int iFile = -1;

  if(psoMIMEOStr) {
    octstr_destroy(psoMIMEOStr);
  }
  /* выбираем необходимые заголовки */
  psoHdrList = mime_entity_headers(p_psoMIME);
  /* Content-type */
  psoHdrName = octstr_create("Content-type");
  psoHdrValue = http_header_value(psoHdrList, psoHdrName);
  if(0 == strncasecmp("application/smil", octstr_get_cstr(psoHdrValue), 16)) {
    goto done;
  }
  octstr_format_append(p_psoHdrs, "\r\n");
  octstr_format_append(p_psoHdrs, "\r\n%S;%S", psoHdrName, psoHdrValue);
  octstr_truncate(psoHdrName, 0);
  /* Content-location */
  octstr_append_cstr(psoHdrName, "Content-location");
  psoHdrValue = http_header_value(psoHdrList, psoHdrName);
  octstr_format_append(p_psoHdrs, "\r\n%S;%S", psoHdrName, psoHdrValue);
  octstr_truncate(psoHdrName, 0);
  /**/

  /* формируем имя файла */
  psoFileName = octstr_create(p_pszWebDir);
  if('/' != octstr_get_char(psoFileName, octstr_len(psoFileName) - 1)) {
    octstr_append_char(psoFileName, '/');
  }
  octstr_append(psoFileName, psoHdrValue);
  /* создаем файл */
  iFile = open(octstr_get_cstr(psoFileName), O_CREAT | O_EXCL | O_WRONLY, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
  if(0 > iFile) {
    if(errno != EEXIST) {
      iRetVal = 7;
      goto done;
    }
  }
  psoMIMEOStr = mime_entity_body(p_psoMIME);
  octstr_base64_to_binary(psoMIMEOStr);
  if(octstr_len(psoMIMEOStr) != write(iFile, octstr_get_cstr(psoMIMEOStr), octstr_len(psoMIMEOStr))) {
    iRetVal = 8;
    goto done;
  }

  done:
  if(psoHdrName) {
    octstr_destroy(psoHdrName);
  }
  if(psoFileName) {
    octstr_destroy(psoFileName);
  }
  if(0 < iFile) {
    close(iFile);
  }

  return iRetVal;
}
