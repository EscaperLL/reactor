#include "stdafx.h"


#include "TsFileMediaSource.h"
#include"Guard.h"

#include "PrintLog.h"
#include <memory.h>

CTsFileMediaSource::CTsFileMediaSource(string strFileName)
	:m_strFileName(strFileName)
{

	if (m_file != NULL)
		fclose(m_file), m_file = NULL;
	m_file = fopen64(strFileName.c_str(), "rb");
	if (m_file == NULL) {

		//LogError("open file failed, filename:%s\n", filename);
		
	}
	if (init_file() < 0)
		
	LogInfo("file size:%lld, first pcr:%lld, last pcr:%lld\n", m_file_size, m_first_pcr, m_last_pcr);
}


CTsFileMediaSource::~CTsFileMediaSource()
{
	if (m_file != NULL)
		fclose(m_file), m_file = NULL;
}


int CTsFileMediaSource::GetFrameFromTsFile(char* buf, int len, uint64_t& pcr)
{
	int ret = fread(buf, sizeof(char), len, m_file);
	if (ret < TS_PKT_LEN) {
		LogError("read ts pkts failed\n");
		return -1;
	}

	uint64_t cur_pcr = -1;
	for (int i = 0; i < ret / TS_PKT_LEN; i++)
		CTsParser::GetPcr((const uint8_t*)buf + i*TS_PKT_LEN, cur_pcr);
	if (cur_pcr != (uint64_t)-1)
		m_cur_pcr = cur_pcr;
	pcr = m_cur_pcr;
	return ret;
}

void CTsFileMediaSource::readFrame()
{
	CGuard cuard(mMutex);
	
	if (mAVFrameInputQueue.empty())
	{
		return;
	}
	AVFrame* frame = mAVFrameInputQueue.front();
	uint64_t pcr = 0;
	frame->mFrameSize = GetFrameFromTsFile((char*)frame->mBuffer, 7 * TS_PKT_LEN, pcr);

	if (frame->mFrameSize < 0)
		return;
	mAVFrameInputQueue.pop();
	mAVFrameOutputQueue.push(frame);
}

int CTsFileMediaSource::init_file()
{
	m_file_size = 0;
	m_cur_pcr = -1;
	m_first_pcr = -1;
	m_last_pcr = -1;
	if (fseeko64(m_file, 0, SEEK_END) != 0) {
		LogError("seek to end failed\n");
		return -1;
	}
	m_file_size = ftello64(m_file);
	if (m_file_size < TS_PKT_LEN) {
		LogError("file is to small, size:%ld\n", m_file_size);
		return -1;
	}
	if (fseeko64(m_file, 0, SEEK_SET) != 0) {
		LogError("seek to begin failed\n");
		return -1;
	}
	int64_t offset = -1;
	while (1) {
		if (offset >= 0) {
			if (fseeko64(m_file, offset, SEEK_SET) == -1) {
				LogError("seek file to %lld failed\n", offset);
				return -1;
			}
		}
		uint8_t buf[TS_PKT_LEN];
		if (fread(buf, sizeof(uint8_t), TS_PKT_LEN, m_file) != TS_PKT_LEN) {
			LogError("read one ts pkt failed\n");
			return -1;
		}
		uint64_t pcr = 0;
		if (CTsParser::GetPcr(buf, pcr) < 0) {
			if (offset != -1)
				offset -= TS_PKT_LEN;
			continue;
		}
		if (m_first_pcr == (uint64_t)-1) {
			m_cur_pcr = m_first_pcr = pcr;
			offset = (m_file_size / TS_PKT_LEN - 1)*TS_PKT_LEN;
		}
		else {
			m_last_pcr = pcr;
			break;
		}
	}
	if (m_last_pcr <= m_first_pcr) {
		LogError("get file range failed\n");
		return -1;
	}
	if (fseeko64(m_file, 0, SEEK_SET) != 0) {
		LogError("seek to end failed\n");
		return -1;
	}
	return 0;
}