#pragma once

#include"MediaSource.h"

#include "Def.h"
#include <stdio.h>
#include "TsParser.h"
class CTsFileMediaSource :public MediaSource
{
public:
	CTsFileMediaSource(string strFileName);
	~CTsFileMediaSource();

protected:
	virtual void readFrame();

private:
	int GetFrameFromTsFile(char* buf, int len, uint64_t& pcr);
	int init_file();
private:

	string m_strFileName;
	FILE* m_file;
	char m_data_buf[TS_PKT_LEN];
	uint64_t m_file_size;
	uint64_t m_cur_pcr;
	uint64_t m_first_pcr;
	uint64_t m_last_pcr;
};

