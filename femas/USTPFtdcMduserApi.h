/////////////////////////////////////////////////////////////////////////
///@system ·ç¿ØÇ°ÖÃÏµÍ³
///@company ÉÏº£½ðÈÚÆÚ»õÐÅÏ¢¼¼ÊõÓÐÏÞ¹«Ë¾
///@file USTPFtdcMduserApi.h
///@brief ¶¨ÒåÁË¿Í»§¶Ë½Ó¿Ú
///@history 
///20130520	ÐìÖÒ»ª	´´½¨¸ÃÎÄ¼þ
/////////////////////////////////////////////////////////////////////////

#if !defined(USTP_FTDCMDUSERAPI_H)
#define USTP_FTDCMDUSERAPI_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "USTPFtdcUserApiStruct.h"

#if defined(ISLIB) && defined(WIN32)
#ifdef LIB_MDUSER_API_EXPORT
#define MDUSER_API_EXPORT __declspec(dllexport)
#else
#define MDUSER_API_EXPORT __declspec(dllimport)
#endif
#else
#define MDUSER_API_EXPORT 
#endif

class CUstpFtdcMduserSpi
{
public:
	///µ±¿Í»§¶ËÓë½»Ò×ºóÌ¨½¨Á¢ÆðÍ¨ÐÅÁ¬½ÓÊ±£¨»¹Î´µÇÂ¼Ç°£©£¬¸Ã·½·¨±»µ÷ÓÃ¡£
	virtual void OnFrontConnected(){};
	
	///µ±¿Í»§¶ËÓë½»Ò×ºóÌ¨Í¨ÐÅÁ¬½Ó¶Ï¿ªÊ±£¬¸Ã·½·¨±»µ÷ÓÃ¡£µ±·¢ÉúÕâ¸öÇé¿öºó£¬API»á×Ô¶¯ÖØÐÂÁ¬½Ó£¬¿Í»§¶Ë¿É²»×ö´¦Àí¡£
	///@param nReason ´íÎóÔ­Òò
	///        0x1001 ÍøÂç¶ÁÊ§°Ü
	///        0x1002 ÍøÂçÐ´Ê§°Ü
	///        0x2001 ½ÓÊÕÐÄÌø³¬Ê±
	///        0x2002 ·¢ËÍÐÄÌøÊ§°Ü
	///        0x2003 ÊÕµ½´íÎó±¨ÎÄ
	virtual void OnFrontDisconnected(int nReason){};
		
	///ÐÄÌø³¬Ê±¾¯¸æ¡£µ±³¤Ê±¼äÎ´ÊÕµ½±¨ÎÄÊ±£¬¸Ã·½·¨±»µ÷ÓÃ¡£
	///@param nTimeLapse ¾àÀëÉÏ´Î½ÓÊÕ±¨ÎÄµÄÊ±¼ä
	virtual void OnHeartBeatWarning(int nTimeLapse){};
	
	///±¨ÎÄ»Øµ÷¿ªÊ¼Í¨Öª¡£µ±APIÊÕµ½Ò»¸ö±¨ÎÄºó£¬Ê×ÏÈµ÷ÓÃ±¾·½·¨£¬È»ºóÊÇ¸÷Êý¾ÝÓòµÄ»Øµ÷£¬×îºóÊÇ±¨ÎÄ»Øµ÷½áÊøÍ¨Öª¡£
	///@param nTopicID Ö÷Ìâ´úÂë£¨ÈçË½ÓÐÁ÷¡¢¹«¹²Á÷¡¢ÐÐÇéÁ÷µÈ£©
	///@param nSequenceNo ±¨ÎÄÐòºÅ
	virtual void OnPackageStart(int nTopicID, int nSequenceNo){};
	
