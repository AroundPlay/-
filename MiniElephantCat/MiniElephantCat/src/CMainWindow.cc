#include "CMainWindow.h"

#include <string>

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
#include <libavutil/imgutils.h>
}

#include "libavformat/avformat.h"

#include "CCommandLineHelper.h"

#include "encode_helper.h"



static wchar_t kPlayer[] = L"--Player";
static wchar_t kExit[] = L"/Exit";

#define VIDEOTIMEOUT 1000
#define VIDEOTIMERID 1

CMainWindow::CMainWindow()
{
}

CMainWindow::~CMainWindow()
{
}

void CMainWindow::Player()
{
	do
	{
		if (!CCommandLineHelper::getInstance()->hasOption(kPlayer))
		{
			break;
		}
		AVFormatContext* pFormatCtx = avformat_alloc_context();
		if (avformat_open_input(&pFormatCtx, encode_helper::Unicode2Utf8(CP_ACP, CCommandLineHelper::getInstance()->getOption(kPlayer)).c_str(), nullptr, nullptr) != 0) {
			// 打开文件失败
			break;
		}

		if (avformat_find_stream_info(pFormatCtx, nullptr) < 0) {
			// 找不到流信息
			break;
		}

		// 查找视频流
		int videoStreamIndex = -1;
		for (unsigned i = 0; i < pFormatCtx->nb_streams; i++) {
			if (pFormatCtx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
				videoStreamIndex = i;
				break;
			}
		}

		if (videoStreamIndex == -1) {
			// 没有找到视频流
			break;
		}

		// 获取视频流的解码器上下文
		AVCodecParameters* pCodecPar = pFormatCtx->streams[videoStreamIndex]->codecpar;
		const AVCodec* pCodec = avcodec_find_decoder(pCodecPar->codec_id);
		if (!pCodec) {
			// 没有找到解码器
			break;
		}

		AVCodecContext* pCodecCtx = avcodec_alloc_context3(pCodec);
		if (avcodec_parameters_to_context(pCodecCtx, pCodecPar) < 0) {
			break;
		}

		if (avcodec_open2(pCodecCtx, pCodec, nullptr) < 0) {
			break;
		}

		HDC hdc = GetDC();  // hWnd 是你的窗口句柄
		BITMAPINFO bmi = { 0 };
		bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
		bmi.bmiHeader.biWidth = pCodecCtx->width;
		bmi.bmiHeader.biHeight = -pCodecCtx->height;  // 注意，负数表示从上到下绘制
		bmi.bmiHeader.biPlanes = 1;
		bmi.bmiHeader.biBitCount = 24;
		bmi.bmiHeader.biCompression = BI_RGB;

		AVFrame* pFrame = av_frame_alloc();
		AVFrame* pFrameRGB = av_frame_alloc();
		uint8_t* buffer = (uint8_t*)av_malloc(av_image_get_buffer_size(AV_PIX_FMT_RGB24, pCodecCtx->width, pCodecCtx->height, 1));
		av_image_fill_arrays(pFrameRGB->data, pFrameRGB->linesize, buffer, AV_PIX_FMT_RGB24, pCodecCtx->width, pCodecCtx->height, 1);

		// 初始化用于转换像素格式的结构体
		struct SwsContext* swsCtx = sws_getContext(pCodecCtx->width, pCodecCtx->height, pCodecCtx->pix_fmt,
			pCodecCtx->width, pCodecCtx->height, AV_PIX_FMT_RGB24,
			SWS_BILINEAR, nullptr, nullptr, nullptr);

		AVPacket packet;
		while (av_read_frame(pFormatCtx, &packet) >= 0) {
			if (packet.stream_index == videoStreamIndex) {
				if (avcodec_send_packet(pCodecCtx, &packet) == 0) {
					while (avcodec_receive_frame(pCodecCtx, pFrame) == 0) {
						// 转换为RGB格式
						sws_scale(swsCtx, pFrame->data, pFrame->linesize, 0, pCodecCtx->height, pFrameRGB->data, pFrameRGB->linesize);

						// 显示图像
						StretchDIBits(hdc, 0, 0, pCodecCtx->width, pCodecCtx->height, 0, 0, pCodecCtx->width, pCodecCtx->height,
							pFrameRGB->data[0], &bmi, DIB_RGB_COLORS, SRCCOPY);
					}
				}
			}
			av_packet_unref(&packet);
		}

		// 清理
		sws_freeContext(swsCtx);
		av_frame_free(&pFrame);
		av_frame_free(&pFrameRGB);
		av_free(buffer);
		avcodec_free_context(&pCodecCtx);
		avformat_close_input(&pFormatCtx);
		ReleaseDC(hdc);
	} while (false);
}