	///±¨ÎÄ»Øµ÷½áÊøÍ¨Öª¡£µ±APIÊÕµ½Ò»¸ö±¨ÎÄºó£¬Ê×ÏÈµ÷ÓÃ±¨ÎÄ»Øµ÷¿ªÊ¼Í¨Öª£¬È»ºóÊÇ¸÷Êý¾ÝÓòµÄ»Øµ÷£¬×îºóµ÷ÓÃ±¾·½·¨¡£
	///@param nTopicID Ö÷Ìâ´úÂë£¨ÈçË½ÓÐÁ÷¡¢¹«¹²Á÷¡¢ÐÐÇéÁ÷µÈ£©
	///@param nSequenceNo ±¨ÎÄÐòºÅ
	virtual void OnPackageEnd(int nTopicID, int nSequenceNo){};


	///´íÎóÓ¦´ð
	virtual void OnRspError(CUstpFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {};

	///·ç¿ØÇ°ÖÃÏµÍ³ÓÃ»§µÇÂ¼Ó¦´ð
	virtual void OnRspUserLogin(CUstpFtdcRspUserLoginField *pRspUserLogin, CUstpFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {};

	///ÓÃ»§ÍË³öÓ¦´ð
	virtual void OnRspUserLogout(CUstpFtdcRspUserLogoutField *pRspUserLogout, CUstpFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {};

	///ÓÃ»§ÃÜÂëÐÞ¸ÄÓ¦´ð
	virtual void OnRspUserPasswordUpdate(CUstpFtdcUserPasswordUpdateField *pUserPasswordUpdate, CUstpFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {};

	///¶©ÔÄÖ÷ÌâÓ¦´ð
	virtual void OnRspSubscribeTopic(CUstpFtdcDisseminationField *pDissemination, CUstpFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {};

	///Ö÷Ìâ²éÑ¯Ó¦´ð
	virtual void OnRspQryTopic(CUstpFtdcDisseminationField *pDissemination, CUstpFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {};

	///Éî¶ÈÐÐÇéÍ¨Öª
	virtual void OnRtnDepthMarketData(CUstpFtdcDepthMarketDataField *pDepthMarketData) {};

	///¶©ÔÄºÏÔ¼µÄÏà¹ØÐÅÏ¢
	virtual void OnRspSubMarketData(CUstpFtdcSpecificInstrumentField *pSpecificInstrument, CUstpFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {};

	///ÍË¶©ºÏÔ¼µÄÏà¹ØÐÅÏ¢
	virtual void OnRspUnSubMarketData(CUstpFtdcSpecificInstrumentField *pSpecificInstrument, CUstpFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {};
};

class MDUSER_API_EXPORT CUstpFtdcMduserApi
{
public:
	///´´½¨MduserApi
	///@param pszFlowPath ´æÖü¶©ÔÄÐÅÏ¢ÎÄ¼þµÄÄ¿Â¼£¬Ä¬ÈÏÎªµ±Ç°Ä¿Â¼
	///@return ´´½¨³öµÄUserApi
	static CUstpFtdcMduserApi *CreateFtdcMduserApi(const char *pszFlowPath = "");
	
	///»ñÈ¡ÏµÍ³°æ±¾ºÅ
	///@param nMajorVersion Ö÷°æ±¾ºÅ
	///@param nMinorVersion ×Ó°æ±¾ºÅ
	///@return ÏµÍ³±êÊ¶×Ö·û´®
	static const char *GetVersion(int &nMajorVersion, int &nMinorVersion);
	
	///É¾³ý½Ó¿Ú¶ÔÏó±¾Éí
	///@remark ²»ÔÙÊ¹ÓÃ±¾½Ó¿Ú¶ÔÏóÊ±,µ÷ÓÃ¸Ãº¯ÊýÉ¾³ý½Ó¿Ú¶ÔÏó
	virtual void Release() = 0;
	
	///³õÊ¼»¯
	///@remark ³õÊ¼»¯ÔËÐÐ»·¾³,Ö»ÓÐµ÷ÓÃºó,½Ó¿Ú²Å¿ªÊ¼¹¤×÷
	virtual void Init() = 0;
	
	///µÈ´ý½Ó¿ÚÏß³Ì½áÊøÔËÐÐ
	///@return Ïß³ÌÍË³ö´úÂë
	virtual int Join() = 0;
	
	///»ñÈ¡µ±Ç°½»Ò×ÈÕ
	///@retrun »ñÈ¡µ½µÄ½»Ò×ÈÕ
	///@remark Ö»ÓÐµÇÂ¼³É¹¦ºó,²ÅÄÜµÃµ½ÕýÈ·µÄ½»Ò×ÈÕ
	virtual const char *GetTradingDay() = 0;
	
	///×¢²áÇ°ÖÃ»úÍøÂçµØÖ·
	///@param pszFrontAddress£ºÇ°ÖÃ»úÍøÂçµØÖ·¡£
	///@remark ÍøÂçµØÖ·µÄ¸ñÊ½Îª£º¡°protocol://ipaddress:port¡±£¬Èç£º¡±tcp://127.0.0.1:17001¡±¡£ 
	///@remark ¡°tcp¡±´ú±í´«ÊäÐ­Òé£¬¡°127.0.0.1¡±´ú±í·þÎñÆ÷µØÖ·¡£¡±17001¡±´ú±í·þÎñÆ÷¶Ë¿ÚºÅ¡£
	virtual void RegisterFront(char *pszFrontAddress) = 0;
	
	///×¢²áÃû×Ö·þÎñÆ÷ÍøÂçµØÖ·
	///@param pszNsAddress£ºÃû×Ö·þÎñÆ÷ÍøÂçµØÖ·¡£
	///@remark ÍøÂçµØÖ·µÄ¸ñÊ½Îª£º¡°protocol://ipaddress:port¡±£¬Èç£º¡±tcp://127.0.0.1:12001¡±¡£ 
	///@remark ¡°tcp¡±´ú±í´«ÊäÐ­Òé£¬¡°127.0.0.1¡±´ú±í·þÎñÆ÷µØÖ·¡£¡±12001¡±´ú±í·þÎñÆ÷¶Ë¿ÚºÅ¡£
	///@remark RegisterFrontÓÅÏÈÓÚRegisterNameServer
	virtual void RegisterNameServer(char *pszNsAddress) = 0;
	
	///×¢²á»Øµ÷½Ó¿Ú
	///@param pSpi ÅÉÉú×Ô»Øµ÷½Ó¿ÚÀàµÄÊµÀý
	virtual void RegisterSpi(CUstpFtdcMduserSpi *pSpi) = 0;
	
	///¼ÓÔØÖ¤Êé
	///@param pszCertFileName ÓÃ»§Ö¤ÊéÎÄ¼þÃû
	///@param pszKeyFileName ÓÃ»§Ë½Ô¿ÎÄ¼þÃû
	///@param pszCaFileName ¿ÉÐÅÈÎCAÖ¤ÊéÎÄ¼þÃû
	///@param pszKeyFilePassword ÓÃ»§Ë½Ô¿ÎÄ¼þÃÜÂë
	///@return 0 ²Ù×÷³É¹¦
	///@return -1 ¿ÉÐÅÈÎCAÖ¤ÊéÔØÈëÊ§°Ü
	///@return -2 ÓÃ»§Ö¤ÊéÔØÈëÊ§°Ü
	///@return -3 ÓÃ»§Ë½Ô¿ÔØÈëÊ§°Ü	
	///@return -4 ÓÃ»§Ö¤ÊéÐ£ÑéÊ§°Ü
	virtual int RegisterCertificateFile(const char *pszCertFileName, const char *pszKeyFileName, 
		const char *pszCaFileName, const char *pszKeyFilePassword) = 0;

	///¶©ÔÄÊÐ³¡ÐÐÇé¡£
	///@param nTopicID ÊÐ³¡ÐÐÇéÖ÷Ìâ  
	///@param nResumeType ÊÐ³¡ÐÐÇéÖØ´«·½Ê½  
	///        USTP_TERT_RESTART:´Ó±¾½»Ò×ÈÕ¿ªÊ¼ÖØ´«
	///        USTP_TERT_RESUME:´ÓÉÏ´ÎÊÕµ½µÄÐø´«(·Ç¶©ÔÄÈ«²¿ºÏÔ¼Ê±£¬²»Ö§³ÖÐø´«Ä£Ê½)
	///        USTP_TERT_QUICK:ÏÈ´«ËÍµ±Ç°ÐÐÇé¿ìÕÕ,ÔÙ´«ËÍµÇÂ¼ºóÊÐ³¡ÐÐÇéµÄÄÚÈÝ
	///@remark ¸Ã·½·¨ÒªÔÚInit·½·¨Ç°µ÷ÓÃ¡£Èô²»µ÷ÓÃÔò²»»áÊÕµ½Ë½ÓÐÁ÷µÄÊý¾Ý¡£
	virtual void SubscribeMarketDataTopic(int nTopicID, USTP_TE_RESUME_TYPE nResumeType) = 0;

	///¶©ÔÄºÏÔ¼ÐÐÇé¡£
	///@param ppInstrumentID ºÏÔ¼ID  
	///@param nCount Òª¶©ÔÄ/ÍË¶©ÐÐÇéµÄºÏÔ¼¸öÊý
	///@remark 
	virtual int SubMarketData(char *ppInstrumentID[], int nCount)=0;

	///ÍË¶©ºÏÔ¼ÐÐÇé¡£
	///@param ppInstrumentID ºÏÔ¼ID  
	///@param nCount Òª¶©ÔÄ/ÍË¶©ÐÐÇéµÄºÏÔ¼¸öÊý
	///@remark 
	virtual int UnSubMarketData(char *ppInstrumentID[], int nCount)=0;		
	
	///ÉèÖÃÐÄÌø³¬Ê±Ê±¼ä¡£
	///@param timeout ÐÄÌø³¬Ê±Ê±¼ä(Ãë)  
	virtual void SetHeartbeatTimeout(unsigned int timeout) = 0;


	///·ç¿ØÇ°ÖÃÏµÍ³ÓÃ»§µÇÂ¼ÇëÇó
	virtual int ReqUserLogin(CUstpFtdcReqUserLoginField *pReqUserLogin, int nRequestID) = 0;

	///ÓÃ»§ÍË³öÇëÇó
	virtual int ReqUserLogout(CUstpFtdcReqUserLogoutField *pReqUserLogout, int nRequestID) = 0;

	///ÓÃ»§ÃÜÂëÐÞ¸ÄÇëÇó
	virtual int ReqUserPasswordUpdate(CUstpFtdcUserPasswordUpdateField *pUserPasswordUpdate, int nRequestID) = 0;

	///¶©ÔÄÖ÷ÌâÇëÇó
	virtual int ReqSubscribeTopic(CUstpFtdcDisseminationField *pDissemination, int nRequestID) = 0;

	///Ö÷Ìâ²éÑ¯ÇëÇó
	virtual int ReqQryTopic(CUstpFtdcDisseminationField *pDissemination, int nRequestID) = 0;

	///¶©ÔÄºÏÔ¼µÄÏà¹ØÐÅÏ¢
	virtual int ReqSubMarketData(CUstpFtdcSpecificInstrumentField *pSpecificInstrument, int nRequestID) = 0;

	///ÍË¶©ºÏÔ¼µÄÏà¹ØÐÅÏ¢
	virtual int ReqUnSubMarketData(CUstpFtdcSpecificInstrumentField *pSpecificInstrument, int nRequestID) = 0;
protected:
	~CUstpFtdcMduserApi(){};
};

#endif