LRESULT CMainWindow::OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& hHandled)
{
	HWND hProgman = FindWindow(L"Progman", L"Program Manager");
	if (hProgman)
	{
		SetParent(hProgman);
		DWORD_PTR dResult = 0;
		SendMessageTimeout(hProgman, 0x052C, 0, NULL, SMTO_NORMAL, 1000, &dResult);
	}
	return LRESULT();
}

BOOL CALLBACK EnumWinProc(HWND hwnd, LPARAM lParam)
{
	HWND hDefView = FindWindowEx(hwnd, 0, L"SHELLDLL_DefView", 0);
	if (hDefView != 0) {
		HWND hWorkerw = FindWindowEx(0, hwnd, L"WorkerW", 0);
		if (lParam == 0)
		{
			ShowWindow(hWorkerw, SW_HIDE);
			RedrawWindow(hWorkerw, NULL, NULL, RDW_INVALIDATE | RDW_UPDATENOW | RDW_ERASE);
		}
		else
		{
			ShowWindow(hWorkerw, SW_SHOW);
			RedrawWindow(hWorkerw, NULL, NULL, RDW_INVALIDATE | RDW_UPDATENOW | RDW_ERASE);

		}
		return FALSE;
	}
	return TRUE;
}

LRESULT CMainWindow::OnCopyData(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& hHandled)
{
	PCOPYDATASTRUCT datas = reinterpret_cast<PCOPYDATASTRUCT>(lParam);

	if (datas->dwData == 0)
	{
		std::wstring commandLine = std::wstring(reinterpret_cast<wchar_t*> (datas->lpData), datas->cbData / sizeof(wchar_t));
		CCommandLineHelper helper(commandLine);
		if (helper.hasOption(kExit))
		{
			EnumWindows(EnumWinProc, 1);
			PostMessage(WM_CLOSE, 0, 0);
		}
		else if (helper.hasOption(kPlayer))
		{
			EnumWindows(EnumWinProc, 0);
			SetWindowPos(HWND_TOP, 0, 0, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN), SWP_SHOWWINDOW);

			CCommandLineHelper::addOption(kPlayer, helper.getOption(kPlayer));
			KillTimer(VIDEOTIMERID);
			SetTimer(VIDEOTIMERID, VIDEOTIMEOUT);
		}
	}
	return LRESULT();
}

LRESULT CMainWindow::OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& hHandled)
{
	KillTimer(VIDEOTIMERID);
	if (playerThread_.joinable())
	{
		playerThread_.join();
	}
	PostQuitMessage(0);
	return LRESULT();
}

LRESULT CMainWindow::OnTimeOut(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& hHandled)
{
	if (VIDEOTIMERID == wParam)
	{
		KillTimer(VIDEOTIMERID);
		if (CCommandLineHelper::getInstance()->hasOption(kPlayer))
		{
			if (playerThread_.joinable())
			{
				playerThread_.join();
			}

			auto tmp = std::thread(&CMainWindow::Player, this);
			playerThread_.swap(tmp);
			SetTimer(VIDEOTIMERID, VIDEOTIMEOUT);
		}
	}
	return LRESULT();
}
